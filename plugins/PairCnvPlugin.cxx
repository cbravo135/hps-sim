#include "SimPlugin.h"

#include "PrimaryGeneratorAction.h"
#include "SimPluginMessenger.h"
#include "UserTrackingAction.h"

#include "G4PhysicalVolumeStore.hh"
#include "G4VProcess.hh"

namespace hpssim {

/**
 * @class PairCnvPlugin
 * @brief Plugin to select events where pair conversions occur in specified detector volumes
 * @note Generator events are reread until a conversion occurs or max events is reached.
 */
class PairCnvPlugin : public SimPlugin {

    public:

        PairCnvPlugin() {
            messenger_ = new SimPluginMessenger(this);
            mgr_ = UserTrackingAction::getUserTrackingAction()->getTrackingManager();
        }

        virtual ~PairCnvPlugin() {
            delete messenger_;
        }

        std::string getName() {
            return "PairCnvPlugin";
        }

        std::vector<PluginAction> getActions() {
            return {PluginAction::EVENT, PluginAction::TRACKING, PluginAction::STEPPING, PluginAction::RUN};
        }

        G4ClassificationOfNewTrack stackingClassifyNewTrack(const G4Track* track,
                const G4ClassificationOfNewTrack& currentTrackClass) {
            if (!(track->GetDynamicParticle()->GetPrimaryParticle()
                    && track->GetParticleDefinition()->GetPDGEncoding() == 22)) {
                return fWaiting;
            }
            return currentTrackClass;
        }

        void beginRun(const G4Run* run) {

            if (PrimaryGeneratorAction::getPrimaryGeneratorAction()->getGenerators().size() > 1) {
                G4Exception("", "", FatalException, "This plugin cannot run when using multiple event generators.");
            }

            volumes_.clear();

            // FIXME: hard-coded volume names; need a custom messenger command to set the volume names
            addVolume("target_vol");
            addVolume("module_L1b_halfmodule_stereo_sensor_volume");
            addVolume("module_L1b_halfmodule_axial_sensor_volume");
            addVolume("module_L1t_halfmodule_stereo_sensor_volume");
            addVolume("module_L1t_halfmodule_axial_sensor_volume");
            addVolume("module_L2b_halfmodule_stereo_sensor_volume");
            addVolume("module_L2b_halfmodule_axial_sensor_volume");
            addVolume("module_L2t_halfmodule_stereo_sensor_volume");
            addVolume("module_L2t_halfmodule_axial_sensor_volume");
        }

        void postTracking(const G4Track* aTrack) {
            if (aTrack->GetDynamicParticle()->GetPrimaryParticle()) {
                if (aTrack->GetDynamicParticle()->GetPDGcode() == 22) {
                    if (!cnvFlag_) {
                        auto anEvent = const_cast<G4Event*>(G4RunManager::GetRunManager()->GetCurrentEvent());
                        const_cast<G4Event*>(anEvent)->SetEventAborted();
                    }
                }
            }
        }

        void stepping(const G4Step* step) {
            auto track = step->GetTrack();
            auto pdef = track->GetParticleDefinition();
            if (!cnvFlag_ && track->GetDynamicParticle()->GetPrimaryParticle() && pdef->GetPDGEncoding() == 22) {
                auto secos = step->GetSecondaryInCurrentStep();
                if (secos->size()) {
                    if (inVolumes(step->GetPreStepPoint()->GetTouchable())) {
                        for (auto seco : *secos) {
                            if (seco->GetCreatorProcess()) {
                                auto proc = seco->GetCreatorProcess();
                                auto pv = step->GetPreStepPoint()->GetPhysicalVolume();
                                if (proc->GetProcessName() == "conv") {
                                    if (verbose_ > 1) {
                                        std::cout << "PairCnvPlugin: Secondary created by process '" << proc->GetProcessName()
                                                << "' in volume '" << pv->GetName() << "'" << std::endl;
                                    }
                                    cnvFlag_ = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        bool inVolumes(const G4VTouchable* touchable) {
            int depth = touchable->GetHistoryDepth();
            for (int iDepth = depth; iDepth >= 0; iDepth--) {
                G4VPhysicalVolume* pv = touchable->GetVolume(iDepth);
                if (pv) {
                    if(std::find(volumes_.begin(), volumes_.end(), pv) != volumes_.end()) {
                        std::cout << "PairCnvPlugin: Found volume '" << pv->GetName() << "' in list." << std::endl;
                        return true;
                    }
                }
            }
            return false;
        }

        void addVolume(std::string volName) {
            auto vol = G4PhysicalVolumeStore::GetInstance()->GetVolume(volName);
            if (vol) {
                volumes_.push_back(G4PhysicalVolumeStore::GetInstance()->GetVolume(volName));
            } else {
                //G4Exception("", "", FatalException, "Volume not found in PV store.");
                std::cerr << "PairCnvPlugin: WARNING - Volume '" << volName << "' was not found in PV store." << std::endl;
            }
        }

        void endEvent(const G4Event* event) {
            auto generators = PrimaryGeneratorAction::getPrimaryGeneratorAction()->getGenerators();
            std::cout << "PairCnvPlugin: Setting read flag to " << cnvFlag_ << std::endl;
            generators[0]->setReadFlag(cnvFlag_);
            if (cnvFlag_) {
                // Conversion occurred so propagate event weight.
                auto anEvent = const_cast<G4Event*>(G4RunManager::GetRunManager()->GetCurrentEvent());
                float weight = 1.0 / nEventsRead_;
                anEvent->GetPrimaryVertex()->SetWeight(weight);
                if (verbose_ > 1) {
                    std::cout << "PairCnvPlugin: Set weight to " << weight << " from " << nEventsRead_ << " events generated." << std::endl;
                }
            } else {
                // Reached maximum number of events so reset plugin state and read next generator event.
                nEventsRead_ += 1;
                if (nEventsRead_ > maxEvents_) {
                    std::cout << "PairCnvPlugin: Reread " << nEventsRead_ << " generator events without pair cnv; reading next input event." << std::endl;
                    nEventsRead_ = 1;
                    generators[0]->setReadFlag(true);
                }
            }
        }

        void beginEvent(const G4Event*) {
            if (cnvFlag_) {
                nEventsRead_ = 1;
            }
            cnvFlag_ = false;
        }

    private:

        /* List of physical volumes to check for conversions. */
        std::vector<G4VPhysicalVolume*> volumes_;

        /* Flag indicating if conversion occurred; reset for every event. */
        bool cnvFlag_{false};

        G4UImessenger* messenger_;

        G4TrackingManager* mgr_;

        int nEventsRead_{1};

        int maxEvents_{1000};
};
}

SIM_PLUGIN(hpssim, PairCnvPlugin)
