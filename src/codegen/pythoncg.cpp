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
// Python code generation writen by
//   Michal Bližňak - michal.bliznak@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

#include "pythoncg.h"
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

PythonTemplateParser::PythonTemplateParser( PObjectBase obj, wxString _template, bool useI18N, bool useRelativePath, wxString basePath )
:
TemplateParser(obj,_template),
m_i18n( useI18N ),
m_useRelativePath( useRelativePath ),
m_basePath( basePath )
{
	if ( !wxFileName::DirExists( m_basePath ) )
	{
		m_basePath.clear();
	}

	SetupModulePrefixes();
}

PythonTemplateParser::PythonTemplateParser( const PythonTemplateParser & that, wxString _template )
:
TemplateParser( that, _template ),
m_i18n( that.m_i18n ),
m_useRelativePath( that.m_useRelativePath ),
m_basePath( that.m_basePath )
{
	SetupModulePrefixes();
}

wxString PythonTemplateParser::RootWxParentToCode()
{
	return wxT("self");
}

PTemplateParser PythonTemplateParser::CreateParser( const TemplateParser* oldparser, wxString _template )
{
	const PythonTemplateParser* pythonOldParser = dynamic_cast< const PythonTemplateParser* >( oldparser );
	if ( pythonOldParser != NULL )
	{
		PTemplateParser newparser( new PythonTemplateParser( *pythonOldParser, _template ) );
		return newparser;
	}
	return PTemplateParser();
}

