// NLM Python Bindings - Enhanced Error Handling
// This file extends the Python bindings with comprehensive error handling,
// exception types, and validation utilities.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <nlohmann/json.hpp>

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

// Custom exception types for NLM Python bindings
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message)
        : std::runtime_error(message) {}
    
    explicit NLMError(const std::string& message, const std::string& operation)
        : std::runtime_error("NLM Error: " + message + " (Operation: " + operation + ")") {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message)
        : NLMError(message, "Configuration") {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message)
        : NLMError(message, "Brain") {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& message)
        : NLMError(message, "World") {}
};

class AgentError : public NLMError {
public:
    explicit AgentError(const std::string& message)
        : NLMError(message, "Agent") {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& message)
        : NLMError(message, "Validation") {}
};

// Error helper functions
void throwIfConfigError(bool condition, const std::string& message) {
    if (condition) {
        throw ConfigurationError(message);
    }
}

void throwIfBrainError(bool condition, const std::string& message) {
    if (condition) {
        throw BrainError(message);
    }
}

void throwIfWorldError(bool condition, const std::string& message) {
    if (condition) {
        throw WorldError(message);
    }
}

void throwIfAgentError(bool condition, const std::string& message) {
    if (condition) {
        throw AgentError(message);
    }
}

void throwIfValidationError(bool condition, const std::string& message) {
    if (condition) {
        throw ValidationError(message);
    }
}

// Pythonic iterator classes for container-like access
class NeuronView {
public:
    NeuronView(std::vector<Neuron*>& neurons, const std::string& name)
        : neurons_(neurons), name_(name) {}
    
    // Python-style iterator support
    class iterator {
    public:
        iterator(std::vector<Neuron*>& neurons, size_t index)
            : neurons_(neurons), index_(index) {}
        
        Neuron* operator*() {
            return neurons_[index_];
        }
        
        iterator& operator++() {
            ++index_;
            return *this;
        }
        
        iterator operator++(int) {
            iterator temp = *this;
            ++index_;
            return temp;
        }
        
        bool operator!=(const iterator& other) const {
            return index_ != other.index_;
        }
        
        bool operator==(const iterator& other) const {
            return index_ == other.index_;
        }
        
    private:
        std::vector<Neuron*>& neurons_;
        size_t index_;
    };
    
    iterator begin() {
        return iterator(neurons_, 0);
    }
    
    iterator end() {
        return iterator(neurons_, neurons_.size());
    }
    
    size_t size() const {
        return neurons_.size();
    }
    
    std::string getName() const {
        return name_;
    }
    
    // Python list-like access
    Neuron* operator[](size_t index) {
        return neurons_[index];
    }
    
    // Python context manager support
    void __enter__() {}
    void __exit__(int type, const char* value, const char* traceback) {}
    
private:
    std::vector<Neuron*>& neurons_;
    std::string name_;
};

class SynapseView {
public:
    SynapseView(std::vector<std::unique_ptr<Synapse>>& synapses, const std::string& name)
        : synapses_(synapses), name_(name) {}
    
    // Python-style iterator support
    class iterator {
    public:
        iterator(std::vector<std::unique_ptr<Synapse>>& synapses, size_t index)
            : synapses_(synapses), index_(index) {}
        
        Synapse* operator*() {
            return synapses_[index_].get();
        }
        
        iterator& operator++() {
            ++index_;
            return *this;
        }
        
        iterator operator++(int) {
            iterator temp = *this;
            ++index_;
            return temp;
        }
        
        bool operator!=(const iterator& other) const {
            return index_ != other.index_;
        }
        
        bool operator==(const iterator& other) const {
            return index_ == other.index_;
        }
        
    private:
        std::vector<std::unique_ptr<Synapse>>& synapses_;
        size_t index_;
    };
    
    iterator begin() {
        return iterator(synapses_, 0);
    }
    
    iterator end() {
        return iterator(synapses_, synapses_.size());
    }
    
    size_t size() const {
        return synapses_.size();
    }
    
    std::string getName() const {
        return name_;
    }
    
    // Python list-like access
    Synapse* operator[](size_t index) {
        return synapses_[index].get();
    }
    
    // Context manager support
    void __enter__() {}
    void __exit__(int type, const char* value, const char* traceback) {}
    
private:
    std::vector<std::unique_ptr<Synapse>>& synapses_;
    std::string name_;
};

// Enhanced wrapper classes for better Python integration
class EnhancedConfig {
public:
    EnhancedConfig(std::shared_ptr<Config> config) : config_(config) {}
    
