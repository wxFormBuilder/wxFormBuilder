///////////////////////////////////////////////////////////////////////////////
//
// wxFormBuilder - A Visual Dialog Editor for wxWidgets.
// Copyright (C) 2005 José Antonio Hurtado
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// Written by
//   José Antonio Hurtado - joseantonio.hurtado@gmail.com
//   Juan Antonio Ortega  - jortegalalmolda@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

#include "xrcconv.h"

#include <set>

#include <ticpp.h>
#include <wx/tokenzr.h>


static wxString StringToXrcText(const wxString& str)
{
    wxString result;
    result.reserve(str.size() * 1.2);

    for (auto current = str.begin(); current != str.end(); ++current) {
        const auto ch = *current;
        switch (ch.GetValue()) {
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '_':
                result += "__";
                break;
            case '&':
                result += "_";
                break;
            default:
                result += ch;
                break;
        }
    }

    return result;
}

static wxString XrcTextToString(const wxString& str)
{
    wxString result;
    result.reserve(str.size());

    for (auto current = str.begin(); current != str.end(); ++current) {
        const auto ch = *current;
        if (ch == '\\') {
            auto next = current + 1;
            if (next != str.end()) {
                switch ((*next).GetValue()) {
                    case 'n':
                        result += '\n';
                        break;
                    case 'r':
                        result += '\r';
                        break;
                    case 't':
                        result += '\t';
                        break;
                    case '\\':
                        result += '\\';
                        break;
                    default:
                        // Invalid escape sequence, drop
                        break;
                }
                ++current;
            }  // else Broken escape sequence at end, drop
        } else if (ch == '_') {
            auto next = current + 1;
            if (next != str.end() && *next == '_') {
                result += '_';
                ++current;
            } else {
                result += '&';
            }
        } else {
            result += ch;
        }
    }

    return result;
}

static wxString ReplaceSynonymous(const IComponentLibrary* lib, const wxString& bitlist)
{
    wxString result;
    result.reserve(bitlist.size());

    wxStringTokenizer tkz(bitlist, "|");
    while (tkz.HasMoreTokens()) {
        auto token = tkz.GetNextToken();
        token.Trim(true);
        token.Trim(false);

        if (!result.empty()) {
            result += '|';
        }
        result += lib->ReplaceSynonymous(token);
    }

    return result;
}


///////////////////////////////////////////////////////////////////////////////


ObjectToXrcFilter::ObjectToXrcFilter(
  const IComponentLibrary* lib, const IObject* obj, const wxString& classname, const wxString& objname,
  const wxString& base) :
    m_lib(lib), m_obj(obj)
{
    m_xrcObj = new ticpp::Element("object");
    m_xrcObj->SetAttribute("class", classname.mb_str(wxConvUTF8));
    if (!objname.empty()) {
        m_xrcObj->SetAttribute("name", objname.mb_str(wxConvUTF8));
    }
    if (!base.empty()) {
        m_xrcObj->SetAttribute("base", base.mb_str(wxConvUTF8));
    }
}

ObjectToXrcFilter::~ObjectToXrcFilter()
{
    delete m_xrcObj;
}


