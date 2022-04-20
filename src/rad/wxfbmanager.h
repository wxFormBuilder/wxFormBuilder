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

#ifndef RAD_WXFBMANAGER_H
#define RAD_WXFBMANAGER_H

#include <component.h>

#include "utils/wxfbdefs.h"


class ObjectBase;
class VisualEditor;


class wxFBManager : public IManager
{
public:
    wxFBManager() : m_visualEdit(nullptr) {}

    void SetVisualEditor(VisualEditor* visualEdit) { m_visualEdit = visualEdit; }

    IObject* GetIObject(wxObject* wxobject) const override;

    std::size_t GetChildCount(wxObject* wxobject) const override;
    wxObject* GetChild(wxObject* wxobject, std::size_t childIndex) const override;

    wxObject* GetParent(wxObject* wxobject) const override;
    IObject* GetIParent(wxObject* wxobject) const override;

    bool SelectObject(wxObject* wxobject) override;
    void ModifyProperty(
      wxObject* wxobject, const wxString& property, const wxString& value, bool allowUndo = true) override;

    wxNoObject* NewNoObject() override;

private:
    VisualEditor* m_visualEdit;
};

#endif  // RAD_WXFBMANAGER_H
