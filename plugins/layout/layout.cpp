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

#include <plugin.h>
#include <ticpp.h>
#include <wx/gbsizer.h>
#include <wx/wrapsizer.h>
#include <xrcconv.h>

#ifdef __WX24__
	#define wxFIXED_MINSIZE wxADJUST_MINSIZE
#endif

class SpacerComponent : public ComponentBase
{
public:
	// ImportFromXRC is handled in sizeritem components

	ticpp::Element* ExportToXrc(IObject* obj) override {
		ObjectToXrcFilter xrc(obj, _("spacer"));
		xrc.AddPropertyPair( _("width"), _("height"), _("size") );
		return xrc.GetXrcObject();
	}
};

class GBSizerItemComponent : public ComponentBase
{
public:
	ticpp::Element* ExportToXrc(IObject* obj) override {
		ObjectToXrcFilter xrc(obj, _("sizeritem"));
		xrc.AddPropertyPair( _("row"), _("column"), _("cellpos") );
		xrc.AddPropertyPair( _("rowspan"), _("colspan"), _("cellspan") );
		xrc.AddProperty(_("flag"),   _("flag"),   XRC_TYPE_BITLIST);
		xrc.AddProperty(_("border"), _("border"), XRC_TYPE_INTEGER);
		return xrc.GetXrcObject();
	}

	ticpp::Element* ImportFromXrc(ticpp::Element* xrcObj) override {
		// XrcLoader::GetObject imports spacers as sizeritems
		XrcToXfbFilter filter(xrcObj, _("gbsizeritem"));
		filter.AddPropertyPair( "cellpos", _("row"), _("column") );
		filter.AddPropertyPair( "cellspan", _("rowspan"), _("colspan") );
		filter.AddProperty(_("flag"),   _("flag"),   XRC_TYPE_BITLIST);
		filter.AddProperty(_("border"), _("border"), XRC_TYPE_INTEGER);
		ticpp::Element* sizeritem = filter.GetXfbObject();

		// XrcLoader::GetObject imports spacers as sizeritems, so check for a spacer
		if ( xrcObj->FirstChildElement( "size", false ) && !xrcObj->FirstChildElement( "object", false ) )
		{
			// it is a spacer
			XrcToXfbFilter spacer( xrcObj, _("spacer") );
			spacer.AddPropertyPair( "size", _("width"), _("height") );
			sizeritem->LinkEndChild( spacer.GetXfbObject() );
		}

		return sizeritem;
	}
};

class SizerItemComponent : public ComponentBase
{
public:
	void OnCreated(wxObject* wxobject, wxWindow* /*wxparent*/) override {
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
		IObject* childObj = GetManager()->GetIObject( child );

		// Add the spacer
		if ( _("spacer") == childObj->GetClassName() )
		{
			sizer->Add(	childObj->GetPropertyAsInteger( _("width") ),
						childObj->GetPropertyAsInteger( _("height") ),
						obj->GetPropertyAsInteger(_("proportion")),
						obj->GetPropertyAsInteger(_("flag")),
						obj->GetPropertyAsInteger(_("border"))
						);
			return;
		}

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
			wxLogError( wxT("The SizerItem component's child is not a wxWindow or a wxSizer or a spacer - this should not be possible!") );
		}
	}

	ticpp::Element* ExportToXrc(IObject* obj) override {
		ObjectToXrcFilter xrc(obj, _("sizeritem"));
		xrc.AddProperty(_("proportion"), _("option"), XRC_TYPE_INTEGER);
		xrc.AddProperty(_("flag"),   _("flag"),   XRC_TYPE_BITLIST);
		xrc.AddProperty(_("border"), _("border"), XRC_TYPE_INTEGER);
		return xrc.GetXrcObject();
	}

	ticpp::Element* ImportFromXrc(ticpp::Element* xrcObj) override {
		XrcToXfbFilter filter(xrcObj, _("sizeritem"));
		filter.AddProperty(_("option"), _("proportion"), XRC_TYPE_INTEGER);
		filter.AddProperty(_("flag"),   _("flag"),   XRC_TYPE_BITLIST);
		filter.AddProperty(_("border"), _("border"), XRC_TYPE_INTEGER);
		ticpp::Element* sizeritem = filter.GetXfbObject();

		// XrcLoader::GetObject imports spacers as sizeritems, so check for a spacer
		if ( xrcObj->FirstChildElement("size", false ) && !xrcObj->FirstChildElement("object", false ) )
		{
			// it is a spacer
			XrcToXfbFilter spacer( xrcObj, _("spacer") );
			spacer.AddPropertyPair( "size", _("width"), _("height") );
			sizeritem->LinkEndChild( spacer.GetXfbObject() );
		}
		return sizeritem;
	}
};

