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
// Lua code generation written by
//   Vratislav Zival
//
///////////////////////////////////////////////////////////////////////////////

/**
@file
@author Michal Bližňák - michal.bliznak@gmail.com
@note
*/

#ifndef RAD_LUAPANEL_LUAPANEL_H
#define RAD_LUAPANEL_LUAPANEL_H

#include <wx/panel.h>

#include "utils/wxfbdefs.h"


class CodeEditor;
class wxFBEvent;
class wxFBEventHandlerEvent;
class wxFBObjectEvent;
class wxFBPropertyEvent;
class wxFindDialogEvent;
class wxStyledTextCtrl;


class LuaPanel : public wxPanel
{
private:
    CodeEditor* m_luaPanel;
    PTCCodeWriter m_luaCW;

    void InitStyledTextCtrl(wxStyledTextCtrl* stc);

public:
    LuaPanel(wxWindow* parent, int id);
    ~LuaPanel() override;

    void OnPropertyModified(wxFBPropertyEvent& event);
    void OnProjectRefresh(wxFBEvent& event);
    void OnCodeGeneration(wxFBEvent& event);
    void OnObjectChange(wxFBObjectEvent& event);
    void OnEventHandlerModified(wxFBEventHandlerEvent& event);

    void OnFind(wxFindDialogEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif  // RAD_LUAPANEL_LUAPANEL_H
