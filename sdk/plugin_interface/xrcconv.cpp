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
    tinyxml2::XMLElement* xrcElement,
    const IComponentLibrary* lib, const IObject* obj,
    std::optional<wxString> className, std::optional<wxString> objectName
) :
    m_lib(lib), m_obj(obj),
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


void ObjectToXrcFilter::AddProperty(Type propType, const wxString& objPropName, const wxString& xrcPropName)
{
    auto* propertyElement = m_xrcElement->InsertNewChildElement(!xrcPropName.empty() ? xrcPropName.utf8_str() : objPropName.utf8_str());
    switch (propType) {
        case Type::Bool:
        case Type::Integer:
            SetInteger(propertyElement, m_obj->GetPropertyAsInteger(objPropName));
            break;
        case Type::Float:
            SetFloat(propertyElement, m_obj->GetPropertyAsFloat(objPropName));
            break;
        case Type::String:
        case Type::Point:
        case Type::Size:
        case Type::Option:
        case Type::BitList:
            SetText(propertyElement, m_obj->GetPropertyAsString(objPropName), false);
            break;
        case Type::Text:
            // The text must be converted to XRC format
            SetText(propertyElement, m_obj->GetPropertyAsString(objPropName), true);
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
                SetText(propertyElement, filename.Trim().Trim(false));
            } else if (bitmapProp.StartsWith("Load From Art Provider")) {
                XMLUtils::SetAttribute(propertyElement, "stock_id", filename.BeforeFirst(';').Trim().Trim(false));
                XMLUtils::SetAttribute(propertyElement, "stock_client", filename.AfterFirst(';').Trim().Trim(false));
                // TODO: Don't specify a fallback bitmap at all?
                SetText(propertyElement, "undefined.png");
            }
            break;
        }
        case Type::Colour:
            SetColour(propertyElement, m_obj->GetPropertyAsColour(objPropName));
            break;
        case Type::Font:
            SetFont(propertyElement, m_obj->GetPropertyAsFont(objPropName));
            break;
        case Type::TextList:
            SetStringList(propertyElement, m_obj->GetPropertyAsArrayString(objPropName));
            break;
    }
}

