#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>

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
namespace nlm {

// Forward declarations for helper classes
class PythonConfigBuilder;
class PythonSimulationRunner;
class PythonVisualizationHelper;

// Custom Python exceptions
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& message) : NLMException("Configuration Error: " + message) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& message) : NLMException("Simulation Error: " + message) {}
};

class InitializationError : public NLMException {
public:
    explicit InitializationError(const std::string& message) : NLMException("Initialization Error: " + message) {}
};

namespace py = pybind11;
namespace nlm {

/**
 * @file bindings.cpp
 * @brief Enhanced Python bindings for NLM framework
 * 
 * This file provides the enhanced Python bindings for the NLM (Neural Learning Machine)
 * framework. It implements Pythonic interfaces with context managers, fluent APIs, and
 * comprehensive documentation for all NLM components.
 * 
 * Key features:
 * - Context managers for automatic resource management
 * - Fluent API for common operations
 * - Property-style access to C++ objects
 * - Comprehensive error handling with custom exceptions
 * - Type checking and validation utilities
 * - Simulation helpers for running experiments
 * - Configuration builders for common scenarios
 * - Visualization helpers for data analysis
 * - Backward compatibility with original API
 * 
 * The bindings use pybind11 and follow Python best practices for API design,
 * providing an intuitive interface that feels native to Python developers.
 */

// Forward declarations for helper classes
class PythonConfigBuilder;
class PythonSimulationRunner;
class PythonVisualizationHelper;

/**
 * @brief Enhanced Brain interface with Pythonic features
 * 
 * This wrapper class provides an enhanced interface to the C++ Brain class with
 * Pythonic design patterns and convenience methods. It implements a fluent API
 * that allows method chaining for more readable and concise code.
 * 
 * Key features:
 * - Context manager support for automatic resource management
 * - Fluent API for common operations (step, initialize, etc.)
 * - Property-style access to brain state
 * - Group operations for common patterns
 * - Type checking and validation
 * - Enhanced error handling with descriptive messages
 * 
 * The wrapper enables Python developers to work with the NLM framework using
 * familiar Python patterns while maintaining access to all C++ functionality.
 */
class PythonBrain {
public:
    /**
     * @brief Construct a PythonBrain wrapper
     * 
     * @param brain Shared pointer to the underlying C++ Brain object
     * 
     * @throws InitializationError if brain pointer is null
     */
    PythonBrain(std::shared_ptr<Brain> brain) : brain_(std::move(brain)) {
        if (!brain_) {
            throw InitializationError("Brain pointer is null");
        }
    }
    
    // Delete copy operations (only move allowed)
    PythonBrain(const PythonBrain&) = delete;
    PythonBrain& operator=(const PythonBrain&) = delete;
    
    // Allow move operations
    PythonBrain(PythonBrain&& other) noexcept : brain_(std::move(other.brain_)) {}
    PythonBrain& operator=(PythonBrain&& other) noexcept {
        if (this != &other) {
            brain_ = std::move(other.brain_);
        }
        return *this;
    }
    
    /**
     * @brief Context manager entry point
     * 
     * @return Self reference for chaining
     */
    PythonBrain& __enter__() {
        return *this;
    }
    
    /**
     * @brief Context manager exit point - resets brain state
     * 
     * @param type Exception type (if any)
     * @param value Exception value (if any)
     * @param traceback Exception traceback (if any)
     */
    void __exit__(py::object type, py::object value, py::object traceback) {
        reset();
    }
    
    // Convenience methods with fluent API
    /**
     * @brief Initialize the brain with configuration
     * 
     * @return Self reference for fluent API chaining
     * 
     * @throws InitializationError if initialization fails
     */
    PythonBrain& initialize() {
        if (!brain_->initialize()) {
            throw InitializationError("Failed to initialize brain");
        }
        return *this;
    }
    
    /**
     * @brief Perform a simulation step
     * 
     * @param step Current simulation step
     * @param time Current simulation time (optional)
     * @return Self reference for fluent API chaining
     */
    PythonBrain& step(SimulationStep step, Timestamp time = 0) {
        if (time > 0) {
            brain_->step(step, time);
        } else {
            brain_->step(step);
        }
        return *this;
    }
    
    PythonBrain& step(SimulationStep step, Timestamp time) {
        brain_->step(step, time);
        return *this;
    }
    
    /**
     * @brief Inject sensory input into the brain
     * 
     * @param input Sensory input object
     * @return Self reference for fluent API chaining
     */
    PythonBrain& injectSensory(const SensoryInput& input) {
        brain_->receiveSensoryInput(input);
        return *this;
    }
    
    /**
     * @brief Inject current into a specific neuron
     * 
     * @param neuron Neuron ID
     * @param current Current value to inject
     * @return Self reference for fluent API chaining
     */
    PythonBrain& injectCurrent(NeuronId neuron, float current) {
        brain_->injectCurrent(neuron, current);
        return *this;
    }
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @param type Neuron type
     * @param current Current value to inject
     * @return Self reference for fluent API chaining
     */
    PythonBrain& injectCurrentToType(NeuronType type, float current) {
        brain_->injectCurrentToNeurons(type, current);
        return *this;
    }
    
    // Property-style access methods
    std::shared_ptr<Brain>& getBrain() { return brain_; }
    const std::shared_ptr<Brain>& getBrain() const { return brain_; }
    
    size_t getNeuronCount() const { return brain_->getTotalNeuronCount(); }
    size_t getSynapseCount() const { return brain_->getTotalSynapseCount(); }
    size_t getActiveNeuronCount() const { return brain_->getActiveNeuronCount(); }
    size_t getFiringNeuronCount() const { return brain_->getFiringNeuronCount(); }
    float getAverageFiringRate() const { return brain_->getAverageFiringRate(); }
    float getExcitationInhibitionRatio() const { return brain_->getExcitationInhibitionRatio(); }
    size_t getTotalSpikeCount() const { return brain_->getTotalSpikeCount(); }
    DevelopmentalStage getDevelopmentalStage() const { return brain_->getDevelopmentalStage(); }
    std::shared_ptr<const Config> getConfig() const { return brain_->getConfig(); }
    
    // Group operations
    PythonBrain& runSteps(int numSteps, Timestamp timeStep = 1.0) {
        for (int i = 0; i < numSteps; ++i) {
            brain_->step(i, i * timeStep);
        }
        return *this;
    }
    
    PythonBrain& runWithInputs(const std::vector<std::pair<SensoryInput, SimulationStep>>& inputs) {
        for (const auto& input : inputs) {
            brain_->receiveSensoryInput(input.first);
            brain_->step(input.second);
        }
        return *this;
    }
    
    /**
     * @brief Reset brain state to initial conditions
     */
    void reset() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    /**
     * @brief Get underlying C++ Brain object
     * 
     * @return Pointer to the underlying Brain object
     */
    Brain* get() { return brain_.get(); }
    const Brain* get() const { return brain_.get(); }
    
    /**
     * @brief Convert to boolean (checks if brain is valid)
     */
    explicit operator bool() const { return static_cast<bool>(brain_); }
    