    // Pythonic property access
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        return config_->get<T>(key);
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        return config_->getOr<T>(key, defaultValue);
    }
    
    // Type-specific getters with Pythonic interface
    int getInt(const std::string& key, int defaultValue = 0) const {
        return config_->getOr<int>(key, defaultValue);
    }
    
    double getDouble(const std::string& key, double defaultValue = 0.0) const {
        return config_->getOr<double>(key, defaultValue);
    }
    
    bool getBool(const std::string& key, bool defaultValue = false) const {
        return config_->getOr<bool>(key, defaultValue);
    }
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        return config_->getOr<std::string>(key, defaultValue);
    }
    
    // Validation with descriptive errors
    class ValidationHelper {
    public:
        ValidationHelper(std::shared_ptr<Config> config) : config_(config) {}
        
        ValidationResult validate(const nlohmann::json& schema) const {
            return config_->validate(schema);
        }
        
        bool isValid(const nlohmann::json& schema) const {
            ValidationResult result = validate(schema);
            return result.valid;
        }
        
        std::vector<std::string> getErrors(const nlohmann::json& schema) const {
            ValidationResult result = validate(schema);
            return result.errors;
        }
        
        std::vector<std::string> getWarnings(const nlohmann::json& schema) const {
            ValidationResult result = validate(schema);
            return result.warnings;
        }
        
    private:
        std::shared_ptr<Config> config_;
    };
    
    ValidationHelper validator() const {
        return ValidationHelper(config_);
    }
    
    // Python context manager for safe configuration usage
    class ConfigContext {
    public:
        ConfigContext(std::shared_ptr<Config> config, const nlohmann::json& schema)
            : config_(config), schema_(schema), originalValid_(config_->validate(schema).valid) {}
        
        bool isValid() const {
            return originalValid_;
        }
        
        void __enter__() {
            if (!originalValid_) {
                ValidationResult result = config_->validate(schema_);
                throw ConfigurationError("Configuration validation failed: " + result.getErrorString());
            }
        }
        
        void __exit__(int type, const char* value, const char* traceback) {
            // Nothing to clean up
        }
        
    private:
        std::shared_ptr<Config> config_;
        nlohmann::json schema_;
        bool originalValid_;
    };
    
    ConfigContext safeContext(const nlohmann::json& schema) const {
        return ConfigContext(config_, schema);
    }
    
    // Enhanced setter methods with validation
    template<typename T>
    void set(const std::string& key, const T& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "") {
        config_->set(key, value, source, description);
    }
    
    // Convenience methods for common types
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "") {
        config_->set(key, value, source, description);
    }
    
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "") {
        config_->set(key, value, source, description);
    }
    
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "") {
        config_->set(key, value, source, description);
    }
    
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "") {
        config_->set(key, value, source, description);
    }
    
    // Python list-like interface for getting keys
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    // String representation
    std::string toString() const {
        return "EnhancedConfig: " + std::to_string(config_->getEntryCount()) + " entries";
    }
    
private:
    std::shared_ptr<Config> config_;
};

