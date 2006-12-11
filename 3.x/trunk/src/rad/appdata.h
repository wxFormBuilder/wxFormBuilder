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

#ifndef __APP_DATA__
#define __APP_DATA__

#include "model/database.h"
#include "rad/cmdproc.h"
#include <set>
#include <wx/ipc.h>
#include <memory>

namespace ticpp
{
	class Node;
	class Element;
}

class Property;

class wxFBEvent;
class wxFBManager;

class wxSingleInstanceChecker;
class AppServer;

#define AppData()         	(ApplicationData::Get())
#define AppDataCreate(path) (ApplicationData::Get(path))
#define AppDataInit()		(ApplicationData::Initialize())
#define AppDataDestroy()  	(ApplicationData::Destroy())

// This class is a singleton class.
class ApplicationData
{
 private:
  static ApplicationData *s_instance;


  wxString m_rootDir;       // directorio raíz (mismo que el ejecutable)
  bool m_modFlag;           // flag de proyecto modificado
  PObjectDatabase m_objDb;  // Base de datos de objetos
  shared_ptr<ObjectBase> m_project;    // Proyecto
  shared_ptr<ObjectBase> m_selObj;     // Objeto seleccionado

  shared_ptr<ObjectBase> m_clipboard;
  bool m_copyOnPaste; // flag que indica si hay que copiar el objeto al pegar

  // Procesador de comandos Undo/Redo
  CommandProcessor m_cmdProc;
  wxString m_projectFile;
  wxString m_projectPath;

  shared_ptr< wxFBManager > m_manager;


  typedef vector< wxEvtHandler* > HandlerVector;
  HandlerVector m_handlers;

  // Only allow one instance of a project to be loaded at a time
  std::auto_ptr< wxSingleInstanceChecker > m_checker;
  std::auto_ptr< AppServer > m_server;

  void NotifyEvent( wxFBEvent& event );

   // Notifican a cada observador el evento correspondiente
  void NotifyProjectLoaded();
  void NotifyProjectSaved();
  void NotifyObjectExpanded(shared_ptr<ObjectBase> obj);
  void NotifyObjectSelected(shared_ptr<ObjectBase> obj);
  void NotifyObjectCreated(shared_ptr<ObjectBase> obj);
  void NotifyObjectRemoved(shared_ptr<ObjectBase> obj);
  void NotifyPropertyModified(shared_ptr<Property> prop);
  void NotifyProjectRefresh();
  void NotifyCodeGeneration( bool panelOnly = false );

  /**
   * Comprueba las referencias cruzadas de todos los nodos del árbol
   */
  void CheckProjectTree(shared_ptr<ObjectBase> obj);

  /**
   * Resuelve un posible conflicto de nombres.
   * @note el objeto a comprobar debe estar insertado en proyecto, por tanto
   *       no es válida para arboles "flotantes".
   */
  void ResolveNameConflict(shared_ptr<ObjectBase> obj);

  /**
   * Rename all objects that have the same name than any object of a subtree.
   */
  void ResolveSubtreeNameConflicts(shared_ptr<ObjectBase> obj, shared_ptr<ObjectBase> topObj = shared_ptr<ObjectBase>());

  /**
   * Rutina auxiliar de ResolveNameConflict
   */
  void BuildNameSet(shared_ptr<ObjectBase> obj, shared_ptr<ObjectBase> top, set<wxString> &name_set);

  /**
   * Calcula la posición donde deberá ser insertado el objeto.
   *
   * Dado un objeto "padre" y un objeto "seleccionado", esta rutina calcula la
   * posición de inserción de un objeto debajo de "parent" de forma que el objeto
   * quede a continuación del objeto "seleccionado".
   *
   * El algoritmo consiste ir subiendo en el arbol desde el objeto "selected"
   * hasta encontrar un objeto cuyo padre sea el mismo que "parent" en cuyo
   * caso se toma la posición siguiente a ese objeto.
   *
   * @param parent objeto "padre"
   * @param selected objeto "seleccionado".
   * @return posición de insercción (-1 si no se puede insertar).
   */
  int CalcPositionOfInsertion(shared_ptr<ObjectBase> selected,shared_ptr<ObjectBase> parent);


  /**
   * Elimina aquellos items que no contengan hijos.
   *
   * Esta rutina se utiliza cuando el árbol que se carga de un fichero
   * no está bien formado, o la importación no ha sido correcta.
   */
   void RemoveEmptyItems(shared_ptr<ObjectBase> obj);

   /**
    * Eliminar un objeto.
    */
   void DoRemoveObject(shared_ptr<ObjectBase> object, bool cutObject);

   void Execute(PCommand cmd);

   /**
    * Search a size in the hierarchy of an object
    */
   shared_ptr<ObjectBase> SearchSizerInto(shared_ptr<ObjectBase> obj);


	/**
	Convert a project from an older version.
	@param path The path to the project file
	@param fileMajor The major revision of the file
	@param fileMinor The minor revision of the file
	@return true if successful, false otherwise
	*/
	bool ConvertProject( const wxString& path, int fileMajor, int fileMinor );

	/**
	Recursive function used to convert the object tree in the project file to the latest version.
	@param object A pointer to the object element
	@param fileMajor The major revision of the file
	@param fileMinor The minor revision of the file
	*/
	void ConvertObject( ticpp::Element* object, int fileMajor, int fileMinor );

