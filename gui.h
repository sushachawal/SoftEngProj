#ifndef gui_h
#define gui_h

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/wfstream.h>
#include "names.h"
#include "devices.h"
#include "monitor.h"
#include "network.h"
#include "scanner.h"
#include "parser.h"
#include <vector>

enum MyEnum{
  MY_SPINCNTRL_ID = wxID_HIGHEST + 1,
  MY_OPEN,
  MY_BUTTON_ID,
  MY_BUTTON_ID2,
  HOME_BUTTON,
  MY_MAX_ID = wxID_HIGHEST + 1000,
}; // widget identifiers

class MyGLCanvas;

class MyFrame: public wxFrame
{
 public:
  MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size, 
	  names *names_mod = NULL, network *network_mod = NULL, devices *devices_mod = NULL, monitor *monitor_mod = NULL, 
	  long style = wxDEFAULT_FRAME_STYLE); // constructor
 private:
  MyGLCanvas *canvas;                     // OpenGL drawing area widget to draw traces
  wxSpinCtrl *spin;                       // control widget to select the number of cycles
  wxButton *continueButton;               // pointer to the continue button
  wxButton *homeButton;					  // pointer to the home button
  wxMenu *monitorMenu = new wxMenu;		  // pointer to the monitor menu
  wxMenu *switchMenu = new wxMenu;        // pointer to the switches menu
  names *nmz;                             // pointer to names class
  devices *dmz;                           // pointer to devices class
  monitor *mmz;                           // pointer to monitor class
  network *netz;						  // pointer to network class
  int cyclescompleted;                    // how many simulation cycles have been completed
  void runnetwork(int ncycles);           // function to run the logic network
  void OnExit(wxCommandEvent& event);     // event handler for exit menu item
  void OnAbout(wxCommandEvent& event);    // event handler for about menu item
  void OnButton(wxCommandEvent& event);   // event handler for push button
  void OnButton2(wxCommandEvent& event);  // event handler for push button 2
  void OnSpin(wxSpinEvent& event);        // event handler for spin control
  void OnCheck(wxCommandEvent& event);    // event handler for checkbox
  void OnHomeButton(wxCommandEvent &event);
  vector<int> gui_ids_signals, netw_ids_signals, dev_ids_signals;
  vector<bool> is_monitored, switch_on;
  vector<int> gui_ids_switches, netw_ids_switches;
  bool continueOn = false;
  DECLARE_EVENT_TABLE()
};
    
class MyGLCanvas: public wxGLCanvas
{
 public:
  MyGLCanvas(wxWindow *parent, wxWindowID id = wxID_ANY, monitor* monitor_mod = NULL, names* names_mod = NULL,
	     const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
	     const wxString& name = "MyGLCanvas", const wxPalette &palette=wxNullPalette); // constructor
  void Render(wxString example_text = "", int cycles = -1); // function to draw canvas contents
  void SetZoom(double z){zoom = z;}
  void SetXPan(int px){pan_x = px;}
  void SetYPan(int py){pan_y = py;}
  void SetInit(bool in){init = in;}
 private:
  int w, h;							 // Window width and height
  
  // Draw the monitor signal with index monum 
  // starting at vertical postion y
  void DrawMonSig(float y, float gap, int monnum, int cyclesdisplayed); 
  
  // Draws the monitor label from the list of monitors
  void DrawMonLabel(float x, float y, int monnum);
  
  //Converts a 2 digit into a string to draw with OpenGL
  string inttostring(int i);
  
  wxGLContext *context;              // OpenGL rendering context
  bool init;                         // has the OpenGL context been initialised?
  int pan_x;                         // the current x pan
  int pan_y;                         // the current y pan
  double zoom;                       // the current zoom
  int cyclesdisplayed;               // how many simulation cycles have been displayed
  monitor *mmz;                      // pointer to monitor class, used to extract signal traces
  names *nmz;                        // pointer to names class, used to extract signal names
  void InitGL();                     // function to initialise OpenGL context
  void OnSize(wxSizeEvent& event);   // event handler for when canvas is resized
  void OnPaint(wxPaintEvent& event); // event handler for when canvas is exposed
  void OnMouse(wxMouseEvent& event); // event handler for mouse events inside canvas
  DECLARE_EVENT_TABLE()
};   
 
#endif /* gui_h */
