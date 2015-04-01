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

#include "visualeditor.h"

#include "visualeditor.h"
#include "visualobj.h"
#include "utils/typeconv.h"
#include "utils/debug.h"
#include "menubar.h"
#include "wx/statline.h"
#include "rad/designer/resizablepanel.h"
#include "rad/wxfbevent.h"
#include "rad/wxfbmanager.h"
#include <rad/appdata.h>
#include "utils/wxfbexception.h"
#include "model/objectbase.h"

#ifdef __WX24__
#define wxFULL_REPAINT_ON_RESIZE 0
#endif

static const int ID_TIMER_SCAN = wxNewId();

BEGIN_EVENT_TABLE(VisualEditor,wxScrolledWindow)
	//EVT_SASH_DRAGGED(-1, VisualEditor::OnResizeBackPanel)
	//EVT_COMMAND(-1, wxEVT_PANEL_RESIZED, VisualEditor::OnResizeBackPanel)
	EVT_INNER_FRAME_RESIZED(wxID_ANY, VisualEditor::OnResizeBackPanel)

	EVT_FB_PROJECT_LOADED( VisualEditor::OnProjectLoaded )
	EVT_FB_PROJECT_SAVED( VisualEditor::OnProjectSaved )
	EVT_FB_OBJECT_SELECTED( VisualEditor::OnObjectSelected )
	EVT_FB_OBJECT_CREATED( VisualEditor::OnObjectCreated )
	EVT_FB_OBJECT_REMOVED( VisualEditor::OnObjectRemoved )
	EVT_FB_PROPERTY_MODIFIED( VisualEditor::OnPropertyModified )
	EVT_FB_PROJECT_REFRESH( VisualEditor::OnProjectRefresh )
	EVT_FB_CODE_GENERATION( VisualEditor::OnProjectRefresh )

	EVT_TIMER( ID_TIMER_SCAN, VisualEditor::OnAuiScaner )

END_EVENT_TABLE()

VisualEditor::VisualEditor(wxWindow *parent)
:
wxScrolledWindow(parent,-1,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER),
m_stopSelectedEvent( false ),
m_stopModifiedEvent( false )
{
	SetExtraStyle( wxWS_EX_BLOCK_EVENTS );

	AppData()->AddHandler( this->GetEventHandler() );

	#ifdef __WXMSW__
		SetOwnBackgroundColour(wxColour(150,150,150));
	#else
		SetOwnBackgroundColour(wxColour(192,192,192));
	#endif

	SetScrollRate(5, 5);

	m_back = new DesignerWindow( this, wxID_ANY, wxPoint(10,10) );
	m_back->GetEventHandler()->Connect( wxID_ANY, wxEVT_LEFT_DOWN, wxMouseEventHandler( VisualEditor::OnClickBackPanel ), NULL, this );

	m_auimgr = NULL;
	m_auipanel = NULL;

	// scan aui panes in run-time
	m_AuiScaner.SetOwner( this, ID_TIMER_SCAN );
	m_AuiScaner.Start( 200 );

    m_wizard = NULL;
}

void VisualEditor::DeleteAbstractObjects()
{
	wxObjectMap::iterator it;
	for ( it = m_wxobjects.begin(); it != m_wxobjects.end(); ++it )
	{
	    // The abstract objects are stored as wxNoObject*'s
		wxNoObject* noobject = dynamic_cast< wxNoObject* >( it->first );
		if ( noobject != 0 )
		{
		    delete noobject;
		}
		else
		{
			//TODO: fix wxStaticBox later...
			/*if( wxStaticBoxSizer* staticBoxSizer = dynamic_cast< wxStaticBoxSizer* >( it->first ) )
			{
				if( wxWindow* window = staticBoxSizer->GetStaticBox() )
					window->PopEventHandler( true );
			}*/
            // Delete push'd visual object event handlers
            wxWindow* window = dynamic_cast< wxWindow* > ( it->first );
            if ( window != 0 )
            {
                window->PopEventHandler( true );
            }
		}
	}
}

VisualEditor::~VisualEditor()
{
	m_AuiScaner.Stop();

	AppData()->RemoveHandler( this->GetEventHandler() );
	DeleteAbstractObjects();

	ClearAui();
    ClearWizard();
	ClearComponents( m_back->GetFrameContentPanel() );
}

void VisualEditor::UpdateVirtualSize()
{
	int w, h, panelW, panelH;
	GetVirtualSize(&w, &h);
	m_back->GetSize(&panelW, &panelH);
	panelW += 20; panelH += 20;
	if (panelW != w || panelH != h) SetVirtualSize(panelW, panelH);
}

void VisualEditor::OnClickBackPanel( wxMouseEvent& event )
{
	if ( m_form )
	{
		AppData()->SelectObject(m_form);
	}
	event.Skip();

#if wxVERSION_NUMBER < 2900
		LogDebug( wxT( "event: %s" ), wxT("OnClickBackPanel") );
#else
		//LogDebug( "event: " + wxString("OnClickBackPanel") );
#endif
}

void VisualEditor::OnResizeBackPanel (wxCommandEvent &) //(wxSashEvent &event)
{
	/*wxRect rect(event.GetDragRect());
	LogDebug("VisualEditor::OnResizeBackPanel [%d,%d,%d,%d]",rect.x,rect.y,rect.width, rect.height);
	m_back->SetSize(rect.width,rect.height);
	m_back->Layout();*/
#if wxVERSION_NUMBER < 2900
		LogDebug( wxT( "event: %s" ), wxT("OnResizeBackPanel") );
#else
		LogDebug( "event: " + wxString("OnResizeBackPanel") );
#endif

	PObjectBase form (AppData()->GetSelectedForm());

	if (form)
	{
		PProperty prop(form->GetProperty( wxT("size") ));
		if (prop)
		{
			wxString value(TypeConv::PointToString(wxPoint(m_back->GetSize().x, m_back->GetSize().y)));
			AppData()->ModifyProperty(prop, value);
		}
	}

	/*event.Skip();*/
}

void VisualEditor::OnWizardPageChanged( WizardEvent &event )
{
    int i = event.GetInt();
    if ( !i )
    {
        AppData()->GetManager()->SelectObject( event.GetPage() );
        return;
    }
    event.Skip();
}

PObjectBase VisualEditor::GetObjectBase( wxObject* wxobject )
{
	if ( NULL == wxobject )
	{
		wxLogError( _("wxObject was NULL!") );
		return PObjectBase();
	}

	wxObjectMap::iterator obj = m_wxobjects.find( wxobject );
	if ( obj != m_wxobjects.end() )
	{
		return obj->second;
	}
	else
	{
		wxLogError( _("No corresponding ObjectBase for wxObject. Name: %s"), wxobject->GetClassInfo()->GetClassName() );
		return PObjectBase();
	}
}

