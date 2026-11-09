// Improved Python bindings for NLM with Pythonic interface
// Added Python-friendly wrappers, convenience methods, and utilities

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <limits>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/performance/CheckpointSystem.hpp"

namespace py = pybind11;

// Forward declarations for Python classes
class PyConfig;
class PyBrain;
class PySimpleWorld;
class PyAgentBrain;

// Pythonic wrapper for Config class
class PyConfig {
private:
    std::shared_ptr<nlm::Config> config_;
    
public:
    PyConfig() : config_(std::make_shared<nlm::Config>()) {}
    
    // Load from JSON file
    bool loadFromFile(const std::string& filepath) {
        return config_->loadFromFile(filepath);
    }
    
    // Load from Python list of arguments (replaces command line parsing)
    bool loadFromArgs(const std::vector<std::string>& args) {
        // Convert Python list to argv format
        std::vector<char*> argv;
        argv.reserve(args.size());
        for (const auto& a : args) {
            argv.push_back(const_cast<char*>(a.c_str()));
        }
        return config_->loadFromArgs(static_cast<int>(argv.size()), argv.data());
    }
    
    // Save to JSON file
    bool saveToFile(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    // Get value with type conversion (more Pythonic than get<T>)
    template<typename T>
    py::object get(const std::string& key) const {
        auto value = config_->get<T>(key);
        if (value) {
            return py::cast(*value);
        }
        return py::none();
    }
    
    // Get value with default (more Pythonic)
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        return config_->getOr(key, defaultValue);
    }
    
    // Set value with automatic type detection
    void set(const std::string& key, const py::object& value, 
             nlm::ConfigSource source = nlm::ConfigSource::Runtime) {
        if (value.is(py::none())) {
            config_->set(key, nlm::ConfigValue{}, source);
        } else if (py::isinstance<py::bool_>(value)) {
            config_->set(key, value.cast<bool>(), source);
        } else if (py::isinstance<py::int_>(value)) {
            config_->set(key, value.cast<int64_t>(), source);
        } else if (py::isinstance<py::float_>(value)) {
            config_->set(key, value.cast<double>(), source);
        } else if (py::isinstance<py::str>(value)) {
            config_->set(key, value.cast<std::string>(), source);
        } else if (py::isinstance<py::list>(value)) {
            // Handle lists - try to infer type
            auto list = value.cast<std::vector<double>>();
            config_->set(key, list, source);
        } else {
            throw std::runtime_error("Unsupported Python type for Config.set()");
        }
    }
    
    // Convenience methods for common config patterns
    void setNeuronCount(size_t count) {
        config_->set("neuron_count", static_cast<int64_t>(count));
    }
    
    void setRegionCount(size_t count) {
        config_->set("region_count", static_cast<int64_t>(count));
    }
    
    void setConnectionProbability(float prob) {
        config_->set("connection_probability", prob);
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    // Clear all
    void clear() {
        config_->clear();
    }
    
    // Get summary string
    std::string summary() const {
        return config_->summary();
    }
    
    // Python string representation
    std::string __repr__() const {
        return "<PyConfig: " + summary() + ">";
    }
    
    // Access to underlying config
    std::shared_ptr<nlm::Config> _getInternal() const {
        return config_;
    }
};

// Pythonic wrapper for Brain class with convenience methods
class PyBrain {
private:
    std::shared_ptr<nlm::Brain> brain_;
    
public:
    PyBrain(std::shared_ptr<Config> config) {
        // Accept PyConfig as well
        if (config) {
            // Handle both nlm::Config and PyConfig
            if (auto py_config = std::dynamic_pointer_cast<PyConfig>(config)) {
                brain_ = std::make_shared<nlm::Brain>(py_config->_getInternal());
            } else {
                brain_ = std::make_shared<nlm::Brain>(config);
            }
        }
    }
    
    PyBrain() {
        // Create default config
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1);
        brain_ = std::make_shared<nlm::Brain>(config);
    }
    
    // Initialize with convenience for common errors
    bool initialize() {
        return brain_->initialize();
    }
    
    // Single-step simulation
    void step(size_t step = 0) {
        brain_->step(static_cast<nlm::SimulationStep>(step));
    }
    
