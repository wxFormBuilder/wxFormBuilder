#ifndef WXFBMANAGER
#define WXFBMANAGER

#include <component.h>
#include <boost/smart_ptr.hpp>

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
	wxObject* wxFBManager::GetParent( wxObject* wxobject );
	IObject* GetIObject( wxObject* wxobject );
	wxObject* GetWxObject( boost::shared_ptr< ObjectBase > obj );

	void ModifyProperty( wxObject* wxobject, wxString property, wxString value, bool allowUndo = true );

	void SelectObject( wxObject* wxobject );
};

#endif //WXFBMANAGER
