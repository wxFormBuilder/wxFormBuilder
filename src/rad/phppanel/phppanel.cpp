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
// Modified by
//   Jefferson Gonzalez
//
///////////////////////////////////////////////////////////////////////////////

#include "phppanel.h"

#include "../appdata.h"
#include "../codeeditor/codeeditor.h"
#include "../wxfbevent.h"

#include "../../utils/encodingutils.h"
#include "../../utils/typeconv.h"
#include "../../utils/wxfbexception.h"

#include "../../model/objectbase.h"

#include "../../codegen/codewriter.h"
#include "../../codegen/phpcg.h"

#include <wx/fdrepdlg.h>

#include <wx/stc/stc.h>

BEGIN_EVENT_TABLE ( PHPPanel,  wxPanel )
	EVT_FB_CODE_GENERATION( PHPPanel::OnCodeGeneration )
	EVT_FB_PROJECT_REFRESH( PHPPanel::OnProjectRefresh )
	EVT_FB_PROPERTY_MODIFIED( PHPPanel::OnPropertyModified )
	EVT_FB_OBJECT_CREATED( PHPPanel::OnObjectChange )
	EVT_FB_OBJECT_REMOVED( PHPPanel::OnObjectChange )
	EVT_FB_OBJECT_SELECTED( PHPPanel::OnObjectChange )
	EVT_FB_EVENT_HANDLER_MODIFIED( PHPPanel::OnEventHandlerModified )

	EVT_FIND( wxID_ANY, PHPPanel::OnFind )
	EVT_FIND_NEXT( wxID_ANY, PHPPanel::OnFind )
END_EVENT_TABLE()

PHPPanel::PHPPanel( wxWindow *parent, int id )
:
wxPanel( parent, id )
{
	AppData()->AddHandler( this->GetEventHandler() );
	wxBoxSizer *top_sizer = new wxBoxSizer( wxVERTICAL );

	m_phpPanel = new CodeEditor( this, -1 );
	InitStyledTextCtrl( m_phpPanel->GetTextCtrl() );

	top_sizer->Add( m_phpPanel, 1, wxEXPAND, 0 );

	SetSizer( top_sizer );
	SetAutoLayout( true );
	//top_sizer->SetSizeHints( this );
	top_sizer->Fit( this );
	top_sizer->Layout();

	m_phpCW = PTCCodeWriter( new TCCodeWriter( m_phpPanel->GetTextCtrl() ) );
}

