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

#include "component.h"


namespace ticpp
{
class Element;
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
      const IComponentLibrary* lib, const IObject* obj, const wxString& classname,
      const wxString& objname = wxEmptyString, const wxString& base = wxEmptyString);
    ~ObjectToXrcFilter();

    void AddProperty(const wxString& objPropName, const wxString& xrcPropName, Type propType);
    void AddPropertyValue(const wxString& xrcPropName, const wxString& xrcPropValue, bool xrcFormat = false);
    void AddPropertyPair(const wxString& objPropName1, const wxString& objPropName2, const wxString& xrcPropName);

    void AddWindowProperties();

    ticpp::Element* GetXrcObject();

private:
    void LinkInteger(int integer, ticpp::Element* propElement);
    void LinkFloat(double value, ticpp::Element* propElement);
    void LinkText(const wxString& text, ticpp::Element* propElement, bool xrcFormat = false);
    void LinkColour(const wxColour& colour, ticpp::Element* propElement);
    void LinkFont(const wxFontContainer& font, ticpp::Element* propElement);
    void LinkStringList(const wxArrayString& array, ticpp::Element* propElement, bool xrcFormat = false);

private:
    const IComponentLibrary* m_lib;
    const IObject* m_obj;

    ticpp::Element* m_xrcObj;
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
    XrcToXfbFilter(const IComponentLibrary* lib, const ticpp::Element* obj, const wxString& classname);
    XrcToXfbFilter(
      const IComponentLibrary* lib, const ticpp::Element* obj, const wxString& classname, const wxString& objname);
    ~XrcToXfbFilter();

    void AddProperty(const wxString& xrcPropName, const wxString& xfbPropName, Type propType);
    void AddPropertyValue(const wxString& xfbPropName, const wxString& xfbPropValue, bool parseXrcText = false);
    void AddPropertyPair(const wxString& xrcPropName, const wxString& xfbPropName1, const wxString& xfbPropName2);

    void AddWindowProperties();

    ticpp::Element* GetXfbObject();

private:
    void ImportIntegerProperty(const wxString& xrcPropName, ticpp::Element* property);
    void ImportFloatProperty(const wxString& xrcPropName, ticpp::Element* property);
    void ImportTextProperty(const wxString& xrcPropName, ticpp::Element* property, bool parseXrcText = false);
    void ImportBitmapProperty(const wxString& xrcPropName, ticpp::Element* property);
    void ImportColourProperty(const wxString& xrcPropName, ticpp::Element* property);
    void ImportFontProperty(const wxString& xrcPropName, ticpp::Element* property);
    void ImportBitlistProperty(const wxString& xrcPropName, ticpp::Element* property);
    void ImportStringListProperty(const wxString& xrcPropName, ticpp::Element* property, bool parseXrcText = false);

    void AddStyleProperty();
    void AddExtraStyleProperty();

    ticpp::Element* GetXrcProperty(const wxString& name);

private:
    const IComponentLibrary* m_lib;
    const ticpp::Element* m_xrcObj;

    ticpp::Element* m_xfbObj;
};

#endif  // SDK_PLUGIN_INTERFACE_XRCCONV_H
