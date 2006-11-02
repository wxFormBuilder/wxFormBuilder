#ifndef __INNER_FRAME__
#define __INNER_FRAME__

#include <wx/wx.h>


class wxInnerFrame : public wxPanel
{
private:

  DECLARE_EVENT_TABLE()

    enum{
      NONE,
      RIGHTBOTTOM,
      RIGHT,
      BOTTOM
    } m_sizing;

    int m_curX, m_curY, m_difX, m_difY;
    int m_resizeBorder;
    wxSize m_minSize;
    wxSize m_size;


  class TitleBar;

  TitleBar *m_titleBar;
  wxPanel *m_frameContent;

protected:
  //void AddChild(wxWindow *child);
public:
  wxInnerFrame(wxWindow *parent, wxWindowID id,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = 0);


  wxPanel *GetFrameContentPanel() { return m_frameContent; }
  void OnMouseMotion(wxMouseEvent& e);
  void OnLeftDown(wxMouseEvent& e);
  void OnLeftUp(wxMouseEvent& e);

  void SetTitle(const wxString &title);
  wxString GetTitle();

  void ShowTitleBar(bool show = true);
  bool IsTitleBarShown();
};

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_INNER_FRAME_RESIZED, 6000)
END_DECLARE_EVENT_TYPES()

#define EVT_INNER_FRAME_RESIZED(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_INNER_FRAME_RESIZED, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),



#endif //__INNER_FRAME__

