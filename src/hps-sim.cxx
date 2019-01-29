#include <iostream>

/*
 * Geant4
 */
#include "FTFP_BERT.hh"
#include "G4RunManager.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"

/*
 * LCDD
 */
#include "lcdd/core/LCDDDetectorConstruction.hh"

/*
 * HPS
 */
#include "SteppingAction.h"
#include "LcioPersistencyManager.h"
#include "PrimaryGeneratorAction.h"
#include "RunManager.h"
#include "UserTrackingAction.h"
#include "UserRunAction.h"
#include "UserEventAction.h"
#include "UserStackingAction.h"
#include "UnknownDecayPhysics.h"

using namespace hpssim;

int main(int argc, char* argv[]) {

    std::cout << "Hello hps-sim!" << std::endl;

    G4UIExecutive* UIExec = 0;
    if (argc == 1) {
        UIExec = new G4UIExecutive(argc, argv);
    }

    std::cout << "Setting up RunManager ..." << std::endl;
    RunManager* mgr = new RunManager();
    std::cout << "Done setting up RunManager!" << std::endl;

    mgr->setupPhysList();
    std::cout << "Done setting up phys list" << std::endl;

    LCDDDetectorConstruction* det = new LCDDDetectorConstruction();
    mgr->SetUserInitialization(det);
    std::cout << "Registered detector construction" << std::endl;

    mgr->SetUserAction(new PrimaryGeneratorAction);
    mgr->SetUserAction(new UserTrackingAction);
    mgr->SetUserAction(new UserRunAction);
    mgr->SetUserAction(new UserEventAction);
    mgr->SetUserAction(new SteppingAction);
    mgr->SetUserAction(new UserStackingAction);
    std::cout << "Registered user actions" << std::endl;

    LcioPersistencyManager* lcio = new LcioPersistencyManager();
    std::cout << "Created persistency manager" << std::endl;

    G4VisManager* vis = new G4VisExecutive;
    vis->Initialize();
    std::cout << "Initialized vis engine" << std::endl;

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

    std::cout << "Application is exiting ..." << std::endl;

    delete lcio;
    delete mgr;

    std::cout << "Bye hps-sim!" << std::endl;
}
