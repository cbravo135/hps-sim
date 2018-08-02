#ifndef HPSSIM_PRIMARYGENERATORACTION_H_
#define HPSSIM_PRIMARYGENERATORACTION_H_

#include "CLHEP/Random/RandFlat.h"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4VPrimaryGenerator.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"

#include "PGAMessenger.h"
#include "UserPrimaryParticleInformation.h"

namespace hpssim {

/**
 * @class PrimaryGeneratorAction
 * @brief Performs event generation using the currently registered list of PrimaryGenerator objects.
 */
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {

    public:

        PrimaryGeneratorAction();

        virtual ~PrimaryGeneratorAction();
        /**
         * Set the verbose level.
         */
        void setVerbose(int verbose);

        std::vector<PrimaryGenerator*>& getGenerators();

        /**
         * Generate primaries using the current set of event generators.
         *
         * Instead of using a single generator, this class iterates over a list of
         * PrimaryGenerator objects that generate single events and optionally
         * transforms them before they are overlaid over the actual Geant4 event.
         *
         * @note
         * Method pseudo-code:
         * @code
         * GeneratorPrimaries(G4Event anEvent)
         *     foreach gen in generators:
         *         nevents = gen.getNumberOfEventsFromSampling()
         *         for i = 0 to nevents:
         *             overlayEvent = new G4Event()
         *             gen.readNextEvent()
         *             gen.generatePrimaryVertex(overlayEvent)
         *             gen.applyTransforms(overlayEvent)
         *             overlayFromTo(overlayEvent, anEvent)
         * @endcode
         */
        virtual void GeneratePrimaries(G4Event* anEvent);

        /**
         * Add a PrimaryGenerator to the list.
         */
        void addGenerator(PrimaryGenerator* generator);

        /**
         * Initialize all PrimaryGenerator objects before run starts.
         */
        void initialize();

        void endEvent(const G4Event*);

        /**
         * Get the global instance of this class.
         */
        static PrimaryGeneratorAction* getPrimaryGeneratorAction();

    private:

        /**
         * Set the generator status on G4PrimaryParticle objects in the event with
         * extra user info.
         */
        void setGenStatus(G4Event* anEvent);

        /**
         * This method is called recursively to set the generator status on the G4PrimaryParticle
         * objects and their daughters.
         */
        void setGenStatus(G4PrimaryParticle* primaryParticle);

        /**
         * Reads in the next event from the generator and manages exceptions that might occur.
         *
         * @todo The logic in this method would be helped if the generators could peek into their
         * data stream to see if there are more events left.
         */
        void readNextEvent(hpssim::PrimaryGenerator* gen) throw (EndOfDataException);

        /**
         * Performs the method calls on PrimaryGenerator to read the next generator event.
         */
        void doNextRead(hpssim::PrimaryGenerator* gen);

    protected:

        /** Verbose level with access for sub-classes. */
        int verbose_{1};

    private:

        /** Geant4 UI messenger for macro command processing. */
        G4UImessenger* messenger_;

        /** List of primary generators to run for every Geant4 event. */
        std::vector<PrimaryGenerator*> generators_;
};

}

#endif
