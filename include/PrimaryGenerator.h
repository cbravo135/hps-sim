#ifndef HPSSIM_PRIMARYGENERATOR_H_
#define HPSSIM_PRIMARYGENERATOR_H_

#include "G4VPrimaryGenerator.hh"

#include "PrimaryGeneratorMessenger.h"

namespace hpssim {

class PrimaryGeneratorMessenger;

// TODO:
// -verbose level
// -event transforms
// -number of events to sample
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

    private:

        std::string name_;

        std::vector<std::string> files_;

        PrimaryGeneratorMessenger* messenger_;

        int verbose_{1};
};

}

#endif
