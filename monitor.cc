#include <iostream>
#include "monitor.h"

using namespace std;

/***********************************************************************
 *
 * Sets a monitor on the output/input 'pin' of device 'dev' by placing an   
 * entry in the monitor table. 'ok' is set true if operation succeeds. 
 *
 */
void monitor::makemonitor (name dev, name pin, bool& ok)
{
  devlink d;
  outplink o;
  inplink i;
  ok = (mtab.used < maxmonitors);
  if (ok) {
    d = netz->finddevice (dev);
    ok = (d != NULL);
    if (ok) {
      i = netz->findinput(d, pin);
      o = netz->findoutput (d, pin);
      if (o != NULL){
        mtab.sigs[mtab.used].devid = dev;
        mtab.sigs[mtab.used].op = o;
        mtab.sigs[mtab.used].ip = NULL;
        (mtab.used)++;
        ok = true;
        cout<<"output pin"<<" ";
      } else if (i != NULL){
        mtab.sigs[mtab.used].devid = dev;
        mtab.sigs[mtab.used].ip = i;
        mtab.sigs[mtab.used].op = NULL;
        (mtab.used)++;
        ok = true;
        cout<<"input pin"<<" ";
        /*
        o = i->connect;
        dev = i->outid;
        mtab.sigs[mtab.used].devid = dev;
        mtab.sigs[mtab.used].op = o;
        (mtab.used)++;
        ok = true;
         */
      }
    }
  }
  cout<<endl;
}


/***********************************************************************
 *
 * Removes the monitor set the 'outp' output of device 'dev'. 'ok' is  
 * set true if operation succeeds.                                     
 *
 */
void monitor::remmonitor (name dev, name pin, bool& ok)
{
  int i, j;
  bool found, found_dev, found_pin;
  ok = (mtab.used > 0);
  if (ok) {
    found = false;
    found_dev = false;
    found_pin = false;
    for (i = 0; ((i < mtab.used) && (! found)); i++){
      found_dev = ((mtab.sigs[i].devid == dev));
      if (found_dev){
        if (mtab.sigs[i].op){
          found_pin = (mtab.sigs[i].op->id == pin);
        } else found_pin = (mtab.sigs[i].ip->id == pin);
      }
      found = found_dev && found_pin;
    }
    ok = found;
    if (found) {
      (mtab.used)--;
      for(j = (i-1); j < mtab.used; j++)
	mtab.sigs[j] = mtab.sigs[j + 1];
    }
  }
}


/***********************************************************************
 *
 * Returns number of signals currently monitored.  
 *
 */
int monitor::moncount (void)
{
  return (mtab.used);
}


/***********************************************************************
 *
 * Returns signal level of n'th monitor point. 
 *
 */
asignal monitor::getmonsignal (int n)
{
  if (mtab.sigs[n].op){
    return mtab.sigs[n].op->sig;
  }
  else return(mtab.sigs[n].ip->connect->sig);
}


/***********************************************************************
 *
 * Returns name of n'th monitor. 
 *
 */
void monitor::getmonname (int n, name& dev, name& pin)
{
  dev = mtab.sigs[n].devid;
  if (mtab.sigs[n].op){
    pin = mtab.sigs[n].op->id;
  }
  else pin = mtab.sigs[n].ip->id;
}


/***********************************************************************
 *
 * Initialises monitor memory in preparation for a new output sequence.
 *
 */
void monitor::resetmonitor (void)
{
  cycles = 0;
}


/***********************************************************************
 *
 * Called every clock cycle to record the state of each monitored     
 * signal.                                                            
 *
 */
void monitor::recordsignals (void)
{
  int n;
  for (n = 0; n < moncount (); n++)
    disp[n][cycles] = getmonsignal(n);
  cycles++;
}

/***********************************************************************
 *
 * Access recorded signal trace, returns false if invalid monitor 
 * or cycle.
 *
 */
bool monitor::getsignaltrace(int m, int c, asignal &s)
{
  if ((c < cycles) && (m < moncount ())) {
    s = disp[m][c];
    return true;
  }
  return false;
}

/***********************************************************************
 *
 * Displays state of monitored signals.  
 *
 */
void monitor::displaysignals (void)
{
  const int margin = 20;
  int n, i;
  name dev, pin;
  int namesize;
  for (n = 0; n < moncount (); n++) {
    getmonname (n, dev, pin);
    namesize = nmz->namelength (dev);
    nmz->writename (dev);
    if (pin != blankname) {
      cout << ".";
      nmz->writename (pin);
      namesize = namesize + nmz->namelength (pin) + 1;
    }
    if ((margin - namesize) > 0) {
      for (i = 0; i < (margin - namesize - 1); i++)
	cout << " ";
      cout << ":";
    }
    for (i = 0; i < cycles; i++) 
      switch (disp[n][i]) {
        case high:    cout << "-"; break;
        case low:     cout << "_"; break;
        case rising:  cout << "/"; break;
        case falling: cout << "\\"; break;
      }
    cout << endl;
  }
}


/***********************************************************************
 *
 * Called to initialise the monitor module.  
 * Remember the names of the shared names and network modules.
 *
 */
monitor::monitor (names* names_mod, network* network_mod)
{
  nmz = names_mod;
  netz = network_mod;
  mtab.used = 0;
}
