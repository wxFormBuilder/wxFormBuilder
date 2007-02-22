#include "wxFBEventMainFrame.h"
#include "wxFBEventAdditionalPanel.h"

wxFBEventMainFrame::wxFBEventMainFrame( wxWindow* parent )
:
MainFrame( parent )
{
	// Change the panel you want to show here.
	new wxFBEventAdditionalPanel( this );
}
