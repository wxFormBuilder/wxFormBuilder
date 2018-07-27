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

#include "title.h"

Title::Title(wxWindow *parent,const wxString &title) : wxPanel(parent, wxID_ANY)
{
  wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );

  wxStaticText *text = new wxStaticText(this, wxID_ANY,title);//,wxDefaultPosition,wxDefaultSize,wxSIMPLE_BORDER);
  SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );
  text->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );
  text->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );
	text->SetFont(
	  wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxString()));

  sizer->Add(text,0,wxALL | wxEXPAND,2);
  SetSizer(sizer);
  Fit();
}

wxWindow * Title::CreateTitle (wxWindow *inner, const wxString &title)
{
  wxWindow *parent = inner->GetParent();

  wxPanel *container = new wxPanel(parent, wxID_ANY);
  Title *titleWin = new Title(container,title);
  inner->Reparent(container);

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(titleWin,0,wxEXPAND);
  sizer->Add(inner,1,wxEXPAND);
  container->SetSizer(sizer);

  return container;
}
