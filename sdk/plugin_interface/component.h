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

#pragma once


#include <vector>
#include <utility>

#include "wx/wx.h"
#include "fontcontainer.h"

#define COMPONENT_TYPE_ABSTRACT 0
#define COMPONENT_TYPE_WINDOW   1
#define COMPONENT_TYPE_SIZER    2

namespace ticpp
{
	class Element;
}

class IComponent;

// Sections for source code generation
enum
{
  CG_DECLARATION,
  CG_CONSTRUCTION,
  CG_POST_CONSTRUCTION,
  CG_SETTINGS
};

// Programming languages for source code generation
enum
{
  CG_CPP
};


// Plugins interface
// The point is to provide an interface for accessing the object's properties
// from the plugin itself, in a safe way.
class IObject
{
 public:
  virtual bool     IsNull (const wxString& pname) = 0;
  virtual int      GetPropertyAsInteger (const wxString& pname) = 0;
  virtual wxFontContainer   GetPropertyAsFont    (const wxString& pname) = 0;
  virtual wxColour GetPropertyAsColour  (const wxString& pname) = 0;
  virtual wxString GetPropertyAsString  (const wxString& pname) = 0;
  virtual wxPoint  GetPropertyAsPoint   (const wxString& pname) = 0;
  virtual wxSize   GetPropertyAsSize    (const wxString& pname) = 0;
  virtual wxBitmap GetPropertyAsBitmap  (const wxString& pname) = 0;
  virtual wxArrayInt GetPropertyAsArrayInt(const wxString& pname) = 0;
  virtual wxArrayString GetPropertyAsArrayString(const wxString& pname) = 0;
  virtual std::vector<std::pair<int, int>> GetPropertyAsVectorIntPair(const wxString& pname) = 0;
  virtual double GetPropertyAsFloat(const wxString& pname) = 0;
  virtual wxString GetChildFromParentProperty( const wxString& parentName, const wxString& childName ) = 0;
  virtual wxString GetClassName() = 0;
  virtual unsigned int  GetChildCount() = 0;
  virtual wxString GetObjectTypeName() = 0;
  virtual IObject* GetChildPtr (unsigned int idx) = 0;
  virtual ~IObject(){}
};

// Interface which intends to contain all the components for a plugin
// This is an abstract class and it'll be the object that the DLL will export.
class IComponentLibrary
{
 public:

  // Used by the plugin for registering components and macros
  virtual void RegisterComponent(const wxString &text, IComponent *c) = 0;
  virtual void RegisterMacro(const wxString &text, const int value) = 0;
  virtual void RegisterMacroSynonymous(const wxString &text, const wxString &name) = 0;

  // Used by wxFormBuilder for recovering components and macros
  virtual IComponent* GetComponent(unsigned int idx) = 0;
  virtual wxString    GetComponentName(unsigned int idx) = 0;
  virtual wxString    GetMacroName(unsigned int i) = 0;
  virtual int         GetMacroValue(unsigned int i) = 0;
  //virtual wxString    GetMacroSynonymous(unsigned int i) = 0;
  //virtual wxString    GetSynonymousName(unsigned int i) = 0;
  virtual bool FindSynonymous(const wxString& syn, wxString& trans) = 0;

  virtual unsigned int GetMacroCount() = 0;
  virtual unsigned int GetComponentCount() = 0;
  //virtual unsigned int GetSynonymousCount() = 0;
  virtual ~IComponentLibrary(){}
};

/**
 * Component Interface
 */
class IComponent
{
 public:
  /**
   * Create an instance of the wxObject and return a pointer
   */
  virtual wxObject* Create( IObject* obj, wxObject* parent ) = 0;

  /**
   * Cleanup (do the reverse of Create)
   */
   virtual void Cleanup( wxObject* obj ) = 0;

  /**
   * Allows components to do something after they have been created.
   * For example, Abstract components like NotebookPage and SizerItem can
   * add the actual widget to the Notebook or sizer.
   *
   * @param wxobject The object which was just created.
   * @param wxparent The wxWidgets parent - the wxObject that the created object was added to.
   */
  virtual void OnCreated( wxObject* wxobject, wxWindow* wxparent ) = 0;

