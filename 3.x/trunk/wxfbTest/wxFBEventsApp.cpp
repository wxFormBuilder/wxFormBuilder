/***************************************************************
 * Name:      wxFBEventsApp.cpp
 * Purpose:   Code for Application Class
 * Author:    RJP Computing (rjpcomputing@gmail.com)
 * Created:   2007-02-22
 * Copyright: RJP Computing (http://rjpcomputing.com)
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "wxFBEventsApp.h"
#include "wxFBEventMainFrame.h"

IMPLEMENT_APP( wxFBEventsApp );

bool wxFBEventsApp::OnInit()
{
	::wxInitAllImageHandlers();

	wxFBEventMainFrame* frame = new wxFBEventMainFrame( 0L );
	frame->SetIcon( wxICON( aaaa ) ); // To Set App Icon
	frame->Show();

	return true;
}
