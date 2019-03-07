/**
 * @file SimPluginMessenger.h
 * @brief Class defining a macro messenger for a SimPlugin
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef HPSSIM_SIMPLUGINMESSENGER_H_
#define HPSSIM_SIMPLUGINMESSENGER_H_

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"

namespace hpssim {

class SimPlugin;

/**
 * @class SimPluginMessenger
 * @brief Messenger for sending macro commands to a SimPlugin
 *
 * @note
 * By default, this class creates a directory for the plugin and provides
 * a command for setting of the verbose level.  Users can override this
 * class to provide additional commands for their specific plugins.
 *
 * @note
 * This class is not automatically created within a SimPlugin.  Instead, within
 * their plugin constructor, the user should create an instance of this
 * or a derived class.
 */
class SimPluginMessenger : public G4UImessenger {

    public:

        /**
         * Class constructor.
         * @param userPlugin The associated SimPlugin.
         */
        SimPluginMessenger(SimPlugin* userPlugin);

        /**
         * Class destructor.
         */
        virtual ~SimPluginMessenger() {
            delete verboseCmd_;
            delete pluginDir_;
        }

        /**
         * Process the macro command.
         * @param[in] command The macro command.
         * @param[in] newValue The argument values.
         */
        void SetNewValue(G4UIcommand *command, G4String newValue);

        /**
         * Get the command path (plugin's macro directory).
         * @return The command path.
         */
        const std::string& getPath() {
            return pluginDir_->GetCommandPath();
        }

        /**
         * Get the associated SimPlugin.
         * @return The associated SimPlugin.
         */
        SimPlugin* getPlugin() {
            return userPlugin_;
        }

    private:

        /**
         * The associated UserActionPligin.
         */
        SimPlugin* userPlugin_;

        /**
         * The plugin's command directory.
         */
        G4UIdirectory* pluginDir_;

        /**
         * The command for setting verbose level.
         */
        G4UIcommand* verboseCmd_;

        /** The double parameters for the plugin. */
        G4UIcommand* paramCmd_;
};

} // namespace sim

#endif
