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

#ifndef __MAIN_FRAME__
#define __MAIN_FRAME__

#include "wx/splitter.h"
#include "wx/wx.h"

#include <wx/aui/auibook.h>
#include <wx/fdrepdlg.h>

class wxFBEvent;
class wxFBObjectEvent;
class wxFBPropertyEvent;
class wxFBEventHandlerEvent;

class CppPanel;
class PythonPanel;
class LuaPanel;
class PHPPanel;
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
  wxSplitterWindow *m_leftSplitter;
  wxSplitterWindow *m_rightSplitter;
  int m_leftSplitterWidth;
  int m_rightSplitterWidth;
  wxString m_rightSplitterType;

  //wxFrameManager m_mgr;
  wxAuiNotebook *m_notebook;
  wxFbPalette *m_palette;
  ObjectTree *m_objTree;
  ObjectInspector *m_objInsp;
  VisualEditor *m_visualEdit;
  CppPanel *m_cpp;
  PythonPanel *m_python;
  LuaPanel *m_lua;
  PHPPanel *m_php;
  XrcPanel *m_xrc;
  int m_style;

  // Save which page is selected
  int m_page_selection;

  // Save right splitter's sash position
  int m_rightSplitter_sash_pos;

  // Automatically update sash in splitter window base on user action
  bool m_autoSash;

  wxString m_currentDir;
  wxString m_recentProjects[4];

  void UpdateFrame();

  // Actualiza los projectos más recientes en el menu
  void UpdateRecentProjects();
  void OnOpenRecent(wxCommandEvent &event);
  void UpdateLayoutTools();

  // Used to correctly restore splitter position
  void OnIdle( wxIdleEvent& );

  wxFindReplaceData m_findData;
  wxFindReplaceDialog* m_findDialog;

  // Used to force propgrid to save on lost focus
  wxEvtHandler* m_focusKillEvtHandler;

  DECLARE_EVENT_TABLE()
 public:
  MainFrame(wxWindow *parent, int id = wxID_ANY, int style = wxFB_DEFAULT_GUI, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 1000, 800 ) );
	~MainFrame() override;
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
  void OnClipboardCopy(wxCommandEvent& e);
  void OnClipboardPaste(wxCommandEvent& e);
  void OnClipboardPasteUpdateUI( wxUpdateUIEvent& e );
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
  void OnWindowSwap(wxCommandEvent& e);

  void OnAuiNotebookPageChanged( wxAuiNotebookEvent& event );

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

  void OnSplitterChanged( wxSplitterEvent &event );

  void InsertRecentProject(const wxString &file);

  wxWindow  *CreateComponentPalette (wxWindow *parent);
  wxWindow  *CreateDesignerWindow   (wxWindow *parent);
  wxWindow  *CreateObjectTree       (wxWindow *parent);
  wxWindow  *CreateObjectInspector  (wxWindow *parent);
  wxMenuBar *CreateFBMenuBar();
  wxToolBar *CreateFBToolBar();

  void CreateWideGui();
  void CreateClassicGui();

  void OnFindDialog( wxCommandEvent& event );
  void OnFind( wxFindDialogEvent& event );
  void OnFindClose( wxFindDialogEvent& event );

  bool SaveWarning();
  };


#endif //__MAIN_FRAME__
