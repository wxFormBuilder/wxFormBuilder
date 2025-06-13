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

#ifndef RAD_APPDATA_H
#define RAD_APPDATA_H

#include <set>
#include <unordered_set>

#include "model/database.h"
#include "rad/cmdproc.h"


namespace tinyxml2
{
class XMLDocument;
class XMLElement;
}  // namespace tinyxml2

class Property;
class wxFBEvent;
class wxFBIPC;
class wxFBManager;


// This class is a singleton class.
class ApplicationData
{
public:
    struct Version {
        int major;
        int minor;
    };

public:
    static ApplicationData* Get(const wxString& rootdir = ".");
    static void Initialize();
    static void Destroy();

private:
    explicit ApplicationData(const wxString& rootdir);

public:
    ~ApplicationData();

    ApplicationData(const ApplicationData&) = delete;
    ApplicationData& operator=(const ApplicationData&) = delete;
    ApplicationData(ApplicationData&&) = delete;
    ApplicationData& operator=(ApplicationData&&) = delete;

#ifdef __WXFB_DEBUG__
    wxLog* GetDebugLogTarget() { return m_debugLogTarget; }
#endif

    const Version& getFbpVersion() const { return m_fbpVersion; }

    // Initialize application
    void LoadApp();
    // Allow a single instance check from outside the AppData class
    bool VerifySingleInstance(const wxString& file, bool switchTo = true);

    const wxString& GetApplicationPath() const { return m_rootDir; }

    // Hold a pointer to the wxFBManager
    PwxFBManager GetManager() const { return m_manager; }
    PObjectDatabase GetObjectDatabase() const { return m_objDb; }
    PObjectPackage GetPackage(unsigned int idx) const { return m_objDb->GetPackage(idx); }
    unsigned int GetPackageCount() const { return m_objDb->GetPackageCount(); }

    // Procedures for register/unregister wxEvtHandlers to be notified of wxFBEvents
    void AddHandler(wxEvtHandler* handler);
    void RemoveHandler(wxEvtHandler* handler);

    void SetDarkMode(bool darkMode) { m_darkMode = darkMode; }
    bool IsDarkMode() const { return m_darkMode; }

    void NewProject();
    // Operaciones sobre los datos
    bool LoadProject(const wxString& file, bool justGenerate = false);
    void SaveProject(const wxString& filename);
    bool IsModified() const { return m_modFlag; }

    /**
     * Convert a project from an older version.
     * @param path The path to the project file
     * @param fileMajor The major revision of the file
     * @param fileMinor The minor revision of the file
     * @return true if successful, false otherwise
     */
    bool ConvertProject(tinyxml2::XMLDocument* doc, const wxString& path, int versionMajor, int versionMinor);
    /**
     * Recursive function used to convert the object tree in the project file to the latest version.
     * @param object A pointer to the object element
     * @param fileMajor The major revision of the file
     * @param fileMinor The minor revision of the file
     */
    void ConvertObject(tinyxml2::XMLElement* object, int versionMajor, int versionMinor);

    /** Path to the fbp file that is opened. */
    const wxString& GetProjectPath() const { return m_projectPath; }
    const wxString& GetProjectFileName() const { return m_projectFile; }
    /**
     * Path where the files will be generated.
     */
    wxString GetOutputPath() const;
    /**
     * Path where the embedded bitmap files will be generated.
     */
    wxString GetEmbeddedFilesOutputPath() const;

    PObjectBase GetProjectData() const { return m_project; }
    PObjectBase GetSelectedForm() const;
    // Servicios para los observadores
    PObjectBase GetSelectedObject() const { return m_selObj; }

    // Servicios específicos, no definidos en DataObservable
    void SetClipboardObject(PObjectBase obj) { m_clipboard = obj; }
    PObjectBase GetClipboardObject() const { return m_clipboard; }

    void GenerateCode(bool panelOnly = false, bool noDelayed = false);
    void GenerateCodeToFiles(bool noDelayed = false);
    void GenerateInheritedClass(PObjectBase form, const wxString& className, const wxString& path, const wxString& file);
    void ShowXrcPreview();

    // Object will not be selected if it already is selected, unless force = true
    // Returns true if selection changed, false if already selected
    bool SelectObject(PObjectBase obj, bool force = false, bool notify = true);
    void DetermineObjectToSelect(PObjectBase parent, unsigned int pos);
    void ExpandObject(PObjectBase obj, bool expand);

    void MergeProject(PObjectBase project);

    void CreateObject(const wxString& name);
    void InsertObject(PObjectBase obj, PObjectBase parent);
    void CreateBoxSizerWithObject(PObjectBase obj);
    void RemoveObject(PObjectBase obj);

    void ModifyProperty(PProperty prop, const wxString& value);
    void ModifyEventHandler(PEvent evt, const wxString& value);

    bool CanCopyObject() const;
    bool CanPasteObject() const;
    bool CanPasteObjectFromClipboard() const;

    void CopyObject(PObjectBase obj);
    void CutObject(PObjectBase obj);
    bool PasteObject(PObjectBase parent, PObjectBase objToPaste = PObjectBase());

    void CopyObjectToClipboard(PObjectBase obj);
    bool PasteObjectFromClipboard(PObjectBase parent);

    void MoveHierarchy(PObjectBase obj, bool up);
    void MovePosition(PObjectBase, bool right, unsigned int num = 1);

