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

#include "database.h"

#include <wx/dir.h>
#include <wx/filename.h>

#include <common/xmlutils.h>

#include "model/objectbase.h"
#include "rad/bitmaps.h"
#include "rad/wxfbmanager.h"
#include "utils/debug.h"
#include "utils/stringutils.h"
#include "utils/typeconv.h"
#include "utils/wxfbexception.h"

//#define DEBUG_PRINT(x) cout << x

#define OBJINFO_TAG "objectinfo"
#define CODEGEN_TAG "codegen"
#define TEMPLATE_TAG "template"
#define NAME_TAG "name"
#define DESCRIPTION_TAG "help"
#define CUSTOM_EDITOR_TAG "editor"
#define PROPERTY_TAG "property"
#define CHILD_TAG "child"
#define EVENT_TAG "event"
#define EVENT_CLASS_TAG "class"
#define CATEGORY_TAG "category"
#define OBJECT_TAG "object"
#define CLASS_TAG "class"
#define PACKAGE_TAG "package"
#define PKGDESC_TAG "desc"
#define PRGLANG_TAG "language"
#define ICON_TAG "icon"
#define SMALL_ICON_TAG "smallIcon"
#define EXPANDED_TAG "expanded"
#define WXVERSION_TAG "wxversion"


ObjectPackage::ObjectPackage(wxString name, wxString desc, wxBitmap icon)
{
    m_name = name;
    m_desc = desc;
    m_icon = icon;
}

PObjectInfo ObjectPackage::GetObjectInfo(unsigned int idx)
{
    assert(idx < m_objs.size());
    return m_objs[idx];
}

void ObjectPackage::AppendPackage(PObjectPackage package)
{
    m_objs.insert(m_objs.end(), package->m_objs.begin(), package->m_objs.end());
}

///////////////////////////////////////////////////////////////////////////////

ObjectDatabase::ObjectDatabase()
{
    // InitObjectTypes();
    //   InitWidgetTypes();
    InitPropertyTypes();
}

ObjectDatabase::~ObjectDatabase()
{
    for (const auto& plugin : m_pluginLibraries) {
        if (plugin.second.freeComponentLibrary && plugin.second.componentLibrary) {
            plugin.second.freeComponentLibrary(plugin.second.componentLibrary);
        }
    }
}

PObjectInfo ObjectDatabase::GetObjectInfo(wxString class_name)
{
    PObjectInfo info;
    ObjectInfoMap::iterator it = m_objs.find(class_name);

    if (it != m_objs.end()) {
        info = it->second;
    }

    return info;
}

PObjectPackage ObjectDatabase::GetPackage(unsigned int idx)
{
    assert(idx < m_pkgs.size());

    return m_pkgs[idx];
}


/**
 * @todo La herencia de propiedades ha de ser de forma recursiva.
 */

PObjectBase ObjectDatabase::NewObject(PObjectInfo obj_info)
{
    PObjectBase object;

    // Llagados aquí el objeto se crea seguro...
    object = PObjectBase(new ObjectBase(obj_info->GetClassName()));
    object->SetObjectTypeName(obj_info->GetObjectTypeName());  // *FIXME*

    object->SetObjectInfo(obj_info);

    PPropertyInfo prop_info;
    PEventInfo event_info;
    PObjectInfo class_info = obj_info;

    unsigned int base = 0;

    while (class_info) {
        unsigned int i;
        for (i = 0; i < class_info->GetPropertyCount(); i++) {
            prop_info = class_info->GetPropertyInfo(i);

            PProperty property(new Property(prop_info, object));

            // Set the default value, either from the property info, or an override from this class
            wxString defaultValue = prop_info->GetDefaultValue();
            if (base > 0) {
                wxString defaultValueTemp = obj_info->GetBaseClassDefaultPropertyValue(base - 1, prop_info->GetName());
                if (!defaultValueTemp.empty()) {
                    defaultValue = defaultValueTemp;
                }
            }
            property->SetValue(defaultValue);

            // Las propiedades están implementadas con una estructura "map",
            // ello implica que no habrá propiedades duplicadas.
            // En otro caso habrá que asegurarse de que dicha propiedad
            // no existe.
            // Otra cosa importante, es que el orden en que se insertan
            // las propiedades, de abajo-arriba, esto permite que se pueda redefir
            // alguna propiedad.
            object->AddProperty(property);
        }

        for (i = 0; i < class_info->GetEventCount(); i++) {
            event_info = class_info->GetEventInfo(i);
            PEvent event(new Event(event_info, object));
            // notice that for event there isn't a default value on its creation
            // because there is not handler at the moment
            object->AddEvent(event);
        }

        class_info = (base < obj_info->GetBaseClassCount() ? obj_info->GetBaseClass(base++) : PObjectInfo());
    }

    // si el objeto tiene la propiedad name (reservada para el nombre del
    // objeto) le añadimos el contador para no repetir nombres.

    obj_info->IncrementInstanceCount();

    unsigned int ins = obj_info->GetInstanceCount();
    PProperty pname = object->GetProperty(wxT(NAME_TAG));
    if (pname)
        pname->SetValue(pname->GetValue() + StringUtils::IntToStr(ins));

    return object;
}


int ObjectDatabase::CountChildrenWithSameType(PObjectBase parent, PObjectType type)
{
    unsigned int count = 0;
    unsigned int numChildren = parent->GetChildCount();
    for (unsigned int i = 0; i < numChildren; i++) {
        if (type == parent->GetChild(i)->GetObjectInfo()->GetObjectType())
            count++;
    }

    return count;
}

int ObjectDatabase::CountChildrenWithSameType(PObjectBase parent, const std::set<PObjectType>& types)
{
    unsigned int count = 0;
    unsigned int numChildren = parent->GetChildCount();
    for (unsigned int i = 0; i < numChildren; ++i) {
        if (types.find(parent->GetChild(i)->GetObjectInfo()->GetObjectType()) != types.end())
            ++count;
    }

    return count;
}

