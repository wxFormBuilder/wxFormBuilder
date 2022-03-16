#ifndef RAD_AUITABART_H
#define RAD_AUITABART_H

#include <wx/aui/auibook.h>


class AuiTabArt : public wxAuiGenericTabArt
{
public:
#if wxCHECK_VERSION(3, 1, 6)
    wxSize GetTabSize(
      wxDC& dc, wxWindow* wnd, const wxString& caption, const wxBitmapBundle& bitmap, bool active,
      int close_button_state, int* x_extent) override{
#else
    wxSize GetTabSize(
      wxDC& dc, wxWindow* wnd, const wxString& caption, const wxBitmap& bitmap, bool active, int close_button_state,
      int* x_extent) override
    {
#endif
      return wxAuiGenericTabArt::GetTabSize(dc, wnd, caption, bitmap, active, close_button_state, x_extent) +
             wxSize(0, 2);
}
}
;

#endif  // RAD_AUITABART_H
