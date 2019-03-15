#ifndef HPSSIM_USEREVENTACTION_H_
#define HPSSIM_USEREVENTACTION_H_

#include "G4UserEventAction.hh"

#include "lcdd/detectors/CurrentTrackState.hh"

#include "PluginManager.h"

namespace hpssim {

/**
 * @class UserEventAction
 * @brief Implementation of Geant4 user event action
 */
class UserEventAction : public G4UserEventAction {

    public:

        void BeginOfEventAction(const G4Event* anEvent);

        void EndOfEventAction(const G4Event* anEvent);
};
}




#endif /* INCLUDE_EVENTACTION_H_ */
