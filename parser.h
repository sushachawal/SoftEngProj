#ifndef parser_h
#define parser_h

#include "names.h"
#include "scanner.h"
/*
#include "network.h"
#include "devices.h"
#include "monitor.h"
*/
#include <exception>


using namespace std;

class parser_exception : public std::exception {
  const char* info;
  int stage;
    
public:
  parser_exception(const char* info_) :
  info (info_),
  stage (-1){
  }
  
  const char* get_info() const {
    return info;
    
  }
};

class parser {
  /*
  network* netz; // instantiations of various classes for parser to use.
  devices* dmz;
  monitor* mmz;
  */
  scanner* smz;

  /* put other stuff that the class uses internally here */
  /* also declare internal functions                     */
  scanner::symbol cursym;
  name curid;
  int curnum;
  name devid; // used for storing the full output pin description
  int stage; // used for jumping into a particular stage of parsing
  int errorcount; // counts the number of errors
  bool parsed; // true when file is parsed
  bool eofile; // true when end of file is reached
  void generators(void);
  void generator(void);
  void devs(void);
  void dev(void);
  void logic(void);
  void dtype(void);
  void Xor(void);
  void connections(void);
  void output(void);
  void input(void);
  void monitors(void);
  void error(void);
  void checkendsym(void);
  void begin(int stage); // restarts the program in stage: 1 generators()
                          //                                2 devs()
                          //                                3 connections()
                          //                                4 monitors()
  void recover(void); // get first symbol after first semicolon is found


 public:
  bool readin ();
    /* Reads the definition of the logic system and builds the             */
    /* corresponding internal representation via calls to the 'Network'    */
    /* module and the 'Devices' module.                                    */

  parser (/*network* network_mod, devices* devices_mod,
	  monitor* monitor_mod,*/ scanner* scanner_mod);
    /* the constructor takes pointers to various other classes as parameters */
};

#endif /* parser_h */
