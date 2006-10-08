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

#include "cpppanel.h"
#include "rad/bitmaps.h"
#include "utils/typeconv.h"

#include <wx/filename.h>

#include "rad/bitmaps.h"
#include "rad/wxfbevent.h"
#include <rad/appdata.h>

BEGIN_EVENT_TABLE ( CppPanel,  wxPanel )
	EVT_FB_CODE_GENERATION( CppPanel::OnCodeGeneration )
END_EVENT_TABLE()

CppPanel::CppPanel(wxWindow *parent, int id)
: wxPanel (parent,id)
{
	AppData()->AddHandler( this->GetEventHandler() );
	wxBoxSizer *top_sizer = new wxBoxSizer(wxVERTICAL);

	wxNotebookChooser* notebook = new ChooseNotebook( this, -1 );

	// Set notebook icons
	m_icons.Add( AppBitmaps::GetBitmap( wxT("cpp"), 16 ) );
	m_icons.Add( AppBitmaps::GetBitmap( wxT("h"), 16 ) );
	notebook->SetImageList( &m_icons );

	m_cppPanel = new CodeEditor(notebook,-1);
	InitStyledTextCtrl(m_cppPanel->GetTextCtrl());
	notebook->AddPage( m_cppPanel, wxT("cpp"), false, 0 );

	m_hPanel = new CodeEditor(notebook,-1);
	InitStyledTextCtrl(m_hPanel->GetTextCtrl());
	notebook->AddPage( m_hPanel, wxT("h"), false, 1 );

	top_sizer->Add(notebook,1,wxEXPAND,0);

	SetSizer(top_sizer);
	SetAutoLayout(true);
	top_sizer->SetSizeHints(this);
	top_sizer->Fit(this);
	top_sizer->Layout();

	m_hCW = PTCCodeWriter(new TCCodeWriter(m_hPanel->GetTextCtrl()));
	m_cppCW = PTCCodeWriter(new TCCodeWriter(m_cppPanel->GetTextCtrl()));
}

CppPanel::~CppPanel()
{
	AppData()->RemoveHandler( this->GetEventHandler() );
}

void CppPanel::InitStyledTextCtrl(wxScintilla *stc)
{
	stc->SetLexer(wxSCI_LEX_CPP);
	stc->SetKeyWords(0, wxT("asm auto bool break case catch char class const const_cast \
						   continue default delete do double dynamic_cast else enum explicit \
						   export extern false float for friend goto if inline int long \
						   mutable namespace new operator private protected public register \
						   reinterpret_cast return short signed sizeof static static_cast \
						   struct switch template this throw true try typedef typeid \
						   typename union unsigned using virtual void volatile wchar_t \
						   while"));

#ifdef __WXMSW__
	wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL);
#elif defined(__WXGTK__)
	// Debe haber un bug en wxGTK ya que la familia wxMODERN no es de ancho fijo.
	wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);
	font.SetFaceName(wxT("Monospace"));
#endif
	stc->StyleSetFont(wxSCI_STYLE_DEFAULT, font);
	stc->StyleClearAll();
	stc->StyleSetBold(wxSCI_C_WORD, true);
	stc->StyleSetForeground(wxSCI_C_WORD, *wxBLUE);
	stc->StyleSetForeground(wxSCI_C_STRING, *wxRED);
	stc->StyleSetForeground(wxSCI_C_STRINGEOL, *wxRED);
	stc->StyleSetForeground(wxSCI_C_PREPROCESSOR, wxColour(49, 106, 197));
	stc->StyleSetForeground(wxSCI_C_COMMENT, wxColour(0, 128, 0));
	stc->StyleSetForeground(wxSCI_C_COMMENTLINE, wxColour(0, 128, 0));
	stc->StyleSetForeground(wxSCI_C_COMMENTDOC, wxColour(0, 128, 0));
	stc->StyleSetForeground(wxSCI_C_COMMENTLINEDOC, wxColour(0, 128, 0));
	stc->StyleSetForeground(wxSCI_C_NUMBER, *wxBLUE );
	stc->SetUseTabs(true);
	stc->SetTabWidth(4);
	stc->SetTabIndents(true);
	stc->SetBackSpaceUnIndents(true);
	stc->SetIndent(4);
	stc->SetSelBackground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
	stc->SetSelForeground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));

	stc->SetCaretWidth(2);
}

