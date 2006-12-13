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
#include "nativeinnerframe.h"

#ifdef __WX24__
#define wxFULL_REPAINT_ON_RESIZE 0
#endif

#ifdef __WXGTK__
#define VISUAL_EDITOR_BORDER wxRAISED_BORDER
#else
#define VISUAL_EDITOR_BORDER (wxFULL_REPAINT_ON_RESIZE | wxDOUBLE_BORDER)
#endif

BEGIN_EVENT_TABLE(VisualEditor,wxScrolledWindow)
	//EVT_SASH_DRAGGED(-1, VisualEditor::OnResizeBackPanel)
	//EVT_COMMAND(-1, wxEVT_PANEL_RESIZED, VisualEditor::OnResizeBackPanel)
	EVT_PANEL_RESIZED(-1, VisualEditor::OnResizeBackPanel)
	//EVT_PAINT(VisualEditor::OnPaintPanel)

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

	// Parece ser que han modificado el comportamiento en wxMSW 2.5.x ya que al
	// poner un color de background, este es aplicado a los hijos también.
	//SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	SetOwnBackgroundColour(wxColour(150,150,150));
	SetScrollRate(5, 5);

	//m_back = new DesignerWindow(this,-1,wxPoint(10,10),wxSize(350,200),0);//,VISUAL_EDITOR_BORDER);
	wxNativeInnerFrame* frame = new wxNativeInnerFrame( this, wxID_ANY, wxEmptyString, wxPoint( 10, 10 ), wxSize( 350, 200 ), wxDEFAULT_FRAME_STYLE );
	m_back = frame->GetFrameContentPanel();
	m_highlighter.SetWindow( m_back );
	m_back->PushEventHandler( &m_highlighter );
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

void VisualEditor::Setup()
{
#ifdef __WXFB_EXPERIMENTAL__
	EditorHandler *handler = new EditorHandler(AppData());
	handler->SetWindow(m_back);
	m_back->PushEventHandler(handler);
#endif //__WXFB_EXPERIMENTAL__

}

void VisualEditor::UpdateVirtualSize()
{
	int w, h, panelW, panelH;
	GetVirtualSize(&w, &h);
	m_back->GetSize(&panelW, &panelH);
	panelW += 20; panelH += 20;
	if (panelW != w || panelH != h) SetVirtualSize(panelW, panelH);
}

void VisualEditor::OnPaintPanel (wxPaintEvent &event)
{
	// es necesario esto para que se pinte el panel de oscuro
	// con wxGTK.
	wxPaintDC dc(this);
	//dc.SetBackground(wxBrush(wxColour(150,150,150),wxSOLID));
	dc.SetBackground(wxBrush(wxColour(192,192,192),wxSOLID));
	dc.Clear();
}

void VisualEditor::OnResizeBackPanel (wxCommandEvent &event) //(wxSashEvent &event)
{
	/*wxRect rect(event.GetDragRect());
	Debug::Print("VisualEditor::OnResizeBackPanel [%d,%d,%d,%d]",rect.x,rect.y,rect.width, rect.height);
	m_back->SetSize(rect.width,rect.height);
	m_back->Layout();*/

	shared_ptr<ObjectBase> form (AppData()->GetSelectedForm());

	if (form)
	{
		shared_ptr<Property> prop(form->GetProperty( wxT("size") ));
		if (prop)
		{
			wxString value(TypeConv::PointToString(wxPoint(m_back->GetSize().x, m_back->GetSize().y)));
			AppData()->ModifyProperty(prop, value);
		}
	}

	//event.Skip();
}

shared_ptr< ObjectBase > VisualEditor::GetObjectBase( wxObject* wxobject )
{
	if ( NULL == wxobject )
	{
		wxLogError( _("wxObject was NULL!") );
		return shared_ptr< ObjectBase >();
	}

	wxObjectMap::iterator obj = m_wxobjects.find( wxobject );
	if ( obj != m_wxobjects.end() )
	{
		return obj->second;
	}
	else
	{
		wxLogError( _("No corresponding ObjectBase for wxObject. Name: %s"), wxobject->GetClassInfo()->GetClassName() );
		return shared_ptr< ObjectBase >();
	}
}

