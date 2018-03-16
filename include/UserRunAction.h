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

        UserRunAction();

        virtual ~UserRunAction();

        void BeginOfRunAction(const G4Run* aRun);

        void EndOfRunAction(const G4Run*);
    };

}

#endif
