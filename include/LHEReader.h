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

        double getCrossSection() {
            return crossSection_;
        }

    private:

        void readCrossSection();

    private:

        double crossSection_{0};

        /**
         * The input file stream.
         */
        std::ifstream ifs_;
};

}

#endif
