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
// C++ code generated with wxFormBuilder (version May  4 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __wxfbTest_GUI__
#define __wxfbTest_GUI__

#include <wx/wx.h>

#include <wx/menu.h>
#include <wx/button.h>
#include <wx/bmpbuttn.h>
#include <wx/grid.h>
#include <wx/statbmp.h>
#include <wx/statline.h>
#include <wx/listctrl.h>
#include <wx/choice.h>
#include <wx/slider.h>
#include <wx/checklst.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default
#define ID_EXIT 1000
#define ID_ABOUT 1001

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame 
{
	private:
	
	protected:
		wxMenuBar* m_menubar1;
		wxMenu* file;
		wxMenu* help;
		wxStatusBar* m_statusBar1;
		wxToolBar* m_toolBar1;
	
	public:
		MainFrame( wxWindow* parent, int id = ID_DEFAULT, wxString title = wxT("wxFormBuilder Test App"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 500,300 ), int style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class MainPanel
///////////////////////////////////////////////////////////////////////////////
class MainPanel : public wxPanel 
{
	private:
	
	protected:
		wxButton* m_button1;
		wxBitmapButton* m_bpButton1;
		wxTextCtrl* m_textCtrl1;
		wxStaticText* m_staticText1;
		wxCheckBox* m_checkBox1;
		wxComboBox* m_comboBox1;
		wxListBox* m_listBox1;
		wxRadioBox* m_radioBox1;
		wxGrid* m_grid1;
		wxStaticBitmap* m_bitmap1;
		wxStaticText* m_staticText;
		wxStaticLine* m_staticline1;
		wxButton* m_button;
		wxListCtrl* m_listCtrl;
		wxChoice* m_choice1;
		wxSlider* m_slider1;
		wxCheckListBox* m_checkList1;
	
	public:
		MainPanel( wxWindow* parent, int id = ID_DEFAULT, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 800,600 ), int style = wxTAB_TRAVERSAL );
	
};

#endif //__wxfbTest_GUI__
