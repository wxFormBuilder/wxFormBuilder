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
#include "wx/wx.h"
#include "utils/debug.h"
#include "utils/typeconv.h"
#include "utils/stringutils.h"
#include "rad/appdata.h"
#include <ticpp.h>
#include <wx/tokenzr.h>

int ObjectBase::s_instances = 0;

/*wxString OptionList::GetOption(unsigned int idx)
{
assert (idx < m_options.size());

return m_options[idx];
}*/

PropertyInfo::PropertyInfo(wxString name, PropertyType type, wxString def_value, wxString description,
						   bool hidden, shared_ptr<OptionList> opt_list, const std::list< PropertyChild >& children )
{
	m_name = name;
	m_type = type;
	m_def_value = def_value;
	m_hidden = hidden;    //Juan
	m_opt_list = opt_list;
	m_description = description;
	m_children = children;
}

PropertyInfo::~PropertyInfo()
{
}

bool Property::IsDefaultValue()
{
	return (m_info->GetDefaultValue() == m_value);
}

bool Property::IsNull()
{
	switch ( m_info->GetType() )
	{
		case PT_BITMAP:
		{
			wxString path;
			size_t semicolonIndex = m_value.find_first_of( wxT(";") );
			if ( semicolonIndex != m_value.npos )
			{
				path = m_value.substr( 0, semicolonIndex );
			}
			else
			{
				path = m_value;
			}

			return path.empty();
		}
		case PT_WXSIZE:
		{
			return ( wxDefaultSize == TypeConv::StringToSize( m_value ) );
		}
		default:
		{
			return m_value.empty();
		}
	}
}

void Property::SetDefaultValue()
{
	m_value = m_info->GetDefaultValue();
}

void Property::SetValue(const wxFont &font)
{
	m_value = TypeConv::FontToString( font );
}
void Property::SetValue(const wxColour &colour)
{
	m_value = TypeConv::ColourToString( colour );
}
void Property::SetValue(const wxString &str, bool format)
{
	m_value = (format ? TypeConv::TextToString(str) : str );
}

void Property::SetValue(const wxPoint &point)
{
	m_value = TypeConv::PointToString(point);
}

void Property::SetValue(const wxSize &size)
{
	m_value = TypeConv::SizeToString(size);
}

void Property::SetValue(const int integer)
{
	m_value = StringUtils::IntToStr(integer);
}

void Property::SetValue(const double val )
{
	m_value = TypeConv::FloatToString( val );
}

wxFont Property::GetValueAsFont()
{
	return TypeConv::StringToFont( m_value );
}

wxColour Property::GetValueAsColour()
{
	return TypeConv::StringToColour(m_value);
}
wxPoint Property::GetValueAsPoint()
{
	return TypeConv::StringToPoint(m_value);
}
wxSize Property::GetValueAsSize()
{
	return TypeConv::StringToSize(m_value);
}

wxBitmap Property::GetValueAsBitmap()
{
	return TypeConv::StringToBitmap( m_value );
}

int Property::GetValueAsInteger()
{
	int result = 0;

	switch (GetType())
	{
	case PT_OPTION:
	case PT_MACRO:
		result = TypeConv::GetMacroValue(m_value);
		break;
	case PT_BITLIST:
		result = TypeConv::BitlistToInt(m_value);
		break;
	default:
		result = TypeConv::StringToInt(m_value);
		break;
	}
	return result;
}

wxString Property::GetValueAsString()
{
	return m_value;
}

wxString Property::GetValueAsText()
{
	return TypeConv::StringToText(m_value);
}


wxArrayString Property::GetValueAsArrayString()
{
	return TypeConv::StringToArrayString(m_value);
}

double Property::GetValueAsFloat()
{
	return TypeConv::StringToFloat(m_value);
}

void Property::SplitParentProperty( std::map< wxString, wxString >* children )
{
	children->clear();
	if ( m_info->GetType() != PT_PARENT )
	{
		return;
	}

	std::list< PropertyChild >* myChildren = m_info->GetChildren();
	std::list< PropertyChild >::iterator it = myChildren->begin();

	wxStringTokenizer tkz( m_value, wxT(";"), wxTOKEN_RET_EMPTY_ALL );
	while ( tkz.HasMoreTokens() )
	{
		if ( myChildren->end() == it )
		{
			return;
		}
		wxString child = tkz.GetNextToken();
		child.Trim( false );
		child.Trim( true );
		children->insert( std::map< wxString, wxString >::value_type( it->m_name, child ) );
		it++;
	}
}

