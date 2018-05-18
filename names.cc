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
  	int i = 0;
	for (i = 0; i < book.size(); i++) {
		if (str == book[i]) {
			return i;
			break;
		}
	}
	book.push_back(str);
	return i;
}

name names::cvtname (namestring str)
{
	for (int i = 0; i < book.size(); i++) {
		if (str == book[i]) {
			return str;
			break;
		}
	}
	else {
		return 'blankname';
	}
}

void names::writename (name id)
{
	cout << book[id];
}

int names::namelength (name id)
{
	return book[id].length();
}
