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

#include <cstring>
#include <map>

#include <wx/gbsizer.h>
#include <wx/wrapsizer.h>

#include <common/xmlutils.h>
#include <plugin_interface/plugin.h>
#include <plugin_interface/xrcconv.h>


class SpacerComponent : public ComponentBase
{
public:
    // ImportFromXRC is handled in sizeritem components

    tinyxml2::XMLElement* ExportToXrc(tinyxml2::XMLElement* xrc, const IObject* obj) override
    {
        ObjectToXrcFilter filter(xrc, GetLibrary(), obj, std::nullopt, "");
        filter.AddPropertyPair("width", "height", "size");
        return xrc;
    }
};

class GBSizerItemComponent : public ComponentBase
{
public:
    tinyxml2::XMLElement* ExportToXrc(tinyxml2::XMLElement* xrc, const IObject* obj) override
    {
        ObjectToXrcFilter filter(xrc, GetLibrary(), obj, "sizeritem", "");
        filter.AddProperty(XrcFilter::Type::BitList, "flag");
        filter.AddProperty(XrcFilter::Type::Integer, "border");
        filter.AddPropertyPair("row", "column", "cellpos");
        filter.AddPropertyPair("rowspan", "colspan", "cellspan");
        return xrc;
    }

    tinyxml2::XMLElement* ImportFromXrc(tinyxml2::XMLElement* xfb, const tinyxml2::XMLElement* xrc) override
    {
        XrcToXfbFilter filter(xfb, GetLibrary(), xrc, "gbsizeritem", "");
        filter.AddProperty(XrcFilter::Type::BitList, "flag");
        filter.AddProperty(XrcFilter::Type::Integer, "border");
        filter.AddPropertyPair("cellpos", "row", "column");
        filter.AddPropertyPair("cellspan", "rowspan", "colspan");

        // XrcLoader::GetObject imports spacers as sizeritems, check if this is a spacer
        if (!xrc->FirstChildElement("object") && xrc->FirstChildElement("size")) {
            XrcToXfbFilter spacerFilter(xfb->InsertNewChildElement(""), GetLibrary(), xrc, "spacer", "");
            spacerFilter.AddPropertyPair("size", "width", "height");
        }

        return xfb;
    }
};

class SizerItemComponent : public ComponentBase
{
public:
    void OnCreated(wxObject* wxobject, wxWindow* /*wxparent*/) override
    {
        // Get parent sizer
        wxObject* parent = GetManager()->GetParent(wxobject);
        wxSizer* sizer = wxDynamicCast(parent, wxSizer);

        if (NULL == sizer) {
            wxLogError(
              wxT("The parent of a SizerItem is either missing or not a wxSizer - this should not be possible!"));
            return;
        }

        // Get child window
        wxObject* child = GetManager()->GetChild(wxobject, 0);
        if (NULL == child) {
            wxLogError(wxT("The SizerItem component has no child - this should not be possible!"));
            return;
        }

        // Get IObject for property access
        IObject* obj = GetManager()->GetIObject(wxobject);
        IObject* childObj = GetManager()->GetIObject(child);

        // Add the spacer
        if (_("spacer") == childObj->GetClassName()) {
            sizer->Add(
              childObj->GetPropertyAsInteger(_("width")), childObj->GetPropertyAsInteger(_("height")),
              obj->GetPropertyAsInteger(_("proportion")), obj->GetPropertyAsInteger(_("flag")),
              obj->GetPropertyAsInteger(_("border")));
            return;
        }

        // Add the child ( window or sizer ) to the sizer
        wxWindow* windowChild = wxDynamicCast(child, wxWindow);
        wxSizer* sizerChild = wxDynamicCast(child, wxSizer);

        if (windowChild != NULL) {
            sizer->Add(
              windowChild, obj->GetPropertyAsInteger(_("proportion")), obj->GetPropertyAsInteger(_("flag")),
              obj->GetPropertyAsInteger(_("border")));
        } else if (sizerChild != NULL) {
            sizer->Add(
              sizerChild, obj->GetPropertyAsInteger(_("proportion")), obj->GetPropertyAsInteger(_("flag")),
              obj->GetPropertyAsInteger(_("border")));
        } else {
            wxLogError(
              wxT("The SizerItem component's child is not a wxWindow or a wxSizer or a spacer - this should not be "
                  "possible!"));
        }
    }