/**
* Crea una instancia de classname por debajo de parent.
* La función realiza la comprobación de tipos para crear el objeto:
* - Comprueba si el tipo es un tipo-hijo válido de "parent", en cuyo caso
*   se comprobará también que el número de hijos del mismo tipo no sobrepase
el máximo definido. El objeto no se crea si supera el máximo permitido.
* - Si el tipo-hijo no se encuentra entre los definidos para el tipo de
*   "parent" se intentará crearlo como hijo de alguno de los tipos hijos con el
*   flag item a "1". Para ello va recorriendo todos los tipos con flag item,
*   si no puede crear el objeto, bien por que el tipo no es válido o porque
*   sobrepasa el máximo permitido si intenta con el siguiente hasta que no queden
*   más.
*
* Nota: quizá sea conveniente que el método cree el objeto sin enlazarlo
*       en el árbol, para facilitar el undo-redo.
*/
PObjectBase ObjectDatabase::CreateObject(const wxString& classname, PObjectBase parent)
{
    PObjectBase object;
    PObjectInfo objInfo = GetObjectInfo(classname);

    if (!objInfo) {
        THROW_WXFBEX(
          wxT("Unknown Object Type: ") << classname
                                       << wxT("\n") wxT("The most likely causes are that this copy of wxFormBuilder is "
                                                        "out of date, or that there is a plugin missing.\n")
                                            wxT("Please check at http://www.wxFormBuilder.org")
                                       << wxT("\n"))
    }

    PObjectType objType = objInfo->GetObjectType();

    if (parent) {
        // Comprobamos si el tipo es válido
        PObjectType parentType = parent->GetObjectInfo()->GetObjectType();

        // AUI
        bool aui = false;
        if (parentType->GetName() == wxT("form")) {
            aui = parent->GetPropertyAsInteger(wxT("aui_managed")) != 0;
        }

        int max = parentType->FindChildType(objType, aui);

        // FIXME! Esto es un parche para evitar crear los tipos menubar,statusbar y
        // toolbar en un form que no sea wxFrame.
        // Hay que modificar el conjunto de tipos para permitir tener varios tipos
        // de forms (como childType de project), pero hay mucho código no válido
        // para forms que no sean de tipo "form". Dicho de otra manera, hay
        // código que dependen del nombre del tipo, cosa que hay que evitar.
        if (
          parentType->GetName() == wxT("form") && parent->GetClassName() != wxT("Frame") &&
          (objType->GetName() == wxT("statusbar") || objType->GetName() == wxT("menubar") ||
           objType->GetName() == wxT("ribbonbar") || objType->GetName() == wxT("toolbar")))
            return PObjectBase();  // tipo no válido

        // No menu dropdown for wxToolBar until wx 2.9 :(
        if (parentType->GetName() == wxT("tool")) {
            PObjectBase gParent = parent->GetParent();
            if ((gParent->GetClassName() == wxT("wxToolBar")) && (objType->GetName() == wxT("menu")))
                return PObjectBase();  // not a valid type
        }

        if (max != 0)  // tipo válido
        {
            bool create = true;

            // we check the number of instances
            int count;
            if (objType == GetObjectType(wxT("sizer")) || objType == GetObjectType(wxT("gbsizer"))) {
                count = CountChildrenWithSameType(parent, {GetObjectType(wxT("sizer")), GetObjectType(wxT("gbsizer"))});
            } else {
                count = CountChildrenWithSameType(parent, objType);
            }

            if (max > 0 && count >= max)
                create = false;

            if (create)
                object = NewObject(objInfo);
        } else  // max == 0
        {
            // el tipo no es válido, vamos a comprobar si podemos insertarlo
            // como hijo de un "item"
            bool created = false;
            for (unsigned int i = 0; !created && i < parentType->GetChildTypeCount(); i++) {
                PObjectType childType = parentType->GetChildType(i);
                int childMax = childType->FindChildType(objType, aui);

                if (childType->IsItem() && childMax != 0) {
                    childMax = parentType->FindChildType(childType, aui);

                    // si el tipo es un item y además el tipo del objeto a crear
                    // puede ser hijo del tipo del item vamos a intentar crear la
                    // instancia del item para crear el objeto como hijo de este
                    if (childMax < 0 || CountChildrenWithSameType(parent, childType) < childMax) {
                        // No hay problemas para crear el item debajo de parent
                        PObjectBase item = NewObject(GetObjectInfo(childType->GetName()));

                        // PObjectBase obj = CreateObject(classname,item);
                        PObjectBase obj = NewObject(objInfo);

                        // la siguiente condición debe cumplirse siempre
                        // ya que un item debe siempre contener a otro objeto
                        if (obj) {
                            // enlazamos item y obj
                            item->AddChild(obj);
                            obj->SetParent(item);

                            // sizeritem es un tipo de objeto reservado, para que el uso sea
                            // más práctico se asignan unos valores por defecto en función
                            // del tipo de objeto creado
                            if (item->GetObjectInfo()->IsSubclassOf(wxT("sizeritembase")))
                                SetDefaultLayoutProperties(item);

                            object = item;
                            created = true;
                        } else
                            wxLogError(wxT("Review your definitions file (objtypes.xml)"));
                    }
                }
            }
        }
        ///////////////////////////////////////////////////////////////////////
        // Nota: provisionalmente vamos a enlazar el objeto al padre pero
        //       esto debería hacerse fuera para poder implementar el Undo-Redo
        ///////////////////////////////////////////////////////////////////////
        // if (object)
        //{
        //   parent->AddChild(object);
        //   object->SetParent(parent);
        // }
    } else  // parent == NULL;
    {
        object = NewObject(objInfo);
    }

    return object;
}

PObjectBase ObjectDatabase::CopyObject(PObjectBase obj)
{
    assert(obj);

    PObjectInfo objInfo = obj->GetObjectInfo();

    PObjectBase copyObj = NewObject(objInfo);  // creamos la copia
    assert(copyObj);

    // copiamos las propiedades
    unsigned int i;
    unsigned int count = obj->GetPropertyCount();
    for (i = 0; i < count; i++) {
        PProperty objProp = obj->GetProperty(i);
        assert(objProp);

        PProperty copyProp = copyObj->GetProperty(objProp->GetName());
        assert(copyProp);

        wxString propValue = objProp->GetValue();
        copyProp->SetValue(propValue);
    }

    // ...and the event handlers
    count = obj->GetEventCount();
    for (i = 0; i < count; i++) {
        PEvent event = obj->GetEvent(i);
        PEvent copyEvent = copyObj->GetEvent(event->GetName());
        copyEvent->SetValue(event->GetValue());
    }

    // creamos recursivamente los hijos
    count = obj->GetChildCount();
    for (i = 0; i < count; i++) {
        PObjectBase childCopy = CopyObject(obj->GetChild(i));
        copyObj->AddChild(childCopy);
        childCopy->SetParent(copyObj);
    }

    return copyObj;
}

