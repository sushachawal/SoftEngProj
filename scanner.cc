#include "scanner.h"
#include "names.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

scanner::scanner(names* names_mod, const char* defname)
{

	nmz = names_mod;
	eofile = false;
  symcount = 0;

	clkname = nmz->lookup("CLOCK");
	swtchname = nmz->lookup("SWITCH");
  rcname = nmz->lookup("RC");
	andname = nmz->lookup("AND");
	nandname = nmz->lookup("NAND");
	orname = nmz->lookup("OR");
	norname = nmz->lookup("NOR");
	dtypename = nmz->lookup("DTYPE");
	xorname = nmz->lookup("XOR");
  notname = nmz->lookup("NOT");
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
}

void scanner::skipspaces()
{
  if (eofile) {
    return;
  }
	while (isspace(curch)) {
		getch();
    if (eofile){
      break;
    }
	}
	if (curch == '%') {
		getch();
		if (curch == '%') {
			while(!eofile){
				getch();
				if (curch == '%') {
					getch();
					if (curch == '%') break;
				}
			}
    }else {
			while (curch != '\n' && !eofile) {
				getch();
			}
    }
		skipspaces();	
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
	if (eofile) {// end of file:
		s = eofsym;
    symcount ++;
	}
	else {
		if (isdigit(curch)) { // number:
			s=numsym;
			getnumber(num);
      symcount ++;
		}
		else {
			if (isalpha(curch)) { // name or keyword:
				getname(id);
				if (id == clkname || id == swtchname || id == rcname){
					s=gensym;
          symcount ++;
				} else{
					if (id == andname){
						s = andsym;
            symcount ++;
					} else {
						if (id == nandname){
						  s = nandsym;
              symcount ++;
						} else {
						  if (id == orname){
							s = orsym;
                symcount ++;
						  } else {
								if (id == norname){
									s = norsym;
                  symcount ++;
								} else {
									if (id == dtypename) {
									s = dtypesym;
                    symcount ++;
									} else {
										if (id == xorname) {
											s = xorsym;
                      symcount ++;
										} else {
                      if (id == notname) {
                        s = notsym;
                        symcount ++;
                      } else {
                        if (id == monitorname) {
                          s=monsym;
                          symcount ++;
                        } else {
                          s=namesym; // not a keyword
                          symcount ++;
                        }
                      }
										}
									}
								}
						  }
						}
					}
				}
			}else { //Neither a name nor a number:
              if (curch == '-') {
                getch();
                if (curch == '>') {
                  s = arrowsym;
                  symcount ++;
                } else {
                  s = badsym;
                  symcount ++;
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

bool scanner::checksemicol()
{
  int pos = inf.tellg();
  skipspaces();
  if (curch == ';') {
    inf.seekg(pos, inf.beg); // return to position before semicol checked
    return true;
  }
  else {
    inf.seekg(pos, inf.beg); // return to position before semicol checked
    return false;
  }
}

void scanner::reporterror()
{
  // first check if file is empty
  if (symcount == 0){
    cout << "Empty file" << endl;
    return;
  }
	//cout << "Error starts at:" << int(curch) << endl;
  string line_str;
  string report_str = "";
  int counter = 0;
  int line = 0; // line counter
  int c_count = 0; // character counter, reset every line
  char c;

  int pos = inf.tellg(); // store current positon
  
  if (pos == -1) {          // file isn't empty, so this is end of file.
    inf.seekg(0, ios::end);  // go to the last character.
    pos == inf.tellg();     // store position of the last character
  }
  
  inf.seekg(0, inf.beg); // go to the beginning of the file
  
  while(counter<pos){
    inf.get(c);
    if (c == '\n') {
      line++;
      c_count = 0;
    } else c_count ++;
    counter++;
  }

  inf.seekg(0, inf.beg); // go to the beginning of the file
  
  if (c_count == 0){     // if error detected at the start of a line, mark error at the end of the previous line.
    line --;
    for (counter = 0; counter <= line; counter ++){
      if(!getline(inf, line_str));
    }
    c_count = line_str.length() + 2;
  }
  else{
    for (counter = 0; counter <= line; counter ++){
      if(!getline(inf, line_str));
    }
  }
  
  inf.seekg(pos, inf.beg); // return to position before line is read

  for (counter= 0; counter < c_count-1; counter++){
    report_str += ' ';
  }
  report_str[c_count-2] = '^';
  cout << endl << "Line: " << line << " Character: " << c_count << endl;
  cout << line_str << endl;
  cout << report_str << endl;
  return;
}
