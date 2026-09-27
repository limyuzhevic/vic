#include "WorldPlugin.hpp"

namespace nlm {
namespace plugin {

WorldPlugin::WorldPlugin() : state_(PluginState::UNLOADED) {}

WorldPlugin::~WorldPlugin() {
    for (auto* world : worldSystems_) {
        delete world;
    }
    worldSystems_.clear();
}

std::string WorldPlugin::getName() const {
    return metadata_.name;
}

std::string WorldPlugin::getVersion() const {
    return metadata_.version.toString();
}

std::string WorldPlugin::getDescription() const {
    return metadata_.description;
}

PluginType WorldPlugin::getType() const {
    return metadata_.type;
}

PluginMetadata WorldPlugin::getMetadata() const {
    return metadata_;
}

bool WorldPlugin::initialize(const PluginMetadata& metadata) {
    state_ = PluginState::INITIALIZING;
    
    try {
        metadata_ = metadata;
        
        if (!validateInput(ProcessingContext())) {
            error_ = "Input validation failed";
            state_ = PluginState::ERROR;
            return false;
        }
        
        if (!loadDependencies()) {
            error_ = "Failed to load dependencies";
            state_ = PluginState::ERROR;
            return false;
        }
        
        if (!preIntegrate()) {
            error_ = "Pre-integration failed";
            state_ = PluginState::ERROR;
            return false;
        }
        
        // Create world systems based on configuration
        for (const auto& worldName : config_.worldNames) {
            worldSystems_.push_back(new SimpleWorld(worldName, this));
        }
        
        if (!postIntegrate()) {
            error_ = "Post-integration failed";
            state_ = PluginState::ERROR;
            return false;
        }
        
        state_ = PluginState::INITIALIZED;
        return true;
    } catch (const std::exception& e) {
        error_ = std::string("Exception during initialization: ") + e.what();
        state_ = PluginState::ERROR;
        return false;
    }
}

void WorldPlugin::shutdown() {
    state_ = PluginState::SHUTDOWN;
    
    if (!preDeintegrate()) {
        error_ = "Pre-deintegration failed";
        return;
    }
    
    for (auto* world : worldSystems_) {
        delete world;
    }
    worldSystems_.clear();
    
    if (!postDeintegrate()) {
        error_ = "Post-deintegration failed";
        return;
    }
    
    metadata_ = PluginMetadata();
    config_ = WorldConfig();
    integrationExtensions_.clear();
    eventHandlers_.clear();
    config_.clear();
    integrationCount_ = 0;
    integrationErrors_ = 0;
    totalIntegrationTime_ = std::chrono::duration<double>(0.0);
}

bool WorldPlugin::activate() {
    state_ = PluginState::ACTIVATING;
    
    try {
        if (state_ != PluginState::INITIALIZED) {
            error_ = "Plugin not initialized";
            state_ = PluginState::ERROR;
            return false;
        }
        
        // Reset world systems
        for (auto* world : worldSystems_) {
            world->reset();
        }
        
        state_ = PluginState::ACTIVATED;
        return true;
    } catch (const std::exception& e) {
        error_ = std::string("Exception during activation: ") + e.what();
        state_ = PluginState::ERROR;
        return false;
    }
}

bool WorldPlugin::deactivate() {
    state_ = PluginState::DEACTIVATING;
    
    try {
        for (auto* world : worldSystems_) {
            // Deactivate world if needed
        }
        
        state_ = PluginState::DEACTIVATED;
        return true;
    } catch (const std::exception& e) {
        error_ = std::string("Exception during deactivation: ") + e.what();
        state_ = PluginState::ERROR;
        return false;
    }
}

bool WorldPlugin::loadDependencies() {
    // Load plugin dependencies
    for (const auto& dependency : metadata_.dependencies) {
        // Implement dependency loading logic
    }
    return true;
}

bool WorldPlugin::checkConflicts() {
    // Check for plugin conflicts
    return true;
}

WorldPlugin::WorldSystem::WorldSystem(const std::string& name, WorldPlugin* plugin) 
    : name_(name), plugin_(plugin) {}

WorldPlugin::WorldSystem::~WorldSystem() {}

WorldPlugin::SimpleWorld::SimpleWorld(const std::string& name, WorldPlugin* plugin)
    : WorldSystem(name, plugin) {
    type_ = "SimpleWorld";
}

WorldPlugin::SimpleWorld::~SimpleWorld() {}

bool WorldPlugin::SimpleWorld::update(double dt) {
    // Update world state based on time step
    return true;
}

bool WorldPlugin::SimpleWorld::reset() {
    // Reset world state
    return true;
}

std::string WorldPlugin::SimpleWorld::getState() const {
    return environmentState_;
}

std::vector<double> WorldPlugin::SimpleWorld::getSensorInput(const std::string& sensorType) const {
    auto it = sensorInputs_.find(sensorType);
    if (it == sensorInputs_.end()) {
        return std::vector<double>();
    }
    return it->second;
}

bool WorldPlugin::SimpleWorld::setSensorInput(const std::string& sensorType, const std::vector<double>& values) {
    sensorInputs_[sensorType] = values;
    return true;
}

std::vector<double> WorldPlugin::SimpleWorld::getMotorOutput(const std::string& motorType) const {
    auto it = motorOutputs_.find(motorType);
    if (it == motorOutputs_.end()) {
        return std::vector<double>();
    }
    return it->second;
}

bool WorldPlugin::SimpleWorld::setMotorOutput(const std::string& motorType, const std::vector<double>& values) {
    motorOutputs_[motorType] = values;
    return true;
}

std::string WorldPlugin::SimpleWorld::getEnvironmentState() const {
    return environmentState_;
}

bool WorldPlugin::SimpleWorld::setEnvironmentState(const std::string& state) {
    environmentState_ = state;
    return true;
}

WorldPlugin::WorldSystem* WorldPlugin::createWorldSystem(const std::string& name, const std::string& type) {
    if (type == "SimpleWorld") {
        return new SimpleWorld(name, this);
    }
    return nullptr;
}

bool WorldPlugin::destroyWorldSystem(WorldSystem* world) {
    if (!world) return false;
    
    for (auto it = worldSystems_.begin(); it != worldSystems_.end(); ++it) {
        if (*it == world) {
            delete *it;
            worldSystems_.erase(it);
            return true;
        }
    }
    return false;
}

std::vector<WorldPlugin::WorldSystem*> WorldPlugin::getWorldSystems() const {
    return worldSystems_;
}

WorldPlugin::WorldSystem* WorldPlugin::getWorldSystem(const std::string& name) const {
    for (auto* world : worldSystems_) {
        if (world->getName() == name) {
            return world;
        }
    }
    return nullptr;
}

bool WorldPlugin::addWorldSystem(WorldSystem* world) {
    if (!world) return false;
    
    // Check for duplicate
    for (const auto* existingWorld : worldSystems_) {
        if (existingWorld->getName() == world->getName()) {
            return false;
        }
    }
    
    worldSystems_.push_back(world);
    return true;
}

bool WorldPlugin::removeWorldSystem(const std::string& name) {
    for (auto it = worldSystems_.begin(); it != worldSystems_.end(); ++it) {
        if ((*it)->getName() == name) {
            delete *it;
            worldSystems_.erase(it);
            return true;
        }
    }
    return false;
}

bool WorldPlugin::updateAllWorldSystems(double dt) {
    for (auto* world : worldSystems_) {
        if (!world->update(dt)) {
            return false;
        }
    }
    return true;
}

bool WorldPlugin::resetAllWorldSystems() {
    for (auto* world : worldSystems_) {
        if (!world->reset()) {
            return false;
        }
    }
    return true;
}

} // namespace plugin
} // namespace nlm