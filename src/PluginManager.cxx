#include "PluginManager.h"

#include "PluginMessenger.h"

namespace hpssim {

    PluginManager::PluginManager() {
        messenger_ = new PluginMessenger(this);
    }

    PluginManager::~PluginManager() {
        destroyPlugins();
        delete messenger_;
    }

    void PluginManager::beginRun(const G4Run* run) {
        auto plugins = actions_[SimPlugin::RUN];
        for (PluginVec::iterator it = plugins.begin(); it != plugins.end(); it++) {
            (*it)->beginRun(run);
        }
    }

    void PluginManager::endRun(const G4Run* run) {
        auto plugins = actions_[SimPlugin::RUN];
        for (PluginVec::iterator it = plugins.begin(); it != plugins.end(); it++) {
            (*it)->endRun(run);
        }
    }

    void PluginManager::stepping(const G4Step* step) {
        auto plugins = actions_[SimPlugin::STEPPING];
        for (PluginVec::iterator it = plugins.begin(); it != plugins.end(); it++) {
            (*it)->stepping(step);
        }
    }

    void PluginManager::preTracking(const G4Track* track) {
        auto plugins = actions_[SimPlugin::TRACKING];
        for (PluginVec::iterator it = plugins.begin(); it != plugins.end(); it++) {
            (*it)->preTracking(track);
        }
    }

    void PluginManager::postTracking(const G4Track* track) {
        auto plugins = actions_[SimPlugin::TRACKING];
        for (PluginVec::iterator it = plugins.begin(); it != plugins.end(); it++) {
            (*it)->postTracking(track);
        }
    }

    void PluginManager::beginEvent(const G4Event* event) {
        auto plugins = actions_[SimPlugin::EVENT];
        for (PluginVec::iterator it = plugins.begin(); it != plugins.end(); it++) {
            (*it)->beginEvent(event);
        }
    }

    void PluginManager::endEvent(const G4Event* event) {
        auto plugins = actions_[SimPlugin::EVENT];
        for (PluginVec::iterator it = plugins.begin(); it != plugins.end(); it++) {
            (*it)->endEvent(event);
        }
    }

    void PluginManager::generatePrimary(G4Event* event) {
        auto plugins = actions_[SimPlugin::PRIMARY];
        for (PluginVec::iterator it = plugins.begin(); it != plugins.end(); it++) {
            if ((*it)->hasPrimaryGeneratorAction()) {
                (*it)->generatePrimary(event);
            }
        }
    }

    G4ClassificationOfNewTrack PluginManager::stackingClassifyNewTrack(const G4Track* track) {

        auto plugins = actions_[SimPlugin::STACKING];

        // Default value of a track is fUrgent.
        G4ClassificationOfNewTrack currentTrackClass = G4ClassificationOfNewTrack::fUrgent;

        for (PluginVec::iterator it = plugins.begin(); it != plugins.end(); it++) {

            // Get proposed new track classification from this plugin.
            G4ClassificationOfNewTrack newTrackClass = (*it)->stackingClassifyNewTrack(track, currentTrackClass);

            // Only set the current classification if the plugin changed it.
            if (newTrackClass != currentTrackClass) {

                // Set the track classification from this plugin.
                currentTrackClass = newTrackClass;
            }
        }

        // Return the current track classification.
        return currentTrackClass;
    }

    void PluginManager::stackingNewStage() {
        auto plugins = actions_[SimPlugin::STACKING];
        for (PluginVec::iterator it = plugins.begin(); it != plugins.end(); it++) {
            (*it)->stackingNewStage();
        }
    }

    void PluginManager::stackingPrepareNewEvent() {
        auto plugins = actions_[SimPlugin::STACKING];
        for (PluginVec::iterator it = plugins.begin(); it != plugins.end(); it++) {
            (*it)->stackingPrepareNewEvent();
        }
    }

    SimPlugin* PluginManager::findPlugin(const std::string& pluginName) {
        SimPlugin* foundPlugin = nullptr;
        for (PluginVec::iterator iPlugin = plugins_.begin(); iPlugin != plugins_.end(); iPlugin++) {
            if ((*iPlugin)->getName() == pluginName) {
                foundPlugin = *iPlugin;
                break;
            }
        }
        return foundPlugin;
    }

    void PluginManager::create(const std::string& pluginName, const std::string& libName) {
        if (findPlugin(pluginName) == nullptr) {
            SimPlugin* plugin = pluginLoader_.create(pluginName, libName);
            registerPlugin(plugin);
        } else {
            std::cerr << "[ PluginManager ] - Plugin " << pluginName << " already exists." << std::endl;
            throw new std::runtime_error("Plugin already loaded.");
        }
    }

    void PluginManager::destroy(const std::string& pluginName) {
        SimPlugin* plugin = findPlugin(pluginName);
        if (plugin != nullptr) {
            destroy(plugin);
        }
    }

    void PluginManager::destroy(SimPlugin* plugin) {
        if (plugin != nullptr) {
            deregisterPlugin(plugin);
            pluginLoader_.destroy(plugin);
        }
    }

    std::ostream& PluginManager::print(std::ostream& os) {
        for (PluginVec::iterator iPlugin = plugins_.begin(); iPlugin != plugins_.end(); iPlugin++) {
            os << (*iPlugin)->getName() << std::endl;
        }
        return os;
    }

    void PluginManager::registerPlugin(SimPlugin* plugin) {

        // register plugin actions
        for (auto action : plugin->getActions()) {
            actions_[action].push_back(plugin);
        }

        // remove from master list
        plugins_.push_back(plugin);
    }

    void PluginManager::deregisterPlugin(SimPlugin* plugin) {

        // deregister plugin actions
        for (auto action : plugin->getActions()) {
            auto plugins = actions_[action];
            std::vector<SimPlugin*>::iterator itPlugin = std::find(plugins_.begin(), plugins_.end(), plugin);
            if (itPlugin != plugins_.end()) {
                plugins_.erase(itPlugin);
            }
        }

        // remove from master list
        std::vector<SimPlugin*>::iterator pos = std::find(plugins_.begin(), plugins_.end(), plugin);
        if (pos != plugins_.end()) {
            plugins_.erase(pos);
        }
    }

    void PluginManager::destroyPlugins() {
        for (unsigned iPlugin = 0; iPlugin < plugins_.size(); iPlugin++) {
            destroy(plugins_[iPlugin]);
        }
        plugins_.clear();
    }

} // namespace sim
