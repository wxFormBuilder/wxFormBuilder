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
///////////////////////////////////////////////////////////////////////////////

#include "cppcg.h"
#include "utils/typeconv.h"
#include "rad/appdata.h"
#include "model/objectbase.h"
#include <algorithm>

#include <wx/filename.h>
#include <wx/tokenzr.h>

CppTemplateParser::CppTemplateParser( PObjectBase obj, wxString _template, bool useI18N, bool useRelativePath, wxString basePath )
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
}

CppTemplateParser::CppTemplateParser( const CppTemplateParser & that, wxString _template )
:
TemplateParser( that, _template ),
m_i18n( that.m_i18n ),
m_useRelativePath( that.m_useRelativePath ),
m_basePath( that.m_basePath )
{
}

wxString CppTemplateParser::RootWxParentToCode()
{
	return wxT("this");
}

PTemplateParser CppTemplateParser::CreateParser( const TemplateParser* oldparser, wxString _template )
{
	const CppTemplateParser* cppOldParser = dynamic_cast< const CppTemplateParser* >( oldparser );
	if ( cppOldParser != NULL )
	{
		PTemplateParser newparser( new CppTemplateParser( *cppOldParser, _template ) );
		return newparser;
	}
	return PTemplateParser();
}

/**
* Convert the value of the property to C++ code
*/
wxString CppTemplateParser::ValueToCode( PropertyType type, wxString value )
{
	wxString result;

	switch ( type )
	{
	case PT_WXSTRING:
		{
			if ( value.empty() )
			{
				result << wxT("wxEmptyString");
			}
			else
			{
				result << wxT("wxT(\"") << CppCodeGenerator::ConvertCppString( value ) << wxT("\")");
			}
			break;
		}
	case PT_WXSTRING_I18N:
		{
			if ( value.empty() )
			{
				result << wxT("wxEmptyString");
			}
			else
			{
				if ( m_i18n )
				{
					result << wxT("_(\"") << CppCodeGenerator::ConvertCppString(value) << wxT("\")");
				}
				else
				{
					result << wxT("wxT(\"") << CppCodeGenerator::ConvertCppString(value) << wxT("\")");
				}
			}
			break;
		}
	case PT_MACRO:
	case PT_TEXT:
	case PT_OPTION:
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
			break;
		}
	case PT_WXPOINT:
		{
			if ( value.empty() )
			{
				result = wxT("wxDefaultPosition");
			}
			else
			{
				result << wxT("wxPoint( ") << value << wxT(" )");
			}
			break;
		}
	case PT_WXSIZE:
		{
			if ( value.empty() )
			{
				result = wxT("wxDefaultSize");
			}
			else
			{
				result << wxT("wxSize( ") << value << wxT(" )");
			}
			break;
		}
	case PT_BOOL:
		{
			result = ( value == wxT("0") ? wxT("false") : wxT("true") );
			break;
		}
	case PT_WXFONT:
		{
			if ( !value.empty() )
			{
				wxFont font = TypeConv::StringToFont( value );
				result	= wxString::Format( wxT("wxFont( %i, %i, %i, %i, %s, wxT(\"%s\") )" ),
											font.GetPointSize(),
											font.GetFamily(),
											font.GetStyle(),
											font.GetWeight(),
											( font.GetUnderlined() ? wxT("true") : wxT("false") ),
											font.GetFaceName().c_str()
											);
			}
			else
			{
				result = wxT("wxFont()");
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
					result << wxT("wxSystemSettings::GetColour( ") << value << wxT(" )");
				}
				else
				{
					wxColour colour = TypeConv::StringToColour( value );
					result = wxString::Format( wxT("wxColour( %i, %i, %i )"), colour.Red(), colour.Green(), colour.Blue() );
				}
			}
			else
			{
				result = wxT("wxColour()");
			}
			break;
		}
	case PT_BITMAP:
		{
			// Splitting bitmap resource property value - it is of the form "path; source"
			size_t semicolonIndex = value.find_first_of( wxT(";") );
			wxString path;
			wxString source;
			if ( semicolonIndex != value.npos )
			{
				path = value.substr( 0, semicolonIndex );
				source = value.substr( semicolonIndex + 2 ); // Separated by "; "
			}
			else
			{
				path = value;
				source = wxT("Load From File");
			}

			if ( path.empty() )
			{
				// Empty path, generate Null Bitmap
				result = wxT("wxNullBitmap");
				break;
			}

			if ( source == wxT("Load From File") )
			{
				wxString absPath = TypeConv::MakeAbsolutePath( path, AppData()->GetProjectPath() );
				wxString file = ( m_useRelativePath ? TypeConv::MakeRelativePath( absPath, m_basePath ) : absPath );

				wxString cppString = CppCodeGenerator::ConvertCppString( file );

				wxFileName bmpFileName( path );
				if ( bmpFileName.GetExt().Upper() == wxT("XPM") )
				{
					// If the bitmap is an XPM we will embed it in the code, otherwise it will be loaded from the file at run time.
					result << wxT("wxBitmap( ") << CppCodeGenerator::ConvertXpmName( path ) << wxT(" )");
				}
				else
				{
					result << wxT("wxBitmap( wxT(\"") << cppString << wxT("\"), wxBITMAP_TYPE_ANY )");
				}
			}
			else if ( source == wxT("Load From Resource") )
			{
				result << wxT("wxBitmap( wxT(\"") << path << wxT("\"), wxBITMAP_TYPE_RESOURCE )");
			}
			else if ( source == wxT("Load From Icon Resource") )
			{
				result << wxT("wxICON( ") << path << wxT(" )");
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

CppCodeGenerator::CppCodeGenerator()
{
	SetupPredefinedMacros();
	m_useRelativePath = false;
	m_i18n = false;
	m_firstID = 1000;
}

wxString CppCodeGenerator::ConvertCppString( wxString text )
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

wxString CppCodeGenerator::ConvertXpmName( const wxString& text )
{
	wxString name = text;
	// the name consists of extracting the name of the file (without the directory)
	// and replacing the character '.' by ' _ '.

	size_t last_slash = name.find_last_of( wxT("\\/") );
	if ( last_slash != name.npos )
	{
		name = name.substr( last_slash + 1 );
	}

	name.replace( name.rfind( wxT(".") ), 1, wxT("_") );

	return name;
}

void CppCodeGenerator::GenerateInheritedClass( PObjectBase userClasses )
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

	wxString code = GetCode( userClasses, wxT("guard_macro_open") );
	m_header->WriteLn( code );
	m_header->WriteLn( wxEmptyString );

	code = GetCode( userClasses, wxT("header_comment") );
	m_header->WriteLn( code );
	m_header->WriteLn( wxEmptyString );

	code = GetCode( userClasses, wxT("header_include") );
	m_header->WriteLn( code );
	m_header->WriteLn( wxEmptyString );

	code = GetCode( userClasses, wxT("class_decl") );
	m_header->WriteLn( code );
	m_header->WriteLn( wxT("{") );
	m_header->WriteLn( wxT("public:") );

	m_header->Indent();
	code = GetCode( userClasses, type + wxT("_cons_decl") );
	m_header->WriteLn( code );
	m_header->Unindent();

	m_header->WriteLn( wxT("};") );
	m_header->WriteLn( wxEmptyString );
	code = GetCode( userClasses, wxT("guard_macro_close") );
	m_header->WriteLn( code );

	code = GetCode( userClasses, wxT("source_include") );
	m_source->WriteLn( code );
	m_source->WriteLn( wxEmptyString );

	code = GetCode( userClasses, type + wxT("_cons_def") );
	m_source->WriteLn( code );
	m_source->WriteLn( wxT("{") );
	m_source->WriteLn( wxEmptyString );
	m_source->WriteLn( wxT("}") );
}

bool CppCodeGenerator::GenerateCode( PObjectBase project )
{
	if (!project)
	{
		wxLogError(wxT("There is no project to generate code"));
		return false;
	}

	bool useEnum = false;

	PProperty useEnumProperty = project->GetProperty( wxT("use_enum") );
	if (useEnumProperty && useEnumProperty->GetValueAsInteger())
		useEnum = true;

	m_i18n = false;
	PProperty i18nProperty = project->GetProperty( wxT("internationalize") );
	if (i18nProperty && i18nProperty->GetValueAsInteger())
		m_i18n = true;

	m_header->Clear();
	m_source->Clear();
	wxString code (
		wxT("///////////////////////////////////////////////////////////////////////////\n")
		wxT("// C++ code generated with wxFormBuilder (version ") wxT(__DATE__) wxT(")\n")
		wxT("// http://www.wxformbuilder.org/\n")
		wxT("//\n")
		wxT("// PLEASE DO \"NOT\" EDIT THIS FILE!\n")
		wxT("///////////////////////////////////////////////////////////////////////////\n") );

	m_header->WriteLn( code );
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

	m_header->WriteLn( wxT("#ifndef __") + file + wxT("__") );
	m_header->WriteLn( wxT("#define __") + file + wxT("__") );
	m_header->WriteLn( wxT("") );

	code = GetCode( project, wxT("header_preamble") );
	m_header->WriteLn( code );

	// Generate the subclass sets
	std::set< wxString > subclasses;
	std::set< wxString > subclassSourceIncludes;
	std::vector< wxString > headerIncludes;

	GenSubclassSets( project, &subclasses, &subclassSourceIncludes, &headerIncludes );

	// Write the forward declaration lines
	std::set< wxString >::iterator subclass_it;
	for ( subclass_it = subclasses.begin(); subclass_it != subclasses.end(); ++subclass_it )
	{
		m_header->WriteLn( *subclass_it );
	}
	if ( !subclasses.empty() )
	{
		m_header->WriteLn( wxT("") );
	}

	// generamos en el h los includes de las dependencias de los componentes.
	std::set< wxString > templates;
	GenIncludes(project, &headerIncludes, &templates );

	// Write the include lines
	std::vector<wxString>::iterator include_it;
	for ( include_it = headerIncludes.begin(); include_it != headerIncludes.end(); ++include_it )
	{
		m_header->WriteLn( *include_it );
	}
	if ( !headerIncludes.empty() )
	{
		m_header->WriteLn( wxT("") );
	}

	code = GetCode( project, wxT("header_epilogue") );
	m_header->WriteLn( code );
	m_header->WriteLn( wxT("") );

	// Precompiled Headers
	PProperty pch_prop = project->GetProperty( wxT("precompiled_header") );
	if ( pch_prop )
	{
		wxString pch = pch_prop->GetValueAsString();
		if ( !pch.empty() )
		{
			m_source->WriteLn( wxT("#include \"") + pch + wxT("\"") );
			m_source->WriteLn( wxEmptyString );
		}
	}

	// en el cpp generamos el include del .h generado y los xpm
	code = GetCode( project, wxT("cpp_preamble") );
	m_source->WriteLn( code );
	m_source->WriteLn( wxEmptyString );

	// Write include lines for subclasses
	for ( subclass_it = subclassSourceIncludes.begin(); subclass_it != subclassSourceIncludes.end(); ++subclass_it )
	{
		m_source->WriteLn( *subclass_it );
	}
	if ( !subclassSourceIncludes.empty() )
	{
		m_source->WriteLn( wxEmptyString );
	}

	// Generated header
	m_source->WriteLn( wxT("#include \"") + file + wxT(".h\""));

	m_source->WriteLn( wxEmptyString );
	GenXpmIncludes( project );

	code = GetCode( project, wxT("cpp_epilogue") );
	m_source->WriteLn( code );

	// generamos los defines de las macros
	if ( !useEnum )
	{
		GenDefines( project );
	}

	for ( unsigned int i = 0; i < project->GetChildCount(); i++ )
	{
	  EventVector events;
	  FindEventHandlers(project->GetChild(i), events);

		GenClassDeclaration( project->GetChild( i ), useEnum, events);
		GenEventTable( project->GetChild(i), events );
		GenConstructor( project->GetChild( i ) );
	}

	m_header->WriteLn( wxT("#endif //__") + file + wxT("__") );

	return true;
}

void CppCodeGenerator::GenEventTable( PObjectBase class_obj, const EventVector &events)
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
		wxLogError( wxT("Object name cannot be null") );
		return;
	}

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
		base_class = wxT("wx") + class_obj->GetClassName();

	if ( events.size() > 0 )
	{
		m_source->WriteLn( wxT("BEGIN_EVENT_TABLE( ") + class_name + wxT(", ") + base_class + wxT(" )") );
		m_source->Indent();
		for ( size_t i = 0; i < events.size(); i++ )
		{
			PEvent event = events[i];
			wxString handlerName = class_name + wxT("::_wxFB_") + event->GetValue();
			wxString templateName = wxT("evt_entry_") + event->GetName();

			PObjectBase obj = event->GetObject();
			if ( !GenEventTableEntry( obj, obj->GetObjectInfo(), templateName, handlerName ) )
			{
				wxLogError( wxT("Missing \"%s\" template for \"%s\" class. Review your XML object description"),
					templateName.c_str(), class_name.c_str() );
			}
		}
		m_source->Unindent();
		m_source->WriteLn( wxT("END_EVENT_TABLE()") );
	}
}

