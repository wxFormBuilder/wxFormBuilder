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
#include <wx/colour.h>
#include <wx/tokenzr.h>

#include <common/xmlutils.h>


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
  const IComponentLibrary* lib, const IObject* obj, const wxString& classname, const wxString& objname) :
    m_lib(lib), m_obj(obj), m_xrcElement(nullptr)
{
    m_xrcObj = new ticpp::Element("object");
    m_xrcObj->SetAttribute("class", classname.mb_str(wxConvUTF8));
    if (!objname.empty()) {
        m_xrcObj->SetAttribute("name", objname.mb_str(wxConvUTF8));
    }
}

ObjectToXrcFilter::ObjectToXrcFilter(
    tinyxml2::XMLElement* xrcElement,
    const IComponentLibrary* lib, const IObject* obj,
    std::optional<wxString> className, std::optional<wxString> objectName
) :
    m_lib(lib), m_obj(obj),
    m_xrcObj(nullptr),
    m_xrcElement(xrcElement)
{
    xrcElement->SetName("object");
    if (!className || !className->empty()) {
        XMLUtils::SetAttribute(xrcElement, "class", className.value_or(obj->GetClassName()));
    }
    if (!objectName || !objectName->empty()) {
        XMLUtils::SetAttribute(xrcElement, "name", objectName.value_or(obj->GetPropertyAsString("name")));
    }
}

ObjectToXrcFilter::~ObjectToXrcFilter()
{
    delete m_xrcObj;
}


void ObjectToXrcFilter::AddProperty(const wxString& objPropName, const wxString& xrcPropName, Type propType)
{
    ticpp::Element propElement(xrcPropName.mb_str(wxConvUTF8));
    auto* propertyElement = m_xrcElement ? m_xrcElement->InsertNewChildElement(xrcPropName.utf8_str()) : nullptr;

    switch (propType) {
        case Type::Size:
        case Type::Point:
        case Type::BitList:
            LinkText(m_obj->GetPropertyAsString(objPropName), &propElement);
            if (propertyElement) SetText(propertyElement, m_obj->GetPropertyAsString(objPropName));
            break;
        case Type::Text: {
            // The text must be converted to XRC format
            auto text = m_obj->GetPropertyAsString(objPropName);
            LinkText(text, &propElement, true);
            if (propertyElement) SetText(propertyElement, m_obj->GetPropertyAsString(objPropName), true);
            break;
        }
        case Type::Bool:
        case Type::Integer:
            LinkInteger(m_obj->GetPropertyAsInteger(objPropName), &propElement);
            if (propertyElement) SetInteger(propertyElement, m_obj->GetPropertyAsInteger(objPropName));
            break;
        case Type::Float:
            LinkFloat(m_obj->GetPropertyAsFloat(objPropName), &propElement);
            if (propertyElement) SetFloat(propertyElement, m_obj->GetPropertyAsFloat(objPropName));
            break;
        case Type::Colour:
            LinkColour(m_obj->GetPropertyAsColour(objPropName), &propElement);
            if (propertyElement) SetColour(propertyElement, m_obj->GetPropertyAsColour(objPropName));
            break;
        case Type::Font:
            LinkFont(m_obj->GetPropertyAsFont(objPropName), &propElement);
            if (propertyElement) SetFont(propertyElement, m_obj->GetPropertyAsFont(objPropName));
            break;
        case Type::StringList:
            LinkStringList(m_obj->GetPropertyAsArrayString(objPropName), &propElement);
            if (propertyElement) SetStringList(propertyElement, m_obj->GetPropertyAsArrayString(objPropName));
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
                if (propertyElement) SetText(propertyElement, filename.Trim().Trim(false));
            } else if (bitmapProp.StartsWith("Load From Art Provider")) {
                propElement.SetAttribute("stock_id", filename.BeforeFirst(';').Trim().Trim(false).mb_str(wxConvUTF8));
                propElement.SetAttribute(
                  "stock_client", filename.AfterFirst(';').Trim().Trim(false).mb_str(wxConvUTF8));

                LinkText("undefined.png", &propElement);

                if (propertyElement) XMLUtils::SetAttribute(propertyElement, "stock_id", filename.BeforeFirst(';').Trim().Trim(false));
                if (propertyElement) XMLUtils::SetAttribute(propertyElement, "stock_client", filename.AfterFirst(';').Trim().Trim(false));
                // TODO: Don't specify a fallback bitmap at all?
                if (propertyElement) SetText(propertyElement, "undefined.png");
            }
            break;
        }
    }

    if (m_xrcObj) m_xrcObj->LinkEndChild(&propElement);
}

