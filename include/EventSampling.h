#ifndef HPSSIM_EVENTSAMPLING_H_
#define HPSSIM_EVENTSAMPLING_H_

#include "G4Event.hh"
#include "G4Poisson.hh"

namespace hpssim {

/**
 * @class EventSampling
 * @brief Calculates how many events to read from a generator to overlay onto a single Geant4 event.
 * @note The sampling distributions may use a single generic double parameter for configuration.
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
         * Set the double param value.
         */
        void setParam(double param) {
            param_ = param;
        }

        /**
         * Get the double param value.
         */
        double getParam() {
            return param_;
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

/**
 * @class PoissonEventSampling
 * @brief Sample the number of events from a Poisson distribution.
 */
class PoissonEventSampling : public EventSampling {

    public:

        int getNumberOfEvents(G4Event*) {
            double nevents = G4Poisson(param_);
            return nevents;
        }
};

/**
 * @class PeriodicEventSampling
 * @brief Sample one event every Nth Geant4 event using a modulus.
 */
class PeriodicEventSampling : public EventSampling {

    public:

        int getNumberOfEvents(G4Event* anEvent) {
            if (anEvent->GetEventID() % (int)param_ == 0) {
                return 1;
            } else { 
                return 0;
            }    
        }

};

/**
 * @class CrossSectionEventSampling
 * @brief Sample the number of events from a Poisson distribution determined from the
 * physics cross section.
 */
class CrossSectionEventSampling : public PoissonEventSampling {

    public:

        void setCrossSection(double crossSection) {
            crossSection_ = crossSection;
            calculateMu();
        }

        void calculateMu() {
            double integratedLuminosity = density_ * nElectrons_ * targetThickness_;
            param_ = integratedLuminosity * 1e-12 * crossSection_;
        }

    private:

        /*
         * Default parameters for integrated luminosity calculation.
         */
        double targetThickness_{0.0004062};
        double nElectrons_{625};
        double density_{6.306e-2};

        /* Physics cross section which should be externally set by the generator during initialization. */
        double crossSection_{0};
};

} // namespace hpssim

#endif
