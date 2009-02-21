#include "wxFBDialogsFBSettings.h"

wxFBDialogsFBSettings::wxFBDialogsFBSettings( wxWindow* parent )
:
FBSettings( parent )
{

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
	EndModal(wxID_OK);
}
