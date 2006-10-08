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

#ifndef __OBJ_DATABASE__
#define __OBJ_DATABASE__

#include "tinyxml.h"
#include <wx/dynlib.h>
#include <set>
#include <boost/smart_ptr.hpp>
#include <vector>
#include <map>
#include "model/types.h"

using namespace std;
using namespace boost;

class ObjectInfo;
class ObjectBase;

class ObjectPackage;
class ObjectDatabase;
class ObjectTypeDictionary;
class PropertyCategory;

typedef shared_ptr<ObjectPackage> PObjectPackage;
typedef shared_ptr<ObjectDatabase> PObjectDatabase;

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
  vector< shared_ptr< ObjectInfo > > m_objs;

 public:
  /**
   * Constructor.
   */
  ObjectPackage(wxString name, wxString desc, wxBitmap icon);

  /**
   * Incluye en el paquete la información de un objeto.
   */
  void Add(shared_ptr<ObjectInfo> obj) { m_objs.push_back(obj); };

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
  shared_ptr<ObjectInfo> GetObjectInfo(unsigned int idx);
};



class wxFBManager;

/**
 * Base de datos de objetos.
 * Todos las informaciones de objetos importadas de los archivos XML, serán
 * almacenados por esta clase.
 */
class ObjectDatabase
{
 private:
  typedef vector<PObjectPackage> PackageVector;

  // Map the property type string to the property type number
  typedef map<wxString,PropertyType> PTMap;
  typedef map<wxString,PObjectType> ObjectTypeMap;
  typedef vector<wxDynamicLibrary *> CLibraryVector;
  typedef set<wxString> MacroSet;

  std::string m_xmlPath;
  std::string m_iconPath;
  wxString m_pluginPath;
  map< wxString, shared_ptr< ObjectInfo > > m_objs;
  PackageVector m_pkgs;
  PTMap m_propTypes;
  CLibraryVector m_libs;
  ObjectTypeMap m_types; // register object types

  // para comprobar que no se nos han quedado macros sin añadir en las
  // liberias de componentes, vamos a crear un conjunto con las macros
  // definidas en los XML, y al importar las librerías vamos a ir eliminando
  // dichas macros del conjunto, quedando al final las macros que faltan
  // por registrar en la librería.
  MacroSet m_macroSet;

  /**
   * Initialize the property type map.
   */
  void InitPropertyTypes();

  /**
   * Carga las plantillas de generación de código de un fichero
   * xml de código dado
   */
  void LoadCodeGen( std::string file);

  /**
   * Carga los objetos de un paquete con todas sus propiedades salvo
   * los objetos heredados
   */
  PObjectPackage LoadPackage( std::string file, wxString iconPath = wxEmptyString );

  void ParseProperties( ticpp::Element* elem_obj, shared_ptr<ObjectInfo> obj_info, shared_ptr< PropertyCategory > category );

  /**
   * Importa una librería de componentes y lo asocia a cada clase.
   * @throw wxFBException If the library could not be imported.
   */
  void ImportComponentLibrary( wxString libfile, shared_ptr< wxFBManager > manager );

  /**
   * Incluye la información heredada de los objetos de un paquete.
   * En la segunda pasada configura cada paquete con sus objetos base.
   */
  void SetupPackage( std::string file, wxString libPath, shared_ptr< wxFBManager > manager );

  /**
   * Determina si el tipo de objeto hay que incluirlo en la paleta de
   * componentes.
   */
  bool ShowInPalette(wxString type);
  bool HasCppProperties(wxString type);

  // rutinas de conversión
  PropertyType ParsePropertyType (wxString str);
  wxString       ParseObjectType   (wxString str);


  PObjectType GetObjectType(wxString name);

  int CountChildrenWithSameType(shared_ptr<ObjectBase> parent,PObjectType type);

  void SetDefaultLayoutProperties(shared_ptr<ObjectBase> obj);

 public:
  ObjectDatabase();
  ~ObjectDatabase();

  shared_ptr<ObjectBase> NewObject(shared_ptr<ObjectInfo> obj_info);

  /**
   * Obtiene la información de un objeto a partir del nombre de la clase.
   */
  shared_ptr<ObjectInfo> GetObjectInfo(wxString class_name);

  /**
   * Configura la ruta donde se encuentran los ficheros con la descripción.
   */
  void SetXmlPath( std::string path) { m_xmlPath = path; }

  /**
   * Configura la ruta donde se encuentran los iconos asociados a los objetos.
   */
  void SetIconPath( std::string path) { m_iconPath = path; }
  void SetPluginPath( wxString path) { m_pluginPath = path; }

  /**
   * Obtiene la ruta donde se encuentran los ficheros con la descripción de
   * objetos.
   */
  std::string GetXmlPath()          { return m_xmlPath; }
  wxString GetPluginPath()          { return m_pluginPath; }

  /**
   * Carga las definiciones de tipos de objetos.
   */
  bool LoadObjectTypes();

  /**
   * Find and load plugins from the plugins directory
   */
  void LoadPlugins( shared_ptr< wxFBManager > manager );

  /**
   * Fabrica de objetos.
   * A partir del nombre de la clase se crea una nueva instancia de un objeto.
   */
  shared_ptr<ObjectBase> CreateObject( std::string class_name, shared_ptr<ObjectBase> parent = shared_ptr<ObjectBase>());

  /**
   * Fábrica de objetos a partir de un objeto XML.
   * Este método se usará para cargar un proyecto almacenado.
   */
  shared_ptr<ObjectBase> CreateObject(TiXmlElement *obj, shared_ptr<ObjectBase> parent = shared_ptr<ObjectBase>());

  /**
   * Crea un objeto como copia de otro.
   */

  shared_ptr<ObjectBase> CopyObject(shared_ptr<ObjectBase> obj);

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
