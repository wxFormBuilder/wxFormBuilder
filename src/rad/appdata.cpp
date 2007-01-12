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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   José Antonio Hurtado - joseantonio.hurtado@gmail.com
//   Juan Antonio Ortega  - jortegalalmolda@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

#include "appdata.h"
#include "model/objectbase.h"
#include "utils/typeconv.h"
#include "utils/debug.h"
#include "codegen/codegen.h"
#include "rad/cpppanel/cpppanel.h"
#include "bitmaps.h"
#include "rad/wxfbevent.h"
#include "codegen/xrccg.h"
#include "wxfbmanager.h"
#include "utils/wxfbexception.h"
#include "utils/stringutils.h"
#include "utils/wxfbipc.h"

#include <ticpp.h>
#include <set>
#include <wx/tokenzr.h>
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/xrc/xmlres.h>


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
	void DoExecute();
	void DoRestore();

public:
	ExpandObjectCmd( PObjectBase object, bool expand );
};

/**
* Comando para insertar un objeto en el árbol.
*/
class InsertObjectCmd : public Command
{
private:
	ApplicationData *m_data;
	PObjectBase m_parent;
	PObjectBase m_object;
	int m_pos;
	PObjectBase m_oldSelected;


protected:
	void DoExecute();
	void DoRestore();

public:
	InsertObjectCmd(ApplicationData *data, PObjectBase object, PObjectBase parent, int pos = -1);
};

/**
* Comando para borrar un objeto.
*/
class RemoveObjectCmd : public Command
{
private:
	ApplicationData *m_data;
	PObjectBase m_parent;
	PObjectBase m_object;
	int m_oldPos;
	PObjectBase m_oldSelected;

protected:
	void DoExecute();
	void DoRestore();

public:
	RemoveObjectCmd(ApplicationData *data,PObjectBase object);
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
	void DoExecute();
	void DoRestore();

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
	void DoExecute();
	void DoRestore();

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
	void DoExecute();
	void DoRestore();

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
	ApplicationData *m_data;
	//PObjectBase m_clipboard;
	PObjectBase m_parent;
	PObjectBase m_object;
	int m_oldPos;
	PObjectBase m_oldSelected;

protected:
	void DoExecute();
	void DoRestore();

public:
	CutObjectCmd(ApplicationData *data, PObjectBase object);
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
protected:
	void DoExecute();
	void DoRestore();

public:
	ReparentObjectCmd (PObjectBase sizeritem, PObjectBase sizer);
};

///////////////////////////////////////////////////////////////////////////////
// Implementación de los Comandos
///////////////////////////////////////////////////////////////////////////////
ExpandObjectCmd::ExpandObjectCmd( PObjectBase object, bool expand )
: m_object(object), m_expand(expand)
{
}

void ExpandObjectCmd::DoExecute()
{
	m_object->SetExpanded( m_expand );
}

void ExpandObjectCmd::DoRestore()
{
	m_object->SetExpanded( !m_expand );
}

InsertObjectCmd::InsertObjectCmd(ApplicationData *data, PObjectBase object,
								 PObjectBase parent, int pos)
								 : m_data(data), m_parent(parent), m_object(object), m_pos(pos)
{
	m_oldSelected = data->GetSelectedObject();
}

void InsertObjectCmd::DoExecute()
{
	m_parent->AddChild(m_object);
	m_object->SetParent(m_parent);

	if (m_pos >= 0)
		m_parent->ChangeChildPosition(m_object,m_pos);
}

void InsertObjectCmd::DoRestore()
{
	m_parent->RemoveChild(m_object);
	m_object->SetParent(PObjectBase());
	m_data->SelectObject(m_oldSelected);
}

//-----------------------------------------------------------------------------

RemoveObjectCmd::RemoveObjectCmd(ApplicationData *data, PObjectBase object)
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
}

void RemoveObjectCmd::DoRestore()
{
	m_parent->AddChild(m_object);
	m_object->SetParent(m_parent);

	// restauramos la posición
	m_parent->ChangeChildPosition(m_object,m_oldPos);
	m_data->SelectObject(m_oldSelected);
}

//-----------------------------------------------------------------------------

ModifyPropertyCmd::ModifyPropertyCmd(PProperty prop, wxString value)
: m_property(prop), m_newValue(value)
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

ModifyEventHandlerCmd::ModifyEventHandlerCmd(PEvent event, wxString value)
: m_event(event), m_newValue(value)
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
	if (m_oldPos != m_newPos)
	{
		PObjectBase parent (m_object->GetParent());
		parent->ChangeChildPosition(m_object,m_newPos);
	}
}

void ShiftChildCmd::DoRestore()
{
	if (m_oldPos != m_newPos)
	{
		PObjectBase parent (m_object->GetParent());
		parent->ChangeChildPosition(m_object,m_oldPos);
	}
}

//-----------------------------------------------------------------------------

CutObjectCmd::CutObjectCmd(ApplicationData *data, PObjectBase object)
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
	//m_clipboard = m_data->GetClipboardObject();

	m_data->SetClipboardObject(m_object);
	m_parent->RemoveChild(m_object);
	m_object->SetParent(PObjectBase());
}

void CutObjectCmd::DoRestore()
{
	// reubicamos el objeto donde estaba
	m_parent->AddChild(m_object);
	m_object->SetParent(m_parent);
	m_parent->ChangeChildPosition(m_object,m_oldPos);

	// restauramos el clipboard
	//m_data->SetClipboardObject(m_clipboard);
	m_data->SetClipboardObject(PObjectBase());
	m_data->SelectObject(m_oldSelected);
}

//-----------------------------------------------------------------------------

ReparentObjectCmd ::ReparentObjectCmd (PObjectBase sizeritem, PObjectBase sizer)
{
	m_sizeritem = sizeritem;
	m_sizer = sizer;
	m_oldSizer = m_sizeritem->GetParent();
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
}

///////////////////////////////////////////////////////////////////////////////
// ApplicationData
///////////////////////////////////////////////////////////////////////////////

ApplicationData* ApplicationData::s_instance = NULL;

ApplicationData* ApplicationData::Get(const wxString &rootdir)
{
  if (!s_instance)
    s_instance = new ApplicationData(rootdir);

  return s_instance;
}

void ApplicationData::Destroy()
{
  if (s_instance)
    delete s_instance;

  s_instance = NULL;
}

void ApplicationData::Initialize()
{
	ApplicationData* appData = ApplicationData::Get();
	appData->LoadApp();
}

ApplicationData::ApplicationData(const wxString &rootdir)
:
m_rootDir( rootdir ),
m_objDb( new ObjectDatabase() ),
m_manager( new wxFBManager ),
m_ipc( new wxFBIPC ),
m_fbpVerMajor( 1 ),
m_fbpVerMinor( 5 )
{
	m_objDb->SetXmlPath( m_rootDir + wxFILE_SEP_PATH + wxT("xml") + wxFILE_SEP_PATH ) ;
	m_objDb->SetIconPath( m_rootDir + wxFILE_SEP_PATH + wxT("resources") + wxFILE_SEP_PATH + wxT("icons") + wxFILE_SEP_PATH );
	m_objDb->SetPluginPath( m_rootDir + wxFILE_SEP_PATH + wxT("plugins") + wxFILE_SEP_PATH ) ;
}

