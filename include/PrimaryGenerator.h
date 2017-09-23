#ifndef HPSSIM_PRIMARYGENERATOR_H_
#define HPSSIM_PRIMARYGENERATOR_H_

#include "G4VPrimaryGenerator.hh"

#include "PrimaryGeneratorMessenger.h"
#include "EventSampling.h"
#include "EventTransform.h"

namespace hpssim {

class PrimaryGeneratorMessenger;

// TODO:
// -verbose level
// -new run hook (for setup/init)
// -activate and deactivate
// -print out
// -delete
// -file management: current file, file queue, hook on new file, etc.
class PrimaryGenerator : public G4VPrimaryGenerator {

    public:

        PrimaryGenerator(std::string name);

        virtual ~PrimaryGenerator();

        const std::string& getName() {
            return name_;
        }

        virtual void addFile(std::string file) {
            files_.push_back(file);
        }

        void setVerbose(int verbose) {
            verbose_ = verbose;
        }

        void setEventSampling(EventSampling* sampling) {
            if (sampling_) {
                delete sampling_;
            }
            sampling_ = sampling;
        }

        EventSampling* getEventSampling() {
            return sampling_;
        }

        void addTransform(EventTransform* transform) {
            transforms_.push_back(transform);
        }

        const std::vector<EventTransform*>& getTransforms() {
            return transforms_;
        }

        virtual void GeneratePrimaryVertex(G4Event* anEvent) = 0;

    private:

        std::string name_;
        std::vector<std::string> files_;
        PrimaryGeneratorMessenger* messenger_;
        int verbose_{1};
        EventSampling* sampling_{new UniformEventSampling};
        std::vector<EventTransform*> transforms_;
};

}

#endif
