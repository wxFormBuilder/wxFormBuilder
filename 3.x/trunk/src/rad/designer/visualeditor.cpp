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
#include <rad/appdata.h>
#include "utils/wxfbexception.h"
#include "model/objectbase.h"

#ifdef __WX24__
#define wxFULL_REPAINT_ON_RESIZE 0
#endif

BEGIN_EVENT_TABLE(VisualEditor,wxScrolledWindow)
	//EVT_SASH_DRAGGED(-1, VisualEditor::OnResizeBackPanel)
	//EVT_COMMAND(-1, wxEVT_PANEL_RESIZED, VisualEditor::OnResizeBackPanel)
	EVT_INNER_FRAME_RESIZED(-1, VisualEditor::OnResizeBackPanel)

	EVT_FB_PROJECT_LOADED( VisualEditor::OnProjectLoaded )
	EVT_FB_PROJECT_SAVED( VisualEditor::OnProjectSaved )
	EVT_FB_OBJECT_SELECTED( VisualEditor::OnObjectSelected )
	EVT_FB_OBJECT_CREATED( VisualEditor::OnObjectCreated )
	EVT_FB_OBJECT_REMOVED( VisualEditor::OnObjectRemoved )
	EVT_FB_PROPERTY_MODIFIED( VisualEditor::OnPropertyModified )
	EVT_FB_PROJECT_REFRESH( VisualEditor::OnProjectRefresh )

END_EVENT_TABLE()

VisualEditor::VisualEditor(wxWindow *parent)
:
wxScrolledWindow(parent,-1,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER),
m_stopSelectedEvent( false ),
m_stopModifiedEvent( false )
{
	AppData()->AddHandler( this->GetEventHandler() );

	#ifdef __WXMSW__
		SetOwnBackgroundColour(wxColour(150,150,150));
	#else
		SetOwnBackgroundColour(wxColour(192,192,192));
	#endif

	SetScrollRate(5, 5);

	m_back = new DesignerWindow( this, wxID_ANY, wxPoint(10,10) );
	m_back->GetEventHandler()->Connect( wxID_ANY, wxEVT_LEFT_DOWN, wxMouseEventHandler( VisualEditor::OnClickBackPanel ), NULL, this );
}

void VisualEditor::DeleteAbstractObjects()
{
	wxObjectMap::iterator wxNoObjectIt;
	wxNoObject* wxnoobject;
	for ( wxNoObjectIt = m_wxobjects.begin(); wxNoObjectIt != m_wxobjects.end(); ++wxNoObjectIt )
	{
		wxnoobject = dynamic_cast< wxNoObject* >( wxNoObjectIt->first );
		if ( wxnoobject )
		{
			wxnoobject->Destroy();
		}
	}
}

VisualEditor::~VisualEditor()
{
	AppData()->RemoveHandler( this->GetEventHandler() );
	DeleteAbstractObjects();
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
}

