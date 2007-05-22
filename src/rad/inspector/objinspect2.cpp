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

#include "objinspect2.h"
#include "model/objectbase.h"
#include "utils/debug.h"
#include "utils/typeconv.h"
#include "rad/bitmaps.h"
#include "rad/wxfbevent.h"
#include "rad/appdata.h"
#include "model/objectbase.h"

#include <wx/tokenzr.h>
#include <wx/config.h>

#define WXFB_PROPERTY_GRID 1000
#define WXFB_EVENT_GRID    1001

// -----------------------------------------------------------------------
// wxSlider-based property editor
// -----------------------------------------------------------------------

#if wxUSE_SLIDER

//
// Implement an editor control that allows using wxSlider to edit value of
// wxFloatProperty (and similar).
//
// Note that new editor classes needs to be registered before use.
// This can be accomplished using wxPGRegisterEditorClass macro.
// Registeration can also be performed in a constructor of a
// property that is likely to require the editor in question.
//


#include <wx/slider.h>

class wxPGSliderEditor : public wxPGEditor
{
    WX_PG_DECLARE_EDITOR_CLASS()
private:
	int m_max;
public:
	wxPGSliderEditor()
	:
	m_max( 10000 )
	{
	}

    virtual ~wxPGSliderEditor();

    // See below for short explanations of what these are suppposed to do.
    wxPG_DECLARE_CREATECONTROLS

	virtual void UpdateControl( wxPGProperty* property, wxWindow* wnd ) const;
    virtual bool OnEvent( wxPropertyGrid* propgrid, wxPGProperty* property,
        wxWindow* wnd, wxEvent& event ) const;
    virtual bool CopyValueFromControl( wxPGProperty* property, wxWindow* wnd ) const;
    virtual void SetValueToUnspecified( wxWindow* wnd ) const;
};


// This macro also defines global wxPGEditor_Slider for storing
// the singleton class instance.
WX_PG_IMPLEMENT_EDITOR_CLASS(Slider,wxPGSliderEditor,wxPGEditor)

// Trivial destructor.
wxPGSliderEditor::~wxPGSliderEditor()
{
}


#ifndef __WXPYTHON__
wxWindow* wxPGSliderEditor::CreateControls( wxPropertyGrid* propgrid,
											wxPGProperty* property,
											const wxPoint& pos,
											const wxSize& sz,
											wxWindow** ) const
#else
wxPGWindowPair wxPGSliderEditor::CreateControls( wxPropertyGrid* propgrid,
												 wxPGProperty* property,
												 const wxPoint& pos,
												 const wxSize& sz ) const
#endif
{
    wxCHECK_MSG( property->IsKindOf(WX_PG_CLASSINFO(wxFloatProperty)),
                 NULL,
                 wxT("Slider editor can only be used with wxFloatProperty or derivative.") );

    // Use two stage creation to allow cleaner display on wxMSW
    wxSlider* ctrl = new wxSlider();
#ifdef __WXMSW__
    ctrl->Hide();
#endif
	wxString s = property->GetValueAsString();
	double v_d = 0;
	if ( s.ToDouble(&v_d) )
	{
		if ( v_d < 0 )
			v_d = 0;
		else if ( v_d > 1 )
			v_d = 1;
	}

    ctrl->Create(propgrid,
                 wxPG_SUBID1,
                 (int)(v_d * m_max),
                 0,
                 m_max,
                 pos,
                 sz,
                 wxSL_HORIZONTAL);

    // Connect all required events to grid's OnCustomEditorEvent
    // (all relevenat wxTextCtrl, wxComboBox and wxButton events are
    // already connected)
    propgrid->Connect( wxPG_SUBID1, wxEVT_SCROLL_THUMBTRACK,
                       (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                       &wxPropertyGrid::OnCustomEditorEvent, NULL, propgrid );

#ifdef __WXMSW__
    ctrl->Show();
#endif

    return ctrl;
}

// Copies value from property to control
void wxPGSliderEditor::UpdateControl( wxPGProperty* property, wxWindow* wnd ) const
{
    wxSlider* ctrl = wxDynamicCast( wnd, wxSlider );
    if ( ctrl )
    {
		double val = wxPGVariantToDouble( property->DoGetValue() );
		if ( val < 0 )
			val = 0;
		else if ( val > 1 )
			val = 1;
		ctrl->SetValue( (int)(val * m_max) );
    }
}

// Control's events are redirected here
bool wxPGSliderEditor::OnEvent( wxPropertyGrid* WXUNUSED(propgrid),
                                        wxPGProperty* WXUNUSED(property),
                                        wxWindow* WXUNUSED(wnd),
                                        wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_SCROLL_THUMBTRACK )
    {
        return true;
    }

    return false;
}

