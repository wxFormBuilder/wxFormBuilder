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
// Modified by
//   Andrea Zanellato - zanellato.andrea@gmail.com
//   This code was part of previous objtree.cpp version.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __WXFBADVPROPS_H__
#define __WXFBADVPROPS_H__

#include "fontcontainer.h"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

// -----------------------------------------------------------------------
// wxFBSizeProperty
// -----------------------------------------------------------------------

class wxFBSizeProperty : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS( wxFBSizeProperty )
public:
    wxFBSizeProperty( const wxString& label = wxPG_LABEL,
                      const wxString& name  = wxPG_LABEL,
                      const wxSize&   value = wxSize() );

	wxVariant ChildChanged(wxVariant& thisValue, int childIndex,
	                       wxVariant& childValue) const override;

	void RefreshChildren() override;

protected:
    void DoSetValue( const wxSize& value ) { m_value = WXVARIANT( value ); }
};

// -----------------------------------------------------------------------
// wxFBPointProperty
// -----------------------------------------------------------------------

class wxFBPointProperty : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS( wxFBPointProperty )
public:
    wxFBPointProperty( const wxString& label = wxPG_LABEL,
                       const wxString& name  = wxPG_LABEL,
                       const wxPoint&  value = wxPoint() );
	~wxFBPointProperty() override;

	wxVariant ChildChanged(wxVariant& thisValue, int childIndex,
	                       wxVariant& childValue) const override;

	void RefreshChildren() override;

protected:
    void DoSetValue( const wxPoint& value ) { m_value = WXVARIANT( value ); }
};

// -----------------------------------------------------------------------
// wxFBBitmapProperty
// -----------------------------------------------------------------------

class wxFBBitmapProperty : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS( wxFBBitmapProperty )

public:
    wxFBBitmapProperty( const wxString& label = wxPG_LABEL,
                        const wxString& name  = wxPG_LABEL,
                        const wxString& value = wxString() );

	~wxFBBitmapProperty() override;

    wxPGProperty *CreatePropertySource( int sourceIndex = 0 );
    wxPGProperty *CreatePropertyFilePath() ;
    wxPGProperty *CreatePropertyResourceName();
    wxPGProperty *CreatePropertyIconSize();
	wxPGProperty *CreatePropertyXrcName();
    wxPGProperty *CreatePropertyArtId();
    wxPGProperty *CreatePropertyArtClient();

    wxString SetupImage( const wxString &imgPath = wxEmptyString ) ;
    wxString SetupResource( const wxString &resName = wxEmptyString ) ;

	int prevSrc;
	void SetPrevSource(int src){prevSrc = src;}

	wxVariant ChildChanged(wxVariant& thisValue, int childIndex,
	                       wxVariant& childValue) const override;

	void OnSetValue() override;
	void CreateChildren();

	void UpdateChildValues(const wxString& value);
protected:

	void GetChildValues( const wxString& parentValue, wxArrayString& childValues ) const;

    static wxArrayString m_ids;
    static wxArrayString m_clients;
    wxArrayString m_strings;


};

// -----------------------------------------------------------------------
// wxSlider-based property editor
// -----------------------------------------------------------------------

#if wxUSE_SLIDER
//
// Implement an editor control that allows using wxSlider to edit value of
// wxFloatProperty (and similar).
//
// Note that new editor classes needs to be registered before use.
// This can be accomplished using wxPGRegisterEditorClass macro.
// Registeration can also be performed in a constructor of a
// property that is likely to require the editor in question.
//

class wxPGSliderEditor : public wxPGEditor
{
    wxDECLARE_DYNAMIC_CLASS( wxPGSliderEditor );
public:
    wxPGSliderEditor()
    :
    m_max( 10000 )
    {
    }

	~wxPGSliderEditor() override;
	wxPGWindowList CreateControls(wxPropertyGrid* propgrid, wxPGProperty* property,
	                              const wxPoint& pos, const wxSize& size) const override;
	void UpdateControl(wxPGProperty* property, wxWindow* wnd) const override;
	bool OnEvent(wxPropertyGrid* propgrid, wxPGProperty* property, wxWindow* wnd,
	             wxEvent& event) const override;
	bool GetValueFromControl(wxVariant& variant, wxPGProperty* property,
	                         wxWindow* ctrl) const override;
	void SetValueToUnspecified(wxPGProperty* property, wxWindow* ctrl) const override;

private:
    int m_max;
};
#endif // wxUSE_SLIDER

// -----------------------------------------------------------------------
// wxFBFontProperty
// -----------------------------------------------------------------------

class wxFBFontProperty : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxFBFontProperty)
public:

    wxFBFontProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxFontContainer& value = *wxNORMAL_FONT);
	~wxFBFontProperty() override;

	wxVariant ChildChanged(wxVariant& thisValue, int childIndex,
	                       wxVariant& childValue) const override;

	void RefreshChildren() override;

	void OnSetValue() override;
	wxString GetValueAsString(int argFlags = 0) const override;

	bool OnEvent(wxPropertyGrid* propgrid, wxWindow* primary, wxEvent& event) override;
};

#endif //__WXFBADVPROPS_H__
