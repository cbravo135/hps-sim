#ifndef HPSSIM_LCIOPERSISTENCYMESSENGER_H_
#define HPSSIM_LCIOPERSISTENCYMESSENGER_H_

#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"

namespace hpssim {

class LcioPersistencyManager;

// TODO: additional macro commands
// -drop collections
// -enable/disable persistency engine
// -set coll flags
// -configure cal hit contribs
class LcioPersistencyMessenger : public G4UImessenger {

    public:

        LcioPersistencyMessenger(LcioPersistencyManager* mgr);

        void SetNewValue(G4UIcommand* command, G4String newValues);

    private:

        G4UIdirectory* dir_;
        G4UIcmdWithAString* fileCmd_;
        G4UIcmdWithAnInteger* verboseCmd_;

        /*
         * Write mode commands.
         */
        G4UIcommand* newCmd_;
        G4UIcommand* appendCmd_;
        G4UIcommand* recreateCmd_;

        LcioPersistencyManager* mgr_{nullptr};
};

}

#endif
