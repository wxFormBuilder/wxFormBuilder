////////////////////////////////////////////////////////////////////////////////
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

#include "cppcg.h"

#include <algorithm>

#include <wx/filename.h>
#include <wx/tokenzr.h>

#include "codegen/codewriter.h"
#include "model/objectbase.h"
#include "rad/appdata.h"
#include "rad/revision.h"
#include "rad/version.h"
#include "utils/filetocarray.h"
#include "utils/typeconv.h"
#include "utils/wxfbexception.h"


CppTemplateParser::CppTemplateParser(
  PObjectBase obj, const wxString& _template, bool useI18N, bool useRelativePath, const wxString& basePath) :
    TemplateParser(obj, _template), m_i18n(useI18N), m_useRelativePath(useRelativePath), m_basePath(basePath)
{
    if (!wxFileName::DirExists(m_basePath)) {
        m_basePath.clear();
    }
}

CppTemplateParser::CppTemplateParser(const CppTemplateParser& that, const wxString& _template) :
    TemplateParser(that, _template),
    m_i18n(that.m_i18n),
    m_useRelativePath(that.m_useRelativePath),
    m_basePath(that.m_basePath)
{
}


PTemplateParser CppTemplateParser::CreateParser(const TemplateParser* oldparser, const wxString& _template) const
{
    const CppTemplateParser* cppOldParser = dynamic_cast<const CppTemplateParser*>(oldparser);
    if (cppOldParser) {
        PTemplateParser newparser(new CppTemplateParser(*cppOldParser, _template));
        return newparser;
    }
    return PTemplateParser();
}


wxString CppTemplateParser::RootWxParentToCode() const
{
    return wxT("this");
}

/**
 * Convert the value of the property to C++ code
 */
