#ifndef HPSSIM_LCIOPERSISTENCYMESSENGER_H_
#define HPSSIM_LCIOPERSISTENCYMESSENGER_H_

#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"

namespace hpssim {

class LcioPersistencyManager;

// TODO:
// -drop collections
// -enable/disable
// -coll flags
// -cal hit contribs
class LcioPersistencyMessenger : public G4UImessenger {

    public:

        LcioPersistencyMessenger(LcioPersistencyManager* mgr);

        void SetNewValue(G4UIcommand* command, G4String newValues);

    private:

        G4UIdirectory* dir_;
        G4UIcmdWithAString* fileCmd_;
        G4UIcmdWithAnInteger* verboseCmd_;

        LcioPersistencyManager* mgr_{nullptr};
};

}

#endif