    tinyxml2::XMLElement* ExportToXrc(tinyxml2::XMLElement* xrc, const IObject* obj) override
    {
        ObjectToXrcFilter filter(xrc, GetLibrary(), obj, std::nullopt, "");
        filter.AddProperty(XrcFilter::Type::BitList, "flag");
        filter.AddProperty(XrcFilter::Type::Integer, "border");
        filter.AddProperty(XrcFilter::Type::Integer, "proportion", "option");
        return xrc;
    }

    tinyxml2::XMLElement* ImportFromXrc(tinyxml2::XMLElement* xfb, const tinyxml2::XMLElement* xrc) override
    {
        XrcToXfbFilter filter(xfb, GetLibrary(), xrc, std::nullopt, "");
        filter.AddProperty(XrcFilter::Type::BitList, "flag");
        filter.AddProperty(XrcFilter::Type::Integer, "border");
        filter.AddProperty(XrcFilter::Type::Integer, "option", "proportion");

        // XrcLoader::GetObject imports spacers as sizeritems, check if this is a spacer
        if (!xrc->FirstChildElement("object") && xrc->FirstChildElement("size")) {
            XrcToXfbFilter spacerFilter(xfb->InsertNewChildElement(""), GetLibrary(), xrc, "spacer", "");
            spacerFilter.AddPropertyPair("size", "width", "height");
        }

        return xfb;
    }
};

class BoxSizerComponent : public ComponentBase
{
public:
    wxObject* Create(IObject* obj, wxObject* /*parent*/) override
    {
        wxBoxSizer* sizer = new wxBoxSizer(obj->GetPropertyAsInteger(_("orient")));
        sizer->SetMinSize(obj->GetPropertyAsSize(_("minimum_size")));
        return sizer;
    }

    tinyxml2::XMLElement* ExportToXrc(tinyxml2::XMLElement* xrc, const IObject* obj) override
    {
        ObjectToXrcFilter filter(xrc, GetLibrary(), obj);
        if (obj->GetPropertyAsSize("minimum_size") != wxDefaultSize) {
            filter.AddProperty(XrcFilter::Type::Size, "minimum_size", "minsize");
        }
        filter.AddProperty(XrcFilter::Type::Option, "orient");
        return xrc;
    }

    tinyxml2::XMLElement* ImportFromXrc(tinyxml2::XMLElement* xfb, const tinyxml2::XMLElement* xrc) override
    {
        XrcToXfbFilter filter(xfb, GetLibrary(), xrc);
        filter.AddProperty(XrcFilter::Type::Size, "minsize", "minimum_size");
        filter.AddProperty(XrcFilter::Type::Option, "orient");
        return xfb;
    }
};

class WrapSizerComponent : public ComponentBase
{
public:
    wxObject* Create(IObject* obj, wxObject* /*parent*/) override
    {
        const auto sizer =
          new wxWrapSizer(obj->GetPropertyAsInteger(_("orient")), obj->GetPropertyAsInteger(_("flags")));
        sizer->SetMinSize(obj->GetPropertyAsSize(_("minimum_size")));
        return sizer;
    }

    tinyxml2::XMLElement* ExportToXrc(tinyxml2::XMLElement* xrc, const IObject* obj) override
    {
        ObjectToXrcFilter filter(xrc, GetLibrary(), obj);
        if (obj->GetPropertyAsSize("minimum_size") != wxDefaultSize) {
            filter.AddProperty(XrcFilter::Type::Size, "minimum_size", "minsize");
        }
        filter.AddProperty(XrcFilter::Type::Option, "orient");
        filter.AddProperty(XrcFilter::Type::BitList, "flags");
        return xrc;
    }

    tinyxml2::XMLElement* ImportFromXrc(tinyxml2::XMLElement* xfb, const tinyxml2::XMLElement* xrc) override
    {
        XrcToXfbFilter filter(xfb, GetLibrary(), xrc);
        filter.AddProperty(XrcFilter::Type::Size, "minsize", "minimum_size");
        filter.AddProperty(XrcFilter::Type::Option, "orient");
        filter.AddProperty(XrcFilter::Type::BitList, "flags");
        return xfb;
    }
};

