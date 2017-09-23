/**
 * @file BeamTrackSelectionPlugin.cc
 * @brief Class that defines a dummy simulation plugin
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#include "SimPlugin.h"
#include "SimPluginMessenger.h"

#include "G4AntiNeutron.hh"
#include "G4AntiProton.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Neutron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"
#include "G4UImessenger.hh"

namespace hpssim {

    /**
     * @class BeamTrackSelectionPlugin
     * @brief Dummy implementation of SimPlugin
     */
    class BeamTrackSelectionPlugin : public SimPlugin {

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

            void beginEvent(const G4Event* event) {
                if (verbose_ > 1) {
                    std::cout << "BeamTrackSelectionPlugin: Begin event " << event->GetEventID() << "." << std::endl;
                }
                nKilled_ = 0;
                nTracks_ = 0;
            }

            void endEvent(const G4Event* event) {
                if (verbose_ > 1) {
                    std::cout << "BeamTrackSelectionPlugin: End event " << event->GetEventID() << "." << std::endl;
                    std::cout << "BeamTrackSelectionPlugin: Killed " << nKilled_ << " tracks of " << nTracks_
                            << " in event " << event->GetEventID() << "." << std::endl;
                }
            }

            void preTracking(const G4Track*) {
                ++nTracks_;
            }

            void stepping(const G4Step* step) {
                if (volumeName_.compare(step->GetTrack()->GetVolume()->GetName())) {
                    return;
                }
                if (step->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) {
                    if (!volumeName_.compare(step->GetPreStepPoint()->GetPhysicalVolume()->GetName())) {
                        if (verbose_ > 3) {
                            std::cout << "BeamTrackSelectionPlugin: Step in track " << step->GetTrack()->GetTrackID()
                                        << " at " << step->GetPreStepPoint()->GetPosition() << " from '"
                                        << step->GetPreStepPoint()->GetPhysicalVolume()->GetName() << "' to '"
                                        << step->GetPostStepPoint()->GetPhysicalVolume()->GetName() << "'." << std::endl;
                        }

                        G4Track* track = step->GetTrack();
                        if (!passes(track)) {
                            if (verbose_ > 2) {
                                std::cout << "BeamTrackSelectionPlugin: Killing track " << track->GetTrackID()
                                            << " with PID " << track->GetParticleDefinition()->GetPDGEncoding()
                                            << " and momentum " << track->GetMomentum() << std::endl;
                            }
                            step->GetTrack()->SetTrackStatus(G4TrackStatus::fKillTrackAndSecondaries);
                            ++nKilled_;
                        } else {
                            if (verbose_ > 2) {
                                std::cout << "BeamTrackSelectionPlugin: Track " << track->GetTrackID() << " with PID "
                                        << track->GetParticleDefinition()->GetPDGEncoding() << " and momentum "
                                        << track->GetMomentum() << " passed selection." << std::endl;
                            }
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
                            << track->GetParticleDefinition()->GetPDGEncoding()
                            << " and momentum " << track->GetMomentum() << " has thetaY " << thetaY << "."
                            << std::endl;
                }
                auto def = track->GetParticleDefinition();
                int pid = def->GetPDGEncoding();
                if (pid == G4Gamma::Definition()->GetPDGEncoding()) {
                    // gamma cut
                    if (abs(thetaY) < gammaThetaYCut_) {
                        return false;
                    }
                } else if (pid == G4Electron::Definition()->GetPDGEncoding()
                        || G4Positron::Definition()->GetPDGEncoding()) {
                    // electron cut
                    if (abs(thetaY) < electronThetaYCut_) {
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
            double electronThetaYCut_{0.005};

            /** Name of target volume in geometry. */
            std::string volumeName_{"target_vol"};

            /** Number of tracks killed in event. */
            int nKilled_{0};

            /** Total number of tracks seen in event. */
            int nTracks_{0};

            /** Plugin messenger for UI commands. */
            G4UImessenger* messenger_;
    };
}

SIM_PLUGIN(hpssim, BeamTrackSelectionPlugin)
