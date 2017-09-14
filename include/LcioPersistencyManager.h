#ifndef HPSSIM_LCIOPERSISTENCYMANAGER_H_
#define HPSSIM_LCIOPERSISTENCYMANAGER_H_

#include "lcdd/core/LCDDProcessor.hh"
#include "lcdd/hits/CalorimeterHit.hh"
#include "lcdd/hits/TrackerHit.hh"

#include "G4PersistencyManager.hh"
#include "G4PersistencyCenter.hh"
#include "G4Run.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"

#include "IO/LCWriter.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/MCParticleImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/SimTrackerHitImpl.h"
#include "EVENT/LCIO.h"
#include "IOIMPL/LCFactory.h"

#include "MCParticleBuilder.h"

using IMPL::LCCollectionVec;
using EVENT::LCIO;

namespace hpssim {

class LcioPersistencyManager : public G4PersistencyManager {

    public:

        LcioPersistencyManager() :
                G4PersistencyManager(G4PersistencyCenter::GetPersistencyCenter(), "LcioPersistencyManager") {
            G4PersistencyCenter::GetPersistencyCenter()->RegisterPersistencyManager(this);
            G4PersistencyCenter::GetPersistencyCenter()->SetPersistencyManager(this, "LcioPersistencyManager");
            writer_ = nullptr;
            builder_ = new MCParticleBuilder();
        }

        virtual ~LcioPersistencyManager() {
            if (writer_) {
                delete writer_;
            }
        }

        static LcioPersistencyManager* getInstance() {
            return (LcioPersistencyManager*) G4PersistencyCenter::GetPersistencyCenter()->CurrentPersistencyManager();
        }

        G4bool Store(const G4Event* anEvent) {

            std::cout << "LcioPersistencyManager: store event " << anEvent->GetEventID() << std::endl;

            IMPL::LCEventImpl* lcioEvent = new IMPL::LCEventImpl();
            lcioEvent->setEventNumber(anEvent->GetEventID());
            lcioEvent->setRunNumber(G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID());
            lcioEvent->setDetectorName(LCDDProcessor::instance()->getDetectorName());

            // write MCParticles to LCIO event
            IMPL::LCCollectionVec* mcpColl = builder_->buildMCParticleColl(anEvent);
            lcioEvent->addCollection(mcpColl, EVENT::LCIO::MCPARTICLE);

            // write hits collections to LCIO event
            writeHitsCollections(anEvent, lcioEvent);

            delete lcioEvent;

            return true;
        }

        // end of run?
        G4bool Store(const G4Run* aRun) {
            std::cout << "LcioPersistencyManager: store run " << aRun->GetRunID() << std::endl;

            writer_->close();

            return true;
        }

        G4bool Store(const G4VPhysicalVolume*) {
            return false;
        }

        // start run
        void Initialize() {

            std::cout << "LcioPersistencyManager: initialize" << std::endl;

            writer_ = IOIMPL::LCFactory::getInstance()->createLCWriter();
            writer_->open(outputFile_);

            auto runHeader = new IMPL::LCRunHeaderImpl();
            runHeader->setDetectorName(LCDDProcessor::instance()->getDetectorName());
            runHeader->setRunNumber(G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID());
            runHeader->setDescription("HPS MC events");

            writer_->writeRunHeader(static_cast<EVENT::LCRunHeader*>(runHeader));
        }

        void setOutputFile(std::string outputFile) {
            outputFile_ = outputFile;
        }

    private:

