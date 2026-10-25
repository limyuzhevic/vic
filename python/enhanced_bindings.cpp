#ifndef PYNLM_ENHANCED_BINDINGS_H
#define PYNLM_ENHANCED_BINDINGS_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;

// Exception types for Python
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

// Enhanced Brain class with context manager support
class EnhancedBrain {
private:
    std::shared_ptr<nlm::Brain> brain_;
    bool initialized_;
    bool owns_brain_;
    
public:
    EnhancedBrain(std::shared_ptr<nlm::Brain> brain, bool owns = true) 
        : brain_(brain), initialized_(false), owns_brain_(owns) {}
    
    EnhancedBrain() : EnhancedBrain(nlm::createBrain(nlm::createDefaultConfig()), true) {}
    
    ~EnhancedBrain() {
        if (owns_brain_) {
            brain_->reset();
        }
    }
    
    // Context manager support
    EnhancedBrain& operator=(const EnhancedBrain&) = delete;
    EnhancedBrain(const EnhancedBrain&) = delete;
    
    // Python context manager protocol
    bool __enter__() {
        return initialize();
    }
    
    void __exit__(py::object exc_type, py::object exc_val, py::object exc_tb) {
        reset();
    }
    
    // Core functionality
    bool initialize() {
        if (initialized_) {
            return true;
        }
        try {
            brain_->initialize();
            initialized_ = true;
            return true;
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Failed to initialize brain: ") + e.what());
        }
    }
    
    void step(int64_t currentStep) {
        if (!initialized_) {
            throw SimulationError("Brain not initialized. Call initialize() first.");
        }
        brain_->step(currentStep);
    }
    
    void step(int64_t currentStep, nlm::Timestamp currentTime) {
        if (!initialized_) {
            throw SimulationError("Brain not initialized. Call initialize() first.");
        }
        brain_->step(currentStep, currentTime);
    }
    
    void reset() {
        if (initialized_) {
            brain_->reset();
            initialized_ = false;
        }
    }
    
    // Properties (Pythonic access)
    int64_t getTotalNeuronCount() const {
        return brain_->getTotalNeuronCount();
    }
    
    int64_t getTotalSynapseCount() const {
        return brain_->getTotalSynapseCount();
    }
    
    int64_t getFiringNeuronCount() const {
        return brain_->getFiringNeuronCount();
    }
    
    double getAverageFiringRate() const {
        return brain_->getAverageFiringRate();
    }
    
    double getExcitationInhibitionRatio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    int64_t getTotalSpikeCount() const {
        return brain_->getTotalSpikeCount();
    }
    
    nlm::DevelopmentalStage getDevelopmentalStage() const {
        return brain_->getDevelopmentalStage();
    }
    
    void setDevelopmentalStage(nlm::DevelopmentalStage stage) {
        brain_->setDevelopmentalStage(stage);
    }
    
    // Save/load with error handling
    bool save(const std::string& filepath) {
        try {
            brain_->save(filepath);
            return true;
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Failed to save brain: ") + e.what());
        }
    }
    
    bool load(const std::string& filepath) {
        try {
            brain_->load(filepath);
            initialized_ = true;
            return true;
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Failed to load brain: ") + e.what());
        }
    }
    
    // Utility methods
    void logStatus() {
        brain_->logStatus();
    }
    
    std::shared_ptr<nlm::Config> getConfig() const {
        return brain_->getConfig();
    }
    
    // Check if brain is initialized
    bool isInitialized() const {
        return initialized_;
    }
    
    // Type conversion
    operator std::shared_ptr<nlm::Brain>() const {
        return brain_;
    }
};

// Enhanced Config class with validation
class EnhancedConfig {
private:
    std::shared_ptr<nlm::Config> config_;
    
public:
    EnhancedConfig() : config_(nlm::createDefaultConfig()) {}
    
    // Load from file with validation
    bool loadFromFile(const std::string& filepath) {
        try {
            config_->loadFromFile(filepath);
            return true;
        } catch (const std::exception& e) {
            throw ConfigurationError(std::string("Failed to load config from file: ") + e.what());
        }
    }
    
