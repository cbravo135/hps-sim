#include <iostream>

#include "G4RunManager.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4RunManager.hh"
#include "FTFP_BERT.hh"

#include "lcdd/core/LCDDDetectorConstruction.hh"

#include "PrimaryGeneratorAction.h"

using namespace hpssim;

int main(int argc, char* argv[]) {

    std::cout << "Hello hps-sim!" << std::endl;

    G4UIExecutive* UIExec = 0;
    if (argc == 1) {
        UIExec = new G4UIExecutive(argc, argv);
    }

    G4RunManager* run = new G4RunManager();

    LCDDDetectorConstruction* det = new LCDDDetectorConstruction();

    run->SetUserInitialization(det);
    run->SetUserInitialization(new FTFP_BERT);
    run->SetUserAction(new PrimaryGeneratorAction);

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

    delete run;

    std::cout << "Bye hps-sim!" << std::endl;
}
