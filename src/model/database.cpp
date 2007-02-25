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

#include "objectbase.h"
#include "database.h"
#include "rad/bitmaps.h"
#include "utils/stringutils.h"
#include "utils/typeconv.h"
#include "utils/debug.h"
#include "utils/wxfbexception.h"
#include "rad/appdata.h"
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/dir.h>
#include <ticpp.h>
#include <wx/config.h>
#include <wx/tokenzr.h>

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
#define HIDDEN_TAG "hidden"     //Juan
#define ICON_TAG "icon"
#define SMALL_ICON_TAG "smallIcon"
#define EXPANDED_TAG "expanded"


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


///////////////////////////////////////////////////////////////////////////////

ObjectDatabase::ObjectDatabase()
{
	//InitObjectTypes();
	//  InitWidgetTypes();
	InitPropertyTypes();
}

ObjectDatabase::~ObjectDatabase()
{
	// destruir todos los objetos
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
		int max = parentType->FindChildType(objType);

		// FIXME! Esto es un parche para evitar crear los tipos menubar,statusbar y
		// toolbar en un form que no sea wxFrame.
		// Hay que modificar el conjunto de tipos para permitir tener varios tipos
		// de forms (como childType de project), pero hay mucho código no válido
		// para forms que no sean de tipo "form". Dicho de otra manera, hay
		// código que dependen del nombre del tipo, cosa que hay que evitar.
		if (parentType->GetName() == wxT("form") && parent->GetClassName() != wxT("Frame") &&
			(objType->GetName() == wxT("statusbar") ||
			objType->GetName() == wxT("menubar") ||
			objType->GetName() == wxT("toolbar") ))
			return PObjectBase(); // tipo no válido

		if (max != 0) // tipo válido
		{
			bool create = true;

			// comprobamos el número de instancias
			if (max > 0 && CountChildrenWithSameType(parent, objType) >= max)
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
				int max = childType->FindChildType(objType);

				if (childType->IsItem() && max != 0)
				{
					max = parentType->FindChildType(childType);

					// si el tipo es un item y además el tipo del objeto a crear
					// puede ser hijo del tipo del item vamos a intentar crear la
					// instancia del item para crear el objeto como hijo de este
					if (max < 0 || CountChildrenWithSameType(parent, childType) < max)
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
		Debug::Print( wxT("SetDefaultLayoutProperties expects a subclass of sizeritembase") );
		return;
	}

	PObjectBase child = sizeritem->GetChild(0);
	PObjectInfo childInfo = child->GetObjectInfo();
	wxString obj_type = child->GetObjectTypeName();

	PProperty proportion = sizeritem->GetProperty( wxT("proportion") );

	if ( childInfo->IsSubclassOf( wxT("sizer") ) || obj_type == wxT("splitter") || childInfo->GetClassName() == wxT("spacer") )
	{
		if ( proportion )
		{
			proportion->SetValue( wxT("1") );
		}
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
				obj_type == wxT("choicebook")		||
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

PObjectBase  ObjectDatabase::CreateObject(TiXmlElement *xml_obj, PObjectBase parent)
{
	if (!xml_obj->Attribute(CLASS_TAG))
		return PObjectBase();

	std::string class_name = xml_obj->Attribute(CLASS_TAG);
	PObjectBase object = CreateObject(class_name,parent);

	if (object)
	{
		// Get the state of expansion in the object tree
		int expanded = 1;
		if ( TIXML_SUCCESS != xml_obj->QueryIntAttribute( EXPANDED_TAG, &expanded ) )
		{
			object->SetExpanded( true );
		}
		else
		{
			object->SetExpanded( (expanded != 0) );
		}

		// modificamos cada propiedad
		TiXmlElement *xml_prop = xml_obj->FirstChildElement(PROPERTY_TAG);
		while (xml_prop)
		{
			std::string prop_value;
			std::string prop_name = xml_prop->Attribute(NAME_TAG);
			PProperty prop = object->GetProperty( _WXSTR(prop_name) );
			if (prop) // ¿ existe la propiedad ?
			{
				// modificamos el valor
				TiXmlNode *xml_value = xml_prop->FirstChild();
				if (xml_value && xml_value->ToText())
					prop_value = xml_value->ToText()->Value();
				else
					prop_value = ""; // valor nulo

				prop->SetValue( _WXSTR(prop_value) );
			}

			xml_prop = xml_prop->NextSiblingElement(PROPERTY_TAG);
		}

		// now, we modify every event handler
		TiXmlElement *xml_event = xml_obj->FirstChildElement(EVENT_TAG);
		while (xml_event)
		{
		  std::string event_value;
			std::string event_name = xml_event->Attribute(NAME_TAG);
			PEvent event = object->GetEvent( _WXSTR(event_name) );
			if (event)
			{
				TiXmlNode *xml_value = xml_event->FirstChild();
				if (xml_value && xml_value->ToText())
					event_value = xml_value->ToText()->Value();
				else
					event_value = "";

				event->SetValue( _WXSTR(event_value) );
			}

			xml_event = xml_event->NextSiblingElement(EVENT_TAG);
		}


		if (parent)
		{
			// enlazamos
			parent->AddChild(object);
			object->SetParent(parent);
		}

		// creamos los hijos
		TiXmlElement *child = xml_obj->FirstChildElement(OBJECT_TAG);
		while (child)
		{
			PObjectBase childObj = CreateObject(child,object);
			child = child->NextSiblingElement(OBJECT_TAG);
		}
	}
	return object;
}

//////////////////////////////

bool IncludeInPalette(wxString type)
{
	return true;
}

void ObjectDatabase::LoadPlugins( PwxFBManager manager )
{
	// Load some default templates
	LoadPackage( m_xmlPath + wxT("default.xml"), m_iconPath );
	LoadCodeGen( m_xmlPath + wxT("default.cppcode") );

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
								// Setup the inheritance for base classes
								wxFileName fullNextPluginPath( nextPluginPath );
								if ( !fullNextPluginPath.IsAbsolute() )
								{
									fullNextPluginPath.MakeAbsolute();
								}
								SetupPackage( nextXmlFile.GetFullPath(), fullNextPluginPath.GetFullPath(), manager );

								// Load the C++ code tempates
								nextXmlFile.SetExt( wxT("cppcode") );
								LoadCodeGen( nextXmlFile.GetFullPath() );
								if ( !packages.insert( PackageMap::value_type( package->GetPackageName(), package ) ).second )
								{
									wxLogError( _("There are two plugins named \"%s\""), package->GetPackageName().c_str() );
								}
							}
						}
						catch ( wxFBException& ex )
						{
							wxLogError( ex.what() );
						}
						moreXmlFiles = pluginXmlDir.GetNext( &packageXmlFile );
					}
				}
    		}
    	}

        moreDirectories = pluginsDir.GetNext( &pluginDirName );
    }

    // Get previous plugin order
	wxConfigBase* config = wxConfigBase::Get();
	wxString pages = config->Read( wxT("/palette/pageOrder"), wxT("Common,Additional,Menu/Toolbar,Layout,Forms,") );

	// Add packages to the vector in the correct order
	wxStringTokenizer packageList( pages, wxT(",") );
	while ( packageList.HasMoreTokens() )
	{
		wxString packageName = packageList.GetNextToken();
		PackageMap::iterator packageIt = packages.find( packageName );
		if ( packages.end() == packageIt )
		{
			// Plugin missing - move on
			continue;
		}
		m_pkgs.push_back( packageIt->second );
		packages.erase( packageIt );
	}

    // If any packages remain in the map, they are new plugins and must still be added
    for ( PackageMap::iterator packageIt = packages.begin(); packageIt != packages.end(); ++packageIt )
    {
    	m_pkgs.push_back( packageIt->second );
    }
}

