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

#include "objecttree.h"
#include "rad/bitmaps.h"
#include "utils/debug.h"
#include <wx/imaglist.h>
#include <wx/image.h>
#include "rad/menueditor.h"
#include "utils/typeconv.h"
#include "rad/wxfbevent.h"
#include <rad/appdata.h>
#include "model/objectbase.h"
#include <wx/utils.h>

BEGIN_EVENT_TABLE( ObjectTree, wxPanel )
	EVT_TREE_SEL_CHANGED( -1, ObjectTree::OnSelChanged )
	EVT_TREE_ITEM_RIGHT_CLICK( -1, ObjectTree::OnRightClick )
	EVT_TREE_BEGIN_DRAG( -1, ObjectTree::OnBeginDrag )
	EVT_TREE_END_DRAG( -1, ObjectTree::OnEndDrag )

	EVT_FB_PROJECT_LOADED( ObjectTree::OnProjectLoaded )
	EVT_FB_PROJECT_SAVED( ObjectTree::OnProjectSaved )
	EVT_FB_OBJECT_CREATED( ObjectTree::OnObjectCreated )
	EVT_FB_OBJECT_REMOVED( ObjectTree::OnObjectRemoved )
	EVT_FB_PROPERTY_MODIFIED( ObjectTree::OnPropertyModified )
	EVT_FB_PROJECT_REFRESH( ObjectTree::OnProjectRefresh )

END_EVENT_TABLE()

