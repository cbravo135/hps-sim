/**
 * @file LHEPrimaryGenerator.h
 * @brief Class for generating a Geant4 event from LHE event data
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef HPSSIM_LHEPRIMARYGENERATOR_H_
#define HPSSIM_LHEPRIMARYGENERATOR_H_

#include "G4RunManager.hh"
#include "G4VPrimaryGenerator.hh"

#include "LHEReader.h"
#include "PrimaryGenerator.h"

namespace hpssim {

/**
 * @class LHEPrimaryGenerator
 * @brief Generates a Geant4 event from an LHEEvent
 */
class LHEPrimaryGenerator : public PrimaryGenerator {

    public:

        /**
         * Class constructor.
         * @param reader The LHE reader with the event data.
         */
        LHEPrimaryGenerator(std::string name, LHEReader* reader);

        LHEPrimaryGenerator(std::string name) : PrimaryGenerator(name) {
            reader_ = nullptr;
        }

        /**
         * Class destructor.
         */
        virtual ~LHEPrimaryGenerator();

        /**
         * Generate vertices in the Geant4 event.
         * @param anEvent The Geant4 event.
         */
        void GeneratePrimaryVertex(G4Event* anEvent);

        void initialize() {
            std::cout << "LHEPrimaryGenerator: Setting file '" << files_[0] << "' on LHE reader." << std::endl;
            reader_ = new LHEReader(files_[0]);
        }

    private:

        /**
         * The LHE reader with the event data.
         */
        LHEReader* reader_;
};

}

#endif
