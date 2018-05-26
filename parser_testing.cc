#include "parser.h"
#include "names.h"
#include "scanner.h"

using namespace std;

int main(int argc, char **argv){
  if (argc != 2) {
    cout << "Usage:         " << argv[0] << " [filename]" << endl;
    exit(1);
  }

  names *nmz;
  scanner *smz;
  parser *prsr;

  nmz = new names();
  smz = new scanner(nmz, argv[1]);
  prsr = new parser(smz);
  cout << prsr -> readin() << endl;
}
