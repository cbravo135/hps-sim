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

EventPrintPlugin::EventPrintPlugin() {
    _pluginMessenger = new EventPrintPluginMessenger(this);
}

EventPrintPlugin::~EventPrintPlugin() {
    delete _pluginMessenger;
}

std::string EventPrintPlugin::getName() {
    return "EventPrintPlugin";
}

std::vector<SimPlugin::PluginAction> EventPrintPlugin::getActions() {
    return {SimPlugin::PluginAction::RUN, SimPlugin::PluginAction::EVENT, SimPlugin::PluginAction::PRIMARY};
}

void EventPrintPlugin::beginRun(const G4Run* aRun) {
    if (enableStartRun_) {
        std::cout << prepend_ << " Start Run " << aRun->GetRunID() << " "
                << append_ << std::endl;
    }
}

void EventPrintPlugin::endRun(const G4Run* aRun) {
    if (enableEndRun_) {
        std::cout << prepend_ << " End Run " << aRun->GetRunID() << " "
                << append_ << std::endl;
    }
}

void EventPrintPlugin::generatePrimary(G4Event* anEvent) {
    if (enableStartEvent_) {
        if (anEvent->GetEventID() % modulus_ == 0) {
            std::cout << prepend_ << " Start Event " << anEvent->GetEventID()
                    << " " << append_ << std::endl;
        }
    }
}

void EventPrintPlugin::endEvent(const G4Event* anEvent) {
    if (enableEndEvent_) {
        if (anEvent->GetEventID() % modulus_ == 0) {
            std::cout << prepend_ << " End Event " << anEvent->GetEventID()
                    << " " << append_ << std::endl;
        }
    }
}

void EventPrintPlugin::setPrepend(std::string prepend) {
    prepend_ = prepend;
}

void EventPrintPlugin::setAppend(std::string append) {
    append_ = append;
}

void EventPrintPlugin::setModulus(unsigned modulus) {
    modulus_ = modulus;
}

void EventPrintPlugin::setEnableEndRun(bool enableEndRun) {
    enableEndRun_ = enableEndRun;
}

void EventPrintPlugin::setEnableStartRun(bool enableStartRun) {
    enableStartRun_ = enableStartRun;
}

void EventPrintPlugin::setEnableStartEvent(bool enableStartEvent) {
    enableStartEvent_ = enableStartEvent;
}

void EventPrintPlugin::setEnableEndEvent(bool enableEndEvent) {
    enableEndEvent_ = enableEndEvent;
}

void EventPrintPlugin::reset() {
    enableStartRun_ = true;
    enableEndRun_ = true;
    enableEndEvent_ = true;
    enableStartEvent_ = true;
    modulus_ = 1;
    prepend_ = ">>>";
    append_ = "<<<";
}

} // namespace hpssim

SIM_PLUGIN(hpssim, EventPrintPlugin)
