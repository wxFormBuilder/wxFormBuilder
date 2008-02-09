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
//   Ryan Mulder - rjmyst3@gmail.com
//
///////////////////////////////////////////////////////////////////////////////
#ifndef WXFBDEFS_H
#define WXFBDEFS_H

#include <boost/smart_ptr.hpp>
#include <map>
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

typedef boost::shared_ptr<OptionList> POptionList;
typedef boost::shared_ptr<ObjectBase> PObjectBase;
typedef boost::weak_ptr<ObjectBase>   WPObjectBase;
typedef boost::shared_ptr<ObjectPackage>   PObjectPackage;
typedef boost::weak_ptr<ObjectPackage>   WPObjectPackage;

typedef boost::shared_ptr<CodeInfo>     PCodeInfo;
typedef boost::shared_ptr<ObjectInfo>   PObjectInfo;
typedef boost::shared_ptr<Property>     PProperty;
typedef boost::shared_ptr<PropertyInfo> PPropertyInfo;
typedef boost::shared_ptr<EventInfo>    PEventInfo;
typedef boost::shared_ptr<Event>        PEvent;
typedef boost::shared_ptr<PropertyCategory> PPropertyCategory;

typedef std::map<wxString, PPropertyInfo> PropertyInfoMap;
typedef std::map<wxString, PObjectInfo>   ObjectInfoMap;
typedef std::map<wxString, PEventInfo>    EventInfoMap;
typedef std::map<wxString, PProperty>     PropertyMap;
typedef std::map<wxString, PEvent>        EventMap;


typedef std::vector<PObjectBase> ObjectBaseVector;
typedef std::vector<PEvent>      EventVector;

typedef boost::shared_ptr<wxFBManager> PwxFBManager;
typedef boost::shared_ptr<CodeWriter> PCodeWriter;
typedef boost::shared_ptr<TemplateParser> PTemplateParser;
typedef boost::shared_ptr<TCCodeWriter> PTCCodeWriter;
typedef boost::shared_ptr<StringCodeWriter> PStringCodeWriter;

#endif //WXFBDEFS_H
