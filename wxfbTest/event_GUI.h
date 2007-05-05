///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __event_GUI__
#define __event_GUI__

#include <wx/wx.h>

#include <wx/menu.h>
#include <wx/button.h>
#include <wx/bmpbuttn.h>
#include <wx/choice.h>
#include <wx/listctrl.h>
#include <wx/slider.h>
#include <wx/grid.h>
#include <wx/calctrl.h>
#include <wx/datectrl.h>
#include <wx/html/htmlwin.h>
#include <wx/treectrl.h>
#include <wx/radiobut.h>
#include <wx/tglbtn.h>
#include <wx/scrolbar.h>
#include <wx/spinctrl.h>
#include <wx/spinbutt.h>
#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/checklst.h>
#include <wx/notebook.h>
#include <wx/listbook.h>
#include <wx/choicebk.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnClose( wxCloseEvent& event ){ OnClose( event ); }
		void _wxFB_OnSize( wxSizeEvent& event ){ OnSize( event ); }
		void _wxFB_OnFileOpen( wxCommandEvent& event ){ OnFileOpen( event ); }
		void _wxFB_OnFileSave( wxCommandEvent& event ){ OnFileSave( event ); }
		void _wxFB_OnFileExit( wxCommandEvent& event ){ OnFileExit( event ); }
		void _wxFB_OnOpenSelected( wxCommandEvent& event ){ OnOpenSelected( event ); }
		void _wxFB_OnOpenEntered( wxCommandEvent& event ){ OnOpenEntered( event ); }
		void _wxFB_OnAboutClicked( wxCommandEvent& event ){ OnAboutClicked( event ); }
		void _wxFB_OnAboutRClick( wxCommandEvent& event ){ OnAboutRClick( event ); }
		
	
	protected:
		enum
		{
			ID_FILE_OPEN = 1000,
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
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ){ event.Skip(); }
		virtual void OnSize( wxSizeEvent& event ){ event.Skip(); }
		virtual void OnFileOpen( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnFileSave( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnFileExit( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnOpenSelected( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnOpenEntered( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAboutClicked( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAboutRClick( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		MainFrame( wxWindow* parent, int id = wxID_ANY, wxString title = wxEmptyString, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 500,300 ), int style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CommonPanel
///////////////////////////////////////////////////////////////////////////////
class CommonPanel : public wxPanel 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnClick( wxCommandEvent& event ){ OnClick( event ); }
		void _wxFB_OnClickBitmap( wxCommandEvent& event ){ OnClickBitmap( event ); }
		void _wxFB_OnText( wxCommandEvent& event ){ OnText( event ); }
		void _wxFB_OnTextEnter( wxCommandEvent& event ){ OnTextEnter( event ); }
		void _wxFB_OnTextMaxLen( wxCommandEvent& event ){ OnTextMaxLen( event ); }
		void _wxFB_OnTextURL( wxTextUrlEvent& event ){ OnTextURL( event ); }
		void _wxFB_OnCombobox( wxCommandEvent& event ){ OnCombobox( event ); }
		void _wxFB_OnComboText( wxCommandEvent& event ){ OnComboText( event ); }
		void _wxFB_OnComboTextEnter( wxCommandEvent& event ){ OnComboTextEnter( event ); }
		void _wxFB_OnChoice( wxCommandEvent& event ){ OnChoice( event ); }
		void _wxFB_OnListBox( wxCommandEvent& event ){ OnListBox( event ); }
		void _wxFB_OnListBoxDClick( wxCommandEvent& event ){ OnListBoxDClick( event ); }
		void _wxFB_OnListColClick( wxListEvent& event ){ OnListColClick( event ); }
		void _wxFB_OnListDeleteAllItems( wxListEvent& event ){ OnListDeleteAllItems( event ); }
		void _wxFB_OnCheckBox( wxCommandEvent& event ){ OnCheckBox( event ); }
		void _wxFB_OnRadioBox( wxCommandEvent& event ){ OnRadioBox( event ); }
		void _wxFB_OnCommandScroll( wxScrollEvent& event ){ OnCommandScroll( event ); }
		void _wxFB_OnCommandScrollBottom( wxScrollEvent& event ){ OnCommandScrollBottom( event ); }
		void _wxFB_OnCommandScrollChanged( wxScrollEvent& event ){ OnCommandScrollChanged( event ); }
		void _wxFB_OnScroll( wxScrollEvent& event ){ OnScroll( event ); }
		void _wxFB_OnScrollChanged( wxScrollEvent& event ){ OnScrollChanged( event ); }
		void _wxFB_OnGridCellClick( wxGridEvent& event ){ OnGridCellClick( event ); }
		void _wxFB_OnGridCellDClick( wxGridEvent& event ){ OnGridCellDClick( event ); }
		void _wxFB_OnGridCellChanged( wxGridEvent& event ){ OnGridCellChanged( event ); }
		void _wxFB_OnGridCellRightClick( wxGridEvent& event ){ OnGridCellRightClick( event ); }
		void _wxFB_OnGridCellRightDClick( wxGridEvent& event ){ OnGridCellRightDClick( event ); }
		void _wxFB_OnGridEditorCreated( wxGridEditorCreatedEvent& event ){ OnGridEditorCreated( event ); }
		void _wxFB_OnGridEditorHidden( wxGridEvent& event ){ OnGridEditorHidden( event ); }
		void _wxFB_OnGridLabelRightClick( wxGridEvent& event ){ OnGridLabelRightClick( event ); }
		void _wxFB_OnGridLabelRightDClick( wxGridEvent& event ){ OnGridLabelRightDClick( event ); }
		void _wxFB_OnGridRowSize( wxGridSizeEvent& event ){ OnGridRowSize( event ); }
		void _wxFB_OnGridColSize( wxGridSizeEvent& event ){ OnGridColSize( event ); }
		void _wxFB_OnGridEditorShown( wxGridEvent& event ){ OnGridEditorShown( event ); }
		void _wxFB_OnGridLabelClick( wxGridEvent& event ){ OnGridLabelClick( event ); }
		void _wxFB_OnGridLabelDClick( wxGridEvent& event ){ OnGridLabelDClick( event ); }
		void _wxFB_OnGridRangeSelect( wxGridRangeSelectEvent& event ){ OnGridRangeSelect( event ); }
		void _wxFB_OnGridCellSelected( wxGridEvent& event ){ OnGridCellSelected( event ); }
		
	
	protected:
		enum
		{
			ID_DEFAULT = wxID_ANY, // Default
			ID_BUTTON = 1000,
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
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnClickBitmap( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnText( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnTextEnter( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnTextMaxLen( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnTextURL( wxTextUrlEvent& event ){ event.Skip(); }
		virtual void OnCombobox( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnComboText( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnComboTextEnter( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnChoice( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnListBox( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnListBoxDClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnListColClick( wxListEvent& event ){ event.Skip(); }
		virtual void OnListDeleteAllItems( wxListEvent& event ){ event.Skip(); }
		virtual void OnCheckBox( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRadioBox( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCommandScroll( wxScrollEvent& event ){ event.Skip(); }
		virtual void OnCommandScrollBottom( wxScrollEvent& event ){ event.Skip(); }
		virtual void OnCommandScrollChanged( wxScrollEvent& event ){ event.Skip(); }
		virtual void OnScroll( wxScrollEvent& event ){ event.Skip(); }
		virtual void OnScrollChanged( wxScrollEvent& event ){ event.Skip(); }
		virtual void OnGridCellClick( wxGridEvent& event ){ event.Skip(); }
		virtual void OnGridCellDClick( wxGridEvent& event ){ event.Skip(); }
		virtual void OnGridCellChanged( wxGridEvent& event ){ event.Skip(); }
		virtual void OnGridCellRightClick( wxGridEvent& event ){ event.Skip(); }
		virtual void OnGridCellRightDClick( wxGridEvent& event ){ event.Skip(); }
		virtual void OnGridEditorCreated( wxGridEditorCreatedEvent& event ){ event.Skip(); }
		virtual void OnGridEditorHidden( wxGridEvent& event ){ event.Skip(); }
		virtual void OnGridLabelRightClick( wxGridEvent& event ){ event.Skip(); }
		virtual void OnGridLabelRightDClick( wxGridEvent& event ){ event.Skip(); }
		virtual void OnGridRowSize( wxGridSizeEvent& event ){ event.Skip(); }
		virtual void OnGridColSize( wxGridSizeEvent& event ){ event.Skip(); }
		virtual void OnGridEditorShown( wxGridEvent& event ){ event.Skip(); }
		virtual void OnGridLabelClick( wxGridEvent& event ){ event.Skip(); }
		virtual void OnGridLabelDClick( wxGridEvent& event ){ event.Skip(); }
		virtual void OnGridRangeSelect( wxGridRangeSelectEvent& event ){ event.Skip(); }
		virtual void OnGridCellSelected( wxGridEvent& event ){ event.Skip(); }
		
	
	public:
		CommonPanel( wxWindow* parent, int id = ID_DEFAULT, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 420,382 ), int style = wxTAB_TRAVERSAL );
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class AdditionalPanel
///////////////////////////////////////////////////////////////////////////////
class AdditionalPanel : public wxPanel 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnCheckListBox( wxCommandEvent& event ){ OnCheckListBox( event ); }
		void _wxFB_OnCheckListBoxDClick( wxCommandEvent& event ){ OnCheckListBoxDClick( event ); }
		void _wxFB_OnCheckListBoxToggle( wxCommandEvent& event ){ OnCheckListBoxToggle( event ); }
		
	
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
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCheckListBox( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCheckListBoxDClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCheckListBoxToggle( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		AdditionalPanel( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 447,514 ), int style = wxTAB_TRAVERSAL );
		void m_splitter1OnIdle( wxIdleEvent& )
		{
		m_splitter1->SetSashPosition( 85 );
		m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( AdditionalPanel::m_splitter1OnIdle ), NULL, this );
		}
		
	
};

#endif //__event_GUI__
