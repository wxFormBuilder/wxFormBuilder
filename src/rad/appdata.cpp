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

#include "appdata.h"

#include <cstring>

#include <wx/clipbrd.h>
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/fs_arc.h>
#include <wx/fs_filter.h>
#include <wx/fs_mem.h>
#include <wx/richmsgdlg.h>
#include <wx/tokenzr.h>

#include <common/xmlutils.h>

#include "codegen/codewriter.h"
#include "codegen/cppcg.h"
#include "codegen/luacg.h"
#include "codegen/phpcg.h"
#include "codegen/pythoncg.h"
#include "model/objectbase.h"
#include "rad/bitmaps.h"
#include "rad/dataobject/dataobject.h"
#include "rad/wxfbevent.h"
#include "rad/wxfbmanager.h"
#include "rad/xrcpreview/xrcpreview.h"
#include "utils/typeconv.h"
#include "utils/wxfbexception.h"
#include "utils/wxfbipc.h"


using namespace TypeConv;


///////////////////////////////////////////////////////////////////////////////
// Comandos
///////////////////////////////////////////////////////////////////////////////

/** Command for expanding an object in the object tree */

class ExpandObjectCmd : public Command
{

private:
    PObjectBase m_object;
    bool m_expand;

protected:
    void DoExecute() override;
    void DoRestore() override;

public:
    ExpandObjectCmd(PObjectBase object, bool expand);
};

/**
 * Comando para insertar un objeto en el árbol.
 */

class InsertObjectCmd : public Command
{

private:
    ApplicationData* m_data;
    PObjectBase m_parent;
    PObjectBase m_object;
    int m_pos;
    PObjectBase m_oldSelected;


protected:
    void DoExecute() override;
    void DoRestore() override;

public:
    InsertObjectCmd(ApplicationData* data, PObjectBase object, PObjectBase parent, int pos = -1);
};

/**
 * Comando para borrar un objeto.
 */

class RemoveObjectCmd : public Command
{

private:
    ApplicationData* m_data;
    PObjectBase m_parent;
    PObjectBase m_object;
    int m_oldPos;
    PObjectBase m_oldSelected;

protected:
    void DoExecute() override;
    void DoRestore() override;

public:
    RemoveObjectCmd(ApplicationData* data, PObjectBase object);
};

/**
 * Comando para modificar una propiedad.
 */

class ModifyPropertyCmd : public Command
{

private:
    PProperty m_property;
    wxString m_oldValue, m_newValue;

protected:
    void DoExecute() override;
    void DoRestore() override;

public:
    ModifyPropertyCmd(PProperty prop, wxString value);
};

/**
 * Command for modifying an event
 */

class ModifyEventHandlerCmd : public Command
{

private:
    PEvent m_event;
    wxString m_oldValue, m_newValue;

protected:
    void DoExecute() override;
    void DoRestore() override;

public:
    ModifyEventHandlerCmd(PEvent event, wxString value);
};

/**
 * Comando para mover de posición un objeto.
 */

class ShiftChildCmd : public Command
{

private:
    PObjectBase m_object;
    int m_oldPos, m_newPos;

protected:
    void DoExecute() override;
    void DoRestore() override;

public:
    ShiftChildCmd(PObjectBase object, int pos);
};

/**
 * CutObjectCmd ademas de eliminar el objeto del árbol se asegura
 * de eliminar la referencia "clipboard" deshacer el cambio.
 */

class CutObjectCmd : public Command
{

private:
    // necesario para consultar/modificar el objeto "clipboard"
    ApplicationData* m_data;
    // PObjectBase m_clipboard;
    PObjectBase m_parent;
    PObjectBase m_object;
    int m_oldPos;
    PObjectBase m_oldSelected;

protected:
    void DoExecute() override;
    void DoRestore() override;

public:
    CutObjectCmd(ApplicationData* data, PObjectBase object);
};

/**
 * Cambia el padre.
 */

class ReparentObjectCmd : public Command
{

private:
    PObjectBase m_sizeritem;
    PObjectBase m_sizer;
    PObjectBase m_oldSizer;
    int m_oldPosition;

protected:
    void DoExecute() override;
    void DoRestore() override;

public:
    ReparentObjectCmd(PObjectBase sizeritem, PObjectBase sizer);
};

///////////////////////////////////////////////////////////////////////////////
// Implementación de los Comandos
///////////////////////////////////////////////////////////////////////////////
ExpandObjectCmd::ExpandObjectCmd(PObjectBase object, bool expand) : m_object(object), m_expand(expand)
{
}

void ExpandObjectCmd::DoExecute()
{
    m_object->SetExpanded(m_expand);
}

void ExpandObjectCmd::DoRestore()
{
    m_object->SetExpanded(!m_expand);
}

InsertObjectCmd::InsertObjectCmd(ApplicationData* data, PObjectBase object, PObjectBase parent, int pos) :
  m_data(data), m_parent(parent), m_object(object), m_pos(pos)
{
    m_oldSelected = data->GetSelectedObject();
}

void InsertObjectCmd::DoExecute()
{
    m_parent->AddChild(m_object);
    m_object->SetParent(m_parent);

    if (m_pos >= 0)
        m_parent->ChangeChildPosition(m_object, m_pos);

    PObjectBase obj = m_object;
    while (obj && obj->GetObjectInfo()->GetObjectType()->IsItem()) {
        if (obj->GetChildCount() > 0)
            obj = obj->GetChild(0);
        else
            return;
    }
    m_data->SelectObject(obj, false, false);
}

void InsertObjectCmd::DoRestore()
{
    m_parent->RemoveChild(m_object);
    m_object->SetParent(PObjectBase());
    m_data->SelectObject(m_oldSelected);
}

//-----------------------------------------------------------------------------

RemoveObjectCmd::RemoveObjectCmd(ApplicationData* data, PObjectBase object)
{
    m_data = data;
    m_object = object;
    m_parent = object->GetParent();
    m_oldPos = m_parent->GetChildPosition(object);
    m_oldSelected = data->GetSelectedObject();
}

void RemoveObjectCmd::DoExecute()
{
    m_parent->RemoveChild(m_object);
    m_object->SetParent(PObjectBase());
    m_data->DetermineObjectToSelect(m_parent, m_oldPos);
}

void RemoveObjectCmd::DoRestore()
{
    m_parent->AddChild(m_object);
    m_object->SetParent(m_parent);

    // restauramos la posición
    m_parent->ChangeChildPosition(m_object, m_oldPos);
    m_data->SelectObject(m_oldSelected, true, false);
}

//-----------------------------------------------------------------------------

ModifyPropertyCmd::ModifyPropertyCmd(PProperty prop, wxString value) : m_property(prop), m_newValue(value)
{
    m_oldValue = prop->GetValue();
}

void ModifyPropertyCmd::DoExecute()
{
    m_property->SetValue(m_newValue);
}

void ModifyPropertyCmd::DoRestore()
{
    m_property->SetValue(m_oldValue);
}

//-----------------------------------------------------------------------------

ModifyEventHandlerCmd::ModifyEventHandlerCmd(PEvent event, wxString value) : m_event(event), m_newValue(value)
{
    m_oldValue = event->GetValue();
}

void ModifyEventHandlerCmd::DoExecute()
{
    m_event->SetValue(m_newValue);
}

void ModifyEventHandlerCmd::DoRestore()
{
    m_event->SetValue(m_oldValue);
}

//-----------------------------------------------------------------------------

ShiftChildCmd::ShiftChildCmd(PObjectBase object, int pos)
{
    m_object = object;
    PObjectBase parent = object->GetParent();

    assert(parent);

    m_oldPos = parent->GetChildPosition(object);
    m_newPos = pos;
}

void ShiftChildCmd::DoExecute()
{
    if (m_oldPos != m_newPos) {
        PObjectBase parent(m_object->GetParent());
        parent->ChangeChildPosition(m_object, m_newPos);
    }
}

void ShiftChildCmd::DoRestore()
{
    if (m_oldPos != m_newPos) {
        PObjectBase parent(m_object->GetParent());
        parent->ChangeChildPosition(m_object, m_oldPos);
    }
}

//-----------------------------------------------------------------------------

CutObjectCmd::CutObjectCmd(ApplicationData* data, PObjectBase object)
{
    m_data = data;
    m_object = object;
    m_parent = object->GetParent();
    m_oldPos = m_parent->GetChildPosition(object);
    m_oldSelected = data->GetSelectedObject();
}

void CutObjectCmd::DoExecute()
{
    // guardamos el clipboard ???
    // m_clipboard = m_data->GetClipboardObject();

    m_data->SetClipboardObject(m_object);
    m_parent->RemoveChild(m_object);
    m_object->SetParent(PObjectBase());
    m_data->DetermineObjectToSelect(m_parent, m_oldPos);
}

void CutObjectCmd::DoRestore()
{
    // reubicamos el objeto donde estaba
    m_parent->AddChild(m_object);
    m_object->SetParent(m_parent);
    m_parent->ChangeChildPosition(m_object, m_oldPos);


    // restauramos el clipboard
    // m_data->SetClipboardObject(m_clipboard);
    m_data->SetClipboardObject(PObjectBase());
    m_data->SelectObject(m_oldSelected, true, false);
}

//-----------------------------------------------------------------------------

ReparentObjectCmd ::ReparentObjectCmd(PObjectBase sizeritem, PObjectBase sizer)
{
    m_sizeritem = sizeritem;
    m_sizer = sizer;
    m_oldSizer = m_sizeritem->GetParent();
    m_oldPosition = m_oldSizer->GetChildPosition(sizeritem);
}

void ReparentObjectCmd::DoExecute()
{
    m_oldSizer->RemoveChild(m_sizeritem);
    m_sizeritem->SetParent(m_sizer);
    m_sizer->AddChild(m_sizeritem);
}

void ReparentObjectCmd::DoRestore()
{
    m_sizer->RemoveChild(m_sizeritem);
    m_sizeritem->SetParent(m_oldSizer);
    m_oldSizer->AddChild(m_sizeritem);
    m_oldSizer->ChangeChildPosition(m_sizeritem, m_oldPosition);
}

///////////////////////////////////////////////////////////////////////////////
// ApplicationData
///////////////////////////////////////////////////////////////////////////////

ApplicationData* ApplicationData::s_instance = NULL;

ApplicationData* ApplicationData::Get(const wxString& rootdir)
{
    if (!s_instance)
        s_instance = new ApplicationData(rootdir);

    return s_instance;
}

void ApplicationData::Destroy()

{
    delete s_instance;

    s_instance = NULL;
}

void ApplicationData::Initialize()
{
    ApplicationData* appData = ApplicationData::Get();
    appData->LoadApp();

    // Use the color of a dominant text to determine if dark mode should be used.
    // TODO: Depending on the used theme it is not clear which color that is,
    //       using the window text has given the best results so far.
    const auto col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    const auto lightness = (col.Red() * 299 + col.Green() * 587 + col.Blue() * 114) / 1000;
    appData->SetDarkMode(lightness > 127);
}