bool wxPGSliderEditor::CopyValueFromControl( wxPGProperty* property, wxWindow* wnd ) const
{
    wxSlider* ctrl = wxDynamicCast( wnd, wxSlider );
    if ( ctrl )
    {
		property->DoSetValue( wxPGVariant( (double)(ctrl->GetValue())/(double)(m_max) ) );
    }

    return true;
}

void wxPGSliderEditor::SetValueToUnspecified( wxWindow* WXUNUSED(wnd) ) const
{
    // TODO?
    //wxDateProperty* prop = (wxDateProperty*) property;
    //ctrl->SetValue(?);
}


#endif // wxUSE_SLIDER

// -----------------------------------------------------------------------
// fbColourProperty
// -----------------------------------------------------------------------

// Colour labels. Last (before NULL, if any) must be Custom.
static const wxChar* fbcolprop_labels[] = {
    wxT("Default"),
    wxT("AppWorkspace"),
    wxT("ActiveBorder"),
    wxT("ActiveCaption"),
    wxT("ButtonFace"),
    wxT("ButtonHighlight"),
    wxT("ButtonShadow"),
    wxT("ButtonText"),
    wxT("CaptionText"),
    wxT("ControlDark"),
    wxT("ControlLight"),
    wxT("Desktop"),
    wxT("GrayText"),
    wxT("Highlight"),
    wxT("HighlightText"),
    wxT("InactiveBorder"),
    wxT("InactiveCaption"),
    wxT("InactiveCaptionText"),
    wxT("Menu"),
    wxT("Scrollbar"),
    wxT("Tooltip"),
    wxT("TooltipText"),
    wxT("Window"),
    wxT("WindowFrame"),
    wxT("WindowText"),
    wxT("Custom"),
    (const wxChar*) NULL
};

static long fbcolprop_colours[] = {
    wxSYS_COLOUR_MAX,
    wxSYS_COLOUR_APPWORKSPACE,
    wxSYS_COLOUR_ACTIVEBORDER,
    wxSYS_COLOUR_ACTIVECAPTION,
    wxSYS_COLOUR_BTNFACE,
    wxSYS_COLOUR_BTNHIGHLIGHT,
    wxSYS_COLOUR_BTNSHADOW,
    wxSYS_COLOUR_BTNTEXT ,
    wxSYS_COLOUR_CAPTIONTEXT,
    wxSYS_COLOUR_3DDKSHADOW,
    wxSYS_COLOUR_3DLIGHT,
    wxSYS_COLOUR_BACKGROUND,
    wxSYS_COLOUR_GRAYTEXT,
    wxSYS_COLOUR_HIGHLIGHT,
    wxSYS_COLOUR_HIGHLIGHTTEXT,
    wxSYS_COLOUR_INACTIVEBORDER,
    wxSYS_COLOUR_INACTIVECAPTION,
    wxSYS_COLOUR_INACTIVECAPTIONTEXT,
    wxSYS_COLOUR_MENU,
    wxSYS_COLOUR_SCROLLBAR,
    wxSYS_COLOUR_INFOBK,
    wxSYS_COLOUR_INFOTEXT,
    wxSYS_COLOUR_WINDOW,
    wxSYS_COLOUR_WINDOWFRAME,
    wxSYS_COLOUR_WINDOWTEXT,
    wxPG_COLOUR_CUSTOM
};

class fbColourPropertyClass : public wxSystemColourPropertyClass
{
    WX_PG_DECLARE_DERIVED_PROPERTY_CLASS()
public:
    fbColourPropertyClass( const wxString& label, const wxString& name,
	const wxColourPropertyValue& value );
    virtual ~fbColourPropertyClass ();
    virtual long GetColour ( int index );
};

static wxPGChoices gs_fbColourProperty_choicesCache;

WX_PG_IMPLEMENT_DERIVED_PROPERTY_CLASS(fbColourProperty, wxSystemColourProperty, const wxColourPropertyValue&)

fbColourPropertyClass::fbColourPropertyClass( const wxString& label, const wxString& name, const wxColourPropertyValue& value )
:
wxPG_PROPCLASS(wxSystemColourProperty)( label, name, fbcolprop_labels, fbcolprop_colours, &gs_fbColourProperty_choicesCache, value )
{
    wxPG_INIT_REQUIRED_TYPE(wxColourPropertyValue)
    m_flags |= wxPG_PROP_TRANSLATE_CUSTOM;
    DoSetValue ( &m_value );
}

fbColourPropertyClass::~fbColourPropertyClass () { }

long fbColourPropertyClass::GetColour ( int index )
{
    if ( index == wxSYS_COLOUR_MAX )
        return wxPG_COLOUR(255, 255, 255);
    else
        return wxSystemColourPropertyClass::GetColour( index );
}

// -----------------------------------------------------------------------
// wxSizeProperty
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP(wxSize,wxSizeProperty,wxSize(0,0))

