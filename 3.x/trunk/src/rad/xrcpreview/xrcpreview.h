
#include "utils/wxfbdefs.h"

class wxWindow;

class XRCPreview
{
private:
	static void AddEventHandler( wxWindow* window, wxWindow* form );
public:
	static void Show( PObjectBase form, const wxString& projectpath );
};
