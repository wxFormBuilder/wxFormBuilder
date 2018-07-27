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

#include "menubar.h"
#include "wx/debug.h"

Menubar::Menubar() : wxPanel()
{
    m_sizer = NULL;
}

Menubar::Menubar(wxWindow *parent, int id, const wxPoint& pos, const wxSize &size,
            long style, const wxString &name) : wxPanel(parent, id, pos, size, style, name)
{
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    m_sizer = new wxBoxSizer(wxHORIZONTAL);
    m_sizer->Add(new wxStaticText(this, wxID_ANY, wxT(" ")), 0, wxRIGHT | wxLEFT, 0);
    mainSizer->Add(m_sizer, 1, wxTOP | wxBOTTOM, 3);
    SetSizer(mainSizer);
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
}

Menubar::~Menubar()
{
    while (!m_menus.empty())
    {
        wxMenu *menu = m_menus[0];
        delete menu;
        m_menus.erase(m_menus.begin());
    }

    // Delete spawned event handlers
    wxSizerItemList& labels = m_sizer->GetChildren();
    for ( wxSizerItemList::iterator label = labels.begin(); label != labels.end(); ++label )
    {
        wxStaticText* text = dynamic_cast< wxStaticText* >( (*label)->GetWindow() );
        if ( text != 0 )
        {
            if ( text->GetEventHandler() != text )
            {
                text->PopEventHandler( true );
            }
        }
    }
}

void Menubar::AppendMenu(const wxString& name, wxMenu *menu)
{
    wxStaticText *st = new wxStaticText(this, wxID_ANY, name);
    st->PushEventHandler(new MenuEvtHandler(st, menu));
    m_sizer->Add(st, 0, wxALIGN_LEFT | wxRIGHT | wxLEFT, 5);
    Layout();
    m_menus.push_back(menu);
}

wxMenu* Menubar::GetMenu(int i)
{
    wxASSERT(i < int(m_menus.size()));
    return m_menus[i];
}

int Menubar::GetMenuCount()
{
    return (unsigned int)m_menus.size();
}

wxMenu* Menubar::Remove(int /*i*/)
{
    return NULL; //TODO: Implementar Menubar::Remove
}

BEGIN_EVENT_TABLE(MenuEvtHandler, wxEvtHandler)
    EVT_LEFT_DOWN(MenuEvtHandler::OnMouseEvent)
END_EVENT_TABLE()

MenuEvtHandler::MenuEvtHandler(wxStaticText *st, wxMenu *menu)
{
    wxASSERT(menu != NULL && st != NULL);

    m_label = st;
    m_menu = menu;
}

void MenuEvtHandler::OnMouseEvent(wxMouseEvent& )
{
    m_label->PopupMenu(m_menu, 0, m_label->GetSize().y + 3);
}


