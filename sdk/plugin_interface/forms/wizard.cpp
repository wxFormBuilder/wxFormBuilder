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
//   Andrea Zanellato - zanellato.andrea@gmail.com
//   based on original wizard.cpp file in wxWidgets source code
//
///////////////////////////////////////////////////////////////////////////////
#include "wizard.h"

#include <wx/log.h>

DEFINE_EVENT_TYPE(wxFB_EVT_WIZARD_PAGE_CHANGED)
DEFINE_EVENT_TYPE(wxFB_EVT_WIZARD_PAGE_CHANGING)
DEFINE_EVENT_TYPE(wxFB_EVT_WIZARD_CANCEL)
DEFINE_EVENT_TYPE(wxFB_EVT_WIZARD_FINISHED)
DEFINE_EVENT_TYPE(wxFB_EVT_WIZARD_HELP)
#if wxABI_VERSION >= 20811
DEFINE_EVENT_TYPE(wxFB_EVT_WIZARD_PAGE_SHOWN)
#endif
/*
BEGIN_EVENT_TABLE( Wizard, wxPanel )
    EVT_BUTTON( wxID_CANCEL, Wizard::OnCancel )
    EVT_BUTTON( wxID_BACKWARD, Wizard::OnBackOrNext )
    EVT_BUTTON( wxID_FORWARD, Wizard::OnBackOrNext )
    EVT_BUTTON( wxID_HELP, Wizard::OnHelp )

    EVT_WXFB_WIZARD_PAGE_CHANGED( wxID_ANY, Wizard::OnWizEvent )
    EVT_WXFB_WIZARD_PAGE_CHANGING( wxID_ANY, Wizard::OnWizEvent )
    EVT_WXFB_WIZARD_CANCEL( wxID_ANY, Wizard::OnWizEvent )
    EVT_WXFB_WIZARD_FINISHED( wxID_ANY, Wizard::OnWizEvent )
    EVT_WXFB_WIZARD_HELP( wxID_ANY, Wizard::OnWizEvent )
END_EVENT_TABLE()
*/
WizardPageSimple::WizardPageSimple( Wizard *parent )
                : wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 )
{
}

WizardPageSimple::~WizardPageSimple()
{
}

Wizard::Wizard( wxWindow* parent, wxWindowID id,
                const wxPoint& pos, const wxSize& size, long style )
: wxPanel( parent, id, pos, size, style )
{
    m_page = NULL;
    m_bitmap = wxNullBitmap;

    wxBoxSizer *windowSizer  = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *mainColumn   = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *buttonRow    = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *backNextPair = new wxBoxSizer( wxHORIZONTAL );
    m_sizerBmpAndPage        = new wxBoxSizer( wxHORIZONTAL );
    m_sizerPage              = new wxBoxSizer( wxVERTICAL );

    m_statbmp                = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap );

    m_sizerBmpAndPage->SetMinSize( wxSize( 270, 270 ) );
    m_sizerBmpAndPage->Add( m_statbmp, 0, wxALL, 5 );
    m_sizerBmpAndPage->Add( 5, 0, 0, wxEXPAND, 0 );
    m_sizerBmpAndPage->Add( m_sizerPage, 1, wxEXPAND, 0 );

    m_btnHelp   = new wxButton( this, wxID_HELP,     _("&Help")   );
    m_btnPrev   = new wxButton( this, wxID_BACKWARD, _("< &Back") );
    m_btnNext   = new wxButton( this, wxID_FORWARD,  _("&Next >") );
    m_btnCancel = new wxButton( this, wxID_CANCEL,   _("&Cancel") );

    m_btnPrev->Enable( false );
    m_btnNext->Enable( false );

    backNextPair->Add( m_btnPrev, 0, wxBOTTOM|wxLEFT|wxTOP, 0 );
    backNextPair->Add( 10, 0, 0, wxEXPAND, 0 );
    backNextPair->Add( m_btnNext, 0, wxBOTTOM|wxRIGHT|wxTOP, 0 );

    buttonRow->Add( m_btnHelp, 0, wxALL, 5 );

    m_btnHelp->Hide();

    buttonRow->Add( backNextPair, 0, wxALL, 5 );
    buttonRow->Add( m_btnCancel, 0, wxALL, 5 );

    mainColumn->Add( m_sizerBmpAndPage, 1, wxEXPAND );
    mainColumn->Add( 0, 5, 0, wxEXPAND );
    mainColumn->Add( new wxStaticLine( this ), 0, wxEXPAND | wxALL, 5 );
    mainColumn->Add( 0, 5, 0, wxEXPAND ); 
    mainColumn->Add( buttonRow, 0, wxALIGN_RIGHT );

    windowSizer->Add( mainColumn, 1, wxALL|wxEXPAND, 5 );

    this->SetSizer( windowSizer );
    this->Layout();
    windowSizer->Fit( this );

    m_btnHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Wizard::OnHelp ), NULL, this );
    m_btnPrev->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Wizard::OnBackOrNext ), NULL, this );
    m_btnNext->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Wizard::OnBackOrNext ), NULL, this );
    m_btnCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Wizard::OnCancel ), NULL, this );

    this->Connect( wxID_ANY, wxFB_EVT_WIZARD_PAGE_CHANGED, WizardEventHandler( Wizard::OnWizEvent ) );
    this->Connect( wxID_ANY, wxFB_EVT_WIZARD_PAGE_CHANGING, WizardEventHandler( Wizard::OnWizEvent ) );
    this->Connect( wxID_ANY, wxFB_EVT_WIZARD_CANCEL, WizardEventHandler( Wizard::OnWizEvent ) );
    this->Connect( wxID_ANY, wxFB_EVT_WIZARD_FINISHED, WizardEventHandler( Wizard::OnWizEvent ) );
    this->Connect( wxID_ANY, wxFB_EVT_WIZARD_HELP, WizardEventHandler( Wizard::OnWizEvent ) );
}

