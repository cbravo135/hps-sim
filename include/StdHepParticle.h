#ifndef HPSSIM_STDHEPPARTICLE_H_
#define HPSSIM_STDHEPPARTICLE_H_

namespace hpssim {

/**
 * @class StdHepParticle
 * @brief Provides access to StdHep track data with direct pointers to mother and daughter particles
 * @note Data member values are copied directly from lStdTrack object, because the way that class is managed
 * by the lStdHep interface means that trying to store direct references or pointers to it causes memory corruption.
 */
class StdHepParticle {

    public:

        class Data {
            public:
                int pid;
                double fourVec[4];
                double mass;
                double properTime;
                double position[3];
                int dau[2];
                int mom[2];

                StdHepParticle* mothers_[2];
                StdHepParticle* daughters_[2];
        };

    public:

        StdHepParticle(lStdTrack* track) {

            data_.mothers_[0] = nullptr;
            data_.mothers_[1] = nullptr;
            data_.daughters_[1] = nullptr;
            data_.daughters_[2] = nullptr;

            data_.pid = track->pid;
            data_.fourVec[0] = track->Px;
            data_.fourVec[1] = track->Py;
            data_.fourVec[2] = track->Pz;
            data_.fourVec[3] = track->E;
            data_.mass = track->M;
            data_.properTime = track->T;
            data_.position[0] = track->X;
            data_.position[1] = track->Y;
            data_.position[2] = track->Z;
            data_.dau[0] = track->daughter1;
            data_.dau[1] = track->daughter2;
            data_.mom[0] = track->mother1;
            data_.mom[1] = track->mother2;
        }

        void setMother(int i, StdHepParticle* particle) {
            data_.mothers_[i] = particle;
        }

        StdHepParticle* getMother(int i) {
            return data_.mothers_[i];
        }

        StdHepParticle* getDaughter(int i) {
            return data_.daughters_[i];
        }

        void setDaughter(int i, StdHepParticle* particle) {
            data_.daughters_[i] = particle;
        }

        const Data& getData() const {
            return data_;
        }

    private:

        Data data_;
};

}

#endif
