/*
* This file was copied from Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* Revision: 4204
* Id: annoyingdialog.cpp 4204 2007-07-02 12:30:47Z mandrav
* HeadURL: http://svn.berlios.de/svnroot/repos/codeblocks/trunk/src/sdk/annoyingdialog.cpp
*/

#include <wx/dialog.h>
#include <wx/string.h>
#include <wx/artprov.h>

class wxCheckBox;

/**
Dialog that contains a "Don't annoy me" checkbox.

Using this dialog, the user can select not to display this dialog again.
The dialog can be then re-enabled in the settings
*/
class AnnoyingDialog : public wxDialog
{
    public:
        enum dStyle
        {
            OK,
            YES_NO,
            YES_NO_CANCEL,
            OK_CANCEL,
            ONE_BUTTON,
            TWO_BUTTONS,
            THREE_BUTTONS
        };

        AnnoyingDialog(const wxString& caption, const wxString& message, const wxArtID icon = wxART_INFORMATION,
                       dStyle style = YES_NO, int defaultReturn = wxID_YES, bool separate = true,
                       const wxString& b1 = wxEmptyString, const wxString& b2 = wxEmptyString, const wxString& b3 = wxEmptyString);
        virtual ~AnnoyingDialog(){}
        virtual int ShowModal();
    private:
        void OnButton( wxCommandEvent& event );
        wxCheckBox* m_cb;
        bool m_dontAnnoy;
        int m_defRet;
        DECLARE_EVENT_TABLE()
};
