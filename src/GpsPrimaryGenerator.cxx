/**
 * @file GpsPrimaryGenerator.cxx
 * @brief Allows the use of general particle source to generate particles.
 * @author Omar Moreno, SLAC National Accelerator Laboratory
 */

//-------------//
//   hps-sim   //
//-------------//
#include "GpsPrimaryGenerator.h"

namespace hpssim { 

    GpsPrimaryGenerator::GpsPrimaryGenerator(std::string name) : 
        PrimaryGenerator(name) { 
    }

    void GpsPrimaryGenerator::GeneratePrimaryVertex(G4Event* event) { 
        gps_->GeneratePrimaryVertex(event);     
    }
    
} // hpssim