bool CppCodeGenerator::GenEventTableEntry( PObjectBase obj, PObjectInfo obj_info, const wxString& templateName, const wxString& handlerName )
{
	wxString _template;
	PCodeInfo code_info = obj_info->GetCodeInfo( wxT("C++") );
	if ( code_info )
	{
		_template = code_info->GetTemplate( templateName );
		if ( !_template.empty() )
		{
			_template.Replace( wxT("#handler"), handlerName.c_str() ); // Ugly patch!
			CppTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath );
			m_source->WriteLn( parser.ParseTemplate() );
			return true;
		}
	}

	for ( unsigned int i = 0; i < obj_info->GetBaseClassCount(); i++ )
	{
		PObjectInfo base_info = obj_info->GetBaseClass( i );
		if ( GenEventTableEntry( obj, base_info, templateName, handlerName ) )
		{
			return true;
		}
	}

	return false;
}

void CppCodeGenerator::GenPrivateEventHandlers( const EventVector& events )
{
	if ( events.size() > 0 )
	{
		m_header->WriteLn( wxEmptyString );
		m_header->WriteLn( wxT("// Private event handlers") );

		for ( size_t i = 0; i < events.size(); i++ )
		{
			PEvent event = events[i];
			wxString aux;

			aux = wxT("void _wxFB_") + event->GetValue() + wxT("( ") +
			event->GetEventInfo()->GetEventClassName() + wxT("& event ){ ") +
			event->GetValue() + wxT("( event ); }");

			m_header->WriteLn( aux );
		}
		m_header->WriteLn( wxEmptyString );
	}
}

