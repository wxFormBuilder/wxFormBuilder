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

#include "rad/genericpanel.h"

BEGIN_EVENT_TABLE(GenericWindow,wxPanel)
  EVT_PAINT(GenericWindow::OnPaint)
END_EVENT_TABLE()

GenericWindow::GenericWindow(wxWindow *parent)
  : wxPanel (parent, -1,wxDefaultPosition,wxSize(30,30))
{
}


void GenericWindow::OnPaint(wxPaintEvent &)
{  
  wxPaintDC dc(this);
  wxSize size = GetSize();
  dc.SetPen(*wxBLACK_PEN);
  for (int i=0;i<size.GetWidth();i += 2)
    for (int j=0;j<size.GetHeight();j += 2)
      dc.DrawPoint(i-1,j-1);
}

