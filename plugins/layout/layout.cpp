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

#include <component.h>
#include <plugin.h>
#include <xrcconv.h>
#include <tinyxml.h>
#include <wx/tokenzr.h>

#ifdef __WX24__
	#define wxFIXED_MINSIZE wxADJUST_MINSIZE
#endif

class SpacerComponent : public ComponentBase
{
private:
	void AddSizeProperty(XrcToXfbFilter &filter, TiXmlElement *xrcObj)
	{
		TiXmlElement *sizeProp = xrcObj->FirstChildElement("size");
		if (sizeProp)
		{
			TiXmlText *xmlValue = sizeProp->FirstChild()->ToText();
			if (xmlValue)
			{
				wxString width = wxT("");
				wxString height = wxT("");
				wxStringTokenizer tkz(wxString(xmlValue->Value(),wxConvUTF8),wxT(","));
				if (tkz.HasMoreTokens())
				{
					width = tkz.GetNextToken();
					if (tkz.HasMoreTokens())
						height = tkz.GetNextToken();
				}
				filter.AddPropertyValue(_("width"),width);
				filter.AddPropertyValue(_("height"),height);
			}
		}
	}

public:
	void OnCreated( wxObject* wxobject, wxWindow* wxparent )
	{
		// Get parent sizer
		wxObject* parent = GetManager()->GetParent( wxobject );
		wxSizer* sizer = wxDynamicCast( parent, wxSizer );

		if ( NULL == sizer )
		{
			wxLogError( wxT("The parent of a SizerItem is either missing or not a wxSizer - this should not be possible!") );
			return;
		}

		// Get IObject to read property values
		IObject* obj = GetManager()->GetIObject( wxobject );

		sizer->Add(
			obj->GetPropertyAsInteger(_("width")),
			obj->GetPropertyAsInteger(_("height")),
			obj->GetPropertyAsInteger(_("proportion")),
			obj->GetPropertyAsInteger(_("flag")),
			obj->GetPropertyAsInteger(_("border")));
	}

	TiXmlElement* ExportToXrc(IObject *obj)
	{
		ObjectToXrcFilter xrc(obj, _("spacer"));

		// las propiedades width y height se mapean como size
		xrc.AddPropertyValue(_("size"),
			wxString::Format(_("%d,%d"),obj->GetPropertyAsInteger(_("width")),
			obj->GetPropertyAsInteger(_("height"))));

		xrc.AddProperty(_("proportion"), _("option"), XRC_TYPE_INTEGER);
		xrc.AddProperty(_("flag"),   _("flag"),   XRC_TYPE_BITLIST);
		xrc.AddProperty(_("border"), _("border"), XRC_TYPE_INTEGER);
		return xrc.GetXrcObject();
	}

	TiXmlElement* ImportFromXrc(TiXmlElement *xrcObj)
	{
		XrcToXfbFilter filter(xrcObj, _("spacer"));

		// la propiedad "size" hay que descomponerla en weight y height
		AddSizeProperty(filter, xrcObj);

		filter.AddProperty(_("option"), _("proportion"), XRC_TYPE_INTEGER);
		filter.AddProperty(_("flag"),   _("flag"),   XRC_TYPE_BITLIST);
		filter.AddProperty(_("border"), _("border"), XRC_TYPE_INTEGER);
		return filter.GetXfbObject();
	}
};

