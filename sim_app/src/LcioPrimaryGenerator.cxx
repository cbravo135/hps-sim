#include "LcioPrimaryGenerator.h"

namespace hpssim {

LcioPrimaryGenerator::LcioPrimaryGenerator(std::string name) :
        PrimaryGenerator(name) {
}

LcioPrimaryGenerator::~LcioPrimaryGenerator() {
    if (reader_) {
        delete reader_;
    }
}

void LcioPrimaryGenerator::GeneratePrimaryVertex(G4Event* anEvent) {
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
                primaryParticle->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(pid));
                primaryParticle->Set4Momentum(p[0] * GeV, p[1] * GeV, p[2] * GeV, energy);
                if (verbose_ > 3) {
                    std::cout << "LcioPrimaryGenerator: Created primary with PID " << pid << " and momentum "
                            << primaryParticle->GetMomentum() << " and energy " << primaryParticle->GetTotalEnergy()
                            << std::endl;
                }
                G4PrimaryVertex* vertex = nullptr;
                if (!particle->getParents().size()) {
                    vertex = new G4PrimaryVertex();
                    auto origin = particle->getVertex();
                    vertex->SetPosition(origin[0] * mm, origin[1] * mm, origin[2] * mm);
                    vertex->SetPrimary(primaryParticle);
                    anEvent->AddPrimaryVertex(vertex);
                    if (verbose_ > 3) {
                        std::cout << "LcioPrimaryGenerator: Added vertex at " << vertex->GetPosition() << std::endl;
                    }
                } else {
                    EVENT::MCParticle* mcpParent = particle->getParents()[0];
                    if (!mcpParent) {
                        G4Exception("", "", FatalException, "Failed to find MCParticle parent.");
                    }
                    G4PrimaryParticle* primaryParent = particleMap[mcpParent];
                    if (primaryParent) {
                        primaryParent->SetDaughter(primaryParticle);
                        double properTime = fabs((particle->getTime() - mcpParent->getTime()) * mcpParent->getMass())
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

bool LcioPrimaryGenerator::isFileBased() {
    return true;
}

bool LcioPrimaryGenerator::supportsRandomAccess() {
    return true;
}

void LcioPrimaryGenerator::readEvent(long index, bool removeEvent) throw (NoSuchRecordException) {
    // TODO: check validity of index
    long eventNumber = events_[index];
    lcEvent_ = reader_->readEvent(runHeader_->getRunNumber(), eventNumber);
    if (removeEvent) {
        events_.erase(events_.begin() + index);
    }
}

void LcioPrimaryGenerator::readNextEvent() throw (EndOfFileException) {
    lcEvent_ = reader_->readNextEvent();
}

void LcioPrimaryGenerator::openFile(std::string file) {
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

int LcioPrimaryGenerator::getNumEvents() {
    return events_.size();
}

void LcioPrimaryGenerator::cacheEvents() {

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

} // namespace hpssim