wxObject* VisualEditor::GetWxObject( PObjectBase baseobject )
{
	if ( !baseobject )
	{
		wxLogError( _("baseobject was NULL!") );
		return NULL;
	}

	ObjectBaseMap::iterator obj = m_baseobjects.find( baseobject.get() );
	if ( obj != m_baseobjects.end() )
	{
		return obj->second;
	}
	else
	{
		wxLogError( _("No corresponding wxObject for ObjectBase. Name: %s"), baseobject->GetClassName().c_str() );
		return NULL;
	}
}

void VisualEditor::ScanPanes( wxWindow* parent)
{
	bool updateNeeded;

	wxLogNull stopTheLogging;
	const wxWindowList& children = parent->GetChildren();
	for ( wxWindowList::const_reverse_iterator child = children.rbegin(); child != children.rend(); ++child )
	{
		ScanPanes(*child);

		PObjectBase obj = GetObjectBase( *child );

		if ( obj )
		{
			updateNeeded = false;

			PObjectInfo obj_info = obj->GetObjectInfo();
			wxString cname = obj_info->GetObjectType()->GetName();

			if( cname == wxT("widget") ||
				cname == wxT("expanded_widget") ||
				cname == wxT("ribbonbar") ||
				cname == wxT("propgrid") ||
				cname == wxT("propgridman") ||
				cname == wxT("dataviewctrl") ||
				cname == wxT("dataviewtreectrl") ||
				cname == wxT("dataviewlistctrl") ||
				cname == wxT("toolbar") ||
				cname == wxT("container") )
			{
				wxAuiPaneInfo inf = m_auimgr->GetPane(*child);
				if(inf.IsOk())
				{
					// scan position and docking mode
					if( !obj->GetPropertyAsInteger( wxT("center_pane") ) )
					{
						wxString dock;
						if( inf.IsDocked())
						{
							wxString dockDir;
							switch(inf.dock_direction)
							{
								case 1:
									dockDir = wxT("Top");
									break;

								case 2:
									dockDir = wxT("Right");
									break;

								case 3:
									dockDir = wxT("Bottom");
									break;

								case 4:
									dockDir = wxT("Left");
									break;

								case 5:
									dockDir = wxT("Center");
									break;

								default:
									dockDir = wxT("Left");
									break;
							}
							PProperty pdock = obj->GetProperty( wxT("docking") );

							if( pdock->GetValue() != dockDir )
							{
								pdock->SetValue( dockDir );
								updateNeeded = true;
							}

							dock = wxT("Dock");
						}
						else
						{
							// scan "floating position"
							wxPoint pos = inf.floating_pos;
							if ( pos.x != -1 && pos.y != -1 )
							{
								PProperty pposition = obj->GetProperty( wxT("pane_position") );
								if( pposition->GetValue() != TypeConv::PointToString( pos ) )
								{
									pposition->SetValue( TypeConv::PointToString( pos ) );
									updateNeeded = true;
								}
							}

							// scan "floating size"
							wxSize paneSize = inf.floating_size;
							if ( paneSize.x != -1 && paneSize.y != -1 )
							{
								PProperty psize = obj->GetProperty( wxT("pane_size") );

								if( psize->GetValue() != TypeConv::SizeToString( paneSize ) )
								{
									psize->SetValue( TypeConv::SizeToString( paneSize )  );
									obj->GetProperty( wxT("resize") )->SetValue( wxT("Resizable") );

									updateNeeded = true;
								}
							}

							dock = wxT("Float");
						}
						PProperty pfloat = obj->GetProperty(wxT("dock") );
						if( pfloat->GetValue() != dock )
						{
							pfloat->SetValue( dock );
							updateNeeded = true;
						}

						// scan "best size"
						/*wxSize bestSize = inf.best_size;
						if ( bestSize.x != -1 && bestSize.y != -1 )
						{
							PProperty psize = obj->GetProperty( wxT("best_size") );

							if( psize->GetValue() != TypeConv::SizeToString( bestSize ) )
							{
								psize->SetValue( TypeConv::SizeToString( bestSize )  );
								obj->GetProperty( wxT("resize") )->SetValue( wxT("Resizable") );

								updateNeeded = true;
							}
						}*/

						// scan "row" and "layer"
						PProperty prop = obj->GetProperty(wxT("aui_row") );
						if( obj->GetPropertyAsInteger( wxT("aui_row") ) != inf.dock_row )
						{
							prop->SetValue( inf.dock_row );
							updateNeeded = true;
						}
						prop = obj->GetProperty(wxT("aui_layer") );
						if( obj->GetPropertyAsInteger( wxT("aui_layer") ) != inf.dock_layer )
						{
							prop->SetValue( inf.dock_layer );
							updateNeeded = true;
						}
					}

					// scan "show" property
					PProperty pshow = obj->GetProperty(wxT("show") );
					if( obj->GetPropertyAsInteger( wxT("show") ) != (int) inf.IsShown() )
					{
						pshow->SetValue( inf.IsShown() );
						updateNeeded = true;
					}

					if( updateNeeded ) AppData()->SelectObject( obj, true, true );
				}
			}
		}
	}

}

void VisualEditor::ClearAui()
{
	if ( m_auimgr )
	{
		m_auimgr->UnInit();

		delete m_auimgr;
		m_auimgr = NULL;
		m_auipanel = NULL;
	}
}

void VisualEditor::ClearWizard()
{
	if ( m_wizard )
	{
		m_wizard->Disconnect( wxID_ANY, wxFB_EVT_WIZARD_PAGE_CHANGED, WizardEventHandler( VisualEditor::OnWizardPageChanged ) );
        m_wizard->Destroy();
		m_wizard = NULL;
	}
}

void VisualEditor::ClearComponents( wxWindow* parent )
{
    wxLogNull stopTheLogging;
    const wxWindowList& children = parent->GetChildren();
    for ( wxWindowList::const_reverse_iterator child = children.rbegin(); child != children.rend(); ++child )
    {
		ClearComponents( *child );

        PObjectBase obj = GetObjectBase( *child );
        if ( obj )
        {
			PObjectInfo obj_info = obj->GetObjectInfo();
            IComponent* comp = obj_info->GetComponent();
            if ( comp )
            {
				comp->Cleanup( *child );
            }
        }
    }
}

