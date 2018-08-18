/**
 * @file LHEPrimaryGenerator.h
 * @brief Class for generating a Geant4 event from LHE event data
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef HPSSIM_STDHEPPRIMARYGENERATOR_H_
#define HPSSIM_STDHEPPRIMARYGENERATOR_H_

#include "G4RunManager.hh"
#include "G4VPrimaryGenerator.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "lStdHep.h"
#include "StdHepParticle.h"
#include "PrimaryGenerator.h"

#include <vector>

namespace hpssim {

/**
 * @class STDHEPPrimaryGenerator
 * @brief Generates a Geant4 event from StdHep data
 */
class StdHepPrimaryGenerator : public PrimaryGenerator {

    public:

        StdHepPrimaryGenerator(std::string name) : PrimaryGenerator(name) {
        }

        virtual ~StdHepPrimaryGenerator() {
            if (reader_) {
                delete reader_;
            }
        }

        void GeneratePrimaryVertex(G4Event* anEvent) {

            if (verbose_ > 1) {
                std::cout << "StdHepPrimaryGenerator: Generate event " << anEvent->GetEventID() << std::endl;
            }

            /*
             * Create a vector with the track data.
             */
            std::vector<StdHepParticle> particles;
            int nTracks = stdEvent_.nTracks();
            for (int iTrack = 0; iTrack < nTracks; iTrack++) {
                lStdTrack& track = stdEvent_[iTrack];
                particles.push_back(StdHepParticle(track));
            }

            if (verbose_ > 1) {
                std::cout << "StdHepPrimaryGenerator: Read " << particles.size() << " StdHep tracks" << std::endl;
            }

            /*
             * Assign mother and daughter particles.
             */
            for (auto particle : particles) {

                const lStdTrack& data = particle.getData();

                //std::cout << "mom1, mom2, dau1, dau2: " << data.daughter1 << " " << data.daughter2 << " "
                //        << data.mother1 << " " << data.mother2 << std::endl;

                if (data.daughter1) {
                    long idau1 = data.daughter1 % 10000 - 1;
                    particle.setDaughter(0, &particles[idau1]);
                }

                if (data.daughter2) {
                    long idau2 = data.daughter2 % 10000 - 1;
                    particle.setDaughter(1, &particles[idau2]);
                }

                if (data.mother1) {
                    long imom1 = data.mother1 % 10000 - 1;
                    particle.setMother(0, &particles[imom1]);
                }

                if (data.mother2) {
                    long imom2 = data.mother2  % 10000 - 1;
                    particle.setMother(1, &particles[imom2]);
                }
            }

            /*
             * Main loop to generate the Geant4 primaries and vertices from the track data.
             */
            std::map<StdHepParticle*, G4PrimaryParticle*> particleMap;
            G4PrimaryVertex* vertex = nullptr;
            for (std::vector<StdHepParticle>::iterator it = particles.begin(); it != particles.end(); it++) {

                /*
                 * Get particle to generate, its data, and parentage info.
                 */
                StdHepParticle* particle = &(*it);
                const lStdTrack& data = particle->getData();
                StdHepParticle* mom = particle->getMother(0);
                StdHepParticle* dau1 = particle->getDaughter(0);

                /*
                 * Create a new primary particle for this track.
                 */
                G4PrimaryParticle* primary = new G4PrimaryParticle();
                primary->SetPDGcode(data.pid);
                primary->Set4Momentum(data.Px * GeV, data.Py * GeV, data.Pz * GeV, data.E * GeV);

                if (verbose_ > 3) {
                    std::cout << "StdHepPrimaryGenerator: Creating primary with PDG ID " << data.pid << " and four-momentum: "
                            << data.Px * GeV << " " << data.Py * GeV << " " << data.Pz * GeV << " " << data.E * GeV
                            << " [GeV]" << std::endl;
                }
                particleMap[particle] = primary;

                if (!mom) {
                    /*
                     * This is a primary without a mother particle which needs its own vertex position.
                     */
                    if (verbose_ > 3) {
                        std::cout << "StdHepPrimaryGenerator: Creating new vertex at ( "
                                << data.X << ", " << data.Y << ", " << data.Z << " )" << std::endl;
                    }
                    vertex = new G4PrimaryVertex();
                    vertex->SetPosition(data.X, data.Y, data.Z);
                    anEvent->AddPrimaryVertex(vertex);
                    vertex->SetPrimary(primary);
                } else {
                    /*
                     * This is a daughter primary that needs to be assigned to a parent.
                     */
                    G4PrimaryParticle* primaryMom = particleMap[particle->getMother(0)];
                    if (primaryMom) {
                        primaryMom->SetDaughter(primary);
                    } else {
                        // This should never happen but if it does we need to bail.
                        G4Exception("", "", FatalException, "Missing expected primary mother particle!");
                    }
                }

                // Copied from the MCParticleManager in SLIC and LCIO's LCStdHepRdr class.
                // TODO: This needs to be double checked!
                if (dau1) {
                    const lStdTrack& dauData = dau1->getData();
                    double properTime = fabs(((dauData.T / c_light) - (data.T / c_light))
                            * data.M) / data.E;
                    //std::cout << "StdHepPrimaryGenerator: Setting proper time " << properTime << " on particle." << std::endl;
                    primary->SetProperTime(properTime * ns);
                }
            }
        }

