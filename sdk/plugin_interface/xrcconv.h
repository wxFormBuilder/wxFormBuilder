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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   José Antonio Hurtado - joseantonio.hurtado@gmail.com
//   Juan Antonio Ortega  - jortegalalmolda@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

#include "wx/wx.h"
#include "component.h"

#define XRC_TYPE_TEXT    0
#define XRC_TYPE_INTEGER 1
#define XRC_TYPE_BOOL    2
#define XRC_TYPE_COLOUR  3
#define XRC_TYPE_FONT    4
#define XRC_TYPE_BITLIST 5
#define XRC_TYPE_SIZE    6
#define XRC_TYPE_POINT   7
#define XRC_TYPE_STRINGLIST 8
#define XRC_TYPE_BITMAP  9
#define XRC_TYPE_FLOAT 10

namespace ticpp
{
	class Element;
}

/**
 * Filter for exporting an objetc to XRC format.
 *
 * This class helps exporting an objetc to XRC format. Just it's needed to setup
 * the properties names' "mapping" with their types, and the XML element will be
 * created in XRC format.
 *
 * For instance:
 *
 * @code
 *  ...
 *  ObjectToXrcFilter xrc(obj, "wxButton", "button1");
 *  xrc.AddProperty("label", "label", XRC_TYPE_STRING);
 *  xrc.AddProperty("style", "style", XRC_TYPE_BITLIST);
 *  xrc.AddProperty("default", "default", XRC_TYPE_BOOL);
 *  ticpp::Element *xrcObj = xrc.GetXrcObject();
 * @endcode
 */
class ObjectToXrcFilter
{
 public:
  ObjectToXrcFilter(IObject *obj,
                    const wxString &classname,
                    const wxString &objname = wxT(""),
                    const wxString &base = wxT(""));

  ~ObjectToXrcFilter();

  void AddProperty (const wxString &objPropName,
                    const wxString &xrcPropName,
                    const int &propType);

  void AddPropertyValue (const wxString &xrcPropName,
                         const wxString &xrcPropValue);

  void AddPropertyPair ( const wxString& objPropName1, const wxString& objPropName2, const wxString& xrcPropName );

  void AddWindowProperties();

  ticpp::Element* GetXrcObject();

 private:
  ticpp::Element* m_xrcObj;
  IObject *m_obj;

  void LinkText(const wxString &text, ticpp::Element *propElement, bool xrcFormat = false);
  void LinkColour(const wxColour &colour, ticpp::Element *propElement);
  void LinkFont(const wxFontContainer &font, ticpp::Element *propElement);
  void LinkInteger(const int &integer, ticpp::Element *propElement);
  void LinkFloat( const double& value, ticpp::Element* propElement );
  void LinkStringList(const wxArrayString &array, ticpp::Element *propElement,  bool xrcFormat = false);
};

/**
 * Filter for exporting an XRC object to XFB format (Xml-FormBuilder)
 *
 * The usage is similar to the ObjectToXrcFilter filter. It's only
 * needed to add the properties with their related types.
 *
 */
class XrcToXfbFilter
{
 public:

  XrcToXfbFilter(ticpp::Element *obj,
                 const wxString &classname);

  XrcToXfbFilter(ticpp::Element *obj,
                 const wxString &classname,
                 const wxString &objname);
  ~XrcToXfbFilter();

  void AddProperty (const wxString &xrcPropName,
                    const wxString &xfbPropName, const int &propType);

  void AddPropertyValue (const wxString &xfbPropName,
                         const wxString &xfbPropValue);

  void AddWindowProperties();

  void AddPropertyPair( const char* xrcPropName, const wxString& xfbPropName1, const wxString& xfbPropName2 );

  ticpp::Element* GetXfbObject();

 private:
  ticpp::Element *m_xfbObj;
  ticpp::Element *m_xrcObj;

  void ImportTextProperty(const wxString &xrcPropName, ticpp::Element *property, bool parseXrcText = false);
  void ImportIntegerProperty(const wxString &xrcPropName, ticpp::Element *property);
  void ImportFloatProperty(const wxString &xrcPropName, ticpp::Element *property);
  void ImportBitlistProperty(const wxString &xrcPropName, ticpp::Element *property);
  void ImportBitmapProperty(const wxString &xrcPropName, ticpp::Element *property);
  void ImportColourProperty(const wxString &xrcPropName, ticpp::Element *property);
  void ImportFontProperty(const wxString &xrcPropName, ticpp::Element *property);
  void ImportStringListProperty(const wxString &xrcPropName, ticpp::Element *property, bool parseXrcText = false);

  void AddStyleProperty();
  void AddExtraStyleProperty();

  ticpp::Element *GetXrcProperty(const wxString &name);
};


