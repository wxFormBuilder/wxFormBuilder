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

#ifndef __CODE_EDITOR__
#define __CODE_EDITOR__

#include <wx/panel.h>

#if wxVERSION_NUMBER < 2900
    class wxScintilla;
    class wxScintillaEvent;
#else
    class wxStyledTextCtrl;
    class wxStyledTextEvent;
#endif

class wxFindDialogEvent;

class CodeEditor : public wxPanel
{
private:
#if wxVERSION_NUMBER < 2900
	wxScintilla* m_code;
	void OnMarginClick( wxScintillaEvent& event );
#else
    wxStyledTextCtrl* m_code;
    void OnMarginClick( wxStyledTextEvent& event );
#endif
	DECLARE_EVENT_TABLE()

public:
	CodeEditor( wxWindow *parent, int id );

#if wxVERSION_NUMBER < 2900
	wxScintilla* GetTextCtrl();
#else
    wxStyledTextCtrl* GetTextCtrl();
#endif

	void OnFind( wxFindDialogEvent& event );
};

#endif //__CODE_EDITOR__
