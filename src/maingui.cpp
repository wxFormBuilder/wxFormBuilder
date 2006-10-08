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
//   José Antonio Hurtado - joseantonio.hurtado@gmail.com
//   Juan Antonio Ortega  - jortegalalmolda@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include <wx/splash.h>
#include "rad/mainframe.h"
#include "rad/appdata.h"
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/sysopt.h>

#include <memory>
#include "maingui.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
  wxInitAllImageHandlers();

  // Obtenemos la ruta del ejecutable
  wxString exeFile(argv[0]);
  wxFileName appFileName(exeFile);
  wxString path = appFileName.GetPath();

  AppDataInit( path );

  // Guardamos la ruta del ejecutable
  AppData()->SetApplicationPath(path);

  wxSystemOptions::SetOption(wxT("msw.remap"), 0);
  wxSystemOptions::SetOption(wxT("msw.staticbox.optimized-paint"), 0);

#ifndef _DEBUG
  wxBitmap bitmap;
  std::auto_ptr< wxSplashScreen > splash;
  if (bitmap.LoadFile(path + wxFILE_SEP_PATH + wxT("resources") + wxFILE_SEP_PATH + wxT("splash.png"), wxBITMAP_TYPE_PNG))
  {
	splash = std::auto_ptr< wxSplashScreen >( new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
          3000, NULL, -1, wxDefaultPosition, wxDefaultSize,
          wxSIMPLE_BORDER|wxSTAY_ON_TOP) );
  }
#endif

  wxApp::SetVendorName( wxT(" wxFormBuilder") );
  wxApp::SetAppName( wxT(" wxFormBuilder") );

  wxYield();

  #ifdef __WXFB_DEBUG__
  m_log = new wxLogWindow(NULL,wxT("Logging"));
  m_old_log = wxLog::SetActiveTarget(m_log);
  #endif //__WXFB_DEBUG__



  MainFrame *frame = new MainFrame(NULL);
  frame->Show(TRUE);
  SetTopWindow(frame);

  #ifdef __WXFB_DEBUG__
  frame->AddChild(m_log->GetFrame());
  #endif //__WXFB_DEBUG__

  if (argc > 1)
  {
    wxString arg(argv[1]);

    if (::wxFileExists(arg))
    {
      // No va bien (en mainframe aparece untitled)
      if (AppData()->LoadProject(arg))
        frame->InsertRecentProject(arg);

      return TRUE;
    }
  }


  AppData()->NewProject();
  return TRUE;
}

// OnQuit?? -> AppDataDestroy()