class StaticBoxSizerComponent : public ComponentBase
{
public:
    int m_count;
    StaticBoxSizerComponent() { m_count = 0; }
    wxObject* Create(IObject* obj, wxObject* parent) override
    {
        m_count++;
        wxStaticBox* box = new wxStaticBox((wxWindow*)parent, wxID_ANY, obj->GetPropertyAsString(_("label")));

        wxStaticBoxSizer* sizer = new wxStaticBoxSizer(box, obj->GetPropertyAsInteger(_("orient")));

        sizer->SetMinSize(obj->GetPropertyAsSize(_("minimum_size")));

        return sizer;
    }

    tinyxml2::XMLElement* ExportToXrc(tinyxml2::XMLElement* xrc, const IObject* obj) override
    {
        ObjectToXrcFilter filter(xrc, GetLibrary(), obj);
        if (obj->GetPropertyAsSize("minimum_size") != wxDefaultSize) {
            filter.AddProperty(XrcFilter::Type::Size, "minimum_size", "minsize");
        }
        filter.AddProperty(XrcFilter::Type::Option, "orient");
        filter.AddProperty(XrcFilter::Type::Text, "label");
        return xrc;
    }

    tinyxml2::XMLElement* ImportFromXrc(tinyxml2::XMLElement* xfb, const tinyxml2::XMLElement* xrc) override
    {
        XrcToXfbFilter filter(xfb, GetLibrary(), xrc);
        filter.AddProperty(XrcFilter::Type::Size, "minsize", "minimum_size");
        filter.AddProperty(XrcFilter::Type::Option, "orient");
        filter.AddProperty(XrcFilter::Type::Text, "label");
        return xfb;
    }
};

class GridSizerComponent : public ComponentBase
{
public:
    wxObject* Create(IObject* obj, wxObject* /*parent*/) override
    {
        wxGridSizer* sizer = new wxGridSizer(
          obj->GetPropertyAsInteger(_("rows")), obj->GetPropertyAsInteger(_("cols")),
          obj->GetPropertyAsInteger(_("vgap")), obj->GetPropertyAsInteger(_("hgap")));

        sizer->SetMinSize(obj->GetPropertyAsSize(_("minimum_size")));

        return sizer;
    }

    tinyxml2::XMLElement* ExportToXrc(tinyxml2::XMLElement* xrc, const IObject* obj) override
    {
        ObjectToXrcFilter filter(xrc, GetLibrary(), obj);
        if (obj->GetPropertyAsSize("minimum_size") != wxDefaultSize) {
            filter.AddProperty(XrcFilter::Type::Size, "minimum_size", "minsize");
        }
        filter.AddProperty(XrcFilter::Type::Integer, "rows");
        filter.AddProperty(XrcFilter::Type::Integer, "cols");
        filter.AddProperty(XrcFilter::Type::Integer, "vgap");
        filter.AddProperty(XrcFilter::Type::Integer, "hgap");
        return xrc;
    }

    tinyxml2::XMLElement* ImportFromXrc(tinyxml2::XMLElement* xfb, const tinyxml2::XMLElement* xrc) override
    {
        XrcToXfbFilter filter(xfb, GetLibrary(), xrc);
        filter.AddProperty(XrcFilter::Type::Size, "minsize", "minimum_size");
        filter.AddProperty(XrcFilter::Type::Integer, "rows");
        filter.AddProperty(XrcFilter::Type::Integer, "cols");
        filter.AddProperty(XrcFilter::Type::Integer, "vgap");
        filter.AddProperty(XrcFilter::Type::Integer, "hgap");
        return xfb;
    }
};

class FlexGridSizerBase : public ComponentBase
{
public:
    void AddProperties(IObject* obj, wxFlexGridSizer* sizer)
    {
        for (const auto& col : obj->GetPropertyAsVectorIntPair(_("growablecols"))) {
            sizer->AddGrowableCol(col.first, col.second);
        }
        for (const auto& row : obj->GetPropertyAsVectorIntPair(_("growablerows"))) {
            sizer->AddGrowableRow(row.first, row.second);
        }
        sizer->SetMinSize(obj->GetPropertyAsSize(_("minimum_size")));
        sizer->SetFlexibleDirection(obj->GetPropertyAsInteger(_("flexible_direction")));
        sizer->SetNonFlexibleGrowMode((wxFlexSizerGrowMode)obj->GetPropertyAsInteger(_("non_flexible_grow_mode")));
    }

