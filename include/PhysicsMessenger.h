#ifndef HPSSIM_PHYSICSMESSENGER_H_
#define HPSSIM_PHYSICSMESSENGER_H_

/*
 * Geant4
 */
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4PhysListFactory.hh"

namespace hpssim {

/**
 * @class PhysicsMessenger
 */
class PhysicsMessenger : public G4UImessenger {

    public:

        /**
         * Class constructor.
         * @brief Needs a reference to PhysListFactory for setting up guidance.
         */
        PhysicsMessenger(G4PhysListFactory*);

        /**
         * Class destructor.
         */
        virtual ~PhysicsMessenger();

        /**
         * Handle messenger commands.
         * @note Currently, this only sets the name of the physics list
         * with the run manager.
         */
        void SetNewValue(G4UIcommand* command, G4String newValues);

    private:

        /**
         * UI dir for physics commands.
         */
        G4UIdirectory* physicsDir_;

        /**
         * UI command to set name of physics list.
         */
        G4UIcmdWithAString* physicsListCmd_;
};

} // namespace hpssim

#endif
