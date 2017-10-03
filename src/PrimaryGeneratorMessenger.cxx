#include "PrimaryGeneratorMessenger.h"

#include "EventSampling.h"
#include "EventTransform.h"
#include "PrimaryGenerator.h"

namespace hpssim {

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGenerator* generator) : generator_(generator) {

    auto genDir = G4String("/hps/generators/" + generator->getName() + "/");

    dir_ = new G4UIdirectory(genDir, this);

    fileCmd_ = new G4UIcmdWithAString(G4String(genDir + "file"), this);

    sampleCmd_ = new G4UIcommand(G4String(genDir + "sample"), this);
    G4UIparameter* p = new G4UIparameter("distribution", 's', false);
    sampleCmd_->SetParameter(p);
    p = new G4UIparameter("param", 'd', false);
    sampleCmd_->SetParameter(p);

    verboseCmd_ = new G4UIcmdWithAnInteger(G4String(genDir + "verbose"), this);

    paramCmd_ = new G4UIcommand(G4String(genDir + "param"), this);
    p = new G4UIparameter("name", 's', false);
    paramCmd_->SetParameter(p);
    p = new G4UIparameter("value", 'd', false);
    paramCmd_->SetParameter(p);

    auto transformDir = G4String(genDir + "transform/");
    transformDir_ = new G4UIdirectory(transformDir, this);

    posCmd_ = new G4UIcommand(G4String(transformDir + "pos"), this);
    p = new G4UIparameter("x", 'd', false);
    posCmd_->SetParameter(p);
    p = new G4UIparameter("y", 'd', false);
    posCmd_->SetParameter(p);
    p = new G4UIparameter("z", 'd', false);
    posCmd_->SetParameter(p);

    smearCmd_ = new G4UIcommand(G4String(transformDir + "smear"), this);
    p = new G4UIparameter("sigma_x", 'd', false);
    smearCmd_->SetParameter(p);
    p = new G4UIparameter("sigma_y", 'd', false);
    smearCmd_->SetParameter(p);
    p = new G4UIparameter("sigma_z", 'd', false);
    smearCmd_->SetParameter(p);

    rotCmd_ = new G4UIcommand(G4String(transformDir + "rot"), this);
    p = new G4UIparameter("theta", 'd', false);
    rotCmd_->SetParameter(p);

    randzCmd_ = new G4UIcommand(G4String(transformDir + "randz"), this);
    p = new G4UIparameter("width", 'd', false);
    randzCmd_->SetParameter(p);

    randomCmd_ = new G4UIcommand(G4String(genDir + "random"), this);

    sequentialCmd_ = new G4UIcommand(G4String(genDir + "sequential"), this);
}

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger() {
    delete dir_;
}

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValues) {
    std::stringstream sstream(newValues);
    if (command == fileCmd_) {
        generator_->addFile(newValues);
    } else if (command == verboseCmd_ ) {
        int newLevel = verboseCmd_->ConvertToInt(newValues);
        generator_->setVerbose(newLevel);
        std::cout << "PrimaryGeneratorMessenger: Set verbose level to " << newLevel
                << " for generator " << generator_->getName() << "." << std::endl;
    } else if (command == paramCmd_) {
        std::string name;
        double value;
        sstream >> name;
        sstream >> value;
        generator_->getParameters().set(name, value);
        std::cout << "PrimaryGeneratorMessenger: Set param '" << name << "' = " << value <<
                " for generator " << generator_->getName() << std::endl;
    } else if (command == sampleCmd_) {
        /*
         * Create an event sampling object for this generator.
         */
        std::string distrib;
        double param = 0;
        sstream >> distrib;
        sstream >> param;
        EventSampling* eventSampling = nullptr;
        if (distrib == "poisson") {
            eventSampling = new PoissonEventSampling();
        } else if (distrib == "uniform") {
            eventSampling = new UniformEventSampling();
        } else if (distrib == "period") {
            eventSampling = new PeriodicEventSampling();
        } else if (distrib == "sigma") {
            eventSampling = new CrossSectionEventSampling();
        } else {
            G4Exception("", "", FatalException, G4String("Do not know about sampling '" + distrib + "'."));
        }
        std::cout << "PrimaryGeneratorMessenger: Creating new event sampling '"
                << distrib << "' with param " << param << "." << std::endl;
        eventSampling->setParam(param);
        generator_->setEventSampling(eventSampling);
    } else if (command == posCmd_) {
        double xyz[3];
        sstream >> xyz[0] >> xyz[1] >> xyz[2];
        generator_->addTransform(new PositionTransform(xyz[0], xyz[1], xyz[2]));
    } else if (command == smearCmd_) {
        double sigmas[3];
        sstream >> sigmas[0] >> sigmas[1] >> sigmas[2];
        generator_->addTransform(new SmearTransform(sigmas[0], sigmas[1], sigmas[2]));
    } else if (command == rotCmd_) {
        double theta;
        sstream >> theta;
        generator_->addTransform(new RotateTransform(theta));
    } else if (command == randzCmd_) {
        double width;
        sstream >> width;
        generator_->addTransform(new RandZTransform(width));
    } else if (command == randomCmd_) {
        if (!generator_->supportsRandomAccess()) {
            G4Exception("", "", FatalException,
                    G4String("The generator " + G4String(generator_->getName()) + " does not support random access."));
        }
        generator_->setReadMode(PrimaryGenerator::Random);
    } else if (command == sequentialCmd_) {
        generator_->setReadMode(PrimaryGenerator::Sequential);
    }
}

}
