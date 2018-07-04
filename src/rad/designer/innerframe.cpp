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
//   José Antonio Hurtado - joseantonio.hurtado@gmail.com
//   Juan Antonio Ortega  - jortegalalmolda@gmail.com
//
///////////////////////////////////////////////////////////////////////////////
#include "innerframe.h"
#include "window_buttons.h"
#include <wx/dcbuffer.h>
#include <wx/settings.h>

DEFINE_EVENT_TYPE( wxEVT_INNER_FRAME_RESIZED )

class wxInnerFrame::TitleBar : public wxPanel
{
private:
	DECLARE_EVENT_TABLE()

	void DrawTitleBar ( wxDC &dc );

	wxBitmap m_minimize;
	wxBitmap m_minimizeDisabled;
	wxBitmap m_maximize;
	wxBitmap m_maximizeDisabled;
	wxBitmap m_close;
	wxBitmap m_closeDisabled;
	wxColour m_colour1, m_colour2;
	wxString m_titleText;
	long m_style;

protected:
	wxSize DoGetBestSize() const
	{
		return wxSize( 100, 19 );
	}

public:
	TitleBar ( wxWindow *parent, wxWindowID id,
	           const wxPoint &pos = wxDefaultPosition,
	           const wxSize &size = wxDefaultSize,
	           long style = 0 );

	void OnPaint( wxPaintEvent &event );

	void OnLeftClick ( wxMouseEvent &event );
	void SetTitle( const wxString &title ) { m_titleText = title; }
	wxString GetTitle() { return m_titleText; }
	void SetStyle( long style ) { m_style = style; }

};


BEGIN_EVENT_TABLE( wxInnerFrame::TitleBar, wxPanel )
    EVT_LEFT_DOWN( wxInnerFrame::TitleBar::OnLeftClick )
    EVT_PAINT( wxInnerFrame::TitleBar::OnPaint )
END_EVENT_TABLE()

wxInnerFrame::TitleBar::TitleBar ( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style )
:
wxPanel( parent, id, pos, size, 0/*wxFULL_REPAINT_ON_RESIZE*/ ),
m_minimize( minimize_xpm ),
m_minimizeDisabled( minimize_disabled_xpm ),
m_maximize( maximize_xpm ),
m_maximizeDisabled( maximize_disabled_xpm ),
m_close( close_xpm ),
m_closeDisabled( close_disabled_xpm ),
m_style( style )
{
	//m_colour1 = wxColour(10,36,106);
	//m_colour2 = wxColour(166,202,240);

	m_colour1 = wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION );

	int r, g, b;

	r = wxMin( 255, m_colour1.Red() + 30 );
	g = wxMin( 255, m_colour1.Green() + 30 );
	b = wxMin( 255, m_colour1.Blue() + 30 );

	m_colour2 = wxColour( r, g, b );
	m_titleText = wxT( "wxFormBuilder rocks!" );
	SetMinSize( wxSize( 100, 19 ) );
}

void wxInnerFrame::TitleBar::OnLeftClick ( wxMouseEvent &event )
{
	LogDebug("OnLeftClick");
	GetParent()->GetEventHandler()->ProcessEvent( event );
}

void wxInnerFrame::TitleBar::OnPaint ( wxPaintEvent& )
{
	wxPaintDC dc( this );
	wxBufferedDC bdc( &dc, GetClientSize() );
	DrawTitleBar( bdc );
}


