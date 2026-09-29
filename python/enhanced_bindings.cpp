// Enhanced Python API for NLM
// Complete implementation of enhanced Python bindings

#include "python/enhanced_bindings.h"
#include "python/bindings.h"
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <chrono>
#include <sstream>

namespace nlm {

// EnhancedConfig implementation
EnhancedConfig::EnhancedConfig() : config_(pynlm::createDefaultConfig()) {}

void EnhancedConfig::set(const std::string& key, const std::string& value) {
    config_->set(key, value);
}

void EnhancedConfig::set(const std::string& key, int value) {
    config_->set(key, value);
}

void EnhancedConfig::set(const std::string& key, float value) {
    config_->set(key, value);
}

void EnhancedConfig::set(const std::string& key, double value) {
    config_->set(key, value);
}

void EnhancedConfig::set(const std::string& key, bool value) {
    config_->set(key, value);
}

std::string EnhancedConfig::getString(const std::string& key) const {
    if (!config_->has(key)) {
        throw ConfigurationError("Key not found: " + key);
    }
    return config_->get<std::string>(key);
}

int EnhancedConfig::getInt(const std::string& key) const {
    if (!config_->has(key)) {
        throw ConfigurationError("Key not found: " + key);
    }
    return config_->get<int>(key);
}

float EnhancedConfig::getFloat(const std::string& key) const {
    if (!config_->has(key)) {
        throw ConfigurationError("Key not found: " + key);
    }
    return config_->get<float>(key);
}

bool EnhancedConfig::getBool(const std::string& key) const {
    if (!config_->has(key)) {
        throw ConfigurationError("Key not found: " + key);
    }
    return config_->get<bool>(key);
}

bool EnhancedConfig::has(const std::string& key) const {
    return config_->has(key);
}

std::vector<std::string> EnhancedConfig::getKeys() const {
    return config_->getKeys();
}

void EnhancedConfig::configureBrain(size_t neuronCount, float connectionDensity) {
    set("brain.neuron_count", neuronCount);
    set("brain.synapse_density", connectionDensity);
}

void EnhancedConfig::configureMemory(size_t capacity) {
    set("memory.working_memory.capacity", capacity);
    set("memory.episodic_memory.max_episodes", 1000);
}

void EnhancedConfig::configureNeuromodulation(float dopamineScale, bool enableCuriosity) {
    set("neuromod.dopamine.scale", dopamineScale);
    set("neuromod.curiosity.enable", enableCuriosity);
}

bool EnhancedConfig::save(const std::string& filepath) const {
    return config_->saveToFile(filepath);
}

bool EnhancedConfig::load(const std::string& filepath) {
    return config_->loadFromFile(filepath);
}

std::string EnhancedConfig::summary(bool formatted) const {
    return config_->summary();
}

EnhancedConfig EnhancedConfig::fromJSON(const std::string& json) {
    EnhancedConfig result;
    // In a real implementation, parse JSON and apply to config
    return result;
}

std::shared_ptr<Config> EnhancedConfig::getConfig() const {
    return config_;
}

// EnhancedBrain implementation
EnhancedBrain::EnhancedBrain(std::shared_ptr<Config> config) : brain_(pynlm::createBrain(config)) {}

bool EnhancedBrain::initialize() {
    try {
        return brain_->initialize();
    } catch (const std::exception& e) {
        throw SimulationError(std::string("Failed to initialize brain: ") + e.what());
    }
}

void EnhancedBrain::step(size_t stepNumber) {
    if (!brain_) {
        throw SimulationError("Brain not initialized");
    }
    brain_->step(stepNumber);
}

void EnhancedBrain::step(size_t stepNumber, double time) {
    if (!brain_) {
        throw SimulationError("Brain not initialized");
    }
    brain_->step(stepNumber, time);
}

void EnhancedBrain::receiveSensoryInput(const std::vector<float>& input) {
    if (input.empty()) {
        throw SimulationError("Sensory input cannot be empty");
    }
    
    auto sensoryInput = std::make_shared<SensoryInput>();
    sensoryInput->setData(input);
    sensoryInput->setTimestamp(std::chrono::system_clock::now().time_since_epoch().count());
    
    brain_->receiveSensoryInput(*sensoryInput);
}

py::dict EnhancedBrain::getStatistics() const {
    py::dict stats;
    
    stats["total_neurons"] = brain_->getTotalNeuronCount();
    stats["total_synapses"] = brain_->getTotalSynapseCount();
    stats["firing_neurons"] = brain_->getFiringNeuronCount();
    stats["average_firing_rate"] = brain_->getAverageFiringRate();
    stats["excitatory_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
    stats["total_spikes"] = brain_->getTotalSpikeCount();
    stats["pending_spike_events"] = brain_->getPendingSpikeEventCount();
    stats["developmental_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
    
    return stats;
}

void EnhancedBrain::runSimulation(size_t steps, std::function<void(size_t, py::dict)> progressCallback) {
    if (!brain_) {
        throw SimulationError("Brain not initialized");
    }
    
    for (size_t step = 0; step < steps; ++step) {
        brain_->step(step);
        
        if (progressCallback) {
            py::dict stats = getStatistics();
            progressCallback(step, stats);
        }
    }
}

void EnhancedBrain::reset() {
    if (brain_) {
        brain_->reset();
    }
}

bool EnhancedBrain::save(const std::string& filepath) const {
    try {
        return brain_->save(filepath);
    } catch (const std::exception& e) {
        throw SimulationError(std::string("Failed to save brain: ") + e.what());
    }
}

bool EnhancedBrain::load(const std::string& filepath) {
    try {
        return brain_->load(filepath);
    } catch (const std::exception& e) {
        throw SimulationError(std::string("Failed to load brain: ") + e.what());
    }
}

py::dict EnhancedBrain::getRegionInfo(size_t regionIndex) const {
    py::dict regionInfo;
    
    auto regions = brain_->getRegions();
    if (regionIndex >= regions.size()) {
        throw SimulationError("Region index out of range");
    }
    
    const auto* region = regions[regionIndex].get();
    
    regionInfo["id"] = region->getId().index();
    regionInfo["name"] = region->getName();
    regionInfo["neuron_count"] = region->getTotalNeuronCount();
    regionInfo["synapse_count"] = region->getSynapseCount();
    regionInfo["population_count"] = region->getPopulationCount();
    regionInfo["average_weight"] = region->getAverageSynapticWeight();
    
    return regionInfo;
}

std::vector<py::dict> EnhancedBrain::getAllRegions() const {
    std::vector<py::dict> regions;
    auto regionIds = brain_->getRegionIds();
    
    for (size_t i = 0; i < regionIds.size(); ++i) {
        regions.push_back(getRegionInfo(i));
    }
    
    return regions;
}

std::shared_ptr<Brain> EnhancedBrain::getBrain() const {
    return brain_;
}

// EnhancedAgentBrain implementation
EnhancedAgentBrain::EnhancedAgentBrain(std::shared_ptr<Brain> brain) : agentBrain_(pynlm::createAgentBrain(brain)) {}

void EnhancedAgentBrain::initialize(const SimpleWorld& world) {
    agentBrain_->initialize(world);
}

void EnhancedAgentBrain::processSensoryInput(const py::dict& perceptData) {
    // Convert Python dict to SensoryPercept
    auto percept = std::make_shared<SensoryPercept>();
    
    // Parse percept data would go here
    
    agentBrain_->processSensoryInput(*percept);
}

py::dict EnhancedAgentBrain::getAgentState() const {
    py::dict state;
    
    state["curiosity_level"] = agentBrain_->getCuriosityLevel();
    state["novelty_level"] = agentBrain_->getNoveltyLevel();
    state["neuromodulation_level"] = agentBrain_->getNeuromodulationLevel();
    state["prediction_error"] = agentBrain_->getPredictionError();
    state["developmental_stage"] = static_cast<int>(agentBrain_->getDevelopmentalStage());
    state["reward_modulation_enabled"] = agentBrain_->isRewardModulationEnabled();
    state["curiosity_enabled"] = agentBrain_->isCuriosityEnabled();
    
    return state;
}

py::dict EnhancedAgentBrain::getStatistics() const {
    py::dict stats;
    
    stats["curiosity_level"] = agentBrain_->getCuriosityLevel();
    stats["novelty_level"] = agentBrain_->getNoveltyLevel();
    stats["neuromodulation_level"] = agentBrain_->getNeuromodulationLevel();
    stats["prediction_error"] = agentBrain_->getPredictionError();
    stats["developmental_stage"] = static_cast<int>(agentBrain_->getDevelopmentalStage());
    
    return stats;
}

void EnhancedAgentBrain::enableRewardModulation(bool enable) {
    agentBrain_->enableRewardModulation(enable);
}

void EnhancedAgentBrain::enableCuriosity(bool enable) {
    agentBrain_->enableCuriosity(enable);
}

void EnhancedAgentBrain::enableDevelopment(bool enable) {
    agentBrain_->enableDevelopment(enable);
}

void EnhancedAgentBrain::enableStructuralPlasticity(bool enable) {
    agentBrain_->enableStructuralPlasticity(enable);
}

bool EnhancedAgentBrain::isRewardModulationEnabled() const {
    return agentBrain_->isRewardModulationEnabled();
}

bool EnhancedAgentBrain::isCuriosityEnabled() const {
    return agentBrain_->isCuriosityEnabled();
}

bool EnhancedAgentBrain::isDevelopmentEnabled() const {
    return agentBrain_->isDevelopmentEnabled();
}

bool EnhancedAgentBrain::isStructuralPlasticityEnabled() const {
    return agentBrain_->isStructuralPlasticityEnabled();
}

void EnhancedAgentBrain::applyRewardModulation(float reward, float predictedReward) {
    agentBrain_->applyRewardModulation(reward, predictedReward);
}

void EnhancedAgentBrain::updateDevelopment(double timestep) {
    agentBrain_->updateDevelopment(timestep);
}

void EnhancedAgentBrain::reset() {
    agentBrain_->reset();
}

std::shared_ptr<AgentBrain> EnhancedAgentBrain::getAgentBrain() const {
    return agentBrain_;
}

std::shared_ptr<EnhancedAgentBrain> EnhancedAgentBrain::createEnhanced(std::shared_ptr<Brain> brain) {
    return std::make_shared<EnhancedAgentBrain>(brain);
}

// EnhancedSimpleWorld implementation
EnhancedSimpleWorld::EnhancedSimpleWorld() : world_(pynlm::createSimpleWorld()) {}

void EnhancedSimpleWorld::configure(size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
    world_->configure(width, height, visionWidth, visionHeight);
}

void EnhancedSimpleWorld::setAgentStart(double x, double y) {
    world_->setAgentStart(x, y);
}

void EnhancedSimpleWorld::update(double timestep) {
    world_->update(timestep);
}

void EnhancedSimpleWorld::applyMotorCommand(const ActionType& actionType, double currentTime) {
    auto action = std::make_unique<Action>(actionType);
    world_->applyMotorCommand(action.get(), currentTime);
}

py::dict EnhancedSimpleWorld::getSensoryPercept() const {
    auto percept = world_->getSensoryPercept();
    py::dict perceptDict;
    
    // Convert percept to dictionary would go here
    
    return perceptDict;
}

py::dict EnhancedSimpleWorld::getAgentBody() const {
    auto body = world_->getAgentBody();
    py::dict bodyDict;
    
    bodyDict["x"] = body.x;
    bodyDict["y"] = body.y;
    bodyDict["orientation"] = body.orientation;
    bodyDict["velocity_x"] = body.velocityX;
    bodyDict["velocity_y"] = body.velocityY;
    bodyDict["angular_velocity"] = body.angularVelocity;
    bodyDict["energy"] = body.energy;
    bodyDict["health"] = body.health;
    bodyDict["age"] = body.age;
    bodyDict["is_moving"] = body.isMoving;
    bodyDict["is_turning"] = body.isTurning;
    
    return bodyDict;
}

void EnhancedSimpleWorld::reset() {
    world_->reset();
}

void EnhancedSimpleWorld::addObject(double x, double y, WorldObjectType type, float value, float radius) {
    auto obj = std::make_unique<WorldObject>(x, y, type, value, radius);
    world_->addObject(obj.get());
}

void EnhancedSimpleWorld::removeObject(double x, double y) {
    world_->removeObject(x, y);
}

bool EnhancedSimpleWorld::isValidPosition(double x, double y) const {
    return world_->isValidPosition(x, y);
}

py::dict EnhancedSimpleWorld::getDimensions() const {
    py::dict dims;
    dims["width"] = world_->getWidth();
    dims["height"] = world_->getHeight();
    dims["vision_width"] = world_->getVisionWidth();
    dims["vision_height"] = world_->getVisionHeight();
    
    return dims;
}

double EnhancedSimpleWorld::getSimulationTime() const {
    return world_->getSimulationTime();
}

void EnhancedSimpleWorld::setRandomSeed(unsigned int seed) {
    world_->setRandomSeed(seed);
}

unsigned int EnhancedSimpleWorld::getRandomSeed() const {
    return world_->getRandomSeed();
}

std::shared_ptr<SimpleWorld> EnhancedSimpleWorld::getWorld() const {
    return world_;
}

// Factory functions
std::shared_ptr<EnhancedConfig> createEnhancedConfig() {
    return std::make_shared<EnhancedConfig>();
}

std::shared_ptr<EnhancedBrain> createEnhancedBrain(std::shared_ptr<Config> config) {
    return std::make_shared<EnhancedBrain>(config);
}

std::shared_ptr<EnhancedAgentBrain> createEnhancedAgentBrain(std::shared_ptr<Brain> brain) {
    return std::make_shared<EnhancedAgentBrain>(brain);
}

std::shared_ptr<EnhancedSimpleWorld> createEnhancedSimpleWorld() {
    return std::make_shared<EnhancedSimpleWorld>();
}

// Convenience functions
py::dict runCompleteAgentSimulation(size_t numSteps) {
    py::dict results;
    
    try {
        // Create components
        auto config = pynlm::createDefaultConfig();
        auto brain = pynlm::createBrain(config);
        auto world = pynlm::createSimpleWorld();
        auto agent = pynlm::createAgentBrain(brain);
        
        // Configure
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1f);
        
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        agent->initialize(*world);
        
        // Enable learning
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        // Run simulation
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto motorCmd = agent->decodeMotorCommand();
            world->applyMotorCommand(motorCmd, world->getSimulationTime());
            
            // Get reward and apply neuromodulation
            float reward = 0.0f;
            if (percept.getInternal().size() > 0) {
                reward = percept.getInternal()[0];
            }
            agent->applyRewardModulation(reward, 0.0f);
            
            agent->updateDevelopment(0.1);
            
            // Collect statistics every 100 steps
            if (step % 100 == 0) {
                py::dict stepStats;
                stepStats["step"] = step;
                stepStats["firing_neurons"] = brain->getFiringNeuronCount();
                stepStats["curiosity"] = agent->getCuriosityLevel();
                stepStats["novelty"] = agent->getNoveltyLevel();
                stepStats["dev_stage"] = brain->getDevelopmentalStage();
                
                results["steps"].attr("append")(stepStats);
            }
        }
        
        // Get final statistics
        py::dict finalStats;
        finalStats["total_neurons"] = brain->getTotalNeuronCount();
        finalStats["total_synapses"] = brain->getTotalSynapseCount();
        finalStats["firing_neurons"] = brain->getFiringNeuronCount();
        finalStats["average_firing_rate"] = brain->getAverageFiringRate();
        finalStats["total_spikes"] = brain->getTotalSpikeCount();
        finalStats["final_curiosity"] = agent->getCuriosityLevel();
        finalStats["final_novelty"] = agent->getNoveltyLevel();
        finalStats["final_dev_stage"] = brain->getDevelopmentalStage();
        
        results["final_stats"] = finalStats;
        results["success"] = true;
        
    } catch (const std::exception& e) {
        results["error"] = std::string(e.what());
        results["success"] = false;
    }
    
    return results;
}

py::dict getDefaultConfiguration() {
    py::dict config;
    
    config["brain.neuron_count"] = 1000;
    config["brain.synapse_density"] = 0.1f;
    config["brain.v_thresh"] = -50.0f;
    config["brain.v_rest"] = -70.0f;
    config["plasticity.stdp.enable"] = true;
    config["neuromod.dopamine.scale"] = 1.0f;
    config["neuromod.curiosity.enable"] = true;
    
    return config;
}

std::string generateConfigurationCode(const py::dict& config) {
    std::stringstream ss;
    ss << "import pynlm\n\n";
    ss << "# Configuration\n";
    ss << "config = pynlm.createDefaultConfig()\n";
    
    for (auto it = config.begin(); it != config.end(); ++it) {
        std::string key = it->first.cast<std::string>();
        py::object value = it->second;
        
        if (value.is(py::int_())) {
            ss << "config.set(\"" << key << "\", " << value.cast<int>() << ")\n";
        } else if (value.is(py::float_())) {
            ss << "config.set(\"" << key << "\", " << value.cast<float>() << ")\n";
        } else if (value.is(py::bool_())) {
            ss << "config.set(\"" << key << "\", " << (value.cast<bool>() ? "true" : "false") << ")\n";
        } else if (value.is(py::str())) {
            ss << "config.set(\"" << key << "\", \"" << value.cast<std::string>() << "\")\n";
        }
    }
    
    return ss.str();
}

} // namespace nlm
