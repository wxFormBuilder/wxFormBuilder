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

#ifndef __OBJECT_TREE__
#define __OBJECT_TREE__

#include "model/objectbase.h"
#include <map>
#include "rad/customkeys.h"

#include <wx/treectrl.h>

class wxFBEvent;
class wxFBPropertyEvent;
class wxFBObjectEvent;

class ObjectTree : public wxPanel
{
private:
   typedef map<shared_ptr<ObjectBase>, wxTreeItemId> ObjectItemMap;
   typedef map<wxString, int> IconIndexMap;
   typedef map<shared_ptr<ObjectBase>, bool> ItemExpandedMap;

   ObjectItemMap m_map;
   ItemExpandedMap m_expandedMap;

   wxImageList *m_iconList;
   IconIndexMap m_iconIdx;

   wxTextCtrl* m_txtSelected;
   wxTreeCtrl* m_tcObjects;

   /**
    * Crea el arbol completamente.
    */
   void RebuildTree();
   void AddChildren(shared_ptr<ObjectBase> child, wxTreeItemId &parent, bool is_root = false);
   int GetImageIndex (wxString type);
   void UpdateItem(wxTreeItemId id, shared_ptr<ObjectBase> obj);

   void SaveItemStatus(shared_ptr<ObjectBase> obj);
   void RestoreItemStatus(shared_ptr<ObjectBase> obj);

   DECLARE_EVENT_TABLE()

public:
  ObjectTree(wxWindow *parent, int id);
  ~ObjectTree();
  void Create();

  void OnSelChanged(wxTreeEvent &event);
  void OnRightClick(wxTreeEvent &event);

  void OnProjectLoaded ( wxFBEvent &event );
  void OnProjectSaved  ( wxFBEvent &event );
  void OnObjectSelected( wxFBObjectEvent &event );
  void OnObjectCreated ( wxFBObjectEvent &event );
  void OnObjectRemoved ( wxFBObjectEvent &event );
  void OnPropertyModified ( wxFBPropertyEvent &event );
  void OnProjectRefresh ( wxFBEvent &event);

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
  shared_ptr<ObjectBase> m_object;
 public:
  ObjectTreeItemData(shared_ptr<ObjectBase> obj);
  shared_ptr<ObjectBase> GetObject() { return m_object; }
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
  shared_ptr<ObjectBase> m_object;

  DECLARE_EVENT_TABLE()

 public:
  void OnUpdateEvent(wxUpdateUIEvent& e);
  ItemPopupMenu(shared_ptr<ObjectBase> obj);
  void OnMenuEvent (wxCommandEvent & event);
};

#endif //__OBJECT_TREE__
