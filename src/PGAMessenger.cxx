#include "PrimaryGeneratorAction.h"

#include "TestGenerator.h"

#include <sstream>
#include "../include/PGAMessenger.h"

namespace hpssim {

    PGAMessenger::PGAMessenger(PrimaryGeneratorAction* pga) : pga_(pga) {

        dir_ = new G4UIdirectory("/hps/generators");

        createCmd_ = new G4UIcommand("/hps/generators/create", this);
        G4UIparameter* p = new G4UIparameter("name", 's', false);
        createCmd_->SetParameter(p);
        p = new G4UIparameter("type", 's', false);
        createCmd_->SetParameter(p);

        sourceType_["TEST"] = PrimaryGenerator::TEST;
    }

    void PGAMessenger::SetNewValue(G4UIcommand* command, G4String newValues) {
        if (command == createCmd_) {
            std::stringstream params(newValues);
            std::string name, type;
            params >> name >> type;
            std::cout << "PrimaryGeneratorMessenger: Creating generator '" << name << "' with type '" << type << "'" << std::endl;
            auto generator = createGenerator(name, type);
            pga_->addGenerator(generator);
        }
    }

    PrimaryGenerator* PGAMessenger::createGenerator(std::string name, std::string type) {
        PrimaryGenerator::SourceType srcType = sourceType_[type];
        if (srcType == PrimaryGenerator::TEST) {
            return new TestGenerator(name);
        }
        return nullptr;
    }
}
