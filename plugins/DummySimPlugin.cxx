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

            bool hasEventAction() {
                return true;
            }

            bool hasRunAction() {
                return true;
            }

            /*
            bool hasSteppingAction() {
                return true;
            }

            bool hasTrackingAction() {
                return true;
            }

            bool hasStackingAction() {
                return true;
            }

            bool hasPrimaryGeneratorAction() {
                return true;
            }
            */

            void beginRun(const G4Run* run) {
                std::cout << "DummySimPlugin: Begin run " << run->GetRunID() << std::endl;
            }

            void endRun(const G4Run* run) {
                std::cout << "DummySimPlugin: End run " << run->GetRunID() << std::endl;
            }

            /*
            void stepping(const G4Step* step) {
                std::cout << "DummySimPlugin::stepping - pre-point: " << step->GetPreStepPoint()->GetPosition() << ", post-point: ";
                if (step->GetPostStepPoint()) {
                    std::cout << step->GetPostStepPoint()->GetPosition();
                } else {
                    std::cout << "NONE";
                }
                std::cout << std::endl;
            }

            void preTracking(const G4Track* track) {
                std::cout << "DummySimPlugin::preTracking - track ID " << track->GetTrackID() << std::endl;
            }

            void postTracking(const G4Track* track) {
                std::cout << "DummySimPlugin::postTracking - track ID " << track->GetTrackID() << std::endl;
            }
            */

            void beginEvent(const G4Event* event) {
                std::cout << "DummySimPlugin: Begin event " << event->GetEventID() << std::endl;
            }

            void endEvent(const G4Event* event) {
                std::cout << "DummySimPlugin: End event " << event->GetEventID() << std::endl;
            }

            /*
            G4ClassificationOfNewTrack stackingClassifyNewTrack(const G4Track* aTrack, const G4ClassificationOfNewTrack& currentTrackClass) {
                std::cout << "DummySimPlugin::stackingClassifyNewTrack - track ID " << aTrack->GetTrackID() << " classified as " << currentTrackClass << std::endl;
                return currentTrackClass;
            }

            void stackingNewStage() {
                std::cout << "DummySimPlugin::stackingNewStage" << std::endl;
            }

            void stackingPrepareNewEvent() {
                std::cout << "DummySimPlugin::stackingPrepareNewEvent" << std::endl;
            }

            void generatePrimary(G4Event* event) {
                std::cout << "DummySimPlugin::generatorPrimary - event " << event->GetEventID() << std::endl;
            }
            */
    };
}

SIM_PLUGIN(hpssim, DummySimPlugin)
