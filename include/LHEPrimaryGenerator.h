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

        bool isFileBased() {
            return true;
        }

        bool supportsRandomAccess() {
            return true;
        }

        int getNumEvents() {
            return events_.size();
        }

        void readNextEvent() throw(EndOfFileException) {
            lheEvent_ = reader_->readNextEvent();
            if (!lheEvent_) {
                throw EndOfFileException();
            }
        }

        void readEvent(long index, bool removeEvent) throw(NoSuchRecordException) {
            // TODO: check validity of index
            lheEvent_ = events_[index];
            if (removeEvent) {
                events_.erase(events_.begin() + index);
            }
        }

        void openFile(std::string file) {

            // Cleanup the prior reader.
            if (reader_) {
                reader_->close();
                delete reader_;
            }

            // Create reader for next file.
            reader_ = new LHEReader(file);

            // Setup event sampling if using cross section.
            setupEventSampling();
        }

        void cacheEvents() {

            // Clear record cache.
            if (events_.size()) {
                for (auto event : events_) {
                    delete event;
                }
            }

            LHEEvent* event = reader_->readNextEvent();
            while (event != nullptr) {
                events_.push_back(event);
                event = reader_->readNextEvent();
            }
            
            if (verbose_ > 1) {
                std::cout << "LHEPrimaryGenerator: Cached " << events_.size() << " LHE events for random access" << std::endl;
            }
        }

        void deleteEvent() {
            if (lheEvent_) {
                std::cout << "LHEPrimaryGenerator: Deleting LHE event" << std::endl;
                delete lheEvent_;
                lheEvent_ = nullptr;
            }
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
                    std::cout << "LHEPrimaryGenerator: Calculated mu of " << sampling->getParam()
                            << " from cross-section " << reader_->getCrossSection() << std::endl;
                }
            }
        }

    private:

        /** The LHE reader with the event data. */
        LHEReader* reader_;

        /** The current LHE event. */
        LHEEvent* lheEvent_;

        /** Queue of LHE events when running in random mode. */
        std::vector<LHEEvent*> events_;
};

}

#endif
