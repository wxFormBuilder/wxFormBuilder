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
//   Ryan Mulder - rjmyst3@gmail.com
//
///////////////////////////////////////////////////////////////////////////////
#ifndef WXFBMANAGER
#define WXFBMANAGER

#include <component.h>
#include "utils/wxfbdefs.h"

class VisualEditor;
class ObjectBase;

class wxFBManager : public IManager
{
private:
	VisualEditor* m_visualEdit;

public:
	wxFBManager();
	void SetVisualEditor( VisualEditor* visualEdit );
	size_t GetChildCount( wxObject* wxobject );
	wxObject* GetChild( wxObject* wxobject, size_t childIndex );
	wxObject* GetParent( wxObject* wxobject );
	IObject* GetIParent( wxObject* wxobject );
	IObject* GetIObject( wxObject* wxobject );
	wxObject* GetWxObject( PObjectBase obj );
	wxNoObject* NewNoObject();

	void ModifyProperty( wxObject* wxobject, wxString property, wxString value, bool allowUndo = true );

	// Returns true if selection changed, false if already selected
	bool SelectObject( wxObject* wxobject );
};

#endif //WXFBMANAGER
