#pragma once

#include "wx/wx.h"

class cMain : public wxFrame
{
public:
	cMain();
	~cMain();

private:
	int field_width_ = 20;
	int field_height_ = 8;
	wxButton **btns_	{nullptr};

	void on_button_click(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();
};

