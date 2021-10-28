////////////////////////////////////////////////////////////////////////////////
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
// Erlang code generation written by
//   Micheus Vieira - micheus@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

#include "erlangcg.h"

#include "../model/objectbase.h"
#include "../rad/appdata.h"
#include "../rad/revision.h"
#include "../rad/version.h"
#include "../utils/debug.h"
#include "../utils/typeconv.h"
#include "../utils/wxfbexception.h"
#include "codewriter.h"

#include <algorithm>
#include <time.h>

#include <wx/filename.h>
#include <wx/tokenzr.h>

ErlangTemplateParser::ErlangTemplateParser( PObjectBase obj, wxString _template, bool useI18N, bool useRelativePath, wxString basePath, std::vector<wxString> strUserIDsVec )
:
TemplateParser(obj,_template),
m_i18n( useI18N ),
m_useRelativePath( useRelativePath ),
m_basePath( basePath ),
m_strUserIDsVec(strUserIDsVec)
{
	if ( !wxFileName::DirExists( m_basePath ) )
	{
		m_basePath.clear();
	}
//	SetupModulePrefixes();
}

ErlangTemplateParser::ErlangTemplateParser( const ErlangTemplateParser & that, wxString _template, std::vector<wxString> strUserIDsVec )
:
TemplateParser( that, _template ),
m_i18n( that.m_i18n ),
m_useRelativePath( that.m_useRelativePath ),
m_basePath( that.m_basePath ),
m_strUserIDsVec(strUserIDsVec)
{
//	SetupModulePrefixes();
}

wxString ErlangTemplateParser::RootWxParentToCode()
{
	PObjectBase form = AppData()->GetSelectedForm();
	if ( form )
	{
		wxString formName = form->GetPropertyAsString(_( "name" ) );
		return formName;
	}
	else
	{
		// in heppening, the code needs to be reviewed - usually, in *.erlangcode files, we may
		// need to use #parent instead of #wxparent. It also happens when exporting the project
		// and not the only a selected form. Anyway, there is a replace code to try fix it.
		return "this()";
	}
}

PTemplateParser ErlangTemplateParser::CreateParser( const TemplateParser* oldparser, wxString _template )
{
	const ErlangTemplateParser* erlangOldParser = dynamic_cast< const ErlangTemplateParser* >( oldparser );
	if ( erlangOldParser )
	{
		std::vector<wxString> empty;
		PTemplateParser newparser( new ErlangTemplateParser( *erlangOldParser, _template, empty) );
		return newparser;
	}
	return PTemplateParser();
}

