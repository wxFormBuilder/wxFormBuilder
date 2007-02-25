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

#include "xrcconv.h"
#include "wx/wx.h"
#include "wx/tokenzr.h"

#include <string>
#include <set>

static wxString StringToXrcText(const wxString &str)
{
  wxString result;

  for (unsigned int i=0 ; i < str.Length() ; i++)
  {
    wxChar c = str[i];

    switch (c)
    {
      case wxChar('\n'): result = result + wxT("\\n");
                 break;

      case wxChar('\t'): result = result + wxT("\\t");
                 break;

      case wxChar('\r'): result = result + wxT("\\r");
                 break;

      case wxChar('\\'): result = result + wxT("\\\\");
                 break;

      case wxChar('_'):  result = result + wxT("__");
                 break;

      case wxChar('&'):  result = result + wxT("_");
                 break;

      default:   result = result + c;
                 break;
    }
  }
  return result;
}

static wxString XrcTextToString(const wxString &str)
{
  wxString result;

  for (unsigned int i=0 ; i < str.Length() ; i++)
  {
    wxChar c = str[i];
    if (c == wxChar('\\') && i < str.length() - 1)
    {
      wxChar next = str[i+1];

      switch (next)
      {
        case wxChar('n'): result = result + wxChar('\n'); i++;
                  break;

        case wxChar('t'): result = result + wxChar('\t'); i++;
                  break;

        case wxChar('r'): result = result + wxChar('\r'); i++;
                  break;

        case wxChar('\\'): result = result + wxChar('\\'); i++;
                   break;
      }
    }
    else if (c == wxChar('_'))
    {
      if (i < str.Length() - 1 && str[i+1] == wxChar('_'))
      {
        result = result + wxChar('_');
        i++;
      }
      else
        result = result + wxChar('&');
    }
    else
      result = result + c;
  }

  return result;
}

static wxString ReplaceSynonymous(const wxString &bitlist)
{
  IComponentLibrary* lib = GetComponentLibrary( NULL );
  wxString result, translation;
  wxStringTokenizer tkz(bitlist, wxT("|"));
  while (tkz.HasMoreTokens())
  {
    wxString token;
    token = tkz.GetNextToken();
    token.Trim(true);
    token.Trim(false);

    if (result != wxT(""))
        result = result + wxChar('|');

    if (lib->FindSynonymous(token, translation))
      result += translation;
    else
      result += token;

  }
  delete lib;

  return result;
}

ObjectToXrcFilter::ObjectToXrcFilter(IObject *obj, const wxString &classname,
                                     const wxString &objname,
                                     const wxString &base)
{
  m_obj = obj;
  m_xrcObj = new TiXmlElement("object");

  m_xrcObj->SetAttribute("class",classname.mb_str( wxConvUTF8 ));

  if (objname != wxT(""))
    m_xrcObj->SetAttribute("name",objname.mb_str( wxConvUTF8 ));

  if (base != wxT(""))
    m_xrcObj->SetAttribute("base",base.mb_str( wxConvUTF8 ));
}

ObjectToXrcFilter::~ObjectToXrcFilter()
{
  delete m_xrcObj;
}