    tinyxml2::XMLElement* ExportToXrc(tinyxml2::XMLElement* xrc, const IObject* obj) override
    {
        ObjectToXrcFilter filter(xrc, GetLibrary(), obj, "", "");
        if (obj->GetPropertyAsSize("minimum_size") != wxDefaultSize) {
            filter.AddProperty(XrcFilter::Type::Size, "minimum_size", "minsize");
        }
        filter.AddProperty(XrcFilter::Type::Integer, "vgap");
        filter.AddProperty(XrcFilter::Type::Integer, "hgap");
        filter.AddProperty(XrcFilter::Type::String, "growablerows");
        filter.AddProperty(XrcFilter::Type::String, "growablecols");
        return xrc;
    }

    tinyxml2::XMLElement* ImportFromXrc(tinyxml2::XMLElement* xfb, const tinyxml2::XMLElement* xrc) override
    {
        XrcToXfbFilter filter(xfb, GetLibrary(), xrc, "", "");
        filter.AddProperty(XrcFilter::Type::Size, "minsize", "minimum_size");
        filter.AddProperty(XrcFilter::Type::Integer, "vgap");
        filter.AddProperty(XrcFilter::Type::Integer, "hgap");
        filter.AddProperty(XrcFilter::Type::String, "growablerows");
        filter.AddProperty(XrcFilter::Type::String, "growablecols");
        return xfb;
    }
};

class FlexGridSizerComponent : public FlexGridSizerBase
{
public:
    wxObject* Create(IObject* obj, wxObject* /*parent*/) override
    {
        wxFlexGridSizer* sizer = new wxFlexGridSizer(
          obj->GetPropertyAsInteger(_("rows")), obj->GetPropertyAsInteger(_("cols")),
          obj->GetPropertyAsInteger(_("vgap")), obj->GetPropertyAsInteger(_("hgap")));

        AddProperties(obj, sizer);

        return sizer;
    }

    tinyxml2::XMLElement* ExportToXrc(tinyxml2::XMLElement* xrc, const IObject* obj) override
    {
        FlexGridSizerBase::ExportToXrc(xrc, obj);

        ObjectToXrcFilter filter(xrc, GetLibrary(), obj);
        filter.AddProperty(XrcFilter::Type::Integer, "rows");
        filter.AddProperty(XrcFilter::Type::Integer, "cols");

        return xrc;
    }

    tinyxml2::XMLElement* ImportFromXrc(tinyxml2::XMLElement* xfb, const tinyxml2::XMLElement* xrc) override
    {
        FlexGridSizerBase::ImportFromXrc(xfb, xrc);

        XrcToXfbFilter filter(xfb, GetLibrary(), xrc);
        filter.AddProperty(XrcFilter::Type::Integer, "rows");
        filter.AddProperty(XrcFilter::Type::Integer, "cols");

        return xfb;
    }
};

class GridBagSizerComponent : public FlexGridSizerBase
{
private:
    wxGBSizerItem* GetGBSizerItem(
      IObject* sizeritem, const wxGBPosition& position, const wxGBSpan& span, wxObject* child)
    {
        IObject* childObj = GetManager()->GetIObject(child);

        if (_("spacer") == childObj->GetClassName()) {
            return new wxGBSizerItem(
              childObj->GetPropertyAsInteger(_("width")), childObj->GetPropertyAsInteger(_("height")), position, span,
              sizeritem->GetPropertyAsInteger(_("flag")), sizeritem->GetPropertyAsInteger(_("border")), NULL);
        }

        // Add the child ( window or sizer ) to the sizer
        wxWindow* windowChild = wxDynamicCast(child, wxWindow);
        wxSizer* sizerChild = wxDynamicCast(child, wxSizer);

        if (windowChild != NULL) {
            return new wxGBSizerItem(
              windowChild, position, span, sizeritem->GetPropertyAsInteger(_("flag")),
              sizeritem->GetPropertyAsInteger(_("border")), NULL);
        } else if (sizerChild != NULL) {
            return new wxGBSizerItem(
              sizerChild, position, span, sizeritem->GetPropertyAsInteger(_("flag")),
              sizeritem->GetPropertyAsInteger(_("border")), NULL);
        } else {
            wxLogError(
              wxT("The GBSizerItem component's child is not a wxWindow or a wxSizer or a Spacer - this should not be "
                  "possible!"));
            return NULL;
        }
    }

public:
    wxObject* Create(IObject* obj, wxObject* /*parent*/) override
    {
        wxGridBagSizer* sizer =
          new wxGridBagSizer(obj->GetPropertyAsInteger(_("vgap")), obj->GetPropertyAsInteger(_("hgap")));

        if (!obj->IsPropertyNull(_("empty_cell_size"))) {
            sizer->SetEmptyCellSize(obj->GetPropertyAsSize(_("empty_cell_size")));
        }

        return sizer;
    }

