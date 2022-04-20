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
//		Ryan Mulder - rjmyst3@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SDK_PLUGIN_INTERFACE_FONTCONTAINER_H
#define SDK_PLUGIN_INTERFACE_FONTCONTAINER_H

#include <wx/font.h>


/**
 * @brief Helper class to allow to store a wxFont with default values as valid object
 *
 * TODO: This class stores only some values of a wxFont, conversion between this class and wxFont is lossy!
 *       This class should be removed once it is possible to store arbitrary values in default state inside the data
 *       model.
 */
class wxFontContainer : public wxObject
{
public:
    wxFontContainer() { InitDefaults(); }

    wxFontContainer(
      int pointSize, wxFontFamily family = wxFONTFAMILY_DEFAULT, wxFontStyle style = wxFONTSTYLE_NORMAL,
      wxFontWeight weight = wxFONTWEIGHT_NORMAL, bool underlined = false, const wxString& faceName = wxEmptyString) :
        m_pointSize(pointSize),
        m_family(family),
        m_style(style),
        m_weight(weight),
        m_underlined(underlined),
        m_faceName(faceName)
    {
    }


    /**
     * @brief Automatic conversion from wxFont
     *
     * @param font Font
     */
    wxFontContainer(const wxFont& font)
    {
        if (font.IsOk()) {
            m_pointSize = font.GetPointSize();
            m_family = font.GetFamily();
            m_style = font.GetStyle();
            m_weight = font.GetWeight();
            m_underlined = font.GetUnderlined();
            m_faceName = font.GetFaceName();
        } else {
            InitDefaults();
        }
    }

    /**
     * @brief Automatic conversion to wxFont
     *
     * @return Font
     */
    operator wxFont() const { return GetFont(); }


    wxFont GetFont() const
    {
        const auto pointSize = (m_pointSize <= 0 ? wxNORMAL_FONT->GetPointSize() : m_pointSize);
        return wxFont(pointSize, m_family, m_style, m_weight, m_underlined, m_faceName);
    }


    int GetPointSize() const { return m_pointSize; }

    void SetPointSize(int pointSize) { m_pointSize = pointSize; }

    wxFontFamily GetFamily() const { return m_family; }

    void SetFamily(wxFontFamily family) { m_family = family; }

    wxFontStyle GetStyle() const { return m_style; }

    void SetStyle(wxFontStyle style) { m_style = style; }

    wxFontWeight GetWeight() const { return m_weight; }

    void SetWeight(wxFontWeight weight) { m_weight = weight; }

    bool GetUnderlined() const { return m_underlined; }

    void SetUnderlined(bool underlined) { m_underlined = underlined; }

    wxString GetFaceName() const { return m_faceName; }

    void SetFaceName(const wxString& faceName) { m_faceName = faceName; }

private:
    void InitDefaults()
    {
        m_pointSize = -1;
        m_family = wxFONTFAMILY_DEFAULT;
        m_style = wxFONTSTYLE_NORMAL;
        m_weight = wxFONTWEIGHT_NORMAL;
        m_underlined = false;
        m_faceName = wxEmptyString;
    }

private:
    int m_pointSize;        ///< Point Size
    wxFontFamily m_family;  ///< Family
    wxFontStyle m_style;    ///< Style
    wxFontWeight m_weight;  ///< Weight
    bool m_underlined;      ///< Underlined
    wxString m_faceName;    ///< Face Name
};

#endif  // SDK_PLUGIN_INTERFACE_FONTCONTAINER_H