/**
* Crea la vista preliminar borrando la previa.
*/
void VisualEditor::Create()
{
#if !defined(__WXGTK__ )
	if ( IsShown() )
	{
        Freeze();   // Prevent flickering on wx 2.8,
                    // Causes problems on wx 2.9 in wxGTK (e.g. wxNoteBook objects)
	}
#endif
	// Delete objects which had no parent
	DeleteAbstractObjects();

	// Clear selections, delete objects
	m_back->SetSelectedItem(NULL);
	m_back->SetSelectedSizer(NULL);
	m_back->SetSelectedObject(PObjectBase());

	ClearAui();
    ClearWizard();
	ClearComponents( m_back->GetFrameContentPanel() );

	m_back->GetFrameContentPanel()->DestroyChildren();
	m_back->GetFrameContentPanel()->SetSizer( NULL ); // *!*

	// Clear all associations between ObjectBase and wxObjects
	m_wxobjects.clear();
	m_baseobjects.clear();

	if( IsShown() )
	{
		m_form = AppData()->GetSelectedForm();
		if ( m_form )
		{
			m_back->Show(true);

			// --- [1] Configure the size of the form ---------------------------

			// Get size properties
			wxSize minSize( m_form->GetPropertyAsSize( wxT("minimum_size") ) );
			m_back->SetMinSize( minSize );

			wxSize maxSize( m_form->GetPropertyAsSize( wxT("maximum_size") ) );
			m_back->SetMaxSize( maxSize );

			wxSize size( m_form->GetPropertyAsSize( wxT("size") ) );

			// Determine necessary size for back panel
			wxSize backSize = size;
			if ( backSize.GetWidth() < minSize.GetWidth() && backSize.GetWidth() != wxDefaultCoord )
			{
				backSize.SetWidth( minSize.GetWidth() );
			}
			if ( backSize.GetHeight() < minSize.GetHeight() && backSize.GetHeight() != wxDefaultCoord )
			{
				backSize.SetHeight( minSize.GetHeight() );
			}
			if ( backSize.GetWidth() > maxSize.GetWidth() && maxSize.GetWidth() != wxDefaultCoord )
			{
				backSize.SetWidth( maxSize.GetWidth() );
			}
			if ( backSize.GetHeight() > maxSize.GetHeight() && maxSize.GetHeight() != wxDefaultCoord )
			{
				backSize.SetHeight( maxSize.GetHeight() );
			}

			// Modify size property to match
			if ( size != backSize )
			{
				PProperty psize = m_form->GetProperty( wxT("size") );
				if ( psize )
				{
					AppData()->ModifyProperty( psize, TypeConv::SizeToString( backSize ) );
				}
			}

			// --- [2] Set the color of the form -------------------------------
			PProperty background( m_form->GetProperty( wxT("bg") ) );
			if ( background && !background->GetValue().empty() )
			{
				m_back->GetFrameContentPanel()->SetBackgroundColour( TypeConv::StringToColour( background->GetValue() ) );
			}
			else
			{
				if ( m_form->GetClassName() == wxT("Frame") )
				{
					m_back->GetFrameContentPanel()->SetOwnBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) );
				}
				else
				{
					#ifdef __WXGTK__
					wxVisualAttributes attribs = wxToolBar::GetClassDefaultAttributes();
					m_back->GetFrameContentPanel()->SetOwnBackgroundColour( attribs.colBg );
					#else
					m_back->GetFrameContentPanel()->SetOwnBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
					#endif
				}
			}

			// --- [3] Title bar Setup
			if (  m_form->GetClassName() == wxT("Frame")  ||
                  m_form->GetClassName() == wxT("Dialog") ||
                  m_form->GetClassName() == wxT("Wizard") )
			{
				m_back->SetTitle( m_form->GetPropertyAsString( wxT("title") ) );
				long style = m_form->GetPropertyAsInteger( wxT("style") );
				m_back->SetTitleStyle( style );
				m_back->ShowTitleBar( (style & wxCAPTION) != 0 );
			}
			else
			  m_back->ShowTitleBar(false);

			// --- AUI
			if(  m_form->GetObjectTypeName() == wxT("form") )
			{
				if(  m_form->GetPropertyAsInteger( wxT("aui_managed") ) == 1)
				{
					m_auipanel = new wxPanel( m_back->GetFrameContentPanel() );
					m_auimgr = new wxAuiManager( m_auipanel, m_form->GetPropertyAsInteger( wxT("aui_manager_style") ) );
				}
			}

            // --- Wizard
            if ( m_form->GetClassName() == wxT("Wizard") )
            {
                m_wizard = new Wizard( m_back->GetFrameContentPanel() );

                bool showbutton = false;
                PProperty pextra_style = m_form->GetProperty( wxT("extra_style") );
                if ( pextra_style )
                {
                    showbutton = pextra_style->GetValue().Contains( wxT("wxWIZARD_EX_HELPBUTTON") );
                }

                m_wizard->ShowHelpButton( showbutton );

                if ( !m_form->GetProperty( wxT("bitmap") )->IsNull() )
                {
                    wxBitmap bmp = m_form->GetPropertyAsBitmap( wxT("bitmap") );
                    if ( bmp.IsOk() )
                    {
                        m_wizard->SetBitmap( bmp );
                    }
                }
            }

			// --- [4] Create the components of the form -------------------------

			// Used to save frame objects for later display
			PObjectBase menubar;
			wxWindow* statusbar = NULL;
			wxWindow* toolbar = NULL;

			for ( unsigned int i = 0; i < m_form->GetChildCount(); i++ )
			{
				PObjectBase child = m_form->GetChild( i );

				if( !menubar && (m_form->GetObjectTypeName() == wxT("menubar_form")) )
				{
					// main form acts as a menubar
					menubar = m_form;
				}
				else if (child->GetObjectTypeName() == wxT("menubar") )
				{
					// Create the menubar later
					menubar = child;
				}
				else if( !toolbar && (m_form->GetObjectTypeName() == wxT("toolbar_form")) )
				{
					Generate( m_form, m_back->GetFrameContentPanel(), m_back->GetFrameContentPanel() );

					ObjectBaseMap::iterator it = m_baseobjects.find( m_form.get() );
					toolbar = wxDynamicCast( it->second, wxToolBar );

					break;
				}
				else
				{
					// Recursively generate the ObjectTree
					try
					{
						// we have to put the content frame panel as parentObject in order
						// to SetSizeHints be called.
						if( m_auipanel )
						{
							Generate( child, m_auipanel, m_auipanel );
						}
                        else if( m_wizard )
                        {
                            Generate( child, m_wizard, m_wizard );
                        }
						else
							Generate( child, m_back->GetFrameContentPanel(), m_back->GetFrameContentPanel() );

					}
					catch ( wxFBException& ex )
					{
						wxLogError ( ex.what() );
					}
				}

				// Attach the toolbar (if any) to the frame
				if (child->GetClassName() == wxT("wxToolBar") )
				{
					ObjectBaseMap::iterator it = m_baseobjects.find( child.get() );
					toolbar = wxDynamicCast( it->second, wxToolBar );
				}
				else if (child->GetClassName() == wxT("wxAuiToolBar") )
				{
					ObjectBaseMap::iterator it = m_baseobjects.find( child.get() );
					toolbar = wxDynamicCast( it->second, wxAuiToolBar );
				}

				// Attach the status bar (if any) to the frame
				if ( child->GetObjectInfo()->IsSubclassOf( wxT("wxStatusBar") ) )
				{
					ObjectBaseMap::iterator it = m_baseobjects.find( child.get() );
					statusbar = wxDynamicCast( it->second, wxStatusBar );
				}

				// Add toolbar(s) to AuiManager and update content
				if( m_auimgr && toolbar )
				{
					SetupAui( GetObjectBase( toolbar ), toolbar );
					toolbar = NULL;
				}
			}

			if ( menubar || statusbar || toolbar || m_auipanel || m_wizard )
			{
				if( m_auimgr )
				{
					m_back->SetFrameWidgets( menubar, NULL, statusbar, m_auipanel );
				}
                else if( m_wizard )
                {
                    m_back->SetFrameWidgets( menubar, NULL, NULL, m_wizard );
                }
				else
					m_back->SetFrameWidgets( menubar, toolbar, statusbar, m_auipanel );
			}

			m_back->Layout();

			if ( backSize.GetHeight() == wxDefaultCoord || backSize.GetWidth() == wxDefaultCoord )
			{
				m_back->GetSizer()->Fit( m_back );
				m_back->SetClientSize( m_back->GetBestSize() );
			}

			// Set size after fitting so if only one dimesion is -1, it still fits that dimension
			m_back->SetSize( backSize );

			PProperty enabled( m_form->GetProperty( wxT("enabled") ) );
			if ( enabled )
			{
				m_back->Enable( TypeConv::StringToInt( enabled->GetValue() ) != 0 );
			}

			PProperty hidden( m_form->GetProperty( wxT("hidden") ) );
			if ( hidden )
			{
				m_back->Show( TypeConv::StringToInt( hidden->GetValue() ) == 0 );
			}

			if( m_auimgr ) m_auimgr->Update();
			else
				m_back->Refresh();

			Refresh();
		}
		else
		{
			// There is no form to display
			m_back->Show(false);
			Refresh();
		}
