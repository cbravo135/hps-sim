/**
 * @file TridentCnvPlugin.cc
 * @brief Class that generates trident events in SVT layers
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

// LDMX
#include "SimPlugin.h"
#include "SimPluginMessenger.h"
#include "UserTrackInformation.h"

namespace hpssim {

    /**
     * @class TridentCnvPlugin
     * @brief Dummy implementation of SimPlugin
     */
    class TridentCnvPlugin : public SimPlugin {

        public:

            TridentCnvPlugin() {
                std::cout << "TridentCnvPlugin: Hello!" << std::endl;
                messenger_ = new SimPluginMessenger(this);
            }

            virtual ~TridentCnvPlugin() {
                std::cout << "TridentCnvPlugin: Goodbye!" << std::endl;
                delete messenger_;
            }

            virtual std::string getName() {
                return "TridentCnvPlugin";
            }

            std::vector<PluginAction> getActions() {
                return {PluginAction::RUN, PluginAction::EVENT, PluginAction::STEPPING, PluginAction::TRACKING};
            }

            void beginRun(const G4Run* run) {
                std::cout << "TridentCnvPlugin: Begin run " << run->GetRunID() << std::endl;
            }

            void endRun(const G4Run* run) {
                std::cout << "TridentCnvPlugin: End run " << run->GetRunID() << std::endl;
            }

            void beginEvent(const G4Event* event) {
                std::cout << "TridentCnvPlugin: Begin event " << event->GetEventID() << std::endl;
            }

            void endEvent(const G4Event* event) {
                std::cout << "TridentCnvPlugin: End event " << event->GetEventID() << std::endl;
            }

            void stepping(const G4Step* step) {
                // Put stepping code here.
                auto track = step->GetTrack();
                if (UserTrackInformation::getUserTrackInformation(track)->getFlag("pair_cnv")) {
                    std::cout << "TridentCnvPlugin: Found pair conversion from track " << track->GetTrackID() << std::endl;
                }
            }

            void preTracking(const G4Track* aTrack) {
                // Put beginning of track processing here.
            }

            void postTracking(const G4Track* aTrack) {
                // Put end of track processing here.
            }

        private:

            G4UImessenger* messenger_{nullptr};
    };
}

SIM_PLUGIN(hpssim, TridentCnvPlugin)
