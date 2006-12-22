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

	void ModifyProperty( wxObject* wxobject, wxString property, wxString value, bool allowUndo = true );

	void SelectObject( wxObject* wxobject );
};

#endif //WXFBMANAGER
