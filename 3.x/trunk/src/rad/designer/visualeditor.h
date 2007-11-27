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

#ifndef __VISUAL_EDITOR__
#define __VISUAL_EDITOR__


#include "wx/wx.h"
#include "utils/wxfbdefs.h"
#include "rad/designer/visualobj.h"
//#include "rad/designer/resizablepanel.h"
#include <wx/sashwin.h>

#include "innerframe.h"

/**
 * Extends the wxInnerFrame to show the object highlight
 */
class DesignerWindow : public wxInnerFrame
{
 private:
   int m_x;
   int m_y;
   wxSizer *m_selSizer;
   wxObject *m_selItem;
   WPObjectBase m_selObj;
   wxWindow *m_actPanel;

   void DrawRectangle(wxDC& dc, const wxPoint& point, const wxSize& size, PObjectBase object);

   DECLARE_CLASS(DesignerWindow)

   // Augh!, this class is needed to paint the highlight in the
   // frame content panel.
   class HighlightPaintHandler : public wxEvtHandler
   {
      DECLARE_EVENT_TABLE()

      wxWindow *m_window;

     public:

       HighlightPaintHandler(wxWindow *win);
       void OnPaint(wxPaintEvent &event);
   };

 protected:

   DECLARE_EVENT_TABLE()

 public:
   DesignerWindow(wxWindow *parent, int id, const wxPoint& pos, const wxSize &size = wxDefaultSize,
     long style = 0, const wxString &name = wxT("designer_win"));
   ~DesignerWindow();
   void SetGrid(int x, int y);
   void SetSelectedSizer(wxSizer *sizer) { m_selSizer = sizer; }
   void SetSelectedItem(wxObject *item) { m_selItem = item; }
   void SetSelectedObject(PObjectBase object) { m_selObj = object; }
   void SetSelectedPanel(wxWindow *actPanel) { m_actPanel = actPanel; }
   wxSizer *GetSelectedSizer() { return m_selSizer; }
   wxObject* GetSelectedItem() { return m_selItem; }
   PObjectBase GetSelectedObject() { return m_selObj.lock(); }
   wxWindow* GetActivePanel() { return m_actPanel; }
   wxMenu* GetMenuFromObject(PObjectBase menu);
   void SetFrameWidgets(PObjectBase menubar, wxWindow *toolbar, wxWindow* statusbar);
   void HighlightSelection(wxDC& dc);
   void OnPaint(wxPaintEvent &event);
};

class wxFBEvent;
class wxFBPropertyEvent;
class wxFBObjectEvent;

class VisualEditor : public wxScrolledWindow
{
 private:
  typedef std::map< wxObject*, PObjectBase > wxObjectMap;
  wxObjectMap m_wxobjects;

  typedef std::map< ObjectBase*, wxObject* > ObjectBaseMap;
  ObjectBaseMap m_baseobjects;

  DesignerWindow *m_back;

  PObjectBase m_form;  // Pointer to last form created

  // Prevent OnSelected in components
  bool m_stopSelectedEvent;

  // Prevent OnModified in components
  bool m_stopModifiedEvent;

  DECLARE_EVENT_TABLE()

 protected:
  void Generate( PObjectBase obj, wxWindow* parent, wxObject* parentObject );
  void SetupWindow( PObjectBase obj, wxWindow* window );
  void SetupSizer( PObjectBase obj, wxSizer* sizer );
  void Create();
  void DeleteAbstractObjects();

 public:
  VisualEditor(wxWindow *parent);
  ~VisualEditor();
  void OnResizeBackPanel (wxCommandEvent &event);
  void OnClickBackPanel( wxMouseEvent& event );
  void PreventOnSelected( bool prevent = true );
  void PreventOnModified( bool prevent = true );

  void UpdateVirtualSize();

  PObjectBase GetObjectBase( wxObject* wxobject );
  wxObject* GetWxObject( PObjectBase baseobject );

  // Events
  void OnProjectLoaded ( wxFBEvent &event );
  void OnProjectSaved  ( wxFBEvent &event );
  void OnObjectSelected( wxFBObjectEvent &event );
  void OnObjectCreated ( wxFBObjectEvent &event );
  void OnObjectRemoved ( wxFBObjectEvent &event );
  void OnPropertyModified ( wxFBPropertyEvent &event );
  void OnProjectRefresh ( wxFBEvent &event);
};


#endif //__VISUAL_EDITOR__

