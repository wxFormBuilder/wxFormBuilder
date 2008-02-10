/* cbSplashScreen.h
 *
 * DESCRIPTION:
 *   Generic Splash Screen class with reduced interface able to draw transparent bitmaps.
 *   It was coded to be used in Code::Blocks IDE but I don't care if you use it for your own projects.
 *
 * AUTHOR:
 *   Ceniza
 *
 * Copyright (C) 2006 Ceniza
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
#ifdef __WXMAC__
    cbSplashScreen(wxBitmap &label, long timeout, wxWindow *parent, wxWindowID id, long style = wxSTAY_ON_TOP | wxNO_BORDER | wxFRAME_TOOL_WINDOW);
#else
    cbSplashScreen(wxBitmap &label, long timeout, wxWindow *parent, wxWindowID id, long style = wxSTAY_ON_TOP | wxNO_BORDER | wxFRAME_NO_TASKBAR | wxFRAME_SHAPED);
#endif
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

