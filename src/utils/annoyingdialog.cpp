/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* Revision: 4204
* Id: annoyingdialog.cpp 4204 2007-07-02 12:30:47Z mandrav
* HeadURL: http://svn.berlios.de/svnroot/repos/codeblocks/trunk/src/sdk/annoyingdialog.cpp
*/

#include "annoyingdialog.h"

#include <memory>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/config.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>

BEGIN_EVENT_TABLE(AnnoyingDialog, wxDialog)
    EVT_BUTTON(-1, AnnoyingDialog::OnButton)
END_EVENT_TABLE()

AnnoyingDialog::AnnoyingDialog(const wxString& caption, const wxString& message, const wxArtID icon,
                               dStyle style, int defaultReturn, bool /*separate*/,
                               const wxString& b1, const wxString& b2, const wxString& b3)
        : wxDialog(NULL, -1, caption, wxDefaultPosition, wxDefaultSize, wxCAPTION),
        m_cb(0),
        m_dontAnnoy(false),
        m_defRet(defaultReturn)
{
	wxConfigBase* config = wxConfigBase::Get();
	int defRet;
	if ( config->Read( wxT("annoyingdialog/") + caption, &defRet ) )
	{
		if ( defRet != wxID_CANCEL )
		{
			m_dontAnnoy = true;
			m_defRet = defRet;
			return;
		}
    }

	auto outerSizer = std::make_unique<wxBoxSizer>(wxVERTICAL);

    wxFlexGridSizer *mainArea = new wxFlexGridSizer(2, 0, 0);
    wxStaticBitmap *bitmap = new wxStaticBitmap(this, -1, wxArtProvider::GetBitmap(icon,  wxART_MESSAGE_BOX), wxDefaultPosition);
    mainArea->Add(bitmap, 0, wxALL, 5);

    wxStaticText *txt = new wxStaticText(this, -1, message, wxDefaultPosition, wxDefaultSize, 0);
    mainArea->Add( txt, 0, wxALIGN_CENTER|wxALL, 5 );

    mainArea->Add( 1, 1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5 );

    int numButtons = 0;
    int id1 = -1;
    int id2 = -1;
    int id3 = -1;
    wxString bTxt1;
    wxString bTxt2;
    wxString bTxt3;

    if(style == OK || style == ONE_BUTTON)
    {
        numButtons = 1;
        id1 = (style == OK ? wxID_OK : 1);
        bTxt1 = b1.IsEmpty() ? wxString(_("&OK")) : b1;
    }
    else if(style == YES_NO || style == OK_CANCEL || style == TWO_BUTTONS)
    {
        numButtons = 2;
        id1 = (style == YES_NO ? wxID_YES : (style == OK_CANCEL ? wxID_OK     : 1));
        id2 = (style == YES_NO ? wxID_NO  : (style == OK_CANCEL ? wxID_CANCEL : 2));
        bTxt1 = b1.IsEmpty() ? (style == YES_NO ? wxString(_("&Yes")) : wxString(_("&OK")))     : b1;
        bTxt2 = b2.IsEmpty() ? (style == YES_NO ? wxString(_("&No"))  : wxString(_("&Cancel"))) : b2;
    }
    else if(style == YES_NO_CANCEL || style == THREE_BUTTONS)
    {
        numButtons = 3;
        id1 = (style == YES_NO_CANCEL ? wxID_YES    : 1);
        id2 = (style == YES_NO_CANCEL ? wxID_NO     : 2);
        id3 = (style == YES_NO_CANCEL ? wxID_CANCEL : 3);
        bTxt1 = b1.IsEmpty() ? wxString(_("&Yes")) : b1;
        bTxt2 = b2.IsEmpty() ? wxString(_("&No")) : b2;
        bTxt3 = b3.IsEmpty() ? wxString(_("&Cancel")) : b3;
    }
    else
    {
        wxLogError( wxT("Fatal error:\nUndefined style in dialog %s"), caption.c_str() );
        return;
    }

	wxSizer* buttonSizer = 0;
	if (style < ONE_BUTTON) // standard buttons? use wxStdDialogButtonSizer
	{
		wxStdDialogButtonSizer *buttonArea = new wxStdDialogButtonSizer();

		wxButton *but1 = new wxButton(this, id1, bTxt1, wxDefaultPosition, wxDefaultSize, 0);
		but1->SetDefault();
		buttonArea->AddButton(but1);

		if(numButtons > 1)
		{
			wxButton *but2 = new wxButton(this, id2, bTxt2, wxDefaultPosition, wxDefaultSize, 0);
			but2->SetDefault();
			buttonArea->AddButton(but2);
		}
		if(numButtons > 2)
		{
			wxButton *but3 = new wxButton(this, id3, bTxt3, wxDefaultPosition, wxDefaultSize, 0);
			but3->SetDefault();
			buttonArea->AddButton(but3);
		}
		buttonArea->Realize();
		buttonSizer = buttonArea;
	}
	else
	{
		// wxStdDialogButtonSizer accepts only standard IDs for its buttons, so we can't use
		// it with custom buttons
		buttonSizer = new wxBoxSizer(wxHORIZONTAL);

		wxButton *but1 = new wxButton(this, id1, bTxt1, wxDefaultPosition, wxDefaultSize, 0);
		but1->SetDefault();
		buttonSizer->Add(but1, 0, wxRIGHT, 5);

		if(numButtons > 1)
		{
			wxButton *but2 = new wxButton(this, id2, bTxt2, wxDefaultPosition, wxDefaultSize, 0);
			but2->SetDefault();
			buttonSizer->Add(but2, 0, wxRIGHT, 5);
		}
		if(numButtons > 2)
		{
			wxButton *but3 = new wxButton(this, id3, bTxt3, wxDefaultPosition, wxDefaultSize, 0);
			but3->SetDefault();
			buttonSizer->Add(but3, 0, wxRIGHT, 5);
		}
	}

    outerSizer->Add( mainArea, 0, wxALIGN_CENTER|wxALL, 5);
    outerSizer->Add( buttonSizer, 0, wxALIGN_CENTER);

    m_cb = new wxCheckBox(this, -1, _("Don't annoy me again!"), wxDefaultPosition, wxDefaultSize, 0);
    outerSizer->Add(m_cb, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

	SetSizer(outerSizer.release());
	GetSizer()->SetSizeHints(this);

    Centre();
}

void AnnoyingDialog::OnButton(wxCommandEvent& event)
{
    if(!m_cb)
    {
        wxLogError(_T("Ow... null pointer."));
        return;
    }

	if ( event.GetId() != wxID_CANCEL )
	{
		wxConfigBase* config = wxConfigBase::Get();
		if ( m_cb->IsChecked() )
		{
			config->Write( wxT("annoyingdialog/") + GetTitle(), event.GetId() );
		}
	}
    EndModal(event.GetId());
}

int AnnoyingDialog::ShowModal()
{
    if(m_dontAnnoy)
        return m_defRet;
    return wxDialog::ShowModal();
}
