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
//   Ryan Mulder - rjmyst3@gmail.com
//
///////////////////////////////////////////////////////////////////////////////
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