class SizerItemComponent : public ComponentBase
{
public:
	void OnCreated( wxObject* wxobject, wxWindow* wxparent )
	{
		// Get parent sizer
		wxObject* parent = GetManager()->GetParent( wxobject );
		wxSizer* sizer = wxDynamicCast( parent, wxSizer );

		if ( NULL == sizer )
		{
			wxLogError( wxT("The parent of a SizerItem is either missing or not a wxSizer - this should not be possible!") );
			return;
		}

		// Get child window
		wxObject* child = GetManager()->GetChild( wxobject, 0 );
		if ( NULL == child )
		{
			wxLogError( wxT("The SizerItem component has no child - this should not be possible!") );
			return;
		}

		// Get IObject for property access
		IObject* obj = GetManager()->GetIObject( wxobject );

		// Add the child ( window or sizer ) to the sizer
		wxWindow* windowChild = wxDynamicCast( child, wxWindow );
		wxSizer* sizerChild = wxDynamicCast( child, wxSizer );

		if ( windowChild != NULL )
		{
			sizer->Add( windowChild,
				obj->GetPropertyAsInteger(_("proportion")),
				obj->GetPropertyAsInteger(_("flag")),
				obj->GetPropertyAsInteger(_("border")));
		}
		else if ( sizerChild != NULL )
		{
			sizer->Add( sizerChild,
				obj->GetPropertyAsInteger(_("proportion")),
				obj->GetPropertyAsInteger(_("flag")),
				obj->GetPropertyAsInteger(_("border")));
		}
		else
		{
			wxLogError( wxT("The SizerItem component's child is not a wxWindow or a wxSizer - this should not be possible!") );
		}
	}

	TiXmlElement* ExportToXrc(IObject *obj)
	{
		ObjectToXrcFilter xrc(obj, _("sizeritem"));
		xrc.AddProperty(_("proportion"), _("option"), XRC_TYPE_INTEGER);
		xrc.AddProperty(_("flag"),   _("flag"),   XRC_TYPE_BITLIST);
		xrc.AddProperty(_("border"), _("border"), XRC_TYPE_INTEGER);
		return xrc.GetXrcObject();
	}

	TiXmlElement* ImportFromXrc(TiXmlElement *xrcObj)
	{
		XrcToXfbFilter filter(xrcObj, _("sizeritem"));
		filter.AddProperty(_("option"), _("proportion"), XRC_TYPE_INTEGER);
		filter.AddProperty(_("flag"),   _("flag"),   XRC_TYPE_BITLIST);
		filter.AddProperty(_("border"), _("border"), XRC_TYPE_INTEGER);
		return filter.GetXfbObject();
	}
};

class BoxSizerComponent : public ComponentBase
{
public:
	wxObject* Create(IObject *obj, wxObject *parent)
	{
		return new wxBoxSizer(obj->GetPropertyAsInteger(_("orient")));
	}

	TiXmlElement* ExportToXrc(IObject *obj)
	{
		ObjectToXrcFilter xrc(obj, _("wxBoxSizer"));
		xrc.AddProperty(_("orient"), _("orient"), XRC_TYPE_TEXT);
		return xrc.GetXrcObject();
	}

	TiXmlElement* ImportFromXrc(TiXmlElement *xrcObj)
	{
		XrcToXfbFilter filter(xrcObj, _("wxBoxSizer"));
		filter.AddProperty(_("orient"),_("orient"),XRC_TYPE_TEXT);
		return filter.GetXfbObject();
	}
};

class StaticBoxSizerComponent : public ComponentBase
{
public:
	int m_count;
	StaticBoxSizerComponent()
	{
		m_count = 0;
	}
	wxObject* Create(IObject *obj, wxObject *parent)
	{
		m_count++;
		wxStaticBox* box = new wxStaticBox((wxWindow *)parent, -1,
			obj->GetPropertyAsString(_("label")));

		wxStaticBoxSizer* sizer = new wxStaticBoxSizer(box,
			obj->GetPropertyAsInteger(_("orient")));

		return sizer;
	}

	TiXmlElement* ExportToXrc(IObject *obj)
	{
		ObjectToXrcFilter xrc(obj, _("wxStaticBoxSizer"));
		xrc.AddProperty(_("orient"), _("orient"), XRC_TYPE_TEXT);
		xrc.AddProperty(_("label"), _("label"), XRC_TYPE_TEXT);
		return xrc.GetXrcObject();
	}

	TiXmlElement* ImportFromXrc(TiXmlElement *xrcObj)
	{
		XrcToXfbFilter filter(xrcObj, _("wxStaticBoxSizer"));
		filter.AddProperty(_("orient"),_("orient"),XRC_TYPE_TEXT);
		filter.AddProperty(_("label"),_("label"),XRC_TYPE_TEXT);
		return filter.GetXfbObject();
	}
};