Wizard::~Wizard()
{
    m_btnHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Wizard::OnHelp ), NULL, this );
    m_btnPrev->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Wizard::OnBackOrNext ), NULL, this );
    m_btnNext->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Wizard::OnBackOrNext ), NULL, this );
    m_btnCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Wizard::OnCancel ), NULL, this );

    this->Disconnect( wxID_ANY, wxFB_EVT_WIZARD_PAGE_CHANGED, WizardEventHandler( Wizard::OnWizEvent ) );
    this->Disconnect( wxID_ANY, wxFB_EVT_WIZARD_PAGE_CHANGING, WizardEventHandler( Wizard::OnWizEvent ) );
    this->Disconnect( wxID_ANY, wxFB_EVT_WIZARD_CANCEL, WizardEventHandler( Wizard::OnWizEvent ) );
    this->Disconnect( wxID_ANY, wxFB_EVT_WIZARD_FINISHED, WizardEventHandler( Wizard::OnWizEvent ) );
    this->Disconnect( wxID_ANY, wxFB_EVT_WIZARD_HELP, WizardEventHandler( Wizard::OnWizEvent ) );

    m_statbmp->SetBitmap( wxNullBitmap );
    m_bitmap = wxNullBitmap;
    m_page = NULL;
    m_pages.Clear();
}

void Wizard::OnHelp( wxCommandEvent& event )
{
    // this function probably can never be called when we don't have an active
    // page, but a small extra check won't hurt
    if( m_page != NULL )
    {
        // Create and send the help event to the specific page handler
        // event data contains the active page so that context-sensitive
        // help is possible
        WizardEvent eventHelp( wxFB_EVT_WIZARD_HELP, GetId(), true, m_page );
        ( void )m_page->GetEventHandler()->ProcessEvent( eventHelp );
    }
}

void Wizard::SetSelection( size_t pageIndex )
{
    size_t pageCount = m_pages.GetCount();          // Internal page array count
    if ( pageIndex >= 0 && pageIndex < pageCount )  // Is it a valid index?
    {
        bool hasPrev = pageIndex > 0;               // Has this page a previous one,
        bool hasNext = pageIndex < pageCount - 1;   // or another after it?

        m_page = m_pages.Item( pageIndex );         // Yes, update current page and

        m_btnPrev->Enable( hasPrev );   // enable 'back' button if a previous page exists,

        wxString label = hasNext ? _("&Next >") : _("&Finish");
        if ( label != m_btnNext->GetLabel() )       // set the correct label on next button
            m_btnNext->SetLabel( label );

        m_btnNext->SetDefault();        // and as default one, user needs it ready to go on.
    }
}