class BoxSizerComponent : public ComponentBase
{
public:
	wxObject* Create(IObject* obj, wxObject* /*parent*/) override {
		wxBoxSizer *sizer = new wxBoxSizer(obj->GetPropertyAsInteger(_("orient")));
		sizer->SetMinSize( obj->GetPropertyAsSize(_("minimum_size")) );
		return sizer;
	}

	ticpp::Element* ExportToXrc(IObject* obj) override {
		ObjectToXrcFilter xrc(obj, _("wxBoxSizer"));
		if( obj->GetPropertyAsSize(_("minimum_size")) != wxDefaultSize ) xrc.AddProperty(_("minimum_size"), _("minsize"), XRC_TYPE_SIZE);
		xrc.AddProperty(_("orient"), _("orient"), XRC_TYPE_TEXT);
		return xrc.GetXrcObject();
	}

	ticpp::Element* ImportFromXrc( ticpp::Element* xrcObj ) override
	{
		XrcToXfbFilter filter(xrcObj, _("wxBoxSizer"));
		filter.AddProperty(_("minsize"), _("minimum_size"), XRC_TYPE_SIZE);
		filter.AddProperty(_("orient"),_("orient"),XRC_TYPE_TEXT);
		return filter.GetXfbObject();
	}
};

class WrapSizerComponent : public ComponentBase
{
public:
	wxObject* Create(IObject* obj, wxObject* /*parent*/) override {
		const auto sizer = new wxWrapSizer(obj->GetPropertyAsInteger(_("orient")),
		                                   obj->GetPropertyAsInteger(_("flags")));
		sizer->SetMinSize( obj->GetPropertyAsSize(_("minimum_size")) );
		return sizer;
	}

	ticpp::Element* ExportToXrc(IObject* obj) override {
		ObjectToXrcFilter xrc(obj, _("wxWrapSizer"));
		if (obj->GetPropertyAsSize(_("minimum_size")) != wxDefaultSize)
		{
			xrc.AddProperty(_("minimum_size"), _("minsize"), XRC_TYPE_SIZE);
		}
		xrc.AddProperty(_("orient"), _("orient"), XRC_TYPE_TEXT);
		xrc.AddProperty(_("flags"), _("flags"), XRC_TYPE_BITLIST);
		return xrc.GetXrcObject();
	}

	ticpp::Element* ImportFromXrc(ticpp::Element* xrcObj) override {
		XrcToXfbFilter filter(xrcObj, _("wxWrapSizer"));
		filter.AddProperty(_("minsize"), _("minimum_size"), XRC_TYPE_SIZE);
		filter.AddProperty(_("orient"), _("orient"), XRC_TYPE_TEXT);
		filter.AddProperty(_("flags"), _("flags"), XRC_TYPE_BITLIST);
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
	wxObject* Create(IObject* obj, wxObject* parent) override {
		m_count++;
		wxStaticBox* box = new wxStaticBox((wxWindow *)parent, wxID_ANY,
			obj->GetPropertyAsString(_("label")));

		wxStaticBoxSizer* sizer = new wxStaticBoxSizer(box,
			obj->GetPropertyAsInteger(_("orient")));

		sizer->SetMinSize( obj->GetPropertyAsSize(_("minimum_size")) );

		return sizer;
	}

	ticpp::Element* ExportToXrc(IObject* obj) override {
		ObjectToXrcFilter xrc(obj, _("wxStaticBoxSizer"));
		if( obj->GetPropertyAsSize(_("minimum_size")) != wxDefaultSize ) xrc.AddProperty(_("minimum_size"), _("minsize"), XRC_TYPE_SIZE);
		xrc.AddProperty(_("orient"), _("orient"), XRC_TYPE_TEXT);
		xrc.AddProperty(_("label"), _("label"), XRC_TYPE_TEXT);
		return xrc.GetXrcObject();
	}

	ticpp::Element* ImportFromXrc(ticpp::Element* xrcObj) override {
		XrcToXfbFilter filter(xrcObj, _("wxStaticBoxSizer"));
		filter.AddProperty(_("minsize"), _("minimum_size"), XRC_TYPE_SIZE);
		filter.AddProperty(_("orient"),_("orient"),XRC_TYPE_TEXT);
		filter.AddProperty(_("label"),_("label"),XRC_TYPE_TEXT);
		return filter.GetXfbObject();
	}
};

class GridSizerComponent : public ComponentBase
{
public:
	wxObject* Create(IObject* obj, wxObject* /*parent*/) override {
		wxGridSizer *sizer = new wxGridSizer(
			obj->GetPropertyAsInteger(_("rows")),
			obj->GetPropertyAsInteger(_("cols")),
			obj->GetPropertyAsInteger(_("vgap")),
			obj->GetPropertyAsInteger(_("hgap")));

		sizer->SetMinSize( obj->GetPropertyAsSize(_("minimum_size")) );

		return sizer;
	}

