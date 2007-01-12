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

#include <wx/wx.h>

#include "geninhertclass.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( GenInheritedClassDlg, wxDialog )
	//EVT_TEXT( ID_CLASS_NAME_TEXT_CTRL, GenInheritedClassDlg::OnText )
END_EVENT_TABLE()

GenInheritedClassDlg::GenInheritedClassDlg( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->Centre( wxBOTH );

	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* instructionsSbSizer;
	instructionsSbSizer = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Instructions") ), wxVERTICAL );

	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("1. Check the top level windows you would like to create the inherited class for.\n2. Change the .h/.cpp file names by selecting each of the items and editing in\nthe 'File Names' section.\n3. Press 'OK'."), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE );
	instructionsSbSizer->Add( m_staticText7, 0, wxALL|wxEXPAND, 5 );

	mainSizer->Add( instructionsSbSizer, 0, wxTOP|wxRIGHT|wxLEFT|wxEXPAND, 5 );

	wxString m_topLevelWindowsCheckListChoices[] = {  };
	int m_topLevelWindowsCheckListNChoices = sizeof( m_topLevelWindowsCheckListChoices ) / sizeof( wxString );
	m_topLevelWindowsCheckList = new wxCheckListBox( this, ID_TOP_LEVEL_WINDOWS_CHECK_LIST, wxDefaultPosition, wxDefaultSize, m_topLevelWindowsCheckListNChoices, m_topLevelWindowsCheckListChoices, 0 );
	m_topLevelWindowsCheckList->SetMinSize( wxSize( 350,150 ) );

	mainSizer->Add( m_topLevelWindowsCheckList, 0, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* fileNamesSbSizer;
	fileNamesSbSizer = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("File Names") ), wxVERTICAL );

	m_classNameStaticText = new wxStaticText( this, wxID_ANY, wxT("Class Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	fileNamesSbSizer->Add( m_classNameStaticText, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );

	m_classNameTextCtrl = new wxTextCtrl( this, ID_CLASS_NAME_TEXT_CTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fileNamesSbSizer->Add( m_classNameTextCtrl, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );

	m_fileNameStaticText = new wxStaticText( this, wxID_ANY, wxT(".cpp/.h:"), wxDefaultPosition, wxDefaultSize, 0 );
	fileNamesSbSizer->Add( m_fileNameStaticText, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );

	m_fileNameTextCtrl = new wxTextCtrl( this, ID_FILE_NAME_TEXT_CTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fileNamesSbSizer->Add( m_fileNameTextCtrl, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );

	m_cppFileNameStaticText = new wxStaticText( this, wxID_ANY, wxT(".cpp:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cppFileNameStaticText->Hide();

	fileNamesSbSizer->Add( m_cppFileNameStaticText, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );

	m_cppFileNameTextCtrl = new wxTextCtrl( this, ID_CPP_FILE_NAME_TEXT_CTRL, wxT("%wxFB_ProjectName%%wxFB_TopLevelName%.cpp"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cppFileNameTextCtrl->Hide();

	fileNamesSbSizer->Add( m_cppFileNameTextCtrl, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );

	mainSizer->Add( fileNamesSbSizer, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizer->AddButton( new wxButton( this, wxID_OK ) );
	m_sdbSizer->AddButton( new wxButton( this, wxID_CANCEL ) );
	m_sdbSizer->Realize();
	mainSizer->Add( m_sdbSizer, 0, wxALL|wxALIGN_RIGHT, 5 );

	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
}
