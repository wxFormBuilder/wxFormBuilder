#pragma once

#include <wx/aui/tabart.h>

class AuiTabArt : public wxAuiGenericTabArt {
public:
    AuiTabArt() { UpdateColoursFromSystem(); }

    AuiTabArt* Clone() override { return new AuiTabArt(*this); }

    void UpdateColoursFromSystem();

    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const wxAuiNotebookPage& page,
                 const wxRect& in_rect,
                 int close_button_state,
                 wxRect* out_tab_rect,
                 wxRect* out_button_rect,
                 int* x_extent) override;

    wxSize GetTabSize(wxDC& dc,
                      wxWindow* wnd,
                      const wxString& caption,
                      const wxBitmap& bitmap,
                      bool active,
                      int close_button_state,
                      int* x_extent) override;
};