class GridSizerComponent : public ComponentBase
{
public:
	wxObject* Create(IObject *obj, wxObject *parent)
	{
		return new wxGridSizer(
			obj->GetPropertyAsInteger(_("rows")),
			obj->GetPropertyAsInteger(_("cols")),
			obj->GetPropertyAsInteger(_("vgap")),
			obj->GetPropertyAsInteger(_("hgap")));
	}

	TiXmlElement* ExportToXrc(IObject *obj)
	{
		ObjectToXrcFilter xrc(obj, _("wxGridSizer"));
		xrc.AddProperty(_("rows"), _("rows"), XRC_TYPE_INTEGER);
		xrc.AddProperty(_("cols"), _("cols"), XRC_TYPE_INTEGER);
		xrc.AddProperty(_("vgap"), _("vgap"), XRC_TYPE_INTEGER);
		xrc.AddProperty(_("hgap"), _("hgap"), XRC_TYPE_INTEGER);
		return xrc.GetXrcObject();
	}

	TiXmlElement* ImportFromXrc(TiXmlElement *xrcObj)
	{
		XrcToXfbFilter filter(xrcObj, _("wxGridSizer"));
		filter.AddProperty(_("rows"), _("rows"), XRC_TYPE_INTEGER);
		filter.AddProperty(_("cols"), _("cols"), XRC_TYPE_INTEGER);
		filter.AddProperty(_("vgap"), _("vgap"), XRC_TYPE_INTEGER);
		filter.AddProperty(_("hgap"), _("hgap"), XRC_TYPE_INTEGER);
		return filter.GetXfbObject();
	}
};

class FlexGridSizerComponent : public ComponentBase
{
public:
	wxObject* Create(IObject *obj, wxObject *parent)
	{
		wxFlexGridSizer *sizer = new wxFlexGridSizer(
			obj->GetPropertyAsInteger(_("rows")),
			obj->GetPropertyAsInteger(_("cols")),
			obj->GetPropertyAsInteger(_("vgap")),
			obj->GetPropertyAsInteger(_("hgap")));

		wxArrayInt gcols, grows;
		gcols = obj->GetPropertyAsArrayInt(_("growablecols"));
		grows = obj->GetPropertyAsArrayInt(_("growablerows"));

		unsigned int i;
		for (i=0; i < gcols.GetCount() ; i++)
			sizer->AddGrowableCol(gcols[i]);

		for (i=0; i < grows.GetCount() ; i++)
			sizer->AddGrowableRow(grows[i]);

		sizer->SetFlexibleDirection( obj->GetPropertyAsInteger(_("flexible_direction")) );
		sizer->SetNonFlexibleGrowMode( (wxFlexSizerGrowMode )obj->GetPropertyAsInteger(_("non_flexible_grow_mode")) );

		return sizer;
	}

	TiXmlElement* ExportToXrc(IObject *obj)
	{
		ObjectToXrcFilter xrc(obj, _("wxFlexGridSizer"));
		xrc.AddProperty(_("rows"), _("rows"), XRC_TYPE_INTEGER);
		xrc.AddProperty(_("cols"), _("cols"), XRC_TYPE_INTEGER);
		xrc.AddProperty(_("vgap"), _("vgap"), XRC_TYPE_INTEGER);
		xrc.AddProperty(_("hgap"), _("hgap"), XRC_TYPE_INTEGER);

		xrc.AddPropertyValue(_("growablecols"), obj->GetPropertyAsString(_("growablecols")));
		xrc.AddPropertyValue(_("growablerows"), obj->GetPropertyAsString(_("growablerows")));
		return xrc.GetXrcObject();
	}

