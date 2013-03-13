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

#include "xrcconv.h"
#include "wx/wx.h"
#include "wx/tokenzr.h"

#include <string>
#include <set>

#include <ticpp.h>

static wxString StringToXrcText( const wxString &str )
{
	wxString result;

	for ( unsigned int i = 0 ; i < str.Length() ; i++ )
	{
		wxChar c = str[i];

		switch ( c )
		{
			case wxChar( '\n' ): result = result + wxT( "\\n" );
				break;

			case wxChar( '\t' ): result = result + wxT( "\\t" );
				break;

			case wxChar( '\r' ): result = result + wxT( "\\r" );
				break;

			case wxChar( '\\' ): result = result + wxT( "\\\\" );
				break;

			case wxChar( '_' ):  result = result + wxT( "__" );
				break;

			case wxChar( '&' ):  result = result + wxT( "_" );
				break;

			default:
				result = result + c;
				break;
		}
	}
	return result;
}

static wxString XrcTextToString( const wxString &str )
{
	wxString result;

	for ( unsigned int i = 0 ; i < str.Length() ; i++ )
	{
		wxChar c = str[i];
		if ( c == wxChar( '\\' ) && i < str.length() - 1 )
		{
			wxChar next = str[i+1];

			switch ( next )
			{
				case wxChar( 'n' ): result = result + wxChar( '\n' ); i++;
					break;

				case wxChar( 't' ): result = result + wxChar( '\t' ); i++;
					break;

				case wxChar( 'r' ): result = result + wxChar( '\r' ); i++;
					break;

				case wxChar( '\\' ): result = result + wxChar( '\\' ); i++;
					break;
			}
		}
		else if ( c == wxChar( '_' ) )
		{
			if ( i < str.Length() - 1 && str[i+1] == wxChar( '_' ) )
			{
				result = result + wxChar( '_' );
				i++;
			}
			else
				result = result + wxChar( '&' );
		}
		else
			result = result + c;
	}

	return result;
}

static wxString ReplaceSynonymous( const wxString &bitlist )
{
	IComponentLibrary* lib = GetComponentLibrary( NULL );
	wxString result, translation;
	wxStringTokenizer tkz( bitlist, wxT( "|" ) );
	while ( tkz.HasMoreTokens() )
	{
		wxString token;
		token = tkz.GetNextToken();
		token.Trim( true );
		token.Trim( false );

		if ( result != wxT( "" ) )
			result = result + wxChar( '|' );

		if ( lib->FindSynonymous( token, translation ) )
			result += translation;
		else
			result += token;

	}
	delete lib;

	return result;
}

ObjectToXrcFilter::ObjectToXrcFilter( 	IObject *obj,
										const wxString &classname,
										const wxString &objname,
										const wxString &base )
{
	m_obj = obj;
	m_xrcObj = new ticpp::Element( "object" );

	m_xrcObj->SetAttribute( "class", classname.mb_str( wxConvUTF8 ) );

	if ( objname != wxT( "" ) )
		m_xrcObj->SetAttribute( "name", objname.mb_str( wxConvUTF8 ) );

	if ( base != wxT( "" ) )
		m_xrcObj->SetAttribute( "base", base.mb_str( wxConvUTF8 ) );
}

ObjectToXrcFilter::~ObjectToXrcFilter()
{
	delete m_xrcObj;
}

