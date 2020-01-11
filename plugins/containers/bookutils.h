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
//   Ryan Mulder - rjmyst3@gmail.com
//
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOKUTILS
#define BOOKUTILS

#include <component.h>
#include <default.xpm>
#include <vector>
#ifdef wxUSE_COLLPANE
#include <wx/collpane.h>
#endif
#include <wx/listbook.h>
#include <wx/choicebk.h>
#include <wx/simplebook.h>
#include <wx/aui/auibook.h>

class SuppressEventHandlers
{
private:
	std::vector< wxEvtHandler* > m_handlers;
	wxWindow* m_window;

public:
	SuppressEventHandlers( wxWindow* window )
	:
	m_window( window  )
	{
		while ( window->GetEventHandler() != window )
		{
			m_handlers.push_back( window->PopEventHandler() );
		}
	}

	~SuppressEventHandlers()
	{
		std::vector< wxEvtHandler* >::reverse_iterator handler;
		for ( handler = m_handlers.rbegin(); handler != m_handlers.rend(); ++handler )
		{
			m_window->PushEventHandler( *handler );
		}
	}
};

namespace BookUtils
{
	template < class T >
		void AddImageList( IObject* obj, T* book )
	{
		if ( !obj->GetPropertyAsString( _("bitmapsize") ).empty() )
		{
			wxSize imageSize = obj->GetPropertyAsSize(_("bitmapsize"));
			wxImageList* images = new wxImageList( imageSize.GetWidth(), imageSize.GetHeight() );
			wxImage image = wxBitmap( default_xpm ).ConvertToImage();
			images->Add( image.Scale( imageSize.GetWidth(), imageSize.GetHeight() ) );
			book->AssignImageList( images );
		}
	}

	template < class T >
		void OnCreated( wxObject* wxobject, wxWindow* wxparent, IManager* manager, wxString name )
	{
		// Easy read-only property access
		IObject* obj = manager->GetIObject( wxobject );

		T* book = wxDynamicCast( wxparent, T );

		//This wouldn't compile in MinGW - strange
		///wxWindow* page = wxDynamicCast( manager->GetChild( wxobject, 0 ), wxWindow );

		// Do this instead
		wxObject* child = manager->GetChild( wxobject, 0 );
		wxWindow* page = NULL;
		if ( child->IsKindOf(CLASSINFO(wxWindow)))
		{
			page = (wxWindow*)child;
		}

		// Error checking
		if ( !( obj && book && page ) )
		{
			wxLogError( _("%s is missing its wxFormBuilder object(%p), its parent(%p), or its child(%p)"), name.c_str(), obj, book, page );
			return;
		}

		// Prevent event handling by wxFB - these aren't user generated events
		SuppressEventHandlers suppress( book );

		// Save selection
		int selection = book->GetSelection();
		book->AddPage( page, obj->GetPropertyAsString( _("label") ) );

		// Apply image to page
		IObject* parentObj = manager->GetIObject( wxparent );
		if ( !parentObj )
		{
			wxLogError( _("%s's parent is missing its wxFormBuilder object"), name.c_str() );
			return;
		}

		if ( !parentObj->GetPropertyAsString( _("bitmapsize") ).empty() )
		{
			if ( !obj->GetPropertyAsString( _("bitmap") ).empty() )
			{
				wxSize imageSize = parentObj->GetPropertyAsSize( _("bitmapsize") );
				int width = imageSize.GetWidth();
				int height = imageSize.GetHeight();
				if ( width > 0 && height > 0 )
				{
					wxImageList* imageList = book->GetImageList();
					if ( imageList != NULL )
					{
						wxImage image = obj->GetPropertyAsBitmap( _("bitmap") ).ConvertToImage();
						imageList->Add( image.Scale( width, height ) );
						book->SetPageImage( book->GetPageCount() - 1, imageList->GetImageCount() - 1 );
					}
				}
			}
		}

		if ( obj->GetPropertyAsString( _("select") ) == wxT("0") && selection >= 0 )
		{
			book->SetSelection(selection);
		}
		else
		{
			book->SetSelection( book->GetPageCount() - 1 );
		}
	}

	template < class T >
		void OnSelected( wxObject* wxobject, IManager* manager )
	{
		// Get actual page - first child
		wxObject* page = manager->GetChild( wxobject, 0 );
		if ( NULL == page )
		{
			return;
		}

		T* book = wxDynamicCast( manager->GetParent( wxobject ), T );
		if ( book )
		{
			for ( size_t i = 0; i < book->GetPageCount(); ++i )
			{
				if ( book->GetPage( i ) == page )
				{
					// Prevent infinite event loop
					SuppressEventHandlers suppress( book );

					// Select Page
					book->SetSelection( i );
				}
			}
		}
	}
}

#endif //BOOKUTILS
