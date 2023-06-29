#include "xmlutils.h"

#include <cassert>
#include <utility>

#include <wx/ffile.h>
#include <wx/intl.h>
#include <wx/log.h>


namespace
{

class CompactPrinter : public tinyxml2::XMLPrinter
{
public:
    CompactPrinter(std::FILE* file = nullptr, bool compact = false, int depth = 0) :
        tinyxml2::XMLPrinter(file, compact, depth)
    {
    }

protected:
    void PrintSpace(int depth) override
    {
        for (int i = 0; i < depth; ++i) {
            Write("  ");
        }
    }
};


const std::pair<wxUniChar, wxString> entities[] = {
  {'&', "&amp;"}, {'<', "&lt;"}, {'>', "&gt;"}, {'\'', "&apos;"}, {'"', "&quot;"}, {'\n', "&#x0A;"},
};


wxString encodeEntities(const wxString& input)
{
    if (input.find_first_of("&<>'\"\n") == wxString::npos) {
        return input;
    }

    wxString output;
    output.reserve(input.size() * 1.2);
    auto isEntity = false;
    for (const auto& c : input) {
        for (const auto& [ch, entity] : entities) {
            if (c == ch) {
                output.append(entity);
                isEntity = true;
                break;
            }
        }
        if (!isEntity) {
            output.Append(c);
        } else {
            isEntity = false;
        }
    }

    return output;
}

wxString decodeEntities(const wxString& input)
{
    if (input.find_first_of("&") == wxString::npos) {
        return input;
    }

    wxString output;
    output.reserve(input.size());
    wxString sequence;
    sequence.reserve(8);
    enum class State {
        SEARCH,
        BUILD,
    };
    State state = State::SEARCH;
    for (const auto& c : input) {
        switch (state) {
            case State::SEARCH:
                if (c.GetValue() == '&') {
                    sequence = c;
                    state = State::BUILD;
                } else {
                    output.Append(c);
                }
                break;
            case State::BUILD:
                sequence.Append(c);
                if (c.GetValue() == ';') {
                    auto isEntity = false;
                    for (const auto& [ch, entity] : entities) {
                        if (sequence == entity) {
                            output.Append(ch);
                            isEntity = true;
                            break;
                        }
                    }
                    if (!isEntity) {
                        auto isNumeric = false;
                        if (sequence[1] == '#') {
                            unsigned long value = 0;
                            if (sequence[2] == 'x') {
                                if ((isNumeric = sequence.substr(3, sequence.length() - 3 - 1).ToCULong(&value, 16))) {
                                    output.Append(wxUniChar(value));
                                }
                            } else {
                                if ((isNumeric = sequence.substr(2, sequence.length() - 2 - 1).ToCULong(&value, 10))) {
                                    output.Append(wxUniChar(value));
                                }
                            }
                        }
                        if (!isNumeric) {
                            output.append(sequence);
                        }
                    }
                    sequence.clear();
                    state = State::SEARCH;
                }
                break;
        }
    }
    if (!sequence.empty()) {
        output.append(sequence);
    }

    return output;
}

}  // namespace


namespace XMLUtils
{

std::unique_ptr<tinyxml2::XMLDocument> LoadXMLFile(const wxString& path, bool collapseWhitespace)
{
    wxLogDebug("Loading XML file: %s", path);
    // Use wxFFile to get full unicode support for path on Windows
    wxFFile file;
    if (wxLogNull noLog; !file.Open(path, "rb")) {
        return std::unique_ptr<tinyxml2::XMLDocument>();
    }
    auto doc = std::make_unique<tinyxml2::XMLDocument>(
      false, collapseWhitespace ? tinyxml2::COLLAPSE_WHITESPACE : tinyxml2::PRESERVE_WHITESPACE);
    doc->LoadFile(file.fp());

    return doc;
}

bool SaveXMLFile(const wxString& path, const tinyxml2::XMLDocument& document, bool compact)
{
    assert(!document.ProcessEntities());

    // Use wxFFile to get full unicode support for path on Windows
    wxFFile file;
    if (wxLogNull noLog; !file.Open(path, "w")) {
        return false;
    }
    CompactPrinter printer(file.fp(), compact);
    document.Print(&printer);

    return true;
}

wxString SaveXMLString(const tinyxml2::XMLDocument& document, bool compact)
{
    assert(!document.ProcessEntities());

    CompactPrinter printer(nullptr, compact);
    document.Print(&printer);

    return wxString(printer.CStr(), wxConvUTF8);
}

wxString StringAttribute(const tinyxml2::XMLElement* element, const wxString& name, const wxString& defaultValue)
{
    const auto* value = element->Attribute(name.utf8_str());

    return (value ? decodeEntities(wxString(value, wxConvUTF8)) : defaultValue);
}

void SetAttribute(tinyxml2::XMLElement* element, const wxString& name, const wxString& value)
{
    element->SetAttribute(name.utf8_str(), encodeEntities(value).utf8_str());
}

wxString GetText(const tinyxml2::XMLElement* element, const wxString& defaultValue, bool deepSearch)
{
    const char* value = nullptr;
    if (!deepSearch) {
        value = element->GetText();
    } else {
        for (const auto* node = element->FirstChild(); node; node = node->NextSibling()) {
            if (node->ToText()) {
                value = node->Value();
                break;
            }
        }
    }

    return (value ? decodeEntities(wxString(value, wxConvUTF8)) : defaultValue);
}

void SetText(tinyxml2::XMLElement* element, const wxString& value, bool insertElement)
{
    if (!insertElement) {
        element->SetText(encodeEntities(value).utf8_str());
    } else {
        element->InsertNewText(encodeEntities(value).utf8_str());
    }
}

}  // namespace XMLUtils
