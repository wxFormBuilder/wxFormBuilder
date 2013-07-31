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
#include "utils/wxfbexception.h"

TemplateParser::TemplateParser(PObjectBase obj, wxString _template)
:
m_obj( obj ),
m_in( _template ),
m_indent( 0 )
{
}

TemplateParser::TemplateParser( const TemplateParser & that, wxString _template )
:
m_obj( that.m_obj ),
m_in( _template ),
m_indent( 0 )
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
	case ID_FORM:
		return ParseForm();
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
	case ID_PREDEFINED_INDEX:
		return ParseNPred();
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
	case ID_IFPARENTTYPEEQUAL:
		ParseIfParentTypeEqual();
		break;
	case ID_IFPARENTCLASSEQUAL:
		ParseIfParentClassEqual();
		break;
	case ID_APPEND:
		ParseAppend();
		break;
	case ID_CLASS:
		ParseClass();
		break;
	case ID_INDENT:
		ParseIndent();
		break;
	case ID_UNINDENT:
		ParseUnindent();
		break;
	case ID_IFTYPEEQUAL:
		ParseIfTypeEqual();
		break;
	case ID_IFTYPENOTEQUAL:
		ParseIfTypeNotEqual();
		break;
	case ID_UTBL:
		ParseLuaTable();
		break;
	default:
		THROW_WXFBEX( wxT("Invalid Macro Type") );
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
			(peek >= wxT('0') && peek <= wxT('9') ) ))
		{
			macro += wxChar( m_in.GetC() );
			peek = wxChar( m_in.Peek() );
		}

		// Searching the identifier
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

	//  LogDebug("parsing property %s",propname.c_str());

	return true;
}

bool TemplateParser::ParseText()
{
	wxString text;
	int sspace = 0;

	if ( !m_in.Eof() )
	{
		wxChar peek( m_in.Peek() );

		while (peek != wxChar(EOF) && !m_in.Eof() && peek != wxT('#') && peek != wxT('$') )
		{
			wxChar c( m_in.GetC() );
			if (c == wxT('@') )
			{
				c = wxChar(m_in.GetC());
				if(c == wxT(' '))sspace++;
			}

			text << c;
			peek = wxChar( m_in.Peek() );
		}

		if ( text.find_first_not_of( wxT("\r\n\t ") ) != text.npos )
		{
		    // If text is all whitespace, ignore it
			m_out << text;
		}
		else
		{
		    // ... but allow all '@ ' instances
		    wxString spaces(wxT(' '), sspace);
		    m_out << spaces;
		}
	}

	//LogDebug(wxT("Parsed Text: %s"),text.c_str());
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
	candidates.push_back( m_obj->FindNearAncestor( wxT("auinotebook") ) );
	candidates.push_back( m_obj->FindNearAncestor( wxT("toolbar") ) );
	candidates.push_back( m_obj->FindNearAncestor( wxT("wizardpagesimple") ) );

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
		//m_out << PropertyToCode(property);
		m_out << ValueToCode( PT_WXPARENT, property->GetValue() );
	}
	else
	{
		ignore_whitespaces();
		ParsePropertyName();
		m_out << RootWxParentToCode();
	}

	return true;
}

bool TemplateParser::ParseForm()
{
	PObjectBase form (m_obj);
	PObjectBase parent(form->GetParent());

	if ( !parent )
	{
		return false;
	}

	// form is a form when grandparent is null
	PObjectBase	grandparent = parent->GetParent();
	while ( grandparent )
	{
		form = parent;
		parent = grandparent;
		grandparent = grandparent->GetParent();
	}

	PProperty property = GetRelatedProperty( form );
	m_out << PropertyToCode( property );

	return true;
}

