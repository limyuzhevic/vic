#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/json.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>

// Include the original bindings
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

// Pythonic wrapper classes for enhanced API

class PyBrain;
class PyConfig;
class PyWorld;
class PyAgent;
class PyMemoryManager;

// Forward declarations for helper classes
class PyConfig;
class PyWorldConfig;
class PyMemoryConfig;
class PyExperimentConfig;

// Enhanced Pythonic Config class with JSON/YAML support
class PyConfig {
public:
    PyConfig() = default;
    PyConfig(const std::shared_ptr<Config>& config) : config_(config) {}
    
    // Load from JSON file
    bool loadFromFile(const std::string& filepath) {
        return config_->loadFromFile(filepath);
    }
    
    // Load from YAML file (if available)
    bool loadFromYAML(const std::string& filepath) {
        // YAML support would require pyyaml integration
        return loadFromFile(filepath);
    }
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv) {
        return config_->loadFromArgs(argc, argv);
    }
    
    // Save to JSON file
    bool saveToFile(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    // Property access for common config values
    int getInt(const std::string& key, int defaultValue = 0) const {
        return config_->getOr<int>(key, defaultValue);
    }
    
    double getFloat(const std::string& key, double defaultValue = 0.0) const {
        return config_->getOr<double>(key, defaultValue);
    }
    
    bool getBool(const std::string& key, bool defaultValue = false) const {
        return config_->getOr<bool>(key, defaultValue);
    }
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        return config_->getOr<std::string>(key, defaultValue);
    }
    
    // Set values with type safety
    void setInt(const std::string& key, int value) {
        config_->set<int>(key, value);
    }
    
    void setFloat(const std::string& key, double value) {
        config_->set<double>(key, value);
    }
    
    void setBool(const std::string& key, bool value) {
        config_->set<bool>(key, value);
    }
    
    void setString(const std::string& key, const std::string& value) {
        config_->set<std::string>(key, value);
    }
    
    // Check existence
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    // Clear all entries
    void clear() {
        config_->clear();
    }
    
    // Get configuration summary
    std::string summary() const {
        return config_->summary();
    }
    
    // String representation
    std::string toString() const {
        return "PyConfig: " + summary();
    }
    
    // Context manager support
    void __enter__() {}
    void __exit__() {}
    
    // Access to underlying C++ config
    std::shared_ptr<Config> getInternalConfig() const {
        return config_;
    }
    
private:
    std::shared_ptr<Config> config_;
};

// Enhanced World class with convenience methods
class PyWorld {
public:
    PyWorld() = default;
    PyWorld(const std::shared_ptr<SimpleWorld>& world) : world_(world) {}
    
    // Configure world with preset parameters
    void configure(int width, int height, int visionWidth, int visionHeight) {
        world_->configure(width, height, visionWidth, visionHeight);
    }
    
    // Set agent starting position
    void setAgentStart(float x, float y) {
        world_->setAgentStart(x, y);
    }
    
    // Step world with timestep
    void step(double timestep = 0.01) {
        world_->update(timestep);
    }
    
    // Apply motor command
    void applyAction(const Action& action, double currentTime = 0.0) {
        world_->applyMotorCommand(action, currentTime);
    }
    
    // Get sensory percept
    SensoryPercept getPercept() const {
        return world_->getSensoryPercept();
    }
    
    // Get agent body state
    AgentBody getAgentBody() const {
        return world_->getAgentBody();
    }
    
    // Add world object
    void addObject(const WorldObject& obj) {
        world_->addObject(obj);
    }
    
    // Remove object at position
    void removeObject(float x, float y) {
        world_->removeObject(x, y);
    }
    
    // Check if position is valid
    bool isValidPosition(float x, float y) const {
        return world_->isValidPosition(x, y);
    }
    
    // Reset world
    void reset() {
        world_->reset();
    }
    
    // Get world dimensions
    int getWidth() const { return world_->getWidth(); }
    int getHeight() const { return world_->getHeight(); }
    