PHPPanel::~PHPPanel()
{
	//delete m_icons;
	AppData()->RemoveHandler( this->GetEventHandler() );
}
void PHPPanel::InitStyledTextCtrl( wxStyledTextCtrl *stc )
{
    stc->SetLexer( wxSTC_LEX_CPP );
	stc->SetKeyWords( 0, wxT( "php abstract and array as break case catch cfunction \
                               class clone const continue declare default do \
                               else elseif enddeclare endfor endforeach \
                               endif endswitch endwhile extends final for foreach function \
                               global goto if implements interface instanceof \
                               namespace new old_function or private protected public \
                               static switch throw try use var while xor __class__ __dir__ \
                               __file__ __line__ __function__ __method__ __namespace__ \
                               die echo empty eval exit include include_once isset list require \
                               require_once return print unset null" ) );

#ifdef __WXGTK__
	// Debe haber un bug en wxGTK ya que la familia wxMODERN no es de ancho fijo.
	wxFont font(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	font.SetFaceName( wxT( "Monospace" ) );
#else
	wxFont font(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#endif

    stc->StyleSetFont( wxSTC_STYLE_DEFAULT, font );
    stc->StyleClearAll();
    stc->StyleSetBold( wxSTC_C_WORD, true );
    stc->StyleSetForeground( wxSTC_C_WORD, *wxBLUE );
    stc->StyleSetForeground( wxSTC_C_STRING, *wxRED );
    stc->StyleSetForeground( wxSTC_C_STRINGEOL, *wxRED );
    stc->StyleSetForeground( wxSTC_C_PREPROCESSOR, wxColour( 49, 106, 197 ) );
    stc->StyleSetForeground( wxSTC_C_COMMENT, wxColour( 0, 128, 0 ) );
    stc->StyleSetForeground( wxSTC_C_COMMENTLINE, wxColour( 0, 128, 0 ) );
    stc->StyleSetForeground( wxSTC_C_COMMENTDOC, wxColour( 0, 128, 0 ) );
    stc->StyleSetForeground( wxSTC_C_COMMENTLINEDOC, wxColour( 0, 128, 0 ) );
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

void PHPPanel::OnFind( wxFindDialogEvent& event )
{
	m_phpPanel->GetEventHandler()->ProcessEvent( event );
}

void PHPPanel::OnPropertyModified( wxFBPropertyEvent& event )
{
	// Generate code to the panel only
	event.SetId( 1 );
	OnCodeGeneration( event );
}

void PHPPanel::OnProjectRefresh( wxFBEvent& event )
{
	// Generate code to the panel only
	event.SetId( 1 );
	OnCodeGeneration( event );
}

void PHPPanel::OnObjectChange( wxFBObjectEvent& event )
{
	// Generate code to the panel only
	event.SetId( 1 );
	OnCodeGeneration( event );
}

void PHPPanel::OnEventHandlerModified( wxFBEventHandlerEvent& event )
{
	// Generate code to the panel only
	event.SetId( 1 );
	OnCodeGeneration( event );
}

void PHPPanel::OnCodeGeneration( wxFBEvent& event )
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

    // Get PHP properties from the project

	// If PHP generation is not enabled, do not generate the file
	bool doFile = false;
	PProperty pCodeGen = project->GetProperty( wxT( "code_generation" ) );
	if ( pCodeGen )
	{
		//doFile = TypeConv::FlagSet( wxT("C++"), pCodeGen->GetValue() ) && !panelOnly;
		doFile = TypeConv::FlagSet( wxT("PHP"), pCodeGen->GetValue() ) && !panelOnly;
	}

	if ( !(doPanel || doFile ) )
	{
		return;
	}

	// Get First ID from Project File
	unsigned int firstID = 1000;
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
		PHPCodeGenerator codegen;
		codegen.UseRelativePath( useRelativePath, path );

		if ( pFirstID )
		{
			codegen.SetFirstID( firstID );
		}

		codegen.SetSourceWriter( m_phpCW );

		Freeze();

        wxStyledTextCtrl* phpEditor = m_phpPanel->GetTextCtrl();
		phpEditor->SetReadOnly( false );
		int phpLine = phpEditor->GetFirstVisibleLine() + phpEditor->LinesOnScreen() - 1;
		int phpXOffset = phpEditor->GetXOffset();

		codegen.GenerateCode( project );

		phpEditor->SetReadOnly( true );
		phpEditor->GotoLine( phpLine );
		phpEditor->SetXOffset( phpXOffset );
		phpEditor->SetAnchor( 0 );
		phpEditor->SetCurrentPos( 0 );

		Thaw();
	}

	// Generate code in the file
	if ( doFile )
	{
		try
		{
			PHPCodeGenerator codegen;
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

			PCodeWriter php_cw( new FileCodeWriter( path + file + wxT( ".php" ), useMicrosoftBOM, useUtf8 ) );

			codegen.SetSourceWriter( php_cw );
			codegen.GenerateCode( project );
			wxLogStatus( wxT( "Code generated on \'%s\'." ), path.c_str() );

			// check if we have to convert to ANSI encoding
			if (project->GetPropertyAsString(wxT("encoding")) == wxT("ANSI"))
			{
				UTF8ToAnsi(path + file + wxT( ".php" ));
			}
		}
		catch ( wxFBException& ex )
		{
			wxLogError( ex.what() );
		}
	}
}