	ticpp::Element* ExportToXrc(IObject* obj) override {
		ObjectToXrcFilter xrc(obj, _("wxGridSizer"));
		if( obj->GetPropertyAsSize(_("minimum_size")) != wxDefaultSize ) xrc.AddProperty(_("minimum_size"), _("minsize"), XRC_TYPE_SIZE);
		xrc.AddProperty(_("rows"), _("rows"), XRC_TYPE_INTEGER);
		xrc.AddProperty(_("cols"), _("cols"), XRC_TYPE_INTEGER);
		xrc.AddProperty(_("vgap"), _("vgap"), XRC_TYPE_INTEGER);
		xrc.AddProperty(_("hgap"), _("hgap"), XRC_TYPE_INTEGER);
		return xrc.GetXrcObject();
	}

	ticpp::Element* ImportFromXrc(ticpp::Element* xrcObj) override {
		XrcToXfbFilter filter(xrcObj, _("wxGridSizer"));
		filter.AddProperty(_("minsize"), _("minimum_size"), XRC_TYPE_SIZE);
		filter.AddProperty(_("rows"), _("rows"), XRC_TYPE_INTEGER);
		filter.AddProperty(_("cols"), _("cols"), XRC_TYPE_INTEGER);
		filter.AddProperty(_("vgap"), _("vgap"), XRC_TYPE_INTEGER);
		filter.AddProperty(_("hgap"), _("hgap"), XRC_TYPE_INTEGER);
		return filter.GetXfbObject();
	}
};

class FlexGridSizerBase : public ComponentBase
{
public:
	void AddProperties( IObject* obj, wxFlexGridSizer* sizer )
	{
		for (const auto& col : obj->GetPropertyAsVectorIntPair(_("growablecols")))
		{
			sizer->AddGrowableCol(col.first, col.second);
		}
		for (const auto& row : obj->GetPropertyAsVectorIntPair(_("growablerows")))
		{
			sizer->AddGrowableRow(row.first, row.second);
		}
		sizer->SetMinSize( obj->GetPropertyAsSize(_("minimum_size")) );
		sizer->SetFlexibleDirection( obj->GetPropertyAsInteger(_("flexible_direction")) );
		sizer->SetNonFlexibleGrowMode( (wxFlexSizerGrowMode )obj->GetPropertyAsInteger(_("non_flexible_grow_mode")) );
	}

	void ExportXRCProperties( ObjectToXrcFilter* xrc, IObject* obj )
	{
		if( obj->GetPropertyAsSize(_("minimum_size")) != wxDefaultSize ) xrc->AddProperty(_("minimum_size"), _("minsize"), XRC_TYPE_SIZE);
		xrc->AddProperty(_("vgap"), _("vgap"), XRC_TYPE_INTEGER);
		xrc->AddProperty(_("hgap"), _("hgap"), XRC_TYPE_INTEGER);
		xrc->AddPropertyValue(_("growablecols"), obj->GetPropertyAsString(_("growablecols")));
		xrc->AddPropertyValue(_("growablerows"), obj->GetPropertyAsString(_("growablerows")));
	}

