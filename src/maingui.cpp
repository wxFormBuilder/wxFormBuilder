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
#include "maingui.h"

#include "model/objectbase.h"
#include "rad/appdata.h"
#include "rad/mainframe.h"
#include "utils/typeconv.h"
#include "utils/wxfbexception.h"

#include <wx/clipbrd.h>
#include <wx/cmdline.h>
#include <wx/config.h>
#include <wx/stdpaths.h>
#include <wx/sysopt.h>

#if wxVERSION_NUMBER >= 2905 && wxVERSION_NUMBER <= 3100
#include <wx/xrc/xh_auinotbk.h>
#elif wxVERSION_NUMBER > 3100
#include <wx/xrc/xh_aui.h>
#endif

// Abnormal Termination Handling
#if wxUSE_ON_FATAL_EXCEPTION && wxUSE_STACKWALKER
	#include <wx/stackwalk.h>
#elif defined(_WIN32) && defined(__MINGW32__)
	#include "dbg_stack_trace/stack.hpp"
	#include <sstream>
	#include <excpt.h>

	#if defined  __MINGW64_VERSION_MAJOR && defined __MINGW64_VERSION_MINOR /* MinGW-w64 required */
		__stdcall EXCEPTION_DISPOSITION StructuredExceptionHandler(	struct _EXCEPTION_RECORD *ExceptionRecord, /* breaks build with MinGW 32 */
															void * EstablisherFrame,
															struct _CONTEXT *ContextRecord,
															void * DispatcherContext );
	#else
		EXCEPTION_DISPOSITION StructuredExceptionHandler(	struct _EXCEPTION_RECORD *ExceptionRecord,
															void * EstablisherFrame,
															struct _CONTEXT *ContextRecord,
															void * DispatcherContext );
	#endif
#endif

void LogStack();

static const wxCmdLineEntryDesc s_cmdLineDesc[] = {
	{ wxCMD_LINE_SWITCH, "g", "generate", "Generate code from passed file.", wxCMD_LINE_VAL_STRING,
	  0 },
	{ wxCMD_LINE_OPTION, "l", "language",
	  "Override the code_generation property from the passed file and generate the passed "
	  "languages. Separate multiple languages with commas.",
	  wxCMD_LINE_VAL_STRING, 0 },
	{ wxCMD_LINE_SWITCH, "h", "help", "Show this help message.", wxCMD_LINE_VAL_STRING,
	  wxCMD_LINE_OPTION_HELP },
	{ wxCMD_LINE_SWITCH, "v", "version", "Print version information.", wxCMD_LINE_VAL_STRING, 0 },
	{ wxCMD_LINE_PARAM, nullptr, nullptr, "File to open.", wxCMD_LINE_VAL_STRING,
	  wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_NONE, nullptr, nullptr, nullptr, wxCMD_LINE_VAL_NONE, 0 }
};

IMPLEMENT_APP( MyApp )

