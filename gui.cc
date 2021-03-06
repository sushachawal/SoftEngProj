	#include "gui.h"
#include <GL/glut.h>
//#include "wx_icon.xpm"
#include <iostream>
#include <cmath>


using namespace std;

// /MyGLCanvas ////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(MyGLCanvas, wxGLCanvas)
  EVT_SIZE(MyGLCanvas::OnSize)
  EVT_PAINT(MyGLCanvas::OnPaint)
  EVT_MOUSE_EVENTS(MyGLCanvas::OnMouse)
END_EVENT_TABLE()
  
int wxglcanvas_attrib_list[5] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};

MyGLCanvas::MyGLCanvas(wxWindow *parent, wxWindowID id, monitor* monitor_mod, names* names_mod, const wxPoint& pos, 
		       const wxSize& size, long style, const wxString& name, const wxPalette& palette):
  wxGLCanvas(parent, id, wxglcanvas_attrib_list, pos, size, style, name, palette)
  // Constructor - initialises private variables
{
  context = new wxGLContext(this);
  mmz = monitor_mod;
  nmz = names_mod;
  init = false;
  pan_x = 0;
  pan_y = 0;
  zoom = 1.0;
  cyclesdisplayed = -1;
  GetClientSize(&w, &h);
}

/***********************************************************************
 * Authors: Sushant Achawal and Matt Ashman
 * 
 * Draws a monitor signal at position x, y -
 * The number of cycles is passed as a parameter and the monitor
 * trace for the monitor number passed in, is displayed. Includes axes
 * which scale on window resizing and graph ticks which vary on window
 * resizing and cylces displayed. The height each monitor signal
 * occupies is also passed in as a parameter.
 *
 * @params: (x,y) of bottom left of monitor window,
 *          gap: height of monitor window,
 *          monnum: The index for the monitor being drawn
 *          cyclesdisplayed: No. of cycles to display
 */
void MyGLCanvas::DrawMonSig(float y, float gap, int monnum, int cyclesdisplayed)
{
  //Draw the axes:
  float vert0;
  float vert1;
  if (gap<60){
		vert0 = y + 0.3*gap;
		vert1 = y + 0.7*gap;
  } else{
		vert0 = y + 0.2*gap;
		vert1 = y + 0.8*gap;
  }


  DrawMonLabel(0 + w*0.1,vert1 + (vert1-vert0)*0.1,monnum);
  float cycle_gap = 0.75*w/cyclesdisplayed;
  float sigheight = (vert1-vert0)*0.9;
  
  //Draw Horizontal Axis
  glColor3f(0.0, 0.0, 0.0);
  glBegin(GL_LINE_STRIP);
  glVertex2f(0 + w*0.1, vert0);
  glVertex2f(w - w*0.1, vert0);
  glEnd();
  int every;
  if (cyclesdisplayed < 2){
		every = 1;
	} else {
		every = pow(10.0, floor(log10(cyclesdisplayed/2)));
	}
	if (w<=600) {
		every = every*2;
	}
  /*
  if(cyclesdisplayed>=25 && w <= 1370) {
		every = 10; 
	}
  else if(w <= 600 && cyclesdisplayed>=15) {
		every = 10;
	}
	* */
  for (int i=0; i<=cyclesdisplayed; i+=every){
		// Always show final tick.
		if (cyclesdisplayed-i < 0.5*every){
			i = cyclesdisplayed;						
		}
		else if (cyclesdisplayed - i < every){
			every = cyclesdisplayed - i;
		}
		glColor3f(0.87, 0.87, 0.87);
		glBegin(GL_LINE_STRIP);
		glVertex2f(0 + w*0.1 + cycle_gap*i, vert0);
		glVertex2f(0 + w*0.1 + cycle_gap*i, vert0+sigheight);
		glEnd();
		string number = to_string(i);
		glColor3f(0.0, 0.0, 1.0);
		glRasterPos2f(0 + w*0.1 + cycle_gap*i, vert0 - 10);
		for(int j =0; j<number.size();j++){
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10 , number[j]);
		}
  }
  
  //Draw Horizontal Axis Arrow
  glBegin(GL_TRIANGLES);
  glColor3f(0,0,0);
  glVertex2f(w - w*0.1, vert0 - gap*0.03);
  glVertex2f(w - w*0.1 + gap*0.03*0.7071,vert0);
  glVertex2f(w - w*0.1, vert0 + gap*0.03);
  glEnd();
  
  //Draw Vertical Axis:
  glBegin(GL_LINE_STRIP);
  glVertex2f(0 + w*0.1, vert0);
  glVertex2f(0 + w*0.1, vert1);
  glEnd();

  //Draw Vertical Axis Arrow
  glBegin(GL_TRIANGLES);
  glColor3f(0,0,0);
  glVertex2f(0 + w*0.1 + gap*0.03, vert1);
  glVertex2f(0 + w*0.1 , vert1 + gap*0.03*0.7071);
  glVertex2f(0 + w*0.1 - gap*0.03, vert1);
  glEnd();
  
  //Draw Signal
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_LINE_STRIP);
  asignal s;
  float vert;
  for (int i=0; i<cyclesdisplayed; i++) {
    if (mmz->getsignaltrace(monnum, i, s)) {
      if (s==low) vert = vert0;
      if (s==high) vert = vert0 + sigheight;
      glVertex2f(0 + w*0.1 + cycle_gap*i, vert); 
      glVertex2f(0 + w*0.1 + cycle_gap*(i+1), vert);
    }
  }
  glEnd();
  
}