	void ImportXRCProperties( XrcToXfbFilter* filter )
	{
		filter->AddProperty(_("minsize"), _("minimum_size"), XRC_TYPE_SIZE);
		filter->AddProperty(_("vgap"), _("vgap"), XRC_TYPE_INTEGER);
		filter->AddProperty(_("hgap"), _("hgap"), XRC_TYPE_INTEGER);
		filter->AddProperty(_("growablecols"),_("growablecols"),XRC_TYPE_TEXT);
		filter->AddProperty(_("growablerows"),_("growablerows"),XRC_TYPE_TEXT);
	}
};

class FlexGridSizerComponent : public FlexGridSizerBase
{
public:
	wxObject* Create(IObject* obj, wxObject* /*parent*/) override {
		wxFlexGridSizer *sizer = new wxFlexGridSizer(
			obj->GetPropertyAsInteger(_("rows")),
			obj->GetPropertyAsInteger(_("cols")),
			obj->GetPropertyAsInteger(_("vgap")),
			obj->GetPropertyAsInteger(_("hgap")));

		AddProperties( obj, sizer );

		return sizer;
	}

	ticpp::Element* ExportToXrc(IObject* obj) override {
		ObjectToXrcFilter xrc(obj, _("wxFlexGridSizer"));
		xrc.AddProperty(_("rows"), _("rows"), XRC_TYPE_INTEGER);
		xrc.AddProperty(_("cols"), _("cols"), XRC_TYPE_INTEGER);
		ExportXRCProperties( &xrc, obj );
		return xrc.GetXrcObject();
	}

	ticpp::Element* ImportFromXrc(ticpp::Element* xrcObj) override {
		XrcToXfbFilter filter(xrcObj, _("wxFlexGridSizer"));
		filter.AddProperty(_("rows"), _("rows"), XRC_TYPE_INTEGER);
		filter.AddProperty(_("cols"), _("cols"), XRC_TYPE_INTEGER);
		ImportXRCProperties( &filter );
		return filter.GetXfbObject();
	}
};

class GridBagSizerComponent : public FlexGridSizerBase
{
private:
	wxGBSizerItem* GetGBSizerItem( IObject* sizeritem, const wxGBPosition& position, const wxGBSpan& span, wxObject* child )
	{
		IObject* childObj = GetManager()->GetIObject( child );

		if ( _("spacer") == childObj->GetClassName() )
		{
			return new wxGBSizerItem(	childObj->GetPropertyAsInteger( _("width") ),
										childObj->GetPropertyAsInteger( _("height") ),
										position,
										span,
										sizeritem->GetPropertyAsInteger(_("flag")),
										sizeritem->GetPropertyAsInteger(_("border")),
										NULL
										);
		}

		// Add the child ( window or sizer ) to the sizer
		wxWindow* windowChild = wxDynamicCast( child, wxWindow );
		wxSizer* sizerChild = wxDynamicCast( child, wxSizer );

		if ( windowChild != NULL )
		{
			return new wxGBSizerItem( 	windowChild,
										position,
										span,
										sizeritem->GetPropertyAsInteger(_("flag")),
										sizeritem->GetPropertyAsInteger(_("border")),
										NULL
										);
		}
		else if ( sizerChild != NULL )
		{
			return new wxGBSizerItem( 	sizerChild,
										position,
										span,
										sizeritem->GetPropertyAsInteger(_("flag")),
										sizeritem->GetPropertyAsInteger(_("border")),
										NULL
										);
		}
		else
		{
			wxLogError( wxT("The GBSizerItem component's child is not a wxWindow or a wxSizer or a Spacer - this should not be possible!") );
			return NULL;
		}
	}

public:
	wxObject* Create(IObject* obj, wxObject* /*parent*/) override {
		wxGridBagSizer* sizer = new wxGridBagSizer(
			obj->GetPropertyAsInteger(_("vgap")),
			obj->GetPropertyAsInteger(_("hgap")));

		if ( !obj->IsNull( _("empty_cell_size") ) )
		{
			sizer->SetEmptyCellSize( obj->GetPropertyAsSize( _("empty_cell_size") ) );
		}

		return sizer;
	}

