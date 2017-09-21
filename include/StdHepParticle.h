#ifndef HPSSIM_STDHEPPARTICLE_H_
#define HPSSIM_STDHEPPARTICLE_H_

namespace hpssim {

/**
 * @class StdHepParticle
 * @brief Provides access to StdHep track data with direct pointers to mother and daughter particles
 * @note Data member values are copied directly from the lStdTrack object, because the way that class is managed
 * by the lStdHep interface means that trying to store direct references or pointers to it causes memory corruption.
 */
class StdHepParticle {

    public:

        class Data {
            public:
                int pid;
                double Px;
                double Py;
                double Pz;
                double E;
                double M;
                double T;
                double x;
                double y;
                double z;
                long daughter1;
                long daughter2;
                long mother1;
                long mother2;
        };

    public:

        StdHepParticle(lStdTrack* track) {

            mothers_[0] = nullptr;
            mothers_[1] = nullptr;
            daughters_[0] = nullptr;
            daughters_[1] = nullptr;

            data_.pid = track->pid;
            data_.Px = track->Px;
            data_.Py = track->Py;
            data_.Pz = track->Pz;
            data_.E = track->E;
            data_.M = track->M;
            data_.T = track->T;
            data_.x = track->X;
            data_.y = track->Y;
            data_.z = track->Z;
            data_.daughter1 = track->daughter1;
            data_.daughter2 = track->daughter2;
            data_.mother1 = track->mother1;
            data_.mother2 = track->mother2;
        }

        void setMother(int i, StdHepParticle* particle) {
            mothers_[i] = particle;
        }

        StdHepParticle* getMother(int i) {
            return mothers_[i];
        }

        StdHepParticle* getDaughter(int i) {
            return daughters_[i];
        }

        void setDaughter(int i, StdHepParticle* particle) {
            daughters_[i] = particle;
        }

        const Data& getData() const {
            return data_;
        }

    private:

        StdHepParticle* mothers_[2];
        StdHepParticle* daughters_[2];

        Data data_;
};

}

#endif