void ObjectDatabase::SetDefaultLayoutProperties(PObjectBase sizeritem)
{
    if (!sizeritem->GetObjectInfo()->IsSubclassOf(wxT("sizeritembase"))) {
        LogDebug(wxT("SetDefaultLayoutProperties expects a subclass of sizeritembase"));
        return;
    }

    PObjectBase child = sizeritem->GetChild(0);
    PObjectInfo childInfo = child->GetObjectInfo();
    wxString obj_type = child->GetObjectTypeName();

    PProperty proportion = sizeritem->GetProperty(wxT("proportion"));

    if (
      childInfo->IsSubclassOf(wxT("sizer")) || childInfo->IsSubclassOf(wxT("gbsizer")) || obj_type == wxT("splitter") ||
      childInfo->GetClassName() == wxT("spacer")) {
        if (proportion) {
            proportion->SetValue(wxT("1"));
        }
        sizeritem->GetProperty(wxT("flag"))->SetValue(wxT("wxEXPAND"));
    } else if (childInfo->GetClassName() == wxT("wxStaticLine")) {
        sizeritem->GetProperty(wxT("flag"))->SetValue(wxT("wxEXPAND | wxALL"));
    } else if (childInfo->GetClassName() == wxT("wxToolBar")) {
        sizeritem->GetProperty(wxT("flag"))->SetValue(wxT("wxEXPAND"));
    } else if (obj_type == wxT("widget") || obj_type == wxT("statusbar")) {
        if (proportion) {
            proportion->SetValue(wxT("0"));
        }
        sizeritem->GetProperty(wxT("flag"))->SetValue(wxT("wxALL"));
    } else if (
      obj_type == wxT("notebook") || obj_type == wxT("flatnotebook") || obj_type == wxT("listbook") ||
      obj_type == wxT("simplebook") || obj_type == wxT("choicebook") || obj_type == wxT("auinotebook") ||
      obj_type == wxT("treelistctrl") || obj_type == wxT("expanded_widget") || obj_type == wxT("container")) {
        if (proportion) {
            proportion->SetValue(wxT("1"));
        }
        sizeritem->GetProperty(wxT("flag"))->SetValue(wxT("wxEXPAND | wxALL"));
    }
}

void ObjectDatabase::ResetObjectCounters()
{
    ObjectInfoMap::iterator it;
    for (it = m_objs.begin(); it != m_objs.end(); it++) { it->second->ResetInstanceCount(); }
}

///////////////////////////////////////////////////////////////////////

PObjectBase ObjectDatabase::CreateObject(const tinyxml2::XMLElement* object, PObjectBase parentObject)
{
    auto className = XMLUtils::StringAttribute(object, CLASS_TAG);
    auto baseObject = CreateObject(className, parentObject);
    // CreateObject might return an "item" that contains the acutal object, e.g. sizeritem or splitteritem.
    // In that case, determine the real object and use that one.
    // TODO: What is the proper way to do this?
    auto targetObject = (baseObject && baseObject->GetChildCount() > 0 ? baseObject->GetChild(0) : baseObject);
    if (!targetObject) {
        return baseObject;
    }

    auto expanded = object->BoolAttribute(EXPANDED_TAG, true);
    targetObject->SetExpanded(expanded);

    for (const auto* property = object->FirstChildElement(PROPERTY_TAG); property; property = property->NextSiblingElement(PROPERTY_TAG)) {
        auto propertyName = XMLUtils::StringAttribute(property, NAME_TAG);
        auto propertyValue = XMLUtils::GetText(property);
        auto propertyObject = targetObject->GetProperty(propertyName);

        if (propertyObject) {
            propertyObject->SetValue(propertyValue);
        } else {
            // TODO: The previous code reported the error only if the value was non-empty, why?
            wxLogError(
                _(
                    "The property \"%s\" of class \"%s\" is not supported by this version of wxFormBuilder.\n"
                    "If your project file was just converted from an older version, the conversion is not complete.\n"
                    "Otherwise, this project file is from a newer version of wxFormBuilder.\n\n"
                    "The value of the property is: \"%s\"\n\n"
                    "If you save this project, YOU WILL LOSE DATA!"
                ),
                propertyName, className, propertyValue
            );
        }
    }

    for (const auto* event = object->FirstChildElement(EVENT_TAG); event; event = event->NextSiblingElement(EVENT_TAG)) {
        auto eventName = XMLUtils::StringAttribute(event, NAME_TAG);
        auto eventValue = XMLUtils::GetText(event);
        auto eventObject = targetObject->GetEvent(eventName);

        // TODO: Why does the previous code not report an error for unknown events?
        if (eventObject) {
            eventObject->SetValue(eventValue);
        }
    }

    if (parentObject) {
        parentObject->AddChild(baseObject);
        baseObject->SetParent(parentObject);
    }

    for (const auto* child = object->FirstChildElement(OBJECT_TAG); child; child = child->NextSiblingElement(OBJECT_TAG)) {
        CreateObject(child, targetObject);
    }

    return baseObject;
}

//////////////////////////////

bool IncludeInPalette(wxString /*type*/)
{
    return true;
}

