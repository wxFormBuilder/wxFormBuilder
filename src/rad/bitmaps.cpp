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

#include "bitmaps.h"

#include <wx/intl.h>
#include <wx/log.h>

#include <default.xpm>

#include "utils/wxfbexception.h"
#include "utils/xmlutils.h"


static std::map<wxString, wxBitmap> m_bitmaps;


wxBitmap AppBitmaps::GetBitmap(wxString iconname, unsigned int size)
{
    std::map<wxString, wxBitmap>::iterator bitmap;
    bitmap = m_bitmaps.find(iconname);
    wxBitmap bmp;
    if (bitmap != m_bitmaps.end()) {
        bmp = m_bitmaps[iconname];
    } else {
        bmp = m_bitmaps[wxT("unknown")];
    }
    if (size != 0) {
        // rescale it to requested size
        if (bmp.GetWidth() != (int)size || bmp.GetHeight() != (int)size) {
            wxImage image = bmp.ConvertToImage();
            bmp = wxBitmap(image.Scale(size, size));
        }
    }
    return bmp;
}

void AppBitmaps::LoadBitmaps(wxString filepath, wxString iconpath)
{
    m_bitmaps.insert_or_assign("unknown", wxBitmap(default_xpm));

    std::unique_ptr<tinyxml2::XMLDocument> doc;
    try {
        doc = XMLUtils::LoadXMLFile(filepath, true);
    } catch (wxFBException& ex) {
        wxLogError(ex.what());

        return;
    }
    const auto* root = doc->FirstChildElement("icons");
    if (!root) {
        wxLogError(_("%s: Invalid root node"), filepath);

        return;
    }

    for (const auto* icon = root->FirstChildElement("icon"); icon; icon = icon->NextSiblingElement("icon")) {
        auto name = XMLUtils::StringAttribute(icon, "name");
        if (name.empty()) {
            wxLogError(_("%s: Empty icon name"), filepath);
            continue;
        }
        auto file = XMLUtils::StringAttribute(icon, "file");
        if (file.empty()) {
            wxLogError(_("%s: Empty icon path"), filepath);
            continue;
        }

        m_bitmaps.insert_or_assign(name, wxBitmap(iconpath + file, wxBITMAP_TYPE_ANY));
    }
}
