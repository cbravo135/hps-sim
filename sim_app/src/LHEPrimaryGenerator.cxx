#include "LHEPrimaryGenerator.h"

// Geant4
#include "G4Event.hh"
#include "G4IonTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnknownParticle.hh"

#include "UserPrimaryParticleInformation.h"

namespace hpssim {

LHEPrimaryGenerator::LHEPrimaryGenerator(std::string name, LHEReader* theReader) :
        PrimaryGenerator(name), reader_(theReader), lheEvent_(nullptr) {
}

LHEPrimaryGenerator::~LHEPrimaryGenerator() {
    if (reader_) {
        delete reader_;
    }
}

void LHEPrimaryGenerator::GeneratePrimaryVertex(G4Event* anEvent) {

    auto tbl = G4ParticleTable::GetParticleTable();

    G4PrimaryVertex* vertex = new G4PrimaryVertex();
    vertex->SetPosition(0, 0, 0);
    vertex->SetWeight(lheEvent_->getXWGTUP());

    std::map<LHEParticle*, G4PrimaryParticle*> particleMap;

    int particleIndex = 0;
    const std::vector<LHEParticle*>& particles = lheEvent_->getParticles();
    for (std::vector<LHEParticle*>::const_iterator it = particles.begin(); it != particles.end(); it++) {

        LHEParticle* particle = (*it);
        if( verbose_ > 1) particle->print(std::cout);

        int idup = particle->getIDUP();

        // Change bad generator IDs to valid PDG codes.
        if (idup == 611) {
            idup = 11;
        } else if (idup == -611) {
            idup = -11;
        }

        if (idup > 0) {

            G4PrimaryParticle* primary = new G4PrimaryParticle();
            if (idup == -623) { /* Tungsten ion */
                G4ParticleDefinition* tungstenIonDef = G4IonTable::GetIonTable()->GetIon(74, 184, 0.);
                if (tungstenIonDef != NULL) {
                    primary->SetParticleDefinition(tungstenIonDef);
                } else {
                    G4Exception("LHEPrimaryGenerator::GeneratePrimaryVertex", "EventGenerationError", FatalException,
                            "Failed to find particle definition for W ion.");
                }
            } else {
                auto pdef = tbl->FindParticle(idup);
                if (pdef) {
                    primary->SetParticleDefinition(pdef);
                } else{
                    primary->SetParticleDefinition(G4UnknownParticle::Definition());
                }
            }

            primary->Set4Momentum(particle->getPUP(0) * GeV, particle->getPUP(1) * GeV, particle->getPUP(2) * GeV,
                    particle->getPUP(3) * GeV);
            primary->SetProperTime(particle->getVTIMUP() * nanosecond);

            UserPrimaryParticleInformation* primaryInfo = new UserPrimaryParticleInformation();
            primaryInfo->setGenStatus(particle->getISTUP());
            primary->SetUserInformation(primaryInfo);

            particleMap[particle] = primary;

            /*
             * Assign primary as daughter but only if the mother is not a DOC particle.
             */
            if (particle->getMother(0) != NULL && particle->getMother(0)->getISTUP() > 0) {
                G4PrimaryParticle* primaryMom = particleMap[particle->getMother(0)];
                if (primaryMom != NULL) {
                    primaryMom->SetDaughter(primary);
                }
            } else {
                vertex->SetPrimary(primary);
            }

            if( verbose_ > 1) primary->Print();

        } else {
        }

        if(verbose_ > 1) std::cout << std::endl;

        ++particleIndex;
    }

    if(verbose_ > 1) vertex->Print();
    anEvent->AddPrimaryVertex(vertex);
}

LHEPrimaryGenerator::LHEPrimaryGenerator(std::string name) :
        PrimaryGenerator(name) {
    reader_ = nullptr;
    lheEvent_ = nullptr;
}

bool LHEPrimaryGenerator::isFileBased() {
    return true;
}

bool LHEPrimaryGenerator::supportsRandomAccess() {
    return true;
}

int LHEPrimaryGenerator::getNumEvents() {
    return events_.size();
}

void LHEPrimaryGenerator::readNextEvent() throw(EndOfFileException) {
    lheEvent_ = reader_->readNextEvent();
    if (!lheEvent_) {
        throw EndOfFileException();
    }
}

void LHEPrimaryGenerator::readEvent(long index, bool removeEvent) throw(NoSuchRecordException) {
    // TODO: check validity of index
    lheEvent_ = events_[index];
    if (removeEvent) {
        events_.erase(events_.begin() + index);
    }
}

void LHEPrimaryGenerator::openFile(std::string file) {

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

void LHEPrimaryGenerator::cacheEvents() {

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

void LHEPrimaryGenerator::deleteEvent() {
    if (lheEvent_) {
        if( verbose_ > 1) std::cout << "LHEPrimaryGenerator: Deleting LHE event" << std::endl;
        delete lheEvent_;
        lheEvent_ = nullptr;
    }
}

void LHEPrimaryGenerator::setupEventSampling() {
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

} // namespace hpssim
