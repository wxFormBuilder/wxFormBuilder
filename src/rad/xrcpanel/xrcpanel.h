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

#ifndef __XRC_PANEL__
#define __XRC_PANEL__

#include <wx/panel.h>

#include "utils/wxfbdefs.h"

class CodeEditor;

class wxFBEvent;
class wxFBPropertyEvent;
class wxFBObjectEvent;

#if wxVERSION_NUMBER < 2900
    #include <wx/wxScintilla/wxscintilla.h>
#else
    #include <wx/stc/stc.h>
#endif

class wxFindDialogEvent;

class XrcPanel : public wxPanel
{
private:
	CodeEditor* m_xrcPanel;
	PTCCodeWriter m_cw;

#if wxVERSION_NUMBER < 2900
    void InitStyledTextCtrl( wxScintilla* stc );
#else
    void InitStyledTextCtrl( wxStyledTextCtrl* stc );
#endif

public:
	XrcPanel( wxWindow *parent, int id );
	~XrcPanel();

	void OnPropertyModified( wxFBPropertyEvent& event );
	void OnProjectRefresh( wxFBEvent& event );
	void OnCodeGeneration( wxFBEvent& event );
	void OnObjectChange( wxFBObjectEvent& event );

	void OnFind( wxFindDialogEvent& event );

	DECLARE_EVENT_TABLE()
};


#endif //__XRC_PANEL__