void wxInnerFrame::TitleBar::DrawTitleBar( wxDC &dc )
{
	static const int margin = 2;

	int tbPosX, tbPosY; // title bar
	int tbWidth, tbHeight;

	int wbPosX, wbPosY; // window buttons
	int wbWidth /*, wbHeight*/;

	int txtPosX, txtPosY; // title text position
	int /*txtWidth,*/ txtHeight;

	// setup all variables

	wxSize clientSize( GetClientSize() );
	//wxSize clientSize(500,100);

	tbPosX = tbPosY = 0;
	tbHeight = m_close.GetHeight() + margin * 2;
	tbWidth = clientSize.GetWidth();

	/*wbHeight = m_close.GetHeight();*/
	wbWidth = m_close.GetWidth();
	wbPosX = tbPosX + tbWidth - wbWidth - 2 * margin;
	wbPosY = tbPosX + margin;

	txtPosY = tbPosY + margin;
	txtPosX = tbPosX + 15 + 2 * margin;
	txtHeight = tbHeight - 2 * margin + 1;
	/*txtWidth = wbPosX - 2 * margin - txtPosX;*/

	// Draw title background with vertical gradient.
	float incR = ( float )( m_colour2.Red() - m_colour1.Red() ) / tbWidth;
	float incG = ( float )( m_colour2.Green() - m_colour1.Green() ) / tbWidth;
	float incB = ( float )( m_colour2.Blue() - m_colour1.Blue() ) / tbWidth;

	float colourR = m_colour1.Red();
	float colourG = m_colour1.Green();
	float colourB = m_colour1.Blue();

	wxColour colour;
	wxPen pen;
	for ( int i = 0; i < tbWidth; i++ )
	{
		colour.Set( ( unsigned char )colourR, ( unsigned char )colourG, ( unsigned char )colourB );
		pen.SetColour( colour );
		dc.SetPen( pen );
		dc.DrawLine( tbPosX + i, tbPosY, tbPosX + i, tbPosY + tbHeight );

		colourR += incR;
		colourG += incG;
		colourB += incB;
	}

	// Draw title background with horizontal gradient.
	/*float incR = (float)(m_colour2.Red() - m_colour1.Red()) / tbHeight;
	float incG = (float)(m_colour2.Green() - m_colour1.Green()) / tbHeight;
	float incB = (float)(m_colour2.Blue() - m_colour1.Blue()) / tbHeight;

	float colourR = m_colour1.Red();
	float colourG = m_colour1.Green();
	float colourB = m_colour1.Blue();

	wxColour colour;
	wxPen pen;
	for (int i=0; i<tbHeight; i++)
	{
	  colour.Set((unsigned char)colourR, (unsigned char)colourG, (unsigned char)colourB);
	pen.SetColour(colour);
	dc.SetPen(pen);
	dc.DrawLine(tbPosX, tbPosY + i, tbPosX + tbWidth, tbPosY + i);

	colourR += incR;
	colourG += incG;
	colourB += incB;
	}*/

	// Draw title text
	wxFont font = dc.GetFont();
	wxSize ppi = dc.GetPPI();

	int fontSize = 72 * txtHeight / ppi.GetHeight();

	font.SetWeight( wxBOLD );
	dc.SetTextForeground( *wxWHITE );

	// text vertical adjustment
	wxCoord tw, th;
	do
	{
		font.SetPointSize( fontSize-- );
		dc.SetFont( font );
		dc.GetTextExtent( m_titleText, &tw, &th );
	} while ( th > txtHeight );

	dc.DrawLabel( m_titleText, wxRect( txtPosX, txtPosY, tw, th ) );

	// Draw Buttons
	bool hasClose = ( m_style & wxCLOSE_BOX ) != 0;
	bool hasMinimize = ( m_style & wxMINIMIZE_BOX ) != 0;
	bool hasMaximize = ( m_style & wxMAXIMIZE_BOX ) != 0;

	#ifdef __WXMSW__
		if ( ( m_style & wxSYSTEM_MENU ) == 0 )
		{
			// On Windows, no buttons are drawn without System Menu
			return;
		}

		dc.DrawBitmap( hasClose ? m_close : m_closeDisabled, wbPosX, wbPosY, true );
		wbPosX -= m_close.GetWidth();

		if ( hasMaximize )
		{
			dc.DrawBitmap( m_maximize, wbPosX, wbPosY, true );
		}
		else if ( hasMinimize )
		{
			dc.DrawBitmap( m_maximizeDisabled, wbPosX, wbPosY, true );
		}
		wbPosX -= m_maximize.GetWidth();

		if ( hasMinimize )
		{
			dc.DrawBitmap( m_minimize, wbPosX, wbPosY, true );
		}
		else if ( hasMaximize )
		{
			dc.DrawBitmap( m_minimizeDisabled, wbPosX, wbPosY, true );
		}
	#else // GTK
		if ( hasClose )
		{
			dc.DrawBitmap( m_close, wbPosX, wbPosY, true );
			wbPosX -= m_close.GetWidth();
		}

		bool hasResizeBorder = ( m_style & wxRESIZE_BORDER ) != 0;
		if ( hasMaximize && hasResizeBorder )
		{
			dc.DrawBitmap( m_maximize, wbPosX, wbPosY, true );
			wbPosX -= m_maximize.GetWidth();
		}

		if ( hasMinimize )
		{
			dc.DrawBitmap( m_minimize, wbPosX, wbPosY, true );
		}
	#endif
}

//////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( wxInnerFrame, wxPanel )
	EVT_MOTION( wxInnerFrame::OnMouseMotion )
	EVT_LEFT_DOWN( wxInnerFrame::OnLeftDown )
	EVT_LEFT_UP( wxInnerFrame::OnLeftUp )
