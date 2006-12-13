#ifndef INNERFRAMETESTPANEL_H
#define INNERFRAMETESTPANEL_H

#include "resizablepanel.h"

class wxNativeInnerFrame : public ResizablePanel
{
	DECLARE_CLASS( wxNativeInnerFrame )
public:
	wxNativeInnerFrame( wxWindow* parent, wxWindowID id = -1, wxString title = wxEmptyString, wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize, long frameStyle = wxDEFAULT_FRAME_STYLE );
	~wxNativeInnerFrame();
	void Recreate();
	wxPanel* GetFrameContentPanel();
	void SetFrameContentPanel( wxPanel* panel );
	void ShowTitleBar( bool show );
	void SetTitle( const wxString& title );
	void SetStyle( long frameStyle );
protected:
	void OnErase( wxEraseEvent& event );
	void OnSize( wxCommandEvent& event );
	void OnPaint( wxPaintEvent& event );


	wxBitmap m_background;
	wxString m_title;
	bool m_showTitleBar;
	long m_frameStyle;
	wxPanel* m_frameContent;

	wxBoxSizer* m_vertSizer;
	wxBoxSizer* m_horiSizer;

	DECLARE_EVENT_TABLE();
};

#endif //INNERFRAMETESTPANEL_H
