#ifndef HPSSIM_PRIMARYGENERATORMESSENGER_H_
#define HPSSIM_PRIMARYGENERATORMESSENGER_H_

#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"

namespace hpssim {

class PrimaryGenerator;

/**
 * Messenger assigned to a single primary generator to configure and customize it.
 */
class PrimaryGeneratorMessenger : public G4UImessenger {

    public:

        PrimaryGeneratorMessenger(PrimaryGenerator*);

        virtual ~PrimaryGeneratorMessenger();

        void SetNewValue(G4UIcommand* command, G4String newValues);

    public:

        PrimaryGenerator* generator_;

        G4UIdirectory* dir_;
        G4UIcmdWithAString* fileCmd_;
        G4UIcommand* sampleCmd_;
};

}

#endif
