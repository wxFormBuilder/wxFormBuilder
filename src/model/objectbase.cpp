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

PropertyInfo::PropertyInfo(wxString name, PropertyType type, wxString def_value, wxString description, wxString customEditor,
						   POptionList opt_list, const std::list< PropertyChild >& children )
{
	m_name = name;
	m_type = type;
	m_def_value = def_value;
	m_opt_list = opt_list;
	m_description = description;
	m_children = children;
	m_customEditor = customEditor;
}

PropertyInfo::~PropertyInfo()
{
}

EventInfo::EventInfo(const wxString &name, const wxString &eventClass,
  const wxString &defValue,  const wxString &description)
  : m_name(name), m_eventClass(eventClass), m_defaultValue(defValue),
    m_description(description)
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

void Property::SetValue(const wxFontContainer &font)
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

wxFontContainer Property::GetValueAsFont()
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
	m_class = class_name;

	Debug::Print(wxT("new ObjectBase"));
}

ObjectBase::~ObjectBase()
{
	// remove the reference in the parent
	PObjectBase parent = m_parent.lock();

	if (parent)
	{
		PObjectBase pobj(GetThis());
		parent->RemoveChild(pobj);
	}

	Debug::Print(wxT("delete ObjectBase"));
}

wxString ObjectBase::GetIndentString(int indent)
{
	int i;
	wxString s;

	for (i=0;i<indent;i++)
		s += wxT(" ");

	return s;
}


PProperty ObjectBase::GetProperty (wxString name)
{
	PropertyMap::iterator it = m_properties.find( name );
	if ( it != m_properties.end() )
		return it->second;

  //Debug::Print(wxT("[ObjectBase::GetProperty] Property %s not found!"),name.c_str());
	// este aserto falla siempre que se crea un sizeritem
	// assert(false);
	return PProperty();
}

PProperty ObjectBase::GetProperty (unsigned int idx)
{
	assert (idx < m_properties.size());

	PropertyMap::iterator it = m_properties.begin();
	unsigned int i = 0;
	while (i < idx && it != m_properties.end())
	{
		i++;
		it++;
	}

	if (it != m_properties.end())
		return it->second;

	return PProperty();
}

PEvent ObjectBase::GetEvent (wxString name)
{
	EventMap::iterator it = m_events.find( name );
	if ( it != m_events.end() )
		return it->second;

#if wxVERSION_NUMBER < 2900
	Debug::Print(wxT("[ObjectBase::GetEvent] Event %s not found!"),name.c_str());
#else
    Debug::Print("[ObjectBase::GetEvent] Event " + name + " not found!");
#endif
	return PEvent();
}

PEvent ObjectBase::GetEvent (unsigned int idx)
{
	assert (idx < m_events.size());

	EventMap::iterator it = m_events.begin();
	unsigned int i = 0;
	while (i < idx && it != m_events.end())
	{
		i++;
		it++;
	}

	if (it != m_events.end())
		return it->second;

  return PEvent();
}

void ObjectBase::AddProperty (PProperty prop)
{
	m_properties.insert( PropertyMap::value_type( prop->GetName(), prop ) );
}

void ObjectBase::AddEvent(PEvent event)
{
	m_events.insert( EventMap::value_type( event->GetName(), event ) );
}

PObjectBase ObjectBase::FindNearAncestor(wxString type)
{
	PObjectBase result;
	PObjectBase parent = GetParent();
	if (parent)
	{
		if (parent->GetObjectTypeName() == type)
			result = parent;
		else
			result = parent->FindNearAncestor(type);
	}

	return result;
}

PObjectBase ObjectBase::FindNearAncestorByBaseClass(wxString type)
{
	PObjectBase result;
	PObjectBase parent = GetParent();
	if (parent)
	{
		if ( parent->GetObjectInfo()->IsSubclassOf( type ) )
			result = parent;
		else
			result = parent->FindNearAncestorByBaseClass( type );
	}

	return result;
}

PObjectBase ObjectBase::FindParentForm()
{
	PObjectBase retObj = this->FindNearAncestor( wxT( "form" ) );
	if( retObj == NULL ) retObj = this->FindNearAncestor( wxT( "menubar_form" ) );
	if( retObj == NULL ) retObj = this->FindNearAncestor( wxT( "toolbar_form" ) );
    if( retObj == NULL ) retObj = this->FindNearAncestor( wxT("wizard") );
	
	return retObj;
}