void Wizard::OnBackOrNext( wxCommandEvent& event )
{
    int pageIndex = m_pages.Index( m_page );  // Get index of previous selected page

    bool forward = event.GetEventObject() == m_btnNext;

    if ( forward )
    {
        pageIndex++;                             // Update current page index depending
    }
    else // ( event.GetEventObject() == m_btnPrev )
    {
        pageIndex--;                             // on which button was pressed.
    }

    SetSelection( pageIndex );

    WizardEvent eventChanged( wxFB_EVT_WIZARD_PAGE_CHANGED, GetId(), forward, m_page );
    m_page->GetEventHandler()->ProcessEvent( eventChanged );
}

void Wizard::OnCancel( wxCommandEvent& event )
{
    WizardEvent eventCancel( wxFB_EVT_WIZARD_CANCEL, GetId(), false, m_page );
    GetEventHandler()->ProcessEvent( eventCancel );
}

void Wizard::OnWizEvent( WizardEvent& event )
{
    if ( event.IsAllowed() )
    {
        wxEventType eventType = event.GetEventType();

        if ( eventType == wxFB_EVT_WIZARD_PAGE_CHANGED )
        {
            for ( unsigned int i = 0; i < m_pages.GetCount(); i++ )
                m_pages.Item( i )->Hide();

            event.GetPage()->Show();

            Layout();
        }
#if 0
        else if ( eventType == wxFB_EVT_WIZARD_PAGE_CHANGING )
        {
            wxLogDebug( wxT("Wizard Page changing.") );
        }
        else if ( eventType == wxFB_EVT_WIZARD_CANCEL )
        {
            wxLogDebug( wxT("Wizard Cancel button was pressed.") );
        }
        else if ( eventType == wxFB_EVT_WIZARD_HELP )
        {
            wxLogDebug( wxT("Wizard Help button was pressed.") );
        }
#if wxABI_VERSION >= 20811
        else if ( eventType == wxFB_EVT_WIZARD_FINISHED )
        {
            wxLogDebug( wxT("Wizard Finish button was pressed.") );
        }
#endif
        else if ( eventType == wxFB_EVT_WIZARD_PAGE_SHOWN )
        {
            wxLogDebug( wxT("Wizard Page shown.") );
        }
#endif
    }
}

void Wizard::AddPage( WizardPageSimple* page )
{
    m_page = page;                             // Update current page,
    m_pages.Add( page );                       // add it to internal page array,

    size_t pageCount = m_pages.GetCount();     // Internal page array count

    for ( unsigned int i = 0; i < pageCount; i++ )
        m_pages.Item( i )->Hide();

    page->Show();

    m_sizerPage->Add( page, 1, wxEXPAND, 0 );  // insert it into the page sizer,

    Layout();                                  // update layout,

    if ( pageCount == 1 )                      // First page: no previous, no next
    {
        m_btnNext->Enable( true );             // but enable the next page button
        m_btnNext->SetLabel(_("&Finish") );    // because it acts as 'finish'
    }
    else if ( pageCount == 2 )                 // Enable previous page button:
    {                                          // from now on everything is done in the
        m_btnPrev->Enable( true );             // OnBackOrNext() event when user clicks on
    }                                          // 'back' or 'next' buttons.
}

void Wizard::SetBitmap( const wxBitmap& bitmap )
{
    m_bitmap = bitmap;
    if ( m_statbmp )
    {
        m_statbmp->SetBitmap( m_bitmap );
        wxSize pageSize = m_sizerBmpAndPage->GetSize();
        pageSize.IncTo( wxSize( 0, m_bitmap.GetHeight() ) );
        m_sizerBmpAndPage->SetMinSize( pageSize );
    }
}

// ----------------------------------------------------------------------------
// WizardEvent
// ----------------------------------------------------------------------------

WizardEvent::WizardEvent( wxEventType type, int id, bool direction, WizardPageSimple *page )
            : wxNotifyEvent( type, id )
{
    // Modified 10-20-2001 Robert Cavanaugh
    // add the active page to the event data
    m_direction = direction;
    m_page = page;
}
