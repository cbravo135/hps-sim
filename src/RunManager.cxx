#include "RunManager.h"

/*
 * HPS
 */
#include "UnknownDecayPhysics.h"

namespace hpssim {

RunManager::RunManager() : G4RunManager() {
    // Make sure plugin manager is initialized.
    auto pluginMgr = PluginManager::getPluginManager();
}

RunManager::~RunManager() {
    delete physicsMessenger_;
    delete physListFactory_;
}

void RunManager::setupPhysList() {
    std::cout << "Setting up physics list '" << physListName_ << "' ..." << std::endl;
    auto physList = physListFactory_->GetReferencePhysList(physListName_);
    physList->RegisterPhysics(new UnknownDecayPhysics);
    this->SetUserInitialization(physList);
    std::cout << "Setup physics list '" << physListName_ << "' successfully!" << std::endl;
}

}
