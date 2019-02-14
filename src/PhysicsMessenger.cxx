#include "PhysicsMessenger.h"

/*
 * HPS
 */
#include "RunManager.h"

/*
 * C++
 */
#include <sstream>

namespace hpssim {

PhysicsMessenger::PhysicsMessenger(G4PhysListFactory* fac) {

    physicsDir_ = new G4UIdirectory("/hps/physics/", this);

    physicsListCmd_ = new G4UIcmdWithAString("/hps/physics/list", this);
    physicsListCmd_->AvailableForStates(G4ApplicationState::G4State_PreInit);
    physicsListCmd_->SetGuidance("Set the name of the Geant4 reference physics list.");

    // Setup guidance with names of available physics lists.
    auto param = physicsListCmd_->GetParameter(0);
    std::stringstream ss;
    for (const G4String& listName : fac->AvailablePhysLists()) {
        ss << listName << " ";
    }
    param->SetGuidance(ss.str().c_str());
}

PhysicsMessenger::~PhysicsMessenger() {
    delete physicsListCmd_;
}

void PhysicsMessenger::SetNewValue(G4UIcommand* command, G4String newValues) {
    if (command == physicsListCmd_) {
        RunManager::getRunManager()->setPhysListName(newValues);
    }
}

} // namespace hpssim
