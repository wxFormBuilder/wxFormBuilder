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
#ifndef __GENINHERTCLASS_IMP__
#define __GENINHERTCLASS_IMP__

/**
@file
Helper dialog that helps users to generate the needed class that inherits from
the wxFormBuilder GUI code.
@author	Ryan Pusztai <rpusztai@gmail.com>
@date	01/14/2007
*/


#include "geninhertclass_gui.h"
#include <map>

/** Holds the details of the class to generate. */
class GenClassDetails
{
public:

	/** Default Constructor. */
	GenClassDetails() {}

	/**
		Constructor.
		@param className Name of the class to generate.
		@param fileName File name of the output files the at generated class will be in.
		@param isSelected If true then the class is selected, else it is not.
	*/
	GenClassDetails( wxString className, wxString fileName, bool isSelected = false )
	{
		m_className = className;
		m_fileName = fileName;
		m_isSelected = isSelected;
	}

	wxString m_className;		/**< Name of the class to generate. */
	wxString m_fileName;		/**< File name to generate the class in.  */
	bool     m_isSelected;		/**< Holds if the checkbox is selected for the form. */
};

/**  */
class GenInheritedClassDlg : public GenInheritedClassDlgBase
{
public:
	GenInheritedClassDlg( wxWindow* parent, const wxArrayString& availableForms, const wxString& projectName );
	wxString GetClassName( const wxString& form );
	wxString GetFileName( const wxString& form );
	wxArrayString GetFormsSelected();

private:
	wxArrayString	m_forms;
	wxArrayString	m_selectedForms;
	wxString		m_projectName;
	std::map< wxString, GenClassDetails > m_classDetails;

	void OnFormsSelected( wxCommandEvent& event );
	void OnFormsToggle( wxCommandEvent& event );
	void OnClassNameChange( wxCommandEvent& event );
	void OnFileNameChange( wxCommandEvent& event );
};

#endif //__GENINHERTCLASS_IMP__