void ObjectToXrcFilter::AddProperty(const wxString& objPropName, const wxString& xrcPropName, Type propType)
{
    ticpp::Element propElement(xrcPropName.mb_str(wxConvUTF8));

    switch (propType) {
        case Type::Size:
        case Type::Point:
        case Type::BitList:
            LinkText(m_obj->GetPropertyAsString(objPropName), &propElement);
            break;
        case Type::Text: {
            // The text must be converted to XRC format
            auto text = m_obj->GetPropertyAsString(objPropName);
            LinkText(text, &propElement, true);
            break;
        }
        case Type::Bool:
        case Type::Integer:
            LinkInteger(m_obj->GetPropertyAsInteger(objPropName), &propElement);
            break;
        case Type::Float:
            LinkFloat(m_obj->GetPropertyAsFloat(objPropName), &propElement);
            break;
        case Type::Colour:
            LinkColour(m_obj->GetPropertyAsColour(objPropName), &propElement);
            break;
        case Type::Font:
            LinkFont(m_obj->GetPropertyAsFont(objPropName), &propElement);
            break;
        case Type::StringList:
            LinkStringList(m_obj->GetPropertyAsArrayString(objPropName), &propElement);
            break;
        case Type::Bitmap: {
            auto bitmapProp = m_obj->GetPropertyAsString(objPropName);
            if (bitmapProp.empty()) {
                break;
            }
            auto filename = bitmapProp.AfterFirst(';');
            if (filename.empty()) {
                break;
            }
            if (bitmapProp.size() < (filename.size() + 2)) {
                break;
            }

            if (
              bitmapProp.StartsWith("Load From File") || bitmapProp.StartsWith("Load From Embedded File") ||
              bitmapProp.StartsWith("Load From XRC")) {
                LinkText(filename.Trim().Trim(false), &propElement);
            } else if (bitmapProp.StartsWith("Load From Art Provider")) {
                propElement.SetAttribute("stock_id", filename.BeforeFirst(';').Trim().Trim(false).mb_str(wxConvUTF8));
                propElement.SetAttribute(
                  "stock_client", filename.AfterFirst(';').Trim().Trim(false).mb_str(wxConvUTF8));

                LinkText("undefined.png", &propElement);
            }
            break;
        }
    }

    m_xrcObj->LinkEndChild(&propElement);
}

void ObjectToXrcFilter::AddPropertyValue(const wxString& xrcPropName, const wxString& xrcPropValue, bool xrcFormat)
{
    ticpp::Element propElement(xrcPropName.mb_str(wxConvUTF8));
    LinkText(xrcPropValue, &propElement, xrcFormat);
    m_xrcObj->LinkEndChild(&propElement);
}

void ObjectToXrcFilter::AddPropertyPair(
  const wxString& objPropName1, const wxString& objPropName2, const wxString& xrcPropName)
{
    AddPropertyValue(
      xrcPropName,
      wxString::Format("%i,%i", m_obj->GetPropertyAsInteger(objPropName1), m_obj->GetPropertyAsInteger(objPropName2)));
}


void ObjectToXrcFilter::AddWindowProperties()
{
    wxString style;
    if (!m_obj->IsPropertyNull("style")) {
        style = m_obj->GetPropertyAsString("style");
    }
    if (!m_obj->IsPropertyNull("window_style")) {
        if (!style.empty()) {
            style += '|';
        }
        style += m_obj->GetPropertyAsString("window_style");
    }
    if (!style.empty()) {
        AddPropertyValue("style", style);
    }

    wxString extraStyle;
    if (!m_obj->IsPropertyNull("extra_style")) {
        extraStyle = m_obj->GetPropertyAsString("extra_style");
    }
    if (!m_obj->IsPropertyNull("window_extra_style")) {
        if (!extraStyle.empty()) {
            extraStyle += '|';
        }
        extraStyle += m_obj->GetPropertyAsString("window_extra_style");
    }
    if (!extraStyle.empty()) {
        AddPropertyValue("exstyle", extraStyle);
    }

    if (!m_obj->IsPropertyNull("pos")) {
        AddProperty("pos", "pos", Type::Size);
    }
    if (!m_obj->IsPropertyNull("size")) {
        AddProperty("size", "size", Type::Size);
    }

    if (!m_obj->IsPropertyNull("bg")) {
        AddProperty("bg", "bg", Type::Colour);
    }
    if (!m_obj->IsPropertyNull("fg")) {
        AddProperty("fg", "fg", Type::Colour);
    }

    if (!m_obj->IsPropertyNull("enabled") && m_obj->GetPropertyAsInteger("enabled") == 0) {
        AddProperty("enabled", "enabled", Type::Bool);
    }
    if (!m_obj->IsPropertyNull("hidden") && m_obj->GetPropertyAsInteger("hidden") != 0) {
        AddProperty("hidden", "hidden", Type::Bool);
    }
    if (!m_obj->IsPropertyNull("focused")) {
        AddPropertyValue("focused", "0");
    }

    if (!m_obj->IsPropertyNull("font")) {
        AddProperty("font", "font", Type::Font);
    }
    if (!m_obj->IsPropertyNull("tooltip")) {
        AddProperty("tooltip", "tooltip", Type::Text);
    }

    if (!m_obj->IsPropertyNull("subclass")) {
        auto subclass = m_obj->GetChildFromParentProperty("subclass", "name");
        if (!subclass.empty()) {
            m_xrcObj->SetAttribute("subclass", subclass.mb_str(wxConvUTF8));
        }
    }
}


