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

#include "../rad/bitmaps.h"
#include "../rad/wxfbmanager.h"
#include "../utils/debug.h"
#include "../utils/stringutils.h"
#include "../utils/typeconv.h"
#include "../utils/wxfbexception.h"
#include "objectbase.h"

#include <ticpp.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

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


#ifdef __WXMAC__
#include <dlfcn.h>
#endif

ObjectPackage::ObjectPackage(wxString name, wxString desc, wxBitmap icon)
{
	m_name = name;
	m_desc = desc;
	m_icon = icon;
}

PObjectInfo ObjectPackage::GetObjectInfo(unsigned int idx)
{
	assert (idx < m_objs.size());
	return m_objs[idx];
}

void ObjectPackage::AppendPackage( PObjectPackage package )
{
	m_objs.insert( m_objs.end(), package->m_objs.begin(), package->m_objs.end() );
}

///////////////////////////////////////////////////////////////////////////////

ObjectDatabase::ObjectDatabase()
{
	//InitObjectTypes();
	//  InitWidgetTypes();
	InitPropertyTypes();
}

ObjectDatabase::~ObjectDatabase()
{
    #ifndef WXFB_PLUGINS_RESOLVE
	for ( ComponentLibraryMap::iterator lib = m_componentLibs.begin(); lib != m_componentLibs.end(); ++lib )
    {
        (*(lib->first))( lib->second );
    }

    for ( LibraryVector::iterator lib = m_libs.begin(); lib != m_libs.end(); ++lib )
    {
        #ifdef __WXFB_DEBUG__
			// Only unload in release - can't get a good stack trace if the library is unloaded
			#ifdef __WXMAC__
				dlclose( *lib );
			#else
				(*lib)->Detach();
			#endif
        #endif

		#ifndef __WXMAC__
			delete *lib;
		#endif
    }
	#else
	for (const auto& plugin : m_pluginLibraries) {
		if (plugin.second.freeComponentLibrary && plugin.second.componentLibrary) {
			plugin.second.freeComponentLibrary(plugin.second.componentLibrary);
		}
	}
	#endif
}

PObjectInfo ObjectDatabase::GetObjectInfo(wxString class_name)
{
	PObjectInfo info;
	ObjectInfoMap::iterator it = m_objs.find( class_name );

	if ( it != m_objs.end() )
	{
		info = it->second;
	}

	return info;
}

PObjectPackage ObjectDatabase::GetPackage(unsigned int idx)
{
	assert (idx < m_pkgs.size());

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
	object->SetObjectTypeName(obj_info->GetObjectTypeName()); // *FIXME*

	object->SetObjectInfo(obj_info);

	PPropertyInfo prop_info;
	PEventInfo    event_info;
	PObjectInfo   class_info = obj_info;

	unsigned int base = 0;

	while (class_info)
	{
		unsigned int i;
		for (i = 0; i < class_info->GetPropertyCount(); i++)
		{
			prop_info = class_info->GetPropertyInfo(i);

			PProperty property(new Property(prop_info, object));

			// Set the default value, either from the property info, or an override from this class
			wxString defaultValue = prop_info->GetDefaultValue();
			if ( base > 0 )
			{
				wxString defaultValueTemp = obj_info->GetBaseClassDefaultPropertyValue( base - 1, prop_info->GetName() );
				if ( !defaultValueTemp.empty() )
				{
					defaultValue = defaultValueTemp;
				}
			}
			property->SetValue( defaultValue );

			// Las propiedades están implementadas con una estructura "map",
			// ello implica que no habrá propiedades duplicadas.
			// En otro caso habrá que asegurarse de que dicha propiedad
			// no existe.
			// Otra cosa importante, es que el orden en que se insertan
			// las propiedades, de abajo-arriba, esto permite que se pueda redefir
			// alguna propiedad.
			object->AddProperty (property);
		}

		for (i=0; i < class_info->GetEventCount(); i++)
		{
		  event_info = class_info->GetEventInfo(i);
		  PEvent event(new Event(event_info,object));
		  // notice that for event there isn't a default value on its creation
		  // because there is not handler at the moment
		  object->AddEvent(event);
		}

		class_info = ( base < obj_info->GetBaseClassCount() ?
			obj_info->GetBaseClass(base++) : PObjectInfo());
	}

	// si el objeto tiene la propiedad name (reservada para el nombre del
	// objeto) le añadimos el contador para no repetir nombres.

	obj_info->IncrementInstanceCount();

	unsigned int ins = obj_info->GetInstanceCount();
	PProperty pname = object->GetProperty( wxT(NAME_TAG) );
	if (pname)
		pname->SetValue(pname->GetValue() + StringUtils::IntToStr(ins));

	return object;
}


int ObjectDatabase::CountChildrenWithSameType(PObjectBase parent,PObjectType type)
{
	unsigned int count = 0;
	unsigned int numChildren = parent->GetChildCount();
	for (unsigned int i=0; i < numChildren ; i++)
	{
		if (type == parent->GetChild(i)->GetObjectInfo()->GetObjectType())
			count++;
	}

	return count;
}

