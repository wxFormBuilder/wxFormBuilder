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

#ifndef RAD_PALETTE_H
#define RAD_PALETTE_H

// Include bitmap.h to fix compile error of auibar.h of wxWidgets 3.0.x
#include <wx/bitmap.h>
#include <wx/aui/auibar.h>
#include <wx/aui/auibook.h>
#include <wx/spinbutt.h>

#include "model/database.h"


class wxFbPalette : public wxPanel
{
private:
    wxAuiNotebook* m_notebook;

    void PopulateToolbar(PObjectPackage pkg, wxAuiToolBar* toolbar);

public:
    wxFbPalette(wxWindow* parent, wxWindowID id);

    void SavePosition();

    /**
     * Crea la paleta, previamente se ha debido configurar el objeto
     * DataObservable.
     */
    void Create();
    void OnButtonClick(wxCommandEvent& event, wxAuiToolBar* parent);
};

#endif  // RAD_PALETTE_H