END_EVENT_TABLE()

wxInnerFrame::wxInnerFrame( wxWindow *parent, wxWindowID id,
                            const wxPoint &pos, const wxSize &size, long style )
#ifdef __WXGTK__
	: wxPanel( parent, id, pos, size, wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE )
#else
	: wxPanel( parent, id, pos, size, wxRAISED_BORDER | wxFULL_REPAINT_ON_RESIZE )
#endif
{
	m_sizing = NONE;
	m_curX = m_curY = -1;
	m_resizeBorder = 10;

	m_titleBar = new TitleBar( this, -1, wxDefaultPosition, wxDefaultSize, style );
	m_frameContent = new wxPanel( this, -1, wxDefaultPosition, wxDefaultSize );

	// Use spacers to create a 1 pixel border on left and top of content panel - this is for drawing the selection box
	// Use borders to create a 2 pixel border on right and bottom - this is so the back panel can catch mouse events for resizing
	wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
	sizer->Add( m_titleBar, 0, wxGROW | wxRIGHT, 2 );
	sizer->AddSpacer( 1 );
	wxBoxSizer *horiSizer = new wxBoxSizer( wxHORIZONTAL );
	horiSizer->AddSpacer( 1 );
	horiSizer->Add( m_frameContent, 1, wxGROW );
	sizer->Add( horiSizer, 1, wxGROW | wxBOTTOM | wxRIGHT, 2 );

	SetSizer( sizer );
	SetAutoLayout( true );
	Layout();

	m_minSize = m_titleBar->GetMinSize();
	m_minSize.x += 8;
	m_minSize.y += 10;
	m_baseMinSize = m_minSize;

	if ( wxDefaultSize == size )
	{
		SetSize( GetBestSize() );
	}
}

wxSize wxInnerFrame::DoGetBestSize() const
{
    wxSize best;
    best = m_titleBar->GetBestSize();
    wxSize content = m_frameContent->GetBestSize();
    best.IncBy( 0, content.GetHeight() );
    int border = wxSystemSettings::GetMetric( wxSYS_BORDER_X );
    best.SetWidth( ( content.GetWidth() + 1 > best.GetWidth() ? content.GetWidth() + 1 : best.GetWidth() ) + 2 + 2 * ( border > 0 ? border : 2 ) );

    // spacers and borders
    best.IncBy( 0, 3 );

    return best;
}

void wxInnerFrame::OnMouseMotion( wxMouseEvent& e )
{
	if ( m_sizing != NONE )
	{
		wxScreenDC dc;
		wxPen pen( *wxBLACK, 1, wxDOT );

		dc.SetPen( pen );
		dc.SetBrush( *wxTRANSPARENT_BRUSH );
		dc.SetLogicalFunction( wxINVERT );

		//wxPoint pos = ClientToScreen(wxPoint(0, 0));
		wxPoint pos = GetParent()->ClientToScreen( GetPosition() );

		if ( m_curX >= 0 && m_curY >= 0 )
			dc.DrawRectangle( pos.x, pos.y, m_curX, m_curY );

		if ( m_sizing == RIGHT || m_sizing == RIGHTBOTTOM )
			m_curX = e.GetX() + m_difX;
		else
			m_curX = GetSize().x;

		if ( m_sizing == BOTTOM || m_sizing == RIGHTBOTTOM )
			m_curY = e.GetY() + m_difY;
		else
			m_curY = GetSize().y;

		// User min size
		wxSize minSize = GetMinSize();
		if ( m_curX < minSize.x ) m_curX = minSize.x;
		if ( m_curY < minSize.y ) m_curY = minSize.y;

		// Internal min size
		if ( m_curX < m_minSize.x ) m_curX = m_minSize.x;
		if ( m_curY < m_minSize.y ) m_curY = m_minSize.y;

		wxSize maxSize = GetMaxSize();
		if ( m_curX > maxSize.x && maxSize.x != wxDefaultCoord ) m_curX = maxSize.x;
		if ( m_curY > maxSize.y && maxSize.y != wxDefaultCoord ) m_curY = maxSize.y;

		dc.DrawRectangle( pos.x, pos.y, m_curX, m_curY );

		dc.SetLogicalFunction( wxCOPY );
		dc.SetPen( wxNullPen );
		dc.SetBrush( wxNullBrush );
	}

	else
	{
		int x, y;
		GetClientSize( &x, &y );

		if ( ( e.GetX() >= x - m_resizeBorder && e.GetY() >= y - m_resizeBorder ) ||
		        ( e.GetX() < m_resizeBorder && e.GetY() < m_resizeBorder ) )
		{
			SetCursor( wxCursor( wxCURSOR_SIZENWSE ) );
		}
		else if ( ( e.GetX() < m_resizeBorder && e.GetY() >= y - m_resizeBorder ) ||
		          ( e.GetX() > x - m_resizeBorder && e.GetY() < m_resizeBorder ) )
		{
			SetCursor( wxCursor( wxCURSOR_SIZENESW ) );
		}
		else if ( e.GetX() >= x - m_resizeBorder || e.GetX() < m_resizeBorder )
		{
			SetCursor( wxCursor( wxCURSOR_SIZEWE ) );
		}
		else if ( e.GetY() >= y - m_resizeBorder || e.GetY() < m_resizeBorder )
		{
			SetCursor( wxCursor( wxCURSOR_SIZENS ) );
		}
		else
		{
			SetCursor( *wxSTANDARD_CURSOR );
		}

		m_titleBar->SetCursor( *wxSTANDARD_CURSOR );
		m_frameContent->SetCursor( *wxSTANDARD_CURSOR );
	}
}

