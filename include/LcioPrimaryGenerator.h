/**
 * @file LHEPrimaryGenerator.h
 * @brief Class for generating a Geant4 event from LHE event data
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef HPSSIM_LCIOPRIMARYGENERATOR_H_
#define HPSSIM_LCIOPRIMARYGENERATOR_H_

#include "G4RunManager.hh"
#include "G4VPrimaryGenerator.hh"

#include "EVENT/LCCollection.h"
#include "EVENT/MCParticle.h"
#include "IO/LCReader.h"
#include "IOIMPL/LCFactory.h"

#include "PrimaryGenerator.h"

#include <set>

namespace hpssim {

/**
 * @class LHEPrimaryGenerator
 * @brief Generates a Geant4 event from an LCIO MCParticle collection
 *
 * @note It appears that the current incarnation of the SIOReader is 'clever'
 * about how it manages the LCEvent objects, so deleting them explicitly causes
 * seg faults!  For this reason, none of the events read from the file are
 * deleted.  This does not appear to cause a memory leak.
 */
class LcioPrimaryGenerator : public PrimaryGenerator {

    public:

        LcioPrimaryGenerator(std::string name);

        virtual ~LcioPrimaryGenerator();

        /**
         * Generate vertices in the Geant4 event from the MCParticle data.
         * @param anEvent The Geant4 event.
         */
        void GeneratePrimaryVertex(G4Event* anEvent);

        bool isFileBased();

        bool supportsRandomAccess();

        /**
         * Read an event by index for random access.
         * This method uses the cache of event numbers to find the event in the file
         * for that index, and then removes that record from the cache so it
         * is not reused.
         */
        void readEvent(long index, bool removeEvent) throw(NoSuchRecordException);

        /**
         * Read the next event sequentially from the SIO reader.
         */
        void readNextEvent() throw(EndOfFileException);

        /**
         * Open a new reader, deleting the old one if necessary, and get the first run header
         * from the file.  If there is no run header found, there will be a fatal exception,
         * because this is required for random access support.
         */
        void openFile(std::string file);

        /**
         * Return the number of events left in the cache for random access.
         */
        int getNumEvents();

        /**
         * Cache a list of valid event numbers in the file that can be used when running in random access mode.
         */
        void cacheEvents();

    private:

        /** The LCIO reader with the event data. */
        IO::LCReader* reader_{nullptr};

        /** The current LC event. */
        EVENT::LCEvent* lcEvent_{nullptr};

        /** The current run header. */
        EVENT::LCRunHeader* runHeader_{nullptr};

        /** List of event indices that is used for random access via the reader. */
        std::vector<long> events_;
};

}

#endif
