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
//   José Antonio Hurtado - joseantonio.hurtado@gmail.com
//   Juan Antonio Ortega  - jortegalalmolda@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

#include "wxfbevent.h"

DEFINE_EVENT_TYPE( wxEVT_FB_PROJECT_LOADED )
DEFINE_EVENT_TYPE( wxEVT_FB_PROJECT_SAVED )
DEFINE_EVENT_TYPE( wxEVT_FB_OBJECT_EXPANDED )
DEFINE_EVENT_TYPE( wxEVT_FB_OBJECT_SELECTED )
DEFINE_EVENT_TYPE( wxEVT_FB_OBJECT_CREATED )
DEFINE_EVENT_TYPE( wxEVT_FB_OBJECT_REMOVED )
DEFINE_EVENT_TYPE( wxEVT_FB_PROPERTY_MODIFIED )
DEFINE_EVENT_TYPE( wxEVT_FB_PROJECT_REFRESH )
DEFINE_EVENT_TYPE( wxEVT_FB_CODE_GENERATION )
DEFINE_EVENT_TYPE( wxEVT_FB_EVENT_HANDLER_MODIFIED )

wxFBEvent::wxFBEvent( wxEventType commandType )
:
wxEvent( 0, commandType )
{
	//ctor
}

// required for sending with wxPostEvent()
wxEvent* wxFBEvent::Clone() const
{
	return new wxFBEvent( *this );
}

wxFBEvent::wxFBEvent( const wxFBEvent& event )
:
wxEvent( event ),
m_string( event.m_string )
{
}

wxFBEvent::~wxFBEvent()
{
	//dtor
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
	CASE( wxEVT_FB_OBJECT_EXPANDED )
	CASE( wxEVT_FB_OBJECT_SELECTED )
	CASE( wxEVT_FB_OBJECT_CREATED )
	CASE( wxEVT_FB_OBJECT_REMOVED )
	CASE( wxEVT_FB_PROPERTY_MODIFIED )
	CASE( wxEVT_FB_EVENT_HANDLER_MODIFIED )
	CASE( wxEVT_FB_PROJECT_REFRESH )
	CASE( wxEVT_FB_CODE_GENERATION )

	return wxT( "Unknown Type" );
}

void wxFBEvent::SetString( const wxString& newString )
{
	m_string = newString;
}

wxString wxFBEvent::GetString()
{
	return m_string;
}

wxFBPropertyEvent::wxFBPropertyEvent(wxEventType commandType, PProperty property)
 : wxFBEvent(commandType), m_property(property)
{
}

wxFBPropertyEvent::wxFBPropertyEvent( const wxFBPropertyEvent& event )
:
wxFBEvent( event ),
m_property( event.m_property )
{
}

wxEvent* wxFBPropertyEvent::Clone() const
{
	return new wxFBPropertyEvent( *this );
}

wxFBObjectEvent::wxFBObjectEvent(wxEventType commandType, PObjectBase object)
 : wxFBEvent(commandType), m_object(object)
{
}

wxFBObjectEvent::wxFBObjectEvent( const wxFBObjectEvent& event )
:
wxFBEvent( event ),
m_object( event.m_object )
{
}

wxEvent* wxFBObjectEvent::Clone() const
{
	return new wxFBObjectEvent( *this );
}

wxFBEventHandlerEvent::wxFBEventHandlerEvent(wxEventType commandType, PEvent event)
 : wxFBEvent(commandType), m_event(event)
{
}

wxFBEventHandlerEvent::wxFBEventHandlerEvent( const wxFBEventHandlerEvent& event )
:
wxFBEvent( event ),
m_event( event.m_event )
{
}

wxEvent* wxFBEventHandlerEvent::Clone() const
{
	return new wxFBEventHandlerEvent( *this );
}
