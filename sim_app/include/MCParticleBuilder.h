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

        MCParticleBuilder(TrackMap* trackMap);

        virtual ~MCParticleBuilder();

        void buildParticleMap(G4TrajectoryContainer* trajectories, IMPL::LCCollectionVec* collVec);

        IMPL::MCParticleImpl* findMCParticle(G4int trackID);

        void buildMCParticle(Trajectory* traj);

        IMPL::LCCollectionVec* buildMCParticleColl(const G4Event* anEvent);

        TrackMap& getTrackMap();

    private:

        MCParticleMap particleMap_;

        TrackMap* trackMap_;
};
}



#endif
