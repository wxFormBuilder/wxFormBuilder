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
// C++ code generated with wxFormBuilder (version Sep  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "event_GUI.h"

///////////////////////////////////////////////////////////////////////////

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
	
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnClose ) );
	this->Connect( wxEVT_SIZE, wxSizeEventHandler( MainFrame::OnSize ) );
	this->Connect( fileOpenMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnFileOpen ) );
	this->Connect( fileSaveMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnFileSave ) );
	this->Connect( fileExitMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnFileExit ) );
	this->Connect( ID_OPEN_TOOL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnOpenSelected ));
	this->Connect( ID_OPEN_TOOL, wxEVT_COMMAND_TOOL_ENTER, wxCommandEventHandler( MainFrame::OnOpenEntered ) );
	this->Connect( ID_ABOUT_TOOL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnAboutClicked ) );
	this->Connect( ID_ABOUT_TOOL, wxEVT_COMMAND_TOOL_RCLICKED, wxCommandEventHandler( MainFrame::OnAboutRClick ) );
}

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
	m_radioBox1->SetSelection( 0 );
	fgSizer1->Add( m_radioBox1, 0, wxALL, 5 );
	
	m_slider1 = new wxSlider( this, ID_SLIDER, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	fgSizer1->Add( m_slider1, 0, wxALL, 5 );
	
	m_grid1 = new wxGrid( this, ID_GRID, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	m_grid1->CreateGrid( 2, 2 );
	m_grid1->EnableEditing( true );
	m_grid1->EnableGridLines( true );
	m_grid1->EnableDragGridSize( false );
	m_grid1->SetMargins( 0, 0 );
	
	// Columns
	m_grid1->EnableDragColMove( false );
	m_grid1->EnableDragColSize( true );
	m_grid1->SetColLabelSize( 30 );
	m_grid1->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_grid1->EnableDragRowSize( true );
	m_grid1->SetRowLabelSize( 80 );
	m_grid1->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_grid1->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	fgSizer1->Add( m_grid1, 0, wxALL, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
	
	// Connect Events
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CommonPanel::OnClick ), NULL, this );
	m_bpButton1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CommonPanel::OnClickBitmap ), NULL, this );
	m_textCtrl1->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CommonPanel::OnText ), NULL, this );
	m_textCtrl1->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( CommonPanel::OnTextEnter ), NULL, this );
	m_textCtrl1->Connect( wxEVT_COMMAND_TEXT_MAXLEN, wxCommandEventHandler( CommonPanel::OnTextMaxLen ), NULL, this );
	m_textCtrl1->Connect( wxEVT_COMMAND_TEXT_URL, wxTextUrlEventHandler( CommonPanel::OnTextURL ), NULL, this );
	m_comboBox1->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( CommonPanel::OnCombobox ), NULL, this );
	m_comboBox1->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CommonPanel::OnComboText ), NULL, this );
	m_comboBox1->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( CommonPanel::OnComboTextEnter ), NULL, this );
	m_choice1->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CommonPanel::OnChoice ), NULL, this );
	m_listBox1->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( CommonPanel::OnListBox ), NULL, this );
	m_listBox1->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( CommonPanel::OnListBoxDClick ), NULL, this );
	m_listCtrl1->Connect( wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler( CommonPanel::OnListColClick ), NULL, this );
	m_listCtrl1->Connect( wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, wxListEventHandler( CommonPanel::OnListDeleteAllItems ), NULL, this );
	m_checkBox1->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CommonPanel::OnCheckBox ), NULL, this );
	m_radioBox1->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( CommonPanel::OnRadioBox ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( CommonPanel::OnCommandScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( CommonPanel::OnCommandScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( CommonPanel::OnCommandScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( CommonPanel::OnCommandScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( CommonPanel::OnCommandScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( CommonPanel::OnCommandScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( CommonPanel::OnCommandScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( CommonPanel::OnCommandScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( CommonPanel::OnCommandScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( CommonPanel::OnCommandScrollBottom ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( CommonPanel::OnCommandScrollChanged ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( CommonPanel::OnScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( CommonPanel::OnScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( CommonPanel::OnScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( CommonPanel::OnScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( CommonPanel::OnScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( CommonPanel::OnScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( CommonPanel::OnScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( CommonPanel::OnScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( CommonPanel::OnScroll ), NULL, this );
	m_slider1->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( CommonPanel::OnScrollChanged ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler( CommonPanel::OnGridCellClick ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_CELL_LEFT_DCLICK, wxGridEventHandler( CommonPanel::OnGridCellDClick ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( CommonPanel::OnGridCellChanged ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( CommonPanel::OnGridCellRightClick ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_CELL_RIGHT_DCLICK, wxGridEventHandler( CommonPanel::OnGridCellRightDClick ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_EDITOR_CREATED, wxGridEditorCreatedEventHandler( CommonPanel::OnGridEditorCreated ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_EDITOR_HIDDEN, wxGridEventHandler( CommonPanel::OnGridEditorHidden ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_LABEL_RIGHT_CLICK, wxGridEventHandler( CommonPanel::OnGridLabelRightClick ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_LABEL_RIGHT_DCLICK, wxGridEventHandler( CommonPanel::OnGridLabelRightDClick ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_ROW_SIZE, wxGridSizeEventHandler( CommonPanel::OnGridRowSize ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_COL_SIZE, wxGridSizeEventHandler( CommonPanel::OnGridColSize ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_EDITOR_SHOWN, wxGridEventHandler( CommonPanel::OnGridEditorShown ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_LABEL_LEFT_CLICK, wxGridEventHandler( CommonPanel::OnGridLabelClick ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_LABEL_LEFT_DCLICK, wxGridEventHandler( CommonPanel::OnGridLabelDClick ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_RANGE_SELECT, wxGridRangeSelectEventHandler( CommonPanel::OnGridRangeSelect ), NULL, this );
	m_grid1->Connect( wxEVT_GRID_SELECT_CELL, wxGridEventHandler( CommonPanel::OnGridCellSelected ), NULL, this );
}

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
	m_staticText1->Wrap( -1 );
	fgSizer2->Add( m_staticText1, 0, wxALL, 5 );
	
	m_scrolledWindow2 = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow2->SetScrollRate( 5, 5 );
	fgSizer2->Add( m_scrolledWindow2, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( fgSizer2 );
	this->Layout();
	
	// Connect Events
	m_checkList1->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( AdditionalPanel::OnCheckListBox ), NULL, this );
	m_checkList1->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( AdditionalPanel::OnCheckListBoxDClick ), NULL, this );
	m_checkList1->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( AdditionalPanel::OnCheckListBoxToggle ), NULL, this );
}
