#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <random>
#include <chrono>

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

// Custom Python exceptions for NLM
class NLMException : public py::exception<std::runtime_error> {
public:
    using py::exception<std::runtime_error>::exception;
};

class ConfigValidationError : public NLMException {
public:
    using NLMException::exception;
    ConfigValidationError(const std::string& message) : NLMException(message) {}
};

class BrainInitializationError : public NLMException {
public:
    using NLMException::exception;
    BrainInitializationError(const std::string& message) : NLMException(message) {}
};

class MemoryAccessError : public NLMException {
public:
    using NLMException::exception;
    MemoryAccessError(const std::string& message) : NLMException(message) {}
};

class SimulationError : public NLMException {
public:
    using NLMException::exception;
    SimulationError(const std::string& message) : NLMException(message) {}
};

// Forward declarations for helper functions
class EnhancedConfig;
class EnhancedBrain;
class EnhancedAgentBrain;
class NLMHelper;

// Enhanced configuration class with better Python integration
class EnhancedConfig {
public:
    EnhancedConfig() : config(std::make_shared<Config>()) {}
    
    // Load from file with validation
    bool loadFromFile(const std::string& filepath) {
        return config->loadFromFile(filepath);
    }
    
    // Load from file with schema validation
    bool loadFromFileWithValidation(const std::string& filepath, const ConfigSchema& schema) {
        auto result = config->validateAll(schema);
        bool allValid = std::all_of(result.begin(), result.end(), 
                                   [](const ConfigValidationResult& r) { return r.valid; });
        if (!allValid) {
            std::string errors;
            for (const auto& r : result) {
                if (!r.valid) errors += r.errorMessage + "; ";
            }
            throw ConfigValidationError("Configuration validation failed: " + errors);
        }
        return config->loadFromFile(filepath);
    }
    
    // Enhanced get methods with type safety and validation
    template<typename T>
    T getWithTypeCheck(const std::string& key) {
        if (!config->has(key)) {
            throw ConfigValidationError("Configuration key not found: " + key);
        }
        return config->get<T>(key);
    }
    
    template<typename T>
    T getWithTypeCheck(const std::string& key, const T& defaultValue) {
        return config->get<T>(key, defaultValue);
    }
    
    // Set methods with validation
    void setValidated(const std::string& key, const ConfigValue& value, const ConfigSchema& schema) {
        auto validation = schema.validate(key, value);
        if (!validation.valid) {
            throw ConfigValidationError("Invalid configuration value for key '" + key + "': " + validation.errorMessage);
        }
        config->set(key, value);
    }
    
    // Get configuration summary with schema
    std::string getSummaryWithSchema(const ConfigSchema& schema) const {
        std::ostringstream oss;
        oss << "Configuration Summary (Schema Validated):" << std::endl;
        oss << "  Valid: " << (config->isValid(schema) ? "Yes" : "No") << std::endl;
        oss << "  Keys: " << config->getKeys().size() << std::endl;
        oss << "  Deprecated: " << config->getDeprecatedKeys().size() << std::endl;
        
        auto errors = config->getValidationErrors();
        if (!errors.empty()) {
            oss << "  Validation Errors: " << std::endl;
            for (const auto& error : errors) {
                oss << "    - " << error << std::endl;
            }
        }
        
        return oss.str();
    }
    
    // Get validation results
    std::vector<ConfigValidationResult> getValidationResults(const ConfigSchema& schema) const {
        return config->validateAll(schema);
    }
    
    // Check if configuration is valid
    bool isValid(const ConfigSchema& schema) const {
        return config->isValid(schema);
    }
    
    // Get raw config for advanced usage
    std::shared_ptr<Config> getRawConfig() const {
        return config;
    }
    
    // String representation
    std::string toString() const {
        return "EnhancedConfig[keys=" + std::to_string(config->getKeys().size()) + "]";
    }
    
    friend class EnhancedBrain;
    
private:
    std::shared_ptr<Config> config;
};

// Forward declaration for EnhancedBrain to handle MemorySystemAccessor
class EnhancedBrain;

