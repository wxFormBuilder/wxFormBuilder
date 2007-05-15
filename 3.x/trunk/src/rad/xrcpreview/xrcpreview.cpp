
#include "rad/xrcpreview/xrcpreview.h"
#include "model/objectbase.h"
#include "codegen/xrccg.h"
#include "rad/cpppanel/cpppanel.h"

#include <wx/xrc/xmlres.h>
#include <wx/filename.h>

#include "wx/wxFlatNotebook/xh_fnb.h"

#define MENU_DELETE 109

class XrcPreviewPopupMenu : public wxMenu
{
	DECLARE_EVENT_TABLE()

private:
	wxWindow* m_window;

public:
	XrcPreviewPopupMenu( wxWindow* window )
	:
	wxMenu(),
	m_window( window )
	{
		Append( MENU_DELETE, wxT("Close Preview") );
	}

	void OnMenuEvent ( wxCommandEvent& event )
	{
		int id = event.GetId();

		switch ( id )
		{
		case MENU_DELETE:
			m_window->Destroy();
			break;
		default:
			break;
		}
	}
};

BEGIN_EVENT_TABLE( XrcPreviewPopupMenu, wxMenu )
	EVT_MENU( wxID_ANY, XrcPreviewPopupMenu::OnMenuEvent )
END_EVENT_TABLE()

class XRCPreviewEvtHandler : public wxEvtHandler
{
private:
	wxWindow* m_window;

public:
	XRCPreviewEvtHandler( wxWindow* win )
	:
	m_window( win )
	{
	}

protected:
	void OnKeyUp( wxKeyEvent& event )
	{
		if ( event.GetKeyCode() == WXK_ESCAPE )
		{
			m_window->Destroy();
		}
	}

	void OnRightDown( wxMouseEvent& event )
	{
		wxMenu* menu = new XrcPreviewPopupMenu( m_window );
		wxPoint pos = event.GetPosition();
		m_window->PopupMenu( menu, pos.x, pos.y );
	}

	void OnClose( wxCloseEvent& )
	{
		m_window->Destroy();
	}

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE( XRCPreviewEvtHandler, wxEvtHandler )
	EVT_KEY_UP( XRCPreviewEvtHandler::OnKeyUp )
	EVT_RIGHT_DOWN( XRCPreviewEvtHandler::OnRightDown )
	EVT_CLOSE ( XRCPreviewEvtHandler::OnClose )
END_EVENT_TABLE()

void XRCPreview::Show( PObjectBase form, const wxString& projectPath )
{
	wxString className = form->GetClassName();

	XrcCodeGenerator codegen;
	wxString filePath = wxFileName::CreateTempFileName( wxT( "wxFB" ) );
	PCodeWriter cw( new FileCodeWriter( filePath ) );

	codegen.SetWriter( cw );
	codegen.GenerateCode( form );

	wxString workingDir = ::wxGetCwd();
	// We change the current directory so that the relative paths work properly
	::wxSetWorkingDirectory( projectPath );
	wxXmlResource *res = wxXmlResource::Get();
	res->InitAllHandlers();
	res->AddHandler( new wxFlatNotebookXmlHandler );
	res->Load( filePath );

	wxWindow* window = NULL;
	if ( className == wxT( "Frame" ) )
	{
		wxFrame* frame = new wxFrame();
		res->LoadFrame( frame, wxTheApp->GetTopWindow(), form->GetPropertyAsString( wxT( "name" ) ) );
		// Prevent events from propagating up to wxFB's frame
		frame->SetExtraStyle( frame->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
		frame->Show();
		window = frame;
	}
	else if ( className == wxT( "Dialog" ) )
	{
		wxDialog* dialog = new wxDialog;
		res->LoadDialog( dialog, wxTheApp->GetTopWindow(), form->GetPropertyAsString( wxT( "name" ) ) );
		// Prevent events from propagating up to wxFB's frame
		dialog->SetExtraStyle( dialog->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
		dialog->Show();
		window = dialog;
	}
	else if ( className == wxT( "Panel" ) )
	{
		wxDialog* dialog = new wxDialog( wxTheApp->GetTopWindow(), -1, wxT( "Dialog" ), wxDefaultPosition,
		                 wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );
		// Prevent events from propagating up to wxFB's frame
		dialog->SetExtraStyle( wxWS_EX_BLOCK_EVENTS );
		wxPanel *panel = new wxPanel();
		res->LoadPanel( panel, dialog, form->GetPropertyAsString( wxT( "name" ) ) );
		dialog->SetClientSize( panel->GetSize() );
		dialog->SetSize( form->GetPropertyAsSize( wxT( "size" ) ) );
		dialog->CenterOnScreen();
		dialog->Show();
		window = dialog;
	}

	if ( window )
	{
		AddEventHandler( window, window );
	}

	::wxSetWorkingDirectory( workingDir );

	#if wxCHECK_VERSION( 2, 6, 3 )
	res->Unload( filePath );
	#endif

	::wxRemoveFile( filePath );
}

void XRCPreview::AddEventHandler( wxWindow* window, wxWindow* form )
{
	const wxWindowList& children = window->GetChildren();
	for ( size_t i = 0; i < children.GetCount(); ++i )
	{
		AddEventHandler( children.Item( i )->GetData(), form );
	}
	window->PushEventHandler( ( new XRCPreviewEvtHandler( form ) ) );
}
