#include "names.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;


/* Name storage and retrieval routines */

names::names(void)  /* the constructor */
{
  /* over to you */
}

name names::lookup (namestring str)
{
  static name count = 0; //Create a counter to ensure unique key/values.
  auto search = namestrtoname.find(str); //C++ 11 auto, automatically picks type!
  if (search != namestrtoname.end()) return search->second; //Return value
  else {
    //If maxnames is about to be exceeded throw an error.
    if (count + 1 > maxnames){
      cout << "Warning maximum number of names " << "(" << maxnames << ") ";
      cout << "has been reached! " << "\'" << str << "\'" << "was rejected.";
      cout << endl;
      return blankname;
    }
    //Otherwise go ahead and insert it into the map
    namestrtoname[str] = count++; //Assign count THEN increment it
    nametonamestr[namestrtoname[str]] = str; //Add new name to both maps
    return namestrtoname[str];
  }
}

name names::cvtname (namestring str)
{
  auto search = namestrtoname.find(str); //C++ 11 auto, automatically picks type!
  if (search != namestrtoname.end()) return search->second;
  else return blankname;
}

void names::writename (name id)
{
	cout << nametonamestr[id];
}

int names::namelength (name id)
{
	return nametonamestr[id].length();
}
