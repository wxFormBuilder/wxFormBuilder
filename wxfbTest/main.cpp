#include "main.h"
#include <wx/image.h>

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_EXIT, MyFrame::OnQuit)
    EVT_MENU(ID_ABOUT, MyFrame::OnAbout)
END_EVENT_TABLE()

MyFrame::MyFrame() : MainFrame( NULL, wxID_ANY )
{
	::wxInitAllImageHandlers();
	new MainPanel( this );
}

MyFrame::~MyFrame()
{
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

