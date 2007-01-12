///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jan 12 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "geninhertclass_gui.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( GenInheritedClassDlgBase, wxDialog )
	EVT_TEXT( ID_CLASS_NAME_TEXT_CTRL, GenInheritedClassDlgBase::_wxFB_OnT )
END_EVENT_TABLE()

GenInheritedClassDlgBase::GenInheritedClassDlgBase( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->Centre( wxBOTH );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* instructionsSbSizer;
	instructionsSbSizer = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Instructions") ), wxVERTICAL );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("1. Check the forms you would like to create the inherited class for.\n2. You can edit individual class details by clicking on their names in the list\nand then:\n\t2a. Edit the 'Class Name:' as required.\n\t2b. Edit the 'File Names: (.h/.cpp)' as required.\n3. Click 'OK'."), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE );
	instructionsSbSizer->Add( m_staticText7, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( instructionsSbSizer, 0, wxTOP|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	wxString m_formsCheckListChoices[] = {  };
	int m_formsCheckListNChoices = sizeof( m_formsCheckListChoices ) / sizeof( wxString );
	m_formsCheckList = new wxCheckListBox( this, ID_FORMS_CHECK_LIST, wxDefaultPosition, wxDefaultSize, m_formsCheckListNChoices, m_formsCheckListChoices, 0 );
	m_formsCheckList->SetMinSize( wxSize( 350,150 ) );
	
	mainSizer->Add( m_formsCheckList, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* classDescriptionSbSizer;
	classDescriptionSbSizer = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Class Details") ), wxVERTICAL );
	
	m_classNameStaticText = new wxStaticText( this, wxID_ANY, wxT("Class Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	classDescriptionSbSizer->Add( m_classNameStaticText, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_classNameTextCtrl = new wxTextCtrl( this, ID_CLASS_NAME_TEXT_CTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	classDescriptionSbSizer->Add( m_classNameTextCtrl, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_fileNameStaticText = new wxStaticText( this, wxID_ANY, wxT("File Names: (.cpp/.h)"), wxDefaultPosition, wxDefaultSize, 0 );
	classDescriptionSbSizer->Add( m_fileNameStaticText, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_fileNameTextCtrl = new wxTextCtrl( this, ID_FILE_NAME_TEXT_CTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	classDescriptionSbSizer->Add( m_fileNameTextCtrl, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	mainSizer->Add( classDescriptionSbSizer, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizer->AddButton( new wxButton( this, wxID_OK ) );
	m_sdbSizer->AddButton( new wxButton( this, wxID_CANCEL ) );
	m_sdbSizer->Realize();
	mainSizer->Add( m_sdbSizer, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
}
