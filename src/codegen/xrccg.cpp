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

#include "xrccg.h"

#include <ticpp.h>

#include <common/xmlutils.h>

#include "codegen/codewriter.h"
#include "model/objectbase.h"
#include "utils/typeconv.h"


void XrcCodeGenerator::SetWriter(PCodeWriter cw)
{
    m_cw = cw;
}


bool XrcCodeGenerator::GenerateCode(PObjectBase project)
{
    m_xrc.Clear();
    auto* prolog = m_xrc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"");
    auto* root = m_xrc.NewElement("resource");
    XMLUtils::SetAttribute(root, "xmlns", "http://www.wxwidgets.org/wxxrc");
    XMLUtils::SetAttribute(root, "version", "2.5.3.0");
    m_xrc.InsertEndChild(prolog);
    m_xrc.InsertEndChild(root);

    // If the given argument is a project, generate code for all its children.
    // Otherwise assume it is an object and generate code only for it.
    m_contextMenus2.clear();
    if (project->GetClassName() == "Project") {
        for (std::size_t i = 0; i < project->GetChildCount(); ++i) {
            auto* childXrc = GetElement(project->GetChild(i), static_cast<tinyxml2::XMLElement*>(nullptr));
            if (childXrc) {
                root->InsertEndChild(childXrc);
            }
        }
    } else {
        auto* objectXrc = GetElement(project, static_cast<tinyxml2::XMLElement*>(nullptr));
        if (objectXrc) {
            root->InsertEndChild(objectXrc);
        }
    }
    // Generate context menus as top level menus
    for (auto& contextMenu : m_contextMenus2) {
        root->InsertEndChild(contextMenu);
    }
    m_contextMenus2.clear();

    m_cw->Clear();
    auto code = XMLUtils::SaveXMLString(m_xrc);
    m_cw->Write(code);

    return true;
}


ticpp::Element* XrcCodeGenerator::GetElement(PObjectBase obj, ticpp::Element* parent)
{
    ticpp::Element* element = NULL;

    IComponent* comp = obj->GetObjectInfo()->GetComponent();

    if (comp)
        element = comp->ExportToXrc(obj.get());

    if (element) {
        std::string class_name = element->GetAttribute("class");
        if (class_name == "__dummyitem__") {
            delete element;
            element = NULL;

            if (obj->GetChildCount() > 0)
                element = GetElement(obj->GetChild(0));

            return element;
        } else if (class_name == "spacer") {
            // Dirty hack to replace the containing sizeritem with the spacer
            if (parent) {
                parent->SetAttribute("class", "spacer");
                for (ticpp::Node* child = element->FirstChild(false); child; child = child->NextSibling(false)) {
                    parent->LinkEndChild(child->Clone().release());
                }
                delete element;
                return NULL;
            }

        } else if (class_name == "wxFrame") {
            // Dirty hack to prevent sizer generation directly under a wxFrame
            // If there is a sizer, the size property of the wxFrame is ignored
            // when loading the xrc file at runtime
            if (obj->GetPropertyAsInteger(_("xrc_skip_sizer")) != 0) {
                for (unsigned int i = 0; i < obj->GetChildCount(); i++) {
                    ticpp::Element* aux = NULL;

                    PObjectBase child = obj->GetChild(i);
                    if (child->GetObjectInfo()->IsSubclassOf(wxT("sizer"))) {
                        if (child->GetChildCount() == 1) {
                            PObjectBase sizeritem = child->GetChild(0);
                            if (sizeritem) {
                                aux = GetElement(sizeritem->GetChild(0), element);
                            }
                        }
                    }

                    if (!aux) {
                        aux = GetElement(child, element);
                    }

                    if (aux) {
                        element->LinkEndChild(aux);
                        delete aux;
                    }
                }
                return element;
            }
        } else if (class_name == "wxMenu") {
            if (parent) {
                // Do not generate context menus assigned to forms or widgets
                std::string parent_name = parent->GetAttribute("class");
                if ((parent_name != "wxMenuBar") && (parent_name != "wxMenu")) {
                    // insert context menu into vector for delayed processing (context menus will be
                    // generated as top-level menus)
                    for (unsigned int i = 0; i < obj->GetChildCount(); i++) {
                        ticpp::Element* aux = GetElement(obj->GetChild(i), element);
                        if (aux) {
                            element->LinkEndChild(aux);
                            delete aux;
                        }
                    }

                    m_contextMenus.push_back(element);
                    return nullptr;
                }
            }
        } else if (class_name == "wxCollapsiblePane") {
            if (obj->GetChildCount() > 0) {
                ticpp::Element* aux = new ticpp::Element("object");
                aux->SetAttribute("class", "panewindow");

                ticpp::Element* child = GetElement(obj->GetChild(0), aux);

                aux->LinkEndChild(child);
                element->LinkEndChild(aux);

                delete aux;
                delete child;
            }

            return element;
        }

        for (unsigned int i = 0; i < obj->GetChildCount(); i++) {
            ticpp::Element* aux = GetElement(obj->GetChild(i), element);
            if (aux) {
                element->LinkEndChild(aux);
                delete aux;
            }
        }
    } else {
        if (obj->GetObjectTypeName() != wxT("nonvisual")) {
            // The component does not XRC
            element = new ticpp::Element("object");
            element->SetAttribute("class", "unknown");
            element->SetAttribute("name", _STDSTR(obj->GetPropertyAsString(_("name"))));
        }
    }

    return element;
}