wxObject* VisualEditor::GetWxObject( shared_ptr< ObjectBase > baseobject )
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
	m_highlighter.SetSelectedItem(NULL);
	m_highlighter.SetSelectedSizer(NULL);
	m_highlighter.SetSelectedObject(shared_ptr<ObjectBase>());
	m_back->DestroyChildren();
	m_back->SetSizer( NULL ); // *!*

	// Clear all associations between ObjectBase and wxObjects
	m_wxobjects.clear();
	m_baseobjects.clear();

	m_form = AppData()->GetSelectedForm();
	if ( m_form )
	{
		m_back->Show(true);

		// --- [1] Configure the size of the form ---------------------------
		shared_ptr<Property> pminsize( m_form->GetProperty( wxT("minimum_size") ) );
		if( pminsize)
		{
			wxSize minsize( TypeConv::StringToSize( pminsize->GetValue() ) );
			m_back->SetMinSize( minsize );
		}

		shared_ptr<Property> pmaxsize( m_form->GetProperty( wxT("maximum_size") ) );
		if( pmaxsize)
		{
			wxSize maxsize( TypeConv::StringToSize( pmaxsize->GetValue() ) );
			m_back->SetMaxSize( maxsize );
		}

		bool need_fit = false;
		shared_ptr<Property> psize( m_form->GetProperty( wxT("size") ) );
		if ( psize )
		{
			wxSize wsize( TypeConv::StringToSize( psize->GetValue() ) );
			m_back->SetSize( wsize );
			need_fit = ( wxDefaultSize == wsize );
		}
		else
		{
			m_back->SetSize( wxDefaultSize );
			need_fit = true;
		}

		// --- [2] Set the color of the form -------------------------------
		shared_ptr<Property> background( m_form->GetProperty( wxT("bg") ) );
		if ( background && !background->GetValue().empty() )
		{
			m_back->SetBackgroundColour( TypeConv::StringToColour( background->GetValue() ) );
		}
		else
		{
			if ( m_form->GetClassName() == wxT("Frame") )
			{
				m_back->SetOwnBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) );
			}
			else
			{
				m_back->SetOwnBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
			}
		}


		// --- [3] Title bar Setup
		/*if (  m_form->GetClassName() == wxT("Frame") || m_form->GetClassName() == wxT("Dialog") )
		{
			m_back->SetTitle(m_form->GetPropertyAsString(wxT("title")));
			shared_ptr<Property> style( m_form->GetProperty( wxT("style") ) );
			if ( style && !style->GetValue().empty() )
			{
				m_back->SetStyle( style->GetValueAsInteger() );
			}
			m_back->ShowTitleBar(true);
		}
		else
		  m_back->ShowTitleBar(false);
*/
		// --- [4] Create the components of the form -------------------------

		// Used to save frame objects for later display
		shared_ptr<ObjectBase> menubar;
		wxWindow* statusbar = NULL;
		wxWindow* toolbar = NULL;

		for ( unsigned int i = 0; i < m_form->GetChildCount(); i++ )
		{
			shared_ptr<ObjectBase> child = m_form->GetChild( i );

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
					Generate( child, m_back, NULL );
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

		if ( need_fit && m_back->GetSizer() )
		{
		  m_back->GetSizer()->Fit( m_back );
		}

		if ( menubar || statusbar || toolbar )
		{
//			m_back->SetFrameWidgets( menubar, toolbar, statusbar );
		}

		shared_ptr<Property> enabled( m_form->GetProperty( wxT("enabled") ) );
		if ( enabled )
		{
			m_back->Enable( TypeConv::StringToInt( enabled->GetValue() ) != 0 );
		}

		shared_ptr<Property> hidden( m_form->GetProperty( wxT("hidden") ) );
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
void VisualEditor::Generate( shared_ptr< ObjectBase > obj, wxWindow* wxparent, wxObject* parentObject )
{
	// Get Component
	shared_ptr< ObjectInfo > obj_info = obj->GetObjectInfo();
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

	// Associate the wxObject* with the shared_ptr< ObjectBase >
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

void VisualEditor::SetupSizer( shared_ptr< ObjectBase > obj, wxSizer* sizer )
{
	shared_ptr<Property> pminsize  = obj->GetProperty( wxT("minimum_size") );
	if (pminsize)
	{
		wxSize minsize = TypeConv::StringToSize( pminsize->GetValue() );
		sizer->SetMinSize( minsize );
		sizer->Layout();
	}
}

void VisualEditor::SetupWindow( shared_ptr< ObjectBase > obj, wxWindow* window )
{
	// All of the properties of the wxWindow object are applied in this function

	// Position
	wxPoint pos;
	shared_ptr< Property > ppos = obj->GetProperty( wxT("pos") );
	if ( ppos )
	{
		pos = TypeConv::StringToPoint( ppos->GetValue() );
	}

	// Size
	wxSize size;
	shared_ptr< Property > psize = obj->GetProperty( wxT("size") );
	if ( psize )
	{
		size = TypeConv::StringToSize(psize->GetValue());
	}

	window->SetSize( pos.x, pos.y, size.GetWidth(), size.GetHeight() );

	// Minimum size
	shared_ptr< Property > pminsize = obj->GetProperty( wxT("minimum_size") );
	if ( pminsize && !pminsize->GetValue().empty() )
	{
		window->SetMinSize( TypeConv::StringToSize( pminsize->GetValue() ) );
	}

	// Maximum size
	shared_ptr< Property > pmaxsize = obj->GetProperty( wxT("maximum_size") );
	if ( pmaxsize && !pmaxsize->GetValue().empty() )
	{
		window->SetMaxSize( TypeConv::StringToSize( pmaxsize->GetValue() ) );
	}

	// Font
	shared_ptr< Property > pfont = obj->GetProperty( wxT("font") );
	if ( pfont && !pfont->GetValue().empty() )
	{
		window->SetFont( TypeConv::StringToFont( pfont->GetValue() ) );
	}

	// Foreground
	shared_ptr< Property > pfg_colour = obj->GetProperty( wxT("fg") );
	if ( pfg_colour && !pfg_colour->GetValue().empty() )
	{
		window->SetForegroundColour( TypeConv::StringToColour( pfg_colour->GetValue() ) );
	}

	// Background
	shared_ptr< Property > pbg_colour = obj->GetProperty( wxT("bg") );
	if ( pbg_colour && !pbg_colour->GetValue().empty() )
	{
		window->SetBackgroundColour( TypeConv::StringToColour( pbg_colour->GetValue() ) );
	}

	// Extra Style
	shared_ptr< Property > pextra_style = obj->GetProperty( wxT("window_extra_style") );
	if ( pextra_style )
	{
		window->SetExtraStyle( TypeConv::StringToInt( pextra_style->GetValue() ) );
	}

	// Enabled
	shared_ptr< Property > penabled = obj->GetProperty( wxT("enabled") );
	if ( penabled )
	{
		window->Enable( ( penabled->GetValueAsInteger() !=0 ) );
	}

	// Hidden
	shared_ptr< Property > phidden = obj->GetProperty( wxT("hidden") );
	if ( phidden )
	{
		window->Show( !phidden->GetValueAsInteger() );
	}

	// Tooltip
	shared_ptr< Property > ptooltip = obj->GetProperty( wxT("tooltip") );
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
  Create();
}

void VisualEditor::OnObjectSelected( wxFBObjectEvent &event )
{
	// It is only necessary to Create() if the selected object is on a different form
	if ( AppData()->GetSelectedForm() != m_form )
	{
		Create();
	}

	// Get the ObjectBase from the event
	shared_ptr< ObjectBase > obj = event.GetFBObject();
	if ( !obj )
	{
		// Strange...
		wxLogDebug( _("The event object is NULL - why?") );
		return;
	}

	// Make sure this is a visible object
	ObjectBaseMap::iterator it = m_baseobjects.find( obj.get() );
	if ( m_baseobjects.end() == it )
	{
		m_highlighter.SetSelectedSizer( NULL );
		m_highlighter.SetSelectedItem( NULL );
		m_highlighter.SetSelectedObject( shared_ptr<ObjectBase>() );
		m_highlighter.SetSelectedPanel( NULL );
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
		shared_ptr< ObjectBase > parent = obj->GetParent();
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
	shared_ptr< ObjectBase > nextParent = obj->GetParent();
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
			selPanel = m_back;
		}
		else
		{
			selPanel = wxDynamicCast( it->second, wxWindow );
		}
	}
	else
	{
		selPanel = m_back;
	}

	// Find the first COMPONENT_TYPE_WINDOW or COMPONENT_TYPE_SIZER
	// If it is a sizer, save it
	wxSizer* sizer = NULL;
	shared_ptr< ObjectBase > nextObj = obj->GetParent();
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

	m_highlighter.SetSelectedSizer( sizer );
	m_highlighter.SetSelectedItem( item );
	m_highlighter.SetSelectedObject( obj );
	m_highlighter.SetSelectedPanel( selPanel );
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
		PObjectBase aux = m_highlighter.GetSelectedObject();
		Create();
		wxFBObjectEvent objEvent( wxEVT_FB_OBJECT_SELECTED, aux );
		this->ProcessEvent( objEvent );
		UpdateVirtualSize();
	}
}

