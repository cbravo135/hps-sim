#include <math.h>

#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"

#include "CLHEP/Random/RandGauss.h"

#include "PrimaryGenerator.h"

namespace hpssim {

class BeamPrimaryGenerator : public PrimaryGenerator {

    public:

        BeamPrimaryGenerator(std::string name) : PrimaryGenerator(name) {
            position_ = G4ThreeVector(0, 0, -10);
            momentum_ = G4ThreeVector(0, 0, 1);
        }

        virtual void GeneratePrimaryVertex(G4Event* anEvent) {
            if (!numberOfElectrons_) {
                numberOfElectrons_ = computeNumberOfElectrons();
            }
            std::cout << "BeamPrimaryGenerator: Generating " << numberOfElectrons_ << " electrons." << std::endl;
            gun_.SetParticleEnergy(energy_);
            gun_.SetParticleMomentum(momentum_);
            gun_.SetParticlePosition(position_);
            for (int i = 0; i < numberOfElectrons_; i++) {
                G4ThreeVector sampledPosition;
                sampledPosition.setX(position_.x() + CLHEP::RandGauss::shoot(0, sigmaX_));
                sampledPosition.setY(position_.y() + CLHEP::RandGauss::shoot(0, sigmaY_));
                sampledPosition.setZ(position_.z());
                std::cout << "BeamPrimaryGenerator: Sampled pos " << sampledPosition << "." << std::endl;
                gun_.SetParticlePosition(sampledPosition);
                gun_.GeneratePrimaryVertex(anEvent);
            }
        }

        void setEnergy(double energy) {
            energy_ = energy;
        }

        void setPosition(double x, double y, double z) {
            position_.set(x, y, z);
        }

        void setCurrent(double current) {
            current_ = current;
            numberOfElectrons_ = 0; // n electrons now needs to be recalculated
        }

    private:

        int computeNumberOfElectrons() {

            // electrons per second for 100 nA
            static double electronsPerSecond = 6.25 * pow(10., 11.);

            // number of bunches per second (machine parameter)
            static int nBunchesPerSecond = 500000000;

            // number of electrons per bunch
            static int electronsPerBunch = electronsPerSecond / nBunchesPerSecond;

            // convert to specified current
            int nelectrons = electronsPerBunch * current_ / 100.;

            return nelectrons;
        }

    private:

        /* Beam current in nA (may use 200, 450). */
        double current_{50};

        /* Beam energy in GeV */
        double energy_{1.056};

        /** Number of electrons computed from current. */
        int numberOfElectrons_{0};

        /** Vertex position of the beam particles. */
        G4ThreeVector position_;

        /** Gaussian sigma of vertex X coordinate. */
        double sigmaX_{0.300};

        /** Gaussian sigma of vertex Y coordinate. */
        double sigmaY_{0.030};

        /** Beam particle momentum. */
        G4ThreeVector momentum_;

        /** Particle gun for generating events. */
        G4ParticleGun gun_;
};

}
