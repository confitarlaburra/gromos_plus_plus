/** 
 * @file   SoluteWeightedDistance.h
 */

#ifndef SOLUTEWEIGHTEDDISTANCE_H
#define	SOLUTEWEIGHTEDDISTANCE_H

#include <fstream>
#include "AtomSpecifier.h"

namespace args {
  class Arguments;
}

namespace gcore {
  class System;
}

namespace bound {
  class Boundary;
}

namespace utils {
  
  /**
   * @struct SWD_Param 
   * 
   * Holds parameters (force constant and cut off) 
   * for the solute wigthed distance.
   */
  struct SWD_Param {
    SWD_Param(double f, double c) :
    forceConstant(f),
    cutoff(c)
    {}
    double forceConstant;
    double cutoff;
  };
  
  /**
   * @class SoluteWeightedDistance
   * 
   * Calculates the solute weighted distance for coarse grained and
   * fine grained solvents from the solute as well as their energies.
   */
  class SoluteWeightedDistance {
  public:
    SoluteWeightedDistance(gcore::System &sys, args::Arguments &args);
    ~SoluteWeightedDistance();
    
    /**
     * Calculate the distances
     */
    void calculate(double time = 0.0);
    /**
     * Return the title
     */
    std::string title();
    /**
     * Print distances to os.
     */
    void distances(std::ostream &os) const;
    /// Alias for the distances
    typedef std::vector<double> Distances;
    /**
     * Return the fine grained distances
     */
    Distances &fgDistances();
    /**
     * Return the coarse grained distances
     */
    Distances &cgDistances();
    /**
     * Atomspecifier of the solute;
     */
    AtomSpecifier &solute();
    /**
     * Atomspecifier of the fine grained solvent;
     */
    AtomSpecifier &fgSolvent();
    /**
     * Atomspecifier of the coarse grained solvent;
     */
    AtomSpecifier &cgSolvent();
    /**
     * Print energies to os.
     */
    void energies(std::ostream &os) const;
    /**
     * Do we have the energy parameters?
     */
    bool withEnergy() const;
    /** 
     * What is the name of the energy out file?
     */
    std::string energyFile() const;
  protected:
    AtomSpecifier _solute;
    AtomSpecifier _fgSolvent;
    AtomSpecifier _cgSolvent;
    
    typedef std::vector<SWD_Param> Params;
    Params _params;
    std::string _energyFile;
    bool _withEnergy;
    
    Distances _fgDistances;
    Distances _cgDistances;
    
    int _exponent;
    bool _withMeasures;
    std::ofstream _foutMeasures;
    typedef std::vector<double> Measures;
    Measures _measures;
    void _writeMeasures(double time);
    
    bool _withWeights;
    std::ofstream _foutWeights;
    typedef std::vector<double> Weights;
    Weights _weights;
    void _writeWeights(double R_j);
    
    bool _withMinimalDistances;
    std::ofstream _foutMinDists;
    typedef std::vector<double> MinDists;
    MinDists _minDists;
    void _writeMinDists(double time);
    
    gcore::System &_sys;
    bound::Boundary *_pbc;
  };
  
  std::ostream &operator<<(std::ostream &os, SoluteWeightedDistance const & sad);
}

#endif	/* SOLUTEAVERAGEDISTANCE_H */

