/**
 * @file BeamTrackSelectionPlugin.cc
 * @brief Class that defines beam track selection sim plugin
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#include "SimPlugin.h"
#include "SimPluginMessenger.h"
#include "UserTrackInformation.h"

#include "G4AntiNeutron.hh"
#include "G4AntiProton.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Neutron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"
#include "G4SystemOfUnits.hh"
#include "G4UImessenger.hh"
#include "G4VProcess.hh"

namespace hpssim {

/**
 * @class BeamTrackSelectionPlugin
 * @brief Plugin for filtering particles for saving beam backgrounds after target interaction
 */
class BeamTrackSelectionPlugin: public SimPlugin {

    public:

        BeamTrackSelectionPlugin() {
            messenger_ = new SimPluginMessenger(this);
        }

        virtual ~BeamTrackSelectionPlugin() {
            delete messenger_;
        }

        virtual std::string getName() {
            return "BeamTrackSelectionPlugin";
        }

        std::vector<PluginAction> getActions() {
            return {PluginAction::STEPPING, PluginAction::TRACKING, PluginAction::EVENT};
        }

        void initialize() {
            Parameters& params = getParameters();
            gammaThetaYCut_ = params.get("gammaThetaYCut", gammaThetaYCut_);
            electronThetaYCut_ = params.get("electronThetaYCut", electronThetaYCut_);
            electronEnergyCut_ = params.get("electronEnergyCut", electronEnergyCut_);
            electronEnergyThreshold_ = params.get("electronEnergyThreshold", electronEnergyThreshold_);

            // TODO: Compute energy cuts automatically from particle energy (beam E) if not set from parameters.
        }

        void beginEvent(const G4Event* event) {
            if (verbose_ > 1) {
                std::cout << "BeamTrackSelectionPlugin: Begin event " << event->GetEventID() << std::endl;
            }
            nKilled_ = 0;
            nTracks_ = 0;
            nPassed_ = 0;
        }

        void endEvent(const G4Event* event) {
            if (verbose_ > 1) {
                std::cout << "BeamTrackSelectionPlugin: Killed " << nKilled_ << " tracks of " << nTracks_
                        << " and passed " << nPassed_ << " in event " << event->GetEventID() << std::endl;
            }
        }

        void preTracking(const G4Track* aTrack) {
            if (verbose_ > 2) {
                std::cout << "BeamTrackSelectionPlugin: Track " << aTrack->GetTrackID()
                        << " with PID " << aTrack->GetParticleDefinition()->GetPDGEncoding()
                        << " and momentum " << aTrack->GetMomentum()
                        << " with parent " << aTrack->GetParentID()
                        << " and vertex " << aTrack->GetVertexPosition()
                        << std::endl;
            }
            ++nTracks_;
        }

        void stepping(const G4Step* step) {
            if (volumeName_.compare(step->GetTrack()->GetVolume()->GetName())) {
                return;
            }
            if (step->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) {
                if (!volumeName_.compare(step->GetPreStepPoint()->GetPhysicalVolume()->GetName())) {
                    if (verbose_ > 3) {
                        std::cout << "BeamTrackSelectionPlugin: Processing track " << step->GetTrack()->GetTrackID()
                                << " at " << step->GetPreStepPoint()->GetPosition() << " stepping from '"
                                << step->GetPreStepPoint()->GetPhysicalVolume()->GetName() << "' to '"
                                << step->GetPostStepPoint()->GetPhysicalVolume()->GetName() << std::endl;
                    }

                    G4Track* track = step->GetTrack();
                    if (!passes(track)) {
                        if (verbose_ > 2) {
                            std::cout << "BeamTrackSelectionPlugin: Track " << track->GetTrackID() << " with PID "
                                    << track->GetParticleDefinition()->GetPDGEncoding() << " and momentum "
                                    << track->GetMomentum() << " failed selection" << std::endl;
                        }

                        // Stop and kill the track; let secondaries propagate.
                        step->GetTrack()->SetTrackStatus(G4TrackStatus::fStopAndKill);

                        // Do not save this track in output particle coll.
                        UserTrackInformation::getUserTrackInformation(track)->setSaveFlag(false);

                        ++nKilled_;
                    } else {
                        if (verbose_ > 2) {
                            std::cout << "BeamTrackSelectionPlugin: Track " << track->GetTrackID() << " with PID "
                                    << track->GetParticleDefinition()->GetPDGEncoding() << " and momentum "
                                    << track->GetMomentum() << " passed selection" << std::endl;
                        }

                        // Save this track in output particle coll.
                        UserTrackInformation::getUserTrackInformation(track)->setSaveFlag(true);

                        ++nPassed_;
                    }
                }
            }
        }

    private:

        bool passes(G4Track* track) {
            auto p = track->GetMomentum();
            double thetaY = (p.y() / p.mag());
            if (verbose_ > 3) {
                std::cout << "BeamTrackSelectionPlugin: Track " << track->GetTrackID() << " with PID "
                        << track->GetParticleDefinition()->GetPDGEncoding() << " and momentum " << track->GetMomentum()
                        << " has thetaY " << thetaY << std::endl;
            }
            auto def = track->GetParticleDefinition();
            int pid = def->GetPDGEncoding();
            if (pid == G4Gamma::Definition()->GetPDGEncoding()) {
                // apply gamma theta Y cut
                if (abs(thetaY) < gammaThetaYCut_) {
                    return false;
                }
            } else if (pid == G4Electron::Definition()->GetPDGEncoding()
                    || G4Positron::Definition()->GetPDGEncoding()) {
                // apply electron energy cut
                if (track->GetTotalEnergy() < electronEnergyCut_) {
                    return false;
                }
                // apply electron theta Y cut with energy threshold
                if (track->GetTotalEnergy() > this->electronEnergyThreshold_ && abs(thetaY) < electronThetaYCut_) {
                    return false;
                }
            } else if (pid == G4Proton::Definition()->GetPDGEncoding()) {
                return false;
            } else if (pid == G4AntiProton::Definition()->GetPDGEncoding()) {
                return false;
            } else if (pid == G4Neutron::Definition()->GetPDGEncoding()) {
                return false;
            } else if (pid == G4AntiNeutron::Definition()->GetPDGEncoding()) {
                return false;
            }
            return true;
        }

    private:

        /** Theta Y cut in radians for gammas. */
        double gammaThetaYCut_{0.004};

        /** Theta Y cut in radians for electrons. */
        double electronThetaYCut_{0.004};

        /**
         * Electron energy cut, e.g. for killing delta rays.
         * Default = 1.056 GeV * 0.005
         */
        double electronEnergyCut_{5.28 * MeV};

        /**
         * Threshold for theta Y cut.
         * Default = 1.056 GeV * 0.6
         */
        double electronEnergyThreshold_{0.6336 * GeV};

        /** Name of target volume in geometry. */
        std::string volumeName_{"target_vol"};

        /** Number of tracks killed in event. */
        int nKilled_{0};

        /** Total number of tracks seen in event. */
        int nTracks_{0};

        /** Total number of tracks that passed selection. */
        int nPassed_{0};

        /** Plugin messenger for UI commands. */
        G4UImessenger* messenger_;
};
}

SIM_PLUGIN(hpssim, BeamTrackSelectionPlugin)
