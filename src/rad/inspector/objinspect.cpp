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

#include "objinspect.h"

#include "wxfbadvprops.h"

#include "../../utils/debug.h"
#include "../../utils/typeconv.h"
#include "../../utils/wxfbexception.h"

#include "../appdata.h"
#include "../auitabart.h"
#include "../bitmaps.h"
#include "../wxfbevent.h"

#include <wx/config.h>
#include <wx/propgrid/propgrid.h>

static int wxEVT_FB_PROP_BITMAP_CHANGED = wxNewEventType();

enum {
	WXFB_PROPERTY_GRID = wxID_HIGHEST + 1000,
	WXFB_EVENT_GRID,
};

// -----------------------------------------------------------------------
// ObjectInspector
// -----------------------------------------------------------------------
BEGIN_EVENT_TABLE(ObjectInspector, wxPanel)
	EVT_PG_CHANGING(WXFB_PROPERTY_GRID, ObjectInspector::OnPropertyGridChanging)
	EVT_PG_CHANGED(WXFB_PROPERTY_GRID, ObjectInspector::OnPropertyGridChanged)
	EVT_PG_CHANGED(WXFB_EVENT_GRID, ObjectInspector::OnEventGridChanged)
	EVT_PG_DOUBLE_CLICK(WXFB_EVENT_GRID, ObjectInspector::OnEventGridDblClick)
	EVT_PG_DOUBLE_CLICK(WXFB_PROPERTY_GRID, ObjectInspector::OnPropertyGridDblClick)
	EVT_PG_ITEM_COLLAPSED(WXFB_PROPERTY_GRID, ObjectInspector::OnPropertyGridExpand)
	EVT_PG_ITEM_EXPANDED(WXFB_PROPERTY_GRID, ObjectInspector::OnPropertyGridExpand)
	EVT_PG_ITEM_COLLAPSED(WXFB_EVENT_GRID, ObjectInspector::OnEventGridExpand)
	EVT_PG_ITEM_EXPANDED(WXFB_EVENT_GRID, ObjectInspector::OnEventGridExpand)
	EVT_PG_SELECTED(WXFB_PROPERTY_GRID, ObjectInspector::OnPropertyGridItemSelected)
	EVT_PG_SELECTED(WXFB_EVENT_GRID, ObjectInspector::OnPropertyGridItemSelected)

	EVT_FB_OBJECT_SELECTED( ObjectInspector::OnObjectSelected )
	EVT_FB_PROJECT_REFRESH( ObjectInspector::OnProjectRefresh )
	EVT_FB_PROPERTY_MODIFIED( ObjectInspector::OnPropertyModified )
	EVT_FB_EVENT_HANDLER_MODIFIED( ObjectInspector::OnEventHandlerModified )
	EVT_CHILD_FOCUS( ObjectInspector::OnChildFocus )
END_EVENT_TABLE()

ObjectInspector::ObjectInspector( wxWindow* parent, int id, int style )
: wxPanel(parent,id), m_style(style)
{
	AppData()->AddHandler( this->GetEventHandler() );
	m_currentSel = PObjectBase();

	m_nb = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP );
	m_nb->SetArtProvider( new AuiTabArt() );

	// The colour of property grid description looks ugly if we don't set this colour
	m_nb->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

	// Register the slider editor
#if wxUSE_SLIDER
	// TODO
#endif
	m_pg = CreatePropertyGridManager(m_nb, WXFB_PROPERTY_GRID);
	m_eg = CreatePropertyGridManager(m_nb, WXFB_EVENT_GRID);

	m_nb->AddPage( m_pg, _("Properties"), false, 0 );
	m_nb->AddPage( m_eg, _("Events"),     false, 1 );

	m_nb->SetPageBitmap( 0, AppBitmaps::GetBitmap( wxT("properties"), 16 ) );
	m_nb->SetPageBitmap( 1, AppBitmaps::GetBitmap( wxT("events"), 16 ) );

	wxBoxSizer* topSizer = new wxBoxSizer( wxVERTICAL );
	topSizer->Add( m_nb, 1, wxALL | wxEXPAND, 0 );
	SetSizer( topSizer );

	Connect( wxID_ANY, wxEVT_FB_PROP_BITMAP_CHANGED, wxCommandEventHandler( ObjectInspector::OnBitmapPropertyChanged) );
}

ObjectInspector::~ObjectInspector()
{
	Disconnect( wxID_ANY, wxEVT_FB_PROP_BITMAP_CHANGED, wxCommandEventHandler( ObjectInspector::OnBitmapPropertyChanged) );

	AppData()->RemoveHandler( this->GetEventHandler() );
}

void ObjectInspector::SavePosition()
{
	// Save Layout
	wxConfigBase* config = wxConfigBase::Get();
	config->Write( wxT("/mainframe/objectInspector/DescBoxHeight" ), m_pg->GetDescBoxHeight() );
}

