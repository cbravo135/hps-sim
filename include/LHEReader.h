/**
 * @file LHEReader.h
 * @brief Class for reading LHE event data
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef HPSSIM_LHEREADER_H_
#define HPSSIM_LHEREADER_H_

#include "LHEEvent.h"

#include <fstream>

namespace hpssim {

/**
 * @class LHEReader
 * @brief Reads LHE event data into an LHEEvent object
 */
class LHEReader {

    public:

        /**
         * Class constructor.
         * @param fileName The input file name.
         */
        LHEReader(std::string& fileName);

        /**
         * Class destructor.
         */
        virtual ~LHEReader();

        /**
         * Read the next event.
         * @return The next LHE event.
         */
        LHEEvent* readNextEvent();

        /**
         * Get the cross section for the file, read from header data.
         */
        double getCrossSection() {
            return crossSection_;
        }

        /**
         * Get the number of events, read from the header data.
         */
        int getNumEvents() {
            return numEvents_;
        }

        /**
         * Close the current file.
         */
        void close() {
            if (ifs_.is_open()) {
                ifs_.close();
            }
        }

    private:

        /**
         * Read cross section from header.
         */
        void readCrossSection();

        /**
         * Read number of events in file from header.
         */
        void readNumEvents();

    private:

        /** Cross section of physics process read from header. */
        double crossSection_{0};

        /** The input file stream. */
        std::ifstream ifs_;

        /** Number of events in the file. */
        int numEvents_{-1};
};

}

#endif