class wxSizePropertyClass : public wxPGPropertyWithChildren
{
	WX_PG_DECLARE_PROPERTY_CLASS()
public:

	wxSizePropertyClass ( const wxString& label, const wxString& name,
		const wxSize& value );
	virtual ~wxSizePropertyClass ();

	WX_PG_DECLARE_PARENTAL_TYPE_METHODS()
		WX_PG_DECLARE_PARENTAL_METHODS()

protected:
	wxSize                  m_value;
};

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxSizeProperty,wxBaseParentProperty,wxSize,const wxSize&,TextCtrl)

wxSizePropertyClass::wxSizePropertyClass ( const wxString& label, const wxString& name,
										  const wxSize& value) : wxPGPropertyWithChildren(label,name)
{
	wxPG_INIT_REQUIRED_TYPE(wxSize)
	AddChild( wxIntProperty(wxT("Width"),wxPG_LABEL,value.GetWidth()) );
	AddChild( wxIntProperty(wxT("Height"),wxPG_LABEL,value.GetHeight()) );

	DoSetValue((void*)&value);

	// Only allow editing from the children
	m_flags |= wxPG_PROP_NOEDITOR;
}

wxSizePropertyClass::~wxSizePropertyClass () { }

void wxSizePropertyClass::DoSetValue ( wxPGVariant value )
{
	wxSize* pObj = (wxSize*)wxPGVariantToVoidPtr(value);
	m_value = *pObj;
	RefreshChildren();
}

wxPGVariant wxSizePropertyClass::DoGetValue () const
{
	return wxPGVariant((void*)&m_value);
}

void wxSizePropertyClass::RefreshChildren()
{
	Item(0)->DoSetValue( (long)m_value.GetWidth() );
	Item(1)->DoSetValue( (long)m_value.GetHeight() );
}

void wxSizePropertyClass::ChildChanged ( wxPGProperty* p )
{
	switch ( p->GetIndexInParent() )
	{
	case 0: m_value.SetWidth( p->DoGetValue().GetLong() ); break;
	case 1: m_value.SetHeight( p->DoGetValue().GetLong() ); break;
	}
}

// -----------------------------------------------------------------------
// wxPointProperty
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP(wxPoint,wxPointProperty,wxPoint(0,0))

class wxPointPropertyClass : public wxPGPropertyWithChildren
{
	WX_PG_DECLARE_PROPERTY_CLASS()
public:

	wxPointPropertyClass( const wxString& label, const wxString& name,
		const wxPoint& value );
	virtual ~wxPointPropertyClass ();

	WX_PG_DECLARE_PARENTAL_TYPE_METHODS()
		WX_PG_DECLARE_PARENTAL_METHODS()

protected:
	wxPoint                  m_value;
};

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxPointProperty,wxBaseParentProperty,wxPoint,const wxPoint&,TextCtrl)

wxPointPropertyClass::wxPointPropertyClass ( const wxString& label, const wxString& name,
											const wxPoint& value) : wxPGPropertyWithChildren(label,name)
{
	wxPG_INIT_REQUIRED_TYPE(wxPoint)
		DoSetValue((void*)&value);
	AddChild( wxIntProperty(wxT("X"),wxPG_LABEL,value.x) );
	AddChild( wxIntProperty(wxT("Y"),wxPG_LABEL,value.y) );
	// Only allow editing from the children
	m_flags |= wxPG_PROP_NOEDITOR;
}

wxPointPropertyClass::~wxPointPropertyClass () { }

void wxPointPropertyClass::DoSetValue ( wxPGVariant value )
{
	wxPoint* pObj = (wxPoint*)wxPGVariantToVoidPtr(value);
	m_value = *pObj;
	RefreshChildren();
}

wxPGVariant wxPointPropertyClass::DoGetValue () const
{
	return wxPGVariant((void*)&m_value);
}

void wxPointPropertyClass::RefreshChildren()
{
	if ( !GetCount() ) return;
	Item(0)->DoSetValue( m_value.x );
	Item(1)->DoSetValue( m_value.y );
}

void wxPointPropertyClass::ChildChanged ( wxPGProperty* p )
{
	switch ( p->GetIndexInParent() )
	{
		case 0:
			m_value.x = p->DoGetValue().GetLong();
			break;
		case 1:
			m_value.y = p->DoGetValue().GetLong();
			break;
	}
}

// -----------------------------------------------------------------------
// wxBitmapWithResourcePropertyClass
// -----------------------------------------------------------------------

class wxBitmapWithResourcePropertyClass : public wxPGPropertyWithChildren
{
	WX_PG_DECLARE_PROPERTY_CLASS()
public:

	wxBitmapWithResourcePropertyClass( const wxString& label, const wxString& name, const wxString& value );
	virtual ~wxBitmapWithResourcePropertyClass(){};

