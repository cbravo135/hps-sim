#include "LHEReader.h"

// STL
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>

#include <stdexcept>

namespace hpssim {

LHEReader::LHEReader(std::string& filename) {
    std::cout << "LHEReader: Opening LHE file '" << filename << "'" << std::endl;
    ifs_.open(filename.c_str(), std::ifstream::in);

    // Read number of events from header.
    //std::cout << "LHEReader: Reading number of events ..." << std::endl;
    readNumEvents();

    // Read cross section from header.
    //std::cout << "LHEReader: Reading cross section ..." << std::endl;
    readCrossSection();

    std::cout << "LHEReader: Number of events " << numEvents_ << " from header data" << std::endl;

    //std::cout << "LHEReader: Done reading in LHE file '" << filename << "'" << std::endl;
}

LHEReader::~LHEReader() {
    ifs_.close();
}

/*
 * Get cross section from LHE file line which looks like:
 * @verbatim
 * #  Integrated weight (pb)  :  0.10715E+10
 * @endverbatim
*/
void LHEReader::readCrossSection() {
    std::string line;
    while (getline(ifs_, line)) {
        if (line.find("Integrated weight") !=std::string::npos) {
            std::stringstream ss(line);
            std::vector<std::string> tokens;
            std::string token;
            while (ss >> token) {
                tokens.push_back(token);
            }
            crossSection_ = atof(tokens[5].c_str());
        }
        if (line == "</MGGenerationInfo>") {
            break;
        }
    }
}

LHEEvent* LHEReader::readNextEvent() {

    std::string line;
    bool foundEventElement = false;
    while (getline(ifs_, line)) {
        if (line == "<event>") {
            foundEventElement = true;
            break;
        }
    }

    if (!foundEventElement) {
        // This probably just means that all events have been processed.
        return nullptr;
    }

    getline(ifs_, line);

    LHEEvent* nextEvent = new LHEEvent(line);

    while (getline(ifs_, line)) {

        if (line == "</event>") {
            break;
        }

        if (line[0] == '<') {
            // Ignore tags embedded in event block by MG5!
            std::cerr << "LHEReader: Ignoring garbage line \"" << line << "\" in input!" << std::endl;
        } else {
            LHEParticle* particle = new LHEParticle(line);
            nextEvent->addParticle(particle);
        }
    }

    const std::vector<LHEParticle*>& particles = nextEvent->getParticles();
    int particleIndex = 0;
    for (std::vector<LHEParticle*>::const_iterator it = particles.begin(); it != particles.end(); it++) {
        LHEParticle* particle = (*it);
        if (particle->getMOTHUP(0) != 0) {
            int mother1 = particle->getMOTHUP(0);
            int mother2 = particle->getMOTHUP(1);
            if (mother1 > 0) {
                particle->setMother(0, particles[mother1 - 1]);
            }
            if (mother2 > 0) {
                particle->setMother(1, particles[mother2 - 1]);
            }
        }
        ++particleIndex;
    }

    return nextEvent;
}

void LHEReader::readNumEvents() {
    std::string line;
    while (getline(ifs_, line)) {
        if (line.find("nevents") != std::string::npos) {
            std::stringstream ss(line);
            std::vector<std::string> tokens;
            std::string token;
            while (ss >> token) {
                tokens.push_back(token);
            }
            numEvents_ = atoi(tokens[0].c_str());
            break;
        }
        if (line == "</MGRunCard>") {
            break;
        }
    }
}

double LHEReader::getCrossSection() {
    return crossSection_;
}

int LHEReader::getNumEvents() {
    return numEvents_;
}

void LHEReader::close() {
    if (ifs_.is_open()) {
        ifs_.close();
    }
}

}
