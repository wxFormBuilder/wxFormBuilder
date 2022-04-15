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

#ifndef SDK_PLUGIN_INTERFACE_COMPONENT_H
#define SDK_PLUGIN_INTERFACE_COMPONENT_H

#include <wx/wx.h>

#include <utility>
#include <vector>

#include "fontcontainer.h"


namespace ticpp
{
class Element;
}
class IComponentLibrary;


// Plugins interface
// The point is to provide an interface for accessing the object's properties
// from the plugin itself, in a safe way.
class IObject
{
public:
    virtual ~IObject() = default;

    virtual wxString GetClassName() const = 0;
    virtual wxString GetObjectTypeName() const = 0;
    virtual std::size_t GetChildCount() const = 0;
    virtual IObject* GetChildObject(std::size_t index) const = 0;

    virtual wxString GetChildFromParentProperty(const wxString& parentName, const wxString& childName) const = 0;

    virtual bool IsPropertyNull(const wxString& name) const = 0;
    virtual int GetPropertyAsInteger(const wxString& name) const = 0;
    virtual double GetPropertyAsFloat(const wxString& name) const = 0;
    virtual wxString GetPropertyAsString(const wxString& name) const = 0;
    virtual wxPoint GetPropertyAsPoint(const wxString& name) const = 0;
    virtual wxSize GetPropertyAsSize(const wxString& name) const = 0;
    virtual wxBitmap GetPropertyAsBitmap(const wxString& name) const = 0;
    virtual wxColour GetPropertyAsColour(const wxString& name) const = 0;
    virtual wxFontContainer GetPropertyAsFont(const wxString& name) const = 0;
    virtual wxArrayInt GetPropertyAsArrayInt(const wxString& name) const = 0;
    virtual wxArrayString GetPropertyAsArrayString(const wxString& name) const = 0;
    virtual std::vector<std::pair<int, int>> GetPropertyAsVectorIntPair(const wxString& name) const = 0;
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
    virtual ~IManager() = default;

    /**
    Get the corresponding object interface pointer for the object.
    This allows easy read only access to properties.
    */
    virtual IObject* GetIObject(wxObject* wxobject) const = 0;

    /**
    Get the count of the children of this object.
    */
    virtual std::size_t GetChildCount(wxObject* wxobject) const = 0;
    /**
    Get a child of the object.
    @param childIndex Index of the child to get.
    */
    virtual wxObject* GetChild(wxObject* wxobject, std::size_t childIndex) const = 0;

    /**
    Get the parent of the object.
    */
    virtual wxObject* GetParent(wxObject* wxobject) const = 0;
    /**
    Get the IObject interface to the parent of the object.
    */
    virtual IObject* GetIParent(wxObject* wxobject) const = 0;

    /**
    Select the object in the object tree
    Returns true if selection changed, false if already selected
    */
    virtual bool SelectObject(wxObject* wxobject) = 0;

    /**
    Modify a property of the object.
    @param property The name of the property to modify.
    @param value The new value for the property.
    @param allowUndo If true, the property change will be placed into the undo stack, if false it will be modified
    silently.
    */
    virtual void ModifyProperty(wxObject* wxobject, const wxString& property, const wxString& value, bool allowUndo = true) = 0;

    // used so the wxNoObjects are both created and destroyed in the application
    virtual wxNoObject* NewNoObject() = 0;
};


/**
 * Component Interface
 */
class IComponent
{
public:
    enum class Type {
        Abstract = 0,
        Window,
        Sizer,
    };

public:
    virtual ~IComponent() = default;

    virtual IComponentLibrary* GetLibrary() const = 0;
    
    virtual Type GetComponentType() const = 0;

    /**
     * Create an instance of the wxObject and return a pointer
     */
    virtual wxObject* Create(IObject* obj, wxObject* parent) = 0;
    /**
     * Cleanup (do the reverse of Create)
     */
    virtual void Cleanup(wxObject* wxobject) = 0;

    /**
     * Allows components to do something after they have been created.
     * For example, Abstract components like NotebookPage and SizerItem can
     * add the actual widget to the Notebook or sizer.
     *
     * @param wxobject The object which was just created.
     * @param wxparent The wxWidgets parent - the wxObject that the created object was added to.
     */
    virtual void OnCreated(wxObject* wxobject, wxWindow* wxparent) = 0;
    /**
     * Allows components to respond when selected in object tree.
     * For example, when a wxNotebook's page is selected, it can switch to that page
     */
    virtual void OnSelected(wxObject* wxobject) = 0;

    /**
     * Export the object to an XRC node
     */
    virtual ticpp::Element* ExportToXrc(IObject* obj) = 0;
    /**
     * Converts from an XRC element to a wxFormBuilder project file XML element
     */
    virtual ticpp::Element* ImportFromXrc(ticpp::Element* xrcObj) = 0;
};


// Interface which intends to contain all the components for a plugin
// This is an abstract class and it'll be the object that the DLL will export.
class IComponentLibrary
{
public:
    virtual ~IComponentLibrary() = default;

    // Used by the plugin for registering components and macros
    virtual void RegisterComponent(const wxString& name, IComponent* component) = 0;
    virtual void RegisterMacro(const wxString& macro, int value) = 0;
    virtual void RegisterSynonymous(const wxString& synonymous, const wxString& macro) = 0;

    virtual IManager* GetManager() const = 0;
    virtual wxString ReplaceSynonymous(const wxString& synonymous, bool* replaced = nullptr) const = 0;

    // Used by wxFormBuilder for recovering components and macros
    virtual std::vector<std::pair<wxString, IComponent*>> GetComponents() const = 0;
    virtual std::vector<std::pair<wxString, int>> GetMacros() const = 0;
};


#ifdef BUILD_DLL
    #define DLL_FUNC extern "C" WXEXPORT
#else
    #define DLL_FUNC extern "C"
#endif

// Function that the application calls to get the library
DLL_FUNC IComponentLibrary* CreateComponentLibrary(IManager* manager);

// Function that the application calls to free the library
DLL_FUNC void FreeComponentLibrary(IComponentLibrary* lib);

#endif  // SDK_PLUGIN_INTERFACE_COMPONENT_H