void ObjectInspector::Create( bool force )
{
	PObjectBase sel_obj = AppData()->GetSelectedObject();
	if ( sel_obj && ( sel_obj != m_currentSel || force ) )
	{
		Freeze();

		m_currentSel = sel_obj;

		int pageNumber = m_pg->GetSelectedPage();
		wxString pageName;
		if ( pageNumber != wxNOT_FOUND )
		{
			pageName = m_pg->GetPageName( pageNumber );
		}

		// Clear Property Grid Manager
		m_pg->Clear();

		// Now we do the same thing for event grid...
		m_eg->Clear();

		m_propMap.clear();
		m_eventMap.clear();

		PObjectInfo obj_desc = sel_obj->GetObjectInfo();
		if (obj_desc)
		{
			PropertyMap propMap, dummyPropMap;
			EventMap eventMap, dummyEventMap;

			// We create the categories with the properties of the object organized by "classes"
			CreateCategory( obj_desc->GetClassName(), sel_obj, obj_desc, propMap, false );
			CreateCategory( obj_desc->GetClassName(), sel_obj, obj_desc, eventMap, true );

			for (unsigned int i=0; i<obj_desc->GetBaseClassCount() ; i++)
			{
				PObjectInfo info_base = obj_desc->GetBaseClass(i);
				CreateCategory( info_base->GetClassName(), sel_obj, info_base, propMap, false );
				CreateCategory( info_base->GetClassName(), sel_obj, info_base, eventMap, true );
			}

			PObjectBase parent = sel_obj->GetParent();
			if ( parent )
			{
				PObjectInfo parent_desc = parent->GetObjectInfo();
				if ( parent_desc->GetObjectType()->IsItem())
				{
					CreateCategory( parent_desc->GetClassName(), parent, parent_desc, dummyPropMap, false );
					CreateCategory( parent_desc->GetClassName(), parent, parent_desc, dummyEventMap, true );

					for (unsigned int i=0; i<parent_desc->GetBaseClassCount() ; i++)
					{
						PObjectInfo info_base = parent_desc->GetBaseClass(i);
						CreateCategory( info_base->GetClassName(), parent, info_base, dummyPropMap, false );
						CreateCategory( info_base->GetClassName(), parent, info_base, dummyEventMap, true );
					}
				}
			}

			// Select previously selected page, or first page
			if ( m_pg->GetPageCount() > 0 )
			{
				int pageIndex = m_pg->GetPageByName( pageName );
				if ( wxNOT_FOUND != pageIndex )
				{
					m_pg->SelectPage( pageIndex );
				}
				else
				{
					m_pg->SelectPage( 0 );
				}
			}
		}

		m_pg->Refresh();
		m_pg->Update();
		m_eg->Refresh();
		m_eg->Update();
		Thaw();

		RestoreLastSelectedPropItem();
	}
}

int ObjectInspector::StringToBits(const wxString& strVal, wxPGChoices& constants)
{
	wxStringTokenizer strTok(strVal, wxT(" |"));
	int val = 0;
	while (strTok.HasMoreTokens())
	{
		wxString token = strTok.GetNextToken();
		unsigned int i = 0;
		bool done = false;
		while (i < constants.GetCount() && !done)
		{
			if (constants.GetLabel(i) == token)
			{
				val |= constants.GetValue(i);
				done = true;
			}
			i++;
		}
	}
	return val;
}