    void OnCreated(wxObject* wxobject, wxWindow* /*wxparent*/) override
    {
        // For storing objects whose position needs to be determined
        std::vector<std::pair<wxObject*, wxGBSizerItem*> > newObjects;
        wxGBPosition lastPosition(0, 0);

        // Get sizer
        wxGridBagSizer* sizer = wxDynamicCast(wxobject, wxGridBagSizer);
        if (NULL == sizer) {
            wxLogError(wxT("This should be a wxGridBagSizer!"));
            return;
        }

        // Add the children
        IManager* manager = GetManager();
        size_t count = manager->GetChildCount(wxobject);
        if (0 == count) {
            // wxGridBagSizer gets upset sometimes without children
            sizer->Add(0, 0, wxGBPosition(0, 0));
            return;
        }
        for (size_t i = 0; i < count; ++i) {
            // Should be a GBSizerItem
            wxObject* wxsizerItem = manager->GetChild(wxobject, i);
            IObject* isizerItem = manager->GetIObject(wxsizerItem);

            // Get the location of the item
            wxGBSpan span(
              isizerItem->GetPropertyAsInteger(_("rowspan")), isizerItem->GetPropertyAsInteger(_("colspan")));

            int column = isizerItem->GetPropertyAsInteger(_("column"));
            if (column < 0) {
                // Needs to be auto positioned after the other children are added
                wxGBSizerItem* item = GetGBSizerItem(isizerItem, lastPosition, span, manager->GetChild(wxsizerItem, 0));
                if (item != NULL) {
                    newObjects.push_back(std::pair<wxObject*, wxGBSizerItem*>(wxsizerItem, item));
                }
                continue;
            }

            wxGBPosition position(isizerItem->GetPropertyAsInteger(_("row")), column);

            // Check for intersection
            if (sizer->CheckForIntersection(position, span)) {
                continue;
            }

            lastPosition = position;

            // Add the child to the sizer
            wxGBSizerItem* item = GetGBSizerItem(isizerItem, position, span, manager->GetChild(wxsizerItem, 0));
            if (item != NULL) {
                sizer->Add(item);
            }
        }

        std::vector<std::pair<wxObject*, wxGBSizerItem*> >::iterator it;
        for (it = newObjects.begin(); it != newObjects.end(); ++it) {
            wxGBPosition position = it->second->GetPos();
            wxGBSpan span = it->second->GetSpan();
            int column = position.GetCol();
            while (sizer->CheckForIntersection(position, span)) {
                column++;
                position.SetCol(column);
            }
            it->second->SetPos(position);
            sizer->Add(it->second);
            GetManager()->ModifyProperty(it->first, _("row"), wxString::Format(wxT("%i"), position.GetRow()), false);
            GetManager()->ModifyProperty(it->first, _("column"), wxString::Format(wxT("%i"), column), false);
        }

        AddProperties(manager->GetIObject(wxobject), sizer);
    }

    tinyxml2::XMLElement* ExportToXrc(tinyxml2::XMLElement* xrc, const IObject* obj) override
    {
        FlexGridSizerBase::ExportToXrc(xrc, obj);

        ObjectToXrcFilter filter(xrc, GetLibrary(), obj);

        return xrc;
    }

