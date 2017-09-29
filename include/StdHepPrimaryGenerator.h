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
        }

        void GeneratePrimaryVertex(G4Event* anEvent) {

            //std::cout << "StdHepPrimaryGenerator: Generate event " << anEvent->GetEventID() << "." << std::endl;

            /*
             * Read the next Stdhep event.
             */
            lStdEvent stdEvent;
            // TODO: check result from reading to see if no more events, etc.
            int res = reader_->readEvent(stdEvent);
            //reader_->printEvent();

            /*
             * Create a vector with the track data.
             */
            std::vector<StdHepParticle*> particles;
            int nTracks = reader_->nTracks();
            for (int iTrack = 0; iTrack < nTracks; iTrack++) {
                //reader_->printTrack(iTrack);
                lStdTrack* track = &stdEvent[iTrack];
                particles.push_back(new StdHepParticle(track));
            }

            /*
             * Assign mother and daughter particles.
             */
            for (auto particle : particles) {

                const StdHepParticle::Data& data = particle->getData();

                //std::cout << "mom1, mom2, dau1, dau2: " << data.daughter1 << " " << data.daughter2 << " "
                //        << data.mother1 << " " << data.mother2 << std::endl;

                if (data.daughter1) {
                    long idau1 = data.daughter1 % 10000 - 1;
                    particle->setDaughter(0, particles[idau1]);
                    //std::cout << "StdHepPrimaryGenerator: Assigned daughter index " << idau1 << "." << std::endl;
                }

                if (data.daughter2) {
                    long idau2 = data.daughter2 % 10000 - 1;
                    particle->setDaughter(1, particles[idau2]);
                }

                if (data.mother1) {
                    long imom1 = data.mother1 % 10000 - 1;
                    particle->setMother(0, particles[imom1]);
                }

                if (data.mother2) {
                    long imom2 = data.mother2  % 10000 - 1;
                    particle->setMother(1, particles[imom2]);
                }
            }

            /*
             * Main loop to generate the Geant4 primaries and vertices from the track data.
             */
            std::map<StdHepParticle*, G4PrimaryParticle*> particleMap;
            G4PrimaryVertex* vertex = nullptr;
            for (std::vector<StdHepParticle*>::const_iterator it = particles.begin(); it != particles.end(); it++) {

                /*
                 * Get particle to generate, its data, and parentage info.
                 */
                StdHepParticle* particle = (*it);
                const StdHepParticle::Data& data = particle->getData();
                StdHepParticle* mom = particle->getMother(0);
                StdHepParticle* dau1 = particle->getDaughter(0);
                //StdHepParticle* dau2 = particle->getDaughter(1);

                /*
                 * Create a new primary particle for this track.
                 */
                G4PrimaryParticle* primary = new G4PrimaryParticle();
                primary->SetPDGcode(data.pid);
                primary->Set4Momentum(data.Px * GeV, data.Py * GeV, data.Pz * GeV, data.E * GeV);
                //std::cout << "StdHepPrimaryGenerator: Creating primary with PDG ID " << data.pid << " and four-momentum: "
                //        << data.Px * GeV << " " << data.Py * GeV << " " << data.Pz * GeV << " " << data.E * GeV
                //        << " [GeV]" << std::endl;
                particleMap[particle] = primary;

                if (!mom) {
                    /*
                     * This is a primary without a mother particle which needs its own vertex position.
                     */
                    //std::cout << "StdHepPrimaryGenerator: Creating new vertex at ( "
                    //        << data.x << ", " << data.y << ", " << data.z << " )." << std::endl;
                    vertex = new G4PrimaryVertex();
                    vertex->SetPosition(data.x, data.y, data.z);
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
                    const StdHepParticle::Data& dauData = dau1->getData();
                    double properTime = fabs(((dauData.T / c_light) - (data.T / c_light))
                            * data.M) / data.E;
                    //std::cout << "StdHepPrimaryGenerator: Setting proper time " << properTime << " on particle." << std::endl;
                    primary->SetProperTime(properTime * ns);
                }
            }

            // Cleanup StdHep particle list.
            for (auto particle : particles) {
                delete particle;
            }
            particles.clear();
        }

        void initialize() {
            std::cout << "StdHepPrimaryGenerator: Opening '" << files_[0] << "' for reading" << std::endl;
            reader_ = new lStdHep(files_[0].c_str());
        }

    private:

        lStdHep* reader_{nullptr};
};

}

#endif
