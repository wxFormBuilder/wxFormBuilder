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
// Modified by
//   Michal Bliznak
//
///////////////////////////////////////////////////////////////////////////////

#include "cpppanel.h"

#include <wx/aui/auibook.h>
#include <wx/fdrepdlg.h>
#include <wx/stc/stc.h>

#include "codegen/codewriter.h"
#include "codegen/cppcg.h"
#include "model/objectbase.h"
#include "rad/appdata.h"
#include "rad/auitabart.h"
#include "rad/bitmaps.h"
#include "rad/codeeditor/codeeditor.h"
#include "rad/wxfbevent.h"
#include "utils/typeconv.h"
#include "utils/wxfbexception.h"


BEGIN_EVENT_TABLE(CppPanel, wxPanel)
EVT_FB_CODE_GENERATION(CppPanel::OnCodeGeneration)
EVT_FB_CODE_GENERATION_TO_FILES(CppPanel::OnCodeGenerationToFiles)
EVT_FB_PROJECT_REFRESH(CppPanel::OnProjectRefresh)
EVT_FB_PROPERTY_MODIFIED(CppPanel::OnPropertyModified)
EVT_FB_OBJECT_CREATED(CppPanel::OnObjectChange)
EVT_FB_OBJECT_REMOVED(CppPanel::OnObjectChange)
EVT_FB_OBJECT_SELECTED(CppPanel::OnObjectChange)
EVT_FB_EVENT_HANDLER_MODIFIED(CppPanel::OnEventHandlerModified)

EVT_FIND(wxID_ANY, CppPanel::OnFind)
EVT_FIND_NEXT(wxID_ANY, CppPanel::OnFind)
END_EVENT_TABLE()


CppPanel::CppPanel(wxWindow* parent, int id) : wxPanel(parent, id)
{
    auto* topSizer = new wxBoxSizer(wxVERTICAL);

    m_notebook = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP);
    m_notebook->SetArtProvider(new AuiTabArt());

    m_cppPanel = new CodeEditor(m_notebook, wxID_ANY);
    InitStyledTextCtrl(m_cppPanel->GetTextCtrl());
    m_notebook->AddPage(m_cppPanel, _("cpp"), false);
    m_notebook->SetPageBitmap(m_notebook->GetPageCount() - 1, AppBitmaps::GetBitmap("cpp", AppBitmaps::Size::Icon_Medium));

    m_hPanel = new CodeEditor(m_notebook, wxID_ANY);
    InitStyledTextCtrl(m_hPanel->GetTextCtrl());
    m_notebook->AddPage(m_hPanel, _("h"), false);
    m_notebook->SetPageBitmap(m_notebook->GetPageCount() - 1, AppBitmaps::GetBitmap("h", AppBitmaps::Size::Icon_Medium));

    topSizer->Add(m_notebook, 1, wxEXPAND, 0);

    SetSizer(topSizer);

    m_hCW = std::make_shared<TCCodeWriter>(m_hPanel->GetTextCtrl());
    m_cppCW = std::make_shared<TCCodeWriter>(m_cppPanel->GetTextCtrl());

    AppData()->AddHandler(this->GetEventHandler());
}

CppPanel::~CppPanel()
{
    AppData()->RemoveHandler(this->GetEventHandler());
}

