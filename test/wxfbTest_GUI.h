///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-110-g61ece364)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statusbr.h>
#include <wx/toolbar.h>
#include <wx/frame.h>
#include <wx/button.h>
#include <wx/bmpbuttn.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/listbox.h>
#include <wx/radiobox.h>
#include <wx/grid.h>
#include <wx/statbox.h>
#include <wx/statbmp.h>
#include <wx/statline.h>
#include <wx/listctrl.h>
#include <wx/choice.h>
#include <wx/slider.h>
#include <wx/checklst.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default
#define ID_EXIT 6000
#define ID_ABOUT 6001

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
		wxToolBarToolBase* m_tool1;
		wxToolBarToolBase* m_tool2;

	public:

		MainFrame( wxWindow* parent, wxWindowID id = ID_DEFAULT, const wxString& title = wxT("wxFormBuilder Test App"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MainFrame();

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

		MainPanel( wxWindow* parent, wxWindowID id = ID_DEFAULT, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~MainPanel();

};

