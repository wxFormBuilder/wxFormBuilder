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

#include <wx/filename.h>

CppTemplateParser::CppTemplateParser(shared_ptr<ObjectBase> obj, wxString _template)
: TemplateParser(obj,_template)
{
	m_useRelativePath = false;
	m_i18n = false;
}
wxString CppTemplateParser::RootWxParentToCode()
{
	return wxT("this");
}

shared_ptr<TemplateParser> CppTemplateParser::CreateParser(shared_ptr<ObjectBase> obj, wxString _template)
{
	shared_ptr<TemplateParser> newparser(new CppTemplateParser(obj,_template));
	return newparser;
}

void CppTemplateParser::UseRelativePath(bool relative, wxString basePath)
{
	m_useRelativePath = relative;

	if (m_useRelativePath)
	{
		bool result = wxFileName::DirExists( basePath );
		m_basePath = ( result ? basePath : wxT("") );
	}
}

void CppTemplateParser::UseI18n(bool i18n)
{
	m_i18n = i18n;
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

bool CppCodeGenerator::GenerateCode( shared_ptr<ObjectBase> project )
{
	if (!project)
	{
		wxLogError(wxT("There is no project to generate code"));
		return false;
	}

	bool useEnum = false;

	shared_ptr< Property > useEnumProperty = project->GetProperty( wxT("use_enum") );
	if (useEnumProperty && useEnumProperty->GetValueAsInteger())
		useEnum = true;

	m_i18n = false;
	shared_ptr< Property > i18nProperty = project->GetProperty( wxT("internationalize") );
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

	shared_ptr<Property> propFile = project->GetProperty( wxT("file") );
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

	// generamos en el h los includes de las dependencias de los componentes.
	set<wxString> includes;
	GenIncludes(project, &includes);

	// Write the include lines
	set<wxString>::iterator include_it;
	for ( include_it = includes.begin(); include_it != includes.end(); ++include_it )
	{
		m_header->WriteLn( *include_it );
	}
	if ( !includes.empty() )
	{
		m_header->WriteLn( wxT("") );
	}

	// Generate the subclass set
	set< wxString > subclasses;
	GenSubclassForwardDeclarations( project, &subclasses );

	// Write the forward declaration lines
	set< wxString >::iterator subclass_it;
	for ( subclass_it = subclasses.begin(); subclass_it != subclasses.end(); ++subclass_it )
	{
		m_header->WriteLn( *subclass_it );
	}
	if ( !subclasses.empty() )
	{
		m_header->WriteLn( wxT("") );
	}

	code = GetCode( project, wxT("header_epilogue") );
	m_header->WriteLn( code );
	m_header->WriteLn( wxT("") );

	// en el cpp generamos el include del .h generado y los xpm
	code = GetCode( project, wxT("cpp_preamble") );
	m_source->WriteLn( code );
	m_source->WriteLn( wxT("") );

	// User Headers
	set< wxString > user_headers;
	shared_ptr<Property> user_headers_prop = project->GetProperty( wxT("user_headers") );
	if ( user_headers_prop )
	{
		wxArrayString array = user_headers_prop->GetValueAsArrayString();
		for ( unsigned int i = 0; i < array.Count(); i++ )
		{
			if ( user_headers.insert( array[i] ).second )
			{
				m_source->WriteLn( wxT("#include \"") + array[i] + wxT("\"") );
			}
		}
	}

	// Generated header
	m_source->WriteLn( wxT("#include \"") + file + wxT(".h\""));

	m_source->WriteLn( wxT("") );
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
		GenClassDeclaration( project->GetChild( i ), useEnum );
		GenConstructor( project->GetChild( i ) );
	}

	m_header->WriteLn( wxT("#endif //__") + file + wxT("__") );

	return true;
}

void CppCodeGenerator::GenAttributeDeclaration(shared_ptr<ObjectBase> obj, Permission perm)
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
		shared_ptr<ObjectBase> child = obj->GetChild(i);

		GenAttributeDeclaration(child,perm);
	}
}

