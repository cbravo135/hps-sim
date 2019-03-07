#ifndef HPSSIM_STEPPINGACTION_H_
#define HPSSIM_STEPPINGACTION_H_

#include "G4UserSteppingAction.hh"

#include "PluginManager.h"

namespace hpssim {

/**
 * @class SteppingAction
 * @brief Implementing of Geant4 stepping action
 */
class SteppingAction : public G4UserSteppingAction {

    public:

        virtual ~SteppingAction() {
        }

        void UserSteppingAction(const G4Step* aStep) {
            PluginManager::getPluginManager()->stepping(aStep);
        }
};

}

#endif
