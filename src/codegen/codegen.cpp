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

#include "codegen.h"
#include "utils/debug.h"
#include "utils/typeconv.h"
#include "wx/wx.h"
#include <wx/tokenzr.h>
#include "rad/appdata.h"
#include "model/objectbase.h"

void CodeWriter::WriteLn(wxString code)
{
	// no se permitirán saltos de linea dentro de "code"
	// si los hubiera, FixWrite toma la cadena y la trocea en líneas
	// e inserta una a una mediante WriteLn
	if ( !StringOk( code ) )
	{
		FixWrite( code );
	}
	else
	{
		Write( code );
		Write( wxT("\n") );
		m_cols = 0;
	}
}

bool CodeWriter::StringOk(wxString s)
{
	if ( s.find( wxT("\n"), 0 ) == wxString::npos )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CodeWriter::FixWrite( wxString s )
{
	wxStringTokenizer tkz( s, wxT("\n"), wxTOKEN_RET_EMPTY_ALL );

	while ( tkz.HasMoreTokens() )
	{
		wxString line = tkz.GetNextToken();
		line.Trim( false );
		line.Trim( true );
		WriteLn( line );
	}
}


void CodeWriter::Write(wxString code)
{
	if (m_cols == 0)
	{
		// insertamos el indentado
		for ( int i = 0; i < m_indent; i++ )
		{
			DoWrite( wxT("\t") );
		}

		m_cols = m_indent;
	}


	// aquí debemos comprobar si hemos sobrepasado el maximo de columnas
	//  if (m_cols + code.length() > GetColumns())
	//    BreakLine(code)


	DoWrite( code );
}

TemplateParser::TemplateParser(PObjectBase obj, wxString _template)
: m_obj(obj), m_in(_template)
{
}

TemplateParser::Token TemplateParser::GetNextToken()
{
	// There are 3 special characters
	// #xxxx -> command
	// $xxxx -> property
	// %xxxx -> local variable
	// @x -> Escape a special character. Example: @# is the character #.

	Token result = TOK_ERROR;

	if (!m_in.Eof())
	{
		wxChar c = m_in.Peek();
		if ( c == wxT('#') )
			result = TOK_MACRO;
		else if (c == wxT('$') )
			result = TOK_PROPERTY;
		else
			result = TOK_TEXT;
	}

	return result;
}


bool TemplateParser::ParseMacro()
{
	Ident ident;

	ident = ParseIdent();
	switch (ident)
	{
	case ID_WXPARENT:
		return ParseWxParent();
		break;
	case ID_PARENT:
		return ParseParent();
		break;
	case ID_IFNOTNULL:
		return ParseIfNotNull();
		break;
	case ID_IFNULL:
		return ParseIfNull();
		break;
	case ID_FOREACH:
		return ParseForEach();
		break;
	case ID_PREDEFINED:
		return ParsePred();
		break;
	case ID_CHILD:
		return ParseChild();
		break;
	case ID_NEWLINE:
		return ParseNewLine();
		break;
	case ID_IFEQUAL:
		ParseIfEqual();
		break;
	case ID_IFNOTEQUAL:
		ParseIfNotEqual();
		break;
	case ID_APPEND:
		ParseAppend();
		break;
	case ID_CLASS:
		ParseClass();
		break;
	default:
		assert(false);
		return false;
		break;
	}

	return true;
}


TemplateParser::Ident TemplateParser::ParseIdent()
{
	Ident ident = ID_ERROR;

	if (!m_in.Eof())
	{
		wxString macro;
		m_in.GetC();

		wxChar peek( m_in.Peek() );
		while (peek != wxChar(EOF) && !m_in.Eof() && peek != wxT('#') && peek != wxT('$')
			&& ( (peek >= wxT('a') && peek <= wxT('z') ) ||
			(peek >= wxT('A') && peek <= wxT('Z') ) ||
			(peek >= wxT('0') && peek <= wxT('9') )))
		{
			macro += wxChar( m_in.GetC() );
			peek = wxChar( m_in.Peek() );
		}

		// buscar el identificador
		ident = SearchIdent( macro );
	}
	return ident;
}

wxString TemplateParser::ParsePropertyName( wxString* child )
{
	wxString propname;

	// children of parent properties can be referred to with a '/' like "$parent/child"
	bool foundSlash = false;
	bool saveChild = ( NULL != child );

	if (!m_in.Eof())
	{
		m_in.GetC();

		wxChar peek( m_in.Peek() );
		while (peek != wxChar(EOF) && !m_in.Eof() && peek != wxT('#') && peek != wxT('$')
			&& ( (peek >= wxT('a') && peek <= wxT('z') ) ||
			(peek >= wxT('A') && peek <= wxT('Z') ) ||
			(peek >= wxT('0') && peek <= wxT('9') ) ||
			peek == wxT('_') || peek == wxT('/') ) )
		{
			if ( foundSlash )
			{
				if ( saveChild )
				{
					(*child) << wxChar( m_in.GetC() );
				}
			}
			else
			{
				wxChar next = wxChar( m_in.GetC() );
				if ( wxT('/') == next )
				{
					foundSlash = true;
				}
				else
				{
					propname << next;
				}
			}
			peek = wxChar( m_in.Peek() );
		}
	}
	return propname;
}

bool TemplateParser::ParseProperty()
{
	wxString childName;
	wxString propname = ParsePropertyName( &childName );

	PProperty property = m_obj->GetProperty(propname);
	if ( NULL == property.get() )
	{
		wxLogError( wxT("The property '%s' does not exist for objects of class '%s'"), propname.c_str(), m_obj->GetClassName().c_str() );
		return true;
	}

	if ( childName.empty() )
	{
		wxString code = PropertyToCode(property);
		m_out << code;
	}
	else
	{
		m_out << property->GetChildFromParent( childName );
	}

	//  Debug::Print("parsing property %s",propname.c_str());

	return true;
}

bool TemplateParser::ParseText()
{
	wxString text;

	if ( !m_in.Eof() )
	{
		wxChar peek( m_in.Peek() );
		while (peek != wxChar(EOF) && !m_in.Eof() && peek != wxT('#') && peek != wxT('$') && !m_in.Eof() )
		{
			wxChar c( m_in.GetC() );
			if (c == wxT('@') )
				c = wxChar(m_in.GetC());

			text << c;
			peek = wxChar( m_in.Peek() );
		}

		// If text is all whitespace, ignore it
		if ( text.find_first_not_of( wxT("\r\n\t ") ) != text.npos )
		{
			m_out << text;
		}
	}

	//  Debug::Print("Parsed Text: %s",aux.str().c_str());
	return true;
}

bool TemplateParser::ParseInnerTemplate()
{
	return true;
}

PObjectBase TemplateParser::GetWxParent()
{
	PObjectBase wxparent;

	std::vector< PObjectBase > candidates;
	candidates.push_back( m_obj->FindNearAncestor( wxT("container") ) );
	candidates.push_back( m_obj->FindNearAncestor( wxT("notebook") ) );
	candidates.push_back( m_obj->FindNearAncestor( wxT("splitter") ) );
	candidates.push_back( m_obj->FindNearAncestor( wxT("flatnotebook") ) );
	candidates.push_back( m_obj->FindNearAncestor( wxT("listbook") ) );
	candidates.push_back( m_obj->FindNearAncestor( wxT("choicebook") ) );
	candidates.push_back( m_obj->FindNearAncestor( wxT("toolbar") ) );

	for ( size_t i = 0; i < candidates.size(); i++ )
	{
		if ( !wxparent )
		{
			wxparent = candidates[i];
		}
		else
		{
			if ( candidates[i] && candidates[i]->Deep() > wxparent->Deep() )
			{
				wxparent = candidates[i];
			}
		}
	}

	return wxparent;
}

bool TemplateParser::ParseWxParent()
{
	PObjectBase wxparent( GetWxParent() );

	if ( wxparent )
	{
		PProperty property = GetRelatedProperty( wxparent );
		m_out << PropertyToCode(property);
	}
	else
	{
		ignore_whitespaces();
		ParsePropertyName();
		m_out << RootWxParentToCode();
	}

	return true;
}

bool TemplateParser::ParseParent()
{
	PObjectBase parent(m_obj->GetParent());
	if (parent)
	{
		PProperty property = GetRelatedProperty( parent );
		m_out << PropertyToCode(property);
	}
	else
	{
		m_out << wxT("ERROR");
	}

	return true;
}

bool TemplateParser::ParseChild()
{
	// Get the first child
	PObjectBase child(m_obj->GetChild(0));

	if (child)
	{
		PProperty property = GetRelatedProperty( child );
		m_out << PropertyToCode(property);
	}
	else
		m_out << RootWxParentToCode();

	return true;
}

PProperty TemplateParser::GetRelatedProperty( PObjectBase relative )
{
	ignore_whitespaces();
	wxString propname = ParsePropertyName();
	return relative->GetProperty( propname );
}

bool TemplateParser::ParseForEach()
{
	// ignoramos los espacios que pudiera haber al principio
	ignore_whitespaces();

	// parseamos la propiedad
	if (GetNextToken() == TOK_PROPERTY)
	{
		wxString propname = ParsePropertyName();
		wxString inner_template = ExtractInnerTemplate();

		PProperty property = m_obj->GetProperty(propname);
		wxString propvalue = property->GetValue();

		// el valor de la propiedad debe ser una cadena de caracteres
		// separada por ','. Se va a generar la plantilla anidada tantas
		// veces como tokens se encuentren el el valor de la propiedad.

		if (property->GetType() == PT_INTLIST)
		{
			// Para ello se utiliza la clase wxStringTokenizer de wxWidgets
			wxStringTokenizer tkz( propvalue, wxT(","));
			while (tkz.HasMoreTokens())
			{
				wxString token;
				token = tkz.GetNextToken();
				token.Trim(true);
				token.Trim(false);

				// parseamos la plantilla interna
				{
					wxString code;
					PTemplateParser parser = CreateParser(m_obj,inner_template);
					parser->SetPredefined( token );
					code = parser->ParseTemplate();
					m_out << wxT("\n") << code;
				}
			}
		}
		else if (property->GetType() == PT_STRINGLIST)
		{
			wxArrayString array = property->GetValueAsArrayString();
			for ( unsigned int i = 0 ; i < array.Count(); i++ )
			{
				wxString code;
				PTemplateParser parser = CreateParser(m_obj,inner_template);
				parser->SetPredefined( ValueToCode( PT_WXSTRING_I18N, array[i] ) );
				code = parser->ParseTemplate();
				m_out << wxT("\n") << code;
			}
		}
		else
			wxLogError(wxT("Property type not compatible with \"foreach\" macro"));
	}

	return true;
}

PProperty TemplateParser::GetProperty( wxString* childName )
{
	PProperty property( (Property*)NULL );

	// Check for #wxparent, #parent, or #child
	if ( GetNextToken() == TOK_MACRO )
	{
		Ident ident = ParseIdent();
		switch (ident)
		{
			case ID_WXPARENT:
			{
				PObjectBase wxparent( GetWxParent() );
				if ( wxparent )
				{
					property = GetRelatedProperty( wxparent );
				}
				break;
			}
			case ID_PARENT:
			{
				PObjectBase parent( m_obj->GetParent() );
				if ( parent )
				{
					property = GetRelatedProperty( parent );
				}
				break;
			}
			case ID_CHILD:
			{
				PObjectBase child( m_obj->GetChild( 0 ) );
				if ( child )
				{
					property = GetRelatedProperty( child );
				}
				break;
			}
			default:
				break;
		}
	}

	if ( !property )
	{
		if ( GetNextToken() == TOK_PROPERTY )
		{
			wxString propname = ParsePropertyName( childName );
			property = m_obj->GetProperty( propname );
		}
	}

	return property;
}

void TemplateParser::ignore_whitespaces()
{
	wxChar peek( m_in.Peek() );
	while ( peek != wxChar(EOF) && !m_in.Eof() && peek == wxT(' ') )
	{
		m_in.GetC();
		peek = wxChar( m_in.Peek() );
	}
}


bool TemplateParser::ParseIfNotNull()
{
	ignore_whitespaces();

	// Get the property
	wxString childName;
	PProperty property( GetProperty( &childName ) );
	if ( !property )
	{
		return false;
	}

	wxString inner_template = ExtractInnerTemplate();

	if ( !property->IsNull() )
	{
		if ( !childName.empty() )
		{
			if ( property->GetChildFromParent( childName ).empty() )
			{
				return true;
			}
		}

		// Generate the code from the block
		PTemplateParser parser = CreateParser( m_obj, inner_template );
		m_out << parser->ParseTemplate();
	}

	return true;
}

bool TemplateParser::ParseIfNull()
{
	ignore_whitespaces();

	// Get the property
	wxString childName;
	PProperty property( GetProperty( &childName ) );
	if ( !property )
	{
		return false;
	}

	wxString inner_template = ExtractInnerTemplate();

	if ( property->IsNull() )
	{
		// Generate the code from the block
		PTemplateParser parser = CreateParser( m_obj, inner_template );
		m_out << parser->ParseTemplate();
	}
	else
	{
		if ( !childName.empty() )
		{
			if ( property->GetChildFromParent( childName ).empty() )
			{
				// Generate the code from the block
				PTemplateParser parser = CreateParser( m_obj, inner_template );
				m_out << parser->ParseTemplate();
			}
		}
	}

	return true;
}

wxString TemplateParser::ExtractLiteral()
{
	wxString os;

	wxChar c;

	// ignoramos los espacios que pudiera haber al principio
	ignore_whitespaces();

	c = wxChar(m_in.GetC()); // comillas de inicio

	if ( c == wxT('"') )
	{
		bool end = false;
		// comenzamos la extracción de la plantilla
		while (!end && !m_in.Eof() && m_in.Peek() != EOF )
		{
			c = wxChar(m_in.GetC()); // extraemos un caracter

			// comprobamos si estamos ante un posible cierre de comillas
			if ( c == wxT('"') )
			{
				if ( m_in.Peek() == wxT('"') ) // caracter (") denotado por ("")
				{
					m_in.GetC(); // ignoramos la segunda comilla
					os << wxT('"');
				}
				else // cierre
				{
					end = true;

					// ignoramos todo los caracteres siguientes hasta un espacio
					// así errores como "hola"mundo" -> "hola"
					wxChar peek( m_in.Peek() );
					while (peek != wxChar(EOF) && !m_in.Eof() && peek != wxT(' ') )
					{
						m_in.GetC();
						peek = wxChar( m_in.Peek() );
					}
				}
			}
			else // un caracter del literal
				os << c;
		}
	}

	return os;
}

bool TemplateParser::ParseIfEqual()
{
	// ignore leading whitespace
	ignore_whitespaces();

	// Get the property
	wxString childName;
	PProperty property( GetProperty( &childName ) );
	if ( property )
	{
		// Get the value to compare to
		wxString value = ExtractLiteral();

		// Get the template to generate if comparison is true
		wxString inner_template = ExtractInnerTemplate();

		// Get the value of the property
		wxString propValue;
		if ( childName.empty() )
		{
			propValue = property->GetValue();
		}
		else
		{
			propValue = property->GetChildFromParent( childName );
		}

		// Compare
		if ( propValue == value )
		{
			// Generate the code
			PTemplateParser parser = CreateParser(m_obj,inner_template);
			m_out << parser->ParseTemplate();
			return true;
		}
	}
	return false;
}

bool TemplateParser::ParseIfNotEqual()
{
	// ignore leading whitespace
	ignore_whitespaces();

	// Get the property
	wxString childName;
	PProperty property( GetProperty( &childName ) );
	if ( property )
	{
		// Get the value to compare to
		wxString value = ExtractLiteral();

		// Get the template to generate if comparison is false
		wxString inner_template = ExtractInnerTemplate();

		// Get the value of the property
		wxString propValue;
		if ( childName.empty() )
		{
			propValue = property->GetValue();
		}
		else
		{
			propValue = property->GetChildFromParent( childName );
		}

		// Compare
		if ( propValue != value )
		{
			// Generate the code
			PTemplateParser parser = CreateParser( m_obj, inner_template );
			m_out << parser->ParseTemplate();;
			return true;
		}
	}

	return false;
}

TemplateParser::Ident TemplateParser::SearchIdent(wxString ident)
{
	//  Debug::Print("Parsing command %s",ident.c_str());

	if (ident == wxT("wxparent") )
		return ID_WXPARENT;
	else if (ident == wxT("ifnotnull") )
		return ID_IFNOTNULL;
	else if (ident == wxT("ifnull") )
		return ID_IFNULL;
	else if (ident == wxT("foreach") )
		return ID_FOREACH;
	else if (ident == wxT("pred") )
		return ID_PREDEFINED;
	else if (ident == wxT("child") )
		return ID_CHILD;
	else if (ident == wxT("parent") )
		return ID_PARENT;
	else if (ident == wxT("nl") )
		return ID_NEWLINE;
	else if (ident == wxT("ifequal") )
		return ID_IFEQUAL;
	else if (ident == wxT("ifnotequal") )
		return ID_IFNOTEQUAL;
	else if (ident == wxT("append") )
		return ID_APPEND;
	else if (ident == wxT("class") )
		return ID_CLASS;
	else
		return ID_ERROR;
}

wxString TemplateParser::ParseTemplate()
{
	while (!m_in.Eof())
	{
		Token token = GetNextToken();
		switch (token)
		{
		case TOK_MACRO:
			ParseMacro();
			break;
		case TOK_PROPERTY:
			ParseProperty();
			break;
		case TOK_TEXT:
			ParseText();
			break;
		default:
			return wxT("");
		}
	}

	return m_out;
}

/**
* Extrae la plantilla encerrada entre '@{' y '@}'.
* Nota: Los espacios al comienzo serán ignorados.
*/
wxString TemplateParser::ExtractInnerTemplate()
{
	//  bool error = false;
	wxString os;

	wxChar c1, c2;

	// ignoramos los espacios que pudiera haber al principio
	ignore_whitespaces();

	// los dos caracteres siguientes deberán ser '@{'
	c1 = wxChar(m_in.GetC());
	c2 = wxChar(m_in.GetC());

	if (c1 == wxT('@') && c2 == wxT('{') )
	{
		ignore_whitespaces();

		int level = 1;
		bool end = false;
		// comenzamos la extracción de la plantilla
		while ( !end && !m_in.Eof() && m_in.Peek() != EOF )
		{
			c1 = wxChar(m_in.GetC());

			// comprobamos si estamos ante un posible cierre o apertura de llaves.
			if (c1 == wxT('@') )
			{
				c2 = wxChar(m_in.GetC());

				if (c2 == wxT('}') )
				{
					level--;
					if (level == 0)
						end = true;
					else
					{
						// no es el cierre final, por tanto metemos los caracteres
						// y seguimos
						os << c1;
						os << c2;
					}
				}
				else
				{
					os << c1;
					os << c2;

					if (c2 == wxT('{') )
						level++;
				}
			}
			else
				os << c1;
		}
	}

	return os;
}


bool TemplateParser::ParsePred()
{
	if (m_pred != wxT("") )
		m_out << m_pred;

	return true;
}

bool TemplateParser::ParseNewLine()
{
	m_out << wxT('\n');
	return true;
}

void TemplateParser::ParseAppend()
{
	ignore_whitespaces();
}

void TemplateParser::ParseClass()
{
	PProperty subclass_prop = m_obj->GetProperty( wxT("subclass") );
	if ( subclass_prop )
	{
		wxString subclass = subclass_prop->GetChildFromParent( wxT("name") );
		if ( !subclass.empty() )
		{
			m_out << subclass;
			return;
		}
	}

	m_out << m_obj->GetClassName();
}

wxString TemplateParser::PropertyToCode(PProperty property)
{
	return ValueToCode(property->GetType(), property->GetValue());
}

//////////////////////////////////////////////////////////////////////////////
CodeWriter::CodeWriter()
{
	m_indent = 0;
	m_cols = 0;
}