/**
* Convert the value of the property to Erlang code
*/
wxString ErlangTemplateParser::ValueToCode( PropertyType type, wxString value )
{
	wxString result;
	switch ( type )
	{
	case PT_WXPARENT:	// variable as parameter of a function
	case PT_TEXT: 		// variable assignment
		{
			// We ensure the control's name starts with a upper case char - which is mandatory for Erlang variables
			value[0] = wxToupper( value[0] );
			result = value;
			break;
		}
	case PT_WXPARENT_SB:
		{
			value[0] = wxToupper( value[0] );
			result = wxString::Format( wxT( "wxStaticBoxSizer:getStaticBox(%s)" ), value.c_str() );
			break;
		}
	case PT_WXPARENT_CP:
		{
			value[0] = wxToupper( value[0] );
			result = wxString::Format( wxT( "wxAuiPaneInfo:getPane(%s)" ), value.c_str() );
			break;
		}
	case PT_WXSTRING:
	case PT_FILE:
	case PT_PATH:
		{
			if ( value.empty() )
			{
				result << wxT( "\"\"" );
			}
			else
			{
				result << wxT( "\"" ) << ErlangCodeGenerator::ConvertErlangString(value) << wxT( "\"" );
			}
			break;
		}
	case PT_WXSTRING_I18N:
		{
			if ( value.empty() )
			{
				result << wxT( "\"\"" );
			}
			else
			{
				if ( m_i18n )
				{
					result << wxT( "\"" ) << ErlangCodeGenerator::ConvertErlangString(value) << wxT( "\"" );
				}
				else
				{
					result << wxT( "\"" ) << ErlangCodeGenerator::ConvertErlangString(value) << wxT( "\"" );
				}
			}
			break;
		}
	case PT_MACRO:
	case PT_OPTION:
			if ( !value.empty() )
			{
				if ( value.find_first_of( wxT( "wx" ) ) == 0 )
				{	// wx IDs
					result = wxT( "?" ) + value;
				}
				else
				{	// user IDs
					result = value;
				}
			}
			break;
	case PT_CLASS:
	case PT_EDIT_OPTION:
	case PT_FLOAT:
	case PT_INT:
	case PT_UINT:
		{
			result = value;
			break;
		}
	case PT_BITLIST:
		{
			if ( value.empty() )
			{
				result =  wxT( "0" );
				break;
			}
			else
			{
				result = value;
			}
			wxString bit, res, pref;
			wxStringTokenizer bits( result, wxT( "|" ), wxTOKEN_STRTOK );

			while( bits.HasMoreTokens() )
			{
				bit = bits.GetNextToken();
				res += pref + wxT( "?" ) + bit.Trim(false);
				pref = wxT( " bor " );
			}
			result = res;
			break;
		}
	case PT_WXPOINT:
		{
			if ( value.empty() || (value == wxT( "-1,-1" ) ) )
			{
				result = wxT( "?wxDefaultPosition" );
			}
			else
			{
				result << wxT( "{" ) << value << wxT( "}" );
			}
			break;
		}
	case PT_WXSIZE:
		{
			if ( value.empty() || (value == wxT( "-1,-1" ) ) )
			{
				result = wxT( "?wxDefaultSize" );
			}
			else
			{
				result << wxT( "{" ) << value << wxT( "}" );
			}
			break;
		}
	case PT_BOOL:
		{
			result = ( value == wxT( "0" ) ? wxT( "false" ) : wxT( "true" ) );
			break;
		}
	case PT_WXFONT:
		{
			if ( !value.empty() )
			{
				wxFontContainer fontContainer = TypeConv::StringToFont( value );
				wxFont font = fontContainer.GetFont();

				const int pointSize = fontContainer.GetPointSize();
				wxString options = wxString::Format( "[{underlined,%s}, {face,%s}]",
									(fontContainer.GetUnderlined() ? "true" : "false" ),
									(fontContainer.m_faceName.empty() ? "\"\"" : ( "\"" + fontContainer.m_faceName + "\"" ) ));
				wxString strGetPoint;
				if ( pointSize <= 0 )
				{
					strGetPoint = "wxFont:getPointSize(wxSystemSettings:getFont(?wxSYS_DEFAULT_GUI_FONT) ),\n\t\t";
				}
				else
				{
					strGetPoint = wxString::Format( "%d,", pointSize);
				}
				result = wxString::Format( "wxFont:new(%s ?%s, ?%s, ?%s, %s)",
							strGetPoint,
							TypeConv::FontFamilyToString( fontContainer.GetFamily() ),
							font.GetStyleString(),
							font.GetWeightString(),
							options
						);
			}
			else
			{
				result = wxT( "wxNORMAL_FONT" );
			}
			break;
		}
		case PT_WXCOLOUR:
		{
			// wxColor in Erlang is defined wx module as a datatype:
			// => wx_colour() = {R::byte(), G::byte(), B::byte()} | wx_colour4()
			//    wx_colour4() = {R::byte(), G::byte(), B::byte(), A::byte()}
			if ( !value.empty() )
			{
				if ( value.find_first_of( wxT( "wx" ) ) == 0 )
				{
					// System Colour
					result << wxT( "wxSystemSettings:getColour( " ) << ValueToCode( PT_OPTION, value ) << wxT( " )" );
				}
				else
				{
					wxColour colour = TypeConv::StringToColour( value );
					result = wxString::Format( wxT( "{%i,%i,%i}" ), colour.Red(), colour.Green(), colour.Blue() );
				}
			}
			else
			{
				result = wxT( "{0,0,0}" );
			}
			break;
		}
	case PT_BITMAP:
		{
			wxString path;
			wxString source;
			wxSize icoSize;
			TypeConv::ParseBitmapWithResource( value, &path, &source, &icoSize );

			if ( path.empty() )
			{
				// Empty path, generate an empty Bitmap - Erlang requires a non null parameter
				result = wxT( "wxBitmap:new()" );
				break;
			}

			if ( path.StartsWith( wxT( "file:" ) ) )
			{
				wxLogWarning( wxT( "Erlang code generation does not support using URLs for bitmap properties:\n%s" ), path.c_str() );
				result = wxT( "wxBitmap:new()" );
				break;
			}

			if ( source == _( "Load From File" ) || source == _( "Load From Embedded File" ) )
			{
				wxString absPath;
				try
				{
					absPath = TypeConv::MakeAbsolutePath( path, AppData()->GetProjectPath() );
				}
				catch( wxFBException& ex )
				{
					wxLogError( ex.what() );
					result = wxT( "wxBitmap:new()" );
					break;
				}

				wxString file = ( m_useRelativePath ? TypeConv::MakeRelativePath( absPath, m_basePath ) : absPath );

				result << wxT( "wxBitmap:new(\"" ) << ErlangCodeGenerator::ConvertErlangString(file) << wxT( "\", [{type,?wxBITMAP_TYPE_ANY}])" );
			}
			else if ( source == _( "Load From Resource" ) )
			{
				result << wxT( "wxBitmap:new(\"" ) << path << wxT( "\", [{type,?wxBITMAP_TYPE_RESOURCE}])" );
			}
			else if ( source == _( "Load From Icon Resource" ) )
			{
				if ( wxDefaultSize == icoSize )
				{
					result << wxT( "wxIcon:new( " ) << path << wxT( " )" );
				}
				else
				{
					result.Printf( wxT( "wxIcon:new(\"%s\", [{type,?wxBITMAP_TYPE_ICON_RESOURCE}, {desiredWidth,%i}, {desiredHeight,%i}])" ), path.c_str(), icoSize.GetWidth(), icoSize.GetHeight() );
				}
			}
			else if ( source == _( "Load From XRC" ) )
			{
				// This requires that the global wxXmlResource object is set
				result << wxT( "wxXmlResource:loadBitmap(wxXmlResource:get(), \"" ) << path << wxT( "\" )" );
			}
			else if ( source == _( "Load From Art Provider" ) )
			{
				wxString rid = path.BeforeFirst( wxT(':') );
				if ( rid.StartsWith( wxT( "gtk-" ) ) )
				{
					rid = wxT( "\"" ) + rid + wxT( "\"" );
				}

				wxString cid = path.AfterFirst( wxT(':') );

				if ( wxDefaultSize == icoSize )
				{
					result.Printf( wxT( "wxArtProvider:getIcon(\"%s\", [{client,\"%s\"}, {size,{%i,%i}}])" ), rid.c_str(), cid.c_str(), icoSize.GetWidth(), icoSize.GetHeight() );
				}
				else
				{
					result.Printf( wxT( "wxArtProvider:getBitmap(\"%s\", [{client,\"%s\"}, {size,{%i,%i}}])" ), rid.c_str(), cid.c_str(), icoSize.GetWidth(), icoSize.GetHeight() );
				}
			}
			break;
		}
	case PT_STRINGLIST:
		{
			// Stringlists are generated like a sequence of wxString separated by ', '.
			wxArrayString array = TypeConv::StringToArrayString( value );
			if ( array.Count() > 0 )
			{
				result = ValueToCode( PT_WXSTRING_I18N, array[0] );
			}

			for ( size_t i = 1; i < array.Count(); i++ )
			{
				result << wxT( "," ) << ValueToCode( PT_WXSTRING_I18N, array[i] );
			}
			break;
		}
	default:
		break;
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////

ErlangCodeGenerator::ErlangCodeGenerator()
{
	SetupPredefinedMacros();
	m_useRelativePath = false;
	m_i18n = false;
	m_firstID = 1000;  // this value can be overwritten in erlangpanel.cpp (calling SetFirstID() )

	//this classes aren't wrapped by wx in Erlang - make exception
	// Common tab {file: common.erlangcode}
	m_strUnsupportedClasses.push_back( wxT( "wxAnimationCtrl" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxBitmapComboBox" ) );
	// Aditional tab {file: aditional.erlangcode}
	m_strUnsupportedClasses.push_back( wxT( "wxRichTextCtrl" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxBitmapToggleButton" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxSearchCtrl" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxTimePickerCtrl" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxSpinCtrlDouble" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxHyperlinkCtrl" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxTimer" ) );
	m_strUnsupportedClasses.push_back( wxT( "CustomControl" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxMediaCtrl" ) );
	// Data tab {file: data.erlangcode}
	m_strUnsupportedClasses.push_back( wxT( "wxDataViewCtrl" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxDataViewTreeCtrl" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxDataViewListCtrl" ) );
	m_strUnsupportedClasses.push_back( wxT( "dataViewListColumn" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxDataViewColumn" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxTreeListCtrl" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxDataViewColumn" ) );
	m_strUnsupportedClasses.push_back( wxT( "dataViewListCtrlColumn" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxPropertyGrid" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxPropertyGridManager" ) );
	m_strUnsupportedClasses.push_back( wxT( "propGridItem" ) );
	m_strUnsupportedClasses.push_back( wxT( "propGridPage" ) );
	// Containers tab {file: containers.erlangcode}
	m_strUnsupportedClasses.push_back( wxT( "wxCollapsiblePane" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxSimplebook" ) );
	// Menu/Toolbar tab {file: menutoolbar.erlangcode}
	m_strUnsupportedClasses.push_back( wxT( "wxInfoBar" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxAuiToolBar" ) );
	// Layout tab {file: layout.erlangcode}
	m_strUnsupportedClasses.push_back( wxT( "wxWrapSizer" ) );
	// Forms tab {file: forms.erlangcode}
	m_strUnsupportedClasses.push_back( wxT( "wxWizard" ) );
	m_strUnsupportedClasses.push_back( wxT( "wxWizardPageSimple" ) );
	// Ribbon tab {file: ribbon.erlangcode}
	m_strUnsupportedClasses.push_back( wxT( "wxRibbonBar" ) );

}

wxString ErlangCodeGenerator::ConvertErlangString( wxString text )
{
	wxString result;

	for ( size_t i = 0; i < text.length(); i++ )
	{
		wxChar c = text[i];

		switch ( c )
		{
		case wxT('"'):
			result += wxT( "\\\"" );
			break;

		case wxT('\\'):
			result += wxT( "\\\\" );
			break;

		case wxT('\t'):
			result += wxT( "\\t" );
			break;

		case wxT('\n'):
			result += wxT( "\\n" );
			break;

		case wxT('\r'):
			result += wxT( "\\r" );
			break;

		default:
			result += c;
			break;
		}
	}
	return result;
}

/*
wxString ErlangCodeGenerator::GenEventEntryForInheritedClass( PObjectBase obj, PObjectInfo obj_info, const wxString& templateName, const wxString& handlerName, wxString &strClassName )
{
	wxString code;
	PCodeInfo code_info = obj_info->GetCodeInfo( wxT( "Erlang" ) );
	if ( code_info )
	{
		wxString _template;
		_template = code_info->GetTemplate( wxString::Format( wxT( "evt_%s" ), templateName.c_str() ) );
		if ( !_template.empty() )
		{
			_template.Replace( wxT( "#handler" ),handlerName );
			_template.Replace( wxT( "#skip" ),wxT( "\n" ) + m_strEventHandlerPostfix );
		}

		ErlangTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
		code = parser.ParseTemplate();
		if (code.length() > 0) return code;

		for ( unsigned int i = 0; i < obj_info->GetBaseClassCount(false); i++ )
		{
			PObjectInfo base_info = obj_info->GetBaseClass( i, false );
			if ( (code = GenEventEntryForInheritedClass( obj, base_info, templateName, handlerName, strClassName ) ).length() > 0 )
			{
				return code;
			}
		}
	}
	return code;
}
*/

bool ErlangCodeGenerator::GenerateCode( PObjectBase project )
{
	if ( !project )
	{
		wxLogError( wxT( "There is no project to generate code" ) );
		return false;
	}

	m_i18n = false;
	PProperty i18nProperty = project->GetProperty( wxT( "internationalize" ) );
	if (i18nProperty && i18nProperty->GetValueAsInteger() )
		m_i18n = true;

	// The disconection property was no added to the 'default.xml' for 'Erlang Properties'
	// We may need to evaluate further the need for the disconnection code generation
	m_disconnectEvents = ( project->GetPropertyAsInteger( wxT( "disconnect_erlang_events" ) ) != 0 );

	m_source->Clear();

	PProperty propFile = project->GetProperty( wxT( "file" ) );
	if ( !propFile )
	{
		wxLogError( wxT( "Missing \"file\" property on Project Object" ) );
		return false;
	}

	PProperty lowerIdentifier = project->GetProperty( wxT( "erlang_use_lowercase_identifiers" ) );
	if ( !lowerIdentifier )
	{
		wxLogError( wxT( "Missing \"erlang_use_lowercase_identifiers\" property on Project Object" ) );
		return false;
	}
	m_lowerIdentifier = lowerIdentifier->GetValueAsInteger();

	PProperty createPrefix = project->GetProperty( wxT( "erlang_toplevel_function_prefix" ) );
	if ( !createPrefix )
	{
		wxLogError( wxT( "Missing \"erlang_toplevel_function_prefix\" property on Project Object" ) );
		return false;
	}

	PProperty useParent = project->GetProperty( wxT( "erlang_use_toplevel_parent_parameter" ) );
	if ( !useParent )
	{
		wxLogError( wxT( "Missing \"erlang_use_toplevel_parent_parameter\" property on Project Object" ) );
		return false;
	}

	wxString code = wxString::Format(
		wxT( "%%%% --------------------------------------------------------------------------\n" )
		wxT( "%%%%  Erlang code generated with wxFormBuilder (version %s%s)\n" )
		wxT( "%%%%  http://www.wxformbuilder.org/\n" )
		wxT( "%%%%  This file is changed by wxFormBuilder on saving. PLEASE DO *NOT* EDIT IT!\n" )
		wxT( "%%%% --------------------------------------------------------------------------\n" ),
		getVersion(), getPostfixRevision(getVersion()).c_str()
	);

	m_source->WriteLn( code );

	wxString Prefix = createPrefix->GetValue();
	bool Parent = useParent->GetValueAsInteger();
	wxString file = propFile->GetValue();
	if ( file.empty() )
	{
		file = wxT( "noname" );
	}
	else
	{
		file = MakeErlangIdentifier( file );
	}

	// Insert Erlang preamble
	PCodeInfo code_info = project->GetObjectInfo()->GetCodeInfo( wxT( "Erlang" ) );
	code = code_info->GetTemplate( wxT( "erlang_preamble" ) );
	if ( !code.empty() )
	{
		m_source->WriteLn( code );
		m_source->WriteLn( wxEmptyString );
	}
	PProperty propCopyRight = project->GetProperty( wxT( "erlang_copyright" ) );
	if (propCopyRight)
	{
		code = propCopyRight->GetValueAsString();
		code.Replace("\\n","#nl");
		ErlangTemplateParser parser( project, code, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
		code = parser.ParseTemplate();
		m_source->WriteLn( code );
		m_source->WriteLn( wxEmptyString );
	}

	// Write the module header
	wxString tempName = wxString::Format( wxT( "-module(%s)." ), file );
	m_source->WriteLn( tempName );
	m_source->WriteLn();

	// Generating  includes
	std::vector< wxString > headerIncludes;
	std::set< wxString > templates;
	GenIncludes(project, &headerIncludes, &templates );

	// Write the include lines
	std::vector<wxString>::iterator include_it;
	for ( include_it = headerIncludes.begin(); include_it != headerIncludes.end(); ++include_it )
	{
		m_source->WriteLn( *include_it );
	}
	if ( !headerIncludes.empty() )
	{
		m_source->WriteLn(wxEmptyString);
	}

	// Generate and writes the exports
	PProperty genExport = project->GetProperty( wxT( "erlang_code_generation" ) );
	if ( genExport )
	{
		m_fullExport = genExport->GetValueAsString() == wxT( "full_code_prototype" );
		GenExport( project, code_info, Prefix, Parent );
	}
	else
	{
		m_fullExport = false;
	}

	// Cleaning up the local/temporary event table
	ClearLocalEventTable( wxT("all") );
	// Generate and write the creation functions for each form/window and
	// stores the events for further definition
	unsigned int dProjChildCount = project->GetChildCount();
	for ( unsigned int i = 0; i < dProjChildCount; i++ )
	{
		PObjectBase child = project->GetChild( i );

		// Preprocess to find arrays
		ArrayItems arrays;
		FindArrayObjects(child, arrays, true);

		EventVector events;
		FindEventHandlers( child, events );
		GenClassDeclaration(child, false, wxEmptyString, events, m_strEventHandlerPostfix, arrays, Prefix, Parent);
	}

	// Writes the headers for the events handler
	GenEventHeaders( project, code_info );

	// currently, there is not definition for this session in 'defaul.erlangcode' file
	code = GetCode( project, wxT( "erlang_epilogue" ) );
	if ( !code.empty() )
		m_source->WriteLn( code );

	return true;
}

void ErlangCodeGenerator::ClearLocalEventTable( const wxString &strEvtName )
{
	if ( strEvtName != wxT("all") )
	{
		// this is intended to allow us to remove only sync and async information
		// from the event table and simplify the process to get the event's name to
		// be used in the functions declarations
		auto it = m_handledEvents.find( strEvtName );
		if ( it != m_handledEvents.end() )  // not found
		{
			std::map<wxString, std::map<wxString, wxString>> eventClassList = it->second;

			for ( std::map<wxString, std::map<wxString, wxString>>::iterator
				itC=eventClassList.begin(); itC!=eventClassList.end(); ++itC )
			{
				std::map<wxString, wxString> controlInfoList = itC->second;
				controlInfoList.clear();
				eventClassList.erase(itC);
			}
			m_handledEvents.erase(it);
		}
	}
	else
	{
		// here we are cleanup the entry event table

		// looking at event name list for the event type list
		for ( std::map<wxString, std::map<wxString, std::map<wxString, wxString>>>::iterator
			it=m_handledEvents.begin(); it!=m_handledEvents.end(); ++it )
		{
			// looking at the event type list for the control info list
			std::map<wxString, std::map<wxString, wxString>> eventClassList = it->second;
			m_source->WriteLn( wxString::Format("strEvtType-> %s (%i)\n", it->first, eventClassList.size() ) );

			for ( std::map<wxString, std::map<wxString, wxString>>::iterator
				itC=eventClassList.begin(); itC!=eventClassList.end(); ++itC )
			{
				// looking at the control info list
				std::map<wxString, wxString> controlInfoList = itC->second;
				m_source->WriteLn( wxString::Format("strEvtClass-> %s (%i)\n", itC->first, controlInfoList.size() ) );
				for ( std::map<wxString, wxString>::iterator
					itT=controlInfoList.begin(); itT!=controlInfoList.end(); ++itT )
				{
					m_source->WriteLn( wxString::Format("strControl-> %s (%s)\n", itT->first, itT->second) );
				}
				controlInfoList.clear();
			}
			eventClassList.clear();
		}
		m_handledEvents.clear();
	}
}

void ErlangCodeGenerator::AddLocalEvent( const wxString &strControl, const wxString &strEvtName, const wxString &strEvtClass, const wxString &strEvtType )
{
	control_info_type controlInfoList;
	event_class_type eventClassList;
	event_table_type::iterator it;

	// strEvtName values possible: sync|async|event name

	// checking if the event name already was defined
	it = m_handledEvents.find( strEvtName );
	if ( it == m_handledEvents.end() )  // not found
	{
		controlInfoList.insert( std::make_pair( strEvtType, strControl ) );
		eventClassList.insert( std::make_pair( strEvtClass, controlInfoList ) );
		m_handledEvents.insert( std::make_pair( strEvtName, eventClassList ) );
	}
	else
	{
		event_class_type::iterator itC;

		eventClassList = it->second;
		itC = eventClassList.find( strEvtClass );
		if ( itC == eventClassList.end() )  // not found
		{
			controlInfoList.insert( std::make_pair( strEvtType, strControl ) );
			eventClassList.insert( std::make_pair( strEvtClass, controlInfoList ) );
		}
		else
		{
			control_info_type::iterator itT;

			controlInfoList = itC->second;
			itT = controlInfoList.find( strEvtType );
			if ( itT == controlInfoList.end() )  // not found
			{
				controlInfoList.insert( std::make_pair( strEvtType, strControl ) );
			}
			else
			{
				wxString strPrevControls = itT->second;
				itT->second = strPrevControls + ", " + strControl;
			}
			itC->second = controlInfoList;
		}
		it->second = eventClassList;
	}
}

void ErlangCodeGenerator::GenEvents( PObjectBase class_obj, const EventVector &events, bool disconnect )
{
	if ( events.empty() )
	{
		return;
	}

	m_source->WriteLn();
	if ( disconnect )
	{
		m_source->WriteLn( wxT( "%% Disconnect Events" ) );
	}
	else
	{
		m_source->WriteLn( wxT( "%% Connect Events" ) );
	}

	PProperty propName = class_obj->GetProperty( wxT( "name" ) );
	if ( !propName )
	{
		wxLogError( wxT( "Missing \"name\" property on \"%s\" class. Review your XML object description" ),
			class_obj->GetClassName().c_str() );
		return;
	}

	wxString class_name = propName->GetValue();
	if ( class_name.empty() )
	{
		wxLogError( wxT( "Object name cannot be null" ) );
		return;
	}

	if ( events.size() > 0 )
	{
		for ( size_t i = 0; i < events.size(); i++ )
		{
			PEvent event = events[i];

			// 'evt_entry_...' in *.rlangcode files are not evaluated by Erlang code generator.
			// it's used to define 'generic' event handler which is not
			// implemented in wx (connect) which always requires an event id.
			wxString templateName = wxString::Format( wxT( "connect_%s" ), event->GetName().c_str() );

			PObjectBase obj = event->GetObject();
			if ( !GenEventEntry(obj, obj->GetObjectInfo(), event, templateName, class_name, disconnect) )
			{
				wxLogError( wxT( "Missing \"evt_%s\" template for \"%s\" class. Review your XML object description" ),
					templateName.c_str(), class_name.c_str() );
			}
		}
	}
}

bool ErlangCodeGenerator::GenEventEntry( PObjectBase obj, PObjectInfo obj_info, PEvent event, const wxString& templateName, wxString &strClassName, bool disconnect )
{
	wxString _template;
	PCodeInfo code_info = obj_info->GetCodeInfo( wxT( "Erlang" ) );
	if ( code_info )
	{
		// The disconnect evaluation code was kept, but a property for this was not set
		// in default.xml (in "Erlang Properties" category) as well as it was also not
		// implemented the variable initialization in this source file
		_template = code_info->GetTemplate(wxString::Format( wxT( "evt_%s%s" ), disconnect ? wxT( "dis" ) : wxEmptyString, templateName.c_str() ));
		if ( disconnect && _template.empty() )
		{
			_template = code_info->GetTemplate( wxT( "evt_" ) + templateName );
			_template.Replace( wxT( "connect" ), wxT( "disconnect" ), true );
		}

		if ( !_template.empty() )
		{
			wxString option;
			wxString strHandlerName = event->GetValue();
			// wxFormBuilder is allowing to enter multiple words as name for a control or event
			strHandlerName.Replace( wxT(" "), wxT("_"), true );

			if ( disconnect )
			{
				strHandlerName = MakeErlangIdentifier( strHandlerName );
				const auto pos = _template.Find( wxT( "*option" ) );
				if ( pos != -1 ) {
					_template.Replace( wxT( "*option" ), wxT( "[]" ) );
				}
			}
			else
			{
				// Callbacks are sync events, so we use 'async' to indicate we want to
				// handle the event that way.
				// We can use 'callback' to indicate the event will be handled by a sync event handler
				// or give the event a 'name id' which will create the specific function to handle it
				// and be included as the callback parameter in options.
				if ( strHandlerName.Lower() == wxT( "async" ) )
				{
					option = wxEmptyString;
					strHandlerName = wxT( "async" );
				}
				else if ( strHandlerName.Lower() == wxT( "skip" ) )
				{
					option = wxT( ", [{skip, true}]" );
					// after used, we sign it as async event
					strHandlerName = wxT( "async" );
				}
				else if ( strHandlerName.Lower() == wxT( "callback" ) )
				{
					option = wxString::Format( ", [%s]", strHandlerName.Lower() );
					strHandlerName = wxT( "sync" );
				}
				else
				{
					strHandlerName = MakeErlangIdentifier( strHandlerName );
					option = wxString::Format( ", [{callback, fun %s/2}]", strHandlerName );
				}
				// trying two variations of format possible entered in the *.erlangcode files
				auto pos = _template.Find( wxT( ",*option" ) );
				if ( pos != -1 ) {
					_template.Replace( wxT( ",*option" ), option );
				}
				else
				{
					pos = _template.Find( wxT( ", *option" ) );
					if ( pos != -1 ) {
						_template.Replace( wxT( ", *option" ), option );
					}
				}
			}

			// saving the template data for further use
			wxString eventId = _template;
			// creating the line of code from the template
			ErlangTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
			wxString code = parser.ParseTemplate();

			// After parse the template, we check if the object class has a 'base' template
			// if so, it meams the object is from the 'Form' toolbar and wxFormBuilder will
			// use a class without the 'wx' prefix. The 'base' information has the correct
			// class to be used (p.e. Panel => wxPanel).
			// See https://github.com/wxFormBuilder/wxFormBuilder/issues/638#issuecomment-690295084
			wxString baseClass = GetCode( obj, wxT("base") );
			if ( baseClass != wxEmptyString )
			{
				code = ( baseClass + wxT(":") + code.AfterFirst( wxT(':') ) );
			}

			// This will fix the main form owner name. It will happens when
			// the var got 'this()' as result of a call to RootWxParentToCode()
			if ( code.Find( m_rootWxParent ) != -1 )
			{
				code.Replace( m_rootWxParent, strClassName );
			}

			// when a conditional if clausule is used in events template code can be null
			if ( !code.IsEmpty() )
			{
				m_source->WriteLn( code );

				// Collecting data to build the event handlers table

				// event handlers are defined (usually) in the format:
				//   #class:connect($name, event_id, *option), #nl
				//   #class:connect($name, event_id), #nl
				// or eventually:
				//   #class:disconnect($name, event_id), #nl
				eventId.Replace( wxT(" "), wxEmptyString, true );
				eventId = eventId.AfterFirst( wxT(',') );
				eventId = eventId.BeforeFirst( wxT(')') );
				const auto pos = eventId.Find( wxT(',') );
				if ( pos != -1 ) {
					eventId = eventId.BeforeFirst( wxT(',') );
				}

				wxString strControlName = obj->GetProperty( wxT( "name" ) )->GetValue();
				strControlName[0] = wxToupper( strControlName[0] );  // variables must start with uppercase char

				// Filling the event handlers table
				AddLocalEvent( strControlName,  // control who owns the event
						strHandlerName,  // event name|callback|async|skip
						event->GetEventInfo()->GetEventClassName(),  // event class
						eventId.Lower()  // event type
						);
			}

			return true;
		}
	}

	for ( unsigned int i = 0; i < obj_info->GetBaseClassCount(false); i++ )
	{
		PObjectInfo base_info = obj_info->GetBaseClass( i, false );
		if ( GenEventEntry( obj, base_info, event, templateName, strClassName, disconnect ) )
		{
			return true;
		}
	}

	return false;
}

void ErlangCodeGenerator::GenFormatedEventHeaders( PObjectBase project, const wxString _template )
{
	wxString code;
	ErlangTemplateParser parser( project, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
	code = parser.ParseTemplate();

	// we put back the '#' used by Erlang in the event's class name
	code.Replace( wxT( "*" ), wxT( "#") );
	m_source->WriteLn( code );
}

void ErlangCodeGenerator::GenFormatedEventHeaders( PObjectBase project, const wxString _template, const event_table_type::value_type& event)
{
	const auto& strEvtName = event.first;

	for (const auto& eventClass : event.second) {
		auto StrEvtClass = eventClass.first;
		StrEvtClass.Replace(wxT("Event"), wxEmptyString);

		for (const auto& controlInfo : eventClass.second) {
			const auto& strEvtType = controlInfo.first;
			const auto& strControls = controlInfo.second;
			auto code = _template.Clone();

			// building the event class and type
			auto strEvtParam = wxString::Format( wxT( "*wx{event = *%s{type = %s}}" ),
											StrEvtClass, strEvtType );

			// we try to replace defined macros '*controls', '*fun' and '*event' when available
			// the event - case it was defined on template
			code.Replace( wxT( "*controls" ), strControls );
			code.Replace( wxT( "*fun" ), strEvtName, true );  // only exists for non 'sync' and 'async' templates
			code.Replace( wxT( "*event" ), strEvtParam );

			// if an event for a class is not supported the EvtType will be empty
			// and then we remove its declaration block
			if ( !strEvtType.IsEmpty() ) {
				GenFormatedEventHeaders( project, code);
			}
		}
	}
}

void ErlangCodeGenerator::GenEventHeaders( PObjectBase project, PCodeInfo code_info )
{
	wxString code;
	wxString _template;

	// strEvtName values: sync|async|event name
	if ( m_fullExport )
	{
		m_source->WriteLn( wxEmptyString );
		m_source->WriteLn( wxT( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
					"%% Sync event from callback events" ) );

		_template = code_info->GetTemplate( wxT( "events_sync" ) );
		for ( std::map<wxString, std::map<wxString, std::map<wxString, wxString>>>::iterator
			it=m_handledEvents.find( wxT("sync") ); (it!=m_handledEvents.end() &&
									(it->first == wxT("sync") )); ++it )
		{
			GenFormatedEventHeaders( project, _template, *it );
		}
		_template = code_info->GetTemplate( wxT( "events_sync_end" ) );
		GenFormatedEventHeaders( project, _template );
		ClearLocalEventTable( wxT("sync") );

		m_source->WriteLn( wxEmptyString );
		m_source->WriteLn( wxT( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
					"%% Async Events are handled in handle_event as in handle_info" ) );

		_template = code_info->GetTemplate( wxT( "events_async" ) );
		for ( std::map<wxString, std::map<wxString, std::map<wxString, wxString>>>::iterator
			it=m_handledEvents.find( wxT("async") ); (it!=m_handledEvents.end() &&
									(it->first == wxT("async") )); ++it )
		{
			GenFormatedEventHeaders( project, _template, *it );
		}
		_template = code_info->GetTemplate( wxT( "events_async_end" ) );
		GenFormatedEventHeaders( project, _template );
		ClearLocalEventTable( wxT("async") );

		_template = code_info->GetTemplate( wxT( "full_export_events_end" ) );
		GenFormatedEventHeaders( project, _template );
	}
	else
	{
		ClearLocalEventTable( wxT("sync") );
		ClearLocalEventTable( wxT("async") );
	}

	if (!m_handledEvents.empty())
	{
		m_source->WriteLn( wxEmptyString );
		m_source->WriteLn( wxT( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
					"%% Functions from callback events" ) );

		_template = code_info->GetTemplate( wxT( "events_fun" ) );
		for ( std::map<wxString, std::map<wxString, std::map<wxString, wxString>>>::iterator
			it=m_handledEvents.begin(); it!=m_handledEvents.end(); ++it )
		{
			GenFormatedEventHeaders( project, _template, *it );
		}
	}
}

void ErlangCodeGenerator::GetGenEventHandlers( PObjectBase obj )
{
	GenDefinedEventHandlers( obj->GetObjectInfo(), obj );

	for (unsigned int i = 0; i < obj->GetChildCount() ; i++)
	{
		PObjectBase child = obj->GetChild(i);
		GetGenEventHandlers(child);
	}
}

void ErlangCodeGenerator::GenDefinedEventHandlers( PObjectInfo info, PObjectBase obj )
{
	PCodeInfo code_info = info->GetCodeInfo( wxT( "Erlang" ) );
	if ( code_info )
	{
		wxString _template = code_info->GetTemplate( wxT( "generated_event_handlers" ) );
		if ( !_template.empty() )
		{
			ErlangTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
			wxString code = parser.ParseTemplate();

			if ( !code.empty() )
			{
				m_source->WriteLn(code);
			}
		}
	}

	// Proceeding recursively with the base classes
	for ( unsigned int i = 0; i < info->GetBaseClassCount(false); i++ )
	{
		PObjectInfo base_info = info->GetBaseClass( i, false );
		GenDefinedEventHandlers( base_info, obj );
	}
}


wxString ErlangCodeGenerator::GetCode(PObjectBase obj, wxString name, bool silent /*= false*/, wxString strSelf /*= wxEmptyString*/)
{
	PCodeInfo code_info = obj->GetObjectInfo()->GetCodeInfo( wxT( "Erlang" ) );

	if (!code_info)
	{
		if ( !silent )
		{
			wxString msg( wxString::Format( wxT( "Missing \"%s\" template for \"%s\" class. Review your XML object description" ),
				name.c_str(), obj->GetClassName().c_str() ) );
			wxLogError(msg);
		}
		return wxEmptyString;
	}

	wxString _template = code_info->GetTemplate(name);

	ErlangTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
	wxString code = parser.ParseTemplate();

	//handle unsupported classes
	std::vector<wxString>::iterator iter = m_strUnsupportedInstances.begin();
	for(; iter != m_strUnsupportedInstances.end(); ++iter){
		if (code.Contains(*iter) ) break;
	}
	if ( iter != m_strUnsupportedInstances.end() )
		code.Prepend( wxT( "%% " ) );

	// Handle Option parameter in "construction" template (when available)
	// If the values for the option list are the default one they are removed in the template,
	// but any parameter existing will produce a tuple followed by a comma. This way, the last
	// param will be inserting a invalid comma to the list which needs to be removed (it's always ", " )
	// Also, because to use '$var_name}' was causing problems with the parser it was necessart to add
	// a space between then '$var_name }' which will be removed here too.
	int pos = code.Find( wxT( "  " ) );
	if ( pos != -1 ) {
		code.Replace( wxT( "  " ), wxT( " " ) );
	}
	pos = code.Find( wxT( " }" ) );
	if ( pos != -1 ) {
		code.Replace( wxT( " }" ), wxT( "}" ) );
	}
	// handling malformed strings after the template be processed
	pos = code.Find( wxT( ", ]" ) );
	if ( pos != -1 ) {
		code.Replace( wxT( ", ]" ), wxT( "]" ) );
	}
	// handling malformed strings after the template be processed (for events)
	pos = code.Find( wxT( "|[]" ) );
	if ( pos != -1 ) {
		code.Replace( wxT( "|[]" ), wxT( "" ) );
	}
	// after the previous filter, it can left a list concatenation - we replace it
	pos = code.Find( wxT( "]|[" ) );
	if ( pos != -1 ) {
		code.Replace( wxT( "]|[" ), wxT( "," ) );
	}
	// concatenated flags in template get the space char in its front removed
	pos = code.Find( wxT( " | " ) );
	if ( pos != -1 ) {
		code.Replace( wxT( " | " ), wxT( " bor " ) );
	}  else {
		pos = code.Find( wxT( "| " ) );
		if ( pos != -1 ) {
			code.Replace( wxT( "| " ), wxT( " bor " ) );
		}
	}
	pos = code.Find( wxT( ",}" ) );
	if ( pos != -1 ) {
		code.Replace( wxT( ",}" ), wxT( "" ) );
	}
	pos = code.Find( wxT( ",]" ) );
	if ( pos != -1 ) {
		code.Replace( wxT( ",]" ), wxT( "]" ) );
	}
	// this() will always reference the main form/frame for which the controls are attached
	pos = code.Find( wxT( "this()" ) );
	if ( pos != -1 ) {
		code.Replace( wxT( "this()" ), m_rootWxParent );
	}
	return code;
}

wxString ErlangCodeGenerator::GetConstruction(PObjectBase obj, bool silent, wxString strSelf, ArrayItems& arrays)
{
	// Get the name
	const auto& propName = obj->GetProperty( wxT( "name" ) );
	if (!propName)
	{
		// Object has no name, just get its code
		return GetCode(obj, wxT( "construction" ), silent, strSelf);
	}

	strSelf[0] = wxToupper( strSelf[0] );
	// Object has a name, check if its an array
	const auto& name = propName->GetValue();
	wxString baseName;
	ArrayItem unused;
	if (!ParseArrayName(name, baseName, unused) )
	{
		// Object is not an array, just get its code
		return GetCode(obj, wxT( "construction" ), silent, strSelf);
	}

	// Object is an array, check if it needs to be declared
	auto& item = arrays[baseName];
	if (item.isDeclared)
	{
		// Object is already declared, just get its code
		return GetCode(obj, wxT( "construction" ), silent, strSelf);
	}

	// Array needs to be declared
	wxString code;

	// Get the Code
	code.append(GetCode(obj, wxT( "construction" ), silent, strSelf) );

	// Mark the array as declared
	item.isDeclared = true;

	return code;
}

void ErlangCodeGenerator::GenClassDeclaration(PObjectBase class_obj, bool /*use_enum*/,
						const wxString& /*classDecoration*/,
						const EventVector& events,
						const wxString& /*eventHandlerPostfix*/,
						ArrayItems& arrays,
						const wxString& createPrefix, bool createParent )
{
	wxString strClassName = class_obj->GetClassName();
	PProperty propName = class_obj->GetProperty( wxT( "name" ) );
	if ( !propName )
	{
		wxLogError( wxT( "Missing \"name\" property on \"%s\" class. Review your XML object description" ),
			strClassName.c_str() );
		return;
	}

	wxString strName = propName->GetValue();
	if ( strName.empty() )
	{
		wxLogError( wxT( "Object name can not be null" ) );
		return;
	}
	// we are not processing this template for Erlang ('generated_event_handlers')
	//	GetGenEventHandlers( class_obj );
	strName[0] = wxToupper( strName[0] );
	GenConstructor(class_obj, events, strName, arrays, createPrefix, createParent);

}

void ErlangCodeGenerator::GenExport( PObjectBase project, PCodeInfo code_info,
					const wxString& createPrefixg, bool createParent )
{
	if ( m_fullExport )
	{
		// Write the predefined functions on export session
		wxString code = code_info->GetTemplate( wxT( "full_export_header" ) );
		if ( !code.empty() )
		{
			ErlangTemplateParser parser( project, code, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
			code = parser.ParseTemplate();

			m_source->WriteLn( code );
			m_source->WriteLn( wxEmptyString );
		}

		// Generating "defines" for macros
		GenDefines( project );

		code = code_info->GetTemplate( wxT( "full_export_start" ) );
		if ( !code.empty() )
		{
			// Here we are going to try replace 'create_win_fun' tag in the 'init/1' defined
			// in the template in order to call the function which creates and starts the main window
			if ( project->GetChildCount() > 0 )
			{
				PProperty name = project->GetChild(0)->GetProperty( wxT( "name" ) );
				wxString winName = ClassToCreateFun( name->GetValueAsString(), createPrefixg );
				name = project->GetChild(0)->GetProperty( wxT( "aui_managed" ) );
				if ( createParent || name->GetValueAsInteger() )
					winName.append( wxT("(Options)") );
				else
					winName.append( wxT("()") );

				winName = wxT( "wx:batch(fun() -> #indent #indent #indent #nl " ) +
						winName + wxT( " #unindent #nl "
						"end). #unindent #unindent #nl " );
				code.Replace( wxT( "*create_win_fun" ), winName );
			}

			ErlangTemplateParser parser( project, code, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
			code = parser.ParseTemplate();

			m_source->WriteLn( code );
			m_source->WriteLn( wxEmptyString );
		}
	}
	else
	{
		// Write the forward declaration lines
		PProperty name;
		wxString names, headerVal = wxEmptyString;
		int numParam;
		std::set< wxString > exports;
		// Generate a list of the create functions for the export session
		for ( unsigned int i = 0; i < project->GetChildCount(); i++ )
		{
			PObjectBase child = project->GetChild( i );
			if ( child )
			{
				name = child->GetProperty( wxT( "name" ) );
				headerVal = name->GetValueAsString();
				name = child->GetProperty( wxT( "aui_managed" ) );
				numParam = 0;
				if ( createParent || name->GetValueAsInteger() ) ++numParam;
				if ( !headerVal.empty() )
				{
					names.append( wxString::Format( wxT( "%s/%u, " ), ClassToCreateFun( headerVal, createPrefixg ), numParam ) );
				}
			}
		}
		if ( !names.empty() )
		{
			names = names.Left( names.Len() -2 ),
			m_source->WriteLn( wxString::Format( wxT( "-export([%s])." ), names ) );
			m_source->WriteLn( wxEmptyString );
		}

		// Generating "defines" for macros
		GenDefines( project );
	}
}

wxString ErlangCodeGenerator::ClassToCreateFun( wxString objname, wxString createPrefix )
{
	objname.Replace( wxT( " " ), wxT( "_" ), true );

	if ( !createPrefix.empty() )
	{
		return  wxString::Format( wxT( "%s_%s" ), createPrefix.Lower(), objname.Lower() );
	}
	else
	{
		return  objname.Lower();
	}
}

wxString ErlangCodeGenerator::MakeErlangIdentifier( wxString idname )
{
	idname.Replace( wxT( " " ), wxT( "_" ), true );

	if ( m_lowerIdentifier ){
		return  idname.Lower();
	}
	else	// Ensures the first character is in lowercase otherwise we get a variable declaration
	{
		idname[0] = wxTolower( idname[0] );
		return idname;
	}
}

void ErlangCodeGenerator::GenIncludes( PObjectBase project, std::vector<wxString>* includes, std::set< wxString >* templates )
{
	GenObjectIncludes( project, includes, templates );
}

void ErlangCodeGenerator::GenObjectIncludes( PObjectBase project, std::vector< wxString >* includes, std::set< wxString >* templates )
{
	// Fill the set
	PCodeInfo code_info = project->GetObjectInfo()->GetCodeInfo( wxT( "Erlang" ) );
	if (code_info)
	{
		ErlangTemplateParser parser( project, code_info->GetTemplate( wxT( "include" ) ), m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
		wxString include = parser.ParseTemplate();
		if ( !include.empty() )
		{
			if ( templates->insert( include ).second )
			{
				AddUniqueIncludes( include, includes );
			}
		}
	}
}

void ErlangCodeGenerator::AddUniqueIncludes( const wxString& include, std::vector< wxString >* includes )
{
	// Split on newlines to only generate unique include lines
	// This strips blank lines and trims
	wxStringTokenizer tkz( include, wxT( "\n" ), wxTOKEN_STRTOK );

	while ( tkz.HasMoreTokens() )
	{
		wxString line = tkz.GetNextToken();
		line.Trim( false );
		line.Trim( true );

		// If it is an include, it must be unique to be written
		std::vector< wxString >::iterator it = std::find( includes->begin(), includes->end(), line );
		if ( includes->end() == it )
		{
			includes->push_back( line );
		}
	}
}

void ErlangCodeGenerator::FindDependencies( PObjectBase obj, std::set< PObjectInfo >& info_set )
{
	m_source->WriteLn( wxT( "%% FindDependencies" ) );
	unsigned int ch_count = obj->GetChildCount();
	if (ch_count > 0)
	{
		unsigned int i;
		for (i = 0; i<ch_count; i++)
		{
			PObjectBase child = obj->GetChild(i);
			info_set.insert(child->GetObjectInfo() );
			FindDependencies(child, info_set);
		}
	}
}

void ErlangCodeGenerator::GenConstructor(PObjectBase class_obj, const EventVector& events, wxString& strClassName,
					ArrayItems& arrays, const wxString& createPrefix, bool createParent )
{
	PObjectBase base_obj = class_obj;

	PProperty propName = class_obj->GetProperty( wxT( "name" ) );
	if ( !propName )
	{
		wxLogError( wxT( "Missing \"name\" property on \"%s\" class. Review your XML object description" ),
					class_obj->GetClassName().c_str() );
		return;
	}

	wxString strName = propName->GetValue();
	strName[0] = wxToupper( strName[0] );  // variables must start with uppercase char
	m_rootWxParent = strName;

	// The "after_addchild_erlang" template is used exclusively when 'aui_managed'
	// is defined (see: forms.erlangcode).
	// It will  close a 'try..catch..end' statement which we will insert the 'try' here.
	wxString afterAddChildErlang = GetCode( class_obj, wxT( "after_addchild_erlang" ) );
	bool auiManaged = !afterAddChildErlang.IsEmpty();
	wxString createObj = GetCode( class_obj, wxT( "cons_call" ) );

	// Creation the function header
	// By using AUI management, it will require the window to have a 'parent', so we
	// force the function header be the one with the parameter option
	wxString funHead = ClassToCreateFun( strClassName, createPrefix);
	m_source->WriteLn();
	if ( createParent || auiManaged )
	{
		funHead.append( wxT( "(Options) ->") );
		m_source->WriteLn( funHead );
		m_source->Indent();
		m_source->WriteLn( wxT( "Parent = proplists:get_value(parent, Options, wx:null())," ) );
		if ( auiManaged )
		{
			m_source->WriteLn( wxT( "%% Warning: AUI managed form requires a valid parent window" ) );
		}
		createObj.Replace( wxT( "wx:null()" ), wxT( "Parent" ) );
	}
	else
	{
		funHead.append( wxT( "() ->" ) );
		m_source->WriteLn( funHead );
		m_source->Indent();
	}
	m_source->WriteLn(strName + wxT( " = " ) + createObj );

	wxString settings = GetCode( class_obj, wxT( "settings" ) );
	if ( !settings.IsEmpty() )
	{
		m_source->WriteLn( settings );
		// when aui_managed is defined, in the AUI template we use a
		// try..catch statement. So, we are going to indent the further
		// lines of code(see: forms.erlangcode)
		if ( auiManaged && settings.Find( wxT( "try" ) ) )
			m_source->Indent();
	}

/*  // wizard is not supported by Erlang
	if ( class_obj->GetObjectTypeName() == wxT( "wizard" ) && class_obj->GetChildCount() > 0 )
	{
		m_source->WriteLn( wxT( "function add_page(page)" ) );
		m_source->Indent();
		m_source->WriteLn( wxT( "if ( #" ) + m_strUITable + wxT( "." ) + strClassName + wxT( ".m_pages) > 0 then" ) );
		m_source->Indent();
		m_source->WriteLn( wxT( "local previous_page = " ) + m_strUITable + wxT( "." ) + strClassName + wxT( ".m_pages[ #" ) + m_strUITable + wxT( "." ) + strClassName +wxT( ".m_pages ]" ) );
		m_source->WriteLn( wxT( "page:SetPrev(previous_page)" ) );
		m_source->WriteLn( wxT( "previous_page:SetNext(page)" ) );
		m_source->Unindent();
		m_source->WriteLn( wxT( "end" ) );
		m_source->WriteLn( wxT( "table.insert( " ) + m_strUITable + wxT( "." ) + strClassName + wxT( ".m_pages, page)" ) );
		m_source->Unindent();
		m_source->WriteLn( wxT( "end" ) );
	}
*/
	for ( unsigned int i = 0; i < class_obj->GetChildCount(); i++ )
	{
		GenConstruction(base_obj, class_obj->GetChild(i), true , strClassName, arrays);
	}

	wxString afterAddChild = GetCode( class_obj, wxT( "after_addchild" ) );
	if ( auiManaged ) m_source->WriteLn( afterAddChild );

	GenEvents( class_obj, events );

	// Ending the function
	// the information about the aui_managed window must be handled here
	wxString funEnding;
	if ( m_fullExport )
	{
		// we finish the function by returning the window and the initialized State record
		if ( auiManaged )
			funEnding = wxString::Format( wxT( "{%s, #state{win=%s, params=[{aui,%s_AUImgr}]}}" ), strName, strName, strName );
		else
			funEnding = wxString::Format( wxT( "{%s, #state{win=%s}}" ), strName, strName );
	}
	else
	{
		// we finish the function by returning the window (container
		// passed as parameter or created here)
		if ( auiManaged )
			funEnding = wxString::Format( wxT( "{%s,%s_AUImgr}" ), strName, strName );
		else
			funEnding = wxString::Format( wxT( "%s" ), strName );
	}

	if ( auiManaged )
	{
		// Unindenting the code block and updating #state{} in the 'try..catch..end' statement
		m_source->Unindent();
		afterAddChildErlang.Replace( wxT("*state"), funEnding );
		m_source->WriteLn( afterAddChildErlang );
	}
	else
	{
		m_source->WriteLn( funEnding.append( wxT( "." ) ) );
	}
	m_source->Unindent();
}

void ErlangCodeGenerator::GenDestructor( PObjectBase class_obj, const EventVector &events )
{
	m_source->WriteLn();
	// generate function definition
	m_source->Indent();

	wxString strClassName;
	if ( m_disconnectEvents && !events.empty() )
	{
		GenEvents( class_obj, events, true );
	}
	else if (class_obj->GetPropertyAsInteger( wxT( "aui_managed" ) ) == 0)
	{
		m_source->WriteLn( wxT( "pass" ) );
	}

	// destruct objects
	GenDestruction( class_obj );

	m_source->Unindent();
}

void ErlangCodeGenerator::GenConstruction(PObjectBase base_obj, PObjectBase obj, bool is_widget, wxString& strClassName, ArrayItems& arrays)
{
	wxString type = obj->GetObjectTypeName();
	PObjectInfo info = obj->GetObjectInfo();

	if ( ObjectDatabase::HasCppProperties( type ) )
	{

		wxString strName;
		PProperty propName = obj->GetProperty( wxT( "name" ) );
		if (propName)
		{
			strName = propName->GetValue();
		}

		wxString strClass = obj->GetClassName();


		std::vector<wxString>::iterator itr;
		if (m_strUnsupportedClasses.end() != (itr = std::find(m_strUnsupportedClasses.begin(),m_strUnsupportedClasses.end(),strClass) ))
		{
			strName[0] = wxToupper( strName[0] );
			m_source->WriteLn( wxT( "%% Instance " ) + strName + wxT( " of Control " ) + *itr + wxT( " you are trying to use isn't wrapped by wxErlang." ) );
			m_source->WriteLn( wxT( "%% Please try to use another control" ) );
			m_strUnsupportedInstances.push_back(strName);
			return;
		}
		strClass[0] = wxToupper( strClass[0] );
		strClassName[0] = wxToupper( strClassName[0] );

		m_source->WriteLn(GetConstruction(obj, false, strClassName, arrays) );

		GenSettings( obj->GetObjectInfo(), obj, strClassName );

		bool isWidget = !info->IsSubclassOf( wxT( "sizer" ) );

		for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
		{
			PObjectBase child = obj->GetChild( i );
			GenConstruction(obj, child, isWidget, strClassName, arrays);

			if ( type == wxT( "toolbar" ) )
			{
				GenAddToolbar(child->GetObjectInfo(), child);
			}
		}

		if ( !isWidget ) // sizers
		{
			wxString afterAddChild = GetCode( obj, wxT( "after_addchild" ) );
			if ( !afterAddChild.empty() )
			{
				m_source->WriteLn( afterAddChild );
			}

			if (is_widget)
			{
				// the parent object is not a sizer. There is no template for
				// this so we'll make it manually.
				// It's not a good practice to embed templates into the source code,
				// because you will need to recompile...
				wxString  _template = wxT( "wxWindow:setSizer(#wxparent $name, $name),#nl " )
							wxT( "#class:layout($name), " )
							wxT( "#ifnull #parent $size" )
							wxT( "@{ #nl wxSizer:fit($name, #wxparent $name), @} " );
				ErlangTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
				wxString res  = parser.ParseTemplate();

				// we check if the owner of this sizer is the "Parent" form by
				// checking if its parent is a project.
				// It's trick, but is the way we can correctly translate the codes
				// since Erlang doesn't has object class
				if ( obj->GetParent()->GetParent() )
				{
					wxString pType = obj->GetParent()->GetParent()->GetObjectTypeName();
					// When the project is the parent, then the sizer is the first one
					// and then we "link" it to the Parent window
					if ( pType == wxT( "project" ) )
					{
						// the 'this()' came from RootWxParentToCode()
						res.Replace( wxT( "this()" ), m_rootWxParent );
					}
				}
				m_source->WriteLn(res);
			}
		}
		else if ( type == wxT( "splitter" ) )
		{
			// Generating the split
			switch ( obj->GetChildCount() )
			{
				case 1:
				{
					PObjectBase sub1 = obj->GetChild(0)->GetChild(0);
					wxString _template = wxT( "#class:initialize($name, " );
					_template = _template + sub1->GetProperty( wxT( "name" ) )->GetValue() + wxT( " )" );

					ErlangTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
					m_source->WriteLn(parser.ParseTemplate() );
					break;
				}
				case 2:
				{
					PObjectBase sub1,sub2;
					sub1 = obj->GetChild(0)->GetChild(0);
					sub2 = obj->GetChild(1)->GetChild(0);

					wxString _template;
					wxString strMode = obj->GetProperty( wxT( "splitmode" ) )->GetValue();
					bool bSplitVertical = (strMode == wxT( "wxSPLIT_VERTICAL" ) );
					if (bSplitVertical)
					{
						_template = wxT( "#class:splitVertically($name, " );
					}
					else
					{
						_template = wxT( "#class:splitHorizontally($name, " );
					}
					// ensuring the variables starts with uppercase char
					wxString Form1 = sub1->GetProperty( wxT( "name" ) )->GetValue();
					Form1[0] = wxToupper( Form1[0] );
					wxString Form2 = sub2->GetProperty( wxT( "name" ) )->GetValue();
					Form2[0] = wxToupper( Form2[0] );

					_template = _template + Form1 + wxT( ", " ) + Form2 + wxT( ", [{sashPosition,$sashpos }])," );
					_template = _template + wxT( "#nl " ) + wxT( "#class:setSplitMode($name, " ) + wxString::Format( wxT( "?%s" ),(bSplitVertical ? "wxSPLIT_VERTICAL" : "wxSPLIT_HORIZONTAL") ) + wxT( ")," );

					ErlangTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
					m_source->WriteLn(parser.ParseTemplate() );
					break;
				}
				default:
					wxLogError( wxT( "Missing subwindows for wxSplitterWindow widget." ) );
					break;
			}
		}
		else if ( type == wxT( "menubar" )	||
				type == wxT( "menu" )		||
				type == wxT( "submenu" )	||
				type == wxT( "toolbar" )	||
				type == wxT( "tool" )	||
				type == wxT( "listbook" )	||
				type == wxT( "simplebook" ) ||
				type == wxT( "notebook" )	||
				type == wxT( "auinotebook" )	||
				type == wxT( "treelistctrl" )	||
				type == wxT( "flatnotebook" ) ||
				type == wxT( "wizard" )
			)
		{
			wxString afterAddChild = GetCode( obj, wxT( "after_addchild" ) );
			if ( !afterAddChild.empty() )
			{
				m_source->WriteLn( afterAddChild );
			}
		}
	}
	else if ( info->IsSubclassOf( wxT( "sizeritembase" ) ) )
	{
		// The child must be added to the sizer having in mind the
		// child object type (there are 3 different routines)
		GenConstruction(obj, obj->GetChild(0), false, strClassName, arrays);

		PObjectInfo childInfo = obj->GetChild(0)->GetObjectInfo();
		wxString temp_name;
		if ( childInfo->IsSubclassOf( wxT( "wxWindow" ) ) || wxT( "CustomControl" ) == childInfo->GetClassName() )
		{
			temp_name = wxT( "window_add" );
		}
		else if ( childInfo->IsSubclassOf( wxT( "sizer" ) ) )
		{
			temp_name = wxT( "sizer_add" );
		}
		else if ( childInfo->GetClassName() == wxT( "spacer" ) )
		{
			temp_name = wxT( "spacer_add" );
		}
		else
		{
			LogDebug( wxT( "SizerItem child is not a Spacer and is not a subclass of wxWindow or of sizer." ) );
			return;
		}

		wxString temp_code = GetCode( obj, temp_name );
		if ( !temp_code.empty() )
		{
			m_source->WriteLn( temp_code );
		}
	}
	else if (	type == wxT( "notebookpage" )		||
				type == wxT( "flatnotebookpage" )	||
				type == wxT( "listbookpage" )		||
				type == wxT( "simplebookpage" )	||
				type == wxT( "choicebookpage" )	||
				type == wxT( "auinotebookpage" )
			)
	{
		GenConstruction(obj, obj->GetChild(0), false, strClassName, arrays);
		m_source->WriteLn( GetCode( obj, wxT( "page_add" ) ) );
		GenSettings( obj->GetObjectInfo(), obj, strClassName );
	}
	else if ( type == wxT( "treelistctrlcolumn" ) )
	{
		m_source->WriteLn( GetCode( obj, wxT( "column_add" ) ) );
		GenSettings( obj->GetObjectInfo(), obj, strClassName );
	}
	else if ( type == wxT( "tool" ) )
	{
		// If loading bitmap from ICON resource, and size is not set, set size to toolbars bitmapsize
		// So hacky, yet so useful ...
		wxSize toolbarsize = obj->GetParent()->GetPropertyAsSize( _( "bitmapsize" ) );
		if ( wxDefaultSize != toolbarsize )
		{
			PProperty prop = obj->GetProperty( _( "bitmap" ) );
			if ( prop )
			{
				wxString oldVal = prop->GetValueAsString();
				wxString path, source;
				wxSize toolsize;
				TypeConv::ParseBitmapWithResource( oldVal, &path, &source, &toolsize );
				if ( _( "Load From Icon Resource" ) == source && wxDefaultSize == toolsize )
				{
					prop->SetValue( wxString::Format( wxT( "%s; %s [%i; %i]" ), path.c_str(), source.c_str(), toolbarsize.GetWidth(), toolbarsize.GetHeight() ) );
					m_source->WriteLn(GetConstruction(obj, false, wxEmptyString, arrays) );
					prop->SetValue( oldVal );
					return;
				}
			}
		}
		m_source->WriteLn(GetConstruction(obj, false, wxEmptyString, arrays) );
	}
	else
	{
		// Generate the children
		for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
		{
			GenConstruction(obj, obj->GetChild( i ), false, strClassName, arrays);
		}
	}
}

void ErlangCodeGenerator::GenDestruction( PObjectBase obj )
{
	wxString _template;
	PCodeInfo code_info = obj->GetObjectInfo()->GetCodeInfo( wxT( "Erlang" ) );

	if ( code_info )
	{
		_template = code_info->GetTemplate( wxT( "destruction" ) );

		if ( !_template.empty() )
		{
			ErlangTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
			wxString code = parser.ParseTemplate();
			if ( !code.empty() )
			{
				m_source->WriteLn( code );
			}
		}
	}

	// Process child widgets
	for ( unsigned int i = 0; i < obj->GetChildCount() ; i++ )
	{
		PObjectBase child = obj->GetChild( i );
		GenDestruction( child );
	}
}

void ErlangCodeGenerator::FindMacros( PObjectBase obj, std::vector<wxString>* macros )
{
	// iterate through all of the properties of all objects, add the macros
	// to the vector
	unsigned int i;

	for ( i = 0; i < obj->GetPropertyCount(); i++ )
	{
		PProperty prop = obj->GetProperty( i );
		if ( ( prop->GetType() == PT_MACRO ) || ( prop->GetName() == wxT( "aui_managed" ) ) )
		{
			wxString value = prop->GetValue();
			if ( value.IsEmpty() ) continue;

			// in order to define a macro when aui_management is used we added it to
			// the macro lists. Then we will define it as a value - not id in GenDefines/1
			if ( prop->GetType() != PT_MACRO )
			{
				if ( !prop->GetValueAsInteger() ) continue;

				value = prop->GetName();
			}

			// Skip wx IDs
			if ( ( ! value.Contains( wxT( "XRCID" ) ) ) &&
				 ( m_predMacros.end() == m_predMacros.find( value ) ) )
			{
				if ( macros->end() == std::find( macros->begin(), macros->end(), value ) )
				{
					macros->push_back( value );
				}
			}
		}
	}



	for ( i = 0; i < obj->GetChildCount(); i++ )
	{
		FindMacros( obj->GetChild( i ), macros );
	}
}

void ErlangCodeGenerator::FindEventHandlers(PObjectBase obj, EventVector &events)
{
	unsigned int i;
	unsigned int evt_cnt = obj->GetEventCount();
	for (i=0; i < evt_cnt; i++)
	{
		PEvent event = obj->GetEvent(i);
		if ( !event->GetValue().IsEmpty() )
			events.push_back(event);
	}

	for (i=0; i < obj->GetChildCount(); i++)
	{
		PObjectBase child = obj->GetChild(i);
		FindEventHandlers(child,events);
	}
}

void ErlangCodeGenerator::GenDefines( PObjectBase project )
{
	std::vector< wxString > macros;
	FindMacros( project, &macros );
	m_strUserIDsVec.erase(m_strUserIDsVec.begin(),m_strUserIDsVec.end() );

	unsigned int id = m_firstID;
	wxString strFirstComment;
	if ( id < wxID_HIGHEST )
	{
		// it's not recommended to use values between wxID_LOWEST and wxID_HIGHEST and, accordingly.
		// See https://docs.wxwidgets.org/3.0/page_stdevtid.html
		strFirstComment = wxT( " % wx docs suggests the IDs values must start from wxID_HIGHEST+1 (6000)" );
	}
	else
	{
		strFirstComment = wxT( " % starting from wxID_HIGHEST+1" );
	}

	std::vector< wxString >::iterator it;
	for (it = macros.begin() ; it != macros.end(); it++)
	{
		if ( *it != wxT( "aui_managed" ) )
		{
			m_source->WriteLn( wxString::Format( wxT( "-define(%s, %i). %s" ), it->c_str(), id, strFirstComment ) );
			id++;
		}
		else
			m_source->WriteLn( wxString::Format( wxT( "-define(pi, wxAuiPaneInfo). %s" ), strFirstComment ) );
		m_strUserIDsVec.push_back(*it);
		strFirstComment = wxEmptyString;
	}

	if (!macros.empty() )
	{
		m_source->WriteLn(wxEmptyString);
	}
}

void ErlangCodeGenerator::GenSettings(PObjectInfo info, PObjectBase obj, wxString &strClassName )
{
	wxString _template;
	PCodeInfo code_info = info->GetCodeInfo( wxT( "Erlang" ) );

	if ( !code_info )
	{
		return;
	}

	_template = code_info->GetTemplate( wxT( "settings" ) );

	if ( !_template.empty() )
	{
		ErlangTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
		wxString code = parser.ParseTemplate();

		wxString strRootCode = m_rootWxParent;
		if (code.Find(strRootCode) != -1){
			code.Replace(strRootCode, strClassName);
		}

		if ( !code.empty() )
		{
			m_source->WriteLn(code);
		}
	}

	// Proceeding recursively with the base classes
	for (unsigned int i=0; i< info->GetBaseClassCount(false); i++)
	{
		PObjectInfo base_info = info->GetBaseClass(i, false);
		GenSettings(base_info, obj, strClassName);
	}
}

void ErlangCodeGenerator::GenAddToolbar( PObjectInfo info, PObjectBase obj )
{
	wxArrayString arrCode;

	GetAddToolbarCode( info, obj, arrCode );

	for( size_t i = 0; i < arrCode.GetCount(); i++ ) m_source->WriteLn( arrCode[i] );
}

void ErlangCodeGenerator::GetAddToolbarCode( PObjectInfo info, PObjectBase obj, wxArrayString& codelines )
{
	wxString _template;
	PCodeInfo code_info = info->GetCodeInfo( wxT( "Erlang" ) );

	if ( !code_info )
		return;

	_template = code_info->GetTemplate( wxT( "toolbar_add" ) );

	if ( !_template.empty() )
	{
		ErlangTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
		wxString code = parser.ParseTemplate();
		if ( !code.empty() )
		{
			if ( codelines.Index( code ) == wxNOT_FOUND ) codelines.Add( code );
		}
	}

	// Proceeding recursively with the base classes
	for ( unsigned int i = 0; i < info->GetBaseClassCount(false); i++ )
	{
		PObjectInfo base_info = info->GetBaseClass( i, false );
		GetAddToolbarCode( base_info, obj, codelines );
	}
}

///////////////////////////////////////////////////////////////////////

void ErlangCodeGenerator::UseRelativePath(bool relative, wxString basePath)
{
	m_useRelativePath = relative;

	if (m_useRelativePath)
	{
		if (wxFileName::DirExists(basePath) )
		{
			m_basePath = basePath;
		}
		else
		{
			m_basePath = wxEmptyString;
		}
	}
}

#define ADD_PREDEFINED_MACRO(x) m_predMacros.insert( wxT(#x) )
void ErlangCodeGenerator::SetupPredefinedMacros()
{
	/* no id matches this one when compared to it */
	ADD_PREDEFINED_MACRO(wxID_NONE);

	/*  id for a separator line in the menu (invalid for normal item) */
	ADD_PREDEFINED_MACRO(wxID_SEPARATOR);

	ADD_PREDEFINED_MACRO(wxID_ANY);

	ADD_PREDEFINED_MACRO(wxID_LOWEST);

	ADD_PREDEFINED_MACRO(wxID_OPEN);
	ADD_PREDEFINED_MACRO(wxID_CLOSE);
	ADD_PREDEFINED_MACRO(wxID_NEW);
	ADD_PREDEFINED_MACRO(wxID_SAVE);
	ADD_PREDEFINED_MACRO(wxID_SAVEAS);
	ADD_PREDEFINED_MACRO(wxID_REVERT);
	ADD_PREDEFINED_MACRO(wxID_EXIT);
	ADD_PREDEFINED_MACRO(wxID_UNDO);
	ADD_PREDEFINED_MACRO(wxID_REDO);
	ADD_PREDEFINED_MACRO(wxID_HELP);
	ADD_PREDEFINED_MACRO(wxID_PRINT);
	ADD_PREDEFINED_MACRO(wxID_PRINT_SETUP);
	ADD_PREDEFINED_MACRO(wxID_PREVIEW);
	ADD_PREDEFINED_MACRO(wxID_ABOUT);
	ADD_PREDEFINED_MACRO(wxID_HELP_CONTENTS);
	ADD_PREDEFINED_MACRO(wxID_HELP_COMMANDS);
	ADD_PREDEFINED_MACRO(wxID_HELP_PROCEDURES);
	ADD_PREDEFINED_MACRO(wxID_HELP_CONTEXT);
	ADD_PREDEFINED_MACRO(wxID_CLOSE_ALL);
	ADD_PREDEFINED_MACRO(wxID_PAGE_SETUP);
	ADD_PREDEFINED_MACRO(wxID_HELP_INDEX);
	ADD_PREDEFINED_MACRO(wxID_HELP_SEARCH);
	ADD_PREDEFINED_MACRO(wxID_PREFERENCES);

	ADD_PREDEFINED_MACRO(wxID_EDIT);
	ADD_PREDEFINED_MACRO(wxID_CUT);
	ADD_PREDEFINED_MACRO(wxID_COPY);
	ADD_PREDEFINED_MACRO(wxID_PASTE);
	ADD_PREDEFINED_MACRO(wxID_CLEAR);
	ADD_PREDEFINED_MACRO(wxID_FIND);

	ADD_PREDEFINED_MACRO(wxID_DUPLICATE);
	ADD_PREDEFINED_MACRO(wxID_SELECTALL);
	ADD_PREDEFINED_MACRO(wxID_DELETE);
	ADD_PREDEFINED_MACRO(wxID_REPLACE);
	ADD_PREDEFINED_MACRO(wxID_REPLACE_ALL);
	ADD_PREDEFINED_MACRO(wxID_PROPERTIES);

	ADD_PREDEFINED_MACRO(wxID_VIEW_DETAILS);
	ADD_PREDEFINED_MACRO(wxID_VIEW_LARGEICONS);
	ADD_PREDEFINED_MACRO(wxID_VIEW_SMALLICONS);
	ADD_PREDEFINED_MACRO(wxID_VIEW_LIST);
	ADD_PREDEFINED_MACRO(wxID_VIEW_SORTDATE);
	ADD_PREDEFINED_MACRO(wxID_VIEW_SORTNAME);
	ADD_PREDEFINED_MACRO(wxID_VIEW_SORTSIZE);
	ADD_PREDEFINED_MACRO(wxID_VIEW_SORTTYPE);

	ADD_PREDEFINED_MACRO(wxID_FILE);
	ADD_PREDEFINED_MACRO(wxID_FILE1);
	ADD_PREDEFINED_MACRO(wxID_FILE2);
	ADD_PREDEFINED_MACRO(wxID_FILE3);
	ADD_PREDEFINED_MACRO(wxID_FILE4);
	ADD_PREDEFINED_MACRO(wxID_FILE5);
	ADD_PREDEFINED_MACRO(wxID_FILE6);
	ADD_PREDEFINED_MACRO(wxID_FILE7);
	ADD_PREDEFINED_MACRO(wxID_FILE8);
	ADD_PREDEFINED_MACRO(wxID_FILE9);

	/*  Standard button and menu IDs */

	ADD_PREDEFINED_MACRO(wxID_OK);
	ADD_PREDEFINED_MACRO(wxID_CANCEL);

	ADD_PREDEFINED_MACRO(wxID_APPLY);
	ADD_PREDEFINED_MACRO(wxID_YES);
	ADD_PREDEFINED_MACRO(wxID_NO);
	ADD_PREDEFINED_MACRO(wxID_STATIC);
	ADD_PREDEFINED_MACRO(wxID_FORWARD);
	ADD_PREDEFINED_MACRO(wxID_BACKWARD);
	ADD_PREDEFINED_MACRO(wxID_DEFAULT);
	ADD_PREDEFINED_MACRO(wxID_MORE);
	ADD_PREDEFINED_MACRO(wxID_SETUP);
	ADD_PREDEFINED_MACRO(wxID_RESET);
	ADD_PREDEFINED_MACRO(wxID_CONTEXT_HELP);
	ADD_PREDEFINED_MACRO(wxID_YESTOALL);
	ADD_PREDEFINED_MACRO(wxID_NOTOALL);
	ADD_PREDEFINED_MACRO(wxID_ABORT);
	ADD_PREDEFINED_MACRO(wxID_RETRY);
	ADD_PREDEFINED_MACRO(wxID_IGNORE);
	ADD_PREDEFINED_MACRO(wxID_ADD);
	ADD_PREDEFINED_MACRO(wxID_REMOVE);

	ADD_PREDEFINED_MACRO(wxID_UP);
	ADD_PREDEFINED_MACRO(wxID_DOWN);
	ADD_PREDEFINED_MACRO(wxID_HOME);
	ADD_PREDEFINED_MACRO(wxID_REFRESH);
	ADD_PREDEFINED_MACRO(wxID_STOP);
	ADD_PREDEFINED_MACRO(wxID_INDEX);

	ADD_PREDEFINED_MACRO(wxID_BOLD);
	ADD_PREDEFINED_MACRO(wxID_ITALIC);
	ADD_PREDEFINED_MACRO(wxID_JUSTIFY_CENTER);
	ADD_PREDEFINED_MACRO(wxID_JUSTIFY_FILL);
	ADD_PREDEFINED_MACRO(wxID_JUSTIFY_RIGHT);

	ADD_PREDEFINED_MACRO(wxID_JUSTIFY_LEFT);
	ADD_PREDEFINED_MACRO(wxID_UNDERLINE);
	ADD_PREDEFINED_MACRO(wxID_INDENT);
	ADD_PREDEFINED_MACRO(wxID_UNINDENT);
	ADD_PREDEFINED_MACRO(wxID_ZOOM_100);
	ADD_PREDEFINED_MACRO(wxID_ZOOM_FIT);
	ADD_PREDEFINED_MACRO(wxID_ZOOM_IN);
	ADD_PREDEFINED_MACRO(wxID_ZOOM_OUT);
	ADD_PREDEFINED_MACRO(wxID_UNDELETE);
	ADD_PREDEFINED_MACRO(wxID_REVERT_TO_SAVED);

	/*  System menu IDs (used by wxUniv): */
	ADD_PREDEFINED_MACRO(wxID_SYSTEM_MENU);
	ADD_PREDEFINED_MACRO(wxID_CLOSE_FRAME);
	ADD_PREDEFINED_MACRO(wxID_MOVE_FRAME);
	ADD_PREDEFINED_MACRO(wxID_RESIZE_FRAME);
	ADD_PREDEFINED_MACRO(wxID_MAXIMIZE_FRAME);
	ADD_PREDEFINED_MACRO(wxID_ICONIZE_FRAME);
	ADD_PREDEFINED_MACRO(wxID_RESTORE_FRAME);

	/*  IDs used by generic file dialog (13 consecutive starting from this value) */
	ADD_PREDEFINED_MACRO(wxID_FILEDLGG);

	ADD_PREDEFINED_MACRO(wxID_HIGHEST);

}
