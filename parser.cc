#include <iostream>
#include "parser.h"

using namespace std;

/* The parser for the circuit definition files */


bool parser::readin (void)
{
  generator();
  devs();
	connections();
  monitors();
}

void parser::generator(void){
  //smz->getsymbol();
  cout << "Skipping generators for now..." << endl;
}

void parser::devs(){
  dev();
  while(cursym == scanner::logsym||cursym == scanner::dtypesym||cursym == scanner::xorsym){
    dev();
  }
}

void parser::dev(void){
  switch(cursym){
    case(scanner::logsym): logic();
    case(scanner::dtypesym): dtype();
    case(scanner::xorsym): Xor();
  }
}

void parser::logic(void){
  smz->getsymbol(cursym, curid, curnum);
  if(cursym == scanner::namesym){
    smz->getsymbol(cursym, curid, curnum);
    if (cursym == scanner::numsym){
      if(!(curnum <=16 && curnum > 0)){
        cout << "Making logic device..." << endl;
        smz->nmz->writename(curid);
        cout << endl;
      }else error();
      checkendsym();
      smz->getsymbol(cursym, curid, curnum);
    }else error();
  }else error();
}
void parser::dtype(void){
  cout << "Skipping dtype for now..." << endl;
}

void parser::Xor(void){
  cout << "Skipping Xor for now..." << endl;
}

void parser::connections(void){
  cout << "Skipping Connections for now..." << endl;
}

void parser::monitors(void){
  cout << "Skipping Monitors for now..." << endl;
}

void parser::error(void){
  cout << "WARNING: ERROR!!!" << endl;
  exit(1);
}

void parser::checkendsym(void){
  smz->getsymbol(cursym, curid, curnum);
  if(cursym == scanner::semicol)return;
  error();
}

parser::parser (network* network_mod, devices* devices_mod,
		monitor* monitor_mod, scanner* scanner_mod)
{
  netz = network_mod;  /* make internal copies of these class pointers */
  dmz = devices_mod;   /* so we can call functions from these classes  */
  mmz = monitor_mod;   /* eg. to call makeconnection from the network  */
  smz = scanner_mod;   /* class you say:                               */
                       /* netz->makeconnection (i1, i2, o1, o2, ok);   */

  /* any other initialisation you want to do? */

}