ticpp::Element* ObjectToXrcFilter::GetXrcObject()
{
    return new ticpp::Element(*m_xrcObj);
}


void ObjectToXrcFilter::LinkInteger(int integer, ticpp::Element* propElement)
{
    propElement->SetText(integer);
}

void ObjectToXrcFilter::LinkFloat(double value, ticpp::Element* propElement)
{
    propElement->SetText(value);
}

void ObjectToXrcFilter::LinkText(const wxString& text, ticpp::Element* propElement, bool xrcFormat)
{
    auto value = (xrcFormat ? StringToXrcText(text) : text);
    propElement->SetText(value.mb_str(wxConvUTF8));
}

void ObjectToXrcFilter::LinkColour(const wxColour& colour, ticpp::Element* propElement)
{
    auto value = wxString::Format("#%02x%02x%02x", colour.Red(), colour.Green(), colour.Blue());
    propElement->SetText(value.mb_str(wxConvUTF8));
}

void ObjectToXrcFilter::LinkFont(const wxFontContainer& font, ticpp::Element* propElement)
{
    if (font.GetPointSize() > 0) {
        wxString aux;
        aux.Printf("%i", font.GetPointSize());

        ticpp::Element size("size");
        size.SetText(aux.mb_str(wxConvUTF8));
        propElement->LinkEndChild(&size);
    }

    bool skipFamily = false;
    ticpp::Element family("family");
    switch (font.GetFamily()) {
        case wxFONTFAMILY_DECORATIVE:
            family.SetText("decorative");
            break;
        case wxFONTFAMILY_ROMAN:
            family.SetText("roman");
            break;
        case wxFONTFAMILY_SWISS:
            family.SetText("swiss");
            break;
        case wxFONTFAMILY_SCRIPT:
            family.SetText("script");
            break;
        case wxFONTFAMILY_MODERN:
            family.SetText("modern");
            break;
        case wxFONTFAMILY_TELETYPE:
            family.SetText("teletype");
            break;
        default:
            // wxWidgets 2.9.0 doesn't define "default" family
            skipFamily = true;
            break;
    }
    if (!skipFamily) {
        propElement->LinkEndChild(&family);
    }

    ticpp::Element style("style");
    switch (font.GetStyle()) {
        case wxFONTSTYLE_SLANT:
            style.SetText("slant");
            break;
        case wxFONTSTYLE_ITALIC:
            style.SetText("italic");
            break;
        default:
            style.SetText("normal");
            break;
    }
    propElement->LinkEndChild(&style);

    ticpp::Element weight("weight");
    switch (font.GetWeight()) {
        case wxFONTWEIGHT_LIGHT:
            weight.SetText("light");
            break;
        case wxFONTWEIGHT_BOLD:
            weight.SetText("bold");
            break;
        default:
            weight.SetText("normal");
            break;
    }
    propElement->LinkEndChild(&weight);

    ticpp::Element underlined("underlined");
    underlined.SetText(font.GetUnderlined() ? "1" : "0");
    propElement->LinkEndChild(&underlined);

    if (!font.GetFaceName().empty()) {
        ticpp::Element face("face");
        face.SetText(font.GetFaceName().mb_str(wxConvUTF8));
        propElement->LinkEndChild(&face);
    }
}

