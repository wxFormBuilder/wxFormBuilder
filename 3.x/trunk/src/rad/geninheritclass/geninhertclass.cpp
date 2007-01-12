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
#include "geninhertclass.h"

GenInheritedClassDlg::GenInheritedClassDlg( wxWindow* parent, wxArrayString availableForms )
	: GenInheritedClassDlgBase( parent )
{
	m_forms = availableForms;
	// Set the initial selection.
	m_formsCheckList->SetSelection( 0 );

	// Setup the initial values for the maps of class names and file names.
	for ( size_t i = 0; i < availableForms.size(); ++i )
	{
		GenClassDetails classDetails( wxT("ClassName"), wxT("FileName") );
		m_classDetails[availableForms[i]] = classDetails;
	}

}

wxString GenInheritedClassDlg::GetClassName( const wxString& form )
{
	std::map< wxString, GenClassDetails >::iterator it = m_classDetails.find( form );
	if ( it != m_classDetails.end() )
	{
		return it->second.m_className;
	}

	return wxEmptyString;
}

wxString GenInheritedClassDlg::GetFileName( const wxString& form )
{
	std::map< wxString, GenClassDetails >::iterator it = m_classDetails.find( form );
	if ( it != m_classDetails.end() )
	{
		return it->second.m_fileName;
	}

	return wxEmptyString;
}

wxArrayString GenInheritedClassDlg::GetFormsSelected()
{
	return m_forms;
}

void GenInheritedClassDlg::OnFormsToggle( wxCommandEvent& event )
{
}

void GenInheritedClassDlg::OnClassNameChange( wxCommandEvent& event )
{
}

void GenInheritedClassDlg::OnFileNameChange( wxCommandEvent& event )
{
}
