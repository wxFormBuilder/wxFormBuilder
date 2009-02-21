#include "wxFBDialogsProjectSettings.h"

wxFBDialogsProjectSettings::wxFBDialogsProjectSettings( wxWindow* parent )
:
ProjectSettings( parent )
{

}

void wxFBDialogsProjectSettings::m_sdbSizer2OnCancelButtonClick( wxCommandEvent& event )
{
	EndModal(wxID_CANCEL);
}

void wxFBDialogsProjectSettings::m_sdbSizer2OnOKButtonClick( wxCommandEvent& event )
{
	EndModal(wxID_OK);
}