class EnhancedBrain::MemorySystemAccessor {
public:
    MemorySystemAccessor(std::shared_ptr<Brain> brain) : brain(brain) {}
    
    // Working memory operations
    std::vector<float> getWorkingMemoryContent() const {
        if (auto* wm = brain->getWorkingMemory()) {
            return wm->retrieve();
        }
        throw MemoryAccessError("Working memory not available");
    }
    
    size_t getWorkingMemoryCapacity() const {
        if (auto* wm = brain->getWorkingMemory()) {
            return wm->getCapacity();
        }
        throw MemoryAccessError("Working memory not available");
    }
    
    void setWorkingMemoryCapacity(size_t capacity) {
        if (auto* wm = brain->getWorkingMemory()) {
            wm->setCapacity(capacity);
        } else {
            throw MemoryAccessError("Working memory not available");
        }
    }
    
    float getWorkingMemoryDecayRate() const {
        if (auto* wm = brain->getWorkingMemory()) {
            return wm->getDecayRate();
        }
        throw MemoryAccessError("Working memory not available");
    }
    
    void setWorkingMemoryDecayRate(float rate) {
        if (auto* wm = brain->getWorkingMemory()) {
            wm->setDecayRate(rate);
        } else {
            throw MemoryAccessError("Working memory not available");
        }
    }
    
    // Episodic memory operations
    size_t getEpisodicMemoryCount() const {
        if (auto* em = brain->getEpisodicMemory()) {
            return em->getEpisodeCount();
        }
        throw MemoryAccessError("Episodic memory not available");
    }
    
    float getEpisodicMemoryAverageReward() const {
        if (auto* em = brain->getEpisodicMemory()) {
            return em->getAverageReward();
        }
        throw MemoryAccessError("Episodic memory not available");
    }
    
    // Associative memory operations
    size_t getAssociativeMemoryCount() const {
        if (auto* am = brain->getAssociativeMemory()) {
            return am->getAssociationCount();
        }
        throw MemoryAccessError("Associative memory not available");
    }
    
    // Attention operations
    const std::vector<NeuronId>& getAttentionWinners() const {
        if (auto* attention = brain->getAttention()) {
            return attention->getWinners();
        }
        throw MemoryAccessError("Attention system not available");
    }
    
    // Clear memory systems
    void clearWorkingMemory() {
        if (auto* wm = brain->getWorkingMemory()) {
            wm->clear();
        }
    }
    
    void clearEpisodicMemory() {
        if (auto* em = brain->getEpisodicMemory()) {
            em->clear();
        }
    }
    
    void clearAssociativeMemory() {
        if (auto* am = brain->getAssociativeMemory()) {
            am->clear();
        }
    }
    
    // Get all memory system statistics
    std::map<std::string, std::string> getMemoryStatistics() const {
        std::map<std::string, std::string> stats;
        
        if (auto* wm = brain->getWorkingMemory()) {
            stats["working_memory_capacity"] = std::to_string(wm->getCapacity());
            stats["working_memory_active_traces"] = std::to_string(wm->getActiveTraces());
            stats["working_memory_activity"] = std::to_string(wm->getMemoryActivity());
        }
        
        if (auto* em = brain->getEpisodicMemory()) {
            stats["episodic_memory_count"] = std::to_string(em->getEpisodeCount());
            stats["episodic_memory_average_reward"] = std::to_string(em->getAverageReward());
        }
        
        if (auto* am = brain->getAssociativeMemory()) {
            stats["associative_memory_count"] = std::to_string(am->getAssociationCount());
        }
        
        if (auto* attention = brain->getAttention()) {
            stats["attention_winner_count"] = std::to_string(attention->getWinners().size());
            stats["attention_inhibition_strength"] = std::to_string(attention->getInhibitionFor(NeuronId(0)));
        }
        
        return stats;
    }
    
private:
    std::shared_ptr<Brain> brain;
    friend class EnhancedBrain;
};

// Enhanced Brain class with better Python integration
class EnhancedBrain {
public:
    EnhancedBrain(const EnhancedConfig& config) : brain(std::make_shared<Brain>(config.getRawConfig())) {}
    
