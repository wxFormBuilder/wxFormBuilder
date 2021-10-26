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

#include "dialogfindcomponent.h"
#include <wx/sizer.h>


DialogFindComponent::DialogFindComponent(wxWindow* parent, const wxArrayString& componentsList,
                                         wxWindowID id, const wxString& title, const wxPoint& pos,
                                         const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style), m_componentsList{componentsList}
{
    this->SetSizeHints(wxSize(250,400), wxDefaultSize);

    wxBoxSizer* bSizerMain = new wxBoxSizer(wxVERTICAL);

    m_textCtrlComponent = new wxTextCtrl(this, wxID_ANY);
    bSizerMain->Add(m_textCtrlComponent, 0, wxALL|wxEXPAND, 5);

    m_listBoxComponents = new wxListBox(this, wxID_ANY, wxDefaultPosition,
                                        wxDefaultSize, 0, nullptr, wxLB_SINGLE);
    bSizerMain->Add(m_listBoxComponents, 1, wxALL|wxEXPAND, 5);

    m_staticline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    bSizerMain->Add(m_staticline, 0, wxEXPAND | wxALL, 5);

    wxBoxSizer* bSizerButton = new wxBoxSizer(wxHORIZONTAL);
    bSizerButton->AddStretchSpacer();

    m_buttonInsert = new wxButton(this, wxID_ANY, _("Insert"));
    m_buttonInsert->SetDefault();
    bSizerButton->Add( m_buttonInsert, 0, wxALL, 5 );

    m_buttonCancel = new wxButton(this, wxID_CANCEL);
    bSizerButton->Add( m_buttonCancel, 0, wxALL, 5 );
    
    bSizerMain->Add( bSizerButton, 0, wxEXPAND, 5 );

    m_textCtrlComponent->Bind(wxEVT_TEXT, &DialogFindComponent::OnTextCtrlComponent, this);
    m_listBoxComponents->Bind(wxEVT_LISTBOX_DCLICK, &DialogFindComponent::OnListBoxComponentsDClick,
                              this);
    m_listBoxComponents->Bind(wxEVT_LISTBOX, &DialogFindComponent::OnListBoxComponents,this);

    SetAffirmativeId(m_buttonInsert->GetId());

    this->SetSizer( bSizerMain );
    this->Layout();

    this->Centre( wxBOTH );
}


bool DialogFindComponent::TransferDataToWindow() {
    m_listBoxComponents->Set(m_componentsList);
    m_chosenComponent.clear();
    m_buttonInsert->Enable(false);

    return wxDialog::TransferDataToWindow();
}

bool DialogFindComponent::TransferDataFromWindow() {
    m_chosenComponent = m_listBoxComponents->GetString(m_listBoxComponents->GetSelection());

    return wxDialog::TransferDataFromWindow();
}

bool DialogFindComponent::Validate() {
    if (m_listBoxComponents->GetSelection() == wxNOT_FOUND) {
        return false;
    }

    return wxDialog::Validate();
}


void DialogFindComponent::OnTextCtrlComponent(wxCommandEvent& WXUNUSED(event))
{
    wxString typed = m_textCtrlComponent->GetValue();

    m_componentsFinded.Clear();

    for (const auto& item : m_componentsList)
    {
        auto pos = item.Lower().find(typed.Lower());

        if (pos != wxNOT_FOUND)
        {
            m_componentsFinded.Add(item);
        }
    }

    m_listBoxComponents->Clear();

    if (m_componentsFinded.Count())
    {
        m_listBoxComponents->InsertItems(m_componentsFinded, 0);
    }

    m_buttonInsert->Disable();
}

void DialogFindComponent::OnListBoxComponentsDClick(wxCommandEvent& WXUNUSED(event))
{
    if (Validate() && TransferDataFromWindow()) {
        Close();
    }
}

void DialogFindComponent::OnListBoxComponents(wxCommandEvent& WXUNUSED(event))
{
    m_buttonInsert->Enable();
}