void ObjectToXrcFilter::LinkStringList(const wxArrayString& array, ticpp::Element* propElement, bool xrcFormat)
{
    for (size_t i = 0; i < array.GetCount(); ++i) {
        const auto& value = (xrcFormat ? StringToXrcText(array[i]) : array[i]);
        ticpp::Element item("item");
        item.SetText(value.mb_str(wxConvUTF8));
        propElement->LinkEndChild(&item);
    }
}


///////////////////////////////////////////////////////////////////////////////


XrcToXfbFilter::XrcToXfbFilter(const IComponentLibrary* lib, const ticpp::Element* obj, const wxString& classname) :
    m_lib(lib), m_xrcObj(obj)
{
    m_xfbObj = new ticpp::Element("object");
    m_xfbObj->SetAttribute("class", classname.mb_str(wxConvUTF8));
    try {
        std::string name;
        obj->GetAttribute("name", &name);
        wxString objname(name.c_str(), wxConvUTF8);
        AddPropertyValue("name", objname);
    } catch (ticpp::Exception& ex) {
        wxLogDebug(wxString(ex.m_details.c_str(), wxConvUTF8));
    }
}

XrcToXfbFilter::XrcToXfbFilter(
  const IComponentLibrary* lib, const ticpp::Element* obj, [[maybe_unused]] const wxString& classname,
  const wxString& objname) :
    m_lib(lib), m_xrcObj(obj)
{
    m_xfbObj = new ticpp::Element("object");
    try {
        std::string name;
        obj->GetAttribute("class", &name);
        m_xfbObj->SetAttribute("class", name);
    } catch (ticpp::Exception& ex) {
        wxLogDebug(wxString(ex.m_details.c_str(), wxConvUTF8));
    }
    if (!objname.empty()) {
        AddProperty("name", objname, Type::Text);
    }
}

XrcToXfbFilter::~XrcToXfbFilter()
{
    delete m_xfbObj;
}


void XrcToXfbFilter::AddProperty(const wxString& xrcPropName, const wxString& xfbPropName, Type propType)
{
    ticpp::Element propElement("property");
    propElement.SetAttribute("name", xfbPropName.mb_str(wxConvUTF8));

    switch (propType) {
        case Type::Size:
        case Type::Point:
        case Type::Bool:
            ImportTextProperty(xrcPropName, &propElement);
            break;
        case Type::Text:
            ImportTextProperty(xrcPropName, &propElement, true);
            break;
        case Type::Integer:
            ImportIntegerProperty(xrcPropName, &propElement);
            break;
        case Type::Float:
            ImportFloatProperty(xrcPropName, &propElement);
            break;
        case Type::BitList:
            ImportBitlistProperty(xrcPropName, &propElement);
            break;
        case Type::Colour:
            ImportColourProperty(xrcPropName, &propElement);
            break;
        case Type::Font:
            ImportFontProperty(xrcPropName, &propElement);
            break;
        case Type::StringList:
            ImportStringListProperty(xrcPropName, &propElement, true);
            break;
        case Type::Bitmap:
            ImportBitmapProperty(xrcPropName, &propElement);
            break;
    }

    m_xfbObj->LinkEndChild(&propElement);
}

void XrcToXfbFilter::AddPropertyValue(const wxString& xfbPropName, const wxString& xfbPropValue, bool parseXrcText)
{
    ticpp::Element propElement("property");
    propElement.SetAttribute("name", xfbPropName.mb_str(wxConvUTF8));
    const auto& value = (parseXrcText ? XrcTextToString(xfbPropValue) : xfbPropValue);
    propElement.SetText(value.mb_str(wxConvUTF8));
    m_xfbObj->LinkEndChild(&propElement);
}

