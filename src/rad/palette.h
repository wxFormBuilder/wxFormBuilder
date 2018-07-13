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

#ifndef __PALETTE__
#define __PALETTE__

#include "../model/database.h"

#include <wx/aui/auibar.h>
#include <wx/aui/auibook.h>
#include <wx/spinbutt.h>

typedef std::vector<wxAuiToolBar*> ToolbarVector;

class wxFbPalette : public wxPanel
{
 private:
  ToolbarVector m_tv;
  wxAuiNotebook *m_notebook;
  static wxWindowID nextId;
  std::vector<int> m_posVector;

  void PopulateToolbar(PObjectPackage pkg, wxAuiToolBar *toolbar);

  DECLARE_EVENT_TABLE()

 public:
  wxFbPalette(wxWindow *parent,int id);
  ~wxFbPalette();

  /**
   * Crea la paleta, previamente se ha debido configurar el objeto
   * DataObservable.
   */
  void Create();
  void OnSpinUp(wxSpinEvent& e);
  void OnSpinDown(wxSpinEvent& e);
  void OnButtonClick(wxCommandEvent &event);
};
/*
class PaletteButton : public wxBitmapButton
{
 private:
  wxString m_name;
  DECLARE_EVENT_TABLE()

 public:
  PaletteButton(wxWindow *parent, const wxBitmap &bitmap, wxString &name);
  void OnButtonClick(wxCommandEvent &event);
};

*/
/*
class ToolPanel : public wxPanel, public DataObserver
{
 protected:
   DECLARE_EVENT_TABLE()

 public:
   ToolPanel(wxWindow *parent, int id);
   void OnSaveFile(wxCommandEvent &event);

};  */

/*
class PaletteButtonEventHandler : public wxEvtHandler
{
 private:
  wxString m_name;
  DECLARE_EVENT_TABLE()
  DataObservable *m_data;

 public:
  PaletteButtonEventHandler(wxString name, DataObservable *data);
  void OnButtonClick(wxCommandEvent &event);
};
*/
#endif //__PALETTE__