ApplicationData::ApplicationData(const wxString& rootdir) :
  m_rootDir(rootdir),
  m_modFlag(false),
  m_warnOnAdditionsUpdate(true),
  m_darkMode(false),
  m_objDb(new ObjectDatabase()),
  m_manager(new wxFBManager),
  m_ipc(new wxFBIPC),
  m_fbpVerMajor(1),
  // TODO: Update for next release
  m_fbpVerMinor(17)
{
#ifdef __WXFB_DEBUG__
    // wxLog* log = wxLog::SetActiveTarget( NULL );
    m_debugLogTarget = new wxLogWindow(NULL, wxT("Logging"));
// wxLog::SetActiveTarget( log );
#endif
    m_objDb->SetXmlPath(
      m_rootDir + wxFILE_SEP_PATH + wxT("resources") + wxFILE_SEP_PATH + wxT("xml") + wxFILE_SEP_PATH);
    m_objDb->SetIconPath(
      m_rootDir + wxFILE_SEP_PATH + wxT("resources") + wxFILE_SEP_PATH + wxT("icons") + wxFILE_SEP_PATH);
    m_objDb->SetPluginPath(m_rootDir + wxFILE_SEP_PATH + wxT("plugins") + wxFILE_SEP_PATH);

    // Support loading files from memory
    // Used to load the XRC preview, but could be useful elsewhere
    wxFileSystem::AddHandler(new wxMemoryFSHandler);

    // Support for loading files from archives
    wxFileSystem::AddHandler(new wxArchiveFSHandler);
    wxFileSystem::AddHandler(new wxFilterFSHandler);
}

ApplicationData::~ApplicationData()
{
#ifdef __WXFB_DEBUG__
    delete m_debugLogTarget;
    m_debugLogTarget = 0;
#endif
}

void ApplicationData::LoadApp()

{
    wxString bitmapPath = m_objDb->GetXmlPath() + wxT("icons.xml");
    AppBitmaps::LoadBitmaps(bitmapPath, m_objDb->GetIconPath());
    m_objDb->LoadObjectTypes();
    m_objDb->LoadPlugins(m_manager);
}

PwxFBManager ApplicationData::GetManager()
{
    return m_manager;
}

PObjectBase ApplicationData::GetSelectedObject()
{
    return m_selObj;
}

PObjectBase ApplicationData::GetSelectedForm()
{
    if (
      (m_selObj->GetObjectTypeName() == wxT("form")) || (m_selObj->GetObjectTypeName() == wxT("wizard")) ||
      (m_selObj->GetObjectTypeName() == wxT("menubar_form")) || (m_selObj->GetObjectTypeName() == wxT("toolbar_form")))
        return m_selObj;
    else
        return m_selObj->FindParentForm();
}


PObjectBase ApplicationData::GetProjectData()
{
    return m_project;
}

void ApplicationData::BuildNameSet(PObjectBase obj, PObjectBase top, std::set<wxString>& name_set)
{
    if (obj != top) {
        PProperty nameProp = top->GetProperty(wxT("name"));

        if (nameProp)
            name_set.insert(nameProp->GetValue());
    }

    for (unsigned int i = 0; i < top->GetChildCount(); i++) BuildNameSet(obj, top->GetChild(i), name_set);
}

void ApplicationData::ResolveNameConflict(PObjectBase obj)
{
    while (obj && obj->GetObjectInfo()->GetObjectType()->IsItem()) {
        if (obj->GetChildCount() > 0)
            obj = obj->GetChild(0);
        else
            return;
    }

    PProperty nameProp = obj->GetProperty(wxT("name"));

    if (!nameProp)
        return;

    // Save the original name for use later.
    wxString originalName = nameProp->GetValue();

    // el nombre no puede estar repetido dentro del mismo form
    /*PObjectBase top = obj->FindNearAncestor( wxT( "form" ) );*/
    PObjectBase top = obj->FindParentForm();

    if (!top)
        top = m_project;  // el objeto es un form.

    // construimos el conjunto de nombres
    std::set<wxString> name_set;

    BuildNameSet(obj, top, name_set);

    // comprobamos si hay conflicto
    std::set<wxString>::iterator it = name_set.find(originalName);

    int i = 0;

    wxString name = originalName;  // The name that gets incremented.

    while (it != name_set.end()) {
        i++;
        name = wxString::Format(wxT("%s%i"), originalName, i);
        it = name_set.find(name);
    }

    nameProp->SetValue(name);
}

void ApplicationData::ResolveSubtreeNameConflicts(PObjectBase obj, PObjectBase topObj)
{
    if (!topObj) {
        /*topObj = obj->FindNearAncestor( wxT( "form" ) );*/
        topObj = obj->FindParentForm();

        if (!topObj)
            topObj = m_project;  // object is the project
    }

    // Ignore item objects
    while (obj && obj->GetObjectInfo()->GetObjectType()->IsItem()) {
        if (obj->GetChildCount() > 0)
            obj = obj->GetChild(0);
        else
            return;  // error
    }

    // Resolve a possible name conflict
    ResolveNameConflict(obj);

    // Recurse through all children
    for (unsigned int i = 0; i < obj->GetChildCount(); i++) ResolveSubtreeNameConflicts(obj->GetChild(i), topObj);
}

int ApplicationData::CalcPositionOfInsertion(PObjectBase selected, PObjectBase parent)
{
    int pos = -1;

    if (parent && selected) {
        PObjectBase parentSelected = selected->GetParent();

        while (parentSelected && parentSelected != parent) {
            selected = parentSelected;
            parentSelected = selected->GetParent();
        }

        if (parentSelected && parentSelected == parent)
            pos = parent->GetChildPosition(selected) + 1;
    }

    return pos;
}

void ApplicationData::RemoveEmptyItems(PObjectBase obj)
{
    if (!obj->GetObjectInfo()->GetObjectType()->IsItem()) {
        bool emptyItem = true;

        // esto es un algoritmo ineficiente pero "seguro" con los índices

        while (emptyItem) {
            emptyItem = false;

            for (unsigned int i = 0; !emptyItem && i < obj->GetChildCount(); i++) {
                PObjectBase child = obj->GetChild(i);

                if (child->GetObjectInfo()->GetObjectType()->IsItem() && child->GetChildCount() == 0) {
                    obj->RemoveChild(child);  // borramos el item
                    child->SetParent(PObjectBase());

                    emptyItem = true;  // volvemos a recorrer
                    wxString msg;
                    msg.Printf(wxT("Empty item removed under %s"), obj->GetPropertyAsString(wxT("name")));
                    wxLogWarning(msg);
                }
            }
        }
    }

    for (unsigned int i = 0; i < obj->GetChildCount(); i++) RemoveEmptyItems(obj->GetChild(i));
}

PObjectBase ApplicationData::SearchSizerInto(PObjectBase obj)
{
    PObjectBase theSizer;

    if (obj->GetObjectInfo()->IsSubclassOf(wxT("sizer")) || obj->GetObjectInfo()->IsSubclassOf(wxT("gbsizer")))
        theSizer = obj;
    else {
        for (unsigned int i = 0; !theSizer && i < obj->GetChildCount(); i++)
            theSizer = SearchSizerInto(obj->GetChild(i));
    }

    return theSizer;
}

///////////////////////////////////////////////////////////////////////////////

void ApplicationData::ExpandObject(PObjectBase obj, bool expand)
{
    PCommand command(new ExpandObjectCmd(obj, expand));
    Execute(command);

    // collapse also all children ...
    PropagateExpansion(obj, expand, !expand);

    NotifyObjectExpanded(obj);
}

void ApplicationData::PropagateExpansion(PObjectBase obj, bool expand, bool up)
{
    if (obj) {
        if (up) {
            PObjectBase child;

            for (size_t i = 0; i < obj->GetChildCount(); i++) {
                child = obj->GetChild(i);

                PCommand command(new ExpandObjectCmd(child, expand));
                Execute(command);

                PropagateExpansion(child, expand, up);
            }
        } else {
            PropagateExpansion(obj->GetParent(), expand, up);

            PCommand command(new ExpandObjectCmd(obj, expand));
            Execute(command);
        }
    }
}

bool ApplicationData::SelectObject(PObjectBase obj, bool force /*= false*/, bool notify /*= true */)
{
    if ((obj == m_selObj) && !force) {
        return false;
    }

    m_selObj = obj;

    if (notify) {
        NotifyObjectSelected(obj, force);
    }
    return true;
}

void ApplicationData::CreateObject(wxString name)
{
    try {
        LogDebug("[ApplicationData::CreateObject] New " + name);
        PObjectBase old_selected = GetSelectedObject();
        PObjectBase parent = old_selected;
        PObjectBase obj;

        if (parent) {
            bool created = false;

            // Para que sea más práctico, si el objeto no se puede crear debajo
            // del objeto seleccionado vamos a intentarlo en el padre del seleccionado
            // y seguiremos subiendo hasta que ya no podamos crear el objeto.

            while (parent && !created) {
                // además, el objeto se insertará a continuación del objeto seleccionado
                obj = m_objDb->CreateObject(_STDSTR(name), parent);

                if (obj) {
                    int pos = CalcPositionOfInsertion(GetSelectedObject(), parent);

                    PCommand command(new InsertObjectCmd(this, obj, parent, pos));
                    Execute(command);  // m_cmdProc.Execute(command);
                    created = true;
                    ResolveNameConflict(obj);
                } else {
                    // lo vamos a seguir intentando con el padre, pero cuidado, el padre
                    // no puede ser un item!
                    parent = parent->GetParent();

                    while (parent && parent->GetObjectInfo()->GetObjectType()->IsItem()) parent = parent->GetParent();
                }
            }
        }

        // Seleccionamos el objeto, si este es un item entonces se selecciona
        // el objeto contenido. ¿Tiene sentido tener un item debajo de un item?

        while (obj && obj->GetObjectInfo()->GetObjectType()->IsItem())
            obj = (obj->GetChildCount() > 0 ? obj->GetChild(0) : PObjectBase());

        NotifyObjectCreated(obj);

        if (obj) {
            SelectObject(obj, true, true);
        } else {
            SelectObject(old_selected, true, true);
        }
    } catch (wxFBException& ex) {
        wxLogError(ex.what());
    }
}

void ApplicationData::RemoveObject(PObjectBase obj)
{
    DoRemoveObject(obj, false);
}

void ApplicationData::CutObject(PObjectBase obj)
{
    DoRemoveObject(obj, true);
}

void ApplicationData::DoRemoveObject(PObjectBase obj, bool cutObject)
{
    // Note:
    //  When removing an object it is important that the "item" objects
    // are not left behind
    PObjectBase parent = obj->GetParent();
    PObjectBase deleted_obj = obj;

    if (parent) {
        // Get the top item
        while (parent && parent->GetObjectInfo()->GetObjectType()->IsItem()) {
            obj = parent;
            parent = obj->GetParent();
        }

        if (cutObject) {
            m_copyOnPaste = false;
            PCommand command(new CutObjectCmd(this, obj));
            Execute(command);
        } else {
            PCommand command(new RemoveObjectCmd(this, obj));
            Execute(command);
        }

        NotifyObjectRemoved(deleted_obj);
        SelectObject(GetSelectedObject(), true, true);
    } else {
        if (obj->GetObjectTypeName() != wxT("project"))
            assert(false);
    }

    CheckProjectTree(m_project);
}