	TiXmlElement* ImportFromXrc(TiXmlElement *xrcObj)
	{
		XrcToXfbFilter filter(xrcObj, _("wxFlexGridSizer"));
		filter.AddProperty(_("rows"), _("rows"), XRC_TYPE_INTEGER);
		filter.AddProperty(_("cols"), _("cols"), XRC_TYPE_INTEGER);
		filter.AddProperty(_("vgap"), _("vgap"), XRC_TYPE_INTEGER);
		filter.AddProperty(_("hgap"), _("hgap"), XRC_TYPE_INTEGER);
		filter.AddProperty(_("growablecols"),_("growablecols"),XRC_TYPE_TEXT);
		filter.AddProperty(_("growablerows"),_("growablerows"),XRC_TYPE_TEXT);
		return filter.GetXfbObject();
	}
};

class StdDialogButtonSizerComponent : public ComponentBase
{
public:
	wxObject* Create(IObject *obj, wxObject *parent)
	{
		wxStdDialogButtonSizer* sizer =  new wxStdDialogButtonSizer();
		if ( obj->GetPropertyAsInteger( _("OK") ) )
		{
			sizer->AddButton( new wxButton( (wxWindow*)parent, wxID_OK ) );
		}
		if ( obj->GetPropertyAsInteger( _("Yes") ) )
		{
			sizer->AddButton( new wxButton( (wxWindow*)parent, wxID_YES ) );
		}
		if ( obj->GetPropertyAsInteger( _("Save") ) )
		{
			sizer->AddButton( new wxButton( (wxWindow*)parent, wxID_SAVE ) );
		}
		if ( obj->GetPropertyAsInteger( _("Apply") ) )
		{
			sizer->AddButton( new wxButton( (wxWindow*)parent, wxID_APPLY ) );
		}
		if ( obj->GetPropertyAsInteger( _("No") ) )
		{
			sizer->AddButton( new wxButton( (wxWindow*)parent, wxID_NO ) );
		}
		if ( obj->GetPropertyAsInteger( _("Cancel") ) )
		{
			sizer->AddButton( new wxButton( (wxWindow*)parent, wxID_CANCEL ) );
		}
		if ( obj->GetPropertyAsInteger( _("Help") ) )
		{
			sizer->AddButton( new wxButton( (wxWindow*)parent, wxID_HELP ) );
		}
		if ( obj->GetPropertyAsInteger( _("ContextHelp") ) )
		{
			sizer->AddButton( new wxButton( (wxWindow*)parent, wxID_CONTEXT_HELP ) );
		}
		sizer->Realize();
		return sizer;
	}

};

///////////////////////////////////////////////////////////////////////////////

BEGIN_LIBRARY()
ABSTRACT_COMPONENT("spacer",SpacerComponent)
ABSTRACT_COMPONENT("sizeritem",SizerItemComponent)

SIZER_COMPONENT("wxBoxSizer",BoxSizerComponent)
SIZER_COMPONENT("wxStaticBoxSizer",StaticBoxSizerComponent)
SIZER_COMPONENT("wxGridSizer",GridSizerComponent)
SIZER_COMPONENT("wxFlexGridSizer",FlexGridSizerComponent)
SIZER_COMPONENT("wxStdDialogButtonSizer",StdDialogButtonSizerComponent)

// wxBoxSizer
MACRO(wxHORIZONTAL)
MACRO(wxVERTICAL)

// wxFlexGridSizer
MACRO(wxBOTH)
MACRO(wxFLEX_GROWMODE_NONE)
MACRO(wxFLEX_GROWMODE_SPECIFIED)
MACRO(wxFLEX_GROWMODE_ALL)


// Add
MACRO(wxALL)
MACRO(wxLEFT)
MACRO(wxRIGHT)
MACRO(wxTOP)
MACRO(wxBOTTOM)
MACRO(wxEXPAND)
MACRO(wxALIGN_BOTTOM)
MACRO(wxALIGN_CENTER)
MACRO(wxALIGN_CENTER_HORIZONTAL)
MACRO(wxALIGN_CENTER_VERTICAL)
MACRO(wxSHAPED)
MACRO(wxFIXED_MINSIZE)

SYNONYMOUS(wxGROW, wxEXPAND)


END_LIBRARY()