/***********************************************************************
 * Authors: Sushant Achawal and Matt Ashman
 * 
 * Draws the monitor name at position x, y i.e. labels vertical axis
 *
 * @params: (x,y) of position to draw monitor name,
 *          monnum: The index for the monitor being drawn
 */
void MyGLCanvas::DrawMonLabel(float x, float y, int monnum){
  name monname, pin;
  mmz-> getmonname(monnum, monname, pin);
  namestring monnamestring = nmz->getname(monname);
  namestring pinstring = nmz->getname(pin);
  namestring monlabel = monnamestring;
  if(pinstring != "") monlabel += '.' + pinstring;
  //if(monnamestring == NULL) cout << "Error attempting to access non-existing monitor" << endl; return;
  glColor3f(0.0, 0.0, 1.0);
  glRasterPos2f(x, y);
  for (int i = 0; i < monlabel.size(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10 , monlabel[i]);
}

/***********************************************************************
 * Author: Sushant Achawal
 * 
 * Draws canvas contents -
 * When the simulator is run, the number of cycles is passed as a parameter and the monitor
 * traces are displayed.
 *
 * @param cycles
 *
 */
void MyGLCanvas::Render(int cycles)
{
  float y;
  unsigned int i;
  asignal s;
  int nmons;
  
  GetClientSize(&w, &h);
  
  if (cycles >= 0) cyclesdisplayed = cycles;

  SetCurrent(*context);
  if (!init) {
    InitGL();
    init = true;
  }
  glClear(GL_COLOR_BUFFER_BIT);
  
  int mcount;
  mcount = mmz->moncount();
  //mcount = 10;
  if ((cyclesdisplayed >= 0) && (mcount > 0)) { // draw the first monitor signal, get trace from monitor class
	
	float gap = h/mcount;
	for(int i = 0; i < mcount; i++){
	  DrawMonSig(i*gap, gap, i, cyclesdisplayed);
	}

  }


  // We've been drawing to the back buffer, flush the graphics pipeline and swap the back buffer to the front
  glFlush();
  SwapBuffers();
}

void MyGLCanvas::InitGL()
  // Function to initialise the GL context
{
  //int w, h;
  GetClientSize(&w, &h);
  SetCurrent(*context);
  glDrawBuffer(GL_BACK);
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glViewport(0, 0, (GLint) w, (GLint) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1, 1); 
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(pan_x, pan_y, 0.0);
  if (zoom*w < 230) {
		zoom = 230.0/w;
	} else if (zoom*h < 315) {
		zoom = 315.0/h;
	}
  glScaled(zoom, zoom, zoom);
}

void MyGLCanvas::OnPaint(wxPaintEvent& event)
  // Event handler for when the canvas is exposed
{
  //int w, h;
  wxString text;

  wxPaintDC dc(this); // required for correct refreshing under MS windows
  GetClientSize(&w, &h);
  Render();
}

void MyGLCanvas::OnSize(wxSizeEvent& event)
  // Event handler for when the canvas is resized
{
  init = false; // this will force the viewport and projection matrices to be reconfigured on the next paint
}

void MyGLCanvas::OnMouse(wxMouseEvent& event)
  // Event handler for mouse events inside the GL canvas
{
  wxString text;
  //int w, h;;
  static int last_x, last_y;

  GetClientSize(&w, &h);
  if (event.ButtonDown()) {
    last_x = event.m_x;
    last_y = event.m_y;
  }
  if (event.Dragging()) {
    pan_x += event.m_x - last_x;
    pan_y -= event.m_y - last_y;
    last_x = event.m_x;
    last_y = event.m_y;
    init = false;
  }
  if (event.GetWheelRotation() < 0) {
    zoom = zoom * (1.0 - (double)event.GetWheelRotation()/(20*event.GetWheelDelta()));
    init = false;
  }
  if (event.GetWheelRotation() > 0) {
    zoom = zoom / (1.0 + (double)event.GetWheelRotation()/(20*event.GetWheelDelta()));
    init = false;
  }

  if (event.GetWheelRotation() || event.ButtonDown() || event.ButtonUp() || event.Dragging() || event.Leaving()) Render();
}

// MyFrame ///////////////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  //EVT_MENU(MY_OPEN, MyFrame::OnOpen)
  EVT_BUTTON(MY_BUTTON_ID, MyFrame::OnButton)
  EVT_BUTTON(MY_BUTTON_ID2, MyFrame::OnButton2)
  EVT_BUTTON(HOME_BUTTON, MyFrame::OnHomeButton)
  EVT_SPINCTRL(MY_SPINCNTRL_ID, MyFrame::OnSpin)
END_EVENT_TABLE()
  
MyFrame::MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
		 names *names_mod, network *network_mod, devices *devices_mod, monitor *monitor_mod, long style):
  wxFrame(parent, wxID_ANY, title, pos, size, style)
  // Constructor - initialises pointers to names, devices and monitor classes, lays out widgets
  // using sizers
{
  //SetIcon(wxIcon(wx_icon));

  cyclescompleted = 0;
  nmz = names_mod;
  netz = network_mod;
  dmz = devices_mod;
  mmz = monitor_mod;
  if (nmz == NULL || netz ==NULL || dmz == NULL || mmz == NULL) {
    cout << "Cannot operate GUI without names, network, devices and monitor classes" << endl;
    exit(1);
  }
  
  wxMenu *fileMenu = new wxMenu;
  fileMenu->Append(wxID_ABOUT, _("&About"));
  fileMenu->Append(wxID_EXIT, _("&Quit"));
  
  
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, _("&File"));
  
  
  int gui_sig_index=0, device_id, input_id, output_id, GUI_ID, offset, mon_index, mon_sig_id, mon_dev_id;
  string nameOfDevice, nameOfInput, nameOfOutput, monitorLabel;

  offset = MY_BUTTON_ID2 + 3;
  devlink d;
  inplink i;
  outplink o;
  for (d = netz->devicelist(); d != NULL; d = d->next){
	  device_id = d->id;
	  nameOfDevice = nmz->getname(device_id);
	  
	  for(i = d->ilist; i!=NULL; i = i->next){
			input_id = i->id;
			nameOfInput = nmz->getname(input_id);
			GUI_ID = gui_sig_index + offset;
			gui_ids_signals.push_back( GUI_ID);
			netw_ids_signals.push_back(input_id);
			dev_ids_signals.push_back(device_id);
			is_monitored.push_back(false);
			monitorLabel = nameOfDevice;
			if(nameOfInput != "") monitorLabel += ("." +nameOfInput);
			monitorMenu->AppendCheckItem(gui_ids_signals[gui_sig_index], monitorLabel);
			
			for(mon_index =0; mon_index < mmz->moncount();mon_index++){
				mmz->getmonname(mon_index, mon_dev_id, mon_sig_id);
				if(mon_dev_id == device_id && mon_sig_id == input_id){
					monitorMenu->Check(gui_ids_signals[gui_sig_index], true);
					is_monitored[gui_sig_index] = true;
				}
			}
			
			gui_sig_index++;
		  }
	  
	  for(o = d->olist; o!=NULL; o = o->next){
			output_id = o->id;
			nameOfOutput = nmz->getname(output_id);
			GUI_ID = gui_sig_index + offset;
			gui_ids_signals.push_back( GUI_ID);
			netw_ids_signals.push_back(output_id);
			dev_ids_signals.push_back(device_id);
			is_monitored.push_back(false);
			monitorLabel = nameOfDevice;
			if(nameOfOutput != "") monitorLabel += ("." + nameOfOutput);
			monitorMenu->AppendCheckItem(gui_ids_signals[gui_sig_index], monitorLabel);
			
			for(mon_index =0; mon_index < mmz->moncount();mon_index++){
				mmz->getmonname(mon_index, mon_dev_id, mon_sig_id);
				if(mon_dev_id == device_id && mon_sig_id == output_id){
					monitorMenu->Check(gui_ids_signals[gui_sig_index], true);
					is_monitored[gui_sig_index] = true;
				}
			}
			gui_sig_index++;
		  }
	  }
  
  
  menuBar->Append(monitorMenu, _("&Monitors"));
  

  devicekind kindDevice;
  offset = offset + gui_sig_index + 3;
  int gui_sw_index=0; 
  for (d = netz->devicelist(); d != NULL; d = d->next){
	  device_id = d->id;
	  nameOfDevice = nmz->getname(device_id);
	  kindDevice = (netz->finddevice(device_id))->kind;
	  if(kindDevice == aswitch){
		  GUI_ID = gui_sw_index + offset;
		  gui_ids_switches.push_back( GUI_ID);
		  netw_ids_switches.push_back(device_id);
		  switch_on.push_back(false);
		  switchMenu->AppendCheckItem(gui_ids_switches[gui_sw_index], nameOfDevice);
		  if(d->swstate == high){
			  switchMenu->Check(gui_ids_switches[gui_sw_index], true);
			  switch_on[gui_sw_index] = true;
			  }
          if(d->swstate == low){
			  switchMenu->Check(gui_ids_switches[gui_sw_index], false);
			}
			gui_sw_index++;
			
		  }	  
	  }
  
  
  menuBar->Append(switchMenu, _("&Switches"));
  

  SetMenuBar(menuBar);

  wxBoxSizer *topsizer = new wxBoxSizer(wxHORIZONTAL);
  canvas = new MyGLCanvas(this, wxID_ANY, monitor_mod, names_mod);
  topsizer->Add(canvas, 1, wxEXPAND | wxALL, 10);

  wxBoxSizer *button_sizer = new wxBoxSizer(wxVERTICAL);
  button_sizer->Add(new wxButton(this, MY_BUTTON_ID, _("Run")), 0, wxALL, 10);
  continueButton = new wxButton(this, MY_BUTTON_ID2, _("Continue"));
  homeButton = new wxButton(this, HOME_BUTTON, _("Reset View"));
  button_sizer->Add(continueButton, 0, wxALL, 10);
  button_sizer->Add(homeButton,0, wxALL, 10);
  button_sizer->Add(new wxStaticText(this, wxID_ANY, _("Cycles")), 0, wxTOP|wxLEFT|wxRIGHT, 10);
  spin = new wxSpinCtrl(this, MY_SPINCNTRL_ID, wxString("10"));
  spin->SetRange(0, maxcycles);
  button_sizer->Add(spin, 0 , wxALL, 10);
  
  topsizer->Add(button_sizer, 0, wxALIGN_CENTER);

  SetMinSize(wxSize(400,400));
  SetSizer(topsizer);
  //SetSizeHints(400, 400);
  
}