tinyxml2::XMLElement* XrcCodeGenerator::GetElement(PObjectBase object, tinyxml2::XMLElement* parent)
{
    auto* objectXrc = m_xrc.NewElement("");

    if (auto* objectComponent = object->GetObjectInfo()->GetComponent(); !(objectComponent && objectComponent->ExportToXrc(objectXrc, object.get()))) {
        // Return a special element for unknown objects
        if (object->GetObjectTypeName() != "nonvisual") {
            objectXrc->SetName("object");
            XMLUtils::SetAttribute(objectXrc, "class", "unknown");
            XMLUtils::SetAttribute(objectXrc, "name", object->GetPropertyAsString("name"));

            return objectXrc;
        }

        // Don't return an element for nonvisual objects
        m_xrc.DeleteNode(objectXrc);

        return nullptr;
    }

    if (auto xrcClass = XMLUtils::StringAttribute(objectXrc, "class"); xrcClass == "__dummyitem__") {
        // FIXME: What is this class?
        m_xrc.DeleteNode(objectXrc);
        if (object->GetChildCount() > 0) {
            return GetElement(object->GetChild(0), static_cast<tinyxml2::XMLElement*>(nullptr));
        }

        return nullptr;
    } else if (xrcClass == "spacer") {
        // FIXME: Hack to replace the containing sizeritem with the spacer
        if (parent) {
            XMLUtils::SetAttribute(parent, "class", "spacer");
            for (auto* childXrc = objectXrc->FirstChild(); childXrc; childXrc = childXrc->NextSibling()) {
                parent->InsertEndChild(childXrc);
            }
            m_xrc.DeleteNode(objectXrc);

            return nullptr;
        }
    } else if (xrcClass == "wxFrame") {
        // FIXME: Hack to prevent sizer generation directly under a wxFrame.
        //        If there is a sizer, the size property of the wxFrame is ignored when loading the xrc file at runtime.
        // FIXME: Why only wxFrame, doesn't this apply to all top level windows?
        if (object->GetPropertyAsInteger("xrc_skip_sizer") != 0) {
            for (std::size_t i = 0; i < object->GetChildCount(); ++i) {
                tinyxml2::XMLElement* childXrc = nullptr;

                auto childObject = object->GetChild(i);
                // TODO: Why needs the child count be exactly one? Other parts of the code are not so strict.
                if (childObject->GetObjectInfo()->IsSubclassOf("sizer") && childObject->GetChildCount() == 1) {
                    auto sizerItemObject = childObject->GetChild(0);
                    childXrc = GetElement(sizerItemObject->GetChild(0), objectXrc);
                }
                if (!childXrc) {
                    childXrc = GetElement(childObject, objectXrc);
                }

                if (childXrc) {
                    objectXrc->InsertEndChild(childXrc);
                }
            }

            return objectXrc;
        }
    } else if (xrcClass == "wxMenu") {
        // Do not generate context menus assigned to forms or widgets
        if (parent) {
            auto parentXrcClass = XMLUtils::StringAttribute(parent, "class");
            if (parentXrcClass != "wxMenuBar" && parentXrcClass != "wxMenu") {
                // Process the children and record context menu for delayed processing, context menus will be generated as top level menus
                for (std::size_t i = 0; i < object->GetChildCount(); ++i) {
                    auto* childXrc = GetElement(object->GetChild(i), objectXrc);
                    if (childXrc) {
                        objectXrc->InsertEndChild(childXrc);
                    }
                }
                m_contextMenus2.emplace_back(objectXrc);

                return nullptr;
            }
        }
    } else if (xrcClass == "wxCollapsiblePane") {
        if (object->GetChildCount() > 0) {
            auto* paneWindowXrc = objectXrc->InsertNewChildElement("object");
            XMLUtils::SetAttribute(paneWindowXrc, "class", "panewindow");

            auto* childXrc = GetElement(object->GetChild(0), paneWindowXrc);
            paneWindowXrc->InsertEndChild(childXrc);
        }

        return objectXrc;
    }

    for (std::size_t i = 0; i < object->GetChildCount(); ++i) {
        auto* childXrc = GetElement(object->GetChild(i), objectXrc);
        if (childXrc) {
            objectXrc->InsertEndChild(childXrc);
        }
    }

    return objectXrc;
}
