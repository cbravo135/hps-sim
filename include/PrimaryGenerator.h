#ifndef HPSSIM_PRIMARYGENERATOR_H_
#define HPSSIM_PRIMARYGENERATOR_H_

#include "G4VPrimaryGenerator.hh"

namespace hpssim {

    class PrimaryGenerator : public G4VPrimaryGenerator {

        public:

            enum SourceType {
                TEST
                /*
                LHE,
                STDHEP,
                LCIO,
                GUN,
                GPS
                */
            };

            PrimaryGenerator(std::string name) : name_(name) {
            }

            const std::string& getName() {
                return name_;
            }

            void addFile(std::string file) {
                files_.push_back(file);
            }

            // event transforms

            // fixed prescale factor

            // should sample for this "main" event?

            // number of overlay events

        private:

            std::string name_;

            std::vector<std::string> files_;
    };
}

#endif
