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

#ifndef __OBJ_DATABASE__
#define __OBJ_DATABASE__

#include "../utils/wxfbdefs.h"
#include "types.h"

#include <set>
#ifndef WXFB_PLUGINS_RESOLVE
#include <wx/dynlib.h>
#else
#include <functional>
#include <boost/dll/shared_library.hpp>
#endif

class ObjectDatabase;
class ObjectTypeDictionary;
class PropertyCategory;

typedef std::shared_ptr<ObjectDatabase> PObjectDatabase;

namespace ticpp
{
	class Element;
}

/**
 * Paquete de clases de objetos.
 * Determinará la agrupación en la paleta de componentes.
 */
class ObjectPackage
{
 private:
  wxString m_name;    // nombre del paquete
  wxString m_desc;  // breve descripción del paquete
  wxBitmap m_icon;	// The icon for the notebook page

  // Vector con los objetos que están contenidos en el paquete
  std::vector< PObjectInfo > m_objs;

 public:
  /**
   * Constructor.
   */
  ObjectPackage(wxString name, wxString desc, wxBitmap icon);

  /**
   * Incluye en el paquete la información de un objeto.
   */
  void Add(PObjectInfo obj) { m_objs.push_back(obj); }

  /**
   * Obtiene el nombre del paquete.
   */
  wxString GetPackageName() { return m_name; }

  /**
   * Obtiene el texto que describe el paquete.
   */
  wxString GetPackageDescription() { return m_desc; }

  /**
   * Get Package Icon
   */
  wxBitmap GetPackageIcon() { return m_icon; }

  /**
   * Obtiene el número de objetos incluidos en el paquete.
   */
  unsigned int GetObjectCount() { return (unsigned int)m_objs.size(); }

  /**
   * Obtiene la información de un objeto incluido en el paquete.
   */
  PObjectInfo GetObjectInfo(unsigned int idx);

  /**
  If two xml files specify the same package name, then they merged to one package with this.
  This allows one package to be split across multiple xml files.
  */
  void AppendPackage( PObjectPackage package );

};

class IComponentLibrary;
class IManager;

/**
 * Base de datos de objetos.
 * Todos las informaciones de objetos importadas de los archivos XML, serán
 * almacenados por esta clase.
 */
class ObjectDatabase
{
 public:
  static bool HasCppProperties(wxString type);
 private:
  typedef std::vector<PObjectPackage> PackageVector;

  // Map the property type string to the property type number
  typedef std::map<wxString,PropertyType> PTMap;
  typedef std::map<wxString,PObjectType> ObjectTypeMap;
  typedef std::set<wxString> MacroSet;
  typedef std::map< wxString, PCodeInfo > LangTemplateMap;
  typedef std::map< PropertyType, LangTemplateMap > PTLangTemplateMap;

  #ifndef WXFB_PLUGINS_RESOLVE
  #ifdef __WXMAC__
	typedef std::vector< void * > LibraryVector;
  #else
	typedef std::vector< wxDynamicLibrary * > LibraryVector;
  #endif
  typedef void (*PFFreeComponentLibrary)( IComponentLibrary* lib );
  typedef std::map< PFFreeComponentLibrary, IComponentLibrary * > ComponentLibraryMap;
  #else
  struct PluginLibrary {
    boost::dll::shared_library sharedLibrary;
    std::function<IComponentLibrary*(IManager*)> getComponentLibrary;
    std::function<void(IComponentLibrary*)> freeComponentLibrary;
    IComponentLibrary* componentLibrary = nullptr;
  };
  #endif

  wxString m_xmlPath;
  wxString m_iconPath;
  wxString m_pluginPath;
  std::map< wxString, PObjectInfo > m_objs;
  PackageVector m_pkgs;
  PTMap m_propTypes;
  ObjectTypeMap m_types; // register object types

  // para comprobar que no se nos han quedado macros sin añadir en las
  // liberias de componentes, vamos a crear un conjunto con las macros
  // definidas en los XML, y al importar las librerías vamos a ir eliminando
  // dichas macros del conjunto, quedando al final las macros que faltan
  // por registrar en la librería.
  MacroSet m_macroSet;

  PTLangTemplateMap m_propertyTypeTemplates;

