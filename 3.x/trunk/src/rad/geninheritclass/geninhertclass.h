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

#ifndef __GENINHERTCLASS__
#define __GENINHERTCLASS__

#include <wx/wx.h>
#include <wx/checklst.h>

#include "utils/typeconv.h"
#include "rad/wxfbevent.h"
#include "rad/appdata.h"
#include "model/objectbase.h"

///////////////////////////////////////////////////////////////////////////

#define ID_TOP_LEVEL_WINDOWS_CHECK_LIST 1000
#define ID_CLASS_NAME_TEXT_CTRL 1001
#define ID_FILE_NAME_TEXT_CTRL 1002
#define ID_CPP_FILE_NAME_TEXT_CTRL 1003

///////////////////////////////////////////////////////////////////////////////
/// Class GenInheritedClassDlg
///////////////////////////////////////////////////////////////////////////////
class GenInheritedClassDlg : public wxDialog
{
	DECLARE_EVENT_TABLE()
	private:

	protected:
		wxStaticText* m_staticText7;
		wxCheckListBox* m_topLevelWindowsCheckList;
		wxStaticText* m_classNameStaticText;
		wxTextCtrl* m_classNameTextCtrl;
		wxStaticText* m_fileNameStaticText;
		wxTextCtrl* m_fileNameTextCtrl;
		wxStaticText* m_cppFileNameStaticText;
		wxTextCtrl* m_cppFileNameTextCtrl;
		wxStdDialogButtonSizer* m_sdbSizer;

	public:
		GenInheritedClassDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Generate Inherited Class"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( -1,-1 ), int style = wxDEFAULT_DIALOG_STYLE );

};

#endif //__GENINHERTCLASS__
