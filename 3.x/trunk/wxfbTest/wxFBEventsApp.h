/***************************************************************
 * Name:      wxFBEventsApp.h
 * Purpose:   Defines Application Class
 * Author:    RJP Computing (rjpcomputing@gmail.com)
 * Created:   2007-02-22
 * Copyright: RJP Computing (http://rjpcomputing.com)
 * License:   
 **************************************************************/
 
#ifndef WXFBEVENTSAPP_H
#define WXFBEVENTSAPP_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class wxFBEventsApp : public wxApp
{
	public:
		virtual bool OnInit();
};

#endif // wxFBEventsAPP_H
