#ifndef HPSSIM_UNKNOWNDECAYPHYSICS_H_
#define HPSSIM_UNKNOWNDECAYPHYSICS_H_ 1

#include "G4VPhysicsConstructor.hh"
#include "G4Decay.hh"
#include "G4ParticleTable.hh"
#include "G4UnknownDecay.hh"
#include "G4UnknownParticle.hh"

namespace hpssim {

class UnknownDecayPhysics : public G4VPhysicsConstructor {

    public:

        UnknownDecayPhysics(const G4String& name = "decay") :
            G4VPhysicsConstructor(name) {
        }

        virtual ~UnknownDecayPhysics() {
        }

        void ConstructParticle() {
            G4UnknownParticle::UnknownParticleDefinition();
        }

        void ConstructProcess() {
            G4ParticleTable* tbl = G4ParticleTable::GetParticleTable();
            G4ParticleDefinition* def = tbl->FindParticle("unknown");
            if (def) {
                G4ProcessManager* procMgr = def->GetProcessManager();
                procMgr->AddProcess(&fUnknownDecay);
                procMgr->SetProcessOrdering(&fUnknownDecay, idxPostStep);
            }
/*
            aParticleIterator->reset();
            while( (*aParticleIterator)() ){
                G4ParticleDefinition* particle = aParticleIterator->value();
                G4ProcessManager* pmanager = particle->GetProcessManager();
                if(particle->GetParticleName()=="unknown") {
                    pmanager ->AddProcess(&fUnknownDecay);
                    pmanager ->SetProcessOrdering(&fUnknownDecay, idxPostStep);
                }
            }
*/
        }

    protected:
        G4Decay fDecayProcess;
        G4UnknownDecay fUnknownDecay;
};

}

#endif