void ApplicationData::DetermineObjectToSelect(PObjectBase parent, unsigned int pos)
{
    // get position of next control or last control
    PObjectBase objToSelect;
    unsigned int count = parent->GetChildCount();
    if (0 == count) {
        objToSelect = parent;
    } else {
        pos = (pos < count ? pos : count - 1);
        objToSelect = parent->GetChild(pos);
    }

    while (objToSelect && objToSelect->GetObjectInfo()->GetObjectType()->IsItem()) {
        objToSelect = objToSelect->GetChild(0);
    }

    SelectObject(objToSelect);
}

void ApplicationData::CopyObjectToClipboard(PObjectBase obj)
{
    // Write some text to the clipboard

    // Do not call Open() when the clipboard is opened
    if (!wxTheClipboard->IsOpened()) {
        if (!wxTheClipboard->Open()) {
            return;
        }
    }

    // This data objects are held by the clipboard,
    // so do not delete them in the app.
    wxTheClipboard->SetData(new wxFBDataObject(obj));
    wxTheClipboard->Close();
}

bool ApplicationData::PasteObjectFromClipboard(PObjectBase parent)
{
    // Do not call Open() when the clipboard is opened
    if (!wxTheClipboard->IsOpened()) {
        if (!wxTheClipboard->Open()) {
            return false;
        }
    }

    if (wxTheClipboard->IsSupported(wxFBDataObject::DataObjectFormat())) {
        wxFBDataObject data;
        if (wxTheClipboard->GetData(data)) {
            PObjectBase object = data.GetObject();
            if (object) {
                wxTheClipboard->Close();
                return PasteObject(parent, object);
            }
        }
    }

    wxTheClipboard->Close();

    return false;
}

bool ApplicationData::CanPasteObjectFromClipboard()
{
    // Do not call Open() when the clipboard is opened
    if (!wxTheClipboard->IsOpened()) {
        if (!wxTheClipboard->Open()) {
            return false;
        }
    }

    bool canPaste = wxTheClipboard->IsSupported(wxFBDataObject::DataObjectFormat());

    if (wxTheClipboard->IsOpened())
        wxTheClipboard->Close();

    return canPaste;
}

void ApplicationData::CopyObject(PObjectBase obj)
{
    m_copyOnPaste = true;

    // Make a copy of the object on the clipboard, otherwise
    // modifications to the object after the copy will also
    // be made on the clipboard.
    m_clipboard = m_objDb->CopyObject(obj);

    CheckProjectTree(m_project);
}

bool ApplicationData::PasteObject(PObjectBase parent, PObjectBase objToPaste)
{
    try {
        PObjectBase clipboard;
        if (objToPaste) {
            clipboard = objToPaste;
        } else if (m_clipboard) {
            if (m_copyOnPaste) {
                clipboard = m_objDb->CopyObject(m_clipboard);
            } else {
                clipboard = m_clipboard;
            }
        }

        if (!clipboard) {
            return false;
        }

        // Remove parent/child relationship from clipboard object
        PObjectBase clipParent = clipboard->GetParent();
        if (clipParent) {
            clipParent->RemoveChild(clipboard);
            clipboard->SetParent(PObjectBase());
        }

        // Vamos a hacer un pequeño truco, intentaremos crear un objeto nuevo
        // del mismo tipo que el guardado en m_clipboard debajo de parent.
        // El objeto devuelto quizá no sea de la misma clase que m_clipboard debido
        // a que esté incluido dentro de un "item".
        // Por tanto, si el objeto devuelto es no-nulo, entonces vamos a descender
        // en el arbol hasta que el objeto sea de la misma clase que m_clipboard,
        // momento en que cambiaremos dicho objeto por m_clipboard.
        //
        // Ejemplo:
        //
        //  m_clipboard :: wxButton
        //  parent      :: wxBoxSizer
        //
        //  obj = CreateObject(m_clipboard->GetObjectInfo()->GetClassName(), parent)
        //
        //  obj :: sizeritem
        //              /
        //           wxButton   <- Cambiamos este por m_clipboard
        PObjectBase old_parent = parent;

        PObjectBase obj = m_objDb->CreateObject(_STDSTR(clipboard->GetObjectInfo()->GetClassName()), parent);

        // If the object is already contained in an item, we may need to get the object out of the first
        // item before pasting
        if (!obj) {

            PObjectBase tempItem = clipboard;
            while (tempItem->GetObjectInfo()->GetObjectType()->IsItem()) {
                tempItem = tempItem->GetChild(0);
                if (!tempItem) {
                    break;
                }

                obj = m_objDb->CreateObject(_STDSTR(tempItem->GetObjectInfo()->GetClassName()), parent);
                if (obj) {
                    clipboard = tempItem;
                    break;
                }
            }
        }

        int pos = -1;

        if (!obj) {
            // si no se ha podido crear el objeto vamos a intentar crearlo colgado
            // del padre de "parent" y además vamos a insertarlo en la posición
            // siguiente a "parent"
            PObjectBase selected = parent;
            parent = selected->GetParent();

            while (parent && parent->GetObjectInfo()->GetObjectType()->IsItem()) {
                selected = parent;
                parent = selected->GetParent();
            }

            if (parent) {
                obj = m_objDb->CreateObject(_STDSTR(clipboard->GetObjectInfo()->GetClassName()), parent);
                if (obj) {
                    pos = CalcPositionOfInsertion(selected, parent);
                }
            }
        }

        if (!obj) {
            return false;
        }

        PObjectBase aux = obj;

        while (aux && aux->GetObjectInfo() != clipboard->GetObjectInfo())
            aux = (aux->GetChildCount() > 0 ? aux->GetChild(0) : PObjectBase());

        if (aux && aux != obj) {
            // sustituimos aux por clipboard
            PObjectBase auxParent = aux->GetParent();
            auxParent->RemoveChild(aux);
            aux->SetParent(PObjectBase());

            auxParent->AddChild(clipboard);
            clipboard->SetParent(auxParent);
        } else
            obj = clipboard;

        // y finalmente insertamos en el arbol
        PCommand command(new InsertObjectCmd(this, obj, parent, pos));

        Execute(command);

        if (!m_copyOnPaste)
            m_clipboard.reset();

        ResolveSubtreeNameConflicts(obj);

        NotifyProjectRefresh();

        // vamos a mantener seleccionado el nuevo objeto creado
        // pero hay que tener en cuenta que es muy probable que el objeto creado
        // sea un "item"
        while (obj && obj->GetObjectInfo()->GetObjectType()->IsItem()) {
            assert(obj->GetChildCount() > 0);
            obj = obj->GetChild(0);
        }

        SelectObject(obj, true, true);

        CheckProjectTree(m_project);
    } catch (wxFBException& ex) {
        wxLogError(ex.what());
        return false;
    }

    return true;
}

void ApplicationData::InsertObject(PObjectBase obj, PObjectBase parent)
{
    // FIXME! comprobar obj se puede colgar de parent
    //  if (parent->GetObjectInfo()->GetObjectType()->FindChildType(
    //    obj->GetObjectInfo()->GetObjectType()))
    //  {
    PCommand command(new InsertObjectCmd(this, obj, parent));
    Execute(command);  // m_cmdProc.Execute(command);
    NotifyProjectRefresh();
    //  }
}

void ApplicationData::MergeProject(PObjectBase project)
{
    // FIXME! comprobar obj se puede colgar de parent

    for (unsigned int i = 0; i < project->GetChildCount(); i++) {
        // m_project->AddChild(project->GetChild(i));
        // project->GetChild(i)->SetParent(m_project);

        PObjectBase child = project->GetChild(i);
        RemoveEmptyItems(child);

        InsertObject(child, m_project);
    }

    // Merge bitmaps and icons properties
    PObjectBase thisProject = GetProjectData();
    PProperty prop = thisProject->GetProperty(_("bitmaps"));
    if (prop) {
        wxString value = prop->GetValue();
        value.Trim();
        value << wxT(" ") << project->GetPropertyAsString(_("bitmaps"));
        prop->SetValue(value);
    }
    prop = thisProject->GetProperty(_("icons"));
    if (prop) {
        wxString value = prop->GetValue();
        value.Trim();
        value << wxT(" ") << project->GetPropertyAsString(_("icons"));
        prop->SetValue(value);
    }

    NotifyProjectRefresh();
}

void ApplicationData::ModifyProperty(PProperty prop, wxString str)
{
    PObjectBase object = prop->GetObject();

    if (str != prop->GetValue()) {
        PCommand command(new ModifyPropertyCmd(prop, str));
        Execute(command);  // m_cmdProc.Execute(command);

        NotifyPropertyModified(prop);
    }
}

void ApplicationData::ModifyEventHandler(PEvent evt, wxString value)
{
    PObjectBase object = evt->GetObject();

    if (value != evt->GetValue()) {
        PCommand command(new ModifyEventHandlerCmd(evt, value));
        Execute(command);  // m_cmdProc.Execute(command);

        NotifyEventHandlerModified(evt);
    }
}

void ApplicationData::SaveProject(const wxString& filename)
{
    // Make sure this file is not already open

    if (!m_ipc->VerifySingleInstance(filename, false)) {
        if (
          wxYES == wxMessageBox(
                     wxT("You cannot save over a file that is currently open in another instance.\nWould you like to "
                         "switch to that instance?"),
                     wxT("Open in Another Instance"), wxICON_QUESTION | wxYES_NO, wxTheApp->GetTopWindow())) {
            m_ipc->VerifySingleInstance(filename, true);
        }

        return;
    }

    tinyxml2::XMLDocument doc(false, tinyxml2::PRESERVE_WHITESPACE);
    auto* prolog = doc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"");
    auto* root = doc.NewElement("wxFormBuilder_Project");
    doc.InsertEndChild(prolog);
    doc.InsertEndChild(root);

    auto* version = doc.NewElement("FileVersion");
    version->SetAttribute("major", static_cast<int>(AppData()->m_fbpVerMajor));
    version->SetAttribute("minor", static_cast<int>(AppData()->m_fbpVerMinor));
    root->InsertEndChild(version);

    auto* project = doc.NewElement("");
    m_project->Serialize(project);
    root->InsertEndChild(project);

    wxFileName name(filename);
    if (!XMLUtils::SaveXMLFile(name.GetFullPath(), doc)) {
        THROW_WXFBEX(_("Failed to write to file: ") << name.GetFullPath())
    }

    m_projectFile = name.GetFullPath();
    SetProjectPath(name.GetPath());
    m_modFlag = false;
    m_cmdProc.SetSavePoint();
    NotifyProjectSaved();
}

