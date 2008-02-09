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
// C++ code generated with wxFormBuilder (version Dec 17 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "geninhertclass_gui.h"

///////////////////////////////////////////////////////////////////////////

GenInheritedClassDlgBase::GenInheritedClassDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* instructionsSbSizer;
	instructionsSbSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Instructions") ), wxVERTICAL );
	
	m_instructionsStaticText = new wxStaticText( this, wxID_ANY, wxT("1. Check the forms you would like to create the inherited class for.\n2. You can edit individual class details by clicking on their names in the list\nand then:\n\t2a. Edit the 'Class Name:' as required.\n\t2b. Edit the 'File Names: (.h/.cpp)' as required.\n3. Click 'OK'."), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE );
	m_instructionsStaticText->Wrap( -1 );
	instructionsSbSizer->Add( m_instructionsStaticText, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( instructionsSbSizer, 0, wxTOP|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	wxArrayString m_formsCheckListChoices;
	m_formsCheckList = new wxCheckListBox( this, ID_FORMS_CHECK_LIST, wxDefaultPosition, wxDefaultSize, m_formsCheckListChoices, 0 );
	m_formsCheckList->SetMinSize( wxSize( 350,150 ) );
	
	mainSizer->Add( m_formsCheckList, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* classDescriptionSbSizer;
	classDescriptionSbSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Class Details") ), wxVERTICAL );
	
	m_classNameStaticText = new wxStaticText( this, wxID_ANY, wxT("Class Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_classNameStaticText->Wrap( -1 );
	classDescriptionSbSizer->Add( m_classNameStaticText, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_classNameTextCtrl = new wxTextCtrl( this, ID_CLASS_NAME_TEXT_CTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	classDescriptionSbSizer->Add( m_classNameTextCtrl, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_fileNameStaticText = new wxStaticText( this, wxID_ANY, wxT("File Names: (.cpp/.h)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_fileNameStaticText->Wrap( -1 );
	classDescriptionSbSizer->Add( m_fileNameStaticText, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_fileNameTextCtrl = new wxTextCtrl( this, ID_FILE_NAME_TEXT_CTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	classDescriptionSbSizer->Add( m_fileNameTextCtrl, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	mainSizer->Add( classDescriptionSbSizer, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();
	mainSizer->Add( m_sdbSizer, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_formsCheckList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( GenInheritedClassDlgBase::OnFormsSelected ), NULL, this );
	m_formsCheckList->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( GenInheritedClassDlgBase::OnFormsToggle ), NULL, this );
	m_classNameTextCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GenInheritedClassDlgBase::OnClassNameChange ), NULL, this );
	m_fileNameTextCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GenInheritedClassDlgBase::OnFileNameChange ), NULL, this );
}

GenInheritedClassDlgBase::~GenInheritedClassDlgBase()
{
	// Disconnect Events
	m_formsCheckList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( GenInheritedClassDlgBase::OnFormsSelected ), NULL, this );
	m_formsCheckList->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( GenInheritedClassDlgBase::OnFormsToggle ), NULL, this );
	m_classNameTextCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GenInheritedClassDlgBase::OnClassNameChange ), NULL, this );
	m_fileNameTextCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GenInheritedClassDlgBase::OnFileNameChange ), NULL, this );
}
