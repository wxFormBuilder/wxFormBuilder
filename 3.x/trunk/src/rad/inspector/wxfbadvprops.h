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
// Modified by
//   Andrea Zanellato - zanellato.andrea@gmail.com
//   This code was part of previous objtree.cpp version.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __WXFBADVPROPS_H__
#define __WXFBADVPROPS_H__

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/propdev.h>
#include <wx/propgrid/advprops.h>

#include "fontcontainer.h"

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
    virtual ~wxFBSizeProperty();

#if wxVERSION_NUMBER < 2900
    virtual void
#else
    virtual wxVariant
#endif
    ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const;

    virtual void RefreshChildren();

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
    virtual ~wxFBPointProperty();

#if wxVERSION_NUMBER < 2900
    virtual void
#else
    virtual wxVariant
#endif
    ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const;

    virtual void RefreshChildren();

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

    virtual ~wxFBBitmapProperty();

    wxPGProperty *CreatePropertySource( int sourceIndex = 0 );
    wxPGProperty *CreatePropertyFilePath() ;
    wxPGProperty *CreatePropertyResourceName();
    wxPGProperty *CreatePropertyIconSize();
    wxPGProperty *CreatePropertyArtId();
    wxPGProperty *CreatePropertyArtClient();

    wxString SetupImage( const wxString &imgPath = wxEmptyString ) ;
    wxString SetupResource( const wxString &resName = wxEmptyString ) ;
	
	int prevSrc;
	void SetPrevSource(int src){prevSrc = src;}

#if wxVERSION_NUMBER < 2900
    virtual void
#else
    virtual wxVariant
#endif
    ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const;
	
    virtual void OnSetValue();
	void CreateChildren();

	void UpdateChildValues(const wxString& value);
	
	wxString GetValueAsString( int argFlags =0 ) const;

protected:

	void GetChildValues( const wxString& parentValue, wxArrayString& childValues ) const;

#if wxVERSION_NUMBER < 2900
    static wxPGChoices m_ids;
    static wxPGChoices m_clients;
    wxPGChoices m_strings;
#else
    static wxArrayString m_ids;
    static wxArrayString m_clients;
    wxArrayString m_strings;
#endif

	
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
#include <wx/slider.h>

class wxPGSliderEditor : public wxPGEditor
{
#if wxVERSION_NUMBER < 2900
    WX_PG_DECLARE_EDITOR_CLASS( wxPGSliderEditor )
#else
    wxDECLARE_DYNAMIC_CLASS( wxPGSliderEditor );
#endif
public:
    wxPGSliderEditor()
    :
    m_max( 10000 )
    {
    }

    virtual ~wxPGSliderEditor();
#if wxVERSION_NUMBER < 2900
    // See below for short explanations of what these are supposed to do.
    wxPG_DECLARE_CREATECONTROLS
#else
    virtual wxPGWindowList CreateControls(wxPropertyGrid* propgrid,
                                          wxPGProperty* property,
                                          const wxPoint& pos,
                                          const wxSize& size) const;
#endif
    virtual void UpdateControl( wxPGProperty* property, wxWindow* wnd ) const;
    virtual bool OnEvent( wxPropertyGrid* propgrid, wxPGProperty* property,
                          wxWindow* wnd, wxEvent& event ) const;
    virtual bool GetValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* ctrl ) const;
    virtual void SetValueToUnspecified( wxPGProperty* property, wxWindow* ctrl ) const;

private:
    int m_max;
};
#endif // wxUSE_SLIDER

// -----------------------------------------------------------------------
// wxFBFontProperty
// -----------------------------------------------------------------------

class WXDLLIMPEXP_PG wxFBFontProperty : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxFBFontProperty)
public:

    wxFBFontProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxFontContainer& value = *wxNORMAL_FONT);
    virtual ~wxFBFontProperty();
	
#if wxVERSION_NUMBER < 2900
    virtual void
#else
    virtual wxVariant
#endif
    ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const;

    virtual void RefreshChildren();
	
    virtual void OnSetValue();
    virtual wxString GetValueAsString( int argFlags = 0 ) const;

    WX_PG_DECLARE_EVENT_METHODS()
};

#endif //__WXFBADVPROPS_H__