void wxInnerFrame::OnLeftDown( wxMouseEvent& e )
{
	LogDebug(wxT("OnLeftDown"));
	if ( m_sizing == NONE )
	{
		if ( e.GetX() >= GetSize().x - m_resizeBorder && e.GetY() >= GetSize().y - m_resizeBorder )
			m_sizing = RIGHTBOTTOM;
		else if ( e.GetX() >= GetSize().x - m_resizeBorder )
			m_sizing = RIGHT;
		else if ( e.GetY() >= GetSize().y - m_resizeBorder )
			m_sizing = BOTTOM;

		if ( m_sizing != NONE )
		{
			m_difX = GetSize().x - e.GetX();
			m_difY = GetSize().y - e.GetY();
			CaptureMouse();
			OnMouseMotion( e );
		}
	}
}

void wxInnerFrame::OnLeftUp( wxMouseEvent& )
{
	if ( m_sizing != NONE )
	{
		m_sizing = NONE;
		ReleaseMouse();

		wxScreenDC dc;
		wxPen pen( *wxBLACK, 1, wxDOT );

		dc.SetPen( pen );
		dc.SetBrush( *wxTRANSPARENT_BRUSH );
		dc.SetLogicalFunction( wxINVERT );

		//wxPoint pos = ClientToScreen(wxPoint(0, 0));
		wxPoint pos = GetParent()->ClientToScreen( GetPosition() );

		dc.DrawRectangle( pos.x, pos.y, m_curX, m_curY );

		dc.SetLogicalFunction( wxCOPY );
		dc.SetPen( wxNullPen );
		dc.SetBrush( wxNullBrush );

		wxScrolledWindow * VEditor = (wxScrolledWindow*)GetParent();
		int scrolledposX = 0;
		int scrolledposY = 0;
		VEditor->GetViewStart( &scrolledposX, &scrolledposY );
		SetSize( m_curX, m_curY );
		Freeze();
		VEditor->FitInside();
		VEditor->SetVirtualSize(GetSize().x + 20, GetSize().y + 20);

		wxCommandEvent event( wxEVT_INNER_FRAME_RESIZED, GetId() );
		event.SetEventObject( this );
		GetEventHandler()->AddPendingEvent( event );

		VEditor->Scroll( scrolledposX, scrolledposY );
		Thaw();
		Update();

		m_curX = m_curY = -1;
	}
}


void wxInnerFrame::ShowTitleBar( bool show )
{
	m_titleBar->Show( show );
	m_minSize = ( show ? m_baseMinSize : wxSize( 10, 10 ) );
	Layout();
}

void wxInnerFrame::SetToBaseSize()
{
	if ( m_titleBar->IsShown() )
	{
		SetSize( m_baseMinSize );
	}
	else
	{
		SetSize( wxSize( 10, 10 ) );
	}
}

bool wxInnerFrame::IsTitleBarShown()
{
	return m_titleBar->IsShown();
}

void wxInnerFrame::SetTitle( const wxString &title )
{
	m_titleBar->SetTitle( title );
}

wxString wxInnerFrame::GetTitle()
{
	return m_titleBar->GetTitle();
}

void wxInnerFrame::SetTitleStyle( long style )
{
	m_titleBar->SetStyle( style );
}

