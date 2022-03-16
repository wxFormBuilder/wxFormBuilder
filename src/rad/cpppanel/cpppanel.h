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

#ifndef RAD_CPPPANEL_CPPPANEL_H
#define RAD_CPPPANEL_CPPPANEL_H

#include <wx/panel.h>

#include "utils/wxfbdefs.h"


class CodeEditor;
class wxAuiNotebook;
class wxFBEvent;
class wxFBEventHandlerEvent;
class wxFBObjectEvent;
class wxFBPropertyEvent;
class wxFindDialogEvent;
class wxStyledTextCtrl;


class CppPanel : public wxPanel
{
private:
    CodeEditor* m_cppPanel;
    CodeEditor* m_hPanel;
    PTCCodeWriter m_hCW;
    PTCCodeWriter m_cppCW;
    wxAuiNotebook* m_notebook;

    void InitStyledTextCtrl(wxStyledTextCtrl* stc);

public:
    CppPanel(wxWindow* parent, int id);
    ~CppPanel() override;

    void OnPropertyModified(wxFBPropertyEvent& event);
    void OnProjectRefresh(wxFBEvent& event);
    void OnCodeGeneration(wxFBEvent& event);
    void OnObjectChange(wxFBObjectEvent& event);
    void OnEventHandlerModified(wxFBEventHandlerEvent& event);

    void OnFind(wxFindDialogEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif  // RAD_CPPPANEL_CPPPANEL_H
