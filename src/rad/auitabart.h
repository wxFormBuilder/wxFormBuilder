#ifndef __AUITABART_H__
#define __AUITABART_H__

#if wxVERSION_NUMBER >= 2900

#include <wx/aui/auibook.h>

class AuiTabArt : public wxAuiGenericTabArt
{
public:
	virtual ~AuiTabArt();
	
	virtual wxSize GetTabSize(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxString& caption,
                 const wxBitmap& bitmap,
                 bool active,
                 int close_button_state,
                 int* x_extent) {
					 
		return wxAuiGenericTabArt::GetTabSize( dc, wnd, caption, bitmap, active, close_button_state, x_extent ) + wxSize( 0, 3 );
	}
};

#endif

#endif // __AUITABART_H__