void ObjectToXrcFilter::AddProperty( const wxString &objPropName,
                                     const wxString &xrcPropName,
                                     const int &propType )
{
	std::string name( xrcPropName.mb_str( wxConvUTF8 ) );
	ticpp::Element propElement( name );

	switch ( propType )
	{
		case XRC_TYPE_SIZE:
		case XRC_TYPE_POINT:
		case XRC_TYPE_BITLIST:
			LinkText( m_obj->GetPropertyAsString( objPropName ), &propElement );
			break;

		case XRC_TYPE_TEXT:
			// The text must be converted to XRC format
			{
				wxString text = m_obj->GetPropertyAsString( objPropName );
				LinkText( text, &propElement, true );
			}
			break;

		case XRC_TYPE_BOOL:
		case XRC_TYPE_INTEGER:
			LinkInteger( m_obj->GetPropertyAsInteger( objPropName ), &propElement );
			break;

		case XRC_TYPE_FLOAT:
			LinkFloat( m_obj->GetPropertyAsFloat( objPropName ), &propElement );
			break;

		case XRC_TYPE_COLOUR:
			LinkColour( m_obj->GetPropertyAsColour( objPropName ), &propElement );
			break;

		case XRC_TYPE_FONT:
			LinkFont( m_obj->GetPropertyAsFont( objPropName ), &propElement );
			break;

		case XRC_TYPE_STRINGLIST:
			LinkStringList( m_obj->GetPropertyAsArrayString( objPropName ), &propElement );
			break;

		case XRC_TYPE_BITMAP:
			{
				wxString bitmapProp = m_obj->GetPropertyAsString( objPropName );
				if ( bitmapProp.empty() )
				{
					break;
				}

				wxString filename = bitmapProp.AfterFirst( wxT(';') );
				if ( filename.empty() )
				{
					break;
				}

				if ( bitmapProp.size() < ( filename.size() + 2 ) )
				{
					break;
				}

				if ( bitmapProp.StartsWith( _("Load From File") ) || bitmapProp.StartsWith( _("Load From Embedded File") ) )
				{
					LinkText( filename.Trim().Trim(false), &propElement );
				}
				else if( bitmapProp.StartsWith( _("Load From Art Provider") ) )
				{
					propElement.SetAttribute( "stock_id", filename.BeforeFirst( wxT(';') ).Trim().Trim(false).mb_str( wxConvUTF8 ) );
					propElement.SetAttribute( "stock_client", filename.AfterFirst( wxT(';') ).Trim().Trim(false).mb_str( wxConvUTF8 ) );
					
					LinkText( wxT("undefined.png"), &propElement );
				}
			}
			break;
	}
	
	m_xrcObj->LinkEndChild( &propElement );
}

void ObjectToXrcFilter::AddPropertyValue ( 	const wxString &xrcPropName,
											const wxString &xrcPropValue,
											bool xrcFormat )
{
	ticpp::Element propElement( xrcPropName.mb_str( wxConvUTF8 ) );
	LinkText( xrcPropValue, &propElement, xrcFormat );
	m_xrcObj->LinkEndChild( &propElement );
}

void ObjectToXrcFilter::AddPropertyPair ( const wxString& objPropName1, const wxString& objPropName2, const wxString& xrcPropName )
{
	AddPropertyValue( 	xrcPropName,
						wxString::Format(	_( "%d,%d" ),
											m_obj->GetPropertyAsInteger( objPropName1 ),
											m_obj->GetPropertyAsInteger( objPropName2 )
										)
	                );
}

ticpp::Element* ObjectToXrcFilter::GetXrcObject()
{
	return new ticpp::Element( *m_xrcObj );
}

void ObjectToXrcFilter::LinkText( const wxString &text, ticpp::Element *propElement, bool xrcFormat )
{
	wxString value = ( xrcFormat ? StringToXrcText( text ) : text );
	propElement->SetText( value.mb_str( wxConvUTF8 ) );
}

void ObjectToXrcFilter::LinkInteger( const int &integer, ticpp::Element *propElement )
{
	propElement->SetText( integer );
}

void ObjectToXrcFilter::LinkFloat( const double& value, ticpp::Element* propElement )
{
	propElement->SetText( value );
}

void ObjectToXrcFilter::LinkColour( const wxColour &colour, ticpp::Element *propElement )
{
	wxString value = wxString::Format( wxT( "#%02x%02x%02x" ), colour.Red(), colour.Green(), colour.Blue() );
	propElement->SetText( value.mb_str( wxConvUTF8 ) );
}

