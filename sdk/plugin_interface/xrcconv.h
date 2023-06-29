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

#ifndef SDK_PLUGIN_INTERFACE_XRCCONV_H
#define SDK_PLUGIN_INTERFACE_XRCCONV_H

#include <optional>

#include "component.h"


namespace tinyxml2
{
class XMLElement;
}


/**
 * @brief Base interface for XRC filter classes
 *
 * This class is only used to define common data types.
 */
class XrcFilter
{
public:
    enum class Type {
        Bool = 0,
        Integer,
        Float,
        Text,
        Point,
        Size,
        Bitmap,
        Colour,
        Font,
        BitList,
        StringList,
    };

protected:
    ~XrcFilter() = default;
};


/**
 * @brief Filter for exporting an object to XRC format
 *
 * This class helps exporting an object to XRC format. It is just needed to setup
 * the properties names "mapping" with their types, the XML element will be
 * created in XRC format.
 *
 * Example:
 *
 * @code
 *
 * ObjectToXrcFilter xrc(GetLibrary(), obj, "wxButton", "button1");
 * xrc.AddProperty("label", "label", XrcFilter::Type::Text);
 * xrc.AddProperty("style", "style", XrcFilter::Type::BitList);
 * xrc.AddProperty("default", "default", XrcFilter::Type::Bool);
 * auto* xrcObj = xrc.GetXrcObject();
 *
 * @endcode
 */
class ObjectToXrcFilter : public XrcFilter
{
public:
    ObjectToXrcFilter(
        tinyxml2::XMLElement* xrcElement,
        const IComponentLibrary* lib, const IObject* obj,
        std::optional<wxString> className = std::nullopt,
        std::optional<wxString> objectName = std::nullopt
    );

    void AddProperty(Type propType, const wxString& objPropName, const wxString& xrcPropName = wxEmptyString);
    void AddPropertyValue(const wxString& xrcPropName, const wxString& xrcPropValue, bool xrcFormat = false);
    void AddPropertyPair(const wxString& objPropName1, const wxString& objPropName2, const wxString& xrcPropName);

    void AddWindowProperties();

private:
    void SetInteger(tinyxml2::XMLElement* element, int integer) const;
    void SetFloat(tinyxml2::XMLElement* element, double value) const;
    void SetText(tinyxml2::XMLElement* element, const wxString& text, bool xrcFormat = false) const;
    void SetColour(tinyxml2::XMLElement* element, const wxColour& colour) const;
    void SetFont(tinyxml2::XMLElement* element, const wxFontContainer& font) const;
    void SetStringList(tinyxml2::XMLElement* element, const wxArrayString& array, bool xrcFormat = false) const;

private:
    const IComponentLibrary* m_lib;
    const IObject* m_obj;

    tinyxml2::XMLElement* m_xrcElement;
};


/**
 * @brief Filter for exporting a XRC object to XFB format (Xml-FormBuilder)
 *
 * The usage is similar to the ObjectToXrcFilter filter. It is only
 * needed to add the properties with their related types.
 */
class XrcToXfbFilter : public XrcFilter
{
public:
    XrcToXfbFilter(
        tinyxml2::XMLElement* xfbElement,
        const IComponentLibrary* lib, const tinyxml2::XMLElement* xrcElement,
        std::optional<wxString> className = std::nullopt,
        std::optional<wxString> objectName = std::nullopt
    );

    void AddProperty(Type propType, const wxString& xrcPropName, const wxString& xfbPropName = wxEmptyString);
    void AddPropertyValue(const wxString& xfbPropName, const wxString& xfbPropValue, bool parseXrcText = false);
    void AddPropertyPair(const wxString& xrcPropName, const wxString& xfbPropName1, const wxString& xfbPropName2);

    void AddWindowProperties();

private:
    void SetIntegerProperty(tinyxml2::XMLElement* element, const wxString& name) const;
    void SetFloatProperty(tinyxml2::XMLElement* element, const wxString& name) const;
    void SetTextProperty(tinyxml2::XMLElement* element, const wxString& name, bool xrcFormat = false) const;
    void SetBitmapProperty(tinyxml2::XMLElement* element, const wxString& name) const;
    void SetColourProperty(tinyxml2::XMLElement* element, const wxString& name) const;
    void SetFontProperty(tinyxml2::XMLElement* element, const wxString& name) const;
    void SetBitlistProperty(tinyxml2::XMLElement* element, const wxString& name) const;
    void SetStringListProperty(tinyxml2::XMLElement* element, const wxString& name, bool xrcFormat = false) const;

    void AddStyleProperty();
    void AddExtraStyleProperty();

private:
    const IComponentLibrary* m_lib;
    const tinyxml2::XMLElement* m_xrcElement;

    tinyxml2::XMLElement* m_xfbElement;
};

#endif  // SDK_PLUGIN_INTERFACE_XRCCONV_H
