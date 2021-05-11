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
@note
The implementation of the generation of relative paths is a little hacky, and not a solution.
The value of all properties that are file or a directory paths must be absolute, otherwise the code generation will not work.
*/

#ifndef _CPP_CODE_GEN_
#define _CPP_CODE_GEN_

#include "codegen.h"
#include "codeparser.h"

#include <set>
#include <vector>

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
	PTemplateParser CreateParser(const TemplateParser* oldparser, wxString _template) override;
	wxString RootWxParentToCode() override;
	wxString ValueToCode(PropertyType type, wxString value) override;
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

	CppCodeParser m_inheritedCodeParser;

	PCodeWriter m_header;
	PCodeWriter m_source;

	bool m_useRelativePath;
	bool m_useArrayEnum;
	bool m_i18n;
	wxString m_basePath;
	unsigned int m_firstID;
	bool m_useConnect;
	bool m_disconnectEvents;

	/**
	* Predefined macros won't generate defines.
	*/
	std::set<wxString> m_predMacros;

	void SetupPredefinedMacros();

	/**
	* Given an object and the name for a template, obtains the code.
	*/
	wxString GetCode( PObjectBase obj, wxString name);

	/**
	* Gets the declaration fragment for the specified object.
	*
	* This method encapsulates the adjustments that need to be made for array declarations.
	*/
	wxString GetDeclaration(PObjectBase obj, ArrayItems& arrays, bool useEnum);

	/**
	* Stores the project's objects classes set, for generating the includes.
	*/
	void FindDependencies( PObjectBase obj, std::set< PObjectInfo >& info_set );

	/**
	* Stores the needed "includes" set for the PT_BITMAP properties.
	*/
	void FindEmbeddedBitmapProperties( PObjectBase obj, std::set< wxString >& embedset);

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
	void GenClassDeclaration(PObjectBase class_obj, bool use_enum, const wxString& classDecoration, const EventVector &events, ArrayItems& arrays);

	/**
	* Generates the event table.
	*/
	void GenEvents( PObjectBase class_obj, const EventVector &events, bool disconnect = false );

	/**
	* helper function to find the event table entry template in the class or its base classes
	*/
	bool GenEventEntry( PObjectBase obj, PObjectInfo obj_info, const wxString& templateName, const wxString& handlerName, bool disconnect = false );

	/**
	* Recursive function for the attributes declaration, used inside GenClassDeclaration.
	*/
	void GenAttributeDeclaration(PObjectBase obj, Permission perm, ArrayItems& arrays);

	/**
	* Recursive function for the validators' variables declaration, used inside GenClassDeclaration.
	*/
	void GenValidatorVariables( PObjectBase obj);
	/**
	* Recursive function for the validators' variables declaration, used inside GenClassDeclaration.
	*/
	void GenValVarsBase( PObjectInfo info, PObjectBase obj);

	/**
	* Generates the generated_event_handlers template
	*/
	void GetGenEventHandlers( PObjectBase obj );
	/**
	* Generates the generated_event_handlers template
	*/
	void GenDefinedEventHandlers( PObjectInfo info, PObjectBase obj );

	/**
	* Generates the '#include' section for files.
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
	* Generates the '#include' section for the embedded bitmap properties.
	*/
	void GenEmbeddedBitmapIncludes( PObjectBase project);

	/**
	* Generates the '#define' section for macros.
	*/
	void GenDefines( PObjectBase project);

	/**
	* Generates an enum with wxWindow identifiers.
	*/
	void GenEnumIds( PObjectBase class_obj);

	/**
	* Generates the constructor for a class
	*/
	void GenConstructor(PObjectBase class_obj, const EventVector &events, ArrayItems& arrays);

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

	void GenPrivateEventHandlers(const EventVector &events);

    void GenVirtualEventHandlers( const EventVector &events, const wxString& eventHandlerPrefix, const wxString& eventHandlerPostfix );

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
	* Convert an Embedded Bitmap filename to the name of the character array.
	*/
	static wxString ConvertEmbeddedBitmapName( const wxString& text );

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
	* Parse existing source/header files
	*/
	void ParseFiles(wxString headerFile, wxString sourceFile)
	{
		m_inheritedCodeParser = CppCodeParser(headerFile, sourceFile);
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
	* Generate the project's code
	*/
	bool GenerateCode(PObjectBase project) override;

	/**
	* Generate an inherited class
	*/
	void GenerateInheritedClass( PObjectBase userClasses, PObjectBase form );
};


#endif //_CPP_CODE_GEN_
