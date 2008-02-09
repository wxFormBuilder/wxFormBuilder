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
//   Ryan Mulder - rjmyst3@gmail.com
//
///////////////////////////////////////////////////////////////////////////////
#include "wxfbmanager.h"
#include "designer/visualeditor.h"
#include "model/objectbase.h"
#include "rad/appdata.h"

#define CHECK_NULL( THING, THING_NAME, RETURN ) 									\
	if ( !THING )																	\
	{																				\
		wxLogError( _("%s is NULL! <%s,%i>"), THING_NAME, __TFILE__, __LINE__ );	\
		return RETURN;																\
	}

#define CHECK_VISUAL_EDITOR( RETURN ) \
	CHECK_NULL( m_visualEdit, _("Visual Editor"), RETURN )

#define CHECK_WX_OBJECT( RETURN ) \
	CHECK_NULL( wxobject, _("wxObject"), RETURN )

#define CHECK_OBJECT_BASE( RETURN ) \
	CHECK_NULL( obj, _("ObjectBase"), RETURN )

// Classes to unset flags in VisualEditor during the destructor - this prevents
// forgetting to unset the flag
class FlagFlipper
{
private:
	VisualEditor* m_visualEditor;
	void (VisualEditor::*m_flagFunction)( bool );

public:
	FlagFlipper( VisualEditor* visualEdit, void (VisualEditor::*flagFunction)( bool ) )
	:
	m_visualEditor( visualEdit ),
	m_flagFunction( flagFunction )
	{
		( m_visualEditor->*m_flagFunction )( true );
	}

	~FlagFlipper()
	{
		( m_visualEditor->*m_flagFunction )( false );
	}
};

wxFBManager::wxFBManager()
:
m_visualEdit( NULL )
{
}

void wxFBManager::SetVisualEditor( VisualEditor* visualEdit )
{
	m_visualEdit = visualEdit;
}

IObject* wxFBManager::GetIObject( wxObject* wxobject )
{
	CHECK_VISUAL_EDITOR( NULL )

	CHECK_WX_OBJECT( NULL )

	PObjectBase obj = m_visualEdit->GetObjectBase( wxobject );

	CHECK_OBJECT_BASE( NULL )

	return obj.get();
}

size_t wxFBManager::GetChildCount( wxObject* wxobject )
{
	CHECK_VISUAL_EDITOR( 0 )

	CHECK_WX_OBJECT( 0 )

	PObjectBase obj = m_visualEdit->GetObjectBase( wxobject );

	CHECK_OBJECT_BASE( 0 )

	return obj->GetChildCount();
}

wxObject* wxFBManager::GetChild( wxObject* wxobject, size_t childIndex )
{
	CHECK_VISUAL_EDITOR( NULL )

	CHECK_WX_OBJECT( NULL )

	PObjectBase obj = m_visualEdit->GetObjectBase( wxobject );

	CHECK_OBJECT_BASE( NULL )

	if ( childIndex >= obj->GetChildCount() )
	{
		return NULL;
	}

	return m_visualEdit->GetWxObject( obj->GetChild( childIndex ) );
}

IObject* wxFBManager::GetIParent( wxObject* wxobject )
{
	CHECK_VISUAL_EDITOR( NULL )

	CHECK_WX_OBJECT( NULL )

	PObjectBase obj = m_visualEdit->GetObjectBase( wxobject );

	CHECK_OBJECT_BASE( NULL )

	return obj->GetParent().get();
}

wxObject* wxFBManager::GetParent( wxObject* wxobject )
{
	CHECK_VISUAL_EDITOR( NULL )

	CHECK_WX_OBJECT( NULL )

	PObjectBase obj = m_visualEdit->GetObjectBase( wxobject );

	CHECK_OBJECT_BASE( NULL )

	return m_visualEdit->GetWxObject( obj->GetParent() );
}

wxObject* wxFBManager::GetWxObject( PObjectBase obj )
{
	CHECK_OBJECT_BASE( NULL )

	return m_visualEdit->GetWxObject( obj );
}

void wxFBManager::ModifyProperty( wxObject* wxobject, wxString property, wxString value, bool allowUndo )
{
	CHECK_VISUAL_EDITOR()

	// Prevent modified event in visual editor - no need to redraw when the change is happening in the editor!
	FlagFlipper stopModifiedEvent( m_visualEdit, &VisualEditor::PreventOnModified );

	CHECK_WX_OBJECT()

	PObjectBase obj = m_visualEdit->GetObjectBase( wxobject );

	CHECK_OBJECT_BASE()

	PProperty prop = obj->GetProperty( property );

	if ( !prop )
	{
		wxLogError( _("%s has no property named %s"), obj->GetClassName().c_str(), property.c_str() );
		return;
	}

	if ( allowUndo )
	{
		AppData()->ModifyProperty( prop, value );
	}
	else
	{
		prop->SetValue( value );
	}
}

bool wxFBManager::SelectObject( wxObject* wxobject )
{
	CHECK_VISUAL_EDITOR( false )

	// Prevent loop of selection events
	FlagFlipper stopSelectedEvent( m_visualEdit, &VisualEditor::PreventOnSelected );

	CHECK_WX_OBJECT( false )

	PObjectBase obj = m_visualEdit->GetObjectBase( wxobject );

	CHECK_OBJECT_BASE( false )

	return AppData()->SelectObject( obj );
}

wxNoObject* wxFBManager::NewNoObject()
{
    return new wxNoObject;
}