  #ifndef WXFB_PLUGINS_RESOLVE
  LibraryVector m_libs;
  ComponentLibraryMap m_componentLibs;
  // used so libraries are only imported once, even if multiple libraries use them
  std::set< wxString > m_importedLibraries;
  #else
  std::map<wxString, PluginLibrary> m_pluginLibraries;
  #endif

  /**
   * Initialize the property type map.
   */
  void InitPropertyTypes();

  /**
   * Carga las plantillas de generación de código de un fichero
   * xml de código dado
   */
  void LoadCodeGen( const wxString& file );

  /**
   * Carga los objetos de un paquete con todas sus propiedades salvo
   * los objetos heredados
   */
  PObjectPackage LoadPackage( const wxString& file, const wxString& iconPath = wxEmptyString );

  void ParseProperties( ticpp::Element* elem_obj, PObjectInfo obj_info, PPropertyCategory category, std::set< PropertyType >* types );
  void ParseEvents    ( ticpp::Element* elem_obj, PObjectInfo obj_info, PPropertyCategory category );

  /**
   * Importa una librería de componentes y lo asocia a cada clase.
   * @throw wxFBException If the library could not be imported.
   */
  void ImportComponentLibrary( wxString libfile, PwxFBManager manager );

  /**
   * Incluye la información heredada de los objetos de un paquete.
   * En la segunda pasada configura cada paquete con sus objetos base.
   */
  void SetupPackage( const wxString& file, const wxString& path, PwxFBManager manager );

  /**
   * Determina si el tipo de objeto hay que incluirlo en la paleta de
   * componentes.
   */
  bool ShowInPalette(wxString type);

  // rutinas de conversión
  PropertyType ParsePropertyType (wxString str);
  wxString       ParseObjectType   (wxString str);


  PObjectType GetObjectType(wxString name);

  int CountChildrenWithSameType(PObjectBase parent,PObjectType type);
  int CountChildrenWithSameType(PObjectBase parent, const std::set<PObjectType>& types);

  void SetDefaultLayoutProperties(PObjectBase obj);

 public:
  ObjectDatabase();
  ~ObjectDatabase();

  PObjectBase NewObject(PObjectInfo obj_info);

  /**
   * Obtiene la información de un objeto a partir del nombre de la clase.
   */
  PObjectInfo GetObjectInfo(wxString class_name);

  /**
   * Configura la ruta donde se encuentran los ficheros con la descripción.
   */
  void SetXmlPath( const wxString& path ) { m_xmlPath = path; }

  /**
   * Configura la ruta donde se encuentran los iconos asociados a los objetos.
   */
  void SetIconPath( const wxString& path)  { m_iconPath = path; }
  void SetPluginPath( const wxString& path ) { m_pluginPath = path; }

  /**
   * Obtiene la ruta donde se encuentran los ficheros con la descripción de
   * objetos.
   */
  wxString GetXmlPath()		{ return m_xmlPath; 		}
  wxString GetIconPath()	{ return m_iconPath; 		}
  wxString GetPluginPath()	{ return m_pluginPath; 		}

  /**
   * Carga las definiciones de tipos de objetos.
   */
  bool LoadObjectTypes();

  /**
   * Find and load plugins from the plugins directory
   */
  void LoadPlugins( PwxFBManager manager );

  /**
   * Fabrica de objetos.
   * A partir del nombre de la clase se crea una nueva instancia de un objeto.
   */
  PObjectBase CreateObject( std::string class_name, PObjectBase parent = PObjectBase());

  /**
   * Fábrica de objetos a partir de un objeto XML.
   * Este método se usará para cargar un proyecto almacenado.
   */
  PObjectBase CreateObject( ticpp::Element* obj, PObjectBase parent = PObjectBase());

  /**
   * Crea un objeto como copia de otro.
   */

  PObjectBase CopyObject(PObjectBase obj);

  /**
   * Obtiene un paquete de objetos.
   */
  PObjectPackage GetPackage(unsigned int idx);

  /**
   * Obtiene el número de paquetes registrados.
   */
  unsigned int GetPackageCount() { return (unsigned int)m_pkgs.size(); }

  /**
   * Resetea los contadores que acompañan al nombre.
   * La propiedad "name" es una propiedad especial, reservada para el nombre
   * de la instancia del objeto. Cada clase de objeto tiene asociado un contador
   * para no duplicar nombre en la creación de nuevos objetos
   * (p.e. m_button1, m_button2 ...)
   */
  void ResetObjectCounters();
};



#endif //__OBJ_DATABASE__