void CppCodeGenerator::GenVirtualEventHandlers( const EventVector& events )
{
	if ( events.size() > 0 )
	{
		// There are problems if we create "pure" virtual handlers, because some
		// events could be triggered in the constructor in which virtual methods are
		// execute properly.
		// So we create a default handler which will skip the event.
		m_header->WriteLn( wxEmptyString );
		m_header->WriteLn( wxT("// Virtual event handlers, overide them in your derived class") );

		for ( size_t i = 0; i < events.size(); i++ )
		{
			PEvent event = events[i];
			wxString aux;

			aux = wxT("virtual void ") + event->GetValue() + wxT("( ") +
			event->GetEventInfo()->GetEventClassName() + wxT("& event ){ event.Skip(); }");

			m_header->WriteLn(aux);
		}
		m_header->WriteLn( wxEmptyString );
	}
}

void CppCodeGenerator::GenAttributeDeclaration(PObjectBase obj, Permission perm)
{
	wxString typeName = obj->GetObjectTypeName();
	if (typeName == wxT("notebook")			||
		typeName == wxT("flatnotebook")		||
		typeName == wxT("listbook")			||
		typeName == wxT("choicebook")		||
		typeName == wxT("widget")			||
		typeName == wxT("expanded_widget")	||
		typeName == wxT("statusbar")			||
		typeName == wxT("component")			||
		typeName == wxT("container")			||
		typeName == wxT("menubar")			||
		typeName == wxT("toolbar")			||
		typeName == wxT("splitter")
		)
	{
		wxString perm_str = obj->GetProperty( wxT("permission") )->GetValue();

		if ((perm == P_PUBLIC && perm_str == wxT("public") ) ||
			(perm == P_PROTECTED && perm_str == wxT("protected") ) ||
			(perm == P_PRIVATE && perm_str == wxT("private") ) )
		{
			// Generate the declaration
			wxString code = GetCode( obj, wxT("declaration") );
			m_header->WriteLn(code);
		}
	}

	// recursivamente generamos los demás atributos
	for (unsigned int i = 0; i < obj->GetChildCount() ; i++)
	{
		PObjectBase child = obj->GetChild(i);

		GenAttributeDeclaration(child,perm);
	}
}

