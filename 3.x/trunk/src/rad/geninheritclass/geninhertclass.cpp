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

	// Setup the initial values for the maps of class names and file names.
	for ( size_t i = 0; i < availableForms.size(); ++i )
	{
		m_classDetails[availableForms[i]] = GenClassDetails( wxT("ClassName"), wxT("FileName") );;
	}

	// Add the forms to the listctrl.
	for ( size_t i = 0; i < m_forms.size(); ++i )
	{
		m_formsCheckList->AppendString( m_forms[i] );
	}

	// Set the initial selection.
	//m_formsCheckList->SetSelection( 0 );

	// Disable the controls till the check listbox is selected.
	m_classNameTextCtrl->Disable();
	m_fileNameTextCtrl->Disable();
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

void GenInheritedClassDlg::OnFormsSelected( wxCommandEvent& event )
{
	// Enable controls because an item is selected.
	if ( m_formsCheckList->GetSelection() != wxNOT_FOUND )
	{
		m_classNameTextCtrl->Enable();
		m_fileNameTextCtrl->Enable();
	}

	// Set the values of the controls form the map.
	std::map< wxString, GenClassDetails >::iterator it =
		m_classDetails.find( m_formsCheckList->GetStringSelection() );
	if ( it != m_classDetails.end() )
	{
		m_classNameTextCtrl->SetValue( it->second.m_className );
		m_fileNameTextCtrl->SetValue( it->second.m_fileName );
	}

}

void GenInheritedClassDlg::OnFormsToggle( wxCommandEvent& event )
{
	/*std::map< wxString, GenClassDetails >::iterator it =
		m_classDetails.find( m_formsCheckList->GetStringSelection() );
	if ( it != m_classDetails.end() )
	{
		it->second.m_isSelected = true;
	}*/
}

void GenInheritedClassDlg::OnClassNameChange( wxCommandEvent& event )
{
	std::map< wxString, GenClassDetails >::iterator it =
		m_classDetails.find( m_formsCheckList->GetStringSelection() );
	if ( it != m_classDetails.end() )
	{
		it->second.m_className = m_classNameTextCtrl->GetValue();
	}
}

void GenInheritedClassDlg::OnFileNameChange( wxCommandEvent& event )
{
	std::map< wxString, GenClassDetails >::iterator it =
		m_classDetails.find( m_formsCheckList->GetStringSelection() );
	if ( it != m_classDetails.end() )
	{
		it->second.m_fileName = m_fileNameTextCtrl->GetValue();
	}
}
