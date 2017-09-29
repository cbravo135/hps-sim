#include "PGAMessenger.h"

#include "globals.hh"

#include "BeamPrimaryGenerator.h"
#include "LcioPrimaryGenerator.h"
#include "LHEPrimaryGenerator.h"
#include "PrimaryGeneratorAction.h"
#include "StdHepPrimaryGenerator.h"
#include "TestGenerator.h"

#include <sstream>

namespace hpssim {

PGAMessenger::PGAMessenger(PrimaryGeneratorAction* pga) : pga_(pga) {

    dir_ = new G4UIdirectory("/hps/generators/");

    createCmd_ = new G4UIcommand("/hps/generators/create", this);
    G4UIparameter* p = new G4UIparameter("name", 's', false);
    createCmd_->SetParameter(p);
    p = new G4UIparameter("type", 's', false);
    createCmd_->SetParameter(p);

    verboseCmd_ = new G4UIcmdWithAnInteger("/hps/generators/verbose", this);

    // Define valid source types (this should probably be static and go someplace else).
    sourceType_["TEST"] = TEST;
    sourceType_["LHE"] = LHE;
    sourceType_["STDHEP"] = STDHEP;
    sourceType_["BEAM"] = BEAM;
    sourceType_["LCIO"] = LCIO;
}

void PGAMessenger::SetNewValue(G4UIcommand* command, G4String newValues) {
    if (command == createCmd_) {
        std::stringstream params(newValues);
        std::string name, type;
        params >> name >> type;
        //std::cout << "PrimaryGeneratorMessenger: Creating generator '" << name << "' with type '" << type << "'" << std::endl;
        auto generator = createGenerator(name, type);
        if (!generator) {
            std::cerr << "PGAMessenger: The primary generator type '" << type << "' is not valid!" << std::endl;
            G4Exception("", "", FatalException, "Invalid primary generator type.");
        }
        pga_->addGenerator(generator);
    } else if (command == verboseCmd_) {
        int newLevel = G4UIcommand::ConvertToInt(newValues);
        std::cout << "PrimaryGeneratorMessenger: Setting generator verbose level to " << newLevel << std::endl;
        pga_->setVerbose(newLevel);
    }
}

PrimaryGenerator* PGAMessenger::createGenerator(std::string name, std::string type) { 
    if (sourceType_.find(type) == sourceType_.end()) {
        std::cerr << "PGAMessenger: The generator type '" << type << "' is not valid." << std::endl;
        G4Exception("", "", FatalException, "The generator type is not valid.");
    }
    SourceType srcType = sourceType_[type];
    if (srcType == TEST) {
        return new TestGenerator(name);
    } else if (srcType == LHE) {
        return new LHEPrimaryGenerator(name);
    } else if (srcType == STDHEP) {
        return new StdHepPrimaryGenerator(name);
    } else if (srcType == BEAM) {
        return new BeamPrimaryGenerator(name);
    } else if (srcType == LCIO) {
        return new LcioPrimaryGenerator(name);
    }
    return nullptr;
}
}
