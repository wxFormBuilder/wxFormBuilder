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

// Atencion!!!!!!
// ObjectBase::GetPropertyCount() != ObjectInfo::GetPropertyCount()
//
// En el primer caso devolverá el numero total de propiedades del objeto.
// En el segundo caso sólo devolverá el número de propiedades definidas
// para esa clase.

#ifndef __OBJ__
#define __OBJ__

#include <iostream>
#include <wx/string.h>
#include <map>
#include <vector>
#include <set>
#include <list>
#include <boost/smart_ptr.hpp>
#include "types.h"
#include "tinyxml.h"

#include "wx/wx.h"
#include <component.h>

using namespace std;
using namespace boost;

class ObjectBase;
class ObjectInfo;
class Property;
class PropertyInfo;
class OptionList;
class CodeInfo;


typedef shared_ptr<OptionList> POptionList;
typedef shared_ptr<ObjectBase> PObjectBase;
typedef weak_ptr<ObjectBase> WPObjectBase;

typedef shared_ptr<CodeInfo> PCodeInfo;
typedef shared_ptr<ObjectInfo> PObjectInfo;
typedef shared_ptr<Property> PProperty;
typedef shared_ptr<PropertyInfo> PPropertyInfo;

typedef vector< PObjectBase > ObjectVector;
typedef map<string,PProperty> PropertyMap;
typedef map<string, PPropertyInfo> PropertyInfoMap;
typedef vector<PObjectInfo> ObjectInfoVector;
typedef map<string, PObjectInfo> ObjectInfoMap;

///////////////////////////////////////////////////////////////////////////////

class OptionList
{
private:

	map< wxString, wxString > m_options;

public:

	void AddOption( wxString option, wxString description = wxString() )
	{
		m_options[option] = description;
	}
	unsigned int GetOptionCount()
	{
		return (unsigned int)m_options.size();
	}
	const map< wxString, wxString >& GetOptions()
	{
		return m_options;
	}
};

///////////////////////////////////////////////////////////////////////////////

/**
@internal
Data Container for children of a Parent property
*/
class PropertyChild
{
public:
	wxString m_name;
	wxString m_defaultValue;
	wxString m_description;
};

///////////////////////////////////////////////////////////////////////////////

class PropertyInfo
{
	friend class Property;

private:
	wxString       m_name;
	PropertyType m_type;
	wxString       m_def_value;
	shared_ptr<OptionList>  m_opt_list;
	std::list< PropertyChild > m_children; // Only used for parent properties
	bool m_hidden; // Juan. Determina si la propiedad aparece o no en XRC
	wxString		m_description;

public:

	PropertyInfo(wxString name, PropertyType type, wxString def_value, wxString description,
		bool hidden, shared_ptr<OptionList> opt_list, const std::list< PropertyChild >& children ); //Juan

	~PropertyInfo();

	wxString       GetDefaultValue()        { return m_def_value;  }
	PropertyType GetType()                { return m_type;       }
	wxString       GetName()                { return m_name;       }
	shared_ptr<OptionList>  GetOptionList ()         { return m_opt_list;   }
	std::list< PropertyChild >* GetChildren(){ return &m_children; }
	wxString		 GetDescription	()		  { return m_description;}
	bool         IsHidden()               { return m_hidden; } // Juan
	void         SetHidden(bool hidden)   { m_hidden = hidden; } // Juan
};

///////////////////////////////////////////////////////////////////////////////

class Property
{
private:
	shared_ptr<PropertyInfo> m_info; // puntero a su descriptor
	weak_ptr<ObjectBase> m_object; // una propiedad siempre pertenece a un objeto

	wxString m_name;
	wxString m_value;

public:
	Property(shared_ptr<PropertyInfo> info, shared_ptr<ObjectBase> obj = shared_ptr<ObjectBase>())
	{
		m_name = info->GetName();
		m_object = obj;
		m_info = info;
	};

	shared_ptr<ObjectBase> GetObject() { return m_object.lock(); }
	wxString GetName() { return m_name; };
	wxString GetValue() { return m_value; };
	void SetValue( wxString& val )
	{
		m_value = val;
	};
	void SetValue( wxChar* val )
	{
		m_value = val;
	};
	shared_ptr<PropertyInfo> GetPropertyInfo() { return m_info; }
	PropertyType GetType()          { return m_info->GetType();  }