	void OnCreated(wxObject* wxobject, wxWindow* /*wxparent*/) override {
		// For storing objects whose position needs to be determined
		std::vector< std::pair< wxObject*, wxGBSizerItem* > > newObjects;
		wxGBPosition lastPosition( 0, 0 );

		// Get sizer
		wxGridBagSizer* sizer = wxDynamicCast( wxobject, wxGridBagSizer );
		if ( NULL == sizer )
		{
			wxLogError( wxT("This should be a wxGridBagSizer!") );
			return;
		}

		// Add the children
		IManager* manager = GetManager();
		size_t count = manager->GetChildCount( wxobject );
		if ( 0 == count )
		{
			// wxGridBagSizer gets upset sometimes without children
			sizer->Add( 0, 0, wxGBPosition( 0, 0 ) );
			return;
		}
		for ( size_t i = 0; i < count; ++i )
		{
			// Should be a GBSizerItem
			wxObject* wxsizerItem = manager->GetChild( wxobject, i );
			IObject* isizerItem = manager->GetIObject( wxsizerItem );

			// Get the location of the item
			wxGBSpan span( isizerItem->GetPropertyAsInteger( _("rowspan") ), isizerItem->GetPropertyAsInteger( _("colspan") ) );

			int column = isizerItem->GetPropertyAsInteger( _("column") );
			if ( column < 0 )
			{
				// Needs to be auto positioned after the other children are added
				wxGBSizerItem* item = GetGBSizerItem( isizerItem, lastPosition, span, manager->GetChild( wxsizerItem, 0 ) );
				if ( item != NULL )
				{
					newObjects.push_back( std::pair< wxObject*, wxGBSizerItem* >( wxsizerItem, item ) );
				}
				continue;
			}

			wxGBPosition position( isizerItem->GetPropertyAsInteger( _("row") ), column );

			// Check for intersection
			if ( sizer->CheckForIntersection( position, span ) )
			{
				continue;
			}

			lastPosition = position;

			// Add the child to the sizer
			wxGBSizerItem* item = GetGBSizerItem( isizerItem, position, span, manager->GetChild( wxsizerItem, 0 ) );
			if ( item != NULL )
			{
				sizer->Add( item );
			}
		}

		std::vector< std::pair< wxObject*, wxGBSizerItem* > >::iterator it;
		for ( it = newObjects.begin(); it != newObjects.end(); ++it )
		{
			wxGBPosition position = it->second->GetPos();
			wxGBSpan span = it->second->GetSpan();
			int column = position.GetCol();
			while ( sizer->CheckForIntersection( position, span ) )
			{
				column++;
				position.SetCol( column );
			}
			it->second->SetPos( position );
			sizer->Add( it->second );
			GetManager()->ModifyProperty( it->first, _("row"), wxString::Format( wxT("%i"), position.GetRow() ), false );
			GetManager()->ModifyProperty( it->first, _("column"), wxString::Format( wxT("%i"), column ), false );
		}

		AddProperties(manager->GetIObject(wxobject), sizer);
	}

	ticpp::Element* ExportToXrc(IObject* obj) override {
		ObjectToXrcFilter xrc(obj, _("wxGridBagSizer"));
		ExportXRCProperties( &xrc, obj );
		return xrc.GetXrcObject();
	}

