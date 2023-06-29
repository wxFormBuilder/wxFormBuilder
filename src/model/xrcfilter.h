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

#ifndef MODEL_XRCFILTER_H
#define MODEL_XRCFILTER_H

#include <tinyxml2.h>

#include "model/database.h"


/**
 * Filtro de importación de un fichero XRC.
 */
class XrcLoader
{
public:
    XrcLoader() = default;
    explicit XrcLoader(PObjectDatabase db) : m_objDb(db) {}

    void SetObjectDatabase(PObjectDatabase db) { m_objDb = db; }

    /**
     * @brief Load the given XRC document as Project
     *
     * @param xrc XRC document
     * @return Project
     *
     * @throw wxFBException
     */
    PObjectBase GetProject(const tinyxml2::XMLDocument* xrc);

private:
    /**
     * @brief Load the given XRC element as Object
     *
     * @param object XRC object element
     * @param parent Parent object
     * @return Object
     *
     * @throw wxFBException
     */
    PObjectBase GetObject(const tinyxml2::XMLElement* object, PObjectBase parent);

private:
    PObjectDatabase m_objDb;
    tinyxml2::XMLDocument m_xfb{false, tinyxml2::PRESERVE_WHITESPACE};
};

#endif  // MODEL_XRCFILTER_H
