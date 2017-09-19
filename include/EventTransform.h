#ifndef HPSSIM_EVENTTRANSFORM_H_
#define HPSSIM_EVENTTRANSFORM_H_

#include "G4Event.hh"

namespace hpssim {

class EventTransform {

    public:

        virtual ~EventTransform() {
        }

        virtual void transform(G4Event*) = 0;
};

};

#endif
