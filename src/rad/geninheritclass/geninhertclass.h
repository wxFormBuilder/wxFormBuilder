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
#ifndef __geninhertclass_imp__
#define __geninhertclass_imp__

#include "geninhertclass_gui.h"
#include <map>

class GenClassDetails
{
public:
	GenClassDetails() {}

	GenClassDetails( wxString className, wxString fileName, bool isSelected = false )
	{
		m_className = className;
		m_fileName = fileName;
		m_isSelected = isSelected;
	}

	wxString m_className;
	wxString m_fileName;
	bool     m_isSelected;
};

class GenInheritedClassDlg : public GenInheritedClassDlgBase
{
public:
	GenInheritedClassDlg( wxWindow* parent, wxArrayString availableForms );
	wxString GetClassName( const wxString& form );
	wxString GetFileName( const wxString& form );
	wxArrayString GetFormsSelected();

private:
	wxArrayString m_forms;
	std::map< wxString, GenClassDetails > m_classDetails;

	void OnFormsToggle( wxCommandEvent& event );
	void OnClassNameChange( wxCommandEvent& event );
	void OnFileNameChange( wxCommandEvent& event );
};

#endif //__geninhertclass_imp__
