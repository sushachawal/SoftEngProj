#include <iostream>
#include "parser.h"
#include <exception>

using namespace std;

/* The parser for the circuit definition files */


bool parser::readin (void)
{
  stage = 1;
  parsed = false;
  eofile = false;
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
  
  return eofile;
}

void parser::begin(int stage){
  switch(stage){
    case(-1):
      parsed = true;
      eofile = true;
      break;
    case(-2):
      cout << "Cannot complete parsing" << endl;
      parsed = true;
      break;
    case(1):
      generators();
      devs();
      connections();
      monitors();
      parsed = true;
      eofile = true;
      break;
    case(2):
      devs();
      connections();
      monitors();
      parsed = true;
      eofile = true;
      break;
    case(3):
      connections();
      monitors();
      parsed = true;
      eofile = true;
      break;
    case(4):
      monitors();
      parsed = true;
      eofile = true;
      break;
    default: parser_exception("Error: invalid stage");
      break;
  }
}

void parser::recover(void){
  while(cursym != scanner::semicol && cursym != scanner::eofsym){
    smz->getsymbol(cursym, curid, curnum);
  }
  if(cursym == scanner::eofsym){
    stage = -2;
  } else{
    smz->getsymbol(cursym, curid, curnum);
    switch(cursym){
      case(scanner::eofsym):
        stage = -1;
        break;
      case(scanner::gensym):
        stage = 1;
        break;
      case(scanner::logsym):
      case(scanner::dtypesym):
      case(scanner::xorsym):
        stage = 2;
        break;
      case(scanner::namesym): // can only be namesym after ; if connection
        stage = 3;
        break;
      case(scanner::monsym):
        stage = 4;
        break;
      default:
        stage = -2;
        break;
    }
  }
}

void parser::generators(void){
  if(cursym == scanner::gensym) {
    generator();
  } else throw parser_exception("Error: No generator found"); // begin in stage 2
  while (cursym == scanner::gensym) {
    generator();
  }
}

void parser::generator(void){
	if (curid == smz->nmz->lookup("CLOCK")) {
		  smz->getsymbol(cursym, curid, curnum);
		  if(cursym == scanner::namesym){
              smz->getsymbol(cursym, curid, curnum);
			  if (cursym == scanner::numsym){
				  if(curnum > 0){
					  cout << "Making clock device ";
					  smz->nmz->writename(curid);
					  cout << endl;
          } else throw parser_exception("Error: CLK number must be greater than 0"); // error();
				  checkendsym();
				  smz->getsymbol(cursym, curid, curnum);
        } else throw parser_exception("Error: expected number after device name"); // error();
      } else throw parser_exception("Error: expected device name after 'CLK'"); // error();
	  }
	  else if (curid == smz->nmz->lookup("SWITCH")) {
		  smz->getsymbol(cursym, curid, curnum);
		  if(cursym == scanner::namesym){
              smz->getsymbol(cursym, curid, curnum);
			  if (cursym == scanner::numsym){
				  if(curnum == 1 || curnum == 0){
					  cout << "Making switch device ";
					  smz->nmz->writename(curid);
					  cout << endl;
          } else throw parser_exception("Error: initial state takes values 0 or 1"); // error();
				  checkendsym();
				  smz->getsymbol(cursym, curid, curnum);
        } else throw parser_exception("Error: expected number after device name"); // error();
      } else throw parser_exception("Error: expected device name after 'SWITCH'"); // error();
	  } else throw parser_exception("Error: this should never happen"); // error();
}

void parser::devs(){
  if(cursym == scanner::logsym||cursym == scanner::dtypesym||cursym == scanner::xorsym) {
	  dev();
  } else throw parser_exception("No device found");

  while(cursym == scanner::logsym||cursym == scanner::dtypesym||cursym == scanner::xorsym){
    dev();
  }
}

void parser::dev(void){
  switch(cursym){
    case(scanner::logsym): logic();
          break;
    case(scanner::dtypesym): dtype();
          break;
    case(scanner::xorsym): Xor();
          break;
    default: throw parser_exception("Error: this definetly shouldn't happen");
          break;
  }
}

void parser::logic(void){
  smz->getsymbol(cursym, curid, curnum);
  if(cursym == scanner::namesym){
    smz->getsymbol(cursym, curid, curnum);
    if (cursym == scanner::numsym){
      if(curnum <=16 && curnum > 0){
        cout << "Making logic device ";
        smz->nmz->writename(curid);
        cout << endl;
      }else throw parser_exception("Error: number of inputs not in range 1-16"); // error();
      checkendsym();
      smz->getsymbol(cursym, curid, curnum);
    }else throw parser_exception("Error: expected number after device name"); // error();
  }else throw parser_exception("Error: expected device name after 'AND', 'NAND', 'OR', 'NOR'"); // error();
}