#if !defined(__WXGTK__)
		Thaw();
#endif
	}

	UpdateVirtualSize();
}


/**
* Generates wxObjects from ObjectBase
*
* @param obj ObjectBase to generate.
* @param parent wxWindow parent, necessary to instantiate a widget.
* @param parentObject ObjectBase parent - not always the same as the wxparent (e.g. an abstract component).
*/
void VisualEditor::Generate( PObjectBase obj, wxWindow* wxparent, wxObject* parentObject )
{
	// Get Component
	PObjectInfo obj_info = obj->GetObjectInfo();
	IComponent* comp = obj_info->GetComponent();

	if ( NULL == comp )
	{
		THROW_WXFBEX( wxString::Format( wxT("Component for %s not found!"), obj->GetClassName().c_str() ) );
	}

	// Create Object
	wxObject* createdObject = comp->Create( obj.get(), wxparent );
	wxWindow* createdWindow = NULL;
	wxSizer*  createdSizer  = NULL;

	switch ( comp->GetComponentType() )
	{
		case COMPONENT_TYPE_WINDOW:
			createdWindow = wxDynamicCast( createdObject, wxWindow );
			if ( NULL == createdWindow )
			{
				THROW_WXFBEX( wxString::Format( wxT("Component for %s was registered as a window component, but this is not a wxWindow!"), obj->GetClassName().c_str() ) );
			}
			SetupWindow( obj, createdWindow );

			// Push event handler in order to respond to Paint and Mouse events
			createdWindow->PushEventHandler( new VObjEvtHandler( createdWindow, obj ) );
			break;

		case COMPONENT_TYPE_SIZER:
		{
			wxStaticBoxSizer* staticBoxSizer = wxDynamicCast( createdObject, wxStaticBoxSizer );
			if ( staticBoxSizer )
			{
				createdWindow = staticBoxSizer->GetStaticBox();
				createdSizer = staticBoxSizer;
			}
			else
			{
				createdSizer = wxDynamicCast( createdObject, wxSizer );
			}
			if ( NULL == createdSizer )
			{
				THROW_WXFBEX( wxString::Format( wxT("Component for %s was registered as a sizer component, but this is not a wxSizer!"), obj->GetClassName().c_str() ) );
			}
			SetupSizer( obj, createdSizer );
			// Push event handler in order to respond to Paint and Mouse events
			//TODO: fix wxStaticBox later...
			/*if( createdWindow )
				createdWindow->PushEventHandler( new VObjEvtHandler( createdWindow, obj ) );*/
			break;
		}
		default:
			break;
	}

	// Associate the wxObject* with the PObjectBase
	m_wxobjects.insert( wxObjectMap::value_type( createdObject, obj ) );
	m_baseobjects.insert( ObjectBaseMap::value_type( obj.get(), createdObject ) );

	// New wxparent for the window's children
	wxWindow* new_wxparent = ( createdWindow ? createdWindow : wxparent );

	// Recursively generate the children
	for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
	{
		Generate( obj->GetChild( i ), new_wxparent, createdObject );
	}

	comp->OnCreated( createdObject, wxparent );
	
	// If the created object is a sizer and the parent object is a window, set the sizer to the window
	if (
			( createdSizer != NULL && NULL != wxDynamicCast( parentObject, wxWindow ) )
			||
			( NULL == parentObject && createdSizer != NULL )
		)
	{
		wxparent->SetSizer( createdSizer );
		if ( parentObject )
			createdSizer->SetSizeHints( wxparent );

		wxparent->SetAutoLayout(true);
		wxparent->Layout();
	}
}

void VisualEditor::SetupSizer( PObjectBase obj, wxSizer* sizer )
{
	wxSize minsize = obj->GetPropertyAsSize( wxT("minimum_size") );
	if ( minsize != wxDefaultSize )
	{
		sizer->SetMinSize( minsize );
		sizer->Layout();
	}
}

