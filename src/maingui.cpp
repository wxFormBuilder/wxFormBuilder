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
#include <wx/cmdline.h>
#include "wx/config.h"
#include "utils/wxfbexception.h"

#include <memory>
#include "maingui.h"

#include "utils/wxlogstring.h"
#include "utils/debug.h"

static const wxCmdLineEntryDesc s_cmdLineDesc[] =
{
	{ wxCMD_LINE_SWITCH, wxT("g"), wxT("generate"),	wxT("Generate code from passed file.") },
	{ wxCMD_LINE_SWITCH, wxT("h"), wxT("help"),		wxT("Show this help message."), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_HELP  },
	{ wxCMD_LINE_PARAM, NULL, NULL,	wxT("File to open."), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_NONE }
};

IMPLEMENT_APP( MyApp )

bool MyApp::OnInit()
{
	// Using a space so the initial 'w' will not be capitalized in GUI dialogs
	wxApp::SetAppName( wxT( " wxFormBuilder" ) );

	// Creating the wxConfig manually so there will be no space
	// The old config (if any) is returned, delete it
	delete wxConfigBase::Set( new wxConfig( wxT("wxFormBuilder") ) );

	// Get the path of the executable
	wxString exeFile( argv[0] );
	wxFileName appFileName( exeFile );
	appFileName.Normalize();
	wxString path = appFileName.GetPath();

	// This is not necessary for wxFB to work. However, Windows sets the Current Working Directory
	// to the directory from which a .fbp file was opened, if opened from Windows Explorer.
	// This puts an unneccessary lock on the directory.
	// This changes the CWD to the already locked app directory as a workaround
	#ifdef __WXMSW__
	::wxSetWorkingDirectory( path );
	#endif

	// Parse command line
	wxCmdLineParser parser( s_cmdLineDesc, argc, argv );
	if ( 0 != parser.Parse() )
	{
		return false;
	}

	// Get project to load
	wxString projectToLoad = wxEmptyString;
	if ( parser.GetParamCount() > 0 )
	{
		projectToLoad = parser.GetParam();
	}

	bool justGenerate = false;
	if ( parser.Found( wxT("g") ) )
	{
		delete wxLog::SetActiveTarget( new wxLogStderr );

		if ( projectToLoad.empty() )
		{
			wxLogError( _("You must pass a path to a project file. Nothing to generate.") );
			return false;
		}

		// generate code
		justGenerate = true;
	}
	else
	{
		delete wxLog::SetActiveTarget( new wxLogGui );
	}

	// Create singleton AppData - wait to initialize until sure that this is not the second
	// instance of a project file.
	AppDataCreate( path );

	// Make passed project name absolute
	try
	{
		if ( !projectToLoad.empty() )
		{
			wxFileName path( projectToLoad );
			if ( !path.IsOk() )
			{
				THROW_WXFBEX( wxT("This path is invalid: ") << projectToLoad );
			}

			if ( !path.IsAbsolute() )
			{
				if ( !path.MakeAbsolute() )
				{
					THROW_WXFBEX( wxT("Could not make path absolute: ") << projectToLoad );
				}
			}
			projectToLoad = path.GetFullPath();
		}
	}
	catch ( wxFBException& ex )
	{
		wxLogError( ex.what() );
	}

	// If the project is already loaded in another instance, switch to that instance and quit
	if ( !projectToLoad.empty() && !justGenerate )
	{
		if ( ::wxFileExists( projectToLoad ) )
		{
			if ( !AppData()->VerifySingleInstance( projectToLoad ) )
			{
				return false;
			}
		}
	}

	// Init handlers
	wxInitAllImageHandlers();
	wxXmlResource::Get()->InitAllHandlers();

	// Init AppData
	try
	{
		AppDataInit();
	}
	catch( wxFBException& ex )
	{
		wxLog::FlushActive();
		wxMessageBox( 	wxString::Format( 	_("Error loading application: %s\nwxFormBuilder cannot continue."),
											ex.what()
										),
						_("Error loading application"),
						wxICON_ERROR,
						NULL
					);
		return false;
	}

	wxSystemOptions::SetOption( wxT( "msw.remap" ), 0 );
	wxSystemOptions::SetOption( wxT( "msw.staticbox.optimized-paint" ), 0 );

	MainFrame *frame = NULL;

	#ifndef __WXFB_DEBUG__
	wxBitmap bitmap;
	std::auto_ptr< wxSplashScreen > splash;
	if ( !justGenerate )
	{
		if ( bitmap.LoadFile( path + wxFILE_SEP_PATH + wxT( "resources" ) + wxFILE_SEP_PATH + wxT( "splash.png" ), wxBITMAP_TYPE_PNG ) )
		{
			splash = std::auto_ptr< wxSplashScreen >( new wxSplashScreen( bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
					 3000, NULL, -1, wxDefaultPosition, wxDefaultSize,
					 wxSIMPLE_BORDER | wxSTAY_ON_TOP ) );
		}
	}
	#endif

	wxYield();

	// Read size and position from config file
	wxConfigBase *config = wxConfigBase::Get();
	config->SetPath( wxT("/mainframe") );
	int x, y, w, h;
	x = y = w = h = -1;
	config->Read( wxT( "PosX" ), &x );
	config->Read( wxT( "PosY" ), &y );
	config->Read( wxT( "SizeW" ), &w );
	config->Read( wxT( "SizeH" ), &h );

	long style = config->Read( wxT("style"), wxFB_WIDE_GUI );
	if ( style != wxFB_CLASSIC_GUI )
	{
		style = wxFB_WIDE_GUI;
	}

	config->SetPath( wxT("/") );

	frame = new MainFrame( NULL ,-1, (int)style, wxPoint( x, y ), wxSize( w, h ) );
	if ( !justGenerate )
	{
		frame->Show( TRUE );
		SetTopWindow( frame );

		#ifdef __WXFB_DEBUG__
			wxLogWindow* log = dynamic_cast< wxLogWindow* >( AppData()->GetDebugLogTarget() );
			if ( log )
			{
				frame->AddChild( log->GetFrame() );
			}
		#endif //__WXFB_DEBUG__
	}

	if ( !projectToLoad.empty() )
	{
		if ( AppData()->LoadProject( projectToLoad, !justGenerate ) )
		{
			if ( justGenerate )
			{
				AppData()->GenerateCode( false );
				return false;
			}
			else
			{
				frame->InsertRecentProject( projectToLoad );
				return true;
			}
		}
		else
		{
			wxLogError( wxT("Unable to load project: %s"), projectToLoad.c_str() );
		}
	}

	if ( justGenerate )
	{
		return false;
	}

	AppData()->NewProject();
	return true;
}

MyApp::~MyApp()
{
	AppDataDestroy();
}