wxString CppCodeGenerator::GetCode(shared_ptr<ObjectBase> obj, wxString name)
{
	wxString _template;
	shared_ptr<CodeInfo> code_info = obj->GetObjectInfo()->GetCodeInfo( wxT("C++") );
	if (!code_info)
	{
		wxString msg( wxString::Format( wxT("Missing \"%s\" template for \"%s\" class. Review your XML object description"),
			name.c_str(), obj->GetClassName().c_str() ) );
		wxLogError(msg);
		return wxT("");
	}

	_template = code_info->GetTemplate(name);

	CppTemplateParser parser(obj,_template);
	parser.UseRelativePath(m_useRelativePath, m_basePath);
	parser.UseI18n(m_i18n);
	wxString code = parser.ParseTemplate();

	return code;
}

void CppCodeGenerator::GenClassDeclaration(shared_ptr<ObjectBase> class_obj, bool use_enum)
{
	shared_ptr<Property> propName = class_obj->GetProperty( wxT("name") );
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

	m_header->WriteLn( wxT("/**") );
	m_header->WriteLn( wxT(" * Class ") + class_name);
	m_header->WriteLn( wxT(" */") );

	m_header->WriteLn( wxT("class ") + class_name + wxT(" : ") + GetCode( class_obj, wxT("base") ) );
	m_header->WriteLn( wxT("{") );
	m_header->Indent();

	// private
	m_header->WriteLn( wxT("private:") );
	m_header->Indent();
	GenAttributeDeclaration(class_obj,P_PRIVATE);
	m_header->Unindent();
	m_header->WriteLn( wxT("") );

	// protected
	m_header->WriteLn( wxT("protected:") );
	m_header->Indent();

	if (use_enum)
		GenEnumIds(class_obj);

	GenAttributeDeclaration(class_obj,P_PROTECTED);
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

void CppCodeGenerator::GenEnumIds(shared_ptr< ObjectBase > class_obj)
{
	set<wxString> macro_set;
	FindMacros(class_obj,macro_set);

	set<wxString>::iterator it = macro_set.begin();
	if ( it != macro_set.end())
	{
		m_header->WriteLn( wxT("enum") );
		m_header->WriteLn( wxT("{") );
		m_header->Indent();

		m_header->WriteLn( wxString::Format( wxT("%s = %i,"), it->c_str(), m_firstID ) );
		it++;
		while ( it != macro_set.end() )
		{
			m_header->WriteLn( *it + wxT(",") );
			it++;
		}

		//m_header->WriteLn(id);
		m_header->Unindent();
		m_header->WriteLn( wxT("};") );
		m_header->WriteLn( wxT("") );
	}
}

void CppCodeGenerator::GenSubclassForwardDeclarations( shared_ptr< ObjectBase > obj, set< wxString >* subclasses )
{
	// Call GenSubclassForwardDeclarations on all children as well
	for ( unsigned int i = 0; i < obj->GetChildCount(); i++ )
	{
		GenSubclassForwardDeclarations( obj->GetChild( i ), subclasses );
	}

	// Fill the set
	shared_ptr< Property > subclass = obj->GetProperty( wxT("subclass") );
	if ( subclass )
	{
		wxString val = subclass->GetValueAsString();
		if ( !val.empty() )
		{
			subclasses->insert( wxT("class ") + val + wxT(";") );
		}
	}
}

void CppCodeGenerator::GenIncludes( shared_ptr<ObjectBase> project, set<wxString>* includes)
{
	GenObjectIncludes( project, includes );
}

void CppCodeGenerator::GenObjectIncludes( shared_ptr< ObjectBase > project, set< wxString >* includes )
{
	// Call GenIncludes on all children as well
	for ( unsigned int i = 0; i < project->GetChildCount(); i++ )
	{
		GenObjectIncludes( project->GetChild(i), includes );
	}

	// Fill the set
	shared_ptr< CodeInfo > code_info = project->GetObjectInfo()->GetCodeInfo( wxT("C++") );
	if (code_info)
	{
		CppTemplateParser parser(project,code_info->GetTemplate( wxT("include") ) );
		wxString include = parser.ParseTemplate();
		if ( !include.empty() )
		{
			includes->insert( include );
		}
	}

	// Generate includes for base classes
	GenBaseIncludes( project->GetObjectInfo(), project, includes );
}

void CppCodeGenerator::GenBaseIncludes( shared_ptr< ObjectInfo > info, shared_ptr< ObjectBase > obj, set< wxString >* includes )
{
	if ( !info )
	{
		return;
	}

	// Process all the base classes recursively
	for ( unsigned int i = 0; i < info->GetBaseClassCount(); i++ )
	{
		shared_ptr< ObjectInfo > base_info = info->GetBaseClass( i );
		GenBaseIncludes( base_info, obj, includes );
	}

	shared_ptr< CodeInfo > code_info = info->GetCodeInfo( wxT("C++") );
	if ( code_info )
	{
		CppTemplateParser parser( obj, code_info->GetTemplate( wxT("include") ) );
		wxString include = parser.ParseTemplate();
		if ( !include.empty() )
		{
			includes->insert( include );
		}
	}
}

void CppCodeGenerator::FindDependencies( shared_ptr< ObjectBase > obj, set< shared_ptr< ObjectInfo > >& info_set )
{
	unsigned int ch_count = obj->GetChildCount();
	if (ch_count > 0)
	{
		unsigned int i;
		for (i = 0; i<ch_count; i++)
		{
			shared_ptr<ObjectBase> child = obj->GetChild(i);
			info_set.insert(child->GetObjectInfo());
			FindDependencies(child, info_set);
		}
	}
}

void CppCodeGenerator::GenConstructor(shared_ptr<ObjectBase> class_obj)
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

void CppCodeGenerator::GenConstruction(shared_ptr<ObjectBase> obj, bool is_widget)
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
			shared_ptr< ObjectBase > child = obj->GetChild( i );
			GenConstruction( child, true );

			if ( type == wxT("toolbar") )
			{
				GenAddToolbar(child->GetObjectInfo(), child);
			}
		}

		if ( type == wxT("splitter") )
		{
			// generamos el split
			if (obj->GetChildCount() == 2)
			{
				shared_ptr<ObjectBase> sub1,sub2;
				sub1 = obj->GetChild(0)->GetChild(0);
				sub2 = obj->GetChild(1)->GetChild(0);

				wxString _template;
				if ( obj->GetProperty( wxT("splitmode") )->GetValue() == wxT("wxSPLIT_VERTICAL") )
				{
					_template = wxT("$name->SplitVertically(");
				}
				else
				{
					_template = wxT("$name->SplitHorizontally(");
				}

				_template = _template + sub1->GetProperty( wxT("name") )->GetValue() +
					wxT(",") + sub2->GetProperty( wxT("name") )->GetValue() + wxT(",$sashpos);");

				CppTemplateParser parser(obj,_template);
				parser.UseRelativePath(m_useRelativePath, m_basePath);
				parser.UseI18n(m_i18n);
				m_source->WriteLn(parser.ParseTemplate());
			}
			else
				wxLogError( wxT("Missing subwindows for wxSplitterWindow widget.") );
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

			CppTemplateParser parser(obj,_template);
			parser.UseRelativePath(m_useRelativePath, m_basePath);
			parser.UseI18n(m_i18n);
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

void CppCodeGenerator::FindMacros( shared_ptr< ObjectBase > obj, set< wxString >& macro_set )
{
	// recorre cada propiedad de cada objeto identificando aquellas
	// que sean macros, en cuyo caso la añade al conjunto.
	unsigned int i;

	for ( i = 0; i < obj->GetPropertyCount(); i++ )
	{
		shared_ptr<Property> prop = obj->GetProperty( i );
		if ( prop->GetType() == PT_MACRO )
		{
			wxString value = prop->GetValue();
			set< wxString >::iterator it = m_predMacros.find( value );
			if ( it == m_predMacros.end() )
			{
				macro_set.insert( prop->GetValue() );
			}
		}
		}

	for ( i = 0; i < obj->GetChildCount(); i++ )
	{
		FindMacros( obj->GetChild( i ), macro_set );
	}
}

void CppCodeGenerator::GenDefines( shared_ptr< ObjectBase > project)
{
	set< wxString > macro_set;
	FindMacros( project, macro_set );

	// la macro por defecto tiene valor -1
	m_header->WriteLn( wxT("#define ID_DEFAULT wxID_ANY // Default") );

	// debemos quitar la macro por defecto del conjunto
	set<wxString>::iterator it;
	it = macro_set.find( wxT("ID_DEFAULT") );
	if ( it != macro_set.end() )
	{
		macro_set.erase(it);
	}

	unsigned int id = m_firstID;
	if ( id < 1000 )
	{
		wxLogWarning(wxT("First ID is Less than 1000"));
	}
	for (it = macro_set.begin() ; it != macro_set.end(); it++)
	{
		m_header->WriteLn( wxString::Format( wxT("#define %s %i"), it->c_str(), id ) );
		id++;
	}

	m_header->WriteLn( wxT("") );
}

void CppCodeGenerator::GenSettings(shared_ptr<ObjectInfo> info, shared_ptr<ObjectBase> obj)
{
	wxString _template;
	shared_ptr<CodeInfo> code_info = info->GetCodeInfo( wxT("C++") );

	if ( !code_info )
	{
		return;
	}

	_template = code_info->GetTemplate( wxT("settings") );

	if ( !_template.empty() )
	{
		CppTemplateParser parser(obj,_template);
		parser.UseRelativePath(m_useRelativePath, m_basePath);
		parser.UseI18n(m_i18n);
		wxString code = parser.ParseTemplate();
		if ( !code.empty() )
		{
			m_source->WriteLn(code);
		}
	}

	// procedemos recursivamente con las clases base
	for (unsigned int i=0; i< info->GetBaseClassCount(); i++)
	{
		shared_ptr<ObjectInfo> base_info = info->GetBaseClass(i);
		GenSettings(base_info,obj);
	}
}

void CppCodeGenerator::GenAddToolbar(shared_ptr<ObjectInfo> info, shared_ptr<ObjectBase> obj)
{
	wxString _template;
	shared_ptr<CodeInfo> code_info = info->GetCodeInfo( wxT("C++") );

	if (!code_info)
		return;

	_template = code_info->GetTemplate( wxT("toolbar_add") );

	if ( !_template.empty() )
	{
		CppTemplateParser parser(obj,_template);
		parser.UseRelativePath(m_useRelativePath, m_basePath);
		parser.UseI18n(m_i18n);
		wxString code = parser.ParseTemplate();
		if ( !code.empty() )
		{
			m_source->WriteLn(code);
		}
	}

	// procedemos recursivamente con las clases base
	for (unsigned int i=0; i< info->GetBaseClassCount(); i++)
	{
		shared_ptr<ObjectInfo> base_info = info->GetBaseClass(i);
		GenAddToolbar(base_info,obj);
	}

}

///////////////////////////////////////////////////////////////////////


void CppCodeGenerator::GenXpmIncludes( shared_ptr< ObjectBase > project)
{
	set< wxString > include_set;

	// lo primero es obtener la lista de includes
	FindXpmProperties( project, include_set );

	if ( include_set.empty() )
	{
		return;
	}

	// y los generamos
	set<wxString>::iterator it;
	for ( it = include_set.begin() ; it != include_set.end(); it++ )
	{
		if ( !it->empty() )
		{
			m_source->WriteLn( *it );
		}
	}

	m_source->WriteLn( wxT("") );
}

void CppCodeGenerator::FindXpmProperties( shared_ptr<ObjectBase> obj, set<wxString>& set )
{
	// recorremos cada una de las propiedades del objeto obj, si damos con
	// alguna que sea de tipo PT_XPM_BITMAP añadimos la cadena del "include"
	// en set. Luego recursivamente hacemos lo mismo con los hijos.
	unsigned int i, count;

	count = obj->GetPropertyCount();

	for (i = 0; i < count; i++)
	{
		shared_ptr<Property> property = obj->GetProperty(i);
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
				set.insert(inc);
			}
		}
	}

	count = obj->GetChildCount();
	for (i = 0; i< count; i++)
	{
		shared_ptr<ObjectBase> child = obj->GetChild(i);
		FindXpmProperties( child, set );
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