void ObjectToXrcFilter::AddPropertyValue(const wxString& xrcPropName, const wxString& xrcPropValue, bool xrcFormat)
{
    ticpp::Element propElement(xrcPropName.mb_str(wxConvUTF8));
    auto* propertyElement = m_xrcElement ? m_xrcElement->InsertNewChildElement(xrcPropName.utf8_str()) : nullptr;

    LinkText(xrcPropValue, &propElement, xrcFormat);
    if (propertyElement) SetText(propertyElement, xrcPropValue, xrcFormat);

    if (m_xrcObj) m_xrcObj->LinkEndChild(&propElement);
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
    if (!m_obj->IsPropertyNull("pos")) {
        AddProperty("pos", "pos", Type::Size);
    }
    if (!m_obj->IsPropertyNull("size")) {
        AddProperty("size", "size", Type::Size);
    }

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

    if (!m_obj->IsPropertyNull("fg")) {
        AddProperty("fg", "fg", Type::Colour);
    }
    // TODO: ownfg
    if (!m_obj->IsPropertyNull("bg")) {
        AddProperty("bg", "bg", Type::Colour);
    }
    // TODO: ownbg

    if (!m_obj->IsPropertyNull("enabled") && m_obj->GetPropertyAsInteger("enabled") == 0) {
        AddProperty("enabled", "enabled", Type::Bool);
    }
    // TODO: This property does not exist in the data model
    //if (!m_obj->IsPropertyNull("focused")) {
    //    AddPropertyValue("focused", "0");
    //}
    if (!m_obj->IsPropertyNull("hidden") && m_obj->GetPropertyAsInteger("hidden") != 0) {
        AddProperty("hidden", "hidden", Type::Bool);
    }

    if (!m_obj->IsPropertyNull("tooltip")) {
        AddProperty("tooltip", "tooltip", Type::Text);
    }

    // TODO: variant

    if (!m_obj->IsPropertyNull("font")) {
        AddProperty("font", "font", Type::Font);
    }
    // TODO: ownfont

    // TODO: help

    if (!m_obj->IsPropertyNull("subclass")) {
        auto subclass = m_obj->GetChildFromParentProperty("subclass", "name");
        if (!subclass.empty()) {
            if (m_xrcObj) m_xrcObj->SetAttribute("subclass", subclass.mb_str(wxConvUTF8));
            if (m_xrcElement) XMLUtils::SetAttribute(m_xrcElement, "subclass", subclass);
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


void ObjectToXrcFilter::SetInteger(tinyxml2::XMLElement* element, int integer) const
{
    element->SetText(integer);
}

void ObjectToXrcFilter::SetFloat(tinyxml2::XMLElement* element, double value) const
{
    element->SetText(value);
}

void ObjectToXrcFilter::SetText(tinyxml2::XMLElement* element, const wxString& text, bool xrcFormat) const
{
    XMLUtils::SetText(element, xrcFormat ? StringToXrcText(text) : text);
}

void ObjectToXrcFilter::SetColour(tinyxml2::XMLElement* element, const wxColour& colour) const
{
    XMLUtils::SetText(element, wxString::Format("#%02x%02x%02x", colour.Red(), colour.Green(), colour.Blue()));
}

void ObjectToXrcFilter::SetFont(tinyxml2::XMLElement* element, const wxFontContainer& font) const
{
    // TODO: Since wxWidgets 3.1.2 fractional sizes are supported
    if (auto fontSize = font.GetPointSize(); fontSize > 0) {
        auto* sizeElement = element->InsertNewChildElement("size");
        sizeElement->SetText(fontSize);
    }

    auto* styleElement = element->InsertNewChildElement("style");
    switch(font.GetStyle()) {
        default:
        case wxFONTSTYLE_NORMAL:
            XMLUtils::SetText(styleElement, "normal");
            break;
        case wxFONTSTYLE_ITALIC:
            XMLUtils::SetText(styleElement, "italic");
            break;
        case wxFONTSTYLE_SLANT:
            XMLUtils::SetText(styleElement, "slant");
            break;
    }

    // TODO: Since wxWidgets 3.1.2 more weights are supported
    auto* weightElement = element->InsertNewChildElement("weight");
    switch (font.GetWeight()) {
        default:
        case wxFONTWEIGHT_NORMAL:
            XMLUtils::SetText(weightElement, "normal");
            break;
        case wxFONTWEIGHT_LIGHT:
            XMLUtils::SetText(weightElement, "light");
            break;
        case wxFONTWEIGHT_BOLD:
            XMLUtils::SetText(weightElement, "bold");
            break;
    }

    auto* familyElement = element->InsertNewChildElement("family");
    switch (font.GetFamily()) {
        default:
        case wxFONTFAMILY_DEFAULT:
            XMLUtils::SetText(familyElement, "default");
            break;
        case wxFONTFAMILY_ROMAN:
            XMLUtils::SetText(familyElement, "roman");
            break;
        case wxFONTFAMILY_SCRIPT:
            XMLUtils::SetText(familyElement, "script");
            break;
        case wxFONTFAMILY_DECORATIVE:
            XMLUtils::SetText(familyElement, "decorative");
            break;
        case wxFONTFAMILY_SWISS:
            XMLUtils::SetText(familyElement, "swiss");
            break;
        case wxFONTFAMILY_MODERN:
            XMLUtils::SetText(familyElement, "modern");
            break;
        case wxFONTFAMILY_TELETYPE:
            XMLUtils::SetText(familyElement, "teletype");
            break;
    }

    auto* underlined = element->InsertNewChildElement("underlined");
    underlined->SetText(font.GetUnderlined() ? 1 : 0);

    // TODO: Since wxWidgets 3.1.2 strikethrough

    if (const auto& faceName = font.GetFaceName(); !faceName.empty()) {
        auto* faceElement = element->InsertNewChildElement("face");
        XMLUtils::SetText(faceElement, faceName);
    }

    // TODO: Additional elements missing: encoding, sysfont, inherit, relativesize
}

void ObjectToXrcFilter::SetStringList(tinyxml2::XMLElement* element, const wxArrayString& array, bool xrcFormat) const
{
    for (const auto& item : array) {
        auto* itemElement = element->InsertNewChildElement("item");
        XMLUtils::SetText(itemElement, xrcFormat ? StringToXrcText(item) : item);
    }
}


///////////////////////////////////////////////////////////////////////////////


XrcToXfbFilter::XrcToXfbFilter(const IComponentLibrary* lib, const ticpp::Element* obj, const wxString& classname) :
    m_lib(lib), m_xrcObj(obj), m_xrcElement(nullptr), m_xfbElement(nullptr)
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
    tinyxml2::XMLElement* xfbElement,
    const IComponentLibrary* lib, const tinyxml2::XMLElement* xrcElement,
    std::optional<wxString> className, std::optional<wxString> objectName
) :
    m_lib(lib), m_xrcElement(xrcElement),
    m_xrcObj(nullptr), m_xfbObj(nullptr),
    m_xfbElement(xfbElement)
{
    xfbElement->SetName("object");
    if (!className || !className->empty()) {
        XMLUtils::SetAttribute(xfbElement, "class", className.value_or(XMLUtils::StringAttribute(xrcElement, "class")));
    }
    if (!objectName || !objectName->empty()) {
        AddPropertyValue("name", objectName.value_or(XMLUtils::StringAttribute(xrcElement, "name")));
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
    auto* propertyElement = m_xfbElement ? m_xfbElement->InsertNewChildElement("property") : nullptr;
    if (propertyElement) XMLUtils::SetAttribute(propertyElement, "name", xfbPropName);

    switch (propType) {
        case Type::Size:
        case Type::Point:
        case Type::Bool:
            if (m_xrcObj) ImportTextProperty(xrcPropName, &propElement);
            if (propertyElement) SetTextProperty(propertyElement, xrcPropName);
            break;
        case Type::Text:
            if (m_xrcObj) ImportTextProperty(xrcPropName, &propElement, true);
            if (propertyElement) SetTextProperty(propertyElement, xrcPropName, true);
            break;
        case Type::Integer:
            if (m_xrcObj) ImportIntegerProperty(xrcPropName, &propElement);
            if (propertyElement) SetIntegerProperty(propertyElement, xrcPropName);
            break;
        case Type::Float:
            if (m_xrcObj) ImportFloatProperty(xrcPropName, &propElement);
            if (propertyElement) SetFloatProperty(propertyElement, xrcPropName);
            break;
        case Type::BitList:
            if (m_xrcObj) ImportBitlistProperty(xrcPropName, &propElement);
            if (propertyElement) SetBitlistProperty(propertyElement, xrcPropName);
            break;
        case Type::Colour:
            if (m_xrcObj) ImportColourProperty(xrcPropName, &propElement);
            if (propertyElement) SetColourProperty(propertyElement, xrcPropName);
            break;
        case Type::Font:
            if (m_xrcObj) ImportFontProperty(xrcPropName, &propElement);
            if (propertyElement) SetFontProperty(propertyElement, xrcPropName);
            break;
        case Type::StringList:
            if (m_xrcObj) ImportStringListProperty(xrcPropName, &propElement, true);
            if (propertyElement) SetStringListProperty(propertyElement, xrcPropName, true);
            break;
        case Type::Bitmap:
            if (m_xrcObj) ImportBitmapProperty(xrcPropName, &propElement);
            if (propertyElement) SetBitmapProperty(propertyElement, xrcPropName);
            break;
    }

    if (m_xfbObj) m_xfbObj->LinkEndChild(&propElement);
}

void XrcToXfbFilter::AddPropertyValue(const wxString& xfbPropName, const wxString& xfbPropValue, bool parseXrcText)
{
    ticpp::Element propElement("property");
    propElement.SetAttribute("name", xfbPropName.mb_str(wxConvUTF8));
    auto* propertyElement = m_xfbElement ? m_xfbElement->InsertNewChildElement("property") : nullptr;
    if (propertyElement) XMLUtils::SetAttribute(propertyElement, "name", xfbPropName);

    const auto& value = (parseXrcText ? XrcTextToString(xfbPropValue) : xfbPropValue);
    propElement.SetText(value.mb_str(wxConvUTF8));
    if (propertyElement) XMLUtils::SetText(propertyElement, value);

    if (m_xfbObj) m_xfbObj->LinkEndChild(&propElement);
}

void XrcToXfbFilter::AddPropertyPair(
  const wxString& xrcPropName, const wxString& xfbPropName1, const wxString& xfbPropName2)
{
    if (m_xrcObj) {
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
    } else {
    const auto* propertyElement = m_xrcElement->FirstChildElement(xrcPropName.utf8_str());
    if (!propertyElement) {
        return;
    }

    wxString second;
    auto first = XMLUtils::GetText(propertyElement).BeforeFirst(',', &second);
    AddPropertyValue(xfbPropName1, first);
    AddPropertyValue(xfbPropName2, second);
    }
}


void XrcToXfbFilter::AddWindowProperties()
{
    // FIXME: Check existence of all properties before adding them
    AddProperty("pos", "pos", Type::Point);
    AddProperty("size", "size", Type::Size);

    AddStyleProperty();
    AddExtraStyleProperty();

    AddProperty("fg", "fg", Type::Colour);
    // TODO: ownfg
    AddProperty("bg", "bg", Type::Colour);
    // TODO: ownbg

    AddProperty("enabled", "enabled", Type::Bool);
    // TODO: focused does not exist in the data model
    AddProperty("hidden", "hidden", Type::Bool);

    AddProperty("tooltip", "tooltip", Type::Text);

    // TODO: variant

    AddProperty("font", "font", Type::Font);
    // TODO: ownfont

    // TODO: help

    if (m_xrcObj) {
    std::string subclass;
    m_xrcObj->GetAttribute("subclass", &subclass, false);
    if (!subclass.empty()) {
        ticpp::Element propElement("property");
        propElement.SetAttribute("name", "subclass");
        propElement.SetText(subclass);
        m_xfbObj->LinkEndChild(&propElement);
    }
    } else {
    auto subclass = XMLUtils::StringAttribute(m_xrcElement, "subclass");
    if (!subclass.empty()) {
        AddPropertyValue("subclass", subclass);
    }
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


void XrcToXfbFilter::SetIntegerProperty(tinyxml2::XMLElement* element, const wxString& name) const
{
    if (const auto* propertyElement = m_xrcElement->FirstChildElement(name.utf8_str())) {
        element->SetText(propertyElement->IntText(0));
    } else {
        element->SetText(0);
    }
}

void XrcToXfbFilter::SetFloatProperty(tinyxml2::XMLElement* element, const wxString& name) const
{
    if (const auto* propertyElement = m_xrcElement->FirstChildElement(name.utf8_str())) {
        element->SetText(propertyElement->DoubleText(0.0));
    } else {
        element->SetText(0.0);
    }
}

void XrcToXfbFilter::SetTextProperty(tinyxml2::XMLElement* element, const wxString& name, bool xrcFormat) const
{
    if (const auto* propertyElement = m_xrcElement->FirstChildElement(name.utf8_str())) {
        auto propertyValue = XMLUtils::GetText(propertyElement);
        XMLUtils::SetText(element, xrcFormat ? XrcTextToString(propertyValue) : propertyValue);
    } else {
        XMLUtils::SetText(element, "");
    }
}

void XrcToXfbFilter::SetBitmapProperty(tinyxml2::XMLElement* element, const wxString& name) const
{
    const auto* propertyElement = m_xrcElement->FirstChildElement(name.utf8_str());
    if (!propertyElement) {
        return;
    }

    auto stockId = XMLUtils::StringAttribute(propertyElement, "stock_id");
    auto stockClient = XMLUtils::StringAttribute(propertyElement, "stock_client");
    // TODO: In xrc stockClient is optional, but xfb does require this element
    if (!stockId.empty() && !stockClient.empty()) {
        // wxArtProvider Bitmap
        XMLUtils::SetText(element, wxString::Format("Load From Art Provider;%s;%s", stockId, stockClient));
    } else {
        // File Bitmap
        XMLUtils::SetText(element, wxString::Format("Load From File;%s", XMLUtils::GetText(propertyElement)));
    }
}

void XrcToXfbFilter::SetColourProperty(tinyxml2::XMLElement* element, const wxString& name) const
{
    const auto* propertyElement = m_xrcElement->FirstChildElement(name.utf8_str());
    if (!propertyElement) {
        return;
    }

    wxColour colour;
    colour.Set(XMLUtils::GetText(propertyElement));
    // TODO: TypeConv is not available here, copy its code
    XMLUtils::SetText(element, wxString::Format("%d,%d,%d", colour.Red(), colour.Green(), colour.Blue()));
}

void XrcToXfbFilter::SetFontProperty(tinyxml2::XMLElement* element, const wxString& name) const
{
    const auto* propertyElement = m_xrcElement->FirstChildElement(name.utf8_str());
    if (!propertyElement) {
        return;
    }

    // TODO: Since wxWidgets 3.1.2 fractional sizes are supported
    int fontSize = -1;
    if (const auto* sizeElement = propertyElement->FirstChildElement("size")) {
        fontSize = sizeElement->IntText(fontSize);
    }

    auto fontStyle = wxFONTSTYLE_NORMAL;
    if (const auto* styleElement = propertyElement->FirstChildElement("style")) {
        auto styleValue = XMLUtils::GetText(styleElement);
        if (styleValue == "normal") {
            fontStyle = wxFONTSTYLE_NORMAL;
        } else if (styleValue == "italic") {
            fontStyle = wxFONTSTYLE_ITALIC;
        } else if (styleValue == "slant") {
            fontStyle = wxFONTSTYLE_SLANT;
        }
    }

    // TODO: Since wxWidgets 3.1.2 more weights are supported
    auto fontWeight = wxFONTWEIGHT_NORMAL;
    if (const auto* weightElement = propertyElement->FirstChildElement("weight")) {
        auto weightValue = XMLUtils::GetText(weightElement);
        if (weightValue == "light") {
            fontWeight = wxFONTWEIGHT_LIGHT;
        } else if (weightValue == "normal") {
            fontWeight = wxFONTWEIGHT_NORMAL;
        } else if (weightValue == "bold") {
            fontWeight = wxFONTWEIGHT_BOLD;
        }
    }

    auto fontFamily = wxFONTFAMILY_DEFAULT;
    if (const auto* familyElement = propertyElement->FirstChildElement("family")) {
        auto familyValue = XMLUtils::GetText(familyElement);
        if (familyValue == "default") {
            fontFamily = wxFONTFAMILY_DEFAULT;
        } else if (familyValue == "roman") {
            fontFamily = wxFONTFAMILY_ROMAN;
        } else if (familyValue == "script") {
            fontFamily = wxFONTFAMILY_SCRIPT;
        } else if (familyValue == "decorative") {
            fontFamily = wxFONTFAMILY_DECORATIVE;
        } else if (familyValue == "swiss") {
            fontFamily = wxFONTFAMILY_SWISS;
        } else if (familyValue == "modern") {
            fontFamily = wxFONTFAMILY_MODERN;
        } else if (familyValue == "teletype") {
            fontFamily = wxFONTFAMILY_TELETYPE;
        }
    }

    int fontUnderlined = 0;
    if (const auto* underlinedElement = propertyElement->FirstChildElement("underlined")) {
        fontUnderlined = underlinedElement->IntText(fontUnderlined);
    }

    // TODO: Since wxWidgets 3.1.2 strikethrough

    // TODO: This is actually a comma separated list
    wxString fontFace;
    if (const auto* faceElement = propertyElement->FirstChildElement("face")) {
        fontFace = XMLUtils::GetText(faceElement, fontFace);
    }

    // TODO: Additional elements missing: encoding, sysfont, inherit, relativesize

    // TODO: TypeConv is not available here, copy its code
    XMLUtils::SetText(element, wxString::Format(
        "%s,%d,%d,%d,%d,%d",
        fontFace,
        fontStyle,
        fontWeight,
        fontSize,
        fontFamily,
        fontUnderlined
    ));
}

void XrcToXfbFilter::SetBitlistProperty(tinyxml2::XMLElement* element, const wxString& name) const
{
    const auto* propertyElement = m_xrcElement->FirstChildElement(name.utf8_str());
    if (!propertyElement) {
        return;
    }

    auto bitlist = XMLUtils::GetText(propertyElement);
    bitlist = ReplaceSynonymous(m_lib, bitlist);
    XMLUtils::SetText(element, bitlist);
}

void XrcToXfbFilter::SetStringListProperty(tinyxml2::XMLElement* element, const wxString& name, bool xrcFormat) const
{
    const auto* propertyElement = m_xrcElement->FirstChildElement(name.utf8_str());
    if (!propertyElement) {
        return;
    }

    wxString stringList;
    stringList.reserve(512);
    for (const auto* itemElement = propertyElement->FirstChildElement("item"); itemElement; itemElement = itemElement->NextSiblingElement("item")) {
        auto itemValue = XMLUtils::GetText(itemElement);
        // FIXME: Doesn't this require to escape at least the quotes? The wxArrayStringProperty escapes even more values!
        stringList.append("\"");
        stringList.append(xrcFormat ? XrcTextToString(itemValue) : itemValue);
        stringList.append("\" ");
    }
    stringList.Trim(true);
    XMLUtils::SetText(element, stringList);
}


void XrcToXfbFilter::AddStyleProperty()
{
    const auto* styleProperty = m_xrcElement->FirstChildElement("style");
    if (!styleProperty) {
        return;
    }
    auto styleValue = XMLUtils::GetText(styleProperty);
    styleValue = ReplaceSynonymous(m_lib, styleValue);

    // FIXME: Hardcoded list to separate wxWindow styles from widget styles, better extract from data model
    static const std::set<wxString> windowStyles = {
        "wxBORDER_DEFAULT",
        "wxBORDER_SIMPLE",
        "wxBORDER_SUNKEN",
        "wxBORDER_RAISED",
        "wxBORDER_STATIC",
        "wxBORDER_THEME",
        "wxBORDER_NONE",
        "wxTRANSPARENT_WINDOW",
        "wxTAB_TRAVERSAL",
        "wxWANTS_CHARS",
        "wxVSCROLL",
        "wxHSCROLL",
        "wxALWAYS_SHOW_SB",
        "wxCLIP_CHILDREN",
        "wxFULL_REPAINT_ON_RESIZE",
        "wxNO_FULL_REPAINT_ON_RESIZE",
    };

    wxString windowStyle;
    windowStyle.reserve(styleValue.size());
    wxString widgetStyle;
    widgetStyle.reserve(styleValue.size());
    wxStringTokenizer tkz(styleValue, "|");
    while (tkz.HasMoreTokens()) {
        auto token = tkz.GetNextToken();
        token.Trim(true);
        token.Trim(false);

        auto& style = (windowStyles.find(token) != windowStyles.end() ? windowStyle : widgetStyle);
        if (!style.empty()) {
            style.append("|");
        }
        style.append(token);
    }

    if (!windowStyle.empty()) {
        AddPropertyValue("window_style", windowStyle);
    }
    if (!widgetStyle.empty()) {
        AddPropertyValue("style", widgetStyle);
    }
}

void XrcToXfbFilter::AddExtraStyleProperty()
{
    const auto* extraStyleProperty = m_xrcElement->FirstChildElement("exstyle");
    if (!extraStyleProperty) {
        return;
    }
    auto extraStyleValue = XMLUtils::GetText(extraStyleProperty);
    extraStyleValue = ReplaceSynonymous(m_lib, extraStyleValue);

    // FIXME: Hardcoded list to separate wxWindow extra styles from widget extra styles, better extract from data model
    static const std::set<wxString> windowExtraStyles = {
        "wxWS_EX_VALIDATE_RECURSIVELY",
        "wxWS_EX_BLOCK_EVENTS",
        "wxWS_EX_TRANSIENT",
        "wxWS_EX_CONTEXTHELP",
        "wxWS_EX_PROCESS_IDLE",
        "wxWS_EX_PROCESS_UI_UPDATES",
    };

    wxString windowExtraStyle;
    windowExtraStyle.reserve(extraStyleValue.size());
    wxString widgetExtraStyle;
    widgetExtraStyle.reserve(extraStyleValue.size());
    wxStringTokenizer tkz(extraStyleValue, "|");
    while (tkz.HasMoreTokens()) {
        auto token = tkz.GetNextToken();
        token.Trim(true);
        token.Trim(false);

        auto& style = (windowExtraStyles.find(token) != windowExtraStyles.end() ? windowExtraStyle : widgetExtraStyle);
        if (!style.empty()) {
            style.append("|");
        }
        style.append(token);
    }

    if (!windowExtraStyle.empty()) {
        AddPropertyValue("window_extra_style", windowExtraStyle);
    }
    if (!widgetExtraStyle.empty()) {
        AddPropertyValue("extra_style", widgetExtraStyle);
    }
}