wxString Property::GetChildFromParent( const wxString& childName )
{
	std::map< wxString, wxString > children;
	SplitParentProperty( &children );

	std::map< wxString, wxString >::iterator child;
	child = children.find( childName );

	if ( children.end() == child )
	{
		return wxEmptyString;
	}
	else
	{
		return child->second;
	}
}

///////////////////////////////////////////////////////////////////////////////
const int ObjectBase::INDENT = 2;

ObjectBase::ObjectBase (wxString class_name)
:
m_expanded( true )
{
	s_instances++;

	m_class = class_name;

	Debug::Print(wxT("new ObjectBase (%d)"),s_instances);
}

ObjectBase::~ObjectBase()
{
	s_instances--;

	// eliminamos la referencia en el padre
	shared_ptr<ObjectBase> parent = m_parent.lock();

	if (parent)
	{
		shared_ptr<ObjectBase> pobj(GetThis());
		parent->RemoveChild(pobj);
	}

	Debug::Print(wxT("delete ObjectBase (%d)"),s_instances);
}

wxString ObjectBase::GetIndentString(int indent)
{
	int i;
	wxString s;

	for (i=0;i<indent;i++)
		s += wxT(" ");

	return s;
}


shared_ptr<Property> ObjectBase::GetProperty (wxString name)
{
	map< wxString, shared_ptr< Property > >::iterator it = m_properties.find( name );
	if ( it != m_properties.end() )
	{
		return it->second;
	}
	else
	{
		Debug::Print(wxT("[ObjectBase::GetProperty] Property %s not found!"),name.c_str());
		// este aserto falla siempre que se crea un sizeritem
		// assert(false);
		return shared_ptr<Property>((Property*)NULL);
	}
}

shared_ptr<Property> ObjectBase::GetProperty (unsigned int idx)
{
	assert (idx < m_properties.size());

	map< wxString, shared_ptr< Property > >::iterator it = m_properties.begin();
	unsigned int i = 0;
	while (i < idx && it != m_properties.end())
	{
		i++;
		it++;
	}

	if (it != m_properties.end())
	{
		return it->second;
	}
	else
	{
		return  shared_ptr<Property>((Property*)NULL);
	}
}

void ObjectBase::AddProperty (wxString propname, shared_ptr<Property> value)
{
	m_properties.insert( map< wxString, shared_ptr< Property > >::value_type( propname, value ) );
}

shared_ptr<ObjectBase> ObjectBase::FindNearAncestor(wxString type)
{
	shared_ptr<ObjectBase> result;
	shared_ptr<ObjectBase> parent = GetParent();
	if (parent)
	{
		if (parent->GetObjectTypeName() == type)
			result = parent;
		else
			result = parent->FindNearAncestor(type);
	}

	return result;
}

bool ObjectBase::AddChild (shared_ptr<ObjectBase> obj)
{
	bool result = false;
	if (ChildTypeOk(obj->GetObjectInfo()->GetObjectType()))
		//if (ChildTypeOk(obj->GetObjectTypeName()))
	{
		m_children.push_back(obj);
		result = true;
	}

	return result;
}

bool ObjectBase::AddChild (unsigned int idx, shared_ptr<ObjectBase> obj)
{
	bool result = false;
	if (ChildTypeOk(obj->GetObjectInfo()->GetObjectType()) && idx <= m_children.size())
		//if (ChildTypeOk(obj->GetObjectTypeName()) && idx <= m_children.size())
	{
		m_children.insert(m_children.begin() + idx,obj);
		result = true;
	}

	return result;
}
/*
bool ObjectBase::DoChildTypeOk(wxString type_child, wxString type_parent)
{
bool result;

if (type_parent == "project")
result = ( type_child == "form");

else if (type_parent == "container" || type_parent == "form")
result = (type_child == "sizer" || type_child == "menubar" || type_child == "statusbar" || type_child == "toolbar"); // sólo puede haber uno

else if (type_parent == "notebook")
result = (type_child == "notebookpage");

else if (type_parent == "notebookpage")
result = (type_child == "container"); // sólo puede haber uno

else if (type_parent == "sizer")
result = (type_child == "sizeritem" || type_child == "spacer");

else if (type_parent == "sizeritem")
result = (type_child == "widget" || type_child == "container" ||
type_child == "sizer" || type_child == "notebook" ||
type_child == "statusbar");

else if (type_parent == "menubar")
result = (type_child == "menu");

else if (type_parent == "menu" || type_parent == "submenu")
result = (type_child == "menuitem" || type_child == "submenu");

else if (type_parent == "toolbar")
result = (type_child == "tool" || type_child == "widget");

else
result = false;

return result;
}

bool ObjectBase::ChildTypeOk (wxString type)
{
return DoChildTypeOk(type, GetObjectTypeName());
}*/