wxString CppCodeGenerator::GetCode(PObjectBase obj, wxString name)
{
	wxString _template;
	PCodeInfo code_info = obj->GetObjectInfo()->GetCodeInfo( wxT("C++") );

	if (!code_info)
	{
		wxString msg( wxString::Format( wxT("Missing \"%s\" template for \"%s\" class. Review your XML object description"),
			name.c_str(), obj->GetClassName().c_str() ) );
		wxLogError(msg);
		return wxT("");
	}

	_template = code_info->GetTemplate(name);

	CppTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath );
	wxString code = parser.ParseTemplate();

	return code;
}

void CppCodeGenerator::GenClassDeclaration(PObjectBase class_obj, bool use_enum, const EventVector &events)
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

	m_header->WriteLn( wxT("///////////////////////////////////////////////////////////////////////////////") );
	m_header->WriteLn( wxT("/// Class ") + class_name);
	m_header->WriteLn( wxT("///////////////////////////////////////////////////////////////////////////////") );

	m_header->WriteLn( wxT("class ") + class_name + wxT(" : ") + GetCode( class_obj, wxT("base") ) );
	m_header->WriteLn( wxT("{") );
	m_header->Indent();

	// are there event handlers?
	if (events.size() > 0)
	  m_header->WriteLn(wxT("DECLARE_EVENT_TABLE()"));

	// private
	m_header->WriteLn( wxT("private:") );
	m_header->Indent();
	GenAttributeDeclaration(class_obj,P_PRIVATE);

	GenPrivateEventHandlers(events);

	m_header->Unindent();
	m_header->WriteLn( wxT("") );

	// protected
	m_header->WriteLn( wxT("protected:") );
	m_header->Indent();

	if (use_enum)
		GenEnumIds(class_obj);

	GenAttributeDeclaration(class_obj,P_PROTECTED);
	GenVirtualEventHandlers(events);

	m_header->Unindent();
	m_header->WriteLn( wxT("") );

	// public
	m_header->WriteLn( wxT("public:") );
	m_header->Indent();
	GenAttributeDeclaration(class_obj,P_PUBLIC);

	// The constructor is also included within public
	m_header->WriteLn( GetCode( class_obj, wxT("cons_decl") ) );
	m_header->Unindent();
	m_header->WriteLn( wxT("") );

	m_header->Unindent();
	m_header->WriteLn( wxT("};") );
	m_header->WriteLn( wxT("") );
}