	bool IsDefaultValue();
	bool IsNull();
	void SetDefaultValue();
	void ChangeDefaultValue( const wxString& value )
	{
		m_info->m_def_value = value;
	}

	////////////////////
	void SetValue(const wxFont &font);
	void SetValue(const wxColour &colour);
	void SetValue(const wxString &str, bool format = false);
	void SetValue(const wxPoint &point);
	void SetValue(const wxSize &size);
	void SetValue(const int integer);
	void SetValue(const double val );

	wxFont   GetValueAsFont();
	wxColour GetValueAsColour();
	wxPoint  GetValueAsPoint();
	wxSize   GetValueAsSize();
	int      GetValueAsInteger();
	wxString GetValueAsString();
	wxBitmap GetValueAsBitmap();
	wxString GetValueAsText();   // sustituye los ('\n',...) por ("\\n",...)

	wxArrayString GetValueAsArrayString();
	double GetValueAsFloat();
	void SplitParentProperty( std::map< wxString, wxString >* children );
	wxString GetChildFromParent( const wxString& childName );

};

class PropertyCategory
{
private:
	wxString m_name;
	vector< wxString > m_properties;
	vector< shared_ptr< PropertyCategory > > m_categories;

public:

	PropertyCategory( wxString name ) : m_name( name ){}
	void AddProperty( wxString name ){ m_properties.push_back( name ); }
	void AddCategory( shared_ptr< PropertyCategory > category ){ m_categories.push_back( category ); }
	wxString GetName(){ return m_name; }
	wxString GetPropertyName( size_t index )
	{
		if ( index < m_properties.size() )
		{
			return m_properties[ index ];
		}
		else
		{
			return wxEmptyString;
		}
	}

	shared_ptr< PropertyCategory > GetCategory( size_t index )
	{
		if ( index < m_categories.size() )
		{
			return m_categories[ index ];
		}
		else
		{
			return shared_ptr< PropertyCategory >(  );
		}
	}
	size_t GetPropertyCount() { return m_properties.size(); }
	size_t GetCategoryCount() { return m_categories.size(); }
};

///////////////////////////////////////////////////////////////////////////////

namespace ticpp
{
	class Document;
	class Element;
}

class ObjectBase : public IObject, public enable_shared_from_this<ObjectBase>
{
private:
	// number of instances of the class, será util para comprobar si
	// efectivamente no se están produciendo leaks de memoria
	static int s_instances;

	wxString m_class;            // clase que repesenta el objeto
	wxString m_type; //ObjectType m_type;         // RTTI
	weak_ptr<ObjectBase> m_parent;     // no parent

	vector< shared_ptr<ObjectBase> > m_children;   // children
	map< wxString,shared_ptr<Property> >  m_properties; // Properties of the object
	shared_ptr<ObjectInfo> m_info;


protected:
	// utilites for implementing the tree
	static const int INDENT;  // size of indent
	wxString GetIndentString(int indent); // obtiene la cadena con el indentado

	vector< shared_ptr<ObjectBase> >& GetChildren()   { return m_children; };
	map< wxString,shared_ptr<Property> >&  GetProperties() { return m_properties; };

	// Crea un elemento del objeto
	void SerializeObject( ticpp::Element* serializedElement );

	// devuelve el puntero "this"
	shared_ptr<ObjectBase> GetThis()
	{
		return shared_from_this();
	}


	//bool DoChildTypeOk (wxString type_child ,wxString type_parent);
	/*
	* Configura la instancia en su creación.
	*
	* Este método realiza todas las operaciones necesarias para configurar
	* el objeto en su creación.
	* Las clases derivadas (y esta misma) deberán llamar a esta función en
	* el método NewInstance. De esta forma, emulamos el comportamiento de un
	* constructor.
	*/
	//  void SetupInstance(shared_ptr<ObjectBase> parent = shared_ptr<ObjectBase>());


public:
	/**
	* Constructor. (debe ser "protegido" -> NewInstance)
	*/
	ObjectBase (wxString class_name);

