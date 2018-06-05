#include "guitest.h"
#include "gui.h"
#include <GL/glut.h>

IMPLEMENT_APP(MyApp)
  
bool MyApp::OnInit()
  // This function is automatically called when the application starts
{
  // Construct the four classes required by the innards of the logic simulator
  nmz = new names();
  netz = new network(nmz);
  dmz = new devices(nmz, netz);
  mmz = new monitor(nmz, netz);
  
  //~ bool ok = true;
  //~ dmz->makedevice(aswitch, 1, 1, ok);
  //~ dmz->makedevice(aswitch, 2, 1, ok);
  //~ devicekind k = dmz->devkind(2);
  //~ dmz->writedevice(k);
  
  
  // glutInit cannot cope with Unicode command line arguments, so we pass
  // it some fake ASCII ones instead
  char **tmp1; int tmp2 = 0; glutInit(&tmp2, tmp1);
  // Construct the GUI
  MyFrame *frame = new MyFrame(NULL, "Logic simulator", wxDefaultPosition,  wxSize(800, 600), nmz,netz, dmz, mmz);
  frame->Show(true);
  return(true); // enter the GUI event loop
  
}
