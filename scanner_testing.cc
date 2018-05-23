#include "scanner.h"
#include "names.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

int main(int argc, char **argv)
// If this program is put in a file called small.cc, it can
// be compiled with the command  g++ -g -o small small.cc
{

  if (argc != 2) {
    cout << "Usage:         " << argv[0] << " [filename]" << endl;
    exit(1);
  }
  
  names *nmz;
  scanner *smz; 

  nmz = new names();
  smz = new scanner(nmz, argv[1]);
  name test_id = -1;
  int test_num = -1;
  scanner::symbol s;
  
  for (int i=0; i<9; i++) {
	  smz->getsymbol(s, test_id, test_num);
	  cout << s << endl;
  }
  nmz->printall();
}
