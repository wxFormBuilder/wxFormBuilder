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
//   Ryan Mulder - rjmyst3@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

#include "dataobject.h"

#include <cstring>

#include <tinyxml2.h>

#include "model/objectbase.h"
#include "rad/appdata.h"
#include "utils/wxfbexception.h"


wxDataFormat wxFBDataObject::DataObjectFormat()
{
    static const auto format = wxDataFormat("wxFormBuilderDataFormat");

    return format;
}


wxFBDataObject::wxFBDataObject(PObjectBase object)
{
    if (!object) {
        return;
    }

    tinyxml2::XMLDocument doc(false, tinyxml2::PRESERVE_WHITESPACE);
    auto* root = doc.NewElement("");
    doc.InsertEndChild(root);

    object->Serialize(root);
    root->SetAttribute("fbp_version_major", static_cast<int>(AppData()->m_fbpVerMajor));
    root->SetAttribute("fbp_version_minor", static_cast<int>(AppData()->m_fbpVerMinor));

    tinyxml2::XMLPrinter printer(nullptr, true, 0);
    doc.Print(&printer);
    m_data = printer.CStr();
    wxLogDebug("wxFBDataObject::wxFBDataObject() %s", m_data.c_str());
}


PObjectBase wxFBDataObject::GetObject() const
{
    if (m_data.empty()) {
        return PObjectBase();
    }
    wxLogDebug("wxFBDataObject::GetObj(): %s", m_data.c_str());

    tinyxml2::XMLDocument doc(false, tinyxml2::PRESERVE_WHITESPACE);
    if (doc.Parse(m_data.data(), m_data.size()) != tinyxml2::XML_SUCCESS) {
        wxLogError(_("Failed to parse wxFormBuilderDataFormat: %s"), doc.ErrorStr());

        return PObjectBase();
    }
    auto* root = doc.FirstChildElement();
    if (!root) {
        wxLogError(_("wxFormBuilderDataFormat: Missing root node"));

        return PObjectBase();
    }

    auto versionMajor = root->IntAttribute("fbp_version_major", -1);
    auto versionMinor = root->IntAttribute("fbp_version_minor", -1);
    if (versionMajor < 0 || versionMinor < 0) {
        wxLogError(_("wxFormBuilderDataFormat: Invalid version %i.%i"), versionMajor, versionMinor);

        return PObjectBase();
    }

    if (
        versionMajor > AppData()->m_fbpVerMajor ||
        (versionMajor == AppData()->m_fbpVerMajor && versionMinor > AppData()->m_fbpVerMinor)
    ) {
        wxLogError(_("wxFormBuilderDataFormat: This object cannot be pasted because it is from a newer version of wxFormBuilder"));

        return PObjectBase();
    }
    if (
        versionMajor < AppData()->m_fbpVerMajor ||
        (versionMajor == AppData()->m_fbpVerMajor && versionMinor < AppData()->m_fbpVerMinor)
    ) {
        AppData()->ConvertObject(root, versionMajor, versionMinor);
    }

    return AppData()->GetObjectDatabase()->CreateObject(root);
}


void wxFBDataObject::GetAllFormats(wxDataFormat* formats, Direction dir) const
{
    switch (dir) {
        case Get:
            formats[0] = DataObjectFormat();
            formats[1] = wxDF_TEXT;
            break;
        case Set:
            formats[0] = DataObjectFormat();
            break;
        default:
            break;
    }
}

bool wxFBDataObject::GetDataHere(const wxDataFormat&, void* buf) const
{
    if (!buf) {
        return false;
    }

    std::memcpy(buf, m_data.data(), m_data.size());

    return true;
}

size_t wxFBDataObject::GetDataSize([[maybe_unused]] const wxDataFormat& format) const
{
    return m_data.size();
}

size_t wxFBDataObject::GetFormatCount(Direction dir) const
{
    switch (dir) {
        case Get:
            return 2;
        case Set:
            return 1;
        default:
            return 0;
    }
}

wxDataFormat wxFBDataObject::GetPreferredFormat([[maybe_unused]] Direction dir) const
{
    return DataObjectFormat();
}

bool wxFBDataObject::SetData(const wxDataFormat& format, size_t len, const void* buf)
{
    if (format != DataObjectFormat()) {
        return false;
    }

    m_data.assign(reinterpret_cast<const char*>(buf), len);

    return true;
}
