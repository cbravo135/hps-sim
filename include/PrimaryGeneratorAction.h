#ifndef HPSSIM_PRIMARYGENERATORACTION_H_
#define HPSSIM_PRIMARYGENERATORACTION_H_

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4VPrimaryGenerator.hh"
#include "G4Event.hh"

#include "PGAMessenger.h"

namespace hpssim {

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {

    public:

        PrimaryGeneratorAction() {
            messenger_ = new PGAMessenger(this);
        }

        virtual ~PrimaryGeneratorAction() {
            delete messenger_;
        }

        virtual void GeneratePrimaries(G4Event* anEvent) {

            for (auto gen : generators_) {

                std::cout << "PrimaryGeneratorAction: Generating an event from '" << gen->getName() << "'" << std::endl;

                // create new G4 event to overlay
                G4Event* overlayEvent = new G4Event();
                gen->GeneratePrimaryVertex(overlayEvent);

                std::cout << "PrimaryGeneratorAction: Generator '" << gen->getName() << "' created "
                        << overlayEvent->GetNumberOfPrimaryVertex() << " vertices." << std::endl;

                // overlay the event onto the target output event
                for (int ivtx = 0; ivtx < overlayEvent->GetNumberOfPrimaryVertex(); ivtx++) {
                    anEvent->AddPrimaryVertex(overlayEvent->GetPrimaryVertex(ivtx));
                }
            }
        }

        void addGenerator(PrimaryGenerator* generator) {
            generators_.push_back(generator);
        }

    private:

        G4UImessenger* messenger_;

        std::vector<PrimaryGenerator*> generators_;
        //std::map<G4VPrimaryGenerator*, std::vector<EventTransform*>> transforms_;
};

}

#endif
