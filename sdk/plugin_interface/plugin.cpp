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

#include "plugin.h"


void ComponentLibrary::RegisterComponent(const wxString& name, IComponent* component)
{
    auto entry = m_components.emplace(name, component);
    if (!entry.second) {
        // TODO: Since we have to take ownership, just delete the object.
        //       Would be nice if we could report this situation somehow.
        delete component;
    }
}

void ComponentLibrary::RegisterMacro(const wxString& macro, int value)
{
    m_macros.emplace(macro, value);
}

void ComponentLibrary::RegisterSynonymous(const wxString& synonymous, const wxString& macro)
{
    m_synonymous.emplace(synonymous, macro);
}


wxString ComponentLibrary::ReplaceSynonymous(const wxString& synonymous, bool* replaced) const
{
    auto entry = m_synonymous.find(synonymous);
    const auto found = (entry != m_synonymous.end());

    if (replaced) {
        *replaced = found;
    }
    if (found) {
        return entry->second;
    }
    return synonymous;
}


std::vector<std::pair<wxString, IComponent*>> ComponentLibrary::GetComponents() const
{
    std::vector<std::pair<wxString, IComponent*>> result;
    result.reserve(m_components.size());

    for (const auto& entry : m_components) {
        result.emplace_back(entry.first, entry.second.get());
    }

    return result;
}

std::vector<std::pair<wxString, int>> ComponentLibrary::GetMacros() const
{
    std::vector<std::pair<wxString, int>> result;
    result.reserve(m_macros.size());

    result.insert(result.begin(), m_macros.begin(), m_macros.end());

    return result;
}