    bool GetLayoutSettings(PObjectBase obj, int* flag, int* option, int* border, int* orient);
    void ChangeAlignment(PObjectBase obj, int align, bool vertical);
    void ToggleBorderFlag(PObjectBase obj, int border);
    void ToggleExpandLayout(PObjectBase obj);
    void ToggleStretchLayout(PObjectBase obj);

    bool CanUndo() const { return m_cmdProc.CanUndo(); }
    bool CanRedo() const { return m_cmdProc.CanRedo(); }

    void Undo();
    void Redo();

private:
    /**
     * Convert the properties of the project element. Handle this separately because it does not repeat.
     * @param project The project element.
     * @param path The path to the project file.
     * @param fileMajor The major revision of the file.
     * @param fileMinor The minor revision of the file.
     */
    void ConvertProjectProperties(
      tinyxml2::XMLElement* project, const wxString& path, int versionMajor, int versionMinor);
    /**
     * Iterates through 'property' element children of @a parent.
     * Saves all properties with a 'name' attribute matching one of @a names into @a properties
     * @param parent Object element with child properties.
     * @param names Set of property names to search for.
     * @param properties Pointer to a set in which to store the result of the search.
     */
    std::unordered_set<tinyxml2::XMLElement*> GetProperties(
      tinyxml2::XMLElement* element, const std::set<wxString>& properties);
    /**
     * Iterates through 'property' element children of @a parent.
     * Removes all properties with a 'name' attribute matching one of @a names
     * @param parent Object element with child properties.
     * @param names Set of property names to search for.
     */
    void RemoveProperties(tinyxml2::XMLElement* element, const std::set<wxString>& properties);
    /**
     * Transfers @a options from the text of @a prop to the text of @a newPropName, which will be created if it doesn't
     * exist.
     * @param prop Property containing the options to transfer.
     * @param options Set of options to search for and transfer.
     * @param newPropName Name of property to transfer to, will be created if non-existent.
     */
    bool MoveOptions(
      tinyxml2::XMLElement* src, tinyxml2::XMLElement* dest, const std::set<wxString>& options,
      bool deleteEmptySrc = false);

    /**
     * Helper for GetOutputPath and GetEmbeddedFilesOutputPath
     */
    wxString GetPathProperty(const wxString& pathName) const;

    /**
     * Comprueba las referencias cruzadas de todos los nodos del árbol
     */
    void CheckProjectTree(PObjectBase obj);
    /**
     * Resuelve un posible conflicto de nombres.
     * @note el objeto a comprobar debe estar insertado en proyecto, por tanto
     *       no es válida para arboles "flotantes".
     */
    void ResolveNameConflict(PObjectBase obj);
    /**
     * Rename all objects that have the same name than any object of a subtree.
     */
    void ResolveSubtreeNameConflicts(PObjectBase obj, PObjectBase topObj = PObjectBase());
    /**
     * Rutina auxiliar de ResolveNameConflict
     */
    void BuildNameSet(PObjectBase obj, PObjectBase top, std::set<wxString>& name_set);

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
    int CalcPositionOfInsertion(PObjectBase selected, PObjectBase parent);
    /**
     * Search a size in the hierarchy of an object
     */
    PObjectBase SearchSizerInto(PObjectBase obj);

    void PropagateExpansion(PObjectBase obj, bool expand, bool up);

    /**
     * Elimina aquellos items que no contengan hijos.
     *
     * Esta rutina se utiliza cuando el árbol que se carga de un fichero
     * no está bien formado, o la importación no ha sido correcta.
     */
    void RemoveEmptyItems(PObjectBase obj);
    /**
     * Eliminar un objeto.
     */
    void DoRemoveObject(PObjectBase object, bool cutObject);

    void Execute(PCommand cmd);

    void NotifyEvent(wxFBEvent& event, bool forcedelayed = false);

    // Notifican a cada observador el evento correspondiente
    void NotifyProjectLoaded();
    void NotifyProjectSaved();
    void NotifyProjectRefresh();

    void NotifyObjectExpanded(PObjectBase obj);
    void NotifyObjectSelected(PObjectBase obj, bool force = false);

    void NotifyObjectCreated(PObjectBase obj);
    void NotifyObjectRemoved(PObjectBase obj);

    void NotifyPropertyModified(PProperty prop);
    void NotifyEventHandlerModified(PEvent evtHandler);

    void NotifyCodeGeneration(bool panelOnly = false, bool forcedelayed = false);
    void NotifyCodeGenerationToFiles(bool forcedelayed = false);

private:
    static ApplicationData* s_instance;

    // Current project file format version
    const Version m_fbpVersion;

    // Prevent more than one instance of a project
    std::shared_ptr<wxFBIPC> m_ipc;
#ifdef __WXFB_DEBUG__
    wxLog* m_debugLogTarget;
#endif

    wxString m_rootDir;  // directorio raíz (mismo que el ejecutable)
    PwxFBManager m_manager;
    PObjectDatabase m_objDb;  // Base de datos de objetos
    std::vector<wxEvtHandler*> m_handlers;
    bool m_darkMode;

    PObjectBase m_project;  // Proyecto
    wxString m_projectPath;
    wxString m_projectFile;

    PObjectBase m_selObj;  // Objeto seleccionado
    PObjectBase m_clipboard;

    // Procesador de comandos Undo/Redo
    CommandProcessor m_cmdProc;
    bool m_modFlag;      // flag de proyecto modificado
    bool m_copyOnPaste;  // flag que indica si hay que copiar el objeto al pegar
};

inline ApplicationData* AppData()
{
    return ApplicationData::Get();
}

#endif  // RAD_APPDATA_H