void ObjectDatabase::SetupPackage( const wxString& file, const wxString& libPath, PwxFBManager manager )
{
	try
	{
		ticpp::Document doc;
		XMLUtils::LoadXMLFile( doc, file );

		ticpp::Element* root = doc.FirstChildElement( PACKAGE_TAG );

		// get the library to import
		std::string lib;
		root->GetAttributeOrDefault( "lib", &lib, "" );
		if ( !lib.empty() )
		{
			// Allows plugin dependency dlls to be next to plugin dll in windows
			wxString workingDir = ::wxGetCwd();
			wxFileName::SetCwd( libPath );
			try
			{
				ImportComponentLibrary( libPath + wxFILE_SEP_PATH + _WXSTR(lib), manager );
			}
			catch ( ... )
			{
				// Put Cwd back
				wxFileName::SetCwd( workingDir );
				throw;
			}

			// Put Cwd back
			wxFileName::SetCwd( workingDir );
		}

		ticpp::Element* elem_obj = root->FirstChildElement( OBJINFO_TAG, false );
		while ( elem_obj )
		{
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
			if ( HasCppProperties( class_info->GetObjectTypeName() ) )
			{
				PObjectInfo cpp_interface = GetObjectInfo( wxT("C++") );
				if ( cpp_interface )
				{
					class_info->AddBaseClass( cpp_interface );
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
			type == wxT("choicebook")		||
			type == wxT("widget")			||
			type == wxT("expanded_widget")	||
			type == wxT("statusbar")			||
			type == wxT("component")			||
			type == wxT("container")			||
			type == wxT("menubar")			||
			type == wxT("toolbar")			||
			type == wxT("splitter")
			);
}

void ObjectDatabase::LoadCodeGen( const wxString& file )
{
	try
	{
		ticpp::Document doc;
		XMLUtils::LoadXMLFile( doc, file );

		// read the codegen element
		ticpp::Element* elem_codegen = doc.FirstChildElement("codegen");
		std::string language;
		elem_codegen->GetAttribute( "language", &language );

		// read the templates
		ticpp::Element* elem_templates = elem_codegen->FirstChildElement( "templates", false );
		while ( elem_templates  )
		{
			std::string class_name;
			elem_templates->GetAttribute( "class", &class_name );

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

			PObjectInfo obj_info = GetObjectInfo( _WXSTR(class_name) );
			if ( obj_info )
			{
				obj_info->AddCodeInfo( _WXSTR(language), code_info );
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
		XMLUtils::LoadXMLFile( doc, file );

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

			std::string widget;
			elem_obj->GetAttributeOrDefault( "widget", &widget, "" );

			std::string icon;
			elem_obj->GetAttributeOrDefault( "icon", &icon, "" );
			wxString iconFullPath = iconPath + wxFILE_SEP_PATH + _WXSTR(icon);

			std::string smallIcon;
			elem_obj->GetAttributeOrDefault( "smallIcon", &smallIcon, "" );
			wxString smallIconFullPath = iconPath + wxFILE_SEP_PATH + _WXSTR(smallIcon);

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
			ParseProperties( elem_obj, obj_info, obj_info->GetCategory() );
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

void ObjectDatabase::ParseProperties( ticpp::Element* elem_obj, PObjectInfo obj_info, PPropertyCategory category )
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
		ParseProperties( elem_category, obj_info, new_cat );

		elem_category = elem_category->NextSiblingElement( CATEGORY_TAG, false );
	}

	ticpp::Element* elem_prop = elem_obj->FirstChildElement( PROPERTY_TAG, false );
	while ( elem_prop )
	{
		// Property Name Attribute
		std::string pname;
		elem_prop->GetAttribute( NAME_TAG, &pname );
		category->AddProperty( _WXSTR(pname) );

		// Hidden Attribute
		bool hidden;
		elem_prop->GetAttributeOrDefault( HIDDEN_TAG, &hidden, false );

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
			wxLogError( wxT("Error: %s\nWhile parsing property \"%s\" of class \"%s\""), ex.what(), _WXSTR(pname).c_str(), obj_info->GetClassName().c_str() );
			elem_prop = elem_prop->NextSiblingElement( PROPERTY_TAG, false );
			continue;
		}

		// Get default value
		std::string def_value;
		try
		{
			ticpp::Node* lastChild = elem_prop->LastChild();
			ticpp::Text* text = lastChild->ToText();
			def_value = text->Value();
		}
		catch( ticpp::Exception& ){}

		// if the property is a "bitlist" then parse all of the options
		POptionList opt_list;
		std::list< PropertyChild > children;
		if ( ptype == PT_BITLIST || ptype == PT_OPTION )
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

				// Get default value
				try
				{
					ticpp::Node* lastChild = elem_child->LastChild();
					ticpp::Text* text = lastChild->ToText();
					child.m_defaultValue = _WXSTR( text->Value() );

					// build parent default value
					if ( children.size() > 0 )
					{
						def_value += "; ";
					}
					def_value += text->Value();
				}
				catch( ticpp::Exception& ){}

				children.push_back( child );

				elem_child = elem_child->NextSiblingElement( "child", false );
			}
		}

		// create an instance of PropertyInfo
		PPropertyInfo prop_info( new PropertyInfo( _WXSTR(pname), ptype, _WXSTR(def_value), _WXSTR(description), _WXSTR(customEditor), hidden, opt_list, children ) );

		// add the PropertyInfo to the property
		obj_info->AddPropertyInfo(prop_info);


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
			ticpp::Node* lastChild = elem_evt->LastChild();
			ticpp::Text* text = lastChild->ToText();
			def_value = text->Value();
		}
		catch( ticpp::Exception& ){}

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
	return (type == wxT("form")				||
			type == wxT("sizer")			||
			type == wxT("gbsizer")			||
			type == wxT("menu")				||
			type == wxT("menuitem")			||
			type == wxT("submenu")			||
			type == wxT("tool")				||
			type == wxT("notebook")			||
			type == wxT("flatnotebook")		||
			type == wxT("listbook")			||
			type == wxT("choicebook")		||
			type == wxT("widget")			||
			type == wxT("expanded_widget")	||
			type == wxT("statusbar")			||
			type == wxT("component")			||
			type == wxT("container")			||
			type == wxT("menubar")			||
			type == wxT("toolbar")			||
			type == wxT("splitter")
			);
}


void ObjectDatabase::ImportComponentLibrary( wxString libfile, PwxFBManager manager )
{
	wxString path = libfile;

	// This will prevent loading debug libraries in release and vice versa
	// That used to cause crashes when trying to debug
	#ifdef __WXFB_DEBUG__
		path += wxT("d");
	#endif

	// Attempt to load the DLL
	wxDynamicLibrary* library = new wxDynamicLibrary( path );

	if ( !library->IsLoaded() )
	{
		THROW_WXFBEX( wxT("Error loading library ") << path )
	}

	m_libs.push_back( library );

	// Find the GetComponentLibrary function - all plugins must implement this
	typedef IComponentLibrary* (*PFGetComponentLibrary)( IManager* manager );
	PFGetComponentLibrary GetComponentLibrary =	(PFGetComponentLibrary)library->GetSymbol( wxT("GetComponentLibrary") );

	if ( !GetComponentLibrary )
	{
		THROW_WXFBEX( path << wxT(" is not a valid component library") )
	}

	Debug::Print( wxT("[Database::ImportComponentLibrary] Importing %s library"), path.c_str() );

	// Get the component library
	IComponentLibrary* comp_lib = GetComponentLibrary( (IManager*)manager.get() );

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
			Debug::Print( wxT("ObjectInfo for <%s> not found while loading library <%s>"), class_name.c_str(), path.c_str() );
		}
	}

	// Add all of the macros in the library to the macro dictionary
	for ( unsigned int i = 0; i < comp_lib->GetMacroCount(); i++ )
	{
		PMacroDictionary dic = MacroDictionary::GetInstance();
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
		result = PT_ERROR;
		THROW_WXFBEX( wxString::Format( wxT("Unknown property type \"%s\""), str.c_str() ) );
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
	PT( wxT("option"),		PT_OPTION		);
	PT( wxT("macro"),		PT_MACRO		);
	PT( wxT("path"),			PT_PATH			);
	PT( wxT("wxString"), 	PT_WXSTRING		);
	PT( wxT("wxPoint"),		PT_WXPOINT		);
	PT( wxT("wxSize"),		PT_WXSIZE		);
	PT( wxT("wxFont"),		PT_WXFONT		);
	PT( wxT("wxColour"),		PT_WXCOLOUR		);
	PT( wxT("bitmap"),		PT_BITMAP		);
	PT( wxT("wxString_i18n"),PT_WXSTRING_I18N);
	PT( wxT("stringlist"),	PT_STRINGLIST	);
	PT( wxT("float"),		PT_FLOAT		);
	PT( wxT("parent"),		PT_PARENT		);
}

bool ObjectDatabase::LoadObjectTypes()
{
	TiXmlDocument doc;
	wxString xmlPath = m_xmlPath + wxT("objtypes.xml");
	XMLUtils::LoadXMLFile( doc, xmlPath );

	// se realizará en dos pasos, primero importamos cada uno de los ObjectTypes
	// definidos y a continuación se añadirán los posibles ObjectTypes hijos
	// de cada uno.
	TiXmlElement* root = doc.FirstChildElement("definitions");
	if (root)
	{
		TiXmlElement* elem = root->FirstChildElement("objtype");
		while (elem)
		{
			bool hidden = false, item = false;
			std::string name = elem->Attribute("name");
			if (elem->Attribute("hidden") && std::string(elem->Attribute("hidden"))=="1")
				hidden = true;
			if (elem->Attribute("item") && std::string(elem->Attribute("item"))=="1")
				item = true;

			int id = (int)m_types.size();
			PObjectType objType( new ObjectType( _WXSTR(name), id, hidden, item ) );
			m_types.insert(ObjectTypeMap::value_type( _WXSTR(name), objType ) );

			elem = elem->NextSiblingElement("objtype");
		}

		// ahora continuamos registrando añadiendo las referencias de cada
		// posible hijo
		elem = root->FirstChildElement("objtype");
		while (elem)
		{
			std::string name = elem->Attribute("name");

			// obtenemos el objType
			PObjectType objType = GetObjectType(_WXSTR(name));
			//wxLogMessage(wxString::Format(wxT("ObjectType %s can be parent of..."),name.c_str()));
			TiXmlElement *child = elem->FirstChildElement("childtype");
			while (child)
			{
				int nmax = -1; // sin límite
				std::string childname = child->Attribute("name");
				//wxLogMessage(wxString::Format(wxT("%s"),childname.c_str()));


				if (child->Attribute("nmax"))
					nmax = TypeConv::StringToInt(_WXSTR(child->Attribute("nmax")));
				//nmax = 1;

				PObjectType childType = GetObjectType(_WXSTR(childname));
				assert(childType);

				objType->AddChildType(childType, nmax);

				child = child->NextSiblingElement("childtype");
			}
			elem = elem->NextSiblingElement("objtype");
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


