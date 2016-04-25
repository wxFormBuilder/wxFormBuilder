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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   José Antonio Hurtado - joseantonio.hurtado@gmail.com
//   Juan Antonio Ortega  - jortegalalmolda@gmail.com
//
// Lua code generation written by
//   Vratislav Zival
//
///////////////////////////////////////////////////////////////////////////////

#include "luacg.h"
#include "codewriter.h"
#include "utils/typeconv.h"
#include "utils/debug.h"
#include "rad/appdata.h"
#include "model/objectbase.h"
#include "model/database.h"
#include "utils/wxfbexception.h"

#include <algorithm>

#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <wx/defs.h>

LuaTemplateParser::LuaTemplateParser( PObjectBase obj, wxString _template, bool useI18N, bool useRelativePath, wxString basePath, std::vector<wxString> strUserIDsVec )
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

	SetupModulePrefixes();
}

LuaTemplateParser::LuaTemplateParser( const LuaTemplateParser & that, wxString _template, std::vector<wxString> strUserIDsVec )
:
TemplateParser( that, _template ),
m_i18n( that.m_i18n ),
m_useRelativePath( that.m_useRelativePath ),
m_basePath( that.m_basePath ),
m_strUserIDsVec(strUserIDsVec)
{
	SetupModulePrefixes();
}

wxString LuaTemplateParser::RootWxParentToCode()
{
	return wxT("NS.");
}

PTemplateParser LuaTemplateParser::CreateParser( const TemplateParser* oldparser, wxString _template )
{
	const LuaTemplateParser* luaOldParser = dynamic_cast< const LuaTemplateParser* >( oldparser );
	if ( luaOldParser != NULL )
	{
		std::vector<wxString> empty;
		PTemplateParser newparser( new LuaTemplateParser( *luaOldParser, _template, empty));
		return newparser;
	}
	return PTemplateParser();
}

