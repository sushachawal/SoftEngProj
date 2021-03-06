#include "logsim.h"
#include "userint.h"
#include "gui.h"
#include <GL/glut.h>
#include <wx/intl.h> 

#define USE_GUI

IMPLEMENT_APP(MyApp)
  
bool MyApp::OnInit()
  // This function is automatically called when the application starts
{
  if (argc != 2) { // check we have one command line argument
    wcout << "Usage:      " << argv[0] << " [filename]" << endl;
    exit(1);
  }
  cout << argv[0] << endl;
  // Construct the six classes required by the innards of the logic simulator
  nmz = new names();
  netz = new network(nmz);
  dmz = new devices(nmz, netz);
  mmz = new monitor(nmz, netz);
  smz = new scanner(nmz, wxString(argv[1]).mb_str());
  pmz = new parser(netz, dmz, mmz, smz);

  if (pmz->readin ()) { // check the logic file parsed correctly
#ifdef USE_GUI
    // glutInit cannot cope with Unicode command line arguments, so we pass
    // it some fake ASCII ones instead
    wxLocale * my_locale;

	my_locale = new wxLocale;
	if(my_locale->GetSystemLanguage() == wxLANGUAGE_UNKNOWN) cout << "Loading default system lang due to invalid LANG flag" << endl;
  
	my_locale->AddCatalogLookupPathPrefix(".");
	my_locale->Init();
	if(my_locale->AddCatalog("logsim") ||  my_locale->GetSystemLanguage() == wxLANGUAGE_ENGLISH || wxLANGUAGE_ENGLISH_US);
	else cout << "\033[1;31m" << "Warning specified language is not supported! Loading default language" << "\033[0m\n" << endl;
    char **tmp1; int tmp2 = 0; glutInit(&tmp2, tmp1);
    // Construct the GUI

    MyFrame *frame = new MyFrame(NULL, _("Logic Simulator"), wxDefaultPosition,  wxSize(800, 600), nmz,netz, dmz, mmz);

    frame->Show(true);
    return(true); // enter the GUI event loop
#else
    // Construct the text-based interface
    userint umz(nmz, dmz, mmz);
    umz.userinterface();
#endif /* USE_GUI */
  }
  return(false); // exit the application
}