bool ObjectBase::ChildTypeOk (PObjectType type)
{
	// buscamos si puede haber objectos del tipo "type" como hijos
	// del actual objeto tipo.
	int nmax = GetObjectInfo()->GetObjectType()->FindChildType(type);

	if (nmax == 0)
		return false;

	if (nmax < 0)
		return true;

	// llegados aquí hay que comprobar el número de hijos del tipo pasado
	int count = 0;
	for (unsigned int i=0; i < GetChildCount() && count <= nmax; i++)
	{
		if (GetChild(i)->GetObjectInfo()->GetObjectType() == type)
			count++;
	}

	if (count > nmax)
		return false;

	return true;

}

shared_ptr<ObjectBase> ObjectBase::GetLayout()
{
	shared_ptr<ObjectBase> result;

	if (GetParent() && GetParent()->GetObjectTypeName()==wxT("sizeritem"))
		result = GetParent();

	return result;
}

void ObjectBase::RemoveChild (shared_ptr<ObjectBase> obj)
{
	vector< shared_ptr< ObjectBase > >::iterator it = m_children.begin();
	while (it != m_children.end() && *it != obj)
		it++;

	if (it != m_children.end())
		m_children.erase(it);
}

void ObjectBase::RemoveChild (unsigned int idx)
{
	assert (idx < m_children.size());

	vector< shared_ptr< ObjectBase > >::iterator it =  m_children.begin() + idx;
	m_children.erase(it);
}

shared_ptr<ObjectBase> ObjectBase::GetChild (unsigned int idx)
{
	assert (idx < m_children.size());

	return m_children[idx];
}

int ObjectBase::Deep()
{
	int deep = 0;
	shared_ptr<ObjectBase> obj(GetParent());
	while (obj)
	{
		obj = obj->GetParent();
		deep++;

		if (deep > 1000)
			assert(false);
	}
	return deep;
}

//void ObjectBase::PrintOut(ostream &s, int indent)
//{
//  wxString ind_str = GetIndentString(indent);
//
//  s << ind_str << "[ " << GetClassName() << " ] " << GetObjectType() << endl;
//  map< wxString, shared_ptr< Property > >::const_iterator it_prop;
//  for (it_prop = m_properties.begin(); it_prop!= m_properties.end(); it_prop++)
//  {
//    s << ind_str << "property '" << it_prop->first << "' = '" <<
//      it_prop->second->GetValue() << "'" << endl;
//  }
//
//  vector< shared_ptr< ObjectBase > >::const_iterator it_ch;
//  for (it_ch = m_children.begin() ; it_ch != m_children.end(); it_ch++)
//  {
//    (*it_ch)->PrintOut(s,INDENT + indent);
//  }
//}
//
//ostream& operator << (ostream &s, shared_ptr<ObjectBase> obj)
//{
//  obj->PrintOut(s,0);
//  return s;
//}

void ObjectBase::SerializeObject( ticpp::Element* serializedElement )
{
	ticpp::Element element( "object" );
	element.SetAttribute( "class", _STDSTR( GetClassName() ) );
	element.SetAttribute( "expanded", GetExpanded() );

	for ( unsigned int i = 0; i < GetPropertyCount(); i++ )
	{
		shared_ptr< Property > prop = GetProperty( i );
		ticpp::Element prop_element( "property" );
		prop_element.SetAttribute( "name", _STDSTR( prop->GetName() ) );
		prop_element.SetText( _STDSTR( prop->GetValue() ) );
		element.LinkEndChild( &prop_element );
	}

	for ( unsigned int i = 0 ; i < GetChildCount(); i++ )
	{
		shared_ptr< ObjectBase > child = GetChild( i );
		ticpp::Element child_element;
		child->SerializeObject( &child_element );
		element.LinkEndChild( &child_element );
	}

	*serializedElement = element;
}

void ObjectBase::Serialize( ticpp::Document* serializedDocument )
{
	ticpp::Document document( "document" );

  #if wxUSE_UNICODE
	ticpp::Declaration dec( "1.0", "UTF-8", "yes" );
	#else
	ticpp::Declaration dec( "1.0", "ISO-8859-13", "yes" );
	#endif
	document.LinkEndChild( &dec );

	ticpp::Element root( "wxFormBuilder_Project" );

	ticpp::Element fileVersion( "FileVersion" );
	fileVersion.SetAttribute( "major", AppData()->m_fbpVerMajor );
	fileVersion.SetAttribute( "minor", AppData()->m_fbpVerMinor );

	root.LinkEndChild( &fileVersion );

	ticpp::Element element;
	SerializeObject( &element );

	root.LinkEndChild( &element );
	document.LinkEndChild( &root );

	*serializedDocument = document;
}

