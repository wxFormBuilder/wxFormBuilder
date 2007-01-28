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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   José Antonio Hurtado - joseantonio.hurtado@gmail.com
//   Juan Antonio Ortega  - jortegalalmolda@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENUBAR__
#define __MENUBAR__

#include "wx/wx.h"
#include <vector>

typedef std::vector<wxMenu*> MenuVector;

class Menubar : public wxPanel
{
    public:
        Menubar();
        Menubar(wxWindow *parent, int id, const wxPoint& pos = wxDefaultPosition,
            const wxSize &size = wxDefaultSize,
            long style = 0, const wxString &name = wxT("fbmenubar"));
        ~Menubar();
        void AppendMenu(const wxString& name, wxMenu *menu);
        wxMenu* GetMenu(int i);
        int GetMenuCount();
        wxMenu* Remove(int i);

    private:
        MenuVector m_menus;
        wxBoxSizer *m_sizer;
};

class MenuEvtHandler : public wxEvtHandler
{
    public:
        MenuEvtHandler(wxStaticText *st, wxMenu *menu);
        void OnMouseEvent(wxMouseEvent& event);

        DECLARE_EVENT_TABLE()
    private:
        wxStaticText *m_label;
        wxMenu *m_menu;
};

#endif
