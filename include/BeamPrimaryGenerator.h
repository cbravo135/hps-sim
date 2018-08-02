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

        BeamPrimaryGenerator(std::string name);

        void GeneratePrimaryVertex(G4Event* anEvent);

        void initialize();

    private:

        void computeNumberOfElectrons();

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
