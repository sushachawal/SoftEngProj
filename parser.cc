

#include <iostream>
#include "parser.h"
#include <exception>

using namespace std;

/* The parser for the circuit definition files */

/***********************************************************************
 *
 * Function to be called from logicsim.cc. Interprets the .txt file
 * using the EBNF syntax specified for the logic circuit and returns
 * the success or failure of parsing.
 * 
 * @param NONE
 * 
 * @return bool true if the parsing was successful without any syntax
 * or semantic errors and false if there were some errors.
 * 
 */
bool parser::readin (void)
{
  stage = 1;
  parsed = false;
  errorcount = 0;
  smz->getsymbol(cursym, curid, curnum); // gets first symbol. makes initialisation compatable with error recovery.
  while(!parsed){
    try{
      begin(stage);
    }
    catch(parser_exception& e) {
      smz->reporterror();
      cout << e.get_info() << endl;
      errorcount ++;
      recover();
    }
  }
  if (errorcount == 0){
    return true;
  } else return false;
}

void parser::begin(int stage){
  switch(stage){
    case(-1):
      parsed = true;
      break;
    case(-2):
      cout << "End of file reached before semi-colon detected." << endl;
      parsed = true;
      break;
    case(1):
      generators();
      devs();
      connections();
      monitors();
      parsed = true;
      break;
    case(2):
      devs();
      connections();
      monitors();
      parsed = true;
      break;
    case(3):
      connections();
      monitors();
      parsed = true;
      break;
    case(4):
      monitors();
      parsed = true;
      break;
    default: parser_exception("Error: invalid stage");
      break;
  }
}

void parser::recover(void){
  while(cursym != semicol && cursym != eofsym){
    smz->getsymbol(cursym, curid, curnum);
  }
  if(cursym == eofsym){
    stage = -2;
  } else{
    smz->getsymbol(cursym, curid, curnum);
    switch(cursym){
      case(eofsym):
        stage = -1;
        break;
      case(gensym):
        stage = 1;
        break;
      case(andsym):
      case(nandsym):
      case(orsym):
      case(norsym):
      case(dtypesym):
      case(xorsym):
        stage = 2;
        break;
      case(namesym): // can only be namesym after ; if connection
        stage = 3;
        break;
      case(monsym):
        stage = 4;
        break;
      default:
        stage = -2;
        break;
    }
  }
}

void parser::generators(void){
  if(cursym == gensym) {
    generator();
  } else throw parser_exception("Error: No generator found"); // begin in stage 2
  while (cursym == gensym) {
    generator();
  }
}

void parser::generator(void){
  bool ok;
  name devid;
  int devnum;
  
	if (curid == smz->nmz->lookup("CLOCK")) {          // Check if device is a 'CLOCK' or 'SWITCH'
    smz->getsymbol(cursym, curid, curnum);
    if(cursym == namesym){                  // Check namesym after gensym
      smz->getsymbol(cursym, curid, curnum);
      if (cursym == numsym){                // Check numsym after namesym
        if(curnum > 0){                              // Check if number is valid
          cout << "Making clock device ";
          smz->nmz->writename(curid);
          cout << endl;
          devid = curid;
          devnum = curnum;
          checkendsym();                                    // Don't make device if no semicol;
          smz->getsymbol(cursym, curid, curnum);
          if(errorcount == 0){
            if(!netz->finddevice(devid)){
              dmz->makedevice(aclock, devid, devnum, ok); // Make 'SWITCH' device
              if(!ok){
                throw parser_exception("Error: failed to make 'SWITCH' device");
              }
            } else throw parser_exception("Error: device already exists!");
          }
        } else throw parser_exception("Error: CLK number must be greater than 0");
      } else throw parser_exception("Error: expected number after device name");
    } else throw parser_exception("Error: expected device name after 'CLK'");
  }
  else if (curid == smz->nmz->lookup("SWITCH")) {
    smz->getsymbol(cursym, curid, curnum);
    if(cursym == namesym){
      // Check namesym after gensym
      smz->getsymbol(cursym, curid, curnum);
      if (cursym == numsym){
        if(curnum == 1 || curnum == 0){
          cout << "Making switch device ";
          smz->nmz->writename(curid);
          cout << endl;
          devid = curid;
          devnum = curnum;
          checkendsym();                                    // Don't make device if no semicol;
          smz->getsymbol(cursym, curid, curnum);
          if(errorcount == 0){
            if(!netz->finddevice(devid)){
              dmz->makedevice(aswitch, devid, devnum, ok); // Make 'SWITCH' device
              if(!ok){
                throw parser_exception("Error: failed to make 'SWITCH' device");
              }
            } else throw parser_exception("Error: device already exists!");
          }
        } else throw parser_exception("Error: initial state takes values 0 or 1");
      } else throw parser_exception("Error: expected number after device name");
    } else throw parser_exception("Error: expected device name after 'SWITCH'");
  } else throw parser_exception("Error: this should never happen");
}

