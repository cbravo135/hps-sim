#ifndef HPSSIM_PRIMARYGENERATORMESSENGER_H_
#define HPSSIM_PRIMARYGENERATORMESSENGER_H_

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"

#include "PrimaryGenerator.h"

#include <map>

namespace hpssim {

    class PrimaryGeneratorAction;

    class PGAMessenger : public G4UImessenger {

        public:

            PGAMessenger(PrimaryGeneratorAction* pga);

            void SetNewValue(G4UIcommand* command, G4String newValues);

            PrimaryGenerator* createGenerator(std::string name, std::string type);

        private:

            PrimaryGeneratorAction* pga_;

            G4UIcommand* createCmd_;

            G4UIdirectory* dir_;

            std::map<std::string, PrimaryGenerator::SourceType> sourceType_;
    };

}

#endif