void MyFrame::OnExit(wxCommandEvent &event)
  // Event handler for the exit menu item
{
  Close(true);
}

void MyFrame::OnAbout(wxCommandEvent &event)
  // Event handler for the about menu item
{
  wxMessageDialog about(this, _("Logic Simulator\nAuthors: Vlad Tomescu, Matt Ashman, Sushant Achawal\nMay 2018"), _("About Logic Simulator."), wxICON_INFORMATION | wxOK);
  about.ShowModal();
}

void MyFrame::OnButton(wxCommandEvent &event)
  // Event handler for the push button
{
  int n, ncycles, index, num_monitors, dev_id_delete, sig_id_delete, monindex = 0;
  bool ischecked, ok, run = true;
  
  while(mmz->moncount() > 0){
	mmz->getmonname(0, dev_id_delete, sig_id_delete);
	mmz->remmonitor(dev_id_delete, sig_id_delete, ok);
  }
  
  for(index = 0; index < gui_ids_signals.size(); index++){
	ischecked = monitorMenu->IsChecked(gui_ids_signals[index]);
	is_monitored[index] = false;
	
	if(ischecked && run){
		if(mmz->moncount() < 10){
			mmz->makemonitor(dev_ids_signals[index], netw_ids_signals[index], ok);
			is_monitored[index] = true;
		}
		else{
			run = false;		
			wxMessageBox( _("Please select a maximum of 10 monitors") );
		}
	}
  }
  
  
  for(index = 0; index < gui_ids_switches.size(); index++){
	switch_on[index] = false;
	ischecked = switchMenu->IsChecked(gui_ids_switches[index]);
	if(ischecked){
	dmz->setswitch(netw_ids_switches[index], high, ok);
	switch_on[index] = true;
	}
	else{
	dmz->setswitch(netw_ids_switches[index], low, ok);
	}
	
  }
  
  
  if(run){
	continueOn = true;
    cyclescompleted = 0;
    mmz->resetmonitor ();
    dmz->initdevices();
    runnetwork(spin->GetValue());
    canvas->Render(cyclescompleted);
  }
}

