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

#ifndef __MAIN_FRAME__
#define __MAIN_FRAME__

#include "wx/wx.h"
#include <wx/wxFlatNotebook/wxFlatNotebook.h>
//#include "wx/aui/aui.h"
#include "wx/splitter.h"

class wxFBEvent;
class wxFBObjectEvent;
class wxFBPropertyEvent;
class wxFBEventHandlerEvent;

class CppPanel;
class XrcPanel;
class ObjectTree;
class ObjectInspector;
class wxFbPalette;
class VisualEditor;

/**
 * wxFormBuilder GUI styles.
 */
enum {
  wxFB_DEFAULT_GUI,
  wxFB_DOCKABLE_GUI,
	wxFB_CLASSIC_GUI,
	wxFB_WIDE_GUI
};

class MainFrame : public wxFrame
{
 private:
  #ifdef __WXFB_DEBUG__
  wxLog * m_old_log;
  wxLogWindow * m_log;
  #endif //__WXFB_DEBUG__

  wxSplitterWindow *m_leftSplitter;
  wxSplitterWindow *m_rightSplitter;
  int m_leftSplitterWidth;
  int m_rightSplitterWidth;

  //wxFrameManager m_mgr;
  wxFlatNotebook *m_notebook;
  wxFlatNotebookImageList m_icons;
  wxFbPalette *m_palette;
  ObjectTree *m_objTree;
  ObjectInspector *m_objInsp;
  VisualEditor *m_visualEdit;
  CppPanel *m_cpp;
  XrcPanel *m_xrc;
  int m_style;

  wxString m_currentDir;
  wxString m_recentProjects[4];

  void UpdateFrame();

  // Actualiza los projectos más recientes en el menu
  void UpdateRecentProjects();
  void OnOpenRecent(wxCommandEvent &event);
  void UpdateLayoutTools();
  bool SaveWarning();

  DECLARE_EVENT_TABLE()
 public:
  MainFrame(wxWindow *parent, int id = -1, int style = wxFB_DEFAULT_GUI, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 1000, 800 ) );
  ~MainFrame();
  void RestorePosition(const wxString &name);
  void SavePosition(const wxString &name);
  void OnSaveProject(wxCommandEvent &event);
  void OnSaveAsProject(wxCommandEvent &event);
  void OnOpenProject(wxCommandEvent &event);
  void OnNewProject(wxCommandEvent &event);
  void OnGenerateCode(wxCommandEvent &event);
  void OnAbout(wxCommandEvent &event);
  void OnExit(wxCommandEvent &event);
  void OnClose(wxCloseEvent &event);
  void OnImportXrc(wxCommandEvent &event);
  void OnUndo(wxCommandEvent &event);
  void OnRedo(wxCommandEvent &event);
  void OnCopy (wxCommandEvent &event);
  void OnPaste (wxCommandEvent &event);
  void OnCut (wxCommandEvent &event);
  void OnDelete (wxCommandEvent &event);
  void OnToggleExpand (wxCommandEvent &event);
  void OnToggleStretch (wxCommandEvent &event);
  void OnMoveUp (wxCommandEvent &event);
  void OnMoveDown (wxCommandEvent &event);
  void OnMoveLeft (wxCommandEvent &event);
  void OnMoveRight (wxCommandEvent &event);
  void OnChangeAlignment (wxCommandEvent &event);
  void OnChangeBorder(wxCommandEvent& e);
  void OnXrcPreview(wxCommandEvent& e);
  void OnGenInhertedClass(wxCommandEvent& e);
  void OnFlatNotebookPageChanged( wxFlatNotebookEvent& event );

  void OnProjectLoaded( wxFBEvent& event );
  void OnProjectSaved( wxFBEvent& event );
  void OnObjectExpanded( wxFBObjectEvent& event );
  void OnObjectSelected( wxFBObjectEvent& event );
  void OnObjectCreated( wxFBObjectEvent& event );
  void OnObjectRemoved( wxFBObjectEvent& event );
  void OnPropertyModified( wxFBPropertyEvent& event );
  void OnEventHandlerModified( wxFBEventHandlerEvent& event );
  void OnCodeGeneration( wxFBEvent& event );
  void OnProjectRefresh( wxFBEvent& event );

  void InsertRecentProject(const wxString &file);

  wxWindow  *CreateComponentPalette (wxWindow *parent);
  wxWindow  *CreateDesignerWindow   (wxWindow *parent);
  wxWindow  *CreateObjectTree       (wxWindow *parent);
  wxWindow  *CreateObjectInspector  (wxWindow *parent);
  wxMenuBar *CreateFBMenuBar();
  wxToolBar *CreateFBToolBar();

  void CreateWideGui();
  void CreateClassicGui();
};



#endif //__MAIN_FRAME__