    // Enhanced initialization with error handling
    bool initializeWithValidation() {
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain");
        }
        return true;
    }
    
    // Simulation step with validation
    void step(SimulationStep step) {
        brain->step(step);
    }
    
    void step(SimulationStep step, Timestamp time) {
        brain->step(step, time);
    }
    
    // Access memory system
    MemorySystemAccessor getMemorySystem() {
        return MemorySystemAccessor(brain);
    }
    
    // Enhanced sensory and motor operations
    void receiveSensoryInput(const SensoryInput& input) {
        brain->receiveSensoryInput(input);
    }
    
    void injectCurrent(NeuronId neuron, MembranePotential current) {
        brain->injectCurrent(neuron, current);
    }
    
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) {
        brain->injectCurrentToNeurons(type, current);
    }
    
    SpikeSystem* getSpikeSystem() {
        return brain->getSpikeSystem();
    }
    
    const SpikeSystem* getSpikeSystem() const {
        return brain->getSpikeSystem();
    }
    
    STDP* getSTDP() {
        return brain->getSTDP();
    }
    
    Hebbian* getHebbian() {
        return brain->getHebbian();
    }
    
    StructuralPlasticity* getStructuralPlasticity() {
        return brain->getStructuralPlasticity();
    }
    
    // Enhanced statistics and metrics
    std::map<std::string, std::string> getBrainStatistics() const {
        std::map<std::string, std::string> stats;
        
        stats["total_neurons"] = std::to_string(brain->getTotalNeuronCount());
        stats["total_synapses"] = std::to_string(brain->getTotalSynapseCount());
        stats["active_neurons"] = std::to_string(brain->getActiveNeuronCount());
        stats["firing_neurons"] = std::to_string(brain->getFiringNeuronCount());
        stats["average_firing_rate"] = std::to_string(brain->getAverageFiringRate());
        stats["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
        stats["pending_spikes"] = std::to_string(brain->getPendingSpikeEventCount());
        stats["ei_ratio"] = std::to_string(brain->getExcitationInhibitionRatio());
        
        return stats;
    }
    
    // Simulation control
    void reset() {
        brain->reset();
        brain->initialize();
    }
    
    std::string getStatus() const {
        std::ostringstream oss;
        oss << "EnhancedBrain Status:" << std::endl;
        oss << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        oss << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        oss << "  Active: " << brain->getActiveNeuronCount() << std::endl;
        oss << "  Firing: " << brain->getFiringNeuronCount() << std::endl;
        oss << "  Spikes: " << brain->getTotalSpikeCount() << std::endl;
        oss << "  E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        return oss.str();
    }
    
    // Save and load
    bool save(const std::string& filepath) const {
        return brain->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain->load(filepath);
    }
    
    // Action production
    Action produceAction() {
        return *brain->produceAction();
    }
    
    // Apply neuromodulation
    void applyNeuromodulation(const Neuromodulator& signal) {
        brain->applyNeuromodulation(signal);
    }
    
    // Development
    void develop() {
        brain->develop();
    }
    
    void updatePlasticity() {
        brain->updatePlasticity();
    }
    
    // Get configuration
    std::shared_ptr<Config> getConfig() const {
        return brain->getConfig();
    }
    
    // Get developmental stage
    DevelopmentalStage getDevelopmentalStage() const {
        return brain->getDevelopmentalStage();
    }
    
    void setDevelopmentalStage(DevelopmentalStage stage) {
        brain->setDevelopmentalStage(stage);
    }
    
    std::shared_ptr<Brain> getBrain() const {
        return brain;
    }
    
private:
    std::shared_ptr<Brain> brain;
};

// Enhanced AgentBrain class with better Python integration
class EnhancedAgentBrain {
public:
    EnhancedAgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
        : agentBrain(std::make_unique<AgentBrain>(brain)), world(world) {}
    
    // Initialize with world
    void initialize() {
        agentBrain->initialize(*world);
    }
    
    // Enable features
    void enableRewardModulation(bool enable) {
        agentBrain->enableRewardModulation(enable);
    }
    