wxPGProperty* ObjectInspector::GetProperty( PProperty prop )
{
	wxPGProperty *result = NULL;
	PropertyType type = prop->GetType();
	wxString name = prop->GetName();
	wxVariant vTrue = wxVariant( true, wxT("true") );

	if (type == PT_MACRO)
	{
		result = new wxStringProperty( name, wxPG_LABEL, prop->GetValueAsString() );
	}
	else if (type == PT_INT)
	{
		result = new wxIntProperty( name, wxPG_LABEL, prop->GetValueAsInteger() );
	}
	else if (type == PT_UINT)
	{
		result = new wxUIntProperty( name, wxPG_LABEL, (unsigned)prop->GetValueAsInteger() );
	}
	else if (type == PT_WXSTRING || type == PT_WXSTRING_I18N)
	{
		result = new wxLongStringProperty( name, wxPG_LABEL, prop->GetValueAsText() );
	}
	else if (type == PT_TEXT)
	{
		result = new wxLongStringProperty( name, wxPG_LABEL, prop->GetValueAsString() );
		result->ChangeFlag( wxPG_PROP_NO_ESCAPE, true );
	}
	else if (type == PT_BOOL)
	{
		result = new wxBoolProperty( name, wxPG_LABEL, prop->GetValue() == wxT("1") );
	}
	else if (type == PT_BITLIST)
	{
		PPropertyInfo prop_desc = prop->GetPropertyInfo();
		POptionList opt_list = prop_desc->GetOptionList();

		assert(opt_list && opt_list->GetOptionCount() > 0);

		wxPGChoices constants;
		const std::map< wxString, wxString > options = opt_list->GetOptions();
		std::map< wxString, wxString >::const_iterator it;
		unsigned int index = 0;
		for( it = options.begin(); it != options.end(); ++it )
		{
			constants.Add(it->first, 1 << index++);
		}

		int val = StringToBits(prop->GetValueAsString(), constants);
		result = new wxFlagsProperty(name, wxPG_LABEL, constants, val);

		// Workaround to set the help strings for individual members of a wxFlagsProperty
		wxFlagsProperty* flagsProp = dynamic_cast< wxFlagsProperty* >( result );
		if ( NULL != flagsProp )
		{
			for ( size_t i = 0; i < flagsProp->GetItemCount(); i++ )
			{
				wxPGProperty* itemProp = flagsProp->Item(i);
				std::map<wxString, wxString>::const_iterator option = options.find(itemProp->GetLabel());
				if ( option != options.end() )
				{
					m_pg->SetPropertyHelpString(itemProp, option->second);
				}
			}
		}
	}
	else if (type == PT_INTLIST || type == PT_UINTLIST || type == PT_INTPAIRLIST || type == PT_UINTPAIRLIST)
	{
		result = new wxStringProperty(name, wxPG_LABEL, IntList(prop->GetValueAsString(), type == PT_UINTLIST, (PT_INTPAIRLIST == type || PT_UINTPAIRLIST == type)).ToString(true));
	}
	else if (type == PT_OPTION || type == PT_EDIT_OPTION)
	{
		PPropertyInfo prop_desc = prop->GetPropertyInfo();
		POptionList opt_list = prop_desc->GetOptionList();

		assert(opt_list && opt_list->GetOptionCount() > 0);

		wxString value = prop->GetValueAsString();
		wxString help;

		wxPGChoices constants;
		const std::map< wxString, wxString > options = opt_list->GetOptions();
		std::map< wxString, wxString >::const_iterator it;
		unsigned int i = 0;
		for( it = options.begin(); it != options.end(); ++it )
		{
			constants.Add( it->first, i++ );
			if ( it->first == value )
			{
				// Save help
				help = it->second;
			}
		}

		if ( type == PT_EDIT_OPTION )
		{
			result = new wxEditEnumProperty( name, wxPG_LABEL, constants );
		}
		else
		{
			result = new wxEnumProperty( name, wxPG_LABEL, constants );
		}
		result->SetValueFromString( value, 0 );
		wxString desc = prop_desc->GetDescription();
		if ( desc.empty() )
		{
			desc = value + wxT(":\n") + help;
		}
		else
		{
			desc += wxT("\n\n") + value + wxT(":\n") + help;
		}
		result->SetHelpString( wxGetTranslation( desc ) );

	}
	else if (type == PT_WXPOINT)
	{
		result = new wxFBPointProperty( name, wxPG_LABEL, prop->GetValueAsPoint() );
	}
	else if (type == PT_WXSIZE)
	{
		result = new wxFBSizeProperty( name, wxPG_LABEL, prop->GetValueAsSize() );
	}
	else if (type == PT_WXFONT)
	{
		result = new wxFBFontProperty( name, wxPG_LABEL, TypeConv::StringToFont( prop->GetValueAsString() ) );
	}
	else if (type == PT_WXCOLOUR)
	{
		wxString value = prop->GetValueAsString();
		if ( value.empty() )  // Default Colour
		{
			wxColourPropertyValue colProp;
			colProp.m_type = wxSYS_COLOUR_WINDOW;
			colProp.m_colour = TypeConv::StringToSystemColour( wxT("wxSYS_COLOUR_WINDOW") );
			result = new wxSystemColourProperty( name, wxPG_LABEL, colProp );
		}
		else
		{
			if ( value.find_first_of( wxT("wx") ) == 0 )
			{
				wxColourPropertyValue def; // System Colour
				def.m_type = TypeConv::StringToSystemColour( value );
				result = new wxSystemColourProperty( name, wxPG_LABEL, def );
			}
			else
			{
				result = new wxSystemColourProperty( name, wxPG_LABEL, prop->GetValueAsColour() );
			}
		}
	}
	else if (type == PT_PATH)
	{
		result = new wxDirProperty( name, wxPG_LABEL, prop->GetValueAsString() );
	}
	else if (type == PT_FILE)
	{
		result = new wxFileProperty( name, wxPG_LABEL, prop->GetValueAsString() );
	}
	else if (type == PT_BITMAP)
	{
		wxLogDebug( wxT("OI::GetProperty: prop:%s"), prop->GetValueAsString().c_str() );

		result = new wxFBBitmapProperty( name, wxPG_LABEL, prop->GetValueAsString() );
	}
	else if (type == PT_STRINGLIST)
	{
		result = new wxArrayStringProperty( name, wxPG_LABEL,prop->GetValueAsArrayString() );
#if wxVERSION_NUMBER >= 2901
		wxVariant v("\"");
		result->DoSetAttribute(wxPG_ARRAY_DELIMITER, v);
#endif
	}
	else if (type == PT_FLOAT)
	{
		result = new wxFloatProperty( name, wxPG_LABEL, prop->GetValueAsFloat() );
	}
	else if ( type == PT_PARENT )
	{
		result = new wxPGProperty( name, wxPG_LABEL );

		/*wxPGProperty* parent = new wxPGProperty( name, wxPG_LABEL );
		parent->SetValueFromString( prop->GetValueAsString(), wxPG_FULL_VALUE );
		//wxPGProperty* parent = new wxStringProperty( name, wxPG_LABEL, wxT("<composed>") );
		//parent->SetValueFromString( prop->GetValueAsString() );

		PPropertyInfo prop_desc = prop->GetPropertyInfo();
		std::list< PropertyChild >* children = prop_desc->GetChildren();
		std::list< PropertyChild >::iterator it;
		for ( it = children->begin(); it != children->end(); ++it )
		{
			wxPGProperty* child = new wxStringProperty( it->m_name, wxPG_LABEL, wxEmptyString );
			parent->AppendChild( child );
			m_pg->SetPropertyHelpString( child, it->m_description );
		}

		result = parent;*/
	}
	else // Unknown property
	{
		result = new wxStringProperty( name, wxPG_LABEL, prop->GetValueAsString() );
		result->SetAttribute( wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING, vTrue );
		wxLogError(_("Property type Unknown") );
	}

	return result;
}

