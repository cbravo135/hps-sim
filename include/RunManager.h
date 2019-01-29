#ifndef HPSSIM_RUNMANAGER_H_
#define HPSSIM_RUNMANAGER_H_

/*
 * Geant4
 */
#include "G4RunManager.hh"
#include "G4StateManager.hh"
#include "G4PhysListFactory.hh"

/**
 * HPS
 */
#include "PluginManager.h"
#include "PhysicsMessenger.h"

namespace hpssim {

/*
 * @class RunManager
 * @brief Custom Geant4 run manager implementation
 */
class RunManager: public G4RunManager {

    public:

        RunManager();

        virtual ~RunManager();

        static RunManager* getRunManager() {
            return static_cast<RunManager*>(G4RunManager::GetRunManager());
        }

        void setupPhysList();

        void setPhysListName(const std::string& physListName) {
            physListName_ = physListName;
        }

        /*
        void Initialize() {
            auto stateMgr = G4StateManager::GetStateManager();
            std::cout << "RunManager is initializing ..." << std::endl;
            std::cout << "state: "
                    << stateMgr->GetStateString(stateMgr->GetCurrentState())
                    << std::endl;
            G4RunManager::Initialize();
            std::cout << "RunManager is done initializing!" << std::endl;
            std::cout << "state: "
                    << stateMgr->GetStateString(stateMgr->GetCurrentState())
                    << std::endl;
        }
        */

    private:

        std::string physListName_{"FTFP_BERT"};
        G4UImessenger* physicsMessenger_{new PhysicsMessenger};
        G4PhysListFactory* physListFactory_{new G4PhysListFactory};
};
}

#endif