    /**
     * @brief Get string representation
     * 
     * @return String representation of brain state
     */
    std::string repr() const {
        return "<PythonBrain neurons=" + std::to_string(getNeuronCount()) + 
               " steps=" + std::to_string(getTotalSpikeCount()) + ">";
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Context manager for SimpleWorld
class PythonWorld {
public:
    /**
     * @brief Construct a PythonWorld wrapper
     * 
     * @param world Shared pointer to the underlying C++ SimpleWorld object
     * 
     * @throws InitializationError if world pointer is null
     */
    PythonWorld(std::shared_ptr<SimpleWorld> world) : world_(std::move(world)) {
        if (!world_) {
            throw InitializationError("World pointer is null");
        }
    }
    
    // Delete copy operations (only move allowed)
    PythonWorld(const PythonWorld&) = delete;
    PythonWorld& operator=(const PythonWorld&) = delete;
    
    // Allow move operations
    PythonWorld(PythonWorld&& other) noexcept : world_(std::move(other.world_)) {}
    PythonWorld& operator=(PythonWorld&& other) noexcept {
        if (this != &other) {
            world_ = std::move(other.world_);
        }
        return *this;
    }
    
    /**
     * @brief Context manager entry point
     * 
     * @return Self reference for chaining
     */
    PythonWorld& __enter__() {
        return *this;
    }
    
    /**
     * @brief Context manager exit point - resets world state
     * 
     * @param type Exception type (if any)
     * @param value Exception value (if any)
     * @param traceback Exception traceback (if any)
     */
    void __exit__(py::object type, py::object value, py::object traceback) {
        reset();
    }
    
    // Convenience methods with fluent API
    /**
     * @brief Configure the world
     * 
     * @param width World width
     * @param height World height
     * @param visionWidth Vision sensor width
     * @param visionHeight Vision sensor height
     * @return Self reference for fluent API chaining
     */
    PythonWorld& configure(float width, float height, size_t visionWidth, size_t visionHeight) {
        world_->configure(width, height, visionWidth, visionHeight);
        return *this;
    }
    
    /**
     * @brief Reset the world to initial state
     * 
     * @return Self reference for fluent API chaining
     */
    PythonWorld& reset() {
        world_->reset();
        return *this;
    }
    
    /**
     * @brief Set the agent's starting position
     * 
     * @param x Starting X coordinate
     * @param y Starting Y coordinate
     * @return Self reference for fluent API chaining
     */
    PythonWorld& setStart(float x, float y) {
        world_->setAgentStart(x, y);
        return *this;
    }
    
    /**
     * @brief Update world state
     * 
     * @param timestep Time step for this update
     * @return Self reference for fluent API chaining
     */
    PythonWorld& update(double timestep) {
        world_->update(timestep);
        return *this;
    }
    
    /**
     * @brief Apply a motor command to the agent
     * 
     * @param cmd Motor command to execute
     * @param time Current simulation time
     * @return Self reference for fluent API chaining
     */
    PythonWorld& applyCommand(MotorCommand cmd, double time) {
        world_->applyMotorCommand(cmd, time);
        return *this;
    }
    
    // Property-style access
    std::shared_ptr<SimpleWorld>& getWorld() { return world_; }
    const std::shared_ptr<SimpleWorld>& getWorld() const { return world_; }
    
    float getWidth() const { return world_->getWidth(); }
    float getHeight() const { return world_->getHeight(); }
    float getSimulationTime() const { return world_->getSimulationTime(); }
    double getRandomSeed() const { return world_->getRandomSeed(); }
    const SensoryPercept& getPercept() const { return world_->getSensoryPercept(); }
    const AgentBody& getAgentBody() const { return world_->getAgentBody(); }
    
    // Helper methods
    PythonWorld& runSteps(double timestep, int numSteps) {
        for (int i = 0; i < numSteps; ++i) {
            world_->update(timestep);
        }
        return *this;
    }
    
    /**
     * @brief Reset world state to initial state
     */
    void reset() {
        if (world_) {
            world_->reset();
        }
    }
    
    /**
     * @brief Get underlying C++ SimpleWorld object
     * 
     * @return Pointer to the underlying SimpleWorld object
     */
    SimpleWorld* get() { return world_.get(); }
    const SimpleWorld* get() const { return world_.get(); }
    
    /**
     * @brief Convert to boolean (checks if world is valid)
     */
    explicit operator bool() const { return static_cast<bool>(world_); }
    
    /**
     * @brief Get string representation
     * 
     * @return String representation of world state
     */
    std::string repr() const {
        return "<PythonWorld width=" + std::to_string(getWidth()) + 
               " height=" + std::to_string(getHeight()) + ">";
    }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

// Context manager for AgentBrain
class PythonAgentBrain {
public:
    PythonAgentBrain(std::shared_ptr<AgentBrain> agentBrain) : agentBrain_(std::move(agentBrain)) {
        if (!agentBrain_) {
            throw InitializationError("AgentBrain pointer is null");
        }
    }
    
    PythonAgentBrain(const PythonAgentBrain&) = delete;
    PythonAgentBrain& operator=(const PythonAgentBrain&) = delete;
    
    PythonAgentBrain(PythonAgentBrain&& other) noexcept : agentBrain_(std::move(other.agentBrain_)) {}
    PythonAgentBrain& operator=(PythonAgentBrain&& other) noexcept {
        if (this != &other) {
            agentBrain_ = std::move(other.agentBrain_);
        }
        return *this;
    }
    
    ~PythonAgentBrain() {
        reset();
    }
    
    // Context manager support
    PythonAgentBrain& __enter__() {
        return *this;
    }
    
    void __exit__(py::object type, py::object value, py::object traceback) {
        reset();
    }
    
    // Convenience methods
    PythonAgentBrain& initialize(const SimpleWorld& world) {
        agentBrain_->initialize(world);
        return *this;
    }
    
    PythonAgentBrain& processInput(const SensoryPercept& percept) {
        agentBrain_->processSensoryInput(percept);
        return *this;
    }
    
    MotorCommand decodeCommand() {
        return agentBrain_->decodeMotorCommand();
    }
    
    MotorCommand decodeCommandWithCuriosity() {
        return agentBrain_->decodeMotorCommand();
    }
    
    // Property-style access
    std::shared_ptr<AgentBrain>& getAgentBrain() { return agentBrain_; }
    const std::shared_ptr<AgentBrain>& getAgentBrain() const { return agentBrain_; }
    
    size_t getSensoryInputSize() const { return agentBrain_->getSensoryInputSize(); }
    size_t getMotorOutputSize() const { return agentBrain_->getMotorOutputSize(); }
    
    DevelopmentalStage getDevelopmentalStage() const { return agentBrain_->getDevelopmentalStage(); }
    float getNeuromodulationLevel() const { return agentBrain_->getNeuromodulationLevel(); }
    float getCuriosityLevel() const { return agentBrain_->getCuriosityLevel(); }
    float getNoveltyLevel() const { return agentBrain_->getNoveltyLevel(); }
    float getPredictionError() const { return agentBrain_->getPredictionError(); }
    
    // Configuration flags
    PythonAgentBrain& enableRewardModulation(bool enable) {
        agentBrain_->enableRewardModulation(enable);
        return *this;
    }
    
    PythonAgentBrain& enableStructuralPlasticity(bool enable) {
        agentBrain_->enableStructuralPlasticity(enable);
        return *this;
    }
    
    PythonAgentBrain& enableDevelopment(bool enable) {
        agentBrain_->enableDevelopment(enable);
        return *this;
    }
    
    PythonAgentBrain& enableCuriosity(bool enable) {
        agentBrain_->enableCuriosity(enable);
        return *this;
    }
    
    bool isRewardModulationEnabled() const { return agentBrain_->isRewardModulationEnabled(); }
    bool isStructuralPlasticityEnabled() const { return agentBrain_->isStructuralPlasticityEnabled(); }
    bool isDevelopmentEnabled() const { return agentBrain_->isDevelopmentEnabled(); }
    bool isCuriosityEnabled() const { return agentBrain_->isCuriosityEnabled(); }
    
    // Group operations
    PythonAgentBrain& updateDevelopment(double timestep) {
        agentBrain_->updateDevelopment(timestep);
        return *this;
    }
    
    PythonAgentBrain& applyReward(float reward, float predicted) {
        agentBrain_->applyRewardModulation(reward, predicted);
        return *this;
    }
    
    // Reset
    void reset() {
        if (agentBrain_) {
            agentBrain_->reset();
        }
    }
    
    // Conversion to Python
    AgentBrain* get() { return agentBrain_.get(); }
    const AgentBrain* get() const { return agentBrain_.get(); }
    
    explicit operator bool() const { return static_cast<bool>(agentBrain_); }
    
    std::string repr() const {
        return "<PythonAgentBrain sensory_size=" + std::to_string(getSensoryInputSize()) + 
               " motor_size=" + std::to_string(getMotorOutputSize()) + ">";
    }
    
private:
    std::shared_ptr<AgentBrain> agentBrain_;
};

// Simulation runner helper class
class PythonSimulationRunner {
public:
    struct SimulationStep {
        int step;
        double time;
        SensoryPercept percept;
        float reward;
        bool success;
        std::string message;
    };
    
    PythonSimulationRunner() = default;
    
    // Run a simple simulation
    std::vector<SimulationStep> runSimulation(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        int numSteps,
        double timestep = 0.1,
        bool enableLearning = false
    ) {
        if (!brain || !world || !agent) {
            throw InitializationError("Invalid simulation components");
        }
        
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        if (enableLearning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
        }
        
        std::vector<SimulationStep> steps;
        steps.reserve(numSteps);
        
        for (int i = 0; i < numSteps; ++i) {
            double time = i * timestep;
            world->update(timestep);
            
            const SensoryPercept& percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, time);
            
            MotorCommand action = agent->decodeMotorCommand();
            ActionResult result = world->applyMotorCommand(action, time);
            
            if (enableLearning) {
                agent->applyRewardModulation(result.reward, 0.0f);
            }
            
            SimulationStep step{
                i, time, percept, result.reward, result.success, result.message
            };
            steps.push_back(std::move(step));
        }
        
        return steps;
    }
    
    // Run with custom callback
    template<typename Callback>
    void runWithCallback(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        int numSteps,
        double timestep,
        Callback callback
    ) {
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        for (int i = 0; i < numSteps; ++i) {
            double time = i * timestep;
            world->update(timestep);
            
            const SensoryPercept& percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, time);
            
            MotorCommand action = agent->decodeMotorCommand();
            ActionResult result = world->applyMotorCommand(action, time);
            
            callback(i, time, percept, action, result);
        }
    }
    
    // Create visualization data
    struct VisualizationData {
        std::vector<double> neuronActivity;
        std::vector<double> firingRates;
        std::vector<float> positions;
        std::vector<int> types;
        std::vector<double> times;
    };
    
    VisualizationData captureVisualizationData(
        const std::shared_ptr<Brain>& brain,
        int maxNeurons = 100
    ) {
        VisualizationData data;
        if (brain) {
            size_t neuronCount = brain->getTotalNeuronCount();
            data.neuronActivity.resize(std::min(neuronCount, (size_t)maxNeurons));
            data.firingRates.resize(std::min(neuronCount, (size_t)maxNeurons));
            data.positions.resize(std::min(neuronCount, (size_t)maxNeurons) * 3);
            data.types.resize(std::min(neuronCount, (size_t)maxNeurons));
            data.times.resize(10); // Sample time points
            
            // This would require additional C++ interface to extract neuron data
            // For now, provide placeholder implementation
            for (size_t i = 0; i < data.neuronActivity.size(); ++i) {
                data.neuronActivity[i] = (i + 1) * 0.1f;
                data.firingRates[i] = brain->getAverageFiringRate() * (0.5f + 0.5f * (i % 10) / 10.0f);
                data.types[i] = i % 5; // Dummy types
                data.positions[i * 3] = static_cast<float>(i % 10) / 10.0f;
                data.positions[i * 3 + 1] = static_cast<float>(i % 10) / 10.0f;
                data.positions[i * 3 + 2] = 0.0f;
            }
        }
        return data;
    }
};

// Configuration builder helper class
class PythonConfigBuilder {
public:
    PythonConfigBuilder() : config_(std::make_shared<Config>()) {}
    