void ObjectToXrcFilter::LinkFont( const wxFontContainer &font, ticpp::Element *propElement )
{
	if ( font.GetPointSize() > 0 )
	{
		wxString aux;
		aux.Printf( wxT( "%d" ), font.GetPointSize() );

		ticpp::Element size( "size" );
		size.SetText( aux.mb_str( wxConvUTF8 ) );
		propElement->LinkEndChild( &size );
	}

	bool skipFamily = false;
	ticpp::Element family( "family" );
	switch ( font.GetFamily() )
	{
		case wxFONTFAMILY_DECORATIVE:
			family.SetText( "decorative" );
			break;
		case wxFONTFAMILY_ROMAN:
			family.SetText( "roman" );
			break;
		case wxFONTFAMILY_SWISS:
			family.SetText( "swiss" );
			break;
		case wxFONTFAMILY_SCRIPT:
			family.SetText( "script" );
			break;
		case wxFONTFAMILY_MODERN:
			family.SetText( "modern" );
			break;
		case wxFONTFAMILY_TELETYPE:
			family.SetText( "teletype" );
			break;
		default:
		// wxWidgets 2.9.0 doesn't define "default" family
#if wxVERSION_NUMBER < 2900
			family.SetText( "default" );
#else
			skipFamily = true;
#endif
			break;
	}
	if( ! skipFamily ) propElement->LinkEndChild( &family );

	ticpp::Element style( "style" );
	switch ( font.GetStyle() )
	{
		case wxFONTSTYLE_SLANT:
			style.SetText( "slant" );
			break;
		case wxFONTSTYLE_ITALIC:
			style.SetText( "italic" );
			break;
		default:
			style.SetText( "normal" );
			break;
	}
	propElement->LinkEndChild( &style );

	ticpp::Element weight( "weight" );
	switch ( font.GetWeight() )
	{
		case wxFONTWEIGHT_LIGHT:
			weight.SetText( "light" );
			break;
		case wxFONTWEIGHT_BOLD:
			weight.SetText( "bold" );
			break;
		default:
			weight.SetText( "normal" );
			break;
	}
	propElement->LinkEndChild( &weight );

	ticpp::Element underlined( "underlined" );
	underlined.SetText( font.GetUnderlined() ? "1" : "0" );
	propElement->LinkEndChild( &underlined );

	if ( !font.GetFaceName().empty() )
	{
		ticpp::Element face( "face" );
		face.SetText( font.GetFaceName().mb_str( wxConvUTF8 ) );
		propElement->LinkEndChild( &face );
	}
}

void ObjectToXrcFilter::LinkStringList( const wxArrayString &array, ticpp::Element *propElement, bool xrcFormat )
{
	for ( size_t i = 0; i < array.GetCount(); i++ )
	{
		wxString value = ( xrcFormat ? StringToXrcText( array[i] ) : array[i] );
		ticpp::Element item( "item" );
		item.SetText( value.mb_str( wxConvUTF8 ) );
		propElement->LinkEndChild( &item );
	}
}

void ObjectToXrcFilter::AddWindowProperties()
{
	wxString style;
	if ( !m_obj->IsNull( _( "style" ) ) )
		style = m_obj->GetPropertyAsString( _T( "style" ) );
	if ( !m_obj->IsNull( _( "window_style" ) ) ){
		if ( !style.IsEmpty() ) style += _T( '|' );
		style += m_obj->GetPropertyAsString( _T( "window_style" ) );
	}
	if ( !style.IsEmpty() ) AddPropertyValue( _T( "style" ), style );

	wxString extraStyle;
	if ( !m_obj->IsNull( _( "extra_style" ) ) )
		extraStyle = m_obj->GetPropertyAsString( _T( "extra_style" ) );
	if ( !m_obj->IsNull( _( "window_extra_style" ) ) ){
		if ( !extraStyle.IsEmpty() ) extraStyle += _T( '|' );
		extraStyle += m_obj->GetPropertyAsString( _T( "window_extra_style" ) );
	}
	if ( !extraStyle.IsEmpty() ) AddPropertyValue( _T( "exstyle" ), extraStyle );

	if ( !m_obj->IsNull( _( "pos" ) ) )
		AddProperty( _( "pos" ), _( "pos" ), XRC_TYPE_SIZE );

	if ( !m_obj->IsNull( _( "size" ) ) )
		AddProperty( _( "size" ), _( "size" ), XRC_TYPE_SIZE );

	if ( !m_obj->IsNull( _( "bg" ) ) )
		AddProperty( _( "bg" ), _( "bg" ), XRC_TYPE_COLOUR );

	if ( !m_obj->IsNull( _( "fg" ) ) )
		AddProperty( _( "fg" ), _( "fg" ), XRC_TYPE_COLOUR );

	if ( !m_obj->IsNull( _( "enabled" ) ) && !m_obj->GetPropertyAsInteger( _( "enabled" ) ) )
		AddProperty( _( "enabled" ), _( "enabled" ), XRC_TYPE_BOOL );

	if ( !m_obj->IsNull( _( "focused" ) ) )
		AddPropertyValue( _( "focused" ), _( "0" ) );

	if ( !m_obj->IsNull( _( "hidden" ) ) && m_obj->GetPropertyAsInteger( _( "hidden" ) ) )
		AddProperty( _( "hidden" ), _( "hidden" ), XRC_TYPE_BOOL );

	if ( !m_obj->IsNull( _( "font" ) ) )
		AddProperty( _( "font" ), _( "font" ), XRC_TYPE_FONT );

	if ( !m_obj->IsNull( _( "tooltip" ) ) )
		AddProperty( _( "tooltip" ), wxT( "tooltip" ), XRC_TYPE_TEXT );

	if ( !m_obj->IsNull( _( "subclass" ) ) )
	{
		wxString subclass = m_obj->GetChildFromParentProperty( _( "subclass" ), wxT( "name" ) );
		if ( !subclass.empty() )
		{
			m_xrcObj->SetAttribute( "subclass", subclass.mb_str( wxConvUTF8 ) );
		}
	}
};

