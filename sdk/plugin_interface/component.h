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
namespace tinyxml2
{
class XMLElement;
}
class IComponentLibrary;


/**
 * @brief Object interface for plugins
 *
 * This interface is used by plugin components to query properties for the objects they create.
 */
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


/**
 * @brief Placeholder object for objects not part of a wxWindow hierarchy
 *
 * Plugin components must always create an object, even if they create non-visual elements.
 * Objects of this class must be returned for such elements.
 */
class wxNoObject : public wxObject
{
};


/**
 * @brief Manager interface for plugins
 *
 * This interface is used by plugin components to interact with the main application.
 */
class IManager
{
public:
    virtual ~IManager() = default;

    /**
     * @brief Get the object interface of the object
     *
     * @param wxobject Object
     * @return Object interface
     */
    virtual IObject* GetIObject(wxObject* wxobject) const = 0;

    /**
     * @brief Get the count of child objects
     *
     * @param wxobject Object
     * @return Number of child objects
     */
    virtual std::size_t GetChildCount(wxObject* wxobject) const = 0;
    /**
     * @brief Get a child object
     *
     * @param wxobject Object
     * @param childIndex Child index
     * @return Child object at childIndex
     */
    virtual wxObject* GetChild(wxObject* wxobject, std::size_t childIndex) const = 0;

    /**
     * @brief Get the parent object
     *
     * @param wxobject Object
     * @return Parent object of object
     */
    virtual wxObject* GetParent(wxObject* wxobject) const = 0;
    /**
     * @brief Get the object interface of the parent object
     *
     * @param wxobject Object
     * @return Object interface of parent object of object
     */
    virtual IObject* GetIParent(wxObject* wxobject) const = 0;

    /**
     * @brief Select the object in the object tree
     *
     * @param wxobject The object to select
     * @return True, if the selection did change, false, if the object was already selected
     */
    virtual bool SelectObject(wxObject* wxobject) = 0;
    /**
     * @brief Modify the property of the object
     *
     * @param wxobject Object to modify
     * @param property Property to modify
     * @param value New value of the property
     * @param allowUndo If true, the property change will be placed into the undo stack, otherwise the change will be
     *                  silent
     */
    virtual void ModifyProperty(
      wxObject* wxobject, const wxString& property, const wxString& value, bool allowUndo = true) = 0;

    /**
     * @brief Create a wxNoObject
     *
     * TODO: This shall ensure that these objects are created in the main application and the dynamic cast to test for
     *       them does work there, but is this really necessary? All other objects are created inside the plugin and
     *       deleted by wxWidgets in the main application.
     *
     * @return A wxNoObject
     */
    virtual wxNoObject* NewNoObject() = 0;
};


/**
 * @brief Base interface of a plugin component
 *
 * A plugin component implements the functionality to create objects of a specific type for usage by the main
 * application.
 */
class IComponent
{
public:
    /**
     * @brief Type of the component
     */
    enum class Type {
        /// Non-visual element, won't be inserted into a wxWindow hierarchy
        Abstract = 0,
        /// Visual element that will be inserted into a wxWindow hierarchy
        Window,
        /// Sizer element
        Sizer,
    };

public:
    virtual ~IComponent() = default;

    /**
     * @return Type of this component
     */
    virtual Type GetType() const = 0;
    /**
     * @brief Get the component library of this object
     *
     * @return The component library of this object, this is never nullptr
     */
    virtual IComponentLibrary* GetLibrary() const = 0;

    /**
     * @brief Create a new object
     *
     * @param obj Properties of the object to create
     * @param parent Parent object of the object to create
     * @return The created object
     */
    virtual wxObject* Create(IObject* obj, wxObject* parent) = 0;
    /**
     * @brief Perform additional cleanup actions before deleting the object
     *
     * This function gets called just before the object is about to be deleted
     * and allows to perform additional cleanup actions.
     *
     * @param wxobject Object created by Create(IObject*, wxObject*)
     */
    virtual void Cleanup(wxObject* wxobject) = 0;

    /**
     * @brief Callback function executed after the creation of the object
     *
     * This function gets called after the object has been created and registered into
     * the internal data structure. This function can be used to perform additional steps,
     * e.g. abstract components like notebookpage or sizeritem can add the created widget to
     * the notebook or sizer.
     *
     * @param wxobject The object that was created
     * @param wxparent The parent object of the created object
     */
    virtual void OnCreated(wxObject* wxobject, wxWindow* wxparent) = 0;
    /**
     * @brief Callback function executed after the object has been selected in the object tree
     *
     * This function can be used to perform additional steps after the object has been selected,
     * e.g. after selecting a notebookpage the notebook could switch to that page.
     *
     * @param wxobject The selected object
     */
    virtual void OnSelected(wxObject* wxobject) = 0;

