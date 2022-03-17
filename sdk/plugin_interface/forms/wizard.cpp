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
//   Andrea Zanellato - zanellato.andrea@gmail.com
//   based on original wizard.cpp file in wxWidgets source code
//
///////////////////////////////////////////////////////////////////////////////

#include "wizard.h"


wxDEFINE_EVENT(wxFB_EVT_WIZARD_PAGE_CHANGED, WizardEvent);
wxDEFINE_EVENT(wxFB_EVT_WIZARD_PAGE_CHANGING, WizardEvent);
wxDEFINE_EVENT(wxFB_EVT_WIZARD_CANCEL, WizardEvent);
wxDEFINE_EVENT(wxFB_EVT_WIZARD_FINISHED, WizardEvent);
wxDEFINE_EVENT(wxFB_EVT_WIZARD_HELP, WizardEvent);
wxDEFINE_EVENT(wxFB_EVT_WIZARD_PAGE_SHOWN, WizardEvent);


Wizard::Wizard(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) :
  WizardBase(parent, id, pos, size, style), m_page(nullptr)
{
    this->Connect(wxID_ANY, wxFB_EVT_WIZARD_PAGE_CHANGED, WizardEventHandler(Wizard::OnWizEvent));
    this->Connect(wxID_ANY, wxFB_EVT_WIZARD_PAGE_CHANGING, WizardEventHandler(Wizard::OnWizEvent));
    this->Connect(wxID_ANY, wxFB_EVT_WIZARD_CANCEL, WizardEventHandler(Wizard::OnWizEvent));
    this->Connect(wxID_ANY, wxFB_EVT_WIZARD_FINISHED, WizardEventHandler(Wizard::OnWizEvent));
    this->Connect(wxID_ANY, wxFB_EVT_WIZARD_HELP, WizardEventHandler(Wizard::OnWizEvent));
}

Wizard::~Wizard()
{
    this->Disconnect(wxID_ANY, wxFB_EVT_WIZARD_PAGE_CHANGED, WizardEventHandler(Wizard::OnWizEvent));
    this->Disconnect(wxID_ANY, wxFB_EVT_WIZARD_PAGE_CHANGING, WizardEventHandler(Wizard::OnWizEvent));
    this->Disconnect(wxID_ANY, wxFB_EVT_WIZARD_CANCEL, WizardEventHandler(Wizard::OnWizEvent));
    this->Disconnect(wxID_ANY, wxFB_EVT_WIZARD_FINISHED, WizardEventHandler(Wizard::OnWizEvent));
    this->Disconnect(wxID_ANY, wxFB_EVT_WIZARD_HELP, WizardEventHandler(Wizard::OnWizEvent));
}


void Wizard::SetBitmap(const wxBitmap& bitmap)
{
    m_statbmp->SetBitmap(bitmap);

    auto pageSize = m_sizerBmpAndPage->GetSize();
    pageSize.IncTo(wxSize(0, bitmap.GetHeight()));
    m_sizerBmpAndPage->SetMinSize(pageSize);
}


void Wizard::AddPage(WizardPageSimple* page)
{
    m_page = page;                          // Update current page,
    m_pages.push_back(page);                // add it to internal page array,
    const auto pageCount = m_pages.size();  // Internal page array count

    for (size_t i = 0; i < pageCount; ++i) {
        m_pages[i]->Hide();
    }
    page->Show();

    m_sizerPage->Add(page, 1, wxEXPAND, 0);  // insert it into the page sizer,
    Layout();                                // update layout,

    if (pageCount == 1) {                   // First page: no previous, no next
        m_btnNext->Enable(true);            // but enable the next page button
        m_btnNext->SetLabel(_("&Finish"));  // because it acts as 'finish'
    } else if (pageCount == 2) {            // Enable previous page button:
        m_btnPrev->Enable(true);            // from now on everything is done in the OnBackOrNext() event
    }                                       // when user clicks on 'back' or 'next' buttons.
}

