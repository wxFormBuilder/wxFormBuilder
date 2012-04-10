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

#include "utils/wxfbdefs.h"
#include "model/database.h"
#include "rad/cmdproc.h"
#include <set>

namespace ticpp
{

class Node;

class Element;
}

class Property;

class wxFBEvent;

class wxFBManager;

class wxFBIPC;

#define AppData()         	(ApplicationData::Get())
#define AppDataCreate(path) (ApplicationData::Get(path))
#define AppDataInit()	      (ApplicationData::Initialize())
#define AppDataDestroy()  	(ApplicationData::Destroy())

// This class is a singleton class.

class ApplicationData
{

	private:

		static ApplicationData *s_instance;

		wxString m_rootDir;       // directorio raíz (mismo que el ejecutable)

		bool m_modFlag;           // flag de proyecto modificado

		PObjectDatabase m_objDb;  // Base de datos de objetos

		PObjectBase m_project;    // Proyecto

		PObjectBase m_selObj;     // Objeto seleccionado

		PObjectBase m_clipboard;

		bool m_copyOnPaste; // flag que indica si hay que copiar el objeto al pegar

		// Procesador de comandos Undo/Redo
		CommandProcessor m_cmdProc;

		wxString m_projectFile;

		wxString m_projectPath;

		PwxFBManager m_manager;

		// Prevent more than one instance of a project
		boost::shared_ptr< wxFBIPC > m_ipc;


		typedef std::vector< wxEvtHandler* > HandlerVector;

		HandlerVector m_handlers;

		void NotifyEvent( wxFBEvent& event, bool forcedelayed = false );

		// Notifican a cada observador el evento correspondiente
		void NotifyProjectLoaded();

		void NotifyProjectSaved();

		void NotifyObjectExpanded( PObjectBase obj );

		void NotifyObjectSelected( PObjectBase obj, bool force = false );

		void NotifyObjectCreated( PObjectBase obj );

		void NotifyObjectRemoved( PObjectBase obj );

		void NotifyPropertyModified( PProperty prop );

		void NotifyEventHandlerModified( PEvent evtHandler );

		void NotifyProjectRefresh();

		void NotifyCodeGeneration( bool panelOnly = false, bool forcedelayed = false );

		/**
		 * Comprueba las referencias cruzadas de todos los nodos del árbol
		 */
		void CheckProjectTree( PObjectBase obj );

		/**
		 * Resuelve un posible conflicto de nombres.
		 * @note el objeto a comprobar debe estar insertado en proyecto, por tanto
		 *       no es válida para arboles "flotantes".
		 */
		void ResolveNameConflict( PObjectBase obj );

		/**
		 * Rename all objects that have the same name than any object of a subtree.
		 */
		void ResolveSubtreeNameConflicts( PObjectBase obj, PObjectBase topObj = PObjectBase() );

		/**
		 * Rutina auxiliar de ResolveNameConflict
		 */
		void BuildNameSet( PObjectBase obj, PObjectBase top, std::set<wxString> &name_set );

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
		int CalcPositionOfInsertion( PObjectBase selected, PObjectBase parent );


		/**
		 * Elimina aquellos items que no contengan hijos.
		 *
		 * Esta rutina se utiliza cuando el árbol que se carga de un fichero
		 * no está bien formado, o la importación no ha sido correcta.
		 */
		void RemoveEmptyItems( PObjectBase obj );

		/**
		 * Eliminar un objeto.
		 */
		void DoRemoveObject( PObjectBase object, bool cutObject );

		void Execute( PCommand cmd );

		/**
		 * Search a size in the hierarchy of an object
		 */
		PObjectBase SearchSizerInto( PObjectBase obj );


		/**
		Convert the properties of the project element. Handle this separately because it does not repeat.
		@param project The project element.
		@param path The path to the project file.
		@param fileMajor The major revision of the file.
		@param fileMinor The minor revision of the file.
		*/
		void ConvertProjectProperties( ticpp::Element* project, const wxString& path, int fileMajor, int fileMinor );

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

		void PropagateExpansion(PObjectBase obj, bool expand, bool up);

		// hiden constructor
		ApplicationData( const wxString &rootdir = wxT( "." ) );

		/**
		 * Helper for GetOutputPath and GetEmbeddedFilesOutputPath
		 */
		wxString GetPathProperty( const wxString& pathName );

