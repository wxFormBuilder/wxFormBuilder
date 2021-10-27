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
//
// DialogFindComponent writen by
//   Michał Łukowski - michal.lukowski@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

/**
@file
@author Michał Łukowski - michal.lukowski@gmail.com
@note
*/

#ifndef DIALOGFINDCOMPONENT_H
#define DIALOGFINDCOMPONENT_H

#include <vector>

#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/statline.h>
#include <wx/button.h>


class DialogFindComponent : public wxDialog
{
public:
    DialogFindComponent(wxWindow* parent,
                        wxWindowID id = wxID_ANY, const wxString& title = _("Find Component"),
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxSize( 250,400 ), long style = wxDEFAULT_DIALOG_STYLE);

    const wxString& GetSelected() const { return m_chosenComponent; }

public:
    bool TransferDataToWindow() override;
    bool TransferDataFromWindow() override;
    bool Validate() override;

private:
    void updateEnabledState();

    void OnTextCtrlComponent(wxCommandEvent& event);
    void OnListBoxComponentsDClick(wxCommandEvent& event);
    void OnListBoxComponents(wxCommandEvent& event);

private:
    wxTextCtrl          *m_textCtrlComponent;
    wxListBox           *m_listBoxComponents;
    wxStaticLine        *m_staticline;
    wxButton            *m_buttonCancel;
    wxButton            *m_buttonInsert;

    std::vector<wxString> m_allComponents;
    std::vector<wxString> m_prevComponents;
    wxString m_prevComponent;
    wxString m_chosenComponent;
};

#endif // DIALOGFINDCOMPONENT_H
