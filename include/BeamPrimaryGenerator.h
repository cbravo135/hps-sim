#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

#include "CLHEP/Random/RandGauss.h"

#include "PrimaryGenerator.h"
#include "UserPrimaryParticleInformation.h"

#include <math.h>

namespace hpssim {

/**
 * @class BeamPrimaryGenerator
 * @brief Generates particles matching the HPS beam profile
 * @note
 *
 * @par
 * This class accepts the following parameters through its PrimaryGenerator command interface.
 * <ul>
 * <li>nelectrons - fixed number of electrons to fire (which will not be smeared)</li>
 * <li>current - beam current in nA for calculating number of electrons</li>
 * <li>energy - beam energy in GeV</li>
 * </ul>
 *
 * @par
 * This class has the following features and assumptions:
 * <ul>
 * <li>The energy and current of the beam can be used to calculate the number of electrons (625 electrons per event by default).</li>
 * <li>Number of electrons can be explicitly set to override the calculated value.</li>
 * <li>Gaussian smearing is applied to the number of electrons, if it is not overridden via a parameter.</li>
 * <li>Vertex X and Y positions are smeared according to the beam's transverse profile.</li>
 * <li>Rotation into beam coordinates is automatically applied using the RotateTransform.</li>
 * <li>Particle direction is (0,0,1) before rotation.
 * <li>Origin of beam particles is currently hard-coded to 10 mm upstream of the target at (0,0,0).
 * <li>Position of the target is assumed to be (0,0,0) in the world coordinate system.
 * </ul>
 */
class BeamPrimaryGenerator : public PrimaryGenerator {

    public:

        BeamPrimaryGenerator(std::string name) : PrimaryGenerator(name) {
        }

        void GeneratePrimaryVertex(G4Event* anEvent) {
            if (verbose_ > 1) {
                std::cout << "BeamPrimaryGenerator: Generating " << nelectrons_
                        << " electrons in event " << anEvent->GetEventID() << std::endl;
            }

            // Smear the number of electrons.
            int nGenerate = nelectrons_;
            if (this->smearNElectrons_) {
                nGenerate = CLHEP::RandGauss::shoot(nelectrons_, sqrt(nelectrons_));
                if (verbose_ > 1) {
                    std::cout << "BeamPrimaryGenerator: Generating " << nGenerate << " electrons after Gaussian smearing" << std::endl;
                }
            }

            for (int i = 0; i < nGenerate; i++) {

                G4PrimaryVertex* vertex = new G4PrimaryVertex();
                G4ThreeVector sampledPosition;
                sampledPosition.setX(position_.x() + CLHEP::RandGauss::shoot(0, sigmaX_));
                sampledPosition.setY(position_.y() + CLHEP::RandGauss::shoot(0, sigmaY_));
                sampledPosition.setZ(position_.z());

                if (verbose_ > 2) {
                    std::cout << "BeamPrimaryGenerator: Sampled pos " << sampledPosition
                            << " for electron " << i << std::endl;
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

        void initialize() {

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

    private:

        void computeNumberOfElectrons() {

            // electrons per second for 100 nA
            static double electronsPerSecond = 6.25 * pow(10., 11.);

            // number of bunches per second (machine parameter)
            static int nBunchesPerSecond = 500000000;

            // number of electrons per bunch
            static int electronsPerBunch = electronsPerSecond / nBunchesPerSecond;

            // convert to specified current
            nelectrons_ = electronsPerBunch * current_ / 100.;
        }

    private:

        /** Vertex position of the beam particles. */
        G4ThreeVector position_{G4ThreeVector(0, 0, -10)};

        /** Beam particle momentum in GeV. */
        G4ThreeVector direction_{G4ThreeVector(0, 0, 1.)};

        /* Beam energy in GeV. */
        double energy_{1.056 * GeV};

        /** Number of electrons to fire in one event. */
        int nelectrons_{0};

        /* Beam current in nA (may use 200 or 450 also). */
        double current_{50};

        /** Gaussian sigma of vertex X coordinate. */
        double sigmaX_{0.300};

        /** Gaussian sigma of vertex Y coordinate. */
        double sigmaY_{0.030};

        /** Flag for Gaussian smearing of number of electrons. */
        bool smearNElectrons_{false};
};

}