void ObjectToXrcFilter::AddProperty(const wxString &objPropName,
                                    const wxString &xrcPropName,
                                    const int &propType)
{
	std::string name( xrcPropName.mb_str( wxConvUTF8 ) );
  TiXmlElement *propElement = new TiXmlElement(name);

  switch (propType)
  {
    case XRC_TYPE_SIZE:
    case XRC_TYPE_POINT:
    case XRC_TYPE_BITLIST:
      LinkText(m_obj->GetPropertyAsString(objPropName), propElement);
      break;

    case XRC_TYPE_TEXT:
      // El texto ha de ser convertido a formato XRC
      {
      wxString text = m_obj->GetPropertyAsString(objPropName);
      LinkText(text, propElement, true);
      }
      break;

    case XRC_TYPE_BOOL:
    case XRC_TYPE_INTEGER:
      LinkInteger(m_obj->GetPropertyAsInteger(objPropName), propElement);
      break;

    case XRC_TYPE_COLOUR:
      LinkColour(m_obj->GetPropertyAsColour(objPropName), propElement);
      break;

    case XRC_TYPE_FONT:
      LinkFont(m_obj->GetPropertyAsFont(objPropName), propElement);
      break;

    case XRC_TYPE_STRINGLIST:
      // LinkStringList convierte las cadenas a formato XRC
      LinkStringList(m_obj->GetPropertyAsArrayString(objPropName), propElement, true);
      break;

    case XRC_TYPE_BITMAP:
      {
        wxString bitmapProp = m_obj->GetPropertyAsString(objPropName);
        wxString filename = bitmapProp.BeforeFirst(_T(';')).Trim();
        wxString source = bitmapProp.AfterLast(_T(';')).Trim(false);
        if (source == wxT("Load From File"))
            LinkText(filename, propElement);
      }
      break;
  }

  m_xrcObj->LinkEndChild(propElement);
}

void ObjectToXrcFilter::AddPropertyValue (const wxString &xrcPropName,
                                          const wxString &xrcPropValue)
{
  TiXmlElement *propElement = new TiXmlElement(xrcPropName.mb_str( wxConvUTF8 ));
  LinkText(xrcPropValue, propElement);
  m_xrcObj->LinkEndChild(propElement);
}

void ObjectToXrcFilter::AddPropertyPair ( const wxString& objPropName1, const wxString& objPropName2, const wxString& xrcPropName )
{
	AddPropertyValue( 	xrcPropName,
						wxString::Format( _("%d,%d"),
							m_obj->GetPropertyAsInteger( objPropName1 ),
							m_obj->GetPropertyAsInteger( objPropName2 )
						)
					);
}

TiXmlElement* ObjectToXrcFilter::GetXrcObject()
{
  return (m_xrcObj->Clone())->ToElement();
}

void ObjectToXrcFilter::LinkText(const wxString &text,TiXmlElement *propElement, bool xrcFormat)
{
  wxString value = (xrcFormat ? StringToXrcText(text) : text);
  std::string val( value.mb_str( wxConvUTF8 ) );
  propElement->LinkEndChild(new TiXmlText(val));
}

void ObjectToXrcFilter::LinkInteger(const int &integer, TiXmlElement *propElement)
{
  wxString text = wxString::Format(wxT("%d"),integer);
  propElement->LinkEndChild(new TiXmlText(text.mb_str( wxConvUTF8 )));
}


void ObjectToXrcFilter::LinkColour(const wxColour &colour,
                                   TiXmlElement *propElement)
{

  wxString value = wxString::Format(wxT("#%02x%02x%02x"),
                     colour.Red(), colour.Green(), colour.Blue());

  propElement->LinkEndChild(new TiXmlText(value.mb_str( wxConvUTF8 )));
}