	// Mejor es que sea el propio objeto quien construya todas sus propiedades...
	//ObjectBase(shared_ptr<ObjectInfo> obj_info);

	/*
	* Pseudo-Constructor.
	* Crea una instancia de un objeto de forma dinámica.
	* De esta forma evitamos los problemas de C++ para usar métodos virtuales
	* en un contructor, o el problema de devolver un shared_ptr en lugar
	* de un puntero normal.
	*/
	/*  static shared_ptr<ObjectBase> NewInstance
	(wxString class_name, shared_ptr<ObjectBase> parent = shared_ptr<ObjectBase>());*/


	/**
	* Destructor.
	*/
	virtual ~ObjectBase();

	/**
	* Obtiene el nombre del objeto.
	*
	* @note No confundir con la propiedad nombre que tienen algunos objetos.
	*       Cada objeto tiene un nombre, el cual será el mismo que el usado
	*       como clave en el registro de descriptores.
	*/
	wxString     GetClassName ()            { return m_class;  }

	/**
	* Obtiene el nodo padre.
	*/
	shared_ptr<ObjectBase> GetParent ()                   { return m_parent.lock();   }

	/**
	* Establece la conexión con el padre.
	*/
	void SetParent(shared_ptr<ObjectBase> parent)  { m_parent = parent; }

	/**
	* Obtiene la propiedad identificada por el nombre.
	*
	* @note Notar que no existe el método SetProperty, ya que la modificación
	*       se hace a través de la referencia.
	*/
	shared_ptr<Property> GetProperty (wxString name);

	/**
	* Añade una propiedad al objeto.
	*
	* Este método será usado por el registro de descriptores para crear la
	* instancia del objeto.
	* Los objetos siempre se crearán a través del registro de descriptores.
	*/
	void AddProperty (wxString propname, shared_ptr<Property> value);

	/**
	* Obtiene el número de propiedades del objeto.
	*/
	unsigned int GetPropertyCount() { return (unsigned int)m_properties.size(); }

	/**
	* Obtiene una propiedad del objeto.
	* @todo esta función deberá lanzar una excepción en caso de no encontrarse
	*       dicha propiedad, así se simplifica el código al no tener que hacer
	*       tantas comprobaciones.
	* Por ejemplo, el código sin excepciones sería algo así:
	*
	* @code
	*
	* shared_ptr<Property> plabel = obj->GetProperty("label");
	* shared_ptr<Property> ppos = obj->GetProperty("pos");
	* shared_ptr<Property> psize = obj->GetProperty("size");
	* shared_ptr<Property> pstyle = obj->GetProperty("style");
	*
	* if (plabel && ppos && psize && pstyle)
	* {
	*   wxButton *button = new wxButton(parent,-1,
	*    plabel->GetValueAsString(),
	*    ppos->GetValueAsPoint(),
	*    psize->GetValueAsSize(),
	*    pstyle->GetValueAsInteger());
	* }
	* else
	* {
	*   // manejo del error
	* }
	*
	* @endcode
	*
	* y con excepciones:
	*
	* @code
	*
	* try
	* {
	*   wxButton *button = new wxButton(parent,-1,
	*     obj->GetProperty("label")->GetValueAsString(),
	*     obj->GetProperty("pos")->GetValueAsPoint(),
	*     obj->GetProperty("size")->GetValueAsSize(),
	*     obj->GetProperty("style")->GetValueAsInteger());
	* }
	* catch (...)
	* {
	*   // manejo del error
	* }
	*
	* @endcode
	*
	*/
	shared_ptr<Property>    GetProperty (unsigned int idx); // throws ...;

	/**
	* Devuelve el primer antecesor cuyo tipo coincida con el que se pasa
	* como parámetro.
	*
	* Será útil para encontrar el widget padre.
	*/
	shared_ptr<ObjectBase> FindNearAncestor(wxString type);

	/**
	* Obtiene el documento xml del arbol tomando como raíz el nodo actual.
	*/
	void Serialize( ticpp::Document* serializedDocument );

	/**
	* Añade un hijo al objeto.
	* Esta función es virtual, debido a que puede variar el comportamiento
	* según el tipo de objeto.
	*
	* @return true si se añadió el hijo con éxito y false en caso contrario.
	*/
	virtual bool AddChild    (shared_ptr<ObjectBase>);
	virtual bool AddChild    (unsigned int idx, shared_ptr<ObjectBase> obj);

