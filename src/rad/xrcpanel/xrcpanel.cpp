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

#include "xrcpanel.h"
#include "rad/bitmaps.h"
#include "codegen/xrccg.h"
#include "utils/typeconv.h"
#include <wx/filename.h>
#include "rad/wxfbevent.h"
#include <rad/appdata.h>
#include "model/objectbase.h"
#include "utils/wxfbexception.h"

BEGIN_EVENT_TABLE( XrcPanel,  wxPanel )
EVT_FB_CODE_GENERATION( XrcPanel::OnCodeGeneration )
END_EVENT_TABLE()

XrcPanel::XrcPanel( wxWindow *parent, int id )
		: wxPanel ( parent, id )
{
	AppData()->AddHandler( this->GetEventHandler() );
	wxBoxSizer *top_sizer = new wxBoxSizer( wxVERTICAL );

	m_xrcPanel = new CodeEditor( this, -1 );
	InitStyledTextCtrl( m_xrcPanel->GetTextCtrl() );

	top_sizer->Add( m_xrcPanel, 1, wxEXPAND, 0 );

	SetSizer( top_sizer );
	SetAutoLayout( true );
	top_sizer->SetSizeHints( this );
	top_sizer->Fit( this );
	top_sizer->Layout();

	m_cw = PTCCodeWriter( new TCCodeWriter( m_xrcPanel->GetTextCtrl() ) );
}

XrcPanel::~XrcPanel()
{
	AppData()->RemoveHandler( this->GetEventHandler() );
}

void XrcPanel::InitStyledTextCtrl( wxScintilla *stc )
{
	stc->SetLexer( wxSCI_LEX_XML );

#ifdef __WXMSW__
	wxFont font( 10, wxMODERN, wxNORMAL, wxNORMAL );
#elif defined(__WXGTK__)
	// Debe haber un bug en wxGTK ya que la familia wxMODERN no es de ancho fijo.
	wxFont font( 12, wxMODERN, wxNORMAL, wxNORMAL );
	font.SetFaceName( wxT( "Monospace" ) );
#endif

	stc->StyleSetFont( wxSCI_STYLE_DEFAULT, font );
	stc->StyleClearAll();
	stc->StyleSetForeground( wxSCI_H_DOUBLESTRING, *wxRED );
	stc->StyleSetForeground( wxSCI_H_TAG, wxColour( 0, 0, 128 ) );
	stc->StyleSetForeground( wxSCI_H_ATTRIBUTE, wxColour( 128, 0, 128 ) );
	stc->SetUseTabs( false );
	stc->SetTabWidth( 4 );
	stc->SetTabIndents( true );
	stc->SetBackSpaceUnIndents( true );
	stc->SetIndent( 4 );
	stc->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	stc->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );

	stc->SetCaretWidth( 2 );
	stc->SetReadOnly( true );
}

void XrcPanel::OnCodeGeneration( wxFBEvent& event )
{

	wxScintilla* editor = m_xrcPanel->GetTextCtrl();
	editor->Freeze();
	editor->SetReadOnly( false );

	// Using the previously unused Id field in the event to carry a boolean
	bool panelOnly = ( event.GetId() != 0 );

	PObjectBase project = AppData()->GetProjectData();

	PProperty pCodeGen = project->GetProperty( wxT( "code_generation" ) );

	if ( pCodeGen )
	{
		if ( !TypeConv::FlagSet ( wxT( "XRC" ), pCodeGen->GetValue() ) )
		{
			editor->ClearAll();
			editor->SetReadOnly( true );
			editor->Thaw();
			return;
		}
	}

	// Vamos a generar el código en el panel
	{
		XrcCodeGenerator codegen;
		codegen.SetWriter( m_cw );
		codegen.GenerateCode( project );
		editor->SetReadOnly( true );
		editor->Thaw();
	}

	if ( panelOnly )
	{
		return;
	}

	// And now in the file.
	{
		XrcCodeGenerator codegen;

		wxString file, pathEntry;
		bool useRelativePath = false;

		// Determine if the path is absolute or relative
		PProperty pRelPath = project->GetProperty( wxT( "relative_path" ) );

		if ( pRelPath )
			useRelativePath = ( pRelPath->GetValueAsInteger() ? true : false );

		wxString path;

		try
		{
			// Get the output path
			path = AppData()->GetOutputPath();
		}
		catch ( wxFBException& ex )
		{
			wxLogWarning( ex.what() );
			return;
		}

		PProperty pfile = project->GetProperty( wxT( "file" ) );

		if ( pfile )
			file = pfile->GetValue();

		if ( file.empty() )
		{
			file = wxT( "wxfb_code" );
		}

		wxString filePath;

		filePath << path << file << wxT( ".xrc" );
		PCodeWriter cw( new FileCodeWriter( filePath ) );

		codegen.SetWriter( cw );
		codegen.GenerateCode( project );
		wxLogStatus( wxT( "Code generated on \'%s\'." ), path.c_str() );
	}
}
