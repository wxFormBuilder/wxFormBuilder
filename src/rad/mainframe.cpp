///////////////////////////////////////////////////////////////////////////////
//
// wxFormBuilder - A Visual Dialog Editor for wxWidgets.
// Copyright (C) 2005 José Antonio Hurtado
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
#include "mainframe.h"

#include "../model/xrcfilter.h"
#include "../utils/stringutils.h"
#include "../utils/wxfbexception.h"
#include "revision.h"
#include "version.h"
#include "about.h"
#include "appdata.h"
#include "auitabart.h"
#include "bitmaps.h"
#include "cpppanel/cpppanel.h"
#include "designer/visualeditor.h"
#include "geninheritclass/geninhertclass.h"
#include "inspector/objinspect.h"
#include "luapanel/luapanel.h"
#include "objecttree/objecttree.h"
#include "palette.h"
#include "phppanel/phppanel.h"
#include "pythonpanel/pythonpanel.h"
#include "erlangpanel/erlangpanel.h"
#include "title.h"
#include "wx/config.h"
#include "wxfbevent.h"
#include "wxfbmanager.h"
#include "xrcpanel/xrcpanel.h"
#include "dialogfindcomponent.h"

enum
{
	ID_SAVE_PRJ = wxID_HIGHEST + 1,
	ID_OPEN_PRJ,
	ID_NEW_PRJ,
	ID_GENERATE_CODE,
	ID_IMPORT_XRC,
	ID_UNDO,
	ID_REDO,
	ID_SAVE_AS_PRJ,
	ID_CUT,
	ID_DELETE,
	ID_COPY,
	ID_PASTE,
	ID_EXPAND,
	ID_STRETCH,
	ID_MOVE_UP,
	ID_MOVE_DOWN,
	ID_RECENT_0, // Tienen que tener ids consecutivos
	ID_RECENT_1, // ID_RECENT_n+1 == ID_RECENT_n + 1
	ID_RECENT_2, //
	ID_RECENT_3, //
	ID_RECENT_SEP,

	ID_ALIGN_LEFT,
	ID_ALIGN_CENTER_H,
	ID_ALIGN_RIGHT,
	ID_ALIGN_TOP,
	ID_ALIGN_CENTER_V,
	ID_ALIGN_BOTTOM,

	ID_BORDER_LEFT,
	ID_BORDER_RIGHT,
	ID_BORDER_TOP,
	ID_BORDER_BOTTOM,
	ID_EDITOR_FNB,
	ID_MOVE_LEFT,
	ID_MOVE_RIGHT,

	ID_PREVIEW_XRC,
	ID_GEN_INHERIT_CLS,

	// The preference dialog must use wxID_PREFERENCES for wxMAC
	//ID_SETTINGS_GLOBAL, // For the future preference dialogs
	ID_SETTINGS_PROJ, // For the future preference dialogs

	ID_FIND,

	ID_CLIPBOARD_COPY,
	ID_CLIPBOARD_PASTE,

	//added by tyysoft to define the swap button ID.
	ID_WINDOW_SWAP,

    //added by michallukowski to find component from menu.
    ID_FIND_COMPONENT,
};

#define STATUS_FIELD_OBJECT 2
#define STATUS_FIELD_PATH 1

BEGIN_EVENT_TABLE( MainFrame, wxFrame )
EVT_MENU( ID_NEW_PRJ, MainFrame::OnNewProject )
EVT_MENU( ID_SAVE_PRJ, MainFrame::OnSaveProject )
EVT_MENU( ID_SAVE_AS_PRJ, MainFrame::OnSaveAsProject )
EVT_MENU( ID_OPEN_PRJ, MainFrame::OnOpenProject )
EVT_MENU( wxID_ABOUT, MainFrame::OnAbout )
EVT_MENU( wxID_EXIT, MainFrame::OnExit )
EVT_MENU( ID_IMPORT_XRC, MainFrame::OnImportXrc )
EVT_MENU( ID_GENERATE_CODE, MainFrame::OnGenerateCode )
EVT_MENU( ID_UNDO, MainFrame::OnUndo )
EVT_MENU( ID_REDO, MainFrame::OnRedo )
EVT_MENU( ID_DELETE, MainFrame::OnDelete )
EVT_MENU( ID_CUT, MainFrame::OnCut )
EVT_MENU( ID_COPY, MainFrame::OnCopy )
EVT_MENU( ID_PASTE, MainFrame::OnPaste )
EVT_MENU( ID_EXPAND, MainFrame::OnToggleExpand )
EVT_MENU( ID_STRETCH, MainFrame::OnToggleStretch )
EVT_MENU( ID_MOVE_UP, MainFrame::OnMoveUp )
EVT_MENU( ID_MOVE_DOWN, MainFrame::OnMoveDown )
EVT_MENU( ID_MOVE_LEFT, MainFrame::OnMoveLeft )
EVT_MENU( ID_MOVE_RIGHT, MainFrame::OnMoveRight )
EVT_MENU( ID_RECENT_0, MainFrame::OnOpenRecent )
EVT_MENU( ID_RECENT_1, MainFrame::OnOpenRecent )
EVT_MENU( ID_RECENT_2, MainFrame::OnOpenRecent )
EVT_MENU( ID_RECENT_3, MainFrame::OnOpenRecent )
EVT_MENU( ID_ALIGN_RIGHT, MainFrame::OnChangeAlignment )
EVT_MENU( ID_ALIGN_LEFT, MainFrame::OnChangeAlignment )
EVT_MENU( ID_ALIGN_CENTER_H, MainFrame::OnChangeAlignment )
EVT_MENU( ID_ALIGN_TOP, MainFrame::OnChangeAlignment )
EVT_MENU( ID_ALIGN_BOTTOM, MainFrame::OnChangeAlignment )
EVT_MENU( ID_ALIGN_CENTER_V, MainFrame::OnChangeAlignment )
EVT_MENU_RANGE( ID_BORDER_LEFT, ID_BORDER_BOTTOM, MainFrame::OnChangeBorder )
EVT_MENU( ID_PREVIEW_XRC, MainFrame::OnXrcPreview )
EVT_MENU( ID_GEN_INHERIT_CLS, MainFrame::OnGenInhertedClass )
EVT_MENU( ID_CLIPBOARD_COPY, MainFrame::OnClipboardCopy )
EVT_MENU( ID_CLIPBOARD_PASTE, MainFrame::OnClipboardPaste )
EVT_MENU( ID_WINDOW_SWAP, MainFrame::OnWindowSwap )
EVT_MENU( ID_FIND_COMPONENT, MainFrame::OnFindComponent )

EVT_UPDATE_UI( ID_CLIPBOARD_PASTE, MainFrame::OnClipboardPasteUpdateUI )
EVT_CLOSE( MainFrame::OnClose )

EVT_FB_CODE_GENERATION( MainFrame::OnCodeGeneration )
EVT_FB_OBJECT_CREATED( MainFrame::OnObjectCreated )
EVT_FB_OBJECT_REMOVED( MainFrame::OnObjectRemoved )
EVT_FB_OBJECT_EXPANDED( MainFrame::OnObjectExpanded )
EVT_FB_OBJECT_SELECTED( MainFrame::OnObjectSelected )
EVT_FB_PROJECT_LOADED( MainFrame::OnProjectLoaded )
EVT_FB_PROJECT_REFRESH( MainFrame::OnProjectRefresh )
EVT_FB_PROJECT_SAVED( MainFrame::OnProjectSaved )
EVT_FB_PROPERTY_MODIFIED( MainFrame::OnPropertyModified )
EVT_FB_EVENT_HANDLER_MODIFIED( MainFrame::OnEventHandlerModified )

EVT_MENU( ID_FIND, MainFrame::OnFindDialog )
EVT_FIND( wxID_ANY, MainFrame::OnFind )
EVT_FIND_NEXT( wxID_ANY, MainFrame::OnFind )
EVT_FIND_CLOSE( wxID_ANY, MainFrame::OnFindClose )

END_EVENT_TABLE()