    tinyxml2::XMLElement* ImportFromXrc(tinyxml2::XMLElement* xfb, const tinyxml2::XMLElement* xrc) override
    {
        FlexGridSizerBase::ImportFromXrc(xfb, xrc);

        XrcToXfbFilter filter(xfb, GetLibrary(), xrc);

        return xfb;
    }
};

class StdDialogButtonSizerComponent : public ComponentBase
{
private:
    tinyxml2::XMLElement* ExportButtonToXrc(tinyxml2::XMLElement* xrc, const wxString& buttonId)
    {
        auto* buttonElement = xrc->InsertNewChildElement("object");
        XMLUtils::SetAttribute(buttonElement, "class", "button");

        auto* wxButtonElement = buttonElement->InsertNewChildElement("object");
        XMLUtils::SetAttribute(wxButtonElement, "class", "wxButton");
        XMLUtils::SetAttribute(wxButtonElement, "name", buttonId);

        return xrc;
    }

public:
    wxObject* Create(IObject* obj, wxObject* parent) override
    {
        wxStdDialogButtonSizer* sizer = new wxStdDialogButtonSizer();

        sizer->SetMinSize(obj->GetPropertyAsSize(_("minimum_size")));

        if (obj->GetPropertyAsInteger(_("OK")) != 0) {
            sizer->AddButton(new wxButton((wxWindow*)parent, wxID_OK));
        }
        if (obj->GetPropertyAsInteger(_("Yes")) != 0) {
            sizer->AddButton(new wxButton((wxWindow*)parent, wxID_YES));
        }
        if (obj->GetPropertyAsInteger(_("Save")) != 0) {
            sizer->AddButton(new wxButton((wxWindow*)parent, wxID_SAVE));
        }
        if (obj->GetPropertyAsInteger(_("Apply")) != 0) {
            sizer->AddButton(new wxButton((wxWindow*)parent, wxID_APPLY));
        }
        if (obj->GetPropertyAsInteger(_("No")) != 0) {
            sizer->AddButton(new wxButton((wxWindow*)parent, wxID_NO));
        }
        if (obj->GetPropertyAsInteger(_("Cancel")) != 0) {
            sizer->AddButton(new wxButton((wxWindow*)parent, wxID_CANCEL));
        }
        if (obj->GetPropertyAsInteger(_("Help")) != 0) {
            sizer->AddButton(new wxButton((wxWindow*)parent, wxID_HELP));
        }
        if (obj->GetPropertyAsInteger(_("ContextHelp")) != 0) {
            sizer->AddButton(new wxButton((wxWindow*)parent, wxID_CONTEXT_HELP));
        }

        sizer->Realize();
        return sizer;
    }

    tinyxml2::XMLElement* ExportToXrc(tinyxml2::XMLElement* xrc, const IObject* obj) override
    {
        ObjectToXrcFilter filter(xrc, GetLibrary(), obj);
        if (obj->GetPropertyAsSize("minimum_size") != wxDefaultSize) {
            filter.AddProperty(XrcFilter::Type::Size, "minimum_size", "minsize");
        }
        if (obj->GetPropertyAsInteger("OK") != 0) {
            ExportButtonToXrc(xrc, "wxID_OK");
        }
        if (obj->GetPropertyAsInteger("Yes") != 0) {
            ExportButtonToXrc(xrc, "wxID_YES");
        }
        if (obj->GetPropertyAsInteger("Save") != 0) {
            ExportButtonToXrc(xrc, "wxID_SAVE");
        }
        if (obj->GetPropertyAsInteger("Apply") != 0) {
            ExportButtonToXrc(xrc, "wxID_APPLY");
        }
        // FIXME: Close currently missing in data model
        if (obj->GetPropertyAsInteger("No") != 0) {
            ExportButtonToXrc(xrc, "wxID_NO");
        }
        if (obj->GetPropertyAsInteger("Cancel") != 0) {
            ExportButtonToXrc(xrc, "wxID_CANCEL");
        }
        if (obj->GetPropertyAsInteger("Help") != 0) {
            ExportButtonToXrc(xrc, "wxID_HELP");
        }
        if (obj->GetPropertyAsInteger("ContextHelp") != 0) {
            ExportButtonToXrc(xrc, "wxID_CONTEXT_HELP");
        }
        return xrc;
    }