bool ApplicationData::LoadProject(const wxString& file, bool justGenerate)
{
    wxFileName filename(file);
    if (!filename.FileExists()) {
        wxLogError(_("File does not exist: %s"), file);
        return false;
    }

    if (!justGenerate && !m_ipc->VerifySingleInstance(file)) {
        return false;
    }

    auto doc = XMLUtils::LoadXMLFile(filename.GetFullPath(), false);
    if (!doc) {
        wxLogError(_("%s: Failed to open file"), filename.GetFullPath());
        return false;
    }
    if (doc->Error()) {
        wxLogError(doc->ErrorStr());
        return false;
    }
    const auto* root = doc->FirstChildElement();
    if (!root) {
        wxLogError(_("%s: Missing root node"), file);
        return false;
    }
    const auto* rootName = root->Name();
    if (!rootName) {
        rootName = "";
    }

    int versionMajor = 0;
    int versionMinor = 0;
    if (std::strcmp(rootName, "wxFormBuilder_Project") == 0) {
        const auto* version = root->FirstChildElement("FileVersion");
        if (!version) {
            wxLogError(_("%s: Invalid version node"), file);
            return false;
        }
        versionMajor = version->IntAttribute("major", versionMajor);
        versionMinor = version->IntAttribute("minor", versionMinor);
    } else if (std::strcmp(rootName, "object") != 0) {
        wxLogError(_("%s: Invalid root node"), file);
        return false;
    }

    enum class VersionState {
        OLDER = -1,
        EQUAL,
        NEWER,
    };
    auto versionState = VersionState::EQUAL;
    if (versionMajor == m_fbpVerMajor) {
        if (versionMinor < m_fbpVerMinor) {
            versionState = VersionState::OLDER;
        } else if (versionMinor > m_fbpVerMinor) {
            versionState = VersionState::NEWER;
        }
    } else {
        if (versionMajor < m_fbpVerMajor) {
            versionState = VersionState::OLDER;
        } else if (versionMajor > m_fbpVerMajor) {
            versionState = VersionState::NEWER;
        }
    }
    if (versionState == VersionState::NEWER) {
        if (justGenerate) {
            wxLogError("This project file version is newer than this version of wxFormBuilder.\n");
        } else {
            wxMessageBox(
                _(
                    "This project file version is newer than this version of wxFormBuilder.\n"
                    "The file cannot be opened.\n\n"
                    "Please download an new wxFormBuilder version version from http://www.wxformbuilder.org"
                ),
                _("New Project File Version"),
                wxICON_ERROR
            );
        }
        return false;
    }
    if (versionState == VersionState::OLDER) {
        if (justGenerate) {
            wxLogError("This project file version is outdated, update the file by using the GUI mode first.\n");
        } else {
            wxMessageBox(
                _(
                    "This project file version is older than this version of wxFormBuilder,\n"
                    "the file version will get updated during loading.\n\n"
                    "WARNING: Saving the project file will prevent older versions of wxFormBuilder to open the file!"
                ),
                _("Old Project File Version")
            );
        }

        if (ConvertProject(doc.get(), file, versionMajor, versionMinor)) {
            root = doc->FirstChildElement();
        } else {
            wxLogError(_("%s: Failed to convert project"), file);
            return false;
        }
    }

    const auto* project = root->FirstChildElement("object");
    if (!project) {
        wxLogError(_("%s: Invalid project node"), file);
        return false;
    }

    m_objDb->ResetObjectCounters();

    try {
        if (auto projectObject = m_objDb->CreateObject(project); projectObject && projectObject->GetObjectTypeName() == "project") {
            m_project = projectObject;
            m_selObj = m_project;
            m_projectFile = filename.GetFullPath();
            SetProjectPath(filename.GetPath());
            // Set the modification to true if the project was older and has been converted
            m_modFlag = (versionState == VersionState::OLDER);
            m_cmdProc.Reset();
            NotifyProjectLoaded();
            NotifyProjectRefresh();
        }
    } catch (wxFBException& ex) {
        wxLogError(ex.what());
        return false;
    }

    return true;
}

bool ApplicationData::ConvertProject(tinyxml2::XMLDocument* doc, const wxString& path, int versionMajor, int versionMinor)
{
    auto* root = doc->FirstChildElement();
    if (!root) {
        wxLogError(_("%s: Invalid root node"), path);
        return false;
    }
    tinyxml2::XMLElement* project = nullptr;
    tinyxml2::XMLElement* version = nullptr;

    if (const auto* rootName = root->Name(); rootName && std::strcmp(rootName, "object") == 0) {
        // Very old project file that starts directly with the Project object, assume also a missing prolog
        auto* prolog = doc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"");
        auto* fileRoot = doc->NewElement("wxFormBuilder_Project");
        auto* fileVersion = doc->NewElement("FileVersion");

        doc->InsertFirstChild(prolog);
        doc->InsertAfterChild(prolog, fileRoot);
        fileRoot->InsertEndChild(fileVersion);
        project = root;
        version = fileVersion;
        root = fileRoot;
    } else {
        version = root->FirstChildElement("FileVersion");
        if (!version) {
            wxLogError(_("%s: Invalid version node"), path);
            return false;
        }
        project = root->FirstChildElement("object");
        if (!project) {
            wxLogError(_("%s: Invalid project node"), path);
            return false;
        }
    }

    ConvertProjectProperties(project, path, versionMajor, versionMinor);
    ConvertObject(project, versionMajor, versionMinor);
    version->SetAttribute("major", static_cast<int>(AppData()->m_fbpVerMajor));
    version->SetAttribute("minor", static_cast<int>(AppData()->m_fbpVerMinor));

    return true;
}

void ApplicationData::ConvertProjectProperties(tinyxml2::XMLElement* project, const wxString& path, int versionMajor, int versionMinor)
{
    if (auto projectClass = XMLUtils::StringAttribute(project, "class"); projectClass != "Project") {
        return;
    }

    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 5)) {
        // Remove and optionally save property user_headers
        if (auto properties = GetProperties(project, {"user_headers"}); !properties.empty()) {
            auto* userHeadersProperty = *properties.begin();
            auto userHeadersValue = XMLUtils::GetText(userHeadersProperty);
            if (!userHeadersValue.empty()) {
                if (
                    wxMessageBox(
                        _(
                            "The project property \"user_headers\" has been removed.\n"
                            "The purpose of this property was to include precompiled headers or headers for subclasses.\n"
                            "Now, this is done by using the project property \"precompiled_header\" and\n"
                            "the property \"header\" of the property \"subclass\".\n\n"
                            "This conversion cannot be done automatically, do you want to extract the current value of\n"
                            "the property \"user_classes\" to file to be able to perform this conversion manually?"
                        ),
                        _("Property removed"),
                        wxICON_QUESTION | wxYES_NO | wxYES_DEFAULT,
                        wxTheApp->GetTopWindow()
                    ) == wxYES
                ) {
                    wxFileName projectPath(path);
                    wxFileDialog dlg(
                        wxTheApp->GetTopWindow(),
                        _("Save \"user_headers\""),
                        projectPath.GetPath(),
                        wxString::Format("%s_user_headers.txt", projectPath.GetName()),
                        _("All Files (*.*)|*.*"),
                        wxFD_SAVE
                    );
                    if (dlg.ShowModal() == wxID_OK) {
                        wxLogNull noLog;
                        auto outputPath = dlg.GetPath();
                        if (wxFFile outputFile(outputPath, "w"); outputFile.IsOpened()) {
                            outputFile.Write(userHeadersValue);
                        } else {
                            wxLogError(_("Failed to open %s for writing \"user_headers\":\n%s"), outputPath, userHeadersValue);
                        }
                    }
                }
            }

            userHeadersProperty->GetDocument()->DeleteNode(userHeadersProperty);
        }
    }

    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 8)) {
        // The pch property is now the exact code to be generated, not just the header filename
        // The goal of this conversion block is to determine which of two possible pch blocks to use
        // The pch block that wxFB generated changed in version 1.6
        if (auto properties = GetProperties(project, {"precompiled_header"}); !properties.empty()) {
            auto* pchProperty = *properties.begin();
            auto pchValue = XMLUtils::GetText(pchProperty);
            if (!pchValue.empty()) {
                if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 6)) {
                    XMLUtils::SetText(
                        pchProperty,
                        wxString::Format(
                            "#include \"%s\"\n"
                            "#ifdef __BORLANDC__\n"
                            "#pragma hdrstop\n"
                            "#endif //__BORLANDC__\n"
                            "\n"
                            "#ifndef WX_PRECOMP\n"
                            "#include <wx/wx.h>\n"
                            "#endif //WX_PRECOMP",
                            pchValue
                        )
                    );
                } else {
                    XMLUtils::SetText(
                        pchProperty,
                        wxString::Format(
                            "#ifdef WX_PRECOMP\n"
                            "\n"
                            "#include \"%s\"\n"
                            "#ifdef __BORLANDC__\n"
                            "#pragma hdrstop\n"
                            "#endif //__BORLANDC__\n"
                            "\n"
                            "#else\n"
                            "#include <wx/wx.h>\n"
                            "#endif //WX_PRECOMP",
                            pchValue
                        )
                    );
                }
            }
        }
    }

    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 9)) {
        // The format of string list properties changed
        auto properties = GetProperties(project, {"namespace", "bitmaps", "icons"});
        for (auto* property : properties) {
            auto value = XMLUtils::GetText(property);
            if (!value.empty()) {
                auto convertedValue = TypeConv::OldStringToArrayString(value);
                XMLUtils::SetText(property, TypeConv::ArrayStringToString(convertedValue));
            }
        }
    }

    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 10)) {
        // event_handler moved to the forms
        if (auto properties = GetProperties(project, {"event_handler"}); !properties.empty()) {
            auto* eventHandlerProperty = *properties.begin();
            for (auto* form = project->FirstChildElement("object"); form; form = form->NextSiblingElement("object")) {
                form->InsertEndChild(eventHandlerProperty->DeepClone(nullptr));
            }

            eventHandlerProperty->GetDocument()->DeleteNode(eventHandlerProperty);
        }
    }

    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 18)) {
        auto cppProperties = GetProperties(project, {
            "precompiled_header",
            "namespace",
            "class_decoration",
            "use_enum",
            "use_array_enum",
            "help_provider",
            "event_generation",
            "disconnect_events"
        });
        for (auto& property : cppProperties) {
            auto name = XMLUtils::StringAttribute(property, "name");
            XMLUtils::SetAttribute(property, "name", wxString::Format("cpp_%s", name));
        }

        auto pythonProperties = GetProperties(project, {
            "indent_with_spaces",
            "image_path_wrapper_function_name",
            "skip_python_events",
            "disconnect_python_events",
            "disconnect_mode"
        });
        for (auto& property : pythonProperties) {
            auto name = XMLUtils::StringAttribute(property, "name");
            name.Replace("_python", "");
            XMLUtils::SetAttribute(property, "name", wxString::Format("python_%s", name));
        }

        auto luaProperties = GetProperties(project, {
            "ui_table",
            "skip_lua_events"
        });
        for (auto& property : luaProperties) {
            auto name = XMLUtils::StringAttribute(property, "name");
            name.Replace("_lua", "");
            XMLUtils::SetAttribute(property, "name", wxString::Format("lua_%s", name));
        }

        auto phpProperties = GetProperties(project, {
            "skip_php_events",
            "disconnect_php_events"
        });
        for (auto& property : phpProperties) {
            auto name = XMLUtils::StringAttribute(property, "name");
            name.Replace("_php", "");
            XMLUtils::SetAttribute(property, "name", wxString::Format("php_%s", name));
        }
        // The originally unprefixed python property was silently used before, initialize the own property from that value
        if (auto properties = GetProperties(project, {"python_disconnect_mode"}); !properties.empty()) {
            auto* disconnectModeProperty = *properties.begin();
            auto* phpDisconnectModeProperty = project->InsertNewChildElement("property");
            XMLUtils::SetAttribute(phpDisconnectModeProperty, "name", "php_disconnect_mode");
            XMLUtils::SetText(phpDisconnectModeProperty, XMLUtils::GetText(disconnectModeProperty));
        }
    }
}

