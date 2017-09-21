#ifndef HPSSIM_EVENTSAMPLING_H_
#define HPSSIM_EVENTSAMPLING_H_

#include "G4Event.hh"
#include "G4RandomTools.hh"
#include "CLHEP/Random/RandPoisson.h"

namespace hpssim {

/**
 * @brief Interface for sampling from an event generator into a single Geant4 event.
 * @note The sampling distributions have a single double parameter.
 */
class EventSampling {

    public:

        virtual ~EventSampling() {
        }

        /**
         * Get the number of events to sample given the Geant4 event data.
         */
        virtual int getNumberOfEvents(G4Event* event) = 0;

        /**
         * Hook for setup of any necessary utils like random engine, etc.
         */
        void configure() {
        }

        void setParam(double param) {
            param_ = param;
        }

        double getParam() {
            return param_;
        }

    private:

        double param_{0};
};

/**
 * Sample a fixed number of events per Geant4 event.
 */
class UniformEventSampling : public EventSampling {

    public:

        UniformEventSampling() {
        }

        void setNumberOfEvents(unsigned int nevents) {
            nevents_ = nevents;
        }


        int getNumberOfEvents(G4Event* event) {
            return nevents_;
        }

    private:

        unsigned int nevents_{1};

};

class PoissonEventSampling : public EventSampling {

    public:

        PoissonEventSampling() : mu_{1.0} {
        }

        virtual ~PoissonEventSampling() {
            delete pois_;
        }

        /**
         * Set the mean of the distribution.
         */
        void setMu(double mu) {
            mu_ = mu;
        }

        int getNumberOfEvents(G4Event* event) {
            int nevents = (int) pois_->flat();
            std::cout << "PoissonEventSampling: Rand sample " << nevents << " this time." << std::endl;
            return nevents;
        }

        void configure() {
            CLHEP::HepRandomEngine* engine = G4Random::getTheEngine();
            pois_ = new CLHEP::RandPoisson(*engine, mu_);
        }

    private:

        double mu_;
        CLHEP::RandPoisson* pois_{nullptr};
};

// TODO: class PeriodicEventSampling
// samples an event every Nth Geant4 event

} // namespace hpssim

#endif
