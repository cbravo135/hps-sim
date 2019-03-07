/**
 * @file SimPlugin.h
 * @brief Class defining an interface for a user simulation plugin
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef HPSSIM_SIMPLUGIN_H_
#define HPSSIM_SIMPLUGIN_H_

#include "G4Run.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ClassificationOfNewTrack.hh"

#include "Parameters.h"

#include <vector>

namespace hpssim {

/**
 * @class SimPlugin
 * @brief User simulation plugin
 *
 * @note
 * This class defines a plugin interface to the Geant4 simulation engine
 * which is activated in the "user action" hooks.
 */
class SimPlugin {

    public:

        /**
         * Defines callbacks in the Geant4 engine for which
         * a plugin should be activated.
         */
        enum PluginAction {
            RUN = 1,
            EVENT,
            STACKING,
            STEPPING,
            TRACKING,
            PRIMARY
        };

        /**
         * Class destructor.
         */
        virtual ~SimPlugin() {
        }

        /**
         * Get the name of the plugin.
         * The user must override this function.
         * @return The name of the plugin.
         */
        virtual std::string getName() = 0;


        /**
         * Get a list of actions implemented by this plugin.
         * @return The list of Geant4 action hooks implemented by this plugin.
         */
        virtual std::vector<PluginAction> getActions() = 0;

        /**
         * Set the verbose level of the plugin (1-4).
         * @param verbose The verbose level of the plugin.
         */
        void setVerboseLevel(int verbose) {
            verbose_ = verbose;
            if (verbose_ < 1) {
                verbose = 1;
            } else if (verbose_ > 4) {
                verbose = 4;
            }
        }

        /**
         * Get the current verbose level.
         * @return The current verbose level.
         */
        int getVerboseLevel() {
            return verbose_;
        }

        /**
         * Perform any custom initialization of this plugin.
         */
        virtual void initialize() {
        }

        /**
         * Get the list of double parameters.
         */
        Parameters& getParameters() {
            return params_;
        }

        /**
         * Begin of run action.
         */
        virtual void beginRun(const G4Run*) {
        }

        /**
         * End of run action.
         */
        virtual void endRun(const G4Run*) {
        }

        /**
         * Stepping action.
         */
        virtual void stepping(const G4Step*) {
        }

        /**
         * Pre-tracking action.
         */
        virtual void preTracking(const G4Track*) {
        }

        /**
         * Post-tracking action.
         */
        virtual void postTracking(const G4Track*) {
        }

        /**
         * Begin of event action.
         */
        virtual void beginEvent(const G4Event*) {
        }

        /**
         * End of event action.
         */
        virtual void endEvent(const G4Event*) {
        }

        /**
         * Generate primary action.
         */
        virtual void generatePrimary(G4Event*) {
        }

        /**
         * Classify a new track.
         * @param currentTrackClass The current track classification.
         * @return The current track classification returned by default.
         */
        virtual G4ClassificationOfNewTrack stackingClassifyNewTrack(const G4Track*, const G4ClassificationOfNewTrack& currentTrackClass) {
            return currentTrackClass;
        }

        /**
         * New stacking stage action.
         */
        virtual void stackingNewStage() {
        }

        /**
         * New event stacking action.
         */
        virtual void stackingPrepareNewEvent() {
        }

    protected:

        /** Protected access to verbose level for convenience of sub-classes. */
        int verbose_{1};

        /** The double parameters for this plugin. */
        Parameters params_;
};
}

/*
* Macro for defining the create and destroy methods for a sim plugin.
*/
#define SIM_PLUGIN(NS, NAME) \
extern "C" NS::NAME* create ## NAME() { \
return new NS::NAME; \
} \
extern "C" void destroy ## NAME(NS::NAME* object) { \
delete object; \
}

#endif
