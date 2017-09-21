#ifndef HPSSIM_USERTRACKINGACTION_H_
#define HPSSIM_USERTRACKINGACTION_H_ 1

#include "G4UserTrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4RunManager.hh"

#include "lcdd/core/UserRegionInformation.hh"
#include "lcdd/detectors/CurrentTrackState.hh"

#include "PluginManager.h"
#include "TrackMap.h"
#include "UserTrackInformation.h"

namespace hpssim {

class UserTrackingAction : public G4UserTrackingAction {

    public:

        UserTrackingAction() {
        }

        virtual ~UserTrackingAction() {
        }

        void PreUserTrackingAction(const G4Track* aTrack) {
            std::cout << "UserTrackingAction: pre tracking - " << aTrack->GetTrackID() << std::endl;

            int trackID = aTrack->GetTrackID();

            // This is set for LCDD sensitive detectors.
            // It is a weird way to do this but we do not want to fork LCDD right now!
            CurrentTrackState::setCurrentTrackID(trackID);

            if (trackMap_.contains(trackID)) {
                if (trackMap_.hasTrajectory(trackID)) {
                    // This makes sure the tracking manager does not delete the trajectory.
                    fpTrackingManager->SetStoreTrajectory(true);
                }
            } else {
                // Process a new track.
                processTrack(aTrack);
            }

            PluginManager::getPluginManager()->preTracking(aTrack);
        }

        void PostUserTrackingAction(const G4Track* aTrack) {
            std::cout << "UserTrackingAction: post tracking - " << aTrack->GetTrackID() << std::endl;

            // Save extra trajectories on tracks that were flagged for saving during event processing.
            if (dynamic_cast<UserTrackInformation*>(aTrack->GetUserInformation())->getSaveFlag()) {
                if (!trackMap_.hasTrajectory(aTrack->GetTrackID())) {
                    storeTrajectory(aTrack);
                }
            }

            // Set end point momentum on the trajectory.
            if (fpTrackingManager->GetStoreTrajectory()) {
                auto traj = dynamic_cast<Trajectory*>(fpTrackingManager->GimmeTrajectory());
                if (traj) {
                    if (aTrack->GetTrackStatus() == G4TrackStatus::fStopAndKill) {
                        traj->setEndPointMomentum(aTrack);
                    }
                }
            }

            PluginManager::getPluginManager()->postTracking(aTrack);
        }

        void storeTrajectory(const G4Track* aTrack) {

            std::cout << "UserTrackingAction: creating new traj for " << aTrack->GetTrackID() << std::endl;

            // Create a new trajectory for this track.
            fpTrackingManager->SetStoreTrajectory(true);
            Trajectory* traj = new Trajectory(aTrack);
            fpTrackingManager->SetTrajectory(traj);

            // Update the gen status from the primary particle.
            /*
            if (aTrack->GetDynamicParticle()->GetPrimaryParticle() != NULL) {
                G4VUserPrimaryParticleInformation* primaryInfo = aTrack->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation();
                if (primaryInfo != NULL) {
                    traj->setGenStatus(((UserPrimaryParticleInformation*) primaryInfo)->getHepEvtStatus());
                }
            }
            */

            // Map track ID to trajectory.
            trackMap_.addTrajectory(traj);
        }

        void processTrack(const G4Track* aTrack) {

            // Set user track info on new track.
            if (!aTrack->GetUserInformation()) {
                auto trackInfo = new UserTrackInformation;
                //std::cout << "UserTrackingAction: creating user info for track " << aTrack->GetTrackID() << std::endl;
                trackInfo->setInitialMomentum(aTrack->GetMomentum());
                const_cast<G4Track*>(aTrack)->SetUserInformation(trackInfo);
            }

            // Check if trajectory storage should be turned on or off from the region info.
            UserRegionInformation* regionInfo = (UserRegionInformation*) aTrack->GetLogicalVolumeAtVertex()->GetRegion()->GetUserInformation();

            // Check if trajectory storage should be turned on or off from the gen status info
            /*
            int curGenStatus = -1;
            if (aTrack->GetDynamicParticle()->GetPrimaryParticle() != NULL){
                G4VUserPrimaryParticleInformation* primaryInfo = aTrack->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation();
                curGenStatus = ((UserPrimaryParticleInformation*) primaryInfo)->getHepEvtStatus();
            }
            */

            // Always save a particle if it has gen status == 1
            /*
            if (curGenStatus == 1){
                storeTrajectory(aTrack);
            } else
            */
            if (regionInfo && !regionInfo->getStoreSecondaries()) {
                // Turn off trajectory storage for this track from region flag.
                std::cout << "store trajectory OFF" << std::endl;
                fpTrackingManager->SetStoreTrajectory(false);
            } else {
                std::cout << "store trajectory ON" << std::endl;
                // Store a new trajectory for this track.
                storeTrajectory(aTrack);
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
