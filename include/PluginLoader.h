/**
 * @file PluginLoader.h
 * @brief Class which loads user simulation plugins from external shared libraries
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef HPSSIM_PLUGINLOADER_H_
#define HPSSIM_PLUGINLOADER_H_

// LDMX
#include "SimPlugin.h"

namespace hpssim {

    /**
     * @class PluginLoader
     * @brief Loads user sim plugin classes from external shared libraries
     */
    class PluginLoader {

        public:

            /**
             * Create a user sim plugin with the given name from the supplied
             * library name.
             * @param pluginName The name of the plugin.
             * @param libName The name of the shared library.
             */
            SimPlugin* create(std::string pluginName, std::string libName);

            /**
             * Destroy an existing user sim plugin.
             * @param plugin The sim plugin to destroy.
             */
            void destroy(SimPlugin* plugin);

        private:

            /**
             * Get a handle for a plugin.
             * @param pluginName The name of the plugin.
             */
            void* getHandle(std::string pluginName);

        private:

            /**
             * Map of plugins to their handles.
             */
            std::map<SimPlugin*, void*> pluginHandles_;
    };

}

#endif
