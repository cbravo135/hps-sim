#include "PhysicsMessenger.h"

#include "RunManager.h"

namespace hpssim {

PhysicsMessenger::PhysicsMessenger() {
    physicsDir_ = new G4UIdirectory("/hps/physics/", this);

    physicsListCmd_ = new G4UIcmdWithAString("/hps/physics/list", this);
    physicsListCmd_->AvailableForStates(G4ApplicationState::G4State_PreInit);
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