void ObjectInspector::AddItems( const wxString& name, PObjectBase obj,
  PObjectInfo obj_info, PPropertyCategory category, PropertyMap &properties )
{
	size_t propCount = category->GetPropertyCount();
	for ( size_t i = 0; i < propCount; i++ )
	{
		wxString propName = category->GetPropertyName( i );
		PProperty prop = obj->GetProperty( propName );

		if ( !prop )
			continue;

		PPropertyInfo propInfo = prop->GetPropertyInfo();

		// we do not want to duplicate inherited properties
		if ( properties.find( propName ) == properties.end() )
		{
			wxPGProperty* id       = m_pg->Append( GetProperty( prop ) );
			int           propType = prop->GetType();

			if ( propType != PT_OPTION )
			{
				m_pg->SetPropertyHelpString( id, propInfo->GetDescription() );

				if ( propType == PT_BITMAP )
				{
					wxFBBitmapProperty *bp = wxDynamicCast( id, wxFBBitmapProperty );
					if ( bp )
					{
						bp->CreateChildren();

						// perform delayed child properties update
						wxCommandEvent e( wxEVT_FB_PROP_BITMAP_CHANGED );
						e.SetString( bp->GetName() + wxT(":") + prop->GetValue() );
						GetEventHandler()->AddPendingEvent( e );

						//AppData()->ModifyProperty( prop, bp->GetValueAsString() );
					}
				}
				else if( propType == PT_PARENT )
				{
					PPropertyInfo prop_desc = prop->GetPropertyInfo();
					std::list< PropertyChild >* children = prop_desc->GetChildren();
					std::list< PropertyChild >::iterator it;
					wxArrayString values = wxStringTokenize( prop->GetValueAsString(), wxT(";"), wxTOKEN_RET_EMPTY_ALL );
					size_t index = 0;
					wxString value;

					for ( it = children->begin(); it != children->end(); ++it )
					{
						if (values.GetCount() > index) value = values[index++].Trim().Trim(false);
						else value = wxT("");

						wxPGProperty* child = nullptr;
						if( PT_BOOL == it->m_type )
						{
							// Because the format of a composed wxPGProperty value is stored this needs to be converted
							// true == "<property name>"
							// false == "Not <property name>"
							// TODO: The subclass property is currently the only one using this child type,
							//       because the only instance using this property, the c++ code generator,
							//       interprets a missing value as true and currently no project file update
							//       adds this value if it is missing, here a missing value also needs to be
							//       interpreted as true
							child = new wxBoolProperty(it->m_name, wxPG_LABEL, value.empty() || value == it->m_name);
						}
						else if( PT_WXSTRING == it->m_type )
						{
							child = new wxStringProperty( it->m_name, wxPG_LABEL, value );
						}
						else
						{
							THROW_WXFBEX( wxT("Invalid Child Property Type: ") << it->m_type );
						}

						id->AppendChild( child );
						m_pg->SetPropertyHelpString( child, it->m_description );
					}
				}
			}

			wxString customEditor = propInfo->GetCustomEditor();
			if ( !customEditor.empty() )
			{
				wxPGEditor* editor = m_pg->GetEditorByName( customEditor );
				if ( editor )
				{
					m_pg->SetPropertyEditor( id, editor );
				}
			}

			if (m_style != wxFB_OI_MULTIPAGE_STYLE)
			{
				// Most common classes will be showed with a slightly different colour.
				if (name == wxT("wxWindow"))
					m_pg->SetPropertyBackgroundColour(id,wxColour(255,255,205)); // yellow
				else if (name == wxT("AUI"))
					m_pg->SetPropertyBackgroundColour(id,wxColour(240,240,255)); // light blue
				else if (name == wxT("sizeritem") || name == wxT("gbsizeritem") || name == wxT("sizeritembase") )
					m_pg->SetPropertyBackgroundColour(id,wxColour(220,255,255)); // cyan
			}

			ExpandMap::iterator it = m_isExpanded.find( propName );
			if ( it != m_isExpanded.end() )
			{
				if ( it->second )
				{
					m_pg->Expand( id );
				}
				else
				{
					m_pg->Collapse( id );
				}
			}

			properties.insert( PropertyMap::value_type( propName, prop ) );
			m_propMap.insert( ObjInspectorPropertyMap::value_type( id, prop ) );
		}
	}

	size_t catCount = category->GetCategoryCount();
	for ( size_t i = 0; i < catCount; i++ )
	{
		PPropertyCategory nextCat = category->GetCategory( i );
		if ( 0 == nextCat->GetCategoryCount() && 0 == nextCat->GetPropertyCount() )
		{
			continue;
		}
		wxPGProperty* catId = m_pg->AppendIn( category->GetName() , new wxPropertyCategory( nextCat->GetName() ) );

		AddItems( name, obj, obj_info, nextCat, properties );

		ExpandMap::iterator it = m_isExpanded.find( nextCat->GetName() );
		if ( it != m_isExpanded.end() )
		{
			if ( it->second )
			{
				m_pg->Expand( catId );
			}
			else
			{
				m_pg->Collapse( catId );
			}
		}
	}
}

