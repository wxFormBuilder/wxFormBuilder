#ifndef __DATA_OBJECT__
#define __DATA_OBJECT__

#include <string>
#include <wx/dataobj.h>
#include <wx/dnd.h>
#include "utils/wxfbdefs.h"

#define wxFBDataObjectFormat wxDataFormat( wxT("wxFormBuilderDataFormat") )

class wxFBDataObject : public wxDataObject
{
private:
	std::string m_data;

public:
	wxFBDataObject( PObjectBase obj = PObjectBase() );
	~wxFBDataObject();
	void GetAllFormats( wxDataFormat* formats, Direction dir = Get ) const;
	bool GetDataHere( const wxDataFormat& format, void* buf ) const;
	size_t GetDataSize(const wxDataFormat& format ) const;
	size_t GetFormatCount( Direction dir = Get ) const;
	wxDataFormat GetPreferredFormat( Direction dir = Get ) const;
	bool SetData( const wxDataFormat& format, size_t len, const void *buf );
	PObjectBase GetObj();
};

#endif //__DATA_OBJECT__
