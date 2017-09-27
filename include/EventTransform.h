#ifndef HPSSIM_EVENTTRANSFORM_H_
#define HPSSIM_EVENTTRANSFORM_H_

#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandFlat.h"

#include "G4SystemOfUnits.hh"
#include "G4Event.hh"

#include <iostream>
#include <cstdlib>

namespace hpssim {

/**
 * Interface for transforming generated events.
 */
class EventTransform {

    public:

        virtual ~EventTransform() {
        }

        virtual void transform(G4Event*) = 0;
};

/**
 * Transform vertices to a fixed position.
 */
class PositionTransform : public EventTransform {

    public:

        PositionTransform(double x, double y, double z) {
            x_ = x;
            y_ = y;
            z_ = z;
        }

        void transform(G4Event* anEvent) {
            int nVertex = anEvent->GetNumberOfPrimaryVertex();
            for (int iVertex = 0; iVertex < nVertex; iVertex++) {
                //std::cout << "VertexPositionTransform: Setting vertex position to ( "
                //        << x_ << ", " << y_ << ", " << z_ << " )." << std::endl;
                anEvent->GetPrimaryVertex(iVertex)->SetPosition(x_, y_, z_);
            }
        }

    private:

        double x_;
        double y_;
        double z_;
};

/**
 * Gaussian smearing of vertex positions.
 */
class SmearTransform : public EventTransform {

    public:

        SmearTransform(double sigmaX, double sigmaY, double sigmaZ) {
            sigmaX_ = sigmaX;
            sigmaY_ = sigmaY;
            sigmaZ_ = sigmaZ;
            randX_ = new CLHEP::RandGauss(G4Random::getTheEngine(), 0, sigmaX);
            randY_ = new CLHEP::RandGauss(G4Random::getTheEngine(), 0, sigmaY);
            randZ_ = new CLHEP::RandGauss(G4Random::getTheEngine(), 0, sigmaZ);
        }

        virtual ~SmearTransform() {
            delete randX_;
            delete randY_;
            delete randZ_;
        }

        void transform(G4Event* anEvent) {
            double shiftX, shiftY, shiftZ;
            shiftX = shiftY = shiftZ = 0;
            if (sigmaX_ != 0.) {
                shiftX = randX_->fire();
                //std::cout << "shiftX: " << shiftX << std::endl;
            }
            if (sigmaY_ != 0.) {
                shiftY = randY_->fire();
                //std::cout << "shiftY: " << shiftY << std::endl;
            }
            if (sigmaZ_ != 0.) {
                shiftZ = randZ_->fire();
                //std::cout << "shiftZ: " << shiftZ << std::endl;
            }
            int nVertex = anEvent->GetNumberOfPrimaryVertex();
            for (int iVertex = 0; iVertex < nVertex; iVertex++) {
                auto vertex = anEvent->GetPrimaryVertex(iVertex);
                auto pos = vertex->GetPosition();
                if (shiftX != 0) {
                    pos.setX(pos.x() + shiftX);
                    //std::cout << "posX: " << pos.x() << std::endl;
                }
                if (shiftY != 0) {
                    pos.setY(pos.y() + shiftY);
                    //std::cout << "posY: " << pos.y() << std::endl;
                }
                if (shiftZ != 0) {
                    pos.setZ(pos.z() + shiftZ);
                    //std::cout << "posZ: " << pos.z() << std::endl;
                }
                vertex->SetPosition(pos.x(), pos.y(), pos.z());
                //std::cout << "SmearTransform: Smeared vertex to new pos " << vertex->GetPosition()
                //        << "." << std::endl;
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

/**
 * Rotation into "beam coordinates" which transforms Px and Pz as well as the vertex X and Z positions.
 *
 * @note Based on algorithm from this StdHep tool:
 * @link https://github.com/JeffersonLab/hps-mc/blob/master/tools/stdhep-tools/src/beam_coords.cc
 */
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
                //std::cout << "RotateTransform: Rotated vertex to " << vertex->GetPosition() << "." << std::endl;
                int nPrim = vertex->GetNumberOfParticle();
                for (int iPrim = 0; iPrim < nPrim; iPrim++) {
                    rotatePrimary(vertex->GetPrimary(iPrim));
                }
            }
        }

    private:

        void rotatePrimary(G4PrimaryParticle* primary) {
            double px = primary->GetMomentum().x() * std::cos(theta_) + primary->GetMomentum().z() * std::sin(theta_);
            double py = primary->GetMomentum().y();
            double pz = primary->GetMomentum().z() * std::cos(theta_) - primary->GetMomentum().x() * std::sin(theta_);
            primary->SetMomentum(px, py, pz);
            //std::cout << "RotateTransform: Rotated primary momentum to " << primary->GetMomentum() << "." << std::endl;
            if (primary->GetNext()) {
                rotatePrimary(primary->GetNext());
            }
        }

    private:

        double theta_;
};

/**
 * Transform the vertex Z position in a uniform random range.
 */
class RandZTransform : public EventTransform {

    public:
        RandZTransform(double width) {
            width_ = width;
            //CLHEP::RandFlat::setTheEngine(G4Random::getTheEngine());
        }

        void transform(G4Event* anEvent) {
            int nVertex = anEvent->GetNumberOfPrimaryVertex();
            for (int iVertex = 0; iVertex < nVertex; iVertex++) {
                auto vertex = anEvent->GetPrimaryVertex(iVertex);
                auto pos = vertex->GetPosition();
                double a = pos.z() - width_ / 2;
                double b = pos.z() + width_ / 2;
                double z = vertex->GetPosition().z() + CLHEP::RandFlat::shoot(a, b);
                vertex->SetPosition(pos.x(), pos.y(), z);
                //std::cout << "RandZTransform: Set Z to " << z << "." << std::endl;
            }
        }

    private:

        /** Width of random distribution (default matches 4 micron target thickness). */
        double width_{4.0*um};
};

}

#endif
