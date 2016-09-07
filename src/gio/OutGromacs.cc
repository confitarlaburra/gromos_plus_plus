

#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <set>
#include <map>

#include "../gcore/BondType.h"
#include "../gcore/Bond.h"
#include "../gcore/AngleType.h"
#include "../gcore/Angle.h"
#include "../gcore/DihedralType.h"
#include "../gcore/Dihedral.h"
#include "../gcore/CrossDihedral.h"
#include "../gcore/ImproperType.h"
#include "../gcore/Improper.h"
#include "../gcore/LJType.h"
#include "../gcore/AtomPair.h"
#include "../gcore/Exclusion.h"
#include "../gcore/Constraint.h"
#include "../gcore/AtomTopology.h"
#include "../gcore/LJException.h"
#include "../gcore/MoleculeTopology.h"
#include "../gcore/Molecule.h"
#include "../gcore/Solvent.h"
#include "../gcore/SolventTopology.h"
#include "../gcore/System.h"
#include "../gcore/GromosForceField.h"
#include "../gmath/Physics.h"

#include "OutGromacs.h"

using gio::OutGromacs;
using namespace gcore;
using namespace std;

OutGromacs::OutGromacs(ostream &os):d_os(os){}

OutGromacs::~OutGromacs(){}

void OutGromacs::setTitle(const string &title){
  d_title=title;
}

