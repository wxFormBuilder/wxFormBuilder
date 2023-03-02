#include "xmlutils.h"

#include <wx/ffile.h>
#include <wx/intl.h>
#include <wx/log.h>

#include "utils/wxfbexception.h"


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

}  // namespace


namespace XMLUtils
{

std::unique_ptr<tinyxml2::XMLDocument> LoadXMLFile(const wxString& path, bool collapseWhitespace)
{
    wxLogDebug("Loading XML file: %s", path);
    // Use wxFFile to get full unicode support for path on Windows
    wxFFile file;
    if (wxLogNull noLog; !file.Open(path, "rb")) {
        THROW_WXFBEX(_("Failed to open file for reading: ") << path)
    }
    auto doc = std::make_unique<tinyxml2::XMLDocument>(
      true, collapseWhitespace ? tinyxml2::COLLAPSE_WHITESPACE : tinyxml2::PRESERVE_WHITESPACE);
    if (doc->LoadFile(file.fp()) != tinyxml2::XML_SUCCESS) {
        THROW_WXFBEX(_("Failed to parse file: ") << doc->ErrorStr())
    }

    return doc;
}

void SaveXMLFile(const wxString& path, const tinyxml2::XMLDocument& document, bool compact)
{
    // Use wxFFile to get full unicode support for path on Windows
    wxFFile file;
    if (wxLogNull noLog; !file.Open(path, "wb")) {
        THROW_WXFBEX(_("Failed to open file for writing: ") << path)
    }
    CompactPrinter printer(file.fp(), compact);
    document.Print(&printer);
}

wxString StringAttribute(const tinyxml2::XMLElement* element, const wxString& name, const wxString& defaultValue)
{
    const auto* value = element->Attribute(name.utf8_str());

    return (value ? wxString(value, wxConvUTF8) : defaultValue);
}

void SetAttribute(tinyxml2::XMLElement* element, const wxString& name, const wxString& value)
{
    element->SetAttribute(name.utf8_str(), value.utf8_str());
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

    return (value ? wxString(value, wxConvUTF8) : defaultValue);
}

void SetText(tinyxml2::XMLElement* element, const wxString& value, bool insertElement)
{
    if (!insertElement) {
        element->SetText(value.utf8_str());
    } else {
        element->InsertNewText(value.utf8_str());
    }
}

}  // namespace XMLUtils