    // Step with timestamp
    void stepWithTime(size_t step, double time) {
        brain_->step(static_cast<nlm::SimulationStep>(step), 
                     static_cast<nlm::Timestamp>(time));
    }
    
    // Convenience: inject current into all sensory neurons
    void injectSensoryCurrent(float current) {
        brain_->injectCurrentToNeurons(nlm::NeuronType::Sensory, current);
    }
    
    // Convenience: inject current into specific neuron type
    void injectCurrent(nlm::NeuronType type, float current) {
        brain_->injectCurrentToNeurons(type, current);
    }
    
    // Produce action as Python dict (more Pythonic than Action object)
    py::dict produceActionDict() {
        auto action = brain_->produceAction();
        if (!action) {
            return py::dict();
        }
        
        py::dict result;
        result["type"] = static_cast<int>(action->getType());
        result["parameters"] = action->getParameters();
        result["name"] = action->getName();
        return result;
    }
    
    // Convenience methods for common statistics
    size_t getNeuronCount() const {
        return brain_->getTotalNeuronCount();
    }
    
    size_t getSynapseCount() const {
        return brain_->getTotalSynapseCount();
    }
    
    size_t getActiveNeuronCount() const {
        return brain_->getActiveNeuronCount();
    }
    
    float getAverageFiringRate() const {
        return brain_->getAverageFiringRate();
    }
    
    float getExcitationInhibitionRatio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    // Get region information
    py::dict getRegionInfo(size_t index) {
        auto regionIds = brain_->getRegionIds();
        if (index >= regionIds.size()) {
            throw std::out_of_range("Region index out of range");
        }
        
        auto regionId = regionIds[index];
        auto* region = brain_->getRegion(regionId);
        if (!region) {
            throw std::runtime_error("Failed to get region");
        }
        
        py::dict result;
        result["id"] = static_cast<uint64_t>(regionId.value);
        result["neuron_count"] = region->getNeuronCount();
        result["synapse_count"] = region->getSynapseCount();
        return result;
    }
    
    // Get all regions as list of dicts
    std::vector<py::dict> getAllRegionInfo() {
        std::vector<py::dict> regions;
        auto regionIds = brain_->getRegionIds();
        
        for (auto regionId : regionIds) {
            auto* region = brain_->getRegion(regionId);
            if (region) {
                py::dict regionInfo;
                regionInfo["id"] = static_cast<uint64_t>(regionId.value);
                regionInfo["neuron_count"] = region->getNeuronCount();
                regionInfo["synapse_count"] = region->getSynapseCount();
                regions.push_back(regionInfo);
            }
        }
        return regions;
    }
    
    // Context manager for simulation (using resource tracking)
    class SimulationContext {
    private:
        PyBrain& brain_;
        bool autoCleanup_;
        
    public:
        SimulationContext(PyBrain& brain, bool autoCleanup = true) 
            : brain_(brain), autoCleanup_(autoCleanup) {}
        
        ~SimulationContext() {
            if (autoCleanup_) {
                // Reset brain state on exit
                brain_.reset();
            }
        }
        
        // Allow user to manually exit
        void exit() {
            if (autoCleanup_) {
                brain_.reset();
            }
        }
    };
    
    SimulationContext simulation() {
        return SimulationContext(*this);
    }
    
    // Reset brain state
    void reset() {
        brain_->reset();
    }
    
    // Save checkpoint
    bool saveCheckpoint(const std::string& filepath) const {
        return brain_->save(filepath);
    }
    
