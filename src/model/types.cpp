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

#include "model/types.h"
#include <wx/tokenzr.h>
#include "utils/stringutils.h"
#include "utils/debug.h"
#include "utils/typeconv.h"

#include <cstdlib>

ObjectType::ObjectType(wxString name, int id, bool hidden, bool item)
{
	m_id = id;
	m_name = name;
	m_hidden = hidden;
	m_item = item;
}

void ObjectType::AddChildType(PObjectType type, int max, int aui_max)
{
	/*assert(max != 0);
	assert(aui_max != 0);*/
	m_childTypes.insert(ChildTypeMap::value_type(type,ChildCount(max, aui_max)));
}

int ObjectType::FindChildType(int type_id, bool aui)
{
	int max = 0;
	ChildTypeMap::iterator it;
	for (it = m_childTypes.begin(); it != m_childTypes.end() && max == 0; it++)
	{
		PObjectType type(it->first);
		if (type && type_id == type->GetId())
		{
			if( aui ) max = it->second.aui_max;
			else
				max = it->second.max;
		}
	}
	return max;
}

int ObjectType::FindChildType(PObjectType type, bool aui)
{
	int type_id = type->GetId();
	return FindChildType(type_id, aui);
}

unsigned int ObjectType::GetChildTypeCount()
{
	return (unsigned int)m_childTypes.size();
}

PObjectType ObjectType::GetChildType(unsigned int idx)
{
	PObjectType result;

	assert (idx < GetChildTypeCount());

	unsigned int i = 0;
	ChildTypeMap::iterator it = m_childTypes.begin();

	while (i < idx && it != m_childTypes.end())
	{
		i++;
		it++;
	}

	if (i == idx)
		result = PObjectType(it->first);


	return result;
}

///////////////////////////////////////////////////////////////////////////////

IntList::IntList(wxString value, bool absolute_value )
	:
	m_abs( absolute_value )
{
	SetList(value);
}

void IntList::Add(int value)
{
	m_ints.push_back( m_abs ? std::abs(value) : value );
}

void IntList::DeleteList()
{
	m_ints.erase(m_ints.begin(), m_ints.end());
}

void IntList::SetList(wxString str)
{
	DeleteList();
	wxStringTokenizer tkz(str, wxT(","));
	while (tkz.HasMoreTokens())
	{
		long value;
		wxString token;
		token = tkz.GetNextToken();
		token.Trim(true);
		token.Trim(false);

		if (token.ToLong(&value))
			Add((int)value);
	}
}

wxString IntList::ToString()
{
	wxString result;

	if (m_ints.size() > 0)
	{
		result = StringUtils::IntToStr(m_ints[0]);

		for (unsigned int i=1; i< m_ints.size() ; i++)
			result = result + wxT(",") + StringUtils::IntToStr(m_ints[i]);
	}

	return result;
}
