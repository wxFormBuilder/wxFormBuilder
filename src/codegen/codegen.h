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
///////////////////////////////////////////////////////////////////////////////

/**
@file
@author José Antonio Hurtado - joseantonio.hurtado@gmail.com
@author Juan Antonio Ortega  - jortegalalmolda@gmail.com
*/

#ifndef __CODEGEN__
#define __CODEGEN__

#include <wx/sstream.h>
#include <map>
#include "utils/wxfbdefs.h"
#include "model/types.h"

/**
* Template notes
*
* The preprocessor macros begin with the character '#'.
*   Example: #wxparent $name
*
* The object properties begin with the character '$'.
*   Example: $name
*
* The creation of a local variable begins with the character '%'
*   Example: int %var = $style;
*
* The '@' character is used to escape the special characters to treat them as text
*   Example: @# @$ @@ @%
*
*/

/**
* Preprocessor directives:
*
* - #wxparent $property
*   Used to get the property of the parent of the current object.
*	 This directive is necessary to generate the parameter "wxWindow* parent" in the constructors of each widget.
*   Example:
*     $name = new wxButton( #wxparent $name, ....
*
* - #wxchild $property
*   Used to get a property of the first child of the current object.
*   Used by "sizeritem" in order to add a sizer object.
*
* - #ifnotnull $property @{ .... @}
*   Used to generate code as long as the property is not null
*
* - #foreach $property @{ ..... @}
*   Used to repeat code for each subproperty of $property, where $property is a comma delimited list.
*   The code is contained between '@{' and '@}'. The code will be generated as many times as there are subexpressions
*   in the value of the property. Within the brace, access to the subexpression is obtained with the #pred directive, and
*	access to index of the subexpression is obtained with #npred.
*   Example:
*    #foreach $growable_cols
*    @{
*		$name->AddGrowableCol(#pred);
*    @}
*
* - #pred (see #foreach)
* - #npred (see #foreach)
*
*/

/**
* Template Parser
*/
class TemplateParser
{
private:
	PObjectBase m_obj;
	wxStringInputStream m_in;
	wxString m_out;
	wxString m_pred;
	wxString m_npred;
	void ignore_whitespaces();

	// Current indentation level in the file
	int m_indent;

protected:
	typedef enum {
		TOK_ERROR,
		TOK_MACRO,
		TOK_TEXT,
		TOK_PROPERTY
	} Token;

	typedef enum {
		ID_ERROR,
		ID_WXPARENT,
		ID_PARENT,
		ID_FORM,
		ID_CHILD,
		ID_IFNOTNULL,
		ID_IFNULL,
		ID_FOREACH,
		ID_PREDEFINED,  // predefined symbol '#pred'
		ID_PREDEFINED_INDEX, // #npred
		ID_NEWLINE,
		ID_IFEQUAL,
		ID_IFNOTEQUAL,
		ID_IFPARENTTYPEEQUAL,
		ID_IFPARENTTYPENOTEQUAL,
		ID_IFPARENTCLASSEQUAL,
		ID_IFPARENTCLASSNOTEQUAL,
		ID_APPEND,
		ID_CLASS,
		ID_INDENT,
		ID_UNINDENT,
		ID_IFTYPEEQUAL,
		ID_IFTYPENOTEQUAL,
		ID_UTBL
	} Ident;

	bool IsEqual(const wxString& value, const wxString& set);

	Ident SearchIdent(wxString ident);
	Ident ParseIdent();

	wxString ParsePropertyName( wxString* child = NULL );
	/**
	* This routine extracts the source code from a template enclosed between
	* the #begin and #end macros, having in mind that they can be nested
	*/
	wxString ExtractInnerTemplate();

	/**
	* A literal value is an string enclosed between '"' (e.g. "xxx"),
	* The " character is represented with "".
	*/
	wxString ExtractLiteral();

	/**
	* Look up for the following symbol from input and returns the token.
	* @return TOK_MACRO when it's followed by a command.
	*         TOK_PROPERTY when it's followed by a property.
	*         TOK_TEXT when it's followed by normal text.
	*/
	Token GetNextToken();


	bool ParseInnerTemplate();
	bool ParseWxParent();
	bool ParseParent();
	bool ParseForm();
	bool ParseChild();
	bool ParseForEach();
	bool ParseIfNotNull();
	bool ParseIfNull();
	bool ParseNewLine();
	bool ParseIfEqual();
	bool ParseIfNotEqual();
	bool ParseIfParentTypeEqual();
	bool ParseIfParentTypeNotEqual();
	bool ParseIfParentClassEqual();
	bool ParseIfParentClassNotEqual();
	void ParseAppend();
	void ParseClass();
	void ParseIndent();
	void ParseUnindent();
	bool ParseIfTypeEqual();
	bool ParseIfTypeNotEqual();
	void ParseLuaTable();

	PProperty GetProperty( wxString* childName = NULL );
	PObjectBase GetWxParent();
	PProperty GetRelatedProperty( PObjectBase relative );

	/**
	* Parse a macro.
	*/
	bool ParseMacro();

	/**
	* Parse a property.
	*/
	bool ParseProperty();


	/**
	* Parse text.
	*/
	bool ParseText();

	bool ParsePred();
	bool ParseNPred();

public:
	TemplateParser( PObjectBase obj, wxString _template);
	TemplateParser( const TemplateParser & that, wxString _template );
	/**
	* Returns the code for a property value in the language format.
	* @note use ValueToCode
	*/
	wxString PropertyToCode( PProperty property );

	/**
	* This method creates a new parser with the same type that the object
	* calling such method.
	*/
	virtual PTemplateParser CreateParser( const TemplateParser* oldparser, wxString _template ) = 0;

	virtual ~TemplateParser() {};

	/**
	* Returns the code for a "wxWindow *parent" root attribute' name.
	* In C++ it will be the "this" pointer, but in other languages it
	* could be named differently.
	*/
	virtual wxString RootWxParentToCode() = 0;

	/**
	* Generates the code from a property value.
	*/
	virtual wxString ValueToCode(PropertyType type, wxString value) = 0;

	/**
	* The "star" function for this class. Analyzes a template, returning the code.
	*/
	wxString ParseTemplate();

	/**
	* Set the string for the #pred and #npred macros
	*/
	void SetPredefined( wxString pred, wxString npred ) { m_pred = pred; m_npred = npred; };
};

/**
* Code Generator
*
* This class defines an interface to execute the code generation.
* The algorithms to generate the code are similar from one language to another.
* Examples:
*
* - In C++, we generate the class declarations and then its implementation,
*   besides the related #define and #include.
* - In java, it has a similar syntax, but declaration and implementation are
*   together in the same file, and it's required one file per class.
* - In XRC format (XML), it's a different way, it's more likely to the
*   data model from the application itself.
*
* Given that doesn't exist an "universal" algorithm for generating code, there
* is no choice but to make a different implementation for each language. It's
* possible to reuse the whole code templates system, although, simplifying a lot
* the implementation task for a new language.
*/
class CodeGenerator
{
protected:

public:
	/**
	* Virtual destructor.
	*/
	virtual ~CodeGenerator() {};
	/**
	* Generate the code of the project
	*/
	virtual bool GenerateCode( PObjectBase project ) = 0;
};


#endif //__CODEGEN__
