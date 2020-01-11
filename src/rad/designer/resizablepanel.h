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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// Written by
//   José Antonio Hurtado - joseantonio.hurtado@gmail.com
//   Juan Antonio Ortega  - jortegalalmolda@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

#ifndef RESIZABLEPANEL_H
#define RESIZABLEPANEL_H

#include <wx/wxprec.h>
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/event.h>

class ResizablePanel : public wxPanel
{
    enum{
      NONE,
      RIGHTBOTTOM,
      RIGHT,
      BOTTOM
    } m_sizing;
    
    int m_curX, m_curY, m_difX, m_difY;
    int m_resizeBorder;
    wxSize m_minSize;
  public:
    ResizablePanel(wxWindow *parent, const wxPoint& pos = wxDefaultPosition, 
      const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
    
    void SetResizeBorder(int border);
    int GetResizeBorder();
    void SetMinSize(const wxSize& size);
    wxSize GetMinSize();
    
    void OnMouseMotion(wxMouseEvent& e);
    void OnLeftDown(wxMouseEvent& e);
    void OnSetCursor(wxSetCursorEvent& e);
    void OnLeftUp(wxMouseEvent& e);
    //void OnSize(wxSizeEvent& e);
    void OnPanelResized(wxSizeEvent &e);
    
    DECLARE_EVENT_TABLE()
};

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_PANEL_RESIZED, 6000)
END_DECLARE_EVENT_TYPES()

#define EVT_PANEL_RESIZED(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_PANEL_RESIZED, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#endif