unsigned int ObjectBase::GetChildPosition(shared_ptr<ObjectBase> obj)
{
	unsigned int pos = 0;
	while (pos < GetChildCount() && m_children[pos] != obj)
		pos++;

	return pos;
}

bool ObjectBase::ChangeChildPosition(shared_ptr<ObjectBase> obj, unsigned int pos)
{
	unsigned int obj_pos = GetChildPosition(obj);

	if (obj_pos == GetChildCount() || pos >= GetChildCount())
		return false;

	if (pos == obj_pos)
		return true;

	// Procesamos el cambio de posición
	RemoveChild(obj);
	AddChild(pos,obj);
	return true;
}

///////////////////////////////////////////////////////////////////////////////

bool ObjectBase::IsNull (const wxString& pname)
{
	shared_ptr<Property> property = GetProperty( pname );
	if (property)
		return property->IsNull();
	else
		return true;
}

int ObjectBase::GetPropertyAsInteger (const wxString& pname)
{
	shared_ptr<Property> property = GetProperty( pname );
	if (property)
		return property->GetValueAsInteger();
	else
		return 0;
}

wxFont   ObjectBase::GetPropertyAsFont    (const wxString& pname)
{
	shared_ptr<Property> property = GetProperty( pname );
	if (property)
		return property->GetValueAsFont();
	else
		return wxFont();
}

wxColour ObjectBase::GetPropertyAsColour  (const wxString& pname)
{
	shared_ptr<Property> property = GetProperty( pname );
	if (property)
		return property->GetValueAsColour();
	else
		return wxColour();
}

wxString ObjectBase::GetPropertyAsString  (const wxString& pname)
{
	shared_ptr<Property> property = GetProperty( pname );
	if (property)
		return property->GetValueAsString();
	else
		return wxString();
}

wxPoint  ObjectBase::GetPropertyAsPoint   (const wxString& pname)
{
	shared_ptr<Property> property = GetProperty( pname );
	if (property)
		return property->GetValueAsPoint();
	else
		return wxPoint();
}

wxSize   ObjectBase::GetPropertyAsSize    (const wxString& pname)
{
	shared_ptr<Property> property = GetProperty( pname );
	if (property)
		return property->GetValueAsSize();
	else
		return wxSize();
}

wxBitmap ObjectBase::GetPropertyAsBitmap  (const wxString& pname)
{
	shared_ptr<Property> property = GetProperty( pname );
	if (property)
		return property->GetValueAsBitmap();
	else
		return wxBitmap();
}
double ObjectBase::GetPropertyAsFloat( const wxString& pname )
{
	shared_ptr<Property> property = GetProperty( pname );
	if (property)
		return property->GetValueAsFloat();
	else
		return 0;
}
wxArrayInt ObjectBase::GetPropertyAsArrayInt(const wxString& pname)
{
	wxArrayInt array;
	shared_ptr<Property> property = GetProperty( pname );
	if (property)
	{
		IntList il;
		il.SetList(property->GetValue());
		for (unsigned int i=0; i < il.GetSize() ; i++)
			array.Add(il.GetValue(i));
	}

	return array;
}

wxArrayString ObjectBase::GetPropertyAsArrayString(const wxString& pname)
{
	shared_ptr<Property> property = GetProperty( pname );
	if (property)
		return property->GetValueAsArrayString();
	else
		return wxArrayString();
}

wxString ObjectBase::GetChildFromParentProperty( const wxString& parentName, const wxString& childName )
{
	shared_ptr<Property> property = GetProperty( parentName );
	if (property)
		return property->GetChildFromParent( childName );
	else
		return wxEmptyString;
}

///////////////////////////////////////////////////////////////////////////////

ObjectInfo::ObjectInfo(wxString class_name, PObjectType type, weak_ptr<ObjectPackage> package, bool startGroup )
{
	m_class = class_name;
	m_type = type;
	m_numIns = 0;
	m_component = NULL;
	m_package = package;
	m_category = shared_ptr< PropertyCategory >( new PropertyCategory( m_class ) );
	m_startGroup = startGroup;
}

shared_ptr<ObjectPackage> ObjectInfo::GetPackage()
{
	return m_package.lock();
}

shared_ptr<PropertyInfo> ObjectInfo::GetPropertyInfo(wxString name)
{
	shared_ptr<PropertyInfo> result;

	map< wxString, shared_ptr< PropertyInfo > >::iterator it = m_properties.find(name);
	if (it != m_properties.end())
		result = it->second;

	return result;
}