///////////////////////////////////////////////////////////////////////////////

XrcToXfbFilter::XrcToXfbFilter( ticpp::Element *obj,
								const wxString &/*classname*/,
                                const wxString &objname )
{
	m_xrcObj = obj;
	m_xfbObj = new ticpp::Element( "object" );

	try
	{
		std::string name;
		obj->GetAttribute( "class", &name );
		m_xfbObj->SetAttribute( "class", name );
	}
	catch( ticpp::Exception& ex )
	{
		wxLogDebug( wxString( ex.m_details.c_str(), wxConvUTF8 ) );
	}

	if ( !objname.empty() )
		AddProperty( wxT( "name" ), objname, XRC_TYPE_TEXT );
}

XrcToXfbFilter::XrcToXfbFilter( ticpp::Element *obj, const wxString &classname )
{
	m_xrcObj = obj;
	m_xfbObj = new ticpp::Element( "object" );

	m_xfbObj->SetAttribute( "class", classname.mb_str( wxConvUTF8 ) );

	try
	{
		std::string name;
		obj->GetAttribute( "name", &name );
		wxString objname( name.c_str(), wxConvUTF8 );
		AddPropertyValue( wxT( "name" ), objname );
	}
	catch( ticpp::Exception& ex )
	{
		wxLogDebug( wxString( ex.m_details.c_str(), wxConvUTF8 ) );
	}
}

XrcToXfbFilter::~XrcToXfbFilter()
{
	delete m_xfbObj;
}


ticpp::Element* XrcToXfbFilter::GetXrcProperty( const wxString &name )
{
	return m_xrcObj->FirstChildElement( name.mb_str( wxConvUTF8 ) );
}

void XrcToXfbFilter::AddProperty( const wxString &xrcPropName,
                                  const wxString &xfbPropName,
                                  const int &propType )
{
	ticpp::Element propElement( "property" );
	propElement.SetAttribute( "name", xfbPropName.mb_str( wxConvUTF8 ) );

	switch ( propType )
	{
		case XRC_TYPE_SIZE:
		case XRC_TYPE_POINT:
		case XRC_TYPE_BOOL:
			ImportTextProperty( xrcPropName, &propElement );
			break;

		case XRC_TYPE_TEXT:
			ImportTextProperty( xrcPropName, &propElement, true );
			break;

		case XRC_TYPE_INTEGER:
			ImportIntegerProperty( xrcPropName, &propElement );
			break;

		case XRC_TYPE_FLOAT:
			ImportFloatProperty( xrcPropName, &propElement );
			break;

		case XRC_TYPE_BITLIST:
			ImportBitlistProperty( xrcPropName, &propElement );
			break;

		case XRC_TYPE_COLOUR:
			ImportColourProperty( xrcPropName, &propElement );
			break;

		case XRC_TYPE_FONT:
			ImportFontProperty( xrcPropName, &propElement );
			break;

		case XRC_TYPE_STRINGLIST:
			ImportStringListProperty( xrcPropName, &propElement, true );
			break;

		case XRC_TYPE_BITMAP:
			ImportBitmapProperty( xrcPropName, &propElement );
			break;

	}

	m_xfbObj->LinkEndChild( &propElement );
}