	ticpp::Element* ImportFromXrc(ticpp::Element* xrcObj) override {
		XrcToXfbFilter filter(xrcObj, _("wxGridBagSizer"));
		ImportXRCProperties( &filter );
		return filter.GetXfbObject();
	}
};

class StdDialogButtonSizerComponent : public ComponentBase
{
private:
	void AddXRCButton( ticpp::Element* sizer, const std::string& id, const std::string& label )
	{
		try
		{
			ticpp::Element button( "object" );
			button.SetAttribute( "class", "button" );

			ticpp::Element flag( "flag" );
			flag.SetText( "wxALIGN_CENTER_HORIZONTAL|wxALL" );
			button.LinkEndChild( &flag );

			ticpp::Element border( "border" );
			border.SetText( "5" );
			button.LinkEndChild( &border );

			ticpp::Element wxbutton( "object" );
			wxbutton.SetAttribute( "class", "wxButton" );
			wxbutton.SetAttribute( "name", id );

			ticpp::Element labelEl( "label" );
			labelEl.SetText( label );
			wxbutton.LinkEndChild( &labelEl );

			button.LinkEndChild( &wxbutton );

			sizer->LinkEndChild( &button );
		}
		catch( ticpp::Exception& ex )
		{
			wxLogError( wxString( ex.m_details.c_str(), wxConvUTF8 ) );
		}
	}

public:
	wxObject* Create(IObject* obj, wxObject* parent) override {
		wxStdDialogButtonSizer* sizer =  new wxStdDialogButtonSizer();

		sizer->SetMinSize( obj->GetPropertyAsSize(_("minimum_size")) );

		if (obj->GetPropertyAsInteger(_("OK")) != 0) {
			sizer->AddButton(new wxButton((wxWindow*)parent, wxID_OK));
		}
		if (obj->GetPropertyAsInteger(_("Yes")) != 0) {
			sizer->AddButton(new wxButton((wxWindow*)parent, wxID_YES));
		}
		if (obj->GetPropertyAsInteger(_("Save")) != 0) {
			sizer->AddButton(new wxButton((wxWindow*)parent, wxID_SAVE));
		}
		if (obj->GetPropertyAsInteger(_("Apply")) != 0) {
			sizer->AddButton(new wxButton((wxWindow*)parent, wxID_APPLY));
		}
		if (obj->GetPropertyAsInteger(_("No")) != 0) {
			sizer->AddButton(new wxButton((wxWindow*)parent, wxID_NO));
		}
		if (obj->GetPropertyAsInteger(_("Cancel")) != 0) {
			sizer->AddButton(new wxButton((wxWindow*)parent, wxID_CANCEL));
		}
		if (obj->GetPropertyAsInteger(_("Help")) != 0) {
			sizer->AddButton(new wxButton((wxWindow*)parent, wxID_HELP));
		}
		if (obj->GetPropertyAsInteger(_("ContextHelp")) != 0) {
			sizer->AddButton(new wxButton((wxWindow*)parent, wxID_CONTEXT_HELP));
		}

		sizer->Realize();
		return sizer;
	}

	ticpp::Element* ExportToXrc(IObject* obj) override {
		ObjectToXrcFilter xrc(obj, _("wxStdDialogButtonSizer"));
		ticpp::Element* sizer = xrc.GetXrcObject();

		if (obj->GetPropertyAsSize(_("minimum_size")) != wxDefaultSize) {
			xrc.AddProperty(_("minimum_size"), _("minsize"), XRC_TYPE_SIZE);
		}
		if (obj->GetPropertyAsInteger(_("OK")) != 0) {
			AddXRCButton(sizer, "wxID_OK", "&OK");
		}
		if (obj->GetPropertyAsInteger(_("Yes")) != 0) {
			AddXRCButton(sizer, "wxID_YES", "&Yes");
		}
		if (obj->GetPropertyAsInteger(_("Save")) != 0) {
			AddXRCButton(sizer, "wxID_SAVE", "&Save");
		}
		if (obj->GetPropertyAsInteger(_("Apply")) != 0) {
			AddXRCButton(sizer, "wxID_APPLY", "&Apply");
		}
		if (obj->GetPropertyAsInteger(_("No")) != 0) {
			AddXRCButton(sizer, "wxID_NO", "&No");
		}
		if (obj->GetPropertyAsInteger(_("Cancel")) != 0) {
			AddXRCButton(sizer, "wxID_CANCEL", "&Cancel");
		}
		if (obj->GetPropertyAsInteger(_("Help")) != 0) {
			AddXRCButton(sizer, "wxID_HELP", "&Help");
		}
		if (obj->GetPropertyAsInteger(_("ContextHelp")) != 0) {
			AddXRCButton(sizer, "wxID_CONTEXT_HELP", "");
		}

		return sizer;
	}