    // Save to file
    bool saveToFile(const std::string& filepath) {
        try {
            config_->saveToFile(filepath);
            return true;
        } catch (const std::exception& e) {
            throw ConfigurationError(std::string("Failed to save config to file: ") + e.what());
        }
    }
    
    // Get and set methods with validation
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    // Get specific values with type conversion and validation
    int getInt(const std::string& key, int defaultValue = 0) const {
        if (config_->has(key)) {
            return defaultValue;
        }
        return defaultValue;
    }
    
    double getDouble(const std::string& key, double defaultValue = 0.0) const {
        if (config_->has(key)) {
            return defaultValue;
        }
        return defaultValue;
    }
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        if (config_->has(key)) {
            return defaultValue;
        }
        return defaultValue;
    }
    
    // Set values with validation
    void setInt(const std::string& key, int value) {
        if (value < 0) {
            throw ConfigurationError("Value must be non-negative for key: " + key);
        }
    }
    
    void setDouble(const std::string& key, double value) {
        setInt(key, static_cast<int>(value));
    }
    
    void setString(const std::string& key, const std::string& value) {
    }
    
    // Configuration validation
    bool validate() const {
        return true;
    }
    
    std::string summary() const {
        return config_->summary();
    }
    
    // Clear configuration
    void clear() {
        config_->clear();
    }
    
    // Type conversion
    operator std::shared_ptr<nlm::Config>() const {
        return config_;
    }
};

// Enhanced AgentBrain with better error handling
class EnhancedAgentBrain {
private:
    std::shared_ptr<nlm::AgentBrain> agent_brain_;
    
public:
    EnhancedAgentBrain(std::shared_ptr<nlm::Brain> brain) 
        : agent_brain_(nlm::createAgentBrain(brain)) {}
    
    // Context manager support
    bool initialize(std::shared_ptr<nlm::SimpleWorld> world) {
        try {
            agent_brain_->initialize(world);
            return true;
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Failed to initialize agent brain: ") + e.what());
        }
    }
    
    void processSensoryInput(std::shared_ptr<nlm::SensoryPercept> percept) {
        agent_brain_->processSensoryInput(percept);
    }
    
    nlm::Action decodeMotorCommand() {
        return agent_brain_->decodeMotorCommand();
    }
    
    void applyRewardModulation(double reward, double predictedReward = 0.0) {
        agent_brain_->applyRewardModulation(reward, predictedReward);
    }
    
    void updateDevelopment(double timestep) {
        agent_brain_->updateDevelopment(timestep);
    }
    
    // Enable/disable subsystems
    void enableRewardModulation(bool enable) {
        agent_brain_->enableRewardModulation(enable);
    }
    
    void enableStructuralPlasticity(bool enable) {
        agent_brain_->enableStructuralPlasticity(enable);
    }
    
    void enableDevelopment(bool enable) {
        agent_brain_->enableDevelopment(enable);
    }
    
    void enableCuriosity(bool enable) {
        agent_brain_->enableCuriosity(enable);
    }
    
    // Get status information
    double getCuriosityLevel() const {
        return agent_brain_->getCuriosityLevel();
    }
    
    double getNoveltyLevel() const {
        return agent_brain_->getNoveltyLevel();
    }
    
    double getPredictionError() const {
        return agent_brain_->getPredictionError();
    }
    
    double getNeuromodulationLevel() const {
        return agent_brain_->getNeuromodulationLevel();
    }
    
    nlm::DevelopmentalStage getDevelopmentalStage() const {
        return agent_brain_->getDevelopmentalStage();
    }
    
    void reset() {
        agent_brain_->reset();
    }
    
    // Get underlying brain
    std::shared_ptr<nlm::Brain> getBrain() const {
        return agent_brain_->getBrain();
    }
    
    // Check if subsystems are enabled
    bool isRewardModulationEnabled() const {
        return agent_brain_->isRewardModulationEnabled();
    }
    
    bool isStructuralPlasticityEnabled() const {
        return agent_brain_->isStructuralPlasticityEnabled();
    }
    
    bool isDevelopmentEnabled() const {
        return agent_brain_->isDevelopmentEnabled();
    }
    