void XrcToXfbFilter::AddPropertyValue ( const wxString &xfbPropName,
                                        const wxString &xfbPropValue,
                                        bool parseXrcText )
{
	ticpp::Element propElement( "property" );
	propElement.SetAttribute( "name", xfbPropName.mb_str( wxConvUTF8 ) );
	wxString value = ( parseXrcText ? XrcTextToString( xfbPropValue ) : xfbPropValue );
	propElement.SetText( value.mb_str( wxConvUTF8 ) );
	m_xfbObj->LinkEndChild( &propElement );
}

void XrcToXfbFilter::AddStyleProperty()
{
	try
	{
		ticpp::Element *xrcProperty = m_xrcObj->FirstChildElement( "style" );

		wxString bitlist = wxString( xrcProperty->GetText().c_str(), wxConvUTF8 );
		bitlist = ReplaceSynonymous( bitlist );

		// FIXME: We should avoid hardcoding these things
		std::set< wxString > windowStyles;
		windowStyles.insert( wxT( "wxSIMPLE_BORDER" ) );
		windowStyles.insert( wxT( "wxDOUBLE_BORDER" ) );
		windowStyles.insert( wxT( "wxSUNKEN_BORDER" ) );
		windowStyles.insert( wxT( "wxRAISED_BORDER" ) );
		windowStyles.insert( wxT( "wxSTATIC_BORDER" ) );
		windowStyles.insert( wxT( "wxNO_BORDER" ) );
		windowStyles.insert( wxT( "wxTRANSPARENT_WINDOW" ) );
		windowStyles.insert( wxT( "wxTAB_TRAVERSAL" ) );
		windowStyles.insert( wxT( "wxWANTS_CHARS" ) );
		windowStyles.insert( wxT( "wxVSCROLL" ) );
		windowStyles.insert( wxT( "wxHSCROLL" ) );
		windowStyles.insert( wxT( "wxALWAYS_SHOW_SB" ) );
		windowStyles.insert( wxT( "wxCLIP_CHILDREN" ) );
		windowStyles.insert( wxT( "wxFULL_REPAINT_ON_RESIZE" ) );

		wxString style, windowStyle;
		wxStringTokenizer tkz( bitlist, wxT( " |" ) );
		while ( tkz.HasMoreTokens() )
		{
			wxString token;
			token = tkz.GetNextToken();
			token.Trim( true );
			token.Trim( false );

			if ( windowStyles.find( token ) == windowStyles.end() )
			{
				if ( !style.IsEmpty() ) style += _T( "|" );
				style += token;
			}
			else
			{
				if ( !windowStyle.IsEmpty() ) windowStyle += _T( "|" );
				windowStyle += token;
			}

		}

		if ( !style.empty() )
		{
			AddPropertyValue( wxT( "style" ), style );
		}

		AddPropertyValue( wxT( "window_style" ), windowStyle );
	}
	catch( ticpp::Exception& ex )
	{
		wxLogDebug( wxString( ex.m_details.c_str(), wxConvUTF8 ) );
	}
}

void XrcToXfbFilter::AddExtraStyleProperty()
{
	try
	{
		ticpp::Element *xrcProperty = m_xrcObj->FirstChildElement( "exstyle" );

		wxString bitlist = wxString( xrcProperty->GetText().c_str(), wxConvUTF8 );
		bitlist = ReplaceSynonymous( bitlist );

		// FIXME: We should avoid hardcoding these things
		std::set< wxString > windowStyles;
		windowStyles.insert( wxT( "wxWS_EX_VALIDATE_RECURSIVELY" ) );
		windowStyles.insert( wxT( "wxWS_EX_BLOCK_EVENTS" ) );
		windowStyles.insert( wxT( "wxWS_EX_TRANSIENT" ) );
		windowStyles.insert( wxT( "wxWS_EX_PROCESS_IDLE" ) );
		windowStyles.insert( wxT( "wxWS_EX_PROCESS_UI_UPDATES" ) );

		wxString style, windowStyle;
		wxStringTokenizer tkz( bitlist, wxT( " |" ) );
		while ( tkz.HasMoreTokens() )
		{
			wxString token;
			token = tkz.GetNextToken();
			token.Trim( true );
			token.Trim( false );

			if ( windowStyles.find( token ) == windowStyles.end() )
			{
				if ( !style.IsEmpty() ) style += _T( "|" );
				style += token;
			}
			else
			{
				if ( !windowStyle.IsEmpty() ) windowStyle += _T( "|" );
				windowStyle += token;
			}

		}

		if ( !style.empty() )
		{
			AddPropertyValue( wxT( "extra_style" ), style );
		}
		AddPropertyValue( wxT( "window_extra_style" ), windowStyle );
	}
	catch( ticpp::Exception& ex )
	{
		wxLogDebug( wxString( ex.m_details.c_str(), wxConvUTF8 ) );
	}
}