void ObjectDatabase::LoadPlugins(PwxFBManager manager)
{
    // Load some default templates
    LoadCodeGen(m_xmlPath + wxT("properties.cppcode"));
    LoadCodeGen(m_xmlPath + wxT("properties.pythoncode"));
    LoadCodeGen(m_xmlPath + wxT("properties.luacode"));
    LoadCodeGen(m_xmlPath + wxT("properties.phpcode"));
    LoadPackage(m_xmlPath + wxT("default.xml"), m_iconPath);
    LoadCodeGen(m_xmlPath + wxT("default.cppcode"));
    LoadCodeGen(m_xmlPath + wxT("default.pythoncode"));
    LoadCodeGen(m_xmlPath + wxT("default.luacode"));
    LoadCodeGen(m_xmlPath + wxT("default.phpcode"));

    // Map to temporarily hold plugins.
    // Used to both set page order and to prevent two plugins with the same name.
    typedef std::map<wxString, PObjectPackage> PackageMap;
    PackageMap packages;

    // Open plugins directory for iteration
    if (!wxDir::Exists(m_pluginPath)) {
        return;
    }

    wxDir pluginsDir(m_pluginPath);
    if (!pluginsDir.IsOpened()) {
        return;
    }

    // Iterate through plugin directories and load the package from the xml subdirectory
    wxString pluginDirName;
    bool moreDirectories = pluginsDir.GetFirst(&pluginDirName, wxEmptyString, wxDIR_DIRS | wxDIR_HIDDEN);
    while (moreDirectories) {
        // Iterate through .xml files in the xml directory
        wxString nextPluginPath = m_pluginPath + pluginDirName;
        wxString nextPluginXmlPath = nextPluginPath + wxFILE_SEP_PATH + wxT("xml");
        wxString nextPluginIconPath = nextPluginPath + wxFILE_SEP_PATH + wxT("icons");
        if (wxDir::Exists(nextPluginPath)) {
            if (wxDir::Exists(nextPluginXmlPath)) {
                wxDir pluginXmlDir(nextPluginXmlPath);
                if (pluginXmlDir.IsOpened()) {
                    std::map<wxString, PObjectPackage> packagesToSetup;
                    wxString packageXmlFile;
                    bool moreXmlFiles =
                      pluginXmlDir.GetFirst(&packageXmlFile, wxT("*.xml"), wxDIR_FILES | wxDIR_HIDDEN);
                    while (moreXmlFiles) {
                        try {
                            wxFileName nextXmlFile(nextPluginXmlPath + wxFILE_SEP_PATH + packageXmlFile);
                            if (!nextXmlFile.IsAbsolute()) {
                                nextXmlFile.MakeAbsolute();
                            }

                            PObjectPackage package = LoadPackage(nextXmlFile.GetFullPath(), nextPluginIconPath);
                            if (package) {
                                // Load all packages, then setup all packages
                                // this allows multiple packages sharing one library
                                packagesToSetup[nextXmlFile.GetFullPath()] = package;
                            }
                        } catch (wxFBException& ex) {
                            wxLogError(ex.what());
                        }
                        moreXmlFiles = pluginXmlDir.GetNext(&packageXmlFile);
                    }

                    std::map<wxString, PObjectPackage>::iterator packageIt;
                    for (packageIt = packagesToSetup.begin(); packageIt != packagesToSetup.end(); ++packageIt) {
                        // Setup the inheritance for base classes
                        wxFileName fullNextPluginPath(nextPluginPath);
                        if (!fullNextPluginPath.IsAbsolute()) {
                            fullNextPluginPath.MakeAbsolute();
                        }
                        wxFileName xmlFileName(packageIt->first);
                        try {
                            SetupPackage(xmlFileName.GetFullPath(), fullNextPluginPath.GetFullPath(), manager);

                            // Load the C++ code templates
                            xmlFileName.SetExt(wxT("cppcode"));
                            LoadCodeGen(xmlFileName.GetFullPath());

                            // Load the Python code templates
                            xmlFileName.SetExt(wxT("pythoncode"));
                            LoadCodeGen(xmlFileName.GetFullPath());

                            // Load the PHP code templates
                            xmlFileName.SetExt(wxT("phpcode"));
                            LoadCodeGen(xmlFileName.GetFullPath());

                            // Load the Lua code templates
                            xmlFileName.SetExt(wxT("luacode"));
                            LoadCodeGen(xmlFileName.GetFullPath());

                            std::pair<PackageMap::iterator, bool> addedPackage = packages.insert(
                              PackageMap::value_type(packageIt->second->GetPackageName(), packageIt->second));
                            if (!addedPackage.second) {
                                addedPackage.first->second->AppendPackage(packageIt->second);
                                LogDebug("Merged plugins named \"" + packageIt->second->GetPackageName() + "\"");
                            }

                        } catch (wxFBException& ex) {
                            wxLogError(ex.what());
                        }
                    }
                }
            }
        }

        moreDirectories = pluginsDir.GetNext(&pluginDirName);
    }

    // Add packages to final data structure
    m_pkgs.reserve(packages.size());
    for (auto& package : packages) { m_pkgs.push_back(package.second); }
}

void ObjectDatabase::SetupPackage(const wxString& file, [[maybe_unused]] const wxString& path, PwxFBManager manager)
{
    auto doc = XMLUtils::LoadXMLFile(file, true);
    if (!doc) {
        THROW_WXFBEX(wxString::Format(_("%s: Failed to open file"), file))
    }
    if (doc->Error()) {
        THROW_WXFBEX(doc->ErrorStr())
    }
    const auto* root = doc->FirstChildElement(PACKAGE_TAG);
    if (!root) {
        THROW_WXFBEX(wxString::Format(_("%s: Invalid root node"), file))
    }

    if (auto lib = XMLUtils::StringAttribute(root, "lib"); !lib.empty()) {
#ifdef __WINDOWS__
        // On Windows the plugin libraries reside in the directory of the plugin resources,
        // construct a relative path to that location to be able to find them with the default
        // search algorithm
        lib = "plugins/" + lib + "/" + lib;
#endif
        if (auto pluginLibrary = m_pluginLibraries.find(lib); pluginLibrary == m_pluginLibraries.end()) {
            ImportComponentLibrary(lib, manager);
        }
    }

    for (const auto* object = root->FirstChildElement(OBJINFO_TAG); object;
         object = object->NextSiblingElement(OBJINFO_TAG)) {
        // Skip components that are not supported by the wxWidgets version this application is linked against
        if (auto wxVersion = object->IntAttribute(WXVERSION_TAG, 0); wxVersion > wxVERSION_NUMBER) {
            continue;
        }

        auto className = XMLUtils::StringAttribute(object, CLASS_TAG);
        if (className.empty()) {
            THROW_WXFBEX(wxString::Format(_("%s: Empty class name"), file))
        }
        auto classInfo = GetObjectInfo(className);
        for (const auto* base = object->FirstChildElement("inherits"); base;
             base = base->NextSiblingElement("inherits")) {
            auto baseName = XMLUtils::StringAttribute(base, CLASS_TAG);
            if (baseName.empty()) {
                THROW_WXFBEX(wxString::Format(_("%s: Empty base class name for class \"%s\""), file, className))
            }
            auto baseInfo = GetObjectInfo(baseName);
            if (!(classInfo && baseInfo)) {
                continue;
            }

            auto baseIndex = classInfo->AddBaseClass(baseInfo);
            for (const auto* inheritedProperty = base->FirstChildElement("property"); inheritedProperty;
                 inheritedProperty = inheritedProperty->NextSiblingElement("property")) {
                auto propertyName = XMLUtils::StringAttribute(inheritedProperty, NAME_TAG);
                if (propertyName.empty()) {
                    THROW_WXFBEX(wxString::Format(_("%s: Empty property name for base class \"%s\""), file, baseName))
                }
                auto propertyValue = XMLUtils::GetText(inheritedProperty);

                classInfo->AddBaseClassDefaultPropertyValue(baseIndex, propertyName, propertyValue);
            }
        }

        // Add the "C++" base class, predefined for the components and widgets
        if (auto typeName = classInfo->GetObjectTypeName(); HasCppProperties(typeName)) {
            if (auto cppInfo = GetObjectInfo("C++"); cppInfo) {
                auto baseIndex = classInfo->AddBaseClass(cppInfo);
                // Set default visibility of "non-objects" elements to none
                // FIXME: This shouldn't be done in code, also this list isn't complete
                if (typeName == "sizer" || typeName == "gbsizer" || typeName == "menuitem") {
                    classInfo->AddBaseClassDefaultPropertyValue(baseIndex, "permission", "none");
                }
            }
        }
    }
}

