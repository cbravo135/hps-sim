#ifndef HPSSIM_USERRUNACTION_H_
#define HPSSIM_USERRUNACTION_H_

/*
 * Geant4
 */
#include "G4UserRunAction.hh"

/*
 * HPS
 */
#include "PluginManager.h"

namespace hpssim {

/*
 * @class UserRunAction
 * @brief Implementation of Geant4 user run action
 */
class UserRunAction : public G4UserRunAction {

    public:


        UserRunAction() {
        }

        virtual ~UserRunAction() {
        }

        void BeginOfRunAction(const G4Run* aRun) {

            // init LCIO persistence engine
            LcioPersistencyManager::getInstance()->Initialize();

            // init the primary generators
            PrimaryGeneratorAction::getPrimaryGeneratorAction()->initialize();

            // init sim plugins e.g. read parameter settings into variables, etc.
            PluginManager::getPluginManager()->initializePlugins();

            // activate plugin manager's begin run action
            PluginManager::getPluginManager()->beginRun(aRun);
        }


        void EndOfRunAction(const G4Run*) {
        }
    };

}

#endif
