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

#include "palette.h"
#include "model/objectbase.h"
#include "bitmaps.h"
#include "utils/debug.h"
#include "utils/wxfbdefs.h"
#include "utils/typeconv.h"
#include "rad/title.h"
#include "wx/config.h"
#include "rad/appdata.h"
#include "rad/auitabart.h"

#ifdef __WXMAC__
	#include <wx/tooltip.h>
#endif

#define ID_PALETTE_BUTTON 999

#define DRAG_OPTION 0

wxWindowID wxFbPalette::nextId = wxID_HIGHEST + 1000;

BEGIN_EVENT_TABLE( wxFbPalette, wxPanel )
	#ifdef __WXMAC__
		EVT_BUTTON( -1, wxFbPalette::OnButtonClick )
	#else
		EVT_TOOL(-1, wxFbPalette::OnButtonClick)
	#endif
	EVT_SPIN_UP( -1, wxFbPalette::OnSpinUp )
	EVT_SPIN_DOWN( -1, wxFbPalette::OnSpinDown )
END_EVENT_TABLE()

wxFbPalette::wxFbPalette( wxWindow *parent, int id )
		: wxPanel( parent, id ), m_notebook( NULL )
{
}

void wxFbPalette::PopulateToolbar( PObjectPackage pkg, wxAuiToolBar *toolbar )
{
	unsigned int j = 0;
	while ( j < pkg->GetObjectCount() )
	{
		PObjectInfo info = pkg->GetObjectInfo( j );
		if ( info->IsStartOfGroup() )
		{
			toolbar->AddSeparator();
		}
		if ( NULL == info->GetComponent() )
		{
			LogDebug(_("Missing Component for Class \"" + info->GetClassName() + "\" of Package \"" + pkg->GetPackageName() + "\".") );
		}
		else
		{
			wxString widget( info->GetClassName() );

			wxBitmap icon = info->GetIconFile();

			#ifdef __WXMAC__
				wxBitmapButton* button = new wxBitmapButton( toolbar, nextId++, icon );
				button->SetToolTip( widget );
				toolbar->AddControl( button );
			#else
				toolbar->AddTool(nextId++, widget, icon, widget);
			#endif

			toolbar->Realize();
		}
		j++;
	}
}

void wxFbPalette::Create()
{
	wxBoxSizer *top_sizer = new wxBoxSizer( wxVERTICAL );

#ifdef USE_FLATNOTEBOOK
	long nbStyle;
	wxConfigBase* config = wxConfigBase::Get();
	config->Read( wxT( "/palette/notebook_style" ), &nbStyle, wxFNB_NO_X_BUTTON | wxFNB_NO_NAV_BUTTONS | DRAG_OPTION | wxFNB_DROPDOWN_TABS_LIST  | wxFNB_VC8 | wxFNB_CUSTOM_DLG );

	m_notebook = new wxFlatNotebook( this, -1, wxDefaultPosition, wxDefaultSize, FNB_STYLE_OVERRIDES( nbStyle ) );
	m_notebook->SetCustomizeOptions( wxFNB_CUSTOM_TAB_LOOK | wxFNB_CUSTOM_ORIENTATION | wxFNB_CUSTOM_LOCAL_DRAG );
#else
	m_notebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_SCROLL_BUTTONS );
	m_notebook->SetArtProvider( new AuiTabArt() );
#endif

	unsigned int pkg_count = AppData()->GetPackageCount();

	LogDebug( wxT( "[Palette] Pages %d" ), pkg_count );

#ifdef USE_FLATNOTEBOOK
	// Populate icon vector
	for ( unsigned int j = 0; j < pkg_count;j++ )
	{
		PObjectPackage pkg = AppData()->GetPackage( j );
		m_icons.Add( pkg->GetPackageIcon() );
	}

	// Add icons to notebook
	m_notebook->SetImageList( &m_icons );
#else
	wxSize minsize;
#endif

	for ( unsigned int i = 0; i < pkg_count;i++ )
	{
		PObjectPackage pkg = AppData()->GetPackage( i );
		wxString pkg_name = pkg->GetPackageName();

		wxPanel *panel = new wxPanel( m_notebook, -1 );
		//panel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );
		wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );

		wxAuiToolBar *toolbar = new wxAuiToolBar( panel, -1, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxNO_BORDER );
		toolbar->SetToolBitmapSize( wxSize( 22, 22 ) );
		PopulateToolbar( pkg, toolbar );
		m_tv.push_back( toolbar );

		sizer->Add( toolbar, 1, wxEXPAND, 0 );

		panel->SetAutoLayout( true );
		panel->SetSizer( sizer );
		sizer->Fit( panel );
		sizer->SetSizeHints( panel );

#ifndef USE_FLATNOTEBOOK
		wxSize cursize = panel->GetSize();
		if( cursize.x > minsize.x ) minsize.x = cursize.x;
		if( cursize.y > minsize.y ) minsize.y = cursize.y + 30;
#endif

		m_notebook->AddPage( panel, pkg_name, false, i );
#ifndef USE_FLATNOTEBOOK
		m_notebook->SetPageBitmap( i, pkg->GetPackageIcon() );
#endif

	}
	//Title *title = new Title( this, wxT("Component Palette") );
	//top_sizer->Add(title,0,wxEXPAND,0);
#ifdef USE_FLATNOTEBOOK
	top_sizer->Add( m_notebook, 1, wxEXPAND, 0 );
	SetAutoLayout( true );
	SetSizer( top_sizer );
	top_sizer->Fit( this );
	top_sizer->SetSizeHints( this );
#else
	top_sizer->Add( m_notebook, 1, wxEXPAND, 0 );
	SetSizer( top_sizer );
	SetSize( minsize );
	SetMinSize( minsize );
	Layout();
	Fit();
#endif
}

void wxFbPalette::OnSpinUp( wxSpinEvent& )
{
}

void wxFbPalette::OnSpinDown( wxSpinEvent& )
{
}

void wxFbPalette::OnButtonClick( wxCommandEvent &event )
{
	#ifdef __WXMAC__
		wxWindow* win = dynamic_cast< wxWindow* >( event.GetEventObject() );
		if ( win != 0 )
		{
			AppData()->CreateObject( win->GetToolTip()->GetTip() );
		}
	#else
		for ( unsigned int i = 0; i < m_tv.size(); i++ )
		{
			if ( m_tv[i]->GetToolIndex( event.GetId() ) != wxNOT_FOUND )
			{
				wxString name = m_tv[i]->GetToolShortHelp( event.GetId() );
				AppData()->CreateObject( name );
				return;
			}
		}
	#endif
}

wxFbPalette::~wxFbPalette()
{
#ifdef USE_FLATNOTEBOOK
	wxConfigBase* config = wxConfigBase::Get();
	wxString pages;
	for ( size_t i = 0; i < ( size_t )m_notebook->GetPageCount(); ++i )
	{
		pages << m_notebook->GetPageText( i ) << wxT( "," );
	}
	config->Write( wxT( "/palette/pageOrder" ), pages );
	config->Write( wxT( "/palette/notebook_style" ), m_notebook->GetWindowStyleFlag() );
#endif
}
