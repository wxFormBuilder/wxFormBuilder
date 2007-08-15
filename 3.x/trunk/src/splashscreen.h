/* cbSplashScreen.h
 *
 * DESCRIPTION:
 *   Generic Splash Screen class with reduced interface able to draw transparent bitmaps.
 *   It was coded to be used in Code::Blocks IDE but I don't care if you use it for your own projects.
 *
 * AUTHOR:
 *   Ceniza
 */

#ifndef CBSPLASH_SCREEN_H
#define CBSPLASH_SCREEN_H

#include <wx/bitmap.h>
#include <wx/dc.h>
#include <wx/timer.h>
#include <wx/frame.h>

class cbSplashScreen : public wxFrame
{
  private:
    wxBitmap m_label;
    wxTimer m_timer;
    wxRegion r;
  public:
    // A value of -1 for timeout makes it stay forever (you need to close it manually)
    cbSplashScreen(wxBitmap &label, long timeout, wxWindow *parent, wxWindowID id, long style = wxSTAY_ON_TOP | wxNO_BORDER | wxFRAME_NO_TASKBAR | wxFRAME_SHAPED);
    ~cbSplashScreen();

  private:
    void DoPaint(wxDC &dc);
    void OnPaint(wxPaintEvent &);
    void OnEraseBackground(wxEraseEvent &);
    void OnTimer(wxTimerEvent &);
    void OnCloseWindow(wxCloseEvent &);
    void OnChar(wxKeyEvent &);
    void OnMouseEvent(wxMouseEvent &event);

  DECLARE_EVENT_TABLE()
};

#endif // CBSPLASH_SCREEN_H