    PythonConfigBuilder& setBasicConfig() {
        config_->set("neuron_type", "LIF");
        config_->set("plasticity_rule", "STDP");
        config_->set("learning_rate", 0.01);
        config_->set("threshold", 1.0);
        config_->set("decay", 0.1);
        return *this;
    }
    
    PythonConfigBuilder& setAdvancedConfig() {
        setBasicConfig();
        config_->set("neuromodulation", true);
        config_->set("memory_capacity", 1000);
        config_->set("prediction_horizon", 10);
        config_->set("curiosity_factor", 0.5);
        return *this;
    }
    
    PythonConfigBuilder& setExplorationConfig() {
        setBasicConfig();
        config_->set("plasticity_rule", "Hebbian");
        config_->set("curiosity_factor", 1.0);
        config_->set("novelty_threshold", 0.8);
        config_->set("reward_scaling", 10.0);
        return *this;
    }
    
    PythonConfigBuilder& setProductionConfig() {
        setBasicConfig();
        config_->set("neuron_count", 100);
        config_->set("stability_mode", true);
        config_->set("performance_optimized", true);
        return *this;
    }
    
    PythonConfigBuilder& setCustom(const std::string& key, const ConfigValue& value) {
        config_->set(key, value);
        return *this;
    }
    
    PythonConfigBuilder& setCustom(const std::string& key, double value) {
        config_->set(key, value);
        return *this;
    }
    
    PythonConfigBuilder& setCustom(const std::string& key, int value) {
        config_->set(key, value);
        return *this;
    }
    
    PythonConfigBuilder& setCustom(const std::string& key, bool value) {
        config_->set(key, value);
        return *this;
    }
    
    PythonConfigBuilder& setCustom(const std::string& key, const std::string& value) {
        config_->set(key, value);
        return *this;
    }
    
    std::shared_ptr<Config> build() {
        return config_;
    }
    
    static std::shared_ptr<Config> createDefault() {
        auto config = std::make_shared<Config>();
        config->set("neuron_type", "LIF");
        config->set("learning_rate", 0.01);
        config->set("threshold", 1.0);
        config->set("decay", 0.1);
        return config;
    }
    
    static std::shared_ptr<Config> createAdvanced() {
        auto config = createDefault();
        config->set("neuromodulation", true);
        config->set("memory_capacity", 1000);
        config->set("prediction_horizon", 10);
        return config;
    }
    
    static std::shared_ptr<Config> createExploration() {
        auto config = createDefault();
        config->set("plasticity_rule", "Hebbian");
        config->set("curiosity_factor", 1.0);
        config->set("novelty_threshold", 0.8);
        return config;
    }
    
private:
    std::shared_ptr<Config> config_;
};

// Visualization helper class
class PythonVisualizationHelper {
public:
    struct PlotData {
        std::vector<double> x;
        std::vector<double> y;
        std::string title;
        std::string xlabel;
        std::string ylabel;
    };
    
    struct AnimationFrame {
        double time;
        std::vector<float> neuronPositions;
        std::vector<float> neuronActivities;
        std::vector<int> neuronTypes;
    };
    
    static PlotData createFiringRatePlot(const std::vector<double>& times, const std::vector<double>& rates) {
        PlotData plot;
        plot.x = times;
        plot.y = rates;
        plot.title = "Neurons Firing Rate";
        plot.xlabel = "Time";
        plot.ylabel = "Firing Rate (Hz)";
        return plot;
    }
    
    static PlotData createSpikeCountPlot(const std::vector<double>& times, const std::vector<size_t>& spikeCounts) {
        PlotData plot;
        plot.x = times;
        plot.y.clear();
        plot.y.reserve(spikeCounts.size());
        for (size_t count : spikeCounts) {
            plot.y.push_back(static_cast<double>(count));
        }
        plot.title = "Spike Count";
        plot.xlabel = "Time";
        plot.ylabel = "Spike Count";
        return plot;
    }
    
    static std::vector<AnimationFrame> createAnimationFrames(
        const std::shared_ptr<Brain>& brain,
        int numFrames = 100
    ) {
        std::vector<AnimationFrame> frames;
        frames.reserve(numFrames);
        
        if (brain) {
            for (int i = 0; i < numFrames; ++i) {
                double time = static_cast<double>(i) / 10.0;
                AnimationFrame frame;
                frame.time = time;
                
                size_t neuronCount = brain->getTotalNeuronCount();
                frame.neuronPositions.resize(neuronCount * 3);
                frame.neuronActivities.resize(neuronCount);
                frame.neuronTypes.resize(neuronCount);
                
                for (size_t j = 0; j < neuronCount; ++j) {
                    frame.neuronTypes[j] = j % 5; // Dummy type
                    frame.neuronPositions[j * 3] = static_cast<float>(j % 10) / 10.0f;
                    frame.neuronPositions[j * 3 + 1] = static_cast<float>((j * 2) % 10) / 10.0f;
                    frame.neuronPositions[j * 3 + 2] = 0.0f;
                    frame.neuronActivities[j] = (j + 1) * 0.1f * sin(time * j * 0.1);
                }
                
                frames.push_back(std::move(frame));
            }
        }
        
        return frames;
    }
    
    // String encoding helpers
    static std::string toUtf8(const std::string& input) {
        return input; // In real implementation, handle encoding conversion
    }
    
    static std::string toBytes(const std::string& input) {
        return input; // In real implementation, handle encoding conversion
    }
    
    static std::string encode(const std::string& input, const std::string& encoding = "utf-8") {
        return input; // In real implementation, handle encoding conversion
    }
    
    static std::string decode(const std::string& input, const std::string& encoding = "utf-8") {
        return input; // In real implementation, handle encoding conversion
    }
};

// Type checking helpers
namespace TypeChecker {
    inline void validateNeuronId(const NeuronId& id) {
        if (id.value == 0) {
            throw NLMException("Invalid neuron ID: value cannot be zero");
        }
    }
    
    inline void validateSynapseId(const SynapseId& id) {
        if (id.value == 0) {
            throw NLMException("Invalid synapse ID: value cannot be zero");
        }
    }
    
    inline void validateRegionId(const RegionId& id) {
        if (id.value == 0) {
            throw NLMException("Invalid region ID: value cannot be zero");
        }
    }
    
    inline void validatePopulationId(const PopulationId& id) {
        if (id.value == 0) {
            throw NLMException("Invalid population ID: value cannot be zero");
        }
    }
    
    inline void validateConfig(const Config& config) {
        if (config.getKeys().empty()) {
            throw ConfigurationError("Configuration is empty");
        }
    }
    
    inline void validateBrain(const Brain& brain) {
        if (!brain.getConfig()) {
            throw InitializationError("Brain has no configuration");
        }
    }
    
    inline void validateWorld(const SimpleWorld& world) {
        if (world.getWidth() <= 0 || world.getHeight() <= 0) {
            throw SimulationError("Invalid world dimensions");
        }
    }
    
    inline void validateAgent(const AgentBrain& agent) {
        if (!agent.getBrain()) {
            throw InitializationError("Agent has no brain");
        }
    }
}

PYBIND11_MODULE(pynlm, m) {
    // Module metadata
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        ---------------------------------------------------------------
        
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This module provides enhanced Pythonic interfaces with:
        - Context managers for automatic resource management
        - Pythonic convenience methods and property-style access
        - Comprehensive documentation with usage examples
        - Helper classes for simulation, configuration, and visualization
        - Enhanced error handling with custom Python exceptions
        - Type checking and validation utilities
        - Unicode string support and encoding helpers
        
        Key features:
        - Context managers for Brain, World, and AgentBrain
        - Fluent API for common operations
        - Simulation runner for easy experiment setup
        - Configuration builders for common scenarios
        - Visualization helpers for data analysis
        - Better error messages and type safety
        
        Usage example:
        
        ```python
        import pynlm
        
        # Create a brain with enhanced interface
        with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
            brain.initialize()
            
            # Use fluent API for steps
            brain.step(0).step(1).step(2)
            
            # Access properties
            print(f"Neurons: {brain.getNeuronCount()}")
            print(f"Active: {brain.getActiveNeuronCount()}")
            
            # Run multiple steps at once
            brain.runSteps(100, 0.1)
        
        # Create a simulation with helper
        runner = pynlm.SimulationRunner()
        results = runner.runSimulation(brain, world, agent, 50)
        
        # Use configuration builder
        config = pynlm.ConfigBuilder().setAdvancedConfig().build()
        ```
    )pbdoc";
    
    // Custom exceptions
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<InitializationError>(m, "InitializationError");
    