void VisualEditor::OnResizeBackPanel (wxCommandEvent &event) //(wxSashEvent &event)
{
	/*wxRect rect(event.GetDragRect());
	Debug::Print("VisualEditor::OnResizeBackPanel [%d,%d,%d,%d]",rect.x,rect.y,rect.width, rect.height);
	m_back->SetSize(rect.width,rect.height);
	m_back->Layout();*/

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

	//event.Skip();
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

/**
* Crea la vista preliminar borrando la previa.
*/
void VisualEditor::Create()
{
	if ( IsShown() )
	{
		Freeze(); // Prevent flickering
	}

	// Delete objects which had no parent
	DeleteAbstractObjects();

	// Clear selections, delete objects
	m_back->SetSelectedItem(NULL);
	m_back->SetSelectedSizer(NULL);
	m_back->SetSelectedObject(PObjectBase());
	m_back->GetFrameContentPanel()->DestroyChildren();
	m_back->GetFrameContentPanel()->SetSizer( NULL ); // *!*

	// Clear all associations between ObjectBase and wxObjects
	m_wxobjects.clear();
	m_baseobjects.clear();

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
				m_back->GetFrameContentPanel()->SetOwnBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
			}
		}


		// --- [3] Title bar Setup
		if (  m_form->GetClassName() == wxT("Frame") || m_form->GetClassName() == wxT("Dialog") )
		{
			m_back->SetTitle( m_form->GetPropertyAsString( wxT("title") ) );
			long style = m_form->GetPropertyAsInteger( wxT("style") );
			m_back->SetTitleStyle( style );
			m_back->ShowTitleBar( (style & wxCAPTION) != 0 );
		}
		else
		  m_back->ShowTitleBar(false);

		// --- [4] Create the components of the form -------------------------

		// Used to save frame objects for later display
		PObjectBase menubar;
		wxWindow* statusbar = NULL;
		wxWindow* toolbar = NULL;

		for ( unsigned int i = 0; i < m_form->GetChildCount(); i++ )
		{
			PObjectBase child = m_form->GetChild( i );

			if (child->GetObjectTypeName() == wxT("menubar") )
			{
				// Create the menubar later
				menubar = child;
			}
			else
			{
				// Recursively generate the ObjectTree
				try
				{
				  // we have to put the content frame panel as parentObject in order
				  // to SetSizeHints be called.
					Generate( child, m_back->GetFrameContentPanel(), m_back->GetFrameContentPanel() );
				}
				catch ( wxFBException& ex )
				{
					wxLogError ( ex.what() );
				}
			}

			// Attach the status bar (if any) to the frame
			if ( child->GetClassName() == wxT("wxStatusBar") )
			{
				ObjectBaseMap::iterator it = m_baseobjects.find( child.get() );
				statusbar = wxDynamicCast( it->second, wxStatusBar );
			}

			// Attach the toolbar (if any) to the frame
			if (child->GetClassName() == wxT("wxToolBar") )
			{
				ObjectBaseMap::iterator it = m_baseobjects.find( child.get() );
				toolbar = wxDynamicCast( it->second, wxToolBar );
			}
		}

		m_back->Layout();

		if ( backSize.GetHeight() == wxDefaultCoord || backSize.GetWidth() == wxDefaultCoord )
		{
			if ( m_back->GetFrameContentPanel()->GetSizer() )
			{
				m_back->GetSizer()->Fit( m_back );
			}
			else
			{
				m_back->SetToBaseSize();
			}
		}

		// Set size after fitting so if only one dimesion is -1, it still fits that dimension
		m_back->SetSize( backSize );

		if ( menubar || statusbar || toolbar )
		{
			m_back->SetFrameWidgets( menubar, toolbar, statusbar );
		}

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
	}
	else
	{
		// There is no form to display
		//m_back->SetSize(10,10);
		//m_back->GetFrameContentPanel()->SetOwnBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
		m_back->Show(false);
	}

	if ( IsShown() )
	{
		Thaw();
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
			createdSizer = wxDynamicCast( createdObject, wxSizer );
			if ( NULL == createdSizer )
			{
				THROW_WXFBEX( wxString::Format( wxT("Component for %s was registered as a sizer component, but this is not a wxSizer!"), obj->GetClassName().c_str() ) );
			}
			SetupSizer( obj, createdSizer );
			break;

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

	comp->OnCreated( createdObject, new_wxparent );

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

void VisualEditor::OnProjectLoaded ( wxFBEvent &event )
{
  Create();
}

void VisualEditor::OnProjectSaved  ( wxFBEvent &event )
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
		Debug::Print( wxT("The event object is NULL - why?") );
		return;
	}

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

void VisualEditor::OnObjectCreated( wxFBObjectEvent &event )
{
	Create();
}

void VisualEditor::OnObjectRemoved( wxFBObjectEvent &event )
{
	Create();
}

void VisualEditor::OnPropertyModified( wxFBPropertyEvent &event )
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

void VisualEditor::OnProjectRefresh( wxFBEvent &event )
{
	Create();
}

IMPLEMENT_CLASS( DesignerWindow, wxInnerFrame)

BEGIN_EVENT_TABLE(DesignerWindow,wxInnerFrame)
  EVT_PAINT(DesignerWindow::OnPaint)
END_EVENT_TABLE()

DesignerWindow::DesignerWindow( wxWindow *parent, int id, const wxPoint& pos, const wxSize &size, long style, const wxString &name )
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
	bool isSizer = ( object->GetObjectInfo()->IsSubclassOf( wxT("sizer") ) );
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
	wxSize size;
	PObjectBase object = m_selObj.lock();
	if ( m_selSizer )
	{
		wxPoint point = m_selSizer->GetPosition();
		size = m_selSizer->GetSize();
		wxPen bluePen( *wxBLUE, 1, wxSOLID );
		dc.SetPen( bluePen );
		dc.SetBrush( *wxTRANSPARENT_BRUSH );
		PObjectBase sizerParent = object->FindNearAncestorByBaseClass( wxT("sizer") );
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
			point = windowItem->GetPosition();
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
			menuWidget->Append( lastMenuId++, menuItem->GetPropertyAsString( wxT("label") ), GetMenuFromObject( menuItem ) );
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
												( menuItem->GetPropertyAsInteger( wxT("kind") ) != 0 )
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

void DesignerWindow::SetFrameWidgets(PObjectBase menubar, wxWindow *toolbar, wxWindow *statusbar)
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

	if (toolbar)
		dummySizer->Add(toolbar, 0, wxEXPAND | wxALL, 0);

	if (mainSizer)
	{
		dummySizer->Add(mainSizer, 1, wxEXPAND | wxALL, 0);
		if ( mainSizer->GetChildren().IsEmpty() )
		{
			// Sizers do not expand if they are empty
			mainSizer->AddStretchSpacer(1);
		}
	}
	else
		dummySizer->AddStretchSpacer(1);

	if (statusbar)
		dummySizer->Add(statusbar, 0, wxEXPAND | wxALL, 0);


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
//	wxPaintDC dc(this);
/*	wxSize size = GetSize();
	dc.SetPen(*wxBLACK_PEN);
	for ( int i = 0; i < size.GetWidth(); i += m_x )
	{
		for ( int j = 0; j < size.GetHeight(); j += m_y )
		{
			dc.DrawPoint( i - 1, j - 1 );
		}
	}*/

	/*if ( m_actPanel == this)
	{
		HighlightSelection( dc );
	}*/

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

