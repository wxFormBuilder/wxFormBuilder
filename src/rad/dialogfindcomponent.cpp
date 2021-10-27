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

#include <algorithm>

#include <wx/sizer.h>

#include "appdata.h"
#include "../model/objectbase.h"
#include "../utils/wxfbdefs.h"


DialogFindComponent::DialogFindComponent(wxWindow* parent,
                                         wxWindowID id, const wxString& title, const wxPoint& pos,
                                         const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
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

    m_buttonInsert = new wxButton(this, wxID_OK, _("Insert"));
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
    m_allComponents.clear();
    for ( unsigned i = 0; i < AppData()->GetPackageCount(); ++i )
    {
        auto pkg = AppData()->GetPackage( i );
		m_allComponents.reserve(m_allComponents.size() + pkg->GetObjectCount());

        for ( unsigned j = 0; j < pkg->GetObjectCount(); ++j)
        {
            PObjectInfo info = pkg->GetObjectInfo( j );

            m_allComponents.emplace_back(info->GetClassName());
        }
    }
    std::sort(m_allComponents.begin(), m_allComponents.end());
    m_prevComponents.clear();
    m_prevComponent.clear();
    m_chosenComponent.clear();

    // The std::vector overload is not available in wxWidgets 3.0
    if (!m_allComponents.empty()) {
        m_listBoxComponents->Set(m_allComponents.size(), &m_allComponents.front());
    } else {
        m_listBoxComponents->Clear();
    }
    // Clear() emits a text change event
    m_textCtrlComponent->ChangeValue(wxEmptyString);
    
    updateEnabledState();

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


void DialogFindComponent::updateEnabledState() {
    const auto enableInsert = (m_listBoxComponents->GetSelection() != wxNOT_FOUND);
    if (m_buttonInsert->IsThisEnabled() != enableInsert) {
        m_buttonInsert->Enable(enableInsert);
    }
}


void DialogFindComponent::OnTextCtrlComponent(wxCommandEvent& WXUNUSED(event))
{
    auto nextComponent = m_textCtrlComponent->GetValue();
    nextComponent.MakeLower();
    const auto& searchComponents = (!m_prevComponent.empty() && nextComponent.Find(m_prevComponent) != wxNOT_FOUND ? m_prevComponents : m_allComponents);

    std::vector<wxString> nextComponents;
    nextComponents.reserve(searchComponents.size());
    for (const auto& component : searchComponents) {
        if (component.Lower().Find(nextComponent) != wxNOT_FOUND) {
            nextComponents.emplace_back(component);
        }
    }

    if (!nextComponents.empty()) {
        m_listBoxComponents->Set(nextComponents.size(), &nextComponents.front());
    } else {
        m_listBoxComponents->Clear();
    }
    m_prevComponents.swap(nextComponents);

    if (m_listBoxComponents->GetCount() == 1) {
        m_listBoxComponents->SetSelection(0);
    }
    updateEnabledState();
}

void DialogFindComponent::OnListBoxComponentsDClick(wxCommandEvent& WXUNUSED(event))
{
    if (Validate() && TransferDataFromWindow()) {
        if (IsModal()) {
            EndModal(GetAffirmativeId());
        } else {
            Show(false);
        }
    }
}

void DialogFindComponent::OnListBoxComponents(wxCommandEvent& WXUNUSED(event))
{
    updateEnabledState();
}
