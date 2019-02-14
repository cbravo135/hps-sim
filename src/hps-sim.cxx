/*
 * C++
 */
#include <iostream>

/*
 * Geant4
 */
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"

/*
 * HPS
 */
#include "RunManager.h"

using namespace hpssim;

/**
 * Application's main entry point which performs all required setup of Geant4
 * and custom user classes in the correct initialization order.
 */
int main(int argc, char* argv[]) {

    // Create the Geant4 UI executive to process macro commands.
    G4UIExecutive* UIExec = 0;
    if (argc == 1) {
        UIExec = new G4UIExecutive(argc, argv);
    }

    // Initialize the custom run manager.
    RunManager* mgr = new RunManager();

    // Initialize the visualization engine.
    G4VisManager* vis = new G4VisExecutive;
    vis->Initialize();

    // Execute the UI session to run the application.
    G4UImanager* UImgr = G4UImanager::GetUIpointer();
    if (UIExec == 0) {
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        //std::cout << "Executing macro " << fileName << " ..." << std::endl;
        UImgr->ApplyCommand(command + fileName);
    } else {
        //std::cout << "Starting interactive session ..." << std::endl;
        UIExec->SessionStart();
        delete UIExec;
    }

    // Delete the run manager.
    delete mgr;
}