void VisualEditor::SetupWindow( PObjectBase obj, wxWindow* window )
{
	// All of the properties of the wxWindow object are applied in this function

	// Position
	/* Position does nothing in wxFB - this is pointless
	wxPoint pos;
	PProperty ppos = obj->GetProperty( wxT("pos") );
	if ( ppos )
	{
		pos = TypeConv::StringToPoint( ppos->GetValue() );
	}
	*/

	// Size
	wxSize size = obj->GetPropertyAsSize( wxT("size") );
	if ( size != wxDefaultSize )
	{
		window->SetSize( size );
	}

	// Minimum size
	wxSize minsize = obj->GetPropertyAsSize( wxT("minimum_size") );
	if ( minsize != wxDefaultSize )
	{
		window->SetMinSize( minsize );
	}

	// Maximum size
	wxSize maxsize = obj->GetPropertyAsSize( wxT("maximum_size") );
	if ( maxsize != wxDefaultSize )
	{
		window->SetMaxSize( maxsize );
	}

	// Font
	PProperty pfont = obj->GetProperty( wxT("font") );
	if ( pfont && !pfont->GetValue().empty() )
	{
		window->SetFont( TypeConv::StringToFont( pfont->GetValue() ) );
	}

	// Foreground
	PProperty pfg_colour = obj->GetProperty( wxT("fg") );
	if ( pfg_colour && !pfg_colour->GetValue().empty() )
	{
		window->SetForegroundColour( TypeConv::StringToColour( pfg_colour->GetValue() ) );
	}

	// Background
	PProperty pbg_colour = obj->GetProperty( wxT("bg") );
	if ( pbg_colour && !pbg_colour->GetValue().empty() )
	{
		window->SetBackgroundColour( TypeConv::StringToColour( pbg_colour->GetValue() ) );
	}

	// Extra Style
	PProperty pextra_style = obj->GetProperty( wxT("window_extra_style") );
	if ( pextra_style )
	{
		window->SetExtraStyle( TypeConv::StringToInt( pextra_style->GetValue() ) );
	}

	// Enabled
	PProperty penabled = obj->GetProperty( wxT("enabled") );
	if ( penabled )
	{
		window->Enable( ( penabled->GetValueAsInteger() !=0 ) );
	}

	// Hidden
	PProperty phidden = obj->GetProperty( wxT("hidden") );
	if ( phidden )
	{
		window->Show( !phidden->GetValueAsInteger() );
	}

	// Tooltip
	PProperty ptooltip = obj->GetProperty( wxT("tooltip") );
	if ( ptooltip )
	{
		window->SetToolTip( ptooltip->GetValueAsString() );
	}

	//AUI
	wxString tname = obj->GetObjectInfo()->GetObjectType()->GetName();
	if( m_auimgr && ( tname == wxT("widget") ||
					tname == wxT("expanded_widget") ||
					tname == wxT("container") ||
					tname == wxT("notebook") ||
					tname == wxT("auinotebook") ||
					tname == wxT("flatnotebook") ||
					tname == wxT("listbook") ||
					tname == wxT("choicebook") ||
					tname == wxT("treelistctrl") ||
					tname == wxT("ribbonbar") ||
					tname == wxT("dataviewctrl") ||
					tname == wxT("dataviewtreectrl") ||
					tname == wxT("dataviewlistctrl") ||
					tname == wxT("propgrid") ||
					tname == wxT("propgridman") ||
					tname == wxT("splitter") ) )
	{
		if( obj->GetParent()->GetObjectTypeName() == wxT("form") )
		{
			SetupAui(obj, window);
		}
	}
    // Wizard
    else if ( obj->GetParent()->GetObjectTypeName() == wxT("wizard") )
    {
        SetupWizard( obj, window, true );
    }
}

void VisualEditor::SetupAui( PObjectBase obj, wxWindow* window )
{
	wxAuiPaneInfo info;

	// check whether the object contains AUI info...
	if( !obj->GetProperty( wxT("aui_name") ) ) return;

	wxString name = obj->GetPropertyAsString( wxT("aui_name") );
	if( name != wxT("") ) info.Name( name );

	if( obj->GetPropertyAsInteger( wxT("center_pane") )) info.CenterPane();
	if( obj->GetPropertyAsInteger( wxT("default_pane") )) info.DefaultPane();

	if( !obj->IsNull(wxT("caption"))) info.Caption(obj->GetPropertyAsString(wxT("caption")));
	info.CaptionVisible( obj->GetPropertyAsInteger( wxT("caption_visible") ) );
	info.CloseButton( obj->GetPropertyAsInteger( wxT("close_button") ) );
	info.MaximizeButton( obj->GetPropertyAsInteger( wxT("maximize_button") ) );
	info.MinimizeButton( obj->GetPropertyAsInteger( wxT("minimize_button") ) );
	info.PinButton( obj->GetPropertyAsInteger( wxT("pin_button") ) );
	info.PaneBorder( obj->GetPropertyAsInteger( wxT("pane_border") ) );
	info.Gripper(obj->GetPropertyAsInteger( wxT("gripper") ));

	info.BottomDockable( obj->GetPropertyAsInteger( wxT("BottomDockable") ) );
	info.TopDockable( obj->GetPropertyAsInteger( wxT("TopDockable") ) );
	info.LeftDockable( obj->GetPropertyAsInteger( wxT("LeftDockable") ) );
	info.RightDockable( obj->GetPropertyAsInteger( wxT("RightDockable") ) );

	if( !obj->IsNull(wxT("dock")) )
	{
		if( obj->GetPropertyAsString( wxT("dock") ) == wxT("Dock"))
		{
			info.Dock();
			if( !obj->IsNull(wxT("docking")) )
			{
				if( obj->GetPropertyAsString(wxT("docking")) == wxT("Bottom") ) info.Bottom();
				else if( obj->GetPropertyAsString(wxT("docking")) == wxT("Top") ) info.Top();
				else if( obj->GetPropertyAsString(wxT("docking")) == wxT("Center") ) info.Center();
				else if( obj->GetPropertyAsString(wxT("docking")) == wxT("Right") ) info.Right();
			}
		}
		else
		{
			info.Float();
			info.FloatingPosition( obj->GetPropertyAsPoint( wxT("pane_position") ) );
		}
	}

	if( !obj->IsNull(wxT("resize")) )
	{
		if( obj->GetPropertyAsString( wxT("resize") ) == wxT("Resizable")) info.Resizable();
		else info.Fixed();
	}

	info.DockFixed( obj->GetPropertyAsInteger( wxT("dock_fixed") ) );
	info.Movable( obj->GetPropertyAsInteger( wxT("moveable") ));
	info.Floatable(obj->GetPropertyAsInteger( wxT("floatable") ));

	if( !obj->GetProperty( wxT("pane_size" ) )->IsNull() ) info.FloatingSize( obj->GetPropertyAsSize( wxT("pane_size") ));
	if( !obj->GetProperty( wxT("best_size" ) )->IsNull() ) info.BestSize( obj->GetPropertyAsSize( wxT("best_size") ) );
	if( !obj->GetProperty( wxT("min_size" ) )->IsNull() ) info.MinSize( obj->GetPropertyAsSize( wxT("min_size") ) );
	if( !obj->GetProperty( wxT("max_size" ) )->IsNull() ) info.MaxSize( obj->GetPropertyAsSize( wxT("max_size") ) );

	if( obj->GetPropertyAsInteger( wxT("toolbar_pane") ) ) info.ToolbarPane();
	if( !obj->IsNull( wxT("aui_position") ) ) info.Position( obj->GetPropertyAsInteger( wxT("aui_position") ));
	if( !obj->IsNull( wxT("aui_row") ) ) info.Row( obj->GetPropertyAsInteger( wxT("aui_row") ));
    if( !obj->IsNull( wxT("aui_layer") ) ) info.Layer( obj->GetPropertyAsInteger( wxT("aui_layer") ));
	if( !obj->GetPropertyAsInteger( wxT("show") ) ) info.Hide();

	m_auimgr->AddPane( window, info );
}

