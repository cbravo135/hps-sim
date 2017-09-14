#ifndef HPSSIM_USEREVENTACTION_H_
#define HPSSIM_USEREVENTACTION_H_

#include "G4UserEventAction.hh"

#include "lcdd/detectors/CurrentTrackState.hh"

namespace hpssim {

    class UserEventAction : public G4UserEventAction {

        public:

            void BeginOfEventAction(const G4Event* anEvent) {

                CurrentTrackState::setCurrentTrackID(-1);

                UserTrackingAction::getUserTrackingAction()->getTrackMap()->clear();
            }
    };
}




#endif /* INCLUDE_EVENTACTION_H_ */
