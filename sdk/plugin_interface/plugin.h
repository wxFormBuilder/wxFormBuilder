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

#ifndef SDK_PLUGIN_INTERFACE_PLUGIN_H
#define SDK_PLUGIN_INTERFACE_PLUGIN_H

#include <map>
#include <memory>
#include <vector>

#include "component.h"


/**
 * @brief ComponentLibrary implementation for plugins
 */
class ComponentLibrary : public IComponentLibrary
{
public:
    ComponentLibrary(IManager* manager) : m_manager(manager) {}

    void RegisterComponent(const wxString& name, IComponent* component) override;
    void RegisterMacro(const wxString& macro, int value) override;
    void RegisterSynonymous(const wxString& synonymous, const wxString& macro) override;

    IManager* GetManager() const override { return m_manager; }
    wxString ReplaceSynonymous(const wxString& synonymous, bool* replaced = nullptr) const override;

    std::vector<std::pair<wxString, IComponent*>> GetComponents() const override;
    std::vector<std::pair<wxString, int>> GetMacros() const override;

private:
    /// Registered components
    std::map<wxString, std::unique_ptr<IComponent>> m_components;
    /// Registered macros
    std::map<wxString, int> m_macros;
    /// Registered synonymous
    std::map<wxString, wxString> m_synonymous;

    /// Manager this object was created for
    IManager* m_manager;
};


/**
 * @brief Base class of plugin components
 */
class ComponentBase : public IComponent
{
public:
    ComponentBase() : m_type(Type::Abstract), m_library(nullptr) {}

    /**
     * @brief Set the component type
     *
     * TODO: Maybe component implementations should define their type themself
     *
     * @param type Component type
     */
    void SetType(Type type) { m_type = type; }
    /**
     * @brief Set the component library this object is part of
     *
     * @param library Component library
     */
    void SetLibrary(IComponentLibrary* library) { m_library = library; }

    /**
     * @brief Get the manager object
     *
     * Helper/compatibility function to get the manager directly from the component object.
     *
     * @return Manager object
     */
    IManager* GetManager() const { return m_library->GetManager(); }

    Type GetType() const override { return m_type; }
    IComponentLibrary* GetLibrary() const override { return m_library; }

    wxObject* Create([[maybe_unused]] IObject* obj, [[maybe_unused]] wxObject* parent) override
    {
        return m_library->GetManager()->NewNoObject();
    }
    void Cleanup([[maybe_unused]] wxObject* wxobject) override {}

    void OnCreated([[maybe_unused]] wxObject* wxobject, [[maybe_unused]] wxWindow* wxparent) override {}
    void OnSelected([[maybe_unused]] wxObject* wxobject) override {}

    tinyxml2::XMLElement* ExportToXrc([[maybe_unused]] tinyxml2::XMLElement* xrc, [[maybe_unused]] const IObject* obj) override { return nullptr; }
    tinyxml2::XMLElement* ImportFromXrc([[maybe_unused]] tinyxml2::XMLElement* xfb, [[maybe_unused]] const tinyxml2::XMLElement* xrc) override { return nullptr; }

private:
    /// Component type
    Type m_type;

    /// Component library this object is part of
    IComponentLibrary* m_library;
};


#define BEGIN_LIBRARY() \
    DLL_FUNC IComponentLibrary* CreateComponentLibrary(IManager* manager) \
    { \
        auto* componentLibrary = new ComponentLibrary(manager);

#define END_LIBRARY() \
    return componentLibrary; \
    } \
\
    DLL_FUNC void FreeComponentLibrary(IComponentLibrary* lib) { delete lib; }


#define MACRO(macro) componentLibrary->RegisterMacro(wxT(#macro), macro);

#define SYNONYMOUS(synonymous, macro) componentLibrary->RegisterSynonymous(wxT(#synonymous), wxT(#macro));

#define _REGISTER_COMPONENT(name, class, type) \
    { \
        ComponentBase* component = new class(); \
        component->SetType(type); \
        component->SetLibrary(componentLibrary); \
        componentLibrary->RegisterComponent(wxT(name), component); \
    }

#define WINDOW_COMPONENT(name, class) _REGISTER_COMPONENT(name, class, ComponentBase::Type::Window)

#define SIZER_COMPONENT(name, class) _REGISTER_COMPONENT(name, class, ComponentBase::Type::Sizer)

#define ABSTRACT_COMPONENT(name, class) _REGISTER_COMPONENT(name, class, ComponentBase::Type::Abstract)

#endif  // SDK_PLUGIN_INTERFACE_PLUGIN_H