void ObjectInspector::AddItems( const wxString& name, PObjectBase obj,
  PObjectInfo obj_info, PPropertyCategory category, EventMap &events )
{
	size_t eventCount = category->GetEventCount();
	for ( size_t i = 0; i < eventCount; i++ )
	{
		wxString eventName = category->GetEventName( i );
		PEvent event = obj->GetEvent( eventName );

		if ( !event )
			continue;

		PEventInfo eventInfo = event->GetEventInfo();

		// We do not want to duplicate inherited events
		if ( events.find( eventName ) == events.end() )
		{
			wxPGProperty* pgProp = new wxStringProperty( eventInfo->GetName(), wxPG_LABEL, event->GetValue() );
			wxPGProperty* id = m_eg->Append( pgProp );

			m_eg->SetPropertyHelpString( id, wxGetTranslation( eventInfo->GetDescription() ) );

			if (m_style != wxFB_OI_MULTIPAGE_STYLE)
			{
				// Most common classes will be showed with a slightly different colour.
				if (name == wxT("wxWindow"))
					m_eg->SetPropertyBackgroundColour( id, wxColour( 255, 255, 205 ) ); // Yellow
				else if (name == wxT("AUI Events"))
					m_eg->SetPropertyBackgroundColour( id, wxColour(240,240,255) ); // light blue
				else if (name == wxT("sizeritem") || name == wxT("gbsizeritem") || name == wxT("sizeritembase") )
					m_eg->SetPropertyBackgroundColour( id, wxColour( 220, 255, 255 ) ); // Cyan
			}

			ExpandMap::iterator it = m_isExpanded.find(eventName);
			if ( it != m_isExpanded.end() )
			{
				if ( it->second )
				{
					m_eg->Expand( id );
				}
				else
				{
					m_eg->Collapse( id );
				}
			}

			events.insert( EventMap::value_type( eventName, event ) );
			m_eventMap.insert( ObjInspectorEventMap::value_type( id, event ) );
		}
	}

	size_t catCount = category->GetCategoryCount();
	for ( size_t i = 0; i < catCount; i++ )
	{
		PPropertyCategory nextCat = category->GetCategory( i );
		if ( 0 == nextCat->GetCategoryCount() && 0 == nextCat->GetEventCount() )
		{
			continue;
		}
		wxPGProperty* catId = m_eg->AppendIn( category->GetName(), new wxPropertyCategory( nextCat->GetName() ) );

		AddItems( name, obj, obj_info, nextCat, events );

		ExpandMap::iterator it = m_isExpanded.find( nextCat->GetName() );
		if ( it != m_isExpanded.end() )
		{
			if ( it->second )
			{
				m_eg->Expand( catId );
			}
			else
			{
				m_eg->Collapse( catId );
			}
		}
	}
}

void ObjectInspector::OnPropertyGridChanging( wxPropertyGridEvent& event )
{
	wxImageFileProperty *imgFileProp = wxDynamicCast( event.GetProperty(), wxImageFileProperty );

	if ( imgFileProp )
	{
		// GetValue() returns the pending value, but is only supported by wxEVT_PG_CHANGING.
		wxFBBitmapProperty *bmpProp = wxDynamicCast( imgFileProp->GetParent(), wxFBBitmapProperty );

		if ( bmpProp )
		{
			wxString img = bmpProp->SetupImage( event.GetValue().GetString() );
			if ( img == wxEmptyString )
			{
				event.Veto();
				return;
			}
		}
	}
}

