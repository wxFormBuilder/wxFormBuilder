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

#include "resizablepanel.h"

DEFINE_EVENT_TYPE(wxEVT_PANEL_RESIZED)

BEGIN_EVENT_TABLE(ResizablePanel, wxPanel)
  EVT_SET_CURSOR(ResizablePanel::OnSetCursor)
  EVT_LEFT_DOWN(ResizablePanel::OnLeftDown)
  EVT_LEFT_UP(ResizablePanel::OnLeftUp)
  EVT_MOTION(ResizablePanel::OnMouseMotion)
  //EVT_SIZE(ResizablePanel::OnSize)
END_EVENT_TABLE()

ResizablePanel::ResizablePanel(wxWindow *parent, const wxPoint& pos, const wxSize& size,
  long style) : 
  wxPanel(parent, -1, pos, size, style)
{
  m_sizing = NONE;
  m_curX = m_curY = -1;
  m_minSize = wxSize(10, 10);
  m_resizeBorder = 10;
}

void ResizablePanel::SetResizeBorder(int border)
{
  wxASSERT(border > 0);
  m_resizeBorder = border;
}

int ResizablePanel::GetResizeBorder()
{
  return m_resizeBorder;
}

void ResizablePanel::SetMinSize(const wxSize& size)
{
  m_minSize = size;
}

wxSize ResizablePanel::GetMinSize()
{
  return m_minSize;
}

void ResizablePanel::OnSetCursor(wxSetCursorEvent& e)
{
  if (e.GetX() >= GetSize().x - m_resizeBorder && e.GetY() >= GetSize().y - m_resizeBorder)
    e.SetCursor(wxCursor(wxCURSOR_SIZENWSE));
  else if (e.GetX() >= GetSize().x - m_resizeBorder)
    e.SetCursor(wxCursor(wxCURSOR_SIZEWE));
  else if (e.GetY() >= GetSize().y - m_resizeBorder)
    e.SetCursor(wxCursor(wxCURSOR_SIZENS));
  else
    e.SetCursor(*wxSTANDARD_CURSOR);
}

void ResizablePanel::OnMouseMotion(wxMouseEvent& e)
{
  if (m_sizing != NONE)
  { 
    wxScreenDC dc;
    wxPen pen(*wxBLACK, 1, wxDOT);
    
    dc.SetPen(pen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetLogicalFunction(wxINVERT);
    
    //wxPoint pos = ClientToScreen(wxPoint(0, 0));
    wxPoint pos = GetParent()->ClientToScreen(GetPosition());
    
    if (m_curX >= 0 && m_curY >= 0)
      dc.DrawRectangle(pos.x, pos.y, m_curX, m_curY);
      
    if (m_sizing == RIGHT || m_sizing == RIGHTBOTTOM)
      m_curX = e.GetX() + m_difX;
    else
      m_curX = GetSize().x;
    
    if (m_sizing == BOTTOM || m_sizing == RIGHTBOTTOM)  
      m_curY = e.GetY() + m_difY;
    else
      m_curY = GetSize().y;
      
    if (m_curX < m_minSize.x) m_curX = m_minSize.x;
    if (m_curY < m_minSize.y) m_curY = m_minSize.y;
    
    dc.DrawRectangle(pos.x, pos.y, m_curX, m_curY);
  
    dc.SetLogicalFunction(wxCOPY);
    dc.SetPen(wxNullPen);
    dc.SetBrush(wxNullBrush);
  }
}

void ResizablePanel::OnLeftDown(wxMouseEvent& e)
{
  if (m_sizing == NONE)
  {
    if (e.GetX() >= GetSize().x - m_resizeBorder && e.GetY() >= GetSize().y - m_resizeBorder)
      m_sizing = RIGHTBOTTOM;
    else if (e.GetX() >= GetSize().x - m_resizeBorder)
      m_sizing = RIGHT;
    else if (e.GetY() >= GetSize().y - m_resizeBorder)
      m_sizing = BOTTOM;
      
    if (m_sizing != NONE)
    {
      m_difX = GetSize().x - e.GetX();
      m_difY = GetSize().y - e.GetY();
      CaptureMouse();
      OnMouseMotion(e);
    }
  }
}

void ResizablePanel::OnLeftUp(wxMouseEvent& )
{
  if (m_sizing != NONE)
  {
    m_sizing = NONE;
    ReleaseMouse();
    
    wxScreenDC dc;
    wxPen pen(*wxBLACK, 1, wxDOT);
    
    dc.SetPen(pen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetLogicalFunction(wxINVERT);
    
    //wxPoint pos = ClientToScreen(wxPoint(0, 0));
    wxPoint pos = GetParent()->ClientToScreen(GetPosition());
    
    dc.DrawRectangle(pos.x, pos.y, m_curX, m_curY);
  
    dc.SetLogicalFunction(wxCOPY);
    dc.SetPen(wxNullPen);
    dc.SetBrush(wxNullBrush);
    
    SetSize(m_curX, m_curY);
    
    wxCommandEvent event(wxEVT_PANEL_RESIZED, GetId());
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
    
    m_curX = m_curY = -1;
  }
}

/*
// Cuando cambie de tamaño el panel no interesa que se genere el evento
// Resized. Si el panel cambiaba de tamaño por un SetSize (p.e. cuando estaba
// el tamaño por defecto) se actualizaba la propiedad size del form.
//
void ResizablePanel::OnSize(wxSizeEvent& e)
{
  wxCommandEvent event(wxEVT_PANEL_RESIZED, GetId());
  event.SetEventObject(this);
  GetEventHandler()->ProcessEvent(event);
}
*/
