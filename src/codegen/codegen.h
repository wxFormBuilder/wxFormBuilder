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
*   Used to repeat code for each subproperty of $property, where &propery is a comma delimited list.
*   The code is contained between '@{' and '@}'. The code will be generated as many times as there are subexpressions
*   in the value of the property. Within the brace, access to the subexpression is obtained with the #pred directive.
*   Example:
*    #foreach $growable_cols
*    @{
*		$name->AddGrowableCol(#pred);
*    @}
*
* - #pred (see #foreach)
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
	void ignore_whitespaces();

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
		ID_CHILD,
		ID_IFNOTNULL,
		ID_IFNULL,
		ID_FOREACH,
		ID_PREDEFINED,  // simbolo predefinido '#pred'
		ID_NEWLINE,
		ID_IFEQUAL,
		ID_IFNOTEQUAL,
		ID_APPEND,
		ID_CLASS
	} Ident;

	Ident SearchIdent(wxString ident);
	Ident ParseIdent();

	wxString ParsePropertyName( wxString* child = NULL );
	/**
	* Esta rutina extrae el codigo de una plantilla encerrada entre
	* las macros #begin y #end, teniendo en cuenta que puede estar anidados
	*/
	wxString ExtractInnerTemplate();

	/**
	* Un valor literal es una cadena encerrada entre '"' (ej. "xxx"),
	* el caracter " se representa con "".
	*/
	wxString ExtractLiteral();

	/**
	* Consulta el siguiente símbolo de la entrada y devuelve el token.
	* @return TOK_MACRO si a continuación viene un comando.
	*         TOK_PROPERTY si a continuación viene una propiedad.
	*         TOK_TEXT si a continuación viene texto normal.
	*/
	Token GetNextToken();


	bool ParseInnerTemplate();
	bool ParseWxParent();
	bool ParseParent();
	bool ParseChild();
	bool ParseForEach();
	bool ParseIfNotNull();
	bool ParseIfNull();
	bool ParseNewLine();
	bool ParseIfEqual();
	bool ParseIfNotEqual();
	void ParseAppend();
	void ParseClass();

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

public:
	TemplateParser( PObjectBase obj, wxString _template);
	TemplateParser( const TemplateParser & that, wxString _template );
	/**
	* Devuelve el código del valor de una propiedad en el formato del lenguaje.
	* @note use ValueToCode
	*/
	wxString PropertyToCode( PProperty property );

	/**
	* Este método crea un nuevo parser del mismo tipo que el objeto que llama
	* a dicho método.
	*/
	virtual PTemplateParser CreateParser( const TemplateParser* oldparser, wxString _template ) = 0;

	virtual ~TemplateParser() {};

	/** Devuelve el código del nombre del atributo "wxWindow *parent" raíz.
	* En C++ será el puntero "this" pero en otros lenguajes no tiene porqué
	* llamarse así.
	*/
	virtual wxString RootWxParentToCode() = 0;

	/**
	* A partir del valor de una propiedad genera el código.
	*/
	virtual wxString ValueToCode(PropertyType type, wxString value) = 0;

	/**
	* La función "estrella" de la clase. Analiza una plantilla devolviendo el
	* código.
	*/
	wxString ParseTemplate();

	/**
	* establece el texto predefinido, el cual será devuelto con la directiva
	* #pred.
	*/
	void SetPredefined(wxString pred) { m_pred = pred; };
};

/**
* CodeWriter.
*
* This class abstracts the code generation from the target.
* Because, in some cases the target is a file, sometimes a TextCtrl, and sometimes both
*/
class CodeWriter
{
private:
	int m_indent;
	int m_cols;

protected:
	/**
	* Write a wxString
	*/
	virtual void DoWrite( wxString code ) {};

	/**
	* Returns the size of the indentation - was useful when using spaces, now it is 1 because using tabs
	*/
	virtual int GetIndentSize() { return 1; }

	/**
	* Return the column count for the file
	* @note Used to fit the code into a certain number of columns.
	*/
	virtual int GetColumns() { return 80; }

	/**
	* Verifies that the wxString does not contain carraige return characters
	*/
	bool StringOk( wxString s );

	/**
	* Divide una cadena de texto mal formada (con retornos de carro), en
	* columnas simples insertándolas una a una respetando el indentado.
	*/
	void FixWrite( wxString s );

public:
	/**
	* Constructor.
	*/
	CodeWriter();

	/**
	* Destructor.
	*/
	virtual ~CodeWriter() {};

	/**
	* Increment the indent
	*/
	void Indent()
	{
		m_indent += GetIndentSize();
	}

	/**
	* Decrement the indent
	*/
	void Unindent()
	{
		m_indent -= GetIndentSize();
		if ( m_indent < 0 )
		{
			m_indent = 0;
		}
	}

	/**
	* Write a line of code
	*/
	void WriteLn(wxString code);

	/**
	* Escribe una cadena de texto en el código.
	*/
	void Write( wxString code );

	/**
	* borra todo el código previamente escrito.
	*/
	virtual void Clear() = 0;
};

/**
* Code Generator
*
* This class defines an interface to execute the code generation.
* The algorithms to generate the code are similar from one language to another.
* Examples:
*
* - En C++ tenemos que generar las declaraciones de una clase y luego su
*   implementación, a parte de los #defines e #includes pertinentes.
* - En java, aunque tiene una sintaxis "parecida", implementación y declaración
*   van ligadas en el mismo fichero, además requiere de un fichero por clase.
* - En el formato XRC (XML), no tiene nada que ver con ninguno de los
*   anteriores, pareciendose mucho al modelo de datos de la propia aplicación.
*
* Dado que no existe un algoritmo "universal" para generar código no queda más
* remedio que realizar una implementación diferente por cada lenguaje. A pesar
* de esto, es posible reutilizar todo el sistema de plantillas de código
* simplificando bastante la tarea de implementar el generador de un nuevo
* lenguaje.
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