void TemplateParser::ParseLuaTable()
{
	PObjectBase project = PObjectBase(new ObjectBase(*AppData()->GetProjectData()));
	PProperty propNs= project->GetProperty( wxT( "ui_table" ) );
	if ( propNs )
	{
		wxString strTableName = propNs->GetValueAsString();
		if(strTableName.length() <= 0)
			strTableName = wxT("UI");
		m_out <<strTableName + wxT(".");
	}
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
	// Whitespaces at the very start are ignored
	ignore_whitespaces();

	// parsing the property
	if (GetNextToken() == TOK_PROPERTY)
	{
		wxString propname = ParsePropertyName();
		wxString inner_template = ExtractInnerTemplate();

		PProperty property = m_obj->GetProperty(propname);
		wxString propvalue = property->GetValue();

		// Property value must be an string using ',' as separator.
		// The template will be generated nesting as many times as
		// tokens were found in the property value.

		if (property->GetType() == PT_INTLIST || property->GetType() == PT_UINTLIST)
		{
			// For doing that we will use wxStringTokenizer class from wxWidgets
			wxStringTokenizer tkz( propvalue, wxT(","));
			int i = 0;
			while (tkz.HasMoreTokens())
			{
				wxString token;
				token = tkz.GetNextToken();
				token.Trim(true);
				token.Trim(false);

				// Parsing the internal template
				{
					wxString code;
					PTemplateParser parser = CreateParser( this, inner_template );
					parser->SetPredefined( token, wxString::Format( wxT("%i"), i++ ) );
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
				PTemplateParser parser = CreateParser(this,inner_template);
				parser->SetPredefined( ValueToCode( PT_WXSTRING_I18N, array[i] ), wxString::Format( wxT("%i"), i ) );
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
	    try
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
	    catch( wxFBException& ex )
	    {
	        wxLogError( ex.what() );
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
		PTemplateParser parser = CreateParser( this, inner_template );
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
		PTemplateParser parser = CreateParser( this, inner_template );
		m_out << parser->ParseTemplate();
	}
	else
	{
		if ( !childName.empty() )
		{
			if ( property->GetChildFromParent( childName ).empty() )
			{
				// Generate the code from the block
				PTemplateParser parser = CreateParser( this, inner_template );
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

	// Whitespaces at the very start are ignored
	ignore_whitespaces();

	c = wxChar(m_in.GetC()); // Initial quotation mark

	if ( c == wxT('"') )
	{
		bool end = false;
		// Beginning the template extraction
		while (!end && !m_in.Eof() && m_in.Peek() != EOF )
		{
			c = wxChar(m_in.GetC()); // obtaining one char

			// Checking for a possible closing quotation mark
			if ( c == wxT('"') )
			{
				if ( m_in.Peek() == wxT('"') ) // Char (") denoted as ("")
				{
					m_in.GetC(); // Second quotation mark is ignored
					os << wxT('"');
				}
				else // Closing
				{
					end = true;

					// All the following chars are ignored up to an space char,
					// so we can avoid errors like "hello"world" -> "hello"
					wxChar peek( m_in.Peek() );
					while (peek != wxChar(EOF) && !m_in.Eof() && peek != wxT(' ') )
					{
						m_in.GetC();
						peek = wxChar( m_in.Peek() );
					}
				}
			}
			else // one char from literal (N.B. ??)
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
		//if ( propValue == value )
		if ( IsEqual( propValue, value ) )
		{
			// Generate the code
			PTemplateParser parser = CreateParser(this,inner_template);
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
			PTemplateParser parser = CreateParser( this, inner_template );
			m_out << parser->ParseTemplate();;
			return true;
		}
	}

	return false;
}

bool TemplateParser::ParseIfParentTypeEqual()
{
    PObjectBase parent( m_obj->GetParent() );

    // get examined type name
    wxString type = ExtractLiteral();

    // get the template to generate if comparison is true
    wxString inner_template = ExtractInnerTemplate();

    // compare give type name with type of the wx parent object
    if( parent && IsEqual( parent->GetObjectTypeName(), type) )
    {
		// generate the code
		PTemplateParser parser = CreateParser( this, inner_template );
		m_out << parser->ParseTemplate();
		return true;
    }

    return false;
}

bool TemplateParser::ParseIfParentClassEqual()
{
    PObjectBase parent( m_obj->GetParent() );

    // get examined type name
    wxString type = ExtractLiteral();

    // get the template to generate if comparison is true
    wxString inner_template = ExtractInnerTemplate();

    // compare give type name with type of the wx parent object
    if( parent && IsEqual( parent->GetClassName(), type) )
    {
		// generate the code
		PTemplateParser parser = CreateParser( this, inner_template );
		m_out << parser->ParseTemplate();
		return true;
    }

    return false;
}

bool TemplateParser::ParseIfTypeEqual()
{
    // get examined type name
    wxString type = ExtractLiteral();

    // get the template to generate if comparison is true
    wxString inner_template = ExtractInnerTemplate();

    // compare give type name with type of the wx parent object
    if( IsEqual( m_obj->GetObjectTypeName(), type) )
    {
        // generate the code
		PTemplateParser parser = CreateParser( this, inner_template );
		m_out << parser->ParseTemplate();
		return true;
	}

    return false;
}

bool TemplateParser::ParseIfTypeNotEqual()
{
    // get examined type name
    wxString type = ExtractLiteral();

    // get the template to generate if comparison is true
    wxString inner_template = ExtractInnerTemplate();

    // compare give type name with type of the wx parent object
    if( !IsEqual( m_obj->GetObjectTypeName(), type) )
    {
        // generate the code
		PTemplateParser parser = CreateParser( this, inner_template );
		m_out << parser->ParseTemplate();
		return true;
	}

    return false;
}

TemplateParser::Ident TemplateParser::SearchIdent(wxString ident)
{
	//  LogDebug("Parsing command %s",ident.c_str());

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
	else if (ident == wxT("npred") )
		return ID_PREDEFINED_INDEX;
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
	else if (ident == wxT("ifparenttypeequal") )
		return ID_IFPARENTTYPEEQUAL;
	else if (ident == wxT("ifparentclassequal") )
		return ID_IFPARENTCLASSEQUAL;
	else if (ident == wxT("append") )
		return ID_APPEND;
	else if (ident == wxT("class") )
		return ID_CLASS;
	else if (ident == wxT("form") || ident == wxT("wizard"))
		return ID_FORM;
	else if (ident == wxT("indent") )
		return ID_INDENT;
	else if (ident == wxT("unindent") )
		return ID_UNINDENT;
	else if (ident == wxT("iftypeequal") )
		return ID_IFTYPEEQUAL;
	else if (ident == wxT("iftypenotequal") )
		return ID_IFTYPENOTEQUAL;
	else if(ident == wxT("utbl"))
		return ID_UTBL;
	else
		THROW_WXFBEX( wxString::Format( wxT("Unknown macro: \"%s\""), ident.c_str() ) );
}

wxString TemplateParser::ParseTemplate()
{
    try
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
    }
    catch ( wxFBException& ex )
    {
        wxLogError( ex.what() );
    }

	return m_out;
}

/**
* Obtaining the template enclosed between '@{' y '@}'.
* Note: whitespaces at the very start will be ignored.
*/
wxString TemplateParser::ExtractInnerTemplate()
{
	//  bool error = false;
	wxString os;

	wxChar c1, c2;

	// Initial whitespaces are ignored
	ignore_whitespaces();

	// The two following characters must be '@{'
	c1 = wxChar(m_in.GetC());
	c2 = wxChar(m_in.GetC());

	if (c1 == wxT('@') && c2 == wxT('{') )
	{
		ignore_whitespaces();

		int level = 1;
		bool end = false;
		// Beginning with the template extraction
		while ( !end && !m_in.Eof() && m_in.Peek() != EOF )
		{
			c1 = wxChar(m_in.GetC());

			// Checking if there are initial or closing braces
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
						// There isn't a final closing brace, so that we put in
						// the chars and continue
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

bool TemplateParser::ParseNPred()
{
	if (m_npred != wxT("") )
		m_out << m_npred;

	return true;
}

bool TemplateParser::ParseNewLine()
{
	m_out << wxT('\n');
	// append custom indentation define in code templates (will be replace by '\t' in code writer)
	for( int i = 0; i < m_indent; i++ ) m_out << wxT("%TAB%");

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

	m_out << ValueToCode( PT_CLASS, m_obj->GetClassName() );
}

void TemplateParser::ParseIndent()
{
	m_indent++;
}

void TemplateParser::ParseUnindent()
{
	m_indent--;

	if( m_indent < 0 ) m_indent = 0;
}

wxString TemplateParser::PropertyToCode(PProperty property)
{
	if ( property )
	{
		return ValueToCode(property->GetType(), property->GetValue());
	}
	else
	{
		return wxEmptyString;
	}
}

bool TemplateParser::IsEqual(const wxString& value, const wxString& set)
{
	bool contains = false;

	wxStringTokenizer tokens( set, wxT("||") );
	while ( tokens.HasMoreTokens() )
	{
		wxString token = tokens.GetNextToken();
		token.Trim().Trim(false);

		if( token == value )
		{
			contains = true;
			break;
		}
	}

	return contains;
}
