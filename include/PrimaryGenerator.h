#ifndef HPSSIM_PRIMARYGENERATOR_H_
#define HPSSIM_PRIMARYGENERATOR_H_

#include "G4VPrimaryGenerator.hh"

#include "EventSampling.h"
#include "EventTransform.h"
#include "Parameters.h"
#include "PrimaryGeneratorMessenger.h"

#include <map>

namespace hpssim {

class PrimaryGeneratorMessenger;

// TODO:
// -activate and deactivate
// -print out
// -delete
// -file management: current file, file queue, hook on new file, etc.
class PrimaryGenerator : public G4VPrimaryGenerator {

    public:

        PrimaryGenerator(std::string name);

        virtual ~PrimaryGenerator();

        /**
         * Sub-classes must implement this method to generate an event.
         */
        virtual void GeneratePrimaryVertex(G4Event* anEvent) = 0;

        /**
         * Initialization callback to open reader.
         */
        virtual void initialize() {
        }

        /**
         * Cleanup callback to close open readers.
         */
        virtual void cleanup() {
        }

        Parameters& getParameters() {
            return params_;
        }

        const std::string& getName() {
            return name_;
        }

        virtual void addFile(std::string file) {
            files_.push_back(file);
        }

        const std::vector<std::string>& getFiles() {
            return files_;
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

    protected:
        int verbose_{1};
        std::vector<std::string> files_;

    private:
        std::string name_;
        PrimaryGeneratorMessenger* messenger_;
        std::vector<EventTransform*> transforms_;
        EventSampling* sampling_{new UniformEventSampling};
        Parameters params_;
};

}

#endif
