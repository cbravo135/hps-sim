#ifndef HPSSIM_PRIMARYGENERATORMESSENGER_H_
#define HPSSIM_PRIMARYGENERATORMESSENGER_H_

#include "G4UImessenger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"

namespace hpssim {

class PrimaryGenerator;

/**
 * @class PrimaryGeneratorMessenger
 * @brief Messenger assigned to a single primary generator to configure and customize it.
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
        G4UIcmdWithAnInteger* verboseCmd_;
        G4UIcommand* paramCmd_;
        G4UIcommand* sampleCmd_;

        G4UIdirectory* transformDir_;
        G4UIcommand* smearCmd_;
        G4UIcommand* posCmd_;
        G4UIcommand* rotCmd_;
        G4UIcommand* randzCmd_;

        G4UIcommand* randomCmd_;
        G4UIcommand* sequentialCmd_;
        G4UIcommand* linearCmd_;
        G4UIcommand* semirandomCmd_;
};

}

#endif
