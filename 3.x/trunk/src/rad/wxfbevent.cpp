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

#include "wxfbevent.h"

DEFINE_EVENT_TYPE( wxEVT_FB_PROJECT_LOADED )
DEFINE_EVENT_TYPE( wxEVT_FB_PROJECT_SAVED )
DEFINE_EVENT_TYPE( wxEVT_FB_OBJECT_SELECTED )
DEFINE_EVENT_TYPE( wxEVT_FB_OBJECT_CREATED )
DEFINE_EVENT_TYPE( wxEVT_FB_OBJECT_REMOVED )
DEFINE_EVENT_TYPE( wxEVT_FB_PROPERTY_MODIFIED )
DEFINE_EVENT_TYPE( wxEVT_FB_PROJECT_REFRESH )
DEFINE_EVENT_TYPE( wxEVT_FB_CODE_GENERATION )

wxFBEvent::wxFBEvent( wxEventType commandType, int priority )
:
wxEvent( 0, commandType ),
m_priority( priority )
{
	//ctor
}

// required for sending with wxPostEvent()
wxEvent* wxFBEvent::Clone() const
{
	return new wxFBEvent( *this );
}

#define CASE( EVENT )									\
	if ( EVENT == m_eventType )							\
	{													\
		return wxT( #EVENT );							\
	}

wxString wxFBEvent::GetEventName()
{
	CASE( wxEVT_FB_PROJECT_LOADED )
	CASE( wxEVT_FB_PROJECT_SAVED )
	CASE( wxEVT_FB_OBJECT_SELECTED )
	CASE( wxEVT_FB_OBJECT_CREATED )
	CASE( wxEVT_FB_OBJECT_REMOVED )
	CASE( wxEVT_FB_PROPERTY_MODIFIED )
	CASE( wxEVT_FB_PROJECT_REFRESH )
	CASE( wxEVT_FB_CODE_GENERATION )

	return wxT( "Unknown Type" );
}

bool wxFBEvent::operator < ( const wxFBEvent& right ) const
{
	return this->m_priority < right.m_priority;
}

wxFBEvent::~wxFBEvent()
{
	//dtor
}

wxFBPropertyEvent::wxFBPropertyEvent(wxEventType commandType, PProperty property)
 : wxFBEvent(commandType), m_property(property)
{
}

wxFBObjectEvent::wxFBObjectEvent(wxEventType commandType, PObjectBase object)
 : wxFBEvent(commandType), m_object(object)
{
}
