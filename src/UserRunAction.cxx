#include "UserRunAction.h"

#include "LcioPersistencyManager.h"
#include "PrimaryGeneratorAction.h"

namespace hpssim {

UserRunAction::UserRunAction() {
}

UserRunAction::~UserRunAction() {
}

void UserRunAction::BeginOfRunAction(const G4Run* aRun) {

    // init LCIO persistence engine
    LcioPersistencyManager::getInstance()->Initialize();

    // init the primary generators
    PrimaryGeneratorAction::getPrimaryGeneratorAction()->initialize();

    // init sim plugins e.g. read parameter settings into variables, etc.
    PluginManager::getPluginManager()->initializePlugins();

    // activate plugin manager's begin run action
    PluginManager::getPluginManager()->beginRun(aRun);
}

void UserRunAction::EndOfRunAction(const G4Run*) {
}
}
