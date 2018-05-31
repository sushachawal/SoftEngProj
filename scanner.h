#ifndef scanner_h
#define scanner_h

#include "names.h"
#include <iostream>
#include <fstream>


using namespace std;

/* Symbol specification */

typedef enum {gensym, andsym, nandsym, orsym, norsym, dtypesym, xorsym, consym, monsym,
  namesym, numsym, eofsym, semicol, dotsym, arrowsym, badsym} symbol;

class scanner {

	char curch; // current character
	bool eofile; // true when end of file has been reached
	ifstream inf; // input file to be read from
	// names* nmz;

	name clkname;
	name swtchname;
	name andname;
  name nandname;
  name orname;
  name norname;
  name dtypename;
  name xorname;
  name connectname;
  name monitorname;

public:
	names* nmz;

	int line; // line counter
	int c_count; // character counter, reset every line
		
	// this is the constructor of the scanner class
	scanner(names* names_mod,     // pointer to names class, so scanner can use it
        const char* defname); // name of file from which to read the definitions

	~scanner(){}; // destructor of the class, closes input file

	void getch(); // set character to next character
	void skipspaces(); // set character to next non-space character
	void getname(name& id); // scans the string, adds to names_mod if not already there and returns name id
	void getnumber(int& num); // scans the number in the file and returns it's value
	void getsymbol(symbol& s, // the sort of symbol read from the file
				   name&  id, // return the name id here if it is a name
				   int& num); // return the value here if it is a number
  void reporterror(); // report location of the error
};

#endif /* scanner_h */