int ObjectDatabase::CountChildrenWithSameType(PObjectBase parent, const std::set<PObjectType>& types)
{
	unsigned int count = 0;
	unsigned int numChildren = parent->GetChildCount();
	for (unsigned int i = 0; i < numChildren; ++i)
	{
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
PObjectBase ObjectDatabase::CreateObject( std::string classname, PObjectBase parent)
{
	PObjectBase object;
	PObjectInfo objInfo = GetObjectInfo( _WXSTR(classname) );

	if (!objInfo)
	{
		THROW_WXFBEX( 	wxT("Unknown Object Type: ") << _WXSTR(classname) << wxT("\n")
						wxT("The most likely causes are that this copy of wxFormBuilder is out of date, or that there is a plugin missing.\n")
						wxT("Please check at http://www.wxFormBuilder.org") << wxT("\n") )
	}

	PObjectType objType = objInfo->GetObjectType();

	if (parent)
	{
		// Comprobamos si el tipo es válido
		PObjectType parentType = parent->GetObjectInfo()->GetObjectType();

		//AUI
		bool aui = false;
		if( parentType->GetName() == wxT("form") )
		{
			aui = parent->GetPropertyAsInteger(wxT("aui_managed")) != 0;
		}

		int max = parentType->FindChildType(objType, aui);

		// FIXME! Esto es un parche para evitar crear los tipos menubar,statusbar y
		// toolbar en un form que no sea wxFrame.
		// Hay que modificar el conjunto de tipos para permitir tener varios tipos
		// de forms (como childType de project), pero hay mucho código no válido
		// para forms que no sean de tipo "form". Dicho de otra manera, hay
		// código que dependen del nombre del tipo, cosa que hay que evitar.
		if (parentType->GetName() == wxT("form") && parent->GetClassName() != wxT("Frame") &&
			(objType->GetName() == wxT("statusbar") ||
			objType->GetName() == wxT("menubar") ||
			objType->GetName() == wxT("ribbonbar") ||
			objType->GetName() == wxT("toolbar") ))
			return PObjectBase(); // tipo no válido

		// No menu dropdown for wxToolBar until wx 2.9 :(
		if ( parentType->GetName() == wxT("tool") )
		{
			PObjectBase gParent = parent->GetParent();
			if (
				( gParent->GetClassName() == wxT("wxToolBar") ) &&
				( objType->GetName() == wxT("menu") )
			)
				return PObjectBase(); // not a valid type
		}

		if (max != 0) // tipo válido
		{
			bool create = true;

			// we check the number of instances
			int count;
			if (objType == GetObjectType(wxT("sizer")) || objType == GetObjectType(wxT("gbsizer")))
			{
				count = CountChildrenWithSameType(parent, { GetObjectType(wxT("sizer")), GetObjectType(wxT("gbsizer")) });
			}
			else 
			{
				count = CountChildrenWithSameType(parent, objType);
			}
			
			if (max > 0 && count >= max)
				create = false;

			if (create)
				object = NewObject(objInfo);
		}
		else // max == 0
		{
			// el tipo no es válido, vamos a comprobar si podemos insertarlo
			// como hijo de un "item"
			bool created = false;
			for (unsigned int i=0; !created && i < parentType->GetChildTypeCount(); i++)
			{
				PObjectType childType = parentType->GetChildType(i);
				int childMax = childType->FindChildType(objType, aui);

				if (childType->IsItem() && childMax != 0)
				{
					childMax = parentType->FindChildType(childType, aui);

					// si el tipo es un item y además el tipo del objeto a crear
					// puede ser hijo del tipo del item vamos a intentar crear la
					// instancia del item para crear el objeto como hijo de este
					if (childMax < 0 || CountChildrenWithSameType(parent, childType) < childMax)
					{
						// No hay problemas para crear el item debajo de parent
						PObjectBase item = NewObject(GetObjectInfo(childType->GetName()));

						//PObjectBase obj = CreateObject(classname,item);
						PObjectBase obj = NewObject(objInfo);

						// la siguiente condición debe cumplirse siempre
						// ya que un item debe siempre contener a otro objeto
						if (obj)
						{
							// enlazamos item y obj
							item->AddChild(obj);
							obj->SetParent(item);

							// sizeritem es un tipo de objeto reservado, para que el uso sea
							// más práctico se asignan unos valores por defecto en función
							// del tipo de objeto creado
							if ( item->GetObjectInfo()->IsSubclassOf( wxT("sizeritembase") ) )
								SetDefaultLayoutProperties(item);

							object = item;
							created = true;
						}
						else
							wxLogError(wxT("Review your definitions file (objtypes.xml)"));
					}
				}
			}
		}
		///////////////////////////////////////////////////////////////////////
		// Nota: provisionalmente vamos a enlazar el objeto al padre pero
		//       esto debería hacerse fuera para poder implementar el Undo-Redo
		///////////////////////////////////////////////////////////////////////
		//if (object)
		//{
		//  parent->AddChild(object);
		//  object->SetParent(parent);
		//}
	}
	else // parent == NULL;
	{
		object = NewObject(objInfo);
	}

	return object;
}

PObjectBase ObjectDatabase::CopyObject(PObjectBase obj)
{
	assert(obj);

	PObjectInfo objInfo = obj->GetObjectInfo();

	PObjectBase copyObj = NewObject(objInfo); // creamos la copia
	assert(copyObj);

	// copiamos las propiedades
	unsigned int i;
	unsigned int count = obj->GetPropertyCount();
	for (i = 0; i < count; i++)
	{
		PProperty objProp = obj->GetProperty(i);
		assert(objProp);

		PProperty copyProp = copyObj->GetProperty(objProp->GetName());
		assert(copyProp);

		wxString propValue = objProp->GetValue();
		copyProp->SetValue(propValue);
	}

	// ...and the event handlers
	count = obj->GetEventCount();
	for (i = 0; i < count; i++)
	{
	  PEvent event = obj->GetEvent(i);
	  PEvent copyEvent = copyObj->GetEvent(event->GetName());
	  copyEvent->SetValue(event->GetValue());
	}

	// creamos recursivamente los hijos
	count = obj->GetChildCount();
	for (i = 0; i<count; i++)
	{
		PObjectBase childCopy = CopyObject(obj->GetChild(i));
		copyObj->AddChild(childCopy);
		childCopy->SetParent(copyObj);
	}

	return copyObj;
}

void ObjectDatabase::SetDefaultLayoutProperties(PObjectBase sizeritem)
{
	if ( !sizeritem->GetObjectInfo()->IsSubclassOf( wxT("sizeritembase") ) )
	{
		LogDebug( wxT("SetDefaultLayoutProperties expects a subclass of sizeritembase") );
		return;
	}

	PObjectBase child = sizeritem->GetChild(0);
	PObjectInfo childInfo = child->GetObjectInfo();
	wxString obj_type = child->GetObjectTypeName();

	PProperty proportion = sizeritem->GetProperty( wxT("proportion") );

	if ( childInfo->IsSubclassOf( wxT("sizer") ) || childInfo->IsSubclassOf( wxT("gbsizer") ) || obj_type == wxT("splitter") || childInfo->GetClassName() == wxT("spacer") )
	{
		if ( proportion )
		{
			proportion->SetValue( wxT("1") );
		}
		sizeritem->GetProperty( wxT("flag") )->SetValue( wxT("wxEXPAND") );
	}
	else if ( childInfo->GetClassName() == wxT("wxStaticLine") )
	{
		sizeritem->GetProperty( wxT("flag") )->SetValue( wxT("wxEXPAND | wxALL") );
	}
	else if ( childInfo->GetClassName() == wxT("wxToolBar") )
	{
		sizeritem->GetProperty( wxT("flag") )->SetValue( wxT("wxEXPAND") );
	}
	else if ( obj_type == wxT("widget") || obj_type == wxT("statusbar") )
	{
		if ( proportion )
		{
			proportion->SetValue( wxT("0") );
		}
		sizeritem->GetProperty( wxT("flag") )->SetValue( wxT("wxALL") );
	}
	else if (	obj_type == wxT("notebook")			||
				obj_type == wxT("flatnotebook")		||
				obj_type == wxT("listbook")			||
				obj_type == wxT("simplebook")       ||
				obj_type == wxT("choicebook")		||
				obj_type == wxT("auinotebook")		||
				obj_type == wxT("treelistctrl")		||
				obj_type == wxT("expanded_widget")	||
				obj_type == wxT("container")
				)
	{
		if ( proportion )
		{
			proportion->SetValue( wxT("1") );
		}
		sizeritem->GetProperty( wxT("flag") )->SetValue( wxT("wxEXPAND | wxALL") );
	}
}

void ObjectDatabase::ResetObjectCounters()
{
	ObjectInfoMap::iterator it;
	for (it = m_objs.begin() ; it != m_objs.end() ; it++)
	{
		it->second->ResetInstanceCount();
	}
}

///////////////////////////////////////////////////////////////////////

PObjectBase ObjectDatabase::CreateObject( ticpp::Element* xml_obj, PObjectBase parent )
{
	try
	{
		std::string class_name;
		xml_obj->GetAttribute( CLASS_TAG, &class_name, false );

		PObjectBase newobject = CreateObject( class_name, parent );

		// It is possible the CreateObject returns an "item" containing the object, e.g. SizerItem or SplitterItem
		// If that is the case, reassign "object" to the actual object
		PObjectBase object = newobject;
		if ( object && object->GetChildCount() > 0 )
		{
			object = object->GetChild( 0 );
		}

		if ( object )
		{
			// Get the state of expansion in the object tree
			bool expanded;
			xml_obj->GetAttributeOrDefault( EXPANDED_TAG, &expanded, true );
			object->SetExpanded( expanded );

			// Load the properties
			ticpp::Element* xml_prop = xml_obj->FirstChildElement( PROPERTY_TAG, false );
			while ( xml_prop )
			{
				std::string prop_name;
				xml_prop->GetAttribute( NAME_TAG, &prop_name, false );
				PProperty prop = object->GetProperty( _WXSTR(prop_name) );

				if ( prop ) // does the property exist
				{
					// load the value
					prop->SetValue( _WXSTR( xml_prop->GetText( false ) ) );
				}
				else
				{
					std::string value = xml_prop->GetText( false );
					if ( !value.empty() )
					{
						wxLogError( wxT("The property named \"%s\" of class \"%s\" is not supported by this version of wxFormBuilder.\n")
									wxT("If your project file was just converted from an older version, then the conversion was not complete.\n")
									wxT("Otherwise, this project is from a newer version of wxFormBuilder.\n\n")
									wxT("The property's value is: %s\n")
									wxT("If you save this project, YOU WILL LOSE DATA"), _WXSTR(prop_name), _WXSTR(class_name), _WXSTR(value) );
					}
				}

				xml_prop = xml_prop->NextSiblingElement( PROPERTY_TAG, false );
			}

			// load the event handlers
			ticpp::Element* xml_event = xml_obj->FirstChildElement( EVENT_TAG, false );
			while ( xml_event )
			{
				std::string event_name;
				xml_event->GetAttribute( NAME_TAG, &event_name, false );
				PEvent event = object->GetEvent( _WXSTR(event_name) );
				if ( event )
				{
					event->SetValue( _WXSTR( xml_event->GetText( false ) ) );
				}

				xml_event = xml_event->NextSiblingElement( EVENT_TAG, false );
			}


			if ( parent )
			{
				// set up parent/child relationship
				parent->AddChild( newobject );
				newobject->SetParent( parent );
			}

			// create the children
			ticpp::Element* child = xml_obj->FirstChildElement( OBJECT_TAG, false );
			while ( child )
			{
				CreateObject( child, object );
				child = child->NextSiblingElement( OBJECT_TAG, false );
			}
		}

		return newobject;
	}
	catch( ticpp::Exception& )
	{
		return PObjectBase();
	}
}

//////////////////////////////

bool IncludeInPalette(wxString /*type*/) {
	return true;
}

void ObjectDatabase::LoadPlugins( PwxFBManager manager )
{
	// Load some default templates
	LoadCodeGen( m_xmlPath + wxT("properties.cppcode") );
	LoadCodeGen( m_xmlPath + wxT("properties.pythoncode") );
	LoadCodeGen( m_xmlPath + wxT("properties.luacode") );
	LoadCodeGen( m_xmlPath + wxT("properties.phpcode") );
	LoadPackage( m_xmlPath + wxT("default.xml"), m_iconPath );
	LoadCodeGen( m_xmlPath + wxT("default.cppcode") );
	LoadCodeGen( m_xmlPath + wxT("default.pythoncode") );
	LoadCodeGen( m_xmlPath + wxT("default.luacode") );
	LoadCodeGen( m_xmlPath + wxT("default.phpcode") );

	// Map to temporarily hold plugins.
	// Used to both set page order and to prevent two plugins with the same name.
	typedef std::map< wxString, PObjectPackage > PackageMap;
	PackageMap packages;

	// Open plugins directory for iteration
	if ( !wxDir::Exists( m_pluginPath ) )
	{
		return;
	}

	wxDir pluginsDir( m_pluginPath );
	if ( !pluginsDir.IsOpened() )
	{
		return;
	}

	// Iterate through plugin directories and load the package from the xml subdirectory
	wxString pluginDirName;
	bool moreDirectories = pluginsDir.GetFirst( &pluginDirName, wxEmptyString, wxDIR_DIRS | wxDIR_HIDDEN );
    while ( moreDirectories )
    {
    	// Iterate through .xml files in the xml directory
    	wxString nextPluginPath = m_pluginPath + pluginDirName;
    	wxString nextPluginXmlPath = nextPluginPath + wxFILE_SEP_PATH + wxT("xml");
    	wxString nextPluginIconPath = nextPluginPath + wxFILE_SEP_PATH + wxT("icons");
    	if ( wxDir::Exists( nextPluginPath ) )
    	{
    		if ( wxDir::Exists( nextPluginXmlPath ) )
    		{
				wxDir pluginXmlDir( nextPluginXmlPath );
				if ( pluginXmlDir.IsOpened() )
				{
					std::map< wxString, PObjectPackage > packagesToSetup;
					wxString packageXmlFile;
					bool moreXmlFiles = pluginXmlDir.GetFirst( &packageXmlFile, wxT("*.xml"), wxDIR_FILES | wxDIR_HIDDEN );
					while ( moreXmlFiles )
					{
						try
						{
							wxFileName nextXmlFile( nextPluginXmlPath + wxFILE_SEP_PATH + packageXmlFile );
							if ( !nextXmlFile.IsAbsolute() )
							{
								nextXmlFile.MakeAbsolute();
							}

							PObjectPackage package = LoadPackage( nextXmlFile.GetFullPath(), nextPluginIconPath );
							if ( package )
							{
								// Load all packages, then setup all packages
								// this allows multiple packages sharing one library
								packagesToSetup[ nextXmlFile.GetFullPath() ] = package;
							}
						}
						catch ( wxFBException& ex )
						{
							wxLogError( ex.what() );
						}
						moreXmlFiles = pluginXmlDir.GetNext( &packageXmlFile );
					}

					std::map< wxString, PObjectPackage >::iterator packageIt;
					for ( packageIt = packagesToSetup.begin(); packageIt != packagesToSetup.end(); ++packageIt )
					{
						// Setup the inheritance for base classes
						wxFileName fullNextPluginPath( nextPluginPath );
						if ( !fullNextPluginPath.IsAbsolute() )
						{
							fullNextPluginPath.MakeAbsolute();
						}
						wxFileName xmlFileName( packageIt->first );
						try
						{
							SetupPackage( xmlFileName.GetFullPath(), fullNextPluginPath.GetFullPath(), manager );

							// Load the C++ code templates
							xmlFileName.SetExt( wxT("cppcode") );
							LoadCodeGen( xmlFileName.GetFullPath() );

							// Load the Python code templates
							xmlFileName.SetExt( wxT("pythoncode") );
							LoadCodeGen( xmlFileName.GetFullPath() );

							// Load the PHP code templates
							xmlFileName.SetExt( wxT("phpcode") );
							LoadCodeGen( xmlFileName.GetFullPath() );

							// Load the Lua code templates
							xmlFileName.SetExt( wxT("luacode") );
							LoadCodeGen( xmlFileName.GetFullPath() );

							std::pair< PackageMap::iterator, bool > addedPackage = packages.insert( PackageMap::value_type( packageIt->second->GetPackageName(), packageIt->second ) );
							if ( !addedPackage.second )
							{
								addedPackage.first->second->AppendPackage( packageIt->second );
                                LogDebug( "Merged plugins named \"" + packageIt->second->GetPackageName() + "\"" );
							}

						}
						catch ( wxFBException& ex )
						{
							wxLogError( ex.what() );
						}
					}
				}
    		}
    	}

        moreDirectories = pluginsDir.GetNext( &pluginDirName );
    }

	// Add packages to final data structure
	m_pkgs.reserve(packages.size());
	for (auto& package : packages)
	{
		m_pkgs.push_back(package.second);
	}
}

void ObjectDatabase::SetupPackage(const wxString& file, [[maybe_unused]] const wxString& path, PwxFBManager manager) {
	#ifdef __WXMSW__
		wxString libPath = path;
	#else
		wxStandardPathsBase& stdpaths = wxStandardPaths::Get();
		wxString libPath = stdpaths.GetPluginsDir();
		libPath.Replace( wxTheApp->GetAppName(), wxT("wxformbuilder") );
	#endif

    // Renamed libraries for convenience in debug using a "-xx" wx version as suffix.
    // This will also prevent loading debug libraries in release and vice versa,
    // that used to cause crashes when trying to debug.
    wxString wxver = wxT("");

	#ifdef DEBUG
		#ifdef APPEND_WXVERSION
			wxver = wxver + wxString::Format( wxT("-%i%i"), wxMAJOR_VERSION, wxMINOR_VERSION );
		#endif
	#endif

	try
	{
		ticpp::Document doc;
		XMLUtils::LoadXMLFile( doc, true, file );

		ticpp::Element* root = doc.FirstChildElement( PACKAGE_TAG );

		// get the library to import
		std::string lib;
		root->GetAttributeOrDefault( "lib", &lib, "" );
		if ( !lib.empty() )
		{
			#ifndef WXFB_PLUGINS_RESOLVE
			// Add prefix required by non-CMake builds
			lib.insert(0, "lib");
			// Allows plugin dependency dlls to be next to plugin dll in windows
			wxString workingDir = ::wxGetCwd();
			wxFileName::SetCwd( libPath );
			try
			{
				wxString fullLibPath = libPath + wxFILE_SEP_PATH + _WXSTR(lib) + wxver;
				if ( m_importedLibraries.insert( fullLibPath ).second )
				{
					ImportComponentLibrary( fullLibPath, manager );
				}
			}
			catch ( ... )
			{
				// Put Cwd back
				wxFileName::SetCwd( workingDir );
				throw;
			}

			// Put Cwd back
			wxFileName::SetCwd( workingDir );
			#else
			#ifdef __WINDOWS__
			// On Windows the plugin libraries reside in the directory of the plugin resources,
			// construct a relative path to that location to be able to find them with the default
			// search algorithm
			lib = "plugins/" + lib + "/" + lib;
			#endif
			auto pluginLibrary = m_pluginLibraries.find(lib);
			if (pluginLibrary == m_pluginLibraries.end()) {
				ImportComponentLibrary(lib, manager);
			}
			#endif
		}

		ticpp::Element* elem_obj = root->FirstChildElement( OBJINFO_TAG, false );
		while ( elem_obj )
		{
			std::string wxver_obj;
			elem_obj->GetAttributeOrDefault(WXVERSION_TAG, &wxver_obj, "");
			if (!wxver_obj.empty())
			{
				long wxversion = 0;
				// skip widgets supported by higher wxWidgets version than used for the build
				if((!_WXSTR(wxver_obj).ToLong(&wxversion)) || (wxversion > wxVERSION_NUMBER))
				{
					elem_obj = elem_obj->NextSiblingElement( OBJINFO_TAG, false );
					continue;
				}
			}

			std::string class_name;
			elem_obj->GetAttribute( CLASS_TAG, &class_name );

			PObjectInfo class_info = GetObjectInfo( _WXSTR(class_name) );

			ticpp::Element* elem_base = elem_obj->FirstChildElement( "inherits", false );
			while ( elem_base )
			{
				std::string base_name;
				elem_base->GetAttribute( CLASS_TAG, &base_name );

				// Add a reference to its base class
				PObjectInfo base_info  = GetObjectInfo( _WXSTR(base_name) );
				if ( class_info && base_info )
				{
					size_t baseIndex = class_info->AddBaseClass( base_info );

					std::string prop_name, value;
					ticpp::Element* inheritedProperty = elem_base->FirstChildElement( "property", false );
					while( inheritedProperty )
					{
						inheritedProperty->GetAttribute( NAME_TAG, &prop_name );
						value = inheritedProperty->GetText();
						class_info->AddBaseClassDefaultPropertyValue( baseIndex, _WXSTR(prop_name), _WXSTR(value) );
						inheritedProperty = inheritedProperty->NextSiblingElement( "property", false );
					}
				}
				elem_base = elem_base->NextSiblingElement( "inherits", false );
			}

			// Add the "C++" base class, predefined for the components and widgets
			wxString typeName = class_info->GetObjectTypeName();
			if ( HasCppProperties( typeName ) )
			{
				PObjectInfo cpp_interface = GetObjectInfo( wxT("C++") );
				if ( cpp_interface )
				{
					size_t baseIndex = class_info->AddBaseClass( cpp_interface );
					if (    typeName == wxT("sizer")    ||
                            typeName == wxT("gbsizer")  ||
                            typeName == wxT("menuitem")  )
					{
						class_info->AddBaseClassDefaultPropertyValue( baseIndex, _("permission"), _("none") );
					}
				}
			}

			elem_obj = elem_obj->NextSiblingElement( OBJINFO_TAG, false );
		}
	}
	catch ( ticpp::Exception& ex )
	{
		THROW_WXFBEX( _WXSTR(ex.m_details) );
	}
}

bool ObjectDatabase::HasCppProperties(wxString type)
{
	return (type == wxT("notebook")			||
			type == wxT("flatnotebook")		||
			type == wxT("listbook")			||
			type == wxT("simplebook")		||
			type == wxT("choicebook")		||
			type == wxT("auinotebook")		||
			type == wxT("widget")			||
			type == wxT("expanded_widget")	||
			type == wxT("propgrid")	||
			type == wxT("propgridman")	||
			type == wxT("statusbar")		   ||
			type == wxT("component")		   ||
			type == wxT("container")		   ||
			type == wxT("menubar")			   ||
			type == wxT("menu")				   ||
			type == wxT("menuitem")			   ||
			type == wxT("submenu")			   ||
			type == wxT("toolbar")			   ||
			type == wxT("ribbonbar")			||
			type == wxT("ribbonpage")			||
			type == wxT("ribbonpanel")			||
			type == wxT("ribbonbuttonbar")		||
			type == wxT("ribbonbutton")			||
			type == wxT("ribbondropdownbutton" )	||
			type == wxT("ribbonhybridbutton" ) 	||
			type == wxT("ribbontogglebutton" ) 	||
			type == wxT("ribbontoolbar")	   		||
			type == wxT("ribbontool")		   	||
			type == wxT("ribbondropdowntool" )  	||
			type == wxT("ribbonhybridtool" )  		||
			type == wxT("ribbontoggletool" )  		||
			type == wxT("ribbongallery")	   		||
			type == wxT("ribbongalleryitem")   		||
			type == wxT("dataviewctrl")		   	||
			type == wxT("dataviewtreectrl")	  	||
			type == wxT("dataviewlistctrl")	   	||
			type == wxT("dataviewlistcolumn")	   	||
			type == wxT("dataviewcolumn")	   	||
			type == wxT("tool")				||
			type == wxT("splitter")			||
			type == wxT("treelistctrl")		||
			type == wxT("sizer")			||
			type == wxT("nonvisual")		||
			type == wxT("gbsizer")          ||
			type == wxT("propgriditem")          ||
			type == wxT("propgridpage")          ||
			type == wxT("gbsizer")          ||
            type == wxT("wizardpagesimple")
			);
}

void ObjectDatabase::LoadCodeGen( const wxString& file )
{
	try
	{
		ticpp::Document doc;
		XMLUtils::LoadXMLFile( doc, true, file );

		// read the codegen element
		ticpp::Element* elem_codegen = doc.FirstChildElement("codegen");
		std::string language;
		elem_codegen->GetAttribute( "language", &language );
		wxString lang = _WXSTR(language);

		// read the templates
		ticpp::Element* elem_templates = elem_codegen->FirstChildElement( "templates", false );
		while ( elem_templates  )
		{

			std::string prop_name;
			elem_templates->GetAttribute( "property", &prop_name, false );
			bool hasProp = !prop_name.empty();

			std::string class_name;
			elem_templates->GetAttribute( "class", &class_name, !hasProp );

			PCodeInfo code_info( new CodeInfo() );

			ticpp::Element* elem_template = elem_templates->FirstChildElement( "template", false );
			while ( elem_template )
			{
				std::string template_name;
				elem_template->GetAttribute( "name", &template_name );

				std::string template_code = elem_template->GetText( false );

				code_info->AddTemplate( _WXSTR(template_name), _WXSTR(template_code) );

				elem_template = elem_template->NextSiblingElement( "template", false );
			}

			if ( hasProp )
			{
				// store code info for properties
				if ( !m_propertyTypeTemplates[ ParsePropertyType( _WXSTR(prop_name) ) ].insert( LangTemplateMap::value_type( lang, code_info ) ).second )
				{
					wxLogError( _("Found second template definition for property \"%s\" for language \"%s\""), _WXSTR(prop_name), lang );
				}
			}
			else
			{
				// store code info for objects
				PObjectInfo obj_info = GetObjectInfo( _WXSTR(class_name) );
				if ( obj_info )
				{
					obj_info->AddCodeInfo( lang, code_info );
				}
			}

			elem_templates = elem_templates->NextSiblingElement( "templates", false );
		}
	}
	catch( ticpp::Exception& ex )
	{
		wxLogError( _WXSTR(ex.m_details) );
	}
	catch( wxFBException& ex )
	{
		wxLogError( ex.what() );
	}
}

PObjectPackage ObjectDatabase::LoadPackage( const wxString& file, const wxString& iconPath )
{
	PObjectPackage package;

	try
	{
		ticpp::Document doc;
		XMLUtils::LoadXMLFile( doc, true, file );

		ticpp::Element* root = doc.FirstChildElement( PACKAGE_TAG );

		// Name Attribute
		std::string pkg_name;
		root->GetAttribute( NAME_TAG, &pkg_name );

		// Description Attribute
		std::string pkg_desc;
		root->GetAttributeOrDefault( PKGDESC_TAG, &pkg_desc, "" );

		// Icon Path Attribute
		std::string pkgIconName;
		root->GetAttributeOrDefault( ICON_TAG, &pkgIconName, "" );
		wxString pkgIconPath = iconPath + wxFILE_SEP_PATH +  _WXSTR(pkgIconName);

		wxBitmap pkg_icon;
		if ( !pkgIconName.empty() && wxFileName::FileExists( pkgIconPath ) )
		{
			wxImage image( pkgIconPath, wxBITMAP_TYPE_ANY );
			pkg_icon = wxBitmap( image.Scale( 16, 16 ) );
		}
		else
		{
			pkg_icon = AppBitmaps::GetBitmap( wxT("unknown"), 16 );
		}

		package = PObjectPackage ( new ObjectPackage( _WXSTR(pkg_name), _WXSTR(pkg_desc), pkg_icon ) );


		ticpp::Element* elem_obj = root->FirstChildElement( OBJINFO_TAG, false );

		while (elem_obj)
		{
			std::string class_name;
			elem_obj->GetAttribute( CLASS_TAG, &class_name );

			std::string type;
			elem_obj->GetAttribute( "type", &type );

			std::string icon;
			elem_obj->GetAttributeOrDefault( "icon", &icon, "" );
			wxString iconFullPath = iconPath + wxFILE_SEP_PATH + _WXSTR(icon);

			std::string smallIcon;
			elem_obj->GetAttributeOrDefault( "smallIcon", &smallIcon, "" );
			wxString smallIconFullPath = iconPath + wxFILE_SEP_PATH + _WXSTR(smallIcon);

			std::string wxver;
			elem_obj->GetAttributeOrDefault( WXVERSION_TAG, &wxver, "" );
			if( wxver != "" )
			{
				long wxversion = 0;
				// skip widgets supported by higher wxWidgets version than used for the build
				if( (! _WXSTR(wxver).ToLong( &wxversion ) ) || (wxversion > wxVERSION_NUMBER) )
				{
					elem_obj = elem_obj->NextSiblingElement( OBJINFO_TAG, false );
					continue;
				}
			}

			bool startGroup;
			elem_obj->GetAttributeOrDefault( "startgroup", &startGroup, false );

			PObjectInfo obj_info( new ObjectInfo( _WXSTR(class_name), GetObjectType( _WXSTR(type) ), package, startGroup ) );

			if ( !icon.empty() && wxFileName::FileExists( iconFullPath ) )
			{
				wxImage img( iconFullPath, wxBITMAP_TYPE_ANY );
				obj_info->SetIconFile( wxBitmap( img.Scale( ICON_SIZE, ICON_SIZE ) ) );
			}
			else
			{
				obj_info->SetIconFile( AppBitmaps::GetBitmap( wxT("unknown"), ICON_SIZE ) );
			}

			if ( !smallIcon.empty() && wxFileName::FileExists( smallIconFullPath ) )
			{
				wxImage img( smallIconFullPath, wxBITMAP_TYPE_ANY );
				obj_info->SetSmallIconFile( wxBitmap( img.Scale( SMALL_ICON_SIZE, SMALL_ICON_SIZE ) ) );
			}
			else
			{
				wxImage img = obj_info->GetIconFile().ConvertToImage();
				obj_info->SetSmallIconFile( wxBitmap( img.Scale( SMALL_ICON_SIZE, SMALL_ICON_SIZE ) ) );
			}

			// Parse the Properties
			std::set< PropertyType > types;
			ParseProperties( elem_obj, obj_info, obj_info->GetCategory(), &types );
			ParseEvents    ( elem_obj, obj_info, obj_info->GetCategory() );

			// Add the ObjectInfo to the map
			m_objs.insert(ObjectInfoMap::value_type( _WXSTR(class_name), obj_info ) );

			// Add the object to the palette
			if ( ShowInPalette( obj_info->GetObjectTypeName() ) )
			{
				package->Add( obj_info );
			}

			elem_obj = elem_obj->NextSiblingElement( OBJINFO_TAG, false );
		}
	}
	catch ( ticpp::Exception& ex )
	{
		THROW_WXFBEX( _WXSTR(ex.m_details) );
	}

	return package;
}

void ObjectDatabase::ParseProperties( ticpp::Element* elem_obj, PObjectInfo obj_info, PPropertyCategory category, std::set< PropertyType >* types )
{
	ticpp::Element* elem_category = elem_obj->FirstChildElement( CATEGORY_TAG, false );
	while ( elem_category )
	{
		// Category name attribute
		std::string cname;
		elem_category->GetAttribute( NAME_TAG, &cname );
		PPropertyCategory new_cat( new PropertyCategory( _WXSTR( cname ) ) );

		// Add category
		category->AddCategory( new_cat );

		// Recurse
		ParseProperties( elem_category, obj_info, new_cat, types );

		elem_category = elem_category->NextSiblingElement( CATEGORY_TAG, false );
	}

	ticpp::Element* elem_prop = elem_obj->FirstChildElement( PROPERTY_TAG, false );
	while ( elem_prop )
	{
		// Property Name Attribute
		std::string pname;
		elem_prop->GetAttribute( NAME_TAG, &pname );
		category->AddProperty( _WXSTR(pname) );

		std::string description;
		elem_prop->GetAttributeOrDefault( DESCRIPTION_TAG, &description, "" );

		std::string customEditor;
		elem_prop->GetAttributeOrDefault( CUSTOM_EDITOR_TAG, &customEditor, "" );

		std::string prop_type;
		elem_prop->GetAttribute( "type", &prop_type );
		PropertyType ptype;
		try
		{
			ptype = ParsePropertyType( _WXSTR( prop_type ) );
		}
		catch( wxFBException& ex )
		{
			wxLogError( wxT("Error: %s\nWhile parsing property \"%s\" of class \"%s\""), ex.what(), _WXSTR(pname), obj_info->GetClassName() );
			elem_prop = elem_prop->NextSiblingElement( PROPERTY_TAG, false );
			continue;
		}

		// Get default value
		std::string def_value;
		try
		{
			ticpp::Node* lastChild = elem_prop->LastChild(false);
			if (lastChild && lastChild->Type() == TiXmlNode::TEXT)
			{
				ticpp::Text* text = lastChild->ToText();
				wxASSERT(text);
				def_value = text->Value();
			}
		}
		catch (ticpp::Exception& ex)
		{
			wxLogDebug(ex.what());
		}

		// if the property is a "bitlist" then parse all of the options
		POptionList opt_list;
		std::list< PropertyChild > children;
		if ( ptype == PT_BITLIST || ptype == PT_OPTION || ptype == PT_EDIT_OPTION )
		{
			opt_list = POptionList( new OptionList() );
			ticpp::Element* elem_opt = elem_prop->FirstChildElement( "option", false );
			while( elem_opt )
			{
				std::string macro_name;
				elem_opt->GetAttribute( NAME_TAG, &macro_name );

				std::string macro_description;
				elem_opt->GetAttributeOrDefault( DESCRIPTION_TAG, &macro_description, "" );

				opt_list->AddOption( _WXSTR(macro_name), _WXSTR(macro_description) );

				m_macroSet.insert( _WXSTR(macro_name) );

				elem_opt = elem_opt->NextSiblingElement( "option", false );
			}
		}
		else if ( ptype == PT_PARENT )
		{
			// If the property is a parent, then get the children
			def_value.clear();
			ticpp::Element* elem_child = elem_prop->FirstChildElement( "child", false );
			while ( elem_child )
			{
				PropertyChild child;

				std::string child_name;
				elem_child->GetAttribute( NAME_TAG, &child_name );
				child.m_name = _WXSTR( child_name );

				std::string child_description;
				elem_child->GetAttributeOrDefault( DESCRIPTION_TAG, &child_description, "" );
				child.m_description = _WXSTR( child_description );

				std::string child_type;
				elem_child->GetAttributeOrDefault( "type", &child_type, "wxString" );
				child.m_type = ParsePropertyType( _WXSTR( child_type ) );

				// Get default value
				// Empty tags don't contain any child so this will throw in that case
				std::string child_value;
				try
				{
					ticpp::Node* lastChild = elem_child->LastChild(false);
					if (lastChild && lastChild->Type() == TiXmlNode::TEXT)
					{
						ticpp::Text* text = lastChild->ToText();
						wxASSERT(text);
						child_value = text->Value();
					}
				}
				catch (ticpp::Exception& ex)
				{
					wxLogDebug(ex.what());
				}
				child.m_defaultValue = _WXSTR(child_value);

				// build parent default value
				if (children.size() > 0)
				{
					def_value += "; ";
				}
				def_value += child_value;

				children.push_back(child);

				elem_child = elem_child->NextSiblingElement( "child", false );
			}
		}

		// create an instance of PropertyInfo
		PPropertyInfo prop_info( new PropertyInfo( _WXSTR(pname), ptype, _WXSTR(def_value), _WXSTR(description), _WXSTR(customEditor), opt_list, children ) );

		// add the PropertyInfo to the property
		obj_info->AddPropertyInfo( prop_info );

		// merge property code templates, once per property type
		if ( types->insert( ptype ).second )
		{
			LangTemplateMap& propLangTemplates = m_propertyTypeTemplates[ ptype ];
			LangTemplateMap::iterator lang;
			for ( lang = propLangTemplates.begin(); lang != propLangTemplates.end(); ++lang )
			{
				if ( lang->second )
				{
					obj_info->AddCodeInfo( lang->first, lang->second );
				}
			}
		}

		elem_prop = elem_prop->NextSiblingElement( PROPERTY_TAG, false );
	}
}

void ObjectDatabase::ParseEvents( ticpp::Element* elem_obj, PObjectInfo obj_info, PPropertyCategory category )
{
	ticpp::Element* elem_category = elem_obj->FirstChildElement( CATEGORY_TAG, false );
	while ( elem_category )
	{
		// Category name attribute
		std::string cname;
		elem_category->GetAttribute( NAME_TAG, &cname );
		PPropertyCategory new_cat( new PropertyCategory( _WXSTR( cname ) ) );

		// Add category
		category->AddCategory( new_cat );

		// Recurse
		ParseEvents( elem_category, obj_info, new_cat );

		elem_category = elem_category->NextSiblingElement( CATEGORY_TAG, false );
	}

	ticpp::Element* elem_evt = elem_obj->FirstChildElement( EVENT_TAG, false );
	while ( elem_evt )
	{
		// Event Name Attribute
		std::string evt_name;
		elem_evt->GetAttribute( NAME_TAG, &evt_name );
		category->AddEvent( _WXSTR(evt_name) );

		// Event class
		std::string evt_class;
		elem_evt->GetAttributeOrDefault( EVENT_CLASS_TAG, &evt_class, "wxEvent" );


		// Help string
		std::string description;
		elem_evt->GetAttributeOrDefault( DESCRIPTION_TAG, &description, "" );

		// Get default value
		std::string def_value;
		try
		{
			ticpp::Node* lastChild = elem_evt->LastChild(false);
			if (lastChild && lastChild->Type() == TiXmlNode::TEXT)
			{
				ticpp::Text* text = lastChild->ToText();
				wxASSERT(text);
				def_value = text->Value();
			}
		}
		catch (ticpp::Exception& ex)
		{
			wxLogDebug(ex.what());
		}

		// create an instance of EventInfo
		PEventInfo evt_info(
		  new EventInfo( _WXSTR(evt_name),  _WXSTR(evt_class), _WXSTR(def_value), _WXSTR(description)));

		// add the EventInfo to the event
		obj_info->AddEventInfo(evt_info);

		elem_evt = elem_evt->NextSiblingElement( EVENT_TAG, false );
	}
}

bool ObjectDatabase::ShowInPalette(wxString type)
{
	return (type == wxT("form")					||
            type == wxT("wizard")               ||
            type == wxT("wizardpagesimple")     ||
			type == wxT("menubar_form")			||
			type == wxT("toolbar_form")			||
			type == wxT("sizer")				||
			type == wxT("gbsizer")				||
			type == wxT("menu")					||
			type == wxT("menuitem")				||
			type == wxT("submenu")				||
			type == wxT("tool")					||
			type == wxT("ribbonbar")			||
			type == wxT("ribbonpage")			||
			type == wxT("ribbonpanel")      		||
			type == wxT("ribbonbuttonbar")  		||
			type == wxT("ribbonbutton")  			||
			type == wxT("ribbondropdownbutton" )	||
			type == wxT("ribbonhybridbutton" )  	||
			type == wxT("ribbontogglebutton" )  	||
			type == wxT("ribbontoolbar")	   		||
			type == wxT("ribbontool")		   	||
			type == wxT("ribbondropdowntool" )  	||
			type == wxT("ribbonhybridtool" ) 		||
			type == wxT("ribbontoggletool" )  		||
			type == wxT("ribbongallery")  			||
			type == wxT("ribbongalleryitem")		||
			type == wxT("dataviewctrl")			||
			type == wxT("dataviewtreectrl")		||
			type == wxT("dataviewlistctrl")		||
			type == wxT("dataviewlistcolumn")		||
			type == wxT("dataviewcolumn")		||
			type == wxT("notebook")				||
			type == wxT("flatnotebook")			||
			type == wxT("listbook")				||
			type == wxT("simplebook")			||
			type == wxT("choicebook")			||
			type == wxT("auinotebook")			||
			type == wxT("widget")				||
			type == wxT("expanded_widget")		||
			type == wxT("propgrid")		||
			type == wxT("propgridman")		||
			type == wxT("propgridpage")		||
			type == wxT("propgriditem")		||
			type == wxT("statusbar")			||
			type == wxT("component")			||
			type == wxT("container")			||
			type == wxT("menubar")				||
			type == wxT("treelistctrl")			||
			type == wxT("treelistctrlcolumn")	||
			type == wxT("toolbar")				||
			type == wxT("nonvisual")			||
			type == wxT("splitter")
			);
}


void ObjectDatabase::ImportComponentLibrary( wxString libfile, PwxFBManager manager )
{
	#ifndef WXFB_PLUGINS_RESOLVE
	wxString path = libfile;

	// Find the GetComponentLibrary function - all plugins must implement this
	typedef IComponentLibrary* (*PFGetComponentLibrary)( IManager* manager );

	#ifdef __WXMAC__
		path += wxT(".dylib");

		// open the library
		void* handle = dlopen( path.mb_str(), RTLD_LAZY );

		if ( !handle )
		{
			wxString error = wxString( dlerror(), wxConvUTF8 );

			THROW_WXFBEX( wxT("Error loading library ") << path << wxT(" ") << error )
		}
		dlerror(); // reset errors

		// load the symbol

		PFGetComponentLibrary GetComponentLibrary = (PFGetComponentLibrary) dlsym(handle, "GetComponentLibrary");
		PFFreeComponentLibrary FreeComponentLibrary = (PFFreeComponentLibrary) dlsym(handle, "FreeComponentLibrary");

		const char *dlsym_error = dlerror();
		if (dlsym_error)
		{
			wxString error = wxString( dlsym_error, wxConvUTF8 );
            THROW_WXFBEX( path << " is not a valid component library: " << error )
			dlclose( handle );
		}
		else
		{
			m_libs.push_back( handle );
		}
	#else

		// Attempt to load the DLL
		wxDynamicLibrary* library = new wxDynamicLibrary( path );
		if ( !library->IsLoaded() )
		{
			THROW_WXFBEX( wxT("Error loading library ") << path )
		}

		m_libs.push_back( library );

		PFGetComponentLibrary GetComponentLibrary =	(PFGetComponentLibrary)library->GetSymbol( wxT("GetComponentLibrary") );
		PFFreeComponentLibrary FreeComponentLibrary =	(PFFreeComponentLibrary)library->GetSymbol( wxT("FreeComponentLibrary") );

		if ( !(GetComponentLibrary && FreeComponentLibrary) )
		{
            THROW_WXFBEX( path << " is not a valid component library" )
		}

	#endif
	LogDebug("[Database::ImportComponentLibrary] Importing " + path + " library");
	// Get the component library
	IComponentLibrary* comp_lib = GetComponentLibrary( (IManager*)manager.get() );

	// Store the function to free the library
	m_componentLibs[ FreeComponentLibrary ] = comp_lib;
	#else
	wxString path;
	auto pluginLibrary = m_pluginLibraries.emplace(std::piecewise_construct, std::forward_as_tuple(libfile), std::forward_as_tuple()).first;
	try {
		pluginLibrary->second.sharedLibrary.load(libfile.ToStdString(),
			boost::dll::load_mode::default_mode |        // Use platform default parameters
			boost::dll::load_mode::append_decorations |  // Only the library base name is supplied, it needs to be decorated for the platform
			boost::dll::load_mode::search_system_folders // This enables usage of RPATH
		);
		path = pluginLibrary->second.sharedLibrary.location().native();
	} catch (const std::system_error& ex) {
		THROW_WXFBEX("Error loading library " << libfile << ": " << ex.what())
	}
	try {
		pluginLibrary->second.getComponentLibrary = pluginLibrary->second.sharedLibrary.get<IComponentLibrary*(IManager*)>("GetComponentLibrary");
		pluginLibrary->second.freeComponentLibrary = pluginLibrary->second.sharedLibrary.get<void(IComponentLibrary*)>("FreeComponentLibrary");
	} catch (const std::system_error& ex) {
		THROW_WXFBEX(path << " is not a valid component library: " << ex.what())
	}

	LogDebug("[Database::ImportComponentLibrary] Importing " + path + " library");
	pluginLibrary->second.componentLibrary = pluginLibrary->second.getComponentLibrary(manager.get());
	auto* comp_lib = pluginLibrary->second.componentLibrary;
	#endif

	// Import all of the components
	for ( unsigned int i = 0; i < comp_lib->GetComponentCount(); i++ )
	{
		wxString class_name = comp_lib->GetComponentName( i );
		IComponent* comp = comp_lib->GetComponent( i );

		// Look for the class in the data read from the .xml files
		PObjectInfo class_info = GetObjectInfo( class_name );
		if ( class_info )
		{
			class_info->SetComponent(comp);
		}
		else
		{
            LogDebug("ObjectInfo for <" + class_name + "> not found while loading library <" + path + ">");
		}
	}

	// Add all of the macros in the library to the macro dictionary
	PMacroDictionary dic = MacroDictionary::GetInstance();
	for ( unsigned int i = 0; i < comp_lib->GetMacroCount(); i++ )
	{
		wxString name = comp_lib->GetMacroName( i );
		int value = comp_lib->GetMacroValue( i );
		dic->AddMacro( name, value );
		m_macroSet.erase( name );
	}
}

PropertyType ObjectDatabase::ParsePropertyType( wxString str )
{
	PropertyType result;
	PTMap::iterator it = m_propTypes.find(str);
	if (it != m_propTypes.end())
		result = it->second;
	else
	{
		THROW_WXFBEX( wxString::Format( wxT("Unknown property type \"%s\""), str ) );
	}

	return result;
}

wxString  ObjectDatabase::ParseObjectType( wxString str )
{
	return str;
}


#define PT(x,y) m_propTypes.insert(PTMap::value_type(x,y))
void ObjectDatabase::InitPropertyTypes()
{
	PT( wxT("bool"),			PT_BOOL		);
	PT( wxT("text"),			PT_TEXT		);
	PT( wxT("int"),				PT_INT		);
	PT( wxT("uint"),			PT_UINT		);
	PT( wxT("bitlist"),		PT_BITLIST		);
	PT( wxT("intlist"),		PT_INTLIST		);
	PT( wxT("uintlist"),	PT_UINTLIST		);
	PT(wxT("intpairlist"), PT_INTPAIRLIST);
	PT(wxT("uintpairlist"), PT_UINTPAIRLIST);
	PT( wxT("option"),		PT_OPTION		);
	PT( wxT("macro"),		PT_MACRO		);
	PT( wxT("path"),		PT_PATH			);
	PT( wxT("file"),		PT_FILE			);
	PT( wxT("wxString"), 	PT_WXSTRING		);
	PT( wxT("wxPoint"),		PT_WXPOINT		);
	PT( wxT("wxSize"),		PT_WXSIZE		);
	PT( wxT("wxFont"),		PT_WXFONT		);
	PT( wxT("wxColour"),	PT_WXCOLOUR		);
	PT( wxT("bitmap"),		PT_BITMAP		);
	PT( wxT("wxString_i18n"),PT_WXSTRING_I18N);
	PT( wxT("stringlist"),	PT_STRINGLIST	);
	PT( wxT("float"),		PT_FLOAT		);
	PT( wxT("parent"),		PT_PARENT		);
	PT( wxT("editoption"),	PT_EDIT_OPTION	);
}

bool ObjectDatabase::LoadObjectTypes()
{
	ticpp::Document doc;
	wxString xmlPath = m_xmlPath + wxT("objtypes.xml");
	XMLUtils::LoadXMLFile( doc, true, xmlPath );

	// First load the object types, then the children
	try
	{
		ticpp::Element* root = doc.FirstChildElement("definitions");
		ticpp::Element* elem = root->FirstChildElement( "objtype" );
		while ( elem )
		{
			bool hidden;
			elem->GetAttributeOrDefault( "hidden", &hidden, false );

			bool item;
			elem->GetAttributeOrDefault( "item", &item, false );

			wxString name = _WXSTR( elem->GetAttribute("name") );

			PObjectType objType( new ObjectType( name, (int)m_types.size(), hidden, item ) );
			m_types.insert( ObjectTypeMap::value_type( name, objType ) );

			elem = elem->NextSiblingElement( "objtype", false );
		}

		// now load the children
		elem = root->FirstChildElement("objtype");
		while (elem)
		{
			wxString name = _WXSTR( elem->GetAttribute("name") );

			// get the objType
			PObjectType objType = GetObjectType( name );
			ticpp::Element* child = elem->FirstChildElement( "childtype", false );
			while ( child )
			{
				int nmax = -1; // no limit
				int aui_nmax = -1; // no limit
				child->GetAttributeOrDefault( "nmax", &nmax, -1 );
				child->GetAttributeOrDefault( "aui_nmax", &aui_nmax, -1 );

				wxString childname = _WXSTR( child->GetAttribute("name") );

				PObjectType childType = GetObjectType( childname );
				if ( !childType )
				{
					wxLogError( _("No Object Type found for \"%s\""), childname );
					continue;
				}

				objType->AddChildType( childType, nmax, aui_nmax );

				child = child->NextSiblingElement( "childtype", false );
			}
			elem = elem->NextSiblingElement( "objtype", false );
		}
	}
	catch( ticpp::Exception& ex )
	{
		wxLogError( _WXSTR( ex.m_details ) );
		return false;
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
