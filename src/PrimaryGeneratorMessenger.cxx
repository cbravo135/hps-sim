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

    transformCmd_ = new G4UIcommand(G4String("/hps/generators/" + generator->getName() + "/transform"), this);
    p = new G4UIparameter("name", 's', false);
    transformCmd_->SetParameter(p);
    p = new G4UIparameter("param1", 'd', false);
    transformCmd_->SetParameter(p);
    p = new G4UIparameter("param2", 'd', false);
    transformCmd_->SetParameter(p);
    p = new G4UIparameter("param3", 'd', false);
    transformCmd_->SetParameter(p);
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
    } else if (command == transformCmd_) {
        std::stringstream sstream(newValues);
        std::string name;
        double params[3];   
        sstream >> name;
        sstream >> params[0] >> params[1] >> params[2];
        EventTransform* transform = nullptr;
        if (name == "pos") {
            transform = new VertexPositionTransform(params[0], params[1], params[2]);
        }
        generator_->addTransform(transform);
    }
}

}
