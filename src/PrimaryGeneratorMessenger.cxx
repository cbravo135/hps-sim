#include "PrimaryGeneratorMessenger.h"

#include "EventSampling.h"
#include "EventTransform.h"
#include "PrimaryGenerator.h"

namespace hpssim {

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGenerator* generator) : generator_(generator) {
    dir_ = new G4UIdirectory(G4String("/hps/generators/" + generator->getName() + "/"), this);

    fileCmd_ = new G4UIcmdWithAString(G4String("/hps/generators/" + generator->getName() + "/file"), this);

    sampleCmd_ = new G4UIcommand(G4String("/hps/generators/" + generator->getName() + "/sample"), this);
    G4UIparameter* p = new G4UIparameter("distribution", 's', false);
    sampleCmd_->SetParameter(p);
    p = new G4UIparameter("param", 'd', false);
    sampleCmd_->SetParameter(p);

    auto transformPath = G4String("/hps/generators/" + generator->getName() + "/transform/");
    transformDir_ = new G4UIdirectory(transformPath, this);

    posCmd_ = new G4UIcommand(G4String(transformPath + "pos"), this);
    p = new G4UIparameter("x", 'd', false);
    posCmd_->SetParameter(p);
    p = new G4UIparameter("y", 'd', false);
    posCmd_->SetParameter(p);
    p = new G4UIparameter("z", 'd', false);
    posCmd_->SetParameter(p);

    smearCmd_ = new G4UIcommand(G4String(transformPath + "smear"), this);
    p = new G4UIparameter("sigma_x", 'd', false);
    smearCmd_->SetParameter(p);
    p = new G4UIparameter("sigma_y", 'd', false);
    smearCmd_->SetParameter(p);
    p = new G4UIparameter("sigma_z", 'd', false);
    smearCmd_->SetParameter(p);

    rotCmd_ = new G4UIcommand(G4String(transformPath + "rot"), this);
    p = new G4UIparameter("theta", 'd', false);
    rotCmd_->SetParameter(p);
}

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger() {
    delete dir_;
}

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValues) {
    if (command == fileCmd_) {
        generator_->addFile(newValues);
    } else if (command == sampleCmd_) {
        /*
         * Create an event sampling object for this generator.
         */
        std::stringstream sstream(newValues);
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
        } else {
            G4Exception("", "", FatalException, G4String("Do not know about sampling '" + distrib + "'."));
        }
        std::cout << "PrimaryGeneratorMessenger: Creating new event sampling '"
                << distrib << "' with param " << param << "." << std::endl;
        eventSampling->setParam(param);
        generator_->setEventSampling(eventSampling);
    } else if (command == posCmd_) {
        std::stringstream sstream(newValues);
        double xyz[3];
        sstream >> xyz[0] >> xyz[1] >> xyz[2];
        generator_->addTransform(new VertexPositionTransform(xyz[0], xyz[1], xyz[2]));
    } else if (command == smearCmd_) {
        std::stringstream sstream(newValues);
        double sigmas[3];
        sstream >> sigmas[0] >> sigmas[1] >> sigmas[2];
        generator_->addTransform(new SmearVertexTransform(sigmas[0], sigmas[1], sigmas[2]));
    } else if (command == rotCmd_) {
        std::stringstream sstream(newValues);
        double theta;
        sstream >> theta;
        generator_->addTransform(new RotateTransform(theta));
    }
}

}