void VisualEditor::SetupWizard( PObjectBase obj, wxWindow *window, bool pageAdding )
{
    WizardPageSimple *wizpage = wxDynamicCast( window, WizardPageSimple );

    if ( pageAdding )
    {
        m_wizard->AddPage( wizpage );
        m_wizard->Connect( wxID_ANY, wxFB_EVT_WIZARD_PAGE_CHANGED, WizardEventHandler( VisualEditor::OnWizardPageChanged ) );
    }
    else
    {
        WizardEvent eventChanged( wxFB_EVT_WIZARD_PAGE_CHANGED, m_wizard->GetId(), false, wizpage );
        eventChanged.SetInt( 1 );
        wizpage->GetEventHandler()->ProcessEvent( eventChanged );

        bool wizBmpOk = !obj->GetParent()->GetProperty( wxT("bitmap") )->IsNull();
        bool pgeBmpOk = !obj->GetProperty( wxT("bitmap") )->IsNull();
        wxBitmap wizBmp = obj->GetParent()->GetPropertyAsBitmap( wxT("bitmap") );
        wxBitmap pgeBmp = obj->GetPropertyAsBitmap( wxT("bitmap") );

        if ( pgeBmpOk && pgeBmp.IsOk() )
        {
            m_wizard->SetBitmap( pgeBmp );
        }
        else if ( wizBmpOk && wizBmp.IsOk() )
        {
            m_wizard->SetBitmap( wizBmp );
        }
        size_t selection = m_wizard->GetPageIndex( wizpage );
        m_wizard->SetSelection( selection );
    }
}

/////////////////////////////////////////////////////////////////////////////
void VisualEditor::PreventOnSelected( bool prevent )
{
	m_stopSelectedEvent = prevent;
}

void VisualEditor::PreventOnModified( bool prevent )
{
	m_stopModifiedEvent = prevent;
}

void VisualEditor::OnProjectLoaded ( wxFBEvent &)
{
  Create();
}

void VisualEditor::OnProjectSaved  ( wxFBEvent & )
{
  //Create();
}

void VisualEditor::OnObjectSelected( wxFBObjectEvent &event )
{
	// It is only necessary to Create() if the selected object is on a different form
	if ( AppData()->GetSelectedForm() != m_form )
	{
		Create();
	}

	// Get the ObjectBase from the event
	PObjectBase obj = event.GetFBObject();
	if ( !obj )
	{
		// Strange...
		LogDebug( wxT("The event object is NULL - why?") );
		return;
	}

	// highlight parent toolbar instead of its children
	PObjectBase toolbar = obj->FindNearAncestor( wxT("toolbar") );
	if( !toolbar ) toolbar = obj->FindNearAncestor( wxT("toolbar_form") );
	if( toolbar ) obj = toolbar;

	// Make sure this is a visible object
	ObjectBaseMap::iterator it = m_baseobjects.find( obj.get() );
	if ( m_baseobjects.end() == it )
	{
		m_back->SetSelectedSizer( NULL );
		m_back->SetSelectedItem( NULL );
		m_back->SetSelectedObject( PObjectBase() );
		m_back->SetSelectedPanel( NULL );
		m_back->Refresh();
		return;
	}

	// Save wxobject
	wxObject* item = it->second;

	int componentType = COMPONENT_TYPE_ABSTRACT;
	IComponent *comp = obj->GetObjectInfo()->GetComponent();
	if ( comp )
	{
		componentType = comp->GetComponentType();

		// Fire selection event in plugin
		if ( !m_stopSelectedEvent )
		{
			comp->OnSelected( item );
		}
	}

    if ( obj->GetObjectInfo()->GetObjectTypeName() == wxT("wizardpagesimple") )
    {
        ObjectBaseMap::iterator pageIt = m_baseobjects.find( obj.get() );
        WizardPageSimple* wizpage = wxDynamicCast( pageIt->second, WizardPageSimple );

        SetupWizard( obj, wizpage );
    }

	if ( componentType != COMPONENT_TYPE_WINDOW && componentType != COMPONENT_TYPE_SIZER )
	{
		item = NULL;
	}

	// Fire selection event in plugin for all parents
	if ( !m_stopSelectedEvent )
	{
		PObjectBase parent = obj->GetParent();
		while ( parent )
		{
			IComponent* parentComp = parent->GetObjectInfo()->GetComponent();
			if ( parentComp )
			{
				ObjectBaseMap::iterator parentIt = m_baseobjects.find( parent.get() );
				if ( parentIt != m_baseobjects.end() )
				{
                    if ( parent->GetObjectInfo()->GetObjectTypeName() == wxT("wizardpagesimple") )
                    {
                        WizardPageSimple* wizpage = wxDynamicCast( parentIt->second, WizardPageSimple );

                        SetupWizard( parent, wizpage );
                    }
					parentComp->OnSelected( parentIt->second );
				}
			}
			parent = parent->GetParent();
		}
	}

	// Look for the active panel - this is where the boxes will be drawn during OnPaint
	// This is the closest parent of type COMPONENT_TYPE_WINDOW
	PObjectBase nextParent = obj->GetParent();
	while ( nextParent )
	{
		IComponent* parentComp = nextParent->GetObjectInfo()->GetComponent();
		if ( !parentComp )
		{
			nextParent.reset();
			break;
		}

		if ( parentComp->GetComponentType() == COMPONENT_TYPE_WINDOW )
		{
			break;
		}

		//TODO: fix wxStaticBox later...
		/*ObjectBaseMap::iterator it = m_baseobjects.find( nextParent.get() );
		if ( m_baseobjects.end() != it )
		{
			wxObject* parentObj = it->second;
			if( wxDynamicCast( parentObj, wxStaticBoxSizer ))
				break;
		}*/

		nextParent = nextParent->GetParent();
	}

	// Get the panel to draw on
	wxWindow* selPanel = NULL;
	if ( nextParent )
	{
		it = m_baseobjects.find( nextParent.get() );
		if ( m_baseobjects.end() == it )
		{
			selPanel = m_back->GetFrameContentPanel();
		}
		else
		{
			//TODO: fix wxStaticBox later...
			/*if( wxStaticBoxSizer *sizer = wxDynamicCast( it->second, wxStaticBoxSizer ))
			{
				selPanel = sizer->GetStaticBox();
			}
			else*/
				selPanel = wxDynamicCast( it->second, wxWindow );
		}
	}
	else
	{
		selPanel = m_back->GetFrameContentPanel();
	}

	// Find the first COMPONENT_TYPE_WINDOW or COMPONENT_TYPE_SIZER
	// If it is a sizer, save it
	wxSizer* sizer = NULL;
	PObjectBase nextObj = obj->GetParent();
	while ( nextObj )
	{
		IComponent* nextComp = nextObj->GetObjectInfo()->GetComponent();
		if ( !nextComp )
		{
			break;
		}

		if ( nextComp->GetComponentType() == COMPONENT_TYPE_SIZER )
		{
			it = m_baseobjects.find( nextObj.get() );
			if ( it != m_baseobjects.end() )
			{
				sizer = wxDynamicCast( it->second, wxSizer );
			}
			break;
		}
		else if ( nextComp->GetComponentType() == COMPONENT_TYPE_WINDOW )
		{
			break;
		}

		nextObj = nextObj->GetParent();
	}

	m_back->SetSelectedSizer( sizer );
	m_back->SetSelectedItem( item );
	m_back->SetSelectedObject( obj );
	m_back->SetSelectedPanel( selPanel );
	m_back->Refresh();
}

