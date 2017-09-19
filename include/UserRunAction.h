#ifndef HPSSIM_USERRUNACTION_H_
#define HPSSIM_USERRUNACTION_H_

#include "G4UserRunAction.hh"

#include "PluginManager.h"

namespace hpssim {

class UserRunAction : public G4UserRunAction {

    public:


        UserRunAction() {
        }

        virtual ~UserRunAction() {
        }

        void BeginOfRunAction(const G4Run* aRun) {
            LcioPersistencyManager::getInstance()->Initialize();

            PluginManager::getPluginManager()->beginRun(aRun);
        }


        void EndOfRunAction(const G4Run* aRun) {
        }
    };

}

#endif
