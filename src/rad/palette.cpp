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

#include "palette.h"

#include <map>
#include <vector>

#include <wx/config.h>
#include <wx/tokenzr.h>

#include "model/objectbase.h"
#include "rad/appdata.h"
#include "rad/auitabart.h"
#include "rad/bitmaps.h"
#include "utils/debug.h"

#ifdef __WXMAC__
    #include <wx/tooltip.h>
#endif


wxFbPalette::wxFbPalette(wxWindow* parent, wxWindowID id) : wxPanel(parent, id), m_notebook(nullptr)
{
}

void wxFbPalette::PopulateToolbar(PObjectPackage pkg, wxAuiToolBar* toolbar)
{
    for (unsigned int i = 0; i < pkg->GetObjectCount(); ++i) {
        auto info = pkg->GetObjectInfo(i);
        if (info->IsStartOfGroup()) {
            toolbar->AddSeparator();
        }
        if (!info->GetComponent()) {
            LogDebug(wxString::Format(
                "Missing Component for Class \"%s\" of Package \"%s\"",
                info->GetClassName(),
                pkg->GetPackageName()
            ));
            continue;
        }

        auto widget = info->GetClassName();
        auto icon = info->GetIconFile();

#ifdef __WXMAC__
        auto* button = new wxBitmapButton(toolbar, wxID_ANY, icon);
        button->SetToolTip(widget);
        toolbar->AddControl(button);
#else
        toolbar->AddTool(wxID_ANY, widget, icon, widget);
#endif
    }
    toolbar->Realize();

#ifdef __WXMAC__
    toolbar->Bind(wxEVT_BUTTON, [this, toolbar](wxCommandEvent& event){ OnButtonClick(event, toolbar); }, wxID_ANY);
#else
    toolbar->Bind(wxEVT_TOOL, [this, toolbar](wxCommandEvent& event){ OnButtonClick(event, toolbar); }, wxID_ANY);
#endif
}

void wxFbPalette::SavePosition()
{
    auto* config = wxConfigBase::Get();
    wxString pageOrder;

    for (size_t i = 0; i < m_notebook->GetPageCount(); ++i) {
        if (!pageOrder.empty()) {
            pageOrder.append(wxT(","));
        }
        pageOrder.append(m_notebook->GetPageText(i));
    }

    config->Write("/palette/pageOrder", pageOrder);
}

void wxFbPalette::Create()
{
    // Package count
    auto pkg_count = AppData()->GetPackageCount();
    // Lookup map of all packages
    std::map<wxString, PObjectPackage> packages;
    // List of pages to add to the notebook in desired order
    std::vector<std::pair<wxString, PObjectPackage>> pages;
    pages.reserve(pkg_count);

    LogDebug("[Palette] Pages %u", pkg_count);

    // Fill lookup map of packages
    for (unsigned int i = 0; i < pkg_count; ++i) {
        auto pkg = AppData()->GetPackage(i);
        packages.insert(std::make_pair(pkg->GetPackageName(), pkg));
    }

    // Read the page order from settings and build the list of pages from it
    auto* config = wxConfigBase::Get();
    wxStringTokenizer pageOrder(
      config->Read(
        "/palette/pageOrder", "Common,Additional,Data,Containers,Ribbon,Menu/Toolbar,Layout,Forms"),
      ",");
    while (pageOrder.HasMoreTokens()) {
        const auto packageName = pageOrder.GetNextToken();
        auto package = packages.find(packageName);
        if (packages.end() == package) {
            // Plugin missing - move on
            continue;
        }

        // Add package to pages list and remove from lookup map
        pages.push_back(std::make_pair(package->first, package->second));
        packages.erase(package);
    }

    // The remaining packages from the lookup map need to be added to the page list
    for (auto& package : packages) { pages.push_back(std::make_pair(package.first, package.second)); }
    packages.clear();

    auto* top_sizer = new wxBoxSizer(wxVERTICAL);

    m_notebook = new wxAuiNotebook(
      this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TAB_MOVE);
    m_notebook->SetArtProvider(new AuiTabArt());

    wxSize minsize;

    for (size_t i = 0; i < pages.size(); ++i) {
        const auto& page = pages[i];

        auto* panel = new wxPanel(m_notebook, wxID_ANY);
        // panel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );
        auto* sizer = new wxBoxSizer(wxHORIZONTAL);

        auto* toolbar = new wxAuiToolBar(
          panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
        toolbar->SetToolBitmapSize(wxSize(AppBitmaps::GetPixelSize(AppBitmaps::Size::Icon), AppBitmaps::GetPixelSize(AppBitmaps::Size::Icon)));
        PopulateToolbar(page.second, toolbar);

        sizer->Add(toolbar, 1, wxEXPAND, 0);

        panel->SetAutoLayout(true);
        panel->SetSizer(sizer);
        sizer->Fit(panel);
        sizer->SetSizeHints(panel);

        auto cursize = panel->GetSize();
        if (cursize.x > minsize.x)
            minsize.x = cursize.x;
        if (cursize.y > minsize.y)
            minsize.y = cursize.y + 30;

        m_notebook->InsertPage(i, panel, page.first, false);
        m_notebook->SetPageBitmap(i, page.second->GetPackageIcon());
    }

    // Title *title = new Title( this, wxT("Component Palette") );
    // top_sizer->Add(title,0,wxEXPAND,0);
    top_sizer->Add(m_notebook, 1, wxEXPAND, 0);
    SetSizer(top_sizer);
    SetSize(minsize);
    SetMinSize(minsize);
    Layout();
    Fit();
}


void wxFbPalette::OnButtonClick(wxCommandEvent& event, [[maybe_unused]] wxAuiToolBar* parent)
{
#ifdef __WXMAC__
    auto* win = dynamic_cast<wxWindow*>(event.GetEventObject());
    if (win) {
        AppData()->CreateObject(win->GetToolTip()->GetTip());
    }
#else
    if (parent->GetToolIndex(event.GetId()) != wxNOT_FOUND) {
        AppData()->CreateObject(parent->GetToolShortHelp(event.GetId()));
    }
#endif
}
