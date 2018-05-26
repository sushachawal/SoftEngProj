#include "scanner.h"
#include "names.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

scanner::scanner(names* names_mod, const char* defname)
{
	line = 0;
	c_count = 0;

	nmz = names_mod;
	eofile = false;

	clkname = nmz->lookup("CLOCK");
	swtchname = nmz->lookup("SWITCH");
	andname = nmz->lookup("AND");
    nandname = nmz->lookup("NAND");
    orname = nmz->lookup("OR");
    norname = nmz->lookup("NOR");
    dtypename = nmz->lookup("DTYPE");
    xorname = nmz->lookup("XOR");
    connectname = nmz->lookup("CONNECT");
    monitorname = nmz->lookup("MONITOR");

	inf.open(defname);

	if (!inf) {
		cout << "Error: cannot open file " << defname << " for reading" << endl;
		exit(1);
	}
    getch();
}

void scanner::getch()
{
	eofile = (!inf.get(curch));
  if (curch == '\n') {
    line++;
    c_count = 0;
    return;
  } else c_count ++;
}

void scanner::skipspaces()
{
	while (isspace(curch)) {
		getch();
	}
}

void scanner::getname(name& id)
{
	namestring str = ""; // to store the namestring

	while (!eofile) {
		if (isdigit(curch) || isalpha(curch)) {
			str += curch;
			getch();
		}
		else {
			id = nmz->lookup(str);
			return;
		}
	}
	id = nmz->lookup(str);
	return;
}

void scanner::getnumber(int& num)
{
	num = curch - '0';
	getch(); // get next character
	if(eofile) return;
	while (isdigit(curch))
	{
		num = num*10 + curch -'0';
		getch();
		if (eofile) {
			return;
		}
	}
}

void scanner::getsymbol(symbol& s, name& id, int& num)
{
	skipspaces();
	if (eofile) {
		s = eofsym;
	}
	else {
		if (isdigit(curch)) { // number
			s=numsym;
			getnumber(num);
		}
		else {
			if (isalpha(curch)) { // name
				getname(id);
				if (id == clkname || id == swtchname){
					s=gensym;
				} else{
					if (id == andname || id == nandname || id == orname || id == norname){
						s = logsym;
					} else {
						if (id == dtypename) {
							s = dtypesym;
						}else {
							if (id == xorname) {
								s = xorsym;
							}else {
								if (id == connectname) {
									s=consym;
								} else {
									if (id == monitorname) {
										s=monsym;
									} else {
										s=namesym; // not a keyword
									}
								}
							}
						}
					}
				}
			}else {
                if (curch == '-') {
                    getch();
                    if (curch == '>') {
                        s = arrowsym;
                    } else {
                        s = badsym;
                    }
                } else {
                    switch(curch) {
                        case ';': s=semicol; break;
                        case '.': s=dotsym; break;
                        default: s=badsym; break;
                    }
                }
				getch();
			}
		}
	}
}

void scanner::reporterror()
{
  string line_str;
  string out_str = "";
  string report_str = "";
  int counter;
  
  int pos = inf.tellg(); // store current positon
  inf.seekg(0, inf.beg); // go to the beginning of the file
  
  for (counter = 0; counter <= line; counter ++){
    getline(inf, line_str);
  }
  inf.seekg(pos, inf.beg); // return to position before line is read

  for (counter= 0; counter < c_count-1; counter++){
    out_str += line_str[counter];
    report_str += ' ';
  }
  report_str[c_count-2] = '^';
  cout << "Line: " << line << " Character: " << c_count << endl;
  cout << out_str << endl;
  cout << report_str << endl;
}
