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
#include "mainframe.h"

#include "wx/config.h"
#include "utils/debug.h"
#include "utils/typeconv.h"
#include "rad/title.h"
#include "rad/bitmaps.h"
#include "rad/cpppanel/cpppanel.h"
#include "rad/xrcpanel/xrcpanel.h"
#include "rad/geninheritclass/geninhertclass.h"
#include "inspector/objinspect2.h"
#include "objecttree/objecttree.h"
#include "palette.h"
#include "rad/designer/visualeditor.h"

#include "model/xrcfilter.h"
#include "rad/about.h"
#include "rad/wxfbevent.h"
#include "wxfbmanager.h"
#include "utils/wxfbexception.h"
#include "utils/stringutils.h"
#include <wx/filename.h>

#include <rad/appdata.h>
#include "model/objectbase.h"

#define ID_ABOUT         100
#define ID_QUIT          101
#define ID_SAVE_PRJ      102
#define ID_OPEN_PRJ      103
#define ID_NEW_PRJ       104
#define ID_GENERATE_CODE 105
#define ID_IMPORT_XRC    106
#define ID_UNDO          107
#define ID_REDO          108
#define ID_SAVE_AS_PRJ   109
#define ID_CUT           110
#define ID_DELETE        111
#define ID_COPY          112
#define ID_PASTE         113
#define ID_EXPAND        114
#define ID_STRETCH       115
#define ID_MOVE_UP       116
#define ID_MOVE_DOWN     117
#define ID_RECENT_0      118 // Tienen que tener ids consecutivos
#define ID_RECENT_1      119 // ID_RECENT_n+1 == ID_RECENT_n + 1
#define ID_RECENT_2      120 //
#define ID_RECENT_3      121 //
#define ID_RECENT_SEP    122

#define ID_ALIGN_LEFT     123
#define ID_ALIGN_CENTER_H 124
#define ID_ALIGN_RIGHT    125
#define ID_ALIGN_TOP      126
#define ID_ALIGN_CENTER_V 127
#define ID_ALIGN_BOTTOM   128

#define ID_BORDER_LEFT    129
#define ID_BORDER_RIGHT   130
#define ID_BORDER_TOP     131
#define ID_BORDER_BOTTOM  132
#define ID_EDITOR_FNB	  133
#define ID_MOVE_LEFT	  134
#define ID_MOVE_RIGHT     135

#define ID_PREVIEW_XRC     136
#define ID_GEN_INHERIT_CLS 137

#define ID_SETTINGS_GLOBAL 138	// For the future preference dialogs
#define ID_SETTINGS_PROJ   139	// For the future preference dialogs

#define ID_LEFT_SPLITTER   140
#define ID_RIGHT_SPLITTER  141

#define STATUS_FIELD_OBJECT 1

BEGIN_EVENT_TABLE( MainFrame, wxFrame )
EVT_MENU( ID_NEW_PRJ, MainFrame::OnNewProject )
EVT_MENU( ID_SAVE_PRJ, MainFrame::OnSaveProject )
EVT_MENU( ID_SAVE_AS_PRJ, MainFrame::OnSaveAsProject )
EVT_MENU( ID_OPEN_PRJ, MainFrame::OnOpenProject )
EVT_MENU( ID_ABOUT, MainFrame::OnAbout )
EVT_MENU( ID_QUIT, MainFrame::OnExit )
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
EVT_CLOSE( MainFrame::OnClose )
EVT_FLATNOTEBOOK_PAGE_CHANGED( ID_EDITOR_FNB, MainFrame::OnFlatNotebookPageChanged )

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

END_EVENT_TABLE()

MainFrame::MainFrame( wxWindow *parent, int id, int style, wxPoint pos, wxSize size )
		: wxFrame( parent, id, wxEmptyString, pos, size, wxDEFAULT_FRAME_STYLE ),
		m_style( style )
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

	wxString date( wxT( __DATE__ ) );
	wxString time( wxT( __TIME__ ) );
	SetTitle( wxT( "wxFormBuilder (Build on " ) + date + wxT( " - " ) + time + wxT( ")" ) );

	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

	SetMenuBar( CreateFBMenuBar() );
	CreateStatusBar( 3 );
	SetStatusBarPane( 0 );
	int widths[3] = { -1, 300 };
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
};


