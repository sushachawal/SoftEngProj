#include "parser.h"
#include "names.h"
#include "scanner.h"
#include "userint.h"

using namespace std;

int main(int argc, char **argv){
  if (argc != 2) {
    cout << "Usage:         " << argv[0] << " [filename]" << endl;
    exit(1);
  }

  names *nmz;
  network *netz;
  devices *dmz;
  monitor *mmz;
  scanner *smz;
  parser *prsr;

  // Construct the six classes required by the innards of the logic simulator
  nmz = new names();
  netz = new network(nmz);
  dmz = new devices(nmz, netz);
  mmz = new monitor(nmz, netz);
  smz = new scanner(nmz, argv[1]);
  prsr = new parser(netz, dmz, mmz, smz);

  bool okprsr =prsr->readin();
  bool oknetz;
  netz->checknetwork(oknetz);
  cout << "File read in?: " <<  okprsr << endl;
  cout << "Everything connected?: " << oknetz << endl;

  // Construct the text-based interface
  if (okprsr && oknetz){
		userint umz(nmz, dmz, mmz);
		umz.userinterface();
	}

  return 0;
}
