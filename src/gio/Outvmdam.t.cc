// gio_Outvmdam.t.cc

#include <cassert>
#include <cstdlib>
#include "InG96.h"
#include "../gcore/System.h"
#include "InTopology.h"
#include "Outvmdam.h"
#include <string>
#include <iostream>

using namespace std;
using namespace gcore;
using namespace gio;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cerr << "Usage: " + string(argv[0]) + " <Topology> <Filename>\n";
    exit(1);
  }
  string top = argv[1];
  string file = argv[2];
  // read Simulation data

  InTopology it(top);
  System sys(it.system());

  InG96 ic;
  ic.open(file);
  //  ic.select("ALL");

  Outvmdam oc;

  oc.open(cout);
  //  oc.select("SOL");

  //  oc.writeTitle(ic.title());

  while (!ic.eof()) {
    ic >> sys;
    oc << sys;
  }
  return 0;
}