        bool isFileBased() {
            return true;
        }

        bool supportsRandomAccess() {
            return true;
        }

        int getNumEvents() {
            return records_.size();
        }

        /**
         * Cache a list of events for random access.
         * The StdHep data is copied so pointers are not used in the event vector.
         * Memory will be reclaimed when the vector is cleared.
         */
        void cacheEvents() {

          
          if (verbose_ > 1) {
            std::cout << "StdHepPrimaryGenerator::cacheEvents -- Start caching events. " << std::endl;
          }
            // Clear record cache.
            if (records_.size()) {
                records_.clear();
            }

            // Cache a list of StdHep events.
            while (true) {
                lStdEvent lse;
                long res = reader_->readEvent(lse);
                if (res == LSH_ENDOFFILE) {
                    break;
                } else if (res) {
                    std::cerr << "StdHepPrimaryGenerator: Got non-zero LSH error code " << res << std::endl;
                    G4Exception("", "", FatalException, "Error reading StdHep file.");
                }
                records_.push_back(lse);
            }

            if (verbose_ > 1) {
                std::cout << "StdHepPrimaryGenerator: Cached " << records_.size() << " records for random access" << std::endl;
            }
        }

        void readNextEvent() throw(EndOfFileException) {
            long res = reader_->readEvent(stdEvent_);
            if (res == LSH_ENDOFFILE) {
                throw EndOfFileException();
            } else if (res) {
                std::cerr << "StdHepPrimaryGenerator: Got non-zero LSH error code " << res << std::endl;
                G4Exception("", "", FatalException, "Fatal error reading next StdHep event.");
            }
        }

        void openFile(std::string file) {

            // Cleanup the prior reader.
            if (reader_) {
                delete reader_;
                reader_ = nullptr;
            }

            // Create reader for next file.
            reader_ = new lStdHep(file.c_str());
        }

        void readEvent(long index, bool removeEvent) throw(NoSuchRecordException) {
            // TODO: check validity of index
            stdEvent_ = records_[index];
            if (removeEvent) {
              std::cerr << "Erasing from a vector is a really bad idea. See: http://www.cplusplus.com/reference/vector/vector/erase/" << std::endl;
              records_.erase(records_.begin() + index);
            }
        }

    private:

        lStdHep* reader_{nullptr};
        lStdEvent stdEvent_;

        std::vector<lStdEvent> records_;
};

}

#endif
