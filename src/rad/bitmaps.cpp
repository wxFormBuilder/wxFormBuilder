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

#include "bitmaps.h"
#include "tinyxml.h"
#include "unknown.h"
#include <wx/image.h>
#include "utils/typeconv.h"

static map< wxString, wxBitmap > m_bitmaps;

wxBitmap AppBitmaps::GetBitmap( wxString iconname, unsigned int size )
{
	map< wxString, wxBitmap >::iterator bitmap;
	bitmap = m_bitmaps.find( iconname );
	wxBitmap bmp;
	if ( bitmap != m_bitmaps.end() )
	{
		bmp = m_bitmaps[iconname];
	}
	else
	{
		bmp = m_bitmaps[ wxT("unknown") ];
	}
	if ( size != 0 )
	{
		// rescale it to requested size
		if ( bmp.GetWidth() != (int)size || bmp.GetHeight() != (int)size )
		{
			wxImage image = bmp.ConvertToImage();
			bmp = wxBitmap( image.Scale(size, size) );
		}
	}
	return bmp;
}

void AppBitmaps::LoadBitmaps( wxString filepath, wxString iconpath )
{
	m_bitmaps[ wxT("unknown") ] = wxBitmap( unknown_xpm );

	bool result = false;
	TiXmlDocument doc( _STDSTR( filepath ) );
	result = doc.LoadFile();

	if (result)
	{
		TiXmlElement* root = doc.FirstChildElement("icons");
		if (root)
		{
			TiXmlElement* elem = root->FirstChildElement("icon");
			while (elem)
			{
				wxString name = _WXSTR( elem->Attribute("name") );
				wxString file = _WXSTR( elem->Attribute("file") );
				m_bitmaps[name] = wxBitmap( iconpath + file, wxBITMAP_TYPE_ANY );

				elem = elem->NextSiblingElement("icon");
			}

		}
	}
	else
	{
		wxLogError(wxT("Error loading icons"));
	}
}
