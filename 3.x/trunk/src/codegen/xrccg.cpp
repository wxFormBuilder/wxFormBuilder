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

#include "xrccg.h"
#include "utils/typeconv.h"
#include "utils/debug.h"
#include "tinyxml.h"
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include "model/objectbase.h"
#include "model/xrcfilter.h"
/*
class XrcDocument : public TiXmlDocument {
public:
    XrcDocument() : TiXmlDocument() {};
    void Dump(PCodeWriter cw)
    {
        stringbuf sb;
        ostream os(&sb);
        StreamOut(&os);
        cw->Write(sb.str());
    }
};*/


///////////////////////////////////////////////////////////////////////////////
/*
bool XrcCodeGenerator::GenerateCode(PObjectBase project)
{
  m_cw->Clear();

  // CodeWriter no permite la insercción de texto con formato (\n y \t),
  // por este motivo no funciona esto
  //
  // doc->Dump(m_cw); No inserta ni \t ni \n
  //
  // La solución que se ha dado es volcarlo a un fichero temporal y luego
  // escribirlo línea a línea.

  XrcFilter xrc;
  TiXmlDocument *doc = xrc.GetXrcDocument(project);

  string tmpFileName = _STDSTR(wxFileName::CreateTempFileName(wxT("wxfb")));
  doc->SaveFile(tmpFileName);
  delete doc;

  {
      wxString line;
      wxFileInputStream input(_WXSTR(tmpFileName));
      wxTextInputStream text(input);

      while (!input.Eof())
          m_cw->WriteLn(_STDSTR(text.ReadLine()));
  }
  ::wxRemoveFile(_WXSTR(tmpFileName));

  return true;
}
*/

bool XrcCodeGenerator::GenerateCode( PObjectBase project )
{
	m_cw->Clear();

	TiXmlDocument doc;
	TiXmlDeclaration *decl = new TiXmlDeclaration( "1.0", "UTF-8", "yes" );
	doc.LinkEndChild( decl );

	TiXmlElement *element = new TiXmlElement( "resource" );
	element->SetAttribute( "xmlns", "http://www.wxwindows.org/wxxrc" );
	element->SetAttribute( "version", "2.3.0.1" );

	// wxBitmaps
	wxArrayString bitmaps = project->GetPropertyAsArrayString( _("bitmaps") );
	for ( size_t bitmap = 0; bitmap < bitmaps.size(); ++bitmap )
	{
		TiXmlElement* bmp = new TiXmlElement( "object" );
		bmp->SetAttribute( "class", "wxBitmap" );
		bmp->LinkEndChild( new TiXmlText( _STDSTR( bitmaps[ bitmap ] ) ) );
		element->LinkEndChild( bmp );
	}

	// wxIcons
	wxArrayString icons = project->GetPropertyAsArrayString( _("icons") );
	for ( size_t icon = 0; icon < icons.size(); ++icon )
	{
		TiXmlElement* iconElement = new TiXmlElement( "object" );
		iconElement->SetAttribute( "class", "wxIcon" );
		iconElement->LinkEndChild( new TiXmlText( _STDSTR( icons[ icon ] ) ) );
		element->LinkEndChild( iconElement );
	}


	for ( unsigned int i = 0; i < project->GetChildCount(); i++ )
	{
		TiXmlElement *child = GetElement( project->GetChild( i ) );
		if ( child )
			element->LinkEndChild( child );
	}

	doc.LinkEndChild( element );

	std::string xrcFile = doc.GetAsString();

	m_cw->Write( _WXSTR( xrcFile ) );

	return true;

}


TiXmlElement* XrcCodeGenerator::GetElement( PObjectBase obj )
{
	TiXmlElement *element = NULL;

	IComponent *comp = obj->GetObjectInfo()->GetComponent();

	if ( comp )
		element = comp->ExportToXrc( obj.get() );

	if ( element )
	{
		std::string class_name = element->Attribute( "class" );
		if ( class_name == std::string( "__dummyitem__" ) )
		{
			delete element;
			element = NULL;

			if ( obj->GetChildCount() > 0 )
				element = GetElement( obj->GetChild( 0 ) );

			return element;
		}
		else if ( class_name == std::string( "wxFrame" ) )
		{
			// Dirty hack to prevent sizer generation directly under a wxFrame
			// If there is a sizer, the size property of the wxFrame is ignored
			// when loading the xrc file at runtime
			if ( obj->GetPropertyAsInteger( _("xrc_skip_sizer") ) )
			{
				for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
				{
					TiXmlElement* aux = NULL;

					PObjectBase child = obj->GetChild( i );
					if ( child->GetObjectTypeName() == wxT("sizer") )
					{
						if ( child->GetChildCount() == 1 )
						{
							PObjectBase sizeritem = child->GetChild( 0 );
							if ( sizeritem )
							{
								aux = GetElement( sizeritem->GetChild( 0 ) );
							}
						}
					}

					if ( !aux )
					{
						aux = GetElement( child );
					}

					if ( aux )
					{
						element->LinkEndChild( aux );
					}
				}
				return element;
			}
		}

		for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
		{
			TiXmlElement *aux = GetElement( obj->GetChild( i ) );
			if ( aux ) element->LinkEndChild( aux );
		}
	}
	else
	{
		// El componente no soporta XRC
		element = new TiXmlElement( "object" );
		element->SetAttribute( "class", "unknown" );
		element->SetAttribute( "name", _STDSTR( obj->GetPropertyAsString( _( "name" ) ) ) );
	}

	return element;
}


