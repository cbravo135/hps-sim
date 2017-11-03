// LDMX
#include "SimPlugin.h"

#include "PrimaryGeneratorAction.h"
#include "SimPluginMessenger.h"
#include "UserTrackingAction.h"

#include "G4PhysicalVolumeStore.hh"
#include "G4VProcess.hh"

namespace hpssim {

    class PairCnvPlugin : public SimPlugin {

        public:

            PairCnvPlugin() {
                messenger_ = new SimPluginMessenger(this);
                mgr_ = UserTrackingAction::getUserTrackingAction()->getTrackingManager();

            }

            virtual ~PairCnvPlugin() {
                delete messenger_;
            }

            virtual std::string getName() {
                return "PairCnvPlugin";
            }

            std::vector<PluginAction> getActions() {
                return {PluginAction::EVENT, PluginAction::TRACKING, PluginAction::STEPPING, PluginAction::RUN};
            }

            void beginRun(const G4Run* run) {

                if (PrimaryGeneratorAction::getPrimaryGeneratorAction()->getGenerators().size() > 1) {
                    G4Exception("", "", FatalException, "This plugin cannot run with multiple event generators.");
                }

                auto pvStore = G4PhysicalVolumeStore::GetInstance();
                volumes_.clear();

                // FIXME: hard-coded volume names; need custom messenger to set vol names
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
                if (track->GetDynamicParticle()->GetPrimaryParticle() && pdef->GetPDGEncoding() == 22) {
                    auto secos = step->GetSecondaryInCurrentStep();
                    if (secos->size()) {
                        if (inVolumes(step->GetPreStepPoint()->GetTouchable())) {
                            for (auto seco : *secos) {
                                if (seco->GetCreatorProcess()) {
                                    auto proc = seco->GetCreatorProcess();
                                    auto pv = step->GetPreStepPoint()->GetPhysicalVolume();
                                    if (verbose_ > 1) {
                                    }
                                    if (proc->GetProcessName() == "conv") {
                                        if (verbose_ > 1) {
                                            std::cout << "PairCnvPlugin: seco created by proc '" << proc->GetProcessName() 
                                                    << "' in pv '" << pv->GetName() << "'" << std::endl;
                                        }
                                        cnvFlag_ = true;
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
                        std::cout << pv->GetName() << std::endl;
                        if(std::find(volumes_.begin(), volumes_.end(), pv) != volumes_.end()) {
                            std::cout << "PairCnvPlugin: found vol '" << pv->GetName() << "' in list." << std::endl;
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
                    G4Exception("", "", FatalException, "Volume not found in PV store.");
                }
            }

            void endEvent(const G4Event* event) {
                auto generators = PrimaryGeneratorAction::getPrimaryGeneratorAction()->getGenerators();
                std::cout << "PairCnvPlugin: Setting read flag to " << cnvFlag_ << std::endl;
                generators[0]->setReadFlag(cnvFlag_);
            }

            void beginEvent(const G4Event*) {
                cnvFlag_ = false;
            }

        private:

            /* List of physical volume names where conversions may occur. */
            std::vector<G4VPhysicalVolume*> volumes_;

            bool cnvFlag_{false};

            G4UImessenger* messenger_;

            G4TrackingManager* mgr_;
    };
}

SIM_PLUGIN(hpssim, PairCnvPlugin)
