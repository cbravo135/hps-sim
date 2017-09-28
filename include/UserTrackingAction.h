#ifndef HPSSIM_USERTRACKINGACTION_H_
#define HPSSIM_USERTRACKINGACTION_H_ 1

#include "G4UserTrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4RunManager.hh"

#include "lcdd/core/UserRegionInformation.hh"
#include "lcdd/detectors/CurrentTrackState.hh"

#include "PluginManager.h"
#include "TrackMap.h"
#include "UserPrimaryParticleInformation.h"
#include "UserTrackInformation.h"

namespace hpssim {

class UserTrackingAction : public G4UserTrackingAction {

    public:

        UserTrackingAction() {
        }

        virtual ~UserTrackingAction() {
        }

        void PreUserTrackingAction(const G4Track* aTrack) {
            //std::cout << "UserTrackingAction: pre tracking - " << aTrack->GetTrackID() << std::endl;

            int trackID = aTrack->GetTrackID();

            // This is set for LCDD sensitive detectors, which is strange but we don't want to change it right now!
            CurrentTrackState::setCurrentTrackID(trackID);

            if (trackMap_.contains(trackID)) {
                if (trackMap_.hasTrajectory(trackID)) {
                    // This makes sure the tracking manager does not delete the trajectory if it already exists.
                    fpTrackingManager->SetStoreTrajectory(true);
                }
            } else {
                // Process a new track.
                processTrack(aTrack);
            }

            PluginManager::getPluginManager()->preTracking(aTrack);
        }

        void PostUserTrackingAction(const G4Track* aTrack) {
            //std::cout << "UserTrackingAction: post tracking - " << aTrack->GetTrackID() << std::endl;

            // Save extra trajectories on tracks that were flagged for saving during event processing.
            if (dynamic_cast<UserTrackInformation*>(aTrack->GetUserInformation())->getSaveFlag()) {
                if (!trackMap_.hasTrajectory(aTrack->GetTrackID())) {
                    storeTrajectory(aTrack);
                }
            }

            // Set end point momentum on the trajectory.
            if (fpTrackingManager->GetStoreTrajectory()) {

                auto traj = dynamic_cast<Trajectory*>(fpTrackingManager->GimmeTrajectory());

                // Set end point momentum if track is killed.
                if (traj) {

                    // Set end point momentum from last point if track is being killed.
                    if (aTrack->GetTrackStatus() == G4TrackStatus::fStopAndKill) {
                        traj->setEndPointMomentum(aTrack);
                    }

                    // Pass save flag from track info to trajectory.
                    traj->setSaveFlag(UserTrackInformation::getUserTrackInformation(aTrack)->getSaveFlag());
                }
            }

            PluginManager::getPluginManager()->postTracking(aTrack);
        }

        void storeTrajectory(const G4Track* aTrack) {

            //std::cout << "UserTrackingAction: creating new traj for " << aTrack->GetTrackID() << std::endl;

            // Create a new trajectory for this track.
            fpTrackingManager->SetStoreTrajectory(true);
            Trajectory* traj = new Trajectory(aTrack);
            fpTrackingManager->SetTrajectory(traj);

            // Map track ID to trajectory.
            trackMap_.addTrajectory(traj);
        }

        void processTrack(const G4Track* aTrack) {

            // Setup the track info object.
            if (!aTrack->GetUserInformation()) {
                auto trackInfo = new UserTrackInformation;
                trackInfo->setInitialMomentum(aTrack->GetMomentum());
                const_cast<G4Track*>(aTrack)->SetUserInformation(trackInfo);
            }

            // Check if trajectory storage should be turned on.
            UserRegionInformation* regionInfo =
                    (UserRegionInformation*) aTrack->GetLogicalVolumeAtVertex()->GetRegion()->GetUserInformation();
            bool isPrimary = (aTrack->GetDynamicParticle()->GetPrimaryParticle() != nullptr);
            if ((regionInfo && regionInfo->getStoreSecondaries()) || isPrimary) {
                // Region is flagged for storing secondaries (e.g. "tracking region") or the particle is a primary.
                storeTrajectory(aTrack);
            } else {
                // Trajectory storage is turned off!
                fpTrackingManager->SetStoreTrajectory(false);
            }

            // Save the association between track ID and its parent ID for all tracks in the event.
            trackMap_.addSecondary(aTrack->GetTrackID(), aTrack->GetParentID());
        }

        TrackMap* getTrackMap() {
            return &trackMap_;
        }

        static UserTrackingAction* getUserTrackingAction() {
            return static_cast<UserTrackingAction*>(const_cast<G4UserTrackingAction*>(G4RunManager::GetRunManager()->GetUserTrackingAction()));
        }

    private:

        TrackMap trackMap_;
};

}

#endif
