#ifndef names_h
#define names_h

#include <string>
#include <unordered_map>

using namespace std;

const int maxnames  = 200;  /* max number of distinct names */
const int maxlength = 8;    /* max chars in a name string   */
const int blankname = -1;   /* special name                 */

typedef int name;
typedef string namestring;

class names{

private:
  unordered_map<namestring, name> namestrtoname;
  unordered_map<name, namestring> nametonamestr;
  name count = 0; //Create a counter to ensure unique key/values.

public:                     
  name lookup (namestring str);
    /* Returns the internal representation of the name given in character  */
    /* form.  If the name is not already in the name table, it is          */
    /* automatically inserted.                                             */

  name cvtname (namestring str);
    /* Returns the internal representation of the name given in character  */
    /* form.  If the name is not in the name table then 'blankname' is     */
    /* returned.                                                           */

  void writename (name id);
    /* Prints out the given name on the console                            */
  
  void printall ();
    /* Print out all the names contained in the map                        */

  int namelength (name id);                                                
    /* Returns length ie number of characters in given name                */

  names (void);
    /* names initialises the name table.  This procedure is called at      */
    /* system initialisation before any of the above procedures/functions  */
    /* are used.                                                           */
};

#endif /* names_h */
