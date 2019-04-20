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

#include "../../utils/encodingutils.h"
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
	//top_sizer->SetSizeHints( this );
	top_sizer->Fit( this );
	top_sizer->Layout();

	m_erlangCW = PTCCodeWriter( new TCCodeWriter( m_erlangPanel->GetTextCtrl() ) );
}

ErlangPanel::~ErlangPanel()
{
	//delete m_icons;
	AppData()->RemoveHandler( this->GetEventHandler() );
}
void ErlangPanel::InitStyledTextCtrl( wxStyledTextCtrl *stc )
{
    stc->SetLexer( wxSTC_LEX_ERLANG );
	stc->SetKeyWords( 0, wxT( "abs after after and andalso arity atom atom_to_list band begin \
                              big binary binary binary_to_float binary_to_integer binary_to_list \
                              binary_to_list binary_to_term bit_size bits bitstring bitstring \
                              bnot bool boolean bor bsl bsr bxor byte byte_size bytes case \
                              catch char cond delete div do element end error export export_type \
                              false float float_to_binary float_to_list for fun function get hd \
                              identifier if import int integer integer_to_binary integer_to_list \
                              iodata iolist is_atom is_binary is_bitstring is_boolean is_float \
                              is_function is_integer is_list is_map is_number is_pid is_port \
                              is_record is_reference is_tuple length let list list_to_atom \
                              list_to_binary list_to_float list_to_integer list_to_tuple little \
                              long map_size maybe_improper_list mfa module native neg_integer \
                              nil no_return node non_neg_integer none nonempty_list \
                              nonempty_string not number of or orelse pid port pos_integer put \
                              receive record reference rem round self send short signed size \
                              spec string struct term term_to_binary this throw timeout tl true \
                              trunc try tuple tuple_size tuple_to_list type unsigned utf16 utf32 \
                              utf8 when while xor" ) );

#ifdef __WXGTK__
	// Debe haber un bug en wxGTK ya que la familia wxMODERN no es de ancho fijo.
	wxFont font(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	font.SetFaceName( wxT( "Monospace" ) );
#else
	wxFont font(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#endif

    stc->StyleSetFont( wxSTC_STYLE_DEFAULT, font );
    stc->StyleClearAll();
    stc->StyleSetBold( wxSTC_C_WORD, true );		// Strings
    stc->StyleSetBold( wxSTC_C_COMMENTDOC, true );	// Numbers
    stc->StyleSetForeground( wxSTC_C_WORD, *wxBLUE );
    stc->StyleSetForeground( wxSTC_C_STRING, *wxRED );
    stc->StyleSetForeground( wxSTC_C_STRINGEOL, *wxRED );
    stc->StyleSetForeground( wxSTC_C_PREPROCESSOR, wxColour( 49, 106, 197 ) );
    stc->StyleSetForeground( wxSTC_C_COMMENT, wxColour( 0, 192, 0 ) );
    stc->StyleSetForeground( wxSTC_C_COMMENTLINE, wxColour( 0, 0, 128 ) );
    stc->StyleSetForeground( wxSTC_C_COMMENTDOC, wxColour( 0, 128, 0 ) );
    stc->StyleSetForeground( wxSTC_C_COMMENTLINEDOC, wxColour( 128, 128, 128 ) );
    stc->StyleSetForeground( wxSTC_C_NUMBER, *wxBLUE );
	stc->SetUseTabs( true );
	stc->SetTabWidth( 4 );
	stc->SetTabIndents( true );
	stc->SetBackSpaceUnIndents( true );
	stc->SetIndent( 4 );
	stc->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	stc->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );

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

	if(panelOnly)
	{
	    objectToGenerate = AppData()->GetSelectedForm();
	}

	if(!panelOnly || !objectToGenerate)
	{
	    objectToGenerate = project;
	}

	// If only one project item should be generated then remove the rest items
	// from the temporary project
	if(doPanel && panelOnly && (objectToGenerate != project))
	{
	    if( project->GetChildCount() > 0)
	    {
	        unsigned int i = 0;
            while( project->GetChildCount() > 1 )
            {
                if(project->GetChild( i ) != objectToGenerate)
                {
                    project->RemoveChild( i );
                }
                else
                    i++;
            }
	    }
	}

    if(!project || !objectToGenerate)return;

    // Get Erlang properties from the project

	// If Erlang generation is not enabled, do not generate the file
	bool doFile = false;
	PProperty pCodeGen = project->GetProperty( wxT( "code_generation" ) );
	if ( pCodeGen )
	{
		doFile = TypeConv::FlagSet( wxT("Erlang"), pCodeGen->GetValue() ) && !panelOnly;
	}

	if ( !(doPanel || doFile ) )
	{
		return;
	}

	// Get First ID from Project File
	unsigned int firstID = 6000;	// (wxID_HIGHEST+1)
	PProperty pFirstID = project->GetProperty( wxT("first_id") );
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
		file = wxT("noname");
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
			PProperty pUseUtf8 = project->GetProperty( _("encoding") );

			if ( pUseUtf8 )
			{
				useUtf8 = ( pUseUtf8->GetValueAsString() != wxT("ANSI") );
			}

			PCodeWriter erlang_cw( new FileCodeWriter( path + file + wxT( ".erl" ), useMicrosoftBOM, useUtf8 ) );

			codegen.SetSourceWriter( erlang_cw);
			codegen.GenerateCode( project );
			wxLogStatus( wxT( "Code generated on \'%s\'." ), path.c_str() );

			// check if we have to convert to ANSI encoding
			if (project->GetPropertyAsString(wxT("encoding")) == wxT("ANSI"))
			{
				UTF8ToAnsi(path + file + wxT( ".erl" ));
			}
		}
		catch ( wxFBException& ex )
		{
			wxLogError( ex.what() );
		}
	}
}