	WX_PG_DECLARE_PARENTAL_TYPE_METHODS()
	WX_PG_DECLARE_PARENTAL_METHODS()

protected:
	wxString m_image;
	wxString m_source;
	wxArrayString m_strings;
private:
	enum
	{
		SOURCE_FILE = 0,
		SOURCE_RESOURCE,
		SOURCE_ICON_RESOURCE
	};

	enum
	{
		ITEM_FILE_OR_RESOURCE = 0,
		ITEM_SOURCE
	};
};

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxBitmapWithResourceProperty,wxBaseParentProperty,wxString,const wxString&,TextCtrl)

wxBitmapWithResourcePropertyClass::wxBitmapWithResourcePropertyClass ( const wxString& label, const wxString& name, const wxString& value)
	: wxPGPropertyWithChildren(label,name)
{

	// Add the options
    m_strings.Add(wxT("Load From File"));
    m_strings.Add(wxT("Load From Resource"));
    m_strings.Add(wxT("Load From Icon Resource"));

	// Parse default value, ( sets m_image and m_source based on 'value' )
	DoSetValue( (void*)&value );

	// Add the appropriate child
	if ( m_source == wxT("Load From File") )
	{
		wxPGProperty* child = wxImageFileProperty( wxT("file_path"), wxPG_LABEL, m_image );
		AddChild( child );
		child->SetHelpString( wxT("Path to the image file.") );
	}
	else
	{
		wxPGProperty* child = wxStringProperty( wxT("resource_name"), wxPG_LABEL, m_image );
		AddChild( child );
		child->SetHelpString( wxT("Windows Only. Name of the resource in the .rc file.") );
	}

	wxPGProperty* child2 = wxEnumProperty(wxT("source"), wxPG_LABEL, m_strings, m_strings.Index( m_source ) );
    AddChild( child2 );
    child2->SetHelpString( 	wxT("Load From File:\n")
							wxT("Load the image from a file on disk.\n\n")
							wxT("Load From Resource:\n")
							wxT("Windows Only. Load the image from a BITMAP resource in a .rc file\n\n")
							wxT("Load From Icon Resource:\n")
							wxT("Windows Only. Load the image from a ICON resource in a .rc file\n\n")
						);

	RefreshChildren();
}


void wxBitmapWithResourcePropertyClass::DoSetValue ( wxPGVariant value )
{
	wxString* pObj = (wxString*)wxPGVariantToVoidPtr( value );
	wxString newValue = *pObj;

	// Split on the semi-colon
	size_t splitIndex = newValue.find_first_of( wxT(";") );
	if ( splitIndex != newValue.npos )
	{
		m_image = newValue.substr( 0, splitIndex );
		m_source = newValue.substr( splitIndex + 1 );
		m_source.Trim( false );
		if ( wxNOT_FOUND == m_strings.Index( m_source.c_str() )	)
		{
			m_source = wxT("Load From File");
		}
	}
	else
	{
		m_image = newValue;
		m_source = wxT("Load From File");
	}

	RefreshChildren();
}

wxPGVariant wxBitmapWithResourcePropertyClass::DoGetValue() const
{
	wxString value;
	value << m_image << wxT("; ") << m_source;
	return wxPGVariant( (void*)&value );
}

void wxBitmapWithResourcePropertyClass::RefreshChildren()
{
	if ( !GetCount() )
	{
		return;
	}

	Item( ITEM_FILE_OR_RESOURCE )->DoSetValue( m_image );
	Item( ITEM_SOURCE )->DoSetValue( m_strings.Index( m_source ) );
}

void wxBitmapWithResourcePropertyClass::ChildChanged( wxPGProperty* p )
{

}

// -----------------------------------------------------------------------
// ObjectInspector
// -----------------------------------------------------------------------

DEFINE_EVENT_TYPE( wxEVT_NEW_BITMAP_PROPERTY )

BEGIN_EVENT_TABLE(ObjectInspector, wxPanel)
	EVT_PG_CHANGED(WXFB_PROPERTY_GRID, ObjectInspector::OnPropertyGridChange)
	EVT_PG_CHANGED(WXFB_EVENT_GRID, ObjectInspector::OnEventGridChange)

	EVT_FB_OBJECT_SELECTED( ObjectInspector::OnObjectSelected )
	EVT_FB_PROJECT_REFRESH( ObjectInspector::OnProjectRefresh )
	EVT_FB_PROPERTY_MODIFIED( ObjectInspector::OnPropertyModified )

END_EVENT_TABLE()

