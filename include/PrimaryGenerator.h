#ifndef HPSSIM_PRIMARYGENERATOR_H_
#define HPSSIM_PRIMARYGENERATOR_H_

#include "G4VPrimaryGenerator.hh"

#include "PrimaryGeneratorMessenger.h"

namespace hpssim {

class PrimaryGeneratorMessenger;

// TODO:
// -event transforms (e.g. from stdhep tools)
// -fixed prescale factor
// -true/false should sample into this "main" event?
// -number of overlay events to generate (sampling)
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

    private:

        std::string name_;

        std::vector<std::string> files_;

        PrimaryGeneratorMessenger* messenger_;
};

}

#endif