class EnhancedBrain {
public:
    EnhancedBrain(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Safe access methods with error checking
    size_t getTotalNeuronCount() const {
        return brain_->getTotalNeuronCount();
    }
    
    size_t getTotalSynapseCount() const {
        return brain_->getTotalSynapseCount();
    }
    
    size_t getFiringNeuronCount() const {
        return brain_->getFiringNeuronCount();
    }
    
    float getAverageFiringRate() const {
        return brain_->getAverageFiringRate();
    }
    
    float getExcitationInhibitionRatio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    size_t getTotalSpikeCount() const {
        return brain_->getTotalSpikeCount();
    }
    
    // Safe region access
    class RegionView {
    public:
        RegionView(NeuralRegion* region) : region_(region) {}
        
        RegionId getId() const {
            return region_->getId();
        }
        
        std::string getName() const {
            return region_->getName();
        }
        
        size_t getNeuronCount() const {
            return region_->getTotalNeuronCount();
        }
        
        // Get neurons view
        NeuronView getNeurons() {
            std::vector<Neuron*> neurons = region_->getAllNeurons();
            return NeuronView(neurons, "Region " + std::to_string(region_->getId().index()));
        }
        
        // Get synapses view
        SynapseView getSynapses() {
            std::vector<std::unique_ptr<Synapse>> synapses = region_->getSynapses();
            return SynapseView(synapses, "Region " + std::to_string(region_->getId().index()) + " synapses");
        }
        
    private:
        NeuralRegion* region_;
    };
    
    // Get all regions as views
    std::vector<RegionView> getRegions() {
        std::vector<RegionView> regions;
        auto brainRegions = brain_->getRegions();
        for (auto* region : brainRegions) {
            regions.emplace_back(region);
        }
        return regions;
    }
    
    // Safe action production
    std::unique_ptr<Action> produceAction() {
        return brain_->produceAction();
    }
    
    // Safe brain operations
    void step(SimulationStep step) {
        brain_->step(step);
    }
    
    void step(SimulationStep step, Timestamp time) {
        brain_->step(step, time);
    }
    
    // Context manager for safe brain operations
    class BrainContext {
    public:
        BrainContext(std::shared_ptr<Brain> brain) : brain_(brain) {}
        
        void __enter__() {
            // Nothing to do on enter
        }
        
        void __exit__(int type, const char* value, const char* traceback) {
            // Nothing to clean up
        }
        
        // Safe step with automatic error handling
        bool safeStep(SimulationStep step, Timestamp time = 0.0) {
            try {
                brain_->step(step, time);
                return true;
            } catch (const std::exception& e) {
                // Log error and return false
                return false;
            }
        }
        
    private:
        std::shared_ptr<Brain> brain_;
    };
    
    BrainContext context() {
        return BrainContext(brain_);
    }
    
    // Python dictionary-like access for brain statistics
    py::dict getStatistics() {
        py::dict stats;
        stats["total_neurons"] = brain_->getTotalNeuronCount();
        stats["total_synapses"] = brain_->getTotalSynapseCount();
        stats["firing_neurons"] = brain_->getFiringNeuronCount();
        stats["average_firing_rate"] = brain_->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain_->getTotalSpikeCount();
        stats["pending_spike_events"] = brain_->getPendingSpikeEventCount();
        return stats;
    }
    
    // String representation
    std::string toString() const {
        std::ostringstream oss;
        oss << "EnhancedBrain: " << brain_->getTotalNeuronCount() << " neurons, "
            << brain_->getTotalSynapseCount() << " synapses";
        return oss.str();
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

class EnhancedAgent {
public:
    EnhancedAgent(std::shared_ptr<AgentBrain> agent) : agent_(agent) {}
    
    // Safe agent operations
    void processSensoryInput(const SensoryPercept& percept) {
        agent_->processSensoryInput(percept);
    }
    
    MotorCommand decodeMotorCommand() {
        return agent_->decodeMotorCommand();
    }
    
    // Enhanced diagnostic methods
    std::vector<AgentBrain::MotorGroupActivity> getDetailedMotorActivities() {
        return agent_->getMotorGroupActivities();
    }
    
    // Curiosity and novelty levels
    float getCuriosityLevel() const {
        return agent_->getCuriosityLevel();
    }
    
    float getNoveltyLevel() const {
        return agent_->getNoveltyLevel();
    }
    
    float getNeuromodulationLevel() const {
        return agent_->getNeuromodulationLevel();
    }
    
    float getPredictionError() const {
        return agent_->getPredictionError();
    }
    
    // Development and learning
    void updateDevelopment(double timestep) {
        agent_->updateDevelopment(timestep);
    }
    
    DevelopmentalStage getDevelopmentalStage() const {
        return agent_->getDevelopmentalStage();
    }
    
    // Text encoding (Phase 6 feature)
    std::vector<size_t> encodeText(const std::string& text) {
        return AgentBrain::encodeText(text);
    }
    
    // Thought trace access
    std::string getThoughtTrace() const {
        return agent_->getThoughtTrace();
    }
    
    // Diagnostic counters
    int getReflexBonusCount() const {
        return agent_->getReflexBonusCount();
    }
    
    int getReflexSuppressCount() const {
        return agent_->getReflexSuppressCount();
    }
    
    int getWtaPulseCount() const {
        return agent_->getWtaPulseCount();
    }
    
    int getPlannerOverrideCount() const {
        return agent_->getPlannerOverrideCount();
    }
    
    int getPlannerAgreementCount() const {
        return agent_->getPlannerAgreementCount();
    }
    
    int getPlannerDecisionCount() const {
        return agent_->getPlannerDecisionCount();
    }
    
    int getConceptBiasCount() const {
        return agent_->getConceptBiasCount();
    }
    
    int getSelfBiasCount() const {
        return agent_->getSelfBiasCount();
    }
    
    int getSocialBiasCount() const {
        return agent_->getSocialBiasCount();
    }
    
    // Social learning
    void observeOtherAgent(MotorCommand observed, 
                          const std::vector<float>& beforeVision,
                          const std::vector<float>& afterVision) {
        agent_->observeOtherAgent(observed, beforeVision, afterVision);
    }
    
    // Feature flags
    bool isRewardModulationEnabled() const {
        return agent_->isRewardModulationEnabled();
    }
    
    bool isStructuralPlasticityEnabled() const {
        return agent_->isStructuralPlasticityEnabled();
    }
    
    bool isDevelopmentEnabled() const {
        return agent_->isDevelopmentEnabled();
    }
    
    bool isCuriosityEnabled() const {
        return agent_->isCuriosityEnabled();
    }
    
    // Enable/disable features
    void enableRewardModulation(bool enable) {
        agent_->enableRewardModulation(enable);
    }
    
    void enableStructuralPlasticity(bool enable) {
        agent_->enableStructuralPlasticity(enable);
    }
    
    void enableDevelopment(bool enable) {
        agent_->enableDevelopment(enable);
    }
    
    void enableCuriosity(bool enable) {
        agent_->enableCuriosity(enable);
    }
    
    // Reward modulation with prediction
    void applyRewardModulation(float reward, float predictedReward) {
        agent_->applyRewardModulation(reward, predictedReward);
    }
    
    // Reset for new episode
    void reset() {
        agent_->reset();
    }
    
    // Get brain reference
    std::shared_ptr<Brain> getBrain() {
        return agent_->getBrain();
    }
    
    // Python dictionary-like access for agent state
    py::dict getState() {
        py::dict state;
        state["curiosity_level"] = agent_->getCuriosityLevel();
        state["novelty_level"] = agent_->getNoveltyLevel();
        state["neuromodulation_level"] = agent_->getNeuromodulationLevel();
        state["prediction_error"] = agent_->getPredictionError();
        state["developmental_stage"] = static_cast<int>(agent_->getDevelopmentalStage());
        state["reward_modulation_enabled"] = agent_->isRewardModulationEnabled();
        state["structural_plasticity_enabled"] = agent_->isStructuralPlasticityEnabled();
        state["development_enabled"] = agent_->isDevelopmentEnabled();
        state["curiosity_enabled"] = agent_->isCuriosityEnabled();
        return state;
    }
    
    // String representation
    std::string toString() const {
        return "EnhancedAgent: Curiosity=" + std::to_string(agent_->getCuriosityLevel()) +
               ", Novelty=" + std::to_string(agent_->getNoveltyLevel());
    }
    
private:
    std::shared_ptr<AgentBrain> agent_;
};

// Helper functions for enhanced Python bindings
EnhancedConfig makeEnhancedConfig(std::shared_ptr<Config> config) {
    return EnhancedConfig(config);
}

EnhancedBrain makeEnhancedBrain(std::shared_ptr<Brain> brain) {
    return EnhancedBrain(brain);
}

EnhancedAgent makeEnhancedAgent(std::shared_ptr<AgentBrain> agent) {
    return EnhancedAgent(agent);
}

// Validation utilities
ValidationResult validateWithSchema(std::shared_ptr<Config> config, const nlohmann::json& schema) {
    return config->validate(schema);
}

// Configuration preset helpers
nlohmann::json getPhase2Schema() {
    Config config;
    return config.getPhase2Schema();
}

nlohmann::json getPhase6Schema() {
    Config config;
    return config.getPhase6Schema();
}

// Error checking utilities
def checkConfigValid(std::shared_ptr<Config> config, const nlohmann::json& schema) {
    ValidationResult result = config->validate(schema);
    if (!result.valid) {
        throw ConfigurationError("Configuration validation failed: " + result.getErrorString());
    }
}

// Safe loading functions
bool safeLoadConfig(std::shared_ptr<Config> config, const std::string& filepath, 
                   const nlohmann::json& schema = nlohmann::json()) {
    try {
        bool success = config->loadFromFile(filepath, schema);
        if (success && !schema.is_null()) {
            ValidationResult result = config->validate(schema);
            if (!result.valid) {
                return false;
            }
        }
        return success;
    } catch (const std::exception& e) {
        return false;
    }
}

// Extension utilities
py::dict getPythonTypeMapping() {
    py::dict mapping;
    mapping["int"] = 1;
    mapping["float"] = 2;
    mapping["bool"] = 3;
    mapping["str"] = 4;
    mapping["list"] = 5;
    return mapping;
}

std::string getErrorTypeName(const std::exception& e) {
    if (dynamic_cast<const ConfigurationError*>(&e)) {
        return "ConfigurationError";
    } else if (dynamic_cast<const BrainError*>(&e)) {
        return "BrainError";
    } else if (dynamic_cast<const WorldError*>(&e)) {
        return "WorldError";
    } else if (dynamic_cast<const AgentError*>(&e)) {
        return "AgentError";
    } else if (dynamic_cast<const ValidationError*>(&e)) {
        return "ValidationError";
    } else {
        return "NLMError";
    }
}

} // namespace nlm