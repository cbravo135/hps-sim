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

#include <set>

namespace hpssim {

/**
 * @class LHEPrimaryGenerator
 * @brief Generates a Geant4 event from an LCIO MCParticle collection
 *
 * @note It appears that the current incarnation of the SIOReader is 'clever'
 * about how it manages the LCEvent objects, so deleting them explicitly causes
 * seg faults!  For this reason, none of the events read from the file are
 * deleted.  This does not appear to cause a memory leak.
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
         * Generate vertices in the Geant4 event from the MCParticle data.
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
        }

        bool isFileBased() {
            return true;
        }

        bool supportsRandomAccess() {
            return true;
        }

        /**
         * Read an event by index for random access.
         * This method uses the cache of event numbers to find the event in the file
         * for that index, and then removes that record from the cache so it
         * is not reused.
         */
        void readEvent(long index, bool removeEvent) throw(NoSuchRecordException) {
            // TODO: check validity of index
            long eventNumber = events_[index];
            lcEvent_ = reader_->readEvent(runHeader_->getRunNumber(), eventNumber);
            if (removeEvent) {
                events_.erase(events_.begin() + index);
            }
        }

        /**
         * Read the next event sequentially from the SIO reader.
         */
        void readNextEvent() throw(EndOfFileException) {
            lcEvent_ = reader_->readNextEvent();
        }

        /**
         * Open a new reader, deleting the old one if necessary, and get the first run header
         * from the file.  If there is no run header found, there will be a fatal exception,
         * because this is required for random access support.
         */
        void openFile(std::string file) {
            if (reader_) {
                reader_->close();
                delete reader_;
            }
            reader_ = IOIMPL::LCFactory::getInstance()->createLCReader(IO::LCReader::directAccess);
            reader_->open(file);
            runHeader_ = reader_->readNextRunHeader(); // FIXME: Hope there isn't more than one of these in the file!
            if (!runHeader_) {
                G4Exception("", "", FatalException, G4String("Failed to read run header from LCIO file '" + file + "'"));
            }
        }

        /**
         * Return the number of events left in the cache for random access.
         */
        int getNumEvents() {
            return events_.size();
        }

        /**
         * Cache a list of valid event numbers in the file that can be used when running in random access mode.
         */
        void cacheEvents() {

            if (events_.size()) {
                events_.clear();
            }

            // Create a list of event numbers in the file to be used for random access.
            EVENT::LCEvent* event = reader_->readNextEvent();
            while (event) {
                events_.push_back(event->getEventNumber());
                event = reader_->readNextEvent();
            }

            if (verbose_ > 1) {
                std::cout << "LcioPrimaryGenerator: Cached " << events_.size() << " events for random access" << std::endl;
            }
        }

    private:

        /** The LCIO reader with the event data. */
        IO::LCReader* reader_{nullptr};

        /** The current LC event. */
        EVENT::LCEvent* lcEvent_{nullptr};

        /** The current run header. */
        EVENT::LCRunHeader* runHeader_{nullptr};

        /** List of event indices that is used for random access via the reader. */
        std::vector<long> events_;
};

}

#endif
