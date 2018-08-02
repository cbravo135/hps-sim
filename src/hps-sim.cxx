#include <iostream>

#include "FTFP_BERT.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"

#include "lcdd/core/LCDDDetectorConstruction.hh"

#include "SteppingAction.h"
#include "LcioPersistencyManager.h"
#include "PluginManager.h"
#include "PrimaryGeneratorAction.h"
#include "RunManager.h"
#include "UserTrackingAction.h"
#include "UserRunAction.h"
#include "UserEventAction.h"
#include "UserStackingAction.h"

using namespace hpssim;

int main(int argc, char* argv[]) {

    std::cout << "Hello hps-sim!" << std::endl;

    G4UIExecutive* UIExec = 0;
    if (argc == 1) {
        UIExec = new G4UIExecutive(argc, argv);
    }

    RunManager* mgr = new RunManager();

    auto pluginMgr = PluginManager::getPluginManager();

    LCDDDetectorConstruction* det = new LCDDDetectorConstruction();

    mgr->SetUserInitialization(det);
    mgr->SetUserInitialization(new FTFP_BERT);
    mgr->SetUserAction(new PrimaryGeneratorAction);
    mgr->SetUserAction(new UserTrackingAction);
    mgr->SetUserAction(new UserRunAction);
    mgr->SetUserAction(new UserEventAction);
    mgr->SetUserAction(new SteppingAction);
    mgr->SetUserAction(new UserStackingAction);

    LcioPersistencyManager* lcio = new LcioPersistencyManager();

    G4VisManager* vis = new G4VisExecutive;
    vis->Initialize();

    G4UImanager* UImgr = G4UImanager::GetUIpointer();

    if (UIExec == 0) {
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        std::cout << "Executing macro " << fileName << " ..." << std::endl;
        UImgr->ApplyCommand(command + fileName);
    } else {
        std::cout << "Starting interactive session ..." << std::endl;
        UIExec->SessionStart();
        delete UIExec;
    }

    delete lcio;
    delete mgr;

    std::cout << "Bye hps-sim!" << std::endl;
}
