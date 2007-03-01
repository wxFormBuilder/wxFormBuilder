
#include "rad/xrcpreview/xrcpreview.h"
#include "model/objectbase.h"
#include "codegen/xrccg.h"
#include "rad/cpppanel/cpppanel.h"

#include <wx/xrc/xmlres.h>
#include <wx/filename.h>

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

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE( XRCPreviewEvtHandler, wxEvtHandler )
	EVT_KEY_UP( XRCPreviewEvtHandler::OnKeyUp )
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
		window->PushEventHandler( new XRCPreviewEvtHandler( window ) );
	}

	::wxSetWorkingDirectory( workingDir );

	res->Unload( filePath );
	::wxRemoveFile( filePath );
}
