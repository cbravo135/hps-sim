#include "PrimaryGeneratorAction.h"

#include "CLHEP/Random/RandFlat.h"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4VPrimaryGenerator.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"

#include "PGAMessenger.h"
#include "UserPrimaryParticleInformation.h"

namespace hpssim {

PrimaryGeneratorAction::PrimaryGeneratorAction() {
    messenger_ = new PGAMessenger(this);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete messenger_;

    for (auto generator : generators_) {
        delete generator;
    }
}

void PrimaryGeneratorAction::setVerbose(int verbose) {
    verbose_ = verbose;
}

std::vector<PrimaryGenerator*>& PrimaryGeneratorAction::getGenerators() {
    return generators_;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {

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
            std::cout << "PrimaryGeneratorAction: Sampling " << nevents << " events from '" << gen->getName() << "'"
                    << std::endl;
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

            // When reading multiple events at a time, we cannot reread the same event again so must delete here.
            if (nevents > 1) {
                gen->deleteEvent();
            }
        }
    }

    // Set the generator status on the primaries and attach a user info object, if needed.
    setGenStatus(anEvent);
}

void PrimaryGeneratorAction::addGenerator(PrimaryGenerator* generator) {
    generators_.push_back(generator);
}

void PrimaryGeneratorAction::initialize() {
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

void PrimaryGeneratorAction::endEvent(const G4Event*) {
    for (auto gen : generators_) {
        if (gen->getReadFlag()) {
            gen->deleteEvent();
        }
    }
}

PrimaryGeneratorAction* PrimaryGeneratorAction::getPrimaryGeneratorAction() {
    const PrimaryGeneratorAction* pga =
            static_cast<const PrimaryGeneratorAction*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
    return const_cast<PrimaryGeneratorAction*>(pga);
}

void PrimaryGeneratorAction::setGenStatus(G4Event* anEvent) {
    for (int iVtx = 0; iVtx < anEvent->GetNumberOfPrimaryVertex(); iVtx++) {
        auto vertex = anEvent->GetPrimaryVertex(iVtx);
        G4PrimaryParticle* primaryParticle = vertex->GetPrimary();
        while (primaryParticle) {
            setGenStatus(primaryParticle);
            primaryParticle = primaryParticle->GetNext();
        }
    }
}

void PrimaryGeneratorAction::setGenStatus(G4PrimaryParticle* primaryParticle) {

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

void PrimaryGeneratorAction::readNextEvent(hpssim::PrimaryGenerator* gen) throw (EndOfDataException) {
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
                G4Exception("", "", RunMustBeAborted, G4String("Error reading events from '" + gen->getName() + "'."));
            }
        } catch (EndOfDataException& eod) {
            // Probably we are out of files.
            G4Exception("", "", RunMustBeAborted,
                    G4String("Event generator '" + gen->getName() + "' ran out of files."));
        }
    } catch (std::exception& err) {
        // Some unknown error occurred while reading an event.
        G4Exception("", "", RunMustBeAborted, G4String("Error reading events from '" + gen->getName() + "'."));
    }
}

void PrimaryGeneratorAction::doNextRead(hpssim::PrimaryGenerator* gen) {
    if (gen->getReadMode() == PrimaryGenerator::Random) {
        /*
         * Read a random event from this file generating a number between zero
         * and the max event index.
         */
        int numEvents = gen->getNumEvents();
        if (numEvents > 0) {
            long randEvent = CLHEP::RandFlat::shootInt((long) 0, (long) (numEvents - 1));
            if (verbose_ > 1) {
                std::cout << "PrimaryGeneratorAction: Reading random event " << randEvent << " from '"
                        << gen->getName() + "'" << std::endl;
            }
            if (gen->getReadFlag()) {
                gen->readEvent(randEvent, false); // * MWH * deleting the event is wicked expensive.
            } else {
                if (verbose_ > 1) {
                    std::cout << "PrimaryGeneratorAction: New event was not read from '" << gen->getName()
                            << "' because read flag was set to 'false'." << std::endl;
                }
            }
        } else {
            /*
             * Generator ran out of events so throw an exception that indicates this.
             */
            throw EndOfFileException();
        }
    } else if (gen->getReadMode() == PrimaryGenerator::Sequential){

        if (verbose_ > 2) {
            std::cout << "PrimaryGeneratorAction: Reading event read from '" << gen->getName() << "' sequentially"
                    << std::endl;
        }

        /*
         * Sequentially read next event.
         */
        if (gen->getReadFlag()) {
            gen->readNextEvent();
        } else {
            if (verbose_ > 1) {
                std::cout << "PrimaryGeneratorAction: New event was not read from '" << gen->getName()
                        << "' because read flag was set to 'false'." << std::endl;
            }
        }
    } else if (gen->getReadMode() == PrimaryGenerator::Linear){
      /*
       * Read a random event from this file generating a number between zero
       * and the max event index.
       */
      static long current_event=0;
      int numEvents = gen->getNumEvents();
      if (current_event < numEvents ) {
        long linEvent = (++current_event);
        if (verbose_ > 1) {
          std::cout << "PrimaryGeneratorAction: Reading linear event " << linEvent << " from '"
          << gen->getName() + "'" << std::endl;
        }
        if (gen->getReadFlag()) {
          gen->readEvent(linEvent, false);
        } else {
          if (verbose_ > 1) {
            std::cout << "PrimaryGeneratorAction: New event was not read from '" << gen->getName()
            << "' because read flag was set to 'false'." << std::endl;
          }
        }
      } else {
        /*
         * Generator ran out of events so throw an exception that indicates this.
         */
        throw EndOfFileException();
      }
    } else if (gen->getReadMode() == PrimaryGenerator::SemiRandom){
    } else {
      std::cerr << "Invalid reading mode requested from PrimaryGeneratorAction::doNextRead() " << std::endl;
    }
}

}
