/**
 * @file LHEPrimaryGenerator.h
 * @brief Class for generating a Geant4 event from LHE event data
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef HPSSIM_LHEPRIMARYGENERATOR_H_
#define HPSSIM_LHEPRIMARYGENERATOR_H_

#include "G4RunManager.hh"
#include "G4VPrimaryGenerator.hh"

#include "LHEReader.h"
#include "PrimaryGenerator.h"

namespace hpssim {

/**
 * @class LHEPrimaryGenerator
 * @brief Generates a Geant4 event from an LHEEvent
 */
class LHEPrimaryGenerator: public PrimaryGenerator {

    public:

        /**
         * Class constructor.
         * @param reader The LHE reader with the event data.
         */
        LHEPrimaryGenerator(std::string name, LHEReader* reader);

        LHEPrimaryGenerator(std::string name) :
                PrimaryGenerator(name) {
            reader_ = nullptr;
            lheEvent_ = nullptr;
        }

        /**
         * Class destructor.
         */
        virtual ~LHEPrimaryGenerator();

        /**
         * Generate vertices in the Geant4 event.
         * @param anEvent The Geant4 event.
         */
        void GeneratePrimaryVertex(G4Event* anEvent);

        void initialize() {
            if (fileQueue_.size()) {
                std::string nextFile = fileQueue_.front();
                fileQueue_.pop();
                reader_ = new LHEReader(nextFile);
            } else {
                G4Exception("", "", RunMustBeAborted,
                        G4String("No files were specified for generator '" + this->getName() + "'"));
            }

            // Setup event sampling if using cross section.
            setupEventSampling();
        }

        /*
         * Read the next LHE event from the file.
         * If the current file ran out of events then go to the next file.
         */
        bool readNextEvent() {
            lheEvent_ = reader_->readNextEvent();
            if (lheEvent_ == nullptr) {
                if (reader_) {
                    reader_->close();
                    delete reader_;
                }
                if (fileQueue_.size()) {

                    std::string nextFile = fileQueue_.front();
                    std::cout << "LHEPrimaryGenerator: Opening '" << nextFile << "' for reading" << std::endl;
                    fileQueue_.pop();

                    // Create reader for next file.
                    reader_ = new LHEReader(nextFile);

                    // Setup event sampling if using cross section.
                    setupEventSampling();

                    // Read in the next event.
                    lheEvent_ = reader_->readNextEvent();

                    if (!lheEvent_) {
                        // Should never happen unless the next file is invalid or empty.
                        std::cerr << "LHEPrimaryGenerator: Next file '" << nextFile << "' does not have any events!"
                                << std::endl;
                        return false;
                    }
                } else {
                    // Ran out of files.
                    std::cerr << "LHEPrimaryGenerator: No more files to process!" << std::endl;
                    return false;
                }
            }
            return true;
        }

    private:

        // Setup event sampling if using cross section.
        void setupEventSampling() {
            if (dynamic_cast<CrossSectionEventSampling*>(getEventSampling())) {
                auto sampling = dynamic_cast<CrossSectionEventSampling*>(getEventSampling());
                if (sampling->getParam() != 0.) {
                    // If param is not 0 then cross section was provided as param in the macro.
                    sampling->setCrossSection(sampling->getParam());
                    if (verbose_ > 1) {
                        std::cout << "LHEPrimaryGenerator: User specified cross section sigma = "
                                << sampling->getParam() << std::endl;
                    }
                } else {
                    // Cross section from the LHE file.
                    sampling->setCrossSection(reader_->getCrossSection());
                }
                // Calculate poisson mu from cross section.
                sampling->calculateMu();
                if (verbose_ > 1) {
                    std::cout << "LHEPrimaryGenerator: Calculated mu = " << sampling->getParam()
                            << " from cross-section " << reader_->getCrossSection() << std::endl;
                }
            }
        }

    private:

        /**
         * The LHE reader with the event data.
         */
        LHEReader* reader_;

        LHEEvent* lheEvent_;

};

}

#endif
