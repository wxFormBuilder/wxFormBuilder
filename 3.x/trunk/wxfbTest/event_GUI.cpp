///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "event_GUI.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( MainFrame, wxFrame )
	EVT_CLOSE( MainFrame::_wxFB_OnClose )
	EVT_SIZE( MainFrame::_wxFB_OnSize )
	EVT_MENU( ID_FILE_OPEN, MainFrame::_wxFB_OnFileOpen )
	EVT_MENU( ID_FILE_SAVE, MainFrame::_wxFB_OnFileSave )
	EVT_MENU( ID_FILE_EXIT, MainFrame::_wxFB_OnFileExit )
	EVT_MENU( ID_OPEN_TOOL, MainFrame::_wxFB_OnOpenSelected )
	EVT_TOOL_ENTER(ID_TOOLBAR, MainFrame::_wxFB_OnOpenEntered )
	EVT_TOOL( ID_ABOUT_TOOL, MainFrame::_wxFB_OnAboutClicked )
	EVT_TOOL_RCLICKED( ID_ABOUT_TOOL, MainFrame::_wxFB_OnAboutRClick )
END_EVENT_TABLE()

MainFrame::MainFrame( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	m_menubar1 = new wxMenuBar( 0 );
	m_fileMenu = new wxMenu();
	wxMenuItem* fileOpenMenuItem = new wxMenuItem( m_fileMenu, ID_FILE_OPEN, wxString( wxT("&Open") ) , wxEmptyString, wxITEM_NORMAL );
	m_fileMenu->Append( fileOpenMenuItem );
	wxMenuItem* fileSaveMenuItem = new wxMenuItem( m_fileMenu, ID_FILE_SAVE, wxString( wxT("&Save") ) , wxEmptyString, wxITEM_NORMAL );
	m_fileMenu->Append( fileSaveMenuItem );
	
	m_fileMenu->AppendSeparator();
	wxMenuItem* fileExitMenuItem = new wxMenuItem( m_fileMenu, ID_FILE_EXIT, wxString( wxT("E&xit") ) , wxEmptyString, wxITEM_NORMAL );
	m_fileMenu->Append( fileExitMenuItem );
	m_menubar1->Append( m_fileMenu, wxT("&File") );
	
	m_helpMenu = new wxMenu();
	wxMenuItem* helpAboutMenuItem = new wxMenuItem( m_helpMenu, ID_HELP_ABOUT_MENU_ITEM, wxString( wxT("&About") ) , wxEmptyString, wxITEM_NORMAL );
	m_helpMenu->Append( helpAboutMenuItem );
	m_menubar1->Append( m_helpMenu, wxT("Help") );
	
	this->SetMenuBar( m_menubar1 );
	
	m_toolBar1 = this->CreateToolBar( wxTB_FLAT|wxTB_HORIZONTAL|wxTB_TEXT, ID_TOOLBAR ); 
	m_toolBar1->AddTool( ID_OPEN_TOOL, wxT("Open"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar1->AddTool( ID_SAVE_TOOL, wxT("Save"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar1->AddSeparator();
	m_toolBar1->AddTool( ID_ABOUT_TOOL, wxT("About"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar1->Realize();
	
}
BEGIN_EVENT_TABLE( CommonPanel, wxPanel )
	EVT_BUTTON( ID_BUTTON, CommonPanel::_wxFB_OnClick )
	EVT_BUTTON( ID_BITMAP_BUTTON, CommonPanel::_wxFB_OnClickBitmap )
	EVT_TEXT( ID_TEXTCTRL, CommonPanel::_wxFB_OnText )
	EVT_TEXT_ENTER( ID_TEXTCTRL, CommonPanel::_wxFB_OnTextEnter )
	EVT_TEXT_MAXLEN( ID_TEXTCTRL, CommonPanel::_wxFB_OnTextMaxLen )
	EVT_TEXT_URL( ID_TEXTCTRL, CommonPanel::_wxFB_OnTextURL )
	EVT_COMBOBOX( ID_COMBOBOX, CommonPanel::_wxFB_OnCombobox )
	EVT_TEXT( ID_COMBOBOX, CommonPanel::_wxFB_OnComboText )
	EVT_TEXT_ENTER( ID_COMBOBOX, CommonPanel::_wxFB_OnComboTextEnter )
	EVT_CHOICE( ID_CHOICE, CommonPanel::_wxFB_OnChoice )
	EVT_LISTBOX( ID_LISTBOX, CommonPanel::_wxFB_OnListBox )
	EVT_LISTBOX_DCLICK( ID_LISTBOX, CommonPanel::_wxFB_OnListBoxDClick )
	EVT_LIST_COL_CLICK( ID_LIST_CTRL, CommonPanel::_wxFB_OnListColClick )
	EVT_LIST_DELETE_ALL_ITEMS( ID_LIST_CTRL, CommonPanel::_wxFB_OnListDeleteAllItems )
	EVT_CHECKBOX( ID_CHECKBOX, CommonPanel::_wxFB_OnCheckBox )
	EVT_RADIOBOX( ID_RADIOBOX, CommonPanel::_wxFB_OnRadioBox )
	EVT_COMMAND_SCROLL( ID_SLIDER, CommonPanel::_wxFB_OnCommandScroll )
	EVT_COMMAND_SCROLL_BOTTOM( ID_SLIDER, CommonPanel::_wxFB_OnCommandScrollBottom )
	EVT_COMMAND_SCROLL_CHANGED( ID_SLIDER, CommonPanel::_wxFB_OnCommandScrollChanged )
	EVT_SCROLL( CommonPanel::_wxFB_OnScroll )
	EVT_SCROLL_CHANGED( CommonPanel::_wxFB_OnScrollChanged )
	EVT_GRID_CELL_LEFT_CLICK( CommonPanel::_wxFB_OnGridCellClick )
	EVT_GRID_CELL_LEFT_DCLICK( CommonPanel::_wxFB_OnGridCellDClick )
	EVT_GRID_CMD_CELL_CHANGE( ID_GRID, CommonPanel::_wxFB_OnGridCellChanged )
	EVT_GRID_CMD_CELL_RIGHT_CLICK( ID_GRID, CommonPanel::_wxFB_OnGridCellRightClick )
	EVT_GRID_CMD_CELL_RIGHT_DCLICK( ID_GRID, CommonPanel::_wxFB_OnGridCellRightDClick )
	EVT_GRID_CMD_EDITOR_CREATED( ID_GRID, CommonPanel::_wxFB_OnGridEditorCreated )
	EVT_GRID_CMD_EDITOR_HIDDEN( ID_GRID, CommonPanel::_wxFB_OnGridEditorHidden )
	EVT_GRID_CMD_LABEL_RIGHT_CLICK( ID_GRID, CommonPanel::_wxFB_OnGridLabelRightClick )
	EVT_GRID_CMD_LABEL_RIGHT_DCLICK( ID_GRID, CommonPanel::_wxFB_OnGridLabelRightDClick )
	EVT_GRID_CMD_ROW_SIZE( ID_GRID, CommonPanel::_wxFB_OnGridRowSize )
	EVT_GRID_COL_SIZE( CommonPanel::_wxFB_OnGridColSize )
	EVT_GRID_EDITOR_SHOWN( CommonPanel::_wxFB_OnGridEditorShown )
	EVT_GRID_LABEL_LEFT_CLICK( CommonPanel::_wxFB_OnGridLabelClick )
	EVT_GRID_LABEL_LEFT_DCLICK( CommonPanel::_wxFB_OnGridLabelDClick )
	EVT_GRID_RANGE_SELECT( CommonPanel::_wxFB_OnGridRangeSelect )
	EVT_GRID_SELECT_CELL( CommonPanel::_wxFB_OnGridCellSelected )
END_EVENT_TABLE()

CommonPanel::CommonPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
	
	m_button1 = new wxButton( this, ID_BUTTON, wxT("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_button1, 0, wxALL, 5 );
	
	m_bpButton1 = new wxBitmapButton( this, ID_BITMAP_BUTTON, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer1->Add( m_bpButton1, 0, wxALL, 5 );
	
	m_textCtrl1 = new wxTextCtrl( this, ID_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrl1, 0, wxALL, 5 );
	
	m_comboBox1 = new wxComboBox( this, ID_COMBOBOX, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizer1->Add( m_comboBox1, 0, wxALL, 5 );
	
	wxArrayString m_choice1Choices;
	m_choice1 = new wxChoice( this, ID_CHOICE, wxDefaultPosition, wxDefaultSize, m_choice1Choices, 0 );
	fgSizer1->Add( m_choice1, 0, wxALL, 5 );
	
	m_listBox1 = new wxListBox( this, ID_LISTBOX, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizer1->Add( m_listBox1, 0, wxALL, 5 );
	
	m_listCtrl1 = new wxListCtrl( this, ID_LIST_CTRL, wxDefaultPosition, wxDefaultSize, wxLC_ICON );
	fgSizer1->Add( m_listCtrl1, 0, wxALL, 5 );
	
	m_checkBox1 = new wxCheckBox( this, ID_CHECKBOX, wxT("Check Me!"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer1->Add( m_checkBox1, 0, wxALL, 5 );
	
	wxString m_radioBox1Choices[] = { wxT("Radio Button") };
	int m_radioBox1NChoices = sizeof( m_radioBox1Choices ) / sizeof( wxString );
	m_radioBox1 = new wxRadioBox( this, ID_RADIOBOX, wxT("wxRadioBox"), wxDefaultPosition, wxDefaultSize, m_radioBox1NChoices, m_radioBox1Choices, 1, wxRA_SPECIFY_COLS );
	fgSizer1->Add( m_radioBox1, 0, wxALL, 5 );
	
	m_slider1 = new wxSlider( this, ID_SLIDER, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	fgSizer1->Add( m_slider1, 0, wxALL, 5 );
	
	m_grid1 = new wxGrid( this, ID_GRID, wxDefaultPosition, wxDefaultSize, 0 );
	m_grid1->CreateGrid( 2, 2 );
	fgSizer1->Add( m_grid1, 0, wxALL, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
}
BEGIN_EVENT_TABLE( AdditionalPanel, wxPanel )
	EVT_LISTBOX( wxID_ANY, AdditionalPanel::_wxFB_OnCheckListBox )
	EVT_LISTBOX_DCLICK( wxID_ANY, AdditionalPanel::_wxFB_OnCheckListBoxDClick )
	EVT_CHECKLISTBOX( wxID_ANY, AdditionalPanel::_wxFB_OnCheckListBoxToggle )
END_EVENT_TABLE()

AdditionalPanel::AdditionalPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
	
	m_calendar1 = new wxCalendarCtrl( this, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxCAL_SHOW_HOLIDAYS);
	fgSizer2->Add( m_calendar1, 1, wxALL|wxEXPAND, 5 );
	
	m_datePicker1 = new wxDatePickerCtrl( this, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT);
	fgSizer2->Add( m_datePicker1, 1, wxALL, 5 );
	
	m_htmlWin1 = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO);
	fgSizer2->Add( m_htmlWin1, 1, wxALL|wxEXPAND, 5 );
	
	m_treeCtrl1 = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE);
	fgSizer2->Add( m_treeCtrl1, 1, wxALL|wxEXPAND, 5 );
	
	m_radioBtn1 = new wxRadioButton( this, wxID_ANY, wxT("RadioBtn"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer2->Add( m_radioBtn1, 0, wxALL, 5 );
	
	m_toggleBtn1 = new wxToggleButton( this, wxID_ANY, wxT("Toggle me!"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_toggleBtn1, 1, wxALL|wxEXPAND, 5 );
	
	m_scrollBar1 = new wxScrollBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
	fgSizer2->Add( m_scrollBar1, 0, wxALL, 5 );
	
	m_spinCtrl1 = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0);
	fgSizer2->Add( m_spinCtrl1, 0, wxALL, 5 );
	
	m_spinBtn1 = new wxSpinButton( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer2->Add( m_spinBtn1, 0, wxALL, 5 );
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D);
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( AdditionalPanel::m_splitter1OnIdle ), NULL, this );
	m_panel5 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_button4 = new wxButton( m_panel5, wxID_ANY, wxT("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_button4, 0, wxALL, 5 );
	
	m_panel5->SetSizer( bSizer1 );
	m_panel5->Layout();
	bSizer1->Fit( m_panel5 );
	m_panel6 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_button3 = new wxButton( m_panel6, wxID_ANY, wxT("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button3, 0, wxALL, 5 );
	
	m_panel6->SetSizer( bSizer2 );
	m_panel6->Layout();
	bSizer2->Fit( m_panel6 );
	m_splitter1->SplitVertically( m_panel5, m_panel6, 85 );
	fgSizer2->Add( m_splitter1, 1, wxEXPAND, 5 );
	
	wxString m_checkList1Choices[] = { wxT("1"), wxT("2"), wxT("3") };
	int m_checkList1NChoices = sizeof( m_checkList1Choices ) / sizeof( wxString );
	m_checkList1 = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkList1NChoices, m_checkList1Choices, 0 );
	fgSizer2->Add( m_checkList1, 0, wxALL|wxEXPAND, 5 );
	
	m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel7 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook1->AddPage( m_panel7, wxT("a page"), true );
	m_panel8 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook1->AddPage( m_panel8, wxT("a page"), false );
	m_panel9 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook1->AddPage( m_panel9, wxT("a page"), false );
	
	fgSizer2->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );
	
	m_listbook1 = new wxListbook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT );
	m_panel2 = new wxPanel( m_listbook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl2 = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_textCtrl2, 0, wxALL|wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer4 );
	m_panel2->Layout();
	bSizer4->Fit( m_panel2 );
	m_listbook1->AddPage( m_panel2, wxT("a page"), false );
	#ifndef __WXGTK__ // Small icon style not supported in GTK
	wxListView* m_listbook1ListView = m_listbook1->GetListView();
	long m_listbook1Flags = m_listbook1ListView->GetWindowStyleFlag();
	m_listbook1Flags = ( m_listbook1Flags & ~wxLC_ICON ) | wxLC_SMALL_ICON;
	m_listbook1ListView->SetWindowStyleFlag( m_listbook1Flags );
	#endif
	
	fgSizer2->Add( m_listbook1, 1, wxEXPAND | wxALL, 5 );
	
	m_choicebook1 = new wxChoicebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT );
	m_panel3 = new wxPanel( m_choicebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_listBox2 = new wxListBox( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer3->Add( m_listBox2, 0, wxALL, 5 );
	
	m_panel3->SetSizer( bSizer3 );
	m_panel3->Layout();
	bSizer3->Fit( m_panel3 );
	m_choicebook1->AddPage( m_panel3, wxT("a page"), false );
	fgSizer2->Add( m_choicebook1, 1, wxEXPAND | wxALL, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_staticText1, 0, wxALL, 5 );
	
	m_scrolledWindow2 = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow2->SetScrollRate( 5, 5 );
	fgSizer2->Add( m_scrolledWindow2, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( fgSizer2 );
	this->Layout();
}
