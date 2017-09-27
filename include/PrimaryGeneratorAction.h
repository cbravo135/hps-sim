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
        }

        virtual void GeneratePrimaries(G4Event* anEvent) {

            std::cout << "PrimaryGenerationAction: Generating event " << anEvent->GetEventID() << std::endl;

            for (auto gen : generators_) {

                //std::cout << "PrimaryGeneratorAction: Running generator '" << gen->getName() << "'." << std::endl;

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

                    //std::cout << "PrimaryGeneratorAction: Generator '" << gen->getName() << "' created "
                    //        << genEvent->GetNumberOfPrimaryVertex() << " vertices in sample " << (iEvent + 1) << "."
                    //        << std::endl;

                    // overlay the event onto the target output event
                    for (int ivtx = 0; ivtx < genEvent->GetNumberOfPrimaryVertex(); ivtx++) {
                        //std::cout << "PrimaryGeneratorAction: Overlaying vertex " << ivtx << " from '" << gen->getName() << "' into event." << std::endl;
                        anEvent->AddPrimaryVertex(new G4PrimaryVertex(*genEvent->GetPrimaryVertex(ivtx)));
                    }
          
                    delete genEvent;
                }
            }

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
            if (!info) {
                info = new UserPrimaryParticleInformation;
                primaryParticle->SetUserInformation(info);
            }
            G4PrimaryParticle* dau = primaryParticle->GetDaughter();
            if (dau) {
                info->setGenStatus(2);
                while (dau) {
                    setGenStatus(dau);
                    dau = dau->GetNext();
                }
            } else {
                info->setGenStatus(1);
            }
        }

    private:

        G4UImessenger* messenger_;

        std::vector<PrimaryGenerator*> generators_;
};

}

#endif
