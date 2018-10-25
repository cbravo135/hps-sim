/**
 * @file GpsPrimaryGenerator.h
 * @brief Allows the use of general particle source to generate particles.
 * @author Omar Moreno, SLAC National Accelerator Laboratory
 */

//------------//
//   Geant4   //
//------------//
#include "G4GeneralParticleSource.hh"

//-------------//
//   hps-sim   //
//-------------//
#include "PrimaryGenerator.h"

namespace hpssim { 

    class GpsPrimaryGenerator : public PrimaryGenerator { 
        
        public: 

            /**
             * Constructor
             *
             * @param name Name of the generator 
             */
            GpsPrimaryGenerator(std::string name);  

            /** Method used to generate an event. */
            void GeneratePrimaryVertex(G4Event* event); 

        private: 

            /** General particle source */ 
            G4VPrimaryGenerator* gps_{new G4GeneralParticleSource}; 
    
    }; // GpsPrimaryGenerator

} // hpssim
