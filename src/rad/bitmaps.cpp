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

#include "utils/xmlutils.h"


std::map<wxString, wxBitmap> AppBitmaps::m_bitmaps;


wxBitmap AppBitmaps::GetBitmap(const wxString& iconname, Size size)
{
    wxBitmap bmp;
    if (auto bitmap = m_bitmaps.find(iconname); bitmap != m_bitmaps.end()) {
        bmp = bitmap->second;
    } else {
        bmp = m_bitmaps["unknown"];
    }
    if (
      size != Size::Default &&
      (bmp.GetWidth() != static_cast<int>(size) || bmp.GetHeight() != static_cast<int>(size))) {
        auto image = bmp.ConvertToImage();
        bmp = wxBitmap(image.Scale(static_cast<int>(size), static_cast<int>(size)));
    }

    return bmp;
}

void AppBitmaps::LoadBitmaps(const wxString& filepath, const wxString& iconpath)
{
    m_bitmaps.insert_or_assign("unknown", wxBitmap(default_xpm));

    auto doc = XMLUtils::LoadXMLFile(filepath, true);
    if (!doc) {
        wxLogError(_("%s: Failed to open file"), filepath);

        return;
    }
    if (doc->Error()) {
        wxLogError(doc->ErrorStr());

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


int AppBitmaps::GetPixelSize(Size size)
{
    return static_cast<int>(size);
}