bool ObjectBase::AddChild (PObjectBase obj)
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

bool ObjectBase::AddChild (unsigned int idx, PObjectBase obj)
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

bool ObjectBase::ChildTypeOk (PObjectType type)
{
	// buscamos si puede haber objectos del tipo "type" como hijos
	// del actual objeto tipo.
	
	int nmax = 0;
		
	// check allowed child count
	if( GetObjectInfo()->GetObjectType()->GetName() == wxT("form") )
	{
		nmax = GetObjectInfo()->GetObjectType()->FindChildType(type, this->GetPropertyAsInteger(wxT("aui_managed")));
	}
	else
		nmax = GetObjectInfo()->GetObjectType()->FindChildType(type, false);

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

PObjectBase ObjectBase::GetLayout()
{
	PObjectBase result;

	if (GetParent() && GetParent()->GetObjectInfo()->IsSubclassOf( wxT("sizeritembase") ))
		result = GetParent();

	return result;
}

void ObjectBase::RemoveChild (PObjectBase obj)
{
	std::vector< PObjectBase >::iterator it = m_children.begin();
	while (it != m_children.end() && *it != obj)
		it++;

	if (it != m_children.end())
		m_children.erase(it);
}

void ObjectBase::RemoveChild (unsigned int idx)
{
	assert (idx < m_children.size());

	std::vector< PObjectBase >::iterator it =  m_children.begin() + idx;
	m_children.erase(it);
}

PObjectBase ObjectBase::GetChild (unsigned int idx)
{
	assert (idx < m_children.size());

	return m_children[idx];
}

int ObjectBase::Deep()
{
	int deep = 0;
	PObjectBase obj(GetParent());
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
//  map< wxString, PProperty >::const_iterator it_prop;
//  for (it_prop = m_properties.begin(); it_prop!= m_properties.end(); it_prop++)
//  {
//    s << ind_str << "property '" << it_prop->first << "' = '" <<
//      it_prop->second->GetValue() << "'" << endl;
//  }
//
//  vector< PObjectBase >::const_iterator it_ch;
//  for (it_ch = m_children.begin() ; it_ch != m_children.end(); it_ch++)
//  {
//    (*it_ch)->PrintOut(s,INDENT + indent);
//  }
//}
//
//ostream& operator << (ostream &s, PObjectBase obj)
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
		PProperty prop = GetProperty( i );
		ticpp::Element prop_element( "property" );
		prop_element.SetAttribute( "name", _STDSTR( prop->GetName() ) );
		prop_element.SetText( _STDSTR( prop->GetValue() ) );
		element.LinkEndChild( &prop_element );
	}

	for ( unsigned int i = 0; i < GetEventCount(); i++ )
	{
		PEvent event = GetEvent( i );
		ticpp::Element event_element( "event" );
		event_element.SetAttribute( "name", _STDSTR( event->GetName() ) );
		event_element.SetText( _STDSTR( event->GetValue() ) );
		element.LinkEndChild( &event_element );
	}

	for ( unsigned int i = 0 ; i < GetChildCount(); i++ )
	{
		PObjectBase child = GetChild( i );
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

unsigned int ObjectBase::GetChildPosition(PObjectBase obj)
{
	unsigned int pos = 0;
	while (pos < GetChildCount() && m_children[pos] != obj)
		pos++;

	return pos;
}

bool ObjectBase::ChangeChildPosition( PObjectBase obj, unsigned int pos)
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
	PProperty property = GetProperty( pname );
	if (property)
		return property->IsNull();
	else
		return true;
}

int ObjectBase::GetPropertyAsInteger (const wxString& pname)
{
	PProperty property = GetProperty( pname );
	if (property)
		return property->GetValueAsInteger();
	else
		return 0;
}

wxFontContainer ObjectBase::GetPropertyAsFont(const wxString& pname)
{
	PProperty property = GetProperty( pname );
	if (property)
		return property->GetValueAsFont();
	else
		return wxFontContainer();
}

wxColour ObjectBase::GetPropertyAsColour  (const wxString& pname)
{
	PProperty property = GetProperty( pname );
	if (property)
		return property->GetValueAsColour();
	else
		return wxColour();
}

wxString ObjectBase::GetPropertyAsString  (const wxString& pname)
{
	PProperty property = GetProperty( pname );
	if (property)
		return property->GetValueAsString();
	else
		return wxString();
}

wxPoint  ObjectBase::GetPropertyAsPoint   (const wxString& pname)
{
	PProperty property = GetProperty( pname );
	if (property)
		return property->GetValueAsPoint();
	else
		return wxPoint();
}

wxSize   ObjectBase::GetPropertyAsSize    (const wxString& pname)
{
	PProperty property = GetProperty( pname );
	if (property)
		return property->GetValueAsSize();
	else
		return wxDefaultSize;
}

wxBitmap ObjectBase::GetPropertyAsBitmap  (const wxString& pname)
{
	PProperty property = GetProperty( pname );
	if (property)
		return property->GetValueAsBitmap();
	else
		return wxBitmap();
}
double ObjectBase::GetPropertyAsFloat( const wxString& pname )
{
	PProperty property = GetProperty( pname );
	if (property)
		return property->GetValueAsFloat();
	else
		return 0;
}
wxArrayInt ObjectBase::GetPropertyAsArrayInt(const wxString& pname)
{
	wxArrayInt array;
	PProperty property = GetProperty( pname );
	if (property)
	{
		IntList il( property->GetValue(), property->GetType() == PT_UINTLIST );
		for (unsigned int i=0; i < il.GetSize() ; i++)
			array.Add(il.GetValue(i));
	}

	return array;
}

wxArrayString ObjectBase::GetPropertyAsArrayString(const wxString& pname)
{
	PProperty property = GetProperty( pname );
	if (property)
		return property->GetValueAsArrayString();
	else
		return wxArrayString();
}

wxString ObjectBase::GetChildFromParentProperty( const wxString& parentName, const wxString& childName )
{
	PProperty property = GetProperty( parentName );
	if (property)
		return property->GetChildFromParent( childName );
	else
		return wxEmptyString;
}

///////////////////////////////////////////////////////////////////////////////

ObjectInfo::ObjectInfo(wxString class_name, PObjectType type, WPObjectPackage package, bool startGroup )
{
	m_class = class_name;
	m_type = type;
	m_numIns = 0;
	m_component = NULL;
	m_package = package;
	m_category = PPropertyCategory( new PropertyCategory( m_class ) );
	m_startGroup = startGroup;
}

PObjectPackage ObjectInfo::GetPackage()
{
	return m_package.lock();
}

PPropertyInfo ObjectInfo::GetPropertyInfo(wxString name)
{
	PPropertyInfo result;

	std::map< wxString, PPropertyInfo >::iterator it = m_properties.find(name);
	if (it != m_properties.end())
		result = it->second;

	return result;
}

PPropertyInfo ObjectInfo::GetPropertyInfo(unsigned int idx)
{
	PPropertyInfo result;

	assert (idx < m_properties.size());

	std::map< wxString, PPropertyInfo >::iterator it = m_properties.begin();
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

PEventInfo ObjectInfo::GetEventInfo(wxString name)
{
	PEventInfo result;

	EventInfoMap::iterator it = m_events.find(name);
	if (it != m_events.end())
		result = it->second;

	return result;
}

PEventInfo ObjectInfo::GetEventInfo(unsigned int idx)
{
	PEventInfo result;

	assert (idx < m_events.size());

	EventInfoMap::iterator it = m_events.begin();
	unsigned int i = 0;
	while (i < idx && it != m_events.end())
	{
		i++;
		it++;
	}

	if (it != m_events.end())
		result = it->second;

	return result;
}

void ObjectInfo::AddPropertyInfo( PPropertyInfo prop )
{
	m_properties.insert( PropertyInfoMap::value_type(prop->GetName(), prop) );
}

void ObjectInfo::AddEventInfo(PEventInfo evtInfo)
{
  m_events.insert( EventInfoMap::value_type(evtInfo->GetName(), evtInfo) );
}

void ObjectInfo::AddBaseClassDefaultPropertyValue( size_t baseIndex, const wxString& propertyName, const wxString& defaultValue )
{
	std::map< size_t, std::map< wxString, wxString > >::iterator baseClassMap = m_baseClassDefaultPropertyValues.find( baseIndex );
	if ( baseClassMap != m_baseClassDefaultPropertyValues.end() )
	{
		baseClassMap->second.insert( std::map< wxString, wxString >::value_type( propertyName, defaultValue ) );
	}
	else
	{
		std::map< wxString, wxString > propertyDefaultValues;
		propertyDefaultValues[ propertyName ] = defaultValue;
		m_baseClassDefaultPropertyValues[ baseIndex ] = propertyDefaultValues;
	}
}

wxString ObjectInfo::GetBaseClassDefaultPropertyValue( size_t baseIndex, const wxString& propertyName )
{
	std::map< size_t, std::map< wxString, wxString > >::iterator baseClassMap = m_baseClassDefaultPropertyValues.find( baseIndex );
	if ( baseClassMap != m_baseClassDefaultPropertyValues.end() )
	{
		std::map< wxString, wxString >::iterator defaultValue = baseClassMap->second.find( propertyName );
		if ( defaultValue != baseClassMap->second.end() )
		{
			return defaultValue->second;
		}
	}
	return wxString();
}

PObjectInfo ObjectInfo::GetBaseClass(unsigned int idx, bool inherited)
{
	if( inherited )
	{
		std::vector<PObjectInfo> classes;
		GetBaseClasses( classes );
		
		assert (idx < classes.size());
		return classes[idx];
	}
	else
	{
		assert (idx < m_base.size());
		return m_base[idx];
	}
}

unsigned int ObjectInfo::GetBaseClassCount(bool inherited)
{
	if( inherited )
	{
		std::vector<PObjectInfo> classes;
		GetBaseClasses( classes );
		
		return (unsigned int)classes.size();
	}
	else
		return (unsigned int)m_base.size();
}

void ObjectInfo::GetBaseClasses(std::vector<PObjectInfo> &classes, bool inherited)
{
	for ( std::vector<PObjectInfo>::iterator it = m_base.begin(); it != m_base.end(); ++it )
	{
		PObjectInfo base_info = *it;;
		classes.push_back( base_info );

		if( inherited ) base_info->GetBaseClasses( classes );
	}
}

bool ObjectInfo::IsSubclassOf(wxString classname)
{
	bool found = false;

	if (GetClassName() == classname)
		found = true;
	else

		for (unsigned int i=0; !found && i < GetBaseClassCount() ; i++)
		{
			PObjectInfo base = GetBaseClass(i);
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
//  map< wxString, PPropertyInfo >::const_iterator it_prop;
//  for (it_prop = m_properties.begin(); it_prop!= m_properties.end(); it_prop++)
//  {
//    s << ind_str << "property '" << it_prop->first << "' type = '" <<
//      it_prop->second->GetType() << "' with value = '" <<
//      it_prop->second->GetDefaultValue() << "' by default" << endl;
//  }
//}

void ObjectInfo::AddCodeInfo(wxString lang, PCodeInfo codeinfo)
{
	std::map< wxString, PCodeInfo >::iterator templates = m_codeTemp.find( lang );
	if ( templates == m_codeTemp.end() )
	{
		// First code info is a clean copy
		m_codeTemp[ lang ] = PCodeInfo( new CodeInfo( *codeinfo ) );
	}
	else
	{
		// If code info already existed for the language, merge code info
		templates->second->Merge( codeinfo );
	}
}

PCodeInfo ObjectInfo::GetCodeInfo(wxString lang)
{
	PCodeInfo result;
	std::map< wxString, PCodeInfo >::iterator it = m_codeTemp.find(lang);
	if (it != m_codeTemp.end())
		result = it->second;

	return result;
}

//ostream& operator << (ostream &s, PObjectInfo obj)
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

void CodeInfo::Merge( PCodeInfo merger )
{
	TemplateMap::iterator mergerTemplate;
	for ( mergerTemplate = merger->m_templates.begin(); mergerTemplate != merger->m_templates.end(); ++mergerTemplate )
	{
		std::pair< TemplateMap::iterator, bool > mine = m_templates.insert( TemplateMap::value_type( mergerTemplate->first, mergerTemplate->second ) );
		if ( !mine.second )
		{
			mine.first->second += mergerTemplate->second;
		}
	}
}