shared_ptr<PropertyInfo> ObjectInfo::GetPropertyInfo(unsigned int idx)
{
	shared_ptr<PropertyInfo> result;

	assert (idx < m_properties.size());

	map< wxString, shared_ptr< PropertyInfo > >::iterator it = m_properties.begin();
	unsigned int i = 0;
	while (i < idx && it != m_properties.end())
	{
		i++;
		it++;
	}

	if (it != m_properties.end())
		result = it->second;

	return result;
}
void ObjectInfo::AddPropertyInfo(shared_ptr<PropertyInfo> prop)
{
	//m_properties[ prop->GetName() ] = prop;
	m_properties.insert( map< wxString, shared_ptr< PropertyInfo > >::value_type(prop->GetName(), prop) );
}

void ObjectInfo::AddBaseClassDefaultPropertyValue( size_t baseIndex, const wxString& propertyName, const wxString& defaultValue )
{
	map< size_t, map< wxString, wxString > >::iterator baseClassMap = m_baseClassDefaultPropertyValues.find( baseIndex );
	if ( baseClassMap != m_baseClassDefaultPropertyValues.end() )
	{
		baseClassMap->second.insert( map< wxString, wxString >::value_type( propertyName, defaultValue ) );
	}
	else
	{
		map< wxString, wxString > propertyDefaultValues;
		propertyDefaultValues[ propertyName ] = defaultValue;
		m_baseClassDefaultPropertyValues[ baseIndex ] = propertyDefaultValues;
	}
}

wxString ObjectInfo::GetBaseClassDefaultPropertyValue( size_t baseIndex, const wxString& propertyName )
{
	map< size_t, map< wxString, wxString > >::iterator baseClassMap = m_baseClassDefaultPropertyValues.find( baseIndex );
	if ( baseClassMap != m_baseClassDefaultPropertyValues.end() )
	{
		map< wxString, wxString >::iterator defaultValue = baseClassMap->second.find( propertyName );
		if ( defaultValue != baseClassMap->second.end() )
		{
			return defaultValue->second;
		}
	}
	return wxString();
}

shared_ptr<ObjectInfo> ObjectInfo::GetBaseClass(unsigned int idx)
{
	assert (idx < m_base.size());
	return m_base[idx];
}

unsigned int ObjectInfo::GetBaseClassCount()
{
	return (unsigned int)m_base.size();
}

bool ObjectInfo::IsSubclassOf(wxString classname)
{
	bool found = false;

	if (GetClassName() == classname)
		found = true;
	else

		for (unsigned int i=0; !found && i < GetBaseClassCount() ; i++)
		{
			shared_ptr<ObjectInfo> base = GetBaseClass(i);
			found = base->IsSubclassOf(classname);
		}

		return found;
}

//
//void ObjectInfo::PrintOut(ostream &s, int indent)
//{
//  wxString ind_str = "";
//  for (int i=0;i<indent;i++)
//    ind_str = ind_str + " ";
//
//  s << ind_str << "[ " << GetClassName() << " ] " << GetObjectType() << endl;
//  map< wxString, shared_ptr< PropertyInfo > >::const_iterator it_prop;
//  for (it_prop = m_properties.begin(); it_prop!= m_properties.end(); it_prop++)
//  {
//    s << ind_str << "property '" << it_prop->first << "' type = '" <<
//      it_prop->second->GetType() << "' with value = '" <<
//      it_prop->second->GetDefaultValue() << "' by default" << endl;
//  }
//}

void ObjectInfo::AddCodeInfo(wxString lang, shared_ptr<CodeInfo> codeinfo)
{
	m_codeTemp.insert(map< wxString, shared_ptr< CodeInfo > >::value_type(lang, codeinfo));
}

shared_ptr<CodeInfo> ObjectInfo::GetCodeInfo(wxString lang)
{
	shared_ptr<CodeInfo> result;
	map< wxString, shared_ptr< CodeInfo > >::iterator it = m_codeTemp.find(lang);
	if (it != m_codeTemp.end())
		result = it->second;

	return result;
}

//ostream& operator << (ostream &s, shared_ptr<ObjectInfo> obj)
//{
//  obj->PrintOut(s,0);
//  return s;
//}

///////////////////////////////////////////////////////////////////////////////
wxString CodeInfo::GetTemplate(wxString name)
{
	wxString result;

	TemplateMap::iterator it = m_templates.find(name);
	if (it != m_templates.end())
		result = it->second;

	return result;
}

void CodeInfo::AddTemplate(wxString name, wxString _template)
{
	m_templates.insert(TemplateMap::value_type(name,_template));
}


