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

#include "about.h"

#include "appdata.h"

#include <wx/html/htmlwin.h>
#include <wx/mimetype.h>

#if 0
#define ID_OK 1000

BEGIN_EVENT_TABLE(AboutDialog,wxDialog)
  EVT_BUTTON(ID_OK,AboutDialog::OnButtonEvent)
END_EVENT_TABLE()
#endif

class HtmlWindow : public wxHtmlWindow
{
  public:
    HtmlWindow(wxWindow *parent) : wxHtmlWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
      wxHW_SCROLLBAR_NEVER | wxHW_NO_SELECTION | wxRAISED_BORDER)
    {
    }

    void LaunchBrowser(const wxString& url)
    {
      wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(wxT("html"));
      if (!ft) {
        wxLogError(wxT("Impossible to determine the file type for extension html.\nPlease edit your MIME types."));
        return;
      }

      wxString cmd;
      bool ok = ft->GetOpenCommand(&cmd, wxFileType::MessageParameters(url, wxT("")));
      delete ft;

      if (ok)
          wxExecute(cmd, wxEXEC_ASYNC);
    }

	void OnLinkClicked(const wxHtmlLinkInfo& link) override {
		::wxLaunchDefaultBrowser(link.GetHref());
	}
};

AboutDialog::AboutDialog(wxWindow *parent, int id) : wxDialog(parent,id,wxT("About..."))
{
#if 0
  wxBoxSizer *sizer2;
  sizer2 = new wxBoxSizer(wxVERTICAL);
  m_staticText2 = new wxStaticText(this,wxID_ANY,wxT("wxFormBuilder"),wxDefaultPosition,wxDefaultSize,0);
  m_staticText2->SetFont(wxFont(12,74,90,92,false,wxT("Arial")));
  sizer2->Add(m_staticText2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
  m_staticText3 = new wxStaticText(this,wxID_ANY,wxT("a RAD tool for wxWidgets framework"),wxDefaultPosition,wxDefaultSize,0);
  sizer2->Add(m_staticText3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
  m_staticText6 = new wxStaticText(this,wxID_ANY,wxT("(C) 2005 José Antonio Hurtado"),wxDefaultPosition,wxDefaultSize,0);
  sizer2->Add(m_staticText6, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
  window1 = new wxStaticLine(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxLI_HORIZONTAL);
  sizer2->Add(window1, 0, wxALL|wxEXPAND, 5);
  m_panel1 = new wxPanel(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER|wxTAB_TRAVERSAL);
  wxBoxSizer *sizer3;
  sizer3 = new wxBoxSizer(wxVERTICAL);
  m_staticText8 = new wxStaticText(m_panel1,wxID_ANY,wxT("Developed by:"),wxDefaultPosition,wxDefaultSize,0);
  sizer3->Add(m_staticText8, 0, wxALL, 5);
  m_staticText9 = new wxStaticText(m_panel1,wxID_ANY,wxT("- José Antonio Hurtado"),wxDefaultPosition,wxDefaultSize,0);
  sizer3->Add(m_staticText9, 0, wxALL, 5);
  m_staticText10 = new wxStaticText(m_panel1,wxID_ANY,wxT("- Juan Antonio Ortega"),wxDefaultPosition,wxDefaultSize,0);
  sizer3->Add(m_staticText10, 0, wxALL, 5);
  m_panel1->SetSizer(sizer3);
  m_panel1->SetAutoLayout(true);
  m_panel1->Layout();
  sizer2->Add(m_panel1, 1, wxALL|wxEXPAND, 5);
  window2 = new wxStaticLine(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxLI_HORIZONTAL);
  sizer2->Add(window2, 0, wxALL|wxEXPAND, 5);
  m_button1 = new wxButton(this,ID_OK,wxT("&OK"),wxDefaultPosition,wxDefaultSize,0);
  sizer2->Add(m_button1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
  this->SetSizer(sizer2);
  this->SetAutoLayout(true);
  this->Layout();
#endif

  wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
  wxHtmlWindow *htmlWin = new HtmlWindow(this);

  // I don't know where is the problem, but if you call SetBorders(b) with
  // 'b' between 0..6 it works, but if you use a bigger border, it doesn't fit
  // correctly.
  htmlWin->SetBorders(5);

  htmlWin->LoadFile(wxFileName(AppData()->GetApplicationPath() + wxFILE_SEP_PATH + wxT("resources/about.html")));

#ifdef __WXMAC__
  // work around a wxMac bug
  htmlWin->SetSize(450, 650);
#else
  htmlWin->SetMinSize(wxSize(450, 650));
#endif


  mainSizer->Add(htmlWin, 1, wxEXPAND | wxALL, 5);
  mainSizer->Add(new wxButton(this, wxID_OK, wxT("OK")), 0, wxALIGN_CENTER | wxBOTTOM, 5);

  SetSizerAndFit(mainSizer);
  Center();
}

void AboutDialog::OnButtonEvent (wxCommandEvent &)
{
  Close();
}
