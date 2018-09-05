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
// Modified by
//   Andrea Zanellato - zanellato.andrea@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OBJ_INSPECT__
#define __OBJ_INSPECT__

#include "../../model/objectbase.h"

#include <wx/aui/auibook.h>
#include <wx/propgrid/manager.h>

#if !wxUSE_PROPGRID
#error "wxUSE_PROPGRID must be set to 1 in your wxWidgets library."
#endif

class wxFBEventHandlerEvent;
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

	//save the current selected property
	wxString m_strSelPropItem;
	wxString m_pageName;

	wxAuiNotebook* m_nb;

    wxPropertyGridManager* m_pg;
    wxPropertyGridManager* m_eg;

    int m_style;

    int StringToBits( const wxString& strVal, wxPGChoices& constants );

    typedef std::map< wxString, bool > ExpandMap;
    ExpandMap m_isExpanded;

    template < class ValueT >
        void CreateCategory( const wxString& name, PObjectBase obj, PObjectInfo obj_info, std::map< wxString, ValueT >& itemMap, bool addingEvents )
    {
        // Get Category
        PPropertyCategory category = obj_info->GetCategory();
        if ( !category )
        {
            return;
        }

        // Prevent page creation if there are no properties
        if ( 0 == category->GetCategoryCount() && 0 == ( addingEvents ? category->GetEventCount() : category->GetPropertyCount() ) )
        {
            return;
        }

        wxString pageName;

        if ( m_style == wxFB_OI_MULTIPAGE_STYLE )
            pageName = name;
        else
            pageName = wxT("default");


        wxPropertyGridManager* pg = ( addingEvents ? m_eg : m_pg );
        int pageIndex = pg->GetPageByName( pageName );
        if ( wxNOT_FOUND == pageIndex )
        {
            pg->AddPage( pageName, obj_info->GetSmallIconFile() );
        }

        const wxString& catName = category->GetName();

        wxPGProperty* id = pg->Append( new wxPropertyCategory( catName ) );

        AddItems( name, obj, obj_info, category, itemMap );

        ExpandMap::iterator it = m_isExpanded.find(catName);
        if (it != m_isExpanded.end())
        {
            if ( it->second )
            {
                pg->Expand( id );
            }
            else
            {
                pg->Collapse( id );
            }
        }

        pg->SetPropertyAttributeAll( wxPG_BOOL_USE_CHECKBOX, (long)1 );
    }

    void AddItems( const wxString& name, PObjectBase obj, PObjectInfo obj_info, PPropertyCategory category, PropertyMap& map );
    void AddItems( const wxString& name, PObjectBase obj, PObjectInfo obj_info, PPropertyCategory category, EventMap& map );
    wxPGProperty* GetProperty( PProperty prop );

    void Create( bool force = false );

    void OnPropertyGridChanging( wxPropertyGridEvent& event );
    void OnPropertyGridChanged( wxPropertyGridEvent& event );
    void OnEventGridChanged( wxPropertyGridEvent& event );
    void OnPropertyGridDblClick( wxPropertyGridEvent& event );
    void OnEventGridDblClick( wxPropertyGridEvent& event );
    void OnPropertyGridExpand( wxPropertyGridEvent& event );
    void OnEventGridExpand( wxPropertyGridEvent& event );
	void OnPropertyGridItemSelected( wxPropertyGridEvent& event );
    void OnReCreateGrid( wxCommandEvent& event );
	void OnBitmapPropertyChanged( wxCommandEvent& event );

	void RestoreLastSelectedPropItem();

	void ModifyProperty( PProperty prop, const wxString& str );

	void OnChildFocus( wxChildFocusEvent& event );

public:
    ObjectInspector(wxWindow *parent, int id, int style = wxFB_OI_DEFAULT_STYLE);
	~ObjectInspector() override;

    void OnObjectSelected( wxFBObjectEvent& event );
    void OnProjectRefresh( wxFBEvent& event );
    void OnPropertyModified( wxFBPropertyEvent& event );
    void OnEventHandlerModified( wxFBEventHandlerEvent& event );

    void AutoGenerateId( PObjectBase objectChanged, PProperty propChanged, wxString reason );
    wxPropertyGridManager* CreatePropertyGridManager( wxWindow *parent, wxWindowID id );
    void SavePosition();

    DECLARE_EVENT_TABLE()
};

#endif //__OBJ_INSPECT__
