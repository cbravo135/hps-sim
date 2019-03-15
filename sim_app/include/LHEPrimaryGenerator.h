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
class LHEPrimaryGenerator: public PrimaryGenerator {

    public:

        /**
         * Class constructor.
         * @param reader The LHE reader with the event data.
         */
        LHEPrimaryGenerator(std::string name, LHEReader* reader);

        LHEPrimaryGenerator(std::string name);

        /**
         * Class destructor.
         */
        virtual ~LHEPrimaryGenerator();

        /**
         * Generate vertices in the Geant4 event.
         * @param anEvent The Geant4 event.
         */
        void GeneratePrimaryVertex(G4Event* anEvent);

        bool isFileBased();

        bool supportsRandomAccess();

        int getNumEvents();

        void readNextEvent() throw(EndOfFileException);

        void readEvent(long index, bool removeEvent) throw(NoSuchRecordException);

        void openFile(std::string file);

        void cacheEvents();

        void deleteEvent();

    private:

        // Setup event sampling if using cross section.
        void setupEventSampling();

    private:

        /** The LHE reader with the event data. */
        LHEReader* reader_;

        /** The current LHE event. */
        LHEEvent* lheEvent_;

        /** Queue of LHE events when running in random mode. */
        std::vector<LHEEvent*> events_;
};

}

#endif