void CppPanel::OnCodeGeneration( wxFBEvent& event )
{
	// Using the previously unused Id field in the event to carry a boolean
	bool panelOnly = ( event.GetId() != 0 );

	shared_ptr<ObjectBase> project = AppData()->GetProjectData();

	wxString file, pathEntry;
	wxFileName path;
	bool useRelativePath = false;
	unsigned int firstID = 1000;

	shared_ptr<Property> pCodeGen = project->GetProperty(wxT("code_generation"));
	if (pCodeGen)
	{
		if (!TypeConv::FlagSet  (wxT("C++"),pCodeGen->GetValue()))
			return;
	}

	// Get First ID from Project File
	shared_ptr<Property> pFirstID = project->GetProperty(wxT("first_id"));
	if (pFirstID)
		firstID = pFirstID->GetValueAsInteger();

	// Get the file name
	shared_ptr<Property> pfile = project->GetProperty(wxT("file"));
	if (pfile)
		file = pfile->GetValue();

	if (file == wxT(""))
		file = wxT("noname");

	// Determine if the path is absolute or relative
	shared_ptr<Property> pRelPath = project->GetProperty(wxT("relative_path"));
	if (pRelPath)
		useRelativePath = (pRelPath->GetValueAsInteger() ? true : false);


	// Get the output path
	shared_ptr<Property> ppath = project->GetProperty(wxT("path"));
	if (ppath)
	{
		pathEntry = ppath->GetValue();
		if ( pathEntry.empty() && !panelOnly )
		{
			wxLogWarning(wxT("You must set the \"path\" property of the project to a valid path for output files") );
			return;
		}
		if ( pathEntry.find_last_of( wxFILE_SEP_PATH ) != (pathEntry.length() - 1) )
		{
			pathEntry.append( wxFILE_SEP_PATH );
		}
		path = wxFileName( pathEntry );
		if ( !path.IsAbsolute() )
		{
			wxString projectPath = AppData()->GetProjectPath();
			if ( projectPath.empty() && !panelOnly )
			{
				wxLogWarning(wxT("You must save the project when using a relative path for output files") );
				return;
			}
			path.SetCwd( projectPath );
			path.MakeAbsolute();
		}
	}

	// Generate code in the panel
	{
		CppCodeGenerator codegen;
		//codegen.SetBasePath(ppath->GetValue());
		//codegen.SetRelativePath(useRelativePath);
		codegen.UseRelativePath(useRelativePath, path.GetPath() );

		if (pFirstID)
			codegen.SetFirstID( firstID );

		m_cppPanel->GetTextCtrl()->Freeze();
		m_hPanel->GetTextCtrl()->Freeze();

		codegen.SetHeaderWriter(m_hCW);
		codegen.SetSourceWriter(m_cppCW);

		codegen.GenerateCode(project);
		m_cppPanel->GetTextCtrl()->Thaw();
		m_hPanel->GetTextCtrl()->Thaw();
	}

	// If panelOnly, skip file code generation
	if ( panelOnly )
	{
		return;
	}

	// Generate code in the file
	{
		CppCodeGenerator codegen;
		codegen.UseRelativePath(useRelativePath, path.GetPath());

		if (pFirstID)
			codegen.SetFirstID( firstID );

		if ( path.DirExists() )
		{
				// Determin if Microsoft BOM should be used
			bool useMicrosoftBOM = false;
			shared_ptr< Property > pUseMicrosoftBOM = project->GetProperty( wxT( "use_microsoft_bom" ) );
			if ( pUseMicrosoftBOM )
			{
				useMicrosoftBOM = ( pUseMicrosoftBOM->GetValueAsInteger() != 0 );
			}

			shared_ptr<CodeWriter> h_cw( new FileCodeWriter( path.GetPath() + wxFILE_SEP_PATH + file + wxT(".h"), useMicrosoftBOM) );
			shared_ptr<CodeWriter> cpp_cw( new FileCodeWriter( path.GetPath() + wxFILE_SEP_PATH + file + wxT(".cpp"), useMicrosoftBOM) );

			codegen.SetHeaderWriter(h_cw);
			codegen.SetSourceWriter(cpp_cw);
			codegen.GenerateCode(project);
			wxLogStatus(wxT("Code generated on \'%s\'."),path.GetPath().c_str());
		}
		else
			wxLogWarning(wxT("Invalid Path: %s\nYou must set the \"path\" property of the project to a valid path for output files"), path.GetPath().c_str() );
	}
}

BEGIN_EVENT_TABLE (CodeEditor,  wxPanel )
	EVT_SCI_MARGINCLICK( -1, CodeEditor::OnMarginClick )
END_EVENT_TABLE()