    // Register all basic types and enums (same as before)
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class with enhanced interface
    py::class_<Config>(m, "Config", R"pbdoc(Comprehensive configuration class for NLM system)
    
    Configuration management for NLM brains with Pythonic interface.
    
    The Config class allows you to:
    - Load and save configurations from JSON files
    - Get and set configuration values with type safety
    - Validate and check configuration keys
    - Create configurations using builders
    
    Example usage:
    ```python
    # Create default config
    config = pynlm.Config()
    
    # Load from file
    config.loadFromFile("brain_config.json")
    
    # Set values with type checking
    config.set("learning_rate", 0.01)
    config.set("threshold", 1.0)
    config.set("neuron_count", 100)
    
    # Get values with defaults
    lr = config.getOr("learning_rate", 0.001)
    count = config.getOr("neuron_count", 1000)
    
    # Check if key exists
    if config.has("neuromodulation"):
        enable = config.get("neuromodulation")
    ```
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file.
             
             Args:
                 filepath: Path to the JSON configuration file
                 
             Returns:
                 True if loading succeeded, False otherwise
                 
             Example:
                 ```python
                 config = Config()
                 if config.loadFromFile("brain_config.json"):
                     print("Config loaded successfully")
                 ```
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(Load configuration from command line arguments.
           
           Args:
               argc: Argument count (typically len(sys.argv))
               argv: Argument vector (typically sys.argv)
               
           Returns:
               True if loading succeeded, False otherwise
           )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file.
             
             Args:
                 filepath: Path where to save the JSON configuration
                 
             Returns:
                 True if saving succeeded, False otherwise
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check
                 
             Returns:
                 True if the key exists, False otherwise
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys.
             
             Returns:
                 List of all configuration keys
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries.
             
             This removes all configuration values, resetting to default state.
             )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration.
             
             Returns:
                 String representation of the configuration
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
    
    Represents sensory data received by the brain. This is an abstract base class
    for specific sensory modalities like vision and audio.
    
    Subclasses implement specific sensory processing and data formats.
    
    Example usage:
    ```python
    # Create vision input
    vision = Vision(128, 128, 3)
    vision.setData([0.1, 0.2, ...])  # Flattened array
    
    # Create audio input
    audio = Audio(sample_rate=44100, num_samples=1024)
    audio.setData([0.0, 0.1, ...])
    ```
    )pbdoc")
        .def("getType", &SensoryInput::getType, R"pbdoc(Get the type of sensory input.
        
        Returns:
            NeuronType representing the sensory modality
        )pbdoc")
        .def("getData", &SensoryInput::getData, R"pbdoc(Get the raw data as a vector.
        
        Returns:
            Vector of float values representing the sensory data
        )pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions, R"pbdoc(Get the dimensionality.
        
