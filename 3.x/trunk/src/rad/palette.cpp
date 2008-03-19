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

#include "palette.h"
#include "model/objectbase.h"
#include "bitmaps.h"
#include "utils/debug.h"
#include "utils/wxfbdefs.h"
#include "utils/typeconv.h"
#include "rad/title.h"
#include "wx/config.h"
#include <rad/appdata.h>

#ifdef __WXMAC__
	#include <wx/tooltip.h>
#endif

#define ID_PALETTE_BUTTON 999

#ifdef __WXGTK__
	#if wxCHECK_VERSION( 2, 8, 0 )
		#define DRAG_OPTION 0
	#else
		#define DRAG_OPTION wxFNB_NODRAG
	#endif
#else
	#define DRAG_OPTION 0
#endif

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

void wxFbPalette::PopulateToolbar( PObjectPackage pkg, wxToolBar *toolbar )
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
			Debug::Print( _( "Missing Component for Class \"%s\" of Package \"%s\"." ), info->GetClassName().c_str(), pkg->GetPackageName().c_str() );
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

	long nbStyle;
	wxConfigBase* config = wxConfigBase::Get();
	config->Read( wxT( "/palette/notebook_style" ), &nbStyle, wxFNB_NO_X_BUTTON | wxFNB_NO_NAV_BUTTONS | DRAG_OPTION | wxFNB_DROPDOWN_TABS_LIST  | wxFNB_VC8 | wxFNB_CUSTOM_DLG );

	m_notebook = new wxFlatNotebook( this, -1, wxDefaultPosition, wxDefaultSize, FNB_STYLE_OVERRIDES( nbStyle ) );
	m_notebook->SetCustomizeOptions( wxFNB_CUSTOM_TAB_LOOK | wxFNB_CUSTOM_ORIENTATION | wxFNB_CUSTOM_LOCAL_DRAG );

	unsigned int pkg_count = AppData()->GetPackageCount();

	Debug::Print( wxT( "[Palette] Pages %d" ), pkg_count );

	// Populate icon vector
	for ( unsigned int j = 0; j < pkg_count;j++ )
	{
		PObjectPackage pkg = AppData()->GetPackage( j );
		m_icons.Add( pkg->GetPackageIcon() );
	}

	// Add icons to notebook
	m_notebook->SetImageList( &m_icons );

	for ( unsigned int i = 0; i < pkg_count;i++ )
	{
		PObjectPackage pkg = AppData()->GetPackage( i );
		wxString pkg_name = pkg->GetPackageName();

		wxPanel *panel = new wxPanel( m_notebook, -1 );
		panel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );
		wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );

		wxPanel *tbPanel = new wxPanel( panel, -1 );
		tbPanel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );
		wxBoxSizer *tbSizer = new wxBoxSizer( wxHORIZONTAL );

		wxPanel *sbPanel = new wxPanel( panel, -1 );
		sbPanel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );
		wxBoxSizer *sbSizer = new wxBoxSizer( wxHORIZONTAL );

		wxToolBar *toolbar = new wxToolBar( tbPanel, -1, wxDefaultPosition, wxDefaultSize, wxTB_NODIVIDER | wxTB_FLAT );
		toolbar->SetToolBitmapSize( wxSize( 22, 22 ) );
		toolbar->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );
		PopulateToolbar( pkg, toolbar );
		m_tv.push_back( toolbar );

		tbSizer->Add( toolbar, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL );
		tbPanel->SetSizer( tbSizer );

		wxSpinButton *sb = new wxSpinButton( sbPanel, -1, wxDefaultPosition, wxDefaultSize, wxSP_HORIZONTAL );
		sb->SetRange( 0, ( int )pkg->GetObjectCount() - 1 );
		sb->SetValue( 0 );
		m_posVector.push_back( 0 );
		sbSizer->Add( sb, 0, wxEXPAND );//wxALL | wxALIGN_TOP, 0);
		sbPanel->SetSizer( sbSizer );

		sizer->Add( tbPanel, 1, wxEXPAND, 0 );
		sizer->Add( sbPanel, 0, wxEXPAND, 0 );
		panel->SetAutoLayout( true );
		panel->SetSizer( sizer );
		sizer->Fit( panel );
		sizer->SetSizeHints( panel );

		m_notebook->AddPage( panel, pkg_name, false, i );

	}
	//Title *title = new Title( this, wxT("Component Palette") );
	//top_sizer->Add(title,0,wxEXPAND,0);
	top_sizer->Add( m_notebook, 1, wxEXPAND, 0 );
	SetAutoLayout( true );
	SetSizer( top_sizer );
	top_sizer->Fit( this );
	top_sizer->SetSizeHints( this );
}

void wxFbPalette::OnSpinUp( wxSpinEvent& )
{
	int page = m_notebook->GetSelection();
	PObjectPackage pkg = AppData()->GetPackage( page );

	if (( int )pkg->GetObjectCount() - m_posVector[page] - 1 <= 0 ) return;

	m_posVector[page]++;
	wxToolBar *toolbar = m_tv[page];
	toolbar->DeleteToolByPos( 0 );
	toolbar->Realize();
}

void wxFbPalette::OnSpinDown( wxSpinEvent& )
{
	int page = m_notebook->GetSelection();
	if ( m_posVector[page] <= 0 ) return;

	m_posVector[page]--;
	wxToolBar *toolbar = m_tv[page];
	PObjectPackage pkg = AppData()->GetPackage( page );
	wxString widget( pkg->GetObjectInfo( m_posVector[page] )->GetClassName() );
	wxBitmap icon = pkg->GetObjectInfo( m_posVector[page] )->GetIconFile();

	#ifdef __WXMAC__
		wxBitmapButton* button = new wxBitmapButton( toolbar, nextId++, icon );
		button->SetToolTip( widget );
		toolbar->InsertControl( 0, button );
	#else
		toolbar->InsertTool( 0, nextId++, icon, wxNullBitmap, false, NULL, widget, widget );
	#endif

	toolbar->Realize();
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
			if ( m_tv[i]->FindById( event.GetId() ) )
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
	wxConfigBase* config = wxConfigBase::Get();
	wxString pages;
	for ( size_t i = 0; i < ( size_t )m_notebook->GetPageCount(); ++i )
	{
		pages << m_notebook->GetPageText( i ) << wxT( "," );
	}
	config->Write( wxT( "/palette/pageOrder" ), pages );
	config->Write( wxT( "/palette/notebook_style" ), m_notebook->GetWindowStyleFlag() );
}