bool ObjectDatabase::HasCppProperties(wxString type)
{
    return (
      type == wxT("notebook") || type == wxT("flatnotebook") || type == wxT("listbook") || type == wxT("simplebook") ||
      type == wxT("choicebook") || type == wxT("auinotebook") || type == wxT("widget") ||
      type == wxT("expanded_widget") || type == wxT("propgrid") || type == wxT("propgridman") ||
      type == wxT("statusbar") || type == wxT("component") || type == wxT("container") || type == wxT("menubar") ||
      type == wxT("menu") || type == wxT("menuitem") || type == wxT("submenu") || type == wxT("toolbar") ||
      type == wxT("ribbonbar") || type == wxT("ribbonpage") || type == wxT("ribbonpanel") ||
      type == wxT("ribbonbuttonbar") || type == wxT("ribbonbutton") || type == wxT("ribbondropdownbutton") ||
      type == wxT("ribbonhybridbutton") || type == wxT("ribbontogglebutton") || type == wxT("ribbontoolbar") ||
      type == wxT("ribbontool") || type == wxT("ribbondropdowntool") || type == wxT("ribbonhybridtool") ||
      type == wxT("ribbontoggletool") || type == wxT("ribbongallery") || type == wxT("ribbongalleryitem") ||
      type == wxT("dataviewctrl") || type == wxT("dataviewtreectrl") || type == wxT("dataviewlistctrl") ||
      type == wxT("dataviewlistcolumn") || type == wxT("dataviewcolumn") || type == wxT("tool") ||
      type == wxT("splitter") || type == wxT("treelistctrl") || type == wxT("sizer") || type == wxT("nonvisual") ||
      type == wxT("gbsizer") || type == wxT("propgriditem") || type == wxT("propgridpage") || type == wxT("gbsizer") ||
      type == wxT("wizardpagesimple"));
}

void ObjectDatabase::LoadCodeGen(const wxString& file)
{
    auto doc = XMLUtils::LoadXMLFile(file, true);
    if (!doc) {
        wxLogError(_("%s: Failed to open file"), file);

        return;
    }
    if (doc->Error()) {
        wxLogError(doc->ErrorStr());

        return;
    }
    const auto* root = doc->FirstChildElement("codegen");
    if (!root) {
        wxLogError(_("%s: Invalid root node"), file);

        return;
    }

    auto language = XMLUtils::StringAttribute(root, "language");
    if (language.empty()) {
        wxLogError(_("%s: Empty code generation language"), file);

        return;
    }
    for (const auto* itemTemplates = root->FirstChildElement("templates"); itemTemplates;
         itemTemplates = itemTemplates->NextSiblingElement("templates")) {
        auto propName = XMLUtils::StringAttribute(itemTemplates, "property");
        auto className = XMLUtils::StringAttribute(itemTemplates, "class");
        if (propName.empty() && className.empty()) {
            wxLogError(_("%s: Invalid code templates entry, no type attributes"), file);
            continue;
        }
        if (!propName.empty() && !className.empty()) {
            wxLogError(_("%s: Invalid code templates entry, multiple type attributes, using property type"), file);
        }

        auto codeInfo = std::make_shared<CodeInfo>();
        for (const auto* itemTemplate = itemTemplates->FirstChildElement("template"); itemTemplate;
             itemTemplate = itemTemplate->NextSiblingElement("template")) {
            auto templateName = XMLUtils::StringAttribute(itemTemplate, "name");
            if (templateName.empty()) {
                wxLogError(_("%s: Empty code template name"), file);
                continue;
            }
            auto templateCode = XMLUtils::GetText(itemTemplate);

            codeInfo->AddTemplate(templateName, templateCode);
        }

        if (!propName.empty()) {
            try {
                auto propType = ParsePropertyType(propName);
                auto [propTemplates, propTemplatesInserted] = m_propertyTypeTemplates.try_emplace(propType);
                auto [propTemplate, propTemplateInserted] = propTemplates->second.try_emplace(language, codeInfo);
                if (!propTemplateInserted) {
                    wxLogError(_("%s: Duplicate code template for property \"%s\""), file, propName);
                }
            } catch (wxFBException& ex) {
                wxLogError(wxString::Format(_("%s: %s"), file, ex.what()));
            }
        } else {
            if (auto objectInfo = GetObjectInfo(className); objectInfo) {
                objectInfo->AddCodeInfo(language, codeInfo);
            } else {
                wxLogError(_("%s: Code template for unknown class \"%s\""), file, className);
            }
        }
    }
}