void ObjectToXrcFilter::LinkFont(const wxFont &font, TiXmlElement *propElement)
{
  wxString aux;
  TiXmlElement *element = new TiXmlElement("size");
  aux.Printf(wxT("%d"), font.GetPointSize());
  element->LinkEndChild(new TiXmlText(aux.mb_str( wxConvUTF8 )));
  propElement->LinkEndChild(element);

  element = new TiXmlElement("family");
  switch (font.GetFamily())
  {
      case wxDECORATIVE:
          element->LinkEndChild(new TiXmlText("decorative"));
          break;
      case wxROMAN:
          element->LinkEndChild(new TiXmlText("roman"));
          break;
      case wxSWISS:
          element->LinkEndChild(new TiXmlText("swiss"));
          break;
      case wxMODERN:
          element->LinkEndChild(new TiXmlText("modern"));
          break;
      default:
          element->LinkEndChild(new TiXmlText("default"));
          break;
  }
  propElement->LinkEndChild(element);

  element = new TiXmlElement("style");
  switch (font.GetStyle())
  {
      case wxSLANT:
          element->LinkEndChild(new TiXmlText("slant"));
          break;
      case wxITALIC:
          element->LinkEndChild(new TiXmlText("italic"));
          break;
      default:
          element->LinkEndChild(new TiXmlText("normal"));
          break;
  }
  propElement->LinkEndChild(element);

  element = new TiXmlElement("weight");
  switch (font.GetWeight())
  {
      case wxLIGHT:
          element->LinkEndChild(new TiXmlText("light"));
          break;
      case wxBOLD:
          element->LinkEndChild(new TiXmlText("bold"));
          break;
      default:
          element->LinkEndChild(new TiXmlText("normal"));
          break;
  }
  propElement->LinkEndChild(element);

  element = new TiXmlElement("underlined");
  element->LinkEndChild(new TiXmlText(font.GetUnderlined() ? "1" : "0"));
  propElement->LinkEndChild(element);

  element = new TiXmlElement("face");
  element->LinkEndChild(new TiXmlText(font.GetFaceName().mb_str( wxConvUTF8 )));
  propElement->LinkEndChild(element);
}

void ObjectToXrcFilter::LinkStringList(const wxArrayString &array, TiXmlElement *propElement, bool xrcFormat)
{
    for (size_t i = 0; i < array.GetCount(); i++)
    {
      wxString value = ( xrcFormat ? StringToXrcText(array[i]) : array[i]);
      TiXmlElement *element = new TiXmlElement("item");
      element->LinkEndChild(new TiXmlText(value.mb_str( wxConvUTF8 )));
      propElement->LinkEndChild(element);
    }
}

void ObjectToXrcFilter::AddWindowProperties()
{
  wxString style;
  if (!m_obj->IsNull(_("style")))
    style = m_obj->GetPropertyAsString(_T("style"));
  if (!m_obj->IsNull(_("window_style"))){
    if (!style.IsEmpty()) style += _T('|');
    style += m_obj->GetPropertyAsString(_T("window_style"));
  }
  if (!style.IsEmpty()) AddPropertyValue(_T("style"), style);

  if (!m_obj->IsNull(_("pos")))
    AddProperty(_("pos"), _("pos"), XRC_TYPE_SIZE);

  if (!m_obj->IsNull(_("size")))
    AddProperty(_("size"), _("size"), XRC_TYPE_SIZE);

  if (!m_obj->IsNull(_("bg")))
    AddProperty(_("bg"), _("bg"), XRC_TYPE_COLOUR);

  if (!m_obj->IsNull(_("fg")))
    AddProperty(_("fg"), _("fg"), XRC_TYPE_COLOUR);

  if (!m_obj->IsNull(_("enabled")) && !m_obj->GetPropertyAsInteger(_("enabled")))
    AddProperty(_("enabled"), _("enabled"), XRC_TYPE_BOOL);

  if (!m_obj->IsNull(_("focused")))
    AddPropertyValue(_("focused"),_("0"));

  if (!m_obj->IsNull(_("hidden")) && m_obj->GetPropertyAsInteger(_("hidden")))
    AddProperty(_("hidden"), _("hidden"), XRC_TYPE_BOOL);

  if (!m_obj->IsNull(_("font")))
    AddProperty(_("font"), _("font"), XRC_TYPE_FONT);

  if (!m_obj->IsNull(_("tooltip")))
    AddProperty(_("tooltip"), wxT("tooltip"), XRC_TYPE_TEXT);

  if (!m_obj->IsNull(_("subclass")))
  {
  	wxString subclass = m_obj->GetChildFromParentProperty( _("subclass"), wxT("name") );
  	if ( !subclass.empty() )
  	{
		m_xrcObj->SetAttribute("subclass", subclass.mb_str( wxConvUTF8 ));
  	}
  }
};

///////////////////////////////////////////////////////////////////////////////

