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
// Written by José Antonio Hurtado - joseantonio.hurtado@hispalinux.es
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OBJ_INSPECT__
#define __OBJ_INSPECT__

#include "wx/wx.h"
#include "wx/grid.h"
#include <wx/wxFlatNotebook/wxFlatNotebook.h>

#include "propeditor.h"
#include "rad/appobserver.h"

class ObjectInspector : public wxPanel, public DataObserver
{
 private:
  typedef map<shared_ptr<Property>,PropertyEditor *> ObjInspectorMap;
  ObjInspectorMap m_map;

  shared_ptr<ObjectBase> m_currentSel;
  wxFlatNotebook *m_notebook;

  void CreatePropertyPanel( const wxString& name, shared_ptr< ObjectBase > obj, shared_ptr< ObjectInfo > obj_info, map< string, shared_ptr< Property > >& map );

  void Create();
 protected:

 public:
  ObjectInspector(wxWindow *parent, int id);

  void ProjectLoaded();
  void ProjectSaved();
  void ObjectSelected(shared_ptr<ObjectBase> obj);
  void ObjectCreated(shared_ptr<ObjectBase> obj);
  void ObjectRemoved(shared_ptr<ObjectBase> obj);
  void PropertyModified(shared_ptr<Property> prop);
};

#endif //__OBJ_INSPECT__