int MyApp::OnRun()
{
	// Abnormal Termination Handling
	#if wxUSE_ON_FATAL_EXCEPTION && wxUSE_STACKWALKER
		::wxHandleFatalExceptions( true );
	#elif defined(_WIN32) && defined(__MINGW32__)
		// Structured Exception handlers are stored in a linked list at FS:[0]
		// THIS MUST BE A LOCAL VARIABLE - windows won't use an object outside of the thread's stack frame
		EXCEPTION_REGISTRATION ex;
		ex.handler = StructuredExceptionHandler;
		asm volatile ("movl %%fs:0, %0" : "=r" (ex.prev));
		asm volatile ("movl %0, %%fs:0" : : "r" (&ex));
	#endif

	// Using a space so the initial 'w' will not be capitalized in wxLogGUI dialogs
	wxApp::SetAppName( wxT( " wxFormBuilder" ) );

	// Creating the wxConfig manually so there will be no space
	// The old config (if any) is returned, delete it
	delete wxConfigBase::Set( new wxConfig( wxT("wxFormBuilder") ) );

	// Get the data directory
	wxStandardPathsBase& stdPaths = wxStandardPaths::Get();
	wxString dataDir = stdPaths.GetDataDir();
	dataDir.Replace( GetAppName().c_str(), wxT("wxformbuilder") );

	// Log to stderr while working on the command line
	delete wxLog::SetActiveTarget( new wxLogStderr );

	// Message output to the same as the log target
	delete wxMessageOutput::Set( new wxMessageOutputLog );

	// Parse command line
	wxCmdLineParser parser( s_cmdLineDesc, argc, argv );
	if ( 0 != parser.Parse() )
	{
		return 1;
	}

	if (parser.Found("v")) {
		std::cout << "wxFormBuilder " << VERSION << std::endl;
		return EXIT_SUCCESS;
	}

	// Get project to load
	wxString projectToLoad = wxEmptyString;
	if ( parser.GetParamCount() > 0 )
	{
		projectToLoad = parser.GetParam();
	}

	bool justGenerate = false;
	wxString language;
	bool hasLanguage = parser.Found( wxT("l"), &language );
	if ( parser.Found( wxT("g") ) )
	{
		if ( projectToLoad.empty() )
		{
			wxLogError( _("You must pass a path to a project file. Nothing to generate.") );
			return 2;
		}

		if ( hasLanguage )
		{
			if ( language.empty() )
			{
				wxLogError( _("Empty language option. Nothing generated.") );
				return 3;
			}
			language.Replace( wxT(","), wxT("|"), true );
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
	AppDataCreate( dataDir );

	// Make passed project name absolute
	try
	{
		if ( !projectToLoad.empty() )
		{
			wxFileName projectPath( projectToLoad );
			if ( !projectPath.IsOk() )
			{
				THROW_WXFBEX( wxT("This path is invalid: ") << projectToLoad );
			}

			if ( !projectPath.IsAbsolute() )
			{
				if ( !projectPath.MakeAbsolute() )
				{
					THROW_WXFBEX( wxT("Could not make path absolute: ") << projectToLoad );
				}
			}
			projectToLoad = projectPath.GetFullPath();
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
				return 4;
			}
		}
	}

	// Init handlers
	wxInitAllImageHandlers();
	wxXmlResource::Get()->InitAllHandlers();
	#if wxVERSION_NUMBER >= 2905 && wxVERSION_NUMBER <= 3100
	wxXmlResource::Get()->AddHandler(new wxAuiNotebookXmlHandler);
	#elif wxVERSION_NUMBER > 3100
	wxXmlResource::Get()->AddHandler(new wxAuiXmlHandler);
	#endif

	// Init AppData
	try
	{
		AppDataInit();
	}
	catch( wxFBException& ex )
	{
		wxLogError( _("Error loading application: %s\nwxFormBuilder cannot continue."),	ex.what() );
		wxLog::FlushActive();
		return 5;
	}

	wxSystemOptions::SetOption( wxT( "msw.remap" ), 0 );
	wxSystemOptions::SetOption( wxT( "msw.staticbox.optimized-paint" ), 0 );

	m_frame = NULL;

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

	m_frame = new MainFrame( NULL ,wxID_ANY, (int)style, wxPoint( x, y ), wxSize( w, h ) );
	if ( !justGenerate )
	{
		m_frame->Show( TRUE );
		SetTopWindow( m_frame );

		#ifdef __WXFB_DEBUG__
			wxLogWindow* log = dynamic_cast< wxLogWindow* >( AppData()->GetDebugLogTarget() );
			if ( log )
			{
				m_frame->AddChild( log->GetFrame() );
			}
		#endif //__WXFB_DEBUG__
	}

	// This is not necessary for wxFB to work. However, Windows sets the Current Working Directory
	// to the directory from which a .fbp file was opened, if opened from Windows Explorer.
	// This puts an unneccessary lock on the directory.
	// This changes the CWD to the already locked app directory as a workaround
	#ifdef __WXMSW__
	::wxSetWorkingDirectory( dataDir );
	#endif

	if ( !projectToLoad.empty() )
	{
		if ( AppData()->LoadProject( projectToLoad, justGenerate ) )
		{
			if ( justGenerate )
			{
				if ( hasLanguage )
				{
					PObjectBase project = AppData()->GetProjectData();
					PProperty codeGen = project->GetProperty( _("code_generation") );
					if ( codeGen )
					{
						codeGen->SetValue( language );
					}
				}
				AppData()->GenerateCode( false, true );
				return 0;
			}
			else
			{
				m_frame->InsertRecentProject( projectToLoad );
				return wxApp::OnRun();
			}
		}
		else
		{
			wxLogError( wxT("Unable to load project: %s"), projectToLoad.c_str() );
		}
	}

	if ( justGenerate )
	{
		return 6;
	}

	AppData()->NewProject();

#ifdef __WXMAC__
    // document to open on startup
    if(!m_mac_file_name.IsEmpty())
    {
        if ( AppData()->LoadProject( m_mac_file_name ) )
            m_frame->InsertRecentProject( m_mac_file_name );
    }
#endif

	return wxApp::OnRun();
}

bool MyApp::OnInit()
{
	// Initialization is done in OnRun, so MinGW SEH works for all code (it needs a local variable, OnInit is called before OnRun)
	return true;
}

int MyApp::OnExit()
{
	MacroDictionary::Destroy();
	AppDataDestroy();

	if( !wxTheClipboard->IsOpened() )
	{
        if ( !wxTheClipboard->Open() )
        {
            return wxApp::OnExit();
        }
	}

    // Allow clipboard data to persist after close
    wxTheClipboard->Flush();
    wxTheClipboard->Close();

	return wxApp::OnExit();
}

#ifdef __WXMAC__
void MyApp::MacOpenFile(const wxString &fileName)
{
    if(m_frame == NULL) m_mac_file_name = fileName;
    else
    {
        if(!m_frame->SaveWarning()) return;

        if ( AppData()->LoadProject( fileName ) )
            m_frame->InsertRecentProject( fileName );
    }
}
#endif

#if wxUSE_ON_FATAL_EXCEPTION && wxUSE_STACKWALKER
	class StackLogger : public wxStackWalker
	{
	protected:
		void OnStackFrame(const wxStackFrame& frame) override {
			// Build param string
			wxString params;
			size_t paramCount = frame.GetParamCount();
			if ( paramCount > 0 )
			{
				params << wxT("( ");

				for ( size_t i = 0; i < paramCount; ++i )
				{
					wxString type, name, value;
					if ( frame.GetParam( i, &type, &name, &value) )
					{
						params << type << wxT(" ") << name << wxT(" = ") << value << wxT(", ");
					}
				}

				params << wxT(")");
			}

			wxString source;
			if ( frame.HasSourceLocation() )
			{
				source.Printf( wxT("%s@%i"), frame.GetFileName().c_str(), frame.GetLine() );
			}

			wxLogError( wxT("%03i %i %s %s %s %s"),
							frame.GetLevel(),
							frame.GetAddress(),
							frame.GetModule().c_str(),
							frame.GetName().c_str(),
							params.c_str(),
							source.c_str() );
		}
	};

	void MyApp::OnFatalException()
	{
		LogStack();
	}
#elif defined(_WIN32) && defined(__MINGW32__)
	static _CONTEXT* context = 0;
	EXCEPTION_DISPOSITION StructuredExceptionHandler(	struct _EXCEPTION_RECORD *ExceptionRecord,
																void * EstablisherFrame,
																struct _CONTEXT *ContextRecord,
																void * DispatcherContext )
	{
		context = ContextRecord;
		LogStack();
		return ExceptionContinueSearch;
	}

	class StackLogger
	{
	public:
		virtual ~StackLogger() = default;

		void WalkFromException()
		{
			try
			{
				std::stringstream output;
				dbg::stack s( 0, context );
				dbg::stack::const_iterator frame;
				for ( frame = s.begin(); frame != s.end(); ++frame )
				{
					output << *frame;
					wxLogError( wxString( output.str().c_str(), *wxConvCurrent ) );
					output.str( "" );
				}
			}
			catch ( std::exception& ex )
			{
				wxLogError( wxString( ex.what(), *wxConvCurrent ) );
			}
		}
	};
#endif

#if (wxUSE_ON_FATAL_EXCEPTION && wxUSE_STACKWALKER) || (defined(_WIN32) && defined(__MINGW32__))
class LoggingStackWalker : public StackLogger
{
public:
	LoggingStackWalker() {
		wxLog::Suspend();
	}

	~LoggingStackWalker() override {
        wxLogError( wxT("A Fatal Error Occurred. Click Details for a backtrace.") );
        wxLog::Resume();
        wxLog* logger = wxLog::GetActiveTarget();
        if ( 0 != logger )
        {
            logger->Flush();
        }
        exit(1);
    }
};

void LogStack()
{
    LoggingStackWalker walker;
    walker.WalkFromException();
}
#endif