void ObjectToXrcFilter::AddPropertyValue(const wxString& xrcPropName, const wxString& xrcPropValue, bool xrcFormat)
{
    auto* propertyElement = m_xrcElement->InsertNewChildElement(xrcPropName.utf8_str());
    SetText(propertyElement, xrcPropValue, xrcFormat);
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
        AddProperty(Type::Size, "pos");
    }
    if (!m_obj->IsPropertyNull("size")) {
        AddProperty(Type::Size, "size");
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
        AddProperty(Type::Colour, "fg");
    }
    // TODO: ownfg
    if (!m_obj->IsPropertyNull("bg")) {
        AddProperty(Type::Colour, "bg");
    }
    // TODO: ownbg

    if (!m_obj->IsPropertyNull("enabled") && m_obj->GetPropertyAsInteger("enabled") == 0) {
        AddProperty(Type::Bool, "enabled");
    }
    // TODO: This property does not exist in the data model
    //if (!m_obj->IsPropertyNull("focused")) {
    //    AddPropertyValue("focused", "0");
    //}
    if (!m_obj->IsPropertyNull("hidden") && m_obj->GetPropertyAsInteger("hidden") != 0) {
        AddProperty(Type::Bool, "hidden");
    }

    if (!m_obj->IsPropertyNull("tooltip")) {
        AddProperty(Type::Text, "tooltip");
    }

    // TODO: variant

    if (!m_obj->IsPropertyNull("font")) {
        AddProperty(Type::Font, "font");
    }
    // TODO: ownfont

    // TODO: help

    if (!m_obj->IsPropertyNull("subclass")) {
        auto subclass = m_obj->GetChildFromParentProperty("subclass", "name");
        if (!subclass.empty()) {
            XMLUtils::SetAttribute(m_xrcElement, "subclass", subclass);
        }
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


XrcToXfbFilter::XrcToXfbFilter(
    tinyxml2::XMLElement* xfbElement,
    const IComponentLibrary* lib, const tinyxml2::XMLElement* xrcElement,
    std::optional<wxString> className, std::optional<wxString> objectName
) :
    m_lib(lib), m_xrcElement(xrcElement),
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


void XrcToXfbFilter::AddProperty(Type propType, const wxString& xrcPropName, const wxString& xfbPropName)
{
    auto* propertyElement = m_xfbElement->InsertNewChildElement("property");
    XMLUtils::SetAttribute(propertyElement, "name", !xfbPropName.empty() ? xfbPropName : xrcPropName);
    switch (propType) {
        case Type::Bool:
        case Type::String:
        case Type::Point:
        case Type::Size:
            SetTextProperty(propertyElement, xrcPropName, false);
            break;
        case Type::Integer:
            SetIntegerProperty(propertyElement, xrcPropName);
            break;
        case Type::Float:
            SetFloatProperty(propertyElement, xrcPropName);
            break;
        case Type::Text:
            SetTextProperty(propertyElement, xrcPropName, true);
            break;
        case Type::Bitmap:
            SetBitmapProperty(propertyElement, xrcPropName);
            break;
        case Type::Colour:
            SetColourProperty(propertyElement, xrcPropName);
            break;
        case Type::Font:
            SetFontProperty(propertyElement, xrcPropName);
            break;
        case Type::Option:
            SetOptionProperty(propertyElement, xrcPropName);
            break;
        case Type::BitList:
            SetBitlistProperty(propertyElement, xrcPropName);
            break;
        case Type::TextList:
            SetStringListProperty(propertyElement, xrcPropName, true);
            break;
    }
}

void XrcToXfbFilter::AddPropertyValue(const wxString& xfbPropName, const wxString& xfbPropValue, bool parseXrcText)
{
    auto* propertyElement = m_xfbElement->InsertNewChildElement("property");
    XMLUtils::SetAttribute(propertyElement, "name", xfbPropName);

    const auto& value = (parseXrcText ? XrcTextToString(xfbPropValue) : xfbPropValue);
    XMLUtils::SetText(propertyElement, value);
}

void XrcToXfbFilter::AddPropertyPair(
  const wxString& xrcPropName, const wxString& xfbPropName1, const wxString& xfbPropName2)
{
    const auto* propertyElement = m_xrcElement->FirstChildElement(xrcPropName.utf8_str());
    if (!propertyElement) {
        return;
    }

    wxString second;
    auto first = XMLUtils::GetText(propertyElement).BeforeFirst(',', &second);
    AddPropertyValue(xfbPropName1, first);
    AddPropertyValue(xfbPropName2, second);
}


void XrcToXfbFilter::AddWindowProperties()
{
    // FIXME: Check existence of all properties before adding them
    AddProperty(Type::Point, "pos");
    AddProperty(Type::Size, "size");

    AddStyleProperty();
    AddExtraStyleProperty();

    AddProperty(Type::Colour, "fg");
    // TODO: ownfg
    AddProperty(Type::Colour, "bg");
    // TODO: ownbg

    AddProperty(Type::Bool, "enabled");
    // TODO: focused does not exist in the data model
    AddProperty(Type::Bool, "hidden");

    AddProperty(Type::Text, "tooltip");

    // TODO: variant

    AddProperty(Type::Font, "font");
    // TODO: ownfont

    // TODO: help

    auto subclass = XMLUtils::StringAttribute(m_xrcElement, "subclass");
    if (!subclass.empty()) {
        AddPropertyValue("subclass", subclass);
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

void XrcToXfbFilter::SetOptionProperty(tinyxml2::XMLElement* element, const wxString& name) const
{
    const auto* propertyElement = m_xrcElement->FirstChildElement(name.utf8_str());
    if (!propertyElement) {
        return;
    }

    auto option = XMLUtils::GetText(propertyElement);
    option = m_lib->ReplaceSynonymous(option);
    XMLUtils::SetText(element, option);
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