void VisualEditor::OnObjectCreated( wxFBObjectEvent &)
{
	Create();
}

void VisualEditor::OnObjectRemoved( wxFBObjectEvent & )
{
	Create();
}

void VisualEditor::OnPropertyModified( wxFBPropertyEvent &)
{
	if ( !m_stopModifiedEvent )
	{
		PObjectBase aux = m_back->GetSelectedObject();
		Create();
		if ( aux )
		{
			wxFBObjectEvent objEvent( wxEVT_FB_OBJECT_SELECTED, aux );
			this->ProcessEvent( objEvent );
		}
		UpdateVirtualSize();
	}
}

void VisualEditor::OnProjectRefresh( wxFBEvent &)
{
	Create();
}

void VisualEditor::OnAuiScaner(wxTimerEvent& event)
{
	if( m_auimgr )
	{
		ScanPanes( m_back->GetFrameContentPanel() );
	}
}

IMPLEMENT_CLASS( DesignerWindow, wxInnerFrame)

BEGIN_EVENT_TABLE(DesignerWindow,wxInnerFrame)
  EVT_PAINT(DesignerWindow::OnPaint)
END_EVENT_TABLE()

DesignerWindow::DesignerWindow( wxWindow *parent, int id, const wxPoint& pos, const wxSize &size, long style, const wxString & /*name*/ )
:
wxInnerFrame(parent, id, pos, size, style)
{
	ShowTitleBar(false);
	SetGrid( 10, 10 );
	m_selSizer = NULL;
	m_selItem = NULL;
	m_actPanel = NULL;
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

	GetFrameContentPanel()->PushEventHandler(new HighlightPaintHandler(GetFrameContentPanel()));
}

DesignerWindow::~DesignerWindow()
{
    GetFrameContentPanel()->PopEventHandler( true );
}

void DesignerWindow::SetGrid( int x, int y )
{
	m_x = x;
	m_y = y;
}

void DesignerWindow::OnPaint(wxPaintEvent &event)
{
	// This paint event helps draw the selection boxes
	// when they extend beyond the edges of the content panel
	wxPaintDC dc(this);

	if ( m_actPanel == GetFrameContentPanel() )
	{
		wxPoint origin = GetFrameContentPanel()->GetPosition();
		dc.SetDeviceOrigin( origin.x, origin.y );
		HighlightSelection( dc );
	}

	event.Skip();
}

void DesignerWindow::DrawRectangle( wxDC& dc, const wxPoint& point, const wxSize& size, PObjectBase object )
{
	bool isSizer = ( object->GetObjectInfo()->IsSubclassOf( wxT("sizer") ) || object->GetObjectInfo()->IsSubclassOf( wxT("gbsizer") ) );
	int min = ( isSizer ? 0 : 1 );

	int border = object->GetParent()->GetPropertyAsInteger( wxT("border") );
	if ( border == 0 )
	{
		border = min;
	}

	int flag = object->GetParent()->GetPropertyAsInteger( wxT("flag") );
	int topBorder = 	( flag & wxTOP ) 	== 0 ? min : border;
	int bottomBorder = 	( flag & wxBOTTOM ) == 0 ? min : border;
	int rightBorder = 	( flag & wxRIGHT ) 	== 0 ? min : border;
	int leftBorder = 	( flag & wxLEFT ) 	== 0 ? min : border;

	dc.DrawRectangle( 	point.x - leftBorder,
						point.y - topBorder,
						size.x + leftBorder + rightBorder,
						size.y + topBorder + bottomBorder );
}

void DesignerWindow::HighlightSelection( wxDC& dc )
{
	// do not highlight if AUI is used in floating mode
	VisualEditor *editor = wxDynamicCast( GetParent(), VisualEditor );
	if( editor && editor->m_auimgr )
	{
		wxWindow *windowItem =  wxDynamicCast( m_selItem, wxWindow );
		while( windowItem )
		{
			wxAuiPaneInfo info = editor->m_auimgr->GetPane( windowItem );
			if( info.IsOk() )
			{
				if( info.IsFloating() ) return;
				else break;
			}
			windowItem = windowItem->GetParent();
		}
	}

	wxSize size;
	PObjectBase object = m_selObj.lock();
	if ( m_selSizer )
	{
		wxScrolledWindow* scrolwin = wxDynamicCast(m_selSizer->GetContainingWindow (), wxScrolledWindow);
		if(scrolwin)
		{
			scrolwin->FitInside();
		}
		wxPoint point = m_selSizer->GetPosition();
		//TODO: fix wxStaticBox later...
		/*if( wxStaticBoxSizer *sbSizer = wxDynamicCast(m_selSizer, wxStaticBoxSizer) )
		{
			// In case of wxStaticBoxSizer, m_actPanel is not a parent window
			// of the sizer (m_actPanel==sbSizer->GetStaticBox()).
			// Thus we need to convert the sizer's position into coordinates
			// of m_actPanel.
			// We could do this via parent window of the sizer, but it's hard to
			// obtain this window. The m_selSizer->GetContainingWindow() doesn't
			// always return the parent window of the sizer, namely in the case
			// the m_selSizer is inside another wxStaticBoxSizer
			// (at least in MSW build, wxWidgets 3.0.1).
			// We convert its StaticBox origin (StaticBox is a window) since origins
			// of wxStaticBoxSizer and its StaticBox are the same point.
			point = m_actPanel->ScreenToClient( sbSizer->GetStaticBox()->GetScreenPosition() );
		}*/
		size = m_selSizer->GetSize();

		wxPen bluePen( *wxBLUE, 1, wxSOLID );
		dc.SetPen( bluePen );
		dc.SetBrush( *wxTRANSPARENT_BRUSH );
		PObjectBase sizerParent = object->FindNearAncestorByBaseClass( wxT("sizer") );
		if( !sizerParent ) sizerParent = object->FindNearAncestorByBaseClass( wxT("gbsizer") );
		if ( sizerParent && sizerParent->GetParent() )
		{
			DrawRectangle( dc, point, size, sizerParent );
		}
	}

	if ( m_selItem )
	{
		wxPoint point;
		bool shown;

		wxWindow* windowItem = wxDynamicCast( m_selItem, wxWindow );
		wxSizer* sizerItem = wxDynamicCast( m_selItem, wxSizer );
		if ( NULL != windowItem )
		{
			// In case the windowItem is inside a wxStaticBoxSizer its position is relative to
			// the wxStaticBox which is NOT m_actPanel in on which the highlight is painted,
			// so get the screen coordinates of the item and convert them into client coordinates
			// of the panel to get the correct relative coordinates. This doesn't do any harm if
			// the item is not inside a wxStaticBoxSizer, if this conversion results in a big
			// performance penalty maybe check if the parent is a wxStaticBox and only then do
			// this conversion.
			point = m_actPanel->ScreenToClient(windowItem->GetScreenPosition());
			size = windowItem->GetSize();
			shown = windowItem->IsShown();
		}
		else if ( NULL != sizerItem )
		{
			point = sizerItem->GetPosition();
			size = sizerItem->GetSize();
			shown = true;
		}
		else
		{
			return;
		}

		if ( shown )
		{
			wxPen redPen( *wxRED, 1, wxSOLID );
			dc.SetPen( redPen );
			dc.SetBrush( *wxTRANSPARENT_BRUSH );
			DrawRectangle( dc, point, size, object );
		}
	}
}

