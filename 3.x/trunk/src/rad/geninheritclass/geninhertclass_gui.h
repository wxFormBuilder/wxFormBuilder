///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 17 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __geninhertclass_gui__
#define __geninhertclass_gui__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checklst.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class GenInheritedClassDlgBase
///////////////////////////////////////////////////////////////////////////////
class GenInheritedClassDlgBase : public wxDialog 
{
	private:
	
	protected:
		enum
		{
			ID_FORMS_CHECK_LIST = 1000,
			ID_CLASS_NAME_TEXT_CTRL,
			ID_FILE_NAME_TEXT_CTRL,
		};
		
		wxStaticText* m_instructionsStaticText;
		wxCheckListBox* m_formsCheckList;
		wxStaticText* m_classNameStaticText;
		wxTextCtrl* m_classNameTextCtrl;
		wxStaticText* m_fileNameStaticText;
		wxTextCtrl* m_fileNameTextCtrl;
		wxStdDialogButtonSizer* m_sdbSizer;
		wxButton* m_sdbSizerOK;
		wxButton* m_sdbSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFormsSelected( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnFormsToggle( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnClassNameChange( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnFileNameChange( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		GenInheritedClassDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Generate Inherited Class"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~GenInheritedClassDlgBase();
	
};

#endif //__geninhertclass_gui__