void ObjectInspector::OnPropertyGridChanged( wxPropertyGridEvent& event )
{
	wxPGProperty* propPtr = event.GetProperty();

	ObjInspectorPropertyMap::iterator it = m_propMap.find( propPtr );

	if ( m_propMap.end() == it )
	{
		// Could be a child property
		propPtr = propPtr->GetParent();
		it = m_propMap.find( propPtr );
	}
	if ( it != m_propMap.end() )
	{
		PProperty prop = it->second;
		switch ( prop->GetType() )
		{

			case PT_FLOAT:
			{
				// Use typeconv to properly handle locale
				double val = m_pg->GetPropertyValueAsDouble( propPtr );
				ModifyProperty( prop, TypeConv::FloatToString( val ) );
				break;
			}
			case PT_TEXT:
			case PT_MACRO:
			case PT_INT:
			case PT_UINT:
			{
				ModifyProperty( prop, m_pg->GetPropertyValueAsString( propPtr ) );
				break;
			}
			case PT_OPTION:
			case PT_EDIT_OPTION:
			{
				wxString value = m_pg->GetPropertyValueAsString( propPtr );
				ModifyProperty( prop, value );

				// Update displayed description for the new selection
				PPropertyInfo prop_desc = prop->GetPropertyInfo();
				POptionList opt_list = prop_desc->GetOptionList();

				wxString helpString = prop_desc->GetDescription();
				if ( opt_list && opt_list->GetOptionCount() > 0 )
				{
					const std::map< wxString, wxString > options = opt_list->GetOptions();
					std::map< wxString, wxString >::const_iterator option = options.find( value );
					if ( option != options.end() )
					{
						if ( helpString.empty() )
						{
							helpString = value + wxT(":\n") + option->second;
						}
						else
						{
							helpString += wxT("\n\n") + value + wxT(":\n") + option->second;
						}
					}
				}
				wxString localized = wxGetTranslation( helpString );
				m_pg->SetPropertyHelpString( propPtr, localized );
				m_pg->SetDescription( propPtr->GetLabel(), localized );
				break;
			}
			case PT_PARENT:
			{
				// GenerateComposedValue() is the only method that does actually return a value,
				// although the documentation claims the other methods just call this one,
				// they return an empty value
				const auto value = propPtr->GenerateComposedValue();
				ModifyProperty(prop, value);
				break;
			}
			case PT_WXSTRING:
			case PT_WXSTRING_I18N:
			{
				// ObjectInspector's text strings are formatted.
				wxString value = TypeConv::TextToString( m_pg->GetPropertyValueAsString( propPtr ) );
				ModifyProperty( prop, value );
				break;
			}
			case PT_BOOL:
			{
				if( prop->GetName() == wxT("aui_managed") )
				{
					PObjectBase propobj = prop->GetObject();
					if( propobj->GetChildCount() )
					{
						wxMessageBox(_("You have to remove all child widgets first."));
						m_pg->SetPropertyValue( propPtr, !m_pg->GetPropertyValueAsBool( propPtr )  );
					}
					else
						ModifyProperty( prop, m_pg->GetPropertyValueAsBool( propPtr ) ? wxT("1") : wxT("0") );
				}
				else
					ModifyProperty( prop, m_pg->GetPropertyValueAsBool( propPtr ) ? wxT("1") : wxT("0") );
				break;
			}
			case PT_BITLIST:
			{
				wxString aux = m_pg->GetPropertyValueAsString( propPtr );
				aux.Replace( wxT(" "), wxT("") );
				aux.Replace( wxT(","), wxT("|") );
				ModifyProperty( prop, aux );
				break;
			}
			case PT_WXPOINT:
			{
				wxPoint point = wxPointRefFromVariant( event.GetPropertyValue () );
				ModifyProperty( prop, wxString::Format(  wxT("%i,%i"), point.x, point.y ) );
				break;
			}
			case PT_WXSIZE:
			{
				wxSize size = wxSizeRefFromVariant( event.GetPropertyValue() );
				ModifyProperty( prop, wxString::Format( wxT("%i,%i"), size.GetWidth(), size.GetHeight() ) );
				break;
			}
			case PT_WXFONT:
			{
				ModifyProperty( prop, event.GetPropertyValue().GetString() );
				break;
			}
			case PT_WXCOLOUR:
			{
				wxColourPropertyValue colour;
				colour << event.GetPropertyValue();
				switch ( colour.m_type ) {
				case wxSYS_COLOUR_MAX:
					ModifyProperty( prop, wxT("") );
					break;
				case wxPG_COLOUR_CUSTOM:
					ModifyProperty( prop, TypeConv::ColourToString( colour.m_colour ) );
					break;
				default:
					wxString sCol = TypeConv::SystemColourToString( colour.m_type );
					ModifyProperty( prop, sCol );
				}
				break;
			}
			case PT_INTLIST:
			case PT_UINTLIST:
			case PT_INTPAIRLIST:
			case PT_UINTPAIRLIST:
			{
				IntList il(event.GetPropertyValue(), PT_UINTLIST == prop->GetType(), (PT_INTPAIRLIST == prop->GetType() || PT_UINTPAIRLIST == prop->GetType()));
				ModifyProperty(prop, il.ToString(true));
				break;
			}
			case PT_BITMAP:
			{
				wxVariant childValue = event.GetProperty()->GetValue();

				// Also, handle the case where property value is unspecified
				if ( childValue.IsNull() )
					return;

				// bp->GetValue() have no updated value...
				wxString bmpVal = propPtr->GetValueAsString( wxPG_FULL_VALUE );

				// Handle changes in values, as needed
				wxVariant thisValue  = WXVARIANT(bmpVal);

				wxVariant newVal =
				propPtr->ChildChanged( thisValue, (int)event.GetProperty()->GetIndexInParent(), childValue );

				ModifyProperty( prop, newVal.GetString() );

				if( event.GetProperty()->GetIndexInParent() > 0 )
				{
					// perform delayed child properties update
					wxCommandEvent e( wxEVT_FB_PROP_BITMAP_CHANGED );
					e.SetString( propPtr->GetName() + wxT(":") + bmpVal );
					GetEventHandler()->AddPendingEvent( e );
				}

				break;
			}

			default:
				//ModifyProperty( prop, event.GetPropertyValue() );
				ModifyProperty( prop, propPtr->GetValueAsString() );
		}
	}
}

void ObjectInspector::OnEventGridChanged( wxPropertyGridEvent& event )
{
	ObjInspectorEventMap::iterator it = m_eventMap.find( event.GetProperty() );

	if ( it != m_eventMap.end() )
	{
		PEvent evt = it->second;
		wxString handler = event.GetPropertyValue();
		handler.Trim();
		handler.Trim( false );
		AppData()->ModifyEventHandler( evt, handler );
	}
}

void ObjectInspector::OnPropertyGridExpand( wxPropertyGridEvent& event )
{
	m_isExpanded[event.GetPropertyName()] = event.GetProperty()->IsExpanded();

	wxPGProperty* egProp = m_eg->GetProperty( event.GetProperty()->GetName() );
	if ( egProp )
	{
		if ( event.GetProperty()->IsExpanded() )
		{
			m_eg->Expand( egProp );
		}
		else
		{
			m_eg->Collapse( egProp );
		}
	}
}