ObjectInspector::ObjectInspector( wxWindow* parent, int id, int style )
: wxPanel(parent,id), m_style(style)
{
	AppData()->AddHandler( this->GetEventHandler() );
	m_currentSel = PObjectBase();

	long nbStyle;
	wxConfigBase* config = wxConfigBase::Get();
	config->Read( wxT("/mainframe/objectInspector/notebook_style"), &nbStyle, wxFNB_NO_X_BUTTON | wxFNB_NO_NAV_BUTTONS | wxFNB_NODRAG | wxFNB_DROPDOWN_TABS_LIST | wxFNB_FF2 | wxFNB_CUSTOM_DLG );

	m_nb = new wxFlatNotebook( this, -1, wxDefaultPosition, wxDefaultSize, nbStyle );
	m_nb->SetCustomizeOptions( wxFNB_CUSTOM_TAB_LOOK | wxFNB_CUSTOM_ORIENTATION | wxFNB_CUSTOM_LOCAL_DRAG );

	// the colour of property grid description looks ugly if we don't set this
	// colour
	m_nb->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

	// Register the slider editor
	#if wxUSE_SLIDER
	wxPGRegisterEditorClass( Slider );
	#endif

	m_pg = CreatePropertyGridManager(m_nb, WXFB_PROPERTY_GRID);
	m_eg = CreatePropertyGridManager(m_nb, WXFB_EVENT_GRID);

	m_nb->AddPage(m_pg,wxT("Properties"),false);
	m_nb->AddPage(m_eg,wxT("Events"),false);

	wxBoxSizer* topSizer = new wxBoxSizer( wxVERTICAL );
	topSizer->Add( m_nb, 1, wxALL | wxEXPAND, 0 );
	SetSizer( topSizer );
}

ObjectInspector::~ObjectInspector()
{
	AppData()->RemoveHandler( this->GetEventHandler() );
}

