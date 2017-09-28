#ifndef HPSSIM_MCPARTICLEBUILDER_H_
#define HPSSIM_MCPARTICLEBUILDER_H_

#include "Trajectory.h"
#include "UserTrackingAction.h"

#include "EVENT/LCIO.h"
#include "IMPL/MCParticleImpl.h"
#include "IMPL/LCCollectionVec.h"

#include "G4SystemOfUnits.hh"

#include <map>

namespace hpssim {

class MCParticleBuilder {

    public:

        typedef std::map<G4int, IMPL::MCParticleImpl*> MCParticleMap;

        MCParticleBuilder(TrackMap* trackMap) : trackMap_(trackMap) {
        }

        virtual ~MCParticleBuilder() {
        }

        void buildParticleMap(G4TrajectoryContainer* trajectories, IMPL::LCCollectionVec* collVec) {
            if (!trajectories) {
                G4Exception("", "", FatalException, "The trajectory container is null!");
            }
            particleMap_.clear();
            for (auto trajectory : *trajectories->GetVector()) {
                if (Trajectory::getTrajectory(trajectory)->getSaveFlag()) {
                    auto particle = new IMPL::MCParticleImpl;
                    collVec->addElement(particle);
                    particleMap_[trajectory->GetTrackID()] = particle;
                }
            }
            //std::cout << "MCParticleBuilder: created " << collVec->size() << " empty MCParticle objects" << std::endl;
        }

        IMPL::MCParticleImpl* findMCParticle(G4int trackID) {
            G4VTrajectory* traj = trackMap_->findTrajectory(trackID);
            if (traj != nullptr) {
                return particleMap_[traj->GetTrackID()];
            } else {
                return nullptr;
            }
        }

        void buildMCParticle(Trajectory* traj) {

            //std::cout << "MCParticleBuilder: building MCParticle for track " << traj->GetTrackID() << std::endl;

            IMPL::MCParticleImpl* p = particleMap_[traj->GetTrackID()];

            if (!p) {
                std::cerr << "MCParticleBuilder: MCParticle not found for track ID " << traj->GetTrackID() << std::endl;
                G4Exception("SimParticleBuilder::buildSimParticle", "", FatalException, "MCParticle not found for Trajectory.");
            }

            p->setGeneratorStatus(traj->getGenStatus());
            p->setPDG(traj->GetPDGEncoding());
            p->setCharge(traj->GetCharge());
            p->setMass(traj->getMass());
            //p->setEnergy(traj->getEnergy());
            p->setTime(traj->getGlobalTime());

            double vertexArr[] = {traj->getVertexPosition()[0], traj->getVertexPosition()[1], traj->getVertexPosition()[2]};
            p->setVertex(vertexArr);

            double momentum[] = {traj->GetInitialMomentum()[0] / GeV, traj->GetInitialMomentum()[1] / GeV, traj->GetInitialMomentum()[2] / GeV};
            p->setMomentum(momentum);

            //const G4ThreeVector& endpMomentum = traj->getEndPointMomentum();
            //p->setEndPointMomentum(endpMomentum[0], endpMomentum[1], endpMomentum[2]);

            double endp[] = {traj->getEndPoint()[0], traj->getEndPoint()[1], traj->getEndPoint()[2]};
            p->setEndpoint(endp);

            if (traj->GetParentID() > 0) {
                IMPL::MCParticleImpl* parent = findMCParticle(traj->GetParentID());
                if (parent != nullptr) {
                    p->addParent(parent);
                }
                // Sometimes this is okay if there is track selection from plugins in the event that kills primary tracks.
                /*else {
                    // If the parent particle can not be found by its track ID, this is a fatal error!
                    std::cerr << "MCParticleBuilder : MCParticle with parent ID " << traj->GetParentID() << " not found for track ID " << traj->GetTrackID() << std::endl;
                    G4Exception("MCParticleBuilder::buildMCParticle", "", FatalException, "MCParticle not found from parent track ID.");
                }*/
            }

            // Set sim status to indicate particle was created in simulation.
            if (!traj->getGenStatus()) {
                std::bitset<32> simStatus;
                simStatus[EVENT::MCParticle::BITCreatedInSimulation] = 1;
                p->setSimulatorStatus(simStatus.to_ulong());
            }
        }

        IMPL::LCCollectionVec* buildMCParticleColl(const G4Event* anEvent) {

            auto collVec = new IMPL::LCCollectionVec(EVENT::LCIO::MCPARTICLE);
            auto trajectories = anEvent->GetTrajectoryContainer();

            buildParticleMap(trajectories, collVec);

            for (auto trajectory : *trajectories->GetVector()) {
                if (Trajectory::getTrajectory(trajectory)->getSaveFlag()) {
                    buildMCParticle(static_cast<Trajectory*>(trajectory));
                } /*else {
                    std::cout << "MCParticleBuilder: Save flag off for track " << trajectory->GetTrackID() << std::endl;
                }*/
            }

            return collVec;
        }

    private:

        MCParticleMap particleMap_;

        TrackMap* trackMap_;
};
}



#endif