void ApplicationData::ConvertObject(tinyxml2::XMLElement* object, int versionMajor, int versionMinor)
{
    for (auto* child = object->FirstChildElement("object"); child; child = child->NextSiblingElement("object")) {
        ConvertObject(child, versionMajor, versionMinor);
    }

    auto objectClass = XMLUtils::StringAttribute(object, "class");

    // Convert an unversioned file to version 1.3
    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 3)) {
        // The property 'option' became 'proportion'
        if (objectClass == "sizeritem" || objectClass == "gbsizeritem" || objectClass == "spacer") {
            if (auto properties = GetProperties(object, {"option"}); !properties.empty()) {
                auto* optionProperty = *properties.begin();
                XMLUtils::SetAttribute(optionProperty, "name", "proportion");
            }
        }

        // The 'style' property used to have both wxWindow styles and the styles of the specific controls
        // now it only has the styles of the specific controls, and wxWindow styles are saved in window_style
        // This also applies to 'extra_style', which was once combined with 'style'.
        // And they were named 'WindowStyle' and one point, too...
        auto windowStyleProperties = GetProperties(object, {"style", "WindowStyle"});
        auto* windowStyleProperty = object->InsertNewChildElement("property");
        XMLUtils::SetAttribute(windowStyleProperty, "name", "window_style");
        for (auto* property : windowStyleProperties) {
            MoveOptions(
                property,
                windowStyleProperty,
                {
                    "wxSIMPLE_BORDER",
                    "wxDOUBLE_BORDER",
                    "wxSUNKEN_BORDER",
                    "wxRAISED_BORDER",
                    "wxSTATIC_BORDER",
                    "wxNO_BORDER",
                    "wxTRANSPARENT_WINDOW",
                    "wxTAB_TRAVERSAL",
                    "wxWANTS_CHARS",
                    "wxVSCROLL",
                    "wxHSCROLL",
                    "wxALWAYS_SHOW_SB",
                    "wxCLIP_CHILDREN",
                    "wxFULL_REPAINT_ON_RESIZE",
                },
                true
            );
        }

        auto windowExtraStyleProperties = GetProperties(object, {"style", "extra_style", "WindowStyle"});
        auto* windowExtraStyleProperty = object->InsertNewChildElement("property");
        XMLUtils::SetAttribute(windowExtraStyleProperty, "name", "window_extra_style");
        for (auto* property : windowExtraStyleProperties) {
            MoveOptions(
                property,
                windowExtraStyleProperty,
                {
                    "wxWS_EX_VALIDATE_RECURSIVELY",
                    "wxWS_EX_BLOCK_EVENTS",
                    "wxWS_EX_TRANSIENT",
                    "wxWS_EX_PROCESS_IDLE",
                    "wxWS_EX_PROCESS_UI_UPDATES",
                },
                true
            );
        }
    }

    // Convert to version 1.4
    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 4)) {
        if (objectClass == "wxCheckList") {
            // The class we once named "wxCheckList" really represented a "wxCheckListBox", now that we use the #class
            // macro in code generation, it generates the wrong code
            XMLUtils::SetAttribute(object, "class", "wxCheckListBox");
        }
    }

    // No object changes for version 1.5, only project changes

    // Convert to version 1.6
    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 6)) {
        if (objectClass == "spacer") {
            // Spacer used to be represented by its own class, it is now under a sizeritem like everything else.
            // No need to check for a wxGridBagSizer, because it was introduced at the same time.
            // The goal is to change the class to sizeritem, then create a spacer child, then move "width" and "height" to the spacer
            XMLUtils::SetAttribute(object, "class", "sizeritem");

            auto* spacer = object->InsertNewChildElement("object");
            XMLUtils::SetAttribute(spacer, "class", "spacer");
            if (auto properties = GetProperties(object, {"width"}); !properties.empty()) {
                auto* widthProperty = *properties.begin();
                spacer->InsertEndChild(widthProperty);
            }
            if (auto properties = GetProperties(object, {"height"}); !properties.empty()) {
                auto* heightProperty = *properties.begin();
                spacer->InsertEndChild(heightProperty);
            }
        }
    }

    // Convert to version 1.7
    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 7)) {
        // All font properties get stored now. The font property conversion is automatic because it is just an extension of the old values.

        // Remove deprecated 2.6 things
        if (objectClass == "Dialog") {
            if (auto properties = GetProperties(object, {"style"}); !properties.empty()) {
                auto* styleProperty = *properties.begin();
                auto styleValue = XMLUtils::GetText(styleProperty);
                if (!styleValue.empty()) {
                    if (TypeConv::FlagSet("wxTHICK_FRAME", styleValue)) {
                        styleValue = TypeConv::ClearFlag("wxTHICK_FRAME", styleValue);
                        styleValue = TypeConv::SetFlag("wxRESIZE_BORDER", styleValue);
                    }
                    styleValue = TypeConv::ClearFlag("wxNO_3D", styleValue);

                    XMLUtils::SetText(styleProperty, styleValue);
                }
            }
        }
    }

    // No object changes for version 1.8, only project changes

    // Convert to version 1.9
    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 9)) {
        // The format of string list properties changed
        //std::invoke_result<decltype(GetProperties), ApplicationData*, tinyxml2::XMLElement*, const std::unordered_set<wxString>&>::type properties;
        std::unordered_set<tinyxml2::XMLElement*> properties;
        if (objectClass == "wxComboBox" || objectClass == "wxChoice" || objectClass == "wxListBox" || objectClass == "wxRadioBox" || objectClass == "wxCheckListBox") {
            properties = GetProperties(object, {"choices"});
        } else if (objectClass == "wxGrid") {
            properties = GetProperties(object, {"col_label_values", "row_label_values"});
        }
        for (auto* property : properties) {
            auto value = XMLUtils::GetText(property);
            if (!value.empty()) {
                auto convertedValue = TypeConv::OldStringToArrayString(value);
                XMLUtils::SetText(property, TypeConv::ArrayStringToString(convertedValue));
            }
        }
    }

    // No object changes for version 1.10, only project changes

    // Convert to version 1.11
    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 11)) {
        // bitmap properties are now stored in the format 'source'; 'data' instead of the old format 'data'; 'source'
        if (auto properties = GetProperties(object, {"bitmap"}); !properties.empty()) {
            auto* bitmapProperty = *properties.begin();
            auto bitmapValue = XMLUtils::GetText(bitmapProperty);
            if (!bitmapValue.empty()) {
                wxString source;
                auto data = bitmapValue.BeforeLast(';', &source).Trim(true).Trim(false);
                source.Trim(true).Trim(false);
                if (source.Contains("Load From")) {
                    XMLUtils::SetText(bitmapProperty, wxString::Format("%s; %s", source, data));
                }
            }
        }
    }

    // Convert to version 1.12
    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 12)) {
        // Note: The previous update code converted wxTreeListCtrl and wxTreeListCtrlColumn to wxadditions::wxTreeListCtrl and wxadditions::wxTreeListCtrlColumn.
        //       However, the current XML definitions use the non namespace variants and this change was never reverted, so this update was dropped.
        if (objectClass == "wxScintilla") {
            XMLUtils::SetAttribute(object, "class", "wxStyledTextCtrl");
        }

        if (objectClass == "Dialog" || objectClass == "Panel") {
            RemoveProperties(
                object,
                {
                    "BottomDockable",
                    "LeftDockable",
                    "RightDockable",
                    "TopDockable",
                    "caption_visible",
                    "center_pane",
                    "close_button",
                    "default_pane",
                    "dock",
                    "dock_fixed",
                    "docking",
                    "floatable",
                    "gripper",
                    "maximize_button",
                    "minimize_button",
                    "moveable",
                    "pane_border",
                    "pin_button",
                    "resize",
                    "show",
                    "toolbar_pane",
                    "aui_name",
                }
            );
        }

        if (
            objectClass == "Dialog" || objectClass == "Panel" ||
            objectClass == "wxStaticText" || objectClass == "CustomControl" || objectClass == "wxAuiNotebook" || objectClass == "wxPanel" ||
            objectClass == "wxToolBar" || objectClass == "wxStyledTextCtrl" || objectClass == "wxPropertyGridManager" || objectClass == "wxTreeListCtrl"
        ) {
            RemoveProperties(object, {"validator_style", "validator_type"});
        }

        if (objectClass == "wxStyledTextCtrl" || objectClass == "wxPropertyGridManager") {
            RemoveProperties(object, {"use_wxAddition"});
        }
    }

    // TODO: No object changes for version 1.13, it is unknown what was changed for that version

    // Convert to version 1.14
    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 14)) {
        // Rename all wx*_BORDER-Styles to wxBORDER_*-Styles and remove wxDOUBLE_BORDER
        auto styleProperties = GetProperties(object, {"style", "window_style"});
        for (auto* property : styleProperties) {
            auto value = XMLUtils::GetText(property);
            if (!value.empty()) {
                if (TypeConv::FlagSet("wxSIMPLE_BORDER", value)) {
                    value = TypeConv::ClearFlag("wxSIMPLE_BORDER", value);
                    value = TypeConv::SetFlag("wxBORDER_SIMPLE", value);
                }
                if (TypeConv::FlagSet("wxDOUBLE_BORDER", value)) {
                    value = TypeConv::ClearFlag("wxDOUBLE_BORDER", value);
                }
                if (TypeConv::FlagSet("wxSUNKEN_BORDER", value)) {
                    value = TypeConv::ClearFlag("wxSUNKEN_BORDER", value);
                    value = TypeConv::SetFlag("wxBORDER_SUNKEN", value);
                }
                if (TypeConv::FlagSet("wxRAISED_BORDER", value)) {
                    value = TypeConv::ClearFlag("wxRAISED_BORDER", value);
                    value = TypeConv::SetFlag("wxBORDER_RAISED", value);
                }
                if (TypeConv::FlagSet("wxSTATIC_BORDER", value)) {
                    value = TypeConv::ClearFlag("wxSTATIC_BORDER", value);
                    value = TypeConv::SetFlag("wxBORDER_STATIC", value);
                }
                if (TypeConv::FlagSet("wxNO_BORDER", value)) {
                    value = TypeConv::ClearFlag("wxNO_BORDER", value);
                    value = TypeConv::SetFlag("wxBORDER_NONE", value);
                }

                XMLUtils::SetText(property, value);
            }
        }

        if (objectClass == "wxBitmapButton") {
            // wxBitmapButton: Remove wxBU_AUTODRAW and rename properties selected->pressed, hover->current
            if (auto properties = GetProperties(object, {"style"}); !properties.empty()) {
                auto* styleProperty = *properties.begin();
                auto styleValue = XMLUtils::GetText(styleProperty);
                if (!styleValue.empty()) {
                    if (TypeConv::FlagSet("wxBU_AUTODRAW", styleValue)) {
                        styleValue = TypeConv::ClearFlag("wxBU_AUTODRAW", styleValue);
                    }
                    XMLUtils::SetText(styleProperty, styleValue);
                }
            }

            if (auto properties = GetProperties(object, {"selected"}); !properties.empty()) {
                auto* selectedProperty = *properties.begin();
                XMLUtils::SetAttribute(selectedProperty, "name", "pressed");
            }

            if (auto properties = GetProperties(object, {"hover"}); !properties.empty()) {
                auto* hoverProperty = *properties.begin();
                XMLUtils::SetAttribute(hoverProperty, "name", "current");
            }
        } else if (objectClass == "wxStaticText") {
            // wxStaticText: Rename wxALIGN_CENTRE -> wxALIGN_CENTER_HORIZONTAL
            if (auto properties = GetProperties(object, {"style"}); !properties.empty()) {
                auto* styleProperty = *properties.begin();
                auto styleValue = XMLUtils::GetText(styleProperty);
                if (!styleValue.empty()) {
                    if (TypeConv::FlagSet("wxALIGN_CENTRE", styleValue)) {
                        styleValue = TypeConv::ClearFlag("wxALIGN_CENTRE", styleValue);
                        styleValue = TypeConv::SetFlag("wxALIGN_CENTER_HORIZONTAL", styleValue);
                    }
                    XMLUtils::SetText(styleProperty, styleValue);
                }
            }
        } else if (objectClass == "wxRadioBox") {
            // wxRadioBox: Remove wxRA_USE_CHECKBOX
            if (auto properties = GetProperties(object, {"style"}); !properties.empty()) {
                auto* styleProperty = *properties.begin();
                auto styleValue = XMLUtils::GetText(styleProperty);
                if (!styleValue.empty()) {
                    if (TypeConv::FlagSet("wxRA_USE_CHECKBOX", styleValue)) {
                        styleValue = TypeConv::ClearFlag("wxRA_USE_CHECKBOX", styleValue);
                    }
                    XMLUtils::SetText(styleProperty, styleValue);
                }
            }
        } else if (objectClass == "wxRadioButton") {
            // wxRadioButton: Remove wxRB_USE_CHECKBOX
            if (auto properties = GetProperties(object, {"style"}); !properties.empty()) {
                auto* styleProperty = *properties.begin();
                auto styleValue = XMLUtils::GetText(styleProperty);
                if (!styleValue.empty()) {
                    if (TypeConv::FlagSet("wxRB_USE_CHECKBOX", styleValue)) {
                        styleValue = TypeConv::ClearFlag("wxRB_USE_CHECKBOX", styleValue);
                    }
                    XMLUtils::SetText(styleProperty, styleValue);
                }
            }
        } else if (objectClass == "wxStatusBar") {
            // wxStatusBar: Rename wxST_SIZEGRIP -> wxSTB_SIZEGRIP
            if (auto properties = GetProperties(object, {"style"}); !properties.empty()) {
                auto* styleProperty = *properties.begin();
                auto styleValue = XMLUtils::GetText(styleProperty);
                if (!styleValue.empty()) {
                    if (TypeConv::FlagSet("wxST_SIZEGRIP", styleValue)) {
                        styleValue = TypeConv::ClearFlag("wxST_SIZEGRIP", styleValue);
                        styleValue = TypeConv::SetFlag("wxSTB_SIZEGRIP", styleValue);
                    }
                    XMLUtils::SetText(styleProperty, styleValue);
                }
            }
        } else if (objectClass == "wxMenuBar") {
            // wxMenuBar: Remove wxMB_DOCKABLE
            if (auto properties = GetProperties(object, {"style"}); !properties.empty()) {
                auto* styleProperty = *properties.begin();
                auto styleValue = XMLUtils::GetText(styleProperty);
                if (!styleValue.empty()) {
                    if (TypeConv::FlagSet("wxMB_DOCKABLE", styleValue)) {
                        styleValue = TypeConv::ClearFlag("wxMB_DOCKABLE", styleValue);
                    }
                    XMLUtils::SetText(styleProperty, styleValue);
                }
            }
        }
    }

    // Convert to version 1.15
    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 15)) {
        if (objectClass == "wxTextCtrl" || objectClass == "wxSearchCtrl") {
            // wxTextCtrl, wxSearchCtrl: Rename wxTE_CENTRE -> wxTE_CENTER
            if (auto properties = GetProperties(object, {"style"}); !properties.empty()) {
                auto* styleProperty = *properties.begin();
                auto styleValue = XMLUtils::GetText(styleProperty);
                if (!styleValue.empty()) {
                    if (TypeConv::FlagSet("wxTE_CENTRE", styleValue)) {
                        styleValue = TypeConv::ClearFlag("wxTE_CENTRE", styleValue);
                        styleValue = TypeConv::SetFlag("wxTE_CENTER", styleValue);
                    }
                    XMLUtils::SetText(styleProperty, styleValue);
                }
            }
        } else if (objectClass == "wxGrid") {
            // wxGrid: Rename wxALIGN_CENTRE -> wxALIGN_CENTER
            auto properties = GetProperties(
                object,
                {
                    "col_label_horiz_alignment",
                    "col_label_vert_alignment",
                    "row_label_horiz_alignment",
                    "row_label_vert_alignment",
                    "cell_horiz_alignment",
                    "cell_vert_alignment",
                }
            );
            for (auto* property : properties) {
                auto value = XMLUtils::GetText(property);
                if (!value.empty()) {
                    if (TypeConv::FlagSet("wxALIGN_CENTRE", value)) {
                        value = TypeConv::ClearFlag("wxALIGN_CENTRE", value);
                        value = TypeConv::SetFlag("wxALIGN_CENTER", value);
                    }
                    XMLUtils::SetText(property, value);
                }
            }
        } else if (objectClass == "wxNotebook") {
            // wxNotebook: Remove wxNB_FLAT
            if (auto properties = GetProperties(object, {"style"}); !properties.empty()) {
                auto* styleProperty = *properties.begin();
                auto styleValue = XMLUtils::GetText(styleProperty);
                if (!styleValue.empty()) {
                    if (TypeConv::FlagSet("wxNB_FLAT", styleValue)) {
                        styleValue = TypeConv::ClearFlag("wxNB_FLAT", styleValue);
                    }
                    XMLUtils::SetText(styleProperty, styleValue);
                }
            }
        }
    }

    // Convert to version 1.16
    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 16)) {
        if (objectClass == "wxMenuBar") {
            RemoveProperties(object, {"label"});
        }
    }

    // Convert to version 1.17
    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 17)) {
        if (
          objectClass == "ribbonButton" || objectClass == "ribbonDropdownButton" || objectClass == "ribbonHybridButton" ||
          objectClass == "ribbonToggleButton" || objectClass == "ribbonTool" || objectClass == "ribbonDropdownTool" ||
          objectClass == "ribbonHybridTool" || objectClass == "ribbonToggleTool" || objectClass == "ribbonGalleryItem") {
            RemoveProperties(
              object, {"bg", "context_help", "context_menu", "drag_accept_files", "enabled", "fg", "font", "hidden",
                       "maximum_size", "minimum_size", "name", "permission", "pos", "size", "subclass", "tooltip",
                       "window_extra_style", "window_name", "window_style"});
        }
        if (objectClass == "ribbonGalleryItem") {
            RemoveProperties(object, {"label", "help"});
        }
    }

    // Convert to version 1.18
    if (versionMajor < 1 || (versionMajor == 1 && versionMinor < 18)) {
        if (objectClass == "wxAuiToolBar") {
            RemoveProperties(object, {"label_visible", "toolbar_label"});
        }
    }
}

