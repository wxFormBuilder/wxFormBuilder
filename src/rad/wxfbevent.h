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

#ifndef __WXFBEVENT__
#define __WXFBEVENT__

#include <wx/event.h>
#include "model/objectbase.h"

class wxFBEvent : public wxEvent
{
	public:
		wxFBEvent( wxEventType commandType = wxEVT_NULL );
		virtual ~wxFBEvent();

		wxString GetEventName();

		// required for sending with wxPostEvent()
		wxEvent* Clone() const;
};

class wxFBPropertyEvent : public wxFBEvent
{
public:
  wxFBPropertyEvent(wxEventType commandType, PProperty property);
  wxFBPropertyEvent( const wxFBPropertyEvent& event );
  wxEvent* Clone() const;
  PProperty GetFBProperty() { return m_property; }
private:
  PProperty m_property;
};

class wxFBEventHandlerEvent : public wxFBEvent
{
public:
  wxFBEventHandlerEvent (wxEventType commandType, PEvent event);
  wxFBEventHandlerEvent( const wxFBEventHandlerEvent& event );
  wxEvent* Clone() const;
  PEvent GetFBEventHandler() { return m_event; }
private:
  PEvent m_event;
};

class wxFBObjectEvent : public wxFBEvent
{
public:
  wxFBObjectEvent(wxEventType commandType, PObjectBase object);
  wxFBObjectEvent( const wxFBObjectEvent& event );
  wxEvent* Clone() const;
  PObjectBase GetFBObject() { return m_object; }

private:
  PObjectBase m_object;
};


typedef void (wxEvtHandler::*wxFBEventFunction)        (wxFBEvent&);
typedef void (wxEvtHandler::*wxFBPropertyEventFunction)(wxFBPropertyEvent&);
typedef void (wxEvtHandler::*wxFBObjectEventFunction)  (wxFBObjectEvent&);
typedef void (wxEvtHandler::*wxFBEventHandlerEventFunction)  (wxFBEventHandlerEvent&);

#define wxFBEventHandler(fn) \
  (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxFBEventFunction, &fn)

#define wxFBPropertyEventHandler(fn) \
  (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxFBPropertyEventFunction, &fn)

#define wxFBObjectEventHandler(fn) \
  (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxFBObjectEventFunction, &fn)

#define wxFBEventEventHandler(fn) \
  (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxFBEventHandlerEventFunction, &fn)


BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_LOCAL_EVENT_TYPE( wxEVT_FB_PROJECT_LOADED,    -1 )
  DECLARE_LOCAL_EVENT_TYPE( wxEVT_FB_PROJECT_SAVED,     -1 )
  DECLARE_LOCAL_EVENT_TYPE( wxEVT_FB_OBJECT_EXPANDED,   -1 )
  DECLARE_LOCAL_EVENT_TYPE( wxEVT_FB_OBJECT_SELECTED,   -1 )
  DECLARE_LOCAL_EVENT_TYPE( wxEVT_FB_OBJECT_CREATED,    -1 )
  DECLARE_LOCAL_EVENT_TYPE( wxEVT_FB_OBJECT_REMOVED,    -1 )
  DECLARE_LOCAL_EVENT_TYPE( wxEVT_FB_PROPERTY_MODIFIED, -1 )
  DECLARE_LOCAL_EVENT_TYPE( wxEVT_FB_PROJECT_REFRESH,   -1 )
  DECLARE_LOCAL_EVENT_TYPE( wxEVT_FB_CODE_GENERATION,   -1 )
  DECLARE_LOCAL_EVENT_TYPE( wxEVT_FB_EVENT_HANDLER_MODIFIED, -1 )
END_DECLARE_EVENT_TYPES()

#define EVT_FB_PROJECT_LOADED(fn) \
  wx__DECLARE_EVT0(wxEVT_FB_PROJECT_LOADED,wxFBEventHandler(fn))

#define EVT_FB_PROJECT_SAVED(fn) \
  wx__DECLARE_EVT0(wxEVT_FB_PROJECT_SAVED,wxFBEventHandler(fn))

#define EVT_FB_OBJECT_EXPANDED(fn) \
  wx__DECLARE_EVT0(wxEVT_FB_OBJECT_EXPANDED,wxFBObjectEventHandler(fn))

#define EVT_FB_OBJECT_SELECTED(fn) \
  wx__DECLARE_EVT0(wxEVT_FB_OBJECT_SELECTED,wxFBObjectEventHandler(fn))

#define EVT_FB_OBJECT_CREATED(fn) \
  wx__DECLARE_EVT0(wxEVT_FB_OBJECT_CREATED,wxFBObjectEventHandler(fn))

#define EVT_FB_OBJECT_REMOVED(fn) \
  wx__DECLARE_EVT0(wxEVT_FB_OBJECT_REMOVED,wxFBObjectEventHandler(fn))

#define EVT_FB_PROPERTY_MODIFIED(fn) \
  wx__DECLARE_EVT0(wxEVT_FB_PROPERTY_MODIFIED,wxFBPropertyEventHandler(fn))

#define EVT_FB_EVENT_HANDLER_MODIFIED(fn) \
  wx__DECLARE_EVT0(wxEVT_FB_EVENT_HANDLER_MODIFIED,wxFBEventEventHandler(fn))

#define EVT_FB_PROJECT_REFRESH(fn) \
  wx__DECLARE_EVT0(wxEVT_FB_PROJECT_REFRESH,wxFBEventHandler(fn))

#define EVT_FB_CODE_GENERATION(fn) \
    wx__DECLARE_EVT0(wxEVT_FB_CODE_GENERATION,wxFBEventHandler(fn))

#endif // __WXFBEVENT__
