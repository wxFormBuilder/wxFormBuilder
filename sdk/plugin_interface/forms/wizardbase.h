///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-63fd119)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class WizardBase
///////////////////////////////////////////////////////////////////////////////
class WizardBase : public wxPanel
{
	private:

	protected:
		wxBoxSizer* m_sizerBmpAndPage;
		wxStaticBitmap* m_statbmp;
		wxBoxSizer* m_sizerPage;
		wxButton* m_btnHelp;
		wxButton* m_btnPrev;
		wxButton* m_btnNext;
		wxButton* m_btnCancel;

		// Virtual event handlers, override them in your derived class
		virtual void OnHelp( wxCommandEvent& event ) = 0;
		virtual void OnBackOrNext( wxCommandEvent& event ) = 0;
		virtual void OnCancel( wxCommandEvent& event ) = 0;


	public:

		WizardBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~WizardBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class WizardPageSimple
///////////////////////////////////////////////////////////////////////////////
class WizardPageSimple : public wxPanel
{
	private:

	protected:

	public:

		WizardPageSimple( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~WizardPageSimple();

};

