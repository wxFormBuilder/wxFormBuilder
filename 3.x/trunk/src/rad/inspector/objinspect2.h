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

#ifndef __OBJ_INSPECT__
#define __OBJ_INSPECT__

#include "wx/wx.h"
#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/propdev.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>
#include <model/objectbase.h>

// -----------------------------------------------------------------------

WX_PG_DECLARE_CUSTOM_COLOUR_PROPERTY(fbColourProperty)

// -----------------------------------------------------------------------

WX_PG_DECLARE_VALUE_TYPE_VOIDP(wxPoint)

WX_PG_DECLARE_PROPERTY(wxPointProperty,const wxPoint&,wxPoint(0,0))

// -----------------------------------------------------------------------

WX_PG_DECLARE_VALUE_TYPE_VOIDP(wxSize)

WX_PG_DECLARE_PROPERTY(wxSizeProperty,const wxSize&,wxSize(0,0))

// -----------------------------------------------------------------------

WX_PG_DECLARE_PROPERTY( wxBitmapWithResourceProperty, const wxString&, wxEmptyString )

// -----------------------------------------------------------------------

DECLARE_LOCAL_EVENT_TYPE( wxEVT_NEW_BITMAP_PROPERTY, -1 )

class wxFBPropertyEvent;
class wxFBObjectEvent;
class wxFBEvent;

enum {
  wxFB_OI_DEFAULT_STYLE,
  wxFB_OI_MULTIPAGE_STYLE,
  wxFB_OI_SINGLE_PAGE_STYLE
};

class ObjectInspector : public wxPanel
{
 private:
  typedef std::map< wxPGProperty*, PProperty> ObjInspectorPropertyMap;
  typedef std::map< wxPGProperty*, PEvent> ObjInspectorEventMap;

  ObjInspectorPropertyMap m_propMap;
  ObjInspectorEventMap m_eventMap;

  PObjectBase m_currentSel;
  wxFlatNotebook* m_nb;
  wxPropertyGridManager* m_pg;
  wxPropertyGridManager* m_eg;
  int m_style;

  int StringToBits(const wxString& strVal, wxPGChoices& constants);
  void CreateCategory( const wxString& name, shared_ptr< ObjectBase > obj, shared_ptr< ObjectInfo > obj_info, map< wxString, shared_ptr< Property > >& map );
  void AddProperties( const wxString& name, shared_ptr< ObjectBase > obj, shared_ptr< ObjectInfo > obj_info, shared_ptr< PropertyCategory > category, map< wxString, shared_ptr< Property > >& map );
  void AddEvents(PObjectBase obj, PObjectInfo obj_info );
  wxPGProperty* GetProperty(shared_ptr<Property> prop);

  void Create(bool force = false);

  void OnPropertyGridChange(wxPropertyGridEvent& event);
  void OnEventGridChange(wxPropertyGridEvent& event);
  void OnNewBitmapProperty( wxCommandEvent& event );

 protected:

 public:
  ObjectInspector(wxWindow *parent, int id, int style = wxFB_OI_DEFAULT_STYLE);
  ~ObjectInspector();

  void OnObjectSelected( wxFBObjectEvent& event );
  void OnProjectRefresh( wxFBEvent& event );
  void OnPropertyModified( wxFBPropertyEvent& event );

  wxPropertyGridManager* CreatePropertyGridManager(wxWindow *parent, wxWindowID id);
  void SavePosition();

  DECLARE_EVENT_TABLE()
};

class NewBitmapEventDataHolder
{
public:
	shared_ptr<Property> m_prop;
	wxPropertyGrid* m_grid;
	wxString m_string;

};

#endif //__OBJ_INSPECT__