    void enableStructuralPlasticity(bool enable) {
        agentBrain->enableStructuralPlasticity(enable);
    }
    
    void enableDevelopment(bool enable) {
        agentBrain->enableDevelopment(enable);
    }
    
    void enableCuriosity(bool enable) {
        agentBrain->enableCuriosity(enable);
    }
    
    // Process sensory input
    void processSensoryInput(const SensoryPercept& percept) {
        agentBrain->processSensoryInput(percept);
    }
    
    // Get motor command
    Action decodeMotorCommand() {
        return *agentBrain->decodeMotorCommand();
    }
    
    // Apply reward
    void applyReward(float reward) {
        agentBrain->applyReward(reward);
    }
    
    // Check if active
    bool isActive() const {
        return agentBrain->isActive();
    }
    
    // Get current state
    std::string getState() const {
        return agentBrain->getState();
    }
    
    // Get action history
    std::vector<Action> getActionHistory() const {
        return agentBrain->getActionHistory();
    }
    
    // Get reward history
    std::vector<float> getRewardHistory() const {
        return agentBrain->getRewardHistory();
    }
    
    // Get experience count
    size_t getExperienceCount() const {
        return agentBrain->getExperienceCount();
    }
    
    // Reset
    void reset() {
        agentBrain->reset();
    }
    
private:
    std::unique_ptr<AgentBrain> agentBrain;
    std::shared_ptr<SimpleWorld> world;
};

// Helper functions for common operations
class NLMHelper {
public:
    // Create default configuration schema for neural simulation
    static ConfigSchema createNeuralSimulationSchema() {
        ConfigSchema schema;
        
        // Basic parameters
        schema.addField("random_seed", ConfigConstraints("int", true, "Random seed for reproducibility", "42"));
        schema.addField("simulation_timestep", ConfigConstraints("double", true, "Simulation time step in seconds", "0.001"));
        schema.addField("neuron_count", ConfigConstraints("int64_t", true, "Number of neurons", "500"));
        schema.addField("region_count", ConfigConstraints("int64_t", true, "Number of brain regions", "1"));
        schema.addField("connection_probability", ConfigConstraints("double", true, "Probability of connections", "0.15"));
        
        // STDP parameters
        schema.addField("stdp_ltp_weight", ConfigConstraints("double", true, "STDP LTP weight", "0.02"));
        schema.addField("stdp_ltd_weight", ConfigConstraints("double", true, "STDP LTD weight", "0.015"));
        schema.addField("stdp_tau", ConfigConstraints("double", true, "STDP time constant", "20.0"));
        
        // Structural plasticity
        schema.addField("synaptogenesis_rate", ConfigConstraints("double", true, "Synaptogenesis rate", "0.0001"));
        schema.addField("pruning_rate", ConfigConstraints("double", true, "Pruning rate", "0.00001"));
        
        return schema;
    }
    
    // Create debug schema
    static ConfigSchema createDebugSchema() {
        ConfigSchema schema = createNeuralSimulationSchema();
        
        // Add debug-specific parameters
        schema.addField("enable_logging", ConfigConstraints("bool", false, "Enable detailed logging", "false"));
        schema.addField("log_level", ConfigConstraints("int", false, "Log level (0-3)", "2"));
        schema.addField("enable_checkpoints", ConfigConstraints("bool", false, "Enable checkpoint saving", "false"));
        schema.addField("checkpoint_interval", ConfigConstraints("int64_t", false, "Checkpoint interval in steps", "1000"));
        
        return schema;
    }
    
    // Create performance schema
    static ConfigSchema createPerformanceSchema() {
        ConfigSchema schema = createNeuralSimulationSchema();
        
        // Add performance-specific parameters
        schema.addField("enable_multithreading", ConfigConstraints("bool", false, "Enable multithreading", "false"));
        schema.addField("thread_count", ConfigConstraints("int", false, "Number of threads", "4"));
        schema.addField("enable_vectorization", ConfigConstraints("bool", false, "Enable SIMD vectorization", "false"));
        schema.addField("memory_pool_size", ConfigConstraints("int64_t", false, "Memory pool size in bytes", "1048576"));
        
        return schema;
    }
    
