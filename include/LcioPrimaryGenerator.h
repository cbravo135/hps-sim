/**
 * @file LHEPrimaryGenerator.h
 * @brief Class for generating a Geant4 event from LHE event data
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef HPSSIM_LCIOPRIMARYGENERATOR_H_
#define HPSSIM_LCIOPRIMARYGENERATOR_H_

#include "G4RunManager.hh"
#include "G4VPrimaryGenerator.hh"

#include "EVENT/LCCollection.h"
#include "EVENT/MCParticle.h"
#include "IO/LCReader.h"
#include "IOIMPL/LCFactory.h"

#include "PrimaryGenerator.h"

namespace hpssim {

/**
 * @class LHEPrimaryGenerator
 * @brief Generates a Geant4 event from an LHEEvent
 * @note The readNextEvent() hook does not need to be implemented because the LCIO
 * reader can accept a list of files to process.
 */
class LcioPrimaryGenerator : public PrimaryGenerator {

    public:

        LcioPrimaryGenerator(std::string name) : PrimaryGenerator(name) {
        }

        virtual ~LcioPrimaryGenerator() {
            if (reader_) {
                delete reader_;
            }
        }

        /**
         * Generate vertices in the Geant4 event.
         * @param anEvent The Geant4 event.
         */
        void GeneratePrimaryVertex(G4Event* anEvent) {
            auto particleColl = lcEvent_->getCollection("MCParticle");
            std::map<EVENT::MCParticle*, G4PrimaryParticle*> particleMap;
            if (verbose_ > 1) {
                std::cout << "LcioPrimaryGenerator: Generating event from " << particleColl->getNumberOfElements()
                        << " particles" << std::endl;
            }
            if (particleColl->getNumberOfElements()) {
                for (int i = 0; i < particleColl->getNumberOfElements(); ++i) {
                    auto particle = static_cast<EVENT::MCParticle*>(particleColl->getElementAt(i));
                    if (particle->getGeneratorStatus() || particle->getParents().size() == 0) {
                        int pid = particle->getPDG();
                        double energy = particle->getEnergy() * GeV;
                        auto p = particle->getMomentum();
                        G4PrimaryParticle* primaryParticle = new G4PrimaryParticle();
                        primaryParticle->SetParticleDefinition(
                                G4ParticleTable::GetParticleTable()->FindParticle(pid));
                        primaryParticle->Set4Momentum(p[0] * GeV, p[1] * GeV, p[2] * GeV, energy);
                        if (verbose_ > 3) {
                            std::cout << "LcioPrimaryGenerator: Created primary with PID " << pid
                                    << " and momentum " << primaryParticle->GetMomentum() << " and energy "
                                    << primaryParticle->GetTotalEnergy() << std::endl;
                        }
                        G4PrimaryVertex* vertex = nullptr;
                        if (!particle->getParents().size()) {
                            vertex = new G4PrimaryVertex();
                            auto origin = particle->getVertex();
                            vertex->SetPosition(origin[0] * mm, origin[1] * mm, origin[2] * mm);
                            vertex->SetPrimary(primaryParticle);
                            anEvent->AddPrimaryVertex(vertex);
                            if (verbose_ > 3) {
                                std::cout << "LcioPrimaryGenerator: Added vertex at " << vertex->GetPosition()
                                        << std::endl;
                            }
                        } else {
                            EVENT::MCParticle* mcpParent = particle->getParents()[0];
                            if (!mcpParent) {
                                G4Exception("", "", FatalException, "Failed to find MCParticle parent.");
                            }
                            G4PrimaryParticle* primaryParent = particleMap[mcpParent];
                            if (primaryParent) {
                                primaryParent->SetDaughter(primaryParticle);
                                double properTime = fabs(
                                        (particle->getTime() - mcpParent->getTime()) * mcpParent->getMass())
                                        / mcpParent->getEnergy();
                                primaryParent->SetProperTime(properTime * ns);
                            } else {
                                G4Exception("", "", FatalException, "Failed to find primary particle parent.");
                            }
                        }
                        particleMap[particle] = primaryParticle;
                    }
                }
            } 
            if (getReadMode() != PrimaryGenerator::Random) {
                delete lcEvent_;
            }
        }

        bool supportsRandomAccess() {
            return true;
        }

        void readEvent(long index) {
            lcEvent_ = reader_->readEvent(runHeader_->getRunNumber(), index);
        }

        int getNumEvents() {
            return reader_->getNumberOfEvents();
        }

        void readNextEvent() throw(EndOfFileException) {
            lcEvent_ = reader_->readNextEvent();
        }

        void openFile(std::string file) {
            if (reader_) {
                reader_->close();
                delete reader_;
            }
            reader_ = IOIMPL::LCFactory::getInstance()->createLCReader();
            reader_->open(file);
            runHeader_ = reader_->readNextRunHeader();
        }

    private:

        /** The LCIO reader with the event data. */
        IO::LCReader* reader_{nullptr};

        /** The current LC event. */
        EVENT::LCEvent* lcEvent_{nullptr};

        /** The current run header. */
        EVENT::LCRunHeader* runHeader_{nullptr};
};

}

#endif
