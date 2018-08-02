#include "RunManager.h"

#include "UnknownDecayPhysics.h"

#include "FTFP_BERT.hh"

namespace hpssim {

void RunManager::InitializePhysics() {
// Don't do anything here until "state" issues are resolved.
/*
    G4VUserPhysicsList* thePhysicsList = new FTFP_BERT;
    G4VModularPhysicsList* modularPhysicsList = dynamic_cast<G4VModularPhysicsList*>(thePhysicsList);
    modularPhysicsList->RegisterPhysics(new UnknownDecayPhysics);
    G4RunManager::InitializePhysics();
*/
}

}
