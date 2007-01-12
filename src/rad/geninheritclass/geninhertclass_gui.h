///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jan 12 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __geninhertclass_gui__
#define __geninhertclass_gui__

// Define WX_GCH in order to support precompiled headers with GCC compiler.
// You have to create the header "wx_pch.h" and include all files needed
// for compile your gui inside it.
// Then, compile it and place the file "wx_pch.h.gch" into the same
// directory that "wx_pch.h".
#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#include <wx/checklst.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class GenInheritedClassDlgBase
///////////////////////////////////////////////////////////////////////////////
class GenInheritedClassDlgBase : public wxDialog 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_OnFormsToggle( wxCommandEvent& event ){ OnFormsToggle( event ); }
		void _wxFB_OnClassNameChange( wxCommandEvent& event ){ OnClassNameChange( event ); }
		void _wxFB_OnFileNameChange( wxCommandEvent& event ){ OnFileNameChange( event ); }
		
	
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
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFormsToggle( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnClassNameChange( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnFileNameChange( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		GenInheritedClassDlgBase( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Generate Inherited Class"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( -1,-1 ), int style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__geninhertclass_gui__