  /**
   * Allows components to respond when selected in object tree.
   * For example, when a wxNotebook's page is selected, it can switch to that page
   */
  virtual void OnSelected( wxObject* wxobject ) = 0;

  /**
   * Export the object to an XRC node
   */
  virtual ticpp::Element* ExportToXrc( IObject* obj ) = 0;

  /**
   * Converts from an XRC element to a wxFormBuilder project file XML element
   */
  virtual ticpp::Element* ImportFromXrc( ticpp::Element* xrcObj ) = 0;


  virtual int GetComponentType() = 0;
  virtual ~IComponent(){}
};

// Used to identify wxObject* that must be manually deleted
class wxNoObject : public wxObject
{
};

/**
Interface to the "Manager" class in the application.
Essentially a collection of utility functions that take a wxObject* and do something useful.
*/
class IManager
{
public:
	/**
	Get the count of the children of this object.
	*/
	virtual size_t GetChildCount( wxObject* wxobject ) = 0;

	/**
	Get a child of the object.
	@param childIndex Index of the child to get.
	*/
	virtual wxObject* GetChild( wxObject* wxobject, size_t childIndex ) = 0;

	/**
	Get the parent of the object.
	*/
	virtual wxObject* GetParent( wxObject* wxobject ) = 0;

	/**
	Get the IObject interface to the parent of the object.
	*/
	virtual IObject* GetIParent( wxObject* wxobject ) = 0;

	/**
	Get the corresponding object interface pointer for the object.
	This allows easy read only access to properties.
	*/
	virtual IObject* GetIObject( wxObject* wxobject ) = 0;

	/**
	Modify a property of the object.
	@param property The name of the property to modify.
	@param value The new value for the property.
	@param allowUndo If true, the property change will be placed into the undo stack, if false it will be modified silently.
	*/
	virtual void ModifyProperty( wxObject* wxobject, wxString property, wxString value, bool allowUndo = true ) = 0;

	// used so the wxNoObjects are both created and destroyed in the application
	virtual wxNoObject* NewNoObject() = 0;

	/**
	Select the object in the object tree
	Returns true if selection changed, false if already selected
	*/
	virtual bool SelectObject( wxObject* wxobject ) = 0;

	virtual ~IManager(){}
};

#ifdef BUILD_DLL
	#define DLL_FUNC extern "C" WXEXPORT
#else
	#define DLL_FUNC extern "C"
#endif

// Function that the application calls to get the library
DLL_FUNC IComponentLibrary* GetComponentLibrary( IManager* manager );

// Function that the application calls to free the library
DLL_FUNC void FreeComponentLibrary( IComponentLibrary* lib );

#define BEGIN_LIBRARY()  															\
\
extern "C" WXEXPORT IComponentLibrary* GetComponentLibrary( IManager* manager ) 	\
{ 																					\
  IComponentLibrary* lib = new ComponentLibrary();

#define END_LIBRARY()                                                                   \
        return lib;                                                                     \
	}                                                                                   \
	extern "C" WXEXPORT void FreeComponentLibrary( IComponentLibrary* lib ) 	        \
	{                                                                                   \
        delete lib;                                                                     \
	}

#define MACRO( name ) \
  lib->RegisterMacro( wxT(#name), name );

#define SYNONYMOUS( syn, name ) \
  lib->RegisterMacroSynonymous( wxT(#syn), wxT(#name) );

#define _REGISTER_COMPONENT( name, class, type )\
  {                                     		\
    ComponentBase* c = new class();     		\
    c->__SetComponentType( type );        		\
    c->__SetManager( manager );					\
    lib->RegisterComponent( wxT(name), c ); 	\
  }

#define WINDOW_COMPONENT( name, class ) \
  _REGISTER_COMPONENT( name, class, COMPONENT_TYPE_WINDOW )

#define SIZER_COMPONENT( name,class ) \
  _REGISTER_COMPONENT( name, class, COMPONENT_TYPE_SIZER )

#define ABSTRACT_COMPONENT( name, class ) \
  _REGISTER_COMPONENT( name, class, COMPONENT_TYPE_ABSTRACT )
