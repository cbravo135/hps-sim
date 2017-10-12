#include "LcioMergeMessenger.h"

#include "LcioMergeTool.h"

namespace hpssim {

LcioMergeMessenger::LcioMergeMessenger(LcioMergeTool* merge) : merge_(merge) {

    G4String mergePath = "/hps/lcio/merge/" + merge->getName() + "/";
    mergeDir_ = new G4UIdirectory(mergePath, this);

    G4String filterPath = mergePath + "filter/";
    filterDir_ = new G4UIdirectory(filterPath, this);

    G4String filePath = mergePath + "file";
    fileCmd_ = new G4UIcmdWithAString(filePath, this);

    G4String eventModulusPath = filterPath + "eventModulus";
    //std::cout << eventModulusPath << std::endl;
    eventModulusFilterCmd_ = new G4UIcmdWithAnInteger(eventModulusPath, this);

    //ecalEnergyFilterCmd_
}

void LcioMergeMessenger::SetNewValue(G4UIcommand* command, G4String newValues) {
    if (command == fileCmd_) {
        merge_->addFile(newValues);
    } else if (command == eventModulusFilterCmd_) {
        auto filter = new LcioMergeTool::EventModulusFilter();
        auto modulus = G4UIcmdWithAnInteger::GetNewIntValue(newValues);
        filter->setModulus(modulus);
        merge_->addFilter(filter);
    }
}

}
