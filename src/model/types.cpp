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

#include "types.h"

#include "../utils/stringutils.h"

#include <wx/tokenzr.h>

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

IntList::IntList(bool absolute_value, bool pair_value)
	: m_abs(absolute_value)
	, m_pairs(pair_value)
{
}

IntList::IntList(const wxString& value, bool absolute_value, bool pair_value)
	: IntList(absolute_value, pair_value)
{
	SetList(value);
}


void IntList::Add(int value)
{
	Add(value, 0);
}

void IntList::Add(int first, int second)
{
	if (m_abs)
	{
		m_ints.emplace_back(std::abs(first), (m_pairs ? std::abs(second) : 0));
	}
	else
	{
		m_ints.emplace_back(first, (m_pairs ? second : 0));
	}
}

void IntList::DeleteList()
{
	m_ints.clear();
}

void IntList::SetList(const wxString& str)
{
	DeleteList();

	wxStringTokenizer tkz(str, wxT(","));
	m_ints.reserve(tkz.CountTokens());
	while (tkz.HasMoreTokens())
	{
		wxString secondToken;
		wxString firstToken = tkz.GetNextToken().BeforeFirst(wxT(':'), &secondToken);
		firstToken.Trim(true);
		firstToken.Trim(false);
		secondToken.Trim(true);
		secondToken.Trim(false);

		long first;
		long second = 0;
		if (firstToken.ToLong(&first) && (!m_pairs || secondToken.empty() || secondToken.ToLong(&second)))
		{
			Add(static_cast<int>(first), static_cast<int>(second));
		}
	}
}


wxString IntList::ToString(bool skip_zero_second)
{
	wxString result;
	// Reserve some space to avoid many reallocations, assume one digit numbers
	result.reserve(m_pairs ? m_ints.size() * 3 : m_ints.size() * 2);

	for (const auto& entry : m_ints)
	{
		if (!result.empty()) {
			result.append(wxT(","));
		}

		result.append(StringUtils::IntToStr(entry.first));
		if (m_pairs && !(skip_zero_second && entry.second == 0))
		{
			result.append(wxT(":"));
			result.append(StringUtils::IntToStr(entry.second));
		}
	}

	return result;
}
