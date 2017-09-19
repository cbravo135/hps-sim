#ifndef HPSSIM_TESTGENERATOR_H_
#define HPSSIM_TESTGENERATOR_H_

#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"

#include "PrimaryGenerator.h"

namespace hpssim {

class TestGenerator : public PrimaryGenerator {

    public:

        TestGenerator(std::string name) : PrimaryGenerator(name) {
            gun_->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("e-"));
            gun_->SetParticlePosition(G4ThreeVector(55*mm, 25*mm, 0*mm));
            gun_->SetParticleMomentum(G4ThreeVector(0, 0, 1*GeV));
        }

        void GeneratePrimaryVertex (G4Event *evt) {
            gun_->GeneratePrimaryVertex(evt);
        }

    private:
        G4ParticleGun* gun_{new G4ParticleGun};
};


}

#endif
