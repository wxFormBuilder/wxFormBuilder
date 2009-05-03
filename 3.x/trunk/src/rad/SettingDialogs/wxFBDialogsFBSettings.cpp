#include "wxFBDialogsFBSettings.h"
#include "wx/config.h"

wxFBDialogsFBSettings::wxFBDialogsFBSettings( wxWindow* parent )
:
FBSettings( parent )
{
	wxConfigBase *config = wxConfigBase::Get();
	config->SetPath( wxT("Settings") );

	wxString StrVal;
	bool boolVal;
	config->Read( wxT( "ID_Generation" ), &boolVal );
	m_idGenBox->SetValue(boolVal);
	config->Read( wxT( "ID_GenString" ),  &StrVal);
	m_IDGenString->SetValue(StrVal);
	config->Read( wxT( "event_Generation" ), &boolVal );
	m_eventGenBox->SetValue(boolVal);
	config->Read( wxT( "event_GenString" ), &StrVal);
	m_eventGenString->SetValue(StrVal) ;

	config->SetPath( wxT( ".." ) );

}

void wxFBDialogsFBSettings::m_idBoxOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_idBoxOnCheckBox
}

void wxFBDialogsFBSettings::m_handlerBoxOnCheckBox( wxCommandEvent& event )
{
// TODO: Implement m_handlerBoxOnCheckBox
}

void wxFBDialogsFBSettings::m_sdbSizer1OnCancelButtonClick( wxCommandEvent& event )
{
	EndModal(wxID_CANCEL);
}

void wxFBDialogsFBSettings::m_sdbSizer1OnOKButtonClick( wxCommandEvent& event )
{
	wxConfigBase *config = wxConfigBase::Get();
	config->SetPath( wxT("Settings") );

	config->Write( wxT( "ID_Generation" ), m_idGenBox->GetValue() );
	config->Write( wxT( "ID_GenString" ), m_IDGenString->GetValue() );
	config->Write( wxT( "event_Generation" ), m_eventGenBox->GetValue() );
	config->Write( wxT( "event_GenString" ), m_eventGenString->GetValue() );

	config->SetPath( wxT( ".." ) );

	EndModal(wxID_OK);
}
