#include "RunManager.h"

/*
 * LCDD
 */
#include "lcdd/core/LCDDDetectorConstruction.hh"

/*
 * HPS
 */
#include "RunManager.h"
#include "PrimaryGeneratorAction.h"
#include "SteppingAction.h"
#include "UserTrackingAction.h"
#include "UserRunAction.h"
#include "UserEventAction.h"
#include "UserStackingAction.h"
#include "UnknownDecayPhysics.h"

namespace hpssim {

RunManager::RunManager() : G4RunManager() {

    // Make sure plugin manager is initialized.
    auto pluginMgr = PluginManager::getPluginManager();

    // Setup messenger for physics list.
    physListFactory_ = new G4PhysListFactory;
    physicsMessenger_ = new PhysicsMessenger(physListFactory_);

    // Setup detector construction.
    detectorConstruction_ = new LCDDDetectorConstruction();
    SetUserInitialization(detectorConstruction_);
}

RunManager::~RunManager() {
    delete physicsMessenger_;
    delete physListFactory_;
    delete lcioMgr_;
}

void RunManager::setupPhysList() {
    std::cout << "Creating physics list '" << physListName_ << "' ..." << std::endl;

    /*
     * Setup physics list and throw a fatal error if the name is invalid.
     */
    if (!physListFactory_->IsReferencePhysList(physListName_)) {
        G4Exception("RunManager::setupPhysList", "", FatalException,
                G4String("Invalid reference physics list: " + physListName_));
    }
    auto physList = physListFactory_->GetReferencePhysList(physListName_);

    // TODO: Add messenger command to toggle this extra physics on/off.
    physList->RegisterPhysics(new UnknownDecayPhysics);

    this->SetUserInitialization(physList);

    std::cout << "Done creating physics list!" << std::endl;
}

void RunManager::Initialize() {

    setupPhysList();

    G4RunManager::Initialize();

    // Register all user action classes with the run manager.
    SetUserAction(new PrimaryGeneratorAction);
    SetUserAction(new UserTrackingAction);
    SetUserAction(new UserRunAction);
    SetUserAction(new UserEventAction);
    SetUserAction(new SteppingAction);
    SetUserAction(new UserStackingAction);

    // Create the persistency manager (must go here to get pointer to track map).
    lcioMgr_ = new LcioPersistencyManager();
}

}
