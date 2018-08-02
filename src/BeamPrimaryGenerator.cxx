#include "BeamPrimaryGenerator.h"

namespace hpssim {

BeamPrimaryGenerator::BeamPrimaryGenerator(std::string name) :
        PrimaryGenerator(name) {
}

void BeamPrimaryGenerator::GeneratePrimaryVertex(G4Event* anEvent) {
    if (verbose_ > 1) {
        std::cout << "BeamPrimaryGenerator: Generating " << nelectrons_ << " electrons in event "
                << anEvent->GetEventID() << std::endl;
    }

    // Smear the number of electrons.
    int nGenerate = nelectrons_;
    if (this->smearNElectrons_) {
        nGenerate = CLHEP::RandGauss::shoot(nelectrons_, sqrt(nelectrons_));
        if (verbose_ > 1) {
            std::cout << "BeamPrimaryGenerator: Generating " << nGenerate << " electrons after Gaussian smearing"
                    << std::endl;
        }
    }

    for (int i = 0; i < nGenerate; i++) {

        G4PrimaryVertex* vertex = new G4PrimaryVertex();
        G4ThreeVector sampledPosition;
        sampledPosition.setX(position_.x() + CLHEP::RandGauss::shoot(0, sigmaX_));
        sampledPosition.setY(position_.y() + CLHEP::RandGauss::shoot(0, sigmaY_));
        sampledPosition.setZ(position_.z());

        if (verbose_ > 2) {
            std::cout << "BeamPrimaryGenerator: Sampled pos " << sampledPosition << " for electron " << i << std::endl;
        }

        vertex->SetPosition(sampledPosition.x(), sampledPosition.y(), sampledPosition.z());
        anEvent->AddPrimaryVertex(vertex);

        G4PrimaryParticle* primaryParticle = new G4PrimaryParticle();
        static auto electronDef = G4ParticleTable::GetParticleTable()->FindParticle("e-");
        primaryParticle->SetParticleDefinition(electronDef);
        primaryParticle->SetMomentumDirection(direction_);
        primaryParticle->SetTotalEnergy(energy_);
        vertex->SetPrimary(primaryParticle);
    }
}

void BeamPrimaryGenerator::initialize() {

    /*
     * Handle two basic cases:
     * 1) If nelectrons is set then this is the number of particles fired, without Gaussian smearing.
     * 2) If nelectrons is not set, then the beam parameters are calculated from the current and energy
     * settings, and the number of electrons is smeared.
     */
    Parameters& params = getParameters();
    energy_ = params.get("energy", energy_);
    if (params.has("nelectrons")) {
        /*
         * In the case where number of electrons is assigned explicitly, then this does not need to be
         * calculated and smearing of electron count is not applied.
         */
        nelectrons_ = params.get("nelectrons");
        if (verbose_ > 1) {
            std::cout << "BeamPrimaryGenerator: Number of electrons was set to " << nelectrons_ << std::endl;
        }
        smearNElectrons_ = false;
    } else {
        /*
         * Where number of electrons is not specified, compute from the beam current, and smear the electron
         * count using Gaussian distribution.
         */
        current_ = params.get("current", current_);
        computeNumberOfElectrons();
        if (verbose_ > 1) {
            std::cout << "BeamPrimaryGenerator: Calculated number of electrons " << nelectrons_ << std::endl;
        }
        smearNElectrons_ = true;
    }

    // Add transformation into beam coordinates.
    this->addTransform(new RotateTransform);
}

void BeamPrimaryGenerator::computeNumberOfElectrons() {

    // electrons per second for 100 nA
    static double electronsPerSecond = 6.25 * pow(10., 11.);

    // number of bunches per second (machine parameter)
    static int nBunchesPerSecond = 500000000;

    // number of electrons per bunch
    static int electronsPerBunch = electronsPerSecond / nBunchesPerSecond;

    // convert to specified current
    nelectrons_ = electronsPerBunch * current_ / 100.;
}

}
