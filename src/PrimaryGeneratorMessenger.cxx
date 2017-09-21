#include "PrimaryGeneratorMessenger.h"

#include "EventSampling.h"
#include "PrimaryGenerator.h"

namespace hpssim {

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGenerator* generator) : generator_(generator) {
    dir_ = new G4UIdirectory(G4String("/hps/generators/" + generator->getName() + "/"), this);

    fileCmd_ = new G4UIcmdWithAString(G4String("/hps/generators/" + generator->getName() + "/file"), this);

    sampleCmd_ = new G4UIcommand(G4String("/hps/generators/" + generator->getName() + "/sample"), this);
    G4UIparameter* p = new G4UIparameter("distribution", 's', false);
    sampleCmd_->SetParameter(p);
    p = new G4UIparameter("parameter", 'd', false);
    sampleCmd_->SetParameter(p);
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
    }
}

}
