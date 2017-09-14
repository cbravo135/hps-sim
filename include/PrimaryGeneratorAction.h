#ifndef HPSSIM_PRIMARYGENERATORACTION_H_
#define HPSSIM_PRIMARYGENERATORACTION_H_

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"

namespace hpssim {

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {

    public:

        PrimaryGeneratorAction() {
            gun_ = new G4ParticleGun();
        }

        virtual ~PrimaryGeneratorAction() {
            delete gun_;
        }

        virtual void GeneratePrimaries(G4Event* anEvent) {
            std::cout << "PrimaryGeneratorAction: generate primaries - " << anEvent->GetEventID() << std::endl;
            gun_->GeneratePrimaryVertex(anEvent);
        }

    private:

        G4ParticleGun* gun_;
};

}

#endif
