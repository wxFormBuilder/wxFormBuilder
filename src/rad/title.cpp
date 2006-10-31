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

#include "title.h"

Title::Title(wxWindow *parent,const wxString &title) : wxPanel(parent,-1)
{
  wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );

  wxStaticText *text = new wxStaticText(this,-1,title);//,wxDefaultPosition,wxDefaultSize,wxSIMPLE_BORDER);
  SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );
  text->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );
  text->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );
  text->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, 0, wxT("")));

  sizer->Add(text,0,wxALL | wxEXPAND,2);
  SetSizer(sizer);
  Fit();
}

wxWindow * Title::CreateTitle (wxWindow *inner, const wxString &title)
{
  wxWindow *parent = inner->GetParent();

  wxPanel *container = new wxPanel(parent, -1);
  Title *titleWin = new Title(container,title);
  inner->Reparent(container);

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(titleWin,0,wxEXPAND);
  sizer->Add(inner,1,wxEXPAND);
  container->SetSizer(sizer);

  return container;
}