MainFrame::~MainFrame()
{
	/*m_mgr.UnInit();*/
	AppData()->RemoveHandler( this->GetEventHandler() );
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

void MainFrame::OnSaveAsProject( wxCommandEvent &event )
{
	wxFileDialog *dialog = new wxFileDialog( this, wxT( "Save Project" ), m_currentDir,
	                       wxT( "" ), wxT( "wxFormBuilder Project File (*.fbp)|*.fbp|All files (*.*)|*.*" ), wxSAVE );

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

void MainFrame::OnOpenProject( wxCommandEvent &event )
{
	if ( !SaveWarning() )
		return;

	wxFileDialog *dialog = new wxFileDialog( this, wxT( "Open Project" ), m_currentDir,
	                       wxT( "" ), wxT( "wxFormBuilder Project File (*.fbp)|*.fbp|All files (*.*)|*.*" ), wxOPEN );

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

void MainFrame::OnImportXrc( wxCommandEvent &event )
{
	wxFileDialog *dialog = new wxFileDialog( this, wxT( "Import XRC file" ), m_currentDir,
	                       wxT( "example.xrc" ), wxT( "*.xrc" ), wxOPEN );

	if ( dialog->ShowModal() == wxID_OK )
	{
		m_currentDir = dialog->GetDirectory();

		try
		{
			TiXmlDocument doc;
			XMLUtils::LoadXMLFile( doc, dialog->GetPath() );

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


void MainFrame::OnNewProject( wxCommandEvent &event )
{
	if ( !SaveWarning() )
		return;

	AppData()->NewProject();
}

void MainFrame::OnGenerateCode( wxCommandEvent &event )
{
	AppData()->GenerateCode( false );
}

void MainFrame::OnAbout( wxCommandEvent &event )
{
	AboutDialog *dlg = new AboutDialog( this );
	dlg->ShowModal();
	dlg->Destroy();
}

void MainFrame::OnExit( wxCommandEvent &event )
{
	if ( !SaveWarning() )
		return;

	Close();
}

void MainFrame::OnClose( wxCloseEvent &event )
{
	if ( !SaveWarning() )
		return;

	SavePosition( wxT( "mainframe" ) );

	event.Skip();
}

void MainFrame::OnProjectLoaded( wxFBEvent& event )
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

void MainFrame::OnProjectSaved( wxFBEvent& event )
{
	GetStatusBar()->SetStatusText( wxT( "Project Saved!" ) );
	UpdateFrame();
}

void MainFrame::OnObjectExpanded( wxFBObjectEvent& event )
{
	UpdateFrame();
}

void MainFrame::OnObjectSelected( wxFBObjectEvent& event )
{
	PObjectBase obj = event.GetFBObject();

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

	if ( event.GetFBObject() )
	{
		message.Printf( wxT( "Object '%s' of class '%s' created." ),
		                event.GetFBObject()->GetPropertyAsString( wxT( "name" ) ).c_str(),
		                event.GetFBObject()->GetClassName().c_str() );
	}
	else
		message = wxT( "Impossible to create the object. Did you forget to add a sizer?" );

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

void MainFrame::OnProjectRefresh( wxFBEvent& event )
{
	PObjectBase project = AppData()->GetProjectData();

	if ( project )
	{
		wxString objDetails = wxString::Format( wxT( "Name: %s | Class: %s" ), project->GetPropertyAsString( wxT( "name" ) ).c_str(), project->GetClassName().c_str() );
		GetStatusBar()->SetStatusText( objDetails, STATUS_FIELD_OBJECT );
	}

	UpdateFrame();
}

void MainFrame::OnUndo( wxCommandEvent &event )
{
	AppData()->Undo();
}

void MainFrame::OnRedo( wxCommandEvent &event )
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

	// Enable the layout tools if there are layout settings, else disable the tools
	toolbar->EnableTool( ID_EXPAND, gotLayoutSettings );
	toolbar->EnableTool( ID_STRETCH, option >= 0 );

	bool enableHorizontalTools = ( orient != wxHORIZONTAL ) && gotLayoutSettings;
	toolbar->EnableTool( ID_ALIGN_LEFT, enableHorizontalTools );
	toolbar->EnableTool( ID_ALIGN_CENTER_H, enableHorizontalTools );
	toolbar->EnableTool( ID_ALIGN_RIGHT, enableHorizontalTools );

	bool enableVerticalTools = ( orient != wxVERTICAL ) && gotLayoutSettings;
	toolbar->EnableTool( ID_ALIGN_TOP, enableVerticalTools );
	toolbar->EnableTool( ID_ALIGN_CENTER_V, enableVerticalTools );
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
	// Actualizamos el título
	wxString date( wxT( __DATE__ ) );
	wxString time( wxT( __TIME__ ) );
	wxString title( wxT( "wxFormBuilder (Build on " ) + date + wxT( " - " ) + time + wxT( ") - " ) );

	if ( AppData()->IsModified() )
		title = title + wxChar( '*' );

	wxString filename = AppData()->GetProjectFileName();

	title = title + ( filename.IsEmpty() ?
	                  wxT( "[untitled]" ) :
	                  wxT( "[" ) + filename + wxT( "]" ) );

	SetTitle( title );

	// Actualizamos los menus
	wxMenu *menuEdit = GetMenuBar()->GetMenu( GetMenuBar()->FindMenu( wxT( "Edit" ) ) );

	menuEdit->Enable( ID_REDO, AppData()->CanRedo() );

	menuEdit->Enable( ID_UNDO, AppData()->CanUndo() );

	// Actualizamos la barra de herramientas
	GetToolBar()->EnableTool( ID_REDO, AppData()->CanRedo() );

	GetToolBar()->EnableTool( ID_UNDO, AppData()->CanUndo() );

	GetToolBar()->EnableTool( ID_COPY, AppData()->CanCopyObject() );

	GetToolBar()->EnableTool( ID_CUT, AppData()->CanCopyObject() );

	GetToolBar()->EnableTool( ID_DELETE, AppData()->CanCopyObject() );

	GetToolBar()->EnableTool( ID_PASTE, AppData()->CanPasteObject() );

	UpdateLayoutTools();
}

void MainFrame::UpdateRecentProjects()
{
	int i, fi;
	wxMenu *menuFile = GetMenuBar()->GetMenu( GetMenuBar()->FindMenu( wxT( "File" ) ) );

	// borramos los items del menu de los projectos recientes

	for ( i = 0 ; i < 4 ; i++ )
	{
		if ( menuFile->FindItem( ID_RECENT_0 + i ) )
			menuFile->Destroy( ID_RECENT_0 + i );
	}

	// remove empty filenames and 'compress' the rest
    fi = 0;
	for ( i = 0 ; i < 4 ; i++ )
	{
	    if(!m_recentProjects[i].IsEmpty())
	        m_recentProjects[fi++] = m_recentProjects[i];
	}
	for ( i = fi ; i < 4 ; i++ )
        m_recentProjects[i] = wxT("");

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

void MainFrame::OnCopy( wxCommandEvent &event )

{
	wxWindow *focusedWindow = wxWindow::FindFocus();

	if ( focusedWindow != NULL && focusedWindow->IsKindOf( CLASSINFO( wxScintilla ) ) )
		( ( wxScintilla* )focusedWindow )->Copy();
	else
	{
		AppData()->CopyObject( AppData()->GetSelectedObject() );
		UpdateFrame();
	}
}

void MainFrame::OnCut ( wxCommandEvent &event )
{
	wxWindow *focusedWindow = wxWindow::FindFocus();

	if ( focusedWindow != NULL && focusedWindow->IsKindOf( CLASSINFO( wxScintilla ) ) )
		( ( wxScintilla* )focusedWindow )->Cut();
	else
	{
		AppData()->CutObject( AppData()->GetSelectedObject() );
		UpdateFrame();
	}
}

void MainFrame::OnDelete ( wxCommandEvent &event )
{
	AppData()->RemoveObject( AppData()->GetSelectedObject() );
	UpdateFrame();
}

void MainFrame::OnPaste ( wxCommandEvent &event )
{
	wxWindow *focusedWindow = wxWindow::FindFocus();

	if ( focusedWindow != NULL && focusedWindow->IsKindOf( CLASSINFO( wxScintilla ) ) )
		( ( wxScintilla* )focusedWindow )->Paste();
	else
	{
		AppData()->PasteObject( AppData()->GetSelectedObject() );
		UpdateFrame();
	}
}

void MainFrame::OnToggleExpand ( wxCommandEvent &event )
{
	AppData()->ToggleExpandLayout( AppData()->GetSelectedObject() );
}

void MainFrame::OnToggleStretch ( wxCommandEvent &event )
{
	AppData()->ToggleStretchLayout( AppData()->GetSelectedObject() );
}

void MainFrame::OnMoveUp ( wxCommandEvent &event )
{
	AppData()->MovePosition( AppData()->GetSelectedObject(), false, 1 );
}

void MainFrame::OnMoveDown ( wxCommandEvent &event )
{
	AppData()->MovePosition( AppData()->GetSelectedObject(), true, 1 );
}

void MainFrame::OnMoveLeft ( wxCommandEvent &event )
{
	AppData()->MoveHierarchy( AppData()->GetSelectedObject(), true );
}

void MainFrame::OnMoveRight ( wxCommandEvent &event )
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
	wxArrayString forms;
	std::map< wxString, wxString > types; // base class name : type of class

	PObjectBase project = AppData()->GetProjectData();

	for ( unsigned int i = 0; i < project->GetChildCount(); ++i )
	{
		// Children of project are all Forms
		PObjectBase child = project->GetChild( i );
		wxString form = child->GetPropertyAsString( _( "name" ) );
		forms.Add( form );
		types[form] = child->GetClassName();;
	}


	// Show the dialog
	GenInheritedClassDlg dlg( this, forms, project->GetPropertyAsString( _( "name" ) ) );

	if ( wxID_OK != dlg.ShowModal() )
	{
		return;
	}

	wxArrayString selectedForms = dlg.GetFormsSelected();

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

	for ( size_t i = 0; i < selectedForms.size(); ++i )
	{
		wxString type;

		// Get the class name for the selected form.
		wxString className = dlg.GetClassName( selectedForms[i] );

		// Find the type of the form that is selected.
		std::map< wxString, wxString >::iterator it = types.find( selectedForms[i] );

		if ( it != types.end() )
		{
			type = it->second;
		}

		// Get the file name for the selected form.
		wxString fileName = dlg.GetFileName( selectedForms[i] );

		// Create the class and files.
		AppData()->GenerateInheritedClass( selectedForms[i], className, type, filePath, fileName );
	}

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

void MainFrame::OnFlatNotebookPageChanged( wxFlatNotebookEvent& event )
{
	AppData()->GenerateCode( true );
}

/////////////////////////////////////////////////////////////////////////////

wxMenuBar * MainFrame::CreateFBMenuBar()
{
	wxMenu *menuFile = new wxMenu;
	menuFile->Append( ID_NEW_PRJ, wxT( "&New Project" ), wxT( "Create an empty project" ) );
	menuFile->Append( ID_OPEN_PRJ, wxT( "&Open...\tF2" ), wxT( "Open a project" ) );

	menuFile->Append( ID_SAVE_PRJ,          wxT( "&Save\tCtrl+S" ), wxT( "Save current project" ) );
	menuFile->Append( ID_SAVE_AS_PRJ, wxT( "Save &As...\tF3" ), wxT( "Save current project as..." ) );
	menuFile->AppendSeparator();
	menuFile->Append( ID_IMPORT_XRC, wxT( "&Import XRC..." ), wxT( "Import XRC file" ) );
	menuFile->AppendSeparator();
	menuFile->Append( ID_GENERATE_CODE, wxT( "&Generate Code\tF8" ), wxT( "Generate Code" ) );
	menuFile->AppendSeparator();
	menuFile->Append( ID_QUIT, wxT( "E&xit\tAlt-F4" ), wxT( "Quit wxFormBuilder" ) );
	menuFile->AppendSeparator();

	wxMenu *menuEdit = new wxMenu;
	menuEdit->Append( ID_UNDO, wxT( "&Undo \tCtrl+Z" ), wxT( "Undo changes" ) );
	menuEdit->Append( ID_REDO, wxT( "&Redo \tCtrl+Y" ), wxT( "Redo changes" ) );
	menuEdit->AppendSeparator();
	menuEdit->Append( ID_COPY, wxT( "&Copy \tCtrl+C" ), wxT( "Copy selected object" ) );
	menuEdit->Append( ID_CUT, wxT( "&Cut \tCtrl+X" ), wxT( "Cut selected object" ) );
	menuEdit->Append( ID_PASTE, wxT( "&Paste \tCtrl+V" ), wxT( "Paste on selected object" ) );
	menuEdit->Append( ID_DELETE, wxT( "&Delete \tCtrl+D" ), wxT( "Delete selected object" ) );
	menuEdit->AppendSeparator();
	menuEdit->Append( ID_EXPAND, wxT( "&Toggle Expand\tAlt+W" ), wxT( "Toggle wxEXPAND flag of sizeritem properties" ) );
	menuEdit->Append( ID_STRETCH, wxT( "&Toggle Stretch\tAlt+S" ), wxT( "Toggle option property of sizeritem properties" ) );
	menuEdit->Append( ID_MOVE_UP, wxT( "&Move Up\tAlt+Up" ), wxT( "Move Up selected object" ) );
	menuEdit->Append( ID_MOVE_DOWN, wxT( "&Move Down\tAlt+Down" ), wxT( "Move Down selected object" ) );
	menuEdit->Append( ID_MOVE_LEFT, wxT( "&Move Left\tAlt+Left" ), wxT( "Move Left selected object" ) );
	menuEdit->Append( ID_MOVE_RIGHT, wxT( "&Move Right\tAlt+Right" ), wxT( "Move Right selected object" ) );
	menuEdit->AppendSeparator();
	menuEdit->Append( ID_ALIGN_LEFT,     wxT( "&Align Left" ),           wxT( "Align item to the left" ) );
	menuEdit->Append( ID_ALIGN_CENTER_H, wxT( "&Align Center Horizontal" ), wxT( "Align item to the center horizontally" ) );
	menuEdit->Append( ID_ALIGN_RIGHT,    wxT( "&Align Right" ),         wxT( "Align item to the right" ) );
	menuEdit->Append( ID_ALIGN_TOP,      wxT( "&Align Top" ),              wxT( "Align item to the top" ) );
	menuEdit->Append( ID_ALIGN_CENTER_H, wxT( "&Align Center Vertical" ),   wxT( "Align item to the center vertically" ) );
	menuEdit->Append( ID_ALIGN_BOTTOM,   wxT( "&Align Bottom" ),         wxT( "Align item to the bottom" ) );

	wxMenu *menuView = new wxMenu;
	menuView->Append( ID_PREVIEW_XRC, wxT( "&XRC Window\tF5" ), wxT( "Show a preview of the XRC window" ) );

	wxMenu *menuTools = new wxMenu;
	menuTools->Append( ID_GEN_INHERIT_CLS, wxT( "&Generate Inherited Class\tF6" ), wxT( "Creates the needed files and class for proper inheritance of your designed GUI" ) );

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append( ID_ABOUT, wxT( "&About...\tF1" ), wxT( "Show about dialog" ) );


	// now append the freshly created menu to the menu bar...
	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append( menuFile, wxT( "&File" ) );
	menuBar->Append( menuEdit, wxT( "&Edit" ) );
	menuBar->Append( menuView, wxT( "&View" ) );
	menuBar->Append( menuTools, wxT( "&Tools" ) );
	menuBar->Append( menuHelp, wxT( "&Help" ) );

	return menuBar;
}

wxToolBar * MainFrame::CreateFBToolBar()
{
	wxToolBar* toolbar = CreateToolBar();
	toolbar->SetToolBitmapSize( wxSize( TOOL_SIZE, TOOL_SIZE ) );
	toolbar->AddTool( ID_NEW_PRJ, wxT( "New Project" ), AppBitmaps::GetBitmap( wxT( "new" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "New Project" ), wxT( "Start a new project." ) );
	toolbar->AddTool( ID_OPEN_PRJ, wxT( "Open Project" ), AppBitmaps::GetBitmap( wxT( "open" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Open Project" ), wxT( "Open an existing project." ) );
	toolbar->AddTool( ID_SAVE_PRJ, wxT( "Save Project" ), AppBitmaps::GetBitmap( wxT( "save" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Save Project" ), wxT( "Save the current project." ) );
	toolbar->AddSeparator();
	toolbar->AddTool( ID_UNDO, wxT( "Undo" ), AppBitmaps::GetBitmap( wxT( "undo" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Undo" ), wxT( "Undo the last action." ) );
	toolbar->AddTool( ID_REDO, wxT( "Redo" ), AppBitmaps::GetBitmap( wxT( "redo" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Redo" ), wxT( "Redo the last action that was undone." ) );
	toolbar->AddSeparator();
	toolbar->AddTool( ID_CUT, wxT( "Cut" ), AppBitmaps::GetBitmap( wxT( "cut" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Cut" ), wxT( "Remove the selected object and place it on the clipboard." ) );
	toolbar->AddTool( ID_COPY, wxT( "Copy" ), AppBitmaps::GetBitmap( wxT( "copy" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Copy" ), wxT( "Copy the selected object to the clipboard." ) );
	toolbar->AddTool( ID_PASTE, wxT( "Paste" ), AppBitmaps::GetBitmap( wxT( "paste" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Paste" ), wxT( "Insert an object from the clipboard." ) );
	toolbar->AddTool( ID_DELETE, wxT( "Delete" ), AppBitmaps::GetBitmap( wxT( "delete" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Delete" ), wxT( "Remove the selected object." ) );
	toolbar->AddSeparator();
	toolbar->AddTool( ID_GENERATE_CODE, wxT( "Generate Code" ), AppBitmaps::GetBitmap( wxT( "generate" ), TOOL_SIZE ), wxNullBitmap, wxITEM_NORMAL, wxT( "Generate Code" ), wxT( "Create code from the current project." ) );
	toolbar->AddSeparator();
	toolbar->AddTool( ID_ALIGN_LEFT, wxT( "" ), AppBitmaps::GetBitmap( wxT( "lalign" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Align Left" ), wxT( "The item will be aligned to the left of the space alotted to it by the sizer." ) );
	toolbar->AddTool( ID_ALIGN_CENTER_H, wxT( "" ), AppBitmaps::GetBitmap( wxT( "chalign" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Align Center Horizontally" ), wxT( "The item will be centered horizontally in the space alotted to it by the sizer." ) );
	toolbar->AddTool( ID_ALIGN_RIGHT, wxT( "" ), AppBitmaps::GetBitmap( wxT( "ralign" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Align Right" ), wxT( "The item will be aligned to the right of the space alotted to it by the sizer." ) );
	toolbar->AddSeparator();
	toolbar->AddTool( ID_ALIGN_TOP, wxT( "" ), AppBitmaps::GetBitmap( wxT( "talign" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Align Top" ), wxT( "The item will be aligned to the top of the space alotted to it by the sizer." ) );
	toolbar->AddTool( ID_ALIGN_CENTER_V, wxT( "" ), AppBitmaps::GetBitmap( wxT( "cvalign" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Align Center Vertically" ), wxT( "The item will be centered vertically within space alotted to it by the sizer." ) );
	toolbar->AddTool( ID_ALIGN_BOTTOM, wxT( "" ), AppBitmaps::GetBitmap( wxT( "balign" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Align Bottom" ), wxT( "The item will be aligned to the bottom of the space alotted to it by the sizer." ) );
	toolbar->AddSeparator();
	toolbar->AddTool( ID_EXPAND, wxT( "" ), AppBitmaps::GetBitmap( wxT( "expand" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Expand" ), wxT( "The item will be expanded to fill the space assigned to the item." ) );
	toolbar->AddTool( ID_STRETCH, wxT( "" ), AppBitmaps::GetBitmap( wxT( "stretch" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Stretch" ), wxT( "The item will grow and shrink with the sizer." ) );
	toolbar->AddSeparator();
	toolbar->AddTool( ID_BORDER_LEFT, wxT( "" ), AppBitmaps::GetBitmap( wxT( "left" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Left Border" ), wxT( "A border will be added on the left side of the item." ) );
	toolbar->AddTool( ID_BORDER_RIGHT, wxT( "" ), AppBitmaps::GetBitmap( wxT( "right" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Right Border" ), wxT( "A border will be  added on the right side of the item." ) );
	toolbar->AddTool( ID_BORDER_TOP, wxT( "" ), AppBitmaps::GetBitmap( wxT( "top" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Top Border" ), wxT( "A border will be  added on the top of the item." ) );
	toolbar->AddTool( ID_BORDER_BOTTOM, wxT( "" ), AppBitmaps::GetBitmap( wxT( "bottom" ), TOOL_SIZE ), wxNullBitmap, wxITEM_CHECK, wxT( "Bottom Border" ), wxT( "A border will be  added on the bottom of the item." ) );

	toolbar->Realize();

	return toolbar;
}

wxWindow * MainFrame::CreateDesignerWindow( wxWindow *parent )
{
	m_notebook = new wxFlatNotebook( parent, ID_EDITOR_FNB, wxDefaultPosition, wxDefaultSize, wxFNB_NO_X_BUTTON | wxFNB_NO_NAV_BUTTONS | wxFNB_NODRAG );

	// Set notebook icons
	m_icons.Add( AppBitmaps::GetBitmap( wxT( "designer" ), 16 ) );
	m_icons.Add( AppBitmaps::GetBitmap( wxT( "c++" ), 16 ) );
	m_icons.Add( AppBitmaps::GetBitmap( wxT( "xrc" ), 16 ) );
	m_notebook->SetImageList( &m_icons );

	m_visualEdit = new VisualEditor( m_notebook );
	AppData()->GetManager()->SetVisualEditor( m_visualEdit );

	m_notebook->AddPage( m_visualEdit, wxT( "Designer" ), false, 0 );

	m_cpp = new CppPanel( m_notebook, -1 );
	m_notebook->AddPage( m_cpp, wxT( "C++" ), false, 1 );

	m_xrc = new XrcPanel( m_notebook, -1 );
	m_notebook->AddPage( m_xrc, wxT( "XRC" ), false, 2 );

	return m_notebook;
}

wxWindow * MainFrame::CreateComponentPalette ( wxWindow *parent )
{
	// la paleta de componentes, no es un observador propiamente dicho, ya
	// que no responde ante los eventos de la aplicación
	m_palette = new wxFbPalette( parent, -1 );
	m_palette->Create();
	m_palette->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );

	return m_palette;
}

wxWindow * MainFrame::CreateObjectTree( wxWindow *parent )
{
	m_objTree = new ObjectTree( parent, -1 );
	m_objTree->Create();

	return m_objTree;
}

wxWindow * MainFrame::CreateObjectInspector( wxWindow *parent )
{
	//TO-DO: make object inspector style selectable.
	int style = ( m_style == wxFB_CLASSIC_GUI ? wxFB_OI_MULTIPAGE_STYLE : wxFB_OI_SINGLE_PAGE_STYLE );
	m_objInsp = new ObjectInspector( parent, -1, style );
	return m_objInsp;
}

void MainFrame::CreateWideGui()
{
	m_leftSplitter = new wxSplitterWindow( this, ID_LEFT_SPLITTER, wxDefaultPosition, wxDefaultSize, 0 );

	wxWindow *objectTree = Title::CreateTitle( CreateObjectTree( m_leftSplitter ), wxT( "Object Tree" ) );

	// panel1 contains Palette and splitter2
	wxPanel *panel1 = new wxPanel( m_leftSplitter, -1 );

	wxWindow *palette = Title::CreateTitle( CreateComponentPalette( panel1 ), wxT( "Component Palette" ) );
	m_rightSplitter   =  new wxSplitterWindow( panel1, ID_RIGHT_SPLITTER, wxDefaultPosition, wxDefaultSize, 0 );

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

	m_rightSplitter->SplitVertically( designer, objectInspector, m_rightSplitterWidth );
	m_rightSplitter->SetSashGravity( 1 );
	m_rightSplitter->SetMinimumPaneSize( 2 );

	m_style = wxFB_WIDE_GUI;

	SetMinSize( wxSize( 700, 380 ) );
}

void MainFrame::CreateClassicGui()
{
	m_leftSplitter = new wxSplitterWindow( this, -1, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE );

	m_rightSplitter =  new wxSplitterWindow( m_leftSplitter, -1, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE );
	wxWindow *objectTree      = Title::CreateTitle( CreateObjectTree( m_rightSplitter ), wxT( "Object Tree" ) );
	wxWindow *objectInspector = Title::CreateTitle( CreateObjectInspector( m_rightSplitter ), wxT( "Object Properties" ) );

	// panel1 contains palette and designer
	wxPanel *panel1 = new wxPanel( m_leftSplitter, -1 );

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