wxMenu* DesignerWindow::GetMenuFromObject(PObjectBase menu)
{
	int lastMenuId = wxID_HIGHEST + 1;
	wxMenu* menuWidget = new wxMenu();
	for ( unsigned int j = 0; j < menu->GetChildCount(); j++ )
	{
		PObjectBase menuItem = menu->GetChild( j );
		if ( menuItem->GetObjectTypeName() == wxT("submenu") )
		{
			wxMenuItem *item = new wxMenuItem( 	menuWidget,
												lastMenuId++,
												menuItem->GetPropertyAsString( wxT("label") ),
												menuItem->GetPropertyAsString( wxT("help") ),
												wxITEM_NORMAL,
												GetMenuFromObject( menuItem )
											);
			item->SetBitmap( menuItem->GetPropertyAsBitmap( wxT("bitmap") ) );
			menuWidget->Append( item );
		}
		else if ( menuItem->GetClassName() == wxT("separator") )
		{
			menuWidget->AppendSeparator();
		}
		else
		{
			wxString label = menuItem->GetPropertyAsString( wxT("label") );
			wxString shortcut = menuItem->GetPropertyAsString( wxT("shortcut") );
			if ( !shortcut.IsEmpty() )
			{
				label = label + wxChar('\t') + shortcut;
			}

			wxMenuItem *item = new wxMenuItem( 	menuWidget,
												lastMenuId++,
												label,
												menuItem->GetPropertyAsString( wxT("help") ),
												( wxItemKind ) menuItem->GetPropertyAsInteger( wxT("kind") )
											);

			if ( !menuItem->GetProperty( wxT("bitmap") )->IsNull() )
			{
				wxBitmap unchecked = wxNullBitmap;
				if ( !menuItem->GetProperty( wxT("unchecked_bitmap") )->IsNull() )
				{
					unchecked = menuItem->GetPropertyAsBitmap( wxT("unchecked_bitmap") );
				}
				#ifdef __WXMSW__
					item->SetBitmaps( menuItem->GetPropertyAsBitmap( wxT("bitmap") ), unchecked );
				#elif defined( __WXGTK__ )
					item->SetBitmap( menuItem->GetPropertyAsBitmap( wxT("bitmap") ) );
				#endif
			}
			else
			{
				if ( !menuItem->GetProperty( wxT("unchecked_bitmap") )->IsNull() )
				{
					#ifdef __WXMSW__
						item->SetBitmaps( wxNullBitmap,  menuItem->GetPropertyAsBitmap( wxT("unchecked_bitmap") ) );
					#endif
				}
			}

			menuWidget->Append( item );

			if ( item->GetKind() == wxITEM_CHECK && menuItem->GetPropertyAsInteger( wxT("checked") ) )
			{
				item->Check( true );
			}

			item->Enable( ( menuItem->GetPropertyAsInteger( wxT("enabled") ) != 0 ) );
		}
	}

	return menuWidget;
}

void DesignerWindow::SetFrameWidgets(PObjectBase menubar, wxWindow *toolbar, wxWindow *statusbar, wxWindow *auipanel)
{
	wxWindow *contentPanel = GetFrameContentPanel();
	Menubar *mbWidget = NULL;

	if ( menubar )
	{
		mbWidget = new Menubar(contentPanel, -1);
		for ( unsigned int i = 0; i < menubar->GetChildCount(); i++ )
		{
			PObjectBase menu = menubar->GetChild( i );
			wxMenu *menuWidget = GetMenuFromObject( menu );
			mbWidget->AppendMenu( menu->GetPropertyAsString( wxT("label") ), menuWidget );
		}
	}

	wxSizer *mainSizer = contentPanel->GetSizer();

	contentPanel->SetSizer( NULL, false );

	wxSizer *dummySizer = new wxBoxSizer( wxVERTICAL );

	if ( mbWidget )
	{
		dummySizer->Add(mbWidget, 0, wxEXPAND | wxTOP | wxBOTTOM, 0);
		dummySizer->Add(new wxStaticLine(contentPanel, -1), 0, wxEXPAND | wxALL, 0);
	}

	wxSizer* contentSizer = dummySizer;
	if (toolbar)
	{
		if ( (toolbar->GetWindowStyle() & wxTB_VERTICAL) != 0 )
		{
			wxSizer* horiz = new wxBoxSizer( wxHORIZONTAL );
			horiz->Add(toolbar, 0, wxEXPAND | wxALL, 0);

			wxSizer* vert = new wxBoxSizer( wxVERTICAL );
			horiz->Add( vert, 1, wxEXPAND, 0 );

			dummySizer->Add( horiz, 1, wxEXPAND, 0);

			contentSizer = vert;
		}
		else
		{
			dummySizer->Add(toolbar, 0, wxEXPAND | wxALL, 0);
		}
	}

	if (auipanel)
	{
		contentSizer->Add(auipanel, 1, wxEXPAND | wxALL, 0);
	}
	else if (mainSizer)
	{
		contentSizer->Add(mainSizer, 1, wxEXPAND | wxALL, 0);
		if ( mainSizer->GetChildren().IsEmpty() )
		{
			// Sizers do not expand if they are empty
			mainSizer->AddStretchSpacer(1);
		}
	}
	else
		contentSizer->AddStretchSpacer(1);

	if (statusbar)
	{
		if( auipanel ) statusbar->Reparent( contentPanel );
		contentSizer->Add(statusbar, 0, wxEXPAND | wxALL, 0);
	}

	contentPanel->SetSizer(dummySizer, false);
	contentPanel->Layout();
}


BEGIN_EVENT_TABLE(DesignerWindow::HighlightPaintHandler,wxEvtHandler)
  EVT_PAINT(DesignerWindow::HighlightPaintHandler::OnPaint)
END_EVENT_TABLE()


DesignerWindow::HighlightPaintHandler::HighlightPaintHandler(wxWindow *win)
{
  m_window = win;
}

void DesignerWindow::HighlightPaintHandler::OnPaint(wxPaintEvent &event)
{
	wxWindow *aux = m_window;
	while (!aux->IsKindOf(CLASSINFO(DesignerWindow))) aux = aux->GetParent();
	DesignerWindow *dsgnWin = (DesignerWindow*) aux;
	if (dsgnWin->GetActivePanel() == m_window)
	{
		wxPaintDC dc(m_window);
		dsgnWin->HighlightSelection(dc);
	}

	event.Skip();
}