//OutGromacs &OutGromacs::operator<<(const gcore::Simulation &sim){
void OutGromacs::write(const gcore::System &sys, const gcore::GromosForceField &gff){
  
  // Title block
  d_os << ";" << endl;
  d_os << "; File generated by gromos++" << endl;
  d_os << "; A lot of things will be hardcoded" << endl;
  d_os << ";" << endl;
  d_os << "; This should look like an include topology file" << endl;
  d_os << "; <Stupid quote goes here> (Chris)" << endl;
  d_os << ";" << endl;
  d_os << "; we start with all the parameters" << endl;
  d_os << "#define _FF_GROMOS96" << endl;
  d_os << "#define _FF_GROMOS96A" << endl << endl;
  d_os << "[ defaults ]" << endl;
  d_os << "; nbfunc        comb-rule       gen-pairs       fudgeLJ fudgeQQ" << endl;
  d_os << "  1             1               no              1.0     1.0" << endl;
  d_os << endl << endl; 

  // ATOMTYPES
  d_os << "[ atomtypes ]" << endl;
  d_os << ";name      mass     charge ptype             c6            c12\n";
  int num=gff.numAtomTypeNames();
  for(int i=0;i<num;++i){
    d_os.precision(7);
    d_os.setf(ios::fixed, ios::floatfield);
    d_os.setf(ios::scientific, ios::floatfield);
    LJType lj(gff.ljType(AtomPair(i,i)));
    if(i>0 &&!(i%10))d_os << "; " << i << "\n";
    d_os << setw(5) << gff.atomTypeName(i) << "     0.000      0.000     A "
	 << setw(14) << lj.c6() << " "
	 << setw(14) << lj.c12()
	 << "\n";
  }
  
  // Nonbonded parameters
  d_os << "\n[ nonbond_params ]\n"
       << ";   i      j func             c6            c12\n";
  for(int i=0; i< num; ++i){
    for(int j=0; j< i; ++j){
      d_os.precision(7);
      d_os.setf(ios::fixed, ios::floatfield);
      d_os.setf(ios::scientific, ios::floatfield);
      LJType lj(gff.ljType(AtomPair(i,j)));
      d_os << setw(5) << gff.atomTypeName(i)
	   << setw(7) << gff.atomTypeName(j)
	   << "    1 "
	   << setw(14) << lj.c6() << " "
	   << setw(14) << lj.c12()
	   << "\n";
    }
  }

  // 1,4 interactions
  d_os << "\n[ pairtypes ]\n"
       << ";   i      j func            cs6           cs12\n";
  for(int i=0; i< num; ++i){
    for(int j=0; j<= i; ++j){
      d_os.precision(7);
      d_os.setf(ios::fixed, ios::floatfield);
      d_os.setf(ios::scientific, ios::floatfield);
      LJType lj(gff.ljType(AtomPair(i,j)));
      d_os << setw(5) << gff.atomTypeName(i)
	   << setw(7) << gff.atomTypeName(j)
	   << "    1 "
	   << setw(14) << lj.cs6() << " "
	   << setw(14) << lj.cs12()
	   << "\n";
    }
  }
  
  // Bonded parameters
  // BONDS
  d_os << ";\n; BONDTYPE\n"
       << ";       type              b0          fc\n";
  num = gff.numBondTypes(); 
  for (int i=0;i<num;++i){
    if(i>0 &&!(i%10)) d_os << "; " << i << "\n";
    d_os.precision(5);
    d_os.setf(ios::fixed, ios::floatfield);
    d_os.setf(ios::scientific, ios::floatfield);
    ostringstream os;
    d_os << "#define gb_" << i+1
	 << "   2 ";
    d_os << setw(16) << gff.bondType(i).b0()
	 << setw(12) << gff.bondType(i).fc() << "\n";
  }
  // ANGLES
  d_os << ";\n; BONDANGLETYPE\n"
       << ";       type              b0          fc\n";
  num = gff.numAngleTypes(); 
  for (int i=0;i<num;++i){
    if(i>0 &&!(i%10)) d_os << "; " << i << "\n";
    d_os.precision(5);
    d_os.setf(ios::fixed, ios::floatfield);
    d_os.setf(ios::scientific, ios::floatfield);
    ostringstream os;
    d_os << "#define ga_" << i+1
	 << "   2 ";
    d_os << setw(16) << gff.angleType(i).t0()
	 << setw(12) << gff.angleType(i).fc() << "\n";
  }
  // IMPROPER DIHEDRALS
  d_os << ";\n; IMPDIHEDRALTYPE\n"
       << ";       type              b0          fc\n";
  num = gff.numImproperTypes(); 
  for (int i=0;i<num;++i){
    if(i>0 &&!(i%10)) d_os << "; " << i << "\n";
    d_os.precision(5);
    d_os.setf(ios::fixed, ios::floatfield);
    d_os.setf(ios::scientific, ios::floatfield);
    ostringstream os;
    d_os << "#define gi_" << i+1
	 << "   2 ";
    d_os << setw(16) << gff.improperType(i).q0()
	 << setw(12) << gff.improperType(i).fc() * gmath::physConst.get_radian2degree() * gmath::physConst.get_radian2degree()
	 << "\n";
  }
  // DIHEDRAL
  d_os << ";\n; DIHEDRALTYPE\n"
       << ";       type              pd          fc    np\n";
  num = gff.numDihedralTypes(); 
  for (int i=0;i<num;++i){
    if(i>0 &&!(i%10)) d_os << "; " << i << "\n";
    d_os.precision(3);
    d_os.setf(ios::fixed, ios::floatfield);
    ostringstream os;
    d_os << "#define gd_" << i+1
	 << "   1 ";
    d_os << setw(16) << acos(gff.dihedralType(i).pd()) * gmath::physConst.get_radian2degree()
	 << setw(12) << gff.dihedralType(i).fc() 
	 << setw(6)  << gff.dihedralType(i).np() << "\n";
  }

  // Now the molecular data. 
  // Gromacs expects the system per molecule
  d_os << "\n[ moleculetype ]\n";
  d_os << "; Name            nrexcl\n";
  d_os << "MOL_1           0\n";
  d_os << endl;
  
  // atoms
  d_os << "[ atoms ]\n";
  d_os << ";   nr   type  resnr residue  atom   cgnr     charge       mass  typeB    chargeB      massB\n";
  int counter=0;
  int offres=1;
  int chargegrp=1;
  double totcharge=0.0;
  
  for(int m=0; m < sys.numMolecules(); ++m){
    for(int a=0; a< sys.mol(m).numAtoms(); a++, counter++){
      totcharge+= sys.mol(m).topology().atom(a).charge();
      d_os.precision(5);
      d_os.setf(ios::floatfield);
      
      d_os << setw(6) << counter+1
	   << setw(7) << gff.atomTypeName(sys.mol(m).topology().atom
					  (a).iac())
	   << setw(7) << sys.mol(m).topology().resNum(a)+offres
	   << setw(7) << sys.mol(m).topology().resName(sys.mol(m).topology().resNum(a))
	   << setw(7) << sys.mol(m).topology().atom(a).name()
	   << setw(7) << chargegrp
	   << setw(10) << sys.mol(m).topology().atom(a).charge()
	   << setw(12) << sys.mol(m).topology().atom(a).mass()
	   << "   ; qtot "
	   << totcharge << endl;
      if(sys.mol(m).topology().atom(a).chargeGroup()) chargegrp++;
    }
    offres+=sys.mol(m).topology().numRes();
    
  }
  
  d_os << endl;
  
  d_os << ";\n[ bonds ]\n";
  d_os << ";  ai    aj funct            c0            c1            c2            c3\n";
  
  
  for(int m=0, offatom=1; m< sys.numMolecules(); ++m){
    
    BondIterator bit(sys.mol(m).topology());
    for(;bit;++bit){
      d_os << setw(5) << bit()[0] +offatom
	   << setw(6) << bit()[1]+offatom
	   << "     gb_"
	   << bit().type()+1 << endl;
    }
    offatom+=sys.mol(m).numAtoms();
  }
  
  d_os << endl;
  
  // Now come the exclusions
  // in gromacs the 1,4 exclusions are also given as exclusions
  d_os << endl;
  d_os << "[ exclusions ]\n";
  d_os << ";  ai    aex\n";
  for(int m=0, offatom=1; m< sys.numMolecules(); ++m){
    for(int a=0; a< sys.mol(m).numAtoms(); ++a){
      if(sys.mol(m).topology().atom(a).exclusion().size()){
	
	d_os << setw(6) << a+offatom;
	int l=0;
	for(int k=0;k<sys.mol(m).topology().atom(a).exclusion().size();++k, ++l){
	  //if(l%10==0 && l!=0)
	  //  d_os << "\n"
	  //	 << "      ";
	  d_os << setw(6) << sys.mol(m).topology().atom(a).exclusion().atom(k)+offatom;
	}
	for(int k=0;k<sys.mol(m).topology().atom(a).exclusion14().size(); ++k, ++l){
	  //if(l%10==0 && l!=0)
	  // d_os << "\n"
	  //	 << "      ";
	  d_os << setw(6) << sys.mol(m).topology().atom(a).exclusion14().atom(k)+offatom;
	}
	d_os << "\n";
      }
      
    }
    offatom+=sys.mol(m).numAtoms();
  }
  
    
  d_os << endl;
  
  // and the 1,4 interactions
  d_os << "[ pairs ]\n";
  d_os << ";   ai    aj funct\n";
  for(int m=0, offatom=1; m< sys.numMolecules(); ++m){
    for(int a=0; a< sys.mol(m).numAtoms(); ++a){
      for(int k=0;k<sys.mol(m).topology().atom(a).exclusion14().size();++k){
	d_os << setw(6) << a+offatom 
	     << setw(6) << sys.mol(m).topology().atom(a).exclusion14().atom(k)+offatom
	     << "     1\n";
      }
    }
    offatom+=sys.mol(m).numAtoms();
  }
  
    
  d_os << endl;
  d_os << "[ angles ]\n";
  d_os << ";  ai    aj    ak funct            c0            c1            c2            c3\n";
  for(int m=0, offatom=1; m< sys.numMolecules(); ++m){

    AngleIterator ait(sys.mol(m).topology());
    for(;ait;++ait){
      d_os << setw(5) << ait()[0] +offatom
	   << setw(6) << ait()[1]+offatom
	   << setw(6) << ait()[2]+ offatom
	   << "     ga_"
	   << ait().type()+1 << endl;
    }
    offatom+=sys.mol(m).numAtoms();
  }
  
    
  d_os << endl;
  d_os << "[ dihedrals ]\n";
  d_os << ";  ai    aj    ak    al funct            c0            c1            c2            c3            c4            c5\n";
  for(int m=0, offatom=1; m < sys.numMolecules(); ++m){
    ImproperIterator iit(sys.mol(m).topology());
    for(;iit;++iit){
      d_os << setw(5) << iit()[0] +offatom
	   << setw(6) << iit()[1]+offatom
	   << setw(6) << iit()[2]+ offatom
	   << setw(6) << iit()[3]+ offatom
	   << "     gi_"
	   << iit().type()+1 << endl;
    }
    offatom+=sys.mol(m).numAtoms();
  }
  
  
  d_os << endl;
  d_os << "[ dihedrals ]\n";
  d_os << ";  ai    aj    ak    al funct            c0            c1            c2            c3\n";
  for(int m=0, offatom=1; m < sys.numMolecules(); ++m){
    DihedralIterator dit(sys.mol(m).topology());
    for(;dit;++dit){
      d_os << setw(5) << dit()[0] +offatom
	   << setw(6) << dit()[1]+offatom
	   << setw(6) << dit()[2]+ offatom
	   << setw(6) << dit()[3]+ offatom
	   << "     gd_"
	   << dit().type()+1 << endl;
    }
    offatom+=sys.mol(m).numAtoms();
  }
  
  
  // possibly some position restraints file: ugly to hardcode like this
  d_os << "\n; Include Position restraint file\n";
  d_os << "#ifdef POSRES\n";
  d_os << "#include \"posre.itp\"\n";
  d_os << "#endif\n";
 
 
  // And then the solvent topology
  d_os << "\n[ moleculetype ]\n";
  d_os << "; molname       nrexcl\n";
  d_os << "      SOL            0\n";
  d_os << "\n";
  d_os << "[ atoms ]\n"
       << ";   nr   type  resnr residue  atom   cgnr     charge       mass\n";
  for(int a=0; a< sys.sol(0).topology().numAtoms(); a++){
    d_os.precision(5);
    d_os.setf(ios::floatfield);
    
    d_os << setw(6) << a+1
	 << setw(7) << gff.atomTypeName(sys.sol(0).topology().atom
					  (a).iac())
	 << setw(7) << "1"
	 << setw(7) << "SOL"
	 << setw(7) << sys.sol(0).topology().atom(a).name()
	 << setw(7) << "1"
	 << setw(10) << sys.sol(0).topology().atom(a).charge()
	 << setw(12) << sys.sol(0).topology().atom(a).mass()
	 << "\n";
  }
  d_os << endl;
  
  d_os << "[ exclusions ]\n";
  for(int a=0; a< sys.sol(0).topology().numAtoms()-1; a++){
    d_os << setw(6) << a+1;
    for(int b=a+1; b< sys.sol(0).topology().numAtoms(); b++){
      d_os << setw(6) << b + 1;
    }
    d_os << "\n";
  }
  
  d_os << "\n[ constraints ]\n"
       << ";   ai    aj    func  dist\n";
  ConstraintIterator cit(sys.sol(0).topology());
  for( ; cit; ++cit){
    d_os.precision(7);
    d_os.setf(ios::fixed, ios::floatfield);
    
    d_os << setw(6) << cit()[0] + 1
         << setw(6) << cit()[1] + 1
	 << setw(6) << "1"
         << setw(15) << cit().dist() << "\n";
  }
  
  // possibly a constraints file for solvent
  d_os << "\n#ifdef POSRES_WATER\n";
  d_os << "; Position restraint for each water oxygen\n";
  d_os << "[ position_restraints ]\n";
  d_os << ";  i funct       fcx        fcy        fcz\n";
  d_os << "   1    1       1000       1000       1000\n";
  d_os << "#endif\n";
  

  // Finally everything is put together in a system
  d_os << "\n[ system ]\n";
  d_os << "; Name\n";
  d_os << "SYSTEM\n";
  d_os << endl;
  
  d_os << "[ molecules ]\n";
  d_os << "; Compound        #mols\n";
  d_os << "MOL_1                 1\n";
 
  int numSolvent=sys.sol(0).numPos() / sys.sol(0).topology().numAtoms();
  
  d_os << "SOL" << setw(20) << numSolvent << endl;
  

  d_os << "\n; end of topology file\n";
  



}