PObjectPackage ObjectDatabase::LoadPackage(const wxString& file, const wxString& iconPath)
{
    auto doc = XMLUtils::LoadXMLFile(file, true);
    if (!doc) {
        THROW_WXFBEX(wxString::Format(_("%s: Failed to open file"), file))
    }
    if (doc->Error()) {
        THROW_WXFBEX(doc->ErrorStr())
    }
    const auto* root = doc->FirstChildElement(PACKAGE_TAG);
    if (!root) {
        THROW_WXFBEX(wxString::Format(_("%s: Invalid root node"), file))
    }

    auto packageName = XMLUtils::StringAttribute(root, NAME_TAG);
    if (packageName.empty()) {
        THROW_WXFBEX(wxString::Format(_("%s: Empty package name"), file))
    }
    auto packageDesc = XMLUtils::StringAttribute(root, PKGDESC_TAG);
    auto packageIcon = XMLUtils::StringAttribute(root, ICON_TAG);

    auto packageIconPath = wxFileName(iconPath, packageIcon);
    wxBitmap packageBitmap;
    if (packageIconPath.HasName() && packageIconPath.FileExists()) {
        auto packageImage = wxImage(packageIconPath.GetFullPath(), wxBITMAP_TYPE_ANY);
        packageBitmap = wxBitmap(packageImage.Scale(
          AppBitmaps::GetPixelSize(AppBitmaps::Size::Icon_Medium),
          AppBitmaps::GetPixelSize(AppBitmaps::Size::Icon_Medium)));
    } else {
        packageBitmap = AppBitmaps::GetBitmap("unknown", AppBitmaps::Size::Icon_Medium);
    }

    auto package = std::make_shared<ObjectPackage>(packageName, packageDesc, packageBitmap);

    for (const auto* object = root->FirstChildElement(OBJINFO_TAG); object;
         object = object->NextSiblingElement(OBJINFO_TAG)) {
        // Skip components that are not supported by the wxWidgets version this application is linked against
        if (auto wxVersion = object->IntAttribute(WXVERSION_TAG, 0); wxVersion > wxVERSION_NUMBER) {
            continue;
        }

        auto objectClass = XMLUtils::StringAttribute(object, CLASS_TAG);
        if (objectClass.empty()) {
            THROW_WXFBEX(wxString::Format(_("%s: Empty object class"), file))
        }
        auto objectType = XMLUtils::StringAttribute(object, "type");
        if (objectType.empty()) {
            THROW_WXFBEX(wxString::Format(_("%s: Empty type for class \"%s\""), file, objectClass))
        }
        auto objectIcon = XMLUtils::StringAttribute(object, "icon");
        auto objectIconSmall = XMLUtils::StringAttribute(object, "smallIcon");
        auto startGroup = object->BoolAttribute("startgroup", false);

        auto objectInfo = std::make_shared<ObjectInfo>(objectClass, GetObjectType(objectType), package, startGroup);
        if (auto objectIconPath = wxFileName(iconPath, objectIcon);
            objectIconPath.HasName() && objectIconPath.FileExists()) {
            auto objectImage = wxImage(objectIconPath.GetFullPath(), wxBITMAP_TYPE_ANY);
            objectInfo->SetIconFile(wxBitmap(objectImage.Scale(
              AppBitmaps::GetPixelSize(AppBitmaps::Size::Icon), AppBitmaps::GetPixelSize(AppBitmaps::Size::Icon))));
        } else {
            objectInfo->SetIconFile(AppBitmaps::GetBitmap("unknown", AppBitmaps::Size::Icon));
        }
        if (auto objectIconSmallPath = wxFileName(iconPath, objectIconSmall);
            objectIconSmallPath.HasName() && objectIconSmallPath.FileExists()) {
            auto objectImageSmall = wxImage(objectIconSmallPath.GetFullPath(), wxBITMAP_TYPE_ANY);
            objectInfo->SetSmallIconFile(wxBitmap(objectImageSmall.Scale(
              AppBitmaps::GetPixelSize(AppBitmaps::Size::Icon_Small),
              AppBitmaps::GetPixelSize(AppBitmaps::Size::Icon_Small))));
        } else {
            objectInfo->SetSmallIconFile(AppBitmaps::GetBitmap("unknown", AppBitmaps::Size::Icon_Small));
        }
        std::set<PropertyType> types;
        ParseProperties(object, objectInfo, objectInfo->GetCategory(), types);
        ParseEvents(object, objectInfo, objectInfo->GetCategory());

        m_objs.try_emplace(objectClass, objectInfo);
        // TODO: This property should be stored inside the XML instead beeing hardcoded
        if (ShowInPalette(objectInfo->GetObjectTypeName())) {
            package->Add(objectInfo);
        }
    }

    return package;
}

void ObjectDatabase::ParseProperties(
  const tinyxml2::XMLElement* object, PObjectInfo objectInfo, PPropertyCategory objectCategory,
  std::set<PropertyType>& types)
{
    for (const auto* childCategory = object->FirstChildElement(CATEGORY_TAG); childCategory;
         childCategory = childCategory->NextSiblingElement(CATEGORY_TAG)) {
        auto categoryName = XMLUtils::StringAttribute(childCategory, NAME_TAG);
        if (categoryName.empty()) {
            THROW_WXFBEX(wxString::Format(
              _("Empty child category for category \"%s\" for class \"%s\""), objectCategory->GetName(),
              objectInfo->GetClassName()))
        }

        auto propertyCategory = std::make_shared<PropertyCategory>(categoryName);
        objectCategory->AddCategory(propertyCategory);

        ParseProperties(childCategory, objectInfo, propertyCategory, types);
    }

    for (const auto* property = object->FirstChildElement(PROPERTY_TAG); property;
         property = property->NextSiblingElement(PROPERTY_TAG)) {
        auto propertyName = XMLUtils::StringAttribute(property, NAME_TAG);
        if (propertyName.empty()) {
            THROW_WXFBEX(wxString::Format(
              _("Empty property name for category \"%s\" for class \"%s\""), objectCategory->GetName(),
              objectInfo->GetClassName()))
        }
        auto propertyDesc = XMLUtils::StringAttribute(property, DESCRIPTION_TAG);
        auto propertyCustomEditor = XMLUtils::StringAttribute(property, CUSTOM_EDITOR_TAG);
        auto propertyTypeName = XMLUtils::StringAttribute(property, "type");
        if (propertyTypeName.empty()) {
            THROW_WXFBEX(wxString::Format(
              _("Empty property type for category \"%s\" for class \"%s\""), objectCategory->GetName(),
              objectInfo->GetClassName()))
        }
        auto propertyDefaultValue = XMLUtils::GetText(property, wxEmptyString, true);

        auto propertyType = ParsePropertyType(propertyTypeName);
        auto optionList = std::make_shared<OptionList>();
        std::list<PropertyChild> childList;
        switch (propertyType) {
            case PT_BITLIST:
            case PT_OPTION:
            case PT_EDIT_OPTION:
                for (const auto* option = property->FirstChildElement("option"); option;
                     option = option->NextSiblingElement("option")) {
                    // To allow an empty selection, an empty name must be allowed
                    auto optionName = XMLUtils::StringAttribute(option, NAME_TAG);
                    auto optionDesc = XMLUtils::StringAttribute(option, DESCRIPTION_TAG);

                    optionList->AddOption(optionName, optionDesc);
                    m_macroSet.emplace(optionName);
                }
                break;
            case PT_PARENT:
                // The default value is constructed from the child element default values, so delete a possible present
                // property default value
                propertyDefaultValue.clear();
                for (const auto* child = property->FirstChildElement("child"); child;
                     child = child->NextSiblingElement("child")) {
                    auto childName = XMLUtils::StringAttribute(child, NAME_TAG);
                    if (childName.empty()) {
                        THROW_WXFBEX(wxString::Format(
                          _("Empty child name for property \"%s\" for category \"%s\" for class \"%s\""), propertyName,
                          objectCategory->GetName(), objectInfo->GetClassName()))
                    }
                    auto childDesc = XMLUtils::StringAttribute(child, DESCRIPTION_TAG);
                    auto childTypeName = XMLUtils::StringAttribute(child, "type", "wxString");
                    auto childDefaultValue = XMLUtils::GetText(child, wxEmptyString, true);

                    auto childType = ParsePropertyType(childTypeName);

                    if (!childList.empty()) {
                        propertyDefaultValue.append("; ");
                    }
                    propertyDefaultValue.append(childDefaultValue);
                    // TODO: Due to a missing constructor, aggregate initialize a temporary object
                    childList.emplace_back(PropertyChild{childName, childDefaultValue, childDesc, childType});
                }
                break;
            default:
                break;
        }

        objectCategory->AddProperty(propertyName);
        auto propertyInfo = std::make_shared<PropertyInfo>(
          propertyName, propertyType, propertyDefaultValue, propertyDesc, propertyCustomEditor, optionList, childList);
        objectInfo->AddPropertyInfo(propertyInfo);

        // Merge property code templates, once per property type
        if (types.emplace(propertyType).second) {
            if (auto propertyLanguageMap = m_propertyTypeTemplates.find(propertyType);
                propertyLanguageMap != m_propertyTypeTemplates.end()) {
                for (const auto& [language, codeInfo] : propertyLanguageMap->second) {
                    if (codeInfo) {
                        objectInfo->AddCodeInfo(language, codeInfo);
                    }
                }
            }
        }
    }
}

