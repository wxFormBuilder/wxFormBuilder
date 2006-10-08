#ifndef MAIN_H
#define MAIN_H

#include "app.h"
#include "wxfbTest_GUI.h"

class MyFrame: public MainFrame
{
	public:
		MyFrame();
		~MyFrame();
	private:
		void OnQuit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		DECLARE_EVENT_TABLE();
};

#endif // MAIN_H
