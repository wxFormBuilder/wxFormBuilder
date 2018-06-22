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

#include "xrccg.h"
#include "codewriter.h"
#include "utils/typeconv.h"
#include "utils/debug.h"
#include "model/objectbase.h"
#include "model/xrcfilter.h"

#include <ticpp.h>

void XrcCodeGenerator::SetWriter( PCodeWriter cw )
{
	m_cw = cw;
}

bool XrcCodeGenerator::GenerateCode( PObjectBase project )
{
	m_cw->Clear();
	m_contextMenus.clear();

	ticpp::Document doc;
	ticpp::Declaration decl( "1.0", "UTF-8", "yes" );
	doc.LinkEndChild( &decl );

	ticpp::Element element( "resource" );
	element.SetAttribute( "xmlns", "http://www.wxwindows.org/wxxrc" );
	element.SetAttribute( "version", "2.3.0.1" );

	// If project is not actually a "Project", generate it
	if ( project->GetClassName() == wxT("Project") )
	{
		for( unsigned int i = 0; i < project->GetChildCount(); i++ )
		{
			ticpp::Element* child = GetElement( project->GetChild( i ) );
			if ( child )
			{
				element.LinkEndChild( child );
				delete child;
			}
		}
	}
	else
	{
		ticpp::Element* child = GetElement( project );
		if ( child )
		{
			element.LinkEndChild( child );
			delete child;
		}
	}

	// generate context menus as top-level menus
	for( std::vector<ticpp::Element*>::iterator it = m_contextMenus.begin(); it != m_contextMenus.end(); ++it )
	{
		element.LinkEndChild( *it );
		delete *it;
	}

	doc.LinkEndChild( &element );

	TiXmlPrinter printer;
	printer.SetIndent( "\t" );

	printer.SetLineBreak("\n");

	doc.Accept( &printer );
	const std::string& xrcFile = printer.Str();

	m_cw->Write( _WXSTR( xrcFile ) );

	return true;

}


ticpp::Element* XrcCodeGenerator::GetElement( PObjectBase obj, ticpp::Element* parent )
{
	ticpp::Element *element = NULL;

	IComponent *comp = obj->GetObjectInfo()->GetComponent();

	if ( comp )
		element = comp->ExportToXrc( obj.get() );

	if ( element )
	{
		std::string class_name = element->GetAttribute( "class" );
		if ( class_name == "__dummyitem__" )
		{
			delete element;
			element = NULL;

			if ( obj->GetChildCount() > 0 )
				element = GetElement( obj->GetChild( 0 ) );

			return element;
		}
		else if ( class_name == "spacer" )
		{
			// Dirty hack to replace the containing sizeritem with the spacer
			if ( parent )
			{
				parent->SetAttribute( "class", "spacer" );
				for ( ticpp::Node* child = element->FirstChild( false ); child; child = child->NextSibling( false ) )
				{
					parent->LinkEndChild( child->Clone().release() );
				}
				delete element;
				return NULL;
			}

		}
		else if ( class_name == "wxFrame" )
		{
			// Dirty hack to prevent sizer generation directly under a wxFrame
			// If there is a sizer, the size property of the wxFrame is ignored
			// when loading the xrc file at runtime
			if ( obj->GetPropertyAsInteger( _("xrc_skip_sizer") ) )
			{
				for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
				{
					ticpp::Element* aux = NULL;

					PObjectBase child = obj->GetChild( i );
					if ( child->GetObjectInfo()->IsSubclassOf( wxT("sizer") ) )
					{
						if ( child->GetChildCount() == 1 )
						{
							PObjectBase sizeritem = child->GetChild( 0 );
							if ( sizeritem )
							{
								aux = GetElement( sizeritem->GetChild( 0 ), element );
							}
						}
					}

					if ( !aux )
					{
						aux = GetElement( child, element );
					}

					if ( aux )
					{
						element->LinkEndChild( aux );
						delete aux;
					}
				}
				return element;
			}
		}
		else if( class_name == "wxMenu" )
		{
			// Do not generate context menus assigned to forms or widgets
			std::string parent_name = parent->GetAttribute( "class" );
			if( (parent_name != "wxMenuBar") && (parent_name != "wxMenu") )
			{
				// insert context menu into vector for delayed processing (context menus will be generated as top-level menus)
				for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
				{
					ticpp::Element *aux = GetElement( obj->GetChild( i ), element );
					if ( aux )
					{
						element->LinkEndChild( aux );
						delete aux;
					}
				}

				m_contextMenus.push_back( element );
				return NULL;
			}
		}
		else if ( class_name == "wxCollapsiblePane" )
		{
			ticpp::Element *aux = new ticpp::Element( "object" );
			aux->SetAttribute( "class", "panewindow" );

			ticpp::Element *child = GetElement( obj->GetChild( 0 ), aux );

			aux->LinkEndChild( child );
			element->LinkEndChild( aux );

			delete aux;
			delete child;

			return element;
		}

		for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
		{
			ticpp::Element *aux = GetElement( obj->GetChild( i ), element );
			if ( aux )
			{
				element->LinkEndChild( aux );
				delete aux;
			}
		}
	}
	else
	{
		if( obj->GetObjectTypeName() != wxT("nonvisual") )
		{
			// The componenet does not XRC
			element = new ticpp::Element( "object" );
			element->SetAttribute( "class", "unknown" );
			element->SetAttribute( "name", _STDSTR( obj->GetPropertyAsString( _( "name" ) ) ) );
		}
	}

	return element;
}
