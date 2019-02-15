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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// Written by
//   José Antonio Hurtado - joseantonio.hurtado@gmail.com
//   Juan Antonio Ortega  - jortegalalmolda@gmail.com
//
// Python code generation writen by
//   Michal Bližňak - michal.bliznak@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

/**
@file
@author Michal Bližňák - michal.bliznak@gmail.com
@note
The implementation of the generation of relative paths is a little hacky, and not a solution.
The value of all properties that are file or a directory paths must be absolute, otherwise the code generation will not work.
*/

#ifndef _PYTHON_CODE_GEN_
#define _PYTHON_CODE_GEN_

// wxFormBuilder flags (fbfXXX)
#define fbfSILENT true
#define fbfMESSAGE false

#include "codegen.h"

#include <map>
#include <set>
#include <vector>

/**
* Parse the Python templates.
*/
class PythonTemplateParser : public TemplateParser
{
private:
	bool m_i18n;
	bool m_useRelativePath;
	wxString m_basePath;
	wxString m_imagePathWrapperFunctionName;

	std::map<wxString, wxString> m_predModulePrefix;

	void SetupModulePrefixes();

public:
	PythonTemplateParser( PObjectBase obj, wxString _template, bool useI18N, bool useRelativePath, wxString basePath );
	PythonTemplateParser( const PythonTemplateParser & that, wxString _template );

	// overrides for Python
	PTemplateParser CreateParser(const TemplateParser* oldparser, wxString _template) override;
	wxString RootWxParentToCode() override;
	wxString ValueToCode(PropertyType type, wxString value) override;

	// Parameterized setters
	void SetImagePathWrapperFunctionName( wxString imagePathWrapperFunctionName );
};

/**
* Generate the Python code
*/
class PythonCodeGenerator : public CodeGenerator
{
private:
	PCodeWriter m_source;

	bool m_useRelativePath;
	bool m_i18n;
	wxString m_basePath;
	wxString m_imagePathWrapperFunctionName;
	unsigned int m_firstID;
	bool m_disconnectEvents;
	wxString m_disconnecMode;

	/**
	* Predefined macros won't generate defines.
	*/
	std::set<wxString> m_predMacros;

	void SetupPredefinedMacros();

	/**
	* Given an object and the name for a template, obtains the code.
	*/
	wxString GetCode( PObjectBase obj, wxString name, bool silent = false);

	/**
	* Gets the construction fragment for the specified object.
	*
	* This method encapsulates the adjustments that need to be made for array declarations.
	*/
	wxString GetConstruction(PObjectBase obj, bool silent, ArrayItems& arrays);

	/**
	* Stores the project's objects classes set, for generating the includes.
	*/
	void FindDependencies( PObjectBase obj, std::set< PObjectInfo >& info_set );

	/**
	* Stores all the properties for "macro" type objects, so that their
	* related '#define' can be generated subsequently.
	*/
	void FindMacros( PObjectBase obj, std::vector< wxString >* macros );

	/**
	 * Looks for "non-null" event handlers (PEvent) and collects it into a vector.
	 */
	void FindEventHandlers(PObjectBase obj, EventVector &events);

	/**
	* Generates classes declarations inside the header file.
	*/
	void GenClassDeclaration(PObjectBase class_obj, bool use_enum, const wxString& classDecoration, const EventVector& events, const wxString& eventHandlerPostfix, ArrayItems& arrays);

	/**
	* Generates the event table.
	*/
	void GenEvents( PObjectBase class_obj, const EventVector &events, bool disconnect = false );

	/**
	* helper function to find the event table entry template in the class or its base classes
	*/
	bool GenEventEntry( PObjectBase obj, PObjectInfo obj_info, const wxString& templateName, const wxString& handlerName, bool disconnect = false );

	/**
	* Generates the generated_event_handlers template
	*/
	void GetGenEventHandlers( PObjectBase obj );
	/**
	* Generates the generated_event_handlers template
	*/
	void GenDefinedEventHandlers( PObjectInfo info, PObjectBase obj );

	/**
	* Generates the 'import' section for files.
	*/
	void GenIncludes( PObjectBase project, std::vector< wxString >* includes, std::set< wxString >* templates );
	void GenObjectIncludes( PObjectBase project, std::vector< wxString >* includes, std::set< wxString >* templates );
	void GenBaseIncludes( PObjectInfo info, PObjectBase obj, std::vector< wxString >* includes, std::set< wxString >* templates );
	void AddUniqueIncludes( const wxString& include, std::vector< wxString >* includes );

	/**
	* Generate a set of all subclasses to forward declare in the generated file.
	*/
	void GenSubclassSets( PObjectBase obj, std::set< wxString >* subclasses, std::vector< wxString >* headerIncludes );

	/**
	* Generates the '#define' section for macros.
	*/
	void GenDefines( PObjectBase project);

	/**
	* Generates the constructor for a class
	*/
	void GenConstructor(PObjectBase class_obj, const EventVector& events, ArrayItems& arrays);

	/**
	* Generates the destructor for a class
	*/
	void GenDestructor( PObjectBase class_obj, const EventVector &events );

	/**
	* Makes the objects construction, setting up the objects' and Layout properties.
	* The algorithm is simmilar to that used in the designer preview generation.
	*/
	void GenConstruction(PObjectBase obj, bool is_widget, ArrayItems& arrays);

	/**
	* Makes the objects destructions.
	*/
	void GenDestruction( PObjectBase obj);

	/**
	* Configures the object properties, both own and inherited ones.
	* Information for the class is given, because it will recursively make the
	* configuration in the "super-classes".
	*/
	void GenSettings( PObjectInfo info, PObjectBase obj);

	/**
	* Adds a control for a toolbar. Needs the objectinfo (wxWindow type) where
	* the template is found, and the objectbase for the control.
	*/
	void GenAddToolbar( PObjectInfo info, PObjectBase obj );
	void GetAddToolbarCode( PObjectInfo info, PObjectBase obj, wxArrayString& codelines );

    void GenVirtualEventHandlers( const EventVector &events, const wxString& eventHandlerPostfix );
	void GenImagePathWrapperFunction();

public:
	/**
	* Convert a wxString to the "C/C++" format.
	*/
	static wxString ConvertPythonString( wxString text);

	PythonCodeGenerator();

	/**
	* Set the codewriter for the source file
	*/
	void SetSourceWriter( PCodeWriter cw )
	{
		m_source = cw;
	}


	/**
	* Configures the reference path for generating relative paths to
	* that passed as parameter.
	*
	* @note path is generated with the separators, '/', since on Windows
	*		the compilers interpret path correctly.
	*/
	void UseRelativePath(bool relative = false, wxString basePath = wxEmptyString);

	/**
	* Set the First ID used during Code Generation.
	*/
	void SetFirstID( const unsigned int id ){ m_firstID = id; }

	/**
	 * Configures the function name, image paths should be wrapped
	 * into when generating python code.
	 * 
	 * When set to a non-empty string, this will generate a default
	 * implementation of the method in each class, that just returns
	 * the unmodified path string.
	 */
	void SetImagePathWrapperFunctionName( wxString imagePathWrapperFunctionName );

	/**
	* Generate the project's code
	*/
	bool GenerateCode(PObjectBase project) override;

	/**
	* Generate an inherited class
	*/
	void GenerateInheritedClass( PObjectBase userClasses, PObjectBase form );
};


#endif //_PYTHON_CODE_GEN_