    tinyxml2::XMLElement* ImportFromXrc(tinyxml2::XMLElement* xfb, const tinyxml2::XMLElement* xrc) override
    {
        XrcToXfbFilter filter(xfb, GetLibrary(), xrc);
        filter.AddProperty(XrcFilter::Type::Size, "minsize", "minimum_size");

        std::map<wxString, std::pair<wxString, wxString>> buttons = {
            {"wxID_OK", {"OK", "0"}},
            {"wxID_YES", {"Yes", "0"}},
            {"wxID_SAVE", {"Save", "0"}},
            {"wxID_APPLY", {"Apply", "0"}},
            // FIXME: Close currently missing in data model
            {"wxID_NO", {"No", "0"}},
            {"wxID_CANCEL", {"Cancel", "0"}},
            {"wxID_HELP", {"Help", "0"}},
            {"wxID_CONTEXT_HELP", {"ContextHelp", "0"}},
        };
        for (const auto* button = xrc->FirstChildElement("object"); button; button = button->NextSiblingElement("object")) {
            if (XMLUtils::StringAttribute(button, "class") != "button") {
                continue;
            }

            const auto* wxButton = button->FirstChildElement("object");
            if (XMLUtils::StringAttribute(wxButton, "class") != "wxButton") {
                continue;
            }
            auto buttonEntry = buttons.find(XMLUtils::StringAttribute(wxButton, "name"));
            if (buttonEntry != buttons.end()) {
                buttonEntry->second.second = "1";
            }
        }
        for (const auto& [id, property] : buttons) {
            filter.AddPropertyValue(property.first, property.second);
        }

        return xfb;
    }
};

///////////////////////////////////////////////////////////////////////////////

BEGIN_LIBRARY()

ABSTRACT_COMPONENT("spacer", SpacerComponent)
ABSTRACT_COMPONENT("sizeritem", SizerItemComponent)
ABSTRACT_COMPONENT("gbsizeritem", GBSizerItemComponent)

SIZER_COMPONENT("wxBoxSizer", BoxSizerComponent)
SIZER_COMPONENT("wxWrapSizer", WrapSizerComponent)
SIZER_COMPONENT("wxStaticBoxSizer", StaticBoxSizerComponent)
SIZER_COMPONENT("wxGridSizer", GridSizerComponent)
SIZER_COMPONENT("wxFlexGridSizer", FlexGridSizerComponent)
SIZER_COMPONENT("wxGridBagSizer", GridBagSizerComponent)
SIZER_COMPONENT("wxStdDialogButtonSizer", StdDialogButtonSizerComponent)

// wxBoxSizer
MACRO(wxHORIZONTAL)
MACRO(wxVERTICAL)

// wxWrapSizer
MACRO(wxEXTEND_LAST_ON_EACH_LINE)
MACRO(wxREMOVE_LEADING_SPACES)
MACRO(wxWRAPSIZER_DEFAULT_FLAGS)

// wxFlexGridSizer
MACRO(wxBOTH)
MACRO(wxFLEX_GROWMODE_NONE)
MACRO(wxFLEX_GROWMODE_SPECIFIED)
MACRO(wxFLEX_GROWMODE_ALL)

// Add
MACRO(wxALL)
MACRO(wxLEFT)
MACRO(wxRIGHT)
MACRO(wxTOP)
MACRO(wxBOTTOM)
MACRO(wxEXPAND)
MACRO(wxALIGN_BOTTOM)
MACRO(wxALIGN_CENTER)
MACRO(wxALIGN_CENTER_HORIZONTAL)
MACRO(wxALIGN_CENTER_VERTICAL)
MACRO(wxSHAPED)
MACRO(wxFIXED_MINSIZE)
MACRO(wxRESERVE_SPACE_EVEN_IF_HIDDEN)

SYNONYMOUS(wxGROW, wxEXPAND)
SYNONYMOUS(wxALIGN_CENTRE, wxALIGN_CENTER)
SYNONYMOUS(wxALIGN_CENTRE_HORIZONTAL, wxALIGN_CENTER_HORIZONTAL)
SYNONYMOUS(wxALIGN_CENTRE_VERTICAL, wxALIGN_CENTER_VERTICAL)

END_LIBRARY()