void XrcToXfbFilter::AddPropertyPair(
  const wxString& xrcPropName, const wxString& xfbPropName1, const wxString& xfbPropName2)
{
    try {
        auto* pairProp = m_xrcObj->FirstChildElement(xrcPropName.mb_str(wxConvUTF8));

        wxString width;
        wxString height;
        wxStringTokenizer tkz(wxString(pairProp->GetText().c_str(), wxConvUTF8), ",");
        if (tkz.HasMoreTokens()) {
            width = tkz.GetNextToken();
            if (tkz.HasMoreTokens()) {
                height = tkz.GetNextToken();
            }
        }
        AddPropertyValue(xfbPropName1, width);
        AddPropertyValue(xfbPropName2, height);
    } catch (ticpp::Exception& ex) {
        wxLogDebug(wxString(ex.m_details.c_str(), wxConvUTF8));
    }
}


void XrcToXfbFilter::AddWindowProperties()
{
    AddProperty("pos", "pos", Type::Point);
    AddProperty("size", "size", Type::Size);

    AddProperty("bg", "bg", Type::Colour);
    AddProperty("fg", "fg", Type::Colour);

    if (m_xrcObj->FirstChildElement("enabled", false)) {
        AddProperty("enabled", "enabled", Type::Bool);
    }
    AddProperty("hidden", "hidden", Type::Bool);

    AddProperty("font", "font", Type::Font);
    AddProperty("tooltip", "tooltip", Type::Text);

    AddStyleProperty();
    AddExtraStyleProperty();

    // Subclass
    std::string subclass;
    m_xrcObj->GetAttribute("subclass", &subclass, false);
    if (!subclass.empty()) {
        ticpp::Element propElement("property");
        propElement.SetAttribute("name", "subclass");
        propElement.SetText(subclass);
        m_xfbObj->LinkEndChild(&propElement);
    }
}


ticpp::Element* XrcToXfbFilter::GetXfbObject()
{
    return m_xfbObj->Clone().release()->ToElement();
}


void XrcToXfbFilter::ImportIntegerProperty(const wxString& xrcPropName, ticpp::Element* property)
{
    try {
        auto* xrcProperty = m_xrcObj->FirstChildElement(xrcPropName.mb_str(wxConvUTF8));
        property->SetText(xrcProperty->GetText());
    } catch (ticpp::Exception&) {
        property->SetText("0");
    }
}

void XrcToXfbFilter::ImportFloatProperty(const wxString& xrcPropName, ticpp::Element* property)
{
    try {
        auto* xrcProperty = m_xrcObj->FirstChildElement(xrcPropName.mb_str(wxConvUTF8));
        property->SetText(xrcProperty->GetText());
    } catch (ticpp::Exception&) {
        property->SetText("0.0");
    }
}

void XrcToXfbFilter::ImportTextProperty(const wxString& xrcPropName, ticpp::Element* property, bool parseXrcText)
{
    try {
        auto* xrcProperty = m_xrcObj->FirstChildElement(xrcPropName.mb_str(wxConvUTF8));

        // Convert XRC text to normal text
        auto value = wxString(xrcProperty->GetText().c_str(), wxConvUTF8);
        if (parseXrcText) {
            value = XrcTextToString(value);
        }

        property->SetText(value.mb_str(wxConvUTF8));
    } catch (ticpp::Exception& ex) {
        wxLogDebug(wxString(ex.m_details.c_str(), wxConvUTF8));
    }
}