ObjectTree::ObjectTree( wxWindow *parent, int id )
:
wxPanel( parent, id )
{
	AppData()->AddHandler( this->GetEventHandler() );
	m_tcObjects = new wxTreeCtrl(this, -1, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxTR_LINES_AT_ROOT|wxTR_DEFAULT_STYLE|wxSIMPLE_BORDER);

	wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    sizer_1->Add(m_tcObjects, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    sizer_1->Fit(this);
    sizer_1->SetSizeHints(this);
    Connect( wxID_ANY, wxEVT_FB_OBJECT_EXPANDED, wxFBObjectEventHandler( ObjectTree::OnObjectExpanded ) );
    Connect( wxID_ANY, wxEVT_FB_OBJECT_SELECTED, wxFBObjectEventHandler( ObjectTree::OnObjectSelected ) );
    Connect( wxID_ANY, wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( ObjectTree::OnExpansionChange ) );
    Connect( wxID_ANY, wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEventHandler( ObjectTree::OnExpansionChange ) );
}

ObjectTree::~ObjectTree()
{
	AppData()->RemoveHandler( this->GetEventHandler() );
}

PObjectBase ObjectTree::GetObjectFromTreeItem( wxTreeItemId item )
{
	if ( item.IsOk() )
	{
		wxTreeItemData* item_data = m_tcObjects->GetItemData( item );
		if ( item_data )
		{
			PObjectBase obj( ((ObjectTreeItemData* )item_data)->GetObject() );
			return obj;
		}
	}
	return PObjectBase( (ObjectBase*)NULL );
}

void ObjectTree::RebuildTree()
{
	m_tcObjects->Freeze();
	
	Disconnect( wxID_ANY, wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( ObjectTree::OnExpansionChange ) );
	Disconnect( wxID_ANY, wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEventHandler( ObjectTree::OnExpansionChange ) );

	PObjectBase project = AppData()->GetProjectData();

	// Clear the old tree and map
	m_tcObjects->DeleteAllItems();
	m_map.clear();

	if (project)
	{
		wxTreeItemId dummy;
		AddChildren(project, dummy, true );

		// Expand items that were previously expanded
		//RestoreItemStatus(project);
	}

	Connect( wxID_ANY, wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEventHandler( ObjectTree::OnExpansionChange ) );
	Connect( wxID_ANY, wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( ObjectTree::OnExpansionChange ) );
	
	m_tcObjects->Expand( m_map.find( project )->second );
	
	m_tcObjects->Thaw();
}

void ObjectTree::OnSelChanged(wxTreeEvent &event)
{
	wxTreeItemId id = event.GetItem();
	if (!id.IsOk()) return;

	// Make selected items bold
	wxTreeItemId oldId = event.GetOldItem();
	if ( oldId.IsOk() )
	{
		m_tcObjects->SetItemBold( oldId, false );
	}
	m_tcObjects->SetItemBold( id );

	wxTreeItemData *item_data = m_tcObjects->GetItemData(id);

	if (item_data)
	{
		PObjectBase obj(((ObjectTreeItemData *)item_data)->GetObject());
		assert(obj);
		Disconnect( wxID_ANY, wxEVT_FB_OBJECT_SELECTED, wxFBObjectEventHandler( ObjectTree::OnObjectSelected ) );
		AppData()->SelectObject(obj);
		Connect( wxID_ANY, wxEVT_FB_OBJECT_SELECTED, wxFBObjectEventHandler( ObjectTree::OnObjectSelected ) );
	}
}

void ObjectTree::OnRightClick(wxTreeEvent &event)
{
	wxTreeItemId id = event.GetItem();
	wxTreeItemData *item_data = m_tcObjects->GetItemData(id);
	if (item_data)
	{
		PObjectBase obj(((ObjectTreeItemData *)item_data)->GetObject());
		assert(obj);
		wxMenu * menu = new ItemPopupMenu(obj);
		wxPoint pos = event.GetPoint();
		menu->UpdateUI(menu);
		PopupMenu(menu,pos.x, pos.y);
	}
}

void ObjectTree::OnBeginDrag(wxTreeEvent &event)
{
	// need to explicitly allow drag
    if ( event.GetItem() == m_tcObjects->GetRootItem() )
    {
    	return;
    }

	m_draggedItem = event.GetItem();
	event.Allow();
}

void ObjectTree::OnEndDrag(wxTreeEvent& event)
{
	bool copy = ::wxGetKeyState( WXK_CONTROL );

    wxTreeItemId itemSrc = m_draggedItem,
                 itemDst = event.GetItem();
    m_draggedItem = (wxTreeItemId)0l;

	// ensure that itemDst is not itemSrc or a child of itemSrc
	wxTreeItemId item = itemDst;
	while ( item.IsOk() )
	{
		if ( item == itemSrc )
		{
			return;
		}
		item = m_tcObjects->GetItemParent( item );
	}

	PObjectBase objSrc = GetObjectFromTreeItem( itemSrc );
	if ( !objSrc )
	{
		return;
	}

	PObjectBase objDst = GetObjectFromTreeItem( itemDst );
	if ( !objDst )
	{
		return;
	}

	// backup clipboard
	PObjectBase clipboard = AppData()->GetClipboardObject();

	// set object to clipboard
	if ( copy )
	{
		AppData()->CopyObject( objSrc );
	}
	else
	{
		AppData()->CutObject( objSrc );
	}

	if ( !AppData()->PasteObject( objDst ) && !copy )
	{
		AppData()->Undo();
	}

	AppData()->SetClipboardObject( clipboard );
}

void ObjectTree::OnExpansionChange(wxTreeEvent &event)
{
	wxTreeItemId id = event.GetItem();
	wxTreeItemData *item_data = m_tcObjects->GetItemData(id);
	if (item_data)
	{
		PObjectBase obj(((ObjectTreeItemData *)item_data)->GetObject());
		assert(obj);
		Disconnect( wxID_ANY, wxEVT_FB_OBJECT_EXPANDED, wxFBObjectEventHandler( ObjectTree::OnObjectExpanded ) );
		AppData()->ExpandObject( obj, m_tcObjects->IsExpanded( id ) );
		Connect( wxID_ANY, wxEVT_FB_OBJECT_EXPANDED, wxFBObjectEventHandler( ObjectTree::OnObjectExpanded ) );
	}
}

void ObjectTree::AddChildren(PObjectBase obj, wxTreeItemId &parent, bool is_root)
{
	if (obj->GetObjectInfo()->GetObjectType()->IsItem())
	{
		if (obj->GetChildCount() > 0)
			AddChildren(obj->GetChild(0),parent);
		else
		{
			// Si hemos llegado aquí ha sido porque el arbol no está bien formado
			// y habrá que revisar cómo se ha creado.
			wxString msg;
			PObjectBase itemParent = obj->GetParent();
			assert(parent);

			msg = wxString::Format(wxT("Item without object as child of \'%s:%s\'"),
				itemParent->GetPropertyAsString(wxT("name")).c_str(),
				itemParent->GetClassName().c_str());

			wxLogError(msg);
		}
	}
	else
	{
		wxTreeItemId new_parent;

		ObjectTreeItemData *item_data = new ObjectTreeItemData(obj);

		if (is_root)
			new_parent = m_tcObjects->AddRoot(wxT(""),-1,-1,item_data);
		else
			new_parent = m_tcObjects->AppendItem(parent,wxT(""),-1,-1,item_data);

		// Add the item to the map
		m_map.insert( ObjectItemMap::value_type( obj, new_parent ) );

		// Set the image
		int image_idx = GetImageIndex( obj->GetObjectInfo()->GetClassName() );

		m_tcObjects->SetItemImage(new_parent,image_idx);

		// Set the name
		UpdateItem( new_parent, obj );
		
		// Add the rest of the children
		unsigned int count = obj->GetChildCount();
		unsigned int i;
		for (i = 0; i < count ; i++)
		{
			PObjectBase child = obj->GetChild(i);
			AddChildren(child, new_parent);
		}
	}
}


int ObjectTree::GetImageIndex (wxString name)
{
	int index = 0; //default icon

	IconIndexMap::iterator it = m_iconIdx.find(name);
	if (it != m_iconIdx.end())
		index = it->second;

	return index;
}

void ObjectTree::UpdateItem(wxTreeItemId id, PObjectBase obj)
{
	// mostramos el nombre
	wxString class_name( obj->GetClassName() );
	PProperty prop = obj->GetProperty( wxT("name") );
	wxString obj_name;
	if (prop)
	{
		obj_name =prop->GetValue();
	}

	wxString text = obj_name + wxT(" : ") + class_name;

	// actualizamos el item
	m_tcObjects->SetItemText(id,text);
}

void ObjectTree::Create()
{
	// Cramos la lista de iconos obteniendo los iconos de los paquetes.
	unsigned int index = 0;
	m_iconList = new wxImageList(ICON_SIZE,ICON_SIZE);

	{
		wxBitmap icon = AppBitmaps::GetBitmap(wxT("project"), ICON_SIZE);
		m_iconList->Add(icon);
		m_iconIdx.insert(IconIndexMap::value_type( wxT("_default_"),index++));
	}

	unsigned int pkg_count = AppData()->GetPackageCount();
	for (unsigned int i = 0; i< pkg_count;i++)
	{
		PObjectPackage pkg = AppData()->GetPackage(i);

		unsigned int j;
		for (j=0;j<pkg->GetObjectCount();j++)
		{
			wxString comp_name(pkg->GetObjectInfo(j)->GetClassName());
			m_iconList->Add( pkg->GetObjectInfo(j)->GetIconFile() );
			m_iconIdx.insert(IconIndexMap::value_type(comp_name,index++));
		}
	}

	m_tcObjects->AssignImageList(m_iconList);
}

void ObjectTree::RestoreItemStatus(PObjectBase obj)
{
	ObjectItemMap::iterator item_it = m_map.find(obj);
	if (item_it != m_map.end())
	{
		wxTreeItemId id = item_it->second;

		if ( obj->GetExpanded() )
			m_tcObjects->Expand(id);
		else
			m_tcObjects->Collapse(id);
	}

	unsigned int i,count = obj->GetChildCount();
	for (i = 0; i<count ; i++)
		RestoreItemStatus(obj->GetChild(i));
}

void ObjectTree::AddItem(PObjectBase item, PObjectBase parent)
{
	if( item && parent )
	{
		// find parent item displayed in the object tree
		while( parent && parent->GetObjectInfo()->GetObjectType()->IsItem() ) parent = parent->GetParent();
		
		// add new item to the object tree
		ObjectItemMap::iterator it = m_map.find( parent );
		if( (it != m_map.end()) && it->second.IsOk() )
		{
			AddChildren( item, it->second, false );
		}
	}
}

void ObjectTree::RemoveItem(PObjectBase item)
{
	// remove affected object tree items only
	ObjectItemMap::iterator it = m_map.find( item );
	if( (it != m_map.end()) && it->second.IsOk() )
	{
		m_tcObjects->Delete( it->second );
		// clear map records for all item's children
		ClearMap( it->first );
	}
}

void ObjectTree::ClearMap(PObjectBase obj)
{
	m_map.erase( obj );
	
	for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
	{
		ClearMap( obj->GetChild( i ) );
	}
}

/////////////////////////////////////////////////////////////////////////////
// wxFormBuilder Event Handlers
/////////////////////////////////////////////////////////////////////////////
void ObjectTree::OnProjectLoaded( wxFBEvent &)
{
	RebuildTree();
}

void ObjectTree::OnProjectSaved( wxFBEvent &)
{
}

void ObjectTree::OnObjectExpanded( wxFBObjectEvent& event )
{
	PObjectBase obj = event.GetFBObject();
	ObjectItemMap::iterator it = m_map.find( obj );
	if ( it != m_map.end() )
	{
		if ( m_tcObjects->IsExpanded( it->second ) != obj->GetExpanded() )
		{
			if ( obj->GetExpanded() )
			{
				m_tcObjects->Expand( it->second );
			}
			else
			{
				m_tcObjects->Collapse( it->second );
			}
		}
	}
}

void ObjectTree::OnObjectSelected( wxFBObjectEvent &event )
{
  PObjectBase obj = event.GetFBObject();

	// Find the tree item associated with the object and select it
	ObjectItemMap::iterator it = m_map.find(obj);
	if ( it != m_map.end() )
	{
		// Ignore expand/collapse events
		Disconnect( wxID_ANY, wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( ObjectTree::OnExpansionChange ) );
		Disconnect( wxID_ANY, wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEventHandler( ObjectTree::OnExpansionChange ) );

		m_tcObjects->EnsureVisible( it->second );
		m_tcObjects->SelectItem( it->second );

		// Restore event handling
		Connect( wxID_ANY, wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( ObjectTree::OnExpansionChange ) );
		Connect( wxID_ANY, wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEventHandler( ObjectTree::OnExpansionChange ) );
	}
	else
	{
		wxLogError( wxT("There is no tree item associated with this object.\n\tClass: %s\n\tName: %s"), obj->GetClassName().c_str(), obj->GetPropertyAsString(wxT("name")).c_str() );
	}
}

void ObjectTree::OnObjectCreated ( wxFBObjectEvent &event )
{
	//RebuildTree();
	
	AddItem( event.GetFBObject(), event.GetFBObject()->GetParent() );
}

void ObjectTree::OnObjectRemoved ( wxFBObjectEvent &event )
{
	//RebuildTree();
	
	RemoveItem( event.GetFBObject() );
}

void ObjectTree::OnPropertyModified ( wxFBPropertyEvent &event )
{
  PProperty prop = event.GetFBProperty();

	if (prop->GetName() == wxT("name") )
	{
		ObjectItemMap::iterator it = m_map.find(prop->GetObject());
		if (it != m_map.end())
		{
			UpdateItem(it->second,it->first);
		}
	}
}

void ObjectTree::OnProjectRefresh ( wxFBEvent &)
{
	RebuildTree();
}

///////////////////////////////////////////////////////////////////////////////

ObjectTreeItemData::ObjectTreeItemData(PObjectBase obj) : m_object(obj)
{}

///////////////////////////////////////////////////////////////////////////////

#define MENU_MOVE_UP    100
#define MENU_MOVE_DOWN  101
#define MENU_MOVE_RIGHT 102
#define MENU_MOVE_LEFT  103
#define MENU_CUT        104
#define MENU_PASTE      105
#define MENU_EDIT_MENUS 106
#define MENU_COPY       107
#define MENU_MOVE_NEW_BOXSIZER   108
#define MENU_DELETE 109


BEGIN_EVENT_TABLE(ItemPopupMenu,wxMenu)
	EVT_MENU(-1, ItemPopupMenu::OnMenuEvent)
	EVT_UPDATE_UI(-1, ItemPopupMenu::OnUpdateEvent)
END_EVENT_TABLE()

ItemPopupMenu::ItemPopupMenu(PObjectBase obj)
: wxMenu(), m_object(obj)
{
	Append(MENU_CUT,        wxT("Cut\tCtrl+X"));
	Append(MENU_COPY,       wxT("Copy\tCtrl+C"));
	Append(MENU_PASTE,      wxT("Paste\tCtrl+V"));
	AppendSeparator();
	Append(MENU_DELETE,     wxT("Delete\tCtrl+D"));
	AppendSeparator();
	Append(MENU_MOVE_UP,    wxT("Move Up\tAlt+Up"));
	Append(MENU_MOVE_DOWN,  wxT("Move Down\tAlt+Down"));
	Append(MENU_MOVE_LEFT,  wxT("Move Left\tAlt+Left"));
	Append(MENU_MOVE_RIGHT, wxT("Move Right\tAlt+Right"));
	AppendSeparator();
	Append(MENU_MOVE_NEW_BOXSIZER, wxT("Move into a new wxBoxSizer"));
	AppendSeparator();
	Append(MENU_EDIT_MENUS, wxT("Menu Editor..."));
}

void ItemPopupMenu::OnMenuEvent (wxCommandEvent & event)
{
	int id = event.GetId();

	switch (id)
	{
	case MENU_CUT:
		AppData()->CutObject(m_object);
		break;
	case MENU_COPY:
		AppData()->CopyObject(m_object);
		break;
	case MENU_PASTE:
		AppData()->PasteObject(m_object);
		break;
	case MENU_DELETE:
		AppData()->RemoveObject(m_object);
		break;
	case MENU_MOVE_UP:
		AppData()->MovePosition(m_object,false);
		break;
	case MENU_MOVE_DOWN:
		AppData()->MovePosition(m_object,true);
		break;
	case MENU_MOVE_RIGHT:
		AppData()->MoveHierarchy(m_object,false);
		break;
	case MENU_MOVE_LEFT:
		AppData()->MoveHierarchy(m_object,true);
		break;
	case MENU_MOVE_NEW_BOXSIZER:
		AppData()->CreateBoxSizerWithObject(m_object);
		break;
	case MENU_EDIT_MENUS:
		{
			PObjectBase obj = m_object;
			if (obj && (obj->GetClassName() == wxT("wxMenuBar") || obj->GetClassName() == wxT("Frame") ) )
			{
				MenuEditor me(NULL);
				if (obj->GetClassName() == wxT("Frame") )
				{
					bool found = false;
					PObjectBase menubar;
					for (unsigned int i = 0; i < obj->GetChildCount() && !found; i++)
					{
						menubar = obj->GetChild(i);
						found = menubar->GetClassName() == wxT("wxMenuBar");
					}
					if (found) obj = menubar;
				}

				if (obj->GetClassName() == wxT("wxMenuBar")) me.Populate(obj);
				if (me.ShowModal() == wxID_OK)
				{
					if (obj->GetClassName() == wxT("wxMenuBar"))
					{
						PObjectBase menubar = me.GetMenubar(AppData()->GetObjectDatabase());
						while (obj->GetChildCount() > 0)
						{
							PObjectBase child = obj->GetChild(0);
							obj->RemoveChild(0);
							child->SetParent(PObjectBase());
						}
						for (unsigned int i = 0; i < menubar->GetChildCount(); i++)
						{
							PObjectBase child = menubar->GetChild(i);
							AppData()->InsertObject(child,obj);
						}
					}
					else
						AppData()->InsertObject(me.GetMenubar(AppData()->GetObjectDatabase()),AppData()->GetSelectedForm());
				}
			}
		}
		break;
	default:
		break;
	}
}

void ItemPopupMenu::OnUpdateEvent(wxUpdateUIEvent& e)
{
	switch (e.GetId())
	{
	case MENU_EDIT_MENUS:
		e.Enable(m_object && (m_object->GetClassName() == wxT("wxMenuBar" )
			|| m_object->GetClassName() == wxT("Frame")));
		break;
	case MENU_CUT:
	case MENU_COPY:
	case MENU_DELETE:
	case MENU_MOVE_UP:
	case MENU_MOVE_DOWN:
	case MENU_MOVE_LEFT:
	case MENU_MOVE_RIGHT:
	case MENU_MOVE_NEW_BOXSIZER:
		e.Enable(AppData()->CanCopyObject());
		break;
	case MENU_PASTE:
		e.Enable(AppData()->CanPasteObject());
		break;
	}
}