    /**
     * @brief Export object properties into a XRC node
     *
     * @param obj Object properties
     * @return The created XRC node, the caller gets ownership of the object
     */
    virtual ticpp::Element* ExportToXrc(IObject* obj) = 0;
    /**
     * @brief Export object properties into a XRC node
     *
     * @param xrc Target node, must be an empty node
     * @param obj Object properties
     * @return On success, xrc is returned, otherwise nullptr
     */
    virtual tinyxml2::XMLElement* ExportToXrc(tinyxml2::XMLElement* xrc, const IObject* obj) = 0;
    /**
     * @brief Import object properties from a XRC node
     *
     * The properties are converted into a XML node of a wxFormBuilder project file
     *
     * @param xrcObj XRC node
     * @return wxFormBuilder project file XML node, the caller gets ownership of the object
     */
    virtual ticpp::Element* ImportFromXrc(ticpp::Element* xrcObj) = 0;
    /**
     * @brief Import object properties from a XRC node
     *
     * The properties are converted into a XML node of a wxFormBuilder project file
     *
     * @param xfb Target node, must be an empty node
     * @param xrc XRC node
     * @return On success, xfb is returned, otherwise nullptr
     */
    virtual tinyxml2::XMLElement* ImportFromXrc(tinyxml2::XMLElement* xfb, const tinyxml2::XMLElement* xrc) = 0;
};


/**
 * @brief Component library interface of a plugin
 *
 * This interface is used by the plugin to register all its components.
 * The main application uses this interface to access the components of the plugin.
 */
class IComponentLibrary
{
public:
    virtual ~IComponentLibrary() = default;

    /**
     * @brief Register a component
     *
     * @param name Name of the component
     * @param component Component implementation, ownership is claimed
     */
    virtual void RegisterComponent(const wxString& name, IComponent* component) = 0;
    /**
     * @brief Register a macro
     *
     * @param macro Symbolic name of the macro
     * @param value Value of the macro
     */
    virtual void RegisterMacro(const wxString& macro, int value) = 0;
    /**
     * @brief Register a macro synonymous
     *
     * @param synonymous Synonymous name of the macro
     * @param macro Symbolic name of the macro
     */
    virtual void RegisterSynonymous(const wxString& synonymous, const wxString& macro) = 0;

    /**
     * @brief Get the used manager object
     *
     * @return Manager object, this is never nullptr
     */
    virtual IManager* GetManager() const = 0;
    /**
     * @brief Replace a macro synonymous by the macro itself
     *
     * @param synonymous Synonymous name of the macro
     * @param replaced If not nullptr, set to true if a replacement was done, otherwise to false
     * @return The macro name or synonymous if the given value is not a synonymous of a macro
     */
    virtual wxString ReplaceSynonymous(const wxString& synonymous, bool* replaced = nullptr) const = 0;

    /**
     * @brief Get all registered components
     *
     * @return All registered comonents, the objects are owned by this instance and must not be deleted
     */
    virtual std::vector<std::pair<wxString, IComponent*>> GetComponents() const = 0;
    /**
     * @brief Get all registered macros
     *
     * @return All registered macros
     */
    virtual std::vector<std::pair<wxString, int>> GetMacros() const = 0;
};


// The component library interface functions are part of a statically linked library that
// gets linked into a shared library. The implementation itself is done directly inside the
// shared library though. Since the shared library is loaded dynamically and the functions
// are resolved dynamically, this very limited export specification is sufficient.
#ifdef BUILD_DLL
    #define DLL_FUNC extern "C" WXEXPORT
#else
    #define DLL_FUNC extern "C"
#endif

/**
 * @brief Create a component library object
 *
 * Creates a new instance of the component library of the plugin using the given manager object.
 *
 * @param manager Manager object used by the component library
 * @return The component library object
 */
DLL_FUNC IComponentLibrary* CreateComponentLibrary(IManager* manager);
/**
 * @brief Deletes a component library object
 *
 * @param lib The component library object, it must have been created by this plugin
 */
DLL_FUNC void FreeComponentLibrary(IComponentLibrary* lib);

#endif  // SDK_PLUGIN_INTERFACE_COMPONENT_H
