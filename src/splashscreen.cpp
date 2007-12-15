/* cbSplashScreen.cpp
 *
 * READ THE HEADER FILE COMMENT FOR A DESCRIPTION.
 *
 * Author: Ceniza
 */

#include "splashscreen.h"
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/dcscreen.h>

namespace
{
  int cbSplashScreen_timer_id = wxNewId();
}

void cbSplashScreen::DoPaint(wxDC &dc)
{
  static const wxString release(wxT("svn"));
  static const wxString revision( wxT("3.0.47") );

  #ifdef __WXMSW__
  //dc.SetClippingRegion(r);
  #endif

  dc.DrawBitmap(m_label, 0, 0, false);

  wxFont largeFont(16, wxSWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
  wxFont smallFont(9, wxSWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

  wxCoord a, b, c, d;

  dc.GetTextExtent(release,  &a, &b, 0, 0, &largeFont);
  dc.GetTextExtent(revision, &c, &d, 0, 0, &smallFont);

  a >>= 1; c >>=1;
//  int y = 180 - ((b + d + 8)>>1);

  dc.SetFont(largeFont);
  //dc.DrawText(release,  92 - a, y);
  dc.SetFont(smallFont);
  //dc.DrawText(revision, 92 - c, y + b);
}

void cbSplashScreen::OnPaint(wxPaintEvent &)
{
  // an obscure statement in the wxWidgets book says to
  // allocate the DC even if you don't paint to avoid
  // a paint loop.    //pecan 2006/04/3
  wxPaintDC paint_dc(this);
  DoPaint(paint_dc);
}

void cbSplashScreen::OnEraseBackground(wxEraseEvent &event)
{
  wxDC *dc = event.GetDC();

  if (dc)
  {
    DoPaint(*dc); // why not? :)
  }
}

void cbSplashScreen::OnTimer(wxTimerEvent &)
{
  Close(true);
}

void cbSplashScreen::OnCloseWindow(wxCloseEvent &)
{
  m_timer.Stop();
  this->Destroy();
}

void cbSplashScreen::OnChar(wxKeyEvent &)
{
  Close(true);
}

void cbSplashScreen::OnMouseEvent(wxMouseEvent &event)
{
  if (event.LeftDown() || event.RightDown())
  {
    Close(true);
  }
}

cbSplashScreen::cbSplashScreen(wxBitmap &label, long timeout, wxWindow *parent, wxWindowID id, long style)
: wxFrame(parent, id, wxEmptyString, wxPoint(0, 0), wxSize(100, 100), style),
  m_timer(this, cbSplashScreen_timer_id)//, r(0, 0, 181, 181)
{
  //r.Union(50, 35, 181, 181);
  //r.Union(166, 13, 181, 181);
  //r.Union(259, 29, 181, 181);

  int w = label.GetWidth();
  int h = label.GetHeight();

  SetClientSize(w, h);
  CentreOnScreen();

  wxScreenDC screen_dc;
  wxMemoryDC label_dc;

  #ifndef __WXMAC__
    int x = GetPosition().x;
    int y = GetPosition().y;
  #endif

  m_label.Create(w, h);

  label_dc.SelectObject(m_label);
  #ifndef __WXMAC__
    label_dc.Blit(0, 0, w, h, &screen_dc, x, y);
  #else
    label_dc.SetBackground(*wxWHITE_BRUSH);
    label_dc.Clear();
  #endif
  label_dc.DrawBitmap(label, 0, 0, true);
  label_dc.SelectObject(wxNullBitmap);

  //SetShape(r);

  Show(true);
  SetThemeEnabled(false); // seems to be useful by description
  SetBackgroundStyle(wxBG_STYLE_CUSTOM); // the trick for GTK+ (notice it's after Show())

  #if ( defined( __WXMAC__ ) || defined( __WXMSW__ ) )
    Update();
  #else
    wxYieldIfNeeded();
  #endif

  if (timeout != -1)
  {
    m_timer.Start(timeout, true);
  }
}

cbSplashScreen::~cbSplashScreen()
{
  m_timer.Stop();
}

BEGIN_EVENT_TABLE(cbSplashScreen, wxFrame)
  EVT_PAINT(cbSplashScreen::OnPaint)
  EVT_TIMER(cbSplashScreen_timer_id, cbSplashScreen::OnTimer)
  EVT_ERASE_BACKGROUND(cbSplashScreen::OnEraseBackground)
  EVT_CLOSE(cbSplashScreen::OnCloseWindow)
  EVT_CHAR(cbSplashScreen::OnChar)
  EVT_MOUSE_EVENTS(cbSplashScreen::OnMouseEvent)
END_EVENT_TABLE()