/**
* Convert the value of the property to Lua code
*/
wxString LuaTemplateParser::ValueToCode( PropertyType type, wxString value )
{
	wxString result;

	switch ( type )
	{
	case PT_WXPARENT:
		{
			result = wxT("NS.") + value;
			break;
		}
	case PT_WXPARENT_SB:
		{
			result = value + wxT(":GetStaticBox()");
			break;
		}
	case PT_WXSTRING:
	case PT_FILE:
	case PT_PATH:
		{
			if ( value.empty() )
			{
				result << wxT("\"\"");
			}
			else
			{
				result << wxT("\"") << LuaCodeGenerator::ConvertLuaString( value ) << wxT("\"");
			}
			break;
		}
	case PT_WXSTRING_I18N:
		{
			if ( value.empty() )
			{
				result << wxT("\"\"");
			}
			else
			{
				if ( m_i18n )
				{
					result << wxT("\"") << LuaCodeGenerator::ConvertLuaString(value) << wxT("\"");
				}
				else
				{
					result << wxT("\"") << LuaCodeGenerator::ConvertLuaString(value) << wxT("\"");
				}
			}
			break;
		}
	case PT_CLASS:
	case PT_MACRO:
	case PT_OPTION:
	case PT_EDIT_OPTION:
		{
			result = value;
			wxString pred = m_predModulePrefix[value];

			if( !pred.empty() )
				result.Replace( wxT("wx"), pred );
			else
			{
				//prepend "wx." if value isn't UserID and contains "wx" prefix
				if(m_strUserIDsVec.end() == std::find(m_strUserIDsVec.begin(),m_strUserIDsVec.end(),value) && wxNOT_FOUND != result.Find(wxT("wx"))){
					result.Prepend(wxT("wx."));
				}
			}

			break;
		}
	case PT_TEXT:
	case PT_FLOAT:
	case PT_INT:
	case PT_UINT:
		{
			result = value;
			break;
		}
	case PT_BITLIST:
		{
			if(value.empty() ){
				result =  wxT("0");
				break;
			}else{
				result = value;
			}
			wxString pred, bit, res, pref;
			wxStringTokenizer bits( result, wxT("|"), wxTOKEN_STRTOK );

			while( bits.HasMoreTokens() )
			{
				bit = bits.GetNextToken();
				pred = m_predModulePrefix[bit];

				if(pred.empty() )
				{
					res += pref + wxT("wx.") + bit;
				}else{
					res += pref + pred + bit;
				}
				pref = wxT(" + ");
			}
			result = res;
			break;
		}
	case PT_WXPOINT:
		{
			if ( value.empty() )
			{
				result = wxT("wx.wxDefaultPosition");
			}
			else
			{
				result << wxT("wx.wxPoint( ") << value << wxT(" )");
			}
			break;
		}
	case PT_WXSIZE:
		{
			if ( value.empty() )
			{
				result = wxT("wx.wxDefaultSize");
			}
			else
			{
				result << wxT("wx.wxSize( ") << value << wxT(" )");
			}
			break;
		}
	case PT_BOOL:
		{
			result = ( value == wxT("0") ? wxT("False") : wxT("True") );
			break;
		}
	case PT_WXFONT:
		{
			if ( !value.empty() )
			{
				wxFontContainer fontContainer = TypeConv::StringToFont( value );
				wxFont font = fontContainer.GetFont();

				const int pointSize = fontContainer.GetPointSize();

				result = wxString::Format( "wx.wxFont( %s, %s, %s, %s, %s, %s )",
							((pointSize <= 0) ? "wx.wxNORMAL_FONT:GetPointSize()" : (wxString() << pointSize)),
							"wx." + TypeConv::FontFamilyToString( fontContainer.GetFamily() ),
							"wx." + font.GetStyleString(),
							"wx." + font.GetWeightString(),
							( fontContainer.GetUnderlined() ? "True" : "False" ),
							( fontContainer.m_faceName.empty() ? "\"\"" : ("\"" + fontContainer.m_faceName + "\"") )
						);
			}
			else
			{
				result = wxT("wx.wxNORMAL_FONT");
			}
			break;
		}
		case PT_WXCOLOUR:
		{
			if ( !value.empty() )
			{
				if ( value.find_first_of( wxT("wx") ) == 0 )
				{
					// System Colour
					result << wxT("wx.wxSystemSettings.GetColour( ") << ValueToCode( PT_OPTION, value ) << wxT(" )");
				}
				else
				{
					wxColour colour = TypeConv::StringToColour( value );
					result = wxString::Format( wxT("wx.wxColour( %i, %i, %i )"), colour.Red(), colour.Green(), colour.Blue() );
				}
			}
			else
			{
				result = wxT("wx.wxColour()");
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
				// Empty path, generate Null Bitmap
				result = wxT("wx.wxNullBitmap");
				break;
			}

			if ( path.StartsWith( wxT("file:") ) )
			{
				wxLogWarning( wxT("Lua code generation does not support using URLs for bitmap properties:\n%s"), path.c_str() );
				result = wxT("wx.wxNullBitmap");
				break;
			}

			if ( source == _("Load From File") || source == _("Load From Embedded File"))
			{
				wxString absPath;
				try
				{
					absPath = TypeConv::MakeAbsolutePath( path, AppData()->GetProjectPath() );
				}
				catch( wxFBException& ex )
				{
					wxLogError( ex.what() );
					result = wxT( "wx.wxNullBitmap" );
					break;
				}

				wxString file = ( m_useRelativePath ? TypeConv::MakeRelativePath( absPath, m_basePath ) : absPath );

				result << wxT("wx.wxBitmap( \"") << LuaCodeGenerator::ConvertLuaString( file ) << wxT("\", wx.wxBITMAP_TYPE_ANY )");
			}
			else if ( source == _("Load From Resource") )
			{
				result << wxT("wx.wxBitmap( \"") << path << wxT("\", wx.wxBITMAP_TYPE_RESOURCE )");
			}
			else if ( source == _("Load From Icon Resource") )
			{
				result << wxT("wx.wxBitmap( \"") << path << wxT("\")");
				//load from icon isn't supported by wxLua
				//wxStaticBitmap(wxWindow* parent, wxWindowID id, const wxBitmap& label = wxNullBitmap, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "wxStaticBitmap")
				/*
				if ( wxDefaultSize == icoSize )
				{
					result << wxT("wx.wxICON( ") << path << wxT(" )");
				}
				else
				{
					result.Printf( wxT("wx.Icon( u\"%s\", wx.wxBITMAP_TYPE_ICO_RESOURCE, %i, %i )"), path.c_str(), icoSize.GetWidth(), icoSize.GetHeight() );
				}*/
			}
			else if ( source == _("Load From Art Provider") )
			{
				wxString rid = path.BeforeFirst( wxT(':') );

				if( rid.StartsWith( wxT("gtk-") ) )
				{
					rid = wxT("\"") + rid + wxT("\"");
				}
				else
					rid.Replace( wxT("wx"), wxT("wx.wx") );

				wxString cid = path.AfterFirst( wxT(':') );
				cid.Replace( wxT("wx"), wxT("wx.wx") );

				result = wxT("wx.wxArtProvider.GetBitmap( ") + rid + wxT(", ") +  cid + wxT(" )");
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
				result << wxT(", ") << ValueToCode( PT_WXSTRING_I18N, array[i] );
			}
			break;
		}
	default:
		break;
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////

LuaCodeGenerator::LuaCodeGenerator()
{
	SetupPredefinedMacros();
	m_useRelativePath = false;
	m_i18n = false;
	m_firstID = 1000;
	m_strEventHandlerPostfix = wxT("");
	m_strUITable = wxT("");

	//this classes aren't wrapped by wxLua - make exception
	m_strUnsupportedClasses.push_back(wxT("wxRichTextCtrl"));
	m_strUnsupportedClasses.push_back(wxT("wxSearchCtrl"));
	m_strUnsupportedClasses.push_back(wxT("wxAuiToolBar"));
	m_strUnsupportedClasses.push_back(wxT("wxRibbonBar"));
	m_strUnsupportedClasses.push_back(wxT("wxDataViewCtrl"));
	m_strUnsupportedClasses.push_back(wxT("wxDataViewListCtrl"));
	m_strUnsupportedClasses.push_back(wxT("wxDataViewTreeCtrl"));
}

wxString LuaCodeGenerator::ConvertLuaString( wxString text )
{
	wxString result;

	for ( size_t i = 0; i < text.length(); i++ )
	{
		wxChar c = text[i];

		switch ( c )
		{
		case wxT('"'):
			result += wxT("\\\"");
			break;

		case wxT('\\'):
			result += wxT("\\\\");
			break;

		case wxT('\t'):
			result += wxT("\\t");
			break;

		case wxT('\n'):
			result += wxT("\\n");
			break;

		case wxT('\r'):
			result += wxT("\\r");
			break;

		default:
			result += c;
			break;
		}
	}
	return result;
}

void LuaCodeGenerator::GenerateInheritedClass( PObjectBase userClasses, PObjectBase form,const  wxString & genFileFullPath)
{
	if (!userClasses)
	{
		wxLogError(wxT("There is no object to generate inherited class"));
		return;
	}

	if ( wxT("UserClasses") != userClasses->GetClassName() )
	{
		wxLogError(wxT("This not a UserClasses object"));
		return;
	}

	// Start file
	wxString code = GetCode( userClasses, wxT("file_comment") );
	m_source->WriteLn( code );

	wxString fullGenPath = genFileFullPath;
	fullGenPath.Replace(wxT("\\"), wxT("\\\\"));

	code = wxT("package.path = \"") + fullGenPath + wxT(".lua\"");
	m_source->WriteLn( code );

	code = GetCode( userClasses, wxT("source_include") );
	m_source->WriteLn( code );
	m_source->WriteLn( wxEmptyString );


			EventVector events;
			FindEventHandlers( form, events );


			if ( events.size() > 0 )
			{
				code = GetCode( userClasses, wxT("event_handler_comment") );
				m_source->WriteLn( code );
				m_source->WriteLn( wxEmptyString );

				std::set<wxString> generatedHandlers;
				wxString eventsGroupID ;
				wxString strPrevClassName;
				for ( size_t i = 0; i < events.size(); i++ )
				{
					PEvent event = events[i];

					wxString handlerName = event->GetValue();
					wxString templateName = wxString::Format( wxT("connect_%s"), event->GetName().c_str() );

					PObjectBase obj = event->GetObject();
					PObjectInfo obj_info = obj->GetObjectInfo();

					wxString strClassName = wxT("");
					wxString code = GenEventEntryForInheritedClass(obj, obj_info, templateName, handlerName, strClassName);

					bool bAddCaption = false;
					PProperty propName = obj->GetProperty( wxT("name") );
					if ( propName != NULL){
						strClassName = propName->GetValue();
						if(strPrevClassName != strClassName){
							strPrevClassName = strClassName;
							bAddCaption = true;
							eventsGroupID = wxString::Format( wxT("-- %s (%s) event handlers: "), strClassName.c_str(), obj->GetClassName().c_str());
						}
					}

					if(code.length() > 0){
						if(bAddCaption)
							m_source->WriteLn(eventsGroupID);

						m_source->WriteLn( code);
						m_source->WriteLn();
					}
				}
				m_source->WriteLn( wxEmptyString );
				m_source->WriteLn( wxEmptyString );
			}

	m_source->Unindent();
}

wxString LuaCodeGenerator::GenEventEntryForInheritedClass( PObjectBase obj, PObjectInfo obj_info, const wxString& templateName, const wxString& handlerName, wxString &strClassName )
{
	wxString code;
	PCodeInfo code_info = obj_info->GetCodeInfo( wxT("Lua") );
	if ( code_info )
	{
		wxString _template;
		_template = code_info->GetTemplate( wxString::Format( wxT("evt_%s"), templateName.c_str() ) );
		if ( !_template.empty() )
		{
			_template.Replace( wxT("#handler"),handlerName );
			_template.Replace( wxT("#skip"),wxT("\n") + m_strEventHandlerPostfix );
		}

		LuaTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
		code = parser.ParseTemplate();
		if(code.length() > 0) return code;

		for ( unsigned int i = 0; i < obj_info->GetBaseClassCount(false); i++ )
		{
			PObjectInfo base_info = obj_info->GetBaseClass( i, false );
			if ( (code = GenEventEntryForInheritedClass( obj, base_info, templateName, handlerName, strClassName )).length() > 0 )
			{
				return code;
			}
		}
	}
	return code;
}

bool LuaCodeGenerator::GenerateCode( PObjectBase project )
{
	if (!project)
	{
		wxLogError(wxT("There is no project to generate code"));
		return false;
	}

	m_i18n = false;
	PProperty i18nProperty = project->GetProperty( wxT("internationalize") );
	if (i18nProperty && i18nProperty->GetValueAsInteger())
		m_i18n = true;

	m_disconnectEvents = ( project->GetPropertyAsInteger( wxT("disconnect_lua_events") ) != 0 );

	m_source->Clear();

	// Insert Lua preamble

	wxString code = GetCode( project, wxT("lua_preamble") );
	if ( !code.empty() )
	{
		m_source->WriteLn( code );
		m_source->WriteLn( wxEmptyString );
	}

	code = (
		wxT("----------------------------------------------------------------------------\n")
		wxT("-- Lua code generated with wxFormBuilder (version ") wxT(__DATE__) wxT(")\n")
		wxT("-- http://www.wxformbuilder.org/\n")
		wxT("----------------------------------------------------------------------------\n")
		);

	m_source->WriteLn( code );

	PProperty propFile = project->GetProperty( wxT("file") );
	if (!propFile)
	{
		wxLogError( wxT("Missing \"file\" property on Project Object") );
		return false;
	}

	wxString file = propFile->GetValue();
	if ( file.empty() )
	{
		file = wxT("noname");
	}

	// Generate the subclass sets
	std::set< wxString > subclasses;
	std::vector< wxString > headerIncludes;

	GenSubclassSets( project, &subclasses, &headerIncludes );

	// Generating  includes
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
		m_source->WriteLn( wxT("") );
	}

	// Generating "defines" for macros
	GenDefines( project );

	PProperty propNamespace = project->GetProperty( wxT( "ui_table" ) );
	if ( propNamespace )
	{
		m_strUITable = propNamespace->GetValueAsString();
		if(m_strUITable.length() <= 0){
			m_strUITable = wxT("UI");//default value ... m_strUITable shouldn't be empty
		}
		code = m_strUITable +  wxT(" = {} \n");
		m_source->WriteLn( code );
	}


	PProperty eventKindProp = project->GetProperty( wxT("skip_lua_events") );
	if( eventKindProp->GetValueAsInteger() ){
		 m_strEventHandlerPostfix = wxT("event:Skip()");
	}
	else{
		m_strEventHandlerPostfix = wxT("");
	}


	PProperty disconnectMode = project->GetProperty( wxT("disconnect_mode") );
	m_disconnecMode = disconnectMode->GetValueAsString();

	unsigned int dProjChildCount = project->GetChildCount();
	for ( unsigned int i = 0; i < dProjChildCount; i++ )
	{
		PObjectBase child = project->GetChild( i );

		EventVector events;
		FindEventHandlers( child, events );
		GenClassDeclaration( child, false, wxT(""), events, m_strEventHandlerPostfix );
	}

	code = GetCode( project, wxT("lua_epilogue") );
	if( !code.empty() )
		m_source->WriteLn( code );

	return true;
}

void LuaCodeGenerator::GenEvents( PObjectBase class_obj, const EventVector &events, wxString &strClassName, bool disconnect )
{
	if ( events.empty() )
	{
		return;
	}

	if( disconnect )
	{
		m_source->WriteLn( wxT("-- Disconnect Events\n") );
	}
	else
	{
		m_source->WriteLn();
		m_source->WriteLn( wxT("-- Connect Events\n") );
	}


	PProperty propName = class_obj->GetProperty( wxT("name") );
	if ( !propName )
	{
		wxLogError(wxT("Missing \"name\" property on \"%s\" class. Review your XML object description"),
			class_obj->GetClassName().c_str());
		return;
	}

	wxString class_name = propName->GetValue();
	if ( class_name.empty() )
	{
		wxLogError( wxT("Object name cannot be null") );
		return;
	}

	/*
	wxString base_class;
	PProperty propSubclass = class_obj->GetProperty( wxT("subclass") );
	if ( propSubclass )
	{
		wxString subclass = propSubclass->GetChildFromParent( wxT("name") );
		if ( !subclass.empty() )
		{
			base_class = subclass;
		}
	}

	if ( base_class.empty() )
		base_class = wxT("wx.") + class_obj->GetClassName();

	*/
	wxString handlerName;
	if ( events.size() > 0 )
	{
		for ( size_t i = 0; i < events.size(); i++ )
		{
			PEvent event = events[i];

			handlerName = event->GetValue();// + wxT("_") + class_name;

			wxString templateName = wxString::Format( wxT("connect_%s"), event->GetName().c_str() );

			PObjectBase obj = event->GetObject();
			if ( !GenEventEntry( obj, obj->GetObjectInfo(), templateName, handlerName, strClassName, disconnect ) )
			{
				wxLogError( wxT("Missing \"evt_%s\" template for \"%s\" class. Review your XML object description"),
					templateName.c_str(), class_name.c_str() );
			}
		}
	}
}

bool LuaCodeGenerator::GenEventEntry( PObjectBase obj, PObjectInfo obj_info, const wxString& templateName, const wxString& handlerName,  wxString &strClassName, bool disconnect )
{
	wxString _template;
	PCodeInfo code_info = obj_info->GetCodeInfo( wxT("Lua") );
	if ( code_info )
	{
		_template = code_info->GetTemplate( wxString::Format( wxT("evt_%s%s"), disconnect ? wxT("dis") : wxT(""), templateName.c_str() ) );
		if ( disconnect && _template.empty() )
		{
			_template = code_info->GetTemplate( wxT("evt_") + templateName );
			_template.Replace( wxT("Connect"), wxT("Disconnect"), true );
		}

		if ( !_template.empty() )
		{
			if( disconnect )
			{
				if( m_disconnecMode == wxT("handler_name")) _template.Replace( wxT("#handler"), wxT("handler = ") + handlerName );
				else if(m_disconnecMode == wxT("source_name")) _template.Replace( wxT("#handler"), wxT("None") );
			}
			else{
				_template.Replace( wxT("#handler"),  handlerName );
				_template.Replace( wxT("#skip"),wxT("\n") + m_strEventHandlerPostfix );
			}

			LuaTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
			wxString code = parser.ParseTemplate();
			wxString strRootCode = parser.RootWxParentToCode();
			if(code.Find(strRootCode) != -1){
				code.Replace(strRootCode, strClassName);
			}
			m_source->WriteLn( code);
			m_source->WriteLn();
			return true;
		}
	}

	for ( unsigned int i = 0; i < obj_info->GetBaseClassCount(false); i++ )
	{
		PObjectInfo base_info = obj_info->GetBaseClass( i, false );
		if ( GenEventEntry( obj, base_info, templateName, handlerName, strClassName, disconnect ) )
		{
			return true;
		}
	}

	return false;
}

void LuaCodeGenerator::GenVirtualEventHandlers( const EventVector& events, const wxString& eventHandlerPostfix, const wxString& strClassName  )
{
	if ( events.size() > 0 )
	{
		// There are problems if we create "pure" virtual handlers, because some
		// events could be triggered in the constructor in which virtual methods are
		// execute properly.
		// So we create a default handler which will skip the event.
		m_source->WriteLn( wxEmptyString );
		m_source->WriteLn( wxT("-- event handlers") );

		std::set<wxString> generatedHandlers;
		for ( size_t i = 0; i < events.size(); i++ )
		{
			PEvent event = events[i];
			wxString aux = wxT("function ") + event->GetValue() + wxT("_") + strClassName + wxT("( event )");

			if (generatedHandlers.find(aux) == generatedHandlers.end())
			{
				m_source->WriteLn(aux);
				m_source->Indent();
				m_source->WriteLn(wxT("\n") + eventHandlerPostfix);
				m_source->Unindent();
				m_source->WriteLn(wxT("end"));
				generatedHandlers.insert(aux);
			}
			if( i < (events.size()-1) )  m_source->WriteLn();
		}
		m_source->WriteLn( wxEmptyString );
	}
}

void LuaCodeGenerator::GetGenEventHandlers( PObjectBase obj )
{
	GenDefinedEventHandlers( obj->GetObjectInfo(), obj );

	for (unsigned int i = 0; i < obj->GetChildCount() ; i++)
	{
		PObjectBase child = obj->GetChild(i);
		GetGenEventHandlers(child);
	}
}

void LuaCodeGenerator::GenDefinedEventHandlers( PObjectInfo info, PObjectBase obj )
{
	PCodeInfo code_info = info->GetCodeInfo( wxT( "Lua" ) );
	if ( code_info )
	{
		wxString _template = code_info->GetTemplate( wxT("generated_event_handlers") );
		if ( !_template.empty() )
		{
			LuaTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
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


wxString LuaCodeGenerator::GetCode(PObjectBase obj, wxString name, bool silent/*= false*/, wxString strSelf /*= wxT("")*/)
{
	wxString _template;
	PCodeInfo code_info = obj->GetObjectInfo()->GetCodeInfo( wxT("Lua") );

	if (!code_info)
	{
		if( !silent )
		{
			wxString msg( wxString::Format( wxT("Missing \"%s\" template for \"%s\" class. Review your XML object description"),
				name.c_str(), obj->GetClassName().c_str() ) );
			wxLogError(msg);
		}
		return wxT("");
	}

	_template = code_info->GetTemplate(name);
	_template.Replace(wxT("#parentname"), strSelf);


	if(m_strUITable.length() > 0)
		_template.Replace(wxT("#utbl"), m_strUITable + wxT("."));
	else
		_template.Replace(wxT("#utbl"), wxT(""));


	LuaTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
	wxString code = parser.ParseTemplate();

	//handle unsupported classes
	std::vector<wxString>::iterator iter = m_strUnsupportedInstances.begin();
	for(; iter != m_strUnsupportedInstances.end(); ++iter){
		if(code.Contains(*iter)) break;
	}
	if(iter != m_strUnsupportedInstances.end())
		code.Prepend(wxT("--"));

	wxString strRootCode = parser.RootWxParentToCode();
	int pos = code.Find(strRootCode);
	if(pos != -1){
		wxString strMid = code.Mid(pos + strRootCode.length(),3);
		strMid.Trim(false);
		if(strMid.GetChar(0) == ','){
			code.Replace(strRootCode, strSelf);
		}else{
			code.Replace(strRootCode, wxT(""));
		}
	}

	return code;
}

void LuaCodeGenerator::GenClassDeclaration(PObjectBase class_obj, bool use_enum, const wxString& classDecoration, const EventVector &events, const wxString& eventHandlerPostfix)
{
	wxString strClassName = class_obj->GetClassName();
	PProperty propName = class_obj->GetProperty( wxT("name") );
	if ( !propName )
	{
		wxLogError(wxT("Missing \"name\" property on \"%s\" class. Review your XML object description"),
			strClassName.c_str());
		return;
	}

	wxString strName = propName->GetValue();
	if ( strName.empty() )
	{
		wxLogError( wxT("Object name can not be null") );
		return;
	}

	GetGenEventHandlers( class_obj );
	GenConstructor( class_obj, events, strName);

}

void LuaCodeGenerator::GenSubclassSets( PObjectBase obj, std::set< wxString >* subclasses, std::vector< wxString >* headerIncludes )
{
	// Call GenSubclassForwardDeclarations on all children as well
	for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
	{
		GenSubclassSets( obj->GetChild( i ), subclasses, headerIncludes );
	}

	// Fill the set
	PProperty subclass = obj->GetProperty( wxT("subclass") );
	if ( subclass )
	{
		std::map< wxString, wxString > children;
		subclass->SplitParentProperty( &children );

		std::map< wxString, wxString >::iterator name;
		name = children.find( wxT("name") );

		if ( children.end() == name )
		{
			// No name, so do nothing
			return;
		}

		wxString nameVal = name->second;
		if ( nameVal.empty() )
		{
			// No name, so do nothing
			return;
		}

		// Now get the header
		std::map< wxString, wxString >::iterator header;
		header = children.find( wxT("header") );

		if ( children.end() == header )
		{
			// No header, so do nothing
			return;
		}

		wxString headerVal = header->second;
		if ( headerVal.empty() )
		{
			// No header, so do nothing
			return;
		}

		// Got a header
		PObjectInfo info = obj->GetObjectInfo();
		if ( !info )
		{
			return;
		}

		PObjectPackage pkg = info->GetPackage();
		if ( !pkg )
		{
			return;
		}

		wxString include = wxT("require(\"") + headerVal.Trim() +  wxT("\")\n");
		std::vector< wxString >::iterator it = std::find( headerIncludes->begin(), headerIncludes->end(), include );
		if ( headerIncludes->end() == it )
		{
			headerIncludes->push_back( include );
		}
	}
}

void LuaCodeGenerator::GenIncludes( PObjectBase project, std::vector<wxString>* includes, std::set< wxString >* templates )
{
	GenObjectIncludes( project, includes, templates );
}

void LuaCodeGenerator::GenObjectIncludes( PObjectBase project, std::vector< wxString >* includes, std::set< wxString >* templates )
{
	// Fill the set
	PCodeInfo code_info = project->GetObjectInfo()->GetCodeInfo( wxT("Lua") );
	if (code_info)
	{
		LuaTemplateParser parser( project, code_info->GetTemplate( wxT("include") ), m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
		wxString include = parser.ParseTemplate();
		if ( !include.empty() )
		{
			if ( templates->insert( include ).second )
			{
				AddUniqueIncludes( include, includes );
			}
		}
	}

	// Call GenIncludes on all children as well
	for ( unsigned int i = 0; i < project->GetChildCount(); i++ )
	{
		GenObjectIncludes( project->GetChild(i), includes, templates );
	}

	// Generate includes for base classes
	GenBaseIncludes( project->GetObjectInfo(), project, includes, templates );
}

void LuaCodeGenerator::GenBaseIncludes( PObjectInfo info, PObjectBase obj, std::vector< wxString >* includes, std::set< wxString >* templates )
{
	if ( !info )
	{
		return;
	}

	// Process all the base classes recursively
	for ( unsigned int i = 0; i < info->GetBaseClassCount(false); i++ )
	{
		PObjectInfo base_info = info->GetBaseClass( i, false );
		GenBaseIncludes( base_info, obj, includes, templates );
	}

	PCodeInfo code_info = info->GetCodeInfo( wxT("Lua") );
	if ( code_info )
	{
		LuaTemplateParser parser( obj, code_info->GetTemplate( wxT("include") ), m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
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

void LuaCodeGenerator::AddUniqueIncludes( const wxString& include, std::vector< wxString >* includes )
{
	// Split on newlines to only generate unique include lines
	// This strips blank lines and trims
	wxStringTokenizer tkz( include, wxT("\n"), wxTOKEN_STRTOK );

	while ( tkz.HasMoreTokens() )
	{
		wxString line = tkz.GetNextToken();
		line.Trim( false );
		line.Trim( true );


			includes->push_back( line );
			continue;

		// If it is an include, it must be unique to be written
		std::vector< wxString >::iterator it = std::find( includes->begin(), includes->end(), line );
		if ( includes->end() == it )
		{
			includes->push_back( line );
		}
	}
}

void LuaCodeGenerator::FindDependencies( PObjectBase obj, std::set< PObjectInfo >& info_set )
{
	unsigned int ch_count = obj->GetChildCount();
	if (ch_count > 0)
	{
		unsigned int i;
		for (i = 0; i<ch_count; i++)
		{
			PObjectBase child = obj->GetChild(i);
			info_set.insert(child->GetObjectInfo());
			FindDependencies(child, info_set);
		}
	}
}

void LuaCodeGenerator::GenConstructor( PObjectBase class_obj, const EventVector &events, wxString &strClassName )
{
	PProperty propName = class_obj->GetProperty( wxT("name") );
	if ( !propName )
	{
		wxLogError( wxT( "Missing \"name\" property on \"%s\" class. Review your XML object description" ),
					class_obj->GetClassName().c_str() );
		return;
	}

	wxString strName = propName->GetValue();

	if(m_strUITable.length() > 0)
		strName = m_strUITable + wxT(".") + strName;

	m_source->WriteLn();
	m_source->WriteLn( wxT("-- create ") + strClassName );

	m_source->WriteLn(strName + wxT(" = ") + GetCode( class_obj, wxT("cons_call") ));

	m_source->Indent();
	wxString settings = GetCode( class_obj, wxT("settings") );
	if ( !settings.IsEmpty() )
	{
		m_source->WriteLn( settings );
	}

		if ( class_obj->GetObjectTypeName() == wxT("wizard") && class_obj->GetChildCount() > 0 )
	{
		m_source->WriteLn( wxT("function add_page(page)") );
		m_source->Indent();
		m_source->WriteLn( wxT("if ( #") + m_strUITable + wxT(".") + strClassName + wxT(".m_pages) > 0 then") );
		m_source->Indent();
		m_source->WriteLn( wxT("local previous_page = ") + m_strUITable + wxT(".") + strClassName + wxT(".m_pages[ #") + m_strUITable + wxT(".") + strClassName +wxT(".m_pages ]") );
		m_source->WriteLn( wxT("page:SetPrev(previous_page)") );
		m_source->WriteLn( wxT("previous_page:SetNext(page)") );
		m_source->Unindent();
		m_source->WriteLn( wxT("end") );
		m_source->WriteLn(wxT("table.insert( ") + m_strUITable + wxT(".") + strClassName + wxT(".m_pages, page)") );
		m_source->Unindent();
		m_source->WriteLn( wxT("end") );
	}

	for ( unsigned int i = 0; i < class_obj->GetChildCount(); i++ )
	{
		GenConstruction( class_obj->GetChild( i ), true , strClassName);
	}

	wxString afterAddChild = GetCode( class_obj, wxT("after_addchild") );
	if ( !afterAddChild.IsEmpty() )
	{
		m_source->WriteLn( afterAddChild );
	}

	GenEvents( class_obj, events, strClassName );

	m_source->Unindent();
}

void LuaCodeGenerator::GenDestructor( PObjectBase class_obj, const EventVector &events )
{
	m_source->WriteLn();
	// generate function definition
	m_source->Indent();

	wxString strClassName;
	if ( m_disconnectEvents && !events.empty() )
	{
		GenEvents( class_obj, events, strClassName, true );
	}
	else
		if( !class_obj->GetPropertyAsInteger( wxT("aui_managed") ) ) m_source->WriteLn( wxT("pass") );

	// destruct objects
	GenDestruction( class_obj );

	m_source->Unindent();
}
wxString GetParentWindow(PObjectBase obj)
{
	wxString strName;
	PObjectBase ob = obj->GetLayout();
	if(ob)
		strName = ob->GetClassName();

	return strName;
}
void LuaCodeGenerator::GenConstruction(PObjectBase obj, bool is_widget, wxString &strClassName  )
{
	wxString type = obj->GetObjectTypeName();
	PObjectInfo info = obj->GetObjectInfo();

	if ( ObjectDatabase::HasCppProperties( type ) )
	{

		wxString strName;
		PProperty propName = obj->GetProperty( wxT("name") );
		if(propName)
			strName = propName->GetValue();

		wxString strClass = obj->GetClassName();


		std::vector<wxString>::iterator itr;
		if(m_strUnsupportedClasses.end() != (itr = std::find(m_strUnsupportedClasses.begin(),m_strUnsupportedClasses.end(),strClass))){

			m_source->WriteLn(wxT("--Instance ") + strName + wxT(" of Control ") + *itr + wxT(" you try to use isn't unfortunately wrapped by wxLua."));
			m_source->WriteLn(wxT("--Please try to use another control\n"));
			m_strUnsupportedInstances.push_back(strName);
			return;
		}

		m_source->WriteLn( GetCode( obj, wxT("construction"), false, strClassName ) );

		GenSettings( obj->GetObjectInfo(), obj, strClassName );

		bool isWidget = !info->IsSubclassOf( wxT("sizer") );

		for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
		{
			PObjectBase child = obj->GetChild( i );
			GenConstruction( child, isWidget, strClassName);

			if ( type == wxT("toolbar") )
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
			m_source->WriteLn();

			if (is_widget)
			{
				// the parent object is not a sizer. There is no template for
				// this so we'll make it manually.
				// It's not a good practice to embed templates into the source code,
				// because you will need to recompile...

				wxString _template =	wxT("#utbl#parent$name:SetSizer( #utbl$name ) #nl")
										wxT("#utbl#parent$name:Layout()")
										wxT("#ifnull #parent $size")
										wxT("@{ #nl #utbl$name:Fit( #utbl#parent $name ) @}");

				LuaTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
				wxString res  = parser.ParseTemplate();
				res.Replace(parser.RootWxParentToCode(),wxT(""));
				m_source->WriteLn(res);
			}
		}
		else if ( type == wxT("splitter") )
		{
			// Generating the split
			switch ( obj->GetChildCount() )
			{
				case 1:
				{
					PObjectBase sub1 = obj->GetChild(0)->GetChild(0);
					wxString _template = wxT("#utbl$name:Initialize( ");
					_template = _template + wxT("#utbl") + sub1->GetProperty( wxT("name") )->GetValue() + wxT(" )");
					_template.Replace(wxT("#utbl"), m_strUITable + wxT("."));

					LuaTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
					m_source->WriteLn(parser.ParseTemplate());
					break;
				}
				case 2:
				{
					PObjectBase sub1,sub2;
					sub1 = obj->GetChild(0)->GetChild(0);
					sub2 = obj->GetChild(1)->GetChild(0);

					wxString _template;
					bool bSplitVertical = false;
					wxString strMode = obj->GetProperty( wxT("splitmode") )->GetValue();
					if ( (bSplitVertical = (strMode == wxT("wxSPLIT_VERTICAL"))) )
					{
						_template = wxT("#utbl$name:SplitVertically( ");
					}
					else
					{
						_template = wxT("#utbl$name:SplitHorizontally( ");
					}

					_template = _template + wxT("#utbl") + sub1->GetProperty( wxT("name") )->GetValue() +
						wxT(", #utbl") + sub2->GetProperty( wxT("name") )->GetValue() + wxT(", $sashpos )");
					_template = _template + wxT("#nl #utbl$name") + wxT(":SetSplitMode(") + wxString::Format(wxT("%d"),(bSplitVertical ? 1 : 0)) + wxT(")");
					_template.Replace(wxT("#utbl"), m_strUITable + wxT("."));

					LuaTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
					m_source->WriteLn(parser.ParseTemplate());
					break;
				}
				default:
					wxLogError( wxT("Missing subwindows for wxSplitterWindow widget.") );
					break;
			}
		}
		else if ( 	type == wxT("menubar")	||
				type == wxT("menu")		||
				type == wxT("submenu")	||
				type == wxT("toolbar")	||
				type == wxT("tool")	||
				type == wxT("listbook")	||
				type == wxT("simplebook") ||
				type == wxT("notebook")	||
				type == wxT("auinotebook")	||
				type == wxT("treelistctrl")	||
				type == wxT("flatnotebook") ||
				type == wxT("wizard")
			)
		{
			wxString afterAddChild = GetCode( obj, wxT("after_addchild") );
			if ( !afterAddChild.empty() )
			{
				m_source->WriteLn( afterAddChild );
			}
			m_source->WriteLn();
		}
	}
	else if ( info->IsSubclassOf( wxT("sizeritembase") ) )
	{

		// The child must be added to the sizer having in mind the
		// child object type (there are 3 different routines)
		GenConstruction( obj->GetChild(0), false,  strClassName);

		PObjectInfo childInfo = obj->GetChild(0)->GetObjectInfo();
		wxString temp_name;
		if ( childInfo->IsSubclassOf( wxT("wxWindow") ) || wxT("CustomControl") == childInfo->GetClassName() )
		{
			temp_name = wxT("window_add");
		}
		else if ( childInfo->IsSubclassOf( wxT("sizer") ) )
		{
			temp_name = wxT("sizer_add");
		}
		else if ( childInfo->GetClassName() == wxT("spacer") )
		{
			temp_name = wxT("spacer_add");
		}
		else
		{
			LogDebug( wxT("SizerItem child is not a Spacer and is not a subclass of wxWindow or of sizer.") );
			return;
		}


			m_source->WriteLn( GetCode( obj, temp_name ) );

	}
	else if (	type == wxT("notebookpage")		||
				type == wxT("flatnotebookpage")	||
				type == wxT("listbookpage")		||
				type == wxT("simplebookpage")	||
				type == wxT("choicebookpage")	||
				type == wxT("auinotebookpage")
			)
	{
		GenConstruction( obj->GetChild( 0 ), false, strClassName);
		m_source->WriteLn( GetCode( obj, wxT("page_add") ) );
		GenSettings( obj->GetObjectInfo(), obj, strClassName );
	}
	else if ( type == wxT("treelistctrlcolumn") )
	{
		m_source->WriteLn( GetCode( obj, wxT("column_add") ) );
		GenSettings( obj->GetObjectInfo(), obj, strClassName );
	}
	else if ( type == wxT("tool") )
	{
		// If loading bitmap from ICON resource, and size is not set, set size to toolbars bitmapsize
		// So hacky, yet so useful ...
		wxSize toolbarsize = obj->GetParent()->GetPropertyAsSize( _("bitmapsize") );
		if ( wxDefaultSize != toolbarsize )
		{
			PProperty prop = obj->GetProperty( _("bitmap") );
			if ( prop )
			{
				wxString oldVal = prop->GetValueAsString();
				wxString path, source;
				wxSize toolsize;
				TypeConv::ParseBitmapWithResource( oldVal, &path, &source, &toolsize );
				if ( _("Load From Icon Resource") == source && wxDefaultSize == toolsize )
				{
					prop->SetValue( wxString::Format( wxT("%s; %s [%i; %i]"), path.c_str(), source.c_str(), toolbarsize.GetWidth(), toolbarsize.GetHeight() ) );
					m_source->WriteLn( GetCode( obj, wxT("construction") ) );
					prop->SetValue( oldVal );
					return;
				}
			}
		}
		m_source->WriteLn( GetCode( obj, wxT("construction") ) );
	}
	else
	{
		// Generate the children
		for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
		{
			GenConstruction( obj->GetChild( i ), false, strClassName);
		}
	}
}

void LuaCodeGenerator::GenDestruction( PObjectBase obj )
{
	wxString _template;
	PCodeInfo code_info = obj->GetObjectInfo()->GetCodeInfo( wxT( "Lua" ) );

	if ( code_info )
	{
		_template = code_info->GetTemplate( wxT( "destruction" ) );

		if ( !_template.empty() )
		{
			LuaTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
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

void LuaCodeGenerator::FindMacros( PObjectBase obj, std::vector<wxString>* macros )
{
	// iterate through all of the properties of all objects, add the macros
	// to the vector
	unsigned int i;

	for ( i = 0; i < obj->GetPropertyCount(); i++ )
	{
		PProperty prop = obj->GetProperty( i );
		if ( prop->GetType() == PT_MACRO )
		{
			wxString value = prop->GetValue();
			if( value.IsEmpty() ) continue;

			// Skip wx IDs
			if ( ( ! value.Contains( wxT("XRCID" ) ) ) &&
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

void LuaCodeGenerator::FindEventHandlers(PObjectBase obj, EventVector &events)
{
  unsigned int i;
  unsigned int evt_cnt = obj->GetEventCount();
  for (i=0; i < evt_cnt; i++)
  {
	PEvent event = obj->GetEvent(i);
	if (!event->GetValue().IsEmpty())
	  events.push_back(event);
  }

  for (i=0; i < obj->GetChildCount(); i++)
  {
	PObjectBase child = obj->GetChild(i);
	FindEventHandlers(child,events);
  }
}

void LuaCodeGenerator::GenDefines( PObjectBase project)
{
	std::vector< wxString > macros;
	FindMacros( project, &macros );
	m_strUserIDsVec.erase(m_strUserIDsVec.begin(),m_strUserIDsVec.end());

	// Remove the default macro from the set, for backward compatiblity
	std::vector< wxString >::iterator it;
	it = std::find( macros.begin(), macros.end(), wxT("ID_DEFAULT") );
	if ( it != macros.end() )
	{
		// The default macro is defined to wxID_ANY
		m_source->WriteLn( wxT("wxID_DEFAULT = wxID_ANY -- Default") );
		macros.erase(it);
	}

	unsigned int id = m_firstID;
	if ( id < 1000 )
	{
		wxLogWarning(wxT("First ID is Less than 1000"));
	}
	for (it = macros.begin() ; it != macros.end(); it++)
	{
		// Don't redefine wx IDs
		m_source->WriteLn( wxString::Format( wxT("%s = %i"), it->c_str(), id ) );
		m_strUserIDsVec.push_back(*it);
		id++;
	}
	if( !macros.empty() ) m_source->WriteLn( wxT("") );
}

void LuaCodeGenerator::GenSettings(PObjectInfo info, PObjectBase obj, wxString &strClassName  )
{
	wxString _template;
	PCodeInfo code_info = info->GetCodeInfo( wxT("Lua") );

	if ( !code_info )
	{
		return;
	}

	_template = code_info->GetTemplate( wxT("settings") );

	if ( !_template.empty() )
	{
		LuaTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
		wxString code = parser.ParseTemplate();

		wxString strRootCode = parser.RootWxParentToCode();
		if(code.Find(strRootCode) != -1){
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

void LuaCodeGenerator::GenAddToolbar( PObjectInfo info, PObjectBase obj )
{
	wxArrayString arrCode;

	GetAddToolbarCode( info, obj, arrCode );

	for( size_t i = 0; i < arrCode.GetCount(); i++ ) m_source->WriteLn( arrCode[i] );
}

void LuaCodeGenerator::GetAddToolbarCode( PObjectInfo info, PObjectBase obj, wxArrayString& codelines )
{
	wxString _template;
	PCodeInfo code_info = info->GetCodeInfo( wxT( "Lua" ) );

	if ( !code_info )
		return;

	_template = code_info->GetTemplate( wxT( "toolbar_add" ) );

	if ( !_template.empty() )
	{
		LuaTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath, m_strUserIDsVec );
		wxString code = parser.ParseTemplate();
		if ( !code.empty() )
		{
			if( codelines.Index( code ) == wxNOT_FOUND ) codelines.Add( code );
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

void LuaCodeGenerator::UseRelativePath(bool relative, wxString basePath)
{
	bool result;
	m_useRelativePath = relative;

	if (m_useRelativePath)
	{
		result = wxFileName::DirExists( basePath );
		m_basePath = ( result ? basePath : wxT("") );
	}
}

#define ADD_PREDEFINED_MACRO(x) m_predMacros.insert( wxT(#x) )
#define ADD_PREDEFINED_PREFIX(k, v) m_predModulePrefix[ wxT(#k) ] = wxT(#v)

void LuaCodeGenerator::SetupPredefinedMacros()
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

void LuaTemplateParser::SetupModulePrefixes()
{
	// altered class names
	ADD_PREDEFINED_PREFIX( wxCalendarCtrl, wx.wx );
	ADD_PREDEFINED_PREFIX( wxRichTextCtrl, wx.wx );
	ADD_PREDEFINED_PREFIX( wxHtmlWindow, wx.wx );
	ADD_PREDEFINED_PREFIX( wxAuiNotebook, wxaui.wx );
	ADD_PREDEFINED_PREFIX( wxGrid, wx.wx );
	ADD_PREDEFINED_PREFIX( wxAnimationCtrl, wx.wx );

	// altered macros
	ADD_PREDEFINED_PREFIX( wxCAL_SHOW_HOLIDAYS, wx.);
	ADD_PREDEFINED_PREFIX( wxCAL_MONDAY_FIRST, wx.);
	ADD_PREDEFINED_PREFIX( wxCAL_NO_MONTH_CHANGE, wx.);
	ADD_PREDEFINED_PREFIX( wxCAL_NO_YEAR_CHANGE, wx.);
	ADD_PREDEFINED_PREFIX( wxCAL_SEQUENTIAL_MONTH_SELECTION, wx. );
	ADD_PREDEFINED_PREFIX( wxCAL_SHOW_SURROUNDING_WEEKS, wx. );
	ADD_PREDEFINED_PREFIX( wxCAL_SUNDAY_FIRST, wx. );

	ADD_PREDEFINED_PREFIX( wxHW_DEFAULT_STYLE, wx. );
	ADD_PREDEFINED_PREFIX( wxHW_NO_SELECTION, wx.);
	ADD_PREDEFINED_PREFIX( wxHW_SCROLLBAR_AUTO, wx.);
	ADD_PREDEFINED_PREFIX( wxHW_SCROLLBAR_NEVER, wx. );

	ADD_PREDEFINED_PREFIX( wxAUI_NB_BOTTOM, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_CLOSE_BUTTON, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_CLOSE_ON_ACTIVE_TAB, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_CLOSE_ON_ALL_TABS, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_DEFAULT_STYLE, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_LEFT, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_MIDDLE_CLICK_CLOSE, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_RIGHT, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_SCROLL_BUTTONS, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_TAB_EXTERNAL_MOVE, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_TAB_FIXED_WIDTH, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_TAB_MOVE, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_TAB_SPLIT, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_TOP, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_WINDOWLIST_BUTTON, wxaui. );

	ADD_PREDEFINED_PREFIX( wxAUI_TB_TEXT, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_NO_TOOLTIPS, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_NO_AUTORESIZE, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_GRIPPER, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_OVERFLOW, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_VERTICAL, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_HORZ_LAYOUT, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_HORZ_TEXT, wxaui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_DEFAULT_STYLE, wxaui. );

	ADD_PREDEFINED_PREFIX( wxAC_DEFAULT_STYLE, wx. );
	ADD_PREDEFINED_PREFIX( wxAC_NO_AUTORESIZE, wx. );
}
