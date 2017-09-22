#ifndef HPSSIM_EVENTTRANSFORM_H_
#define HPSSIM_EVENTTRANSFORM_H_

#include "G4Event.hh"

#include <iostream>

namespace hpssim {

// Commands:
// pos - vertex position
// smear - Gaussian smearing of vertex position
// rot - rotate coordinate system

class EventTransform {

    public:

        virtual ~EventTransform() {
        }

        virtual void transform(G4Event*) = 0;
};

class VertexPositionTransform : public EventTransform {

    public:

        VertexPositionTransform(double x, double y, double z) {
            x_ = x;
            y_ = y;
            z_ = z;
        } 
 
        void transform(G4Event* anEvent) {
            int nVertex = anEvent->GetNumberOfPrimaryVertex();
            for (int iVertex = 0; iVertex < nVertex; iVertex++) {
                std::cout << "EventTransform: Setting vertex position to ( " 
                        << x_ << ", " << y_ << ", " << z_ << " )." << std::endl;
                anEvent->GetPrimaryVertex(iVertex)->SetPosition(x_, y_, z_);
            }
        }

    private:

        double x_;
        double y_;
        double z_;

};

}

#endif
