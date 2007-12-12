#ifndef __MAINGUI__
#define __MAINGUI__

#include <wx/app.h>

class MyApp : public wxApp
{
private:
  wxLog * m_old_log;
  wxLogWindow * m_log;

public:
  bool OnInit();
  #if wxUSE_ON_FATAL_EXCEPTION && wxUSE_STACKWALKER
  void OnFatalException();
  #endif
  int OnRun();
  ~MyApp();
};

DECLARE_APP(MyApp)

#endif //__MAINGUI__
