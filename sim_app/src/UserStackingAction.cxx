#include "UserStackingAction.h"

#include "PluginManager.h"

namespace hpssim {

G4ClassificationOfNewTrack UserStackingAction::ClassifyNewTrack(const G4Track *aTrack) {
    return PluginManager::getPluginManager()->stackingClassifyNewTrack(aTrack);
}

void UserStackingAction::NewStage() {
    PluginManager::getPluginManager()->stackingNewStage();
}

void UserStackingAction::PrepareNewEvent() {
    PluginManager::getPluginManager()->stackingPrepareNewEvent();
}

} // namespace
