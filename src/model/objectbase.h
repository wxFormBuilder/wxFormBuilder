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
#include <list>

#include "types.h"
#include "ticpp.h"

#include "wx/wx.h"
#include <component.h>

#include "utils/wxfbdefs.h"

///////////////////////////////////////////////////////////////////////////////

class OptionList
{
private:

	std::map< wxString, wxString > m_options;

public:

	void AddOption( wxString option, wxString description = wxString() )
	{
		m_options[option] = description;
	}
	unsigned int GetOptionCount()
	{
		return (unsigned int)m_options.size();
	}
	const std::map< wxString, wxString >& GetOptions()
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
	POptionList  m_opt_list;
	std::list< PropertyChild > m_children; // Only used for parent properties
	bool m_hidden; // Juan. Determina si la propiedad aparece o no en XRC
	wxString		m_description;
	wxString		m_customEditor; // an optional custom editor for the property grid

public:

	PropertyInfo(wxString name, PropertyType type, wxString def_value, wxString description, wxString customEditor,
		POptionList opt_list, const std::list< PropertyChild >& children );

	~PropertyInfo();

	wxString       GetDefaultValue()        { return m_def_value;  }
	PropertyType GetType()                { return m_type;       }
	wxString       GetName()                { return m_name;       }
	POptionList  GetOptionList ()         { return m_opt_list;   }
	std::list< PropertyChild >* GetChildren(){ return &m_children; }
	wxString		 GetDescription	()		  { return m_description;}
	wxString	GetCustomEditor()			{ return m_customEditor; }
};

class EventInfo
{
private:
	wxString m_name;
	wxString m_eventClass;
  wxString m_defaultValue;
  wxString m_description;

public:
  EventInfo(const wxString &name,
            const wxString &eventClass,
            const wxString &defValue,
            const wxString &description);

  wxString GetName()           { return m_name; }
  wxString GetEventClassName() { return m_eventClass; }
  wxString GetDefaultValue()   { return m_defaultValue; }
  wxString GetDescription()    { return m_description; }
};

///////////////////////////////////////////////////////////////////////////////

class Property
{
private:
	PPropertyInfo m_info;   // pointer to its descriptor
	WPObjectBase  m_object; // pointer to the owner object

	wxString m_value;

public:
	Property(PPropertyInfo info, PObjectBase obj = PObjectBase())
	{
		m_object = obj;
		m_info = info;
	}

	PObjectBase GetObject() { return m_object.lock(); }
	wxString GetName()                 { return m_info->GetName(); }
	wxString GetValue()                { return m_value; }
	void SetValue( wxString& val )     { m_value = val; }
	void SetValue( const wxChar* val )       { m_value = val;	}

	PPropertyInfo GetPropertyInfo() { return m_info; }
	PropertyType  GetType()         { return m_info->GetType();  }

	bool IsDefaultValue();
	bool IsNull();
	void SetDefaultValue();
	void ChangeDefaultValue( const wxString& value )
	{
		m_info->m_def_value = value;
	}

	////////////////////
	void SetValue(const wxFontContainer &font);
	void SetValue(const wxColour &colour);
	void SetValue(const wxString &str, bool format = false);
	void SetValue(const wxPoint &point);
	void SetValue(const wxSize &size);
	void SetValue(const int integer);
	void SetValue(const double val );

	wxFontContainer GetValueAsFont();
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

class Event
{
private:
  PEventInfo  m_info;   // pointer to its descriptor
  WPObjectBase m_object; // pointer to the owner object
  wxString    m_value;  // handler function name

public:
  Event (PEventInfo info, PObjectBase obj)
    : m_info(info), m_object(obj)
  {}

  void SetValue(const wxString &value) { m_value = value; }
  wxString GetValue()                  { return m_value; }
  wxString GetName()                   { return m_info->GetName(); }
  PObjectBase GetObject()              { return m_object.lock(); }
  PEventInfo GetEventInfo()            { return m_info; }
};

class PropertyCategory
{
private:
	wxString m_name;
	std::vector< wxString > m_properties;
	std::vector< wxString > m_events;
	std::vector< PPropertyCategory > m_categories;

public:

	PropertyCategory( wxString name ) : m_name( name ){}
	void AddProperty( wxString name ){ m_properties.push_back( name ); }
	void AddEvent( wxString name ){ m_events.push_back( name ); }
	void AddCategory( PPropertyCategory category ){ m_categories.push_back( category ); }
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

	wxString GetEventName( size_t index )
	{
		if ( index < m_events.size() )
		{
			return m_events[ index ];
		}
		else
		{
			return wxEmptyString;
		}
	}

	PPropertyCategory GetCategory( size_t index )
	{
		if ( index < m_categories.size() )
		{
			return m_categories[ index ];
		}
		else
		{
			return PPropertyCategory();
		}
	}

