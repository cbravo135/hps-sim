#ifndef HPSSIM_PRIMARYGENERATORACTION_H_
#define HPSSIM_PRIMARYGENERATORACTION_H_ 1

#include "G4VUserPrimaryGeneratorAction.hh"

namespace hpssim {

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {

    public:

        PrimaryGeneratorAction() {
        }

        virtual ~PrimaryGeneratorAction() {
        }

        virtual void GeneratePrimaries(G4Event* anEvent) {
            std::cout << "PrimaryGeneratorAction: generate primaries - " << anEvent->GetEventID() << std::endl;
        }
};

}

#endif