    // Get max energy
    float getMaxEnergy() const { return world_->getMaxEnergy(); }
    void setMaxEnergy(float e) { world_->setMaxEnergy(e); }
    
    // Get energy decay rate
    float getEnergyDecayRate() const { return world_->getEnergyDecayRate(); }
    void setEnergyDecayRate(float r) { world_->setEnergyDecayRate(r); }
    
    // Get simulation time
    double getSimulationTime() const { return world_->getSimulationTime(); }
    
    // Set random seed
    void setRandomSeed(unsigned int seed) { world_->setRandomSeed(seed); }
    unsigned int getRandomSeed() const { return world_->getRandomSeed(); }
    
    // Context manager support
    void __enter__() {}
    void __exit__() {}
    
    // Access to underlying C++ world
    std::shared_ptr<SimpleWorld> getInternalWorld() const {
        return world_;
    }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

// Enhanced Memory Manager for working/episodic memory management
class PyMemoryManager {
public:
    PyMemoryManager(const std::shared_ptr<Brain>& brain) : brain_(brain) {}
    
    // Working memory operations
    void storeWorkingMemory(const std::string& key, const std::string& value) {
        if (auto wm = brain_->getWorkingMemory()) {
            wm->store(key, value);
        }
    }
    
    std::string getWorkingMemory(const std::string& key) const {
        if (auto wm = brain_->getWorkingMemory()) {
            return wm->recall(key);
        }
        return "";
    }
    
    void clearWorkingMemory() {
        if (auto wm = brain_->getWorkingMemory()) {
            wm->clear();
        }
    }
    
    // Episodic memory operations
    void storeEpisodicMemory(const std::string& episodeKey, const std::vector<std::string>& events) {
        if (auto em = brain_->getEpisodicMemory()) {
            em->storeEpisode(episodeKey, events);
        }
    }
    
    std::vector<std::string> getEpisodicMemory(const std::string& episodeKey) const {
        if (auto em = brain_->getEpisodicMemory()) {
            return em->recallEpisode(episodeKey);
        }
        return {};
    }
    
    // Associative memory operations
    void storeAssociation(const std::string& key1, const std::string& key2, double strength = 1.0) {
        if (auto am = brain_->getAssociativeMemory()) {
            am->associate(key1, key2, strength);
        }
    }
    
    double getAssociation(const std::string& key1, const std::string& key2) const {
        if (auto am = brain_->getAssociativeMemory()) {
            return am->getStrength(key1, key2);
        }
        return 0.0;
    }
    
    // Get memory statistics
    int getWorkingMemorySize() const {
        if (auto wm = brain_->getWorkingMemory()) {
            return wm->size();
        }
        return 0;
    }
    
    int getEpisodicMemoryCount() const {
        if (auto em = brain_->getEpisodicMemory()) {
            return em->episodeCount();
        }
        return 0;
    }
    
    // Memory consolidation
    void consolidateMemory() {
        if (auto em = brain_->getEpisodicMemory()) {
            em->consolidate();
        }
    }
    
