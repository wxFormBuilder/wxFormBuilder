#ifndef __MAINGUI__
#define __MAINGUI__

#include <wx/app.h>

class MainFrame;

class MyApp : public wxApp
{
private:
  wxLog * m_old_log;
  wxLogWindow * m_log;

  MainFrame *m_frame;
  
public:
  bool OnInit();
  #if wxUSE_ON_FATAL_EXCEPTION && wxUSE_STACKWALKER
  void OnFatalException();
  #endif
  int OnRun();
  int OnExit();
  ~MyApp();
  
  #ifdef __WXMAC__
  wxString m_mac_file_name;
  void MacOpenFile(const wxString &fileName);
  #endif
};

DECLARE_APP(MyApp)

#endif //__MAINGUI__