    // Create world configuration
    static std::pair<int, int> createWorldConfig(const std::string& terrainType = "flat", int visionWidth = 16, int visionHeight = 16) {
        // Return world dimensions based on terrain type
        if (terrainType == "flat") return {20, 20};
        else if (terrainType == "maze") return {30, 30};
        else if (terrainType == "open") return {50, 50};
        else return {20, 20};  // Default to flat
    }
    
    // Create default agent configuration
    static std::map<std::string, std::string> createDefaultAgentConfig() {
        std::map<std::string, std::string> config;
        config["name"] = "DefaultAgent";
        config["max_energy"] = "100.0";
        config["energy_decay_rate"] = "0.01";
        config["health"] = "100.0";
        config["is_movable"] = "true";
        config["can_interact"] = "true";
        config["has_sensory"] = "true";
        config["has_memory"] = "true";
        config["exploration_rate"] = "0.3";
        config["exploitation_rate"] = "0.7";
        return config;
    }
    
    // Validate configuration values
    static bool validateConfig(const Config& config, const ConfigSchema& schema) {
        auto results = config.validateAll(schema);
        return std::all_of(results.begin(), results.end(),
                          [](const ConfigValidationResult& r) { return r.valid; });
    }
    
    // Get configuration errors
    static std::vector<std::string> getConfigErrors(const Config& config, const ConfigSchema& schema) {
        auto results = config.validateAll(schema);
        std::vector<std::string> errors;
        for (const auto& r : results) {
            if (!r.valid) errors.push_back(r.errorMessage);
        }
        return errors;
    }
    
    // Create a simple test environment
    static std::tuple<std::shared_ptr<EnhancedConfig>, std::shared_ptr<EnhancedBrain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<EnhancedAgentBrain>>
    createTestEnvironment(const std::string& configFile = "", bool enableValidation = true) {
        // Create configuration
        auto enhancedConfig = std::make_shared<EnhancedConfig>();
        
        if (!configFile.empty()) {
            enhancedConfig->loadFromFile(configFile);
        }
        
        // Create schema and validate if requested
        auto schema = EnhancedConfig::createNeuralSimulationSchema();
        if (enableValidation) {
            if (!enhancedConfig->isValid(schema)) {
                auto errors = enhancedConfig->getValidationResults(schema);
                std::string errorMsg = "Configuration validation failed:";
                for (const auto& r : errors) {
                    if (!r.valid) errorMsg += " " + r.errorMessage;
                }
                throw ConfigValidationError(errorMsg);
            }
        }
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 16, 16);
        
        // Create brain
        auto enhancedBrain = std::make_shared<EnhancedBrain>(enhancedConfig);
        if (!enhancedBrain->initializeWithValidation()) {
            throw BrainInitializationError("Failed to initialize brain in test environment");
        }
        
        // Create agent brain
        auto agentBrain = std::make_shared<EnhancedAgentBrain>(enhancedBrain->getBrain(), world);
        agentBrain->initialize();
        agentBrain->enableRewardModulation(true);
        agentBrain->enableStructuralPlasticity(true);
        agentBrain->enableDevelopment(true);
        agentBrain->enableCuriosity(true);
        
