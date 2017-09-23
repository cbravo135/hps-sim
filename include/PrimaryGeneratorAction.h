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

            std::cout << "PrimaryGenerationAction: Generating event " << anEvent->GetEventID() << std::endl;

            for (auto gen : generators_) {

                std::cout << "PrimaryGeneratorAction: Running generator '" << gen->getName() << "'." << std::endl;

                auto transforms = gen->getTransforms();

                int nevents = gen->getEventSampling()->getNumberOfEvents(anEvent);
                for (int iEvent = 0; iEvent < nevents; iEvent++) {

                    // create new G4 event to overlay
                    G4Event* genEvent = new G4Event();
                    gen->GeneratePrimaryVertex(genEvent);

                    // apply event transforms
                    for (auto transform : transforms) {
                        transform->transform(genEvent);
                    }

                    std::cout << "PrimaryGeneratorAction: Generator '" << gen->getName() << "' created "
                            << genEvent->GetNumberOfPrimaryVertex() << " vertices in sample " << (iEvent + 1) << "."
                            << std::endl;

                    // overlay the event onto the target output event
                    for (int ivtx = 0; ivtx < genEvent->GetNumberOfPrimaryVertex(); ivtx++) {
                        std::cout << "PrimaryGeneratorAction: Overlaying vertex " << ivtx << " from '" << gen->getName() << "' into event." << std::endl;
                        anEvent->AddPrimaryVertex(new G4PrimaryVertex(*genEvent->GetPrimaryVertex(ivtx)));
                    }
          
                    delete genEvent;
                }
            }
        }

        void addGenerator(PrimaryGenerator* generator) {
            generators_.push_back(generator);
        }

    private:

        G4UImessenger* messenger_;

        std::vector<PrimaryGenerator*> generators_;
};

}

#endif