void XrcToXfbFilter::AddPropertyPair( const char* xrcPropName, const wxString& xfbPropName1, const wxString& xfbPropName2 )
{
	try
	{
		ticpp::Element* pairProp = m_xrcObj->FirstChildElement( xrcPropName );

		wxString width = wxEmptyString;
		wxString height = wxEmptyString;
		wxStringTokenizer tkz( wxString( pairProp->GetText().c_str(), wxConvUTF8 ), wxT( "," ) );
		if ( tkz.HasMoreTokens() )
		{
			width = tkz.GetNextToken();
			if ( tkz.HasMoreTokens() )
			{
				height = tkz.GetNextToken();
			}
		}
		AddPropertyValue( xfbPropName1, width );
		AddPropertyValue( xfbPropName2, height );
	}
	catch( ticpp::Exception& ex )
	{
		wxLogDebug( wxString( ex.m_details.c_str(), wxConvUTF8 ) );
	}
}

ticpp::Element* XrcToXfbFilter::GetXfbObject()
{
	return m_xfbObj->Clone().release()->ToElement();
}

//-----------------------------

void XrcToXfbFilter::ImportTextProperty( 	const wxString &xrcPropName,
											ticpp::Element *property,
											bool parseXrcText )
{
	try
	{
		ticpp::Element *xrcProperty = m_xrcObj->FirstChildElement( xrcPropName.mb_str( wxConvUTF8 ) );

		// Convert XRC text to normal text
		wxString value( wxString( xrcProperty->GetText().c_str(), wxConvUTF8 ) );

		if ( parseXrcText )
			value = XrcTextToString( value );

		property->SetText( value.mb_str( wxConvUTF8 ) );
	}
	catch( ticpp::Exception& ex )
	{
		wxLogDebug( wxString( ex.m_details.c_str(), wxConvUTF8 ) );
	}
}

void XrcToXfbFilter::ImportIntegerProperty( const wxString &xrcPropName, ticpp::Element *property )
{
	try
	{
		ticpp::Element *xrcProperty = m_xrcObj->FirstChildElement( xrcPropName.mb_str( wxConvUTF8 ) );
		property->SetText( xrcProperty->GetText() );
	}
	catch( ticpp::Exception& )
	{
		property->SetText( "0" );
	}
}

void XrcToXfbFilter::ImportFloatProperty( const wxString &xrcPropName, ticpp::Element *property )
{
	try
	{
		ticpp::Element *xrcProperty = m_xrcObj->FirstChildElement( xrcPropName.mb_str( wxConvUTF8 ) );
		property->SetText( xrcProperty->GetText() );
	}
	catch( ticpp::Exception& )
	{
		property->SetText( "0.0" );
	}
}

void XrcToXfbFilter::ImportBitlistProperty( const wxString &xrcPropName, ticpp::Element *property )
{
	try
	{
		ticpp::Element *xrcProperty = m_xrcObj->FirstChildElement( xrcPropName.mb_str( wxConvUTF8 ) );

		wxString bitlist = wxString( xrcProperty->GetText().c_str(), wxConvUTF8 );
		bitlist = ReplaceSynonymous( bitlist );
		property->SetText( bitlist.mb_str( wxConvUTF8 ) );
	}
	catch( ticpp::Exception& ex )
	{
		wxLogDebug( wxString( ex.m_details.c_str(), wxConvUTF8 ) );
	}
}