void ObjectInspector::SavePosition()
{
	// Save Layout
	wxConfigBase* config = wxConfigBase::Get();
	config->Write( wxT("/mainframe/objectInspector/DescBoxHeight" ), m_pg->GetDescBoxHeight() );
	config->Write( wxT("/mainframe/objectInspector/notebook_style"), m_nb->GetWindowStyleFlag() );
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
		int pageCount = (int)m_pg->GetPageCount();
		if ( pageCount > 0 )
		{
			for ( int pageIndex = pageCount - 1; pageIndex >= 0; --pageIndex )
			{
				m_pg->RemovePage( pageIndex );
			}
		}

		// now we do the same thing for event grid...
		pageCount = (int)m_eg->GetPageCount();
		if ( pageCount > 0)
		{
		  for ( int pageIndex = pageCount - 1; pageIndex >= 0; --pageIndex)
		  {
		    m_eg->RemovePage( pageIndex );
		  }
		}

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

wxPGProperty* ObjectInspector::GetProperty(PProperty prop)
{
	wxPGProperty *result;
	PropertyType type = prop->GetType();
	wxString name = prop->GetName();
	wxVariant vTrue = wxVariant( true, wxT("true") );

	if (type == PT_TEXT || type == PT_MACRO)
	{
		result = wxStringProperty(name, wxPG_LABEL, prop->GetValueAsString());
		result->SetAttribute(wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING, vTrue);
	}
	else if (type == PT_INT)
	{
		result = wxIntProperty(name, wxPG_LABEL, prop->GetValueAsInteger());
	}
	else if (type == PT_UINT)
	{
		result = wxUIntProperty(name, wxPG_LABEL, (unsigned)prop->GetValueAsInteger());
	}
  	else if (type == PT_WXSTRING || type == PT_WXSTRING_I18N)
	{
		result = wxLongStringProperty(name, wxPG_LABEL, prop->GetValueAsText());
	}
	else if (type == PT_BOOL)
	{
		result = wxBoolProperty(name, wxPG_LABEL, prop->GetValue() == wxT("1"));
	}
	else if (type == PT_BITLIST)
	{
		PPropertyInfo prop_desc = prop->GetPropertyInfo();
		POptionList opt_list = prop_desc->GetOptionList();

		assert(opt_list && opt_list->GetOptionCount() > 0);

		wxPGChoices constants;
		const std::map< wxString, wxString > options = opt_list->GetOptions();
		std::map< wxString, wxString >::const_iterator it;
		unsigned int i = 0;
		for( it = options.begin(); it != options.end(); ++it )
		{
			constants.Add( it->first, 1 << i++ );
		}

		int val = StringToBits(prop->GetValueAsString(), constants);
		result = wxFlagsProperty(name, wxPG_LABEL, constants, val);

		// Workaround to set the help strings for individual members of a wxFlagsProperty
		wxFlagsPropertyClass* flagsProp = dynamic_cast<wxFlagsPropertyClass*>(result);
		if ( NULL != flagsProp )
		{
			for ( size_t i = 0; i < flagsProp->GetCount(); i++ )
			{
				wxPGProperty* prop = flagsProp->Item( i );
				std::map< wxString, wxString >::const_iterator option = options.find( prop->GetLabel() );
				if ( option != options.end() )
				{
					m_pg->SetPropertyHelpString( prop, option->second );
				}
			}
		}
	}
	else if (type == PT_INTLIST)
	{
		result = wxStringProperty(name, wxPG_LABEL, IntList( prop->GetValueAsString() ).ToString());
	}
	else if (type == PT_OPTION)
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

		result = wxEnumProperty(name, wxPG_LABEL, constants);
		result->SetValueFromString(value, 0);
		wxString desc = prop_desc->GetDescription();
		if ( desc.empty() )
		{
			desc = value + wxT(":\n") + help;
		}
		else
		{
			desc += wxT("\n\n") + value + wxT(":\n") + help;
		}
		result->SetHelpString( desc );

	}
	else if (type == PT_WXPOINT)
	{
		result = wxPointProperty(name, wxPG_LABEL, prop->GetValueAsPoint());
	}
	else if (type == PT_WXSIZE)
	{
		result = wxSizeProperty(name, wxPG_LABEL, prop->GetValueAsSize());
	}
	else if (type == PT_WXFONT)
	{
		result = wxFontProperty(name, wxPG_LABEL, prop->GetValueAsFont());
	}
	else if (type == PT_WXCOLOUR)
	{
		wxString value = prop->GetValueAsString();
		if ( value.empty() )  // Default Colour
		{
			wxColourPropertyValue def;
            def.m_type = wxSYS_COLOUR_MAX;
            result = fbColourProperty( name, wxPG_LABEL, def );
		}
		else
		{
			if ( value.find_first_of( wxT("wx") ) == 0 )
			{
				// System Colour
				wxColourPropertyValue def;
				def.m_type = TypeConv::StringToSystemColour( value );
				result = fbColourProperty( name, wxPG_LABEL, def );
			}
			else
			{
				result = fbColourProperty( name, wxPG_LABEL, prop->GetValueAsColour() );
			}
		}
	}
	else if (type == PT_PATH)
	{
		result = wxDirProperty(name, wxPG_LABEL, prop->GetValueAsString());
	}
	else if (type == PT_BITMAP)
	{
		result = wxBitmapWithResourceProperty( name, wxPG_LABEL, prop->GetValueAsString() );
	}
	else if (type == PT_STRINGLIST)
	{
		result = wxArrayStringProperty(name, wxPG_LABEL,prop->GetValueAsArrayString());
	}
	else if (type == PT_FLOAT)
	{
		result = wxFloatProperty(name, wxPG_LABEL,prop->GetValueAsFloat());
	}
	else if ( type == PT_PARENT )
	{
		wxParentPropertyClass* parent = new wxParentPropertyClass ( name, wxPG_LABEL );

		PPropertyInfo prop_desc = prop->GetPropertyInfo();
		std::list< PropertyChild >* children = prop_desc->GetChildren();
		std::list< PropertyChild >::iterator it;
		for( it = children->begin(); it != children->end(); ++it )
		{
			wxPGProperty* child = wxStringProperty( it->m_name, wxPG_LABEL, wxEmptyString );
			parent->AddChild( child );
			m_pg->SetPropertyHelpString( child, it->m_description );
		}

		parent->SetValueFromString( prop->GetValueAsString(), wxPG_FULL_VALUE );

		result = parent;
	}
	else // propiedad desconocida
	{
		result = wxStringProperty(name, wxPG_LABEL, prop->GetValueAsString());
		result->SetAttribute(wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING, vTrue);
		wxLogError(wxT("Property type Unknown"));
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
			wxPGId id = m_pg->Append( GetProperty( prop ) );
			if ( prop->GetType() != PT_OPTION )
			{
				m_pg->SetPropertyHelpString( id, propInfo->GetDescription() );
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
				// Most common classes will be showed with a slightly different
				// colour.
				if (name == wxT("wxWindow"))
					m_pg->SetPropertyColour(id,wxColour(255,255,205)); // yellow
				else if (name == wxT("sizeritem") || name == wxT("gbsizeritem") || name == wxT("sizeritembase") )
					m_pg->SetPropertyColour(id,wxColour(220,255,255)); // cyan
			}

			properties.insert( PropertyMap::value_type( propName, prop ) );
			m_propMap.insert( ObjInspectorPropertyMap::value_type( id.GetPropertyPtr(), prop ) );
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
		m_pg->AppendIn( category->GetName(), wxPropertyCategory( nextCat->GetName() ) );
		AddItems( name, obj, obj_info, nextCat, properties );
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

		// we do not want to duplicate inherited events
		if ( events.find( eventName ) == events.end() )
		{
			wxPGProperty *pgProp = wxStringProperty( eventInfo->GetName(), wxPG_LABEL, event->GetValue() );
			wxPGId id = m_eg->Append( pgProp );
			m_eg->SetPropertyHelpString( id, eventInfo->GetDescription() );

			if (m_style != wxFB_OI_MULTIPAGE_STYLE)
			{
				// Most common classes will be showed with a slightly different
				// colour.
				if (name == wxT("wxWindow"))
					m_eg->SetPropertyColour(id,wxColour(255,255,205)); // yellow
				else if (name == wxT("sizeritem") || name == wxT("gbsizeritem") || name == wxT("sizeritembase") )
					m_eg->SetPropertyColour(id,wxColour(220,255,255)); // cyan
			}

			events.insert( EventMap::value_type( eventName, event ) );
			m_eventMap.insert( ObjInspectorEventMap::value_type( id.GetPropertyPtr(), event) );
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
		m_eg->AppendIn( category->GetName(), wxPropertyCategory( nextCat->GetName() ) );
		AddItems( name, obj, obj_info, nextCat, events );
	}
}

void ObjectInspector::OnPropertyGridChange( wxPropertyGridEvent& event )
{
	wxPGProperty* propPtr = event.GetPropertyPtr();
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
			case PT_TEXT:
			case PT_MACRO:
			case PT_FLOAT:
			case PT_INT:
			case PT_UINT:
			{
				AppData()->ModifyProperty( prop, event.GetPropertyValueAsString() );
				break;
			}
			case PT_OPTION:
			{
				wxString value = event.GetPropertyValueAsString();
				AppData()->ModifyProperty( prop, value );

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
				m_pg->SetPropertyHelpString( propPtr, helpString );
				m_pg->SetDescription( propPtr->GetLabel(), helpString );
				break;
			}
			case PT_PARENT:
			{
				AppData()->ModifyProperty( prop, propPtr->GetValueAsString( wxPG_FULL_VALUE ) );
				break;
			}
			case PT_WXSTRING:
			case PT_WXSTRING_I18N:
			{
				// las cadenas de texto del inspector son formateadas
				wxString value = TypeConv::TextToString( event.GetPropertyValueAsString() );
				AppData()->ModifyProperty( prop, value );
				break;
			}
			case PT_BOOL:
			{
				AppData()->ModifyProperty( prop, event.GetPropertyValueAsBool() ? wxT("1") : wxT("0") );
				break;
			}
			case PT_BITLIST:
			{
				wxString aux = event.GetPropertyValueAsString();
				aux.Replace( wxT(" "), wxT("") );
				aux.Replace( wxT(","), wxT("|") );
				AppData()->ModifyProperty( prop, aux );
				break;
			}
			case PT_WXPOINT:
			{
				wxPoint point = event.GetPropertyValueAsPoint ();
				AppData()->ModifyProperty( prop, wxString::Format( wxT("%i,%i"), point.x, point.y ) );
				break;
			}
			case PT_WXSIZE:
			{
				wxSize size = event.GetPropertyValueAsSize();
				AppData()->ModifyProperty( prop, wxString::Format( wxT("%i,%i"), size.GetWidth(), size.GetHeight() ) );
				break;
			}
			case PT_WXFONT:
			{
				wxFont* font = wxPGVariantToWxObjectPtr( event.GetPropertyPtr()->DoGetValue(), wxFont );
				AppData()->ModifyProperty( prop, TypeConv::FontToString( *font ) );
				break;
			}
			case PT_WXCOLOUR:
			{
				wxColourPropertyValue* colour = wxDynamicCast( event.GetPropertyValueAsWxObjectPtr(), wxColourPropertyValue );
				switch ( colour->m_type )
				{
				    case wxSYS_COLOUR_MAX:
                        AppData()->ModifyProperty( prop, _T("") );
                        break;
                    case wxPG_COLOUR_CUSTOM:
                        AppData()->ModifyProperty( prop, TypeConv::ColourToString( colour->m_colour ) );
                        break;
                    default:
                        AppData()->ModifyProperty( prop, TypeConv::SystemColourToString( colour->m_type ) );
				}
				break;
			}
			case PT_STRINGLIST:
			{
				const wxArrayString &arraystr = event.GetPropertyValueAsArrayString();
				AppData()->ModifyProperty(prop, TypeConv::ArrayStringToString(arraystr));
				break;
			}
			case PT_BITMAP:
			{
				// TODO: Usar ruta relativa al directorio de salida en el caso
				//       de que la imagen se encuentre en un subdirectorio de este.

				// Get property value
				wxString path = event.GetPropertyValueAsString();
				size_t semicolon_index = path.find_first_of( wxT(";") );
				if ( semicolon_index != path.npos )
				{
					path = TypeConv::MakeRelativePath( path.substr( 0, semicolon_index ), AppData()->GetProjectPath() ) + path.substr( semicolon_index  );
				}

				// Save state from old property to use after grid is recreated
				wxPGPropertyWithChildren* pwc = dynamic_cast< wxPGPropertyWithChildren* >( event.GetPropertyPtr() );
				bool expanded = false;
				if ( pwc )
				{
					expanded = pwc->IsExpanded();
				}
				wxString name = event.GetPropertyName();

				// Respond to property modification
				AppData()->ModifyProperty( prop, path );

				// Recreate grid, the bitmap property may need to change
				Create(true);

				// Re-expand the bitmap property, if it was expanded
				wxPGId bitmapProp = m_pg->GetPropertyByName( name );
				if ( expanded )
				{
					m_pg->Expand( bitmapProp );
				}
				m_pg->SelectProperty( bitmapProp );

				break;
			}

			default:
				AppData()->ModifyProperty( prop, event.GetPropertyValueAsString() );
		}
	}
}

