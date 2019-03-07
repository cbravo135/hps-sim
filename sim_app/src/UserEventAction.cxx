#include "UserEventAction.h"

#include "PluginManager.h"
#include "PrimaryGeneratorAction.h"
#include "UserTrackingAction.h"

namespace hpssim {

void UserEventAction::BeginOfEventAction(const G4Event* anEvent) {

    // Set for LCDD detectors.
    CurrentTrackState::setCurrentTrackID(-1);

    // Clear the global track map.
    UserTrackingAction::getUserTrackingAction()->getTrackMap()->clear();

    // Activate sim plugins.
    PluginManager::getPluginManager()->beginEvent(anEvent);
}

void UserEventAction::EndOfEventAction(const G4Event* anEvent) {

    // Activate sim plugins.
    PluginManager::getPluginManager()->endEvent(anEvent);

    // Cleanup gen event data if necessary.
    PrimaryGeneratorAction::getPrimaryGeneratorAction()->endEvent(anEvent);

}

}