void VisualEditor::OnProjectRefresh( wxFBEvent &event )
{
	Create();
}

//IMPLEMENT_CLASS( DesignerWindow, wxNativeInnerFrame )

//DesignerWindow::DesignerWindow( wxWindow *parent, int id, const wxPoint& pos, const wxSize &size, long style, const wxString &name )
//:
//wxNativeInnerFrame(parent, id, wxEmptyString, pos, size, style)
DesignerWindow::DesignerWindow( wxPanel* panel )
:
m_panel( panel )
{
//    ShowTitleBar(false);
//	SetGrid( 10, 10 );
//	m_selSizer = NULL;
//	m_selItem = NULL;
//	m_actPanel = NULL;
	//GetFrameContentPanel()->PushEventHandler(new HighlightPaintHandler(GetFrameContentPanel()));
//	m_panel->PushEventHandler( new HighlightPaintHandler( m_panel ) );
}

void HighlightPaintHandler::SetGrid( int x, int y )
{
	m_x = x;
	m_y = y;
}

void HighlightPaintHandler::DrawRectangle( wxDC& dc, const wxPoint& point, const wxSize& size, shared_ptr<ObjectBase> object )
{
	int border = object->GetParent()->GetPropertyAsInteger( wxT("border") );
	if ( border == 0 )
	{
		border = 1;
	}

	int flag = object->GetParent()->GetPropertyAsInteger( wxT("flag") );
	int topBorder = 	( flag & wxTOP ) 	== 0 ? 1 : border;
	int bottomBorder = 	( flag & wxBOTTOM ) == 0 ? 1 : border;
	int rightBorder = 	( flag & wxRIGHT ) 	== 0 ? 1 : border;
	int leftBorder = 	( flag & wxLEFT ) 	== 0 ? 1 : border;

	dc.DrawRectangle( 	point.x - leftBorder + 1,
						point.y - topBorder + 1,
						size.x + leftBorder + rightBorder - 2,
						size.y + topBorder + bottomBorder - 2 );

	dc.DrawRectangle( 	point.x - leftBorder,
						point.y - topBorder,
						size.x + leftBorder + rightBorder,
						size.y + topBorder + bottomBorder );
}

