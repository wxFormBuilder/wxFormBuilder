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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/splash.h>
#include "rad/mainframe.h"
#include "rad/appdata.h"
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/sysopt.h>
#include <wx/xrc/xmlres.h>
#include "wx/config.h"

#include <memory>
#include "maingui.h"

IMPLEMENT_APP( MyApp )

bool MyApp::OnInit()
{
	wxApp::SetVendorName( wxT( " wxFormBuilder" ) );
	wxApp::SetAppName( wxT( " wxFormBuilder" ) );

	// Get the path of the executable
	wxString exeFile( argv[0] );
	wxFileName appFileName( exeFile );
	wxString path = appFileName.GetPath();

	// Create singleton AppData - wait to initialize until sure that this is not the second
	// instance of a project file.
	AppDataCreate( path );

	// Get project to load
	// If the project is already loaded in another instance, switch to that instance and quit
	wxString projectToLoad = wxEmptyString;
	if ( argc > 1 )
	{
		wxString arg( argv[1] );
		if ( ::wxFileExists( arg ) )
		{
			if ( !AppData()->VerifySingleInstance( arg ) )
			{
				return false;
			}
			else
			{
				projectToLoad = arg;
			}
		}
	}

	// Init handlers
	wxInitAllImageHandlers();
	wxXmlResource::Get()->InitAllHandlers();

	// Init AppData
	AppDataInit();

	wxSystemOptions::SetOption( wxT( "msw.remap" ), 0 );
	wxSystemOptions::SetOption( wxT( "msw.staticbox.optimized-paint" ), 0 );

#ifndef _DEBUG
	wxBitmap bitmap;
	std::auto_ptr< wxSplashScreen > splash;
	if ( bitmap.LoadFile( path + wxFILE_SEP_PATH + wxT( "resources" ) + wxFILE_SEP_PATH + wxT( "splash.png" ), wxBITMAP_TYPE_PNG ) )
	{
		splash = std::auto_ptr< wxSplashScreen >( new wxSplashScreen( bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
				 3000, NULL, -1, wxDefaultPosition, wxDefaultSize,
				 wxSIMPLE_BORDER | wxSTAY_ON_TOP ) );
	}
#endif

	wxYield();

#ifdef __WXFB_DEBUG__
	m_log = new wxLogWindow( NULL, wxT( "Logging" ) );
	m_old_log = wxLog::SetActiveTarget( m_log );
#endif //__WXFB_DEBUG__

	// Read size and position from config file
	wxConfigBase *config = wxConfigBase::Get();
	config->SetPath( wxT("/mainframe") );
	int x, y, w, h;
	x = y = w = h = -1;
	config->Read( wxT( "PosX" ), &x );
	config->Read( wxT( "PosY" ), &y );
	config->Read( wxT( "SizeW" ), &w );
	config->Read( wxT( "SizeH" ), &h );
	config->SetPath( wxT("/") );


  //TO-DO: Make wxFB gui style selectable
	//MainFrame *frame = new MainFrame( NULL ,-1, wxFB_CLASSIC_GUI);
	MainFrame *frame = new MainFrame( NULL ,-1, wxFB_WIDE_GUI, wxPoint( x, y ), wxSize( w, h ) );
	frame->Show( TRUE );
	SetTopWindow( frame );

#ifdef __WXFB_DEBUG__
	frame->AddChild( m_log->GetFrame() );
#endif //__WXFB_DEBUG__


	// No va bien (en mainframe aparece untitled)
	if ( !projectToLoad.empty() )
	{
		wxFileName path( projectToLoad );
		if ( !path.IsOk() )
		{
			wxLogError( wxT("This path is invalid: %s"), projectToLoad.c_str() );
		}

		if ( !path.IsAbsolute() )
		{
			if ( !path.MakeAbsolute() )
			{
				wxLogError( wxT("Could not make path absolute: %s"), projectToLoad.c_str() );
			}
		}

		if ( AppData()->LoadProject( path.GetFullPath() ) )
		{
			frame->InsertRecentProject( path.GetFullPath() );
			return true;
		}
		else
		{
			wxLogError( wxT( "Unable to load project: %s" ), projectToLoad.c_str() );
		}
	}

	AppData()->NewProject();
	return true;
}

MyApp::~MyApp()
{
	AppDataDestroy();
}
