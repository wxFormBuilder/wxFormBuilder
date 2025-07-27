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
#include <wx/toolbar.h>
#include <wx/frame.h>
#include <wx/button.h>
#include <wx/bmpbuttn.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/choice.h>
#include <wx/listbox.h>
#include <wx/listctrl.h>
#include <wx/checkbox.h>
#include <wx/radiobox.h>
#include <wx/slider.h>
#include <wx/grid.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/calctrl.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/html/htmlwin.h>
#include <wx/treectrl.h>
#include <wx/radiobut.h>
#include <wx/tglbtn.h>
#include <wx/scrolbar.h>
#include <wx/spinctrl.h>
#include <wx/spinbutt.h>
#include <wx/splitter.h>
#include <wx/checklst.h>
#include <wx/notebook.h>
#include <wx/listbook.h>
#include <wx/choicebk.h>
#include <wx/stattext.h>
#include <wx/scrolwin.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame
{
	private:

	protected:
		enum
		{
			ID_FILE_OPEN = 6000,
			ID_FILE_SAVE,
			ID_FILE_EXIT,
			ID_HELP_ABOUT_MENU_ITEM,
			ID_TOOLBAR,
			ID_OPEN_TOOL,
			ID_SAVE_TOOL,
			ID_ABOUT_TOOL,
		};

		wxMenuBar* m_menubar1;
		wxMenu* m_fileMenu;
		wxMenu* m_helpMenu;
		wxToolBar* m_toolBar1;
		wxToolBarToolBase* m_openTool;
		wxToolBarToolBase* m_saveTool;
		wxToolBarToolBase* m_aboutTool;

		// Virtual event handlers, override them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnSize( wxSizeEvent& event ) { event.Skip(); }
		virtual void OnFileOpen( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFileSave( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFileExit( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOpenSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOpenEntered( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAboutClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAboutRClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MainFrame();

};

///////////////////////////////////////////////////////////////////////////////
/// Class CommonPanel
///////////////////////////////////////////////////////////////////////////////
class CommonPanel : public wxPanel
{
	private:

	protected:
		enum
		{
			ID_DEFAULT = wxID_ANY, // Default
			ID_BUTTON = 6000,
			ID_BITMAP_BUTTON,
			ID_TEXTCTRL,
			ID_COMBOBOX,
			ID_CHOICE,
			ID_LISTBOX,
			ID_LIST_CTRL,
			ID_CHECKBOX,
			ID_RADIOBOX,
			ID_SLIDER,
			ID_GRID,
		};

		wxButton* m_button1;
		wxBitmapButton* m_bpButton1;
		wxTextCtrl* m_textCtrl1;
		wxComboBox* m_comboBox1;
		wxChoice* m_choice1;
		wxListBox* m_listBox1;
		wxListCtrl* m_listCtrl1;
		wxCheckBox* m_checkBox1;
		wxRadioBox* m_radioBox1;
		wxSlider* m_slider1;
		wxGrid* m_grid1;

		// Virtual event handlers, override them in your derived class
		virtual void OnClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickBitmap( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTextMaxLen( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTextURL( wxTextUrlEvent& event ) { event.Skip(); }
		virtual void OnCombobox( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnComboText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnComboTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnListBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnListBoxDClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnListColClick( wxListEvent& event ) { event.Skip(); }
		virtual void OnListDeleteAllItems( wxListEvent& event ) { event.Skip(); }
		virtual void OnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRadioBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCommandScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnCommandScrollBottom( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnCommandScrollChanged( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnScrollChanged( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnGridCellClick( wxGridEvent& event ) { event.Skip(); }
		virtual void OnGridCellDClick( wxGridEvent& event ) { event.Skip(); }
		virtual void OnGridCellChanged( wxGridEvent& event ) { event.Skip(); }
		virtual void OnGridCellRightClick( wxGridEvent& event ) { event.Skip(); }
		virtual void OnGridCellRightDClick( wxGridEvent& event ) { event.Skip(); }
		virtual void OnGridEditorCreated( wxGridEditorCreatedEvent& event ) { event.Skip(); }
		virtual void OnGridEditorHidden( wxGridEvent& event ) { event.Skip(); }
		virtual void OnGridLabelRightClick( wxGridEvent& event ) { event.Skip(); }
		virtual void OnGridLabelRightDClick( wxGridEvent& event ) { event.Skip(); }
		virtual void OnGridRowSize( wxGridSizeEvent& event ) { event.Skip(); }
		virtual void OnGridColSize( wxGridSizeEvent& event ) { event.Skip(); }
		virtual void OnGridEditorShown( wxGridEvent& event ) { event.Skip(); }
		virtual void OnGridLabelClick( wxGridEvent& event ) { event.Skip(); }
		virtual void OnGridLabelDClick( wxGridEvent& event ) { event.Skip(); }
		virtual void OnGridRangeSelect( wxGridRangeSelectEvent& event ) { event.Skip(); }
		virtual void OnGridCellSelected( wxGridEvent& event ) { event.Skip(); }


	public:

		CommonPanel( wxWindow* parent, wxWindowID id = ID_DEFAULT, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 420,382 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~CommonPanel();

};

///////////////////////////////////////////////////////////////////////////////
/// Class AdditionalPanel
///////////////////////////////////////////////////////////////////////////////
class AdditionalPanel : public wxPanel
{
	private:

	protected:
		wxCalendarCtrl* m_calendar1;
		wxDatePickerCtrl* m_datePicker1;
		wxHtmlWindow* m_htmlWin1;
		wxTreeCtrl* m_treeCtrl1;
		wxRadioButton* m_radioBtn1;
		wxToggleButton* m_toggleBtn1;
		wxScrollBar* m_scrollBar1;
		wxSpinCtrl* m_spinCtrl1;
		wxSpinButton* m_spinBtn1;
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel5;
		wxButton* m_button4;
		wxPanel* m_panel6;
		wxButton* m_button3;
		wxCheckListBox* m_checkList1;
		wxNotebook* m_notebook1;
		wxPanel* m_panel7;
		wxPanel* m_panel8;
		wxPanel* m_panel9;
		wxListbook* m_listbook1;
		wxPanel* m_panel2;
		wxTextCtrl* m_textCtrl2;
		wxChoicebook* m_choicebook1;
		wxPanel* m_panel3;
		wxListBox* m_listBox2;
		wxStaticText* m_staticText1;
		wxScrolledWindow* m_scrolledWindow2;

		// Virtual event handlers, override them in your derived class
		virtual void OnCheckListBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckListBoxDClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckListBoxToggle( wxCommandEvent& event ) { event.Skip(); }


	public:

		AdditionalPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 447,514 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~AdditionalPanel();

		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 85 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( AdditionalPanel::m_splitter1OnIdle ), NULL, this );
		}

};