void CppCodeGenerator::GenEnumIds(PObjectBase class_obj)
{
	std::vector< wxString > macros;
	FindMacros( class_obj, &macros );

	std::vector< wxString >::iterator it = macros.begin();
	if ( it != macros.end())
	{
		m_header->WriteLn( wxT("enum") );
		m_header->WriteLn( wxT("{") );
		m_header->Indent();

        // Remove the default macro from the set, for backward compatiblity
        it = std::find( macros.begin(), macros.end(), wxT("ID_DEFAULT") );
        if ( it != macros.end() )
        {
            // The default macro is defined to wxID_ANY
            m_header->WriteLn( wxT("ID_DEFAULT = wxID_ANY, // Default") );
            macros.erase(it);
        }

		it = macros.begin();
        if ( it != macros.end())
        {
            m_header->WriteLn( wxString::Format( wxT("%s = %i,"), it->c_str(), m_firstID ) );
            it++;
            while ( it != macros.end() )
            {
                m_header->WriteLn( *it + wxT(",") );
                it++;
            }
        }

		//m_header->WriteLn(id);
		m_header->Unindent();
		m_header->WriteLn( wxT("};") );
		m_header->WriteLn( wxEmptyString );
	}
}

void CppCodeGenerator::GenSubclassSets( PObjectBase obj, std::set< wxString >* subclasses, std::set< wxString >* sourceIncludes, std::vector< wxString >* headerIncludes )
{
	// Call GenSubclassForwardDeclarations on all children as well
	for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
	{
		GenSubclassSets( obj->GetChild( i ), subclasses, sourceIncludes, headerIncludes );
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

		// Got a subclass
		subclasses->insert( wxT("class ") + nameVal + wxT(";") );

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

		wxString include = wxT("#include \"") + headerVal + wxT("\"");
		if ( pkg->GetPackageName() == wxT("Forms") )
		{
			std::vector< wxString >::iterator it = std::find( headerIncludes->begin(), headerIncludes->end(), include );
			if ( headerIncludes->end() == it )
			{
				headerIncludes->push_back( include );
			}
		}
		else
		{
			sourceIncludes->insert( include );
		}
	}
}

void CppCodeGenerator::GenIncludes( PObjectBase project, std::vector<wxString>* includes, std::set< wxString >* templates )
{
	GenObjectIncludes( project, includes, templates );
}

void CppCodeGenerator::GenObjectIncludes( PObjectBase project, std::vector< wxString >* includes, std::set< wxString >* templates )
{
	// Call GenIncludes on all children as well
	for ( unsigned int i = 0; i < project->GetChildCount(); i++ )
	{
		GenObjectIncludes( project->GetChild(i), includes, templates );
	}

	// Fill the set
	PCodeInfo code_info = project->GetObjectInfo()->GetCodeInfo( wxT("C++") );
	if (code_info)
	{
		CppTemplateParser parser( project, code_info->GetTemplate( wxT("include") ), m_i18n, m_useRelativePath, m_basePath );
		wxString include = parser.ParseTemplate();
		if ( !include.empty() )
		{
			if ( templates->insert( include ).second )
			{
				AddUniqueIncludes( include, includes );
			}
		}
	}

	// Generate includes for base classes
	GenBaseIncludes( project->GetObjectInfo(), project, includes, templates );
}