        return std::make_tuple(enhancedConfig, enhancedBrain, world, agentBrain);
    }
    
    // Run a simple learning experiment
    static std::map<std::string, std::string> runLearningExperiment(const std::shared_ptr<EnhancedBrain>& brain, 
                                                                       const std::shared_ptr<SimpleWorld>& world,
                                                                       const std::shared_ptr<EnhancedAgentBrain>& agentBrain,
                                                                       int steps = 1000) {
        std::map<std::string, std::string> results;
        
        // Reset and initialize
        brain->reset();
        
        // Record initial state
        auto initialStats = brain->getBrainStatistics();
        results["initial_neurons"] = std::to_string(std::stoi(initialStats["total_neurons"]));
        results["initial_synapses"] = std::to_string(std::stoi(initialStats["total_synapses"]));
        
        // Enable plasticity
        if (auto* stdp = brain->getSTDP()) {
            stdp->enable(true);
        }
        if (auto* hebbian = brain->getHebbian()) {
            hebbian->enable(true);
        }
        
        // Run experiment
        for (int step = 0; step < steps; ++step) {
            // Generate action
            Action action = agentBrain->decodeMotorCommand();
            
            // Apply action in world
            world->applyMotorCommand(action, step * 0.001f);
            
            // Get percept
            SensoryPercept percept = world->getSensoryPercept();
            
            // Process percept
            agentBrain->processSensoryInput(percept);
            
            // Advance simulation
            brain->step(step, step * 0.001f);
            
            // Apply curiosity bonus
            float curiosity = 0.1f * std::sin(step * 0.01f);  // Oscillating curiosity
            agentBrain->applyReward(curiosity);
            
            // Log progress
            if (step % 100 == 0) {
                auto currentStats = brain->getBrainStatistics();
                results["step_" + std::to_string(step) + "_neurons"] = currentStats["total_neurons"];
                results["step_" + std::to_string(step) + "_spikes"] = currentStats["total_spikes"];
                results["step_" + std::to_string(step) + "_firing"] = currentStats["firing_neurons"];
            }
        }
        
        // Record final state
        auto finalStats = brain->getBrainStatistics();
        results["final_neurons"] = finalStats["total_neurons"];
        results["final_synapses"] = finalStats["total_synapses"];
        results["final_spikes"] = finalStats["total_spikes"];
        results["final_firing"] = finalStats["firing_neurons"];
        results["final_ei_ratio"] = finalStats["ei_ratio"];
        
        // Calculate learning metrics
        int64_t initialSynapses = std::stoll(results["initial_synapses"]);
        int64_t finalSynapses = std::stoll(results["final_synapses"]);
        int totalSpikes = std::stoi(results["final_spikes"]);
        
        double synapseChange = (double)(finalSynapses - initialSynapses) / initialSynapses * 100.0;
        double spikeRate = (double)totalSpikes / steps;
        
        results["synapse_change_percent"] = std::to_string(synapseChange);
        results["spike_rate"] = std::to_string(spikeRate);
        
        if (std::abs(synapseChange) > 1.0) {
            results["learning_result"] = "significant_learning";
        } else if (std::abs(synapseChange) > 0.1) {
            results["learning_result"] = "moderate_learning";
        } else {
            results["learning_result"] = "minimal_learning";
        }
        
        return results;
    }
    