std::unordered_set<tinyxml2::XMLElement*> ApplicationData::GetProperties(tinyxml2::XMLElement* element, const std::set<wxString>& properties)
{
    std::unordered_set<tinyxml2::XMLElement*> result;
    for (auto* property = element->FirstChildElement("property"); property; property = property->NextSiblingElement("property")) {
        auto name = XMLUtils::StringAttribute(property, "name");
        if (name.empty()) {
            continue;
        }
        if (properties.find(name) != properties.end()) {
            result.insert(property);
        }
    }

    return result;
}

void ApplicationData::RemoveProperties(tinyxml2::XMLElement* element, const std::set<wxString>& properties)
{
    for (auto* next = element->FirstChildElement("property"); next; ) {
        auto* current = next;
        next = next->NextSiblingElement("property");

        auto name = XMLUtils::StringAttribute(current, "name");
        if (name.empty()) {
            continue;
        }
        if (properties.find(name) != properties.end()) {
            element->DeleteChild(current);
        }
    }
}

bool ApplicationData::MoveOptions(tinyxml2::XMLElement* src, tinyxml2::XMLElement* dest, const std::set<wxString>& options, bool deleteEmptySrc)
{
    auto currentSrcValue = XMLUtils::GetText(src);
    auto currentDestValue = XMLUtils::GetText(dest);
    wxString nextSrcValue;
    nextSrcValue.reserve(currentSrcValue.size());
    auto nextDestValue = currentDestValue;
    nextDestValue.reserve(currentDestValue.size() + currentSrcValue.size());

    wxStringTokenizer tkz(currentSrcValue, wxT("|"), wxTOKEN_RET_EMPTY_ALL);
    while (tkz.HasMoreTokens()) {
        auto token = tkz.GetNextToken();
        token.Trim(true);
        token.Trim(false);

        if (options.find(token) != options.end()) {
            if (!nextDestValue.empty()) {
                nextDestValue.append("|");
            }
            nextDestValue.append(token);
        } else {
            if (!nextSrcValue.empty()) {
                nextSrcValue.append("|");
            }
            nextSrcValue.append(token);
        }
    }

    if (nextSrcValue != currentSrcValue) {
        XMLUtils::SetText(src, nextSrcValue);
    }
    if (nextDestValue != currentDestValue) {
        XMLUtils::SetText(dest, nextDestValue);
    }

    if (deleteEmptySrc && nextSrcValue.empty()) {
        src->GetDocument()->DeleteNode(src);

        return true;
    }

    return false;
}

void ApplicationData::NewProject()

{
    m_project = m_objDb->CreateObject("Project");
    m_selObj = m_project;
    m_modFlag = false;
    m_cmdProc.Reset();
    m_projectFile = wxT("");
    SetProjectPath(wxT(""));
    m_ipc->Reset();
    NotifyProjectRefresh();
}

void ApplicationData::GenerateCode(bool panelOnly, bool noDelayed)
{
    NotifyCodeGeneration(panelOnly, !noDelayed);
}

