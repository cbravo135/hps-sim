#include "EventPrintPluginMessenger.h"

#include "EventPrintPlugin.h"

#include <sstream>

namespace hpssim {

EventPrintPluginMessenger::EventPrintPluginMessenger(EventPrintPlugin* plugin) :
        SimPluginMessenger(plugin), eventPrintPlugin_(plugin) {

    modulusCmd_ = new G4UIcommand(std::string(getPath() + "modulus").c_str(),
            this);
    G4UIparameter* modulus = new G4UIparameter("modulus", 'i', false);
    modulusCmd_->SetParameter(modulus);
    modulusCmd_->SetGuidance(
            "Set the modulus for event printing (1 for every event, 10 for every 10th event, etc.)");
    modulusCmd_->AvailableForStates(G4ApplicationState::G4State_PreInit,
            G4ApplicationState::G4State_Idle);

    prependCmd_ = new G4UIcommand(std::string(getPath() + "prepend").c_str(),
            this);
    G4UIparameter* prepend = new G4UIparameter("prepend", 's', false);
    prependCmd_->SetParameter(prepend);
    prependCmd_->SetGuidance("Set the string prepended to the print outs.");
    prependCmd_->AvailableForStates(G4ApplicationState::G4State_PreInit,
            G4ApplicationState::G4State_Idle);

    appendCmd_ = new G4UIcommand(std::string(getPath() + "append").c_str(),
            this);
    G4UIparameter* append = new G4UIparameter("append", 's', false);
    appendCmd_->SetParameter(append);
    appendCmd_->SetGuidance("Set the string appended to the print outs.");
    appendCmd_->AvailableForStates(G4ApplicationState::G4State_PreInit,
            G4ApplicationState::G4State_Idle);

    enableStartRunCmd_ = new G4UIcmdWithABool(
            std::string(getPath() + "enableStartRun").c_str(), this);
    enableStartRunCmd_->SetGuidance("Enable or disable print out at start of run.");
    enableStartRunCmd_->SetParameterName("enable", true, false);
    enableStartRunCmd_->SetDefaultValue(true);
    enableStartRunCmd_->AvailableForStates(G4ApplicationState::G4State_PreInit,
            G4ApplicationState::G4State_Idle);

    enableEndRunCmd_ = new G4UIcmdWithABool(
            std::string(getPath() + "enableEndRun").c_str(), this);
    enableEndRunCmd_->SetGuidance("Enable or disable print out at end of run.");
    enableStartRunCmd_->SetParameterName("enable", true, false);
    enableStartRunCmd_->SetDefaultValue(true);
    enableEndRunCmd_->AvailableForStates(G4ApplicationState::G4State_PreInit,
            G4ApplicationState::G4State_Idle);

    enableStartEventCmd_ = new G4UIcmdWithABool(
            std::string(getPath() + "enableStartEvent").c_str(), this);
    enableStartEventCmd_->SetParameterName("enable", true, false);
    enableStartEventCmd_->SetDefaultValue(true);
    enableStartEventCmd_->SetGuidance("Enable or disable print out at start of event.");
    enableStartEventCmd_->AvailableForStates(G4ApplicationState::G4State_PreInit,
            G4ApplicationState::G4State_Idle);

    enableEndEventCmd_ = new G4UIcmdWithABool(
            std::string(getPath() + "enableEndEvent").c_str(), this);
    enableEndEventCmd_->SetParameterName("enable", true, false);
    enableEndEventCmd_->SetDefaultValue(true);
    enableEndEventCmd_->SetGuidance(
            "Enable or disable print out at end of event.");
    enableEndEventCmd_->AvailableForStates(G4ApplicationState::G4State_PreInit,
            G4ApplicationState::G4State_Idle);

    resetCmd_ = new G4UIcommand(std::string(getPath() + "reset").c_str(), this);
    resetCmd_->SetGuidance(
            "Reset all plugin parameters back to default values.");
    resetCmd_->AvailableForStates(G4ApplicationState::G4State_PreInit,
            G4ApplicationState::G4State_Idle);
}

EventPrintPluginMessenger::~EventPrintPluginMessenger() {
    delete modulusCmd_;
    delete prependCmd_;
    delete appendCmd_;
    delete enableStartRunCmd_;
    delete enableEndRunCmd_;
    delete enableStartEventCmd_;
    delete enableEndEventCmd_;
    delete resetCmd_;
}

void EventPrintPluginMessenger::SetNewValue(G4UIcommand *command,
        G4String newValue) {

    // Handles verbose command.
    SimPluginMessenger::SetNewValue(command, newValue);

    if (command == modulusCmd_) {
        eventPrintPlugin_->setModulus(std::atoi(newValue));
    } else if (command == prependCmd_) {
        eventPrintPlugin_->setPrepend(newValue);
    } else if (command == appendCmd_) {
        eventPrintPlugin_->setAppend(newValue);
    } else if (command == resetCmd_) {
        eventPrintPlugin_->reset();
    } else {
        /**
         * Process commands with a boolean argument.
         */
        bool enable;
        std::istringstream(newValue) >> enable;
        if (command == enableStartRunCmd_) {
            eventPrintPlugin_->setEnableEndRun(enable);
        } else if (command == enableEndRunCmd_) {
            eventPrintPlugin_->setEnableStartRun(enable);
        } else if (command == enableStartEventCmd_) {
            eventPrintPlugin_->setEnableStartEvent(enable);
        } else if (command == enableEndEventCmd_) {
            eventPrintPlugin_->setEnableEndEvent(enable);
        }
    }
}

} // namespace sim