/**
* Convert the value of the property to Python code
*/
wxString PythonTemplateParser::ValueToCode( PropertyType type, wxString value )
{
	wxString result;

	switch ( type )
	{
	case PT_WXPARENT:
		{
			result = wxT("self.") + value;
			break;
		}
	case PT_WXPARENT_SB:
		{
			result = value + wxT(".GetStaticBox()");
			break;
		}
	case PT_WXSTRING:
	case PT_FILE:
	case PT_PATH:
		{
			if ( value.empty() )
			{
				result << wxT("wx.EmptyString");
			}
			else
			{
				result << wxT("u\"") << PythonCodeGenerator::ConvertPythonString( value ) << wxT("\"");
			}
			break;
		}
	case PT_WXSTRING_I18N:
		{
			if ( value.empty() )
			{
				result << wxT("wx.EmptyString");
			}
			else
			{
				if ( m_i18n )
				{
					result << wxT("_(u\"") << PythonCodeGenerator::ConvertPythonString(value) << wxT("\")");
				}
				else
				{
					result << wxT("u\"") << PythonCodeGenerator::ConvertPythonString(value) << wxT("\"");
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

			if( !pred.empty() )	result.Replace( wxT("wx"), pred );
			else
			{
				if( result.StartsWith( wxT("XRCID") ) )
					result.Prepend( wxT("wx.xrc.") );
				else
					result.Replace( wxT("wx"), wxT("wx.") );
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
			result = ( value.empty() ? wxT("0") : value );

			wxString pred, bit;
			wxStringTokenizer bits( result, wxT("|"), wxTOKEN_STRTOK );

			while( bits.HasMoreTokens() )
			{
				bit = bits.GetNextToken();
				pred = m_predModulePrefix[bit];

				if( bit.Contains( wxT("wx") ) )
				{
					if( !pred.empty() )	result.Replace( bit, pred + bit.AfterFirst('x') );
					else
						result.Replace( bit, wxT("wx.") + bit.AfterFirst('x') );
				}
			}
			break;
		}
	case PT_WXPOINT:
		{
			if ( value.empty() )
			{
				result = wxT("wx.DefaultPosition");
			}
			else
			{
				result << wxT("wx.Point( ") << value << wxT(" )");
			}
			break;
		}
	case PT_WXSIZE:
		{
			if ( value.empty() )
			{
				result = wxT("wx.DefaultSize");
			}
			else
			{
				result << wxT("wx.Size( ") << value << wxT(" )");
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

				result = wxString::Format( "wx.Font( %s, %s, %s, %s, %s, %s )",
							((pointSize <= 0) ? "wx.NORMAL_FONT.GetPointSize()" : (wxString() << pointSize)),
							TypeConv::FontFamilyToString( fontContainer.GetFamily() ).replace( 0, 2, "wx." ),
							font.GetStyleString().replace( 0, 2, "wx." ),
							font.GetWeightString().replace( 0, 2, "wx." ),
							( fontContainer.GetUnderlined() ? "True" : "False" ),
							( fontContainer.m_faceName.empty() ? "wx.EmptyString" : ("\"" + fontContainer.m_faceName + "\"") )
						);
			}
			else
			{
				result = wxT("wx.NORMAL_FONT");
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
					result << wxT("wx.SystemSettings.GetColour( ") << ValueToCode( PT_OPTION, value ) << wxT(" )");
				}
				else
				{
					wxColour colour = TypeConv::StringToColour( value );
					result = wxString::Format( wxT("wx.Colour( %i, %i, %i )"), colour.Red(), colour.Green(), colour.Blue() );
				}
			}
			else
			{
				result = wxT("wx.Colour()");
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
				result = wxT("wx.NullBitmap");
				break;
			}

			if ( path.StartsWith( wxT("file:") ) )
			{
				wxLogWarning( wxT("Python code generation does not support using URLs for bitmap properties:\n%s"), path.c_str() );
				result = wxT("wx.NullBitmap");
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
					result = wxT( "wx.NullBitmap" );
					break;
				}

				wxString file = ( m_useRelativePath ? TypeConv::MakeRelativePath( absPath, m_basePath ) : absPath );

				result << wxT("wx.Bitmap( u\"") << PythonCodeGenerator::ConvertPythonString( file ) << wxT("\", wx.BITMAP_TYPE_ANY )");
			}
			else if ( source == _("Load From Resource") )
			{
				result << wxT("wx.Bitmap( u\"") << path << wxT("\", wx.BITMAP_TYPE_RESOURCE )");
			}
			else if ( source == _("Load From Icon Resource") )
			{
				if ( wxDefaultSize == icoSize )
				{
					result << wxT("wx.ICON( ") << path << wxT(" )");
				}
				else
				{
					result.Printf( wxT("wx.Icon( u\"%s\", wx.BITMAP_TYPE_ICO_RESOURCE, %i, %i )"), path.c_str(), icoSize.GetWidth(), icoSize.GetHeight() );
				}
			}
			else if ( source == _("Load From Art Provider") )
			{
				wxString rid = path.BeforeFirst( wxT(':') );

				if( rid.StartsWith( wxT("gtk-") ) )
				{
					rid = wxT("u\"") + rid + wxT("\"");
				}
				else
					rid.Replace( wxT("wx"), wxT("wx.") );

				wxString cid = path.AfterFirst( wxT(':') );
				cid.Replace( wxT("wx"), wxT("wx.") );

				result = wxT("wx.ArtProvider.GetBitmap( ") + rid + wxT(", ") +  cid + wxT(" )");
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

PythonCodeGenerator::PythonCodeGenerator()
{
	SetupPredefinedMacros();
	m_useRelativePath = false;
	m_i18n = false;
	m_firstID = 1000;
}

wxString PythonCodeGenerator::ConvertPythonString( wxString text )
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

void PythonCodeGenerator::GenerateInheritedClass( PObjectBase userClasses, PObjectBase form )
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

	wxString type = userClasses->GetPropertyAsString( wxT("type") );

	// Start file
	wxString code = GetCode( userClasses, wxT("file_comment") );
	m_source->WriteLn( code );
	m_source->WriteLn( wxEmptyString );

	code = GetCode( userClasses, wxT("source_include") );
	m_source->WriteLn( code );
	m_source->WriteLn( wxEmptyString );

	code = GetCode( userClasses, wxT("class_decl") );
	m_source->WriteLn( code );
	m_source->Indent();

	code = GetCode( userClasses, type + wxT("_cons_def") );
	m_source->WriteLn( code );

	// Do events
	EventVector events;
	FindEventHandlers( form, events );

	if ( events.size() > 0 )
	{
		code = GetCode( userClasses, wxT("event_handler_comment") );
		m_source->WriteLn( code );

		std::set<wxString> generatedHandlers;
		for ( size_t i = 0; i < events.size(); i++ )
		{
			PEvent event = events[i];
			if ( generatedHandlers.find( event->GetValue() ) == generatedHandlers.end() )
			{
				m_source->WriteLn( wxString::Format( wxT("def %s( self, event ):"),  event->GetValue().c_str() ) );
				m_source->Indent();
				m_source->WriteLn( wxString::Format( wxT("# TODO: Implement %s"), event->GetValue().c_str() ) );
				m_source->WriteLn( wxT("pass") );
				m_source->Unindent();
				m_source->WriteLn( wxEmptyString );
				generatedHandlers.insert(event->GetValue());
			}
		}
		m_source->WriteLn( wxEmptyString );
	}

	m_source->Unindent();
}

bool PythonCodeGenerator::GenerateCode( PObjectBase project )
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

	m_disconnectEvents = ( project->GetPropertyAsInteger( wxT("disconnect_python_events") ) != 0 );

	m_source->Clear();

	// Insert python preamble

	wxString code = GetCode( project, wxT("python_preamble") );
	if ( !code.empty() )
	{
		m_source->WriteLn( code );
		m_source->WriteLn( wxEmptyString );
	}

	code = (
		wxT("###########################################################################\n")
		wxT("## Python code generated with wxFormBuilder (version ") wxT(__DATE__) wxT(")\n")
		wxT("## http://www.wxformbuilder.org/\n")
		wxT("##\n")
		wxT("## PLEASE DO *NOT* EDIT THIS FILE!\n")
		wxT("###########################################################################\n") );

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

	// Generating in the .h header file those include from components dependencies.
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

	// Write internationalization support
	if( m_i18n )
	{
		m_source->WriteLn( wxT("import gettext") );
		m_source->WriteLn( wxT("_ = gettext.gettext") );
		m_source->WriteLn( wxT("") );
	}

	// Generating "defines" for macros
	GenDefines( project );

	wxString eventHandlerPostfix;
	PProperty eventKindProp = project->GetProperty( wxT("skip_python_events") );
	if( eventKindProp->GetValueAsInteger() )
	{
		 eventHandlerPostfix = wxT("event.Skip()");
	}
	else
		eventHandlerPostfix = wxT("pass");

	PProperty disconnectMode = project->GetProperty( wxT("disconnect_mode") );
	m_disconnecMode = disconnectMode->GetValueAsString();

	for ( unsigned int i = 0; i < project->GetChildCount(); i++ )
	{
		PObjectBase child = project->GetChild( i );

		EventVector events;
		FindEventHandlers( child, events );
		//GenClassDeclaration( child, useEnum, classDecoration, events, eventHandlerPrefix, eventHandlerPostfix );
		GenClassDeclaration( child, false, wxT(""), events, eventHandlerPostfix );
	}

	code = GetCode( project, wxT("python_epilogue") );
	if( !code.empty() ) m_source->WriteLn( code );

	return true;
}

void PythonCodeGenerator::GenEvents( PObjectBase class_obj, const EventVector &events, bool disconnect )
{
	if ( events.empty() )
	{
		return;
	}

	if( disconnect )
	{
		m_source->WriteLn( wxT("# Disconnect Events") );
	}
	else
	{
		m_source->WriteLn();
		m_source->WriteLn( wxT("# Connect Events") );
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

	wxString base_class;
	wxString handlerName;

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

	if ( events.size() > 0 )
	{
		for ( size_t i = 0; i < events.size(); i++ )
		{
			PEvent event = events[i];

			handlerName = event->GetValue();

			wxString templateName = wxString::Format( wxT("connect_%s"), event->GetName().c_str() );

			PObjectBase obj = event->GetObject();
			if ( !GenEventEntry( obj, obj->GetObjectInfo(), templateName, handlerName, disconnect ) )
			{
				wxLogError( wxT("Missing \"evt_%s\" template for \"%s\" class. Review your XML object description"),
					templateName.c_str(), class_name.c_str() );
			}
		}
	}
}

bool PythonCodeGenerator::GenEventEntry( PObjectBase obj, PObjectInfo obj_info, const wxString& templateName, const wxString& handlerName, bool disconnect )
{
	wxString _template;
	PCodeInfo code_info = obj_info->GetCodeInfo( wxT("Python") );
	if ( code_info )
	{
		_template = code_info->GetTemplate( wxString::Format( wxT("evt_%s%s"), disconnect ? wxT("dis") : wxT(""), templateName.c_str() ) );
		if ( disconnect && _template.empty() )
		{
			_template = code_info->GetTemplate( wxT("evt_") + templateName );
			_template.Replace( wxT("Bind"), wxT("Unbind"), true );
		}

		if ( !_template.empty() )
		{
			if( disconnect )
			{
				if( m_disconnecMode == wxT("handler_name")) _template.Replace( wxT("#handler"), wxT("handler = self.") + handlerName );
				else if(m_disconnecMode == wxT("source_name")) _template.Replace( wxT("#handler"), wxT("None") ); //wxT("self.") + obj->GetProperty(wxT("name"))->GetValueAsString() );
			}
			else
				_template.Replace( wxT("#handler"), wxT("self.") + handlerName );

			PythonTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath );
			m_source->WriteLn( parser.ParseTemplate() );
			return true;
		}
	}

	for ( unsigned int i = 0; i < obj_info->GetBaseClassCount(false); i++ )
	{
		PObjectInfo base_info = obj_info->GetBaseClass( i, false );
		if ( GenEventEntry( obj, base_info, templateName, handlerName, disconnect ) )
		{
			return true;
		}
	}

	return false;
}

void PythonCodeGenerator::GenVirtualEventHandlers( const EventVector& events, const wxString& eventHandlerPostfix )
{
	if ( events.size() > 0 )
	{
		// There are problems if we create "pure" virtual handlers, because some
		// events could be triggered in the constructor in which virtual methods are
		// execute properly.
		// So we create a default handler which will skip the event.
		m_source->WriteLn( wxEmptyString );
		m_source->WriteLn( wxT("# Virtual event handlers, overide them in your derived class") );

		std::set<wxString> generatedHandlers;
		for ( size_t i = 0; i < events.size(); i++ )
		{
			PEvent event = events[i];
			wxString aux = wxT("def ") + event->GetValue() + wxT("( self, event ):");

			if (generatedHandlers.find(aux) == generatedHandlers.end())
			{
				m_source->WriteLn(aux);
				m_source->Indent();
				m_source->WriteLn(eventHandlerPostfix);
				m_source->Unindent();

				generatedHandlers.insert(aux);
			}
			if( i < (events.size()-1) )  m_source->WriteLn();
		}
		m_source->WriteLn( wxEmptyString );
	}
}

void PythonCodeGenerator::GetGenEventHandlers( PObjectBase obj )
{
	GenDefinedEventHandlers( obj->GetObjectInfo(), obj );

	for (unsigned int i = 0; i < obj->GetChildCount() ; i++)
	{
		PObjectBase child = obj->GetChild(i);
		GetGenEventHandlers(child);
	}
}

void PythonCodeGenerator::GenDefinedEventHandlers( PObjectInfo info, PObjectBase obj )
{
	PCodeInfo code_info = info->GetCodeInfo( wxT( "Python" ) );
	if ( code_info )
	{
		wxString _template = code_info->GetTemplate( wxT("generated_event_handlers") );
		if ( !_template.empty() )
		{
			PythonTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath );
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


wxString PythonCodeGenerator::GetCode(PObjectBase obj, wxString name, bool silent)
{
	wxString _template;
	PCodeInfo code_info = obj->GetObjectInfo()->GetCodeInfo( wxT("Python") );

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

	PythonTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath );
	wxString code = parser.ParseTemplate();

	return code;
}

void PythonCodeGenerator::GenClassDeclaration(PObjectBase class_obj, bool use_enum, const wxString& classDecoration, const EventVector &events, const wxString& eventHandlerPostfix)
{
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
		wxLogError( wxT("Object name can not be null") );
		return;
	}

	m_source->WriteLn( wxT("###########################################################################") );
	m_source->WriteLn( wxT("## Class ") + class_name);
	m_source->WriteLn( wxT("###########################################################################") );
	m_source->WriteLn( );

	m_source->WriteLn( wxT("class ") + classDecoration + class_name + wxT(" ( ") + GetCode( class_obj, wxT("base") ).Trim() + wxT(" ):") );
	m_source->Indent();

	// The constructor is also included within public
	GenConstructor( class_obj, events );
	GenDestructor( class_obj, events );

	m_source->WriteLn( wxT("") );

	// event handlers
	GenVirtualEventHandlers(events, eventHandlerPostfix);
	GetGenEventHandlers( class_obj );

	m_source->Unindent();
	m_source->WriteLn( wxT("") );
}

void PythonCodeGenerator::GenSubclassSets( PObjectBase obj, std::set< wxString >* subclasses, std::vector< wxString >* headerIncludes )
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

		wxString include = wxT("from ") + headerVal + wxT(" import ") + nameVal;
		std::vector< wxString >::iterator it = std::find( headerIncludes->begin(), headerIncludes->end(), include );
		if ( headerIncludes->end() == it )
		{
			headerIncludes->push_back( include );
		}
	}
}

void PythonCodeGenerator::GenIncludes( PObjectBase project, std::vector<wxString>* includes, std::set< wxString >* templates )
{
	GenObjectIncludes( project, includes, templates );
}

void PythonCodeGenerator::GenObjectIncludes( PObjectBase project, std::vector< wxString >* includes, std::set< wxString >* templates )
{
	// Fill the set
	PCodeInfo code_info = project->GetObjectInfo()->GetCodeInfo( wxT("Python") );
	if (code_info)
	{
		PythonTemplateParser parser( project, code_info->GetTemplate( wxT("include") ), m_i18n, m_useRelativePath, m_basePath );
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

void PythonCodeGenerator::GenBaseIncludes( PObjectInfo info, PObjectBase obj, std::vector< wxString >* includes, std::set< wxString >* templates )
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

	PCodeInfo code_info = info->GetCodeInfo( wxT("Python") );
	if ( code_info )
	{
		PythonTemplateParser parser( obj, code_info->GetTemplate( wxT("include") ), m_i18n, m_useRelativePath, m_basePath );
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

void PythonCodeGenerator::AddUniqueIncludes( const wxString& include, std::vector< wxString >* includes )
{
	// Split on newlines to only generate unique include lines
	// This strips blank lines and trims
	wxStringTokenizer tkz( include, wxT("\n"), wxTOKEN_STRTOK );

	while ( tkz.HasMoreTokens() )
	{
		wxString line = tkz.GetNextToken();
		line.Trim( false );
		line.Trim( true );

		// If it is not an include line, it will be written
		if ( !line.StartsWith( wxT("import") ) )
		{
			includes->push_back( line );
			continue;
		}

		// If it is an include, it must be unique to be written
		std::vector< wxString >::iterator it = std::find( includes->begin(), includes->end(), line );
		if ( includes->end() == it )
		{
			includes->push_back( line );
		}
	}
}

void PythonCodeGenerator::FindDependencies( PObjectBase obj, std::set< PObjectInfo >& info_set )
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

void PythonCodeGenerator::GenConstructor( PObjectBase class_obj, const EventVector &events )
{
	m_source->WriteLn();
	// generate function definition
	m_source->WriteLn( GetCode( class_obj, wxT("cons_def") ) );
	m_source->Indent();

	m_source->WriteLn( GetCode( class_obj, wxT("cons_call") ) );
	m_source->WriteLn();

	wxString settings = GetCode( class_obj, wxT("settings") );
	if ( !settings.IsEmpty() )
	{
		m_source->WriteLn( settings );
	}

	for ( unsigned int i = 0; i < class_obj->GetChildCount(); i++ )
	{
		GenConstruction( class_obj->GetChild( i ), true );
	}

	wxString afterAddChild = GetCode( class_obj, wxT("after_addchild") );
	if ( !afterAddChild.IsEmpty() )
	{
		m_source->WriteLn( afterAddChild );
	}

	GenEvents( class_obj, events );

	m_source->Unindent();

	if ( class_obj->GetObjectTypeName() == wxT("wizard") && class_obj->GetChildCount() > 0 )
	{
		m_source->WriteLn( wxT("def add_page(self, page):") );
		m_source->Indent();
		m_source->WriteLn( wxT("if self.m_pages:") );
		m_source->Indent();
		m_source->WriteLn( wxT("previous_page = self.m_pages[-1]") );
		m_source->WriteLn( wxT("page.SetPrev(previous_page)") );
		m_source->WriteLn( wxT("previous_page.SetNext(page)") );
		m_source->Unindent();
		m_source->WriteLn( wxT("self.m_pages.append(page)") );
		m_source->Unindent();
	}
}

void PythonCodeGenerator::GenDestructor( PObjectBase class_obj, const EventVector &events )
{
	m_source->WriteLn();
	// generate function definition
	m_source->WriteLn( wxT("def __del__( self ):") );
	m_source->Indent();

	if ( m_disconnectEvents && !events.empty() )
	{
		GenEvents( class_obj, events, true );
	}
	else
		if( !class_obj->GetPropertyAsInteger( wxT("aui_managed") ) ) m_source->WriteLn( wxT("pass") );

	// destruct objects
	GenDestruction( class_obj );

	m_source->Unindent();
}

void PythonCodeGenerator::GenConstruction(PObjectBase obj, bool is_widget )
{
	wxString type = obj->GetObjectTypeName();
	PObjectInfo info = obj->GetObjectInfo();

	if ( ObjectDatabase::HasCppProperties( type ) )
	{
		m_source->WriteLn( GetCode( obj, wxT("construction") ) );

		GenSettings( obj->GetObjectInfo(), obj );

		bool isWidget = !info->IsSubclassOf( wxT("sizer") );

		for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
		{
			PObjectBase child = obj->GetChild( i );
			GenConstruction( child, isWidget );

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

				wxString _template =	wxT("#wxparent $name.SetSizer( $name ) #nl")
										wxT("#wxparent $name.Layout()")
										wxT("#ifnull #parent $size")
										wxT("@{ #nl $name.Fit( #wxparent $name ) @}");

				PythonTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath );
				m_source->WriteLn(parser.ParseTemplate());
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
					wxString _template = wxT("self.$name.Initialize( ");
					_template = _template + wxT("self.") + sub1->GetProperty( wxT("name") )->GetValue() + wxT(" )");

					PythonTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath );
					m_source->WriteLn(parser.ParseTemplate());
					break;
				}
				case 2:
				{
					PObjectBase sub1,sub2;
					sub1 = obj->GetChild(0)->GetChild(0);
					sub2 = obj->GetChild(1)->GetChild(0);

					wxString _template;
					if ( obj->GetProperty( wxT("splitmode") )->GetValue() == wxT("wxSPLIT_VERTICAL") )
					{
						_template = wxT("self.$name.SplitVertically( ");
					}
					else
					{
						_template = wxT("self.$name.SplitHorizontally( ");
					}

					_template = _template + wxT("self.") + sub1->GetProperty( wxT("name") )->GetValue() +
						wxT(", self.") + sub2->GetProperty( wxT("name") )->GetValue() + wxT(", $sashpos )");

					PythonTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath );
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
				type == wxT( "ribbonbar" )        ||
				type == wxT("toolbar")	||
				type == wxT("tool")	||
				type == wxT("listbook")	||
				type == wxT("simplebook" ) ||
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
		GenConstruction( obj->GetChild(0), false );

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
		GenConstruction( obj->GetChild( 0 ), false );
		m_source->WriteLn( GetCode( obj, wxT("page_add") ) );
		GenSettings( obj->GetObjectInfo(), obj );
	}
	else if ( type == wxT("treelistctrlcolumn") )
	{
		m_source->WriteLn( GetCode( obj, wxT("column_add") ) );
		GenSettings( obj->GetObjectInfo(), obj );
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
			GenConstruction( obj->GetChild( i ), false );
		}
	}
}

void PythonCodeGenerator::GenDestruction( PObjectBase obj )
{
	wxString _template;
	PCodeInfo code_info = obj->GetObjectInfo()->GetCodeInfo( wxT( "Python" ) );

	if ( code_info )
	{
		_template = code_info->GetTemplate( wxT( "destruction" ) );

		if ( !_template.empty() )
		{
			PythonTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath );
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

void PythonCodeGenerator::FindMacros( PObjectBase obj, std::vector<wxString>* macros )
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

			//if( value.Contains( wxT("wx") ) && !value.Contains( wxT("wx.") ) ) value.Replace( wxT("wx"), wxT("wx.") );
			value.Replace( wxT("wx"), wxT("wx.") );

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

void PythonCodeGenerator::FindEventHandlers(PObjectBase obj, EventVector &events)
{
  unsigned int i;
  for (i=0; i < obj->GetEventCount(); i++)
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

void PythonCodeGenerator::GenDefines( PObjectBase project)
{
	std::vector< wxString > macros;
	FindMacros( project, &macros );

	// Remove the default macro from the set, for backward compatiblity
	std::vector< wxString >::iterator it;
	it = std::find( macros.begin(), macros.end(), wxT("ID_DEFAULT") );
	if ( it != macros.end() )
	{
		// The default macro is defined to wxID_ANY
		m_source->WriteLn( wxT("ID_DEFAULT = wx.ID_ANY # Default") );
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
		id++;
	}
	if( !macros.empty() ) m_source->WriteLn( wxT("") );
}

void PythonCodeGenerator::GenSettings(PObjectInfo info, PObjectBase obj)
{
	wxString _template;
	PCodeInfo code_info = info->GetCodeInfo( wxT("Python") );

	if ( !code_info )
	{
		return;
	}

	_template = code_info->GetTemplate( wxT("settings") );

	if ( !_template.empty() )
	{
		PythonTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath );
		wxString code = parser.ParseTemplate();
		if ( !code.empty() )
		{
			m_source->WriteLn(code);
		}
	}

	// Proceeding recursively with the base classes
	for (unsigned int i=0; i< info->GetBaseClassCount(false); i++)
	{
		PObjectInfo base_info = info->GetBaseClass(i, false);
		GenSettings(base_info,obj);
	}
}

void PythonCodeGenerator::GenAddToolbar( PObjectInfo info, PObjectBase obj )
{
	wxArrayString arrCode;

	GetAddToolbarCode( info, obj, arrCode );

	for( size_t i = 0; i < arrCode.GetCount(); i++ ) m_source->WriteLn( arrCode[i] );
}

void PythonCodeGenerator::GetAddToolbarCode( PObjectInfo info, PObjectBase obj, wxArrayString& codelines )
{
	wxString _template;
	PCodeInfo code_info = info->GetCodeInfo( wxT( "Python" ) );

	if ( !code_info )
		return;

	_template = code_info->GetTemplate( wxT( "toolbar_add" ) );

	if ( !_template.empty() )
	{
		PythonTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath );
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

void PythonCodeGenerator::UseRelativePath(bool relative, wxString basePath)
{
	bool result;
	m_useRelativePath = relative;

	if (m_useRelativePath)
	{
		result = wxFileName::DirExists( basePath );
		m_basePath = ( result ? basePath : wxT("") );
	}
}
/*
wxString CppCodeGenerator::ConvertToRelativePath(wxString path, wxString basePath)
{
wxString auxPath = path;
if (basePath != "")
{
wxFileName filename(_WXSTR(auxPath));
if (filename.MakeRelativeTo(_WXSTR(basePath)))
auxPath = _STDSTR(filename.GetFullPath());
}
return auxPath;
}*/

#define ADD_PREDEFINED_MACRO(x) m_predMacros.insert( wxT(#x) )
#define ADD_PREDEFINED_PREFIX(k, v) m_predModulePrefix[ wxT(#k) ] = wxT(#v)

void PythonCodeGenerator::SetupPredefinedMacros()
{
	/* no id matches this one when compared to it */
	ADD_PREDEFINED_MACRO(wx.ID_NONE);

	/*  id for a separator line in the menu (invalid for normal item) */
	ADD_PREDEFINED_MACRO(wx.ID_SEPARATOR);

	ADD_PREDEFINED_MACRO(wx.ID_ANY);

	ADD_PREDEFINED_MACRO(wx.ID_LOWEST);

	ADD_PREDEFINED_MACRO(wx.ID_OPEN);
	ADD_PREDEFINED_MACRO(wx.ID_CLOSE);
	ADD_PREDEFINED_MACRO(wx.ID_NEW);
	ADD_PREDEFINED_MACRO(wx.ID_SAVE);
	ADD_PREDEFINED_MACRO(wx.ID_SAVEAS);
	ADD_PREDEFINED_MACRO(wx.ID_REVERT);
	ADD_PREDEFINED_MACRO(wx.ID_EXIT);
	ADD_PREDEFINED_MACRO(wx.ID_UNDO);
	ADD_PREDEFINED_MACRO(wx.ID_REDO);
	ADD_PREDEFINED_MACRO(wx.ID_HELP);
	ADD_PREDEFINED_MACRO(wx.ID_PRINT);
	ADD_PREDEFINED_MACRO(wx.ID_PRINT_SETUP);
	ADD_PREDEFINED_MACRO(wx.ID_PREVIEW);
	ADD_PREDEFINED_MACRO(wx.ID_ABOUT);
	ADD_PREDEFINED_MACRO(wx.ID_HELP_CONTENTS);
	ADD_PREDEFINED_MACRO(wx.ID_HELP_COMMANDS);
	ADD_PREDEFINED_MACRO(wx.ID_HELP_PROCEDURES);
	ADD_PREDEFINED_MACRO(wx.ID_HELP_CONTEXT);
	ADD_PREDEFINED_MACRO(wx.ID_CLOSE_ALL);
	ADD_PREDEFINED_MACRO(wx.ID_PAGE_SETUP);
	ADD_PREDEFINED_MACRO(wx.ID_HELP_INDEX);
	ADD_PREDEFINED_MACRO(wx.ID_HELP_SEARCH);
	ADD_PREDEFINED_MACRO(wx.ID_PREFERENCES);

	ADD_PREDEFINED_MACRO(wx.ID_EDIT);
	ADD_PREDEFINED_MACRO(wx.ID_CUT);
	ADD_PREDEFINED_MACRO(wx.ID_COPY);
	ADD_PREDEFINED_MACRO(wx.ID_PASTE);
	ADD_PREDEFINED_MACRO(wx.ID_CLEAR);
	ADD_PREDEFINED_MACRO(wx.ID_FIND);

	ADD_PREDEFINED_MACRO(wx.ID_DUPLICATE);
	ADD_PREDEFINED_MACRO(wx.ID_SELECTALL);
	ADD_PREDEFINED_MACRO(wx.ID_DELETE);
	ADD_PREDEFINED_MACRO(wx.ID_REPLACE);
	ADD_PREDEFINED_MACRO(wx.ID_REPLACE_ALL);
	ADD_PREDEFINED_MACRO(wx.ID_PROPERTIES);

	ADD_PREDEFINED_MACRO(wx.ID_VIEW_DETAILS);
	ADD_PREDEFINED_MACRO(wx.ID_VIEW_LARGEICONS);
	ADD_PREDEFINED_MACRO(wx.ID_VIEW_SMALLICONS);
	ADD_PREDEFINED_MACRO(wx.ID_VIEW_LIST);
	ADD_PREDEFINED_MACRO(wx.ID_VIEW_SORTDATE);
	ADD_PREDEFINED_MACRO(wx.ID_VIEW_SORTNAME);
	ADD_PREDEFINED_MACRO(wx.ID_VIEW_SORTSIZE);
	ADD_PREDEFINED_MACRO(wx.ID_VIEW_SORTTYPE);

	ADD_PREDEFINED_MACRO(wx.ID_FILE);
	ADD_PREDEFINED_MACRO(wx.ID_FILE1);
	ADD_PREDEFINED_MACRO(wx.ID_FILE2);
	ADD_PREDEFINED_MACRO(wx.ID_FILE3);
	ADD_PREDEFINED_MACRO(wx.ID_FILE4);
	ADD_PREDEFINED_MACRO(wx.ID_FILE5);
	ADD_PREDEFINED_MACRO(wx.ID_FILE6);
	ADD_PREDEFINED_MACRO(wx.ID_FILE7);
	ADD_PREDEFINED_MACRO(wx.ID_FILE8);
	ADD_PREDEFINED_MACRO(wx.ID_FILE9);

	/*  Standard button and menu IDs */

	ADD_PREDEFINED_MACRO(wx.ID_OK);
	ADD_PREDEFINED_MACRO(wx.ID_CANCEL);

	ADD_PREDEFINED_MACRO(wx.ID_APPLY);
	ADD_PREDEFINED_MACRO(wx.ID_YES);
	ADD_PREDEFINED_MACRO(wx.ID_NO);
	ADD_PREDEFINED_MACRO(wx.ID_STATIC);
	ADD_PREDEFINED_MACRO(wx.ID_FORWARD);
	ADD_PREDEFINED_MACRO(wx.ID_BACKWARD);
	ADD_PREDEFINED_MACRO(wx.ID_DEFAULT);
	ADD_PREDEFINED_MACRO(wx.ID_MORE);
	ADD_PREDEFINED_MACRO(wx.ID_SETUP);
	ADD_PREDEFINED_MACRO(wx.ID_RESET);
	ADD_PREDEFINED_MACRO(wx.ID_CONTEXT_HELP);
	ADD_PREDEFINED_MACRO(wx.ID_YESTOALL);
	ADD_PREDEFINED_MACRO(wx.ID_NOTOALL);
	ADD_PREDEFINED_MACRO(wx.ID_ABORT);
	ADD_PREDEFINED_MACRO(wx.ID_RETRY);
	ADD_PREDEFINED_MACRO(wx.ID_IGNORE);
	ADD_PREDEFINED_MACRO(wx.ID_ADD);
	ADD_PREDEFINED_MACRO(wx.ID_REMOVE);

	ADD_PREDEFINED_MACRO(wx.ID_UP);
	ADD_PREDEFINED_MACRO(wx.ID_DOWN);
	ADD_PREDEFINED_MACRO(wx.ID_HOME);
	ADD_PREDEFINED_MACRO(wx.ID_REFRESH);
	ADD_PREDEFINED_MACRO(wx.ID_STOP);
	ADD_PREDEFINED_MACRO(wx.ID_INDEX);

	ADD_PREDEFINED_MACRO(wx.ID_BOLD);
	ADD_PREDEFINED_MACRO(wx.ID_ITALIC);
	ADD_PREDEFINED_MACRO(wx.ID_JUSTIFY_CENTER);
	ADD_PREDEFINED_MACRO(wx.ID_JUSTIFY_FILL);
	ADD_PREDEFINED_MACRO(wx.ID_JUSTIFY_RIGHT);

	ADD_PREDEFINED_MACRO(wx.ID_JUSTIFY_LEFT);
	ADD_PREDEFINED_MACRO(wx.ID_UNDERLINE);
	ADD_PREDEFINED_MACRO(wx.ID_INDENT);
	ADD_PREDEFINED_MACRO(wx.ID_UNINDENT);
	ADD_PREDEFINED_MACRO(wx.ID_ZOOM_100);
	ADD_PREDEFINED_MACRO(wx.ID_ZOOM_FIT);
	ADD_PREDEFINED_MACRO(wx.ID_ZOOM_IN);
	ADD_PREDEFINED_MACRO(wx.ID_ZOOM_OUT);
	ADD_PREDEFINED_MACRO(wx.ID_UNDELETE);
	ADD_PREDEFINED_MACRO(wx.ID_REVERT_TO_SAVED);

	/*  System menu IDs (used by wxUniv): */
	ADD_PREDEFINED_MACRO(wx.ID_SYSTEM_MENU);
	ADD_PREDEFINED_MACRO(wx.ID_CLOSE_FRAME);
	ADD_PREDEFINED_MACRO(wx.ID_MOVE_FRAME);
	ADD_PREDEFINED_MACRO(wx.ID_RESIZE_FRAME);
	ADD_PREDEFINED_MACRO(wx.ID_MAXIMIZE_FRAME);
	ADD_PREDEFINED_MACRO(wx.ID_ICONIZE_FRAME);
	ADD_PREDEFINED_MACRO(wx.ID_RESTORE_FRAME);

	/*  IDs used by generic file dialog (13 consecutive starting from this value) */
	ADD_PREDEFINED_MACRO(wx.ID_FILEDLGG);

	ADD_PREDEFINED_MACRO(wx.ID_HIGHEST);

}

void PythonTemplateParser::SetupModulePrefixes()
{
	// altered class names
	ADD_PREDEFINED_PREFIX( wxCalendarCtrl, wx.adv. );
	ADD_PREDEFINED_PREFIX( wxRichTextCtrl, wx.richtext. );
	ADD_PREDEFINED_PREFIX( wxHtmlWindow, wx.html. );
	ADD_PREDEFINED_PREFIX( wxAuiNotebook, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxGrid, wx.grid. );
	ADD_PREDEFINED_PREFIX( wxAnimationCtrl, wx.animate. );

	// altered macros
	ADD_PREDEFINED_PREFIX( wxCAL_SHOW_HOLIDAYS, wx.adv. );
	ADD_PREDEFINED_PREFIX( wxCAL_MONDAY_FIRST, wx.adv. );
	ADD_PREDEFINED_PREFIX( wxCAL_NO_MONTH_CHANGE, wx.adv. );
	ADD_PREDEFINED_PREFIX( wxCAL_NO_YEAR_CHANGE, wx.adv. );
	ADD_PREDEFINED_PREFIX( wxCAL_SEQUENTIAL_MONTH_SELECTION, wx.adv. );
	ADD_PREDEFINED_PREFIX( wxCAL_SHOW_SURROUNDING_WEEKS, wx.adv. );
	ADD_PREDEFINED_PREFIX( wxCAL_SUNDAY_FIRST, wx.adv. );

	ADD_PREDEFINED_PREFIX( wxHW_DEFAULT_STYLE, wx.html. );
	ADD_PREDEFINED_PREFIX( wxHW_NO_SELECTION, wx.html. );
	ADD_PREDEFINED_PREFIX( wxHW_SCROLLBAR_AUTO, wx.html. );
	ADD_PREDEFINED_PREFIX( wxHW_SCROLLBAR_NEVER, wx.html. );

	ADD_PREDEFINED_PREFIX( wxAUI_NB_BOTTOM, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_CLOSE_BUTTON, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_CLOSE_ON_ACTIVE_TAB, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_CLOSE_ON_ALL_TABS, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_DEFAULT_STYLE, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_LEFT, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_MIDDLE_CLICK_CLOSE, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_RIGHT, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_SCROLL_BUTTONS, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_TAB_EXTERNAL_MOVE, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_TAB_FIXED_WIDTH, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_TAB_MOVE, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_TAB_SPLIT, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_TOP, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_NB_WINDOWLIST_BUTTON, wx.aui. );

	ADD_PREDEFINED_PREFIX( wxAUI_TB_TEXT, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_NO_TOOLTIPS, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_NO_AUTORESIZE, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_GRIPPER, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_OVERFLOW, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_VERTICAL, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_HORZ_LAYOUT, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_HORZ_TEXT, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_PLAIN_BACKGROUND, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_TB_DEFAULT_STYLE, wx.aui. );

	ADD_PREDEFINED_PREFIX( wxAUI_MGR_ALLOW_FLOATING, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_MGR_ALLOW_ACTIVE_PANE, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_MGR_TRANSPARENT_DRAG, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_MGR_TRANSPARENT_HINT, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_MGR_VENETIAN_BLINDS_HINT, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_MGR_RECTANGLE_HINT, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_MGR_HINT_FADE, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_MGR_NO_VENETIAN_BLINDS_FADE, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_MGR_LIVE_RESIZE, wx.aui. );
	ADD_PREDEFINED_PREFIX( wxAUI_MGR_DEFAULT, wx.aui. );

	ADD_PREDEFINED_PREFIX( wxAC_DEFAULT_STYLE, wx.animate. );
	ADD_PREDEFINED_PREFIX( wxAC_NO_AUTORESIZE, wx.animate. );

	ADD_PREDEFINED_PREFIX( wxRIBBON_BAR_DEFAULT_STYLE, wx.lib.agw.ribbon. );
	ADD_PREDEFINED_PREFIX( wxRIBBON_BAR_FOLDBAR_STYLE, wx.lib.agw.ribbon. );
	ADD_PREDEFINED_PREFIX( wxRIBBON_BAR_SHOW_PAGE_LABELS, wx.lib.agw.ribbon. );
	ADD_PREDEFINED_PREFIX( wxRIBBON_BAR_SHOW_PAGE_ICONS, wx.lib.agw.ribbon. );
	ADD_PREDEFINED_PREFIX( wxRIBBON_BAR_FLOW_HORIZONTAL, wx.lib.agw.ribbon. );
	ADD_PREDEFINED_PREFIX( wxRIBBON_BAR_FLOW_VERTICAL, wx.lib.agw.ribbon. );
	ADD_PREDEFINED_PREFIX( wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS, wx.lib.agw.ribbon. );
	ADD_PREDEFINED_PREFIX( wxRIBBON_BAR_SHOW_PANEL_MINIMISE_BUTTONS, wx.lib.agw.ribbon. );
	ADD_PREDEFINED_PREFIX( wxRIBBON_BAR_SHOW_TOGGLE_BUTTON, wx.lib.agw.ribbon. );
	ADD_PREDEFINED_PREFIX( wxRIBBON_BAR_SHOW_HELP_BUTTON, wx.lib.agw.ribbon. );

	ADD_PREDEFINED_PREFIX( wxRIBBON_PANEL_DEFAULT_STYLE, wx.lib.agw.ribbon. );
	ADD_PREDEFINED_PREFIX( wxRIBBON_PANEL_NO_AUTO_MINIMISE, wx.lib.agw.ribbon. );
	ADD_PREDEFINED_PREFIX( wxRIBBON_PANEL_EXT_BUTTON, wx.lib.agw.ribbon. );
	ADD_PREDEFINED_PREFIX( wxRIBBON_PANEL_MINIMISE_BUTTON, wx.lib.agw.ribbon. );
	ADD_PREDEFINED_PREFIX( wxRIBBON_PANEL_STRETCH, wx.lib.agw.ribbon. );
	ADD_PREDEFINED_PREFIX( wxRIBBON_PANEL_FLEXIBLE, wx.lib.agw.ribbon. );

	ADD_PREDEFINED_PREFIX( wxPG_AUTO_SORT, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_HIDE_CATEGORIES, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_ALPHABETIC_MODE, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_BOLD_MODIFIED, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_SPLITTER_AUTO_CENTER, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_TOOLTIPS, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_HIDE_MARGIN, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_STATIC_SPLITTER, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_STATIC_LAYOUT, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_LIMITED_EDITING, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_DEFAULT_STYLE, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_EX_INIT_NOCAT, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_EX_NO_FLAT_TOOLBAR, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_EX_MODE_BUTTONS, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_EX_HELP_AS_TOOLTIPS, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_EX_NATIVE_DOUBLE_BUFFERING, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_EX_AUTO_UNSPECIFIED_VALUES, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_EX_WRITEONLY_BUILTIN_ATTRIBUTES, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_EX_HIDE_PAGE_BUTTONS, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_EX_MULTIPLE_SELECTION, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_EX_ENABLE_TLP_TRACKING, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_EX_NO_TOOLBAR_DIVIDER, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_EX_TOOLBAR_SEPARATOR, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_DESCRIPTION, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_TOOLBAR, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPGMAN_DEFAULT_STYLE, wx.propgrid. );
	ADD_PREDEFINED_PREFIX( wxPG_NO_INTERNAL_BORDER, wx.propgrid. );

	ADD_PREDEFINED_PREFIX( wxDV_SINGLE, wx.dataview. );
	ADD_PREDEFINED_PREFIX( wxDV_MULTIPLE, wx.dataview. );
	ADD_PREDEFINED_PREFIX( wxDV_ROW_LINES, wx.dataview. );
	ADD_PREDEFINED_PREFIX( wxDV_HORIZ_RULES, wx.dataview. );
	ADD_PREDEFINED_PREFIX( wxDV_VERT_RULES, wx.dataview. );
	ADD_PREDEFINED_PREFIX( wxDV_VARIABLE_LINE_HEIGHT, wx.dataview. );
	ADD_PREDEFINED_PREFIX( wxDV_NO_HEADER, wx.dataview. );
}
