#include "PrimaryGenerator.h"

namespace hpssim {
    
std::once_flag PrimaryGenerator::flag1;
std::mt19937 PrimaryGenerator::random_gen(12345);   // Static random number generator ensures all sub-classes use same one.

PrimaryGenerator::PrimaryGenerator(std::string name) : name_(name) {
    messenger_ = new PrimaryGeneratorMessenger(this);
}

PrimaryGenerator::~PrimaryGenerator() {
    for (auto transform : transforms_) {
        delete transform;
    }
    delete sampling_;
    delete messenger_;
}

}