void ObjectDatabase::ParseEvents(
  const tinyxml2::XMLElement* object, PObjectInfo objectInfo, PPropertyCategory objectCategory)
{
    for (const auto* childCategory = object->FirstChildElement(CATEGORY_TAG); childCategory;
         childCategory = childCategory->NextSiblingElement(CATEGORY_TAG)) {
        auto categoryName = XMLUtils::StringAttribute(childCategory, NAME_TAG);
        if (categoryName.empty()) {
            THROW_WXFBEX(wxString::Format(
              _("Empty child category for category \"%s\" for class \"%s\""), objectCategory->GetName(),
              objectInfo->GetClassName()))
        }

        auto propertyCategory = std::make_shared<PropertyCategory>(categoryName);
        objectCategory->AddCategory(propertyCategory);

        ParseEvents(childCategory, objectInfo, propertyCategory);
    }

    for (const auto* event = object->FirstChildElement(EVENT_TAG); event;
         event = event->NextSiblingElement(EVENT_TAG)) {
        auto eventName = XMLUtils::StringAttribute(event, NAME_TAG);
        if (eventName.empty()) {
            THROW_WXFBEX(wxString::Format(
              _("Empty event name for category \"%s\" for class \"%s\""), objectCategory->GetName(),
              objectInfo->GetClassName()))
        }
        auto eventClass = XMLUtils::StringAttribute(event, EVENT_CLASS_TAG, "wxEvent");
        auto eventDesc = XMLUtils::StringAttribute(event, DESCRIPTION_TAG);
        // TODO: This seems to be unused, deepSearch differs from the old approach of just examining the last element
        // node
        auto eventDefaultValue = XMLUtils::GetText(event, wxEmptyString, true);

        objectCategory->AddEvent(eventName);
        auto eventInfo = std::make_shared<EventInfo>(eventName, eventClass, eventDefaultValue, eventDesc);
        objectInfo->AddEventInfo(eventInfo);
    }
}

bool ObjectDatabase::ShowInPalette(wxString type)
{
    return (
      type == wxT("form") || type == wxT("wizard") || type == wxT("wizardpagesimple") || type == wxT("menubar_form") ||
      type == wxT("toolbar_form") || type == wxT("sizer") || type == wxT("gbsizer") || type == wxT("menu") ||
      type == wxT("menuitem") || type == wxT("submenu") || type == wxT("tool") || type == wxT("ribbonbar") ||
      type == wxT("ribbonpage") || type == wxT("ribbonpanel") || type == wxT("ribbonbuttonbar") ||
      type == wxT("ribbonbutton") || type == wxT("ribbondropdownbutton") || type == wxT("ribbonhybridbutton") ||
      type == wxT("ribbontogglebutton") || type == wxT("ribbontoolbar") || type == wxT("ribbontool") ||
      type == wxT("ribbondropdowntool") || type == wxT("ribbonhybridtool") || type == wxT("ribbontoggletool") ||
      type == wxT("ribbongallery") || type == wxT("ribbongalleryitem") || type == wxT("dataviewctrl") ||
      type == wxT("dataviewtreectrl") || type == wxT("dataviewlistctrl") || type == wxT("dataviewlistcolumn") ||
      type == wxT("dataviewcolumn") || type == wxT("notebook") || type == wxT("flatnotebook") ||
      type == wxT("listbook") || type == wxT("simplebook") || type == wxT("choicebook") || type == wxT("auinotebook") ||
      type == wxT("widget") || type == wxT("expanded_widget") || type == wxT("propgrid") ||
      type == wxT("propgridman") || type == wxT("propgridpage") || type == wxT("propgriditem") ||
      type == wxT("statusbar") || type == wxT("component") || type == wxT("container") || type == wxT("menubar") ||
      type == wxT("treelistctrl") || type == wxT("treelistctrlcolumn") || type == wxT("toolbar") ||
      type == wxT("nonvisual") || type == wxT("splitter"));
}


