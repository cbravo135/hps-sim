#ifndef HPSSIM_EVENTTRANSFORM_H_
#define HPSSIM_EVENTTRANSFORM_H_

#include "CLHEP/Random/RandGauss.h"

#include "G4Event.hh"

#include <iostream>
#include <cstdlib>

namespace hpssim {

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
                std::cout << "VertexPositionTransform: Setting vertex position to ( "
                        << x_ << ", " << y_ << ", " << z_ << " )." << std::endl;
                anEvent->GetPrimaryVertex(iVertex)->SetPosition(x_, y_, z_);
            }
        }

    private:

        double x_;
        double y_;
        double z_;
};

class SmearVertexTransform : public EventTransform {

    public:

        SmearVertexTransform(double sigmaX, double sigmaY, double sigmaZ) {
            sigmaX_ = sigmaX;
            sigmaY_ = sigmaY;
            sigmaZ_ = sigmaZ;
            randX_ = new CLHEP::RandGauss(G4Random::getTheEngine(), 0, sigmaX);
            randY_ = new CLHEP::RandGauss(G4Random::getTheEngine(), 0, sigmaY);
            randZ_ = new CLHEP::RandGauss(G4Random::getTheEngine(), 0, sigmaZ);
        }

        virtual ~SmearVertexTransform() {
            delete randX_;
            delete randY_;
            delete randZ_;
        }

        void transform(G4Event* anEvent) {
            double shiftX, shiftY, shiftZ;
            shiftX = shiftY = shiftZ = 0;
            if (sigmaX_ > 0.) {
                shiftX = sigmaX_ * randX_->fire();
            }
            if (sigmaY_ > 0.) {
                shiftY = sigmaY_ * randY_->fire();
            }
            if (sigmaZ_ > 0.) {
                shiftZ = sigmaZ_ * randZ_->fire();
            }
            int nVertex = anEvent->GetNumberOfPrimaryVertex();
            for (int iVertex = 0; iVertex < nVertex; iVertex++) {
                auto vertex = anEvent->GetPrimaryVertex(iVertex);
                auto pos = vertex->GetPosition();
                double x = pos.x();
                double y = pos.y();
                double z = pos.z();
                if (sigmaX_) {
                    x += shiftX;
                }
                if (sigmaY_) {
                    y += shiftY;
                }
                if (sigmaZ_) {
                    z += shiftZ;
                }
                vertex->SetPosition(x, y, z);
                std::cout << "SmearVertexTransform: Smeared vertex to new pos " << vertex->GetPosition()
                        << "." << std::endl;
            }
        }

    private:

        double sigmaX_;
        double sigmaY_;
        double sigmaZ_;

        CLHEP::RandGauss* randX_;
        CLHEP::RandGauss* randY_;
        CLHEP::RandGauss* randZ_;
};

class RotateTransform : public EventTransform {

    public:
        RotateTransform(double theta) {
            theta_ = theta;
        }

        void transform(G4Event* anEvent) {
            int nVertex = anEvent->GetNumberOfPrimaryVertex();
            for (int iVertex = 0; iVertex < nVertex; iVertex++) {
                auto vertex = anEvent->GetPrimaryVertex(iVertex);
                auto pos = vertex->GetPosition();
                double x = pos.x() * std::cos(theta_) + pos.z() * std::sin(theta_);
                double y = pos.y();
                double z = pos.z() * std::cos(theta_) - pos.x() * std::sin(theta_);
                vertex->SetPosition(x, y, z);
                std::cout << "RotateTransform: Rotated vertex to " << vertex->GetPosition() << "." << std::endl;
                int nPrim = vertex->GetNumberOfParticle();
                for (int iPrim = 0; iPrim < nPrim; iPrim++) {
                    rotatePrimary(vertex->GetPrimary(iPrim));
                }
            }
        }

        void rotatePrimary(G4PrimaryParticle* primary) {
            double px = primary->GetMomentum().x() * std::cos(theta_) + primary->GetMomentum().z() * std::sin(theta_);
            double py = primary->GetMomentum().y();
            double pz = primary->GetMomentum().z() * std::cos(theta_) + primary->GetMomentum().x() * std::sin(theta_);
            primary->SetMomentum(px, py, pz);
            std::cout << "RotateTransform: Rotated primary momentum to " << primary->GetMomentum() << "." << std::endl;
            if (primary->GetNext()) {
                rotatePrimary(primary->GetNext());
            }
        }

    private:

        double theta_;
};

}

#endif
