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
@note
The implementation of the generation of relative paths is a little hacky, and not a solution.
The value of all properties that are file or a directory paths must be absolute, otherwise the code generation will not work.
*/

#ifndef _CPP_CODE_GEN_
#define _CPP_CODE_GEN_

#include <set>
#include "codegen.h"
#include <wx/string.h>

/**
* Parse the C++ templates.
*/
class CppTemplateParser : public TemplateParser
{
private:
	bool m_i18n;
	bool m_useRelativePath;
	wxString m_basePath;

public:
	CppTemplateParser( PObjectBase obj, wxString _template, bool useI18N, bool useRelativePath, wxString basePath );
	CppTemplateParser( const CppTemplateParser & that, wxString _template );

	// overrides for C++
	PTemplateParser CreateParser( const TemplateParser* oldparser, wxString _template );
	wxString RootWxParentToCode();
	wxString ValueToCode( PropertyType type, wxString value);

};

/**
* Generate the C++ code
*/
class CppCodeGenerator : public CodeGenerator
{
private:
	typedef enum
	{
		P_PRIVATE,
		P_PROTECTED,
		P_PUBLIC
	} Permission;

	PCodeWriter m_header;
	PCodeWriter m_source;

	bool m_useRelativePath;
	bool m_i18n;
	wxString m_basePath;
	unsigned int m_firstID;
	bool m_useConnect;

	/**
	* Las macros predefinidas no generarán defines.
	*/
	std::set<wxString> m_predMacros;

	void SetupPredefinedMacros();

	/**
	* Dado un objeto y el nombre de una plantilla, obtiene el código.
	*/
	wxString GetCode( PObjectBase obj, wxString name);

	/**
	* Guarda el conjunto de clases de objetos del proyecto para generar
	* los includes.
	*/
	void FindDependencies( PObjectBase obj, std::set< PObjectInfo >& info_set );

	/**
	* Guarda el conjunto de "includes" que hay que generar para las propiedades
	* PT_XPM_BITMAP.
	*/
	void FindXpmProperties( PObjectBase obj, std::set< wxString >& xpmset);

	/**
	* Guarda todos las propiedades de objetos de tipo "macro" para generar
	* su posterior '#define'.
	*/
	void FindMacros( PObjectBase obj, std::vector< wxString >* macros );

	/**
	 * Looks for "non-null" event handlers (PEvent) and collects it into a vector.
	 */
	void FindEventHandlers(PObjectBase obj, EventVector &events);

	/**
	* Genera la declaración de clases en el fichero de cabecera.
	*/
	void GenClassDeclaration( PObjectBase class_obj, bool use_enum, const wxString& classDecoration, const EventVector &events );

	/**
	* Generates the event table.
	*/
	void GenEvents( PObjectBase class_obj, const EventVector &events );

	/**
	* helper function to find the event table entry template in the class or its base classes
	*/
	bool GenEventEntry( PObjectBase obj, PObjectInfo obj_info, const wxString& templateName, const wxString& handlerName );

	/**
	* Función recursiva para la declaración de atributos, usada dentro
	* de GenClassDeclaration.
	*/
	void GenAttributeDeclaration( PObjectBase obj, Permission perm);

	/**
	Generates the generated_event_handlers template
	*/
	void GetGenEventHandlers( PObjectBase obj );

	/**
	* Genera la sección de '#include' fichero.
	*/
	void GenIncludes( PObjectBase project, std::vector< wxString >* includes, std::set< wxString >* templates );
	void GenObjectIncludes( PObjectBase project, std::vector< wxString >* includes, std::set< wxString >* templates );
	void GenBaseIncludes( PObjectInfo info, PObjectBase obj, std::vector< wxString >* includes, std::set< wxString >* templates );
	void AddUniqueIncludes( const wxString& include, std::vector< wxString >* includes );

	/**
	* Generate a set of all subclasses to forward declare in the generated header file.
	* Also generate sets of header files to be include in either the source or header file.
	*/
	void GenSubclassSets( PObjectBase obj, std::set< wxString >* subclasses, std::set< wxString >* sourceIncludes, std::vector< wxString >* headerIncludes );

	/**
	* Genera la sección de '#include' para las propiedades XPM.
	*/
	void GenXpmIncludes( PObjectBase project);

	/**
	* Genera la sección de '#define' macro.
	*/
	void GenDefines( PObjectBase project);

	/**
	* Generate a enum with wxWindow identifiers.
	*/
	void GenEnumIds( PObjectBase class_obj);

	/**
	* Generate the constructor of a classs
	*/
	void GenConstructor( PObjectBase class_obj, const EventVector &events );

	/**
	* Realiza la construcción de los objetos, configurando las propiedades del
	* objeto y las de layout.
	* El algoritmo es similar al de generación de la vista previa en el designer.
	*/
	void GenConstruction( PObjectBase obj, bool is_widget );

	/**
	* Configura las propiedades del objeto, tanto las propias como las heredadas.
	* Se le pasa la información de la clase porque recursivamente, realizará
	* la configuración en las super-clases.
	*/
	void GenSettings( PObjectInfo info, PObjectBase obj);

	/**
	* Añade un control a una toolbar. Hay que pasarle el objectinfo de tipo
	* wxWindow, donde se encuentra la plantilla, y el objectbase del control
	*/
	void GenAddToolbar( PObjectInfo info, PObjectBase obj );

	void GenPrivateEventHandlers(const EventVector &events);
  void GenVirtualEventHandlers(const EventVector &events);

public:
	/**
	* Convert a wxString to the "C/C++" format.
	*/
	static wxString ConvertCppString( wxString text);

	/**
	* Convert a path to a relative path.
	*/
	//static wxString ConvertToRelativePath( wxString path, wxString basePath);

	/**
	* Convert an XPM filename to the name of the XPM's internal character array.
	*/
	static wxString ConvertXpmName( const wxString& text );

	CppCodeGenerator();

	/**
	* Set the codewriter for the header file
	*/
	void SetHeaderWriter( PCodeWriter cw )
	{
		m_header = cw;
	}

	/**
	* Set the codewriter for the source file
	*/
	void SetSourceWriter( PCodeWriter cw )
	{
		m_source = cw;
	}


	/**
	* Configura el path de referencia para generar las rutas relativas
	* al path que se pasa como parámetro.
	*
	* @note path is generated with the separators, '/', since on Windows
	*		the compilers interpret path correctly.
	*/
	void UseRelativePath( bool relative = false, wxString basePath = wxString() );

	/**
	* Set the First ID used during Code Generation.
	*/
	void SetFirstID( const unsigned int id ){ m_firstID = id; }

	/**
	* Generate the project's code
	*/
	bool GenerateCode( PObjectBase project );

	/**
	* Generate an inherited class
	*/
	void GenerateInheritedClass( PObjectBase userClasses );
};


#endif //_CPP_CODE_GEN_