void ObjectInspector::OnEventGridChange(wxPropertyGridEvent& event)
{
	ObjInspectorEventMap::iterator it = m_eventMap.find( event.GetPropertyPtr() );

	if ( it != m_eventMap.end() )
	{
		PEvent evt = it->second;
		AppData()->ModifyEventHandler( evt, event.GetPropertyValueAsString() );
	}
}

///////////////////////////////////////////////////////////////////////////////
void ObjectInspector::OnObjectSelected( wxFBObjectEvent& event )
{
	Create();
}

void ObjectInspector::OnProjectRefresh( wxFBEvent& event )
{
	Create(true);
}

void ObjectInspector::OnPropertyModified( wxFBPropertyEvent& event )
{
	PProperty prop = event.GetFBProperty();

	if (prop->GetObject() != AppData()->GetSelectedObject())
	  return;

	wxPGId pgid = m_pg->GetPropertyByLabel(prop->GetName());
	if (!pgid.IsOk()) return; // Puede que no se esté mostrando ahora esa página
	wxPGProperty *pgProp = pgid.GetPropertyPtr();

	switch (prop->GetType())
	{
	case PT_TEXT:
	case PT_MACRO:
	case PT_OPTION:
	case PT_FLOAT:
	case PT_PARENT:
	case PT_INT:
	case PT_UINT:
		pgProp->SetValueFromString(prop->GetValueAsString(), 0);
		break;
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
		m_pg->SetPropertyValue(pgid, prop->GetValueAsPoint());
		break;
	case PT_WXSIZE:
		m_pg->SetPropertyValue(pgid, prop->GetValueAsSize());
		break;
	case PT_WXFONT:
		{
			wxFont val = prop->GetValueAsFont();
			pgProp->DoSetValue((void*)&val);
		}
		break;
	case PT_WXCOLOUR:
		{
			wxString value = prop->GetValueAsString();
			if ( value.empty() )  // Default Colour
			{
				wxColourPropertyValue def( wxSYS_COLOUR_MAX, wxColour( 255, 255, 255 ) );
				m_pg->SetPropertyValue( pgid, def );
			}
			else
			{
				if ( value.find_first_of( wxT("wx") ) == 0 )
				{
					// System Colour
					wxColourPropertyValue def;
					def.m_type = TypeConv::StringToSystemColour( value );
					def.m_colour = prop->GetValueAsColour();
					m_pg->SetPropertyValue( pgid, def );
				}
				else
				{
					wxColourPropertyValue def( wxPG_COLOUR_CUSTOM, prop->GetValueAsColour() );
					m_pg->SetPropertyValue( pgid, def );
				}
			}
		}
		break;
	case PT_STRINGLIST:
		{
			wxArrayString val = prop->GetValueAsArrayString();
			pgProp->DoSetValue((void*)&val);
		}
		break;
	default:
		pgProp->SetValueFromString(prop->GetValueAsString(), 0);
	}
	m_pg->Refresh();
}

wxPropertyGridManager* ObjectInspector::CreatePropertyGridManager(wxWindow *parent, wxWindowID id)
{
	int pgStyle;
	int defaultDescBoxHeight;

	switch (m_style)
	{
		case wxFB_OI_MULTIPAGE_STYLE:

			pgStyle = 	wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR |
						wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE;
			defaultDescBoxHeight = 50;
			break;

		case wxFB_OI_DEFAULT_STYLE:
		case wxFB_OI_SINGLE_PAGE_STYLE:
		default:

			pgStyle = 	wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_DESCRIPTION |
						wxPGMAN_DEFAULT_STYLE;
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
	pg->SetDescBoxHeight( descBoxHeight );
	pg->SetExtraStyle( wxPG_EX_NATIVE_DOUBLE_BUFFERING );
	pg->SetExtraStyle( wxPG_EX_PROCESS_EVENTS_IMMEDIATELY );

	return pg;
}