void XrcToXfbFilter::ImportBitmapProperty(const wxString& xrcPropName, ticpp::Element* property)
{
    try {
        auto* xrcProperty = m_xrcObj->FirstChildElement(xrcPropName.mb_str(wxConvUTF8));

        if (!xrcProperty->GetAttribute("stock_id").empty() && !xrcProperty->GetAttribute("stock_client").empty()) {
            // read wxArtProvider-based bitmap
            wxString res = "Load From Art Provider";
            res += ";";
            res += wxString(xrcProperty->GetAttribute("stock_id").c_str(), wxConvUTF8);
            res += ";";
            res += wxString(xrcProperty->GetAttribute("stock_client").c_str(), wxConvUTF8);
            property->SetText(res.Trim().mb_str(wxConvUTF8));
        } else {
            // read file-based bitmap
            wxString res = "Load From File";
            res += ";";
            res += wxString(xrcProperty->GetText().c_str(), wxConvUTF8);
            property->SetText(res.Trim().mb_str(wxConvUTF8));
        }
    } catch (ticpp::Exception& ex) {
        wxLogDebug(wxString(ex.m_details.c_str(), wxConvUTF8));
    }
}

void XrcToXfbFilter::ImportColourProperty(const wxString& xrcPropName, ticpp::Element* property)
{
    try {
        auto* xrcProperty = m_xrcObj->FirstChildElement(xrcPropName.mb_str(wxConvUTF8));
        auto value = xrcProperty->GetText();

        // Changing "#rrggbb" format to "rrr,ggg,bbb"
        std::string hexColour = "0x" + value.substr(1, 2) + " 0x" + value.substr(3, 2) + " 0x" + value.substr(5, 2);

        std::istringstream strIn;
        std::ostringstream strOut;
        unsigned int red, green, blue;

        strIn.str(hexColour);
        strIn >> std::hex;

        strIn >> red;
        strIn >> green;
        strIn >> blue;

        strOut << red << "," << green << "," << blue;

        property->SetText(strOut.str());
    } catch (ticpp::Exception& ex) {
        wxLogDebug(wxString(ex.m_details.c_str(), wxConvUTF8));
    }
}

void XrcToXfbFilter::ImportFontProperty(const wxString& xrcPropName, ticpp::Element* property)
{
    try {
        auto* xrcProperty = m_xrcObj->FirstChildElement(xrcPropName.mb_str(wxConvUTF8));

        wxFontContainer font;

        // the size
        try {
            auto* element = xrcProperty->FirstChildElement("size");
            long size;
            element->GetText(&size);
            font.SetPointSize(size);
        } catch (ticpp::Exception& ex) {
            wxLogDebug(wxString(ex.m_details.c_str(), wxConvUTF8));
        }

        // the family
        try {
            auto* element = xrcProperty->FirstChildElement("family");
            wxString family_str(element->GetText().c_str(), wxConvUTF8);

            if (family_str == "decorative") {
                font.SetFamily(wxFONTFAMILY_DECORATIVE);
            } else if (family_str == "roman") {
                font.SetFamily(wxFONTFAMILY_ROMAN);
            } else if (family_str == "swiss") {
                font.SetFamily(wxFONTFAMILY_SWISS);
            } else if (family_str == "script") {
                font.SetFamily(wxFONTFAMILY_SCRIPT);
            } else if (family_str == "modern") {
                font.SetFamily(wxFONTFAMILY_MODERN);
            } else if (family_str == "teletype") {
                font.SetFamily(wxFONTFAMILY_TELETYPE);
            } else {
                font.SetFamily(wxFONTFAMILY_DEFAULT);
            }
        } catch (ticpp::Exception&) {
            font.SetFamily(wxFONTFAMILY_DEFAULT);
        }

        // the style
        try {
            auto* element = xrcProperty->FirstChildElement("style");
            wxString style_str(element->GetText().c_str(), wxConvUTF8);

            if (style_str == "slant") {
                font.SetStyle(wxFONTSTYLE_SLANT);
            } else if (style_str == "italic") {
                font.SetStyle(wxFONTSTYLE_ITALIC);
            } else {
                font.SetStyle(wxFONTSTYLE_NORMAL);
            }
        } catch (ticpp::Exception&) {
            font.SetStyle(wxFONTSTYLE_NORMAL);
        }


        // weight
        try {
            auto* element = xrcProperty->FirstChildElement("weight");
            wxString weight_str(element->GetText().c_str(), wxConvUTF8);

            if (weight_str == "light") {
                font.SetWeight(wxFONTWEIGHT_LIGHT);
            } else if (weight_str == "bold") {
                font.SetWeight(wxFONTWEIGHT_BOLD);
            } else {
                font.SetWeight(wxFONTWEIGHT_NORMAL);
            }
        } catch (ticpp::Exception&) {
            font.SetWeight(wxFONTWEIGHT_NORMAL);
        }

        // underlined
        try {
            auto* element = xrcProperty->FirstChildElement("underlined");
            wxString underlined_str(element->GetText().c_str(), wxConvUTF8);

            if (underlined_str == "1") {
                font.SetUnderlined(true);
            } else {
                font.SetUnderlined(false);
            }
        } catch (ticpp::Exception&) {
            font.SetUnderlined(false);
        }

        // face
        try {
            auto* element = xrcProperty->FirstChildElement("face");
            wxString face(element->GetText().c_str(), wxConvUTF8);
            font.SetFaceName(face);
        } catch (ticpp::Exception&) {
            font.SetFaceName(wxEmptyString);
        }

        // We already have the font type. So we must now use the wxFB format
        auto font_str = wxString::Format(
          "%s,%i,%i,%i,%i,%i", font.GetFaceName().c_str(), font.GetStyle(), font.GetWeight(), font.GetPointSize(),
          font.GetFamily(), font.GetUnderlined());
        property->SetText(font_str.mb_str(wxConvUTF8));
    } catch (ticpp::Exception& ex) {
        wxLogDebug(wxString(ex.m_details.c_str(), wxConvUTF8));
    }
}

