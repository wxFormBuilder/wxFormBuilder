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

#ifndef __WXFBEVENT__
#define __WXFBEVENT__

#include "../utils/wxfbdefs.h"

#include <wx/event.h>

class wxFBEvent : public wxEvent
{
	private:
		wxString m_string;

	public:
		wxFBEvent( wxEventType commandType = wxEVT_NULL );
		wxFBEvent( const wxFBEvent& event );
	~wxFBEvent() override;

		wxString GetEventName();

		void SetString( const wxString& newString );
		wxString GetString();

		// required for sending with wxPostEvent()
	wxEvent* Clone() const override;
};

class wxFBPropertyEvent : public wxFBEvent
{
public:
  wxFBPropertyEvent(wxEventType commandType, PProperty property);
  wxFBPropertyEvent( const wxFBPropertyEvent& event );
	wxEvent* Clone() const override;
  PProperty GetFBProperty() { return m_property; }
private:
  PProperty m_property;
};

class wxFBEventHandlerEvent : public wxFBEvent
{
public:
  wxFBEventHandlerEvent (wxEventType commandType, PEvent event);
  wxFBEventHandlerEvent( const wxFBEventHandlerEvent& event );
	wxEvent* Clone() const override;
  PEvent GetFBEventHandler() { return m_event; }
private:
  PEvent m_event;
};

class wxFBObjectEvent : public wxFBEvent
{
public:
  wxFBObjectEvent(wxEventType commandType, PObjectBase object);
  wxFBObjectEvent( const wxFBObjectEvent& event );
	wxEvent* Clone() const override;
  PObjectBase GetFBObject() { return m_object; }

private:
  PObjectBase m_object;
};


typedef void (wxEvtHandler::*wxFBEventFunction)(wxFBEvent&);
typedef void (wxEvtHandler::*wxFBPropertyEventFunction)(wxFBPropertyEvent&);
typedef void (wxEvtHandler::*wxFBObjectEventFunction)(wxFBObjectEvent&);
typedef void (wxEvtHandler::*wxFBEventHandlerEventFunction)(wxFBEventHandlerEvent&);

#define wxFBEventHandler(fn) wxEVENT_HANDLER_CAST(wxFBEventFunction, fn)
#define wxFBPropertyEventHandler(fn) wxEVENT_HANDLER_CAST(wxFBPropertyEventFunction, fn)
#define wxFBObjectEventHandler(fn) wxEVENT_HANDLER_CAST(wxFBObjectEventFunction, fn)
#define wxFBEventHandlerEventHandler(fn) wxEVENT_HANDLER_CAST(wxFBEventHandlerEventFunction, fn)

wxDECLARE_EVENT(wxEVT_FB_PROJECT_LOADED, wxFBEvent);
wxDECLARE_EVENT(wxEVT_FB_PROJECT_SAVED, wxFBEvent);
wxDECLARE_EVENT(wxEVT_FB_OBJECT_EXPANDED, wxFBObjectEvent);
wxDECLARE_EVENT(wxEVT_FB_OBJECT_SELECTED, wxFBObjectEvent);
wxDECLARE_EVENT(wxEVT_FB_OBJECT_CREATED, wxFBObjectEvent);
wxDECLARE_EVENT(wxEVT_FB_OBJECT_REMOVED, wxFBObjectEvent);
wxDECLARE_EVENT(wxEVT_FB_PROPERTY_MODIFIED, wxFBPropertyEvent);
wxDECLARE_EVENT(wxEVT_FB_PROJECT_REFRESH, wxFBEvent);
wxDECLARE_EVENT(wxEVT_FB_CODE_GENERATION, wxFBEvent);
wxDECLARE_EVENT(wxEVT_FB_EVENT_HANDLER_MODIFIED, wxFBEventHandlerEvent);

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
  wx__DECLARE_EVT0(wxEVT_FB_EVENT_HANDLER_MODIFIED,wxFBEventHandlerEventHandler(fn))

#define EVT_FB_PROJECT_REFRESH(fn) \
  wx__DECLARE_EVT0(wxEVT_FB_PROJECT_REFRESH,wxFBEventHandler(fn))

#define EVT_FB_CODE_GENERATION(fn) \
    wx__DECLARE_EVT0(wxEVT_FB_CODE_GENERATION,wxFBEventHandler(fn))

#endif // __WXFBEVENT__