void ObjectDatabase::ImportComponentLibrary(wxString libfile, PwxFBManager manager)
{
    wxString path;
    auto pluginLibrary =
      m_pluginLibraries.emplace(std::piecewise_construct, std::forward_as_tuple(libfile), std::forward_as_tuple())
        .first;
    try {
        pluginLibrary->second.sharedLibrary.load(
          libfile.ToStdString(),
          boost::dll::load_mode::default_mode |           // Use platform default parameters
            boost::dll::load_mode::append_decorations |   // Only the library base name is supplied, it needs to be
                                                          // decorated for the platform
            boost::dll::load_mode::search_system_folders  // This enables usage of RPATH
        );
        path = pluginLibrary->second.sharedLibrary.location().native();
    } catch (const std::system_error& ex) {
        // TODO: Workaround for exception messages received from at least MSVC 2019 and 2022 containing percent
        // placeholders
        wxString escapedEx(ex.what());
        escapedEx.Replace("%", "%%");
        THROW_WXFBEX("Error loading library " << libfile << ": " << escapedEx)
    }
    try {
        pluginLibrary->second.createComponentLibrary =
          pluginLibrary->second.sharedLibrary.get<IComponentLibrary*(IManager*)>("CreateComponentLibrary");
        pluginLibrary->second.freeComponentLibrary =
          pluginLibrary->second.sharedLibrary.get<void(IComponentLibrary*)>("FreeComponentLibrary");
    } catch (const std::system_error& ex) {
        THROW_WXFBEX(path << " is not a valid component library: " << ex.what())
    }

    LogDebug("[Database::ImportComponentLibrary] Importing " + path + " library");
    pluginLibrary->second.componentLibrary = pluginLibrary->second.createComponentLibrary(manager.get());

    // Import all of the components
    auto components = pluginLibrary->second.componentLibrary->GetComponents();
    for (auto& component : components) {
        const auto& class_name = component.first;
        auto* component_interface = component.second;

        // Look for the class in the data read from the .xml files
        PObjectInfo class_info = GetObjectInfo(class_name);
        if (class_info) {
            // TODO: Raise an error instead?
            if (class_info->GetComponent()) {
                LogDebug("Duplicate ObjectInfo for <" + class_name + "> found while loading library <" + path + ">");
            }
            class_info->SetComponent(component_interface);
        } else {
            LogDebug("ObjectInfo for <" + class_name + "> not found while loading library <" + path + ">");
        }
    }

    // Add all of the macros in the library to the macro dictionary
    auto macros = pluginLibrary->second.componentLibrary->GetMacros();
    PMacroDictionary dic = MacroDictionary::GetInstance();
    for (const auto& macro : macros) {
        const auto& name = macro.first;
        const auto& value = macro.second;

        dic->AddMacro(name, value);
        m_macroSet.erase(name);
    }
}

PropertyType ObjectDatabase::ParsePropertyType(wxString str)
{
    PropertyType result;
    PTMap::iterator it = m_propTypes.find(str);
    if (it != m_propTypes.end())
        result = it->second;
    else {
        THROW_WXFBEX(wxString::Format(wxT("Unknown property type \"%s\""), str));
    }

    return result;
}

wxString ObjectDatabase::ParseObjectType(wxString str)
{
    return str;
}


#define PT(x, y) m_propTypes.insert(PTMap::value_type(x, y))
void ObjectDatabase::InitPropertyTypes()
{
    PT(wxT("bool"), PT_BOOL);
    PT(wxT("text"), PT_TEXT);
    PT(wxT("text_ml"), PT_TEXT_ML);
    PT(wxT("int"), PT_INT);
    PT(wxT("uint"), PT_UINT);
    PT(wxT("bitlist"), PT_BITLIST);
    PT(wxT("intlist"), PT_INTLIST);
    PT(wxT("uintlist"), PT_UINTLIST);
    PT(wxT("intpairlist"), PT_INTPAIRLIST);
    PT(wxT("uintpairlist"), PT_UINTPAIRLIST);
    PT(wxT("option"), PT_OPTION);
    PT(wxT("macro"), PT_MACRO);
    PT(wxT("path"), PT_PATH);
    PT(wxT("file"), PT_FILE);
    PT(wxT("wxString"), PT_WXSTRING);
    PT(wxT("wxPoint"), PT_WXPOINT);
    PT(wxT("wxSize"), PT_WXSIZE);
    PT(wxT("wxFont"), PT_WXFONT);
    PT(wxT("wxColour"), PT_WXCOLOUR);
    PT(wxT("bitmap"), PT_BITMAP);
    PT(wxT("wxString_i18n"), PT_WXSTRING_I18N);
    PT(wxT("stringlist"), PT_STRINGLIST);
    PT(wxT("float"), PT_FLOAT);
    PT(wxT("parent"), PT_PARENT);
    PT(wxT("editoption"), PT_EDIT_OPTION);
}

bool ObjectDatabase::LoadObjectTypes()
{
    const auto filepath = m_xmlPath + "objtypes.xml";
    auto doc = XMLUtils::LoadXMLFile(filepath, true);
    if (!doc) {
        wxLogError(_("%s: Failed to open file"), filepath);

        return false;
    }
    if (doc->Error()) {
        wxLogError(doc->ErrorStr());

        return false;
    }
    const auto* root = doc->FirstChildElement("definitions");
    if (!root) {
        wxLogError(_("%s: Invalid root node"), filepath);

        return false;
    }

    // Load object types first so that child types can reference object types defined later in the file

    for (const auto* object = root->FirstChildElement("objtype"); object;
         object = object->NextSiblingElement("objtype")) {
        auto hidden = object->BoolAttribute("hidden", false);
        auto item = object->BoolAttribute("item", false);
        auto name = XMLUtils::StringAttribute(object, "name");
        if (name.empty()) {
            wxLogError(_("%s: Empty object type name"), filepath);
            continue;
        }

        auto objectType = std::make_shared<ObjectType>(name, static_cast<int>(m_types.size()), hidden, item);
        m_types.try_emplace(name, objectType);
    }

    for (const auto* object = root->FirstChildElement("objtype"); object;
         object = object->NextSiblingElement("objtype")) {
        auto objectName = XMLUtils::StringAttribute(object, "name");
        if (objectName.empty()) {
            // The error has already been reported in the loop above
            continue;
        }
        auto objectType = GetObjectType(objectName);

        for (const auto* child = object->FirstChildElement("childtype"); child;
             child = child->NextSiblingElement("childtype")) {
            auto nmax = child->IntAttribute("nmax", -1);
            auto aui_nmax = child->IntAttribute("aui_nmax", -1);
            auto childName = XMLUtils::StringAttribute(child, "name");
            if (childName.empty()) {
                wxLogError(_("%s: Empty child name"), filepath);
                continue;
            }

            auto childType = GetObjectType(childName);
            if (!childType) {
                wxLogError(_("%s: Unknown child type \"%s\""), filepath, childName);
                continue;
            }
            objectType->AddChildType(childType, nmax, aui_nmax);
        }
    }

    return true;
}

PObjectType ObjectDatabase::GetObjectType(wxString name)
{
    PObjectType type;
    ObjectTypeMap::iterator it = m_types.find(name);
    if (it != m_types.end())
        type = it->second;

    return type;
}
