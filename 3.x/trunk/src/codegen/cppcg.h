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

using namespace std;

/**
* Parse the C++ templates.
*/
class CppTemplateParser : public TemplateParser
{
private:
	bool m_useRelativePath;
	bool m_i18n;
	wxString m_basePath;

public:
	CppTemplateParser( PObjectBase obj, wxString _template);

	// redefinidas para C++
	shared_ptr<TemplateParser> CreateParser( PObjectBase obj, wxString _template);
	wxString RootWxParentToCode();
	//wxString PropertyToCode( shared_ptr<Property> property);
	wxString ValueToCode( PropertyType type, wxString value);

	// genera rutas relativas en los nombres de archivo
	void UseRelativePath( bool relative = false, wxString basePath = wxString());
	void UseI18n( bool i18n);
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

	shared_ptr<CodeWriter> m_header;
	shared_ptr<CodeWriter> m_source;

	bool m_useRelativePath;
	bool m_i18n;
	wxString m_basePath;
	unsigned int m_firstID;

	/**
	* Las macros predefinidas no generarán defines.
	*/
	set<wxString> m_predMacros;

	void SetupPredefinedMacros();

	/**
	* Dado un objeto y el nombre de una plantilla, obtiene el código.
	*/
	wxString GetCode( PObjectBase obj, wxString name);

	/**
	* Guarda el conjunto de clases de objetos del proyecto para generar
	* los includes.
	*/
	void FindDependencies( PObjectBase obj, set< shared_ptr< ObjectInfo > >& info_set );

	/**
	* Guarda el conjunto de "includes" que hay que generar para las propiedades
	* PT_XPM_BITMAP.
	*/
	void FindXpmProperties( PObjectBase obj, set<wxString> &set);

	/**
	* Guarda todos las propiedades de objetos de tipo "macro" para generar
	* su posterior '#define'.
	*/
	void FindMacros( shared_ptr<ObjectBase> obj, vector<wxString>* macros );

	/**
	 * Looks for "non-null" event handlers (PEvent) and collects it into a vector.
	 */
	void FindEventHandlers(PObjectBase obj, EventVector &events);

	/**
	* Genera la declaración de clases en el fichero de cabecera.
	*/
  void GenClassDeclaration( PObjectBase class_obj, bool use_enum, const EventVector &events);

	/**
   * Generates the event table.
   */
  void GenEventTable( PObjectBase class_obj, const EventVector &events);

	/**
	* Función recursiva para la declaración de atributos, usada dentro
	* de GenClassDeclaration.
	*/
	void GenAttributeDeclaration( PObjectBase obj, Permission perm);

	/**
	* Genera la sección de '#include' fichero.
	*/
	void GenIncludes( PObjectBase project, set<wxString>* includes);
	void GenObjectIncludes( PObjectBase project, set<wxString>* includes);
	void GenBaseIncludes( shared_ptr< ObjectInfo > info, shared_ptr< ObjectBase > obj, set< wxString >* includes );

	/**
	* Generate a set of all subclasses to forward declare in the generated header file.
	* Also generate sets of header files to be include in either the source or header file.
	*/
	void GenSubclassSets( shared_ptr< ObjectBase > obj, set< wxString >* subclasses, set< wxString >* sourceIncludes, set< wxString >* headerIncludes );

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
	void GenConstructor( PObjectBase class_obj );

	/**
	* Realiza la construcción de los objetos, configurando las propiedades del
	* objeto y las de layout.
	* El algoritmo es similar al de generación de la vista previa en el designer.
	*/
	void GenConstruction( PObjectBase obj, bool is_widget);

	/**
	* Configura las propiedades del objeto, tanto las propias como las heredadas.
	* Se le pasa la información de la clase porque recursivamente, realizará
	* la configuración en las super-clases.
	*/
	void GenSettings( shared_ptr<ObjectInfo> info, PObjectBase obj);

	/**
	* Añade un control a una toolbar. Hay que pasarle el objectinfo de tipo
	* wxWindow, donde se encuentra la plantilla, y el objectbase del control
	*/
	void GenAddToolbar( shared_ptr<ObjectInfo> info, PObjectBase obj );

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
	void SetHeaderWriter( shared_ptr<CodeWriter> cw )
	{
		m_header = cw;
	}

	/**
	* Set the codewriter for the source file
	*/
	void SetSourceWriter( shared_ptr<CodeWriter> cw )
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
};


#endif //_CPP_CODE_GEN_
