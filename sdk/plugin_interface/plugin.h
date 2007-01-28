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

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "component.h"
#include "wx/wx.h"
#include <vector>
#include <map>

// Implementación de la librería. Este módulo debe estar implementado en la
// librería.
// En lugar de enlazarlo como un objeto para hacer el plugin
// haremos una plantilla para que el preprocesador la implemente dentro del
// propio módulo.

class ComponentLibrary : public IComponentLibrary
{
 private:
  typedef struct
  {
    wxString name;
    IComponent *component;
  }AComponent;

  typedef struct
  {
    wxString name;
    int value;
  } AMacro;

  typedef struct
  {
    wxString name, syn;
  } ASynonymous;

  std::vector<AComponent>  m_components;
  std::vector<AMacro>      m_macros;
  typedef std::map<wxString,wxString> SynMap;
  SynMap m_synMap;

 public:
  virtual ~ComponentLibrary()
  {
	std::vector< AComponent >::reverse_iterator component;
	for ( component = m_components.rbegin(); component != m_components.rend(); ++component )
	{
		delete component->component;
	}
  }

  void RegisterComponent( const wxString& text, IComponent* c )
  {
    AComponent comp;
    comp.component = c;
    comp.name = text;

    m_components.push_back(comp);
  }

  void RegisterMacro(const wxString &text, const int value)
  {
    AMacro macro;
    macro.name = text;
    macro.value = value;

    m_macros.push_back(macro);
  }

  void RegisterMacroSynonymous(const wxString &syn, const wxString &name)
  {
    /*ASynonymous asyn;
    asyn.name = name;
    asyn.syn = syn;

    m_synonymous.push_back(asyn);*/
    m_synMap.insert(SynMap::value_type(syn, name));
  }

  IComponent* GetComponent(unsigned int idx)
  {
    if (idx < m_components.size())
      return m_components[idx].component;
    return NULL;
  }

  wxString GetComponentName(unsigned int idx)
  {
    if (idx < m_components.size())
      return m_components[idx].name;

    return wxString();
  }

  wxString GetMacroName(unsigned int idx)
  {
    if (idx < m_macros.size())
      return m_macros[idx].name;

    return wxString();
  }

  int GetMacroValue(unsigned int idx)
  {
    if (idx < m_macros.size())
      return m_macros[idx].value;

    return 0;
  }

  /*wxString GetMacroSynonymous(unsigned int idx)
  {
    if (idx < m_synonymous.size())
      return m_synonymous[idx].syn;

    return wxString();
  }

  wxString GetSynonymousName(unsigned int idx)
  {
    if (idx < m_synonymous.size())
      return m_synonymous[idx].name;

    return wxString();
  }*/

  bool FindSynonymous(const wxString& syn, wxString& trans)
  {
    bool found = false;
    SynMap::iterator it = m_synMap.find(syn);
    if (it != m_synMap.end())
    {
      found = true;
      trans = it->second;
    }

    return found;
  }

  unsigned int GetMacroCount()
  {
    return (unsigned int)m_macros.size();
  }

  unsigned int GetComponentCount()
  {
    return (unsigned int)m_components.size();
  }

  /*unsigned int GetSynonymousCount()
  {
    return m_synonymous.size();
  }*/
};

/**
 * Base class for components
 */
class ComponentBase : public IComponent
{
private:
	int m_type;
	IManager* m_manager;

public:
	ComponentBase()
	:
	m_type( 0 ),
	m_manager( NULL )
	{}

	void __SetComponentType( int type )
	{
		m_type = ( type >= 0 && type <= 2 ? type : COMPONENT_TYPE_ABSTRACT );
	}

	void __SetManager( IManager* manager )
	{
		m_manager = manager;
	}

	IManager* GetManager()
	{
		return m_manager;
	}

	wxObject* Create( IObject *obj, wxObject *parent )
	{
		return new wxNoObject; /* Even components which are not visible must be unique in the map */
	}

	void OnCreated( wxObject* wxobject, wxWindow* wxparent )
	{

	}

	void OnSelected( wxObject* wxobject )
	{

	}

	TiXmlElement* ExportToXrc(IObject *obj)
	{
		return NULL;
	}

	TiXmlElement* ImportFromXrc(TiXmlElement *xrcObj)
	{
		return NULL;
	}

	int GetComponentType()
	{
		return m_type;
	}
};

#endif // __PLUGIN_H__