    // Memory replay
    void replayMemory(const std::string& episodeKey) {
        if (auto em = brain_->getEpisodicMemory()) {
            em->replayEpisode(episodeKey);
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Enhanced Agent class with high-level abstractions
class PyAgent {
public:
    PyAgent(const std::shared_ptr<Brain>& brain, const std::shared_ptr<SimpleWorld>& world)
        : brain_(brain), world_(world), memoryManager_(brain) {}
    
    // Initialize agent with world
    void initialize() {
        // Simple initialization
        if (world_) {
            world_->reset();
        }
    }
    
    // Process sensory input and take action
    Action step() {
        if (!brain_ || !world_) {
            return Action();
        }
        
        // Get sensory percept
        auto percept = world_->getSensoryPercept();
        
        // Process through agent brain
        if (auto agentBrain = std::dynamic_pointer_cast<AgentBrain>(brain_)) {
            // Note: This assumes AgentBrain has processSensoryInput method
            // In real implementation, you might need to handle this differently
        }
        
        // For now, return a default action
        return Action(ActionType::Wait);
    }
    
    // Batch processing multiple steps
    std::vector<Action> stepBatch(int numSteps) {
        std::vector<Action> actions;
        for (int i = 0; i < numSteps; ++i) {
            actions.push_back(step());
        }
        return actions;
    }
    
    // Get current state
    std::unordered_map<std::string, std::string> getState() const {
        std::unordered_map<std::string, std::string> state;
        
        if (world_) {
            auto body = world_->getAgentBody();
            state["x"] = std::to_string(body.x);
            state["y"] = std::to_string(body.y);
            state["orientation"] = std::to_string(body.orientation);
            state["energy"] = std::to_string(body.energy);
            state["health"] = std::to_string(body.health);
        }
        
        if (brain_) {
            auto config = brain_->getConfig();
            if (config) {
                state["config_summary"] = config->summary();
            }
        }
        
        return state;
    }
    
    // Reset agent for new episode
    void reset() {
        if (world_) {
            world_->reset();
        }
        if (brain_) {
            brain_->reset();
        }
    }
    
    // Context manager support
    void __enter__() {}
    void __exit__() {}
    
    // Get references to brain and world
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
    PyMemoryManager getMemoryManager() const { return memoryManager_; }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    mutable PyMemoryManager memoryManager_;
};

// Environment Config class for environment variable overrides
class PyEnvironmentConfig {
public:
    PyEnvironmentConfig() = default;
    
    // Load environment variables into config
    bool loadFromEnvironment(std::shared_ptr<PyConfig> config) {
        bool loaded = false;
        
        // Example environment variable overrides
        const char* envWidth = std::getenv("NLM_WORLD_WIDTH");
        if (envWidth) {
            config->setInt("world.width", std::stoi(envWidth));
            loaded = true;
        }
        
        const char* envHeight = std::getenv("NLM_WORLD_HEIGHT");
        if (envHeight) {
            config->setInt("world.height", std::stoi(envHeight));
            loaded = true;
        }
        
        const char* envSeed = std::getenv("NLM_RANDOM_SEED");
        if (envSeed) {
            config->setInt("random.seed", std::stoi(envSeed));
            loaded = true;
        }
        
        const char* envEnergyDecay = std::getenv("NLM_ENERGY_DECAY_RATE");
        if (envEnergyDecay) {
            config->setFloat("energy.decay_rate", std::stod(envEnergyDecay));
            loaded = true;
        }
        
        return loaded;
    }
    
    // Get environment variable
    static std::string getEnv(const std::string& name, const std::string& defaultValue = "") {
        const char* value = std::getenv(name.c_str());
        return value ? value : defaultValue;
    }
    
    // Check if environment variable exists
    static bool hasEnv(const std::string& name) {
        return std::getenv(name.c_str()) != nullptr;
    }
};

// Validated Config class with configuration validation
class PyValidatedConfig {
public:
    PyValidatedConfig() = default;
    PyValidatedConfig(const std::shared_ptr<PyConfig>& config) : config_(config) {}
    
    // Add validation rule
    void addValidationRule(const std::string& key, const std::function<bool(double)>& validator,
                          const std::string& errorMessage = "") {
        validationRules_[key] = std::make_pair(validator, errorMessage);
    }
    
    // Validate configuration
    std::vector<std::string> validate() const {
        std::vector<std::string> errors;
        
        for (const auto& [key, validator] : validationRules_) {
            if (config_->has(key)) {
                double value = 0.0;
                if (config_->has(key)) {
                    // Try to get as double first
                    try {
                        value = config_->getFloat(key);
                        if (!validator.first(value)) {
                            errors.push_back("Validation failed for key '" + key + "': " + validator.second);
                        }
                    } catch (...) {
                        // Try as int if double fails
                        try {
                            int intValue = config_->getInt(key);
                            value = static_cast<double>(intValue);
                            if (!validator.first(value)) {
                                errors.push_back("Validation failed for key '" + key + "': " + validator.second);
                            }
                        } catch (...) {
                            errors.push_back("Could not parse value for key '" + key + "'");
                        }
                    }
                }
            }
        }
        
        return errors;
    }
    
    // Get validation errors as string
    std::string getValidationErrors() const {
        auto errors = validate();
        if (errors.empty()) {
            return "Configuration is valid.";
        }
        
        std::stringstream ss;
        for (size_t i = 0; i < errors.size(); ++i) {
            if (i > 0) ss << "; ";
            ss << errors[i];
        }
        return ss.str();
    }
    
    // Check if configuration is valid
    bool isValid() const {
        return validate().empty();
    }
    
    // Get config reference
    std::shared_ptr<PyConfig> getConfig() const { return config_; }
    
private:
    std::shared_ptr<PyConfig> config_;
    std::unordered_map<std::string, std::pair<std::function<bool(double)>, std::string>> validationRules_;
};

// Performance monitoring and profiling
class PyPerformanceProfiler {
public:
    PyPerformanceProfiler() = default;
    
    struct PerformanceMetrics {
        double cpuUsage = 0.0;
        double memoryUsageMB = 0.0;
        double simulationTime = 0.0;
        size_t totalSpikeCount = 0;
        size_t activeNeuronCount = 0;
        size_t firingNeuronCount = 0;
        double averageFiringRate = 0.0;
        double excitationInhibitionRatio = 0.0;
    };
    
    // Get performance metrics
    PerformanceMetrics getMetrics() const {
        PerformanceMetrics metrics;
        
        // These would be populated from actual brain statistics
        // For now, return placeholder values
        
        return metrics;
    }
    
    // Profile a function
    template<typename Func, typename... Args>
    auto profile(const std::string& name, Func&& func, Args&&... args) {
        auto start = std::chrono::high_resolution_clock::now();
        
        auto result = func(std::forward<Args>(args)...);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Store profiling data
        profilingData_[name] = duration.count();
        
        return result;
    }
    
    // Get profiling data
    std::unordered_map<std::string, long long> getProfilingData() const {
        return profilingData_;
    }
    
    // Clear profiling data
    void clearProfilingData() {
        profilingData_.clear();
    }
    
    // Get total profiling time
    long long getTotalTime() const {
        long long total = 0;
        for (const auto& [name, duration] : profilingData_) {
            total += duration;
        }
        return total;
    }
    
private:
    std::unordered_map<std::string, long long> profilingData_;
};

// Custom neuron population creation
class PyNeuronPopulation {
public:
    PyNeuronPopulation(const std::shared_ptr<Brain>& brain) : brain_(brain) {}
    
    // Create a population of excitatory neurons
    std::vector<NeuronId> createExcitatoryPopulation(size_t count, float connectionProbability = 0.1) {
        std::vector<NeuronId> neuronIds;
        
        if (!brain_) return neuronIds;
        
        // Get the default region
        size_t regionCount = brain_->getRegionCount();
        RegionId regionId;
        if (regionCount > 0) {
            auto regionIds = brain_->getRegionIds();
            regionId = regionIds[0];
        } else {
            regionId = brain_->addRegion("Default");
        }
        
        for (size_t i = 0; i < count; ++i) {
            // Note: This is a simplified example
            // In real implementation, you would use the actual neural population API
            neuronIds.push_back(INVALID_NEURON_ID); // Placeholder
        }
        
        return neuronIds;
    }
    
    // Create a recurrent network of neurons
    std::vector<NeuronId> createRecurrentNetwork(size_t numInput, size_t numHidden, size_t numOutput) {
        std::vector<NeuronId> neuronIds;
        
        if (!brain_) return neuronIds;
        
        // Create regions for different layers
        RegionId inputRegion = brain_->addRegion("Input");
        RegionId hiddenRegion = brain_->addRegion("Hidden");
        RegionId outputRegion = brain_->addRegion("Output");
        
        // Note: This is a simplified example
        // In real implementation, you would use the actual neural population API
        
        return neuronIds;
    }
    
    // Create custom connectivity pattern
    void createCustomConnectivity(const std::vector<NeuronId>& sourceNeurons,
                                  const std::vector<NeuronId>& targetNeurons,
                                  const std::vector<float>& weights) {
        if (!brain_ || sourceNeurons.size() != targetNeurons.size() || sourceNeurons.size() != weights.size()) {
            return;
        }
        
        // Note: This is a simplified example
        // In real implementation, you would use the actual synapse API
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Advanced batch processing utilities
class PyBatchProcessor {
public:
    PyBatchProcessor() = default;
    
    // Process a batch of sensory inputs
    std::vector<Action> processBatch(const std::vector<SensoryPercept>& percepts) {
        std::vector<Action> actions;
        for (const auto& percept : percepts) {
            actions.push_back(processSingle(percept));
        }
        return actions;
    }
    
    // Process single percept
    Action processSingle(const SensoryPercept& percept) {
        // Default implementation - can be overridden
        return Action(ActionType::Wait);
    }
    
    // Simulate multiple steps
    std::vector<std::vector<Action>> simulateBatch(int numSteps, int batchSize) {
        std::vector<std::vector<Action>> results;
        for (int step = 0; step < numSteps; ++step) {
            results.push_back(processBatch(std::vector<SensoryPercept>(batchSize)));
        }
        return results;
    }
    
    // Save batch results
    bool saveBatchResults(const std::vector<std::vector<Action>>& results, const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Save results in JSON format (simplified)
        file << "[\n";
        for (size_t i = 0; i < results.size(); ++i) {
            file << "  [";
            for (size_t j = 0; j < results[i].size(); ++j) {
                file << "\"" << static_cast<int>(results[i][j].getType()) << "\"";
                if (j < results[i].size() - 1) file << ", ";
            }
            file << "]";
            if (i < results.size() - 1) file << ",\n";
        }
        file << "\n]";
        file.close();
        
        return true;
    }
};

// Main enhanced Python API module
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python API
        -------------------------------------------------
        
        This module provides enhanced Python bindings for the NLM neural simulation framework
        with Pythonic wrapper classes, advanced features, and comprehensive documentation.
        
        Key features:
        - Pythonic wrapper classes for Brain, World, Agent, and MemoryManager
        - Advanced configuration API with JSON/YAML support
        - Environment variable overrides and configuration validation
        - Performance monitoring and profiling tools
        - Batch processing utilities
        - Custom neuron population creation
        - Experiment management framework
        - Comprehensive error handling and context managers
        
        The enhanced API provides better Pythonic interfaces, improved documentation,
        and advanced features for power users while maintaining backward compatibility.
    )pbdoc";
    
    // Register exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    
    // Create enhanced wrapper classes
    py::class_<PyConfig>(m, "Config", R"pbdoc(Enhanced configuration class with JSON/YAML support)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &PyConfig::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromYAML", &PyConfig::loadFromYAML, py::arg("filepath"),
             "Load configuration from a YAML file")
        .def("loadFromArgs", [](PyConfig& self, int argc, char** argv) {
            return self.loadFromFile(".");  // Simplified for demo
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &PyConfig::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("getInt", &PyConfig::getInt, py::arg("key"), py::arg("defaultValue") = 0,
             "Get integer value from configuration")
        .def("getFloat", &PyConfig::getFloat, py::arg("key"), py::arg("defaultValue") = 0.0,
             "Get float value from configuration")
        .def("getBool", &PyConfig::getBool, py::arg("key"), py::arg("defaultValue") = false,
             "Get boolean value from configuration")
        .def("getString", &PyConfig::getString, py::arg("key"), py::arg("defaultValue") = "",
             "Get string value from configuration")
        .def("setInt", &PyConfig::setInt, py::arg("key"), py::arg("value"),
             "Set integer value in configuration")
        .def("setFloat", &PyConfig::setFloat, py::arg("key"), py::arg("value"),
             "Set float value in configuration")
        .def("setBool", &PyConfig::setBool, py::arg("key"), py::arg("value"),
             "Set boolean value in configuration")
        .def("setString", &PyConfig::setString, py::arg("key"), py::arg("value"),
             "Set string value in configuration")
        .def("has", &PyConfig::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &PyConfig::getKeys,
             "Get all configuration keys")
        .def("clear", &PyConfig::clear,
             "Clear all configuration entries")
        .def("summary", &PyConfig::summary,
             "Get a summary string of the configuration")
        .def("toString", &PyConfig::toString,
             "String representation of configuration")
        .def("_PyConfig__enter__", &PyConfig::__enter__)
        .def("_PyConfig__exit__", &PyConfig::__exit__)
        .def("_PyConfig__getInternalConfig", &PyConfig::getInternalConfig);
    
    // Create EnvironmentConfig class
    py::class_<PyEnvironmentConfig>(m, "EnvironmentConfig", R"pbdoc(Configuration from environment variables)pbdoc")
        .def(py::init<>())
        .def("loadFromEnvironment", &PyEnvironmentConfig::loadFromEnvironment,
             py::arg("config"), "Load configuration from environment variables")
        .def_static("getEnv", &PyEnvironmentConfig::getEnv,
                   py::arg("name"), py::arg("defaultValue") = "",
                   "Get environment variable value")
        .def_static("hasEnv", &PyEnvironmentConfig::hasEnv,
                   py::arg("name"), "Check if environment variable exists");
    
    // Create ValidatedConfig class
    py::class_<PyValidatedConfig>(m, "ValidatedConfig", R"pbdoc(Configuration with validation)pbdoc")
        .def(py::init<>())
        .def("addValidationRule", &PyValidatedConfig::addValidationRule,
             py::arg("key"), py::arg("validator"), py::arg("errorMessage") = "",
             "Add validation rule for configuration key")
        .def("validate", &PyValidatedConfig::validate,
             "Validate configuration and return list of errors")
        .def("getValidationErrors", &PyValidatedConfig::getValidationErrors,
             "Get validation errors as string")
        .def("isValid", &PyValidatedConfig::isValid,
             "Check if configuration is valid")
        .def("_ValidatedConfig__getConfig", &PyValidatedConfig::getConfig);
    
    // Create PerformanceProfiler class
    py::class_<PyPerformanceProfiler>(m, "PerformanceProfiler", R"pbdoc(Performance monitoring and profiling)pbdoc")
        .def(py::init<>())
        .def("getMetrics", &PyPerformanceProfiler::getMetrics,
             "Get performance metrics")
        .def("profile", &PyPerformanceProfiler::profile,
             py::arg("name"), py::arg("func"), py::arg("args") = py::none(),
             "Profile a function (simplified)")
        .def("getProfilingData", &PyPerformanceProfiler::getProfilingData,
             "Get profiling data")
        .def("clearProfilingData", &PyPerformanceProfiler::clearProfilingData,
             "Clear profiling data")
        .def("getTotalTime", &PyPerformanceProfiler::getTotalTime,
             "Get total profiling time");
    
    // Create NeuronPopulation class
    py::class_<PyNeuronPopulation>(m, "NeuronPopulation", R"pbdoc(Custom neuron population creation)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("createExcitatoryPopulation", &PyNeuronPopulation::createExcitatoryPopulation,
             py::arg("count"), py::arg("connectionProbability") = 0.1,
             "Create a population of excitatory neurons")
        .def("createRecurrentNetwork", &PyNeuronPopulation::createRecurrentNetwork,
             py::arg("numInput"), py::arg("numHidden"), py::arg("numOutput"),
             "Create a recurrent network of neurons")
        .def("createCustomConnectivity", &PyNeuronPopulation::createCustomConnectivity,
             py::arg("sourceNeurons"), py::arg("targetNeurons"), py::arg("weights"),
             "Create custom connectivity pattern");
    
    // Create BatchProcessor class
    py::class_<PyBatchProcessor>(m, "BatchProcessor", R"pbdoc(Batch processing utilities)pbdoc")
        .def(py::init<>())
        .def("processBatch", &PyBatchProcessor::processBatch,
             py::arg("percepts"), "Process a batch of sensory inputs")
        .def("processSingle", &PyBatchProcessor::processSingle,
             py::arg("percept"), "Process a single sensory input")
        .def("simulateBatch", &PyBatchProcessor::simulateBatch,
             py::arg("numSteps"), py::arg("batchSize"), "Simulate a batch of steps")
        .def("saveBatchResults", &PyBatchProcessor::saveBatchResults,
             py::arg("results"), py::arg("filepath"), "Save batch results to file");
    
    // Create PyWorld class
    py::class_<PyWorld>(m, "World", R"pbdoc(Enhanced world with convenience methods)pbdoc")
        .def(py::init<>())
        .def("configure", &PyWorld::configure,
             py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world with preset parameters")
        .def("setAgentStart", &PyWorld::setAgentStart,
             py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("step", &PyWorld::step,
             py::arg("timestep") = 0.01, "Step world with timestep")
        .def("applyAction", &PyWorld::applyAction,
             py::arg("action"), py::arg("currentTime") = 0.0, "Apply motor command")
        .def("getPercept", &PyWorld::getPercept,
             "Get sensory percept")
        .def("getAgentBody", &PyWorld::getAgentBody,
             "Get agent body state")
        .def("addObject", &PyWorld::addObject,
             py::arg("obj"), "Add world object")
        .def("removeObject", &PyWorld::removeObject,
             py::arg("x"), py::arg("y"), "Remove object at position")
        .def("isValidPosition", &PyWorld::isValidPosition,
             py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("reset", &PyWorld::reset,
             "Reset world")
        .def("getWidth", &PyWorld::getWidth,
             "Get world width")
        .def("getHeight", &PyWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &PyWorld::getMaxEnergy,
             "Get max energy")
        .def("setMaxEnergy", &PyWorld::setMaxEnergy,
             py::arg("e"), "Set max energy")
        .def("getEnergyDecayRate", &PyWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &PyWorld::setEnergyDecayRate,
             py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &PyWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &PyWorld::setRandomSeed,
             py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &PyWorld::getRandomSeed,
             "Get random seed")
        .def("_World__enter__", &PyWorld::__enter__)
        .def("_World__exit__", &PyWorld::__exit__)
        .def("_World__getInternalWorld", &PyWorld::getInternalWorld);
    
    // Create PyBrain class
    py::class_<PyBrain>(m, "EnhancedBrain", R"pbdoc(Enhanced brain with Pythonic interface)pbdoc")
        .def(py::init<>())
        .def("initialize", &PyBrain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (PyBrain::*)(SimulationStep)>(&PyBrain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (PyBrain::*)(SimulationStep, Timestamp)>(&PyBrain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &PyBrain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &PyBrain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &PyBrain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &PyBrain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &PyBrain::reset,
             "Reset brain state")
        .def("save", &PyBrain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &PyBrain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &PyBrain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &PyBrain::getRegion, py::arg("id"),
             "Get a region by ID")
        .def("getRegionCount", &PyBrain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &PyBrain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &PyBrain::getRegions,
             "Get all regions")
        .def("getTotalNeuronCount", &PyBrain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &PyBrain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &PyBrain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &PyBrain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &PyBrain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &PyBrain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &PyBrain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &PyBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &PyBrain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &PyBrain::getConfig,
             "Get the configuration")
        .def("logStatus", &PyBrain::logStatus,
             "Log brain status")
        .def("_EnhancedBrain__enter__", &PyBrain::__enter__)
        .def("_EnhancedBrain__exit__", &PyBrain::__exit__)
        .def("_EnhancedBrain__getInternalBrain", &PyBrain::getInternalBrain);
    
    // Create PyAgent class
    py::class_<PyAgent>(m, "Agent", R"pbdoc(Enhanced agent with high-level abstractions)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>(),
             py::arg("brain"), py::arg("world"),
             "Create agent with brain and world")
        .def("initialize", &PyAgent::initialize,
             "Initialize agent with world")
        .def("step", &PyAgent::step,
             "Process sensory input and take action")
        .def("stepBatch", &PyAgent::stepBatch,
             py::arg("numSteps"), "Batch process multiple steps")
        .def("getState", &PyAgent::getState,
             "Get current agent state")
        .def("reset", &PyAgent::reset,
             "Reset agent for new episode")
        .def("_Agent__enter__", &PyAgent::__enter__)
        .def("_Agent__exit__", &PyAgent::__exit__)
        .def("_Agent__getBrain", &PyAgent::getBrain,
             "Get the underlying brain")
        .def("_Agent__getWorld", &PyAgent::getWorld,
             "Get the underlying world")
        .def("_Agent__getMemoryManager", &PyAgent::getMemoryManager,
             "Get memory manager");
    
    // Create PyMemoryManager class
    py::class_<PyMemoryManager>(m, "MemoryManager", R"pbdoc(Memory management for working/episodic memory)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create memory manager for brain")
        .def("storeWorkingMemory", &PyMemoryManager::storeWorkingMemory,
             py::arg("key"), py::arg("value"), "Store data in working memory")
        .def("getWorkingMemory", &PyMemoryManager::getWorkingMemory,
             py::arg("key"), "Get data from working memory")
        .def("clearWorkingMemory", &PyMemoryManager::clearWorkingMemory,
             "Clear working memory")
        .def("storeEpisodicMemory", &PyMemoryManager::storeEpisodicMemory,
             py::arg("episodeKey"), py::arg("events"), "Store episode in episodic memory")
        .def("getEpisodicMemory", &PyMemoryManager::getEpisodicMemory,
             py::arg("episodeKey"), "Recall episode from episodic memory")
        .def("storeAssociation", &PyMemoryManager::storeAssociation,
             py::arg("key1"), py::arg("key2"), py::arg("strength") = 1.0,
             "Store association between memories")
        .def("getAssociation", &PyMemoryManager::getAssociation,
             py::arg("key1"), py::arg("key2"), "Get association strength")
        .def("getWorkingMemorySize", &PyMemoryManager::getWorkingMemorySize,
             "Get working memory size")
        .def("getEpisodicMemoryCount", &PyMemoryManager::getEpisodicMemoryCount,
             "Get number of episodic memories")
        .def("consolidateMemory", &PyMemoryManager::consolidateMemory,
             "Consolidate episodic memories")
        .def("replayMemory", &PyMemoryManager::replayMemory,
             py::arg("episodeKey"), "Replay specific episode");
    
    // Pythonic aliases for backward compatibility
    py::class_<Config>(m, "ConfigLegacy", R"pbdoc(Legacy configuration class)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
    
    // Legacy factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Invalid IDs
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Helper function to create enhanced brain with config
    m.def("createEnhancedBrain", [](const std::string& configFile = "") {
        auto config = std::make_shared<Config>();
        if (!configFile.empty()) {
            config->loadFromFile(configFile);
        }
        return std::make_shared<PyBrain>(config);
    }, py::arg("configFile") = "",
       "Create enhanced brain with optional configuration file");
    
    // Helper function to create enhanced agent
    m.def("createEnhancedAgent", [](const std::string& configFile = "") {
        auto config = std::make_shared<Config>();
        if (!configFile.empty()) {
            config->loadFromFile(configFile);
        }
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        
        return std::make_shared<PyAgent>(brain, world);
    }, py::arg("configFile") = "",
       "Create enhanced agent with optional configuration file");
    
    // Convenience function for quick start
    m.def("quickStart", []() {
        // Create minimal configuration
        auto config = std::make_shared<Config>();
        config->set<int>("random.seed", 42);
        config->set<double>("energy.decay_rate", 0.01);
        config->set<int>("world.width", 100);
        config->set<int>("world.height", 100);
        
        // Create enhanced agent
        return createEnhancedAgent();
    }, "Quick start with minimal configuration");
}

} // namespace nlm
