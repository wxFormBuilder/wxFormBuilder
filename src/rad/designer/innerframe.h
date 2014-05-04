///////////////////////////////////////////////////////////////////////////////
//
// wxFormBuilder - A Visual Dialog Editor for wxWidgets.
// Copyright (C) 2005 José Antonio Hurtado
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   José Antonio Hurtado - joseantonio.hurtado@gmail.com
//   Juan Antonio Ortega  - jortegalalmolda@gmail.com
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __INNER_FRAME__
#define __INNER_FRAME__

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include "utils/debug.h"


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
    wxSize m_baseMinSize;

  class TitleBar;

  TitleBar *m_titleBar;
  wxPanel *m_frameContent;
  //wxAuiManager m_mgr
protected:
    wxSize DoGetBestSize() const;
public:
  wxInnerFrame(wxWindow *parent, wxWindowID id,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = 0);

  //virtual ~wxInnerFrame(){m_mgr.UnInit();}

  wxPanel *GetFrameContentPanel() { return m_frameContent; }
  void OnMouseMotion(wxMouseEvent& e);
  void OnLeftDown(wxMouseEvent& e);
  void OnLeftUp(wxMouseEvent& e);

  //wxAuiManager& GetAuiManager() {return m_mgr;}

  void SetTitle(const wxString &title);
  wxString GetTitle();

  void SetTitleStyle( long style );

  void ShowTitleBar(bool show = true);
  void SetToBaseSize();
  bool IsTitleBarShown();
};

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_INNER_FRAME_RESIZED, -1)
END_DECLARE_EVENT_TYPES()

#define EVT_INNER_FRAME_RESIZED(id, fn) \
  wx__DECLARE_EVT1(wxEVT_INNER_FRAME_RESIZED,id,wxCommandEventHandler(fn))

/*wxDECLARE_EVENT(wxEVT_INNER_FRAME_RESIZED, wxCommandEvent);

#define EVT_INNER_FRAME_RESIZED(id, func) \
wx__DECLARE_EVT1(wxEVT_INNER_FRAME_RESIZED, id, &func)*/

#endif //__INNER_FRAME__