void CppPanel::InitStyledTextCtrl(wxStyledTextCtrl* stc)
{
    stc->SetLexer(wxSTC_LEX_CPP);
    stc->SetKeyWords(0, wxT("asm auto bool break case catch char class const const_cast \
	                          continue default delete do double dynamic_cast else enum explicit \
	                          export extern false float for friend goto if inline int long \
	                          mutable namespace new operator private protected public register \
	                          reinterpret_cast return short signed sizeof static static_cast \
	                          struct switch template this throw true try typedef typeid \
	                          typename union unsigned using virtual void volatile wchar_t \
	                          while"));

#ifdef __WXGTK__
    // Debe haber un bug en wxGTK ya que la familia wxMODERN no es de ancho fijo.
    wxFont font(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    font.SetFaceName(wxT("Monospace"));
#else
    wxFont font(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#endif

    stc->StyleSetForeground(wxSTC_STYLE_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    stc->StyleSetBackground(wxSTC_STYLE_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    stc->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
    stc->StyleClearAll();
    stc->StyleSetBold(wxSTC_C_WORD, true);
    if (!AppData()->IsDarkMode()) {
        stc->StyleSetForeground(wxSTC_C_WORD, *wxBLUE);
        stc->StyleSetForeground(wxSTC_C_STRING, *wxRED);
        stc->StyleSetForeground(wxSTC_C_STRINGEOL, *wxRED);
        stc->StyleSetForeground(wxSTC_C_PREPROCESSOR, wxColour(49, 106, 197));
        stc->StyleSetForeground(wxSTC_C_COMMENT, wxColour(0, 128, 0));
        stc->StyleSetForeground(wxSTC_C_COMMENTLINE, wxColour(0, 128, 0));
        stc->StyleSetForeground(wxSTC_C_COMMENTDOC, wxColour(0, 128, 0));
        stc->StyleSetForeground(wxSTC_C_COMMENTLINEDOC, wxColour(0, 128, 0));
        stc->StyleSetForeground(wxSTC_C_NUMBER, *wxBLUE);
    } else {
        stc->StyleSetForeground(wxSTC_C_WORD, wxColour(221, 40, 103));
        stc->StyleSetForeground(wxSTC_C_STRING, wxColour(23, 198, 163));
        stc->StyleSetForeground(wxSTC_C_STRINGEOL, wxColour(23, 198, 163));
        stc->StyleSetForeground(wxSTC_C_PREPROCESSOR, wxColour(204, 129, 186));
        stc->StyleSetForeground(wxSTC_C_COMMENT, wxColour(98, 98, 98));
        stc->StyleSetForeground(wxSTC_C_COMMENTLINE, wxColour(98, 98, 98));
        stc->StyleSetForeground(wxSTC_C_COMMENTDOC, wxColour(98, 98, 98));
        stc->StyleSetForeground(wxSTC_C_COMMENTLINEDOC, wxColour(98, 98, 98));
        stc->StyleSetForeground(wxSTC_C_NUMBER, wxColour(104, 151, 187));
    }
    stc->SetUseTabs(true);
    stc->SetTabWidth(4);
    stc->SetTabIndents(true);
    stc->SetBackSpaceUnIndents(true);
    stc->SetIndent(4);
    stc->SetSelBackground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    stc->SetSelForeground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));

    stc->SetCaretForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    stc->SetCaretWidth(2);
    stc->SetReadOnly(true);
}

void CppPanel::OnFind(wxFindDialogEvent& event)
{
    auto* page = m_notebook->GetCurrentPage();
    if (!page) {
        return;
    }

    page->GetEventHandler()->ProcessEvent(event);
}

void CppPanel::OnPropertyModified(wxFBPropertyEvent& event)
{
    // Generate code to the panel only
    event.SetId(1);
    OnCodeGeneration(event);
}

void CppPanel::OnProjectRefresh(wxFBEvent& event)
{
    // Generate code to the panel only
    event.SetId(1);
    OnCodeGeneration(event);
}

void CppPanel::OnObjectChange(wxFBObjectEvent& event)
{
    // Generate code to the panel only
    event.SetId(1);
    OnCodeGeneration(event);
}

void CppPanel::OnEventHandlerModified(wxFBEventHandlerEvent& event)
{
    // Generate code to the panel only
    event.SetId(1);
    OnCodeGeneration(event);
}

void CppPanel::OnCodeGeneration(wxFBEvent& event)
{
    PObjectBase objectToGenerate;

    // Generate code in the panel if the panel is active
    bool doPanel = IsShown();

    // Using the previously unused Id field in the event to carry a boolean
    bool panelOnly = (event.GetId() != 0);

    // Only generate to panel + panel is not shown = do nothing
    if (panelOnly && !doPanel) {
        return;
    }

    // For code preview generate only code relevant to selected form,
    // otherwise generate full project code.

    // Create copy of the original project due to possible temporary modifications
    PObjectBase project = PObjectBase(new ObjectBase(*AppData()->GetProjectData()));

    if (panelOnly) {
        objectToGenerate = AppData()->GetSelectedForm();
    }

    if (!panelOnly || !objectToGenerate) {
        objectToGenerate = project;
    }

    // If only one project item should be generated then remove the rest items
    // from the temporary project
    if (doPanel && panelOnly && (objectToGenerate != project)) {
        if (project->GetChildCount() > 0) {
            unsigned int i = 0;
            while (project->GetChildCount() > 1) {
                if (project->GetChild(i) != objectToGenerate) {
                    project->RemoveChild(i);
                } else
                    i++;
            }
        }
    }

    if (!project || !objectToGenerate)
        return;

    // Get C++ properties from the project

    // If C++ generation is not enabled, do not generate the file
    bool doFile = false;
    PProperty pCodeGen = project->GetProperty(wxT("code_generation"));
    if (pCodeGen) {
        doFile = TypeConv::FlagSet(wxT("C++"), pCodeGen->GetValue()) && !panelOnly;
    }

    if (!(doPanel || doFile)) {
        return;
    }

    // Get First ID from Project File
    int firstID = wxID_HIGHEST;
    PProperty pFirstID = project->GetProperty(wxT("first_id"));
    if (pFirstID) {
        firstID = pFirstID->GetValueAsInteger();
    }

    // Get the file name
    wxString file;
    PProperty pfile = project->GetProperty(wxT("file"));
    if (pfile) {
        file = pfile->GetValue();
    }
    if (file.empty()) {
        file = wxT("noname");
    }

    // Determine if the path is absolute or relative
    bool useRelativePath = false;
    PProperty pRelPath = project->GetProperty(wxT("relative_path"));
    if (pRelPath) {
        useRelativePath = (pRelPath->GetValueAsInteger() ? true : false);
    }

    // Get the output path
    wxString path;
    try {
        path = AppData()->GetOutputPath();
    } catch (wxFBException& ex) {
        if (doFile) {
            path = wxEmptyString;
            wxLogWarning(ex.what());
            return;
        }
    }

    // Generate code in the panel
    if (doPanel) {
        CppCodeGenerator codegen;
        codegen.UseRelativePath(useRelativePath, path);

        if (pFirstID) {
            codegen.SetFirstID(firstID);
        }

        codegen.SetHeaderWriter(m_hCW);
        codegen.SetSourceWriter(m_cppCW);

        Freeze();

        wxStyledTextCtrl* cppEditor = m_cppPanel->GetTextCtrl();
        wxStyledTextCtrl* hEditor = m_hPanel->GetTextCtrl();
        cppEditor->SetReadOnly(false);
        int cppLine = cppEditor->GetFirstVisibleLine() + cppEditor->LinesOnScreen() - 1;
        int cppXOffset = cppEditor->GetXOffset();

        hEditor->SetReadOnly(false);
        int hLine = hEditor->GetFirstVisibleLine() + hEditor->LinesOnScreen() - 1;
        int hXOffset = hEditor->GetXOffset();

        codegen.GenerateCode(project);

        cppEditor->SetReadOnly(true);
        cppEditor->GotoLine(cppLine);
        cppEditor->SetXOffset(cppXOffset);
        cppEditor->SetAnchor(0);
        cppEditor->SetCurrentPos(0);

        hEditor->SetReadOnly(true);
        hEditor->GotoLine(hLine);
        hEditor->SetXOffset(hXOffset);
        hEditor->SetAnchor(0);
        hEditor->SetCurrentPos(0);

        Thaw();
    }

    // Generate code in the file
    if (doFile) {
        try {
            CppCodeGenerator codegen;
            codegen.UseRelativePath(useRelativePath, path);

            if (pFirstID) {
                codegen.SetFirstID(firstID);
            }

            // Determine if Microsoft BOM should be used
            bool useMicrosoftBOM = false;
            if (auto property = project->GetProperty("use_microsoft_bom"); property) {
                useMicrosoftBOM = (property->GetValueAsInteger() != 0);
            }
            // Determine encoding
            bool useUtf8 = false;
            if (auto property = project->GetProperty("encoding"); property) {
                useUtf8 = (property->GetValueAsString() != wxT("ANSI"));
            }
            // Determine eol-style
            bool useNativeEOL = false;
            if (auto property = project->GetProperty("use_native_eol"); property) {
                useNativeEOL = (property->GetValueAsInteger() != 0);
            }

            auto h_cw = std::make_shared<FileCodeWriter>(path + file + wxT(".h"), useMicrosoftBOM, useUtf8, useNativeEOL);
            auto cpp_cw = std::make_shared<FileCodeWriter>(path + file + wxT(".cpp"), useMicrosoftBOM, useUtf8, useNativeEOL);

            codegen.SetHeaderWriter(h_cw);
            codegen.SetSourceWriter(cpp_cw);
            codegen.GenerateCode(project);
            wxLogStatus(wxT("Code generated on \'%s\'."), path);
        } catch (wxFBException& ex) {
            wxLogError(ex.what());
        }
    }
}

void CppPanel::_CodeGenerationOneFile(PObjectBase objectToGenerate)
{
    // Create copy of the original project due to possible temporary modifications
    PObjectBase project = PObjectBase(new ObjectBase(*AppData()->GetProjectData()));

    // If C++ generation is not enabled, do not generate the file
    bool doFile = false;
    PProperty pCodeGen = project->GetProperty(wxT("code_generation"));
    if (pCodeGen) {
        doFile = TypeConv::FlagSet(wxT("C++"), pCodeGen->GetValue());
    }

    if ( !doFile) {
        return;
    }

    // Get First ID from Project File
    int firstID = wxID_HIGHEST;
    PProperty pFirstID = project->GetProperty(wxT("first_id"));
    if (pFirstID) {
        firstID = pFirstID->GetValueAsInteger();
    }

    // Determine if the path is absolute or relative
    bool useRelativePath = false;
    PProperty pRelPath = project->GetProperty(wxT("relative_path"));
    if (pRelPath) {
        useRelativePath = (pRelPath->GetValueAsInteger() ? true : false);
    }

    // Get the output path
    wxString path;
    try {
        path = AppData()->GetOutputPath();
    } catch (wxFBException& ex) {
        if (doFile) {
            path = wxEmptyString;
            wxLogWarning(ex.what());
            return;
        }
    }
    wxString file;
    if (project->GetChildCount() > 0) {
        unsigned int i = 0;
        while (project->GetChildCount() > 1) {
            if (project->GetChild(i) != objectToGenerate) {
                project->RemoveChild(i);
                // get file name from class name.
                file = objectToGenerate->GetPropertyAsString(wxT("name"));
            } else
                i++;
        }
    }
    if (file.empty()) {
        file = wxT("noname");
    }
    // Generate code in the file
    if (doFile) {
        try {
            CppCodeGenerator codegen;
            codegen.UseRelativePath(useRelativePath, path);

            if (pFirstID) {
                codegen.SetFirstID(firstID);
            }

            // Determine if Microsoft BOM should be used
            bool useMicrosoftBOM = false;
            if (auto property = project->GetProperty("use_microsoft_bom"); property) {
                useMicrosoftBOM = (property->GetValueAsInteger() != 0);
            }
            // Determine encoding
            bool useUtf8 = false;
            if (auto property = project->GetProperty("encoding"); property) {
                useUtf8 = (property->GetValueAsString() != wxT("ANSI"));
            }
            // Determine eol-style
            bool useNativeEOL = false;
            if (auto property = project->GetProperty("use_native_eol"); property) {
                useNativeEOL = (property->GetValueAsInteger() != 0);
            }

            auto h_cw = std::make_shared<FileCodeWriter>(path + file + wxT(".h"), useMicrosoftBOM, useUtf8, useNativeEOL);
            auto cpp_cw = std::make_shared<FileCodeWriter>(path + file + wxT(".cpp"), useMicrosoftBOM, useUtf8, useNativeEOL);

            codegen.SetHeaderWriter(h_cw);
            codegen.SetSourceWriter(cpp_cw);
            codegen.GenerateCode(project);
            wxLogStatus(wxT("Code generated on \'%s\'."), path);
        } catch (wxFBException& ex) {
            wxLogError(ex.what());
        }
    }
}

void CppPanel::OnCodeGenerationToFiles(wxFBEvent&)
{
    // Create copy of the original project due to possible temporary modifications
    PObjectBase project = PObjectBase(new ObjectBase(*AppData()->GetProjectData()));

    // Generate code for each class
    if (project->GetChildCount() > 0) {
        for (unsigned int i = 0; i < project->GetChildCount(); i++)
        {
            _CodeGenerationOneFile(project->GetChild(i));
        }
    }
}