XrcToXfbFilter::XrcToXfbFilter(TiXmlElement *obj, const wxString &classname,
                                                  const wxString &objname)
{
  m_xrcObj = obj;
  m_xfbObj = new TiXmlElement("object");

  if (obj->Attribute("class"))
    m_xfbObj->SetAttribute("class",obj->Attribute("class"));

  if (objname != wxT(""))
    AddProperty(wxT("name"), objname, XRC_TYPE_TEXT);
}

XrcToXfbFilter::XrcToXfbFilter(TiXmlElement *obj, const wxString &classname)
{
  m_xrcObj = obj;
  m_xfbObj = new TiXmlElement("object");

  m_xfbObj->SetAttribute("class",classname.mb_str( wxConvUTF8 ));

  if (obj->Attribute("name"))
  {
    wxString objname(obj->Attribute("name"),wxConvUTF8);
    AddPropertyValue(wxT("name"), objname);
  }
}

XrcToXfbFilter::~XrcToXfbFilter()
{
  delete m_xfbObj;
}


TiXmlElement* XrcToXfbFilter::GetXrcProperty(const wxString &name)
{
  return m_xrcObj->FirstChildElement(name.mb_str( wxConvUTF8 ));
}

void XrcToXfbFilter::AddProperty(const wxString &xrcPropName,
                                 const wxString &xfbPropName,
                                 const int &propType)
{
  TiXmlElement *propElement = new TiXmlElement("property");
  propElement->SetAttribute("name",xfbPropName.mb_str( wxConvUTF8 ));

  switch (propType)
  {
    case XRC_TYPE_SIZE:
    case XRC_TYPE_POINT:
    case XRC_TYPE_BOOL:
      ImportTextProperty(xrcPropName, propElement);
      break;

    case XRC_TYPE_TEXT:
      ImportTextProperty(xrcPropName, propElement, true);
      break;

    case XRC_TYPE_INTEGER:
      ImportIntegerProperty(xrcPropName, propElement);
      break;

    case XRC_TYPE_BITLIST:
      ImportBitlistProperty(xrcPropName, propElement);
      break;

    case XRC_TYPE_COLOUR:
      ImportColourProperty(xrcPropName, propElement);
      break;

    case XRC_TYPE_FONT:
      ImportFontProperty(xrcPropName, propElement);
      break;

    case XRC_TYPE_STRINGLIST:
      ImportStringListProperty(xrcPropName, propElement, true);
      break;

    case XRC_TYPE_BITMAP:
      ImportBitmapProperty(xrcPropName, propElement);
      break;

  }

  m_xfbObj->LinkEndChild(propElement);
}

void XrcToXfbFilter::AddPropertyValue (const wxString &xfbPropName,
                                       const wxString &xfbPropValue)
{
  TiXmlElement *propElement = new TiXmlElement("property");
  propElement->SetAttribute("name",xfbPropName.mb_str( wxConvUTF8 ));

  TiXmlText *propValue = new TiXmlText(xfbPropValue.mb_str( wxConvUTF8 ));

  propElement->LinkEndChild(propValue);
  m_xfbObj->LinkEndChild(propElement);
}

