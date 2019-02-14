#ifndef HPSSIM_RUNMANAGER_H_
#define HPSSIM_RUNMANAGER_H_

/*
 * HPS
 */
#include "lcdd/core/LCDDDetectorConstruction.hh"

/*
 * Geant4
 */
#include "G4RunManager.hh"
#include "G4PhysListFactory.hh"

/**
 * HPS
 */
#include "LcioPersistencyManager.h"
#include "PluginManager.h"
#include "PhysicsMessenger.h"

namespace hpssim {

/*
 * @class RunManager
 * @brief Custom Geant4 run manager implementation
 */
class RunManager: public G4RunManager {

    public:

        /**
         * Class constructor.
         * @note Creates the plugin manager.
         */
        RunManager();

        /**
         * Class destructor.
         */
        virtual ~RunManager();

        /**
         * Get the instance of the custom run manager.
         */
        static RunManager* getRunManager() {
            return static_cast<RunManager*>(G4RunManager::GetRunManager());
        }

        /**
         * Set the name of the physics list.
         * @param physListName The name of the physics list.
         * @note Only available in the preinit state.
         */
        void setPhysListName(const std::string& physListName) {
            physListName_ = physListName;
        }

        /**
         * Get a pointer to the physics list factory.
         */
        G4PhysListFactory* getPhysListFactory() {
            return physListFactory_;
        }

        /**
         * Initialize the run manager.
         */
        void Initialize();

    private:

        /**
         * Setup the modular physics list.
         */
        void setupPhysList();

    private:

        /**
         * Name of the physics list with default.
         */
        std::string physListName_{"FTFP_BERT"};

        /**
         * Messenger for setting the name of the physics list.
         */
        G4UImessenger* physicsMessenger_{nullptr};

        /**
         * Factory class for instantiating the physics list.
         */
        G4PhysListFactory* physListFactory_{nullptr};

        /**
         * LCIO persistency manager.
         */
        LcioPersistencyManager* lcioMgr_{nullptr};

        /**
         * User detector construction.
         */
        LCDDDetectorConstruction* detectorConstruction_{nullptr};
};
}

#endif
