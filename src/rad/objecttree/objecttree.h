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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// Written by
//   José Antonio Hurtado - joseantonio.hurtado@gmail.com
//   Juan Antonio Ortega  - jortegalalmolda@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OBJECT_TREE__
#define __OBJECT_TREE__

#include "utils/wxfbdefs.h"
#include <map>
#include "rad/customkeys.h"

#include <wx/treectrl.h>

class wxFBEvent;
class wxFBPropertyEvent;
class wxFBObjectEvent;

class ObjectTree : public wxPanel
{
private:
   typedef std::map< PObjectBase, wxTreeItemId> ObjectItemMap;
   typedef std::map<wxString, int> IconIndexMap;

   ObjectItemMap m_map;

   wxImageList *m_iconList;
   IconIndexMap m_iconIdx;

   wxTreeCtrl* m_tcObjects;

   wxTreeItemId m_draggedItem;

   bool m_altKeyIsDown;

   /**
    * Crea el arbol completamente.
    */
   void RebuildTree();
   void AddChildren(PObjectBase child, wxTreeItemId &parent, bool is_root = false);
   int GetImageIndex (wxString type);
   void UpdateItem(wxTreeItemId id, PObjectBase obj);
   void RestoreItemStatus(PObjectBase obj);
   void AddItem(PObjectBase item, PObjectBase parent);
   void RemoveItem(PObjectBase item);
   void ClearMap(PObjectBase obj);

   PObjectBase GetObjectFromTreeItem( wxTreeItemId item );

   DECLARE_EVENT_TABLE()

public:
  ObjectTree(wxWindow *parent, int id);
  ~ObjectTree();
  void Create();

  void OnSelChanged(wxTreeEvent &event);
  void OnRightClick(wxTreeEvent &event);
  void OnBeginDrag(wxTreeEvent &event);
  void OnEndDrag(wxTreeEvent &event);
  void OnExpansionChange(wxTreeEvent &event);

  void OnProjectLoaded ( wxFBEvent &event );
  void OnProjectSaved  ( wxFBEvent &event );
  void OnObjectExpanded( wxFBObjectEvent& event );
  void OnObjectSelected( wxFBObjectEvent &event );
  void OnObjectCreated ( wxFBObjectEvent &event );
  void OnObjectRemoved ( wxFBObjectEvent &event );
  void OnPropertyModified ( wxFBPropertyEvent &event );
  void OnProjectRefresh ( wxFBEvent &event);
  void OnKeyDown ( wxTreeEvent &event);

  void AddCustomKeysHandler(CustomKeysEvtHandler *h) { m_tcObjects->PushEventHandler(h); };
};

/**
 * Gracias a que podemos asociar un objeto a cada item, esta clase nos va
 * a facilitar obtener el objeto (ObjectBase) asociado a un item para
 * seleccionarlo pinchando en el item.
 */
class ObjectTreeItemData : public wxTreeItemData
{
 private:
  PObjectBase m_object;
 public:
  ObjectTreeItemData(PObjectBase obj);
  PObjectBase GetObject() { return m_object; }
};

/**
 * Menu popup asociado a cada item del arbol.
 *
 * Este objeto ejecuta los comandos incluidos en el menu referentes al objeto
 * seleccionado.
 */
class ItemPopupMenu : public wxMenu
{
 private:
  PObjectBase m_object;

  DECLARE_EVENT_TABLE()

 public:
  void OnUpdateEvent(wxUpdateUIEvent& e);
  ItemPopupMenu(PObjectBase obj);
  void OnMenuEvent (wxCommandEvent & event);
};

#endif //__OBJECT_TREE__