	size_t GetPropertyCount() { return m_properties.size(); }
	size_t GetEventCount() { return m_events.size(); }
	size_t GetCategoryCount() { return m_categories.size(); }
};

///////////////////////////////////////////////////////////////////////////////
namespace ticpp
{
	class Document;
	class Element;
}

class ObjectBase : public IObject, public boost::enable_shared_from_this<ObjectBase>
{
	friend class wxFBDataObject;
private:
	wxString     m_class;  // class name
	wxString     m_type;   // type of object
	WPObjectBase m_parent; // weak pointer, no reference loops please!

	ObjectBaseVector m_children;
	PropertyMap      m_properties;
	EventMap         m_events;
	PObjectInfo      m_info;
	bool m_expanded; // is expanded in the object tree, allows for saving to file

protected:
	// utilites for implementing the tree
	static const int INDENT;  // size of indent
	wxString GetIndentString(int indent); // obtiene la cadena con el indentado

	ObjectBaseVector& GetChildren()     { return m_children; };
	PropertyMap&      GetProperties()   { return m_properties; };

	// Crea un elemento del objeto
	void SerializeObject( ticpp::Element* serializedElement );

	// devuelve el puntero "this"
	PObjectBase GetThis() { return shared_from_this(); }

public:

	/// Constructor.
	ObjectBase (wxString class_name);

	/// Destructor.
	virtual ~ObjectBase();

	/**
	Sets whether the object is expanded in the object tree or not.
	*/
	void SetExpanded( bool expanded ){ m_expanded = expanded; }

	/**
	Gets whether the object is expanded in the object tree or not.
	*/
	bool GetExpanded(){ return m_expanded; }

	/**
	* Obtiene el nombre del objeto.
	*
	* @note No confundir con la propiedad nombre que tienen algunos objetos.
	*       Cada objeto tiene un nombre, el cual será el mismo que el usado
	*       como clave en el registro de descriptores.
	*/
	wxString GetClassName () { return m_class;  }

	/// Gets the parent object
	PObjectBase GetParent () { return m_parent.lock(); }

	/// Links the object to a parent
	void SetParent(PObjectBase parent)  { m_parent = parent; }

	/**
	* Obtiene la propiedad identificada por el nombre.
	*
	* @note Notar que no existe el método SetProperty, ya que la modificación
	*       se hace a través de la referencia.
	*/
	PProperty GetProperty (wxString name);

	PEvent GetEvent(wxString name);

	/**
	* Añade una propiedad al objeto.
	*
	* Este método será usado por el registro de descriptores para crear la
	* instancia del objeto.
	* Los objetos siempre se crearán a través del registro de descriptores.
	*/
	void AddProperty (PProperty value);

	void AddEvent(PEvent event);

	/**
	* Obtiene el número de propiedades del objeto.
	*/
	unsigned int GetPropertyCount() { return (unsigned int)m_properties.size(); }

	unsigned int GetEventCount()    { return m_events.size(); }

	/**
	* Obtiene una propiedad del objeto.
	* @todo esta función deberá lanzar una excepción en caso de no encontrarse
	*       dicha propiedad, así se simplifica el código al no tener que hacer
	*       tantas comprobaciones.
	* Por ejemplo, el código sin excepciones sería algo así:
	*
	* @code
	*
	* PProperty plabel = obj->GetProperty("label");
	* PProperty ppos = obj->GetProperty("pos");
	* PProperty psize = obj->GetProperty("size");
	* PProperty pstyle = obj->GetProperty("style");
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
	PProperty GetProperty (unsigned int idx); // throws ...;

	PEvent GetEvent (unsigned int idx); // throws ...;

	/**
	* Devuelve el primer antecesor cuyo tipo coincida con el que se pasa
	* como parámetro.
	*
	* Será útil para encontrar el widget padre.
	*/
	PObjectBase FindNearAncestor(wxString type);
	PObjectBase FindNearAncestorByBaseClass(wxString type);
	PObjectBase FindParentForm();

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
	virtual bool AddChild    (PObjectBase);
	virtual bool AddChild    (unsigned int idx, PObjectBase obj);

	/**
	* Devuelve la posicion del hijo o GetChildCount() en caso de no encontrarlo
	*/
	unsigned int GetChildPosition(PObjectBase obj);
	bool ChangeChildPosition(PObjectBase obj, unsigned int pos);

	/**
	* devuelve la posición entre sus hermanos
	*/
	/*  unsigned int GetPosition();
	bool ChangePosition(unsigned int pos);*/


	/**
	* Elimina un hijo del objeto.
	*/
	void RemoveChild (PObjectBase obj);
	void RemoveChild (unsigned int idx);
	void RemoveAllChildren(){ m_children.clear(); }

	/**
	* Obtiene un hijo del objeto.
	*/
	PObjectBase GetChild (unsigned int idx);
	
	PObjectBase GetChild (unsigned int idx, const wxString& type);

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

	PObjectBase GetLayout();

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
	PObjectInfo GetObjectInfo() { return m_info; };
	void SetObjectInfo(PObjectInfo info) { m_info = info; };

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
	friend std::ostream& operator << (std::ostream &s, PObjectBase obj);