private:
    static std::shared_ptr<Brain> getBrain(const std::shared_ptr<EnhancedBrain>& enhancedBrain) {
        return enhancedBrain->getBrain();
    }
};

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides:
        - Enhanced configuration with validation and error handling
        - Advanced neural memory systems with Pythonic interfaces
        - Enhanced AgentBrain with learning and behavior capabilities
        - Helper functions for common operations and experiments
        - Custom Python exceptions for better error handling
        - Integration with configuration schemas for validation
        
        Key features:
        - Type-safe configuration access with validation
        - Enhanced memory system access (working, episodic, associative)
        - Learning experiment helpers
        - Test environment creation utilities
        - Performance monitoring and metrics
        - Custom exception hierarchy for better error handling
    )pbdoc";
    
    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMException", PyExc_RuntimeError);
    py::register_exception<ConfigValidationError>(m, "ConfigValidationError", PyExc_RuntimeError);
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError", PyExc_RuntimeError);
    py::register_exception<MemoryAccessError>(m, "MemoryAccessError", PyExc_RuntimeError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);
    
    // Register basic types (same as original)
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
    
    // Register other basic types (simplified for brevity)
    
    // Register enhanced configuration class
    py::class_<EnhancedConfig>(m, "EnhancedConfig", R"pbdoc(Enhanced configuration class with validation)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &EnhancedConfig::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromFileWithValidation", &EnhancedConfig::loadFromFileWithValidation,
             py::arg("filepath"), py::arg("schema"),
             "Load and validate configuration from a JSON file with schema")
        .def("get", [](EnhancedConfig& self, const std::string& key) {
            return self.getWithTypeCheck<int>(key);
        }, py::arg("key"), "Get configuration value as int")
        .def("get", [](EnhancedConfig& self, const std::string& key, int defaultValue) {
            return self.getWithTypeCheck<int>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get configuration value with default")
        .def("has", &EnhancedConfig::has, py::arg("key"), "Check if key exists")
        .def("isValid", &EnhancedConfig::isValid, py::arg("schema"), "Check if configuration is valid against schema")
        .def("getValidationResults", &EnhancedConfig::getValidationResults, py::arg("schema"), "Get validation results")
        .def("getSummary", &EnhancedConfig::getSummaryWithSchema, py::arg("schema"), "Get summary with schema info")
        .def("__repr__", &EnhancedConfig::toString);
    
    // Register enhanced brain class
    py::class_<EnhancedBrain>(m, "EnhancedBrain", R"pbdoc(Enhanced neural simulation brain class)pbdoc")
        .def(py::init<const EnhancedConfig&>(), py::arg("config"))
        .def("initialize", &EnhancedBrain::initializeWithValidation, "Initialize brain with validation")
        .def("step", static_cast<void (EnhancedBrain::*)(SimulationStep)>(&EnhancedBrain::step), py::arg("step"), "Simulation step")
        .def("stepWithTime", &EnhancedBrain::step, py::arg("step"), py::arg("time"), "Simulation step with time")
        .def("getMemorySystem", &EnhancedBrain::getMemorySystem, "Get memory system accessor")
        .def("getBrainStatistics", &EnhancedBrain::getBrainStatistics, "Get brain statistics as map")
        .def("reset", &EnhancedBrain::reset, "Reset and reinitialize brain")
        .def("getStatus", &EnhancedBrain::getStatus, "Get brain status as string")
        .def("save", &EnhancedBrain::save, py::arg("filepath"), "Save brain state")
        .def("load", &EnhancedBrain::load, py::arg("filepath"), "Load brain state")
        .def("produceAction", &EnhancedBrain::produceAction, "Produce action from brain")
        .def("getConfig", &EnhancedBrain::getConfig, "Get configuration");
    
    // Register memory system accessor
    py::class_<EnhancedBrain::MemorySystemAccessor>(m, "MemorySystemAccessor", R"pbdoc(Accessor for neural memory systems)pbdoc")
        .def("getWorkingMemory", &EnhancedBrain::MemorySystemAccessor::getWorkingMemoryContent, "Get working memory content")
        .def("getWorkingMemoryCapacity", &EnhancedBrain::MemorySystemAccessor::getWorkingMemoryCapacity, "Get working memory capacity")
        .def("setWorkingMemoryCapacity", &EnhancedBrain::MemorySystemAccessor::setWorkingMemoryCapacity, py::arg("capacity"), "Set working memory capacity")
        .def("getEpisodicMemoryCount", &EnhancedBrain::MemorySystemAccessor::getEpisodicMemoryCount, "Get episodic memory count")
        .def("getAssociativeMemoryCount", &EnhancedBrain::MemorySystemAccessor::getAssociativeMemoryCount, "Get associative memory count")
        .def("getAttentionWinners", &EnhancedBrain::MemorySystemAccessor::getAttentionWinners, "Get attention winners")
        .def("clearAllMemory", [](EnhancedBrain::MemorySystemAccessor& self) {
            self.clearWorkingMemory();
            self.clearEpisodicMemory();
            self.clearAssociativeMemory();
        }, "Clear all memory systems");
    
    // Register agent brain class
    py::class_<EnhancedAgentBrain>(m, "EnhancedAgentBrain", R"pbdoc(Enhanced agent brain interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>(), 
             py::arg("brain"), py::arg("world"))
        .def("initialize", &EnhancedAgentBrain::initialize, "Initialize agent brain")
        .def("enableFeatures", [](EnhancedAgentBrain& self, bool reward, bool structural, bool development, bool curiosity) {
            self.enableRewardModulation(reward);
            self.enableStructuralPlasticity(structural);
            self.enableDevelopment(development);
            self.enableCuriosity(curiosity);
        }, py::arg("reward_modulation"), py::arg("structural_plasticity"), 
           py::arg("development"), py::arg("curiosity"), "Enable agent features")
        .def("processSensoryInput", &EnhancedAgentBrain::processSensoryInput, py::arg("percept"), "Process sensory input")
        .def("decodeMotorCommand", &EnhancedAgentBrain::decodeMotorCommand, "Decode motor command")
        .def("applyReward", &EnhancedAgentBrain::applyReward, py::arg("reward"), "Apply reward signal")
        .def("isActive", &EnhancedAgentBrain::isActive, "Check if agent is active")
        .def("getActionHistory", &EnhancedAgentBrain::getActionHistory, "Get action history")
        .def("getRewardHistory", &EnhancedAgentBrain::getRewardHistory, "Get reward history")
        .def("getExperienceCount", &EnhancedAgentBrain::getExperienceCount, "Get experience count")
        .def("reset", &EnhancedAgentBrain::reset, "Reset agent brain");
    
    // Register helper functions
    m.def("createNeuralSimulationSchema", &NLMHelper::createNeuralSimulationSchema,
          "Create a configuration schema for neural simulation");
    
    m.def("createDebugSchema", &NLMHelper::createDebugSchema,
          "Create a configuration schema for debugging");
    
    m.def("createPerformanceSchema", &NLMHelper::createPerformanceSchema,
          "Create a configuration schema for performance");
    
    m.def("createTestEnvironment", [](const std::string& configFile, bool enableValidation) {
        return NLMHelper::createTestEnvironment(configFile, enableValidation);
    }, py::arg("config_file") = "", py::arg("enable_validation") = true,
       "Create a complete test environment with validation");
    
    m.def("runLearningExperiment", [](std::shared_ptr<EnhancedBrain> brain, 
                                      std::shared_ptr<SimpleWorld> world,
                                      std::shared_ptr<EnhancedAgentBrain> agentBrain,
                                      int steps) {
        return NLMHelper::runLearningExperiment(brain, world, agentBrain, steps);
    }, py::arg("brain"), py::arg("world"), py::arg("agent_brain"), py::arg("steps") = 1000,
       "Run a learning experiment and return results");
    
    m.def("validateConfig", &NLMHelper::validateConfig,
          py::arg("config"), py::arg("schema"), "Validate configuration against schema");
    
    m.def("getConfigErrors", &NLMHelper::getConfigErrors,
          py::arg("config"), py::arg("schema"), "Get configuration validation errors");
    
    // Expose basic types and enums (same as original)
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    // Expose basic world object type
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
    
    // Expose action types
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
    
    // Expose basic action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);
    
    // Expose simple world class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"))
        .def("reset", &SimpleWorld::reset)
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("time"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept, "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody, "Get agent body")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"));
    
    // Expose basic sensory input
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get sensory type")
        .def("getData", &SensoryInput::getData, "Get sensory data");
    
    // Expose basic vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def("setData", &Vision::setData, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight);
    
    // Expose action type helper
    m.def("getActionName", [](ActionType type) {
        return Action::getActionName(type);
    }, py::arg("type"), "Get action name from type");
    
    m.def("createAction", [](ActionType type, const std::vector<float>& params) {
        return std::make_shared<Action>(type, params);
    }, py::arg("type"), py::arg("params") = std::vector<float>(),
       "Create action with type and parameters");
    
    // Expose basic world helper
    m.def("createWorld", [](int width, int height, int visionWidth, int visionHeight) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20,
       py::arg("vision_width") = 16, py::arg("vision_height") = 16,
       "Create a simple world with specified dimensions");
    
    // Expose simulation constants
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();
    
    // Expose development stages
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    // Expose neuron types
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    // Expose world object type
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
}

// Entry point for the enhanced Python module
PYBIND11_MODULE(pynlm_enhanced, m) {
    nlm::PYBIND11_MODULE(pynlm_enhanced, m);
}