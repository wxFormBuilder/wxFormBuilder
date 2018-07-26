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

#ifndef __about__
#define __about__
#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/statline.h>

/**
 * Class AboutDialog
 */
class AboutDialog : public wxDialog
{
  private:
  
  protected:
    wxStaticText *m_staticText2;
    wxStaticText *m_staticText3;
    wxStaticText *m_staticText6;
    wxStaticLine *window1;
    wxPanel *m_panel1;
    wxStaticText *m_staticText8;
    wxStaticText *m_staticText9;
    wxStaticText *m_staticText10;
    wxStaticLine *window2;
    wxButton *m_button1;
    
	#if 0
	DECLARE_EVENT_TABLE()
	#endif
  
  public:
    
    AboutDialog(wxWindow *parent, int id = wxID_ANY);
    void OnButtonEvent (wxCommandEvent &event);
  
};

#endif //__about__

