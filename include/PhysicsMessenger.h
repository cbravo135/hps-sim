#ifndef HPSSIM_PHYSICSMESSENGER_H_
#define HPSSIM_PHYSICSMESSENGER_H_

/*
 * Geant4
 */
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"

namespace hpssim {

/**
 * @class PhysicsMessenger
 */
class PhysicsMessenger : public G4UImessenger {

    public:

        PhysicsMessenger();

        virtual ~PhysicsMessenger();

        void SetNewValue(G4UIcommand* command, G4String newValues);

    private:

        G4UIdirectory* physicsDir_;
        G4UIcmdWithAString* physicsListCmd_;
};

} // namespace hpssim

#endif
