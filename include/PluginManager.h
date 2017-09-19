/**
 * @file PluginManager.h
 * @brief Class for managing the loading, activation and destruction of UserSimPlugin objects
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef HPSSIM_PLUGINMANAGER_H_
#define HPSSIM_PLUGINMANAGER_H_

// LDMX
#include "PluginLoader.h"
#include "SimPlugin.h"

// STL
#include <algorithm>
#include <ostream>

// Geant4
#include "G4ClassificationOfNewTrack.hh"

namespace hpssim {

class PluginMessenger;

/**
 * @class PluginManager
 * @brief Manages user sim pluginsc
 *
 * @note
 * This class loads UserSimPlugin objects from dynamic libraries using a PluginLoader.
 * It is also responsible for activating the user action hooks for all registered plugins.
 * Only one instance of a given plugin can be loaded at a time.
 *
 * @see SimPlugin
 * @see PluginLoader
 */
class PluginManager {

    public:

        /**
         * Vector of plugins.
         */
        typedef std::vector<SimPlugin*> PluginVec;

        typedef std::map<SimPlugin::PluginAction, PluginVec> PluginActionMap;

        static PluginManager* getPluginManager() {
            static PluginManager theInstance;
            return &theInstance;
        }

        /**
         * Class constructor.
         */
        PluginManager();

        /**
         * Class destructor.
         * This will destroy all the currently registered plugins.
         */
        virtual ~PluginManager();

        /**
         * Activate the begin run hook for registered plugins.
         * @param aRun The Geant4 run that is beginning.
         */
        void beginRun(const G4Run* aRun);

        /**
         * Activate the end run hook of registered plugins.
         * @param aRun The Geant4 run that is ending.
         */
        void endRun(const G4Run* aRun);

        /**
         * Activate the stepping hook of registered plugins.
         * @param aStep The Geant4 step.
         */
        void stepping(const G4Step* aStep);

        /**
         * Activate the pre-tracking hook of registered plugins.
         * @param aTrack The Geant4 track.
         */
        void preTracking(const G4Track* aTrack);

        /**
         * Activate the post-tracking hook of registered plugicreatens.
         * @param aTrack The Geant4 track.
         */
        void postTracking(const G4Track* aTrack);

        /**
         * Activate the begin event hook of registered plugins.
         * @param anEvent The Geant4 event.
         */
        void beginEvent(const G4Event* anEvent);

        /**
         * Activate the end event hook of registered plugins.
         * @param anEvent The Geant4 event.
         */
        void endEvent(const G4Event* anEvent);

        /**
         * Activate the generate primary hook of registered plugins.
         * @param anEvent The Geant4 event.
         */
        void generatePrimary(G4Event* anEvent);

        /**
         *
         * Activate the track classification hook of registered plugins.
         * @param aTrack The Geant4 track.
         *
         * @brief Return a track classification from the user plugins that have stacking actions.
         *
         * @note
         * The current classification will only be updated if a plugin actually provides a different classification
         * than the current one.
         */
        G4ClassificationOfNewTrack stackingClassifyNewTrack(const G4Track* aTrack);

        /**
         * Activate the stacking new stage hook of registered plugins.
         */
        void stackingNewStage();

        /**
         * Activate the prepare new event stacking hook of registered plugins.
         */
        void stackingPrepareNewEvent();

        /**
         * Find a plugin by name.
         * @param pluginName The name of the plugin.
         * @return The plugin with the name or <i>nullptr</i> if does not exist.
         */
        SimPlugin* findPlugin(const std::string& pluginName);

        /**
         * Create a new plugin by name from the given library.
         * @param pluginName The name of the plugin.
         * @param libName The name of the shared library.
         */
        void create(const std::string& pluginName, const std::string& libName);

        /**
         * Destroy a plugin by name.
         * @param pluginName The name of the plugin.
         */
        void destroy(const std::string& pluginName);

        /**
         * Print out information about registered plugins.
         * @param os The output stream.
         * @return The same output stream.
         */
        std::ostream& print(std::ostream& os);

    private:

        /**
         * Destroy a plugin.
         * @param plugin Pointer to plugin that should be destroyed.
         */
        void destroy(SimPlugin* plugin);

        /**
         * Register a plugin.
         * @param plugin Pointer to plugin that should be registered.
         */
        void registerPlugin(SimPlugin* plugin);

        /**
         * De-register a plugin.
         * @param plugin Pointer to plugin that should be de-registered.
         */
        void deregisterPlugin(SimPlugin* plugin);

        /**
         * Destroy all registered plugins.
         */
        void destroyPlugins();

    private:

        /**
         * The plugin loader for loading plugins from shared libraries.
         */
        PluginLoader pluginLoader_;

        PluginMessenger* messenger_;

        /**
         * The list of registered plugins.
         */
        PluginVec plugins_;

        PluginActionMap actions_;
};

}

#endif