// Used to kill focus from propgrid when toolbar or menu items are clicked
// This forces the propgrid to save the cell being edited
class FocusKillerEvtHandler : public wxEvtHandler
{
public:
	void OnMenuEvent( wxCommandEvent& event )
	{
		// Get window with focus
		wxWindow* windowWithFocus = wxWindow::FindFocus();

		// Only send the event if the focus is on the propgrid
		while ( windowWithFocus != NULL )
		{
			wxPropertyGrid* propgrid = wxDynamicCast( windowWithFocus, wxPropertyGrid );
			if ( propgrid != NULL )
			{
				wxFocusEvent focusEvent( wxEVT_KILL_FOCUS );
				propgrid->GetEventHandler()->ProcessEvent( focusEvent );
				break;
			}
			windowWithFocus = windowWithFocus->GetParent();
		}

		// Add the event to the mainframe's original handler
		// Add as pending so propgrid saves the property before the event is processed
		GetNextHandler()->AddPendingEvent( event );
	}

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE( FocusKillerEvtHandler, wxEvtHandler )
	EVT_MENU( wxID_ANY, FocusKillerEvtHandler::OnMenuEvent )
END_EVENT_TABLE()

MainFrame::MainFrame( wxWindow *parent, int id, int style, wxPoint pos, wxSize size )
:
wxFrame( parent, id, wxEmptyString, pos, size, wxDEFAULT_FRAME_STYLE ),
m_leftSplitterWidth( 300 ),
m_rightSplitterWidth( -300 ),
m_style( style ),
m_page_selection( 0 ),
m_rightSplitter_sash_pos( 300 ),
m_autoSash( false ), // autosash function is temporarily disabled due to possible bug(?) in wxMSW event system (workaround is needed)
m_findData( wxFR_DOWN ),
m_findDialog( NULL )
{

	// initialize the splitters, wxAUI doesn't use them
	m_leftSplitter = m_rightSplitter = NULL;

	/////////////////////////////////////////////////////////////////////////////
	// Setup frame icons, title bar, status bar, menubar and toolbar
	/////////////////////////////////////////////////////////////////////////////
	wxIconBundle bundle;
	wxIcon ico16;
	ico16.CopyFromBitmap( AppBitmaps::GetBitmap( wxT( "app16" ), 16 ) );
	bundle.AddIcon( ico16 );

	wxIcon ico32;
	ico32.CopyFromBitmap( AppBitmaps::GetBitmap( wxT( "app32" ), 32 ) );
	bundle.AddIcon( ico32 );

	SetIcons( bundle );

	SetTitle( wxT( "wxFormBuilder" ) );

	//SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

	SetMenuBar( CreateFBMenuBar() );
	CreateStatusBar( 3 );
	SetStatusBarPane( 0 );
	int widths[3] = { -1, -1, 300 };
	SetStatusWidths( sizeof( widths ) / sizeof( int ), widths );
	CreateFBToolBar();

	/////////////////////////////////////////////////////////////////////////////
	// Create the gui
	/////////////////////////////////////////////////////////////////////////////

	/*
	//  --- wxAUI version --
	wxWindow *objectTree      = CreateObjectTree(this);
	wxWindow *objectInspector = CreateObjectInspector(this);
	wxWindow *palette         = CreateComponentPalette(this);
	wxWindow *designer        = CreateDesignerWindow(this);


	m_mgr.SetFrame(this);
	m_mgr.AddPane(objectTree,
	             wxPaneInfo().Name(wxT("tree")).
	                          Caption(wxT("Object Tree")).
	                           Left().Layer(1).
	                           BestSize(wxSize(300,400)).
	                           CloseButton(false));
	m_mgr.AddPane(objectInspector,
	             wxPaneInfo().Name(wxT("inspector")).
	                          Caption(wxT("Object Properties")).
	                           Right().BestSize(wxSize(300,400)).
	                           CloseButton(false));

	m_mgr.AddPane(designer,
	             wxPaneInfo().Name(wxT("editor")).
	                          Caption(wxT("Editor")).
	                           Center().
	                           CloseButton(false));

	m_mgr.AddPane(palette,
	             wxPaneInfo().Name(wxT("palette")).
	             Caption(wxT("Component Palette")).
	             Top().
	             RightDockable(false).
	             LeftDockable(false).
	             CloseButton(false));

	m_mgr.Update();*/

	RestorePosition( wxT( "mainframe" ) );
	Layout();

	// Init. m_cpp and m_xrc first
	m_cpp = NULL;
	m_xrc = NULL;
	m_python = NULL;
	m_lua = NULL;
	m_php = NULL;
	m_erlang = NULL;

	switch ( style )
	{

		case wxFB_DOCKABLE_GUI:
			// TO-DO
			CreateWideGui();

			break;

		case wxFB_CLASSIC_GUI:

			/*  //  --- Classic style Gui --
			     //
			     //  +------++-----------------------+
			     //  | Obj. ||  Palette              |
			     //  | Tree ++-----------------------+
			     //  |      ||  Editor               |
			     //  |______||                       |
			     //  |------||                       |
			     //  | Obj. ||                       |
			     //  | Insp.||                       |
			     //  |      ||                       |
			     //  |      ||                       |
			     //  +------++-----------------------+ 	*/

			CreateClassicGui();

			break;

		case wxFB_DEFAULT_GUI:

		case wxFB_WIDE_GUI:

		default:

			/*  //  --- Wide style Gui --
			      //
			      //  +------++-----------------------+
			      //  | Obj. ||  Palette              |
			      //  | Tree ++-------------++--------+
			      //  |      ||  Editor     || Obj.   |
			      //  |      ||             || Insp.  |
			      //  |      ||             ||        |
			      //  |      ||             ||        |
			      //  |      ||             ||        |
			      //  |      ||             ||        |
			      //  |      ||             ||        |
			      //  +------++-------------++--------+ 	*/

			CreateWideGui();
	}

	AppData()->AddHandler( this->GetEventHandler() );

	wxTheApp->SetTopWindow( this );

	m_focusKillEvtHandler = new FocusKillerEvtHandler;
	PushEventHandler( m_focusKillEvtHandler );

	// So splitter windows can be restored correctly
	Connect( wxEVT_IDLE, wxIdleEventHandler( MainFrame::OnIdle ) );

	m_notebook->Connect( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler( MainFrame::OnAuiNotebookPageChanged ), NULL, this );

}


MainFrame::~MainFrame()
{
	m_notebook->Disconnect( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler( MainFrame::OnAuiNotebookPageChanged ), NULL, this );

#ifdef __WXMAC__
    // work around problem on wxMac
    m_rightSplitter->GetWindow1()->GetSizer()->Detach(m_notebook);
    m_notebook->Destroy();
#endif

	/*m_mgr.UnInit();*/

	// the focus killer event handler
	RemoveEventHandler( m_focusKillEvtHandler );
	delete m_focusKillEvtHandler;

	AppData()->RemoveHandler( this->GetEventHandler() );
	delete m_findDialog;
}

void MainFrame::RestorePosition( const wxString &name )
{
	m_currentDir = wxT( "./projects" );

	wxConfigBase *config = wxConfigBase::Get();
	config->SetPath( name );

	bool maximized;
	config->Read( wxT( "IsMaximized" ), &maximized, true );

	if ( maximized )
	{
		Maximize( maximized );
	}
	else
	{
		bool iconized;
		config->Read( wxT( "IsIconized" ), &iconized, false );

		if ( iconized )
		{
			Iconize( iconized );
		}
	}

	config->Read( wxT( "LeftSplitterWidth" ), &m_leftSplitterWidth, 300 );
	config->Read( wxT( "RightSplitterWidth" ), &m_rightSplitterWidth, -300 );
	config->Read( wxT( "RightSplitterType" ), &m_rightSplitterType, _("editor"));

	config->Read( wxT( "AutoSash" ), &m_autoSash, false ); // disabled in default due to possible bug(?) in wxMSW

	config->Read( wxT( "CurrentDirectory" ), &m_currentDir );

	config->Read( wxT( "RecentFile0" ), &m_recentProjects[0] );
	config->Read( wxT( "RecentFile1" ), &m_recentProjects[1] );
	config->Read( wxT( "RecentFile2" ), &m_recentProjects[2] );
	config->Read( wxT( "RecentFile3" ), &m_recentProjects[3] );

	config->SetPath( wxT( ".." ) );
	UpdateRecentProjects();
}

void MainFrame::SavePosition( const wxString &name )
{
	m_objInsp->SavePosition();
	m_palette->SavePosition();

	wxConfigBase *config = wxConfigBase::Get();
	bool isIconized = IsIconized();
	bool isMaximized = IsMaximized();

	config->SetPath( name );

	if ( !isMaximized )
	{
		config->Write( wxT( "PosX" ), isIconized ? -1 : GetPosition().x );
		config->Write( wxT( "PosY" ), isIconized ? -1 : GetPosition().y );
		config->Write( wxT( "SizeW" ), isIconized ? -1 : GetSize().GetWidth() );
		config->Write( wxT( "SizeH" ), isIconized ? -1 : GetSize().GetHeight() );
	}

	config->Write( wxT( "IsMaximized" ), isMaximized );

	config->Write( wxT( "IsIconized" ), isIconized );
	config->Write( wxT( "CurrentDirectory" ), m_currentDir );

	config->Write( wxT( "RecentFile0" ), m_recentProjects[0] );
	config->Write( wxT( "RecentFile1" ), m_recentProjects[1] );
	config->Write( wxT( "RecentFile2" ), m_recentProjects[2] );
	config->Write( wxT( "RecentFile3" ), m_recentProjects[3] );

	if ( m_leftSplitter )
	{
		int leftSashWidth = m_leftSplitter->GetSashPosition();
		config->Write( wxT( "LeftSplitterWidth" ), leftSashWidth );
	}

	if ( m_rightSplitter )
	{
		switch ( m_style )
		{

			case wxFB_WIDE_GUI:
				{
					int rightSash = -1 * ( m_rightSplitter->GetSize().GetWidth() - m_rightSplitter->GetSashPosition() );
					config->Write( wxT( "RightSplitterWidth" ), rightSash );

					if(m_rightSplitter->GetWindow1()->GetChildren()[0]->GetChildren()[0]->GetLabel() == _("Editor"))
                    {
                        config->Write( wxT( "RightSplitterType" ), _("editor") );
                    }
                    else
                    {
                        config->Write( wxT( "RightSplitterType" ), _("prop") );
                    }
					break;
				}

			case wxFB_CLASSIC_GUI:
				{
					int rightSash = -1 * ( m_rightSplitter->GetSize().GetHeight() - m_rightSplitter->GetSashPosition() );
					config->Write( wxT( "RightSplitterWidth" ), rightSash );
					break;
				}

			default:
				break;
		}
	}

	config->SetPath( wxT( ".." ) );
}

void MainFrame::OnSaveProject( wxCommandEvent &event )

{
	wxString filename = AppData()->GetProjectFileName();

	if ( filename == wxT( "" ) )
		OnSaveAsProject( event );
	else
	{
		try
		{
			AppData()->SaveProject( filename );
			InsertRecentProject( filename );
		}
		catch ( wxFBException& ex )
		{
			wxLogError( ex.what() );
		}
	}
}

void MainFrame::OnSaveAsProject( wxCommandEvent & )
{
	wxFileDialog *dialog = new wxFileDialog( this, wxT( "Save Project" ), m_currentDir,
	                       wxT( "" ), wxT( "wxFormBuilder Project File (*.fbp)|*.fbp|All files (*.*)|*.*" ), wxFD_SAVE );

	if ( dialog->ShowModal() == wxID_OK )
	{
		m_currentDir = dialog->GetDirectory();
		wxString filename = dialog->GetPath();

		// Add the default extension if none was chosen
		wxFileName file( filename );

		if ( !file.HasExt() )
		{
			file.SetExt( wxT( "fbp" ) );
			filename = file.GetFullPath();
		}

		// Check the file whether exists or not
		if ( file.FileExists() == true )
		{
		    wxMessageDialog msg_box( this, wxT("The file already exists. Do you want to replace it?"), wxT("Overwrite the file"), wxYES_NO|wxICON_INFORMATION|wxNO_DEFAULT );
		    if( msg_box.ShowModal() == wxID_NO )
			{
		        dialog->Destroy();
		        return;
		    }
		}

		try
		{
			AppData()->SaveProject( filename );
			InsertRecentProject( filename );
		}
		catch ( wxFBException& ex )
		{
			wxLogError( ex.what() );
		}
	};

	dialog->Destroy();
}

void MainFrame::OnOpenProject( wxCommandEvent &)
{
	if ( !SaveWarning() )
		return;

	wxFileDialog *dialog = new wxFileDialog( this, wxT( "Open Project" ), m_currentDir,
	                       wxT( "" ), wxT( "wxFormBuilder Project File (*.fbp)|*.fbp|All files (*.*)|*.*" ), wxFD_OPEN );

	if ( dialog->ShowModal() == wxID_OK )
	{
		m_currentDir = dialog->GetDirectory();
		wxString filename = dialog->GetPath();

		if ( AppData()->LoadProject( filename ) )
			InsertRecentProject( filename );
	};

	dialog->Destroy();
}

void MainFrame::OnOpenRecent( wxCommandEvent &event )
{
    if ( !SaveWarning() )
		return;

	int i = event.GetId() - ID_RECENT_0;

	assert ( i >= 0 && i < 4 );

	wxFileName filename( m_recentProjects[i] );

    if(filename.FileExists())
    {
        if ( AppData()->LoadProject( filename.GetFullPath() ) )
        {
            m_currentDir = filename.GetPath();
            InsertRecentProject( filename.GetFullPath() );
        }
    }
    else
    {
        if(wxMessageBox(wxString::Format(wxT("The project file '%s' doesn't exist. Would you like to remove it from the recent files list?"), filename.GetName().GetData()), wxT("Open recent project"), wxICON_WARNING | wxYES_NO) == wxYES)
        {
            m_recentProjects[i] = wxT("");
            UpdateRecentProjects();
        }
    }
}

void MainFrame::OnImportXrc( wxCommandEvent &)
{
	wxFileDialog *dialog = new wxFileDialog( this, wxT( "Import XRC file" ), m_currentDir,
	                       wxT( "example.xrc" ), wxT( "*.xrc" ), wxFD_OPEN );

	if ( dialog->ShowModal() == wxID_OK )
	{
		m_currentDir = dialog->GetDirectory();

		try
		{
			ticpp::Document doc;
			XMLUtils::LoadXMLFile( doc, false, dialog->GetPath() );

			XrcLoader xrc;
			xrc.SetObjectDatabase( AppData()->GetObjectDatabase() );

			PObjectBase project = xrc.GetProject( &doc );

			if ( project )
			{
				AppData()->MergeProject( project );
			}
			else
			{
				wxLogError( wxT( "Error while loading XRC" ) );
			}
		}
		catch ( wxFBException& ex )
		{
			wxLogError( _( "Error Loading XRC: %s" ), ex.what() );
		}
	}

	dialog->Destroy();
}


void MainFrame::OnNewProject( wxCommandEvent &)
{
	if ( !SaveWarning() )
		return;

	AppData()->NewProject();
}

void MainFrame::OnGenerateCode( wxCommandEvent &)
{
	AppData()->GenerateCode( false );
}

void MainFrame::OnAbout( wxCommandEvent &)
{
	AboutDialog *dlg = new AboutDialog( this );
	dlg->ShowModal();
	dlg->Destroy();
}

void MainFrame::OnExit( wxCommandEvent &)
{
	Close();
}

void MainFrame::OnClose( wxCloseEvent &event )
{
	if ( !SaveWarning() )
		return;

	SavePosition( wxT( "mainframe" ) );
    m_rightSplitter->Disconnect( wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED, wxSplitterEventHandler( MainFrame::OnSplitterChanged ) );
	event.Skip();
}

void MainFrame::OnProjectLoaded( wxFBEvent& )
{
	GetStatusBar()->SetStatusText( wxT( "Project Loaded!" ) );
	PObjectBase project = AppData()->GetProjectData();

	if ( project )
	{
		wxString objDetails = wxString::Format( wxT( "Name: %s | Class: %s" ), project->GetPropertyAsString( wxT( "name" ) ).c_str(), project->GetClassName().c_str() );
		GetStatusBar()->SetStatusText( objDetails, STATUS_FIELD_OBJECT );
	}

	UpdateFrame();
}

void MainFrame::OnProjectSaved( wxFBEvent& )
{
	GetStatusBar()->SetStatusText( wxT( "Project Saved!" ) );
	UpdateFrame();
}

void MainFrame::OnObjectExpanded( wxFBObjectEvent& )
{
	UpdateFrame();
}

void MainFrame::OnObjectSelected( wxFBObjectEvent& event )
{
	PObjectBase obj = event.GetFBObject();

	LogDebug( wxT("MainFrame::OnObjectSelected") );

    // resize sash position if necessary
    if ( m_autoSash )
    {
		wxSize      panel_size;
		int         sash_pos;

		if ( m_style != wxFB_CLASSIC_GUI )
		{
			switch(m_page_selection)
			{
			case 1: // CPP panel
				break;

			case 2: // Python panel
			   break;

            case 3: // PHP panel
			   break;

			case 4: // LUA panel
			   break;

			case 5: // XRC panel
			   break;

			case 6: // Erlang panel
			   break;

			default:
				if ( m_visualEdit != NULL )
				{

					// If selected object is not a Frame or a Panel or a dialog, we won't
					// adjust the sash position
					if ( obj->GetObjectTypeName() == wxT("form") ||
                         obj->GetObjectTypeName() == wxT("wizard")       ||
						 obj->GetObjectTypeName() == wxT("menubar_form") ||
					     obj->GetObjectTypeName() == wxT("toolbar_form") )
					{
						sash_pos = m_rightSplitter->GetSashPosition();
						panel_size = m_visualEdit->GetVirtualSize();

						LogDebug(wxT("MainFrame::OnObjectSelected > sash pos = %d"), sash_pos);
						LogDebug(wxT("MainFrame::OnObjectSelected > virtual width = %d"), panel_size.GetWidth());

						if ( panel_size.GetWidth() > sash_pos )
						{
							//set the sash position
							LogDebug(wxT("MainFrame::OnObjectSelected > set sash position"));
							m_rightSplitter_sash_pos = panel_size.GetWidth();
							m_rightSplitter->SetSashPosition(m_rightSplitter_sash_pos);
						}
					}
				}
				break;
			}
		}
    }

	wxString name;
	PProperty prop( obj->GetProperty( wxT( "name" ) ) );

	if ( prop )
		name = prop->GetValueAsString();
	else
		name = wxT( "\"Unknown\"" );

	//GetStatusBar()->SetStatusText( wxT( "Object " ) + name + wxT( " Selected!" ) );

	wxString objDetails = wxString::Format( wxT( "Name: %s | Class: %s" ), name.c_str(), obj->GetClassName().c_str() );

	GetStatusBar()->SetStatusText( objDetails, STATUS_FIELD_OBJECT );

	UpdateFrame();
}

void MainFrame::OnObjectCreated( wxFBObjectEvent& event )
{
	wxString message;

	LogDebug(wxT("MainFrame::OnObjectCreated"));

	if ( event.GetFBObject() )
	{
		message.Printf( wxT( "Object '%s' of class '%s' created." ),
		                event.GetFBObject()->GetPropertyAsString( wxT( "name" ) ).c_str(),
		                event.GetFBObject()->GetClassName().c_str() );
	}
	else
	{
		message = wxT( "Impossible to create the object. Did you forget to add a sizer/parent object or turn on/off an AUI management?" );
		wxMessageBox( message, wxT("wxFormBuilder"), wxICON_WARNING | wxOK );
	}

	GetStatusBar()->SetStatusText( message );

	UpdateFrame();
}

void MainFrame::OnObjectRemoved( wxFBObjectEvent& event )
{
	wxString message;
	message.Printf( wxT( "Object '%s' removed." ),
	                event.GetFBObject()->GetPropertyAsString( wxT( "name" ) ).c_str() );
	GetStatusBar()->SetStatusText( message );
	UpdateFrame();
}

void MainFrame::OnPropertyModified( wxFBPropertyEvent& event )
{
	PProperty prop = event.GetFBProperty();

	if ( prop )
	{
		if ( prop->GetObject() == AppData()->GetSelectedObject() )
		{
			if ( 0 == prop->GetName().CmpNoCase( wxT( "name" ) ) )
			{
				wxString oldDetails = GetStatusBar()->GetStatusText( STATUS_FIELD_OBJECT );
				wxString newDetails;
				size_t pipeIdx = oldDetails.find( wxT( "|" ) );

				if ( pipeIdx != oldDetails.npos )
				{
					newDetails.Printf( wxT( "Name: %s %s" ), prop->GetValueAsString().c_str(), oldDetails.substr( pipeIdx ).c_str() );
					GetStatusBar()->SetStatusText( newDetails, STATUS_FIELD_OBJECT );
				}
			}

			GetStatusBar()->SetStatusText( wxT( "Property Modified!" ) );
		}

		// When you change the sizeritem properties, the object modified is not
		// the same that the selected object because is a sizeritem object.
		// It's necessary to update the frame for the toolbar buttons.
		UpdateFrame();
	}
}

void MainFrame::OnEventHandlerModified( wxFBEventHandlerEvent& event )
{
	wxString message;
	message.Printf( wxT( "Event handler '%s' of object '%s' modified." ),
	                event.GetFBEventHandler()->GetName().c_str(),
	                event.GetFBEventHandler()->GetObject()->GetPropertyAsString( wxT( "name" ) ).c_str() );

	GetStatusBar()->SetStatusText( message );
	UpdateFrame();
}

void MainFrame::OnCodeGeneration( wxFBEvent& event )
{
	// Using the previously unused Id field in the event to carry a boolean
	bool panelOnly = ( event.GetId() != 0 );

	if ( panelOnly )
	{
		GetStatusBar()->SetStatusText( wxT( "Code Generated!" ) );
	}
}

void MainFrame::OnProjectRefresh( wxFBEvent& )
{
	PObjectBase project = AppData()->GetProjectData();

	if ( project )
	{
		wxString objDetails = wxString::Format( wxT( "Name: %s | Class: %s" ), project->GetPropertyAsString( wxT( "name" ) ).c_str(), project->GetClassName().c_str() );
		GetStatusBar()->SetStatusText( objDetails, STATUS_FIELD_OBJECT );
	}

	UpdateFrame();
}

void MainFrame::OnUndo( wxCommandEvent &)
{
	AppData()->Undo();
}

void MainFrame::OnRedo( wxCommandEvent &)
{
	AppData()->Redo();
}

void MainFrame::UpdateLayoutTools()
{
	int option = -1;
	int border = 0;
	int flag = 0;
	int orient = 0;

	bool gotLayoutSettings = AppData()->GetLayoutSettings( AppData()->GetSelectedObject(), &flag, &option, &border, &orient );
	wxToolBar* toolbar = GetToolBar();
	wxMenu* menuEdit = GetMenuBar()->GetMenu( GetMenuBar()->FindMenu( wxT( "Edit" ) ) );

	// Enable the layout tools if there are layout settings, else disable the tools
	menuEdit->Enable( ID_EXPAND, gotLayoutSettings );
	toolbar->EnableTool( ID_EXPAND, gotLayoutSettings );
	menuEdit->Enable( ID_STRETCH, option >= 0 );
	toolbar->EnableTool( ID_STRETCH, option >= 0 );

	bool enableHorizontalTools = ( orient != wxHORIZONTAL ) && gotLayoutSettings;
	menuEdit->Enable( ID_ALIGN_LEFT, enableHorizontalTools );
	toolbar->EnableTool( ID_ALIGN_LEFT, enableHorizontalTools );
	menuEdit->Enable( ID_ALIGN_CENTER_H, enableHorizontalTools );
	toolbar->EnableTool( ID_ALIGN_CENTER_H, enableHorizontalTools );
	menuEdit->Enable( ID_ALIGN_RIGHT, enableHorizontalTools );
	toolbar->EnableTool( ID_ALIGN_RIGHT, enableHorizontalTools );

	bool enableVerticalTools = ( orient != wxVERTICAL ) && gotLayoutSettings;
	menuEdit->Enable( ID_ALIGN_TOP, enableVerticalTools );
	toolbar->EnableTool( ID_ALIGN_TOP, enableVerticalTools );
	menuEdit->Enable( ID_ALIGN_CENTER_V, enableVerticalTools );
	toolbar->EnableTool( ID_ALIGN_CENTER_V, enableVerticalTools );
	menuEdit->Enable( ID_ALIGN_BOTTOM, enableVerticalTools );
	toolbar->EnableTool( ID_ALIGN_BOTTOM, enableVerticalTools );

	toolbar->EnableTool( ID_BORDER_TOP, gotLayoutSettings );
	toolbar->EnableTool( ID_BORDER_RIGHT, gotLayoutSettings );
	toolbar->EnableTool( ID_BORDER_LEFT, gotLayoutSettings );
	toolbar->EnableTool( ID_BORDER_BOTTOM, gotLayoutSettings );

	// Toggle the toolbar buttons according to the properties, if there are layout settings
	toolbar->ToggleTool( ID_EXPAND,         ( ( flag & wxEXPAND ) != 0 ) && gotLayoutSettings );
	toolbar->ToggleTool( ID_STRETCH,        ( option > 0 ) && gotLayoutSettings );
	toolbar->ToggleTool( ID_ALIGN_LEFT,     !( ( flag & ( wxALIGN_RIGHT | wxALIGN_CENTER_HORIZONTAL ) ) != 0 ) && enableHorizontalTools );
	toolbar->ToggleTool( ID_ALIGN_CENTER_H, ( ( flag & wxALIGN_CENTER_HORIZONTAL ) != 0 ) && enableHorizontalTools );
	toolbar->ToggleTool( ID_ALIGN_RIGHT,    ( ( flag & wxALIGN_RIGHT ) != 0 ) && enableHorizontalTools );
	toolbar->ToggleTool( ID_ALIGN_TOP,      !( ( flag & ( wxALIGN_BOTTOM | wxALIGN_CENTER_VERTICAL ) ) != 0 ) && enableVerticalTools );
	toolbar->ToggleTool( ID_ALIGN_CENTER_V, ( ( flag & wxALIGN_CENTER_VERTICAL ) != 0 ) && enableVerticalTools );
	toolbar->ToggleTool( ID_ALIGN_BOTTOM,   ( ( flag & wxALIGN_BOTTOM ) != 0 ) && enableVerticalTools );

	toolbar->ToggleTool( ID_BORDER_TOP,      ( ( flag & wxTOP ) != 0 ) && gotLayoutSettings );
	toolbar->ToggleTool( ID_BORDER_RIGHT,    ( ( flag & wxRIGHT ) != 0 ) && gotLayoutSettings );
	toolbar->ToggleTool( ID_BORDER_LEFT,     ( ( flag & wxLEFT ) != 0 ) && gotLayoutSettings );
	toolbar->ToggleTool( ID_BORDER_BOTTOM,   ( ( flag & wxBOTTOM ) != 0 ) && gotLayoutSettings );
}

void MainFrame::UpdateFrame()
{
	// Build the title
	wxString filename = AppData()->GetProjectFileName();
	wxString file;

	if ( filename.empty() )
	{
		file = wxT("untitled");
	}
	else
	{
		wxFileName fn( filename );
		file = fn.GetName();
	}

	SetTitle(wxString::Format(wxT("%s%s - wxFormBuilder %s%s"),
	                          AppData()->IsModified() ? wxT("*") : wxT(""), file.c_str(), getVersion(), getReleaseRevision(getVersion()).c_str()));
	GetStatusBar()->SetStatusText( filename, STATUS_FIELD_PATH );

	// Enable/Disable toolbar and menu entries
	wxToolBar* toolbar = GetToolBar();

	wxMenu* menuFile = GetMenuBar()->GetMenu(GetMenuBar()->FindMenu(_("File")));
	menuFile->Enable(ID_SAVE_PRJ, AppData()->IsModified());
	toolbar->EnableTool(ID_SAVE_PRJ, AppData()->IsModified());

	wxMenu* menuEdit = GetMenuBar()->GetMenu( GetMenuBar()->FindMenu( wxT( "Edit" ) ) );

	bool redo = AppData()->CanRedo();
	menuEdit->Enable( ID_REDO, redo );
	toolbar->EnableTool( ID_REDO, redo );

	bool undo = AppData()->CanUndo();
	menuEdit->Enable( ID_UNDO, undo );
	toolbar->EnableTool( ID_UNDO, undo );

	bool copy = AppData()->CanCopyObject();
	bool isEditor = ( _("Designer") != m_notebook->GetPageText( m_notebook->GetSelection() ) );
	menuEdit->Enable( ID_FIND, isEditor );

	menuEdit->Enable( ID_CLIPBOARD_COPY, copy );

	menuEdit->Enable( ID_COPY, copy || isEditor );
	toolbar->EnableTool( ID_COPY, copy || isEditor );

	menuEdit->Enable( ID_CUT, copy );
	toolbar->EnableTool( ID_CUT, copy );

	menuEdit->Enable( ID_DELETE, copy );
	toolbar->EnableTool( ID_DELETE, copy );

	menuEdit->Enable( ID_MOVE_UP, copy );
	menuEdit->Enable( ID_MOVE_DOWN, copy );
	menuEdit->Enable( ID_MOVE_LEFT, copy );
	menuEdit->Enable( ID_MOVE_RIGHT, copy );

	bool paste = AppData()->CanPasteObject();
	menuEdit->Enable( ID_PASTE, paste );
	toolbar->EnableTool( ID_PASTE, paste );

	menuEdit->Enable( ID_CLIPBOARD_PASTE, AppData()->CanPasteObjectFromClipboard() );

	UpdateLayoutTools();
}

void MainFrame::UpdateRecentProjects()
{
	wxMenu *menuFile = GetMenuBar()->GetMenu( GetMenuBar()->FindMenu( wxT( "File" ) ) );

	// borramos los items del menu de los projectos recientes

	for (int i = 0 ; i < 4 ; i++)
	{
		if ( menuFile->FindItem( ID_RECENT_0 + i ) )
			menuFile->Destroy( ID_RECENT_0 + i );
	}

	wxMenuItem* mruSep = menuFile->FindItemByPosition( menuFile->GetMenuItemCount() - 1 );
	if ( mruSep->IsSeparator() )
	{
	    menuFile->Destroy( mruSep );
	}

	// remove empty filenames and 'compress' the rest
    int fi = 0;
	for (int i = 0 ; i < 4 ; i++)
	{
	    if(!m_recentProjects[i].IsEmpty())
	        m_recentProjects[fi++] = m_recentProjects[i];
	}
	for (int i = fi ; i < 4 ; i++)
        m_recentProjects[i] = wxT("");

    if ( !m_recentProjects[0].IsEmpty() )
    {
        menuFile->AppendSeparator();
    }

	// creamos los nuevos ficheros recientes
	for ( unsigned int i = 0 ; i < 4 && !m_recentProjects[i].IsEmpty() ; i++ )
		menuFile->Append( ID_RECENT_0 + i, m_recentProjects[i], wxT( "" ) );
}

void MainFrame::InsertRecentProject( const wxString &file )
{
	bool found = false;
	int i;

	for ( i = 0; i < 4 && !found; i++ )
		found = ( file == m_recentProjects[i] );

	if ( found ) // en i-1 está la posición encontrada (0 < i < 4)
	{
		// desplazamos desde 0 hasta i-1 una posición a la derecha

		for ( i = i - 1; i > 0; i-- )
			m_recentProjects[i] = m_recentProjects[i-1];
	}
	else if ( !found )
	{
		for ( i = 3; i > 0; i-- )
			m_recentProjects[i] = m_recentProjects[i-1];
	}

	m_recentProjects[0] = file;

	UpdateRecentProjects();
}

void MainFrame::OnCopy( wxCommandEvent &)

{
	wxWindow *focusedWindow = wxWindow::FindFocus();

    if ( focusedWindow != NULL && focusedWindow->IsKindOf( wxCLASSINFO( wxStyledTextCtrl ) ) )
    {
        ( ( wxStyledTextCtrl* )focusedWindow )->Copy();
    }
	else
	{
		AppData()->CopyObject( AppData()->GetSelectedObject() );
		UpdateFrame();
	}
}

void MainFrame::OnCut ( wxCommandEvent &)
{
	wxWindow *focusedWindow = wxWindow::FindFocus();

    if ( focusedWindow != NULL && focusedWindow->IsKindOf( wxCLASSINFO( wxStyledTextCtrl ) ) )
    {
        ( ( wxStyledTextCtrl* )focusedWindow )->Cut();
    }
	else
	{
		AppData()->CutObject( AppData()->GetSelectedObject() );
		UpdateFrame();
	}
}

void MainFrame::OnDelete ( wxCommandEvent &)
{
	AppData()->RemoveObject( AppData()->GetSelectedObject() );
	UpdateFrame();
}

void MainFrame::OnPaste ( wxCommandEvent &)
{
	wxWindow *focusedWindow = wxWindow::FindFocus();

    if ( focusedWindow != NULL && focusedWindow->IsKindOf( wxCLASSINFO( wxStyledTextCtrl ) ) )
    {
        ( ( wxStyledTextCtrl* )focusedWindow )->Paste();
    }
	else
	{
		AppData()->PasteObject( AppData()->GetSelectedObject() );
		UpdateFrame();
	}
}

void MainFrame::OnClipboardCopy(wxCommandEvent& )
{
	AppData()->CopyObjectToClipboard( AppData()->GetSelectedObject() );
	UpdateFrame();
}

void MainFrame::OnClipboardPaste(wxCommandEvent& )
{
	AppData()->PasteObjectFromClipboard( AppData()->GetSelectedObject() );
	UpdateFrame();
}

void MainFrame::OnClipboardPasteUpdateUI( wxUpdateUIEvent& e )
{
	e.Enable( AppData()->CanPasteObjectFromClipboard() );
}

void MainFrame::OnToggleExpand ( wxCommandEvent &)
{
	AppData()->ToggleExpandLayout( AppData()->GetSelectedObject() );
}

void MainFrame::OnToggleStretch ( wxCommandEvent &)
{
	AppData()->ToggleStretchLayout( AppData()->GetSelectedObject() );
}

void MainFrame::OnMoveUp ( wxCommandEvent &)
{
	AppData()->MovePosition( AppData()->GetSelectedObject(), false, 1 );
}

void MainFrame::OnMoveDown ( wxCommandEvent &)
{
	AppData()->MovePosition( AppData()->GetSelectedObject(), true, 1 );
}

void MainFrame::OnMoveLeft ( wxCommandEvent &)
{
	AppData()->MoveHierarchy( AppData()->GetSelectedObject(), true );
}

void MainFrame::OnMoveRight ( wxCommandEvent & )
{
	AppData()->MoveHierarchy( AppData()->GetSelectedObject(), false );
}

void MainFrame::OnChangeAlignment ( wxCommandEvent &event )
{
	int align = 0;
	bool vertical = ( event.GetId() == ID_ALIGN_TOP ||
	                  event.GetId() == ID_ALIGN_BOTTOM ||
	                  event.GetId() == ID_ALIGN_CENTER_V );

	switch ( event.GetId() )
	{

		case ID_ALIGN_RIGHT:
			align = wxALIGN_RIGHT;

			break;

		case ID_ALIGN_CENTER_H:
			align = wxALIGN_CENTER_HORIZONTAL;

			break;

		case ID_ALIGN_BOTTOM:
			align = wxALIGN_BOTTOM;

			break;

		case ID_ALIGN_CENTER_V:
			align = wxALIGN_CENTER_VERTICAL;

			break;
	}

	AppData()->ChangeAlignment( AppData()->GetSelectedObject(), align, vertical );

	UpdateLayoutTools();
}

void MainFrame::OnChangeBorder( wxCommandEvent& e )
{
	int border;

	switch ( e.GetId() )
	{
		case ID_BORDER_LEFT:
			border = wxLEFT;
			break;

		case ID_BORDER_RIGHT:
			border = wxRIGHT;
			break;

		case ID_BORDER_TOP:
			border = wxTOP;
			break;

		case ID_BORDER_BOTTOM:
			border = wxBOTTOM;
			break;

		default:
			border = 0;
			break;
	}

	AppData()->ToggleBorderFlag( AppData()->GetSelectedObject(), border );

	UpdateLayoutTools();
}

void MainFrame::OnXrcPreview( wxCommandEvent& WXUNUSED( e ) )
{
	AppData()->ShowXrcPreview();

	/*wxPaneInfoArray& all_panes = m_mgr.GetAllPanes();
	for ( int i = 0, count = all_panes.GetCount(); i < count; ++i )
	{
		wxPaneInfo info = all_panes.Item( i );
	}*/

}

void MainFrame::OnGenInhertedClass( wxCommandEvent& WXUNUSED( e ) )
{
	wxString filePath;
	try
	{
		// Get the output path
		filePath = AppData()->GetOutputPath();
	}
	catch ( wxFBException& ex )
	{
		wxLogWarning( ex.what() );
		return;
	}

	// Show the dialog
	PObjectBase project = AppData()->GetProjectData();
	if ( project->IsNull( _("file") ) )
	{
		wxLogWarning( _("You must set the \"file\" property of the project before generating inherited classes.") );
		return;
	}
	GenInheritedClassDlg dlg( this, project );

	if ( wxID_OK != dlg.ShowModal() )
	{
		return;
	}

	std::vector< GenClassDetails > selectedForms;
	dlg.GetFormsSelected( &selectedForms );

	for ( size_t i = 0; i < selectedForms.size(); ++i )
	{
		const GenClassDetails& details = selectedForms[i];

		// Create the class and files.
		AppData()->GenerateInheritedClass( details.m_form, details.m_className, filePath, details.m_fileName );
	}

	wxMessageBox( wxString::Format( wxT( "Class(es) generated to \'%s\'." ), filePath.c_str() ), wxT("wxFormBuilder") );
}

bool MainFrame::SaveWarning()
{
	int result = wxYES;

	if ( AppData()->IsModified() )
	{
		result = ::wxMessageBox( wxT( "Current project file has been modified...\n" )
		                         wxT( "Do you want to save the changes?" ),
		                         wxT( "Save project" ),
		                         wxYES | wxNO | wxCANCEL,
		                         this );

		if ( result == wxYES )
		{
			wxCommandEvent dummy;
			OnSaveProject( dummy );
		}
	}

	return ( result != wxCANCEL );
}

void MainFrame::OnAuiNotebookPageChanged( wxAuiNotebookEvent& event )
{
	UpdateFrame();

	if ( m_autoSash )
	{
		m_page_selection = event.GetSelection();
		LogDebug(wxT("MainFrame::OnFlatNotebookPageChanged > selection = %d"), m_page_selection);

		wxSize panel_size;
		int sash_pos;

		if(m_style != wxFB_CLASSIC_GUI)
		{
			switch( m_page_selection )
			{
			case 1: // CPP panel
				if( (m_cpp != NULL) && (m_rightSplitter != NULL) )
				{
					panel_size = m_cpp->GetClientSize();
					sash_pos = m_rightSplitter->GetSashPosition();

					LogDebug(wxT("MainFrame::OnFlatNotebookPageChanged > CPP panel: width = %d sash pos = %d"), panel_size.GetWidth(), sash_pos);

					if(panel_size.GetWidth() > sash_pos)
					{
						// set the sash position
						LogDebug(wxT("MainFrame::OnFlatNotebookPageChanged > reset sash position"));
						m_rightSplitter->SetSashPosition(panel_size.GetWidth());
					}
				}
				break;

			case 2: // Python panel
				if( (m_python != NULL) && (m_rightSplitter != NULL) )
				{
					panel_size = m_python->GetClientSize();
					sash_pos = m_rightSplitter->GetSashPosition();

					LogDebug(wxT("MainFrame::OnFlatNotebookPageChanged > Python panel: width = %d sash pos = %d"), panel_size.GetWidth(), sash_pos);

					if(panel_size.GetWidth() > sash_pos)
					{
						// set the sash position
						LogDebug(wxT("MainFrame::OnFlatNotebookPageChanged > reset sash position"));
						m_rightSplitter->SetSashPosition(panel_size.GetWidth());
					}
				}
				break;

            case 3: // PHP panel
				if( (m_php != NULL) && (m_rightSplitter != NULL) )
				{
					panel_size = m_xrc->GetClientSize();
					sash_pos = m_rightSplitter->GetSashPosition();

					LogDebug(wxT("MainFrame::OnFlatNotebookPageChanged > PHP panel: width = %d sash pos = %d"), panel_size.GetWidth(), sash_pos);

					if(panel_size.GetWidth() > sash_pos)
					{
						// set the sash position
						LogDebug(wxT("MainFrame::OnFlatNotebookPageChanged > reset sash position"));
						m_rightSplitter->SetSashPosition(panel_size.GetWidth());
					}
				}
				break;

			case 4: // Lua panel
				if( (m_lua != NULL) && (m_rightSplitter != NULL) )
				{
					panel_size = m_lua->GetClientSize();
					sash_pos = m_rightSplitter->GetSashPosition();

					LogDebug(wxT("MainFrame::OnFlatNotebookPageChanged > Lua panel: width = %d sash pos = %d"), panel_size.GetWidth(), sash_pos);

					if(panel_size.GetWidth() > sash_pos)
					{
						// set the sash position
						LogDebug(wxT("MainFrame::OnFlatNotebookPageChanged > reset sash position"));
						m_rightSplitter->SetSashPosition(panel_size.GetWidth());
					}
				}
				break;

			case 5: // XRC panel
				if((m_xrc != NULL) && (m_rightSplitter != NULL))
				{
					panel_size = m_xrc->GetClientSize();
					sash_pos = m_rightSplitter->GetSashPosition();

					LogDebug(wxT("MainFrame::OnFlatNotebookPageChanged > XRC panel: width = %d sash pos = %d"), panel_size.GetWidth(), sash_pos);

					if(panel_size.GetWidth() > sash_pos)
					{
						// set the sash position
						LogDebug(wxT("MainFrame::OnFlatNotebookPageChanged > reset sash position"));
						m_rightSplitter->SetSashPosition(panel_size.GetWidth());
					}
				}
				break;
            case 6: // Erlang panel
				if( (m_erlang != NULL) && (m_rightSplitter != NULL) )
				{
					panel_size = m_erlang->GetClientSize();
					sash_pos = m_rightSplitter->GetSashPosition();

					LogDebug(wxT("MainFrame::OnFlatNotebookPageChanged > Erlang panel: width = %d sash pos = %d"), panel_size.GetWidth(), sash_pos);

					if(panel_size.GetWidth() > sash_pos)
					{
						// set the sash position
						LogDebug(wxT("MainFrame::OnFlatNotebookPageChanged > reset sash position"));
						m_rightSplitter->SetSashPosition(panel_size.GetWidth());
					}
				}
				break;



			default:
				if(m_visualEdit != NULL)
				{
					sash_pos = m_rightSplitter->GetSashPosition();

					if(m_rightSplitter_sash_pos < sash_pos)
					{
						//restore the sash position
						LogDebug(wxT("MainFrame::OnFlatNotebookPageChanged > restore sash position: sash pos = %d"), m_rightSplitter_sash_pos);
						m_rightSplitter->SetSashPosition(m_rightSplitter_sash_pos);
					}
					else
					{
						// update position
						m_rightSplitter_sash_pos = sash_pos;
					}
				}
				break;
			}
		}
	}

	AppData()->GenerateCode( true );
}

void MainFrame::OnFindDialog( wxCommandEvent& )
{
	if ( NULL == m_findDialog )
	{
		m_findDialog = new wxFindReplaceDialog( this, &m_findData, wxT("Find") );
		m_findDialog->Centre( wxCENTRE_ON_SCREEN | wxBOTH );
	}
	m_findDialog->Show( true );
}

void MainFrame::OnFindClose( wxFindDialogEvent& )
{
	m_findDialog->Destroy();
	m_findDialog = 0;
}

void MainFrame::OnFind( wxFindDialogEvent& event )
{
	/*for ( int page = 0; page < m_notebook->GetPageCount(); ++page )
	{
		event.StopPropagation();
		event.SetClientData( m_findDialog );
		m_notebook->GetPage( page )->GetEventHandler()->ProcessEvent( event );
	}*/

	wxWindow *page = m_notebook->GetCurrentPage();
	if( page )
	{
		event.StopPropagation();
		event.SetClientData( m_findDialog );
		page->GetEventHandler()->ProcessEvent( event );
	}
}

/////////////////////////////////////////////////////////////////////////////

wxMenuBar * MainFrame::CreateFBMenuBar()
{
	wxMenu *menuFile = new wxMenu;
	menuFile->Append( ID_NEW_PRJ, wxT( "&New Project\tCtrl+N" ), wxT( "Create an empty project" ) );
	menuFile->Append( ID_OPEN_PRJ, wxT( "&Open...\tCtrl+O" ), wxT( "Open a project" ) );

	menuFile->Append( ID_SAVE_PRJ,          wxT( "&Save\tCtrl+S" ), wxT( "Save current project" ) );
	menuFile->Append( ID_SAVE_AS_PRJ, wxT( "Save &As...\tCtrl-Shift+S" ), wxT( "Save current project as..." ) );
	menuFile->AppendSeparator();
	menuFile->Append( ID_IMPORT_XRC, wxT( "&Import XRC..." ), wxT( "Import XRC file" ) );
	menuFile->AppendSeparator();
	menuFile->Append( ID_GENERATE_CODE, wxT( "&Generate Code\tF8" ), wxT( "Generate Code" ) );
	menuFile->AppendSeparator();
	menuFile->Append( wxID_EXIT, wxT( "E&xit\tAlt-F4" ), wxT( "Quit wxFormBuilder" ) );

	wxMenu *menuEdit = new wxMenu;
	menuEdit->Append( ID_UNDO, wxT( "&Undo \tCtrl+Z" ), wxT( "Undo changes" ) );
	menuEdit->Append( ID_REDO, wxT( "&Redo \tCtrl+Y" ), wxT( "Redo changes" ) );
	menuEdit->AppendSeparator();
	menuEdit->Append( ID_COPY, wxT( "&Copy \tCtrl+C" ), wxT( "Copy selected object" ) );
	menuEdit->Append( ID_CUT, wxT( "Cut \tCtrl+X" ), wxT( "Cut selected object" ) );
	menuEdit->Append( ID_PASTE, wxT( "&Paste \tCtrl+V" ), wxT( "Paste on selected object" ) );
	menuEdit->Append( ID_DELETE, wxT( "&Delete \tCtrl+D" ), wxT( "Delete selected object" ) );
	menuEdit->AppendSeparator();
	menuEdit->Append( ID_CLIPBOARD_COPY, wxT("Copy Object To Clipboard\tCtrl+Shift+C"), wxT("Copy Object to Clipboard") );
	menuEdit->Append( ID_CLIPBOARD_PASTE, wxT("Paste Object From Clipboard\tCtrl+Shift+V"), wxT("Paste Object from Clipboard") );
	menuEdit->AppendSeparator();
	menuEdit->Append( ID_EXPAND, wxT( "Toggle &Expand\tAlt+W" ), wxT( "Toggle wxEXPAND flag of sizeritem properties" ) );
	menuEdit->Append( ID_STRETCH, wxT( "Toggle &Stretch\tAlt+S" ), wxT( "Toggle option property of sizeritem properties" ) );
	menuEdit->Append( ID_MOVE_UP, wxT( "Move Up\tAlt+Up" ), wxT( "Move Up selected object" ) );
	menuEdit->Append( ID_MOVE_DOWN, wxT( "Move Down\tAlt+Down" ), wxT( "Move Down selected object" ) );
	menuEdit->Append( ID_MOVE_LEFT, wxT( "Move Left\tAlt+Left" ), wxT( "Move Left selected object" ) );
	menuEdit->Append( ID_MOVE_RIGHT, wxT( "Move Right\tAlt+Right" ), wxT( "Move Right selected object" ) );
	menuEdit->AppendSeparator();
	menuEdit->Append( ID_FIND, wxT( "&Find\tCtrl+F" ), wxT( "Find text in the active code viewer" ) );
	menuEdit->AppendSeparator();
	menuEdit->Append( ID_ALIGN_LEFT,     wxT( "&Align &Left\tAlt+Shift+Left" ),           wxT( "Align item to the left" ) );
	menuEdit->Append( ID_ALIGN_CENTER_H, wxT( "&Align Center &Horizontal\tAlt+Shift+H" ), wxT( "Align item to the center horizontally" ) );
	menuEdit->Append( ID_ALIGN_RIGHT,    wxT( "&Align &Right\tAlt+Shift+Right" ),         wxT( "Align item to the right" ) );
	menuEdit->Append( ID_ALIGN_TOP,      wxT( "&Align &Top\tAlt+Shift+Up" ),              wxT( "Align item to the top" ) );
	menuEdit->Append( ID_ALIGN_CENTER_V, wxT( "&Align Center &Vertical\tAlt+Shift+V" ),   wxT( "Align item to the center vertically" ) );
	menuEdit->Append( ID_ALIGN_BOTTOM,   wxT( "&Align &Bottom\tAlt+Shift+Down" ),         wxT( "Align item to the bottom" ) );

    wxMenu *menuComponents = CreateMenuComponents();

    wxMenu *menuView = new wxMenu;
	menuView->Append( ID_PREVIEW_XRC, wxT( "&XRC Window\tF5" ), wxT( "Show a preview of the XRC window" ) );
	menuView->AppendSeparator();
	menuView->Append( ID_WINDOW_SWAP, wxT( "&Swap The Editor and Properties Window\tF12" ), wxT( "Swap The Editor and Properties Window" ) );

	wxMenu *menuTools = new wxMenu;
	menuTools->Append( ID_GEN_INHERIT_CLS, wxT( "&Generate Inherited Class\tF6" ), wxT( "Creates the needed files and class for proper inheritance of your designed GUI" ) );

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append( wxID_ABOUT, wxT( "&About...\tF1" ), wxT( "Show about dialog" ) );

	// now append the freshly created menu to the menu bar...
	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append( menuFile, wxT( "&File" ) );
	menuBar->Append( menuEdit, wxT( "&Edit" ) );
    menuBar->Append( menuComponents, wxT( "&Components" ) );
	menuBar->Append( menuView, wxT( "&View" ) );
	menuBar->Append( menuTools, wxT( "&Tools" ) );
	menuBar->Append( menuHelp, wxT( "&Help" ) );

    return menuBar;
}

wxMenu * MainFrame::CreateMenuComponents()
{
    wxMenu *menuComponents = new wxMenu;

    menuComponents->Append(ID_FIND_COMPONENT, _("&Find Component...\tCtrl+Shift+F"), _("Show Component Search Dialog"));

    // Package count
    unsigned int pkg_count = AppData()->GetPackageCount();
    // Lookup map of all packages
    std::map<wxString, PObjectPackage> packages;
    // List of pages to add to the menu in the same order like notebook
    std::vector<std::pair<wxString, PObjectPackage>> pages;
    pages.reserve( pkg_count );

    // Fill lookup map of packages
    for ( unsigned int i = 0; i < pkg_count; ++i )
    {
        auto pkg = AppData()->GetPackage( i );
        packages.insert( std::make_pair( pkg->GetPackageName(), pkg ) );
    }

    // Read the page order from settings and build the list of pages from it
    auto *config = wxConfigBase::Get();
    wxStringTokenizer pageOrder( config->Read( wxT( "/palette/pageOrder" ),
                                             wxT( "Common,Additional,Data,Containers,Menu/Toolbar,"
                                                 "Layout,Forms,Ribbon" ) ), wxT( "," ) );
    while ( pageOrder.HasMoreTokens() )
    {
        const auto packageName = pageOrder.GetNextToken();
        auto package = packages.find( packageName );
        if ( packages.end() == package )
        {
            // Plugin missing - move on
            continue;
        }

        // Add package to pages list and remove from lookup map
        pages.push_back( std::make_pair( package->first, package->second ) );
        packages.erase( package );
    }

    // The remaining packages from the lookup map need to be added to the page list
    for ( auto& package : packages )
    {
        pages.push_back( std::make_pair( package.first, package.second ) );
    }
    packages.clear();

    for ( size_t i = 0; i < pages.size(); ++i )
    {
        const auto& page = pages[i];

        auto* submenu = CreateSubmenuComponents(page.second);
        wxMenuItem *menuItem = new wxMenuItem( menuComponents, wxID_ANY, page.first, wxEmptyString,
                                              wxITEM_NORMAL, submenu );

//        menuComponents->AppendSubMenu( submenu, page.first );
        menuItem->SetBitmap( page.second->GetPackageIcon() );
        menuComponents->Append( menuItem );
    }

    return menuComponents;
}

wxMenu* MainFrame::CreateSubmenuComponents(PObjectPackage pkg)
{
    wxMenu *submenu = new wxMenu;
    unsigned int j = 0;

    while ( j < pkg->GetObjectCount() )
    {
        PObjectInfo info = pkg->GetObjectInfo( j );

        if ( info->IsStartOfGroup() )
        {
            submenu->AppendSeparator();
        }

        if ( nullptr == info->GetComponent() )
        {
            LogDebug( _( "Missing Component for Class \"" + info->GetClassName() +
                       "\" of Package \"" + pkg->GetPackageName() + "\"." ) );
        }
        else
        {
            auto* item = new wxMenuItem(submenu, wxID_ANY, info->GetClassName());
			item->SetBitmap(info->GetIconFile());
			submenu->Append(item);
        }
        j++;
    }
	submenu->Bind(wxEVT_MENU, &MainFrame::OnMenuComponentsClick, this);

    return submenu;
}

wxToolBar * MainFrame::CreateFBToolBar()
{
	wxToolBar* toolbar = CreateToolBar();
	toolbar->SetToolBitmapSize( wxSize( TOOL_SIZE, TOOL_SIZE ) );
	toolbar->AddTool( ID_NEW_PRJ, wxT( "New Project" ), AppBitmaps::GetBitmap( wxT( "new" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "New Project (Ctrl+N)" ), wxT( "Start a new project." ) );
	toolbar->AddTool( ID_OPEN_PRJ, wxT( "Open Project" ), AppBitmaps::GetBitmap( wxT( "open" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Open Project (Ctrl+O)" ), wxT( "Open an existing project." ) );
	toolbar->AddTool( ID_SAVE_PRJ, wxT( "Save Project" ), AppBitmaps::GetBitmap( wxT( "save" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Save Project (Ctrl+S)" ), wxT( "Save the current project." ) );
	toolbar->AddSeparator();
	toolbar->AddTool( ID_UNDO, wxT( "Undo" ), AppBitmaps::GetBitmap( wxT( "undo" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Undo (Ctrl+Z)" ), wxT( "Undo the last action." ) );
	toolbar->AddTool( ID_REDO, wxT( "Redo" ), AppBitmaps::GetBitmap( wxT( "redo" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Redo (Ctrl+Y)" ), wxT( "Redo the last action that was undone." ) );
	toolbar->AddSeparator();
	toolbar->AddTool( ID_CUT, wxT( "Cut" ), AppBitmaps::GetBitmap( wxT( "cut" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Cut (Ctrl+X)" ), wxT( "Remove the selected object and place it on the clipboard." ) );
	toolbar->AddTool( ID_COPY, wxT( "Copy" ), AppBitmaps::GetBitmap( wxT( "copy" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Copy (Ctrl+C)" ), wxT( "Copy the selected object to the clipboard." ) );
	toolbar->AddTool( ID_PASTE, wxT( "Paste" ), AppBitmaps::GetBitmap( wxT( "paste" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Paste (Ctrl+V)" ), wxT( "Insert an object from the clipboard." ) );
	toolbar->AddTool( ID_DELETE, wxT( "Delete" ), AppBitmaps::GetBitmap( wxT( "delete" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Delete (Ctrl+D)" ), wxT( "Remove the selected object." ) );
	toolbar->AddSeparator();
	toolbar->AddTool( ID_GENERATE_CODE, wxT( "Generate Code" ), AppBitmaps::GetBitmap( wxT( "generate" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Generate Code (F8)" ), wxT( "Create code from the current project." ) );
	toolbar->AddSeparator();
	toolbar->AddTool( ID_ALIGN_LEFT, wxT( "" ), AppBitmaps::GetBitmap( wxT( "lalign" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Align Left" ), wxT( "The item will be aligned to the left of the space allotted to it by the sizer." ) );
	toolbar->AddTool( ID_ALIGN_CENTER_H, wxT( "" ), AppBitmaps::GetBitmap( wxT( "chalign" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Align Center Horizontally" ), wxT( "The item will be centered horizontally in the space allotted to it by the sizer." ) );
	toolbar->AddTool( ID_ALIGN_RIGHT, wxT( "" ), AppBitmaps::GetBitmap( wxT( "ralign" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Align Right" ), wxT( "The item will be aligned to the right of the space allotted to it by the sizer." ) );
	toolbar->AddSeparator();
	toolbar->AddTool( ID_ALIGN_TOP, wxT( "" ), AppBitmaps::GetBitmap( wxT( "talign" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Align Top" ), wxT( "The item will be aligned to the top of the space allotted to it by the sizer." ) );
	toolbar->AddTool( ID_ALIGN_CENTER_V, wxT( "" ), AppBitmaps::GetBitmap( wxT( "cvalign" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Align Center Vertically" ), wxT( "The item will be centered vertically within space allotted to it by the sizer." ) );
	toolbar->AddTool( ID_ALIGN_BOTTOM, wxT( "" ), AppBitmaps::GetBitmap( wxT( "balign" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Align Bottom" ), wxT( "The item will be aligned to the bottom of the space allotted to it by the sizer." ) );
	toolbar->AddSeparator();
	toolbar->AddTool( ID_EXPAND, wxT( "" ), AppBitmaps::GetBitmap( wxT( "expand" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Expand (Alt+W)" ), wxT( "The item will be expanded to fill the space assigned to the item." ) );
	toolbar->AddTool( ID_STRETCH, wxT( "" ), AppBitmaps::GetBitmap( wxT( "stretch" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Stretch (Alt+S)" ), wxT( "The item will grow and shrink with the sizer." ) );
	toolbar->AddSeparator();
	toolbar->AddTool( ID_BORDER_LEFT, wxT( "" ), AppBitmaps::GetBitmap( wxT( "left" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Left Border" ), wxT( "A border will be added on the left side of the item." ) );
	toolbar->AddTool( ID_BORDER_RIGHT, wxT( "" ), AppBitmaps::GetBitmap( wxT( "right" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Right Border" ), wxT( "A border will be  added on the right side of the item." ) );
	toolbar->AddTool( ID_BORDER_TOP, wxT( "" ), AppBitmaps::GetBitmap( wxT( "top" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Top Border" ), wxT( "A border will be  added on the top of the item." ) );
	toolbar->AddTool( ID_BORDER_BOTTOM, wxT( "" ), AppBitmaps::GetBitmap( wxT( "bottom" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Bottom Border" ), wxT( "A border will be  added on the bottom of the item." ) );
    toolbar->AddSeparator();
    toolbar->AddTool( ID_WINDOW_SWAP, wxT( "" ), AppBitmaps::GetBitmap( wxT( "swap" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Swap The Editor and Properties Window (F12)" ), wxT( "Swap the design window and properties window." ) );
	toolbar->Realize();

	return toolbar;
}

wxWindow * MainFrame::CreateDesignerWindow( wxWindow *parent )
{
	m_notebook = new wxAuiNotebook( parent, ID_EDITOR_FNB, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP );
	m_notebook->SetArtProvider( new AuiTabArt() );

	m_visualEdit = new VisualEditor( m_notebook );
	AppData()->GetManager()->SetVisualEditor( m_visualEdit );

	m_notebook->AddPage( m_visualEdit, wxT( "Designer" ), false, 0 );
	m_notebook->SetPageBitmap( 0, AppBitmaps::GetBitmap( wxT( "designer" ), 16 ) );

	m_cpp = new CppPanel( m_notebook, wxID_ANY);
	m_notebook->AddPage( m_cpp, wxT( "C++" ), false, 1 );
	m_notebook->SetPageBitmap( 1, AppBitmaps::GetBitmap( wxT( "c++" ), 16 ) );

	m_python = new PythonPanel( m_notebook, wxID_ANY);
	m_notebook->AddPage( m_python, wxT( "Python" ), false, 2 );
	m_notebook->SetPageBitmap( 2, AppBitmaps::GetBitmap( wxT( "python" ), 16 ) );

	m_php = new PHPPanel( m_notebook, wxID_ANY);
	m_notebook->AddPage( m_php, wxT( "PHP" ), false, 3 );
	m_notebook->SetPageBitmap( 3, AppBitmaps::GetBitmap( wxT( "php" ), 16 ) );

	m_lua = new LuaPanel(m_notebook, wxID_ANY);
	m_notebook->AddPage(m_lua,wxT( "Lua" ), false, 4 );
	m_notebook->SetPageBitmap( 4, AppBitmaps::GetBitmap( wxT( "lua" ), 16 ) );

	m_erlang = new ErlangPanel( m_notebook, wxID_ANY);
	m_notebook->AddPage( m_erlang, wxT( "Erlang" ), false, 5 );
	m_notebook->SetPageBitmap( 5, AppBitmaps::GetBitmap( wxT( "erlang" ), 16 ) );

	m_xrc = new XrcPanel( m_notebook, wxID_ANY);
	m_notebook->AddPage( m_xrc, wxT( "XRC" ), false, 6 );
	m_notebook->SetPageBitmap( 6, AppBitmaps::GetBitmap( wxT( "xrc" ), 16 ) );

	return m_notebook;
}

wxWindow * MainFrame::CreateComponentPalette ( wxWindow *parent )
{
	// la paleta de componentes, no es un observador propiamente dicho, ya
	// que no responde ante los eventos de la aplicación
	m_palette = new wxFbPalette( parent, wxID_ANY);
	m_palette->Create();
	//m_palette->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );

	return m_palette;
}

wxWindow * MainFrame::CreateObjectTree( wxWindow *parent )
{
	m_objTree = new ObjectTree( parent, wxID_ANY);
	m_objTree->Create();

	return m_objTree;
}

wxWindow * MainFrame::CreateObjectInspector( wxWindow *parent )
{
	//TO-DO: make object inspector style selectable.
	int style = ( m_style == wxFB_CLASSIC_GUI ? wxFB_OI_MULTIPAGE_STYLE : wxFB_OI_SINGLE_PAGE_STYLE );
	m_objInsp = new ObjectInspector( parent, wxID_ANY, style );
	return m_objInsp;
}

void MainFrame::CreateWideGui()
{
	// MainFrame only contains m_leftSplitter window
	m_leftSplitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE );

	wxWindow *objectTree = Title::CreateTitle( CreateObjectTree( m_leftSplitter ), wxT( "Object Tree" ) );

	// panel1 contains Palette and splitter2 (m_rightSplitter)
	wxPanel *panel1 = new wxPanel( m_leftSplitter, wxID_ANY);

	wxWindow *palette = Title::CreateTitle( CreateComponentPalette( panel1 ), wxT( "Component Palette" ) );
	m_rightSplitter   =  new wxSplitterWindow( panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE );

	wxBoxSizer *panel1_sizer = new wxBoxSizer( wxVERTICAL );
	panel1_sizer->Add( palette, 0, wxEXPAND );
	panel1_sizer->Add( m_rightSplitter, 1, wxEXPAND );
	panel1->SetSizer( panel1_sizer );

	// splitter2 contains the editor and the object inspector
	wxWindow *designer        = Title::CreateTitle( CreateDesignerWindow( m_rightSplitter ), wxT( "Editor" ) );
	wxWindow *objectInspector = Title::CreateTitle( CreateObjectInspector( m_rightSplitter ), wxT( "Object Properties" ) );

	m_leftSplitter->SplitVertically( objectTree, panel1, m_leftSplitterWidth );

	// Need to update the left splitter so the right one is drawn correctly
	wxSizeEvent update( GetSize(), GetId() );
	ProcessEvent( update );
	m_leftSplitter->UpdateSize();
	m_leftSplitter->SetMinimumPaneSize( 2 );

    //modified by tyysoft to restore the last layout.
    if(m_rightSplitterType == _("editor"))
    {
        m_rightSplitter->SplitVertically( designer, objectInspector, m_rightSplitterWidth );
    }
    else
    {
        m_rightSplitter->SplitVertically( objectInspector, designer, m_rightSplitterWidth );
    }
	m_rightSplitter->SetSashGravity( 1 );
	m_rightSplitter->SetMinimumPaneSize( 2 );

	m_style = wxFB_WIDE_GUI;

	SetMinSize( wxSize( 700, 380 ) );
}

void MainFrame::CreateClassicGui()
{
	// Give ID to left splitter
	//m_leftSplitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE );
	m_leftSplitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE );
	m_rightSplitter =  new wxSplitterWindow( m_leftSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE );
	wxWindow *objectTree      = Title::CreateTitle( CreateObjectTree( m_rightSplitter ), wxT( "Object Tree" ) );
	wxWindow *objectInspector = Title::CreateTitle( CreateObjectInspector( m_rightSplitter ), wxT( "Object Properties" ) );

	// panel1 contains palette and designer
	wxPanel *panel1 = new wxPanel( m_leftSplitter, wxID_ANY);

	wxWindow *palette = Title::CreateTitle( CreateComponentPalette( panel1 ), wxT( "Component Palette" ) );
	wxWindow *designer = Title::CreateTitle( CreateDesignerWindow( panel1 ), wxT( "Editor" ) );

	wxBoxSizer *panel1_sizer = new wxBoxSizer( wxVERTICAL );
	panel1_sizer->Add( palette, 0, wxEXPAND );
	panel1_sizer->Add( designer, 1, wxEXPAND );
	panel1->SetSizer( panel1_sizer );

	m_leftSplitter->SplitVertically( m_rightSplitter, panel1, m_leftSplitterWidth );

	// Need to update the left splitter so the right one is drawn correctly
	wxSizeEvent update( GetSize(), GetId() );
	ProcessEvent( update );
	m_leftSplitter->UpdateSize();

	m_rightSplitter->SplitHorizontally( objectTree, objectInspector, m_rightSplitterWidth );
	m_rightSplitter->SetMinimumPaneSize( 2 );

	SetMinSize( wxSize( 700, 465 ) );
}

void MainFrame::OnIdle( wxIdleEvent& )
{
	if ( m_leftSplitter )
	{
		m_leftSplitter->SetSashPosition( m_leftSplitterWidth );
	}

	if ( m_rightSplitter )
	{
		m_rightSplitter->SetSashPosition( m_rightSplitterWidth );
	}

	Disconnect( wxEVT_IDLE, wxIdleEventHandler( MainFrame::OnIdle ) );

	if ( m_autoSash )
	{
		// Init. m_rightSplitter_sash_pos
		m_rightSplitter_sash_pos = m_rightSplitter->GetSashPosition();
		m_rightSplitter->Connect( wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED, wxSplitterEventHandler( MainFrame::OnSplitterChanged ) );
	}
}

void MainFrame::OnSplitterChanged( wxSplitterEvent &event )
{
	LogDebug(wxT("MainFrame::OnSplitterChanged > pos = %d"), event.GetSashPosition());

	// update position
	m_rightSplitter_sash_pos = event.GetSashPosition();
}

void MainFrame::OnWindowSwap(wxCommandEvent&)
{
    wxWindow* win1 = m_rightSplitter->GetWindow1();
    wxWindow* win2 = m_rightSplitter->GetWindow2();

    int pos = m_rightSplitter->GetSashPosition();
    wxSize sz = m_rightSplitter->GetClientSize();

    m_rightSplitter->Unsplit();
    m_rightSplitter->SplitVertically(win2, win1);

    m_rightSplitter->SetSashPosition(sz.GetWidth() - pos);
}

void MainFrame::OnFindComponent(wxCommandEvent& WXUNUSED(event))
{
    DialogFindComponent dlg(this);
    if (dlg.ShowModal() == wxID_OK) {
        AppData()->CreateObject(dlg.GetSelected());
    }
}

void MainFrame::OnMenuComponentsClick( wxCommandEvent &e )
{
    wxMenu      *menu = static_cast<wxMenu*>( e.GetEventObject() );
    wxMenuItem  *item = menu->FindChildItem( e.GetId() );

    AppData()->CreateObject( item->GetItemLabelText() );
}
