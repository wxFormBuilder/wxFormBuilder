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

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/dialog.h>
#include <wx/scrolwin.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>


class DialogFindComponent : public wxDialog
{
public:
    DialogFindComponent(wxWindow* parent, const wxArrayString &componentsList,
                        wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxSize( 250,400 ), long style = wxDEFAULT_DIALOG_STYLE);

    ~DialogFindComponent();

    wxString GetSelected() {return m_chosenComponent;}

private:
    void OnTextCtrlComponent(wxCommandEvent& event);
    void OnListBoxComponentsDClick(wxCommandEvent& event);
    void OnKeyDownComponents(wxKeyEvent &event);

    wxScrolledWindow* m_scrolledWindow;
    wxTextCtrl* m_textCtrlComponent;
    wxListBox* m_listBoxComponents;

    const wxArrayString& m_componentsList;
    wxArrayString m_componentsFinded;
    wxString m_chosenComponent = wxEmptyString;

    enum
    {
        ID_LIST_BOX = wxID_HIGHEST + 1,
        ID_TEXT_CTRL
    };
    void ListBoxComponentChoose();
};

#endif // DIALOGFINDCOMPONENT_H
