#ifndef HPSSIM_EVENTACTION_H_
#define HPSSIM_EVENTACTION_H_

#include "G4UserEventAction.hh"

#include "lcdd/detectors/CurrentTrackState.hh"

namespace hpssim {

    class EventAction : G4UserEventAction {

        public:

            void BeginOfEventAction(const G4Event* anEvent) {
                CurrentTrackState::setCurrentTrackID(-1);
            }
    };
}




#endif /* INCLUDE_EVENTACTION_H_ */
