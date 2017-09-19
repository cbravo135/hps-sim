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

        MCParticleBuilder() {
            trackMap_ = UserTrackingAction::getUserTrackingAction()->getTrackMap();
        }

        virtual ~MCParticleBuilder() {
        }

        void buildParticleMap(G4TrajectoryContainer* trajectories, IMPL::LCCollectionVec* collVec) {
            particleMap_.clear();
            for (auto trajectory : *trajectories->GetVector()) {
                auto particle = new IMPL::MCParticleImpl;
                collVec->addElement(particle);
                particleMap_[trajectory->GetTrackID()] = particle;
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

            //p->setProcessType(traj->getProcessType());

            double vertexArr[] = {traj->getVertexPosition()[0], traj->getVertexPosition()[1], traj->getVertexPosition()[2]};
            p->setVertex(vertexArr);

            double momentum[] = {traj->GetInitialMomentum()[0] / GeV, traj->GetInitialMomentum()[1] / GeV, traj->GetInitialMomentum()[2] / GeV};
            p->setMomentum(momentum);

            //const G4ThreeVector& endpMomentum = traj->getEndPointMomentum();
            //p->setEndPointMomentum(endpMomentum[0], endpMomentum[1], endpMomentum[2]);

            //G4ThreeVector endpoint = traj->getEndPoint();
            //p->setEndPoint(endpoint[0], endpoint[1], endpoint[2]);
            double endp[] = {traj->getEndPoint()[0], traj->getEndPoint()[1], traj->getEndPoint()[2]};
            p->setEndpoint(endp);

            if (traj->GetParentID() > 0) {
                IMPL::MCParticleImpl* parent = findMCParticle(traj->GetParentID());
                if (parent != nullptr) {
                    p->addParent(parent);
                } else {
                    // If the parent particle can not be found by its track ID, this is a fatal error!
                    std::cerr << "MCParticleBuilder : MCParticle with parent ID " << traj->GetParentID() << " not found for track ID " << traj->GetTrackID() << std::endl;
                    G4Exception("MCParticleBuilder::buildMCParticle", "", FatalException, "MCParticle not found from parent track ID.");
                }
            }
        }

        IMPL::LCCollectionVec* buildMCParticleColl(const G4Event* anEvent) {

            auto collVec = new IMPL::LCCollectionVec(EVENT::LCIO::MCPARTICLE);
            auto trajectories = anEvent->GetTrajectoryContainer();

            buildParticleMap(trajectories, collVec);

            for (auto trajectory : *trajectories->GetVector()) {
                buildMCParticle(static_cast<Trajectory*>(trajectory));
            }

            return collVec;
        }

    private:

        MCParticleMap particleMap_;

        TrackMap* trackMap_;
};
}



#endif