void ObjectInspector::OnEventGridExpand( wxPropertyGridEvent& event )
{
	m_isExpanded[event.GetPropertyName()] = event.GetProperty()->IsExpanded();

	wxPGProperty* pgProp = m_pg->GetProperty( event.GetProperty()->GetName() );
	if ( pgProp )
	{
		if ( event.GetProperty()->IsExpanded() )
		{
			m_pg->Expand( pgProp );
		}
		else
		{
			m_pg->Collapse( pgProp );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void ObjectInspector::OnObjectSelected( wxFBObjectEvent& event)
{
	bool isForced = ( event.GetString() == wxT("force") );

	Create( isForced );
}

void ObjectInspector::OnProjectRefresh( wxFBEvent& )
{
	Create( true );
}

void ObjectInspector::OnEventHandlerModified( wxFBEventHandlerEvent& event )
{
	PEvent e = event.GetFBEventHandler();
	m_eg->SetPropertyValue( e->GetName(), e->GetValue() );
	m_eg->Refresh();
}

void ObjectInspector::OnPropertyModified( wxFBPropertyEvent& event )
{
	LogDebug("");
	PProperty prop = event.GetFBProperty();

	PObjectBase propobj = prop->GetObject();
	PObjectBase appobj = AppData()->GetSelectedObject();

	bool shouldContinue = ( prop->GetObject() == AppData()->GetSelectedObject() );
	if ( !shouldContinue )
	{
		// Item objects cannot be selected - their children are selected instead
		if ( propobj->GetObjectInfo()->GetObjectType()->IsItem() )
		{
			if ( propobj->GetChildCount() > 0 )
			{
				shouldContinue = ( appobj == propobj->GetChild( 0 ) );
			}
		}
	}
	if ( !shouldContinue )
	{
		return;
	}

	wxPGProperty* pgProp = m_pg->GetPropertyByLabel(prop->GetName() );
	if ( pgProp == NULL ) return; // Maybe now isn't showing this page


	switch (prop->GetType())
	{
	case PT_FLOAT:
	{
		// Use float instead of string -> typeconv handles locale
		pgProp->SetValue( WXVARIANT( prop->GetValueAsFloat() ) );
		break;
	}
	case PT_INT:
	case PT_UINT:
	{
		pgProp->SetValueFromString(prop->GetValueAsString(), 0);
		break;
	}
	case PT_TEXT:
		pgProp->SetValueFromString(prop->GetValueAsString(), 0);
		break;
	case PT_MACRO:
	case PT_OPTION:
	case PT_EDIT_OPTION:
	case PT_PARENT:
	case PT_WXSTRING:
		pgProp->SetValueFromString(prop->GetValueAsText(), 0);
		break;
	case PT_WXSTRING_I18N:
		pgProp->SetValueFromString(prop->GetValueAsText(), 0);
		break;
	case PT_BOOL:
		pgProp->SetValueFromInt(prop->GetValueAsString() == wxT("0") ? 0 : 1, 0);
		break;
	case PT_BITLIST:
		{
			wxString aux = prop->GetValueAsString();
			aux.Replace(wxT("|"), wxT(", "));
			if (aux == wxT("0")) aux = wxT("");
			pgProp->SetValueFromString(aux, 0);
		}
		break;
	case PT_WXPOINT:
		{
			//m_pg->SetPropertyValue( pgProp, prop->GetValue() );
			wxString aux = prop->GetValueAsString();
			aux.Replace(wxT(","), wxT(";"));
			pgProp->SetValueFromString(aux, 0);
		}
		break;
	case PT_WXSIZE:
		{
			//m_pg->SetPropertyValue( pgProp, prop->GetValue() );
			wxString aux = prop->GetValueAsString();
			aux.Replace(wxT(","), wxT(";"));
			pgProp->SetValueFromString(aux, 0);
		}
		break;
	case PT_WXFONT:
		pgProp->SetValue( WXVARIANT( prop->GetValueAsString() ) );
		break;
	case PT_WXCOLOUR:
		{
			wxString value = prop->GetValueAsString();
			if ( value.empty() )  // Default Colour
			{
				wxColourPropertyValue def;
				def.m_type = wxSYS_COLOUR_WINDOW;
				def.m_colour = TypeConv::StringToSystemColour( wxT("wxSYS_COLOUR_WINDOW") );
				m_pg->SetPropertyValue( pgProp, def );
			}
			else
			{
				if ( value.find_first_of( wxT("wx") ) == 0 )
				{
					// System Colour
					wxColourPropertyValue def;
					def.m_type = TypeConv::StringToSystemColour( value );
					def.m_colour = prop->GetValueAsColour();
					m_pg->SetPropertyValue( pgProp, WXVARIANT( def ) );
				}
				else
				{
					wxColourPropertyValue def( wxPG_COLOUR_CUSTOM, prop->GetValueAsColour() );
					m_pg->SetPropertyValue( pgProp, WXVARIANT( def ) );
				}
			}
		}
		break;
	case PT_BITMAP:
//      pgProp->SetValue( WXVARIANT( prop->GetValueAsString() ) );
		wxLogDebug( wxT("OI::OnPropertyModified: prop:%s"), prop->GetValueAsString().c_str() );
		break;
	default:
		pgProp->SetValueFromString(prop->GetValueAsString(), wxPG_FULL_VALUE);
	}
	AutoGenerateId(AppData()->GetSelectedObject(), event.GetFBProperty(), wxT("PropChange"));
	m_pg->Refresh();
}

wxPropertyGridManager* ObjectInspector::CreatePropertyGridManager(wxWindow *parent, wxWindowID id)
{
	int pgStyle;
	int defaultDescBoxHeight;

	switch (m_style)
	{
		case wxFB_OI_MULTIPAGE_STYLE:
			pgStyle = wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE;
			defaultDescBoxHeight = 50;
			break;

		case wxFB_OI_DEFAULT_STYLE:
		case wxFB_OI_SINGLE_PAGE_STYLE:
		default:
			pgStyle = wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE;
			defaultDescBoxHeight = 150;
			break;
	}

	int descBoxHeight;
	wxConfigBase* config = wxConfigBase::Get();
	config->Read( wxT( "/mainframe/objectInspector/DescBoxHeight" ), &descBoxHeight, defaultDescBoxHeight );
	if ( -1 == descBoxHeight )
	{
		descBoxHeight = defaultDescBoxHeight;
	}

	wxPropertyGridManager* pg;
	pg = new wxPropertyGridManager( parent, id, wxDefaultPosition, wxDefaultSize, pgStyle );
	pg->SendSizeEvent();
	pg->SetDescBoxHeight( descBoxHeight );
//  pg->SetExtraStyle( wxPG_EX_NATIVE_DOUBLE_BUFFERING ); Both seems to no more needed.
//  pg->SetExtraStyle( wxPG_EX_PROCESS_EVENTS_IMMEDIATELY );

	return pg;
}

void ObjectInspector::OnPropertyGridDblClick(wxPropertyGridEvent& event)
{
	PObjectBase obj = AppData()->GetSelectedObject();
	if( obj )
	{
		wxString propName = event.GetProperty()->GetLabel();
		AutoGenerateId(obj, obj->GetProperty(propName), wxT("DblClk"));
		m_pg->Refresh();
	}
}

void ObjectInspector::OnEventGridDblClick(wxPropertyGridEvent& event)
{
	wxPGProperty *pgProp = m_pg->GetPropertyByLabel( wxT("name") );
	if ( !pgProp ) return;

	wxPGProperty *p = event.GetProperty();
	p->SetValueFromString( pgProp->GetDisplayedString() + event.GetProperty()->GetLabel() );
	ObjInspectorEventMap::iterator it = m_eventMap.find( p );
	if ( it != m_eventMap.end() )
	{
		PEvent evt = it->second;
		wxString handler = p->GetValueAsString();
		handler.Trim();
		handler.Trim( false );
		AppData()->ModifyEventHandler( evt, handler );
	};
}

void ObjectInspector::AutoGenerateId(PObjectBase objectChanged, PProperty propChanged, wxString reason)
{
	if(objectChanged && propChanged)
	{
		PProperty prop;
		if((propChanged->GetName() == wxT("name") && reason == wxT("PropChange")) ||
			(propChanged->GetName() == wxT("id") && reason == wxT("DblClk")))
		{
			//wxPGId pgid = m_pg->GetPropertyByLabel(wxT(""));
			prop = AppData()->GetProjectData()->GetProperty( wxT("event_generation") );
			if ( prop )
			{
				if(prop->GetValueAsString() == wxT("table"))
				{
					prop = objectChanged->GetProperty(wxT("id"));
					if ( prop )
					{
						if(prop->GetValueAsString() == wxT("wxID_ANY") || reason == wxT("DblClk"))
						{
							PProperty name(objectChanged->GetProperty(wxT("name")));
							wxString idString;
							idString << wxT("ID_");
							idString << name->GetValueAsString().Upper();
							ModifyProperty( prop, idString);

							wxPGProperty *pgid = m_pg->GetPropertyByLabel(wxT("id"));
							if ( !pgid ) return;
							m_pg->SetPropertyValue( pgid, idString );
						}
					}
				}
				else
				{
					prop = objectChanged->GetProperty(wxT("id"));
					if ( prop )
					{
						ModifyProperty( prop, wxT("wxID_ANY"));
						wxPGProperty *pgid = m_pg->GetPropertyByLabel(wxT("id"));
						if ( !pgid ) return;
						m_pg->SetPropertyValue( pgid, wxT("wxID_ANY") );
					}
				}
			}
		}
	}
	m_pg->Update();
}

void ObjectInspector::OnBitmapPropertyChanged( wxCommandEvent& event )
{
	wxLogDebug( wxT("OI::BitmapPropertyChanged: %s"), event.GetString().c_str() );

	wxString propName = event.GetString().BeforeFirst( ':' );
	wxString propVal = event.GetString().AfterFirst( ':' );

	if( !propVal.IsEmpty() )
	{
		wxFBBitmapProperty *bp = wxDynamicCast( m_pg->GetPropertyByLabel( propName ), wxFBBitmapProperty );
		if( bp )
		{
			bp->UpdateChildValues( propVal );
		}
	}
}

void ObjectInspector::ModifyProperty( PProperty prop, const wxString& str )
{
	AppData()->RemoveHandler( this->GetEventHandler() );
	AppData()->ModifyProperty( prop, str );
	AppData()->AddHandler( this->GetEventHandler() );
}

void ObjectInspector::OnChildFocus(wxChildFocusEvent&) {
	// do nothing to avoid "scrollbar jump" if wx2.9 is used
}

void ObjectInspector::OnPropertyGridItemSelected( wxPropertyGridEvent& event )
{
	wxPGProperty *p = event.GetProperty();
	if( p )
	{
		if( m_nb->GetSelection() == 0 )
		{
			m_strSelPropItem = m_pg->GetPropertyName( p );
			m_pageName = wxT("Properties");
		}
		else
		{
			m_strSelPropItem = m_eg->GetPropertyName( p );
			m_pageName = wxT("Events");
		}
	}
}

void ObjectInspector::RestoreLastSelectedPropItem()
{
	if( m_pageName == wxT("Properties") )
	{
		wxPGProperty* p = m_pg->GetPropertyByName( m_strSelPropItem );
		if( p )
		{
			m_pg->SelectProperty(p, true);
			m_pg->SetFocus();
		}
	}
	else if( m_pageName == wxT("Events") )
	{
		wxPGProperty* p = m_eg->GetPropertyByName(m_strSelPropItem);
		if( p )
		{
			m_eg->SelectProperty(p, true);
			m_eg->SetFocus();
		}
	}
}