void ApplicationData::LoadApp()
{
	wxString bitmapPath = m_objDb->GetXmlPath() + wxT("icons.xml");
	AppBitmaps::LoadBitmaps( bitmapPath, m_objDb->GetIconPath() );
	m_objDb->LoadObjectTypes();
	m_objDb->LoadPlugins( m_manager );
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
	if (m_selObj->GetObjectTypeName() == wxT("form") )
		return m_selObj;
	else
		return m_selObj->FindNearAncestor( wxT("form") );
}


PObjectBase ApplicationData::GetProjectData()
{
	return m_project;
}

void ApplicationData::BuildNameSet(PObjectBase obj, PObjectBase top, std::set< wxString >& name_set)
{
	if (obj != top)
	{
		PProperty nameProp = top->GetProperty( wxT("name") );
		if (nameProp)
			name_set.insert(nameProp->GetValue());
	}

	for (unsigned int i=0; i< top->GetChildCount(); i++)
		BuildNameSet(obj, top->GetChild(i), name_set);
}

void ApplicationData::ResolveNameConflict(PObjectBase obj)
{
	while (obj && obj->GetObjectInfo()->GetObjectType()->IsItem())
	{
		if (obj->GetChildCount() > 0)
			obj = obj->GetChild(0);
		else
			return;
	}

	PProperty nameProp = obj->GetProperty( wxT("name") );
	if (!nameProp)
		return;

	// Save the original name for use later.
	wxString originalName = nameProp->GetValue();

	// el nombre no puede estar repetido dentro del mismo form
	PObjectBase top = obj->FindNearAncestor( wxT("form") );
	if (!top)
		top = m_project; // el objeto es un form.

	// construimos el conjunto de nombres
	std::set<wxString> name_set;
	BuildNameSet(obj, top, name_set);

	// comprobamos si hay conflicto
	std::set<wxString>::iterator it = name_set.find( originalName );

	int i = 0;
	wxString name = originalName; // The name that gets incremented.

	while ( it != name_set.end() )
	{
		i++;
		name = wxString::Format( wxT("%s%i"), originalName.c_str(), i );
		it = name_set.find( name );
	}

	nameProp->SetValue( name );
}

void ApplicationData::ResolveSubtreeNameConflicts(PObjectBase obj, PObjectBase topObj)
{
	if (!topObj)
	{
		topObj = obj->FindNearAncestor( wxT("form") );
		if (!topObj)
			topObj = m_project; // object is the project
	}

	// Ignore item objects
	while (obj && obj->GetObjectInfo()->GetObjectType()->IsItem())
	{
		if (obj->GetChildCount() > 0)
			obj = obj->GetChild(0);
		else
			return; // error
	}

	// Resolve a possible name conflict
	ResolveNameConflict(obj);

	// Recurse through all children
	for (unsigned int i=0 ; i < obj->GetChildCount() ; i++)
		ResolveSubtreeNameConflicts(obj->GetChild(i), topObj);
}

