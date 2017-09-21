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
 * @brief Generates a Geant4 event from an LHEEvent
 */
class StdHepPrimaryGenerator : public PrimaryGenerator {

    public:

        StdHepPrimaryGenerator(std::string name) : PrimaryGenerator(name) {
        }

        /**
         * Class destructor.
         */
        virtual ~StdHepPrimaryGenerator() {
        }

        void GeneratePrimaryVertex(G4Event* anEvent) {

            std::cout << "StdHepPrimaryGenerator: Generate event " << anEvent->GetEventID() << "." << std::endl;

            // Read the next StdHep event.
            lStdEvent stdEvent;
            int res = reader_->readEvent(stdEvent);
            reader_->printEvent();

            // Create a vector with the track data.
            std::vector<StdHepParticle*> particles;
            int nTracks = reader_->nTracks();
            for (int iTrack = 0; iTrack < nTracks; iTrack++) {
                reader_->printTrack(iTrack);
                lStdTrack* track = &stdEvent[iTrack];
                particles.push_back(new StdHepParticle(track));
            }

            // Assign particle parentage.
            for (auto particle : particles) {

                const StdHepParticle::Data& data = particle->getData();

                int dau1 = data.dau[0];
                int dau2 = data.dau[1];

                int mom1 = data.mom[0];
                int mom2 = data.mom[1];

                int idau1 = dau1 % 10000 - 1;
                int idau2 = dau2 % 10000 - 1;

                int imom1 = mom1  % 10000 - 1;
                int imom2 = mom2  % 10000 - 1;

                if (dau1) {
                    particle->setDaughter(0, particles[idau1]);
                }

                if (dau2) {
                    particle->setDaughter(1, particles[idau2]);
                }

                if (mom1) {
                    particle->setMother(0, particles[imom1]);
                }

                if (mom2) {
                    particle->setMother(1, particles[imom2]);
                }
            }

            std::map<StdHepParticle*, G4PrimaryParticle*> particleMap;
            G4PrimaryVertex* vertex = nullptr;
            for (std::vector<StdHepParticle*>::const_iterator it = particles.begin(); it != particles.end(); it++) {

                StdHepParticle* particle = (*it);
                const StdHepParticle::Data& data = particle->getData();
                StdHepParticle* mom = particle->getMother(0);
                StdHepParticle* dau1 = particle->getDaughter(0);
                StdHepParticle* dau2 = particle->getDaughter(1);

                double px = data.fourVec[0] * GeV;
                double py = data.fourVec[1] * GeV;
                double pz = data.fourVec[2] * GeV;
                double energy = data.fourVec[3] * GeV;
                double x = data.position[0];
                double y = data.position[1];
                double z = data.position[2];
                int pid = data.pid;

                G4PrimaryParticle* primary = new G4PrimaryParticle();
                primary->SetPDGcode(pid);
                primary->Set4Momentum(px * GeV, py * GeV, pz * GeV, energy * GeV);
                std::cout << "StdHepPrimaryGenerator: Creating primary with PDG ID " << pid << " and four-momentum: "
                        << px * GeV << " " << py * GeV << " " << pz * GeV << " " << energy * GeV
                        << " [GeV]" << std::endl;
                particleMap[particle] = primary;

                if (!mom) {
                    /**
                     * This is a primary without a mother particle which needs its own vertex position.
                     */
                    std::cout << "StdHepPrimaryGenerator: Creating new vertex at ( " << x << ", " << y << ", " << z << " )." << std::endl;
                    vertex = new G4PrimaryVertex();
                    vertex->SetPosition(x, y, z);
                    anEvent->AddPrimaryVertex(vertex);
                    vertex->SetPrimary(primary);
                } else {
                    /**
                     * This is a daughter primary that needs to be assigned to a parent.
                     */
                    G4PrimaryParticle* primaryMom = particleMap[particle->getMother(0)];
                    if (primaryMom) {
                        std::cout << "setting dau primary" << std::endl;
                        primaryMom->SetDaughter(primary);
                    } else {
                        throw std::runtime_error("Missing expected primary mother particle!");
                    }
                }

                // This code to set the decay times is copied from the MCParticleManager in SLIC.
                /*
                auto dau = particle->getDaughter(0);
                if (dau) {
                    double dauTime = dau->getStdTrack()->T;
                    double properTime = fabs(((dauTime / c_light) - (particle->getStdTrack()->T / c_light)) * particle->getStdTrack()->M) / particle->getStdTrack()->E;
                    primary->SetProperTime(properTime);
                }
                */
            }

            // Cleanup StdHep particle list.
            for (auto particle : particles) {
                delete particle;
            }
            particles.clear();
        }

        // FIXME: Needs to support multiple input files.
        void addFile(std::string file) {
            PrimaryGenerator::addFile(file);
            std::cout << "StdHepPrimaryGenerator: Setting file '" << file << "' on LHE reader." << std::endl;
            reader_ = new lStdHep(file.c_str());
        }

    private:

        lStdHep* reader_{nullptr};
};

}

#endif