void CppCodeGenerator::GenBaseIncludes( PObjectInfo info, PObjectBase obj, std::vector< wxString >* includes, std::set< wxString >* templates )
{
	if ( !info )
	{
		return;
	}

	// Process all the base classes recursively
	for ( unsigned int i = 0; i < info->GetBaseClassCount(); i++ )
	{
		PObjectInfo base_info = info->GetBaseClass( i );
		GenBaseIncludes( base_info, obj, includes, templates );
	}

	PCodeInfo code_info = info->GetCodeInfo( wxT("C++") );
	if ( code_info )
	{
		CppTemplateParser parser( obj, code_info->GetTemplate( wxT("include") ), m_i18n, m_useRelativePath, m_basePath );
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

void CppCodeGenerator::AddUniqueIncludes( const wxString& include, std::vector< wxString >* includes )
{
	// Split on newlines to only generate unique include lines
	// This strips blank lines and trims
	wxStringTokenizer tkz( include, wxT("\n"), wxTOKEN_STRTOK );

	bool inPreproc = false;

	while ( tkz.HasMoreTokens() )
	{
		wxString line = tkz.GetNextToken();
		line.Trim( false );
		line.Trim( true );

		// Anything within a #if preprocessor block will be written
		if ( line.StartsWith( wxT("#if") ) )
		{
			inPreproc = true;
		}
		else if ( line.StartsWith( wxT("#endif") ) )
		{
			inPreproc = false;
		}

		if ( inPreproc )
		{
			includes->push_back( line );
			continue;
		}

		// If it is not an include line, it will be written
		if ( !line.StartsWith( wxT("#include") ) )
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

void CppCodeGenerator::FindDependencies( PObjectBase obj, std::set< PObjectInfo >& info_set )
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

void CppCodeGenerator::GenConstructor(PObjectBase class_obj)
{
	m_source->WriteLn( wxT("") );
	m_source->WriteLn( GetCode( class_obj, wxT("cons_def") ) );
	m_source->WriteLn( wxT("{") );
	m_source->Indent();

	wxString settings = GetCode( class_obj, wxT("settings") );
	if ( !settings.empty() )
	{
		m_source->WriteLn( settings );
	}

	for ( unsigned int i = 0; i < class_obj->GetChildCount(); i++ )
	{
		GenConstruction( class_obj->GetChild( i ), true );
	}

	m_source->Unindent();
	m_source->WriteLn( wxT("}") );
}

void CppCodeGenerator::GenConstruction(PObjectBase obj, bool is_widget)
{
	wxString type = obj->GetObjectTypeName();

	if (type == wxT("notebook")			||
		type == wxT("flatnotebook")		||
		type == wxT("listbook")			||
		type == wxT("choicebook")		||
		type == wxT("widget")			||
		type == wxT("expanded_widget")	||
		type == wxT("statusbar")			||
		type == wxT("container")			||
		type == wxT("menubar")			||
		type == wxT("toolbar")			||
		type == wxT("splitter")
		)
	{
		// comprobamos si no se ha declarado como atributo de clase
		// en cuyo caso lo declaramos en el constructor

		wxString perm_str = obj->GetProperty( wxT("permission") )->GetValue();
		if ( perm_str == wxT("none") )
		{
			m_source->WriteLn( GetCode( obj, wxT("declaration") ) );
		}

		m_source->WriteLn( GetCode( obj, wxT("construction") ) );
		GenSettings( obj->GetObjectInfo(), obj );

		for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
		{
			PObjectBase child = obj->GetChild( i );
			GenConstruction( child, true );

			if ( type == wxT("toolbar") )
			{
				GenAddToolbar(child->GetObjectInfo(), child);
			}
		}

		if ( type == wxT("splitter") )
		{
			// generamos el split
			switch ( obj->GetChildCount() )
			{
				case 1:
				{
					PObjectBase sub1 = obj->GetChild(0)->GetChild(0);
					wxString _template = wxT("$name->Initialize( ");
					_template = _template + sub1->GetProperty( wxT("name") )->GetValue() + wxT(" )");

					CppTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath );
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
						_template = wxT("$name->SplitVertically( ");
					}
					else
					{
						_template = wxT("$name->SplitHorizontally( ");
					}

					_template = _template + sub1->GetProperty( wxT("name") )->GetValue() +
						wxT(", ") + sub2->GetProperty( wxT("name") )->GetValue() + wxT(", $sashpos );");

					CppTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath );
					m_source->WriteLn(parser.ParseTemplate());
					break;
				}
				default:
					wxLogError( wxT("Missing subwindows for wxSplitterWindow widget.") );
					break;
			}
		}


		if (type == wxT("menubar") || type == wxT("toolbar") || type == wxT("listbook") ||
			type == wxT("notebook") || type == wxT("flatnotebook") )
		{
			wxString afterAddChild = GetCode( obj, wxT("after_addchild") );
			if ( !afterAddChild.empty() )
			{
				m_source->WriteLn( afterAddChild );
			}
			m_source->WriteLn( wxT("") );
		}

	}
	else if ( type == wxT("sizer") )
	{
		m_source->WriteLn( GetCode( obj, wxT("declaration") ) );
		m_source->WriteLn( GetCode( obj, wxT("construction") ) );
		GenSettings( obj->GetObjectInfo(), obj );

		for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
		{
			GenConstruction(obj->GetChild(i),false);
		}

		if (is_widget)
		{
			// the parent object is not a sizer. There is no template for
			// this so we'll make it manually.
			// It's not a good practice to embed templates into the source code,
			// because you will need to recompile...

			wxString _template =	wxT("#wxparent $name->SetSizer( $name ); #nl")
									wxT("#wxparent $name->Layout();")
									wxT("#ifnull #parent $size")
									wxT("@{ #nl $name->Fit( #wxparent $name ); @}");

			CppTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath );
			m_source->WriteLn(parser.ParseTemplate());
		}
	}
	else if ( type == wxT("menu") || type == wxT("submenu") )
	{
		m_source->WriteLn( GetCode( obj, wxT("declaration") ) );
		m_source->WriteLn( GetCode( obj, wxT("construction") ) );

		for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
		{
			GenConstruction( obj->GetChild(i), false );
		}

		m_source->WriteLn( GetCode( obj, wxT("menu_add") ) );

	}
	else if ( type == wxT("spacer") )
	{
		// En lugar de modelar un "spacer" como un objeto que se incluye en
		// un sizer item, los vamos a considerar un como un tipo de
		// de "sizeritem" capaz de existir por sí solo. De esta forma será
		// más facil la exportación XRC.
		m_source->WriteLn( GetCode( obj, wxT("spacer_add") ) );
	}
	else if ( type == wxT("sizeritem") )
	{
		// El hijo, hay que añadirlo al sizer teniendo en cuenta el tipo
		// del objeto hijo (hay 3 rutinas diferentes)
		GenConstruction(obj->GetChild(0),false);

		wxString child_type = obj->GetChild(0)->GetObjectTypeName();
		wxString temp_name;
		if (child_type == wxT("notebook")		||
			child_type == wxT("flatnotebook")	||
			child_type == wxT("listbook")		||
			child_type == wxT("choicebook")		||
			child_type == wxT("widget")			||
			child_type == wxT("expanded_widget")	||
			child_type == wxT("statusbar")		||
			child_type == wxT("container")		||
			child_type == wxT("splitter")
			)
		{
			temp_name = wxT("window_add");
		}
		else if ( child_type == wxT("sizer") )
		{
			temp_name = wxT("sizer_add");
		}
		else
		{
			assert( false );
		}

		m_source->WriteLn( GetCode( obj, temp_name ) );
	}
	else if (	type == wxT("notebookpage")		||
				type == wxT("flatnotebookpage")	||
				type == wxT("listbookpage")		||
				type == wxT("choicebookpage")
			)
	{
		GenConstruction( obj->GetChild( 0 ), false );
		m_source->WriteLn( GetCode( obj, wxT("page_add") ) );
		GenSettings( obj->GetObjectInfo(), obj );
	}
	else if ( type == wxT("menuitem") )
	{
		m_source->WriteLn( GetCode( obj, wxT("construction") ) );
		m_source->WriteLn( GetCode( obj, wxT("menuitem_add") ) );
		GenSettings( obj->GetObjectInfo(), obj );
	}
	else if ( type == wxT("tool") )
	{
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

void CppCodeGenerator::FindMacros( PObjectBase obj, std::vector<wxString>* macros )
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
			// Skip wx IDs
            if ( !value.StartsWith( wxT("wxID_") ) )
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

void CppCodeGenerator::FindEventHandlers(PObjectBase obj, EventVector &events)
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

void CppCodeGenerator::GenDefines( PObjectBase project)
{
	std::vector< wxString > macros;
	FindMacros( project, &macros );

	// Remove the default macro from the set, for backward compatiblity
	std::vector< wxString >::iterator it;
	it = std::find( macros.begin(), macros.end(), wxT("ID_DEFAULT") );
	if ( it != macros.end() )
	{
		// The default macro is defined to wxID_ANY
		m_header->WriteLn( wxT("#define ID_DEFAULT wxID_ANY // Default") );
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
        m_header->WriteLn( wxString::Format( wxT("#define %s %i"), it->c_str(), id ) );
        id++;
	}

	m_header->WriteLn( wxT("") );
}

void CppCodeGenerator::GenSettings(PObjectInfo info, PObjectBase obj)
{
	wxString _template;
	PCodeInfo code_info = info->GetCodeInfo( wxT("C++") );

	if ( !code_info )
	{
		return;
	}

	_template = code_info->GetTemplate( wxT("settings") );

	if ( !_template.empty() )
	{
		CppTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath );
		wxString code = parser.ParseTemplate();
		if ( !code.empty() )
		{
			m_source->WriteLn(code);
		}
	}

	// procedemos recursivamente con las clases base
	for (unsigned int i=0; i< info->GetBaseClassCount(); i++)
	{
		PObjectInfo base_info = info->GetBaseClass(i);
		GenSettings(base_info,obj);
	}
}

void CppCodeGenerator::GenAddToolbar(PObjectInfo info, PObjectBase obj)
{
	wxString _template;
	PCodeInfo code_info = info->GetCodeInfo( wxT("C++") );

	if (!code_info)
		return;

	_template = code_info->GetTemplate( wxT("toolbar_add") );

	if ( !_template.empty() )
	{
		CppTemplateParser parser( obj, _template, m_i18n, m_useRelativePath, m_basePath );
		wxString code = parser.ParseTemplate();
		if ( !code.empty() )
		{
			m_source->WriteLn(code);
		}
	}

	// procedemos recursivamente con las clases base
	for (unsigned int i=0; i< info->GetBaseClassCount(); i++)
	{
		PObjectInfo base_info = info->GetBaseClass(i);
		GenAddToolbar(base_info,obj);
	}

}

///////////////////////////////////////////////////////////////////////


void CppCodeGenerator::GenXpmIncludes( PObjectBase project)
{
	std::set< wxString > include_set;

	// lo primero es obtener la lista de includes
	FindXpmProperties( project, include_set );

	if ( include_set.empty() )
	{
		return;
	}

	// y los generamos
	std::set<wxString>::iterator it;
	for ( it = include_set.begin() ; it != include_set.end(); it++ )
	{
		if ( !it->empty() )
		{
			m_source->WriteLn( *it );
		}
	}

	m_source->WriteLn( wxT("") );
}

void CppCodeGenerator::FindXpmProperties( PObjectBase obj, std::set<wxString>& xpmset )
{
	// recorremos cada una de las propiedades del objeto obj, si damos con
	// alguna que sea de tipo PT_XPM_BITMAP añadimos la cadena del "include"
	// en set. Luego recursivamente hacemos lo mismo con los hijos.
	unsigned int i, count;

	count = obj->GetPropertyCount();

	for (i = 0; i < count; i++)
	{
		PProperty property = obj->GetProperty(i);
		if ( property->GetType() == PT_BITMAP )
		{
			wxString path = property->GetValue();
			size_t semicolonindex = path.find_first_of( wxT(";") );
			if ( semicolonindex != path.npos )
			{
				path = path.substr( 0, semicolonindex );
			}

			wxFileName bmpFileName( path );
			if ( bmpFileName.GetExt().Upper() == wxT("XPM") )
			{
				wxString absPath = TypeConv::MakeAbsolutePath( path, AppData()->GetProjectPath() );

				// Se supone el path contiene la ruta completa del archivo y no
				// una relativa.
				wxString relPath = ( m_useRelativePath ? TypeConv::MakeRelativePath( absPath, AppData()->GetProjectPath() ) : absPath );

				wxString inc;
				inc << wxT("#include \"") << ConvertCppString( relPath ) << wxT("\"");
				xpmset.insert(inc);
			}
		}
	}

	count = obj->GetChildCount();
	for (i = 0; i< count; i++)
	{
		PObjectBase child = obj->GetChild(i);
		FindXpmProperties( child, xpmset );
	}
}

void CppCodeGenerator::UseRelativePath(bool relative, wxString basePath)
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
void CppCodeGenerator::SetupPredefinedMacros()
{
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

	ADD_PREDEFINED_MACRO(wxID_FILE1);
	ADD_PREDEFINED_MACRO(wxID_FILE2);
	ADD_PREDEFINED_MACRO(wxID_FILE3);
	ADD_PREDEFINED_MACRO(wxID_FILE4);
	ADD_PREDEFINED_MACRO(wxID_FILE5);
	ADD_PREDEFINED_MACRO(wxID_FILE6);
	ADD_PREDEFINED_MACRO(wxID_FILE7);
	ADD_PREDEFINED_MACRO(wxID_FILE8);
	ADD_PREDEFINED_MACRO(wxID_FILE9);

	// Standard button IDs
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

	// System menu IDs (used by wxUniv):
	ADD_PREDEFINED_MACRO(wxID_SYSTEM_MENU);
	ADD_PREDEFINED_MACRO(wxID_CLOSE_FRAME);
	ADD_PREDEFINED_MACRO(wxID_MOVE_FRAME);
	ADD_PREDEFINED_MACRO(wxID_RESIZE_FRAME);
	ADD_PREDEFINED_MACRO(wxID_MAXIMIZE_FRAME);
	ADD_PREDEFINED_MACRO(wxID_ICONIZE_FRAME);
	ADD_PREDEFINED_MACRO(wxID_RESTORE_FRAME);

	// IDs used by generic file dialog (13 consecutive starting from this value)

	ADD_PREDEFINED_MACRO(wxID_FILEDLGG);

	ADD_PREDEFINED_MACRO(wxID_HIGHEST);

}