CodeEditor::CodeEditor(wxWindow *parent, int id)
: wxPanel(parent,id)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	m_code = new wxScintilla(this, -1);

	// Line Numbers
	m_code->SetMarginType( 0, wxSCI_MARGIN_NUMBER );
	m_code->SetMarginWidth( 0, m_code->TextWidth (wxSCI_STYLE_LINENUMBER, wxT("_99999"))  );

	// markers
    m_code->MarkerDefine (wxSCI_MARKNUM_FOLDER, wxSCI_MARK_BOXPLUS);
    m_code->MarkerSetBackground (wxSCI_MARKNUM_FOLDER, wxColour (wxT("BLACK")));
    m_code->MarkerSetForeground (wxSCI_MARKNUM_FOLDER, wxColour (wxT("WHITE")));
    m_code->MarkerDefine (wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_BOXMINUS);
    m_code->MarkerSetBackground (wxSCI_MARKNUM_FOLDEROPEN, wxColour (wxT("BLACK")));
    m_code->MarkerSetForeground (wxSCI_MARKNUM_FOLDEROPEN, wxColour (wxT("WHITE")));
    m_code->MarkerDefine (wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_EMPTY);
    m_code->MarkerDefine (wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_BOXPLUS);
	m_code->MarkerSetBackground (wxSCI_MARKNUM_FOLDEREND, wxColour (wxT("BLACK")));
    m_code->MarkerSetForeground (wxSCI_MARKNUM_FOLDEREND, wxColour (wxT("WHITE")));
    m_code->MarkerDefine (wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_BOXMINUS);
	m_code->MarkerSetBackground (wxSCI_MARKNUM_FOLDEROPENMID, wxColour (wxT("BLACK")));
    m_code->MarkerSetForeground (wxSCI_MARKNUM_FOLDEROPENMID, wxColour (wxT("WHITE")));
    m_code->MarkerDefine (wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_EMPTY);
    m_code->MarkerDefine (wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_EMPTY);

	// folding
	m_code->SetMarginType (1, wxSCI_MARGIN_SYMBOL);
	m_code->SetMarginMask (1, wxSCI_MASK_FOLDERS);
	m_code->SetMarginWidth (1, 16);
	m_code->SetMarginSensitive (1, true);

	m_code->SetProperty( wxT("fold"),					wxT("1") );
	m_code->SetProperty( wxT("fold.comment"),			wxT("1") );
	m_code->SetProperty( wxT("fold.compact"),			wxT("1") );
	m_code->SetProperty( wxT("fold.preprocessor"),		wxT("1") );
	m_code->SetProperty( wxT("fold.html"),				wxT("1") );
	m_code->SetProperty( wxT("fold.html.preprocessor"),	wxT("1") );
	m_code->SetFoldFlags( wxSCI_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSCI_FOLDFLAG_LINEAFTER_CONTRACTED );

	m_code->SetIndentationGuides( true );

	m_code->SetMarginWidth( 2, 0 );
	wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL);
//	m_code->StyleSetFont(wxSCI_STYLE_DEFAULT, font);
	sizer->Add(m_code,1,wxEXPAND|wxALL);
	SetSizer(sizer);
	sizer->SetSizeHints(this);
}
void CodeEditor::OnMarginClick ( wxScintillaEvent &event)
{
	if (event.GetMargin() == 1) {
		int lineClick = m_code->LineFromPosition (event.GetPosition());
		int levelClick = m_code->GetFoldLevel (lineClick);
		if ((levelClick & wxSCI_FOLDLEVELHEADERFLAG) > 0)
		{
			m_code->ToggleFold (lineClick);
		}
	}
}
CppToolBar::CppToolBar(wxWindow *parent, int id)
: wxPanel(parent,id)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	wxBitmapButton *button;
	/*
	button = new wxBitmapButton(this,-1,AppBitmaps::GetBitmap(wxT("open")),
	wxDefaultPosition,wxSize(24,24));
	sizer->Add(button,0,0,0);*/

	button = new wxBitmapButton(this,-1,AppBitmaps::GetBitmap(wxT("save")),
		wxDefaultPosition,wxSize(24,24));
	sizer->Add(button,0,0,0);
	SetSizer(sizer);
	sizer->SetSizeHints(this);
}

TCCodeWriter::TCCodeWriter()
{
	m_tc = NULL;
}

TCCodeWriter::TCCodeWriter(wxScintilla *tc )
{
	SetTextCtrl(tc);
}

void TCCodeWriter::DoWrite(wxString code)
{
	if (m_tc)
		m_tc->AddText( code );
}

void TCCodeWriter::Clear()
{
	if (m_tc)
		m_tc->ClearAll(); //*!*
}


FileCodeWriter::FileCodeWriter( const wxString &file, bool useMicrosoftBOM )
:
m_filename(file),
m_useMicrosoftBOM(useMicrosoftBOM)
{
	Clear();
}

void FileCodeWriter::DoWrite(wxString code)
{
	wxString fixEOL = code;
	#if defined( __WXMSW__ )
		fixEOL.Replace( wxT("\n"), wxT("\r\n") );
	#elif defined( __WXMAC__ )
		fixEOL.Replace( wxT("\n"), wxT("\r") );
	#endif
	m_file.Write( fixEOL );
}

void FileCodeWriter::Clear()
{
	m_file.Create(m_filename,true);

	if ( m_useMicrosoftBOM )
	{
		unsigned char microsoftBOM[3] = { 0xEF, 0xBB, 0xBF };
		m_file.Write( microsoftBOM, 3 );
	}
}
