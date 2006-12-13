#include "nativeinnerframe.h"

#ifdef __WXGTK__
#define PANEL_BORDER wxRAISED_BORDER
#else
#define PANEL_BORDER wxDOUBLE_BORDER
#endif

BEGIN_EVENT_TABLE(wxNativeInnerFrame, ResizablePanel)
	//EVT_PAINT( wxNativeInnerFrame::OnPaint )
	EVT_PANEL_RESIZED( -1, wxNativeInnerFrame::OnSize )
	EVT_ERASE_BACKGROUND( wxNativeInnerFrame::OnErase )
END_EVENT_TABLE()

IMPLEMENT_CLASS( wxNativeInnerFrame, ResizablePanel )

wxNativeInnerFrame::wxNativeInnerFrame( wxWindow* parent, wxWindowID id, wxString title, wxPoint pos, wxSize size, long frameStyle )
:
ResizablePanel( parent, pos, size ),
m_title( title ),
m_showTitleBar( false ),
m_frameStyle( frameStyle ),
m_frameContent( NULL ),
m_vertSizer( NULL ),
m_horiSizer( NULL )
{
	//SetOwnBackgroundColour( wxColour( 192, 192, 192 ) );
	//SetOwnBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) );
	SetAutoLayout( true );
	SetFrameContentPanel( NULL );
}

wxNativeInnerFrame::~wxNativeInnerFrame()
{
}

void wxNativeInnerFrame::ShowTitleBar( bool show )
{
	if ( show == m_showTitleBar )
	{
		return;
	}
	m_showTitleBar = show;
	if ( show )
	{
		Connect( wxID_ANY, wxEVT_PAINT, wxPaintEventHandler( wxNativeInnerFrame::OnPaint ) );
		Recreate();
	}
	else
	{
		Disconnect( wxID_ANY, wxEVT_PAINT, wxPaintEventHandler( wxNativeInnerFrame::OnPaint ) );
		if ( NULL == m_frameContent )
		{
			SetSizer( NULL, true );
			m_horiSizer = NULL;
		}
		else
		{
			int borderWidth = 10;
			SetWindowStyle( PANEL_BORDER );
			if ( NULL != m_horiSizer )
			{
				m_horiSizer->Detach( m_frameContent );
			}

			SetSizer( NULL, true );
			m_vertSizer = new wxBoxSizer( wxVERTICAL );
			m_horiSizer = new wxBoxSizer( wxHORIZONTAL );
			m_horiSizer->Add( m_frameContent, 1, wxEXPAND );
			m_horiSizer->AddSpacer( borderWidth );
			m_vertSizer->Add( m_horiSizer, 1, wxEXPAND );
			m_vertSizer->AddSpacer( borderWidth );
			SetSizer( m_vertSizer );
			Layout();
		}
	}
}

void wxNativeInnerFrame::SetTitle( const wxString& title )
{
	m_title = title;
}

void wxNativeInnerFrame::SetStyle( long frameStyle )
{
	m_frameStyle = frameStyle;
}

wxPanel* wxNativeInnerFrame::GetFrameContentPanel()
{
	return m_frameContent;
}

void wxNativeInnerFrame::SetFrameContentPanel( wxPanel* panel )
{
	m_frameContent = panel;
	if ( m_frameContent != NULL )
	{
		m_frameContent->Reparent( this );
	}
	else
	{
		m_frameContent = new wxPanel( this );
	}
	//m_frameContent->Connect( wxID_ANY, wxEVT_LEFT_DOWN, wxMouseEventHandler( ResizablePanel::OnLeftDown ) );
	//m_frameContent->Connect( wxID_ANY, wxEVT_LEFT_UP, wxMouseEventHandler( ResizablePanel::OnLeftUp ) );
	//m_frameContent->Connect( wxID_ANY, wxEVT_MOTION, wxMouseEventHandler( ResizablePanel::OnMouseMotion ) );
}