void XrcToXfbFilter::ImportBitlistProperty(const wxString& xrcPropName, ticpp::Element* property)
{
    try {
        auto* xrcProperty = m_xrcObj->FirstChildElement(xrcPropName.mb_str(wxConvUTF8));

        auto bitlist = wxString(xrcProperty->GetText().c_str(), wxConvUTF8);
        bitlist = ReplaceSynonymous(m_lib, bitlist);
        property->SetText(bitlist.mb_str(wxConvUTF8));
    } catch (ticpp::Exception& ex) {
        wxLogDebug(wxString(ex.m_details.c_str(), wxConvUTF8));
    }
}

void XrcToXfbFilter::ImportStringListProperty(const wxString& xrcPropName, ticpp::Element* property, bool parseXrcText)
{
    try {
        auto* xrcProperty = m_xrcObj->FirstChildElement(xrcPropName.mb_str(wxConvUTF8));
        wxString res;

        auto* element = xrcProperty->FirstChildElement("item", false);
        while (element) {
            try {
                wxString value(element->GetText().c_str(), wxConvUTF8);
                if (parseXrcText) {
                    value = XrcTextToString(value);
                }

                res += '\"' + value + "\" ";
            } catch (ticpp::Exception& ex) {
                wxLogDebug(wxT("%s. line: %i"), wxString(ex.m_details.c_str(), wxConvUTF8).c_str(), __LINE__);
            }

            element = element->NextSiblingElement("item", false);
        }

        res.Trim();
        property->SetText(res.mb_str(wxConvUTF8));
    } catch (ticpp::Exception& ex) {
        wxLogDebug(wxT("%s. line: %i"), wxString(ex.m_details.c_str(), wxConvUTF8).c_str(), __LINE__);
    }
}


