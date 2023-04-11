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


PObjectBase XrcLoader::GetProject(ticpp::Document* xrcDoc)
{
    assert(m_objDb);
    LogDebug(wxT("[XrcFilter::GetProject]"));

    PObjectBase project(m_objDb->CreateObject("Project"));


    ticpp::Element* root = xrcDoc->FirstChildElement("resource", false);
    if (!root) {
        wxLogError(_("Missing root element \"resource\""));
        return project;
    }

    ticpp::Element* element = root->FirstChildElement("object", false);
    while (element) {
        PObjectBase obj = GetObject(element, project);
        element = element->NextSiblingElement("object", false);
    }

    return project;
}

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


PObjectBase XrcLoader::GetObject(ticpp::Element* xrcObj, PObjectBase parent)
{
    // First, create the object by the name, the modify the properties

    std::string className = xrcObj->GetAttribute("class");
    if (parent->GetObjectTypeName() == wxT("project")) {
        if (className == "wxBitmap") {
            PProperty bitmapsProp = parent->GetProperty(_("bitmaps"));
            if (bitmapsProp) {
                wxString value = bitmapsProp->GetValue();
                wxString text = _WXSTR(xrcObj->GetText());
                text.Replace(wxT("\'"), wxT("\'\'"), true);
                value << wxT("\'") << text << wxT("\' ");
                bitmapsProp->SetValue(value);
                return PObjectBase();
            }
        }
        if (className == "wxIcon") {
            PProperty iconsProp = parent->GetProperty(_("icons"));
            if (iconsProp) {
                wxString value = iconsProp->GetValue();
                wxString text = _WXSTR(xrcObj->GetText());
                text.Replace(wxT("\'"), wxT("\'\'"), true);
                value << wxT("\'") << text << wxT("\' ");
                iconsProp->SetValue(value);
                return PObjectBase();
            }
        }

        // Forms wxPanel, wxFrame, wxDialog are stored internally as Panel, Frame, and Dialog
        // to prevent conflicts with wxPanel as a container
        className = className.substr(2, className.size() - 2);
    }

    // Well, this is not nice. wxMenu class name is ambiguous, so we'll get the
    // correct class by the context. If the parent of a wxMenu is another wxMenu
    // then the class name will be "submenu"
    else if (
      className == "wxMenu" && (parent->GetClassName() == wxT("wxMenu") || parent->GetClassName() == wxT("submenu"))) {
        className = "submenu";
    }

    // "separator" is also ambiguous - could be a toolbar separator or a menu separator
    else if (className == "separator") {
        if (parent->GetClassName() == wxT("wxToolBar") || parent->GetClassName() == wxT("ToolBar")) {
            className = "toolSeparator";
        }
    }

    // replace "spacer" with "sizeritem" so it will be imported as a "sizeritem"
    // "sizeritem" is ambiguous - could also be a grid bag sizeritem
    else if (className == "spacer" || className == "sizeritem") {
        if (parent->GetClassName() == wxT("wxGridBagSizer")) {
            className = "gbsizeritem";
        } else {
            className = "sizeritem";
        }
    }

    PObjectBase object;
    PObjectInfo objInfo = m_objDb->GetObjectInfo(_WXSTR(className));
    if (objInfo) {
        IComponent* comp = objInfo->GetComponent();
        if (!comp) {
            wxLogError(_("No component found for class \"%s\", found on line %i."), _WXSTR(className), xrcObj->Row());
        } else {
            ticpp::Element* fbObj = comp->ImportFromXrc(xrcObj);
            if (!fbObj) {
                wxLogError(
                  _("ImportFromXrc returned NULL for class \"%s\", found on line %i."), _WXSTR(className),
                  xrcObj->Row());
            } else {
                object = m_objDb->CreateObject(fbObj, parent);
                if (!object) {
                    // Unable to create the object and add it to the parent - probably needs a sizer
                    PObjectBase newsizer = m_objDb->CreateObject("wxBoxSizer", parent);
                    if (newsizer) {
                        // It is possible the CreateObject returns an "item" containing the object, e.g. SizerItem or
                        // SplitterItem If that is the case, reassign "object" to the actual object
                        PObjectBase sizer = newsizer;
                        if (sizer->GetChildCount() > 0) {
                            sizer = sizer->GetChild(0);
                        }

                        if (sizer) {
                            object = m_objDb->CreateObject(fbObj, sizer);
                            if (object) {
                                parent->AddChild(newsizer);
                                newsizer->SetParent(parent);
                            }
                        }
                    }
                }

                if (!object) {
                    wxLogError(
                      wxT("CreateObject failed for class \"%s\", with parent \"%s\", found on line %i"),
                      _WXSTR(className), parent->GetClassName(), xrcObj->Row());
                } else {
                    // It is possible the CreateObject returns an "item" containing the object, e.g. SizerItem or
                    // SplitterItem If that is the case, reassign "object" to the actual object
                    if (object && object->GetChildCount() > 0)
                        object = object->GetChild(0);

                    if (object) {
                        // Recursively import the children
                        ticpp::Element* element = xrcObj->FirstChildElement("object", false);
                        while (element) {
                            GetObject(element, object);
                            element = element->NextSiblingElement("object", false);
                        }
                    }
                }
            }
        }
    } else {
        // Create a wxPanel to represent unknown classes
        object = m_objDb->CreateObject("wxPanel", parent);
        if (object) {
            parent->AddChild(object);
            object->SetParent(parent);
            wxLogError(
              wxT("Unknown class \"%s\" found on line %i, replaced with a wxPanel"), _WXSTR(className), xrcObj->Row());
        } else {
            wxString msg(wxString::Format(
              wxT("Unknown class \"%s\" found on line %i, and could not replace with a wxPanel as child of \"%s:%s\""),
              _WXSTR(className), xrcObj->Row(), parent->GetPropertyAsString(wxT("name")), parent->GetClassName()));

            wxLogError(msg);
        }
    }

    return object;
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