wxString CppTemplateParser::ValueToCode(PropertyType type, const wxString& value) const
{
    wxString result;

    switch (type) {
        case PT_WXPARENT: {
            result = value;
            break;
        }
        case PT_WXPARENT_SB: {
            result = value + wxT("->GetStaticBox()");
            break;
        }
        case PT_WXPARENT_CP: {
            result = value + wxT("->GetPane()");
            break;
        }
        case PT_WXSTRING:
        case PT_FILE:
        case PT_PATH: {
            if (value.empty()) {
                result << wxT("wxEmptyString");
            } else {
                result << wxT("wxT(\"") << CppCodeGenerator::ConvertCppString(value) << wxT("\")");
            }
            break;
        }
        case PT_WXSTRING_I18N: {
            if (value.empty()) {
                result << wxT("wxEmptyString");
            } else {
                if (m_i18n) {
                    result << wxT("_(\"") << CppCodeGenerator::ConvertCppString(value) << wxT("\")");
                } else {
                    result << wxT("wxT(\"") << CppCodeGenerator::ConvertCppString(value) << wxT("\")");
                }
            }
            break;
        }
        case PT_CLASS:
        case PT_MACRO:
        case PT_TEXT:
        case PT_TEXT_ML:
        case PT_OPTION:
        case PT_EDIT_OPTION:
        case PT_FLOAT:
        case PT_INT:
        case PT_UINT: {
            result = value;
            break;
        }
        case PT_BITLIST: {
            result = (value.empty() ? wxT("0") : value);
            break;
        }
        case PT_WXPOINT: {
            if (value.empty()) {
                result = wxT("wxDefaultPosition");
            } else {
                result << wxT("wxPoint( ") << value << wxT(" )");
            }
            break;
        }
        case PT_WXSIZE: {
            if (value.empty()) {
                result = wxT("wxDefaultSize");
            } else {
                result << wxT("wxSize( ") << value << wxT(" )");
            }
            break;
        }
        case PT_BOOL: {
            result = (value == wxT("0") ? wxT("false") : wxT("true"));
            break;
        }
        case PT_WXFONT: {
            if (!value.empty()) {
                wxFontContainer fontContainer = TypeConv::StringToFont(value);
                wxFont font = fontContainer.GetFont();

                const int pointSize = fontContainer.GetPointSize();

                result = wxString::Format(
                  "wxFont( %s, %s, %s, %s, %s, %s )",
                  ((pointSize <= 0) ? "wxNORMAL_FONT->GetPointSize()" : (wxString() << pointSize)),
                  TypeConv::FontFamilyToString(fontContainer.GetFamily()), font.GetStyleString(),
                  font.GetWeightString(), (fontContainer.GetUnderlined() ? "true" : "false"),
                  (fontContainer.GetFaceName().empty() ? "wxEmptyString"
                                                       : ("wxT(\"" + fontContainer.GetFaceName() + "\")")));
            } else {
                result = wxT("*wxNORMAL_FONT");
            }
            break;
        }
        case PT_WXCOLOUR: {
            if (!value.empty()) {
                if (value.find_first_of(wxT("wx")) == 0) {
                    // System Colour
                    result << wxT("wxSystemSettings::GetColour( ") << value << wxT(" )");
                } else {
                    wxColour colour = TypeConv::StringToColour(value);
                    result =
                      wxString::Format(wxT("wxColour( %i, %i, %i )"), colour.Red(), colour.Green(), colour.Blue());
                }
            } else {
                result = wxT("wxColour()");
            }
            break;
        }
        case PT_BITMAP: {
            wxString path;
            wxString source;
            wxSize icoSize;
            TypeConv::ParseBitmapWithResource(value, &path, &source, &icoSize);

            if (path.empty()) {
                // Empty path, generate Null Bitmap
                result = wxT("wxNullBitmap");
                break;
            }

            if (path.StartsWith(wxT("file:"))) {
                wxLogWarning(wxT("C++ code generation does not support using URLs for bitmap properties:\n%s"), path);
                result = wxT("wxNullBitmap");
                break;
            }

            if (source == _("Load From File")) {
                wxString absPath;
                try {
                    absPath = TypeConv::MakeAbsolutePath(path, AppData()->GetProjectPath());
                } catch (wxFBException& ex) {
                    wxLogError(ex.what());
                    result = wxT("wxNullBitmap");
                    break;
                }

                wxString file = (m_useRelativePath ? TypeConv::MakeRelativePath(absPath, m_basePath) : absPath);

                wxFileName bmpFileName(path);
                if (bmpFileName.GetExt().Upper() == wxT("XPM")) {
                    // If the bitmap is an XPM we will embed it in the code, otherwise it will be loaded from the file
                    // at run time.
                    result << wxT("wxBitmap( ") << CppCodeGenerator::ConvertEmbeddedBitmapName(path) << wxT(" )");
                } else {
                    result << wxT("wxBitmap( wxT(\"") << CppCodeGenerator::ConvertCppString(file)
                           << wxT("\"), wxBITMAP_TYPE_ANY )");
                }
            } else if (source == _("Load From Embedded File")) {
                wxString absPath;
                try {
                    absPath = TypeConv::MakeAbsolutePath(path, AppData()->GetProjectPath());
                } catch (wxFBException& ex) {
                    wxLogError(ex.what());
                    result = wxT("wxNullBitmap");
                    break;
                }

                wxFileName bmpFileName(path);
                if (bmpFileName.GetExt().Upper() == wxT("XPM")) {
                    // If the bitmap is an XPM we will embed it as is, otherwise we'll generate a header
                    result << wxT("wxBitmap( ") << CppCodeGenerator::ConvertEmbeddedBitmapName(path) << wxT(" )");
                } else {
                    result << CppCodeGenerator::ConvertEmbeddedBitmapName(path) << wxT("_to_wx_bitmap()");
                }
            } else if (source == _("Load From Resource")) {
                result << wxT("wxBitmap( wxT(\"") << path << wxT("\"), wxBITMAP_TYPE_RESOURCE )");
            } else if (source == _("Load From Icon Resource")) {
                if (wxDefaultSize == icoSize) {
                    result << wxT("wxICON( ") << path << wxT(" )");
                } else {
                    result.Printf(
                      wxT("wxIcon( wxT(\"%s\"), wxBITMAP_TYPE_ICO_RESOURCE, %i, %i )"), path, icoSize.GetWidth(),
                      icoSize.GetHeight());
                }
            } else if (source == _("Load From XRC")) {
                result << wxT("wxXmlResource::Get()->LoadBitmap( wxT(\"") << path << wxT("\") )");
            } else if (source == _("Load From Art Provider")) {
                wxString rid = path.BeforeFirst(wxT(':'));
                if (rid.StartsWith(wxT("gtk-")))
                    rid = wxT("wxT(\"") + rid + wxT("\")");

                result = wxT("wxArtProvider::GetBitmap( wxASCII_STR(") + rid + wxT("), wxASCII_STR(") +
                         path.AfterFirst(wxT(':')) + wxT(") )");
            } else if (source == _("Load From SVG Resource")) {
                result.Printf(
                  wxT("wxBitmapBundle::FromSVGResource( wxT(\"%s\"), {%i, %i} )"), path, icoSize.GetWidth(),
                  icoSize.GetHeight());
            }
            break;
        }
        case PT_STRINGLIST: {
            // Stringlists are generated like a sequence of wxString separated by ', '.
            wxArrayString array = TypeConv::StringToArrayString(value);
            if (array.Count() > 0) {
                result = ValueToCode(PT_WXSTRING_I18N, array[0]);
            }

            for (size_t i = 1; i < array.Count(); i++) {
                result << wxT(", ") << ValueToCode(PT_WXSTRING_I18N, array[i]);
            }
            break;
        }
        default:
            break;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////

CppCodeGenerator::CppCodeGenerator()
{
    SetupPredefinedMacros();
}

wxString CppCodeGenerator::ConvertCppString(wxString text)
{
    wxString result;

    for (size_t i = 0; i < text.length(); i++) {
        wxChar c = text[i];

        switch (c) {
            case wxT('"'):
                result += wxT("\\\"");
                break;

            case wxT('\\'):
                result += wxT("\\\\");
                break;

            case wxT('\t'):
                result += wxT("\\t");
                break;

            case wxT('\n'):
                result += wxT("\\n");
                break;

            case wxT('\r'):
                result += wxT("\\r");
                break;

            default:
                result += c;
                break;
        }
    }
    return result;
}

wxString CppCodeGenerator::ConvertEmbeddedBitmapName(const wxString& text)
{
    wxString name = text;
    // the name consists of extracting the name of the file (without the directory)
    // and replacing the character '.' by ' _ '.

    size_t last_slash = name.find_last_of(wxT("\\/"));
    if (last_slash != name.npos) {
        name = name.substr(last_slash + 1);
    }

    name.Replace(wxT("."), wxT("_"));

    return name;
}

void CppCodeGenerator::GenerateInheritedClass(PObjectBase userClasses, PObjectBase form)
{
    if (!userClasses) {
        wxLogError(wxT("There is no object to generate inherited class"));
        return;
    }

    if (wxT("UserClasses") != userClasses->GetClassName()) {
        wxLogError(wxT("This not a UserClasses object"));
        return;
    }

    m_inheritedCodeParser.ParseCFiles(userClasses->GetPropertyAsString(_("name")));

    //(FileCodeWriter*)m_header->
    wxString type = userClasses->GetPropertyAsString(wxT("type"));
    wxString userCode;

    // Start header file
    wxString code = GetCode(userClasses, wxT("guard_macro_open"));
    m_header->WriteLn(code);
    m_header->WriteLn(wxEmptyString);

    code = GetCode(userClasses, wxT("header_comment"));
    m_header->WriteLn(code);
    m_header->WriteLn(wxEmptyString);

    code = GetCode(userClasses, wxT("header_include"));
    m_header->WriteLn(code);
    m_header->WriteLn(wxEmptyString);
    m_header->WriteLn(wxT("//// end generated include"));

    m_header->WriteLn(m_inheritedCodeParser.GetUserIncludes());
    if (!userCode.IsEmpty()) {
        m_header->WriteLn(userCode);
    }

    code = GetCode(userClasses, wxT("class_decl"));
    m_header->WriteLn(code);
    m_header->WriteLn(wxT("{"));
    m_header->Indent();

    // Start source file
    code = GetCode(userClasses, wxT("source_include"));
    m_source->WriteLn(code);
    m_source->WriteLn(wxEmptyString);

    code = GetCode(userClasses, type + wxT("_cons_def"));
    LogDebug(code + "<");
    m_source->WriteLn(code);
    m_source->WriteLn(wxT("{"));
    userCode = m_inheritedCodeParser.GetFunctionContents(code);
    if (!userCode.IsEmpty()) {
        m_source->WriteLn(userCode, true);
    } else {
        m_source->WriteLn(wxEmptyString);
    }
    m_source->WriteLn(wxT("}"));

    // Do events in both files
    EventVector events;
    FindEventHandlers(form, events);

    if (events.size() > 0) {
        m_header->WriteLn(wxT("protected:"));
        m_header->Indent();
        code = GetCode(userClasses, wxT("event_handler_comment"));
        m_header->WriteLn(code);

        /** get the event handler kind */
        wxString eventHandlerKind;
        PProperty eventHandlerKindProp = form->GetProperty(wxT("event_handler"));
        if (eventHandlerKindProp) {
            eventHandlerKind = eventHandlerKindProp->GetValueAsString();
        }
        bool is_virtual = true;
        if (0 == eventHandlerKind.compare(wxT("decl"))) {
            is_virtual = false;
        }

        wxString className = userClasses->GetPropertyAsString(_("name"));
        std::set<wxString> generatedHandlers;
        for (size_t i = 0; i < events.size(); i++) {
            PEvent event = events[i];
            wxString prototype;
            if (generatedHandlers.find(event->GetValue()) == generatedHandlers.end()) {
                prototype = wxString::Format(
                  wxT("%s( %s& event )"), event->GetValue(), event->GetEventInfo()->GetEventClassName());
                /** If it is virtual inherited, add keyword virtual + override */
                if (is_virtual) {
                    m_header->WriteLn(wxString::Format(wxT("virtual void %s override;"), prototype));
                } else {
                    m_header->WriteLn(wxString::Format(wxT("void %s;"), prototype));
                }
                userCode = m_inheritedCodeParser.GetFunctionDocumentation(event->GetValue());
                if (!userCode.IsEmpty()) {
                    m_source->Write(userCode);
                } else {
                    m_source->WriteLn();
                }
                m_source->WriteLn(wxString::Format(wxT("void %s::%s"), className, prototype));
                m_source->WriteLn(wxT("{"));
                userCode =
                  m_inheritedCodeParser.GetFunctionContents(wxString::Format(wxT("void %s::%s"), className, prototype));
                LogDebug(wxString::Format(wxT("void %s::%s"), className, prototype) + "<");
                if (!userCode.IsEmpty()) {
                    m_source->WriteLn(userCode);
                } else {
                    m_source->Indent();
                    m_source->WriteLn(wxString::Format(wxT("// TODO: Implement %s"), event->GetValue()), true);
                    m_source->Unindent();
                }
                m_source->WriteLn(wxT("}"));
                generatedHandlers.insert(event->GetValue());
            }
        }

        m_header->Unindent();
    }

    // Finish header file

    m_header->WriteLn(wxT("public:"));

    m_header->Indent();
    code = GetCode(userClasses, type + wxT("_cons_decl"));
    m_header->WriteLn(code);
    m_header->Unindent();
    m_header->WriteLn(wxT("//// end generated class members"));
    userCode = m_inheritedCodeParser.GetUserMembers();
    if (!userCode.IsEmpty()) {
        m_header->WriteLn(userCode, true);
    } else {
        m_header->WriteLn(wxEmptyString);
    }

    m_header->Unindent();

    m_header->WriteLn(wxT("};"));
    m_header->WriteLn(wxEmptyString);
    code = GetCode(userClasses, wxT("guard_macro_close"));
    m_header->WriteLn(code);

    userCode = m_inheritedCodeParser.GetRemainingFunctions();
    if (!userCode.IsEmpty()) {
        m_source->Write(userCode);
    }
    userCode = m_inheritedCodeParser.GetTrailingCode();
    if (!userCode.IsEmpty()) {
        m_source->WriteLn(userCode, true);
    }
}

bool CppCodeGenerator::GenerateCode(PObjectBase project)
{
    if (!project) {
        wxLogError(wxT("There is no project to generate code"));
        return false;
    }

    bool useEnum = false;

    PProperty useEnumProperty = project->GetProperty("cpp_use_enum");
    if (useEnumProperty && useEnumProperty->GetValueAsInteger())
        useEnum = true;

    m_useArrayEnum = false;
    const auto& useArrayEnumProperty = project->GetProperty("cpp_use_array_enum");
    if (useArrayEnumProperty && useArrayEnumProperty->GetValueAsInteger()) {
        m_useArrayEnum = true;
    }

    m_i18n = false;
    PProperty i18nProperty = project->GetProperty(wxT("internationalize"));
    if (i18nProperty && i18nProperty->GetValueAsInteger())
        m_i18n = true;

    m_useConnect = !(_("table") == project->GetPropertyAsString("cpp_event_generation"));
    m_disconnectEvents = (project->GetPropertyAsInteger("cpp_disconnect_events") != 0);

    m_header->Clear();
    m_source->Clear();
    wxString code = wxString::Format(
      wxT("///////////////////////////////////////////////////////////////////////////\n")
        wxT("// C++ code generated with wxFormBuilder (version %s%s)\n") wxT("// http://www.wxformbuilder.org/\n")
          wxT("//\n") wxT("// PLEASE DO *NOT* EDIT THIS FILE!\n")
            wxT("///////////////////////////////////////////////////////////////////////////\n"),
      getVersion(), getPostfixRevision(getVersion()).c_str());

    m_header->WriteLn(code);
    m_source->WriteLn(code);

    PProperty propFile = project->GetProperty(wxT("file"));
    if (!propFile) {
        wxLogError(wxT("Missing \"file\" property on Project Object"));
        return false;
    }

    wxString file = propFile->GetValue();
    if (file.empty()) {
        file = wxT("noname");
    }

    // Generate the subclass sets
    std::set<wxString> subclasses;
    std::set<wxString> subclassSourceIncludes;
    std::vector<wxString> headerIncludes;

    GenSubclassSets(project, &subclasses, &subclassSourceIncludes, &headerIncludes);

    wxString base_name = file;
    if (project->GetChildCount() == 1) {
        PObjectBase base_obj = project->GetChild(0);
        if (base_obj && (!base_obj->IsPropertyNull("name"))) {
            base_name = base_obj->GetPropertyAsString(wxT("name"));
        }
    }
    m_header->WriteLn(wxT("#ifndef __WX_FORM_BUILDER_GEN_") + base_name.Upper() + wxT("_H__"));
    m_header->WriteLn(wxT("#define __WX_FORM_BUILDER_GEN_") + base_name.Upper() + wxT("_H__"));

    m_header->WriteLn(wxT("#pragma once"));
    m_header->WriteLn(wxEmptyString);

    code = GetCode(project, wxT("header_preamble"));
    if (!code.empty()) {
        m_header->WriteLn(code);
    }

    // Write the forward declaration lines
    std::set<wxString>::iterator subclass_it;
    for (subclass_it = subclasses.begin(); subclass_it != subclasses.end(); ++subclass_it) {
        m_header->WriteLn(*subclass_it);
    }
    if (!subclasses.empty()) {
        m_header->WriteLn(wxEmptyString);
    }

    // Generating in the .h header file those include from components dependencies.
    std::set<wxString> templates;
    GenIncludes(project, &headerIncludes, &templates);

    // Write the include lines
    std::vector<wxString>::iterator include_it;
    for (include_it = headerIncludes.begin(); include_it != headerIncludes.end(); ++include_it) {
        m_header->WriteLn(*include_it);
    }
    if (!headerIncludes.empty()) {
        m_header->WriteLn(wxEmptyString);
    }

    // class decoration
    PProperty propClassDecoration = project->GetProperty("cpp_class_decoration");
    wxString classDecoration;
    if (propClassDecoration) {
        // get the decoration to be used by GenClassDeclaration
        std::map<wxString, wxString> children;
        propClassDecoration->SplitParentProperty(&children);

        std::map<wxString, wxString>::iterator decoration;
        decoration = children.find(wxT("decoration"));

        if (decoration != children.end()) {
            classDecoration = decoration->second;
            if (!classDecoration.empty()) {
                classDecoration += wxT(" ");
            }
        }

        // Now get the header
        std::map<wxString, wxString>::iterator header;
        header = children.find(wxT("header"));

        if (header != children.end()) {
            wxString headerVal = header->second;
            if (!headerVal.empty()) {
                wxString include = wxT("#include \"") + headerVal + wxT("\"");
                std::vector<wxString>::iterator findInclude =
                  std::find(headerIncludes.begin(), headerIncludes.end(), include);
                if (findInclude == headerIncludes.end()) {
                    m_header->WriteLn(include);
                    m_header->WriteLn(wxEmptyString);
                }
            }
        }
    }

    code = GetCode(project, wxT("header_epilogue"));
    m_header->WriteLn(code);
    m_header->WriteLn(wxEmptyString);

    // Inserting in the .cpp source file the include corresponding to the
    // generated .h header file and the related xpm includes
    code = GetCode(project, wxT("cpp_preamble"));
    if (!code.empty()) {
        m_source->WriteLn(code);
        m_source->WriteLn(wxEmptyString);
    }

    // Write include lines for subclasses
    for (subclass_it = subclassSourceIncludes.begin(); subclass_it != subclassSourceIncludes.end(); ++subclass_it) {
        m_source->WriteLn(*subclass_it);
    }
    if (!subclassSourceIncludes.empty()) {
        m_source->WriteLn(wxEmptyString);
    }

    // Generated header
    m_source->WriteLn(wxT("#include \"") + base_name + wxT(".h\""));

    m_source->WriteLn(wxEmptyString);
    GenEmbeddedBitmapIncludes(project);

    code = GetCode(project, wxT("cpp_epilogue"));
    m_source->WriteLn(code);

    // namespace
    PProperty propNamespace = project->GetProperty("cpp_namespace");
    wxArrayString namespaceArray;
    if (propNamespace) {
        namespaceArray = propNamespace->GetValueAsArrayString();
        wxString usingNamespaceStr;
        for (unsigned int i = 0; i < namespaceArray.Count(); ++i) {
            m_header->WriteLn(wxT("namespace ") + namespaceArray[i]);
            m_header->WriteLn(wxT("{"));
            m_header->Indent();

            if (usingNamespaceStr.empty()) {
                usingNamespaceStr = wxT("using namespace ");
            } else {
                usingNamespaceStr += wxT("::");
            }
            usingNamespaceStr += namespaceArray[i];
        }

        if (namespaceArray.Count() && !usingNamespaceStr.empty()) {
            usingNamespaceStr += wxT(";");
            m_source->WriteLn(usingNamespaceStr);
        }
    }

    // Generating "defines" for macros
    if (!useEnum) {
        GenDefines(project);
    }

    for (unsigned int i = 0; i < project->GetChildCount(); i++) {
        PObjectBase child = project->GetChild(i);

        // Preprocess to find arrays
        ArrayItems arrays;
        FindArrayObjects(child, arrays, true);

        EventVector events;
        FindEventHandlers(child, events);
        GenClassDeclaration(child, useEnum, classDecoration, events, arrays);
        if (!m_useConnect) {
            GenEvents(child, events);
        }
        GenConstructor(child, events, arrays);
        GenDestructor(child, events);
    }

    // namespace
    if (namespaceArray.Count() > 0) {
        for (size_t i = namespaceArray.Count(); i > 0; --i) {
            m_header->Unindent();
            m_header->WriteLn(wxT("} // namespace ") + namespaceArray[i - 1]);
        }
        m_header->WriteLn(wxT("#endif"));
        m_header->WriteLn(wxEmptyString);
    }

    return true;
}

void CppCodeGenerator::GenEvents(PObjectBase class_obj, const EventVector& events, bool disconnect)
{
    if (events.empty()) {
        return;
    }

    PProperty propName = class_obj->GetProperty(wxT("name"));
    if (!propName) {
        wxLogError(
          wxT("Missing \"name\" property on \"%s\" class. Review your XML object description"),
          class_obj->GetClassName());
        return;
    }

    wxString class_name = propName->GetValue();
    if (class_name.empty()) {
        wxLogError(wxT("Object name cannot be null"));
        return;
    }

    wxString base_class;
    PProperty propSubclass = class_obj->GetProperty(wxT("subclass"));
    if (propSubclass) {
        wxString subclass = propSubclass->GetChildFromParent(wxT("name"));
        if (!subclass.empty()) {
            base_class = subclass;
        }
    }

    if (base_class.empty())
        base_class = wxT("wx") + class_obj->GetClassName();

    if (events.size() > 0) {
        if (!m_useConnect) {
            m_source->WriteLn();
            m_source->WriteLn(wxT("BEGIN_EVENT_TABLE( ") + class_name + wxT(", ") + base_class + wxT(" )"));
            m_source->Indent();
        }

        for (size_t i = 0; i < events.size(); i++) {
            PEvent event = events[i];
            wxString handlerName;
            if (m_useConnect) {
                handlerName.Printf(
                  wxT("%sHandler( %s::%s )"), event->GetEventInfo()->GetEventClassName(), class_name,
                  event->GetValue());
            } else {
                handlerName.Printf(wxT("%s::_wxFB_%s"), class_name, event->GetValue());
            }
            wxString templateName =
              wxString::Format(wxT("%s_%s"), m_useConnect ? wxT("connect") : wxT("entry"), event->GetName());

            PObjectBase obj = event->GetObject();
            if (
              !GenEventEntry(obj, obj->GetObjectInfo(), templateName, handlerName, disconnect) &&
              (!disconnect || event->GetName() != "OnMenuSelection")) {
                // We don't bother with disconnecting OnMenuSelection events, that's why we skip the
                // error message in that case.
                wxLogError(
                  wxT("Missing \"evt_%s\" template for \"%s\" class. Review your XML object description"), templateName,
                  class_name);
            }
        }

        if (!m_useConnect) {
            m_source->Unindent();
            m_source->WriteLn(wxT("END_EVENT_TABLE()"));
        }
    }
}

bool CppCodeGenerator::GenEventEntry(
  PObjectBase obj, PObjectInfo obj_info, const wxString& templateName, const wxString& handlerName, bool disconnect)
{
    wxString _template;
    PCodeInfo code_info = obj_info->GetCodeInfo(wxT("C++"));
    if (code_info) {
        _template = code_info->GetTemplate(
          wxString::Format(wxT("evt_%s%s"), disconnect ? wxT("dis") : wxEmptyString, templateName));
        if (disconnect && _template.empty()) {
            _template = code_info->GetTemplate(wxT("evt_") + templateName);
            if (_template.Replace(wxT("Connect"), wxT("Disconnect"), true) == 0) {
                _template.clear();  // "Connect" wasn't found, skip this entry
            }
        }

        if (!_template.empty()) {
            _template.Replace(wxT("#handler"), handlerName);  // Ugly patch!
            CppTemplateParser parser(obj, _template, m_i18n, m_useRelativePath, m_basePath);
            m_source->WriteLn(parser.ParseTemplate());
            return true;
        }
    }

    for (unsigned int i = 0; i < obj_info->GetBaseClassCount(false); i++) {
        PObjectInfo base_info = obj_info->GetBaseClass(i, false);
        if (GenEventEntry(obj, base_info, templateName, handlerName, disconnect)) {
            return true;
        }
    }

    return false;
}

void CppCodeGenerator::GenPrivateEventHandlers(const EventVector& events)
{
    if (events.size() > 0) {
        m_header->WriteLn(wxEmptyString);
        m_header->WriteLn(wxT("// Private event handlers"));

        std::set<wxString> generatedHandlers;

        for (size_t i = 0; i < events.size(); i++) {
            PEvent event = events[i];
            wxString aux;

            if (generatedHandlers.find(event->GetValue()) == generatedHandlers.end()) {
                aux = wxT("void _wxFB_") + event->GetValue() + wxT("( ") + event->GetEventInfo()->GetEventClassName() +
                      wxT("& event ){ ") + event->GetValue() + wxT("( event ); }");

                m_header->WriteLn(aux);
                generatedHandlers.insert(event->GetValue());
            }
        }
        m_header->WriteLn(wxEmptyString);
    }
}

void CppCodeGenerator::GenVirtualEventHandlers(
  const EventVector& events, const wxString& eventHandlerPrefix, const wxString& eventHandlerPostfix)
{
    if (events.size() > 0) {
        // There are problems if we create "pure" virtual handlers, because some
        // events could be triggered in the constructor in which virtual methods are
        // execute properly.
        // So we create a default handler which will skip the event.
        m_header->WriteLn(wxEmptyString);
        m_header->WriteLn(wxT("// Virtual event handlers, override them in your derived class"));

        std::set<wxString> generatedHandlers;
        for (size_t i = 0; i < events.size(); i++) {
            PEvent event = events[i];
            wxString aux = eventHandlerPrefix + wxT("void ") + event->GetValue() + wxT("( ") +
                           event->GetEventInfo()->GetEventClassName() + wxT("& event )") + eventHandlerPostfix;

            if (generatedHandlers.find(aux) == generatedHandlers.end()) {
                m_header->WriteLn(aux);
                generatedHandlers.insert(aux);
            }
        }
        m_header->WriteLn(wxEmptyString);
    }
}

void CppCodeGenerator::GenAttributeDeclaration(PObjectBase obj, Permission perm, ArrayItems& arrays)
{
    wxString typeName = obj->GetObjectTypeName();
    if (ObjectDatabase::HasCppProperties(typeName)) {
        wxString perm_str = obj->GetProperty(wxT("permission"))->GetValue();

        if (
          (perm == P_PUBLIC && perm_str == wxT("public")) || (perm == P_PROTECTED && perm_str == wxT("protected")) ||
          (perm == P_PRIVATE && perm_str == wxT("private"))) {
            const auto& code = GetDeclaration(obj, arrays, perm != P_PRIVATE && m_useArrayEnum);
            if (!code.empty())
                m_header->WriteLn(code);
        }
    }

    // Generate recursively the rest of the attributes
    for (unsigned int i = 0; i < obj->GetChildCount(); i++) {
        PObjectBase child = obj->GetChild(i);

        GenAttributeDeclaration(child, perm, arrays);
    }
}
void CppCodeGenerator::GenValidatorVariables(PObjectBase obj)
{
    GenValVarsBase(obj->GetObjectInfo(), obj);

    // Proceeding recursively with the children
    for (unsigned int i = 0; i < obj->GetChildCount(); i++) {
        PObjectBase child = obj->GetChild(i);
        GenValidatorVariables(child);
    }
}

void CppCodeGenerator::GenValVarsBase(PObjectInfo info, PObjectBase obj)
{
    wxString _template;
    PCodeInfo code_info = info->GetCodeInfo(wxT("C++"));

    if (!code_info) {
        return;
    }

    _template = code_info->GetTemplate(wxT("valvar_declaration"));

    if (!_template.empty()) {
        CppTemplateParser parser(obj, _template, m_i18n, m_useRelativePath, m_basePath);
        wxString code = parser.ParseTemplate();
        if (!code.empty()) {
            m_header->WriteLn(code);
        }
    }

    // Proceeding recursively with the base classes
    for (unsigned int i = 0; i < info->GetBaseClassCount(false); i++) {
        PObjectInfo base_info = info->GetBaseClass(i, false);
        GenValVarsBase(base_info, obj);
    }
}

void CppCodeGenerator::GetGenEventHandlers(PObjectBase obj)
{
    GenDefinedEventHandlers(obj->GetObjectInfo(), obj);

    // Process child widgets
    for (unsigned int i = 0; i < obj->GetChildCount(); i++) {
        PObjectBase child = obj->GetChild(i);
        GetGenEventHandlers(child);
    }
}

void CppCodeGenerator::GenDefinedEventHandlers(PObjectInfo info, PObjectBase obj)
{
    PCodeInfo code_info = info->GetCodeInfo(wxT("C++"));
    if (code_info) {
        wxString _template = code_info->GetTemplate(wxT("generated_event_handlers"));
        if (!_template.empty()) {
            CppTemplateParser parser(obj, _template, m_i18n, m_useRelativePath, m_basePath);
            wxString code = parser.ParseTemplate();

            if (!code.empty()) {
                m_header->WriteLn(code);
            }
        }
    }

    // Proceeding recursively with the base classes
    for (unsigned int i = 0; i < info->GetBaseClassCount(false); i++) {
        PObjectInfo base_info = info->GetBaseClass(i, false);
        GenDefinedEventHandlers(base_info, obj);
    }
}

wxString CppCodeGenerator::GetCode(PObjectBase obj, wxString name)
{
    wxString _template;
    PCodeInfo code_info = obj->GetObjectInfo()->GetCodeInfo(wxT("C++"));

    if (!code_info) {
        wxString msg(wxString::Format(
          wxT("Missing \"%s\" template for \"%s\" class. Review your XML object description"), name,
          obj->GetClassName()));
        wxLogError(msg);
        return wxEmptyString;
    }

    _template = code_info->GetTemplate(name);

    CppTemplateParser parser(obj, _template, m_i18n, m_useRelativePath, m_basePath);
    wxString code = parser.ParseTemplate();

    return code;
}

wxString CppCodeGenerator::GetDeclaration(PObjectBase obj, ArrayItems& arrays, bool useEnum)
{
    // Get the name
    const auto& propName = obj->GetProperty(wxT("name"));
    if (!propName) {
        // Object has no name, just get its code
        return GetCode(obj, wxT("declaration"));
    }

    // Object has a name, check if its an array
    const auto& name = propName->GetValue();
    wxString baseName;
    ArrayItem unused;
    if (!ParseArrayName(name, baseName, unused)) {
        // Object is not an array, just get its code
        return GetCode(obj, wxT("declaration"));
    }

    // Object is an array, check if it needs to be declared
    auto& item = arrays[baseName];
    if (item.isDeclared) {
        // Object is already declared, return empty result
        return wxEmptyString;
    }

    // Array needs to be declared
    wxString code;

    if (useEnum) {
        // Generate enum value with element count of the array dimensions
        const auto enumBaseName = baseName.Upper();
        if (item.maxIndex.size() == 1) {
            // Without dimension index
            code.append(wxString::Format(
              wxT("enum { %s_SIZE = %u };\n"), enumBaseName, static_cast<unsigned int>(item.maxIndex[0] + 1)));
        } else {
            // With dimension index
            code.append(wxT("enum { "));
            for (size_t i = 0; i < item.maxIndex.size(); ++i) {
                if (i > 0) {
                    code.append(wxT(", "));
                }
                code.append(wxString::Format(
                  wxT("%s_%u_SIZE = %u"), enumBaseName, static_cast<unsigned int>(i),
                  static_cast<unsigned int>(item.maxIndex[i] + 1)));
            }
            code.append(wxT(" };\n"));
        }
    }

    // Construct proper name for declaration
    auto targetName = baseName;
    for (const auto& index : item.maxIndex) {
        targetName.append(wxString::Format(wxT("[%u]"), static_cast<unsigned int>(index + 1)));
    }

    // Set declaration name
    propName->SetValue(targetName);
    // Get the Code
    code.append(GetCode(obj, wxT("declaration")));
    // Restore the name
    propName->SetValue(name);

    // Mark the array as declared
    item.isDeclared = true;

    return code;
}

void CppCodeGenerator::GenClassDeclaration(
  PObjectBase class_obj, bool use_enum, const wxString& classDecoration, const EventVector& events, ArrayItems& arrays)
{
    PProperty propName = class_obj->GetProperty(wxT("name"));
    if (!propName) {
        wxLogError(
          wxT("Missing \"name\" property on \"%s\" class. Review your XML object description"),
          class_obj->GetClassName());
        return;
    }

    wxString class_name = propName->GetValue();
    if (class_name.empty()) {
        wxLogError(wxT("Object name can not be null"));
        return;
    }

    m_header->WriteLn(wxT("///////////////////////////////////////////////////////////////////////////////"));
    m_header->WriteLn(wxT("/// Class ") + class_name);
    m_header->WriteLn(wxT("///////////////////////////////////////////////////////////////////////////////"));

    m_header->WriteLn(wxT("class ") + classDecoration + class_name + wxT(" : ") + GetCode(class_obj, wxT("base")));
    m_header->WriteLn(wxT("{"));
    m_header->Indent();

    // are there event handlers?
    if (!m_useConnect && events.size() > 0) {
        m_header->WriteLn(wxT("DECLARE_EVENT_TABLE()"));
    }

    // private
    m_header->WriteLn(wxT("private:"));
    m_header->Indent();
    GenAttributeDeclaration(class_obj, P_PRIVATE, arrays);

    if (!m_useConnect) {
        GenPrivateEventHandlers(events);
    }

    m_header->Unindent();
    m_header->WriteLn(wxEmptyString);

    // protected
    m_header->WriteLn(wxT("protected:"));
    m_header->Indent();

    if (use_enum)
        GenEnumIds(class_obj);

    GenAttributeDeclaration(class_obj, P_PROTECTED, arrays);

    wxString eventHandlerKind;
    wxString eventHandlerPrefix;
    wxString eventHandlerPostfix;

    PProperty eventHandlerKindProp = class_obj->GetProperty(wxT("event_handler"));
    if (eventHandlerKindProp) {
        eventHandlerKind = eventHandlerKindProp->GetValueAsString();
    }

    if (0 == eventHandlerKind.compare(wxT("decl_pure_virtual"))) {
        eventHandlerPrefix = wxT("virtual ");
        eventHandlerPostfix = wxT(" = 0;");
    } else if (0 == eventHandlerKind.compare(wxT("decl"))) {
        eventHandlerPrefix = wxEmptyString;
        eventHandlerPostfix = wxT(";");
    } else  // Default: impl_virtual
    {
        eventHandlerPrefix = wxT("virtual ");
        eventHandlerPostfix = wxT(" { event.Skip(); }");
    }

    GenVirtualEventHandlers(events, eventHandlerPrefix, eventHandlerPostfix);

    m_header->Unindent();
    m_header->WriteLn(wxEmptyString);

    // public
    m_header->WriteLn(wxT("public:"));
    m_header->Indent();
    GenAttributeDeclaration(class_obj, P_PUBLIC, arrays);

    // Validators' variables
    GenValidatorVariables(class_obj);
    m_header->WriteLn(wxEmptyString);

    // The constructor is also included within public
    m_header->WriteLn(GetCode(class_obj, wxT("cons_decl")));

    // Destructor
    m_header->WriteLn(wxString::Format(wxT("~%s();"), class_name));

    GetGenEventHandlers(class_obj);
    m_header->Unindent();
    m_header->WriteLn(wxEmptyString);

    m_header->Unindent();
    m_header->WriteLn(wxT("};"));
    m_header->WriteLn(wxEmptyString);
}

void CppCodeGenerator::GenEnumIds(PObjectBase class_obj)
{
    std::vector<wxString> macros;
    FindMacros(class_obj, &macros);
    if (macros.empty()) {
        return;
    }

    m_header->WriteLn("enum");
    m_header->WriteLn("{");
    m_header->Indent();

    // Remove the default macro from the set, for backward compatibility
    if (auto macro = std::find(macros.begin(), macros.end(), "ID_DEFAULT"); macro != macros.end()) {
        // The default macro is defined to wxID_ANY
        m_header->WriteLn("ID_DEFAULT = wxID_ANY, // Default");
        macros.erase(macro);
    }

    if (!macros.empty()) {
        if (m_firstID < wxID_HIGHEST) {
            wxLogWarning(_("First ID is less than %i"), wxID_HIGHEST);
        }
        m_header->WriteLn(wxString::Format("%s = %i,", *macros.begin(), m_firstID));
        macros.erase(macros.begin());
    }

    for (const auto& macro : macros) {
        m_header->WriteLn(wxString::Format("%s,", macro));
    }

    m_header->Unindent();
    m_header->WriteLn("};");
    m_header->WriteLn();
}

void CppCodeGenerator::GenSubclassSets(
  PObjectBase obj, std::set<wxString>* subclasses, std::set<wxString>* sourceIncludes,
  std::vector<wxString>* headerIncludes)
{
    // Call GenSubclassForwardDeclarations on all children as well
    for (unsigned int i = 0; i < obj->GetChildCount(); i++) {
        GenSubclassSets(obj->GetChild(i), subclasses, sourceIncludes, headerIncludes);
    }

    // Fill the set
    PProperty subclass = obj->GetProperty(wxT("subclass"));
    if (subclass) {
        std::map<wxString, wxString> children;
        subclass->SplitParentProperty(&children);

        std::map<wxString, wxString>::iterator name;
        name = children.find(wxT("name"));

        if (children.end() == name) {
            // No name, so do nothing
            return;
        }

        wxString nameVal = name->second;
        if (nameVal.empty()) {
            // No name, so do nothing
            return;
        }

        // check if user wants to include the header or forward declare
        bool forward_declare = true;
        std::map<wxString, wxString>::iterator forward_declare_it = children.find(wxT("forward_declare"));
        if (children.end() != forward_declare_it) {
            // The value needs to be tested like ObjectInspector does
            forward_declare = (forward_declare_it->second == forward_declare_it->first);
        }

        // get namespaces
        wxString originalValue = nameVal;
        int delimiter = nameVal.Find(wxT("::"));
        wxString forwardDecl = wxT("class ") + nameVal + wxT(";");
        if (wxNOT_FOUND != delimiter) {
            wxString subClassPrefix, subClassSuffix;
            do {
                wxString namespaceStr;
                namespaceStr = nameVal.Mid(0, delimiter);
                if (namespaceStr.empty()) {
                    break;
                }
                subClassPrefix += wxT("namespace ") + namespaceStr + wxT("{ ");
                subClassSuffix += wxT(" }");

                nameVal = nameVal.Mid(delimiter + 2);
                delimiter = nameVal.Find(wxT("::"));

            } while (delimiter != wxNOT_FOUND);

            if (delimiter != wxNOT_FOUND || nameVal.empty()) {
                wxLogError(
                  _("Invalid Value for Property\n\tObject: %s\n\tProperty: %s\n\tValue: %s"),
                  obj->GetPropertyAsString(_("name")), _("subclass"), originalValue);
                return;
            }

            forwardDecl = subClassPrefix + wxT("class ") + nameVal + wxT(";") + subClassSuffix;
        }

        // Now get the header
        wxString headerVal;
        auto header = children.find(wxT("header"));
        if (children.end() != header) {
            headerVal = header->second;
        }

        if (headerVal.empty()) {
            // No header, do a forward declare if requested, otherwise do nothing
            if (forward_declare) {
                subclasses->insert(forwardDecl);
            }

            return;
        }

        // Got a header
        PObjectInfo info = obj->GetObjectInfo();
        if (!info) {
            return;
        }

        PObjectPackage pkg = info->GetPackage();
        if (!pkg) {
            return;
        }

        wxString include = wxT("#include \"") + headerVal + wxT("\"");
        if (pkg->GetPackageName() == wxT("Forms") || obj->GetChild(1, wxT("menu")) || !forward_declare) {
            std::vector<wxString>::iterator it = std::find(headerIncludes->begin(), headerIncludes->end(), include);
            if (headerIncludes->end() == it) {
                headerIncludes->push_back(include);
            }
        } else {
            subclasses->insert(forwardDecl);
            sourceIncludes->insert(include);
        }
    }
}

void CppCodeGenerator::GenIncludes(PObjectBase project, std::vector<wxString>* includes, std::set<wxString>* templates)
{
    GenObjectIncludes(project, includes, templates);
}

void CppCodeGenerator::GenObjectIncludes(
  PObjectBase project, std::vector<wxString>* includes, std::set<wxString>* templates)
{
    // Call GenIncludes on all children as well
    for (unsigned int i = 0; i < project->GetChildCount(); i++) {
        GenObjectIncludes(project->GetChild(i), includes, templates);
    }

    // Fill the set
    PCodeInfo code_info = project->GetObjectInfo()->GetCodeInfo(wxT("C++"));
    if (code_info) {
        CppTemplateParser parser(
          project, code_info->GetTemplate(wxT("include")), m_i18n, m_useRelativePath, m_basePath);
        wxString include = parser.ParseTemplate();
        if (!include.empty()) {
            if (templates->insert(include).second) {
                AddUniqueIncludes(include, includes);
            }
        }
    }

    // Generate includes for base classes
    GenBaseIncludes(project->GetObjectInfo(), project, includes, templates);
}

void CppCodeGenerator::GenBaseIncludes(
  PObjectInfo info, PObjectBase obj, std::vector<wxString>* includes, std::set<wxString>* templates)
{
    if (!info) {
        return;
    }

    // Process all the base classes recursively
    for (unsigned int i = 0; i < info->GetBaseClassCount(false); i++) {
        PObjectInfo base_info = info->GetBaseClass(i, false);
        GenBaseIncludes(base_info, obj, includes, templates);
    }

    PCodeInfo code_info = info->GetCodeInfo(wxT("C++"));
    if (code_info) {
        CppTemplateParser parser(obj, code_info->GetTemplate(wxT("include")), m_i18n, m_useRelativePath, m_basePath);
        wxString include = parser.ParseTemplate();
        if (!include.empty()) {
            if (templates->insert(include).second) {
                AddUniqueIncludes(include, includes);
            }
        }
    }
}

void CppCodeGenerator::AddUniqueIncludes(const wxString& include, std::vector<wxString>* includes)
{
    // Split on newlines to only generate unique include lines
    // This strips blank lines and trims
    wxStringTokenizer tkz(include, wxT("\n"), wxTOKEN_STRTOK);

    bool inPreproc = false;

    while (tkz.HasMoreTokens()) {
        wxString line = tkz.GetNextToken();
        line.Trim(false);
        line.Trim(true);

        // Anything within a #if preprocessor block will be written
        if (line.StartsWith(wxT("#if"))) {
            inPreproc = true;
        } else if (line.StartsWith(wxT("#endif"))) {
            inPreproc = false;
        }

        if (inPreproc) {
            includes->push_back(line);
            continue;
        }

        // If it is not an include line, it will be written
        if (!line.StartsWith(wxT("#include"))) {
            includes->push_back(line);
            continue;
        }

        // If it is an include, it must be unique to be written
        std::vector<wxString>::iterator it = std::find(includes->begin(), includes->end(), line);
        if (includes->end() == it) {
            includes->push_back(line);
        }
    }
}

void CppCodeGenerator::FindDependencies(PObjectBase obj, std::set<PObjectInfo>& info_set)
{
    unsigned int ch_count = obj->GetChildCount();
    if (ch_count > 0) {
        unsigned int i;
        for (i = 0; i < ch_count; i++) {
            PObjectBase child = obj->GetChild(i);
            info_set.insert(child->GetObjectInfo());
            FindDependencies(child, info_set);
        }
    }
}

void CppCodeGenerator::GenConstructor(PObjectBase class_obj, const EventVector& events, ArrayItems& arrays)
{
    m_source->WriteLn();
    m_source->WriteLn(GetCode(class_obj, wxT("cons_def")));
    m_source->WriteLn(wxT("{"));
    m_source->Indent();

    wxString settings = GetCode(class_obj, wxT("settings"));
    if (!settings.empty()) {
        m_source->WriteLn(settings);
    }

    for (unsigned int i = 0; i < class_obj->GetChildCount(); i++) {
        GenConstruction(class_obj->GetChild(i), true, arrays);
    }

    wxString afterAddChild = GetCode(class_obj, wxT("after_addchild"));
    if (!afterAddChild.empty()) {
        m_source->WriteLn(afterAddChild);
        if (class_obj->GetObjectTypeName() == wxT("wizard") && class_obj->GetChildCount() > 0) {
            m_source->WriteLn(wxT("for ( unsigned int i = 1; i < m_pages.GetCount(); i++ )"));
            m_source->WriteLn(wxT("{"));
            m_source->Indent();
            m_source->WriteLn(wxT("m_pages.Item( i )->SetPrev( m_pages.Item( i - 1 ) );"));
            m_source->WriteLn(wxT("m_pages.Item( i - 1 )->SetNext( m_pages.Item( i ) );"));
            m_source->Unindent();
            m_source->WriteLn(wxT("}"));
        }
    }

    if (m_useConnect && !events.empty()) {
        m_source->WriteLn();
        m_source->WriteLn(wxT("// Connect Events"));
        GenEvents(class_obj, events);
    }

    auto afterConnectEvents = GetCode(class_obj, wxT("after_connectevents"));
    if (!afterConnectEvents.empty()) {
        m_source->WriteLn();
        m_source->WriteLn(afterConnectEvents);
    }

    m_source->Unindent();
    m_source->WriteLn(wxT("}"));
}

void CppCodeGenerator::GenDestructor(PObjectBase class_obj, const EventVector& events)
{
    m_source->WriteLn();
    wxString className = class_obj->GetPropertyAsString(wxT("name"));
    m_source->WriteLn(wxString::Format(wxT("%s::~%s()"), className, className));
    m_source->WriteLn(wxT("{"));
    m_source->Indent();

    if (m_disconnectEvents && m_useConnect && !events.empty()) {
        m_source->WriteLn(wxT("// Disconnect Events"));
        GenEvents(class_obj, events, true);
        m_source->WriteLn();
    }

    // destruct objects
    GenDestruction(class_obj);

    m_source->Unindent();
    m_source->WriteLn(wxT("}"));
}

void CppCodeGenerator::GenConstruction(PObjectBase obj, bool is_widget, ArrayItems& arrays)
{
    wxString type = obj->GetObjectTypeName();
    PObjectInfo info = obj->GetObjectInfo();

    if (ObjectDatabase::HasCppProperties(type)) {
        // Checking if it has not been declared as class attribute
        // so that, we will declare it inside the constructor

        wxString perm_str = obj->GetProperty(wxT("permission"))->GetValue();
        if (perm_str == wxT("none")) {
            const auto& decl = GetDeclaration(obj, arrays, false);
            if (!decl.empty()) {
                m_source->WriteLn(decl);
            }
        }

        m_source->WriteLn(GetCode(obj, wxT("construction")));

        GenSettings(obj->GetObjectInfo(), obj);

        bool isWidget = !info->IsSubclassOf(wxT("sizer"));

        for (unsigned int i = 0; i < obj->GetChildCount(); i++) {
            PObjectBase child = obj->GetChild(i);
            GenConstruction(child, isWidget, arrays);

            if (type == wxT("toolbar")) {
                GenAddToolbar(child->GetObjectInfo(), child);
            }
        }

        if (!isWidget)  // sizers
        {
            wxString afterAddChild = GetCode(obj, wxT("after_addchild"));
            if (!afterAddChild.empty()) {
                m_source->WriteLn(afterAddChild);
            }
            m_source->WriteLn();

            if (is_widget) {
                // the parent object is not a sizer. There is no template for
                // this so we'll make it manually.
                // It's not a good practice to embed templates into the source code,
                // because you will need to recompile...

                wxString _template = wxT("#wxparent $name->SetSizer( $name ); #nl") wxT("#wxparent $name->Layout();")
                  wxT("#ifnull #parent $size") wxT("@{ #nl $name->Fit( #wxparent $name ); @}");

                CppTemplateParser parser(obj, _template, m_i18n, m_useRelativePath, m_basePath);
                m_source->WriteLn(parser.ParseTemplate());
            }
        } else if (type == wxT("splitter")) {
            // Generating the split
            switch (obj->GetChildCount()) {
                case 1: {
                    PObjectBase sub1 = obj->GetChild(0)->GetChild(0);
                    wxString _template = wxT("$name->Initialize( ");
                    _template = _template + sub1->GetProperty(wxT("name"))->GetValue() + wxT(" );");

                    CppTemplateParser parser(obj, _template, m_i18n, m_useRelativePath, m_basePath);
                    m_source->WriteLn(parser.ParseTemplate());
                    break;
                }
                case 2: {
                    PObjectBase sub1, sub2;
                    sub1 = obj->GetChild(0)->GetChild(0);
                    sub2 = obj->GetChild(1)->GetChild(0);

                    wxString _template;
                    if (obj->GetProperty(wxT("splitmode"))->GetValue() == wxT("wxSPLIT_VERTICAL")) {
                        _template = wxT("$name->SplitVertically( ");
                    } else {
                        _template = wxT("$name->SplitHorizontally( ");
                    }

                    _template = _template + sub1->GetProperty(wxT("name"))->GetValue() + wxT(", ") +
                                sub2->GetProperty(wxT("name"))->GetValue() + wxT(", $sashpos );");

                    CppTemplateParser parser(obj, _template, m_i18n, m_useRelativePath, m_basePath);
                    m_source->WriteLn(parser.ParseTemplate());
                    break;
                }
                default:
                    wxLogError(wxT("Missing subwindows for wxSplitterWindow widget."));
                    break;
            }
        } else if (
          type == wxT("menubar") || type == wxT("menu") || type == wxT("submenu") || type == wxT("ribbonbar") ||
          type == wxT("toolbar") || type == wxT("tool") || type == wxT("listbook") || type == wxT("simplebook") ||
          type == wxT("notebook") || type == wxT("auinotebook") || type == wxT("treelistctrl") ||
          type == wxT("flatnotebook")) {
            wxString afterAddChild = GetCode(obj, wxT("after_addchild"));
            if (!afterAddChild.empty()) {
                m_source->WriteLn(afterAddChild);
            }
            m_source->WriteLn();
        }
    } else if (info->IsSubclassOf(wxT("sizeritembase"))) {
        // The child must be added to the sizer having in mind the
        // child object type (there are 3 different routines)
        GenConstruction(obj->GetChild(0), false, arrays);

        PObjectInfo childInfo = obj->GetChild(0)->GetObjectInfo();
        wxString temp_name;
        if (childInfo->IsSubclassOf(wxT("wxWindow")) || wxT("CustomControl") == childInfo->GetClassName()) {
            temp_name = wxT("window_add");
        } else if (childInfo->IsSubclassOf(wxT("sizer"))) {
            temp_name = wxT("sizer_add");
        } else if (childInfo->GetClassName() == wxT("spacer")) {
            temp_name = wxT("spacer_add");
        } else {
            LogDebug(wxT("SizerItem child is not a Spacer and is not a subclass of wxWindow or of sizer."));
            return;
        }

        m_source->WriteLn(GetCode(obj, temp_name));
    } else if (
      type == wxT("notebookpage") || type == wxT("flatnotebookpage") || type == wxT("listbookpage") ||
      type == wxT("choicebookpage") || type == wxT("simplebookpage") || type == wxT("auinotebookpage") ||
      type == wxT(" wizardpagesimple")) {
        GenConstruction(obj->GetChild(0), false, arrays);
        m_source->WriteLn(GetCode(obj, wxT("page_add")));
        GenSettings(obj->GetObjectInfo(), obj);
    } else if (type == wxT("treelistctrlcolumn")) {
        m_source->WriteLn(GetCode(obj, wxT("column_add")));
        GenSettings(obj->GetObjectInfo(), obj);
    } else if (type == wxT("tool")) {
        // If loading bitmap from ICON resource, and size is not set, set size to toolbars bitmapsize
        // So hacky, yet so useful ...
        wxSize toolbarsize = obj->GetParent()->GetPropertyAsSize(_("bitmapsize"));
        if (wxDefaultSize != toolbarsize) {
            PProperty prop = obj->GetProperty(_("bitmap"));
            if (prop) {
                wxString oldVal = prop->GetValueAsString();
                wxString path, source;
                wxSize toolsize;
                TypeConv::ParseBitmapWithResource(oldVal, &path, &source, &toolsize);
                if (_("Load From Icon Resource") == source && wxDefaultSize == toolsize) {
                    prop->SetValue(wxString::Format(
                      wxT("%s; %s [%i; %i]"), path, source, toolbarsize.GetWidth(), toolbarsize.GetHeight()));
                    m_source->WriteLn(GetCode(obj, wxT("construction")));
                    prop->SetValue(oldVal);
                    return;
                }
            }
        }
        m_source->WriteLn(GetCode(obj, wxT("construction")));
    } else {
        // Generate the children
        for (unsigned int i = 0; i < obj->GetChildCount(); i++) {
            GenConstruction(obj->GetChild(i), false, arrays);
        }
    }
}

void CppCodeGenerator::FindMacros(PObjectBase obj, std::vector<wxString>* macros)
{
    // iterate through all of the properties of all objects, add the macros
    // to the vector
    unsigned int i;

    for (i = 0; i < obj->GetPropertyCount(); i++) {
        PProperty prop = obj->GetProperty(i);
        if (prop->GetType() == PT_MACRO) {
            wxString value = prop->GetValue();
            // Skip wx IDs
            if ((!value.Contains(wxT("XRCID"))) && (m_predMacros.end() == m_predMacros.find(value))) {
                if (macros->end() == std::find(macros->begin(), macros->end(), value)) {
                    macros->push_back(value);
                }
            }
        }
    }

    for (i = 0; i < obj->GetChildCount(); i++) {
        FindMacros(obj->GetChild(i), macros);
    }
}

void CppCodeGenerator::FindEventHandlers(PObjectBase obj, EventVector& events)
{
    unsigned int i;
    for (i = 0; i < obj->GetEventCount(); i++) {
        PEvent event = obj->GetEvent(i);
        if (!event->GetValue().IsEmpty())
            events.push_back(event);
    }

    for (i = 0; i < obj->GetChildCount(); i++) {
        PObjectBase child = obj->GetChild(i);
        FindEventHandlers(child, events);
    }
}

void CppCodeGenerator::GenDefines(PObjectBase project)
{
    std::vector<wxString> macros;
    FindMacros(project, &macros);

    // Remove the default macro from the set, for backward compatibility
    if (auto macro = std::find(macros.begin(), macros.end(), "ID_DEFAULT"); macro != macros.end()) {
        // The default macro is defined to wxID_ANY
        m_header->WriteLn("#define ID_DEFAULT wxID_ANY // Default");
        macros.erase(macro);
    }

    if (macros.empty()) {
        return;
    }

    auto id = m_firstID;
    if (id < wxID_HIGHEST) {
        wxLogWarning(_("First ID is less than %i"), wxID_HIGHEST);
    }
    for (const auto& macro : macros) {
        // Don't redefine wx IDs
        m_header->WriteLn(wxString::Format("#define %s %i", macro, id++));
    }
    m_header->WriteLn(wxEmptyString);
}

void CppCodeGenerator::GenSettings(PObjectInfo info, PObjectBase obj)
{
    wxString _template;
    PCodeInfo code_info = info->GetCodeInfo(wxT("C++"));

    if (!code_info) {
        return;
    }

    _template = code_info->GetTemplate(wxT("settings"));

    if (!_template.empty()) {
        CppTemplateParser parser(obj, _template, m_i18n, m_useRelativePath, m_basePath);
        wxString code = parser.ParseTemplate();
        if (!code.empty()) {
            m_source->WriteLn(code);
        }
    }

    // Proceeding recursively with the base classes
    for (unsigned int i = 0; i < info->GetBaseClassCount(false); i++) {
        PObjectInfo base_info = info->GetBaseClass(i, false);
        GenSettings(base_info, obj);
    }
}

void CppCodeGenerator::GenDestruction(PObjectBase obj)
{
    wxString _template;
    PCodeInfo code_info = obj->GetObjectInfo()->GetCodeInfo(wxT("C++"));

    if (code_info) {
        _template = code_info->GetTemplate(wxT("destruction"));

        if (!_template.empty()) {
            CppTemplateParser parser(obj, _template, m_i18n, m_useRelativePath, m_basePath);
            wxString code = parser.ParseTemplate();
            if (!code.empty()) {
                m_source->WriteLn(code);
            }
        }
    }

    // Process child widgets
    for (unsigned int i = 0; i < obj->GetChildCount(); i++) {
        PObjectBase child = obj->GetChild(i);
        GenDestruction(child);
    }
}

void CppCodeGenerator::GenAddToolbar(PObjectInfo info, PObjectBase obj)
{
    wxArrayString arrCode;

    GetAddToolbarCode(info, obj, arrCode);

    for (size_t i = 0; i < arrCode.GetCount(); i++)
        m_source->WriteLn(arrCode[i]);
}

void CppCodeGenerator::GetAddToolbarCode(PObjectInfo info, PObjectBase obj, wxArrayString& codelines)
{
    wxString _template;
    PCodeInfo code_info = info->GetCodeInfo(wxT("C++"));

    if (!code_info)
        return;

    _template = code_info->GetTemplate(wxT("toolbar_add"));

    if (!_template.empty()) {
        CppTemplateParser parser(obj, _template, m_i18n, m_useRelativePath, m_basePath);
        wxString code = parser.ParseTemplate();
        if (!code.empty()) {
            if (codelines.Index(code) == wxNOT_FOUND)
                codelines.Add(code);
        }
    }

    // Proceeding recursively with the base classes
    for (unsigned int i = 0; i < info->GetBaseClassCount(false); i++) {
        PObjectInfo base_info = info->GetBaseClass(i, false);
        GetAddToolbarCode(base_info, obj, codelines);
    }
}

///////////////////////////////////////////////////////////////////////


void CppCodeGenerator::GenEmbeddedBitmapIncludes(PObjectBase project)
{
    std::set<wxString> include_set;

    // We begin obtaining the "include" list
    FindEmbeddedBitmapProperties(project, include_set);

    if (include_set.empty()) {
        return;
    }

    // and then, we generate them
    std::set<wxString>::iterator it;
    for (it = include_set.begin(); it != include_set.end(); it++) {
        if (!it->empty()) {
            m_source->WriteLn(*it);
        }
    }

    m_source->WriteLn();
}

void CppCodeGenerator::FindEmbeddedBitmapProperties(PObjectBase obj, std::set<wxString>& embedset)
{
    // We go through (browse) for each property in "obj" object. If any of the
    // PT_BITMAP type is found, then the proper "include" string is added
    // in "set". After that, we recursively do it the same with the child objects.
    unsigned int i, count;

    count = obj->GetPropertyCount();

    for (i = 0; i < count; i++) {
        PProperty property = obj->GetProperty(i);
        if (property->GetType() == PT_BITMAP) {
            wxString propValue = property->GetValue();

            wxString path;
            wxString source;
            wxSize icoSize;
            TypeConv::ParseBitmapWithResource(propValue, &path, &source, &icoSize);

            wxFileName bmpFileName(path);
            if (bmpFileName.GetExt().Upper() == wxT("XPM")) {
                wxString absPath = TypeConv::MakeAbsolutePath(path, AppData()->GetProjectPath());

                // It's supposed that "path" contains an absolute path to the file
                // and not a relative one.
                wxString relPath = (m_useRelativePath ? TypeConv::MakeRelativePath(absPath, m_basePath) : absPath);

                wxString inc;
                inc << wxT("#include \"") << relPath << wxT("\"");
                embedset.insert(inc);
            } else if (source == _("Load From Embedded File")) {
                wxString absPath = TypeConv::MakeAbsolutePath(path, AppData()->GetProjectPath());
                wxString includePath = FileToCArray::Generate(absPath);
                wxString inc;
                inc << wxT("#include \"") << includePath << wxT("\"");
                embedset.insert(inc);
            }
            // NOTE: This is currently not necessary because the default code already contains this header.
            //       Because the unique include filtering is not global this cannot be enabled without creating a
            //       duplicate entry.
            // else if (source == _("Load From XRC"))
            //{
            //	embedset.insert(wxT("#include <wx/xrc/xmlres.h>"));
            //}
        }
    }

    count = obj->GetChildCount();
    for (i = 0; i < count; i++) {
        PObjectBase child = obj->GetChild(i);
        FindEmbeddedBitmapProperties(child, embedset);
    }
}

void CppCodeGenerator::UseRelativePath(bool relative, wxString basePath)
{
    m_useRelativePath = relative;

    if (m_useRelativePath) {
        if (wxFileName::DirExists(basePath)) {
            m_basePath = basePath;
        } else {
            m_basePath = wxEmptyString;
        }
    }
}

#define ADD_PREDEFINED_MACRO(x) m_predMacros.insert(wxT(#x))

void CppCodeGenerator::SetupPredefinedMacros()
{
    /* no id matches this one when compared to it */
    ADD_PREDEFINED_MACRO(wxID_NONE);

    /*  id for a separator line in the menu (invalid for normal item) */
    ADD_PREDEFINED_MACRO(wxID_SEPARATOR);

    ADD_PREDEFINED_MACRO(wxID_ANY);

    ADD_PREDEFINED_MACRO(wxID_LOWEST);

    ADD_PREDEFINED_MACRO(wxID_OPEN);
    ADD_PREDEFINED_MACRO(wxID_CLOSE);
    ADD_PREDEFINED_MACRO(wxID_NEW);
    ADD_PREDEFINED_MACRO(wxID_SAVE);
    ADD_PREDEFINED_MACRO(wxID_SAVEAS);
    ADD_PREDEFINED_MACRO(wxID_REVERT);
    ADD_PREDEFINED_MACRO(wxID_EXIT);
    ADD_PREDEFINED_MACRO(wxID_UNDO);
    ADD_PREDEFINED_MACRO(wxID_REDO);
    ADD_PREDEFINED_MACRO(wxID_HELP);
    ADD_PREDEFINED_MACRO(wxID_PRINT);
    ADD_PREDEFINED_MACRO(wxID_PRINT_SETUP);
    ADD_PREDEFINED_MACRO(wxID_PREVIEW);
    ADD_PREDEFINED_MACRO(wxID_ABOUT);
    ADD_PREDEFINED_MACRO(wxID_HELP_CONTENTS);
    ADD_PREDEFINED_MACRO(wxID_HELP_COMMANDS);
    ADD_PREDEFINED_MACRO(wxID_HELP_PROCEDURES);
    ADD_PREDEFINED_MACRO(wxID_HELP_CONTEXT);
    ADD_PREDEFINED_MACRO(wxID_CLOSE_ALL);
    ADD_PREDEFINED_MACRO(wxID_PAGE_SETUP);
    ADD_PREDEFINED_MACRO(wxID_HELP_INDEX);
    ADD_PREDEFINED_MACRO(wxID_HELP_SEARCH);
    ADD_PREDEFINED_MACRO(wxID_PREFERENCES);

    ADD_PREDEFINED_MACRO(wxID_EDIT);
    ADD_PREDEFINED_MACRO(wxID_CUT);
    ADD_PREDEFINED_MACRO(wxID_COPY);
    ADD_PREDEFINED_MACRO(wxID_PASTE);
    ADD_PREDEFINED_MACRO(wxID_CLEAR);
    ADD_PREDEFINED_MACRO(wxID_FIND);

    ADD_PREDEFINED_MACRO(wxID_DUPLICATE);
    ADD_PREDEFINED_MACRO(wxID_SELECTALL);
    ADD_PREDEFINED_MACRO(wxID_DELETE);
    ADD_PREDEFINED_MACRO(wxID_REPLACE);
    ADD_PREDEFINED_MACRO(wxID_REPLACE_ALL);
    ADD_PREDEFINED_MACRO(wxID_PROPERTIES);

    ADD_PREDEFINED_MACRO(wxID_VIEW_DETAILS);
    ADD_PREDEFINED_MACRO(wxID_VIEW_LARGEICONS);
    ADD_PREDEFINED_MACRO(wxID_VIEW_SMALLICONS);
    ADD_PREDEFINED_MACRO(wxID_VIEW_LIST);
    ADD_PREDEFINED_MACRO(wxID_VIEW_SORTDATE);
    ADD_PREDEFINED_MACRO(wxID_VIEW_SORTNAME);
    ADD_PREDEFINED_MACRO(wxID_VIEW_SORTSIZE);
    ADD_PREDEFINED_MACRO(wxID_VIEW_SORTTYPE);

    ADD_PREDEFINED_MACRO(wxID_FILE);
    ADD_PREDEFINED_MACRO(wxID_FILE1);
    ADD_PREDEFINED_MACRO(wxID_FILE2);
    ADD_PREDEFINED_MACRO(wxID_FILE3);
    ADD_PREDEFINED_MACRO(wxID_FILE4);
    ADD_PREDEFINED_MACRO(wxID_FILE5);
    ADD_PREDEFINED_MACRO(wxID_FILE6);
    ADD_PREDEFINED_MACRO(wxID_FILE7);
    ADD_PREDEFINED_MACRO(wxID_FILE8);
    ADD_PREDEFINED_MACRO(wxID_FILE9);

    /*  Standard button and menu IDs */

    ADD_PREDEFINED_MACRO(wxID_OK);
    ADD_PREDEFINED_MACRO(wxID_CANCEL);

    ADD_PREDEFINED_MACRO(wxID_APPLY);
    ADD_PREDEFINED_MACRO(wxID_YES);
    ADD_PREDEFINED_MACRO(wxID_NO);
    ADD_PREDEFINED_MACRO(wxID_STATIC);
    ADD_PREDEFINED_MACRO(wxID_FORWARD);
    ADD_PREDEFINED_MACRO(wxID_BACKWARD);
    ADD_PREDEFINED_MACRO(wxID_DEFAULT);
    ADD_PREDEFINED_MACRO(wxID_MORE);
    ADD_PREDEFINED_MACRO(wxID_SETUP);
    ADD_PREDEFINED_MACRO(wxID_RESET);
    ADD_PREDEFINED_MACRO(wxID_CONTEXT_HELP);
    ADD_PREDEFINED_MACRO(wxID_YESTOALL);
    ADD_PREDEFINED_MACRO(wxID_NOTOALL);
    ADD_PREDEFINED_MACRO(wxID_ABORT);
    ADD_PREDEFINED_MACRO(wxID_RETRY);
    ADD_PREDEFINED_MACRO(wxID_IGNORE);
    ADD_PREDEFINED_MACRO(wxID_ADD);
    ADD_PREDEFINED_MACRO(wxID_REMOVE);

    ADD_PREDEFINED_MACRO(wxID_UP);
    ADD_PREDEFINED_MACRO(wxID_DOWN);
    ADD_PREDEFINED_MACRO(wxID_HOME);
    ADD_PREDEFINED_MACRO(wxID_REFRESH);
    ADD_PREDEFINED_MACRO(wxID_STOP);
    ADD_PREDEFINED_MACRO(wxID_INDEX);

    ADD_PREDEFINED_MACRO(wxID_BOLD);
    ADD_PREDEFINED_MACRO(wxID_ITALIC);
    ADD_PREDEFINED_MACRO(wxID_JUSTIFY_CENTER);
    ADD_PREDEFINED_MACRO(wxID_JUSTIFY_FILL);
    ADD_PREDEFINED_MACRO(wxID_JUSTIFY_RIGHT);

    ADD_PREDEFINED_MACRO(wxID_JUSTIFY_LEFT);
    ADD_PREDEFINED_MACRO(wxID_UNDERLINE);
    ADD_PREDEFINED_MACRO(wxID_INDENT);
    ADD_PREDEFINED_MACRO(wxID_UNINDENT);
    ADD_PREDEFINED_MACRO(wxID_ZOOM_100);
    ADD_PREDEFINED_MACRO(wxID_ZOOM_FIT);
    ADD_PREDEFINED_MACRO(wxID_ZOOM_IN);
    ADD_PREDEFINED_MACRO(wxID_ZOOM_OUT);
    ADD_PREDEFINED_MACRO(wxID_UNDELETE);
    ADD_PREDEFINED_MACRO(wxID_REVERT_TO_SAVED);

    /*  System menu IDs (used by wxUniv): */
    ADD_PREDEFINED_MACRO(wxID_SYSTEM_MENU);
    ADD_PREDEFINED_MACRO(wxID_CLOSE_FRAME);
    ADD_PREDEFINED_MACRO(wxID_MOVE_FRAME);
    ADD_PREDEFINED_MACRO(wxID_RESIZE_FRAME);
    ADD_PREDEFINED_MACRO(wxID_MAXIMIZE_FRAME);
    ADD_PREDEFINED_MACRO(wxID_ICONIZE_FRAME);
    ADD_PREDEFINED_MACRO(wxID_RESTORE_FRAME);

    /*  IDs used by generic file dialog (13 consecutive starting from this value) */
    ADD_PREDEFINED_MACRO(wxID_FILEDLGG);

    ADD_PREDEFINED_MACRO(wxID_HIGHEST);
}

#undef ADD_PREDEFINED_MACRO