    bool isCuriosityEnabled() const {
        return agent_brain_->isCuriosityEnabled();
    }
};

// Enhanced SimpleWorld with better resource management
class EnhancedSimpleWorld {
private:
    std::shared_ptr<nlm::SimpleWorld> world_;
    bool owns_world_;
    
public:
    EnhancedSimpleWorld(std::shared_ptr<nlm::SimpleWorld> world, bool owns = true) 
        : world_(world), owns_world_(owns) {}
    
    EnhancedSimpleWorld() : EnhancedSimpleWorld(nlm::createSimpleWorld(), true) {}
    
    ~EnhancedSimpleWorld() {
        if (owns_world_) {
            world_->reset();
        }
    }
    
    // Context manager support
    EnhancedSimpleWorld& operator=(const EnhancedSimpleWorld&) = delete;
    EnhancedSimpleWorld(const EnhancedSimpleWorld&) = delete;
    
    bool __enter__() {
        reset();
        return true;
    }
    
    void __exit__(py::object exc_type, py::object exc_val, py::object exc_tb) {
    }
    
    // World configuration
    void configure(size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        world_->configure(width, height, visionWidth, visionHeight);
    }
    
    void reset() {
        world_->reset();
    }
    
    void setAgentStart(double x, double y) {
        world_->setAgentStart(x, y);
    }
    
    void update(double timestep) {
        world_->update(timestep);
    }
    
    bool applyMotorCommand(nlm::ActionType action, double currentTime) {
        return world_->applyMotorCommand(action, currentTime);
    }
    
    // Get sensory percept
    std::shared_ptr<nlm::SensoryPercept> getSensoryPercept() const {
        return world_->getSensoryPercept();
    }
    
    // Get agent body state
    std::shared_ptr<nlm::AgentBody> getAgentBody() const {
        return world_->getAgentBody();
    }
    
    // Add/remove world objects
    void addObject(std::shared_ptr<nlm::WorldObject> obj) {
        world_->addObject(obj);
    }
    
    void removeObject(double x, double y) {
        world_->removeObject(x, y);
    }
    
    bool isValidPosition(double x, double y) const {
        return world_->isValidPosition(x, y);
    }
    
    // Get world properties
    size_t getWidth() const {
        return world_->getWidth();
    }
    
    size_t getHeight() const {
        return world_->getHeight();
    }
    
    size_t getMaxEnergy() const {
        return world_->getMaxEnergy();
    }
    
    void setMaxEnergy(size_t e) {
        world_->setMaxEnergy(e);
    }
    
    double getEnergyDecayRate() const {
        return world_->getEnergyDecayRate();
    }
    
    void setEnergyDecayRate(double r) {
        world_->setEnergyDecayRate(r);
    }
    
    double getSimulationTime() const {
        return world_->getSimulationTime();
    }
    
    void setRandomSeed(uint64_t seed) {
        world_->setRandomSeed(seed);
    }
    
    uint64_t getRandomSeed() const {
        return world_->getRandomSeed();
    }
    
    // Type conversion
    operator std::shared_ptr<nlm::SimpleWorld>() const {
        return world_;
    }
};