void ApplicationData::GenerateInheritedClass(PObjectBase form, wxString className, wxString path, wxString file)
{
    try {
        PObjectBase project = GetProjectData();
        if (!project) {
            wxLogWarning(_("No Project?!"));
            return;
        }

        if (!::wxDirExists(path)) {
            wxLogWarning(_("Invalid Path: %s"), path);
            return;
        }

        PObjectBase obj = m_objDb->CreateObject("UserClasses", PObjectBase());

        PProperty baseNameProp = obj->GetProperty(wxT("basename"));
        PProperty nameProp = obj->GetProperty(wxT("name"));
        PProperty fileProp = obj->GetProperty(wxT("file"));
        PProperty genfileProp = obj->GetProperty(wxT("gen_file"));
        PProperty typeProp = obj->GetProperty(wxT("type"));
        PProperty pchProp = obj->GetProperty("cpp_precompiled_header");

        if (!(baseNameProp && nameProp && fileProp && typeProp && genfileProp && pchProp)) {
            wxLogWarning(wxT("Missing Property"));
            return;
        }

        wxFileName inherFile(file);
        if (!inherFile.MakeAbsolute(path)) {
            wxLogWarning(_("Unable to make \"%s\" absolute to \"%s\""), file, path);
            return;
        }

        const wxString& genFileValue = project->GetPropertyAsString(_("file"));
        wxFileName genFile(genFileValue);
        if (!genFile.MakeAbsolute(path)) {
            wxLogWarning(_("Unable to make \"%s\" absolute to \"%s\""), genFileValue, path);
            return;
        }

        const wxString& genFileFullPath = genFile.GetFullPath();
        if (!genFile.MakeRelativeTo(inherFile.GetPath(wxPATH_GET_VOLUME))) {
            wxLogWarning(
              _("Unable to make \"%s\" relative to \"%s\""), genFileFullPath, inherFile.GetPath(wxPATH_GET_VOLUME));
            return;
        }

        baseNameProp->SetValue(form->GetPropertyAsString(_("name")));
        nameProp->SetValue(className);
        fileProp->SetValue(inherFile.GetName());
        genfileProp->SetValue(genFile.GetFullPath());
        typeProp->SetValue(form->GetClassName());

        if (auto property = project->GetProperty("cpp_precompiled_header"); property) {
            pchProp->SetValue(property->GetValue());
        }

        // Determine if Microsoft BOM should be used
        bool useMicrosoftBOM = false;
        if (auto property = project->GetProperty("use_microsoft_bom"); property) {
            useMicrosoftBOM = (property->GetValueAsInteger() != 0);
        }
        // Determine encoding
        bool useUtf8 = false;
        if (auto property = project->GetProperty("encoding"); property) {
            useUtf8 = (property->GetValueAsString() != wxT("ANSI"));
        }
        // Determine eol-style
        bool useNativeEOL = false;
        if (auto property = project->GetProperty("use_native_eol"); property) {
            useNativeEOL = (property->GetValueAsInteger() != 0);
        }

        PProperty pCodeGen = project->GetProperty(wxT("code_generation"));
        if (pCodeGen && TypeConv::FlagSet(wxT("C++"), pCodeGen->GetValue())) {
            CppCodeGenerator codegen;

            const wxString& fullPath = inherFile.GetFullPath();
            codegen.ParseFiles(fullPath + wxT(".h"), fullPath + wxT(".cpp"));

            auto h_cw = std::make_shared<FileCodeWriter>(fullPath + wxT(".h"), useMicrosoftBOM, useUtf8, useNativeEOL);
            auto cpp_cw = std::make_shared<FileCodeWriter>(fullPath + wxT(".cpp"), useMicrosoftBOM, useUtf8, useNativeEOL);

            codegen.SetHeaderWriter(h_cw);
            codegen.SetSourceWriter(cpp_cw);

            codegen.GenerateInheritedClass(obj, form);
        } else if (pCodeGen && TypeConv::FlagSet(wxT("Python"), pCodeGen->GetValue())) {
            PythonCodeGenerator codegen;

            const wxString& fullPath = inherFile.GetFullPath();
            auto python_cw = std::make_shared<FileCodeWriter>(fullPath + wxT(".py"), useMicrosoftBOM, useUtf8, useNativeEOL);

            codegen.SetSourceWriter(python_cw);

            codegen.GenerateInheritedClass(obj, form);
        } else if (pCodeGen && TypeConv::FlagSet(wxT("PHP"), pCodeGen->GetValue())) {
            PHPCodeGenerator codegen;

            const wxString& fullPath = inherFile.GetFullPath();
            auto php_cw = std::make_shared<FileCodeWriter>(fullPath + wxT(".php"), useMicrosoftBOM, useUtf8, useNativeEOL);

            codegen.SetSourceWriter(php_cw);

            codegen.GenerateInheritedClass(obj, form);
        } else if (pCodeGen && TypeConv::FlagSet(wxT("Lua"), pCodeGen->GetValue())) {
            LuaCodeGenerator codegen;

            const wxString& fullPath = inherFile.GetFullPath();
            auto lua_cw = std::make_shared<FileCodeWriter>(fullPath + wxT(".lua"), useMicrosoftBOM, useUtf8, useNativeEOL);

            codegen.SetSourceWriter(lua_cw);

            codegen.GenerateInheritedClass(obj, form, genFileFullPath);
        }

        wxLogStatus(wxT("Class generated at \'%s\'."), path);
    } catch (wxFBException& ex) {
        wxLogError(ex.what());
    }
}

void ApplicationData::MovePosition(PObjectBase obj, bool right, unsigned int num)
{
    PObjectBase noItemObj = obj;

    PObjectBase parent = obj->GetParent();

    if (parent) {
        // Si el objeto está incluido dentro de un item hay que desplazar
        // el item

        while (parent && parent->GetObjectInfo()->GetObjectType()->IsItem()) {
            obj = parent;
            parent = obj->GetParent();
        }

        unsigned int pos = parent->GetChildPosition(obj);

        // nos aseguramos de que los límites son correctos

        unsigned int children_count = parent->GetChildCount();

        if ((right && num + pos < children_count) || (!right && (num <= pos))) {
            pos = (right ? pos + num : pos - num);

            PCommand command(new ShiftChildCmd(obj, pos));
            Execute(command);  // m_cmdProc.Execute(command);
            NotifyProjectRefresh();
            SelectObject(noItemObj, true);
        }
    }
}

void ApplicationData::MoveHierarchy(PObjectBase obj, bool up)
{
    PObjectBase sizeritem = obj->GetParent();
    if (!(sizeritem && sizeritem->GetObjectInfo()->IsSubclassOf(wxT("sizeritembase")))) {
        return;
    }

    PObjectBase nextSizer = sizeritem->GetParent();  // points to the object's sizer
    if (nextSizer) {
        if (up) {
            do {
                nextSizer = nextSizer->GetParent();
            } while (nextSizer && !nextSizer->GetObjectInfo()->IsSubclassOf(wxT("sizer")) &&
                     !nextSizer->GetObjectInfo()->IsSubclassOf(wxT("gbsizer")));

            if (
              nextSizer && (nextSizer->GetObjectInfo()->IsSubclassOf(wxT("sizer")) ||
                            nextSizer->GetObjectInfo()->IsSubclassOf(wxT("gbsizer")))) {
                PCommand cmdReparent(new ReparentObjectCmd(sizeritem, nextSizer));
                Execute(cmdReparent);
                NotifyProjectRefresh();
                SelectObject(obj, true);
            }
        } else {
            // object will be move to the top sizer of the next sibling object
            // subtree.
            unsigned int pos = nextSizer->GetChildPosition(sizeritem) + 1;

            if (pos < nextSizer->GetChildCount()) {
                nextSizer = SearchSizerInto(nextSizer->GetChild(pos));

                if (nextSizer) {
                    PCommand cmdReparent(new ReparentObjectCmd(sizeritem, nextSizer));
                    Execute(cmdReparent);
                    NotifyProjectRefresh();
                    SelectObject(obj, true);
                }
            }
        }
    }
}


void ApplicationData::Undo()
{
    m_cmdProc.Undo();
    m_modFlag = !m_cmdProc.IsAtSavePoint();
    NotifyProjectRefresh();
    CheckProjectTree(m_project);
    NotifyObjectSelected(GetSelectedObject());
}

void ApplicationData::Redo()
{
    m_cmdProc.Redo();
    m_modFlag = !m_cmdProc.IsAtSavePoint();
    NotifyProjectRefresh();
    CheckProjectTree(m_project);
    NotifyObjectSelected(GetSelectedObject());
}


void ApplicationData::ToggleExpandLayout(PObjectBase obj)
{
    if (!obj) {
        return;
    }

    PObjectBase parent = obj->GetParent();
    if (!parent) {
        return;
    }

    if (!parent->GetObjectInfo()->IsSubclassOf(wxT("sizeritembase"))) {
        return;
    }

    PProperty propFlag = parent->GetProperty(wxT("flag"));

    if (!propFlag) {
        return;
    }

    wxString value;
    wxString currentValue = propFlag->GetValueAsString();

    value =
      (TypeConv::FlagSet(wxT("wxEXPAND"), currentValue) ? TypeConv::ClearFlag(wxT("wxEXPAND"), currentValue)
                                                        : TypeConv::SetFlag(wxT("wxEXPAND"), currentValue));

    ModifyProperty(propFlag, value);
}

void ApplicationData::ToggleStretchLayout(PObjectBase obj)
{
    if (!obj) {
        return;
    }

    PObjectBase parent = obj->GetParent();
    if (!parent) {
        return;
    }

    if (parent->GetObjectTypeName() != wxT("sizeritem") && parent->GetObjectTypeName() != wxT("gbsizeritem")) {
        return;
    }

    PProperty proportion = parent->GetProperty(wxT("proportion"));
    if (!proportion) {
        return;
    }

    wxString value = (proportion->GetValue() != wxT("0") ? wxT("0") : wxT("1"));
    ModifyProperty(proportion, value);
}

void ApplicationData::CheckProjectTree(PObjectBase obj)
{
    assert(obj);

    for (unsigned int i = 0; i < obj->GetChildCount(); i++) {
        PObjectBase child = obj->GetChild(i);

        if (child->GetParent() != obj) {
            wxLogError(wxString::Format("Parent of object \'" + child->GetPropertyAsString("name") + "\' is wrong!"));
        }
        CheckProjectTree(child);
    }
}

bool ApplicationData::GetLayoutSettings(PObjectBase obj, int* flag, int* option, int* border, int* orient)
{
    if (!obj) {
        return false;
    }

    PObjectBase parent = obj->GetParent();
    if (!parent) {
        return false;
    }

    if (parent->GetObjectInfo()->IsSubclassOf(wxT("sizeritembase"))) {
        PProperty propOption = parent->GetProperty(wxT("proportion"));
        if (propOption) {
            *option = propOption->GetValueAsInteger();
        }

        *flag = parent->GetPropertyAsInteger(wxT("flag"));
        *border = parent->GetPropertyAsInteger(wxT("border"));

        PObjectBase sizer = parent->GetParent();
        if (sizer) {
            wxString parentName = sizer->GetClassName();
            if (wxT("wxBoxSizer") == parentName || wxT("wxStaticBoxSizer") == parentName) {
                PProperty propOrient = sizer->GetProperty(wxT("orient"));
                if (propOrient) {
                    *orient = propOrient->GetValueAsInteger();
                }
            }
        }
        return true;
    }

    return false;
}