void XrcToXfbFilter::AddStyleProperty()
{
  TiXmlElement *xrcProperty = m_xrcObj->FirstChildElement("style");
  if (xrcProperty)
  {
    TiXmlNode *textElement = xrcProperty->FirstChild();
    if (textElement && textElement->ToText())
    {
      wxString bitlist = wxString(textElement->ToText()->Value(),wxConvUTF8);
      bitlist = ReplaceSynonymous(bitlist);

      // FIXME: We should avoid hardcoding these things
      std::set< wxString > windowStyles;
      windowStyles.insert( wxT("wxSIMPLE_BORDER") );
      windowStyles.insert( wxT("wxDOUBLE_BORDER") );
      windowStyles.insert( wxT("wxSUNKEN_BORDER") );
      windowStyles.insert( wxT("wxRAISED_BORDER") );
      windowStyles.insert( wxT("wxSTATIC_BORDER") );
      windowStyles.insert( wxT("wxNO_BORDER") );
      windowStyles.insert( wxT("wxTRANSPARENT_WINDOW") );
      windowStyles.insert( wxT("wxTAB_TRAVERSAL") );
      windowStyles.insert( wxT("wxWANTS_CHARS") );
      windowStyles.insert( wxT("wxVSCROLL") );
      windowStyles.insert( wxT("wxHSCROLL") );
      windowStyles.insert( wxT("wxALWAYS_SHOW_SB") );
      windowStyles.insert( wxT("wxCLIP_CHILDREN") );
      windowStyles.insert( wxT("wxFULL_REPAINT_ON_RESIZE") );

      wxString style, windowStyle;
      wxStringTokenizer tkz(bitlist, wxT(" |"));
      while (tkz.HasMoreTokens())
      {
        wxString token;
        token = tkz.GetNextToken();
        token.Trim(true);
        token.Trim(false);

        if (windowStyles.find(token) == windowStyles.end())
        {
          if (!style.IsEmpty()) style += _T("|");
          style += token;
        }
        else
        {
          if (!windowStyle.IsEmpty()) windowStyle += _T("|");
          windowStyle += token;
        }

      }

      AddPropertyValue(_T("style"), style);
      AddPropertyValue(_T("window_style"), windowStyle);
    }
  }
}

void XrcToXfbFilter::AddPropertyPair( const char* xrcPropName, const wxString& xfbPropName1, const wxString& xfbPropName2 )
{
	TiXmlElement* pairProp = m_xrcObj->FirstChildElement( xrcPropName );
	if ( pairProp )
	{
		TiXmlText* xmlValue = pairProp->FirstChild()->ToText();
		if ( xmlValue )
		{
			wxString width = wxEmptyString;
			wxString height = wxEmptyString;
			wxStringTokenizer tkz( wxString( xmlValue->Value(), wxConvUTF8 ), wxT(",") );
			if ( tkz.HasMoreTokens() )
			{
				width = tkz.GetNextToken();
				if ( tkz.HasMoreTokens() )
				{
					height = tkz.GetNextToken();
				}
			}
			AddPropertyValue( xfbPropName1, width );
			AddPropertyValue( xfbPropName2, height );
		}
	}
}

TiXmlElement* XrcToXfbFilter::GetXfbObject()
{
  return (m_xfbObj->Clone())->ToElement();
}

//-----------------------------

void XrcToXfbFilter::ImportTextProperty(const wxString &xrcPropName,
                                        TiXmlElement *property, bool parseXrcText)
{
  TiXmlElement *xrcProperty = m_xrcObj->FirstChildElement(xrcPropName.mb_str( wxConvUTF8 ));
  if (xrcProperty)
  {
    TiXmlNode *textElement = xrcProperty->FirstChild();
    if (textElement && textElement->ToText())
    {
      // Convertimos el texto XRC a texto normal
      wxString value(wxString(textElement->ToText()->Value(),wxConvUTF8));

      if (parseXrcText)
        value = XrcTextToString(value);

      TiXmlText *xmlText = new TiXmlText(value.mb_str( wxConvUTF8 ));
      property->LinkEndChild(xmlText);
    }
  }
}

void XrcToXfbFilter::ImportIntegerProperty(const wxString &xrcPropName,
                                        TiXmlElement *property)
{
  TiXmlElement *xrcProperty = m_xrcObj->FirstChildElement(xrcPropName.mb_str( wxConvUTF8 ));
  if (xrcProperty)
  {
    TiXmlNode *textElement = xrcProperty->FirstChild();
    if (textElement && textElement->ToText())
      property->LinkEndChild(textElement->Clone());
    else
      property->LinkEndChild(new TiXmlText("0"));
  }
  else
    property->LinkEndChild(new TiXmlText("0"));
}

