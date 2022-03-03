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
//   RJP Computing - rjpcomputing@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

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
	//::wxInitAllImageHandlers();

	wxFBEventMainFrame* frame = new wxFBEventMainFrame( 0L );
//	frame->SetIcon( wxICON( aaaa ) ); // To Set App Icon
	frame->Show();

	return true;
}
