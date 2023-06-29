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

#include "xrcfilter.h"

#include <wx/log.h>

#include <common/xmlutils.h>

#include "model/objectbase.h"
#include "utils/debug.h"
#include "utils/scopeexit.h"
#include "utils/typeconv.h"
#include "utils/wxfbexception.h"


PObjectBase XrcLoader::GetProject(const tinyxml2::XMLDocument* xrc)
{
    const auto* root = xrc->FirstChildElement("resource");
    if (!root) {
        wxLogError(_("Invalid root node"));

        return PObjectBase();
    }

    auto project = m_objDb->CreateObject("Project");
    for (const auto* object = root->FirstChildElement("object"); object; object = object->NextSiblingElement("object")) {
        GetObject(object, project);
    }

    return project;
}


PObjectBase XrcLoader::GetObject(const tinyxml2::XMLElement* object, PObjectBase parent)
{
    auto className = XMLUtils::StringAttribute(object, "class");
    if (className.empty()) {
        THROW_WXFBEX(wxString::Format(_("Line %i: Empty object class"), object->GetLineNum()))
    }

    // Our data model mostly uses the same class names as the XRC data model, but in some cases a different name is used
    if (parent->GetObjectTypeName() == "project") {
        // FIXME: Why does this only return if the property is found?
        if (className == "wxBitmap") {
            auto bitmapsProperty = parent->GetProperty("bitmaps");
            if (bitmapsProperty) {
                auto bitmapsValue = bitmapsProperty->GetValue();
                auto objectValue = XMLUtils::GetText(object);

                // FIXME: What kind of format is this?
                objectValue.Replace("\'", "\'\'", true);
                bitmapsValue << "\'" << objectValue << "\'";
                bitmapsProperty->SetValue(bitmapsValue);

                return PObjectBase();
            }
        } else if (className == "wxIcon") {
            auto iconsProperty = parent->GetProperty("icons");
            if (iconsProperty) {
                auto iconsValue = iconsProperty->GetValue();
                auto objectValue = XMLUtils::GetText(object);

                // FIXME: What kind of format is this?
                objectValue.Replace("\'", "\'\'", true);
                iconsValue << "\'" << objectValue << "\'";
                iconsProperty->SetValue(iconsValue);

                return PObjectBase();
            }
        }

        // In our data model toplevel elements use the class name without wx prefix
        className = className.substr(2);
    } else if (className == "wxMenu") {
        // In our data model a different class is used if the menu is a submenu
        if (parent->GetClassName() == "wxMenu" || parent->GetClassName() == "submenu") {
            className = "submenu";
        }
    } else if (className == "separator") {
        // In our data model toolbars use a different class for their separators
        if (parent->GetClassName() == "wxToolBar" || parent->GetClassName() == "ToolBar") {
            className = "toolSeparator";
        }
    } else if (className == "spacer" || className == "sizeritem") {
        // In our data model spacers are sizeritems and wxGridBagSizer uses a different class for its items
        if (parent->GetClassName() == "wxGridBagSizer") {
            className = "gbsizeritem";
        } else {
            className = "sizeritem";
        }
    }

    if (auto objectInfo = m_objDb->GetObjectInfo(className); objectInfo) {
        auto* objectComponent = objectInfo->GetComponent();
        if (!objectComponent) {
            wxLogError(
                _("Line %i: No component for class \"%s\""),
                object->GetLineNum(),
                className
            );

            return PObjectBase();
        }
        auto* objectXfb = m_xfb.NewElement("");
        ScopeExit objectXfbDeleter([this, objectXfb]() { m_xfb.DeleteNode(objectXfb); });
        if (!objectComponent->ImportFromXrc(objectXfb, object)) {
            wxLogError(
                _("Line %i: No xfb created for class \"%s\""),
                object->GetLineNum(),
                className
            );

            return PObjectBase();
        }

        auto baseObject = m_objDb->CreateObject(objectXfb, parent);
        // FIXME: Why is an attempt made using this intermediate sizer?
        if (!baseObject) {
            auto baseSizer = m_objDb->CreateObject("wxBoxSizer", parent);
            // CreateObject might return an "item" that contains the acutal object, e.g. sizeritem or splitteritem.
            // In that case, determine the real object and use that one.
            // TODO: What is the proper way to do this?
            auto targetSizer = (baseSizer && baseSizer->GetChildCount() > 0 ? baseSizer->GetChild(0) : baseSizer);
            if (targetSizer) {
                baseObject = m_objDb->CreateObject(objectXfb, targetSizer);
                if (baseObject) {
                    parent->AddChild(baseSizer);
                    baseSizer->SetParent(parent);
                }
            }
        }
        if (!baseObject) {
            wxLogError(
                _("Line %i: Failed to create object for class \"%s\" using a parent object of class \"%s\""),
                object->GetLineNum(),
                className,
                parent->GetClassName()
            );

            return PObjectBase();
        }

        // CreateObject might return an "item" that contains the acutal object, e.g. sizeritem or splitteritem.
        // In that case, determine the real object and use that one.
        // TODO: What is the proper way to do this?
        auto targetObject = (baseObject->GetChildCount() > 0 ? baseObject->GetChild(0) : baseObject);
        if (targetObject) {
            for (const auto* child = object->FirstChildElement("object"); child; child = child->NextSiblingElement("object")) {
                GetObject(child, targetObject);
            }
        }

        return targetObject;
    }

    // Unknown class, replace with a wxPanel
    auto replacementObject = m_objDb->CreateObject("wxPanel", parent);
    if (!replacementObject) {
        wxLogError(
            _("Line %i: Unknown class \"%s\" could not be replaced with a wxPanel, parent object \"%s : %s\""),
            object->GetLineNum(),
            className,
            parent->GetPropertyAsString("name"),
            parent->GetClassName()
        );

        return PObjectBase();
    }

    parent->AddChild(replacementObject);
    replacementObject->SetParent(parent);
    wxLogError(
        _("Line %i: Unknown class \"%s\" replaced with a wxPanel"),
        object->GetLineNum(),
        className
    );

    return replacementObject;
}