size_t Wizard::GetPageIndex(WizardPageSimple* wizpage) const
{
    for (size_t i = 0; i < m_pages.size(); ++i) {
        if (m_pages[i] == wizpage) {
            return i;
        }
    }

    return static_cast<size_t>(-1);
}


void Wizard::SetSelection(size_t pageIndex)
{
    const auto pageCount = m_pages.size();         // Internal page array count
    if (pageIndex < pageCount) {                   // Is it a valid index?
        const auto hasPrev = (pageIndex > 0);              // Has this page a previous one,
        const auto hasNext = (pageIndex < pageCount - 1);  // or another after it?

        m_page = m_pages[pageIndex];  // Yes, update current page and

        m_btnPrev->Enable(hasPrev);  // enable 'back' button if a previous page exists,
        wxString label = (hasNext ? _("&Next >") : _("&Finish"));
        if (label != m_btnNext->GetLabel()) {  // set the correct label on next button
            m_btnNext->SetLabel(label);
        }

        m_btnNext->SetDefault();  // and as default one, user needs it ready to go on.
    }
}


void Wizard::OnBackOrNext(wxCommandEvent& event)
{
    auto pageIndex = GetPageIndex(m_page);  // Get index of previous selected page
    auto forward = (event.GetEventObject() == m_btnNext);

    if (forward) {
        ++pageIndex;
    } else {
        --pageIndex;
    }
    SetSelection(pageIndex);

    WizardEvent eventChanged(wxFB_EVT_WIZARD_PAGE_CHANGED, GetId(), forward, m_page);
    m_page->GetEventHandler()->ProcessEvent(eventChanged);
}

void Wizard::OnHelp(wxCommandEvent&)
{
    // this function probably can never be called when we don't have an active
    // page, but a small extra check won't hurt
    if (m_page) {
        // Create and send the help event to the specific page handler
        // event data contains the active page so that context-sensitive
        // help is possible
        WizardEvent eventHelp(wxFB_EVT_WIZARD_HELP, GetId(), true, m_page);
        m_page->GetEventHandler()->ProcessEvent(eventHelp);
    }
}

void Wizard::OnCancel(wxCommandEvent&)
{
    WizardEvent eventCancel(wxFB_EVT_WIZARD_CANCEL, GetId(), false, m_page);
    GetEventHandler()->ProcessEvent(eventCancel);
}


void Wizard::OnWizEvent(WizardEvent& event)
{
    if (event.IsAllowed()) {
        const auto eventType = event.GetEventType();

        if (eventType == wxFB_EVT_WIZARD_PAGE_CHANGED) {
            for (size_t i = 0; i < m_pages.size(); ++i) {
                m_pages[i]->Hide();
            }
            event.GetPage()->Show();

            Layout();
        }
#if 0
        else if (eventType == wxFB_EVT_WIZARD_PAGE_CHANGING) {
            wxLogDebug(wxT("Wizard Page changing."));
        } else if (eventType == wxFB_EVT_WIZARD_CANCEL) {
            wxLogDebug(wxT("Wizard Cancel button was pressed."));
        } else if (eventType == wxFB_EVT_WIZARD_HELP) {
            wxLogDebug(wxT("Wizard Help button was pressed."));
        } else if (eventType == wxFB_EVT_WIZARD_FINISHED) {
            wxLogDebug(wxT("Wizard Finish button was pressed."));
        } else if (eventType == wxFB_EVT_WIZARD_PAGE_SHOWN) {
            wxLogDebug(wxT("Wizard Page shown."));
        }
#endif
    }
}


WizardEvent::WizardEvent(wxEventType type, int id, bool direction, WizardPageSimple* page) : wxNotifyEvent(type, id)
{
    // Modified 10-20-2001 Robert Cavanaugh
    // add the active page to the event data
    m_direction = direction;
    m_page = page;
}
