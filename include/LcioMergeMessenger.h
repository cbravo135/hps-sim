#ifndef HPSSIM_LCIOMERGEMESSENGER_H_
#define HPSSIM_LCIOMERGEMESSENGER_H_

#include "G4UImessenger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"

namespace hpssim {

class LcioMergeTool;

class LcioMergeMessenger : public G4UImessenger {

    public:

        LcioMergeMessenger(LcioMergeTool* merge);

        void SetNewValue(G4UIcommand* command, G4String newValues);

    private:

        LcioMergeTool* merge_;

        G4UIdirectory* mergeDir_;
        G4UIdirectory* filterDir_;

        G4UIcmdWithAString* fileCmd_;

        //G4UIcmdWithADoubleAndUnit* ecalEnergyFilterCmd_;
        G4UIcmdWithAnInteger* eventModulusFilterCmd_;
};

}

#endif