	/**
	Iterates through 'property' element children of @a parent.
	Saves all properties with a 'name' attribute matching one of @a names into @a properties
	@param parent Object element with child properties.
	@param names Set of property names to search for.
	@param properties Pointer to a set in which to store the result of the search.
	*/
	void GetPropertiesToConvert( ticpp::Node* parent, const std::set< std::string >& names, std::set< ticpp::Element* >* properties );

	/**
	Transfers @a options from the text of @a prop to the text of @a newPropName, which will be created if it doesn't exist.
	@param prop Property containing the options to transfer.
	@param options Set of options to search for and transfer.
	@param newPropName Name of property to transfer to, will be created if non-existant.
	*/
	void TransferOptionList( ticpp::Element* prop, std::set< wxString >* options, const std::string& newPropName );


  // hiden constructor
  ApplicationData(const wxString &rootdir = wxT(".") );

public:

  static ApplicationData* Get(const wxString &rootdir = wxT(".") );

  // Force the static AppData instance to Init()
  static void Initialize();
  static void Destroy();

  // Initialize application
  void LoadApp();

  // Hold a pointer to the wxFBManager
  shared_ptr< wxFBManager > GetManager();

  // Procedures for register/unregister wxEvtHandlers to be notified of wxFBEvents
  void AddHandler( wxEvtHandler* handler );
  void RemoveHandler( wxEvtHandler* handler );

  // Operaciones sobre los datos
  bool LoadProject(const wxString &filename);
  void SaveProject(const wxString &filename);
  void NewProject();
  void ExpandObject( shared_ptr<ObjectBase> obj, bool expand );

  // Object will not be selected if it already is selected, unless force = true
  void SelectObject( shared_ptr<ObjectBase> obj, bool force = false );
  void CreateObject(wxString name);
  void RemoveObject(shared_ptr<ObjectBase> obj);
  void CutObject(shared_ptr<ObjectBase> obj);
  void CopyObject(shared_ptr<ObjectBase> obj);
  void PasteObject(shared_ptr<ObjectBase> parent);
  void InsertObject(shared_ptr<ObjectBase> obj, shared_ptr<ObjectBase> parent);
  void MergeProject(shared_ptr<ObjectBase> project);
  void ModifyProperty(shared_ptr<Property> prop, wxString value);
  void GenerateCode( bool projectOnly = false );
  void MovePosition(shared_ptr<ObjectBase>, bool right, unsigned int num = 1);
  void MoveHierarchy( shared_ptr<ObjectBase> obj, bool up);
  void Undo();
  void Redo();
  void ToggleExpandLayout(shared_ptr<ObjectBase> obj);
  void ToggleStretchLayout(shared_ptr<ObjectBase> obj);
  void ChangeAlignment (shared_ptr<ObjectBase> obj, int align, bool vertical);
  void ToggleBorderFlag(shared_ptr<ObjectBase> obj, int border);
  void CreateBoxSizerWithObject(shared_ptr<ObjectBase> obj);
  void ShowXrcPreview();

  // Servicios para los observadores
  shared_ptr<ObjectBase> GetSelectedObject();
  shared_ptr<ObjectBase> GetProjectData();
  shared_ptr<ObjectBase> GetSelectedForm();
  bool CanUndo() { return m_cmdProc.CanUndo(); }
  bool CanRedo() { return m_cmdProc.CanRedo(); }
  bool GetLayoutSettings(shared_ptr<ObjectBase> obj, int *flag, int *option,int *border, int* orient);
  bool CanPasteObject();
  bool CanCopyObject();
  bool IsModified();

  PObjectPackage GetPackage(unsigned int idx)
    { return m_objDb->GetPackage(idx);}

  unsigned int GetPackageCount()
    { return m_objDb->GetPackageCount(); }

  PObjectDatabase GetObjectDatabase()
    { return m_objDb; }


  // Servicios específicos, no definidos en DataObservable
  void        SetClipboardObject(shared_ptr<ObjectBase> obj) { m_clipboard = obj; }
  shared_ptr<ObjectBase> GetClipboardObject()                { return m_clipboard; }

  wxString GetProjectFileName() { return m_projectFile; }

  const int m_fbpVerMajor;
  const int m_fbpVerMinor;
  const int m_port;

  const wxString &GetProjectPath() { return m_projectPath; };
  void SetProjectPath(const wxString &path) { m_projectPath = path; };

  const wxString &GetApplicationPath() { return m_rootDir; };
  void SetApplicationPath(const wxString &path) { m_rootDir = path; };

  // Only allow one instance of a project
  bool VerifySingleInstance( const wxString& file, bool switchTo = true );
  bool CreateServer( const wxString& name );
};

/* Only allow one instance of a project */

// Connection class, for use by both communicationg instances
class AppConnection: public wxConnection
{
public:
	AppConnection(){wxLogMessage(wxT("yo"));}
	~AppConnection(){}

	bool OnExecute( const wxString& topic, wxChar* data, int size, wxIPCFormat format );
};

// Server class, for listening to connection requests
class AppServer: public wxServer
{
public:
	const wxString m_name;

	AppServer( const wxString& name ) : m_name( name ){}
	wxConnectionBase* OnAcceptConnection( const wxString& topic );
};

// Client class, to be used by subsequent instances in OnInit
class AppClient: public wxClient
{
public:
	AppClient(){}
	wxConnectionBase* OnMakeConnection();
};

#endif //__APP_DATA__