void XrcToXfbFilter::AddStyleProperty()
{
    try {
        auto* xrcProperty = m_xrcObj->FirstChildElement("style");

        auto bitlist = wxString(xrcProperty->GetText().c_str(), wxConvUTF8);
        bitlist = ReplaceSynonymous(m_lib, bitlist);

        // FIXME: We should avoid hardcoding these things
        std::set<wxString> windowStyles;
        windowStyles.insert("wxBORDER_DEFAULT");
        windowStyles.insert("wxBORDER_SIMPLE");
        windowStyles.insert("wxBORDER_DOUBLE");
        windowStyles.insert("wxBORDER_SUNKEN");
        windowStyles.insert("wxBORDER_RAISED");
        windowStyles.insert("wxBORDER_STATIC");
        windowStyles.insert("wxBORDER_THEME");
        windowStyles.insert("wxBORDER_NONE");
        windowStyles.insert("wxTRANSPARENT_WINDOW");
        windowStyles.insert("wxTAB_TRAVERSAL");
        windowStyles.insert("wxWANTS_CHARS");
        windowStyles.insert("wxVSCROLL");
        windowStyles.insert("wxHSCROLL");
        windowStyles.insert("wxALWAYS_SHOW_SB");
        windowStyles.insert("wxCLIP_CHILDREN");
        windowStyles.insert("wxFULL_REPAINT_ON_RESIZE");
        windowStyles.insert("wxNO_FULL_REPAINT_ON_RESIZE");

        wxString style, windowStyle;
        wxStringTokenizer tkz(bitlist, " |");
        while (tkz.HasMoreTokens()) {
            auto token = tkz.GetNextToken();
            token.Trim(true);
            token.Trim(false);

            if (windowStyles.find(token) == windowStyles.end()) {
                if (!style.empty()) {
                    style += "|";
                }
                style += token;
            } else {
                if (!windowStyle.empty()) {
                    windowStyle += "|";
                }
                windowStyle += token;
            }
        }

        if (!style.empty()) {
            AddPropertyValue("style", style);
        }
        AddPropertyValue("window_style", windowStyle);
    } catch (ticpp::Exception& ex) {
        wxLogDebug(wxString(ex.m_details.c_str(), wxConvUTF8));
    }
}

void XrcToXfbFilter::AddExtraStyleProperty()
{
    try {
        auto* xrcProperty = m_xrcObj->FirstChildElement("exstyle");

        auto bitlist = wxString(xrcProperty->GetText().c_str(), wxConvUTF8);
        bitlist = ReplaceSynonymous(m_lib, bitlist);

        // FIXME: We should avoid hardcoding these things
        std::set<wxString> windowStyles;
        windowStyles.insert("wxWS_EX_VALIDATE_RECURSIVELY");
        windowStyles.insert("wxWS_EX_BLOCK_EVENTS");
        windowStyles.insert("wxWS_EX_TRANSIENT");
        windowStyles.insert("wxWS_EX_CONTEXTHELP");
        windowStyles.insert("wxWS_EX_PROCESS_IDLE");
        windowStyles.insert("wxWS_EX_PROCESS_UI_UPDATES");

        wxString style, windowStyle;
        wxStringTokenizer tkz(bitlist, " |");
        while (tkz.HasMoreTokens()) {
            auto token = tkz.GetNextToken();
            token.Trim(true);
            token.Trim(false);

            if (windowStyles.find(token) == windowStyles.end()) {
                if (!style.empty()) {
                    style += "|";
                }
                style += token;
            } else {
                if (!windowStyle.empty()) {
                    windowStyle += "|";
                }
                windowStyle += token;
            }
        }

        if (!style.empty()) {
            AddPropertyValue("extra_style", style);
        }
        AddPropertyValue("window_extra_style", windowStyle);
    } catch (ticpp::Exception& ex) {
        wxLogDebug(wxString(ex.m_details.c_str(), wxConvUTF8));
    }
}


ticpp::Element* XrcToXfbFilter::GetXrcProperty(const wxString& name)
{
    return m_xrcObj->FirstChildElement(name.mb_str(wxConvUTF8));
}
