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
//   Ryan Mulder - rjmyst3@gmail.com
//
///////////////////////////////////////////////////////////////////////////////
#ifndef WXFBDEFS_H
#define WXFBDEFS_H

#include <map>
#include <memory>
#include <vector>
#include <wx/string.h>

class ObjectBase;
class ObjectInfo;
class ObjectPackage;
class Property;
class PropertyInfo;
class OptionList;
class CodeInfo;
class EventInfo;
class Event;
class PropertyCategory;
class wxFBManager;
class CodeWriter;
class TemplateParser;
class TCCodeWriter;
class StringCodeWriter;

// Let's go with a few typedefs for frequently used types,
// please use it, code will be cleaner and easier to read.

typedef std::shared_ptr<OptionList> POptionList;
typedef std::shared_ptr<ObjectBase> PObjectBase;
typedef std::weak_ptr<ObjectBase>   WPObjectBase;
typedef std::shared_ptr<ObjectPackage>   PObjectPackage;
typedef std::weak_ptr<ObjectPackage>   WPObjectPackage;

typedef std::shared_ptr<CodeInfo>     PCodeInfo;
typedef std::shared_ptr<ObjectInfo>   PObjectInfo;
typedef std::shared_ptr<Property>     PProperty;
typedef std::shared_ptr<PropertyInfo> PPropertyInfo;
typedef std::shared_ptr<EventInfo>    PEventInfo;
typedef std::shared_ptr<Event>        PEvent;
typedef std::shared_ptr<PropertyCategory> PPropertyCategory;

typedef std::map<wxString, PPropertyInfo> PropertyInfoMap;
typedef std::map<wxString, PObjectInfo>   ObjectInfoMap;
typedef std::map<wxString, PEventInfo>    EventInfoMap;
typedef std::map<wxString, PProperty>     PropertyMap;
typedef std::map<wxString, PEvent>        EventMap;


typedef std::vector<PObjectBase> ObjectBaseVector;
typedef std::vector<PEvent>      EventVector;

typedef std::shared_ptr<wxFBManager> PwxFBManager;
typedef std::shared_ptr<CodeWriter> PCodeWriter;
typedef std::shared_ptr<TemplateParser> PTemplateParser;
typedef std::shared_ptr<TCCodeWriter> PTCCodeWriter;
typedef std::shared_ptr<StringCodeWriter> PStringCodeWriter;

// Flatnotebook styles are stored in config, if style #defines change, or config is manually modified, these style overrides still apply
#define FNB_STYLE_OVERRIDES( x ) ( x | wxFNB_CUSTOM_DLG | wxFNB_NO_X_BUTTON ) & ( ~wxFNB_X_ON_TAB & ~wxFNB_MOUSE_MIDDLE_CLOSES_TABS & ~wxFNB_DCLICK_CLOSES_TABS & ~wxFNB_ALLOW_FOREIGN_DND )

#endif //WXFBDEFS_H
