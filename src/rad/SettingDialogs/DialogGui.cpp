///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 13 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "DialogGui.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( FBSettings, wxDialog )
	EVT_CHECKBOX( ID_M_IDBOX, FBSettings::_wxFB_m_idBoxOnCheckBox )
	EVT_CHECKBOX( ID_M_HANDLERBOX, FBSettings::_wxFB_m_handlerBoxOnCheckBox )
	EVT_BUTTON( wxID_CANCEL, FBSettings::_wxFB_m_sdbSizer1OnCancelButtonClick )
	EVT_BUTTON( wxID_OK, FBSettings::_wxFB_m_sdbSizer1OnOKButtonClick )
END_EVENT_TABLE()

FBSettings::FBSettings( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_listbook2 = new wxListbook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT );
	m_panel5 = new wxPanel( m_listbook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panel5, wxID_ANY, wxT("Inherited Class") ), wxVERTICAL );
	
	UserBlocksCheckBox = new wxCheckBox( m_panel5, wxID_ANY, wxT("Create User Blocks"), wxDefaultPosition, wxDefaultSize, 0 );
	UserBlocksCheckBox->SetToolTip( wxT("This will cause wxFB to create sections in the header files that\nwill not be overwritten in any subsequent generated inherited class.") );
	
	sbSizer1->Add( UserBlocksCheckBox, 0, wxALL, 5 );
	
	bSizer11->Add( sbSizer1, 0, wxEXPAND, 5 );
	
	bSizer10->Add( bSizer11, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_panel5, wxID_ANY, wxT("Event Handling") ), wxVERTICAL );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	m_idGenBox = new wxCheckBox( m_panel5, ID_M_IDBOX, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer17->Add( m_idGenBox, 1, wxALL, 5 );
	
	m_eventGenBox = new wxCheckBox( m_panel5, ID_M_HANDLERBOX, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer17->Add( m_eventGenBox, 1, wxALL, 5 );
	
	bSizer18->Add( bSizer17, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText3 = new wxStaticText( m_panel5, wxID_ANY, wxT("ID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer15->Add( m_staticText3, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticText1 = new wxStaticText( m_panel5, wxID_ANY, wxT("Event Handler Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer15->Add( m_staticText1, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer16->Add( bSizer15, 0, wxALIGN_RIGHT|wxEXPAND|wxALIGN_CENTER_VERTICAL|wxTOP, 5 );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	m_IDGenString = new wxTextCtrl( m_panel5, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer14->Add( m_IDGenString, 1, wxALL|wxEXPAND, 5 );
	
	m_eventGenString = new wxTextCtrl( m_panel5, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer14->Add( m_eventGenString, 1, wxALL|wxEXPAND, 5 );
	
	bSizer16->Add( bSizer14, 1, wxALIGN_RIGHT|wxEXPAND, 5 );
	
	bSizer18->Add( bSizer16, 1, wxEXPAND, 5 );
	
	sbSizer4->Add( bSizer18, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText4 = new wxStaticText( m_panel5, wxID_ANY, wxT("variables = <name> <event_type>\nmodifiers = <upper> <-*> \nexample: wxID<name<upper>>\n"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer19->Add( m_staticText4, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	sbSizer4->Add( bSizer19, 0, wxEXPAND, 5 );
	
	bSizer12->Add( sbSizer4, 0, wxEXPAND, 5 );
	
	bSizer10->Add( bSizer12, 1, wxEXPAND, 5 );
	
	m_panel5->SetSizer( bSizer10 );
	m_panel5->Layout();
	bSizer10->Fit( m_panel5 );
	m_listbook2->AddPage( m_panel5, wxT("Code Generation"), false );
	#ifndef __WXGTK__ // Small icon style not supported in GTK
	wxListView* m_listbook2ListView = m_listbook2->GetListView();
	long m_listbook2Flags = m_listbook2ListView->GetWindowStyleFlag();
	m_listbook2Flags = ( m_listbook2Flags & ~wxLC_ICON ) | wxLC_SMALL_ICON;
	m_listbook2ListView->SetWindowStyleFlag( m_listbook2Flags );
	#endif
	
	bSizer3->Add( m_listbook2, 1, wxEXPAND | wxALL, 5 );
	
	bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	bSizer1->Add( m_sdbSizer1, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
}

FBSettings::~FBSettings()
{
}

BEGIN_EVENT_TABLE( ProjectSettings, wxDialog )
	EVT_BUTTON( wxID_CANCEL, ProjectSettings::_wxFB_m_sdbSizer2OnCancelButtonClick )
	EVT_BUTTON( wxID_OK, ProjectSettings::_wxFB_m_sdbSizer2OnOKButtonClick )
END_EVENT_TABLE()

ProjectSettings::ProjectSettings( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_listbook3 = new wxListbook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT );
	m_panel4 = new wxPanel( m_listbook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_listbook3->AddPage( m_panel4, wxT("a page"), false );
	#ifndef __WXGTK__ // Small icon style not supported in GTK
	wxListView* m_listbook3ListView = m_listbook3->GetListView();
	long m_listbook3Flags = m_listbook3ListView->GetWindowStyleFlag();
	m_listbook3Flags = ( m_listbook3Flags & ~wxLC_ICON ) | wxLC_SMALL_ICON;
	m_listbook3ListView->SetWindowStyleFlag( m_listbook3Flags );
	#endif
	
	bSizer4->Add( m_listbook3, 1, wxEXPAND | wxALL, 5 );
	
	bSizer2->Add( bSizer4, 1, wxEXPAND, 5 );
	
	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton( this, wxID_OK );
	m_sdbSizer2->AddButton( m_sdbSizer2OK );
	m_sdbSizer2Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer2->AddButton( m_sdbSizer2Cancel );
	m_sdbSizer2->Realize();
	bSizer2->Add( m_sdbSizer2, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer2 );
	this->Layout();
}

ProjectSettings::~ProjectSettings()
{
}