void HighlightPaintHandler::HighlightSelection( wxDC& dc )
{
	wxSize size;
	shared_ptr<ObjectBase> object = m_selObj.lock();
	if ( m_selSizer )
	{
		wxPoint point = m_selSizer->GetPosition();
		size = m_selSizer->GetSize();
		wxPen bluePen( *wxBLUE, 1, wxSOLID );
		dc.SetPen( bluePen );
		dc.SetBrush( *wxTRANSPARENT_BRUSH );
		shared_ptr<ObjectBase> sizerParent = object->FindNearAncestor( wxT("sizer") );
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

wxMenu* DesignerWindow::GetMenuFromObject(shared_ptr<ObjectBase> menu)
{
	int lastMenuId = wxID_HIGHEST + 1;
	wxMenu* menuWidget = new wxMenu();
	for ( unsigned int j = 0; j < menu->GetChildCount(); j++ )
	{
		shared_ptr< ObjectBase > menuItem = menu->GetChild( j );
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

void DesignerWindow::SetFrameWidgets(shared_ptr<ObjectBase> menubar, wxWindow *toolbar, wxWindow *statusbar)
{
/*  wxWindow *contentPanel = GetFrameContentPanel();
	Menubar *mbWidget = NULL;


	if ( menubar )
	{
		mbWidget = new Menubar(contentPanel, -1);
		for ( unsigned int i = 0; i < menubar->GetChildCount(); i++ )
		{
			shared_ptr< ObjectBase > menu = menubar->GetChild( i );
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
		dummySizer->Add(mainSizer, 1, wxEXPAND | wxALL, 0);
	else
		dummySizer->AddStretchSpacer(1);

	if (statusbar)
		dummySizer->Add(statusbar, 0, wxEXPAND | wxALL, 0);


	contentPanel->SetSizer(dummySizer, false);
	contentPanel->Layout();*/
}


BEGIN_EVENT_TABLE( HighlightPaintHandler, wxEvtHandler )
	EVT_PAINT( HighlightPaintHandler::OnPaint )
END_EVENT_TABLE()

HighlightPaintHandler::HighlightPaintHandler()
:
m_window( NULL )
{
}

HighlightPaintHandler::HighlightPaintHandler(wxWindow *win)
:
m_window( win )
{
}

void HighlightPaintHandler::OnPaint(wxPaintEvent &event)
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

	if ( m_window != NULL )
	{
		/*wxWindow *aux = m_window;
		while (!aux->IsKindOf(CLASSINFO(DesignerWindow))) aux = aux->GetParent();
		DesignerWindow *dsgnWin = (DesignerWindow*) aux;
		if (dsgnWin->GetActivePanel() == m_window)
		{*/
			wxPaintDC dc(m_window);
			HighlightSelection(dc);
		//}
	}
	event.Skip();
}