	/**
	* Devuelve la posicion del hijo o GetChildCount() en caso de no encontrarlo
	*/
	unsigned int GetChildPosition(shared_ptr<ObjectBase> obj);
	bool ChangeChildPosition(shared_ptr<ObjectBase> obj, unsigned int pos);

	/**
	* devuelve la posición entre sus hermanos
	*/
	/*  unsigned int GetPosition();
	bool ChangePosition(unsigned int pos);*/


	/**
	* Elimina un hijo del objeto.
	*/
	void RemoveChild (shared_ptr<ObjectBase> obj);
	void RemoveChild (unsigned int idx);

	/**
	* Obtiene un hijo del objeto.
	*/
	shared_ptr<ObjectBase> GetChild (unsigned int idx);

	/**
	* Obtiene el número de hijos del objeto.
	*/
	unsigned int  GetChildCount()    { return (unsigned int)m_children.size(); }

	/**
	* Comprueba si el tipo de objeto pasado es válido como hijo del objeto.
	* Esta rutina es importante, ya que define las restricciónes de ubicación.
	*/
	//bool ChildTypeOk (wxString type);
	bool ChildTypeOk (PObjectType type);

	bool IsContainer() { return (GetObjectTypeName() == wxT("container") ); }

	shared_ptr<ObjectBase> GetLayout();

	/**
	* Devuelve el tipo de objeto.
	*
	* Deberá ser redefinida en cada clase derivada.
	*/
	wxString GetObjectTypeName() { return m_type; }
	void SetObjectTypeName(wxString type) { m_type = type; }

	/**
	* Devuelve el descriptor del objeto.
	*/
	shared_ptr<ObjectInfo> GetObjectInfo() { return m_info; };
	void SetObjectInfo(shared_ptr<ObjectInfo> info) { m_info = info; };

	/**
	* Devuelve la profundidad  del objeto en el arbol.
	*/
	int Deep();

	/**
	* Imprime el arbol en un stream.
	*/
	//virtual void PrintOut(ostream &s, int indent);

	/**
	* Sobrecarga del operador inserción.
	*/
	friend ostream& operator << (ostream &s, shared_ptr<ObjectBase> obj);

	/////////////////////////
	// Implementación de la interfaz IObject para su uso dentro de los
	// plugins
	bool     IsNull (const wxString& pname);
	int      GetPropertyAsInteger (const wxString& pname);
	wxFont   GetPropertyAsFont    (const wxString& pname);
	wxColour GetPropertyAsColour  (const wxString& pname);
	wxString GetPropertyAsString  (const wxString& pname);
	wxPoint  GetPropertyAsPoint   (const wxString& pname);
	wxSize   GetPropertyAsSize    (const wxString& pname);
	wxBitmap GetPropertyAsBitmap  (const wxString& pname);
	double	 GetPropertyAsFloat	  (const wxString& pname);

	wxArrayInt    GetPropertyAsArrayInt (const wxString& pname);
	wxArrayString GetPropertyAsArrayString  (const wxString& pname);
	wxString GetChildFromParentProperty( const wxString& parentName, const wxString& childName );
};

///////////////////////////////////////////////////////////////////////////////

/**
* Clase que guarda un conjunto de plantillas de código.
*/
class CodeInfo
{
private:
	typedef map<wxString,wxString> TemplateMap;
	TemplateMap m_templates;
public:
	wxString GetTemplate(wxString name);
	void AddTemplate(wxString name, wxString _template);
};

///////////////////////////////////////////////////////////////////////////////

class ObjectPackage;

/**
* Información de objeto o MetaObjeto.
*/
class ObjectInfo
{
public:
	/**
	* Constructor.
	*/
	ObjectInfo(wxString class_name, PObjectType type, weak_ptr<ObjectPackage> package, bool startGroup = false );

	virtual ~ObjectInfo() {};

	shared_ptr< PropertyCategory > GetCategory(){ return m_category; }

	unsigned int GetPropertyCount() { return (unsigned int)m_properties.size(); }

