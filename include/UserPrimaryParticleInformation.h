/**
 * @file UserPrimaryParticleInformation.h
 * @brief Class that provides extra information for Geant4 primary particles
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef SIMAPPLICATION_USERPRIMARYPARTICLEINFORMATION_H_
#define SIMAPPLICATION_USERPRIMARYPARTICLEINFORMATION_H_

/*
 * Geant4
 */
#include "G4VUserPrimaryParticleInformation.hh"

namespace hpssim {

/**
 * @class UserPrimaryParticleInformation
 * @brief Defines extra information attached to a Geant4 primary particle
 */
class UserPrimaryParticleInformation : public G4VUserPrimaryParticleInformation {

    public:

        /**
         * Class Constructor.
         */
        UserPrimaryParticleInformation() {;}

        /**
         * Class destructor.
         */
        virtual ~UserPrimaryParticleInformation() {;}

        /**
         * Set the HEP event status (generator status) e.g. from an LHE particle.
         * @param hepEvtStatus The HEP event status.
         */
        void setGenStatus(int genStatus) {
            genStatus_ = genStatus;
        }

        /**
         * Get the HEP event status.
         * @return The HEP event status.
         */
        int getGenStatus() {
            return genStatus_;
        }

        /**
         * Implement pure virtual method.
         */
        void Print() const {
        }

        static UserPrimaryParticleInformation* getUserPrimaryParticleInformation(G4PrimaryParticle* aPrimary) {
            return static_cast<UserPrimaryParticleInformation*>(aPrimary->GetUserInformation());
        }

    private:

        /**
         * The HEP event status.
         */
        int genStatus_{-1};
};

}

#endif
