#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wxfbipc.h"
#include <wx/filename.h>


bool wxFBIPC::VerifySingleInstance( const wxString& file, bool switchTo )
{
	// Possible send a message to the running instance through this string later, for now it is left empty
	wxString expression = wxEmptyString;

	// Make path absolute
	wxFileName path( file );
	if ( !path.IsOk() )
	{
		wxLogError( wxT("This path is invalid: %s"), file.c_str() );
		return false;
	}

	if ( !path.IsAbsolute() )
	{
		if ( !path.MakeAbsolute() )
		{
			wxLogError( wxT("Could not make path absolute: %s"), file.c_str() );
			return false;
		}
	}

	// Check for single instance

	// Create lockfile/mutex name
	wxString name = wxString::Format( wxT("wxFormBuilder-%s-%s"), wxGetUserId().c_str(), path.GetFullPath().c_str() );

	// Get forbidden characters
	wxString forbidden = wxFileName::GetForbiddenChars();

	// Repace forbidded characters
	for ( size_t c = 0; c < forbidden.Length(); ++c )
	{
		wxString bad( forbidden.GetChar( c ) );
		name.Replace( bad.c_str(), wxT("_") );
	}

	// Paths are not case sensitive in windows
	#ifdef __WXMSW__
	name = name.MakeLower();
	#endif

	// GetForbiddenChars is missing "/" in unix. Prepend '.' to make lockfiles hidden
	#ifndef __WXMSW__
	name.Replace( wxT("/"), wxT("_") );
	name.Prepend( wxT(".") );
	#endif

	// Check to see if I already have a server with this name - if so, no need to make another!
	if ( m_server.get() )
	{
		if ( m_server->m_name == name )
		{
			return true;
		}
	}

    std::auto_ptr< wxSingleInstanceChecker > checker;
    {
        // Suspend logging, because error messages here are not useful
        #if !( defined( _DEBUG ) || defined( DEBUG ) || defined ( __WXDEBUG__ ) )
        wxLogNull stopLogging;
        #endif
        checker.reset( new wxSingleInstanceChecker( name ) );
    }

    if ( !checker->IsAnotherRunning() )
	{
		// This is the first instance of this project, so setup a server and save the single instance checker
		if ( CreateServer( name ) )
		{
			m_checker = checker;
			return true;
		}
		else
		{
			return false;
		}
	}
	else if ( switchTo )
    {
		// Suspend logging, because error messages here are not useful
		#if !( defined( _DEBUG ) || defined( DEBUG ) || defined ( __WXDEBUG__ ) )
		wxLogNull stopLogging;
		#endif

		// There is another app, so connect and send the expression

		// Cannot have a client and a server at the same time, due to the implementation of wxTCPServer and wxTCPClient,
		// so temporarily drop the server if there is one
		bool hadServer = false;
		wxString oldName;
		if ( m_server.get() != NULL )
		{
			oldName = m_server->m_name;
			m_server.reset();
			hadServer = true;
		}

		// Create the client
		std::auto_ptr< AppClient > client( new AppClient );

		// Create the connection
		std::auto_ptr< wxConnectionBase > connection;
		#ifdef __WXMSW__
			connection.reset( client->MakeConnection( wxT("localhost"), name, name ) );
		#else
			bool connected = false;
			for ( int i = m_port; i < m_port + 20; ++i )
			{
				wxString nameWithPort = wxString::Format( wxT("%i%s"), i, name.c_str() );
				connection.reset( client->MakeConnection( wxT("127.0.0.1"), nameWithPort, name ) );
				if ( NULL != connection.get() )
				{
					connected = true;
					wxChar* pid = connection->Request( wxT("PID"), NULL );
					if ( NULL != pid )
					{
						wxLogStatus( wxT("%s already open in process %s"), file.c_str(), pid );
					}
					break;
				}
			}
			if ( !connected )
			{
				wxLogError( wxT("There is a lockfile named '%s', but unable to make a connection to that instance."), name.c_str() );
			}
		#endif

		// Drop the connection and client
		connection.reset();
		client.reset();

		// Create the server again, if necessary
		if ( hadServer )
		{
			CreateServer( oldName );
		}
    }
    return false;
}

bool wxFBIPC::CreateServer( const wxString& name )
{
	// Suspend logging, because error messages here are not useful
	#if !( defined( _DEBUG ) || defined( DEBUG ) || defined ( __WXDEBUG__ ) )
	wxLogNull stopLogging;
	#endif

	std::auto_ptr< AppServer > server( new AppServer( name ) );

	#ifdef __WXMSW__
		if ( server->Create( name ) )
		{
			m_server = server;
			return true;
		}
	#else
	{
		for ( int i = m_port; i < m_port + 20; ++i )
		{
			wxString nameWithPort = wxString::Format( wxT("%i%s"), i, name.c_str() );
			if( server->Create( nameWithPort ) )
			{
				m_server = server;
				return true;
			}
			else
			{
				wxLogDebug( wxT("Server Creation Failed. %s"), nameWithPort.c_str() );
			}
		}
	}
	#endif

	wxLogError( wxT("Failed to create an IPC service with name %s"), name.c_str() );
	return false;
}

void wxFBIPC::Reset()
{
	m_server.reset();
	m_checker.reset();
}

wxConnectionBase* AppServer::OnAcceptConnection( const wxString& topic )
{
	if ( topic == m_name )
	{
		wxFrame* frame = wxDynamicCast( wxTheApp->GetTopWindow(), wxFrame );
		if ( !frame )
		{
			return NULL;
		}
		frame->Enable();

		if ( frame->IsIconized() )
		{
			frame->Iconize( false );
		}

		frame->Raise();

		return new AppConnection;
	}

	return NULL;
}

wxConnectionBase* AppClient::OnMakeConnection()
{
	return new AppConnection;
}

wxChar* AppConnection::OnRequest( const wxString& topic, const wxString& item, int* size, wxIPCFormat format )
{
	unsigned long pid = ::wxGetProcessId();
	if ( 0 == pid )
	{
		if ( NULL != size )
		{
			*size = 0;
		}
		return NULL;
	}
	else
	{
		int length = m_data.Printf( wxT("%lu"), pid );
		if ( NULL != size )
		{
			*size = (length + 1) * sizeof(wxChar);
		}
		return const_cast< wxChar* >( m_data.c_str() );
	}
}
