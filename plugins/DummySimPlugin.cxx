/**
 * @file DummySimPlugin.cc
 * @brief Class that defines a dummy simulation plugin
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

// LDMX
#include "SimPlugin.h"

namespace hpssim {

    /**
     * @class DummySimPlugin
     * @brief Dummy implementation of SimPlugin
     */
    class DummySimPlugin : public SimPlugin {

        public:

            DummySimPlugin() {
                std::cout << "DummySimPlugin: Hello!" << std::endl;
            }

            virtual ~DummySimPlugin() {
                std::cout << "DummySimPlugin: Goodbye!" << std::endl;
            }

            virtual std::string getName() {
                return "DummySimPlugin";
            }

            std::vector<PluginAction> getActions() {
                return {PluginAction::RUN, PluginAction::EVENT};
            }

            void beginRun(const G4Run* run) {
                std::cout << "DummySimPlugin: Begin run " << run->GetRunID() << std::endl;
            }

            void endRun(const G4Run* run) {
                std::cout << "DummySimPlugin: End run " << run->GetRunID() << std::endl;
            }

            void beginEvent(const G4Event* event) {
                std::cout << "DummySimPlugin: Begin event " << event->GetEventID() << std::endl;
            }

            void endEvent(const G4Event* event) {
                std::cout << "DummySimPlugin: End event " << event->GetEventID() << std::endl;
            }
    };
}

SIM_PLUGIN(hpssim, DummySimPlugin)