void ApplicationData::ChangeAlignment(PObjectBase obj, int align, bool vertical)
{
    if (!obj) {
        return;
    }

    PObjectBase parent = obj->GetParent();
    if (!parent) {
        return;
    }

    if (!parent->GetObjectInfo()->IsSubclassOf(wxT("sizeritembase"))) {
        return;
    }

    PProperty propFlag = parent->GetProperty(wxT("flag"));

    if (!propFlag) {
        return;
    }

    wxString value = propFlag->GetValueAsString();

    // Primero borramos los flags de la configuración previa, para así
    // evitar conflictos de alineaciones.

    if (vertical) {
        value = TypeConv::ClearFlag(wxT("wxALIGN_TOP"), value);
        value = TypeConv::ClearFlag(wxT("wxALIGN_BOTTOM"), value);
        value = TypeConv::ClearFlag(wxT("wxALIGN_CENTER_VERTICAL"), value);
    } else {
        value = TypeConv::ClearFlag(wxT("wxALIGN_LEFT"), value);
        value = TypeConv::ClearFlag(wxT("wxALIGN_RIGHT"), value);
        value = TypeConv::ClearFlag(wxT("wxALIGN_CENTER_HORIZONTAL"), value);
    }

    wxString alignStr;

    switch (align) {

        case wxALIGN_RIGHT:
            alignStr = wxT("wxALIGN_RIGHT");

            break;

        case wxALIGN_CENTER_HORIZONTAL:
            alignStr = wxT("wxALIGN_CENTER_HORIZONTAL");

            break;

        case wxALIGN_BOTTOM:
            alignStr = wxT("wxALIGN_BOTTOM");

            break;

        case wxALIGN_CENTER_VERTICAL:
            alignStr = wxT("wxALIGN_CENTER_VERTICAL");

            break;
    }

    value = TypeConv::SetFlag(alignStr, value);

    ModifyProperty(propFlag, value);
}

void ApplicationData::ToggleBorderFlag(PObjectBase obj, int border)
{
    if (!obj) {
        return;
    }

    PObjectBase parent = obj->GetParent();
    if (!parent) {
        return;
    }

    if (!parent->GetObjectInfo()->IsSubclassOf(wxT("sizeritembase"))) {
        return;
    }

    PProperty propFlag = parent->GetProperty(wxT("flag"));

    if (!propFlag) {
        return;
    }

    wxString value = propFlag->GetValueAsString();

    value = TypeConv::ClearFlag(wxT("wxALL"), value);
    value = TypeConv::ClearFlag(wxT("wxTOP"), value);
    value = TypeConv::ClearFlag(wxT("wxBOTTOM"), value);
    value = TypeConv::ClearFlag(wxT("wxRIGHT"), value);
    value = TypeConv::ClearFlag(wxT("wxLEFT"), value);

    int intVal = propFlag->GetValueAsInteger();
    intVal ^= border;

    if ((intVal & wxALL) == wxALL)
        value = TypeConv::SetFlag(wxT("wxALL"), value);
    else {
        if ((intVal & wxTOP) != 0)
            value = TypeConv::SetFlag(wxT("wxTOP"), value);

        if ((intVal & wxBOTTOM) != 0)
            value = TypeConv::SetFlag(wxT("wxBOTTOM"), value);

        if ((intVal & wxRIGHT) != 0)
            value = TypeConv::SetFlag(wxT("wxRIGHT"), value);

        if ((intVal & wxLEFT) != 0)
            value = TypeConv::SetFlag(wxT("wxLEFT"), value);
    }

    ModifyProperty(propFlag, value);
}

void ApplicationData::CreateBoxSizerWithObject(PObjectBase obj)
{
    PObjectBase parent = obj->GetParent();
    if (!parent) {
        return;
    }

    PObjectBase grandParent = parent->GetParent();
    if (!grandParent) {
        return;
    }

    int childPos = -1;
    if (parent->GetObjectInfo()->IsSubclassOf(wxT("sizeritembase"))) {
        childPos = (int)grandParent->GetChildPosition(parent);
        parent = grandParent;
    }

    // Must first cut the old object in case it is the only allowable object
    PObjectBase clipboard = m_clipboard;
    CutObject(obj);

    // Create the wxBoxSizer
    PObjectBase newSizer = m_objDb->CreateObject("wxBoxSizer", parent);

    if (newSizer) {
        PCommand cmd(new InsertObjectCmd(this, newSizer, parent, childPos));
        Execute(cmd);

        if (newSizer->GetObjectTypeName() == wxT("sizeritem"))
            newSizer = newSizer->GetChild(0);

        PasteObject(newSizer);
        m_clipboard = clipboard;

        // NotifyProjectRefresh();
    } else {
        Undo();
        m_clipboard = clipboard;
    }
}

void ApplicationData::ShowXrcPreview()
{
    PObjectBase form = GetSelectedForm();

    if (form == NULL) {
        wxMessageBox(wxT("Please select a form and try again."), wxT("XRC Preview"), wxICON_ERROR);
        return;
    } else if (form->GetPropertyAsInteger(wxT("aui_managed")) != 0) {
        wxMessageBox(wxT("XRC preview doesn't support AUI-managed frames."), wxT("XRC Preview"), wxICON_ERROR);
        return;
    }

    XRCPreview::Show(form, GetProjectPath());
}

bool ApplicationData::CanPasteObject()
{
    PObjectBase obj = GetSelectedObject();

    if (obj && obj->GetObjectTypeName() != wxT("project"))
        return (m_clipboard != NULL);

    return false;
}

bool ApplicationData::CanCopyObject()
{
    PObjectBase obj = GetSelectedObject();

    if (obj && obj->GetObjectTypeName() != wxT("project"))
        return true;

    return false;
}

bool ApplicationData::IsModified()
{
    return m_modFlag;
}

void ApplicationData::SetDarkMode(bool darkMode)
{
    m_darkMode = darkMode;
}

bool ApplicationData::IsDarkMode() const
{
    return m_darkMode;
}

void ApplicationData::Execute(PCommand cmd)
{
    m_modFlag = true;
    m_cmdProc.Execute(cmd);
}

//////////////////////////////////////////////////////////////////////////////
void ApplicationData::AddHandler(wxEvtHandler* handler)
{
    m_handlers.push_back(handler);
}

void ApplicationData::RemoveHandler(wxEvtHandler* handler)
{
    for (HandlerVector::iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        if (*it == handler) {
            m_handlers.erase(it);
            break;
        }
    }
}

void ApplicationData::NotifyEvent(wxFBEvent& event, bool forcedelayed)
{

    if (!forcedelayed) {
        LogDebug("event: %s", event.GetEventName());

        std::vector<wxEvtHandler*>::iterator handler;

        for (handler = m_handlers.begin(); handler != m_handlers.end(); handler++) { (*handler)->ProcessEvent(event); }
    } else {
        LogDebug("Pending event: %s", event.GetEventName());

        std::vector<wxEvtHandler*>::iterator handler;

        for (handler = m_handlers.begin(); handler != m_handlers.end(); handler++) {
            (*handler)->AddPendingEvent(event);
        }
    }
}

void ApplicationData::NotifyProjectLoaded()
{
    wxFBEvent event(wxEVT_FB_PROJECT_LOADED);
    NotifyEvent(event);
}

void ApplicationData::NotifyProjectSaved()
{
    wxFBEvent event(wxEVT_FB_PROJECT_SAVED);
    NotifyEvent(event);
}

void ApplicationData::NotifyObjectExpanded(PObjectBase obj)
{
    wxFBObjectEvent event(wxEVT_FB_OBJECT_EXPANDED, obj);
    NotifyEvent(event);
}

void ApplicationData::NotifyObjectSelected(PObjectBase obj, bool force)
{
    wxFBObjectEvent event(wxEVT_FB_OBJECT_SELECTED, obj);
    if (force)
        event.SetString(wxT("force"));

    NotifyEvent(event, false);
}

void ApplicationData::NotifyObjectCreated(PObjectBase obj)
{
    wxFBObjectEvent event(wxEVT_FB_OBJECT_CREATED, obj);
    NotifyEvent(event, false);
}

void ApplicationData::NotifyObjectRemoved(PObjectBase obj)
{
    wxFBObjectEvent event(wxEVT_FB_OBJECT_REMOVED, obj);
    NotifyEvent(event, false);
}

void ApplicationData::NotifyPropertyModified(PProperty prop)
{
    wxFBPropertyEvent event(wxEVT_FB_PROPERTY_MODIFIED, prop);
    NotifyEvent(event);
}

void ApplicationData::NotifyEventHandlerModified(PEvent evtHandler)
{
    wxFBEventHandlerEvent event(wxEVT_FB_EVENT_HANDLER_MODIFIED, evtHandler);
    NotifyEvent(event);
}

void ApplicationData::NotifyCodeGeneration(bool panelOnly, bool forcedelayed)
{
    wxFBEvent event(wxEVT_FB_CODE_GENERATION);

    // Using the previously unused Id field in the event to carry a boolean
    event.SetId((panelOnly ? 1 : 0));

    NotifyEvent(event, forcedelayed);
}

void ApplicationData::NotifyProjectRefresh()
{
    wxFBEvent event(wxEVT_FB_PROJECT_REFRESH);
    NotifyEvent(event);
}

bool ApplicationData::VerifySingleInstance(const wxString& file, bool switchTo)
{
    return m_ipc->VerifySingleInstance(file, switchTo);
}

wxString ApplicationData::GetPathProperty(const wxString& pathName)
{
    PObjectBase project = GetProjectData();
    wxFileName path;
    // Get the output path
    PProperty ppath = project->GetProperty(pathName);

    if (ppath) {
        wxString pathEntry = ppath->GetValue();

        if (pathEntry.empty()) {
            THROW_WXFBEX(
              wxT("You must set the \"") + pathName +
              wxT("\" property of the project to a valid path for output files"));
        }

        path = wxFileName::DirName(pathEntry);

        if (!path.IsAbsolute()) {
            wxString projectPath = AppData()->GetProjectPath();

            if (projectPath.empty()) {
                THROW_WXFBEX(wxT("You must save the project when using a relative path for output files"));
            }

            path =
              wxFileName(projectPath + wxFileName::GetPathSeparator() + pathEntry + wxFileName::GetPathSeparator());

            path.Normalize(wxPATH_NORM_ABSOLUTE | wxPATH_NORM_DOTS);

            // this approach is probably incorrect if the fb project is located under a symlink
            /*path.SetCwd( projectPath );
            path.MakeAbsolute();*/
        }
    }

    if (!path.DirExists()) {
        THROW_WXFBEX(
          wxT("Invalid Path: ") << path.GetPath()
                                << wxT("\nYou must set the \"") + pathName +
                                     wxT("\" property of the project to a valid path for output files"));
    }

    return path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
}

wxString ApplicationData::GetOutputPath()
{
    return GetPathProperty(wxT("path"));
}

wxString ApplicationData::GetEmbeddedFilesOutputPath()
{
    return GetPathProperty(wxT("embedded_files_path"));
}
