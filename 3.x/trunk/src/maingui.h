#ifndef __MAINGUI__
#define __MAINGUI__

#include "wx/wxprec.h"

class MyApp : public wxApp
{
private:
  wxLog * m_old_log;
  wxLogWindow * m_log;

public:
  virtual bool OnInit();
};

DECLARE_APP(MyApp)

#endif //__MAINGUI__
