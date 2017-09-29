#ifndef HPSSIM_PRIMARYGENERATORACTION_H_
#define HPSSIM_PRIMARYGENERATORACTION_H_

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4VPrimaryGenerator.hh"
#include "G4Event.hh"

#include "PGAMessenger.h"
#include "UserPrimaryParticleInformation.h"

namespace hpssim {

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

        void setVerbose(int verbose) {
            verbose_ = verbose;
        }

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
                    gen->GeneratePrimaryVertex(overlayEvent);

                    if (overlayEvent->GetNumberOfPrimaryVertex()) {

                        // Apply event transforms to the overlay event.
                        for (auto transform : transforms) {
                            transform->transform(overlayEvent);
                        }

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

        void addGenerator(PrimaryGenerator* generator) {
            generators_.push_back(generator);
        }

        void initialize() {
            for (auto gen : generators_) {
                gen->initialize();
            }
        }

        static PrimaryGeneratorAction* getPrimaryGeneratorAction() {
            const PrimaryGeneratorAction* pga =
                    static_cast<const PrimaryGeneratorAction*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
            return const_cast<PrimaryGeneratorAction*>(pga);
        }

    private:

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

    protected:

        int verbose_{1};

    private:

        G4UImessenger* messenger_;

        /** List of primary generators to run for every Geant4 event. */
        std::vector<PrimaryGenerator*> generators_;
};

}

#endif
