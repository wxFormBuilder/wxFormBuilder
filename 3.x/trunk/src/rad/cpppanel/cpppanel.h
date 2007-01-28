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

#ifndef __CPP_PANEL__
#define __CPP_PANEL__

#include <wx/wx.h>
#include <wx/wxScintilla/wxscintilla.h>

#include "codegen/codegen.h"
#include "codegen/cppcg.h"
#include "wx/file.h"
#include <wx/wxFlatNotebook/wxFlatNotebook.h>

#include <vector>
#include <boost/smart_ptr.hpp>

class TCCodeWriter : public CodeWriter
{
 private:
  wxScintilla *m_tc;
 protected:
  void DoWrite(wxString code);

 public:
  TCCodeWriter();
  TCCodeWriter(wxScintilla *tc);

  void SetTextCtrl (wxScintilla *tc) { m_tc = tc; }
  void Clear();
};

class FileCodeWriter : public CodeWriter
{
 private:
  wxFile m_file;
  wxString m_filename;
  bool m_useMicrosoftBOM;

 protected:
  void DoWrite(wxString code);

 public:
  FileCodeWriter( const wxString &file, bool useMicrosoftBOM = false );
  void Clear();
};

class CodeEditor : public wxPanel
{
 private:
  wxScintilla *m_code;
  void OnMarginClick ( wxScintillaEvent &event);
  DECLARE_EVENT_TABLE()
 public:
  CodeEditor(wxWindow *parent, int id);
  wxScintilla *GetTextCtrl() { return m_code; }
};

class wxFBEvent;

class CppPanel : public wxPanel
{
 private:
  CodeEditor *m_cppPanel;
  CodeEditor *m_hPanel;
  PTCCodeWriter m_hCW;
  PTCCodeWriter m_cppCW;
  wxFlatNotebookImageList m_icons;

  void InitStyledTextCtrl(wxScintilla *stc);

 public:
  CppPanel(wxWindow *parent, int id);
  ~CppPanel();

  void OnCodeGeneration( wxFBEvent& event );

  DECLARE_EVENT_TABLE()
};


class CppToolBar : public wxPanel
{
 public:
   CppToolBar(wxWindow *parent, int id);
};



#endif //__CPP_PANEL__
