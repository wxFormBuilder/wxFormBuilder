///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 17 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DialogGui__
#define __DialogGui__

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/listbook.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class FBSettings
///////////////////////////////////////////////////////////////////////////////
class FBSettings : public wxDialog 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_m_idBoxOnCheckBox( wxCommandEvent& event ){ m_idBoxOnCheckBox( event ); }
		void _wxFB_m_handlerBoxOnCheckBox( wxCommandEvent& event ){ m_handlerBoxOnCheckBox( event ); }
		void _wxFB_m_sdbSizer1OnCancelButtonClick( wxCommandEvent& event ){ m_sdbSizer1OnCancelButtonClick( event ); }
		void _wxFB_m_sdbSizer1OnOKButtonClick( wxCommandEvent& event ){ m_sdbSizer1OnOKButtonClick( event ); }
		
	
	protected:
		enum
		{
			ID_M_IDBOX = 1000,
			ID_M_HANDLERBOX,
		};
		
		wxListbook* m_listbook2;
		wxPanel* m_panel5;
		wxCheckBox* UserBlocksCheckBox;
		wxCheckBox* m_idBox;
		wxCheckBox* m_handlerBox;
		wxStaticText* m_staticText3;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrl2;
		wxTextCtrl* m_textCtrl1;
		wxStaticText* m_staticText4;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void m_idBoxOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_handlerBoxOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_sdbSizer1OnCancelButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_sdbSizer1OnOKButtonClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		FBSettings( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("wxFB Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 492,456 ), long style = wxDEFAULT_DIALOG_STYLE );
		~FBSettings();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ProjectSettings
///////////////////////////////////////////////////////////////////////////////
class ProjectSettings : public wxDialog 
{
	DECLARE_EVENT_TABLE()
	private:
		
		// Private event handlers
		void _wxFB_m_sdbSizer2OnCancelButtonClick( wxCommandEvent& event ){ m_sdbSizer2OnCancelButtonClick( event ); }
		void _wxFB_m_sdbSizer2OnOKButtonClick( wxCommandEvent& event ){ m_sdbSizer2OnOKButtonClick( event ); }
		
	
	protected:
		wxListbook* m_listbook3;
		wxPanel* m_panel4;
		wxStdDialogButtonSizer* m_sdbSizer2;
		wxButton* m_sdbSizer2OK;
		wxButton* m_sdbSizer2Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void m_sdbSizer2OnCancelButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_sdbSizer2OnOKButtonClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ProjectSettings( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Project Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 202,160 ), long style = wxDEFAULT_DIALOG_STYLE );
		~ProjectSettings();
	
};

#endif //__DialogGui__
