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
#include "model/objectbase.h"

GenInheritedClassDlg::GenInheritedClassDlg( wxWindow* parent, PObjectBase project )
:
GenInheritedClassDlgBase( parent )
{
	const wxString& projectName = project->GetPropertyAsString( _("name") );

	// Setup the initial values for the maps of class names and file names.
	for ( unsigned int i = 0; i < project->GetChildCount(); ++i )
	{
		PObjectBase child = project->GetChild( i );
		const wxString& formName = child->GetPropertyAsString( _("name") );
		const wxString& name = projectName + formName;
		m_classDetails.push_back( GenClassDetails( child, name, name ) );

		// Add the forms to the listctrl.
		m_formsCheckList->AppendString( formName );
	}

	// Disable the controls till the check listbox is selected.
	m_classNameTextCtrl->Disable();
	m_fileNameTextCtrl->Disable();
}

void GenInheritedClassDlg::GetFormsSelected( std::vector< GenClassDetails >* forms )
{
	// Clear the selected forms array.
	forms->clear();

	for ( size_t i = 0; i < m_classDetails.size(); ++i )
	{
		if ( m_classDetails[i].m_isSelected )
		{
			forms->push_back( m_classDetails[i] );
		}
	}
}

void GenInheritedClassDlg::OnFormsSelected( wxCommandEvent& )
{
	// Enable controls because an item is selected.
	int selection = m_formsCheckList->GetSelection();
	if ( wxNOT_FOUND == selection )
	{
		m_classNameTextCtrl->Disable();
		m_fileNameTextCtrl->Disable();
		return;
	}

	m_classNameTextCtrl->Enable();
	m_fileNameTextCtrl->Enable();

	// Set the values of the controls
	GenClassDetails& details = m_classDetails[ selection ];
	m_classNameTextCtrl->SetValue( details.m_className );
	m_fileNameTextCtrl->SetValue( details.m_fileName );
}

void GenInheritedClassDlg::OnFormsToggle( wxCommandEvent& event )
{
	int selection = event.GetSelection();
	m_classDetails[ selection ].m_isSelected = m_formsCheckList->IsChecked( selection );
}

void GenInheritedClassDlg::OnClassNameChange( wxCommandEvent& )
{
	m_classDetails[ m_formsCheckList->GetSelection() ].m_className = m_classNameTextCtrl->GetValue();
}

void GenInheritedClassDlg::OnFileNameChange( wxCommandEvent& )
{
	m_classDetails[ m_formsCheckList->GetSelection() ].m_fileName = m_fileNameTextCtrl->GetValue();
}