void XrcToXfbFilter::ImportBitlistProperty(const wxString &xrcPropName,
                                        TiXmlElement *property)
{
  TiXmlElement *xrcProperty = m_xrcObj->FirstChildElement(xrcPropName.mb_str( wxConvUTF8 ));
  if (xrcProperty)
  {
    TiXmlNode *textElement = xrcProperty->FirstChild();
    if (textElement && textElement->ToText())
    {
      wxString bitlist = wxString(textElement->ToText()->Value(),wxConvUTF8);
      bitlist = ReplaceSynonymous(bitlist);
      property->LinkEndChild(new TiXmlText(bitlist.mb_str( wxConvUTF8 )));
    }
  }
}

void XrcToXfbFilter::ImportFontProperty(const wxString &xrcPropName,
                                        TiXmlElement *property)
{
  TiXmlElement *xrcProperty = m_xrcObj->FirstChildElement(xrcPropName.mb_str( wxConvUTF8 ));
  if (!xrcProperty)
    return;

  TiXmlElement *element;
  TiXmlNode *xmlValue;
  wxFont font;

  // el tamaño
  element = xrcProperty->FirstChildElement("size");
  if (element)
  {
    wxString size_str;
    xmlValue = element->FirstChild();
    if (xmlValue && xmlValue->ToText())
      size_str = wxString(xmlValue->ToText()->Value(),wxConvUTF8);

    long size;
    if (size_str.ToLong(&size))
      font.SetPointSize(size);
  }

  // la familia
  element = xrcProperty->FirstChildElement("family");
  if (element)
  {
    wxString family_str;
    xmlValue = element->FirstChild();
    if (xmlValue && xmlValue->ToText())
      family_str = wxString(xmlValue->ToText()->Value(),wxConvUTF8);

    if (family_str == _("decorative"))
      font.SetFamily(wxDECORATIVE);
    else if (family_str == _("roman"))
      font.SetFamily(wxROMAN);
    else if (family_str == _("swiss"))
      font.SetFamily(wxSWISS);
    else if (family_str == _("modern"))
      font.SetFamily(wxMODERN);
    else //if (family_str == "default")
      font.SetFamily(wxDEFAULT);
  }

  // el estilo
  element = xrcProperty->FirstChildElement("style");
  if (element)
  {
    wxString style_str;
    xmlValue = element->FirstChild();
    if (xmlValue && xmlValue->ToText())
      style_str = wxString(xmlValue->ToText()->Value(),wxConvUTF8);

    if (style_str == _("slant"))
      font.SetStyle(wxSLANT);
    else if (style_str == _("italic"))
      font.SetStyle(wxITALIC);
    else //if (style_str == "normal")
      font.SetStyle(wxNORMAL);
  }


  // grosor
  element = xrcProperty->FirstChildElement("weight");
  if (element)
  {
    wxString weight_str;
    xmlValue = element->FirstChild();
    if (xmlValue && xmlValue->ToText())
      weight_str = wxString(xmlValue->ToText()->Value(),wxConvUTF8);

    if (weight_str == _("light"))
      font.SetWeight(wxLIGHT);
    else if (weight_str == _("bold"))
      font.SetWeight(wxBOLD);
    else //if (sweight_str == "normal")
      font.SetWeight(wxNORMAL);
  }

  // subrayado
  element = xrcProperty->FirstChildElement("underlined");
  if (element)
  {
    wxString underlined_str;
    xmlValue = element->FirstChild();
    if (xmlValue && xmlValue->ToText())
      underlined_str = wxString(xmlValue->ToText()->Value(),wxConvUTF8);

    if (underlined_str == _("1"))
      font.SetUnderlined(TRUE);
    else
      font.SetUnderlined(FALSE);
  }

  // tipo de letra
  element = xrcProperty->FirstChildElement("face");
  if (element)
  {
    wxString face;
    xmlValue = element->FirstChild();
    if (xmlValue && xmlValue->ToText())
      face = wxString(xmlValue->ToText()->Value(),wxConvUTF8);

    font.SetFaceName(face);
  }

  if (font.Ok())
  {
  // Ya tenemos el tipo de letra, sólo nos queda pasarlo a formato wxFB
  wxString font_str =
    wxString::Format(wxT("%s,%d,%d,%d"),font.GetFaceName().c_str(), font.GetStyle(),
                                       font.GetWeight(), font.GetPointSize());
  property->LinkEndChild(new TiXmlText(font_str.mb_str( wxConvUTF8 )));
  }

}

