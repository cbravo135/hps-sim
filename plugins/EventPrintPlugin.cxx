/**
 * @file EventPrintPlugin.h
 * @brief Class that defines a sim plugin to print out event information
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

// HPS
#include "EventPrintPlugin.h"
#include "EventPrintPluginMessenger.h"

// Geant4
#include "G4UImessenger.hh"

// STL
#include <string>

namespace hpssim {

/**
 * Class constructor.
 * Creates a messenger for the class.
 */
EventPrintPlugin::EventPrintPlugin() {
    _pluginMessenger = new EventPrintPluginMessenger(this);
}

/**
 * Class destructor.
 * Deletes the messenger for the class.
 */
EventPrintPlugin::~EventPrintPlugin() {
    delete _pluginMessenger;
}

/**
 * Get the name of the plugin.
 */
std::string EventPrintPlugin::getName() {
    return "EventPrintPlugin";
}

std::vector<SimPlugin::PluginAction> EventPrintPlugin::getActions() {
    return {SimPlugin::PluginAction::RUN, SimPlugin::PluginAction::EVENT, SimPlugin::PluginAction::PRIMARY};
}

/**
 * Print a start message with the run number.
 * @param aRun The current Geant4 run that is starting.
 */
void EventPrintPlugin::beginRun(const G4Run* aRun) {
    if (enableStartRun_) {
        std::cout << prepend_ << " Start Run " << aRun->GetRunID() << " "
                << append_ << std::endl;
    }
}

/**
 * Print an end message with the run number.
 * @param aRun The current Geant4 run that is ending.
 */
void EventPrintPlugin::endRun(const G4Run* aRun) {
    if (enableEndRun_) {
        std::cout << prepend_ << " End Run " << aRun->GetRunID() << " "
                << append_ << std::endl;
    }
}

/**
 * Print a start event message.
 * Use the primary generator hook for the start event message so it appears as early as possible in output.
 * @param anEvent The Geant4 event that is starting.
 */
void EventPrintPlugin::generatePrimary(G4Event* anEvent) {
    if (enableStartEvent_) {
        if (anEvent->GetEventID() % modulus_ == 0) {
            std::cout << prepend_ << " Start Event " << anEvent->GetEventID()
                    << " " << append_ << std::endl;
        }
    }
}

/**
 * Print an end event message.
 * @param anEvent The Geant4 event that is ending.
 */
void EventPrintPlugin::endEvent(const G4Event* anEvent) {
    if (enableEndEvent_) {
        if (anEvent->GetEventID() % modulus_ == 0) {
            std::cout << prepend_ << " End Event " << anEvent->GetEventID()
                    << " " << append_ << std::endl;
        }
    }
}

/**
 * Set the character string to prepend to messages.
 * @param prepend The prepending string.
 */
void EventPrintPlugin::setPrepend(std::string prepend) {
    prepend_ = prepend;
}

/**
 * Set the character string to append to messages.
 * @param append The appending string.
 */
void EventPrintPlugin::setAppend(std::string append) {
    append_ = append;
}

/**
 * Set the modulus which determines how often to print event messages.
 * A modulus of 1 will print every event.
 * @param modulus The event print modulus.
 */
void EventPrintPlugin::setModulus(unsigned modulus) {
    modulus_ = modulus;
}

/**
 * Set whether a message should print at end of run.
 * @param enableEndRun True to enable end of run print out.
 */
void EventPrintPlugin::setEnableEndRun(bool enableEndRun) {
    enableEndRun_ = enableEndRun;
}

/**
 * Set whether a message should print at start of run.
 * @param enableStartRun True to enable start of run print out.
 */
void EventPrintPlugin::setEnableStartRun(bool enableStartRun) {
    enableStartRun_ = enableStartRun;
}

/**
 * Set whether a message should print at start of event.
 * @param enableStartEvent True to enable start of event print out.
 */
void EventPrintPlugin::setEnableStartEvent(bool enableStartEvent) {
    enableStartEvent_ = enableStartEvent;
}

/**
 * Set whether a message should print at end of event.
 * @param enableEndEvent True to enable end of event print out.
 */
void EventPrintPlugin::setEnableEndEvent(bool enableEndEvent) {
    enableEndEvent_ = enableEndEvent;
}

/**
 * Reset the plugin state.
 * Turns on all print outs, sets modulus to 1, and restores
 * default prepend and append strings.
 */
void EventPrintPlugin::reset() {
    enableStartRun_ = true;
    enableEndRun_ = true;
    enableEndEvent_ = true;
    enableStartEvent_ = true;
    modulus_ = 1;
    prepend_ = ">>>";
    append_ = "<<<";
}

} // namespace sim

SIM_PLUGIN(hpssim, EventPrintPlugin)
