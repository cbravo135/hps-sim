/**
 * @file EventPrintPluginMessenger.h
 * @brief Class that defines a Geant4 messenger for the EventPrintPlugin
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef HPSSIM_EVENTPRINTPLUGIN_H_
#define HPSSIM_EVENTPRINTPLUGIN_H_

// HPS
#include "SimPlugin.h"

// Geant4
#include "G4UImessenger.hh"

// STL
#include <string>

namespace hpssim {

/**
 * @class EventPrintPlugin
 * @brief Sim plugin for printing out messages at begin and end of event and run
 */
class EventPrintPlugin: public SimPlugin {

    public:

        /**
         * Class constructor.
         * Creates a messenger for the class.
         */
        EventPrintPlugin();

        /**
         * Class destructor.
         * Deletes the messenger for the class.
         */
        virtual ~EventPrintPlugin();

        /**
         * Get the name of the plugin.
         */
        virtual std::string getName();

        std::vector<PluginAction> getActions();

        /**
         * Print a start message with the run number.
         * @param aRun The current Geant4 run that is starting.
         */
        void beginRun(const G4Run* aRun);

        /**
         * Print an end message with the run number.
         * @param aRun The current Geant4 run that is ending.
         */
        void endRun(const G4Run* aRun);

        /**
         * Print a start event message.
         * Use the primary generator hook for the start event message so it appears as early as possible in output.
         * @param anEvent The Geant4 event that is starting.
         */
        void generatePrimary(G4Event* anEvent);

        /**
         * Print an end event message.
         * @param anEvent The Geant4 event that is ending.
         */
        void endEvent(const G4Event* anEvent);

        /**
         * Set the character string to prepend to messages.
         * @param prepend The prepending string.
         */
        void setPrepend(std::string prepend);

        /**
         * Set the character string to append to messages.
         * @param append The appending string.
         */
        void setAppend(std::string append);

        /**
         * Set the modulus which determines how often to print event messages.
         * A modulus of 1 will print every event.
         * @param modulus The event print modulus.
         */
        void setModulus(unsigned modulus);

        /**
         * Set whether a message should print at end of run.
         * @param enableEndRun True to enable end of run print out.
         */
        void setEnableEndRun(bool enableEndRun);

        /**
         * Set whether a message should print at start of run.
         * @param enableStartRun True to enable start of run print out.
         */
        void setEnableStartRun(bool enableStartRun);

        /**
         * Set whether a message should print at start of event.
         * @param enableStartEvent True to enable start of event print out.
         */
        void setEnableStartEvent(bool enableStartEvent);

        /**
         * Set whether a message should print at end of event.
         * @param enableEndEvent True to enable end of event print out.
         */
        void setEnableEndEvent(bool enableEndEvent);

        /**
         * Reset the plugin state.
         * Turns on all print outs, sets modulus to 1, and restores
         * default prepend and append strings.
         */
        void reset();

    private:

        /**
         * The messenger for setting plugin parameters.
         */
        G4UImessenger* _pluginMessenger;

        /**
         * The event print modulus.
         */
        int modulus_ { 1 };

        /**
         * The prepending character string.
         */
        std::string prepend_ { ">>>" };

        /**
         * The appending character string.
         */
        std::string append_ { "<<<" };

        /**
         * Flag to enable start of run print out.
         */
        bool enableStartRun_ { true };

        /**
         * Flag to enable end of run print out.
         */
        bool enableEndRun_ { true };

        /**
         * Flag to enable start of event print out.
         */
        bool enableStartEvent_ { true };

        /**
         * Flag to enable end of event print out.
         */
        bool enableEndEvent_ { true };
};

} // namespace hpssim

#endif