void XrcToXfbFilter::ImportFontProperty( const wxString &xrcPropName, ticpp::Element *property )
{
	try
	{
		ticpp::Element *xrcProperty = m_xrcObj->FirstChildElement( xrcPropName.mb_str( wxConvUTF8 ) );

		ticpp::Element *element;
		wxFontContainer font;

		// the size
		try
		{
			element = xrcProperty->FirstChildElement( "size" );
			long size;
			element->GetText( &size );
			font.SetPointSize( size );
		}
		catch( ticpp::Exception& ex )
		{
			wxLogDebug( wxString( ex.m_details.c_str(), wxConvUTF8 ) );
		}

		// the family
		try
		{
			element = xrcProperty->FirstChildElement( "family" );
			wxString family_str( element->GetText().c_str(), wxConvUTF8 );

			if ( family_str == _( "decorative" ) )
				font.SetFamily( wxDECORATIVE );
			else if ( family_str == _( "roman" ) )
				font.SetFamily( wxROMAN );
			else if ( family_str == _( "swiss" ) )
				font.SetFamily( wxSWISS );
			else if ( family_str == _( "script" ) )
				font.SetFamily( wxSCRIPT );
			else if ( family_str == _( "modern" ) )
				font.SetFamily( wxMODERN );
			else if ( family_str == _( "teletype" ) )
				font.SetFamily( wxTELETYPE );
			else
				font.SetFamily( wxDEFAULT );
		}
		catch( ticpp::Exception& )
		{
			font.SetFamily( wxDEFAULT );
		}

		// the style
		try
		{
			element = xrcProperty->FirstChildElement( "style" );
			wxString style_str( element->GetText().c_str(), wxConvUTF8 );

			if ( style_str == _( "slant" ) )
				font.SetStyle( wxSLANT );
			else if ( style_str == _( "italic" ) )
				font.SetStyle( wxITALIC );
			else
				font.SetStyle( wxNORMAL );
		}
		catch( ticpp::Exception& )
		{
			font.SetStyle( wxNORMAL );
		}


		// weight
		try
		{
			element = xrcProperty->FirstChildElement( "weight" );
			wxString weight_str( element->GetText().c_str(), wxConvUTF8 );

			if ( weight_str == _( "light" ) )
				font.SetWeight( wxLIGHT );
			else if ( weight_str == _( "bold" ) )
				font.SetWeight( wxBOLD );
			else
				font.SetWeight( wxNORMAL );
		}
		catch( ticpp::Exception& )
		{
			font.SetWeight( wxNORMAL );
		}

		// underlined
		try
		{
			element = xrcProperty->FirstChildElement( "underlined" );
			wxString underlined_str( element->GetText().c_str(), wxConvUTF8 );

			if ( underlined_str == wxT( "1" ) )
				font.SetUnderlined( true );
			else
				font.SetUnderlined( false );
		}
		catch( ticpp::Exception& )
		{
			font.SetUnderlined( false );
		}

		// face
		try
		{
			element = xrcProperty->FirstChildElement( "face" );
			wxString face( element->GetText().c_str(), wxConvUTF8 );
			font.SetFaceName( face );
		}
		catch( ticpp::Exception& )
		{
			font.SetFaceName( wxEmptyString );
		}

		// We already have the font type. So we must now use the wxFB format
		wxString font_str =
			wxString::Format( wxT("%s,%d,%d,%d,%d,%d"), font.GetFaceName().c_str(), font.GetStyle(),
														font.GetWeight(), font.GetPointSize(),
														font.GetFamily(), font.GetUnderlined() );
		property->SetText( font_str.mb_str( wxConvUTF8 ) );
	}
	catch( ticpp::Exception& ex )
	{
		wxLogDebug( wxString( ex.m_details.c_str(), wxConvUTF8 ) );
	}
}

void XrcToXfbFilter::ImportBitmapProperty( const wxString &xrcPropName, ticpp::Element *property )
{
	try
	{
		ticpp::Element *xrcProperty = m_xrcObj->FirstChildElement( xrcPropName.mb_str( wxConvUTF8 ) );
		
		if( (xrcProperty->GetAttribute( "stock_id" ) != "") && (xrcProperty->GetAttribute( "stock_client" ) != "") )
		{
			// read wxArtProvider-based bitmap  			
			wxString res = _("Load From Art Provider");
			res += wxT(";");
			res += wxString( xrcProperty->GetAttribute( "stock_id" ).c_str(), wxConvUTF8 );
			res += wxT(";");
			res += wxString( xrcProperty->GetAttribute( "stock_client" ).c_str(), wxConvUTF8 );
			property->SetText( res.Trim().mb_str( wxConvUTF8 ) );
		}
		else
		{
			// read file-based bitmap
			wxString res = _("Load From File");
			res += wxT(";");
			res += wxString( xrcProperty->GetText().c_str(), wxConvUTF8 );
			property->SetText( res.Trim().mb_str( wxConvUTF8 ) );
		}
	}
	catch( ticpp::Exception& ex )
	{
		wxLogDebug( wxString( ex.m_details.c_str(), wxConvUTF8 ) );
	}
}

