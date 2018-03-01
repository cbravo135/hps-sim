#ifndef HPSSIM_USERSTACKINGACTION_H_
#define HPSSIM_USERSTACKINGACTION_H_

#include "G4UserStackingAction.hh"

namespace hpssim {

class UserStackingAction : public G4UserStackingAction {

    public:

        G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track *aTrack) {
            return PluginManager::getPluginManager()->stackingClassifyNewTrack(aTrack);
        }

        void NewStage() {
            PluginManager::getPluginManager()->stackingNewStage();
        }

        void PrepareNewEvent() {
            PluginManager::getPluginManager()->stackingPrepareNewEvent();
        }
};


}

#endif
