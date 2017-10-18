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
// Lua code generation writen by
//   Vratislav Zival
//
///////////////////////////////////////////////////////////////////////////////

/**
@file
@author Michal Bližňák - michal.bliznak@gmail.com
@note
*/

#ifndef __LUA_PANEL__
#define __LUA_PANEL__

#include <wx/panel.h>

#include "utils/wxfbdefs.h"

class CodeEditor;

#if wxVERSION_NUMBER < 2900
    class wxScintilla;
#else
    class wxStyledTextCtrl;
#endif

class wxFindDialogEvent;

class wxFBEvent;
class wxFBPropertyEvent;
class wxFBObjectEvent;
class wxFBEventHandlerEvent;

class LuaPanel : public wxPanel
{
private:
	CodeEditor* m_luaPanel;
	PTCCodeWriter m_luaCW;

#if wxVERSION_NUMBER < 2900
    void InitStyledTextCtrl( wxScintilla* stc );
#else
    void InitStyledTextCtrl( wxStyledTextCtrl* stc );
#endif

public:
	LuaPanel( wxWindow *parent, int id );
	~LuaPanel();

	void OnPropertyModified( wxFBPropertyEvent& event );
	void OnProjectRefresh( wxFBEvent& event );
	void OnCodeGeneration( wxFBEvent& event );
	void OnObjectChange( wxFBObjectEvent& event );
	void OnEventHandlerModified( wxFBEventHandlerEvent& event );

	void OnFind( wxFindDialogEvent& event );

	DECLARE_EVENT_TABLE()
};

#endif //__LUA_PANEL__