void XrcToXfbFilter::ImportColourProperty( const wxString &xrcPropName, ticpp::Element *property )
{
	try
	{
		ticpp::Element *xrcProperty = m_xrcObj->FirstChildElement( xrcPropName.mb_str( wxConvUTF8 ) );
		std::string value = xrcProperty->GetText();

		// Changing "#rrggbb" format to "rrr,ggg,bbb"
		std::string hexColour = "0x" + value.substr( 1, 2 ) +
								" 0x" + value.substr( 3, 2 ) +
		                        " 0x" + value.substr( 5, 2 );

		std::istringstream strIn;
		std::ostringstream strOut;
		unsigned int red, green, blue;

		strIn.str( hexColour );
		strIn >> std::hex;

		strIn >> red;
		strIn >> green;
		strIn >> blue;

		strOut << red << "," << green << "," << blue;

		property->SetText( strOut.str() );
	}
	catch( ticpp::Exception& ex )
	{
		wxLogDebug( wxString( ex.m_details.c_str(), wxConvUTF8 ) );
	}
}

void XrcToXfbFilter::ImportStringListProperty( const wxString &xrcPropName, ticpp::Element *property , bool parseXrcText )
{
	try
	{
		ticpp::Element *xrcProperty = m_xrcObj->FirstChildElement( xrcPropName.mb_str( wxConvUTF8 ) );

		wxString res;

		ticpp::Element *element = xrcProperty->FirstChildElement( "item", false );
		while ( element )
		{
			try
			{
				wxString value( element->GetText().c_str(), wxConvUTF8 );
				if ( parseXrcText )
					value = XrcTextToString( value );

				res += wxChar( '\"' ) + value + wxT( "\" " );
			}
			catch( ticpp::Exception& ex )
			{
				wxLogDebug( wxT("%s. line: %i"), wxString( ex.m_details.c_str(), wxConvUTF8 ).c_str(), __LINE__ );
			}

			element = element->NextSiblingElement( "item", false );
		}

		res.Trim();
		property->SetText( res.mb_str( wxConvUTF8 ) );
	}
	catch( ticpp::Exception& ex )
	{
		wxLogDebug( wxT("%s. line: %i"), wxString( ex.m_details.c_str(), wxConvUTF8 ).c_str(), __LINE__ );
	}
}

void XrcToXfbFilter::AddWindowProperties()
{
	AddProperty( _( "pos" ), _( "pos" ), XRC_TYPE_POINT );
	AddProperty( _( "size" ), _( "size" ), XRC_TYPE_SIZE );
	AddProperty( _( "bg" ), _( "bg" ), XRC_TYPE_COLOUR );
	AddProperty( _( "fg" ), _( "fg" ), XRC_TYPE_COLOUR );
	AddProperty( _( "font" ), _( "font" ), XRC_TYPE_FONT );
	if ( m_xrcObj->FirstChildElement( "enabled", false ) )
	{
		AddProperty( _( "enabled"), _("enabled"), XRC_TYPE_BOOL );
	}
	AddProperty( _( "hidden"), _("hidden"), XRC_TYPE_BOOL );
	AddProperty( _( "tooltip"), _("tooltip"), XRC_TYPE_TEXT );
	AddStyleProperty();
	AddExtraStyleProperty();

	// Subclass
	std::string subclass;
	m_xrcObj->GetAttribute( "subclass", &subclass, false );
	if ( !subclass.empty() )
	{
		ticpp::Element propElement( "property" );
		propElement.SetAttribute( "name", "subclass" );
		propElement.SetText( subclass );
		m_xfbObj->LinkEndChild( &propElement );
	}
};
