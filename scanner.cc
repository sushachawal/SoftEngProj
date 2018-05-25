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

	clkname = nmz->lookup("CLK");
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
	c_count ++;
	//getline(inf, line);
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
				switch(curch) {
					case ';': s=semicol; break;
					case '.': s=dotsym; break;
					default: s=badsym; break;
				}
				getch();
			}
		}
	}
}
