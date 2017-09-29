#ifndef HPSSIM_PGAMESSENGER_H_
#define HPSSIM_PGAMESSENGER_H_

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"

#include "PrimaryGenerator.h"

#include <map>

namespace hpssim {

class PrimaryGeneratorAction;

class PGAMessenger : public G4UImessenger {

    public:

        enum SourceType {
            TEST = 1,
            LHE,
            STDHEP,
            LCIO,
            BEAM
        };

        PGAMessenger(PrimaryGeneratorAction* pga);

        void SetNewValue(G4UIcommand* command, G4String newValues);

        PrimaryGenerator* createGenerator(std::string name, std::string type);

    private:

        PrimaryGeneratorAction* pga_;

        G4UIcommand* createCmd_;

        G4UIdirectory* dir_;

        G4UIcmdWithAnInteger* verboseCmd_;

        std::map<std::string, SourceType> sourceType_;
};

}

#endif
