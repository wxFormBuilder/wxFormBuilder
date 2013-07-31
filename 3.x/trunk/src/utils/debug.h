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

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <wx/string.h>

#ifdef __WXFB_DEBUG__

#define LogDebug(...) \
	{									            	\
		wxString LOG_MSG; 								\
		LOG_MSG << wxT(__FILE__);						\
		LOG_MSG << wxT("@");							\
		LOG_MSG << __LINE__;							\
		LOG_MSG << wxT(" ");							\
		LOG_MSG << __FUNCTION__;						\
		LOG_MSG << wxT(": ");							\
		LOG_MSG << wxPrintf(__VA_ARGS__);				\
		wxLogDebug(LOG_MSG);							\
	}

#else

#define LogDebug(MSG)

#endif

#endif //__DEBUG_H__