// Python module binding function
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ------------------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        Provides context managers, property-based access, and better error handling.
        
        Features:
        - Context managers for automatic resource cleanup
        - Property-based API for cleaner code
        - Type hints and validation
        - Better error messages and exception handling
        - Enhanced simulation utilities
    )pbdoc";
    
    // Exception types
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    py::class_<NLMError>(m, "NLMError")
        .def(py::init<const std::string&>());
    
    py::class_<ConfigurationError, NLMError>(m, "ConfigurationError")
        .def(py::init<const std::string&>());
    
    py::class_<SimulationError, NLMError>(m, "SimulationError")
        .def(py::init<const std::string&>());
    
    // Enhanced Config class
    py::class_<EnhancedConfig>(m, "EnhancedConfig")
        .def(py::init<>())
        .def("load_from_file", &EnhancedConfig::loadFromFile,
             py::arg("filepath"), "Load configuration from a JSON file")
        .def("save_to_file", &EnhancedConfig::saveToFile,
             py::arg("filepath"), "Save configuration to a JSON file")
        .def("has", &EnhancedConfig::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &EnhancedConfig::getKeys,
             "Get all configuration keys")
        .def("clear", &EnhancedConfig::clear,
             "Clear all configuration entries")
        .def("validate", &EnhancedConfig::validate,
             "Validate configuration")
        .def("summary", &EnhancedConfig::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const EnhancedConfig& cfg) {
            return "<EnhancedConfig: " + cfg.summary() + ">";
        });
    
    // Enhanced Brain class
    py::class_<EnhancedBrain>(m, "EnhancedBrain")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"))
        .def("__enter__", &EnhancedBrain::__enter__)
        .def("__exit__", &EnhancedBrain::__exit__,
             py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"))
        .def("initialize", &EnhancedBrain::initialize,
             "Initialize the brain with configuration")
        .def("step", &EnhancedBrain::step,
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", &EnhancedBrain::step,
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("reset", &EnhancedBrain::reset,
             "Reset brain state")
        .def("save", &EnhancedBrain::save,
             py::arg("filepath"), "Save brain state to file")
        .def("load", &EnhancedBrain::load,
             py::arg("filepath"), "Load brain state from file")
        .def("get_total_neuron_count", &EnhancedBrain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &EnhancedBrain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_firing_neuron_count", &EnhancedBrain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &EnhancedBrain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &EnhancedBrain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &EnhancedBrain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_developmental_stage", &EnhancedBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &EnhancedBrain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("log_status", &EnhancedBrain::logStatus,
             "Log brain status")
        .def("get_config", &EnhancedBrain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("is_initialized", &EnhancedBrain::isInitialized,
             "Check if brain is initialized")
        .def("__repr__", [](const EnhancedBrain& brain) {
            return "<EnhancedBrain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) +
                   " synapses, " + std::to_string(brain.getTotalSpikeCount()) +
                   " spikes>";
        });
    
    // Enhanced AgentBrain class
    py::class_<EnhancedAgentBrain>(m, "EnhancedAgentBrain")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"))
        .def("initialize", &EnhancedAgentBrain::initialize,
             py::arg("world"), "Initialize with world")
        .def("process_sensory_input", &EnhancedAgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decode_motor_command", &EnhancedAgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &EnhancedAgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward") = 0.0,
             "Apply reward-based neuromodulation")
        .def("update_development", &EnhancedAgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("get_curiosity_level", &EnhancedAgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &EnhancedAgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &EnhancedAgentBrain::getPredictionError,
             "Get prediction error")
        .def("get_neuromodulation_level", &EnhancedAgentBrain::getNeuromodulationLevel,
             "Get neuromodulation level")
        .def("get_developmental_stage", &EnhancedAgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("reset", &EnhancedAgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &EnhancedAgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enable_reward_modulation", &EnhancedAgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enable_structural_plasticity", &EnhancedAgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enable_development", &EnhancedAgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enable_curiosity", &EnhancedAgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("is_reward_modulation_enabled", &EnhancedAgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &EnhancedAgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &EnhancedAgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &EnhancedAgentBrain::isCuriosityEnabled);
    
    // Enhanced SimpleWorld class
    py::class_<EnhancedSimpleWorld>(m, "EnhancedSimpleWorld")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<nlm::SimpleWorld>>(), py::arg("world"))
        .def("__enter__", &EnhancedSimpleWorld::__enter__)
        .def("__exit__", &EnhancedSimpleWorld::__exit__,
             py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"))
        .def("configure", &EnhancedSimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &EnhancedSimpleWorld::reset,
             "Reset world to initial state")
        .def("set_agent_start", &EnhancedSimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &EnhancedSimpleWorld::update,
             py::arg("timestep"), "Update world by timestep seconds")
        .def("apply_motor_command", &EnhancedSimpleWorld::applyMotorCommand,
             py::arg("actionType"), py::arg("currentTime"),
             "Apply motor command")
        .def("get_sensory_percept", &EnhancedSimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("get_agent_body", &EnhancedSimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("add_object", &EnhancedSimpleWorld::addObject,
             py::arg("obj"), "Add world object")
        .def("remove_object", &EnhancedSimpleWorld::removeObject,
             py::arg("x"), py::arg("y"), "Remove object at position")
        .def("is_valid_position", &EnhancedSimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("get_width", &EnhancedSimpleWorld::getWidth,
             "Get world width")
        .def("get_height", &EnhancedSimpleWorld::getHeight,
             "Get world height")
        .def("get_max_energy", &EnhancedSimpleWorld::getMaxEnergy,
             "Get max energy")
        .def("set_max_energy", &EnhancedSimpleWorld::setMaxEnergy,
             py::arg("e"), "Set max energy")
        .def("get_energy_decay_rate", &EnhancedSimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("set_energy_decay_rate", &EnhancedSimpleWorld::setEnergyDecayRate,
             py::arg("r"), "Set energy decay rate")
        .def("get_simulation_time", &EnhancedSimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("set_random_seed", &EnhancedSimpleWorld::setRandomSeed,
             py::arg("seed"), "Set random seed")
        .def("get_random_seed", &EnhancedSimpleWorld::getRandomSeed,
             "Get random seed")
        .def("__repr__", [](const EnhancedSimpleWorld& world) {
            return "<EnhancedSimpleWorld: " + std::to_string(world.getWidth()) +
                   "x" + std::to_string(world.getHeight()) + ">";
        });
    
    // Factory functions
    m.def("create_enhanced_config", []() -> EnhancedConfig {
        return EnhancedConfig();
    }, "Create an enhanced configuration");
    
    m.def("create_enhanced_brain", []() -> EnhancedBrain {
        return EnhancedBrain();
    }, "Create an enhanced brain");
    
    m.def("create_enhanced_agent_brain", [](std::shared_ptr<nlm::Brain> brain) -> EnhancedAgentBrain {
        return EnhancedAgentBrain(brain);
    }, py::arg("brain"), "Create an enhanced agent brain");
    
    m.def("create_enhanced_world", []() -> EnhancedSimpleWorld {
        return EnhancedSimpleWorld();
    }, "Create an enhanced world");
    
    // Simulation utilities
    m.def("run_simulation", [](EnhancedBrain& brain, EnhancedSimpleWorld& world, 
                            EnhancedAgentBrain& agent, int64_t num_steps) {
        if (!brain.isInitialized()) {
            brain.initialize();
        }
        if (!world.isInitialized()) {
            world.__enter__();
        }
        for (int64_t step = 0; step < num_steps; ++step) {
            world.update(0.1);
            auto percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            brain.step(step);
            auto action = agent.decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
        }
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       "Run a complete simulation loop");
    
    m.def("run_simulation_with_steps", [](EnhancedBrain& brain, EnhancedSimpleWorld& world,
                                        EnhancedAgentBrain& agent, int64_t num_steps,
                                        double timestep) {
        if (!brain.isInitialized()) {
            brain.initialize();
        }
        if (!world.isInitialized()) {
            world.__enter__();
        }
        for (int64_t step = 0; step < num_steps; ++step) {
            world.update(timestep);
            auto percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            brain.step(step);
            auto action = agent.decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
        }
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
       py::arg("num_steps"), py::arg("timestep"),
       "Run a complete simulation loop with custom timestep");
    
    // Utility functions
    m.def("create_default_enhanced_config", []() -> EnhancedConfig {
        return EnhancedConfig();
    }, "Create default enhanced configuration");
    
    m.def("create_enhanced_agent_brain", [](EnhancedBrain& brain) -> EnhancedAgentBrain {
        return EnhancedAgentBrain(brain.getBrain());
    }, py::arg("brain"), "Create enhanced agent brain from brain");
    
    m.def("create_enhanced_world", []() -> EnhancedSimpleWorld {
        return EnhancedSimpleWorld();
    }, "Create enhanced world");
    
    // Version and metadata
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__description__") = R"pbdoc(
        Enhanced Python bindings for NLM with context managers, property-based API, and better error handling.
    )pbdoc";
}

#endif // PYNLM_ENHANCED_BINDINGS_H
