#ifndef HPSSIM_PRIMARYGENERATORACTION_H_
#define HPSSIM_PRIMARYGENERATORACTION_H_

#include "CLHEP/Random/RandFlat.h"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4VPrimaryGenerator.hh"
#include "G4Event.hh"

#include "PGAMessenger.h"
#include "UserPrimaryParticleInformation.h"

namespace hpssim {

/**
 * @class PrimaryGeneratorAction
 * @brief Performs event generation using the currently registered list of PrimaryGenerator objects.
 */
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {

    public:

        PrimaryGeneratorAction() {
            messenger_ = new PGAMessenger(this);
        }

        virtual ~PrimaryGeneratorAction() {
            delete messenger_;

            for (auto generator : generators_) {
                delete generator;
            }
        }

        /**
         * Set the verbose level.
         */
        void setVerbose(int verbose) {
            verbose_ = verbose;
        }

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
        virtual void GeneratePrimaries(G4Event* anEvent) {

            if (verbose_ > 1) {
                std::cout << "PrimaryGenerationAction: Generating event " << anEvent->GetEventID() << std::endl;
            }

            for (auto gen : generators_) {

                if (verbose_ > 1) {
                    std::cout << "PrimaryGeneratorAction: Running generator '" << gen->getName() << "'" << std::endl;
                }

                // Get the list of event transforms to be applied to each generated event.
                auto transforms = gen->getTransforms();

                // Generate N event samples based on sampling setting.
                int nevents = gen->getEventSampling()->getNumberOfEvents(anEvent);
                if (verbose_ > 1) {
                    std::cout << "PrimaryGeneratorAction: Sampling " << nevents << " events from '" << gen->getName() << "'" << std::endl;
                }
                for (int iEvent = 0; iEvent < nevents; iEvent++) {

                    // Create a new event to overlay.
                    G4Event* overlayEvent = new G4Event();

                    // Read next event.
                    readNextEvent(gen);

                    // Generate a primary vertex.
                    gen->GeneratePrimaryVertex(overlayEvent);

                    // Only apply transforms and overlay the event if something was actually generated.
                    if (overlayEvent->GetNumberOfPrimaryVertex()) {

                        // Apply event transforms to the overlay event.
                        gen->applyTransforms(overlayEvent);

                        if (verbose_ > 2) {
                            std::cout << "PrimaryGeneratorAction: Generator '" << gen->getName() << "' created "
                                    << overlayEvent->GetNumberOfPrimaryVertex() << " vertices in sample " << iEvent
                                    << std::endl;
                        }

                        // Overlay the event onto the target event by deep copying the first vertex object.
                        anEvent->AddPrimaryVertex(new G4PrimaryVertex(*overlayEvent->GetPrimaryVertex(0)));
          
                        // Delete the overlay event to avoid memory leak.
                        delete overlayEvent;
                    }
                }
            }

            // Set the generator status on the primaries and attach a user info object, if needed.
            setGenStatus(anEvent);
        }

        /**
         * Add a PrimaryGenerator to the list.
         */
        void addGenerator(PrimaryGenerator* generator) {
            generators_.push_back(generator);
        }

        /**
         * Initialize all PrimaryGenerator objects before run starts.
         */
        void initialize() {
            for (auto gen : generators_) {

                // Initialization for generators with files.
                if (gen->isFileBased()) {

                    // Queues up all files for the generator for processing.
                    gen->queueFiles();

                    // Reads the next file from the generator.
                    gen->readNextFile();
                }

                // Call generator's initialization hook.
                gen->initialize();
            }
        }

        /**
         * Get the global instance of this class.
         */
        static PrimaryGeneratorAction* getPrimaryGeneratorAction() {
            const PrimaryGeneratorAction* pga =
                    static_cast<const PrimaryGeneratorAction*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
            return const_cast<PrimaryGeneratorAction*>(pga);
        }

    private:

        /**
         * Set the generator status on G4PrimaryParticle objects in the event with
         * extra user info.
         */
        void setGenStatus(G4Event* anEvent) {
            for (int iVtx = 0; iVtx < anEvent->GetNumberOfPrimaryVertex(); iVtx++) {
                auto vertex = anEvent->GetPrimaryVertex(iVtx);
                G4PrimaryParticle* primaryParticle = vertex->GetPrimary();
                while (primaryParticle) {
                    setGenStatus(primaryParticle);
                    primaryParticle = primaryParticle->GetNext();
                }
            }
        }

        /**
         * This method is called recursively to set the generator status on the G4PrimaryParticle
         * objects and their daughters.
         */
        void setGenStatus(G4PrimaryParticle* primaryParticle) {

            auto info = UserPrimaryParticleInformation::getUserPrimaryParticleInformation(primaryParticle);

            // Create a new user info object if needed; maybe the generator itself didn't do this!
            if (!info) {
                info = new UserPrimaryParticleInformation;
                primaryParticle->SetUserInformation(info);
            }

            G4PrimaryParticle* dau = primaryParticle->GetDaughter();
            if (dau) {
                // Particles with daughters are given status of 'intermediate'.
                info->setGenStatus(2);
                while (dau) {
                    // Process the list of daughter particles recursively.
                    setGenStatus(dau);
                    dau = dau->GetNext();
                }
            } else {
                // Particles with no daughters are given status of 'final state'.
                info->setGenStatus(1);
            }
        }

        /**
         * Reads in the next event from the generator and manages exceptions that might occur.
         *
         * @todo The logic in this method would be helped if the generators could peek into their
         * data stream to see if there are more events left.
         */
        void readNextEvent(hpssim::PrimaryGenerator* gen) throw (EndOfDataException) {
            try {
                // Perform read on the generator to load data.
                doNextRead(gen);
            } catch (EndOfFileException& eof) {
                // End of this file, so load next file...
                try {

                    // Read in the next file, which will throw an exception if there are no more files left.
                    gen->readNextFile();

                    // Now try to read the first event from the new file.
                    try {
                        doNextRead(gen);
                    } catch (EndOfFileException& e) {
                        // No events in the file or it is corrupt!
                        G4Exception("", "", RunMustBeAborted,
                                G4String("Failed to read first event from '" + gen->getName() + "'."));
                    } catch (std::exception& err) {
                        // Some unknown error occurred while reading this event.
                        G4Exception("", "", RunMustBeAborted,
                                G4String("Error reading events from '" + gen->getName() + "'."));
                    }
                } catch (EndOfDataException& eod) {
                    // Probably we are out of files.
                    G4Exception("", "", RunMustBeAborted,
                            G4String("Event generator '" + gen->getName() + "' ran out of files."));
                }
            } catch (std::exception& err) {
                // Some unknown error occurred while reading an event.
                G4Exception("", "", RunMustBeAborted,
                        G4String("Error reading events from '" + gen->getName() + "'."));
            }
        }

        /**
         * Performs the method calls on PrimaryGenerator to read the next generator event.
         */
        void doNextRead(hpssim::PrimaryGenerator* gen) {
            if (gen->getReadMode() == PrimaryGenerator::Random) {
                /*
                 * Read a random event from this file generating a number between zero
                 * and the max event index.
                 */
                int numEvents = gen->getNumEvents();
                if (numEvents > 0) {
                    long randEvent = CLHEP::RandFlat::shootInt((long)0, (long)(numEvents - 1));
                    if (verbose_ > 1) {
                        std::cout << "PrimaryGeneratorAction: Reading random event " << randEvent << " from '"
                                << gen->getName() + "'" << std::endl;
                    }
                    if (gen->getReadFlag()) {
                        gen->readEvent(randEvent, true);
                    }
                } else {
                    /*
                     * Generator ran out of events so throw an exception that indicates this.
                     */
                    throw EndOfFileException();
                }
            } else {

                if (verbose_ > 2) {
                    std::cout << "PrimaryGeneratorAction: Reading event read from '" << gen->getName() << "' sequentially" << std::endl;
                }

                /*
                 * Sequentially read next event.
                 */
                if (gen->getReadFlag()) {
                    gen->readNextEvent();
                } 
            }
        }

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