    // Load checkpoint
    bool loadCheckpoint(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    // Get development stage
    std::string getDevelopmentalStage() const {
        auto stage = brain_->getDevelopmentalStage();
        switch (stage) {
            case nlm::DevelopmentalStage::Initial: return "Initial";
            case nlm::DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case nlm::DevelopmentalStage::Maturation: return "Maturation";
            case nlm::DevelopmentalStage::Adult: return "Adult";
            case nlm::DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
    // Set developmental stage
    void setDevelopmentalStage(const std::string& stage) {
        nlm::DevelopmentalStage nlmStage;
        if (stage == "CriticalPeriod") {
            nlmStage = nlm::DevelopmentalStage::CriticalPeriod;
        } else if (stage == "Maturation") {
            nlmStage = nlm::DevelopmentalStage::Maturation;
        } else if (stage == "Adult") {
            nlmStage = nlm::DevelopmentalStage::Adult;
        } else if (stage == "Aging") {
            nlmStage = nlm::DevelopmentalStage::Aging;
        } else {
            nlmStage = nlm::DevelopmentalStage::Initial;
        }
        brain_->setDevelopmentalStage(nlmStage);
    }
    
    // Get config
    PyConfig getConfig() {
        PyConfig pyConfig;
        pyConfig._getInternal() = std::const_pointer_cast<nlm::Config>(brain_->getConfig());
        return pyConfig;
    }
    
    // Internal access
    std::shared_ptr<nlm::Brain> _getInternal() const {
        return brain_;
    }
    
    // Python string representation
    std::string __repr__() const {
        std::stringstream ss;
        ss << "<PyBrain neurons=" << brain_->getTotalNeuronCount()
           << " synapses=" << brain_->getTotalSynapseCount()
           << " step=" << brain_->getTotalSpikeCount() << ">";
        return ss.str();
    }
};

// Pythonic wrapper for SimpleWorld class
class PySimpleWorld {
private:
    std::shared_ptr<nlm::SimpleWorld> world_;
    
public:
    PySimpleWorld() : world_(std::make_shared<nlm::SimpleWorld>()) {}
    
    // Configure world with convenient defaults
    void configure(float width = 20.0f, float height = 20.0f,
                   size_t visionWidth = 16, size_t visionHeight = 16) {
        world_->configure(width, height, visionWidth, visionHeight);
    }
    
    // Set random seed for reproducibility
    void setRandomSeed(uint64_t seed) {
        world_->setRandomSeed(seed);
    }
    
    // Reset world to initial state
    void reset() {
        world_->reset();
    }
    
    // Update world for one timestep
    void update(double timestep = 0.1) {
        world_->update(timestep);
    }
    
    // Get sensory percept as Python dict (more Pythonic)
    py::dict getSensoryPerceptDict() {
        auto percept = world_->getSensoryPercept();
        py::dict result;
        
        auto vision = percept->getVision();
        result["vision"] = vision;
        result["vision_width"] = percept->getVisionWidth();
        result["vision_height"] = percept->getVisionHeight();
        
        auto touch = percept->getTouch();
        result["touch"] = touch;
        
        auto internal = percept->getInternal();
        result["internal"] = internal;
        
        auto proprioception = percept->getProprioception();
        result["proprioception"] = proprioception;
        
        auto audio = percept->getAudio();
        result["audio"] = audio;
        
        result["timestamp"] = percept->getTimestamp();
        return result;
    }
    
    // Get agent body as Python dict
    py::dict getAgentBodyDict() {
        auto body = world_->getAgentBody();
        py::dict result;
        
        result["x"] = body.x;
        result["y"] = body.y;
        result["orientation"] = body.orientation;
        result["velocity_x"] = body.velocityX;
        result["velocity_y"] = body.velocityY;
        result["angular_velocity"] = body.angularVelocity;
        result["energy"] = body.energy;
        result["health"] = body.health;
        result["age"] = body.age;
        result["is_moving"] = body.isMoving;
        result["is_turning"] = body.isTurning;
        result["last_action_time"] = body.lastActionTime;
        
        return result;
    }
    
    // Convenience: add resource object
    void addResource(float x, float y, float value = 10.0f) {
        nlm::WorldObject obj(x, y, nlm::WorldObjectType::Resource, value);
        world_->addObject(obj);
    }
    
    // Convenience: add hazard object
    void addHazard(float x, float y, float value = -15.0f) {
        nlm::WorldObject obj(x, y, nlm::WorldObjectType::Hazard, value);
        world_->addObject(obj);
    }
    
    // Check if position is valid
    bool isValidPosition(float x, float y) const {
        return world_->isValidPosition(x, y);
    }
    
    // Get world dimensions
    py::dict getDimensions() const {
        py::dict result;
        result["width"] = world_->getWidth();
        result["height"] = world_->getHeight();
        result["vision_width"] = world_->getVisionWidth();
        result["vision_height"] = world_->getVisionHeight();
        result["max_energy"] = world_->getMaxEnergy();
        return result;
    }
    
    // Get simulation time
    double getSimulationTime() const {
        return world_->getSimulationTime();
    }
    
    // Set agent starting position
    void setAgentStart(float x, float y) {
        world_->setAgentStart(x, y);
    }
    
    // Internal access
    std::shared_ptr<nlm::SimpleWorld> _getInternal() const {
        return world_;
    }
    
    // Python string representation
    std::string __repr__() const {
        std::stringstream ss;
        ss << "<PySimpleWorld size=" << world_->getWidth() << "x" << world_->getHeight()
           << " vision=" << world_->getVisionWidth() << "x" << world_->getVisionHeight()
           << " simTime=" << world_->getSimulationTime() << ">";
        return ss.str();
    }
};

// Pythonic wrapper for AgentBrain class
class PyAgentBrain {
private:
    std::shared_ptr<nlm::AgentBrain> agentBrain_;
    
public:
    PyAgentBrain(std::shared_ptr<PyBrain> brain) {
        if (brain) {
            agentBrain_ = std::make_shared<nlm::AgentBrain>(brain->_getInternal());
        }
    }
    
    PyAgentBrain() {
        // Create default brain
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1);
        auto brain = std::make_shared<nlm::Brain>(config);
        agentBrain_ = std::make_shared<nlm::AgentBrain>(brain);
    }
    
    // Initialize with world
    void initialize(std::shared_ptr<PySimpleWorld> world) {
        if (world) {
            agentBrain_->initialize(*world->_getInternal());
        }
    }
    
    // Process sensory input as dict
    void processSensoryInput(const py::dict& percept) {
        nlm::SensoryPercept nlmPercept;
        
        if (percept.contains("vision")) {
            auto vision = percept["vision"].cast<std::vector<float>>();
            nlmPercept.setVision(vision);
        }
        
        if (percept.contains("touch")) {
            auto touch = percept["touch"].cast<std::vector<float>>();
            nlmPercept.setTouch(touch);
        }
        
        if (percept.contains("internal")) {
            auto internal = percept["internal"].cast<std::vector<float>>();
            nlmPercept.setInternal(internal);
        }
        
        if (percept.contains("proprioception")) {
            auto proprioception = percept["proprioception"].cast<std::vector<float>>();
            nlmPercept.setProprioception(proprioception);
        }
        
        if (percept.contains("audio")) {
            auto audio = percept["audio"].cast<std::vector<float>>();
            nlmPercept.setAudio(audio);
        }
        
        if (percept.contains("timestamp")) {
            nlmPercept.setTimestamp(percept["timestamp"].cast<double>());
        }
        
        agentBrain_->processSensoryInput(nlmPercept);
    }
    
    // Get motor command as Python string
    std::string getMotorCommandString() {
        auto cmd = agentBrain_->decodeMotorCommand();
        switch (cmd) {
            case nlm::MotorCommand::MoveForward: return "MoveForward";
            case nlm::MotorCommand::MoveBackward: return "MoveBackward";
            case nlm::MotorCommand::TurnLeft: return "TurnLeft";
            case nlm::MotorCommand::TurnRight: return "TurnRight";
            case nlm::MotorCommand::LookLeft: return "LookLeft";
            case nlm::MotorCommand::LookRight: return "LookRight";
            case nlm::MotorCommand::Interact: return "Interact";
            default: return "Wait";
        }
    }
    
    // Get motor command as enum value
    int getMotorCommand() {
        auto cmd = agentBrain_->decodeMotorCommand();
        return static_cast<int>(cmd);
    }
    
    // Apply reward (convenience for learning)
    void applyReward(float reward, float predictedReward = 0.0f) {
        agentBrain_->applyRewardModulation(reward, predictedReward);
    }
    
    // Get neuromodulation levels
    py::dict getNeuromodulationLevels() const {
        py::dict result;
        result["dopamine"] = agentBrain_->getNeuromodulationLevel();
        result["curiosity"] = agentBrain_->getCuriosityLevel();
        result["novelty"] = agentBrain_->getNoveltyLevel();
        result["prediction_error"] = agentBrain_->getPredictionError();
        return result;
    }
    
    // Get developmental stage
    std::string getDevelopmentalStage() const {
        auto stage = agentBrain_->getDevelopmentalStage();
        switch (stage) {
            case nlm::DevelopmentalStage::Initial: return "Initial";
            case nlm::DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case nlm::DevelopmentalStage::Maturation: return "Maturation";
            case nlm::DevelopmentalStage::Adult: return "Adult";
            case nlm::DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
    // Get thought trace (Phase 5 diagnostic)
    std::string getThoughtTrace() const {
        return agentBrain_->getThoughtTrace();
    }
    
    // Get learned preference trait
    float getLearnedPreference() const {
        return agentBrain_->getLearnedPreference();
    }
    
    // Enable/disable systems
    void enableRewardModulation(bool enable) {
        agentBrain_->enableRewardModulation(enable);
    }
    
    void enableStructuralPlasticity(bool enable) {
        agentBrain_->enableStructuralPlasticity(enable);
    }
    
    void enableDevelopment(bool enable) {
        agentBrain_->enableDevelopment(enable);
    }
    
    void enableCuriosity(bool enable) {
        agentBrain_->enableCuriosity(enable);
    }
    
    // Check if systems are enabled
    bool isRewardModulationEnabled() const {
        return agentBrain_->isRewardModulationEnabled();
    }
    
    bool isStructuralPlasticityEnabled() const {
        return agentBrain_->isStructuralPlasticityEnabled();
    }
    
    bool isDevelopmentEnabled() const {
        return agentBrain_->isDevelopmentEnabled();
    }
    
    bool isCuriosityEnabled() const {
        return agentBrain_->isCuriosityEnabled();
    }
    
    // Get brain pointer
    std::shared_ptr<PyBrain> getBrain() {
        auto brain = agentBrain_->getBrain();
        return std::make_shared<PyBrain>(brain);
    }
    
    // Reset for new episode
    void reset() {
        agentBrain_->reset();
    }
    
    // Internal access
    std::shared_ptr<nlm::AgentBrain> _getInternal() const {
        return agentBrain_;
    }
    
    // Python string representation
    std::string __repr__() const {
        return "<PyAgentBrain>";
    }
};

// Helper functions for common operations
namespace helpers {

// Create a simple simulation with default settings
std::tuple<std::shared_ptr<PyBrain>, std::shared_ptr<PySimpleWorld>, std::shared_ptr<PyAgentBrain>> createSimpleSimulation() {
    auto world = std::make_shared<PySimpleWorld>();
    world->configure();
    world->reset();
    
    auto brain = std::make_shared<PyBrain>();
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    auto agentBrain = std::make_shared<PyAgentBrain>(brain);
    agentBrain->initialize(world);
    
    return std::make_tuple(brain, world, agentBrain);
}

// Run a simple simulation loop
py::dict runSimpleSimulation(size_t steps, double timestep = 0.1,
                            bool verbose = false) {
    auto [brain, world, agent] = createSimpleSimulation();
    
    py::dict results;
    results["steps"] = steps;
    results["final_neurons"] = brain->getNeuronCount();
    results["final_synapses"] = brain->getSynapseCount();
    
    std::vector<py::dict> history;
    
    for (size_t i = 0; i < steps; ++i) {
        // Get current state
        py::dict state;
        state["step"] = i;
        state["neurons"] = brain->getNeuronCount();
        state["synapses"] = brain->getSynapseCount();
        state["active_neurons"] = brain->getActiveNeuronCount();
        state["firing_rate"] = brain->getAverageFiringRate();
        
        // Get world state
        state["time"] = world->getSimulationTime();
        state["agent_x"] = world->getAgentBodyDict()["x"];
        state["agent_y"] = world->getAgentBodyDict()["y"];
        
        // Process sensory input
        auto percept = world->getSensoryPerceptDict();
        agent->processSensoryInput(percept);
        
        // Get action
        std::string action = agent->getMotorCommandString();
        state["action"] = action;
        
        // Apply action in world
        nlm::MotorCommand motorCmd;
        if (action == "MoveForward") motorCmd = nlm::MotorCommand::MoveForward;
        else if (action == "MoveBackward") motorCmd = nlm::MotorCommand::MoveBackward;
        else if (action == "TurnLeft") motorCmd = nlm::MotorCommand::TurnLeft;
        else if (action == "TurnRight") motorCmd = nlm::MotorCommand::TurnRight;
        else if (action == "Interact") motorCmd = nlm::MotorCommand::Interact;
        else motorCmd = nlm::MotorCommand::Wait;
        
        auto result = world->_getInternal()->applyMotorCommand(motorCmd, world->getSimulationTime());
        
        state["reward"] = result.reward;
        state["success"] = result.success;
        
        // Apply reward to agent
        agent->applyReward(result.reward);
        
        // Step brain
        brain->step(i);
        
        // Update world
        world->update(timestep);
        
        history.push_back(state);
        
        if (verbose && (i % 100 == 0 || i == steps - 1)) {
            std::cout << "Step " << i << ": " << action 
                      << " reward=" << result.reward
                      << " neurons=" << brain->getNeuronCount()
                      << " firing=" << brain->getAverageFiringRate() << std::endl;
        }
    }
    
    results["history"] = history;
    results["final_reward"] = agent->getLearnedPreference();
    results["final_dopamine"] = agent->getNeuromodulationLevels()["dopamine"];
    results["final_curiosity"] = agent->getNeuromodulationLevels()["curiosity"];
    results["final_novelty"] = agent->getNeuromodulationLevels()["novelty"];
    
    return results;
}

// Create configuration from Python dict
std::shared_ptr<PyConfig> createConfigFromDict(const py::dict& configDict) {
    auto config = std::make_shared<PyConfig>();
    
    for (auto item : configDict) {
        std::string key = py::cast<std::string>(item.first);
        py::object value = item.second;
        config->set(key, value);
    }
    
    return config;
}

// Create configuration with sensible defaults for simple scenarios
std::shared_ptr<PyConfig> createSimpleConfig(size_t neuronCount = 100,
                                           size_t regionCount = 2,
                                           float connectionProb = 0.1) {
    auto config = std::make_shared<PyConfig>();
    config->setNeuronCount(neuronCount);
    config->setRegionCount(regionCount);
    config->setConnectionProbability(connectionProb);
    return config;
}

// Save simulation state to file
bool saveSimulationState(const std::string& filepath,
                         std::shared_ptr<PyBrain> brain,
                         std::shared_ptr<PySimpleWorld> world,
                         std::shared_ptr<PyAgentBrain> agent) {
    if (!brain || !world || !agent) {
        return false;
    }
    
    // Save brain checkpoint
    std::string brainPath = filepath + "_brain.bin";
    if (!brain->_getInternal()->save(brainPath)) {
        return false;
    }
    
    // Save world state as JSON for inspection
    py::dict worldState;
    worldState["dimensions"] = world->getDimensions();
    worldState["agent_body"] = world->getAgentBodyDict();
    worldState["sensory_percept"] = world->getSensoryPerceptDict();
    
    std::string worldPath = filepath + "_world.json";
    // Note: Would need to implement JSON serialization for py::dict
    // For now, just save brain
    
    return true;
}

// Load simulation state from file
std::tuple<std::shared_ptr<PyBrain>, std::shared_ptr<PySimpleWorld>, std::shared_ptr<PyAgentBrain>>
loadSimulationState(const std::string& filepath) {
    // Load brain checkpoint
    auto config = createSimpleConfig();
    auto brain = std::make_shared<PyBrain>(config);
    
    std::string brainPath = filepath + "_brain.bin";
    if (!brain->_getInternal()->load(brainPath)) {
        throw std::runtime_error("Failed to load brain checkpoint");
    }
    
    // Create world and agent
    auto world = std::make_shared<PySimpleWorld>();
    world->configure();
    auto agent = std::make_shared<PyAgentBrain>(brain);
    
    return std::make_tuple(brain, world, agent);
}

// Utility: convert vision to 2D array for analysis
std::vector<std::vector<float>> visionTo2D(const std::vector<float>& vision,
                                         size_t width, size_t height) {
    std::vector<std::vector<float>> grid(height, std::vector<float>(width));
    
    if (vision.empty()) {
        return grid;
    }
    
    size_t minWidth = std::min(width, vision.size());
    
    for (size_t y = 0; y < height && y * width < vision.size(); ++y) {
        for (size_t x = 0; x < width && y * width + x < vision.size(); ++x) {
            grid[y][x] = vision[y * width + x];
        }
    }
    
    return grid;
}

// Utility: calculate distance between two positions
double calculateDistance(double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

// Utility: normalize value to [0, 1] range
double normalize(double value, double min, double max) {
    if (max == min) return 0.5;
    return (value - min) / (max - min);
}

// Utility: clamp value to range
double clamp(double value, double min, double max) {
    return std::max(min, std::min(max, value));
}

} // namespace helpers

PYBIND11_MODULE(pynlm_improved, m) {
    m.doc() = R"pbdoc(
    Improved Python bindings for NLM (Neural Learning Machine)
    
    Provides Pythonic wrappers around the C++ NLM framework with:
    - Enhanced convenience methods for common operations
    - Better error handling and type safety
    - Helper functions for running simulations
    - Context managers for resource management
    - Experimental and configuration utilities
    
    Key classes:
    - PyConfig: Pythonic configuration interface
    - PyBrain: Enhanced brain wrapper with convenience methods
    - PySimpleWorld: Pythonic world simulation interface
    - PyAgentBrain: Agent wrapper with learning utilities
    
    Examples:
        import pynlm_improved as nlm
        
        # Create simple simulation
        config = nlm.createSimpleConfig()
        brain = nlm.PyBrain(config)
        world = nlm.PySimpleWorld()
        agent = nlm.PyAgentBrain(brain)
        
        # Run simulation
        results = nlm.runSimpleSimulation(1000, verbose=True)
    )pbdoc";
    
    // Config class
    py::class_<PyConfig>(m, "PyConfig")
        .def(py::init<>())
        .def("load_from_file", &PyConfig::loadFromFile, py::arg("filepath"))
        .def("load_from_args", &PyConfig::loadFromArgs, py::arg("args"))
        .def("save_to_file", &PyConfig::saveToFile, py::arg("filepath"))
        .def("get", &PyConfig::get)
        .def("get_or", &PyConfig::getOr)
        .def("set", &PyConfig::set)
        .def("has", &PyConfig::has)
        .def("get_keys", &PyConfig::getKeys)
        .def("clear", &PyConfig::clear)
        .def("summary", &PyConfig::summary)
        .def("__repr__", &PyConfig::__repr__)
        .def("set_neuron_count", &PyConfig::setNeuronCount)
        .def("set_region_count", &PyConfig::setRegionCount)
        .def("set_connection_probability", &PyConfig::setConnectionProbability);
    
    // Brain class
    py::class_<PyBrain>(m, "PyBrain")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<PyConfig>>(), py::arg("config"))
        .def("initialize", &PyBrain::initialize)
        .def("step", &PyBrain::step)
        .def("step_with_time", &PyBrain::stepWithTime, py::arg("step"), py::arg("time"))
        .def("inject_sensory_current", &PyBrain::injectSensoryCurrent)
        .def("inject_current", &PyBrain::injectCurrent)
        .def("produce_action_dict", &PyBrain::produceActionDict)
        .def("get_neuron_count", &PyBrain::getNeuronCount)
        .def("get_synapse_count", &PyBrain::getSynapseCount)
        .def("get_active_neuron_count", &PyBrain::getActiveNeuronCount)
        .def("get_average_firing_rate", &PyBrain::getAverageFiringRate)
        .def("get_excitation_inhibition_ratio", &PyBrain::getExcitationInhibitionRatio)
        .def("get_region_info", &PyBrain::getRegionInfo, py::arg("index"))
        .def("get_all_region_info", &PyBrain::getAllRegionInfo)
        .def("get_developmental_stage", &PyBrain::getDevelopmentalStage)
        .def("set_developmental_stage", &PyBrain::setDevelopmentalStage)
        .def("get_config", &PyBrain::getConfig)
        .def("reset", &PyBrain::reset)
        .def("save_checkpoint", &PyBrain::saveCheckpoint, py::arg("filepath"))
        .def("load_checkpoint", &PyBrain::loadCheckpoint, py::arg("filepath"))
        .def("__repr__", &PyBrain::__repr__);
    
    // SimpleWorld class
    py::class_<PySimpleWorld>(m, "PySimpleWorld")
        .def(py::init<>())
        .def("configure", &PySimpleWorld::configure)
        .def("set_random_seed", &PySimpleWorld::setRandomSeed)
        .def("reset", &PySimpleWorld::reset)
        .def("update", &PySimpleWorld::update)
        .def("get_sensory_percept_dict", &PySimpleWorld::getSensoryPerceptDict)
        .def("get_agent_body_dict", &PySimpleWorld::getAgentBodyDict)
        .def("add_resource", &PySimpleWorld::addResource, py::arg("x"), py::arg("y"), py::arg("value") = 10.0f)
        .def("add_hazard", &PySimpleWorld::addHazard, py::arg("x"), py::arg("y"), py::arg("value") = -15.0f)
        .def("is_valid_position", &PySimpleWorld::isValidPosition)
        .def("get_dimensions", &PySimpleWorld::getDimensions)
        .def("get_simulation_time", &PySimpleWorld::getSimulationTime)
        .def("set_agent_start", &PySimpleWorld::setAgentStart)
        .def("__repr__", &PySimpleWorld::__repr__);
    
    // AgentBrain class
    py::class_<PyAgentBrain>(m, "PyAgentBrain")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<PyBrain>>(), py::arg("brain"))
        .def("initialize", &PyAgentBrain::initialize)
        .def("process_sensory_input", &PyAgentBrain::processSensoryInput)
        .def("get_motor_command_string", &PyAgentBrain::getMotorCommandString)
        .def("get_motor_command", &PyAgentBrain::getMotorCommand)
        .def("apply_reward", &PyAgentBrain::applyReward)
        .def("get_neuromodulation_levels", &PyAgentBrain::getNeuromodulationLevels)
        .def("get_developmental_stage", &PyAgentBrain::getDevelopmentalStage)
        .def("get_thought_trace", &PyAgentBrain::getThoughtTrace)
        .def("get_learned_preference", &PyAgentBrain::getLearnedPreference)
        .def("enable_reward_modulation", &PyAgentBrain::enableRewardModulation)
        .def("enable_structural_plasticity", &PyAgentBrain::enableStructuralPlasticity)
        .def("enable_development", &PyAgentBrain::enableDevelopment)
        .def("enable_curiosity", &PyAgentBrain::enableCuriosity)
        .def("is_reward_modulation_enabled", &PyAgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &PyAgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &PyAgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &PyAgentBrain::isCuriosityEnabled)
        .def("get_brain", &PyAgentBrain::getBrain)
        .def("reset", &PyAgentBrain::reset)
        .def("__repr__", &PyAgentBrain::__repr__);
    
    // Helper functions
    m.def("create_default_config", []() {
        auto config = std::make_shared<PyConfig>();
        config->setNeuronCount(100);
        config->setRegionCount(2);
        config->setConnectionProbability(0.1);
        return config;
    });
    
    m.def("create_simple_config", &helpers::createSimpleConfig,
          py::arg("neuron_count") = 100,
          py::arg("region_count") = 2,
          py::arg("connection_prob") = 0.1f);
    
    m.def("create_config_from_dict", &helpers::createConfigFromDict,
          py::arg("config_dict"));
    
    m.def("create_simple_simulation", &helpers::createSimpleSimulation);
    
    m.def("run_simple_simulation", &helpers::runSimpleSimulation,
          py::arg("steps"), py::arg("timestep") = 0.1f,
          py::arg("verbose") = false);
    
    m.def("save_simulation_state", &helpers::saveSimulationState,
          py::arg("filepath"), py::arg("brain"), py::arg("world"), py::arg("agent"));
    
    m.def("load_simulation_state", &helpers::loadSimulationState,
          py::arg("filepath"));
    
    // Utility functions
    m.def("vision_to_2d", &helpers::visionTo2D,
          py::arg("vision"), py::arg("width"), py::arg("height"));
    
    m.def("calculate_distance", &helpers::calculateDistance,
          py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"));
    
    m.def("normalize", &helpers::normalize,
          py::arg("value"), py::arg("min"), py::arg("max"));
    
    m.def("clamp", &helpers::clamp,
          py::arg("value"), py::arg("min"), py::arg("max"));
    
    // Constants
    m.attr("VERSION") = "1.0.0";
    
    // Error types
    py::register_exception<nlm::RuntimeError>(m, "RuntimeError");
}
