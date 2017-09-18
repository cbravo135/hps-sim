#include "LcioPersistencyMessenger.h"

// include only in cxx file due to circular dep!
#include "LcioPersistencyManager.h"

namespace hpssim {

LcioPersistencyMessenger::LcioPersistencyMessenger(LcioPersistencyManager* mgr) : mgr_{mgr} {
    dir_ = new G4UIdirectory("/hps/lcio", this);
    fileCmd_ = new G4UIcmdWithAString("/hps/lcio/file", this);
    verboseCmd_ = new G4UIcmdWithAnInteger("/hps/lcio/verbose", this);
}

void LcioPersistencyMessenger::SetNewValue(G4UIcommand* command, G4String newValues) {
    if (command == this->fileCmd_) {
        std::cout << "LcioPersistencyMessenger: Setting output file to '" << newValues << "'" << std::endl;
        mgr_->setOutputFile(newValues);
    } else if (command == this->verboseCmd_) {
        std::cout << "LcioPersistencyMessenger: Setting verbose level to " << newValues << std::endl;
        mgr_->SetVerboseLevel(G4UIcmdWithAnInteger::GetNewIntValue(newValues));
    }
}

}