void MyFrame::OnButton2(wxCommandEvent &event)
  // Event handler for the continue button
{
  int n, ncycles, index, nummons = 0, numsw = 0, maxval = maxcycles;
  bool ischecked, ok, ismon, ishigh;
  
  for(index = 0; index < gui_ids_signals.size(); index++){
	ischecked = monitorMenu->IsChecked(gui_ids_signals[index]);
	ismon = is_monitored[index];
	if(ischecked != ismon){
		continueOn = false;
	}
  }
  
  
  for(index = 0; index < gui_ids_switches.size(); index++){
	switch_on[index] = false;
	ischecked = switchMenu->IsChecked(gui_ids_switches[index]);
	if(ischecked){
	dmz->setswitch(netw_ids_switches[index], high, ok);
	switch_on[index] = true;
	}
	else{
	dmz->setswitch(netw_ids_switches[index], low, ok);
	}
  }
  
  
  
  if(continueOn){
	runnetwork(spin->GetValue());
	canvas->Render(cyclescompleted);
	if(cyclescompleted >= maxval){
		wxMessageBox( _("Maximum number of cycles reached") );
	}
  }
  else{
    wxMessageBox( _("Modifications made/Nothing to continue. Press run first") );
  }
  
}

/***********************************************************************
 * Author: Sushant Achawal
 * 
 * Event handler for pressing "Reset View" -
 * Resets view to default view on starting the application.
 *
 */
void MyFrame::OnHomeButton(wxCommandEvent &event)

{
  canvas->SetXPan(0);
  canvas->SetYPan(0);
  canvas->SetZoom(1.0);
  canvas->SetInit(false);
  canvas->Render();
}

void MyFrame::OnSpin(wxSpinEvent &event)
  // Event handler for the spin control
{
  wxString text;

  text.Printf("New spinctrl value %d", event.GetPosition());
}



void MyFrame::runnetwork(int ncycles)
  // Function to run the network, derived from corresponding function in userint.cc
{
  bool ok = true;
  int n = 0, maxval = maxcycles;

  while ((n < ncycles) && ok && cyclescompleted + n < maxval) {
    dmz->executedevices (ok);
    if (ok) {
      n++;
      mmz->recordsignals ();
    } else
      cout << "Error: network is oscillating" << endl;
  }
  if (ok) cyclescompleted = cyclescompleted + n;
  else cyclescompleted = 0;
}