void parser::devs(){
  if(cursym == andsym||cursym == nandsym||cursym == orsym||cursym == norsym||cursym == dtypesym||cursym == xorsym) {     // Must be at least one device
	  dev();
  } else throw parser_exception("Error: expected device type");

  while(cursym == andsym||cursym == nandsym||cursym == orsym||cursym == norsym||cursym == dtypesym||cursym == xorsym){   // No limit on num. devices
    dev();
  }
}

void parser::dev(void){
  switch(cursym){
    case(andsym): logic(andgate);
      break;
    case(nandsym): logic(nandgate);
      break;
    case(orsym): logic(orgate);
      break;
    case(norsym): logic(norgate);
      break;
    case(dtypesym): dtype_();
      break;
    case(xorsym): Xor();
      break;
    default: throw parser_exception("Error: this definetly shouldn't happen");
      break;
  }
}

void parser::logic(devicekind dkind){
  bool ok;
  name devid;
  int devnum;
  
  smz->getsymbol(cursym, curid, curnum);
  if(cursym == namesym){                                  // Check namesym after logsym
    smz->getsymbol(cursym, curid, curnum);
    if (cursym == numsym){                                // Check numsym after namesym
      if(curnum <=16 && curnum > 0){                      // Check valid input range
        cout << "Making logic device ";
        smz->nmz->writename(curid);
        devid = curid;
        devnum = curnum;
        cout << endl;
        checkendsym();
        smz->getsymbol(cursym, curid, curnum);
        if(errorcount == 0){
          if(!netz->finddevice(devid)){
            dmz->makedevice(dkind, devid, devnum, ok);
            if(!ok){
              throw parser_exception("Error: failed to make logic device");
            }
          } else throw parser_exception("Error: device already exists!");
        }
      }else throw parser_exception("Error: number of inputs not in range 1-16"); // error();
    }else throw parser_exception("Error: expected number after device name"); // error();
  }else throw parser_exception("Error: expected device name after 'AND', 'NAND', 'OR', 'NOR'"); // error();
}

void parser::dtype_(void){
  bool ok;
  name devid;
  
  smz->getsymbol(cursym, curid, curnum);
  if(cursym == namesym){
    cout << "Making dtype device ";
    smz->nmz->writename(curid);
    devid = curid;
    cout << endl;
    checkendsym();
    smz->getsymbol(cursym, curid, curnum);
    if(errorcount == 0){
      if(!netz->finddevice(devid)){
        dmz->makedevice(dtype, devid, blankname, ok);
        if(!ok){
          throw parser_exception("Error: failed to make dtype device");
        }
      } else throw parser_exception("Error: device already exists!");
    }
  } else throw parser_exception("Error: expected device name after 'DTYPE'");
}

void parser::Xor(void){
  bool ok;
  name devid;
  int devnum;
  
  smz->getsymbol(cursym, curid, curnum);
  if(cursym == namesym){
    cout << "Making xor device ";
    smz->nmz->writename(curid);
    devid = curid;
    devnum = curnum;
    cout << endl;
    checkendsym();
    smz->getsymbol(cursym, curid, curnum);
    if(errorcount == 0){
      if(!netz->finddevice(devid)){
        dmz->makedevice(xorgate, devid, blankname, ok);
        if(!ok){
          throw parser_exception("Error: failed to make xor device");
        }
      } else throw parser_exception("Error: device already exists!");
    }
  } else throw parser_exception("Error: expected device name after 'XOR'");
}

