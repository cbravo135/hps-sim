#ifndef HPSSIM_STDHEPPARTICLE_H_
#define HPSSIM_STDHEPPARTICLE_H_

namespace hpssim {

/**
 * @class StdHepParticle
 * @brief Provides access to StdHep track data with direct pointers to mother and daughter particles
 */
class StdHepParticle {

    public:

        StdHepParticle(lStdTrack& track) : data_(track) {
            mothers_[0] = nullptr;
            mothers_[1] = nullptr;
            daughters_[0] = nullptr;
            daughters_[1] = nullptr;
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

        const lStdTrack& getData() const {
            return data_;
        }

    private:

        StdHepParticle* mothers_[2];
        StdHepParticle* daughters_[2];

        lStdTrack& data_;
};

}

#endif