void XrcToXfbFilter::ImportBitmapProperty(const wxString &xrcPropName,
                                        TiXmlElement *property)
{
  TiXmlElement *xrcProperty = m_xrcObj->FirstChildElement(xrcPropName.mb_str( wxConvUTF8 ));
  if (!xrcProperty)
    return;

  TiXmlNode *xmlValue = xrcProperty->FirstChild();
  wxString res;
  if (xmlValue && xmlValue->ToText())
    res = wxString(xmlValue->ToText()->Value(), wxConvUTF8);

  res.Trim();
  res += _T("; Load From File");
  property->LinkEndChild(new TiXmlText(res.mb_str( wxConvUTF8 )));
}

void XrcToXfbFilter::ImportColourProperty(const wxString &xrcPropName,
                                        TiXmlElement *property)
{
  TiXmlElement *xrcProperty = m_xrcObj->FirstChildElement(xrcPropName.mb_str( wxConvUTF8 ));
  if (!xrcProperty)
    return;

  TiXmlNode *xmlValue = xrcProperty->FirstChild();
  if (xmlValue && xmlValue->ToText())
  {
    std::string value = xmlValue->ToText()->Value();

    // convertimos el formato "#rrggbb" a "rrr,ggg,bbb"
    std::string hexColour = "0x" + value.substr(1,2) + " 0x" + value.substr(3,2) +
                       " 0x" + value.substr(5,2);
    std::istringstream strIn;
    std::ostringstream strOut;
    unsigned int red,green,blue;

    strIn.str(hexColour);
    strIn >> std::hex;

    strIn >> red;
    strIn >> green;
    strIn >> blue;

    strOut << red << "," << green << "," << blue;

    property->LinkEndChild(new TiXmlText(strOut.str()));
  }
}

void XrcToXfbFilter::ImportStringListProperty(const wxString &xrcPropName,
  TiXmlElement *property , bool parseXrcText)
{
  TiXmlElement *xrcProperty = m_xrcObj->FirstChildElement(xrcPropName.mb_str( wxConvUTF8 ));
  if (!xrcProperty)
    return;

  TiXmlElement *element = NULL;
  TiXmlNode *xmlValue = NULL;
  wxString res;

  element = xrcProperty->FirstChildElement("item");
  while (element)
  {
    xmlValue = element->FirstChild();
    if (xmlValue && xmlValue->ToText())
    {
      wxString value = wxString(xmlValue->ToText()->Value(), wxConvUTF8);
      if (parseXrcText)
        value = XrcTextToString(value);

      res += wxChar('\'') + value + wxT("' ");
    }

    element = element->NextSiblingElement("item");
  }

  res.Trim();
  property->LinkEndChild(new TiXmlText(res.mb_str( wxConvUTF8 )));
}

void XrcToXfbFilter::AddWindowProperties()
{
  // falta exstyle
  AddProperty(_("pos"), _("pos"), XRC_TYPE_POINT);
  AddProperty(_("size"), _("size"), XRC_TYPE_SIZE);
  AddProperty(_("bg"), _("bg"), XRC_TYPE_COLOUR);
  AddProperty(_("fg"), _("fg"), XRC_TYPE_COLOUR);
  AddProperty(_("font"), _("font"), XRC_TYPE_FONT);
  //AddProperty(_("style"), _("style"), XRC_TYPE_BITLIST);
  AddStyleProperty();
};
