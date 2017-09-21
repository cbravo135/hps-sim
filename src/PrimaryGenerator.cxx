#include "PrimaryGenerator.h"

namespace hpssim {

PrimaryGenerator::PrimaryGenerator(std::string name) : name_(name) {
    messenger_ = new PrimaryGeneratorMessenger(this);
}

PrimaryGenerator::~PrimaryGenerator() {
    delete sampling_;
    delete messenger_;
}

}