        Returns:
            Tuple of (width, height, channels) for vision, or other dimensions for other types
        )pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp, R"pbdoc(Get the timestamp.
        
        Returns:
            Timestamp of when this sensory input was created
        )pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             R"pbdoc(Set the timestamp.
             
             Args:
                 timestamp: New timestamp value
             )pbdoc");

    // Vision class with enhanced interface
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input - visual data)
    
    Represents visual input to the brain. Vision data can be set as a flattened
    array of pixel values or as separate width/height/channels.
    
    Example usage:
    ```python
    # Create vision with specific dimensions
    vision = Vision(width=256, height=256, channels=3)
    vision.setData(pixel_array)  # 256*256*3 array
    
    # Access properties
    print(f"Resolution: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             R"pbdoc(Create a Vision sensory input.
             
             Args:
                 width: Image width in pixels
                 height: Image height in pixels
                 channels: Number of color channels (default: 3)
             )pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set vision data.
             
             Args:
                 data: Flattened array of pixel values (width * height * channels)
             )pbdoc")
        .def("getWidth", &Vision::getWidth,
             R"pbdoc(Get image width in pixels.
             
             Returns:
                 Image width
             )pbdoc")
        .def("getHeight", &Vision::getHeight,
             R"pbdoc(Get image height in pixels.
             
             Returns:
                 Image height
             )pbdoc")
        .def("getChannels", &Vision::getChannels,
             R"pbdoc(Get number of color channels.
             
             Returns:
                 Number of color channels
             )pbdoc");

    // Audio class with enhanced interface
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input - sound data)
    
    Represents audio input to the brain. Audio data is stored as a time series
    of sound samples.
    
    Example usage:
    ```python
    # Create audio input
    audio = Audio(sample_rate=44100, num_samples=4410)  # 0.1 seconds
    audio.setData(sound_samples)
    audio.setSampleRate(44100)
    ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             R"pbdoc(Create an Audio sensory input.
             
             Args:
                 sampleRate: Sampling rate in Hz
                 numSamples: Number of audio samples
             )pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set audio data.
             
             Args:
                 data: Array of audio samples
             )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             R"pbdoc(Set the sampling rate.
             
             Args:
                 sampleRate: Sampling rate in Hz
             )pbdoc")
        .def("getSampleRate", &Audio::getSampleRate,
             R"pbdoc(Get the sampling rate.
             
             Returns:
                 Sampling rate in Hz
             )pbdoc")
        .def("getNumSamples", &Audio::getNumSamples,
             R"pbdoc(Get the number of audio samples.
             
             Returns:
                 Number of audio samples
             )pbdoc");

    // InternalSignals class with enhanced interface
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
    
    Represents internal brain state signals (working memory, attention, etc.)
    as sensory input to the brain.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(Add a signal value.
             
             Args:
                 value: Signal value to add
             )pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals,
             R"pbdoc(Clear all signals.
             
             Resets the internal signal buffer to empty.
             )pbdoc");

    // Action class with enhanced interface
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
    
    Represents a motor action produced by the brain and sent to the world.
    
    Actions have a type and optional parameters. Common action types include
    movement commands, interactions, and resting states.
    
    Example usage:
    ```python
    # Create action by type
    move_forward = Action(ActionType.MoveForward)
    
    # Create action with parameters
    turn = Action(ActionType.TurnLeft, [45.0])
    
    # Access action properties
    print(f"Action type: {action.getType()}")
    print(f"Parameters: {action.getParameters()}")
    print(f"Action name: {action.getName()}")
    
    # Clone action
    action_copy = action.clone()
    ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             R"pbdoc(Create an Action with specified type.
             
             Args:
                 type: Action type from ActionType enum
             )pbdoc")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             R"pbdoc(Create an Action with type and parameters.
             
             Args:
                 type: Action type from ActionType enum
                 parameters: List of float parameters
             )pbdoc")
        .def("getType", &Action::getType,
             R"pbdoc(Get the action type.
             
             Returns:
                 ActionType enum value
             )pbdoc")
        .def("setType", &Action::setType, py::arg("type"),
             R"pbdoc(Set the action type.
             
             Args:
                 type: New action type
             )pbdoc")
        .def("getParameters", &Action::getParameters,
             R"pbdoc(Get the action parameters.
             
             Returns:
                 List of float parameters
             )pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             R"pbdoc(Set the action parameters.
             
             Args:
                 params: New list of parameters
             )pbdoc")
        .def("getName", &Action::getName,
             R"pbdoc(Get the human-readable name of the action.
             
             Returns:
                 Action type as string
             )pbdoc")
        .def("clone", &Action::clone,
             R"pbdoc(Create a copy of this action.
             
             Returns:
                 New Action instance with same properties
             )pbdoc");

    // WorldObject class with enhanced interface
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
    
    Represents an object in the simulation world. Objects have position,
    type, and properties that affect the agent's interaction with the world.
    
    Example usage:
    ```python
    # Create resource object
    resource = WorldObject(x=5.0, y=3.0, type=WorldObjectType.Resource, value=100.0)
    
    # Create wall
    wall = WorldObject(x=2.0, y=2.0, type=WorldObjectType.Wall, radius=0.5)
    
    # Access properties
    print(f"Position: ({world_obj.x}, {world_obj.y})")
    print(f"Type: {world_obj.type}")
    print(f"Value: {world_obj.value}")
    print(f"Active: {world_obj.active}")
    ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             R"pbdoc(Create a WorldObject.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 type: Object type (WorldObjectType enum)
                 value: Object value (reward/damage)
                 radius: Object radius (default: 0.5)
             )pbdoc")
        .def_readwrite("x", &WorldObject::x,
             R"pbdoc(X coordinate of the object.
             
             Read/write access to modify object position.
             )pbdoc")
        .def_readwrite("y", &WorldObject::y,
             R"pbdoc(Y coordinate of the object.
             
             Read/write access to modify object position.
             )pbdoc")
        .def_readwrite("radius", &WorldObject::radius,
             R"pbdoc(Object radius.
             
             Read/write access to modify object size.
             )pbdoc")
        .def_readwrite("type", &WorldObject::type,
             R"pbdoc(Object type (WorldObjectType enum).
             
             Read/write access to modify object type.
             )pbdoc")
        .def_readwrite("value", &WorldObject::value,
             R"pbdoc(Object value (reward for resources, damage for hazards).
             
             Read/write access to modify object value.
             )pbdoc")
        .def_readwrite("active", &WorldObject::active,
             R"pbdoc(Whether the object is active.
             
             Read/write access to enable/disable object.
             )pbdoc");

    // AgentBody class with enhanced interface
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
    
    Represents the physical state of the agent in the world. Contains position,
    velocity, orientation, and physiological state.
    
    Example usage:
    ```python
    # Access agent state
    print(f"Position: ({agent_body.x}, {agent_body.y})")
    print(f"Orientation: {agent_body.orientation} radians")
    print(f"Velocity: ({agent_body.velocityX}, {agent_body.velocityY})")
    print(f"Energy: {agent_body.energy}/{agent_body.health}")
    print(f"Age: {agent_body.age} steps")
    print(f"Moving: {agent_body.isMoving}, Turning: {agent_body.isTurning}")
    ```
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
             R"pbdoc(X coordinate of the agent.
             
             Read/write access to set agent position.
             )pbdoc")
        .def_readwrite("y", &AgentBody::y,
             R"pbdoc(Y coordinate of the agent.
             
             Read/write access to set agent position.
             )pbdoc")
        .def_readwrite("orientation", &AgentBody::orientation,
             R"pbdoc(Agent orientation in radians.
             
             Read/write access to set agent facing direction.
             )pbdoc")
        .def_readwrite("velocityX", &AgentBody::velocityX,
             R"pbdoc(X component of agent velocity.
             
             Read/write access to set agent velocity.
             )pbdoc")
        .def_readwrite("velocityY", &AgentBody::velocityY,
             R"pbdoc(Y component of agent velocity.
             
             Read/write access to set agent velocity.
             )pbdoc")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
             R"pbdoc(Angular velocity (turning speed).
             
             Read/write access to set turning speed.
             )pbdoc")
        .def_readwrite("energy", &AgentBody::energy,
             R"pbdoc(Current energy level.
             
             Read/write access to modify energy.
             )pbdoc")
        .def_readwrite("health", &AgentBody::health,
             R"pbdoc(Current health level.
             
             Read/write access to modify health.
             )pbdoc")
        .def_readwrite("age", &AgentBody::age,
             R"pbdoc(Agent age in simulation steps.
             
             Read/write access to set/modify age.
             )pbdoc")
        .def_readwrite("isMoving", &AgentBody::isMoving,
             R"pbdoc(Whether the agent is currently moving.
             
             Read/write access to set movement state.
             )pbdoc")
        .def_readwrite("isTurning", &AgentBody::isTurning,
             R"pbdoc(Whether the agent is currently turning.
             
             Read/write access to set turning state.
             )pbdoc")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
             R"pbdoc(Time since last action.
             
             Read/write access to set/modify last action time.
             )pbdoc")
        .def("reset", &AgentBody::reset,
             R"pbdoc(Reset agent body to initial state.
             
             Resets position, velocity, energy, health, and age to defaults.
             )pbdoc");

    // ActionResult class with enhanced interface
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)
    
    Represents the result of an action performed in the world. Contains reward,
    success status, and a message describing what happened.
    
    Example usage:
    ```python
    # Create action result
    result = ActionResult(reward=10.0, success=True, message="Found food!")
    
    # Access result properties
    print(f"Reward: {result.reward}")
    print(f"Success: {result.success}")
    print(f"Message: {result.message}")
    ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             R"pbdoc(Create an ActionResult.
             
             Args:
                 reward: Reward value from the action
                 success: Whether the action succeeded
                 message: Description of what happened
             )pbdoc")
        .def_readwrite("reward", &ActionResult::reward,
             R"pbdoc(Reward value from the action.
             
             Read/write access to modify reward.
             )pbdoc")
        .def_readwrite("success", &ActionResult::success,
             R"pbdoc(Whether the action succeeded.
             
             Read/write access to modify success status.
             )pbdoc")
        .def_readwrite("message", &ActionResult::message,
             R"pbdoc(Message describing what happened.
             
             Read/write access to modify message.
             )pbdoc");

    // SensoryPercept class with enhanced interface
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
    
    Contains all sensory information available to the agent at a given time.
    This includes vision, touch, internal signals, proprioception, and audio.
    
    The percept serves as the interface between the world and the agent's brain.
    
    Example usage:
    ```python
    # Create sensory percept
    percept = SensoryPercept()
    
    # Set individual modalities
    percept.setVision(vision_input)
    percept.setTouch(touch_input)
    percept.setInternal(internal_input)
    percept.setProprioception(proprioception_input)
    percept.setAudio(audio_input)
    
    # Access modalities
    vision = percept.getVision()
    touch = percept.getTouch()
    # etc.
    ```
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             R"pbdoc(Get vision sensory data.
             
             Returns:
                 Vision sensory input object
             )pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             R"pbdoc(Set vision sensory data.
             
             Args:
                 vision: Vision sensory input object
             )pbdoc")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             R"pbdoc(Get vision width.
             
             Returns:
                 Vision width in pixels
             )pbdoc")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             R"pbdoc(Get vision height.
             
             Returns:
                 Vision height in pixels
             )pbdoc")
        .def("getTouch", &SensoryPercept::getTouch,
             R"pbdoc(Get touch sensory data.
             
             Returns:
                 Touch sensory input object
             )pbdoc")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             R"pbdoc(Set touch sensory data.
             
             Args:
                 touch: Touch sensory input object
             )pbdoc")
        .def("getInternal", &SensoryPercept::getInternal,
             R"pbdoc(Get internal signals sensory data.
             
             Returns:
                 Internal signals sensory input object
             )pbdoc")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             R"pbdoc(Set internal signals sensory data.
             
             Args:
                 internal: Internal signals sensory input object
             )pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception,
             R"pbdoc(Get proprioception sensory data.
             
             Returns:
                 Proprioception sensory input object
             )pbdoc")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             R"pbdoc(Set proprioception sensory data.
             
             Args:
                 proprioception: Proprioception sensory input object
             )pbdoc")
        .def("getAudio", &SensoryPercept::getAudio,
             R"pbdoc(Get audio sensory data.
             
             Returns:
                 Audio sensory input object
             )pbdoc")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             R"pbdoc(Set audio sensory data.
             
             Args:
                 audio: Audio sensory input object
             )pbdoc")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             R"pbdoc(Get all sensory signals as a dictionary.
             
             Returns:
                 Dictionary containing all sensory modalities
             )pbdoc")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             R"pbdoc(Get the percept timestamp.
             
             Returns:
                 Timestamp when this percept was created
             )pbdoc")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             R"pbdoc(Set the percept timestamp.
             
             Args:
                 timestamp: New timestamp
             )pbdoc");

    // SimpleWorld class with enhanced interface
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
    
    The simulation world where the agent lives and interacts. Provides sensory
    experience without semantic labels, maintaining the world's physics and state.
    
    The world manages agent position, movement, collisions, and interactions
    with world objects.
    
    Example usage:
    ```python
    # Create world with enhanced interface
    world = SimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    # Set agent start position
    world.setAgentStart(10, 10)
    
    # Reset world
    world.reset()
    
    # Update world state
    world.update(0.1)
    
    # Get agent perception
    percept = world.getSensoryPercept()
    
    # Apply action
    result = world.applyMotorCommand(MotorCommand.MoveForward, world.getSimulationTime())
    
    # World properties
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Simulation time: {world.getSimulationTime()}")
    ```
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(Configure the world.
             
             Args:
                 width: World width
                 height: World height
                 visionWidth: Vision sensor width
                 visionHeight: Vision sensor height
             )pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(Reset the world to initial state.
             
             Resets agent position, removes all objects, and resets simulation time.
             )pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(Set the agent's starting position.
             
             Args:
                 x: Starting X coordinate
                 y: Starting Y coordinate
             )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Update world state.
             
             Args:
                 timestep: Time step for this update
             )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(Apply a motor command to the agent.
             
             Args:
                 cmd: Motor command to execute
                 currentTime: Current simulation time
                 
             Returns:
                 ActionResult describing what happened
             )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the current sensory percept.
             
             Returns:
                 SensoryPercept object containing all sensory data
             )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the agent's body state.
             
             Returns:
                 AgentBody object with position, velocity, and physiological state
             )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(Add an object to the world.
             
             Args:
                 obj: WorldObject to add
             )pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(Remove object at position.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
             )pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(Check if a position is valid (not a wall).
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 
             Returns:
                 True if position is valid, False if it's a wall
             )pbdoc")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             R"pbdoc(Get object at position.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 
             Returns:
                 WorldObject at position, or None if no object
             )pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(Get world width.
             
             Returns:
                 World width
             )pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(Get world height.
             
             Returns:
                 World height
             )pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(Get maximum agent energy.
             
             Returns:
                 Maximum energy level
             )pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(Set maximum agent energy.
             
             Args:
                 e: New maximum energy
             )pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(Get energy decay rate.
             
             Returns:
                 Energy decay rate per timestep
             )pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(Set energy decay rate.
             
             Args:
                 r: New energy decay rate
             )pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(Get current simulation time.
             
             Returns:
                 Current simulation time
             )pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(Set random seed for reproducibility.
             
             Args:
                 seed: Random seed value
             )pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(Get current random seed.
             
             Returns:
                 Current random seed
             )pbdoc");

    // Enhanced Brain class with context manager and Pythonic interface
    // Create the enhanced Brain wrapper
    py::class_<PythonBrain>(m, "PythonBrain", R"pbdoc(Enhanced Brain interface with Pythonic features)
    
    Enhanced interface to the NLM Brain with Pythonic design patterns and convenience methods.
    
    This wrapper provides:
    - Context manager support for automatic resource management
    - Fluent API for common operations
    - Property-style access to brain state
    - Group operations for common patterns
    - Type checking and validation
    
    Example usage:
    ```python
    import pynlm
    
    # Create brain with enhanced interface
    with pynlm.PythonBrain(pynlm.createBrain(pynlm.createDefaultConfig())) as brain:
        brain.initialize()
        
        # Use fluent API
        brain.step(0).step(1).step(2)
        
        # Access properties
        print(f"Neurons: {brain.getNeuronCount()}")
        print(f"Active: {brain.getActiveNeuronCount()}")
        print(f"Firing rate: {brain.getAverageFiringRate()}")
        
        # Run multiple steps efficiently
        brain.runSteps(100, 0.1)
        
        # Inject currents
        brain.injectCurrent(neuron_id, 1.0)
        brain.injectCurrentToType(NeuronType.Excitatory, 0.5)
    ```
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(Create a PythonBrain wrapper.
             
             Args:
                 brain: The underlying C++ Brain object
             )pbdoc")
        .def("__enter__", &PythonBrain::__enter__,
             R"pbdoc(Context manager entry.
             
             Returns:
                 Self for chaining
             )pbdoc")
        .def("__exit__", &PythonBrain::__exit__, py::arg("type"), py::arg("value"), py::arg("traceback"),
             R"pbdoc(Context manager exit - resets brain state.
             
             Args:
                 type: Exception type
                 value: Exception value
                 traceback: Exception traceback
             )pbdoc")
        .def("initialize", &PythonBrain::initialize,
             R"pbdoc(Initialize the brain with configuration.
             
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("step", static_cast<PythonBrain& (PythonBrain::*)(SimulationStep, Timestamp)>(&PythonBrain::step),
             py::arg("currentStep"), py::arg("currentTime") = 0,
             R"pbdoc(Perform a simulation step.
             
             Args:
                 currentStep: Current simulation step
                 currentTime: Current simulation time (default: 0)
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("step", static_cast<PythonBrain& (PythonBrain::*)(SimulationStep)>(&PythonBrain::step),
             py::arg("currentStep"),
             R"pbdoc(Perform a simulation step with default time.
             
             Args:
                 currentStep: Current simulation step
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("injectSensory", &PythonBrain::injectSensory, py::arg("input"),
             R"pbdoc(Inject sensory input into the brain.
             
             Args:
                 input: Sensory input object
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("injectCurrent", &PythonBrain::injectCurrent, py::arg("neuron"), py::arg("current"),
             R"pbdoc(Inject current into a specific neuron.
             
             Args:
                 neuron: Neuron ID
                 current: Current value to inject
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("injectCurrentToType", &PythonBrain::injectCurrentToType, py::arg("type"), py::arg("current"),
             R"pbdoc(Inject current into all neurons of a specific type.
             
             Args:
                 type: Neuron type
                 current: Current value to inject
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("runSteps", &PythonBrain::runSteps, py::arg("numSteps"), py::arg("timeStep") = 1.0,
             R"pbdoc(Run multiple simulation steps.
             
             Args:
                 numSteps: Number of steps to run
                 timeStep: Time between steps (default: 1.0)
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("runWithInputs", &PythonBrain::runWithInputs, py::arg("inputs"),
             R"pbdoc(Run simulation with multiple sensory inputs.
             
             Args:
                 inputs: List of (sensory_input, step) tuples
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("getBrain", &PythonBrain::getBrain,
             R"pbdoc(Get the underlying C++ Brain object.
             
             Returns:
                 The Brain object
             )pbdoc")
        .def("reset", &PythonBrain::reset,
             R"pbdoc(Reset brain state to initial state.
             
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("getNeuronCount", &PythonBrain::getNeuronCount,
             R"pbdoc(Get total neuron count.
             
             Returns:
                 Total number of neurons
             )pbdoc")
        .def("getSynapseCount", &PythonBrain::getSynapseCount,
             R"pbdoc(Get total synapse count.
             
             Returns:
                 Total number of synapses
             )pbdoc")
        .def("getActiveNeuronCount", &PythonBrain::getActiveNeuronCount,
             R"pbdoc(Get count of active neurons.
             
             Returns:
                 Number of active neurons
             )pbdoc")
        .def("getFiringNeuronCount", &PythonBrain::getFiringNeuronCount,
             R"pbdoc(Get count of currently firing neurons.
             
             Returns:
                 Number of firing neurons
             )pbdoc")
        .def("getAverageFiringRate", &PythonBrain::getAverageFiringRate,
             R"pbdoc(Get average firing rate across all neurons.
             
             Returns:
                 Average firing rate (Hz)
             )pbdoc")
        .def("getExcitationInhibitionRatio", &PythonBrain::getExcitationInhibitionRatio,
             R"pbdoc(Get excitation/inhibition balance ratio.
             
             Returns:
                 Excitation/inhibition ratio
             )pbdoc")
        .def("getTotalSpikeCount", &PythonBrain::getTotalSpikeCount,
             R"pbdoc(Get total spike count.
             
             Returns:
                 Total number of spikes recorded
             )pbdoc")
        .def("getDevelopmentalStage", &PythonBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.
             
             Returns:
                 Current developmental stage
             )pbdoc")
        .def("getConfig", &PythonBrain::getConfig,
             R"pbdoc(Get the configuration.
             
             Returns:
                 Configuration object
             )pbdoc")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             R"pbdoc(Get working memory system.
             
             Returns:
                 Working memory system pointer
             )pbdoc")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             R"pbdoc(Get episodic memory system.
             
             Returns:
                 Episodic memory system pointer
             )pbdoc")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             R"pbdoc(Get associative memory system.
             
             Returns:
                 Associative memory system pointer
             )pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(Log brain status and statistics.
             
             This is useful for debugging and monitoring.
             )pbdoc")
        .def("__repr__", &PythonBrain::repr,
             R"pbdoc(Get string representation.
             
             Returns:
                 String representation of the brain
             )pbdoc");

    // Enhanced SimpleWorld wrapper with context manager
    py::class_<PythonWorld>(m, "PythonWorld", R"pbdoc(Enhanced SimpleWorld interface with Pythonic features)
    
    Enhanced interface to the SimpleWorld with Pythonic design patterns and convenience methods.
    
    This wrapper provides:
    - Context manager support for automatic resource management
    - Fluent API for common operations
    - Property-style access to world state
    - Group operations for common patterns
    
    Example usage:
    ```python
    import pynlm
    
    # Create world with enhanced interface
    with pynlm.PythonWorld(pynlm.createSimpleWorld()) as world:
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.setAgentStart(10, 10)
        
        # Use fluent API
        world.reset().runSteps(0.1, 10)
        
        # Get state
        print(f"Agent position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
        print(f"World time: {world.getSimulationTime()}")
        
        # Update world
        world.update(0.1)
        
        # Get percept
        percept = world.getPercept()
    ```
    )pbdoc")
        .def(py::init<std::shared_ptr<SimpleWorld>>(), py::arg("world"),
             R"pbdoc(Create a PythonWorld wrapper.
             
             Args:
                 world: The underlying C++ SimpleWorld object
             )pbdoc")
        .def("__enter__", &PythonWorld::__enter__,
             R"pbdoc(Context manager entry.
             
             Returns:
                 Self for chaining
             )pbdoc")
        .def("__exit__", &PythonWorld::__exit__, py::arg("type"), py::arg("value"), py::arg("traceback"),
             R"pbdoc(Context manager exit - resets world state.
             
             Args:
                 type: Exception type
                 value: Exception value
                 traceback: Exception traceback
             )pbdoc")
        .def("configure", &PythonWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(Configure the world.
             
             Args:
                 width: World width
                 height: World height
                 visionWidth: Vision sensor width
                 visionHeight: Vision sensor height
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("reset", &PythonWorld::reset,
             R"pbdoc(Reset the world to initial state.
             
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("setStart", &PythonWorld::setStart, py::arg("x"), py::arg("y"),
             R"pbdoc(Set the agent's starting position.
             
             Args:
                 x: Starting X coordinate
                 y: Starting Y coordinate
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("update", &PythonWorld::update, py::arg("timestep"),
             R"pbdoc(Update world state.
             
             Args:
                 timestep: Time step for this update
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("applyCommand", &PythonWorld::applyCommand, py::arg("cmd"), py::arg("time"),
             R"pbdoc(Apply a motor command to the agent.
             
             Args:
                 cmd: Motor command to execute
                 time: Current simulation time
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("runSteps", &PythonWorld::runSteps, py::arg("timestep"), py::arg("numSteps"),
             R"pbdoc(Run multiple world updates.
             
             Args:
                 timestep: Time between updates
                 numSteps: Number of updates to run
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("getWorld", &PythonWorld::getWorld,
             R"pbdoc(Get the underlying C++ SimpleWorld object.
             
             Returns:
                 The SimpleWorld object
             )pbdoc")
        .def("reset", &PythonWorld::reset,
             R"pbdoc(Reset world state to initial state.
             
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("getWidth", &PythonWorld::getWidth,
             R"pbdoc(Get world width.
             
             Returns:
                 World width
             )pbdoc")
        .def("getHeight", &PythonWorld::getHeight,
             R"pbdoc(Get world height.
             
             Returns:
                 World height
             )pbdoc")
        .def("getSimulationTime", &PythonWorld::getSimulationTime,
             R"pbdoc(Get current simulation time.
             
             Returns:
                 Current simulation time
             )pbdoc")
        .def("getRandomSeed", &PythonWorld::getRandomSeed,
             R"pbdoc(Get current random seed.
             
             Returns:
                 Current random seed
             )pbdoc")
        .def("getPercept", &PythonWorld::getPercept,
             R"pbdoc(Get the current sensory percept.
             
             Returns:
                 SensoryPercept object
             )pbdoc")
        .def("getAgentBody", &PythonWorld::getAgentBody,
             R"pbdoc(Get the agent's body state.
             
             Returns:
                 AgentBody object
             )pbdoc")
        .def("__repr__", &PythonWorld::repr,
             R"pbdoc(Get string representation.
             
             Returns:
                 String representation of the world
             )pbdoc");

    // Enhanced AgentBrain wrapper with context manager
    py::class_<PythonAgentBrain>(m, "PythonAgentBrain", R"pbdoc(Enhanced AgentBrain interface with Pythonic features)
    
    Enhanced interface to the AgentBrain with Pythonic design patterns and convenience methods.
    
    This wrapper provides:
    - Context manager support for automatic resource management
    - Fluent API for common operations
    - Property-style access to brain state
    - Group operations for common patterns
    
    Example usage:
    ```python
    import pynlm
    
    # Create agent brain with enhanced interface
    with pynlm.PythonAgentBrain(pynlm.createAgentBrain(brain)) as agent:
        agent.initialize(world)
        
        # Use fluent API
        agent.processInput(percept).updateDevelopment(0.1)
        
        # Access properties
        print(f"Developmental stage: {agent.getDevelopmentalStage()}")
        print(f"Curiosity level: {agent.getCuriosityLevel()}")
        print(f"Prediction error: {agent.getPredictionError()}")
        
        # Decode command
        action = agent.decodeCommand()
        
        # Enable features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
    ```
    )pbdoc")
        .def(py::init<std::shared_ptr<AgentBrain>>(), py::arg("agentBrain"),
             R"pbdoc(Create a PythonAgentBrain wrapper.
             
             Args:
                 agentBrain: The underlying C++ AgentBrain object
             )pbdoc")
        .def("__enter__", &PythonAgentBrain::__enter__,
             R"pbdoc(Context manager entry.
             
             Returns:
                 Self for chaining
             )pbdoc")
        .def("__exit__", &PythonAgentBrain::__exit__, py::arg("type"), py::arg("value"), py::arg("traceback"),
             R"pbdoc(Context manager exit - resets agent state.
             
             Args:
                 type: Exception type
                 value: Exception value
                 traceback: Exception traceback
             )pbdoc")
        .def("initialize", &PythonAgentBrain::initialize, py::arg("world"),
             R"pbdoc(Initialize the agent brain with world.
             
             Args:
                 world: SimpleWorld object
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("processInput", &PythonAgentBrain::processInput, py::arg("percept"),
             R"pbdoc(Process sensory input.
             
             Args:
                 percept: SensoryPercept object
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("decodeCommand", &PythonAgentBrain::decodeCommand,
             R"pbdoc(Decode brain motor activity into motor command.
             
             Returns:
                 MotorCommand object
             )pbdoc")
        .def("decodeCommandWithCuriosity", &PythonAgentBrain::decodeCommandWithCuriosity,
             R"pbdoc(Decode motor command with curiosity consideration.
             
             Returns:
                 MotorCommand object
             )pbdoc")
        .def("updateDevelopment", &PythonAgentBrain::updateDevelopment, py::arg("timestep"),
             R"pbdoc(Update development system.
             
             Args:
                 timestep: Time step for development update
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("applyReward", &PythonAgentBrain::applyReward, py::arg("reward"), py::arg("predicted"),
             R"pbdoc(Apply reward-based neuromodulation.
             
             Args:
                 reward: Actual reward received
                 predicted: Predicted reward
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("getAgentBrain", &PythonAgentBrain::getAgentBrain,
             R"pbdoc(Get the underlying C++ AgentBrain object.
             
             Returns:
                 The AgentBrain object
             )pbdoc")
        .def("reset", &PythonAgentBrain::reset,
             R"pbdoc(Reset agent for new episode.
             
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("getSensoryInputSize", &PythonAgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size.
             
             Returns:
                 Sensory input size
             )pbdoc")
        .def("getMotorOutputSize", &PythonAgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size.
             
             Returns:
                 Motor output size
             )pbdoc")
        .def("getDevelopmentalStage", &PythonAgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.
             
             Returns:
                 Current developmental stage
             )pbdoc")
        .def("getNeuromodulationLevel", &PythonAgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level.
             
             Returns:
                 Neuromodulation level
             )pbdoc")
        .def("getCuriosityLevel", &PythonAgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level.
             
             Returns:
                 Curiosity level
             )pbdoc")
        .def("getNoveltyLevel", &PythonAgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty level.
             
             Returns:
                 Novelty level
             )pbdoc")
        .def("getPredictionError", &PythonAgentBrain::getPredictionError,
             R"pbdoc(Get prediction error.
             
             Returns:
                 Prediction error
             )pbdoc")
        .def("enableRewardModulation", &PythonAgentBrain::enableRewardModulation, py::arg("enable"),
             R"pbdoc(Enable or disable reward modulation.
             
             Args:
                 enable: Whether to enable reward modulation
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("enableStructuralPlasticity", &PythonAgentBrain::enableStructuralPlasticity, py::arg("enable"),
             R"pbdoc(Enable or disable structural plasticity.
             
             Args:
                 enable: Whether to enable structural plasticity
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("enableDevelopment", &PythonAgentBrain::enableDevelopment, py::arg("enable"),
             R"pbdoc(Enable or disable development.
             
             Args:
                 enable: Whether to enable development
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("enableCuriosity", &PythonAgentBrain::enableCuriosity, py::arg("enable"),
             R"pbdoc(Enable or disable curiosity.
             
             Args:
                 enable: Whether to enable curiosity
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("isRewardModulationEnabled", &PythonAgentBrain::isRewardModulationEnabled,
             R"pbdoc(Check if reward modulation is enabled.
             
             Returns:
                 True if reward modulation is enabled
             )pbdoc")
        .def("isStructuralPlasticityEnabled", &PythonAgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(Check if structural plasticity is enabled.
             
             Returns:
                 True if structural plasticity is enabled
             )pbdoc")
        .def("isDevelopmentEnabled", &PythonAgentBrain::isDevelopmentEnabled,
             R"pbdoc(Check if development is enabled.
             
             Returns:
                 True if development is enabled
             )pbdoc")
        .def("isCuriosityEnabled", &PythonAgentBrain::isCuriosityEnabled,
             R"pbdoc(Check if curiosity is enabled.
             
             Returns:
                 True if curiosity is enabled
             )pbdoc")
        .def("__repr__", &PythonAgentBrain::repr,
             R"pbdoc(Get string representation.
             
             Returns:
                 String representation of the agent brain
             )pbdoc");

    // Simulation runner helper
    py::class_<PythonSimulationRunner>(m, "SimulationRunner", R"pbdoc(Simulation helper for running complete experiments)
    
    Helper class for running complete simulations with proper setup and teardown.
    
    The SimulationRunner manages the complete simulation loop including:
    - Initialization of all components
    - Step-by-step simulation execution
    - Data collection and processing
    - Error handling and cleanup
    
    Example usage:
    ```python
    import pynlm
    
    # Create simulation runner
    runner = SimulationRunner()
    
    # Run a simulation
    results = runner.runSimulation(brain, world, agent, 100, 0.1)
    
    # Process results
    for step in results:
        print(f"Step {step.step}: Reward={step.reward}, Success={step.success}")
    ```
    )pbdoc")
        .def(py::init<>())
        .def("runSimulation", &PythonSimulationRunner::runSimulation,
             py::arg("brain"), py::arg("world"), py::arg("agent"),
             py::arg("numSteps"), py::arg("timestep") = 0.1,
             py::arg("enableLearning") = false,
             R"pbdoc(Run a complete simulation.
             
             Args:
                 brain: Brain object
                 world: World object  
                 agent: AgentBrain object
                 numSteps: Number of simulation steps
                 timestep: Time between steps (default: 0.1)
                 enableLearning: Whether to enable learning features (default: False)
                 
             Returns:
                 List of simulation step results
             )pbdoc")
        .def("runWithCallback", &PythonSimulationRunner::runWithCallback,
             py::arg("brain"), py::arg("world"), py::arg("agent"),
             py::arg("numSteps"), py::arg("timestep"),
             py::arg("callback"),
             R"pbdoc(Run simulation with custom callback.
             
             Args:
                 brain: Brain object
                 world: World object
                 agent: AgentBrain object
                 numSteps: Number of simulation steps
                 timestep: Time between steps
                 callback: Function called for each step with step data
             )pbdoc")
        .def("captureVisualizationData", &PythonSimulationRunner::captureVisualizationData,
             py::arg("brain"), py::arg("maxNeurons") = 100,
             R"pbdoc(Capture visualization data from brain.
             
             Args:
                 brain: Brain object
                 maxNeurons: Maximum number of neurons to capture (default: 100)
                 
             Returns:
                 Dictionary with visualization data
             )pbdoc");

    // Configuration builder helper
    py::class_<PythonConfigBuilder>(m, "ConfigBuilder", R"pbdoc(Configuration builder for common NLM setups)
    
    Builder class for creating common NLM configurations with ease.
    
    Provides factory methods for creating standard configurations:
    - Basic: Minimal configuration for testing
    - Advanced: Full-featured configuration with all systems
    - Exploration: Configuration optimized for exploration and learning
    - Production: Configuration optimized for stable performance
    
    Example usage:
    ```python
    import pynlm
    
    # Create basic config
    config = ConfigBuilder().setBasicConfig().build()
    
    # Create advanced config
    config = ConfigBuilder().setAdvancedConfig().build()
    
    # Create exploration config
    config = ConfigBuilder().setExplorationConfig().build()
    
    # Create production config
    config = ConfigBuilder().setProductionConfig().build()
    
    # Custom config
    config = ConfigBuilder().setCustom("learning_rate", 0.05).build()
    ```
    )pbdoc")
        .def(py::init<>())
        .def("setBasicConfig", &PythonConfigBuilder::setBasicConfig,
             R"pbdoc(Set basic configuration.
             
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("setAdvancedConfig", &PythonConfigBuilder::setAdvancedConfig,
             R"pbdoc(Set advanced configuration.
             
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("setExplorationConfig", &PythonConfigBuilder::setExplorationConfig,
             R"pbdoc(Set exploration configuration.
             
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("setProductionConfig", &PythonConfigBuilder::setProductionConfig,
             R"pbdoc(Set production configuration.
             
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("setCustom", &PythonConfigBuilder::setCustom, py::arg("key"), py::arg("value"),
             R"pbdoc(Set a custom configuration value.
             
             Args:
                 key: Configuration key
                 value: Configuration value
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("setCustom", &PythonConfigBuilder::setCustom, py::arg("key"), py::arg("value"),
             R"pbdoc(Set a custom numeric configuration value.
             
             Args:
                 key: Configuration key
                 value: Numeric value (int or float)
                 
             Returns:
                 Self for fluent API
             )pbdoc")
        .def("build", &PythonConfigBuilder::build,
             R"pbdoc(Build the configuration.
             
             Returns:
                 Config object
             )pbdoc")
        .def("createDefault", &PythonConfigBuilder::createDefault,
             R"pbdoc(Create a default configuration.
             
             Returns:
                 Config object
             )pbdoc")
        .def("createAdvanced", &PythonConfigBuilder::createAdvanced,
             R"pbdoc(Create an advanced configuration.
             
             Returns:
                 Config object
             )pbdoc")
        .def("createExploration", &PythonConfigBuilder::createExploration,
             R"pbdoc(Create an exploration configuration.
             
             Returns:
                 Config object
             )pbdoc");

    // Visualization helper
    py::class_<PythonVisualizationHelper>(m, "VisualizationHelper", R"pbdoc(Visualization and data analysis helpers)
    
    Helper class for visualizing and analyzing NLM simulation data.
    
    Provides methods for creating plots, animations, and data analysis.
    
    Example usage:
    ```python
    import pynlm
    
    # Create visualization helper
    viz = VisualizationHelper()
    
    # Create firing rate plot
    plot = viz.createFiringRatePlot(times, rates)
    
    # Create spike count plot
    plot = viz.createSpikeCountPlot(times, spike_counts)
    
    # Create animation frames
    frames = viz.createAnimationFrames(brain, 100)
    
    # String encoding helpers
    utf8_text = VisualizationHelper.toUtf8("Hello World")
    bytes_data = VisualizationHelper.toBytes("data")
    ```
    )pbdoc")
        .def(py::init<>())
        .def("createFiringRatePlot", &PythonVisualizationHelper::createFiringRatePlot,
             py::arg("times"), py::arg("rates"),
             R"pbdoc(Create firing rate plot data.
             
             Args:
                 times: List of time values
                 rates: List of firing rates
                 
             Returns:
                 Dictionary with plot data
             )pbdoc")
        .def("createSpikeCountPlot", &PythonVisualizationHelper::createSpikeCountPlot,
             py::arg("times"), py::arg("spikeCounts"),
             R"pbdoc(Create spike count plot data.
             
             Args:
                 times: List of time values
                 spikeCounts: List of spike counts
                 
             Returns:
                 Dictionary with plot data
             )pbdoc")
        .def("createAnimationFrames", &PythonVisualizationHelper::createAnimationFrames,
             py::arg("brain"), py::arg("numFrames") = 100,
             R"pbdoc(Create animation frames from brain state.
             
             Args:
                 brain: Brain object
                 numFrames: Number of animation frames
                 
             Returns:
                 List of animation frames
             )pbdoc")
        .def("toUtf8", &PythonVisualizationHelper::toUtf8, py::arg("input"),
             R"pbdoc(Convert string to UTF-8 encoding.
             
             Args:
                 input: Input string
                 
             Returns:
                 UTF-8 encoded string
             )pbdoc")
        .def("toBytes", &PythonVisualizationHelper::toBytes, py::arg("input"),
             R"pbdoc(Convert string to bytes.
             
             Args:
                 input: Input string
                 
             Returns:
                 Bytes representation
             )pbdoc")
        .def("encode", &PythonVisualizationHelper::encode, py::arg("input"), py::arg("encoding") = "utf-8",
             R"pbdoc(Encode string to specified encoding.
             
             Args:
                 input: Input string
                 encoding: Target encoding (default: utf-8)
                 
             Returns:
                 Encoded string
             )pbdoc")
        .def("decode", &PythonVisualizationHelper::decode, py::arg("input"), py::arg("encoding") = "utf-8",
             R"pbdoc(Decode string from specified encoding.
             
             Args:
                 input: Input string
                 encoding: Source encoding (default: utf-8)
                 
             Returns:
                 Decoded string
             )pbdoc");

    // Type checking helper functions
    m.def("validateNeuronId", &TypeChecker::validateNeuronId, py::arg("id"),
         R"pbdoc(Validate a NeuronId.
         
         Args:
             id: NeuronId to validate
             
         Raises:
             NLMException if ID is invalid
         )pbdoc")
    
    m.def("validateSynapseId", &TypeChecker::validateSynapseId, py::arg("id"),
         R"pbdoc(Validate a SynapseId.
         
         Args:
             id: SynapseId to validate
             
         Raises:
             NLMException if ID is invalid
         )pbdoc")
    
    m.def("validateRegionId", &TypeChecker::validateRegionId, py::arg("id"),
         R"pbdoc(Validate a RegionId.
         
         Args:
             id: RegionId to validate
             
         Raises:
             NLMException if ID is invalid
         )pbdoc")
    
    m.def("validatePopulationId", &TypeChecker::validatePopulationId, py::arg("id"),
         R"pbdoc(Validate a PopulationId.
         
         Args:
             id: PopulationId to validate
             
         Raises:
             NLMException if ID is invalid
         )pbdoc")
    
    m.def("validateConfig", &TypeChecker::validateConfig, py::arg("config"),
         R"pbdoc(Validate a Config object.
         
         Args:
             config: Config object to validate
             
         Raises:
             ConfigurationError if config is invalid
         )pbdoc")
    
    m.def("validateBrain", &TypeChecker::validateBrain, py::arg("brain"),
         R"pbdoc(Validate a Brain object.
         
         Args:
             brain: Brain object to validate
             
         Raises:
             InitializationError if brain is invalid
         )pbdoc")
    
    m.def("validateWorld", &TypeChecker::validateWorld, py::arg("world"),
         R"pbdoc(Validate a SimpleWorld object.
         
         Args:
             world: SimpleWorld object to validate
             
         Raises:
             SimulationError if world is invalid
         )pbdoc")
    
    m.def("validateAgent", &TypeChecker::validateAgent, py::arg("agent"),
         R"pbdoc(Validate an AgentBrain object.
         
         Args:
             agent: AgentBrain object to validate
             
         Raises:
             InitializationError if agent is invalid
         )pbdoc")

    // Enhanced factory functions with Pythonic interfaces
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"),
         R"pbdoc(Create a Brain with configuration.
         
         Args:
             config: Configuration object
             
         Returns:
             Brain object
         )pbdoc");

    m.def("createPythonBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<PythonBrain> {
        return std::make_shared<PythonBrain>(std::make_shared<Brain>(config));
    }, py::arg("config"),
         R"pbdoc(Create a PythonBrain with configuration.
         
         Args:
             config: Configuration object
             
         Returns:
             PythonBrain object with enhanced interface
         )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a SimpleWorld.
         
         Returns:
             SimpleWorld object
         )pbdoc");

    m.def("createPythonWorld", []() -> std::shared_ptr<PythonWorld> {
        return std::make_shared<PythonWorld>(std::make_shared<SimpleWorld>());
    }, R"pbdoc(Create a PythonWorld with enhanced interface.
         
         Returns:
             PythonWorld object with enhanced interface
         )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"),
         R"pbdoc(Create an AgentBrain with brain.
         
         Args:
             brain: Brain object
             
         Returns:
             AgentBrain object
         )pbdoc");

    m.def("createPythonAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<PythonAgentBrain> {
        return std::make_shared<PythonAgentBrain>(std::make_shared<AgentBrain>(brain));
    }, py::arg("brain"),
         R"pbdoc(Create a PythonAgentBrain with enhanced interface.
         
         Args:
             brain: Brain object
             
         Returns:
             PythonAgentBrain object with enhanced interface
         )pbdoc");

    // Constants for common values
    m.attr("VERSION") = "0.1.0";
    m.attr("API_VERSION") = "enhanced";
    
    // Module metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__api_version__") = "enhanced";
    
    // Exception classes
    py::class_<NLMException, py::base<std::runtime_error>>(m, "NLMException")
        .def(py::init<const std::string&>());
    
    py::class_<ConfigurationError, py::base<NLMException>>(m, "ConfigurationError")
        .def(py::init<const std::string&>());
    
    py::class_<SimulationError, py::base<NLMException>>(m, "SimulationError")
        .def(py::init<const std::string&>());
    
    py::class_<InitializationError, py::base<NLMException>>(m, "InitializationError")
        .def(py::init<const std::string&>());

    // Invalid IDs constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