	ticpp::Element* ImportFromXrc( ticpp::Element* xrcObj ) override
	{
		std::map< wxString, wxString > buttons;
		buttons[ _("OK") ] 			= wxT("0");
		buttons[ _("Yes") ] 		= wxT("0");
		buttons[ _("Save") ] 		= wxT("0");
		buttons[ _("Apply") ] 		= wxT("0");
		buttons[ _("No") ] 			= wxT("0");
		buttons[ _("Cancel") ] 		= wxT("0");
		buttons[ _("Help") ] 		= wxT("0");
		buttons[ _("ContextHelp") ] = wxT("0");

		XrcToXfbFilter filter(xrcObj, _("wxStdDialogButtonSizer"));
		filter.AddProperty(_("minsize"), _("minimum_size"), XRC_TYPE_SIZE);

		ticpp::Element* button = xrcObj->FirstChildElement( "object", false );
		for (  ; button != 0; button = button->NextSiblingElement( "object", false ) )
		{
			try
			{
				std::string button_class;
				button->GetAttribute( "class", &button_class );
				if ( std::string("button") != button_class )
				{
					continue;
				}

				ticpp::Element* wxbutton = button->FirstChildElement( "object" );
				std::string wxbutton_class;
				wxbutton->GetAttribute( "class", &wxbutton_class );
				if ( std::string("wxButton") != wxbutton_class )
				{
					continue;
				}

				std::string name;
				wxbutton->GetAttribute( "name", &name );

				if ( name == "wxID_OK" )
				{
					buttons[ _("OK") ] = wxT("1");
				}
				else if ( name == "wxID_YES" )
				{
					buttons[ _("Yes") ] = wxT("1");
				}
				else if ( name == "wxID_SAVE" )
				{
					buttons[ _("Save") ] = wxT("1");
				}
				else if ( name == "wxID_APPLY" )
				{
					buttons[ _("Apply") ] = wxT("1");
				}
				else if ( name == "wxID_NO" )
				{
					buttons[ _("No") ] = wxT("1");
				}
				else if ( name == "wxID_CANCEL" )
				{
					buttons[ _("Cancel") ] = wxT("1");
				}
				else if ( name == "wxID_HELP" )
				{
					buttons[ _("Help") ] = wxT("1");
				}
				else if ( name == "wxID_CONTEXT_HELP" )
				{
					buttons[ _("ContextHelp") ] = wxT("1");
				}
			}
			catch( ticpp::Exception& )
			{
				continue;
			}
		}

		std::map< wxString, wxString >::iterator prop;
		for ( prop = buttons.begin(); prop != buttons.end(); ++prop )
		{
			filter.AddPropertyValue( prop->first, prop->second );
		}

		xrcObj->Clear();

		return filter.GetXfbObject();
	}
};

///////////////////////////////////////////////////////////////////////////////

BEGIN_LIBRARY()
ABSTRACT_COMPONENT("spacer",SpacerComponent)
ABSTRACT_COMPONENT("sizeritem",SizerItemComponent)
ABSTRACT_COMPONENT("gbsizeritem",GBSizerItemComponent)

SIZER_COMPONENT("wxBoxSizer",BoxSizerComponent)
SIZER_COMPONENT("wxWrapSizer",WrapSizerComponent)
SIZER_COMPONENT("wxStaticBoxSizer",StaticBoxSizerComponent)
SIZER_COMPONENT("wxGridSizer",GridSizerComponent)
SIZER_COMPONENT("wxFlexGridSizer",FlexGridSizerComponent)
SIZER_COMPONENT("wxGridBagSizer",GridBagSizerComponent)
SIZER_COMPONENT("wxStdDialogButtonSizer",StdDialogButtonSizerComponent)

// wxBoxSizer
MACRO(wxHORIZONTAL)
MACRO(wxVERTICAL)

// wxWrapSizer
MACRO(wxEXTEND_LAST_ON_EACH_LINE)
MACRO(wxREMOVE_LEADING_SPACES)
MACRO(wxWRAPSIZER_DEFAULT_FLAGS)

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
MACRO(wxRESERVE_SPACE_EVEN_IF_HIDDEN)

SYNONYMOUS(wxGROW, wxEXPAND)
SYNONYMOUS(wxALIGN_CENTRE, wxALIGN_CENTER)
SYNONYMOUS(wxALIGN_CENTRE_HORIZONTAL, wxALIGN_CENTER_HORIZONTAL)
SYNONYMOUS(wxALIGN_CENTRE_VERTICAL, wxALIGN_CENTER_VERTICAL)

END_LIBRARY()

