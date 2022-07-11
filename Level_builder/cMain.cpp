#include "cMain.h"

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_BUTTON(10001, on_button_click)
wxEND_EVENT_TABLE()

cMain::cMain()
	:
	wxFrame(nullptr, wxID_ANY, "Level Builder")
{
	
}

cMain::~cMain()
{

}

void cMain::on_button_click(wxCommandEvent& evt)
{
}