		#ifdef __WXFB_DEBUG__
		wxLog* m_debugLogTarget;
		#endif

	public:

		~ApplicationData();

		#ifdef __WXFB_DEBUG__
		wxLog* GetDebugLogTarget(){ return m_debugLogTarget; }
		#endif

		static ApplicationData* Get( const wxString &rootdir = wxT( "." ) );

		// Force the static AppData instance to Init()
		static void Initialize();

		static void Destroy();

		// Initialize application
		void LoadApp();

		// Hold a pointer to the wxFBManager
		PwxFBManager GetManager();

		// Procedures for register/unregister wxEvtHandlers to be notified of wxFBEvents
		void AddHandler( wxEvtHandler* handler );

		void RemoveHandler( wxEvtHandler* handler );

		// Operaciones sobre los datos
		bool LoadProject( const wxString &file, bool checkSingleInstance = true );

		void SaveProject( const wxString &filename );

		void NewProject();

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

		void ExpandObject( PObjectBase obj, bool expand );

		void DetermineObjectToSelect( PObjectBase parent, unsigned int pos );

		// Object will not be selected if it already is selected, unless force = true
		// Returns true if selection changed, false if already selected
		bool SelectObject( PObjectBase obj, bool force = false, bool notify = true );

		void CreateObject( wxString name );

		void RemoveObject( PObjectBase obj );

		void CutObject( PObjectBase obj );

		void CopyObject( PObjectBase obj );

		bool PasteObject( PObjectBase parent, PObjectBase objToPaste = PObjectBase() );

		void CopyObjectToClipboard( PObjectBase obj );

		bool PasteObjectFromClipboard( PObjectBase parent );

		void InsertObject( PObjectBase obj, PObjectBase parent );

		void MergeProject( PObjectBase project );

		void ModifyProperty( PProperty prop, wxString value );

		void ModifyEventHandler( PEvent evt, wxString value );

		void GenerateCode( bool projectOnly = false );

		void GenerateInheritedClass( PObjectBase form, wxString className, wxString path, wxString file );

		void MovePosition( PObjectBase, bool right, unsigned int num = 1 );

		void MoveHierarchy( PObjectBase obj, bool up );

		void Undo();

		void Redo();

		void ToggleExpandLayout( PObjectBase obj );

		void ToggleStretchLayout( PObjectBase obj );

		void ChangeAlignment ( PObjectBase obj, int align, bool vertical );

		void ToggleBorderFlag( PObjectBase obj, int border );

		void CreateBoxSizerWithObject( PObjectBase obj );

		void ShowXrcPreview();

		// Servicios para los observadores
		PObjectBase GetSelectedObject();

		PObjectBase GetProjectData();

		PObjectBase GetSelectedForm();

		bool CanUndo() { return m_cmdProc.CanUndo(); }

		bool CanRedo() { return m_cmdProc.CanRedo(); }

		bool GetLayoutSettings( PObjectBase obj, int *flag, int *option, int *border, int* orient );
		bool CanPasteObject();
		bool CanPasteObjectFromClipboard();
		bool CanCopyObject();
		bool IsModified();

		PObjectPackage GetPackage( unsigned int idx )
		{ return m_objDb->GetPackage( idx );}

		unsigned int GetPackageCount()
		{ return m_objDb->GetPackageCount(); }

		PObjectDatabase GetObjectDatabase()
		{ return m_objDb; }


		// Servicios específicos, no definidos en DataObservable
		void        SetClipboardObject( PObjectBase obj ) { m_clipboard = obj; }

		PObjectBase GetClipboardObject()                { return m_clipboard; }

		wxString GetProjectFileName() { return m_projectFile; }

		const int m_fbpVerMajor;

		const int m_fbpVerMinor;

		/** Path to the fbp file that is opened. */
		const wxString &GetProjectPath() { return m_projectPath; };


		/**
		Path where the files will be generated.
		*/
		wxString GetOutputPath();

		/**
		Path where the embedded bitmap files will be generated.
		*/
		wxString GetEmbeddedFilesOutputPath();

		void SetProjectPath( const wxString &path ) { m_projectPath = path; };

		const wxString &GetApplicationPath() { return m_rootDir; };

		void SetApplicationPath( const wxString &path ) { m_rootDir = path; };

		// Allow a single instance check from outsid the AppData class
		bool VerifySingleInstance( const wxString& file, bool switchTo = true );
};

#endif //__APP_DATA__
