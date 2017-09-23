#ifndef HPSSIM_USEREVENTACTION_H_
#define HPSSIM_USEREVENTACTION_H_

#include "G4UserEventAction.hh"

#include "lcdd/detectors/CurrentTrackState.hh"

#include "PluginManager.h"

namespace hpssim {

class UserEventAction : public G4UserEventAction {

    public:

        void BeginOfEventAction(const G4Event* anEvent) {

            // Set for LCDD detectors.
            CurrentTrackState::setCurrentTrackID(-1);

            // Clear the global track map.
            UserTrackingAction::getUserTrackingAction()->getTrackMap()->clear();

            // Activate sim plugins.
            PluginManager::getPluginManager()->beginEvent(anEvent);
        }

        void EndOfEventAction(const G4Event* anEvent) {
            // Activate sim plugins.
            PluginManager::getPluginManager()->endEvent(anEvent);
        }
};
}




#endif /* INCLUDE_EVENTACTION_H_ */
