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
// Erlang code generation writen by
//   Micheus Vieira - micheus@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

#include "erlangpanel.h"

#include "../appdata.h"
#include "../codeeditor/codeeditor.h"
#include "../wxfbevent.h"

#include "../../utils/typeconv.h"
#include "../../utils/wxfbexception.h"

#include "../../model/objectbase.h"

#include "../../codegen/codewriter.h"
#include "../../codegen/erlangcg.h"

#include <wx/fdrepdlg.h>

#include <wx/stc/stc.h>

BEGIN_EVENT_TABLE ( ErlangPanel,  wxPanel )
	EVT_FB_CODE_GENERATION( ErlangPanel::OnCodeGeneration )
	EVT_FB_PROJECT_REFRESH( ErlangPanel::OnProjectRefresh )
	EVT_FB_PROPERTY_MODIFIED( ErlangPanel::OnPropertyModified )
	EVT_FB_OBJECT_CREATED( ErlangPanel::OnObjectChange )
	EVT_FB_OBJECT_REMOVED( ErlangPanel::OnObjectChange )
	EVT_FB_OBJECT_SELECTED( ErlangPanel::OnObjectChange )
	EVT_FB_EVENT_HANDLER_MODIFIED( ErlangPanel::OnEventHandlerModified )

	EVT_FIND( wxID_ANY, ErlangPanel::OnFind )
	EVT_FIND_NEXT( wxID_ANY, ErlangPanel::OnFind )
END_EVENT_TABLE()

ErlangPanel::ErlangPanel( wxWindow *parent, int id )
:
wxPanel( parent, id )
{
	AppData()->AddHandler( this->GetEventHandler() );
	wxBoxSizer *top_sizer = new wxBoxSizer( wxVERTICAL );

	m_erlangPanel = new CodeEditor( this, wxID_ANY);
	InitStyledTextCtrl( m_erlangPanel->GetTextCtrl() );

	top_sizer->Add( m_erlangPanel, 1, wxEXPAND, 0 );

	SetSizer( top_sizer );
	SetAutoLayout( true );
	top_sizer->Fit( this );
	top_sizer->Layout();

	m_erlangCW = PTCCodeWriter( new TCCodeWriter( m_erlangPanel->GetTextCtrl() ) );
}