        void writeHitsCollections(const G4Event* g4Event, IMPL::LCEventImpl* lcioEvent) {

            G4HCofThisEvent* hce = g4Event->GetHCofThisEvent();
            int nColl = hce->GetNumberOfCollections();

            for (int iColl = 0; iColl < nColl; iColl++) {

                G4VHitsCollection* hc = hce->GetHC(iColl);
                std::string collName = hc->GetName();
                LCCollectionVec* collVec = nullptr;

                if (dynamic_cast<TrackerHitsCollection*>(hc)) {
                    auto trackerHits = dynamic_cast<TrackerHitsCollection*>(hc);
                    collVec = new LCCollectionVec(LCIO::SIMTRACKERHIT);
                    int nhits = trackerHits->GetSize();
                    std::cout << "LcioPersistencyManager: converting " << nhits << " tracker hits to LCIO" << std::endl;
                    for (int i = 0; i < nhits; i++) {

                        auto trackerHit = static_cast<TrackerHit*>(trackerHits->GetHit(i));
                        auto simTrackerHit = new IMPL::SimTrackerHitImpl();

                        // position in mm
                        const G4ThreeVector hitPos = trackerHit->getPosition();
                        double pos[3] = {hitPos.x(), hitPos.y(), hitPos.z()};
                        simTrackerHit->setPosition(pos);

                        // momentum in GeV
                        const G4ThreeVector& momentum = trackerHit->getMomentum();
                        simTrackerHit->setMomentum(momentum.x() / GeV, momentum.y() / GeV, momentum.z() / GeV);

                        // pathLength = distance between exit and entry points in mm
                        simTrackerHit->setPathLength(trackerHit->getLength());

                        // dEdx in GeV (LCIO units)
                        float edep = trackerHit->getEdep();
                        simTrackerHit->setEDep(edep / GeV);

                        // time in NS
                        float tEdep = trackerHit->getTdep();
                        simTrackerHit->setTime(tEdep);

                        // Cell ID.
                        simTrackerHit->setCellID0(trackerHit->getId());
                        collVec->push_back(simTrackerHit);

                        // get the MCParticle for the hit
                        std::cout << "LcioPersistencyManager: looking for track ID " << trackerHit->getTrackID() << std::endl;
                        IMPL::MCParticleImpl* mcp = builder_->findMCParticle(trackerHit->getTrackID());
                        if (!mcp) {
                            std::cout << "LcioPersistencyManager: No MCParticle found for trackID "
                                    << trackerHit->getTrackID() << " from sim tracker hit" << std::endl;
                            G4Exception("LcioPersistencyManager::writeHitsCollections", "",
                                    FatalException, "MCParticle for track ID is missing.");
                         } else {
                             simTrackerHit->setMCParticle(mcp);
                         }
                    }
                } else if (dynamic_cast<CalorimeterHitsCollection*>(hc)) {
                    auto calHits = dynamic_cast<CalorimeterHitsCollection*>(hc);
                    collVec = new LCCollectionVec(LCIO::SIMCALORIMETERHIT);
                    int nhits = calHits->GetSize();
                    std::cout << "LcioPersistencyManager: converting " << nhits << " cal hits to LCIO" << std::endl;
                    for (int i = 0; i < nhits; i++) {

                        auto calHit = static_cast<CalorimeterHit*>(calHits->GetHit(i));
                        auto simCalHit = new IMPL::SimCalorimeterHitImpl();

                        // set cellid from cal hit's id64
                        const Id64bit& id64 = calHit->getId64bit();
                        simCalHit->setCellID0(id64.getId0());
                        simCalHit->setCellID1(id64.getId1());

                        // position
                        const G4ThreeVector hitPos = calHit->getPosition();
                        float pos[3] = {(float)hitPos.x(), (float)hitPos.y(), (float)hitPos.z()};
                        simCalHit->setPosition(pos);

                        // energy
                        simCalHit->setEnergy(calHit->getEdep());

                        // add to output collection
                        collVec->push_back(simCalHit);

                        auto contribs = calHit->getHitContributions();
                        for (auto contrib : contribs) {
                            auto edep = contrib.getEdep();
                            auto hitTime = contrib.getGlobalTime();
                            auto pdg = contrib.getPDGID();
                            auto pos = contrib.getPosition();
                            auto trackID = contrib.getTrackID();

                            if (trackID <= 0) {
                                std::cout << "LcioPersistencyManager: Bad trackID " << trackID << " for cal hit contrib" << std::endl;
                                G4Exception("LcioPersistencyManager::writeHitsCollections", "",
                                        FatalException, "Bad track ID in cal hit contribution.");
                            }

                            auto mcp = builder_->findMCParticle(trackID);

                            if (!mcp) {
                                std::cout << "LcioPersistencyManager: No MCParticle found for track ID " << trackID << std::endl;
                                G4Exception("LcioPersistencyManager::writeHitsCollections", "",
                                        FatalException, "No MCParticle found for track ID.");
                            }

                            simCalHit->addMCParticleContribution(static_cast<EVENT::MCParticle*>(mcp), (float)edep, (float)hitTime, (int)pdg, (float*)pos);

                            std::cout << "LcioPersistencyManager: assigned cal hit contrib with "
                                    << "trackID = " << trackID << "; "
                                    << "edep = " << edep << "; "
                                    << "time = " << hitTime << "; "
                                    << "pdg = " << pdg << "; "
                                    << "pos = ( " << pos[0] << ", " << pos[1] << ", " << pos[2] << ") "
                                    << std::endl;
                        }
                    }
                }

                if (collVec) {
                    lcioEvent->addCollection(collVec, collName);
                    std::cout << "LcioPersistencyManager: stored " << collVec->size() << " hits in " << collName << std::endl;
                }
            }

            writer_->writeEvent(static_cast<EVENT::LCEvent*>(lcioEvent));
            writer_->flush();
        }

    private:

        std::string outputFile_{"hps_sim_events.slcio"};

        IO::LCWriter* writer_;

        std::string detectorName_{"DUMMY"};

        MCParticleBuilder* builder_;
};


}

#endif
