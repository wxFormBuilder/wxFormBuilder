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
//   Ryan Mulder - rjmyst3@gmail.com
//
///////////////////////////////////////////////////////////////////////////////
#include "rad/xrcpreview/xrcpreview.h"
#include "model/objectbase.h"
#include "codegen/xrccg.h"
#include "codegen/codewriter.h"
#include "utils/annoyingdialog.h"
#include "utils/wxfbexception.h"
#include "utils/typeconv.h"

#include <wx/fs_mem.h>
#include <wx/xrc/xmlres.h>

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
			m_window->Close();
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
			m_window->Close();
		}
	}

	void OnRightDown( wxMouseEvent& event )
	{
		wxMenu* menu = new XrcPreviewPopupMenu( m_window );
		wxPoint pos = event.GetPosition();
		m_window->PopupMenu( menu, pos.x, pos.y );
	}

    void RemoveEventHandler( wxWindow* window )
    {
        const wxWindowList& children = window->GetChildren();
        for ( size_t i = 0; i < children.GetCount(); ++i )
        {
            RemoveEventHandler( children.Item( i )->GetData() );
        }
        wxEvtHandler* handler = window->PopEventHandler();
        if ( handler != this )
        {
            delete handler;
        }
    }

	void OnClose( wxCloseEvent& )
	{
	    RemoveEventHandler( m_window );
		m_window->Destroy();
		delete this;
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

	AnnoyingDialog dlg(_("WARNING - For XRC Developers ONLY!!"),
						wxGetTranslation(	wxT("The XRC language is not as powerful as C++.\n")
											wxT("It has limitations that will affect the GUI\n")
											wxT("layout. This preview will ONLY show how the\n")
											wxT("generated XRC will look, and it will probably\n")
											wxT("be different from the Designer.\n\n")
											wxT("If you are not using XRC, do NOT use the XRC\n")
											wxT("preview, it will only confuse you.")
										),
								wxART_WARNING,
								AnnoyingDialog::OK_CANCEL,
								wxID_CANCEL);

	if ( wxID_CANCEL == dlg.ShowModal() )
	{
		return;
	}

	wxString className = form->GetClassName();

	PStringCodeWriter cw( new StringCodeWriter );
	try
	{
		XrcCodeGenerator codegen;
		codegen.SetWriter( cw );
		codegen.GenerateCode( form );
	}
	catch ( wxFBException& ex )
	{
		wxLogError( ex.what() );
		return;
	}

	wxString workingDir = ::wxGetCwd();
	// We change the current directory so that the relative paths work properly
	::wxSetWorkingDirectory( projectPath );
	wxXmlResource *res = wxXmlResource::Get();
	res->InitAllHandlers();

	const std::string& data = _STDSTR( cw->GetString() );
	wxMemoryFSHandler::AddFile(wxT("xrcpreview.xrc"), data.c_str(), data.size() );
	res->Load( wxT("memory:xrcpreview.xrc") );

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
	res->Unload( wxT("memory:xrcpreview.xrc") );
	#endif

	wxMemoryFSHandler::RemoveFile( wxT("xrcpreview.xrc") );
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