void parser::dtype(void){
    smz->getsymbol(cursym, curid, curnum);
    if(cursym == scanner::namesym){
        cout << "Making dtype device ";
        smz->nmz->writename(curid);
        cout << endl;
        checkendsym();
        smz->getsymbol(cursym, curid, curnum);
    } else throw parser_exception("Error: expected device name after 'DTYPE'");
}

void parser::Xor(void){
    smz->getsymbol(cursym, curid, curnum);
    if(cursym == scanner::namesym){
        cout << "Making xor device ";
        smz->nmz->writename(curid);
        cout << endl;
        checkendsym();
        smz->getsymbol(cursym, curid, curnum);
    } else throw parser_exception("Error: expected device name after 'XOR'");
}

void parser::connections(void){
    while(cursym == scanner::namesym){
        cout << "Connecting output ";
        output();
        if(cursym == scanner::arrowsym){
            cout << " to input ";
            smz->getsymbol(cursym, curid, curnum);
            input();
            cout << endl;
            checkendsym();
            smz->getsymbol(cursym, curid, curnum);
        } else throw parser_exception("Error: expected '->' after output pin");
    }
}

void parser::output(void){
  smz->getsymbol(cursym, curid, curnum);
  if(cursym == scanner::dotsym){
    smz->getsymbol(cursym, curid, curnum);
    if(cursym == scanner::namesym){
      if(curid == smz->nmz->cvtname("Q")){
        cout << ".Q";
        smz->getsymbol(cursym, curid, curnum);
      } else if(curid == smz->nmz->cvtname("QBAR")){
        cout << ".QBAR";
        smz->getsymbol(cursym, curid, curnum);
      } else throw parser_exception("Error: invalid output pin. Expected 'Q'/'QBAR'");
    } else throw parser_exception("Error: invalid output pin");
  } else smz->nmz->writename(curid);
}

void parser::input(void){
    if(cursym == scanner::namesym){
        devid = curid;
        smz->getsymbol(cursym, curid, curnum);
        if(cursym == scanner::dotsym){
            smz->getsymbol(cursym, curid, curnum);
            if(cursym == scanner::namesym){
                //  Check output exists
                smz->nmz->writename(devid);
                cout << ".";
                smz->nmz->writename(curid);
            } else throw parser_exception("Error: invalid input pin");
        } else throw parser_exception("Error: expected '.' after device name");
    } else throw parser_exception("Error: expected input pin after '->'");
}

void parser::monitors(void){
    if(cursym == scanner::monsym){
      cout << "Monitoring ";
      smz->getsymbol(cursym, curid, curnum);
      while(cursym != scanner::semicol){
        if(cursym == scanner::namesym){
          smz->nmz->writename(curid);
          smz->getsymbol(cursym, curid, curnum);
          if(cursym == scanner::namesym){
            cout << " ";
            smz->nmz->writename(curid);
            smz->getsymbol(cursym, curid, curnum);
          } else if(cursym == scanner::dotsym){
              cout << ".";
              smz->getsymbol(cursym, curid, curnum);
              if(cursym == scanner::namesym){
                smz->nmz->writename(curid);
                smz->getsymbol(cursym, curid, curnum);
              } else throw parser_exception("Error: invalid output/input pin");
          } else {
            throw parser_exception("Error: expected '.' or output/input pin");
          }
        } else throw parser_exception("Error: expected output/input pin");
      }
      smz->getsymbol(cursym, curid, curnum); // Should be eofsym
      if(cursym != scanner::eofsym){
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
  if(cursym == scanner::semicol)return;
  throw parser_exception("Error: expected ';'"); // error();
}

parser::parser (/*network* network_mod, devices* devices_mod,
		monitor* monitor_mod,*/ scanner* scanner_mod)
{
  //netz = network_mod;  /* make internal copies of these class pointers */
  //dmz = devices_mod;   /* so we can call functions from these classes  */
  //mmz = monitor_mod;   /* eg. to call makeconnection from the network  */
  smz = scanner_mod;   /* class you say:                               */
                       /* netz->makeconnection (i1, i2, o1, o2, ok);   */

  /* any other initialisation you want to do? */

}