int ApplicationData::CalcPositionOfInsertion(PObjectBase selected,PObjectBase parent)
{
	int pos = -1;

	if (parent && selected)
	{
		PObjectBase parentSelected = selected->GetParent();
		while (parentSelected && parentSelected != parent)
		{
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
	if (!obj->GetObjectInfo()->GetObjectType()->IsItem())
	{
		bool emptyItem = true;

		// esto es un algoritmo ineficiente pero "seguro" con los índices
		while (emptyItem)
		{
			emptyItem = false;
			for (unsigned int i=0; !emptyItem && i<obj->GetChildCount(); i++)
			{
				PObjectBase child = obj->GetChild(i);
				if (child->GetObjectInfo()->GetObjectType()->IsItem() &&
					child->GetChildCount() == 0)
				{
					obj->RemoveChild(child); // borramos el item
					child->SetParent(PObjectBase());

					emptyItem = true;        // volvemos a recorrer
					wxString msg;
					msg.Printf(wxT("Empty item removed under %s"),obj->GetPropertyAsString(wxT("name")).c_str());
					wxLogWarning(msg);
				}
			}
		}
	}

	for (unsigned int i=0; i<obj->GetChildCount() ; i++)
		RemoveEmptyItems(obj->GetChild(i));
}

PObjectBase  ApplicationData::SearchSizerInto(PObjectBase obj)
{
	PObjectBase theSizer;

	if (obj->GetObjectTypeName() == wxT("sizer") )
		theSizer = obj;
	else
	{
		for (unsigned int i = 0; !theSizer && i < obj->GetChildCount(); i++)
			theSizer = SearchSizerInto(obj->GetChild(i));
	}

	return theSizer;
}

///////////////////////////////////////////////////////////////////////////////

void ApplicationData::ExpandObject( PObjectBase obj, bool expand )
{
	PCommand command( new ExpandObjectCmd( obj, expand ) );
	Execute( command );
	NotifyObjectExpanded( obj );
}

void ApplicationData::SelectObject(PObjectBase obj, bool force /*= false*/)
{
	if ( ( obj == m_selObj ) && !force )
	{
		return;
	}

	Debug::Print( wxT("Object Selected!") );
	m_selObj = obj;
	/*
	if (obj->GetObjectType() != T_FORM)
	{
	m_selForm = shared_dynamic_cast<FormObject>(obj->FindNearAncestor(T_FORM));
	}
	else
	m_selForm = shared_dynamic_cast<FormObject>(obj);*/

	NotifyObjectSelected(obj);
}

void ApplicationData::CreateObject(wxString name)
{
	try
	{
		Debug::Print( wxT("ApplicationData::CreateObject] New %s"),name.c_str());

		PObjectBase parent = GetSelectedObject();
		PObjectBase obj;
		if (parent)
		{
			bool created = false;

			// Para que sea más práctico, si el objeto no se puede crear debajo
			// del objeto seleccionado vamos a intentarlo en el padre del seleccionado
			// y seguiremos subiendo hasta que ya no podamos crear el objeto.
			while (parent && !created)
			{
				// además, el objeto se insertará a continuación del objeto seleccionado
				obj = m_objDb->CreateObject( _STDSTR(name),parent);

				if (obj)
				{
					int pos = CalcPositionOfInsertion(GetSelectedObject(),parent);

					PCommand command(new InsertObjectCmd(this,obj,parent,pos));
					Execute(command); //m_cmdProc.Execute(command);
					created = true;
					ResolveNameConflict(obj);
				}
				else
				{
					// lo vamos a seguir intentando con el padre, pero cuidado, el padre
					// no puede ser un item!
					parent = parent->GetParent();
					while (parent && parent->GetObjectInfo()->GetObjectType()->IsItem())
						parent = parent->GetParent();
				}
			}
		}

		NotifyObjectCreated(obj);

		// Seleccionamos el objeto, si este es un item entonces se selecciona
		// el objeto contenido. ¿Tiene sentido tener un item debajo de un item?
		while (obj && obj->GetObjectInfo()->GetObjectType()->IsItem())
			obj = ( obj->GetChildCount() > 0 ? obj->GetChild(0) : PObjectBase());

		if (obj)
			SelectObject(obj);
	}
	catch ( wxFBException& ex )
	{
		wxLogError( ex.what() );
	}
}

void ApplicationData::RemoveObject(PObjectBase obj)
{
	DoRemoveObject(obj,false);
}

void ApplicationData::CutObject(PObjectBase obj)
{
	DoRemoveObject(obj,true);
}

void ApplicationData::DoRemoveObject(PObjectBase obj, bool cutObject)
{
	// Nota:
	//  cuando se va a eliminar un objeto es importante que no se dejen
	//  nodos ficticios ("items") en las hojas del árbol.
	PObjectBase parent = obj->GetParent();
	if (parent)
	{
		while (parent && parent->GetObjectInfo()->GetObjectType()->IsItem())
		{
			obj = parent;
			parent = obj->GetParent();
		}

		if (cutObject)
		{
			m_copyOnPaste = false;
			PCommand command(new CutObjectCmd(this, obj));
			Execute(command); //m_cmdProc.Execute(command);
		}
		else
		{
			PCommand command(new RemoveObjectCmd(this,obj));
			Execute(command); //m_cmdProc.Execute(command);
		}

		NotifyObjectRemoved(obj);

		// "parent" será el nuevo objeto seleccionado tras eliminar obj.
		SelectObject(parent);
	}
	else
	{
		if (obj->GetObjectTypeName()!=wxT("project") )
			assert(false);
	}

	CheckProjectTree(m_project);
}

void ApplicationData::CopyObject(PObjectBase obj)
{
	m_copyOnPaste = true;

	// Hacemos una primera copia del objeto, ya que si despues de copiar
	// el objeto se modificasen las propiedades, dichas modificaciones se verian
	// reflejadas en la copia.
	m_clipboard = m_objDb->CopyObject(obj);

	CheckProjectTree(m_project);
}

void ApplicationData::PasteObject(PObjectBase parent)
{
	try
	{
		if (m_clipboard)
		{
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

			PObjectBase obj =
				m_objDb->CreateObject(_STDSTR(m_clipboard->GetObjectInfo()->GetClassName() ), parent);

			int pos = -1;

			if (!obj)
			{
				// si no se ha podido crear el objeto vamos a intentar crearlo colgado
				// del padre de "parent" y además vamos a insertarlo en la posición
				// siguiente a "parent"
				PObjectBase selected = parent;
				parent = selected->GetParent();
				while (parent && parent->GetObjectInfo()->GetObjectType()->IsItem())
				{
					selected = parent;
					parent = selected->GetParent();
				}

				if (parent)
				{
					obj = m_objDb->CreateObject( _STDSTR( m_clipboard->GetObjectInfo()->GetClassName() ), parent);

					if (obj)
						pos = CalcPositionOfInsertion(selected,parent);
				}
			}

			if (obj)
			{
				PObjectBase clipboard(m_clipboard);
				if (m_copyOnPaste)
					clipboard = m_objDb->CopyObject(m_clipboard);

				PObjectBase aux = obj;
				while (aux && aux->GetObjectInfo() != clipboard->GetObjectInfo())
					aux = ( aux->GetChildCount() > 0 ? aux->GetChild(0) : PObjectBase());

				if (aux && aux != obj)
				{
					// sustituimos aux por clipboard
					PObjectBase auxParent = aux->GetParent();
					auxParent->RemoveChild(aux);
					aux->SetParent( PObjectBase() );

					auxParent->AddChild(clipboard);
					clipboard->SetParent(auxParent);
				}
				else
					obj = clipboard;

				// y finalmente insertamos en el arbol
				PCommand command(new InsertObjectCmd(this,obj,parent,pos));
				Execute(command); //m_cmdProc.Execute(command);

				if (!m_copyOnPaste)
					m_clipboard.reset();

				ResolveSubtreeNameConflicts(obj);

				NotifyProjectRefresh();

				// vamos a mantener seleccionado el nuevo objeto creado
				// pero hay que tener en cuenta que es muy probable que el objeto creado
				// sea un "item"
				while (obj && obj->GetObjectInfo()->GetObjectType()->IsItem())
				{
					assert(obj->GetChildCount() > 0);
					obj = obj->GetChild(0);
				}

				SelectObject(obj);
			}
		}

		CheckProjectTree(m_project);
	}
	catch ( wxFBException& ex )
	{
		wxLogError( ex.what() );
	}
}

void ApplicationData::InsertObject(PObjectBase obj, PObjectBase parent)
{
	// FIXME! comprobar obj se puede colgar de parent
	//  if (parent->GetObjectInfo()->GetObjectType()->FindChildType(
	//    obj->GetObjectInfo()->GetObjectType()))
	//  {
	PCommand command(new InsertObjectCmd(this,obj,parent));
	Execute(command); //m_cmdProc.Execute(command);
	NotifyProjectRefresh();
	//  }
}

void ApplicationData::MergeProject(PObjectBase project)
{
	// FIXME! comprobar obj se puede colgar de parent
	for (unsigned int i=0; i<project->GetChildCount(); i++)
	{
		//m_project->AddChild(project->GetChild(i));
		//project->GetChild(i)->SetParent(m_project);

		PObjectBase child = project->GetChild(i);
		RemoveEmptyItems(child);

		InsertObject(child,m_project);
	}
	NotifyProjectRefresh();
}

void ApplicationData::ModifyProperty(PProperty prop, wxString str)
{
	PObjectBase object = prop->GetObject();

	if ( str != prop->GetValue())
	{
		PCommand command( new ModifyPropertyCmd( prop, str ) );
		Execute(command); //m_cmdProc.Execute(command);

		NotifyPropertyModified(prop);
	}
}

void ApplicationData::ModifyEventHandler(PEvent evt, wxString value)
{
  PObjectBase object = evt->GetObject();

	if ( value != evt->GetValue())
	{
		PCommand command( new ModifyEventHandlerCmd( evt, value ) );
		Execute(command); //m_cmdProc.Execute(command);

		NotifyEventHandlerModified(evt);
	}
}

void ApplicationData::SaveProject( const wxString& filename )
{
	// Make sure this file is not already open
	if ( !m_ipc->VerifySingleInstance( filename, false ) )
	{
		if ( wxYES == wxMessageBox( wxT("You cannot save over a file that is currently open in another instance.\nWould you like to switch to that instance?"),
										wxT("Open in Another Instance"), wxICON_QUESTION | wxYES_NO, wxTheApp->GetTopWindow() ) )
		{
			m_ipc->VerifySingleInstance( filename, true );
		}
		return;
	}

	try
	{
		ticpp::Document doc;
		m_project->Serialize( &doc );
		doc.SaveFile( std::string( filename.mb_str( wxConvFile ) ) );

		m_projectFile = filename;
		SetProjectPath( ::wxPathOnly( filename ) );
		m_modFlag = false;
		NotifyProjectSaved();
	}
	catch ( ticpp::Exception& ex )
	{
		wxString message = _WXSTR( ex.m_details );
		if ( message.empty() )
		{
			message = wxString( ex.m_details.c_str(), wxConvFile );
		}
		THROW_WXFBEX( message )
	}
}

bool ApplicationData::LoadProject(const wxString &file)
{
	Debug::Print( wxT("LOADING") );

	if ( !wxFileName::FileExists( file ) )
	{
		wxLogError( wxT("This file does not exist: %s"), file.c_str() );
		return false;
	}

	if ( !m_ipc->VerifySingleInstance( file ) )
	{
		return false;
	}

	bool result = false;

	TiXmlDocument doc = TiXmlDocument();
	if ( doc.LoadFile( file.mb_str( wxConvFile ) ) )
	{
		TiXmlNode* root = doc.RootElement();
		if ( NULL == root )
		{
			return false;
		}

		m_objDb->ResetObjectCounters();

		int fbpVerMajor = 0;
		int fbpVerMinor = 0;

		if ( root->Value() != std::string("object") )
		{
			TiXmlElement* fileVersion = root->FirstChildElement("FileVersion");
			if ( NULL != fileVersion )
			{
				int rc = fileVersion->QueryIntAttribute( "major", &fbpVerMajor );
				if ( rc != TIXML_SUCCESS )
				{
					fbpVerMajor = 0;
				}

				rc = fileVersion->QueryIntAttribute( "minor", &fbpVerMinor );
				if ( rc != TIXML_SUCCESS )
				{
					fbpVerMinor = 0;
				}
			}
		}

		bool older = false;
		bool newer = false;

		if ( fbpVerMajor < m_fbpVerMajor )
		{
			older = true;
		}
		else if ( fbpVerMajor > m_fbpVerMajor )
		{
			newer = true;
		}
		else
		{
			if ( fbpVerMinor < m_fbpVerMinor )
			{
				older = true;
			}
			else if ( fbpVerMinor > m_fbpVerMinor )
			{
				newer = true;
			}
		}

		if ( newer )
		{
			wxMessageBox( wxT("This project file is newer than this version of wxFormBuilder.\n")
						  wxT("It cannot be opened.\n\n")
						  wxT("Please download an updated version from http://www.wxFormBuilder.org"), _("New Version"), wxICON_ERROR );
			return false;
		}

		if ( older )
		{
			if ( wxYES == wxMessageBox( wxT("This project file is not of the current version.\n")
										wxT("Would you to attempt automatic conversion?\n\n")
										wxT("NOTE: This will modify your project file on disk!"), _("Old Version"), wxYES_NO ) )
			{
				// we make a backup of the project
				::wxCopyFile( file, file + wxT(".bak") );

				if ( !ConvertProject( file, fbpVerMajor, fbpVerMinor ) )
				{
					wxLogError( wxT("Unable to convert project") );
					return false;
				}

				if ( doc.LoadFile( file.mb_str( wxConvFile ) ) )
					root = doc.RootElement();
				else
					return false;
			}
			else
			{
				return false;
			}
		}

		TiXmlElement* object = root->FirstChildElement("object");
		if ( NULL == object )
		{
			return false;
		}

		PObjectBase proj;
		try
		{
			proj = m_objDb->CreateObject(object);
		}
		catch ( wxFBException& ex )
		{
			wxLogError( ex.what() );
			return false;
		}

		if (proj && proj->GetObjectTypeName() == wxT("project") )
		{
			PObjectBase old_proj = m_project;
			//m_project = shared_dynamic_cast<ProjectObject>(proj);
			m_project = proj;
			m_selObj = m_project;
			result = true;
			m_modFlag = false;
			m_cmdProc.Reset();
			m_projectFile = file;
			SetProjectPath(::wxPathOnly(file));
			NotifyProjectLoaded();
			NotifyProjectRefresh();
		}
	}

	return result;
}

bool ApplicationData::ConvertProject( const wxString& path, int fileMajor, int fileMinor )
{
	try
	{
		// Version prior to 1 were not UTF-8
		if ( fileMajor < 1 )
		{
			try
			{
				XMLUtils::ConvertAndAddDeclaration( path, wxFONTENCODING_ISO8859_1, false );
			}
			catch ( wxFBException& ex )
			{
				wxLogError( ex.what() );
				return false;
			}
		}

		ticpp::Document doc( std::string( path.mb_str( wxConvFile ) ) );
		doc.LoadFile();
		ticpp::Element* root = doc.FirstChildElement();
		if ( root->Value() == std::string("object") )
		{
			ConvertProjectProperties( root, path, fileMajor, fileMinor );
			ConvertObject( root, fileMajor, fileMinor );

			// Create a clone of now-converted object tree, so it can be linked
			// underneath the root element
			std::auto_ptr< ticpp::Node > objectTree = root->Clone();

			// Clear the document to add the declatation and the root element
			doc.Clear();

			// Add the declaration
			doc.LinkEndChild( new ticpp::Declaration( "1.0", "UTF-8", "yes" ) );

			// Add the root element, with file version
			ticpp::Element* newRoot = new ticpp::Element( "wxFormBuilder_Project" );

			ticpp::Element* fileVersion = new ticpp::Element( "FileVersion" );
			fileVersion->SetAttribute( "major", m_fbpVerMajor );
			fileVersion->SetAttribute( "minor", m_fbpVerMinor );

			newRoot->LinkEndChild( fileVersion );

			// Add the object tree
			newRoot->LinkEndChild( objectTree.release() );

			doc.LinkEndChild( newRoot );
		}
		else
		{
			// Handle project separately because it only occurs once
			ticpp::Element* project = root->FirstChildElement( "object" );
			ConvertProjectProperties( project, path, fileMajor, fileMinor );
			ConvertObject( project, fileMajor, fileMinor );
			ticpp::Element* fileVersion = root->FirstChildElement( "FileVersion" );
			fileVersion->SetAttribute( "major", m_fbpVerMajor );
			fileVersion->SetAttribute( "minor", m_fbpVerMinor );
		}
		doc.SaveFile();
	}
	catch( ticpp::Exception& ex )
	{
		wxLogError( _WXSTR( ex.m_details ) );
		return false;
	}
	return true;
}

void ApplicationData::ConvertProjectProperties( ticpp::Element* project, const wxString& path, int fileMajor, int fileMinor )
{
	// Ensure that this is the "project" element
	std::string objClass;
	project->GetAttribute( "class", &objClass );

	if ( objClass != "Project" )
	{
		return;
	}

	if ( fileMajor < 1 || ( 1 == fileMajor && fileMinor < 5 ) )
	{
		// Find the user_headers property
		std::set< std::string > oldProps;
		std::set< ticpp::Element* > newProps;

		oldProps.insert( "user_headers" );
		GetPropertiesToConvert( project, oldProps, &newProps );

		std::string user_headers;
		if ( !newProps.empty() )
		{
			user_headers = (*newProps.begin())->GetText( false );
		}

		if ( !user_headers.empty() )
		{
			wxString 	msg  = _("The \"user_headers\" property has been removed.\n");
						msg += _("Its purpose was to provide a place to include precompiled headers or\n");
						msg += _("headers for subclasses.\n");
						msg += _("There is now a \"precompiled_header\" property and a \"header\" subitem\n");
						msg += _("on the subclass property.\n\n");
						msg += _("Would you like the current value of the \"user_headers\" property to be saved\n");
						msg += _("to a file so that you can distribute the headers among the \"precompiled_header\"\n");
						msg += _("and \"subclass\" properties\?");
			if ( wxYES == wxMessageBox( msg, _("The \"user_headers\" property has been removed"), wxICON_QUESTION | wxYES_NO | wxYES_DEFAULT, wxTheApp->GetTopWindow() ) )
			{
				wxString name;
				wxFileName::SplitPath( path, NULL, NULL, &name, NULL );
				wxFileDialog dialog( wxTheApp->GetTopWindow(), _("Save \"user_headers\""), ::wxPathOnly(path),
						   name + wxT("_user_headers.txt"), wxT("All files (*.*)|*.*"), wxSAVE );

				if ( dialog.ShowModal() == wxID_OK )
				{
					wxString wxuser_headers = _WXSTR(user_headers);
					wxString filename = dialog.GetPath();
					bool success = false;
					wxFFile output( filename, wxT("w") );
					if ( output.IsOpened() )
					{
						if ( output.Write( wxuser_headers ) )
						{
							output.Close();
							success = true;
						}
					}

					if ( !success )
					{
						wxLogError( _("Unable to open %s for writing.\nUser Headers:\n%s"), filename.c_str(), wxuser_headers.c_str() );
					}
				}
			}
		}
	}
}

void ApplicationData::ConvertObject( ticpp::Element* parent, int fileMajor, int fileMinor )
{
	ticpp::Iterator< ticpp::Element > object( "object" );
	for ( object = parent->FirstChildElement( "object", false ); object != object.end(); ++object )
	{
		ConvertObject( object.Get(), fileMajor, fileMinor );
	}

	// Reusable sets to find properties with
	std::set< std::string > oldProps;
	std::set< ticpp::Element* > newProps;
	std::set< ticpp::Element* >::iterator newProp;

	// Get the class of the object
	std::string objClass;
	parent->GetAttribute( "class", &objClass );

	/* The changes below will convert an unversioned file to version 1.3 */

	if ( fileMajor < 1 || ( 1 == fileMajor && fileMinor < 3 ) )
	{
		// The property 'option' became 'proportion'
		if ( objClass == "sizeritem" || objClass == "spacer" )
		{
			oldProps.clear();
			oldProps.insert( "option" );
			GetPropertiesToConvert( parent, oldProps, &newProps );
			if ( !newProps.empty() )
			{
				// One in, one out
				(*newProps.begin())->SetAttribute( "name", "proportion" );
			}
		}

		// The 'style' property used to have both wxWindow styles and the styles of the specific controls
		// now it only has the styles of the specfic controls, and wxWindow styles are saved in window_style
		// This also applies to 'extra_style', which was once combined with 'style'.
		// And they were named 'WindowStyle' and one point, too...

		std::set< wxString > windowStyles;
		windowStyles.insert( wxT("wxSIMPLE_BORDER") );
		windowStyles.insert( wxT("wxDOUBLE_BORDER") );
		windowStyles.insert( wxT("wxSUNKEN_BORDER") );
		windowStyles.insert( wxT("wxRAISED_BORDER") );
		windowStyles.insert( wxT("wxSTATIC_BORDER") );
		windowStyles.insert( wxT("wxNO_BORDER") );
		windowStyles.insert( wxT("wxTRANSPARENT_WINDOW") );
		windowStyles.insert( wxT("wxTAB_TRAVERSAL") );
		windowStyles.insert( wxT("wxWANTS_CHARS") );
		windowStyles.insert( wxT("wxVSCROLL") );
		windowStyles.insert( wxT("wxHSCROLL") );
		windowStyles.insert( wxT("wxALWAYS_SHOW_SB") );
		windowStyles.insert( wxT("wxCLIP_CHILDREN") );
		windowStyles.insert( wxT("wxFULL_REPAINT_ON_RESIZE") );

		// Transfer the window styles
		oldProps.clear();
		oldProps.insert( "style" );
		oldProps.insert( "WindowStyle" );
		GetPropertiesToConvert( parent, oldProps, &newProps );
		for ( newProp = newProps.begin(); newProp != newProps.end(); ++newProp )
		{
			TransferOptionList( *newProp, &windowStyles, "window_style" );
		}


		std::set< wxString > extraWindowStyles;
		extraWindowStyles.insert( wxT("wxWS_EX_VALIDATE_RECURSIVELY") );
		extraWindowStyles.insert( wxT("wxWS_EX_BLOCK_EVENTS") );
		extraWindowStyles.insert( wxT("wxWS_EX_TRANSIENT") );
		extraWindowStyles.insert( wxT("wxWS_EX_PROCESS_IDLE") );
		extraWindowStyles.insert( wxT("wxWS_EX_PROCESS_UI_UPDATES") );

		// Transfer the window extra styles
		oldProps.clear();
		oldProps.insert( "style" );
		oldProps.insert( "extra_style" );
		oldProps.insert( "WindowStyle" );
		GetPropertiesToConvert( parent, oldProps, &newProps );
		for ( newProp = newProps.begin(); newProp != newProps.end(); ++newProp )
		{
			TransferOptionList( *newProp, &extraWindowStyles, "window_extra_style" );
		}
	}

	/* The file is now at at least version 1.3 */

	if ( fileMajor < 1 || ( 1 == fileMajor && fileMinor < 4 ) )
	{
		if ( objClass == "wxCheckList" )
		{
			// The class we once named "wxCheckList" really represented a "wxCheckListBox", now that we use the #class macro in
			// code generation, it generates the wrong code
			parent->SetAttribute( "class", "wxCheckListBox" );
		}
	}

	/* The file is now at at least version 1.4 */
}

void ApplicationData::GetPropertiesToConvert( ticpp::Node* parent, const std::set< std::string >& names, std::set< ticpp::Element* >* properties )
{
	// Clear result set
	properties->clear();

	ticpp::Iterator< ticpp::Element > prop( "property" );
	for ( prop = parent->FirstChildElement( "property", false ); prop != prop.end(); ++prop )
	{
		std::string name;
		prop->GetAttribute( "name", &name );
		if ( names.find( name ) != names.end() )
		{
			properties->insert( prop.Get() );
		}
	}
}

void ApplicationData::TransferOptionList( ticpp::Element* prop, std::set< wxString >* options, const std::string& newPropName )
{
	wxString value = _WXSTR( prop->GetText( false ) );
	std::set< wxString > transfer;
	std::set< wxString > keep;

	// Sort options - if in the 'options' set, they should be transferred to a property named 'newPropName'
	// otherwise, they should stay
	wxStringTokenizer tkz( value, wxT("|"), wxTOKEN_RET_EMPTY_ALL );
	while ( tkz.HasMoreTokens() )
	{
		wxString option = tkz.GetNextToken();
		option.Trim( false );
		option.Trim( true );

		if ( options->find( option ) != options->end() )
		{
			// Needs to be transferred
			transfer.insert( option );
		}
		else
		{
			// Should be kept
			keep.insert( option );
		}
	}

	// Reusable sets to find properties with
	std::set< std::string > oldProps;
	std::set< ticpp::Element* > newProps;

	// If there are any to transfer, add to the target property, or make a new one
	ticpp::Node* parent = prop->Parent();
	if ( !transfer.empty() )
	{
		// Check for the target property
		ticpp::Element* newProp;
		wxString newOptionList;

		oldProps.clear();
		oldProps.insert( newPropName );
		GetPropertiesToConvert( parent, oldProps, &newProps );
		if ( !newProps.empty() )
		{
			newProp = *newProps.begin();
			newOptionList << wxT("|") << _WXSTR( newProp->GetText( false ) );
		}
		else
		{
			newProp = new ticpp::Element( "property" );
			newProp->SetAttribute( "name", newPropName );
		}

		std::set< wxString >::iterator option;
		for ( option = transfer.begin(); option != transfer.end(); ++option )
		{
			newOptionList << wxT("|") << *option;
		}

		newProp->SetText( _STDSTR( newOptionList.substr(1) ) );
		if ( newProps.empty() )
		{
			parent->InsertBeforeChild( prop, *newProp );
			delete newProp;
		}
	}

	// Set the value of the property to whatever is left
	if ( keep.empty() )
	{
		parent->RemoveChild( prop );
	}
	else
	{
		std::set< wxString >::iterator option;
		wxString newOptionList;
		for ( option = keep.begin(); option != keep.end(); ++option )
		{
			newOptionList << wxT("|") << *option;
		}
		prop->SetText( _STDSTR( newOptionList.substr(1) ) );
	}
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

void ApplicationData::GenerateCode( bool panelOnly )
{
	NotifyCodeGeneration( panelOnly );
}

void ApplicationData::GenerateInheritedClass( wxString baseName, wxString className, wxString type, wxString path, wxString file )
{
	if ( !::wxDirExists( path ) )
	{
		wxLogWarning(wxT("Invalid Path: %s"), path.c_str() );
		return;
	}

	PObjectBase obj = m_objDb->CreateObject( "UserClasses", PObjectBase() );

	PProperty baseNameProp = obj->GetProperty( wxT("basename") );
	PProperty nameProp = obj->GetProperty( wxT("name") );
	PProperty fileProp = obj->GetProperty( wxT("file") );
	PProperty typeProp = obj->GetProperty( wxT("type") );

	if ( !(baseNameProp && nameProp && fileProp && typeProp) )
	{
		wxLogWarning( wxT("Missing Property") );
		return;
	}

	baseNameProp->SetValue( baseName );
	nameProp->SetValue( className );
	fileProp->SetValue( file );
	typeProp->SetValue( type );

	CppCodeGenerator codegen;

	// Determine if Microsoft BOM should be used
	bool useMicrosoftBOM = false;
	PProperty pUseMicrosoftBOM = GetProjectData()->GetProperty( wxT("use_microsoft_bom") );
	if ( pUseMicrosoftBOM )
	{
		useMicrosoftBOM = ( pUseMicrosoftBOM->GetValueAsInteger() != 0 );
	}

	PCodeWriter h_cw( new FileCodeWriter( path + wxFILE_SEP_PATH + file + wxT(".h"), useMicrosoftBOM ) );
	PCodeWriter cpp_cw( new FileCodeWriter( path + wxFILE_SEP_PATH + file + wxT(".cpp"), useMicrosoftBOM ) );

	codegen.SetHeaderWriter(h_cw);
	codegen.SetSourceWriter(cpp_cw);

	codegen.GenerateInheritedClass(obj);

	wxLogStatus( wxT("Class generated at \'%s\'."), path.c_str() );
}

void ApplicationData::MovePosition(PObjectBase obj, bool right, unsigned int num)
{
	PObjectBase noItemObj = obj;

	PObjectBase parent = obj->GetParent();
	if (parent)
	{
		// Si el objeto está incluido dentro de un item hay que desplazar
		// el item
		while (parent && parent->GetObjectInfo()->GetObjectType()->IsItem())
		{
			obj = parent;
			parent = obj->GetParent();
		}

		unsigned int pos = parent->GetChildPosition(obj);

		// nos aseguramos de que los límites son correctos

		unsigned int children_count = parent->GetChildCount();

		if ((right && num + pos < children_count) ||
			!right  && (num <= pos))
		{
			pos = (right ? pos+num : pos-num);

			PCommand command(new ShiftChildCmd(obj,pos));
			Execute(command); //m_cmdProc.Execute(command);
			NotifyProjectRefresh();
			SelectObject( noItemObj, true );

		}
	}
}

void ApplicationData::MoveHierarchy(PObjectBase obj, bool up)
{
	PObjectBase sizeritem;

	// object must be inside a sizer
	if ( obj->GetObjectTypeName() == wxT("spacer") )
	{
		sizeritem = obj;
	}
	else
	{
		sizeritem = obj->GetParent();
		if ( !(sizeritem && sizeritem->GetObjectTypeName() == wxT("sizeritem")) )
		{
			return;
		}
	}

	PObjectBase nextSizer = sizeritem->GetParent(); // points to the object's sizer
	if ( nextSizer )
	{
		if ( up )
		{
			do
			{
				nextSizer = nextSizer->GetParent();
			}
			while (nextSizer && nextSizer->GetObjectTypeName() != wxT("sizer") );

			if (nextSizer && nextSizer->GetObjectTypeName() == wxT("sizer") )
			{
				PCommand cmdReparent(new ReparentObjectCmd(sizeritem,nextSizer));
				Execute(cmdReparent);
				NotifyProjectRefresh();
				SelectObject( obj, true );
			}
		}
		else
		{
			// object will be move to the top sizer of the next sibling object
			// subtree.
			unsigned int pos = nextSizer->GetChildPosition(sizeritem) + 1;

			if (pos < nextSizer->GetChildCount())
			{
				nextSizer = SearchSizerInto(nextSizer->GetChild(pos));
				if (nextSizer)
				{
					PCommand cmdReparent(new ReparentObjectCmd(sizeritem,nextSizer));
					Execute(cmdReparent);
					NotifyProjectRefresh();
					SelectObject( obj, true );
				}
			}
		}
	}
}


void ApplicationData::Undo()
{
	m_cmdProc.Undo();
	NotifyProjectRefresh();
	CheckProjectTree(m_project);
	NotifyObjectSelected(GetSelectedObject());
}

void ApplicationData::Redo()
{
	m_cmdProc.Redo();
	NotifyProjectRefresh();
	CheckProjectTree(m_project);
	NotifyObjectSelected(GetSelectedObject());
}


void ApplicationData::ToggleExpandLayout(PObjectBase obj)
{
	if (!obj)
	{
		return;
	}

	PObjectBase object;
	PObjectBase parent = obj->GetParent();
	if ( obj->GetObjectTypeName() == wxT("spacer") )
	{
		object = obj;
	}
	else if ( parent && parent->GetObjectTypeName() == wxT("sizeritem") )
	{
		object = parent;
	}
	else
	{
		return;
	}

	PProperty propFlag = object->GetProperty( wxT("flag") );
	assert(propFlag);

	wxString value;
	wxString currentValue = propFlag->GetValueAsString();

	value =
		(TypeConv::FlagSet(wxT("wxEXPAND"),currentValue) ?
		TypeConv::ClearFlag(wxT("wxEXPAND"),currentValue) :
	TypeConv::SetFlag(wxT("wxEXPAND"),currentValue));

	ModifyProperty(propFlag,value);
}

void ApplicationData::ToggleStretchLayout(PObjectBase obj)
{
	if (!obj)
	{
		return;
	}

	PObjectBase object;
	PObjectBase parent = obj->GetParent();
	if ( obj->GetObjectTypeName() == wxT("spacer") )
	{
		object = obj;
	}
	else if ( parent && parent->GetObjectTypeName() == wxT("sizeritem") )
	{
		object = parent;
	}
	else
	{
		return;
	}

	PProperty propOption = object->GetProperty( wxT("proportion") );
	assert(propOption);

	wxString value = ( propOption->GetValue() == wxT("1") ? wxT("0") : wxT("1") );

	ModifyProperty(propOption, value);
}

void ApplicationData::CheckProjectTree(PObjectBase obj)
{
	assert(obj);
	for (unsigned int i=0; i< obj->GetChildCount(); i++)
	{
		PObjectBase child = obj->GetChild(i);
		if (child->GetParent() != obj)
			wxLogError(wxString::Format(wxT("Parent of object \'%s\' is wrong!"),child->GetPropertyAsString(wxT("name")).c_str()));

		CheckProjectTree(child);
	}
}

bool ApplicationData::GetLayoutSettings(PObjectBase obj, int *flag, int *option,int *border, int* orient)
{
	if (obj)
	{
		PObjectBase parent = obj->GetParent();
		if ( obj->GetObjectTypeName() == wxT("spacer") )
		{
			PProperty propOption = obj->GetProperty( wxT("proportion") );
			PProperty propFlag   = obj->GetProperty( wxT("flag") );
			PProperty propBorder = obj->GetProperty( wxT("border") );
			assert(propOption && propFlag && propBorder);

			*option = propOption->GetValueAsInteger();
			*flag   = propFlag->GetValueAsInteger();
			*border = propBorder->GetValueAsInteger();

			if ( parent )
			{
				wxString parentName = parent->GetClassName();
				if ( wxT("wxBoxSizer") == parentName || wxT("wxStaticBoxSizer") == parentName )
				{
					PProperty propOrient = parent->GetProperty( wxT("orient") );
					if ( propOrient )
					{
						*orient = propOrient->GetValueAsInteger();
					}
				}
			}
			return true;
		}
		else if ( parent && parent->GetObjectTypeName() == wxT("sizeritem") )
		{
			PProperty propOption = parent->GetProperty( wxT("proportion") );
			PProperty propFlag   = parent->GetProperty( wxT("flag") );
			PProperty propBorder = parent->GetProperty( wxT("border") );
			assert(propOption && propFlag && propBorder);

			*option = propOption->GetValueAsInteger();
			*flag   = propFlag->GetValueAsInteger();
			*border = propBorder->GetValueAsInteger();

			if ( parent )
			{
				PObjectBase sizer = parent->GetParent();
				if ( sizer )
				{
					wxString parentName = sizer->GetClassName();
					if ( wxT("wxBoxSizer") == parentName || wxT("wxStaticBoxSizer") == parentName )
					{
						PProperty propOrient = sizer->GetProperty( wxT("orient") );
						if ( propOrient )
						{
							*orient = propOrient->GetValueAsInteger();
						}
					}
				}
			}
			return true;
		}
	}
	return false;
}

void ApplicationData::ChangeAlignment (PObjectBase obj, int align, bool vertical)
{
	if (!obj)
	{
		return;
	}

	PObjectBase object;
	PObjectBase parent = obj->GetParent();
	if ( obj->GetObjectTypeName() == wxT("spacer") )
	{
		object = obj;
	}
	else if ( parent && parent->GetObjectTypeName() == wxT("sizeritem") )
	{
		object = parent;
	}
	else
	{
		return;
	}

	PProperty propFlag = object->GetProperty( wxT("flag") );
	assert(propFlag);

	wxString value = propFlag->GetValueAsString();

	// Primero borramos los flags de la configuración previa, para así
	// evitar conflictos de alineaciones.
	if (vertical)
	{
		value = TypeConv::ClearFlag(wxT("wxALIGN_TOP"), value);
		value = TypeConv::ClearFlag(wxT("wxALIGN_BOTTOM"), value);
		value = TypeConv::ClearFlag(wxT("wxALIGN_CENTER_VERTICAL"), value);
	}
	else
	{
		value = TypeConv::ClearFlag(wxT("wxALIGN_LEFT"), value);
		value = TypeConv::ClearFlag(wxT("wxALIGN_RIGHT"), value);
		value = TypeConv::ClearFlag(wxT("wxALIGN_CENTER_HORIZONTAL"), value);
	}

	wxString alignStr;
	switch (align)
	{
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
	ModifyProperty(propFlag,value);
}

void ApplicationData::ToggleBorderFlag(PObjectBase obj, int border)
{
	if (!obj)
	{
		return;
	}

	PObjectBase borderObject;
	PObjectBase parent = obj->GetParent();
	if ( obj->GetObjectTypeName() == wxT("spacer") )
	{
		borderObject = obj;
	}
	else if ( parent && parent->GetObjectTypeName() == wxT("sizeritem") )
	{
		borderObject = parent;
	}
	else
	{
		return;
	}

	PProperty propFlag = borderObject->GetProperty( wxT("flag") );
	assert(propFlag);

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
	else
	{
		if ((intVal & wxTOP) != 0) value = TypeConv::SetFlag(wxT("wxTOP"), value);
		if ((intVal & wxBOTTOM) != 0) value = TypeConv::SetFlag(wxT("wxBOTTOM"), value);
		if ((intVal & wxRIGHT) != 0) value = TypeConv::SetFlag(wxT("wxRIGHT"), value);
		if ((intVal & wxLEFT) != 0) value = TypeConv::SetFlag(wxT("wxLEFT"), value);
	}

	ModifyProperty(propFlag, value);
}

void ApplicationData::CreateBoxSizerWithObject(PObjectBase obj)
{
	PObjectBase sizer, sizeritem;

	if ( obj->GetObjectTypeName() == wxT("spacer") )
	{
		sizeritem = obj;
	}
	else
	{
		sizeritem = obj->GetParent();
		if ( !(sizeritem && sizeritem->GetObjectTypeName() == wxT("sizeritem")) )
		{
			return;
		}
	}

	sizer = sizeritem->GetParent();
	unsigned int childPos = sizer->GetChildPosition(sizeritem);

	// creamos un wxBoxSizer
	PObjectBase newSizer = m_objDb->CreateObject("wxBoxSizer",sizer);
	if (newSizer)
	{
		PCommand cmd(new InsertObjectCmd(this,newSizer,sizer,childPos));
		Execute(cmd);

		if (newSizer->GetObjectTypeName() == wxT("sizeritem") )
			newSizer = newSizer->GetChild(0);

		PCommand cmdReparent(new ReparentObjectCmd(sizeritem,newSizer));
		Execute(cmdReparent);
		NotifyProjectRefresh();
	}
}

void ApplicationData::ShowXrcPreview()
{
    PObjectBase form = GetSelectedForm();
    if ( form == NULL )
    {
    	wxMessageBox( wxT("Please select a form and try again."), wxT("No Form Selected"), wxICON_ERROR );
    	return;
    }

    wxString className = form->GetClassName();

    XrcCodeGenerator codegen;
    wxString filePath = wxFileName::CreateTempFileName( wxT("wxFB") );
    PCodeWriter cw(new FileCodeWriter(filePath));

    codegen.SetWriter(cw);
    codegen.GenerateCode(m_project);

    wxString workingDir = ::wxGetCwd();
    // We change the current directory so that the relative paths work properly
    ::wxSetWorkingDirectory(GetProjectPath());
    wxXmlResource *res = wxXmlResource::Get();
    res->Load(filePath);

    if (className == wxT("Frame"))
    {
        wxFrame *frame = new wxFrame();
        res->LoadFrame(frame, wxTheApp->GetTopWindow(), form->GetPropertyAsString(wxT("name")));
        frame->Show();
    }
    else if (className == wxT("Dialog"))
    {
        wxDialog dialog;
        res->LoadDialog(&dialog, wxTheApp->GetTopWindow(), form->GetPropertyAsString(wxT("name")));
        dialog.ShowModal();
    }
    else if (className == wxT("Panel"))
    {
        wxDialog dialog(wxTheApp->GetTopWindow(), -1, wxT("Dialog"), wxDefaultPosition,
            wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
        wxPanel *panel = new wxPanel();
        res->LoadPanel(panel, &dialog, form->GetPropertyAsString(wxT("name")));
        dialog.SetClientSize(panel->GetSize());
        dialog.SetSize(form->GetPropertyAsSize(wxT("size")));
        dialog.CenterOnScreen();
        dialog.ShowModal();
    }
    ::wxSetWorkingDirectory(workingDir);
    res->Unload(filePath);
    ::wxRemoveFile(filePath);
}

bool ApplicationData::CanPasteObject()
{
	PObjectBase obj = GetSelectedObject();
	if (obj && obj->GetObjectTypeName() != wxT("project") )
		return (m_clipboard != NULL);

	return false;
}

bool ApplicationData::CanCopyObject()
{
	PObjectBase obj = GetSelectedObject();
	if (obj && obj->GetObjectTypeName() != wxT("project") )
		return true;

	return false;
}

bool ApplicationData::IsModified()
{
	return m_modFlag;
}

void ApplicationData::Execute(PCommand cmd)
{
	m_modFlag = true;
	m_cmdProc.Execute(cmd);
}

//////////////////////////////////////////////////////////////////////////////
void ApplicationData::AddHandler( wxEvtHandler* handler )
{
	m_handlers.push_back( handler );
}

void ApplicationData::RemoveHandler( wxEvtHandler* handler )
{
	for ( HandlerVector::iterator it = m_handlers.begin(); it != m_handlers.end(); ++it )
	{
		if ( *it == handler )
		{
			m_handlers.erase( it );
			break;
		}
	}
}

void ApplicationData::NotifyEvent( wxFBEvent& event )
{
  static int count = 0;

  if (count == 0)
  {
	  count++;
	  wxLogDebug( wxT("event: %s"), event.GetEventName().c_str() );
	  std::vector< wxEvtHandler* >::iterator handler;
	  for ( handler = m_handlers.begin(); handler != m_handlers.end(); handler++ )
	    (*handler)->ProcessEvent( event );

	  count--;
  }
  else
  {
    wxLogDebug( wxT("Pending event: %s"), event.GetEventName().c_str() );
    std::vector< wxEvtHandler* >::iterator handler;
	for ( handler = m_handlers.begin(); handler != m_handlers.end(); handler++ )
	    (*handler)->AddPendingEvent( event );
  }
}

void ApplicationData::NotifyProjectLoaded()
{
  wxFBEvent event( wxEVT_FB_PROJECT_LOADED );
  NotifyEvent( event );
}

void ApplicationData::NotifyProjectSaved()
{
  wxFBEvent event( wxEVT_FB_PROJECT_SAVED );
  NotifyEvent( event );
}

void ApplicationData::NotifyObjectExpanded(PObjectBase obj)
{
  wxFBObjectEvent event( wxEVT_FB_OBJECT_EXPANDED, obj);
  NotifyEvent( event );
}

void ApplicationData::NotifyObjectSelected(PObjectBase obj)
{
  wxFBObjectEvent event( wxEVT_FB_OBJECT_SELECTED, obj);
  NotifyEvent( event );
}

void ApplicationData::NotifyObjectCreated(PObjectBase obj)
{
  wxFBObjectEvent event( wxEVT_FB_OBJECT_CREATED, obj);
  NotifyEvent( event );
}

void ApplicationData::NotifyObjectRemoved(PObjectBase obj)
{
  wxFBObjectEvent event( wxEVT_FB_OBJECT_REMOVED, obj);
  NotifyEvent( event );
}

void ApplicationData::NotifyPropertyModified(PProperty prop)
{
  wxFBPropertyEvent event( wxEVT_FB_PROPERTY_MODIFIED, prop);
  NotifyEvent( event );
}

void ApplicationData::NotifyEventHandlerModified(PEvent evtHandler)
{
  wxFBEventHandlerEvent event( wxEVT_FB_EVENT_HANDLER_MODIFIED, evtHandler);
  NotifyEvent( event );
}

void ApplicationData::NotifyCodeGeneration( bool panelOnly )
{
  wxFBEvent event( wxEVT_FB_CODE_GENERATION );

  // Using the previously unused Id field in the event to carry a boolean
  event.SetId( ( panelOnly ? 1 : 0 ) );

  NotifyEvent( event );
}

void ApplicationData::NotifyProjectRefresh()
{
  wxFBEvent event( wxEVT_FB_PROJECT_REFRESH );
  NotifyEvent( event );
}

bool ApplicationData::VerifySingleInstance( const wxString& file, bool switchTo )
{
	return m_ipc->VerifySingleInstance( file, switchTo );
}
