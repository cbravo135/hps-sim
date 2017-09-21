#ifndef HPSSIM_EVENTSAMPLING_H_
#define HPSSIM_EVENTSAMPLING_H_

#include "G4Event.hh"
#include "G4Poisson.hh"

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

        void setParam(double param) {
            param_ = param;
        }

    protected:

        double param_{1.};
};

/**
 * Sample a fixed number of events per Geant4 event.
 */
class UniformEventSampling : public EventSampling {

    public:

        int getNumberOfEvents(G4Event*) {
            return param_;
        }
};

class PoissonEventSampling : public EventSampling {

    public:

        int getNumberOfEvents(G4Event*) {
            double nevents = G4Poisson(param_);
            std::cout << "PoissonEventSampling: Rand sample " << nevents << " events." << std::endl;
            return nevents;
        }
};

// TODO: class PeriodicEventSampling
// samples an event every Nth Geant4 event

} // namespace hpssim

#endif
