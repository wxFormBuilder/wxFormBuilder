#include "dataobject.h"
#include "model/objectbase.h"
#include "utils/typeconv.h"
#include <ticpp.h>
#include <string>
#include "rad/appdata.h"
#include <wx/utils.h>

#include <ticpp.h>

wxFBDataObject::wxFBDataObject( PObjectBase obj )
:
wxDataObject()
{
	if ( obj )
	{
		// create xml representation of ObjectBase
		ticpp::Element element;
		obj->SerializeObject( &element );

		ticpp::Document doc;
		doc.LinkEndChild( &element );
		m_data = doc.GetAsString();
	}
}

wxFBDataObject::~wxFBDataObject()
{
}

void wxFBDataObject::GetAllFormats( wxDataFormat* formats, Direction dir ) const
{
	switch ( dir )
	{
		case Get:
			formats[0] = wxFBDataObjectFormat;
			formats[1] = wxDF_TEXT;
			break;
		case Set:
			formats[0] = wxFBDataObjectFormat;
			break;
		default:
			break;
	}
}

bool wxFBDataObject::GetDataHere( const wxDataFormat&, void* buf ) const
{
	if ( NULL == buf )
	{
		return false;
	}

	memcpy( (char*)buf, m_data.c_str(), m_data.length() );

	return true;
}

size_t wxFBDataObject::GetDataSize( const wxDataFormat& format ) const
{
	return m_data.length();
}

size_t wxFBDataObject::GetFormatCount( Direction dir ) const
{
	switch ( dir )
	{
		case Get:
			return 2;
		case Set:
			return 1;
		default:
			return 0;
	}
}

wxDataFormat wxFBDataObject::GetPreferredFormat( Direction dir ) const
{
	return wxFBDataObjectFormat;
}

bool wxFBDataObject::SetData( const wxDataFormat& format, size_t len, const void *buf )
{
	if ( format != wxFBDataObjectFormat )
	{
		return false;
	}

	m_data.assign( reinterpret_cast< const char* >( buf ), len );
	return true;
}

PObjectBase wxFBDataObject::GetObj()
{
	if ( m_data.empty() )
	{
		return PObjectBase();
	}

	// Load FILE into TiXmlDocument
	TiXmlDocument doc;
	if ( !doc.LoadFromString( m_data, TIXML_ENCODING_UTF8 ) )
	{
		return PObjectBase();
	}

	// Read ObjectBase from xml
	TiXmlElement* element = doc.RootElement();
	if ( NULL == element )
	{
		return PObjectBase();
	}

	PObjectDatabase db = AppData()->GetObjectDatabase();
	return db->CreateObject( element );
}
