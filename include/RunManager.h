#ifndef HPSSIM_RUNMANAGER_H_
#define HPSSIM_RUNMANAGER_H_

#include "G4RunManager.hh"

#include "UnknownDecayPhysics.h"

namespace hpssim {

    class RunManager : public G4RunManager {

        public:

            void InitializePhysics();
    };
}




#endif
