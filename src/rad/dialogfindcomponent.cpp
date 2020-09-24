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
#include <wx/msgdlg.h>


DialogFindComponent::DialogFindComponent(wxWindow* parent, const wxArrayString& componentsList,
                                         wxWindowID id, const wxString& title, const wxPoint& pos,
                                         const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style), m_componentsList{componentsList}
{
    this->SetSizeHints( wxSize( 250,400 ), wxDefaultSize );

    wxBoxSizer* bSizerMain;
    bSizerMain = new wxBoxSizer( wxVERTICAL );

    m_scrolledWindow = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
    m_scrolledWindow->SetScrollRate( 5, 5 );
    wxBoxSizer* bSizer2;
    bSizer2 = new wxBoxSizer( wxVERTICAL );

    m_textCtrlComponent = new wxTextCtrl( m_scrolledWindow, ID_TEXT_CTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    bSizer2->Add( m_textCtrlComponent, 0, wxALL|wxEXPAND, 5 );

    m_listBoxComponents = new wxListBox( m_scrolledWindow, ID_LIST_BOX, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxLB_SINGLE|wxWANTS_CHARS );
    bSizer2->Add( m_listBoxComponents, 1, wxALL|wxEXPAND, 5 );


    m_scrolledWindow->SetSizer( bSizer2 );
    m_scrolledWindow->Layout();
    bSizer2->Fit( m_scrolledWindow );
    bSizerMain->Add( m_scrolledWindow, 1, wxEXPAND | wxALL, 5 );

    m_textCtrlComponent->Bind(wxEVT_TEXT, &DialogFindComponent::OnTextCtrlComponent, this);
    m_textCtrlComponent->Bind(wxEVT_KEY_DOWN, &DialogFindComponent::OnKeyDownComponents, this);
    m_listBoxComponents->Bind(wxEVT_LISTBOX_DCLICK, &DialogFindComponent::OnListBoxComponentsDClick,
                              this);
    m_listBoxComponents->Bind(wxEVT_KEY_DOWN, &DialogFindComponent::OnKeyDownComponents, this);

    if (m_componentsList.Count())
    {
        m_listBoxComponents->InsertItems(m_componentsList, 0);
    }

    this->SetSizer( bSizerMain );
    this->Layout();

    this->Centre( wxBOTH );
}

DialogFindComponent::~DialogFindComponent()
{

}

void DialogFindComponent::OnTextCtrlComponent(wxCommandEvent &event)
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
}

void DialogFindComponent::ListBoxComponentChoose()
{
    if (m_listBoxComponents->GetSelection() != wxNOT_FOUND)
    {
        m_chosenComponent = m_listBoxComponents->GetString(m_listBoxComponents->GetSelection());
    }
}

void DialogFindComponent::OnListBoxComponentsDClick(wxCommandEvent &event)
{
    ListBoxComponentChoose();
    this->Close();
}

void DialogFindComponent::OnKeyDownComponents(wxKeyEvent &event)
{
    if (event.GetKeyCode() == WXK_ESCAPE)
    {
        this->Close();
    }
    else if (event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_SPACE)
    {
        if (event.GetId() == ID_LIST_BOX)
        {
            ListBoxComponentChoose();
            this->Close();
        }
    }
    else
    {
        event.Skip();
    }
}
