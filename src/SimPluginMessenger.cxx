#include "SimPluginMessenger.h"

#include "SimPlugin.h"

namespace hpssim {

    SimPluginMessenger::SimPluginMessenger(SimPlugin* userPlugin) :
            userPlugin_(userPlugin) {

        pluginDir_ = new G4UIdirectory(std::string("/hps/plugins/" + userPlugin->getName() + "/").c_str());
        pluginDir_->SetGuidance(std::string("Commands for the sim plugin " + userPlugin->getName()).c_str());

        verboseCmd_ = new G4UIcommand(std::string(getPath() + "verbose").c_str(), this);
        G4UIparameter* verbose = new G4UIparameter("verboseLevel", 'i', false);
        verboseCmd_->SetParameter(verbose);
        verboseCmd_->SetGuidance("Set the verbosity level of the sim plugin (1-4).");
        verboseCmd_->AvailableForStates(G4ApplicationState::G4State_PreInit, G4ApplicationState::G4State_Idle);
    }

    void SimPluginMessenger::SetNewValue(G4UIcommand *command, G4String newValue) {
        if (command == verboseCmd_) {
            userPlugin_->setVerboseLevel(std::atoi(newValue));
            std::cout << userPlugin_->getName() << " verbose set to " << userPlugin_->getVerboseLevel() << std::endl;
        }
    }
} // namespace hpssim
