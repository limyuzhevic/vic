// Enhanced Python bindings for NLM
// Provides improved Pythonic API with better error handling, validation, and advanced features

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Exception types for Python bindings
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError("Validation Error: " + msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

// Helper functions for type conversion and validation
namespace helpers {
    
    // Convert Python object to float with validation
    float toFloat(const py::object& obj, const std::string& paramName = "parameter") {
        if (obj.is(py::float_())) {
            return obj.cast<float>();
        } else if (obj.is(py::int_())) {
            return static_cast<float>(obj.cast<int64_t>());
        } else {
            throw ValidationError("Expected float for " + paramName + ", got " + std::string(PyFloat_Check(obj.ptr()) ? "float" : "other type"));
        }
    }
    
    // Convert Python object to int with validation
    int64_t toInt(const py::object& obj, const std::string& paramName = "parameter") {
        if (obj.is(py::int_())) {
            return obj.cast<int64_t>();
        } else if (obj.is(py::float_())) {
            float val = obj.cast<float>();
            if (val == static_cast<int64_t>(val)) {
                return static_cast<int64_t>(val);
            }
        }
        throw ValidationError("Expected integer for " + paramName + ", got " + std::string(PyLong_Check(obj.ptr()) ? "int" : "other type"));
    }
    
    // Convert Python object to string
    std::string toString(const py::object& obj) {
        return obj.cast<std::string>();
    }
    
    // Convert Python object to bool
    bool toBool(const py::object& obj) {
        return obj.cast<bool>();
    }
    
    // Validate configuration key
    void validateKey(const Config& config, const std::string& key) {
        if (!config.has(key)) {
            throw ConfigurationError("Configuration key not found: '" + key + "'");
        }
    }
    
    // Safe configuration get with type checking
    template<typename T>
    T safeGet(Config& config, const std::string& key, const T& defaultValue) {
        validateKey(config, key);
        return config.getOr<T>(key, defaultValue);
    }
}

// Configuration wrapper with validation
class ConfigWrapper {
private:
    std::shared_ptr<Config> config_;
    
public:
    ConfigWrapper() : config_(std::make_shared<Config>()) {}
    
    explicit ConfigWrapper(const std::string& filename) : config_(std::make_shared<Config>()) {
        loadFromFile(filename);
    }
    
    // Factory function for easy usage
    static std::shared_ptr<ConfigWrapper> createDefaultConfig() {
        return std::make_shared<ConfigWrapper>();
    }
    
    // Load from file with error handling
    void loadFromFile(const std::string& filename) {
        if (!config_->loadFromFile(filename)) {
            throw NLMError("Failed to load configuration from file: '" + filename + "'");
        }
    }
    
    // Save to file
    void saveToFile(const std::string& filename) const {
        config_->saveToFile(filename);
    }
    
    // Set configuration with validation
    void set(const std::string& key, const py::object& value) {
        if (value.is(py::int_())) {
            config_->set(key, helpers::toInt(value, key), ConfigSource::User);
        } else if (value.is(py::float_())) {
            config_->set(key, helpers::toFloat(value, key), ConfigSource::User);
        } else if (value.is(py::bool_())) {
            config_->set(key, helpers::toBool(value), ConfigSource::User);
        } else if (value.is(py::str())) {
            config_->set(key, helpers::toString(value), ConfigSource::User);
        } else if (value.is(py::list())) {
            auto list = value.cast<std::vector<double>>();
            config_->set(key, list, ConfigSource::User);
        } else {
            throw ValidationError("Unsupported type for configuration key '" + key + "'");
        }
    }
    
    // Get configuration with type safety
    py::object get(const std::string& key) const {
        helpers::validateKey(*config_, key);
        return py::cast(config_->get(key));
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    // Clear all configuration
    void clear() {
        config_->clear();
    }
    
    // Get configuration summary
    std::string summary() const {
        return config_->summary();
    }
    
    // Convert to Python dictionary
    py::dict toDict() const {
        py::dict result;
        auto keys = config_->getKeys();
        for (const auto& key : keys) {
            result[key] = config_->get(key);
        }
        return result;
    }
    
    // Merge with another configuration
    void merge(const ConfigWrapper& other) {
        // This would need to be implemented based on Config API
        // For now, it's a placeholder
    }
    
    // Get the underlying Config pointer for advanced usage
    std::shared_ptr<Config> getInternal() const {
        return config_;
    }
    
    // Create a copy
    std::shared_ptr<ConfigWrapper> clone() const {
        auto wrapper = std::make_shared<ConfigWrapper>();
        wrapper->config_ = std::make_shared<Config>(*config_);
        return wrapper;
    }
    
    // Validate required configuration
    void validateRequired(const std::vector<std::string>& requiredKeys) {
        for (const auto& key : requiredKeys) {
            if (!has(key)) {
                throw ValidationError("Required configuration key missing: '" + key + "'");
            }
        }
    }
    
    // Get configuration value with default
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        return config_->getOr<T>(key, defaultValue);
    }
};

// Brain wrapper with enhanced functionality
class BrainWrapper {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    BrainWrapper(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
    
    // Factory function
    static std::shared_ptr<BrainWrapper> createBrain(std::shared_ptr<Config> config) {
        return std::make_shared<BrainWrapper>(config);
    }
    
    // Initialize the brain
    bool initialize() {
        return brain_->initialize();
    }
    
    // Step the brain simulation
    void step(size_t step, double time = 0.0) {
        brain_->step(step, time);
    }
    
    // Inject current into specific neuron
    void injectCurrent(size_t neuronId, float current) {
        brain_->injectCurrent(NeuronId(neuronId), current);
    }
    
    // Inject current into neurons of specific type
    void injectCurrentToNeurons(NeuronType type, float current) {
        brain_->injectCurrentToNeurons(type, current);
    }
    
    // Produce motor action
    std::shared_ptr<Action> produceAction() {
        return brain_->produceAction();
    }
    
    // Reset brain state
    void reset() {
        brain_->reset();
    }
    
    // Save brain state
    bool save(const std::string& filepath) {
        return brain_->save(filepath);
    }
    
    // Load brain state
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    // Add neural region
    RegionId addRegion(const std::string& name = "") {
        return brain_->addRegion(name);
    }
    
    // Get region by ID
    std::shared_ptr<NeuralRegion> getRegion(RegionId id) {
        return brain_->getRegion(id);
    }
    
    // Get region count
    size_t getRegionCount() const {
        return brain_->getRegionCount();
    }
    
    // Get all region IDs
    std::vector<RegionId> getRegionIds() const {
        return brain_->getRegionIds();
    }
    
    // Get all regions
    std::vector<std::shared_ptr<NeuralRegion>> getRegions() const {
        return brain_->getRegions();
    }
    
    // Get total neuron count
    size_t getTotalNeuronCount() const {
        return brain_->getTotalNeuronCount();
    }
    
    // Get total synapse count
    size_t getTotalSynapseCount() const {
        return brain_->getTotalSynapseCount();
    }
    
    // Get active neuron count
    size_t getActiveNeuronCount() const {
        return brain_->getActiveNeuronCount();
    }
    
    // Get firing neuron count
    size_t getFiringNeuronCount() const {
        return brain_->getFiringNeuronCount();
    }
    
    // Get average firing rate
    float getAverageFiringRate() const {
        return brain_->getAverageFiringRate();
    }
    
    // Get excitation/inhibition ratio
    float getExcitationInhibitionRatio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    // Get total spike count
    size_t getTotalSpikeCount() const {
        return brain_->getTotalSpikeCount();
    }
    
    // Get developmental stage
    DevelopmentalStage getDevelopmentalStage() const {
        return brain_->getDevelopmentalStage();
    }
    
    // Set developmental stage
    void setDevelopmentalStage(DevelopmentalStage stage) {
        brain_->setDevelopmentalStage(stage);
    }
    
    // Get configuration
    std::shared_ptr<Config> getConfig() const {
        return brain_->getConfig();
    }
    
    // Get the underlying brain pointer
    std::shared_ptr<Brain> getInternal() const {
        return brain_;
    }
    
    // Get region by index (convenience method)
    std::shared_ptr<NeuralRegion> getRegionAt(size_t index) {
        auto regionIds = getRegionIds();
        if (index < regionIds.size()) {
            return getRegion(regionIds[index]);
        }
        return nullptr;
    }
    
    // Get region name (convenience method)
    std::string getRegionName(RegionId id) const {
        auto* region = brain_->getRegion(id);
        return region ? region->getName() : "";
    }
    
    // Batch process multiple steps
    void batchSteps(size_t numSteps, double timestep = 0.001) {
        for (size_t step = 0; step < numSteps; ++step) {
            brain_->step(step, step * timestep);
        }
    }
    
    // Get brain statistics as dictionary
    py::dict getStatistics() const {
        py::dict stats;
        stats["total_neurons"] = getTotalNeuronCount();
        stats["total_synapses"] = getTotalSynapseCount();
        stats["firing_neurons"] = getFiringNeuronCount();
        stats["total_spikes"] = getTotalSpikeCount();
        stats["avg_firing_rate"] = getAverageFiringRate();
        stats["e_i_ratio"] = getExcitationInhibitionRatio();
        stats["developmental_stage"] = static_cast<int>(getDevelopmentalStage());
        stats["region_count"] = getRegionCount();
        return stats;
    }
};