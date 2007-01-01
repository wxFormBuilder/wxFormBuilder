#ifndef WXFBIPC_H
#define WXFBIPC_H

#include <wx/ipc.h>
#include <memory>
#include <wx/snglinst.h>

/* Only allow one instance of a project to be loaded at a time */

class AppServer;

class wxFBIPC
{
	private:
		std::auto_ptr< wxSingleInstanceChecker > m_checker;
		std::auto_ptr< AppServer > m_server;
		const int m_port;

		bool CreateServer( const wxString& name );

	public:
		wxFBIPC()
		:
		m_port( 4242 )
		{
		}

		bool VerifySingleInstance( const wxString& file, bool switchTo = true );
		void Reset();
};

// Connection class, for use by both communicationg instances
class AppConnection: public wxConnection
{
private:
	wxString m_data;

public:
	AppConnection(){}
	~AppConnection(){}

	wxChar* OnRequest( const wxString& topic, const wxString& item, int* size, wxIPCFormat format );
};

// Server class, for listening to connection requests
class AppServer: public wxServer
{
public:
	const wxString m_name;

	AppServer( const wxString& name ) : m_name( name ){}
	wxConnectionBase* OnAcceptConnection( const wxString& topic );
};

// Client class, to be used by subsequent instances in OnInit
class AppClient: public wxClient
{
public:
	AppClient(){}
	wxConnectionBase* OnMakeConnection();
};

#endif //WXFBIPC_H