	/////////////////////////
	// Implementación de la interfaz IObject para su uso dentro de los
	// plugins
	bool     IsNull (const wxString& pname);
	int      GetPropertyAsInteger (const wxString& pname);
	wxFontContainer   GetPropertyAsFont    (const wxString& pname);
	wxColour GetPropertyAsColour  (const wxString& pname);
	wxString GetPropertyAsString  (const wxString& pname);
	wxPoint  GetPropertyAsPoint   (const wxString& pname);
	wxSize   GetPropertyAsSize    (const wxString& pname);
	wxBitmap GetPropertyAsBitmap  (const wxString& pname);
	double	 GetPropertyAsFloat	  (const wxString& pname);

	wxArrayInt    GetPropertyAsArrayInt (const wxString& pname);
	wxArrayString GetPropertyAsArrayString  (const wxString& pname);
	wxString GetChildFromParentProperty( const wxString& parentName, const wxString& childName );
	
	IObject* GetChildPtr (unsigned int idx) { return GetChild(idx).get(); }
};

///////////////////////////////////////////////////////////////////////////////

/**
* Clase que guarda un conjunto de plantillas de código.
*/
class CodeInfo
{
private:
	typedef std::map<wxString,wxString> TemplateMap;
	TemplateMap m_templates;
public:
	wxString GetTemplate(wxString name);
	void AddTemplate(wxString name, wxString _template);
	void Merge( PCodeInfo merger );
};

///////////////////////////////////////////////////////////////////////////////

/**
* Información de objeto o MetaObjeto.
*/
class ObjectInfo
{
public:
	/**
	* Constructor.
	*/
	ObjectInfo(wxString class_name, PObjectType type, WPObjectPackage package, bool startGroup = false );

	virtual ~ObjectInfo() {};

	PPropertyCategory GetCategory(){ return m_category; }

	unsigned int GetPropertyCount() { return (unsigned int)m_properties.size(); }
	unsigned int GetEventCount()    { return (unsigned int)m_events.size();     }

	/**
	* Obtiene el descriptor de la propiedad.
	*/
	PPropertyInfo GetPropertyInfo(wxString name);
	PPropertyInfo GetPropertyInfo(unsigned int idx);

	PEventInfo GetEventInfo(wxString name);
	PEventInfo GetEventInfo(unsigned int idx);

	/**
	* Añade un descriptor de propiedad al descriptor de objeto.
	*/
	void AddPropertyInfo(PPropertyInfo pinfo);

	/**
	 * Adds a event descriptor.
	 */
	void AddEventInfo(PEventInfo evtInfo);

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
	friend std::ostream& operator << (std::ostream &s, PObjectInfo obj);

	// nos serán utiles para generar el nombre del objeto
	unsigned int GetInstanceCount() { return m_numIns; }
	void IncrementInstanceCount()   { m_numIns++; }
	void ResetInstanceCount() { m_numIns = 0; }

	/**
	* Añade la información de un objeto al conjunto de clases base.
	*/
	size_t AddBaseClass(PObjectInfo base)
	{
		m_base.push_back(base);
		return m_base.size() - 1;
	}

	/**
	* Comprueba si el tipo es derivado del que se pasa como parámetro.
	*/
	bool IsSubclassOf(wxString classname);

	PObjectInfo GetBaseClass(unsigned int idx, bool inherited = true);
	void GetBaseClasses(std::vector<PObjectInfo> &classes, bool inherited = true);
	unsigned int GetBaseClassCount(bool inherited = true);

	void SetIconFile(wxBitmap icon) { m_icon = icon; };
	wxBitmap GetIconFile() { return m_icon; }

	void SetSmallIconFile(wxBitmap icon) { m_smallIcon = icon; };
	wxBitmap GetSmallIconFile() { return m_smallIcon; }

	void AddCodeInfo(wxString lang, PCodeInfo codeinfo);
	PCodeInfo GetCodeInfo(wxString lang);

	PObjectPackage GetPackage();

	bool IsStartOfGroup() { return m_startGroup; }

	/**
	* Le asigna un componente a la clase.
	*/
	void SetComponent(IComponent *c) { m_component = c; };
	IComponent* GetComponent() { return m_component; };

private:
	wxString m_class;         // nombre de la clase (tipo de objeto)

	PObjectType m_type;     // tipo del objeto
	WPObjectPackage m_package; 	// Package that the object comes from

	PPropertyCategory m_category;

	wxBitmap m_icon;
	wxBitmap m_smallIcon; // The icon for the property grid toolbar
	bool m_startGroup; // Place a separator in the palette toolbar just before this widget

	std::map< wxString, PCodeInfo > m_codeTemp;  // plantillas de codigo K=language_name T=PCodeInfo

	unsigned int m_numIns;  // número de instancias del objeto

	std::map< wxString, PPropertyInfo > m_properties;
	std::map< wxString, PEventInfo >    m_events;

	std::vector< PObjectInfo > m_base; // base classes
	std::map< size_t, std::map< wxString, wxString > > m_baseClassDefaultPropertyValues;
	IComponent* m_component;  // componente asociado a la clase los objetos del
	// designer
};

#endif