void wxNativeInnerFrame::OnPaint( wxPaintEvent& event )
{
	wxPaintDC paint( this );
	if ( !m_background.Ok() )
	{
		Recreate();
	}
	paint.DrawBitmap( m_background, 0, 0 );
}

void wxNativeInnerFrame::OnErase( wxEraseEvent& event )
{

}

void wxNativeInnerFrame::OnSize( wxCommandEvent& event )
{
	if ( m_showTitleBar )
	{
		Recreate();
	}
	event.Skip();
}
void wxNativeInnerFrame::Recreate()
{
	if ( !m_showTitleBar )
	{
		return;
	}

	Freeze();

	int borderWidth = 0;
	int bottomBorder = 0;
	int titleBarHeight = 0;

	m_frameContent->SetWindowStyle( 0 );

	wxSize mySize = GetSize();
	wxPoint where = ClientToScreen( wxPoint( 0, 0 ) );

	// Draw frame
	wxFrame* frame = new wxFrame( NULL, wxID_ANY, m_title, where, mySize, m_frameStyle | wxSTAY_ON_TOP | wxFRAME_NO_TASKBAR );
	frame->Show();
	frame->Raise();
	frame->Update();
	wxMilliSleep(100);

	int panelWidth;
	int panelHeight;
	frame->GetClientSize( &panelWidth, &panelHeight );

	#ifdef __WXGTK__
		wxPanel* framesPanel = new wxPanel( frame );
		wxPoint panelWhere = frame->ClientToScreen( framesPanel->GetPosition() );
		borderWidth = panelWhere.x - where.x + 1;
		bottomBorder = borderWidth;
		titleBarHeight = panelWhere.y - where.y + 1;

		int frameWidth = panelWidth;
		int frameHeight = panelHeight;

		frame->Destroy();
		wxSize rightSize( frameWidth - ( 2 * borderWidth ), frameHeight - ( titleBarHeight + bottomBorder ) );
		frame = new wxFrame( NULL, wxID_ANY, m_title, where, rightSize, m_frameStyle | wxSTAY_ON_TOP | wxFRAME_NO_TASKBAR );
		frame->Show();
		frame->Raise();
		frame->Update();
		wxMilliSleep(100);
	#else
		frame->Show();
		frame->Raise();
		frame->Update();

		int frameWidth;
		int frameHeight;
		frame->GetSize( &frameWidth, &frameHeight );

		borderWidth = ( frameWidth - panelWidth )/2;
		bottomBorder = borderWidth;
		titleBarHeight = frameHeight - panelHeight - bottomBorder;
	#endif

	if( !m_background.Create( frameWidth, frameHeight ) )
	{
		wxLogError( _("Could not create bmp") );
	}

	wxMemoryDC mem;
	mem.SelectObject( m_background );

	wxScreenDC screen;
	if( !( mem.Blit( 0, 0, frameWidth, frameHeight, &screen, where.x, where.y, wxCOPY, false ) ) )
	{
		wxLogError( _("Blit to mem failed") );
	}

	frame->Destroy();

	if ( NULL == m_frameContent )
	{
		SetSizer( NULL, true );
		m_horiSizer = NULL;
	}
	else
	{
		if ( NULL != m_horiSizer )
		{
			m_horiSizer->Detach( m_frameContent );
		}
		SetSizer( NULL, true );
		m_vertSizer = new wxBoxSizer( wxVERTICAL );
		m_vertSizer->AddSpacer( titleBarHeight );
		m_horiSizer = new wxBoxSizer( wxHORIZONTAL );
		m_horiSizer->AddSpacer( borderWidth );
		m_horiSizer->Add( m_frameContent, 1, wxEXPAND );
		m_horiSizer->AddSpacer( borderWidth );
		m_vertSizer->Add( m_horiSizer, 1, wxEXPAND );
		m_vertSizer->AddSpacer( bottomBorder );
		SetSizer( m_vertSizer );
		Layout();
	}

	Thaw();
}
