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

void wxFBManager::SelectObject( wxObject* wxobject )
{
	CHECK_VISUAL_EDITOR()

	// Prevent loop of selection events
	FlagFlipper stopSelectedEvent( m_visualEdit, &VisualEditor::PreventOnSelected );

	CHECK_WX_OBJECT()

	PObjectBase obj = m_visualEdit->GetObjectBase( wxobject );

	CHECK_OBJECT_BASE()

	AppData()->SelectObject( obj );
}