void parser::connections(void){
  name indev, insig, outdev, outsig;
  bool ok;
  
  while(cursym == namesym){
    cout << "Connecting output ";
    outdev = curid;
    output(outdev, outsig);
    if(cursym == arrowsym){
      cout << " to input ";
      smz->getsymbol(cursym, curid, curnum);
      indev = curid;
      input(indev, insig);
      cout << endl;
      checkendsym();
      smz->getsymbol(cursym, curid, curnum);
      if(errorcount == 0){
        netz->makeconnection(indev, insig, outdev, outsig, ok);
        if(!ok){
          throw parser_exception("Error: connection failed");
        }
      }
    } else throw parser_exception("Error: expected '->' after output pin");
  }
}

void parser::output(name outdev, name& outsig){
  devlink dev = netz->finddevice(outdev);
  outplink olink;
  
  if(dev){                                              // Check device exists
    smz->getsymbol(cursym, curid, curnum);
    if(cursym == dotsym){                               // if dotsym, must be dtype
      smz->getsymbol(cursym, curid, curnum);
      if(cursym == namesym){
        if(errorcount == 0){
          olink = netz->findoutput(dev, curid);
          if(olink){
            outsig = curid;
          } else throw parser_exception("Error: output pin does not exist");
        }
        smz->nmz->writename(outdev);
        cout << ".";
        smz->nmz->writename(curid);
        smz->getsymbol(cursym, curid, curnum);
      } else throw parser_exception("Error: invalid output pin");
    } else {
      if(errorcount == 0){
        olink = netz->findoutput(dev, blankname);                 // Check output pin exists
        if(olink){
          outsig = blankname;
        } else throw parser_exception("Error: output pin does not exist");
      }
      smz->nmz->writename(outdev);
    }
  } else throw parser_exception("Error: device does not exist");
}

void parser::input(name indev, name& insig){
  devlink dev = netz->finddevice(indev);
  inplink ilink;
  
  if(cursym == namesym){
    if(dev){                                                // Check device exists
      smz->getsymbol(cursym, curid, curnum);
      if(cursym == dotsym){
        smz->getsymbol(cursym, curid, curnum);
        if(cursym == namesym){
          if(errorcount == 0){
            ilink = netz->findinput(dev, curid);
            if(ilink){
              insig = curid;
            } else throw parser_exception("Error: input pin does not exist");
          }
          smz->nmz->writename(indev);
          cout << ".";
          smz->nmz->writename(curid);
        } else throw parser_exception("Error: invalid input pin");
      } else throw parser_exception("Error: expected '.' after device name");
    } else throw parser_exception("Error: device does not exist");
  } else throw parser_exception("Error: expected input pin after '->'");
}

void parser::monitors(void){
  name devid;
  bool ok;
  
  if(cursym == monsym){
    cout << "Monitoring ";
    smz->getsymbol(cursym, curid, curnum);
    while(cursym != semicol){
      if(cursym == namesym){                      // Output/input pin always begins with namesym.
        smz->nmz->writename(curid);
        devid = curid;
        smz->getsymbol(cursym, curid, curnum);
        if(cursym == dotsym){
          cout << ".";
          smz->getsymbol(cursym, curid, curnum);
          if(cursym == namesym){                               //namesym->dotsym->namesym: input/multiple output device.
            smz->nmz->writename(curid);
            cout << " ";
            if(errorcount == 0){
              mmz->makemonitor(devid, curid, ok);
              if(!ok){
                throw parser_exception("Error: could not monitor device (currently only monitors output pins)");
              }
            }
            smz->getsymbol(cursym, curid, curnum);
          }else throw parser_exception("Error: invalid output/input pin");
        } else if(cursym == namesym || cursym == semicol){         // namesym/semicol sym -> monitor single output device.
          cout << " ";
          if(errorcount == 0){
            mmz->makemonitor(devid, blankname, ok);
            if(!ok){
              throw parser_exception("Error: could not monitor device");
            }
          }
        } else {
          throw parser_exception("Error: expected '.' or output/input pin");
        }
      } else throw parser_exception("Error: expected output/input pin");
    }
    smz->getsymbol(cursym, curid, curnum); // Should be eofsym
    if(cursym != eofsym){
      throw parser_exception("Error: end of file expected");
    }
    cout << endl;
  } else throw parser_exception("Error: expected 'MONITOR'");
}

void parser::error(void){
  cout << "WARNING: ERROR!!!" << endl;
  exit(1);
}

void parser::checkendsym(void){
  smz->getsymbol(cursym, curid, curnum);
  if(cursym == semicol)return;
  throw parser_exception("Error: expected ';'"); // error();
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
