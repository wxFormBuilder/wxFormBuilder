///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-63fd119)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wizardbase.h"

///////////////////////////////////////////////////////////////////////////

WizardBase::WizardBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* windowSizer;
	windowSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* mainColumn;
	mainColumn = new wxBoxSizer( wxVERTICAL );

	m_sizerBmpAndPage = new wxBoxSizer( wxHORIZONTAL );

	m_sizerBmpAndPage->SetMinSize( wxSize( 270,270 ) );
	m_statbmp = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_sizerBmpAndPage->Add( m_statbmp, 0, wxALL, 5 );


	m_sizerBmpAndPage->Add( 5, 0, 0, 0, 0 );

	m_sizerPage = new wxBoxSizer( wxVERTICAL );


	m_sizerBmpAndPage->Add( m_sizerPage, 1, wxEXPAND, 0 );


	mainColumn->Add( m_sizerBmpAndPage, 1, wxEXPAND, 0 );


	mainColumn->Add( 0, 5, 0, 0, 0 );

	wxStaticLine* statline;
	statline = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainColumn->Add( statline, 0, wxEXPAND | wxALL, 5 );


	mainColumn->Add( 0, 5, 0, 0, 0 );

	wxBoxSizer* buttonRow;
	buttonRow = new wxBoxSizer( wxHORIZONTAL );

	m_btnHelp = new wxButton( this, wxID_HELP, _("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btnHelp->Hide();

	buttonRow->Add( m_btnHelp, 0, wxALL, 5 );

	wxBoxSizer* backNextPair;
	backNextPair = new wxBoxSizer( wxHORIZONTAL );

	m_btnPrev = new wxButton( this, wxID_BACKWARD, _("< &Back"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btnPrev->Enable( false );

	backNextPair->Add( m_btnPrev, 0, wxBOTTOM|wxLEFT|wxTOP, 0 );


	backNextPair->Add( 10, 0, 0, 0, 0 );

	m_btnNext = new wxButton( this, wxID_FORWARD, _("&Next >"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btnNext->Enable( false );

	backNextPair->Add( m_btnNext, 0, wxBOTTOM|wxRIGHT|wxTOP, 0 );


	buttonRow->Add( backNextPair, 0, wxALL, 5 );

	m_btnCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonRow->Add( m_btnCancel, 0, wxALL, 5 );


	mainColumn->Add( buttonRow, 0, wxALIGN_RIGHT, 0 );


	windowSizer->Add( mainColumn, 1, wxALL|wxEXPAND, 5 );


	this->SetSizer( windowSizer );
	this->Layout();
	windowSizer->Fit( this );

	// Connect Events
	m_btnHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WizardBase::OnHelp ), NULL, this );
	m_btnPrev->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WizardBase::OnBackOrNext ), NULL, this );
	m_btnNext->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WizardBase::OnBackOrNext ), NULL, this );
	m_btnCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WizardBase::OnCancel ), NULL, this );
}

WizardBase::~WizardBase()
{
	// Disconnect Events
	m_btnHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WizardBase::OnHelp ), NULL, this );
	m_btnPrev->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WizardBase::OnBackOrNext ), NULL, this );
	m_btnNext->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WizardBase::OnBackOrNext ), NULL, this );
	m_btnCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WizardBase::OnCancel ), NULL, this );

}

WizardPageSimple::WizardPageSimple( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
}

WizardPageSimple::~WizardPageSimple()
{
}