ErlangPanel::~ErlangPanel()
{
	AppData()->RemoveHandler( this->GetEventHandler() );
}
void ErlangPanel::InitStyledTextCtrl( wxStyledTextCtrl *stc )
{
    stc->SetLexer( wxSTC_LEX_ERLANG );
	stc->SetKeyWords( 0, wxT( "and andalso band bnot bor bxor callback case create end \
	                          export false fun function if ignore import include_lib module \
	                          none new not of ok or orelse receive true try undefined xor" ) );

#ifdef __WXGTK__
	// Debe haber un bug en wxGTK ya que la familia wxMODERN no es de ancho fijo.
	wxFont font(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	font.SetFaceName( wxT( "Monospace" ) );
#else
	wxFont font(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#endif

	stc->StyleSetForeground(wxSTC_STYLE_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	stc->StyleSetBackground(wxSTC_STYLE_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    stc->StyleSetFont( wxSTC_STYLE_DEFAULT, font );
    stc->StyleClearAll();
    // wxSTC's reference: https://docs.wxwidgets.org/3.0.0/stc_8h.html
    stc->StyleSetBold( wxSTC_ERLANG_KEYWORD, true );
    stc->StyleSetBold( wxSTC_ERLANG_NUMBER, true );
    stc->StyleSetBold( wxSTC_ERLANG_PREPROC, true );
	if (!AppData()->IsDarkMode())
	{
		stc->StyleSetForeground(wxSTC_ERLANG_KEYWORD, wxColour( 130, 40, 172 ));
		stc->StyleSetForeground(wxSTC_ERLANG_STRING, wxColour( 170, 45, 132 ));
 		stc->StyleSetForeground(wxSTC_ERLANG_VARIABLE, wxColor( 150, 100, 40 ));
 		stc->StyleSetForeground(wxSTC_ERLANG_MACRO, wxColour( 40, 144, 170 ));
		stc->StyleSetForeground(wxSTC_ERLANG_PREPROC, wxColour( 49, 106, 197 ));
		stc->StyleSetForeground(wxSTC_ERLANG_COMMENT, wxColour( 160, 53, 35 ));
		stc->StyleSetForeground(wxSTC_ERLANG_COMMENT_MODULE, wxColour( 160, 53, 35 ));
		stc->StyleSetForeground(wxSTC_ERLANG_COMMENT_FUNCTION, wxColour( 160, 53, 35 ));
		stc->StyleSetForeground(wxSTC_ERLANG_COMMENT_DOC, wxColour( 160, 53, 35 ));
		stc->StyleSetForeground(wxSTC_ERLANG_NUMBER, wxColour( 5, 5, 100 ));
	}
	else
	{
		stc->StyleSetForeground(wxSTC_ERLANG_KEYWORD, wxColour( 221, 40, 103 ));
		stc->StyleSetForeground(wxSTC_ERLANG_STRING, wxColour( 23, 198, 163 ));
 		stc->StyleSetForeground(wxSTC_ERLANG_VARIABLE, wxColour( 0, 0, 186 ));
 		stc->StyleSetForeground(wxSTC_ERLANG_MACRO, wxColour( 0, 192, 0 ));
		stc->StyleSetForeground(wxSTC_ERLANG_PREPROC, wxColour( 204, 129, 186 ));
		stc->StyleSetForeground(wxSTC_ERLANG_COMMENT, wxColour( 98, 98, 98 ));
		stc->StyleSetForeground(wxSTC_ERLANG_COMMENT_MODULE, wxColour( 98, 98, 98 ));
		stc->StyleSetForeground(wxSTC_ERLANG_COMMENT_FUNCTION, wxColour( 98, 98, 98 ));
		stc->StyleSetForeground(wxSTC_ERLANG_COMMENT_DOC, wxColour( 98, 98, 98 ));
		stc->StyleSetForeground(wxSTC_ERLANG_NUMBER, wxColour( 104, 151, 187 ));
	}
	stc->SetUseTabs( false );
	stc->SetTabWidth( 4 );
	stc->SetTabIndents( true );
	stc->SetBackSpaceUnIndents( true );
	stc->SetIndent( 4 );
	stc->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	stc->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );

	stc->SetCaretForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	stc->SetCaretWidth( 2 );
	stc->SetReadOnly( true );
}

void ErlangPanel::OnFind( wxFindDialogEvent& event )
{
	m_erlangPanel->GetEventHandler()->ProcessEvent( event );
}

void ErlangPanel::OnPropertyModified( wxFBPropertyEvent& event )
{
	// Generate code to the panel only
	event.SetId( 1 );
	OnCodeGeneration( event );
}

void ErlangPanel::OnProjectRefresh( wxFBEvent& event )
{
	// Generate code to the panel only
	event.SetId( 1 );
	OnCodeGeneration( event );
}

void ErlangPanel::OnObjectChange( wxFBObjectEvent& event )
{
	// Generate code to the panel only
	event.SetId( 1 );
	OnCodeGeneration( event );
}

void ErlangPanel::OnEventHandlerModified( wxFBEventHandlerEvent& event )
{
	// Generate code to the panel only
	event.SetId( 1 );
	OnCodeGeneration( event );
}

void ErlangPanel::OnCodeGeneration( wxFBEvent& event )
{
    PObjectBase objectToGenerate;

	// Generate code in the panel if the panel is active
	bool doPanel = IsShown();

	// Using the previously unused Id field in the event to carry a boolean
	bool panelOnly = ( event.GetId() != 0 );

	// Only generate to panel + panel is not shown = do nothing
	if ( panelOnly && !doPanel )
	{
		return;
	}

	// For code preview generate only code relevant to selected form,
	// otherwise generate full project code.

	// Create copy of the original project due to possible temporary modifications
	PObjectBase project = PObjectBase(new ObjectBase(*AppData()->GetProjectData()));

	if ( panelOnly )
	{
	    objectToGenerate = AppData()->GetSelectedForm();
	}

	if ( !panelOnly || !objectToGenerate )
	{
	    objectToGenerate = project;
	}

	// If only one project item should be generated then remove the rest items
	// from the temporary project
	if ( doPanel && panelOnly && (objectToGenerate != project) )
	{
	    if ( project->GetChildCount() > 0 )
	    {
	        unsigned int i = 0;
            while( project->GetChildCount() > 1 )
            {
                if ( project->GetChild( i ) != objectToGenerate )
                {
                    project->RemoveChild( i );
                }
                else
                    i++;
            }
	    }
	}

    if ( !project || !objectToGenerate ) return;

    // Get Erlang properties from the project

	// If Erlang generation is not enabled, do not generate the file
	bool doFile = false;
	PProperty pCodeGen = project->GetProperty( wxT( "code_generation" ) );
	if ( pCodeGen )
	{
		doFile = TypeConv::FlagSet( wxT( "Erlang" ), pCodeGen->GetValue() ) && !panelOnly;
	}

	if ( !(doPanel || doFile ) )
	{
		return;
	}

	// Get First ID from Project File
	unsigned int firstID = 6000;	// (wxID_HIGHEST+1)
	PProperty pFirstID = project->GetProperty( wxT( "first_id" ) );
	if ( pFirstID )
	{
		firstID = pFirstID->GetValueAsInteger();
	}

	// Get the file name
	wxString file;
	PProperty pfile = project->GetProperty( wxT( "file" ) );
	if ( pfile )
	{
		file = pfile->GetValue();
	}
	if ( file.empty() )
	{
		file = wxT( "noname" );
	}

	// Determine if the path is absolute or relative
	bool useRelativePath = false;
	PProperty pRelPath = project->GetProperty( wxT( "relative_path" ) );
	if ( pRelPath )
	{
		useRelativePath = ( pRelPath->GetValueAsInteger() ? true : false );
	}

	// Get the output path
	wxString path;
	try
	{
		path = AppData()->GetOutputPath();
	}
	catch ( wxFBException& ex )
	{
		if ( doFile )
		{
			path = wxEmptyString;
			wxLogWarning( ex.what() );
			return;
		}
	}

	// Generate code in the panel
	if ( doPanel )
	{
		ErlangCodeGenerator codegen;
		codegen.UseRelativePath( useRelativePath, path );

		if ( pFirstID )
		{
			codegen.SetFirstID( firstID );
		}

		codegen.SetSourceWriter( m_erlangCW );

		Freeze();

        wxStyledTextCtrl* erlangEditor = m_erlangPanel->GetTextCtrl();
		erlangEditor->SetReadOnly( false );
		int erlangLine = erlangEditor->GetFirstVisibleLine() + erlangEditor->LinesOnScreen() - 1;
		int erlangXOffset = erlangEditor->GetXOffset();

		codegen.GenerateCode( project );

		erlangEditor->SetReadOnly( true );
		erlangEditor->GotoLine( erlangLine );
		erlangEditor->SetXOffset( erlangXOffset );
		erlangEditor->SetAnchor( 0 );
		erlangEditor->SetCurrentPos( 0 );

		Thaw();
	}

	// Generate code in the file
	if ( doFile )
	{
		try
		{
			ErlangCodeGenerator codegen;
			codegen.UseRelativePath( useRelativePath, path );

			if ( pFirstID )
			{
				codegen.SetFirstID( firstID );
			}

			// Determin if Microsoft BOM should be used
			bool useMicrosoftBOM = false;

			PProperty pUseMicrosoftBOM = project->GetProperty( wxT( "use_microsoft_bom" ) );

			if ( pUseMicrosoftBOM )
			{
				useMicrosoftBOM = ( pUseMicrosoftBOM->GetValueAsInteger() != 0 );
			}

			// Determine if Utf8 or Ansi is to be created
			bool useUtf8 = false;
			PProperty pUseUtf8 = project->GetProperty( _( "encoding" ) );

			if ( pUseUtf8 )
			{
				useUtf8 = ( pUseUtf8->GetValueAsString() != wxT( "ANSI" ) );
			}

			PCodeWriter erlang_cw( new FileCodeWriter( path + file + wxT( ".erl" ), useMicrosoftBOM, useUtf8 ) );

			codegen.SetSourceWriter( erlang_cw);
			codegen.GenerateCode( project );
			wxLogStatus( wxT( "Code generated on \'%s\'." ), path.c_str() );
		}
		catch ( wxFBException& ex )
		{
			wxLogError( ex.what() );
		}
	}
}