	/**
	* Obtiene el descriptor de la propiedad.
	*/
	shared_ptr<PropertyInfo> GetPropertyInfo(wxString name);
	shared_ptr<PropertyInfo> GetPropertyInfo(unsigned int idx);

	/**
	* Añade un descriptor de propiedad al descriptor de objeto.
	*/
	void AddPropertyInfo(shared_ptr<PropertyInfo> pinfo);

	/**
	* Add a default value for an inherited property.
	* @param baseIndex Index of base class returned from AddBaseClass.
	* @param propertyName Property of base class to assign a default value to.
	* @param defaultValue Default value of the inherited property.
	*/
	void AddBaseClassDefaultPropertyValue( size_t baseIndex, const wxString& propertyName, const wxString& defaultValue );

	/**
	* Get a default value for an inherited property.
	* @param baseIndex Index of base class in the base class vector
	* @param propertName Name of the property to get the default value for
	* @return The default value for the property
	*/
	wxString GetBaseClassDefaultPropertyValue( size_t baseIndex, const wxString& propertyName );

	/**
	* Devuelve el tipo de objeto, será util para que el constructor de objetos
	* sepa la clase derivada de ObjectBase que ha de crear a partir del
	* descriptor.
	*/
	wxString GetObjectTypeName() { return m_type->GetName();   }

	PObjectType GetObjectType() { return m_type; }

	wxString GetClassName () { return m_class;  }

	/**
	* Imprime el descriptor en un stream.
	*/
	//virtual void PrintOut(ostream &s, int indent);


	/**
	* Sobrecarga del operador inserción.
	*/
	friend ostream& operator << (ostream &s, shared_ptr<ObjectInfo> obj);

	// nos serán utiles para generar el nombre del objeto
	unsigned int GetInstanceCount() { return m_numIns; }
	void IncrementInstanceCount()   { m_numIns++; }
	void ResetInstanceCount() { m_numIns = 0; }

	/**
	* Añade la información de un objeto al conjunto de clases base.
	*/
	size_t AddBaseClass(shared_ptr<ObjectInfo> base)
	{
		m_base.push_back(base);
		return m_base.size() - 1;
	}

	/**
	* Comprueba si el tipo es derivado del que se pasa como parámetro.
	*/
	bool IsSubclassOf(wxString classname);

	shared_ptr<ObjectInfo> GetBaseClass(unsigned int idx);
	unsigned int GetBaseClassCount();


	//
	void SetIconFile(wxBitmap icon) { m_icon = icon; };
	wxBitmap GetIconFile() { return m_icon; }

	void SetSmallIconFile(wxBitmap icon) { m_smallIcon = icon; };
	wxBitmap GetSmallIconFile() { return m_smallIcon; }

	void AddCodeInfo(wxString lang, shared_ptr<CodeInfo> codeinfo);
	shared_ptr<CodeInfo> GetCodeInfo(wxString lang);

	shared_ptr<ObjectPackage> GetPackage();

	bool IsStartOfGroup() { return m_startGroup; }

	/**
	* Le asigna un componente a la clase.
	*/
	void SetComponent(IComponent *c) { m_component = c; };
	IComponent* GetComponent() { return m_component; };

private:
	wxString m_class;         // nombre de la clase (tipo de objeto)

	PObjectType m_type;     // tipo del objeto
	weak_ptr<ObjectPackage> m_package; 	// Package that the object comes from

	shared_ptr< PropertyCategory > m_category;

	wxBitmap m_icon;
	wxBitmap m_smallIcon; // The icon for the property grid toolbar
	bool m_startGroup; // Place a separator in the palette toolbar just before this widget

	map< wxString, shared_ptr< CodeInfo > > m_codeTemp;  // plantillas de codigo K=language_name T=shared_ptr<CodeInfo>

	unsigned int m_numIns;  // número de instancias del objeto

	map< wxString, shared_ptr< PropertyInfo > > m_properties;
	vector< shared_ptr< ObjectInfo > > m_base; // base classes
	map< size_t, map< wxString, wxString > > m_baseClassDefaultPropertyValues;
	IComponent* m_component;  // componente asociado a la clase los objetos del
	// designer
};

#endif
