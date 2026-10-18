// Enhanced Python Bindings Headers for NLM
// 
// This file provides enhanced Python bindings with:
// 1. Pythonic APIs for all NLM components
// 2. Comprehensive error handling and validation
// 3. Factory methods and advanced features
// 4. Better documentation and examples
// 5. Custom exception types
// 6. Enhanced configuration management
// 7. Built-in analysis and monitoring tools

#pragma once

#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/core/Types/Types.hpp"
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

namespace nlm {

// Custom exception types for Python bindings
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& message) : NLMException("Configuration error: " + message) {}
};

class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& message) : NLMException("Brain error: " + message) {}
};

class NeuronError : public NLMException {
public:
    explicit NeuronError(const std::string& message) : NLMException("Neuron error: " + message) {}
};

class SynapseError : public NLMException {
public:
    explicit SynapseError(const std::string& message) : NLMException("Synapse error: " + message) {}
};

/**
 * Enhanced Neuron for Python - Pythonic interface with advanced features
 */
class EnhancedNeuron {
public:
    EnhancedNeuron(NeuronId id, std::shared_ptr<Brain> brain = nullptr);
    ~EnhancedNeuron();
    
    // Disable copying, enable moving
    EnhancedNeuron(const EnhancedNeuron&) = delete;
    EnhancedNeuron& operator=(const EnhancedNeuron&) = delete;
    EnhancedNeuron(EnhancedNeuron&&) noexcept;
    EnhancedNeuron& operator=(EnhancedNeuron&&) noexcept;
    
    // Core properties
    NeuronId getId() const;
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State properties with enhanced access
    double getMembranePotential() const;
    void setMembranePotential(double potential);
    double getThreshold() const;
    void setThreshold(double threshold);
    double getRestingPotential() const;
    double getFiringRate() const;
    bool isActive() const;
    bool isRefractory() const;
    
    // Enhanced LIF control
    bool stepLIF(double currentTime, double dt = 0.001);
    
    // Input/output methods
    void injectCurrent(double current);
    void receiveExcitatoryInput(double amplitude);
    void receiveInhibitoryInput(double amplitude);
    
    // Analysis methods
    double getExcitabilityScore() const;
    std::string getStateDescription() const;
    
    // Connectivity
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    
    // Conversion
    std::shared_ptr<Neuron> getInternalNeuron();
    
    // Factory methods
    static std::shared_ptr<EnhancedNeuron> createRandom(int id, double seed = 42.0);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<Brain> brain_;
    
    friend class EnhancedBrain;
};

/**
 * Enhanced Synapse for Python - Enhanced synapse management
 */
class EnhancedSynapse {
public:
    EnhancedSynapse(SynapseId id, std::shared_ptr<Brain> brain = nullptr);
    ~EnhancedSynapse();
    
    // Disable copying, enable moving
    EnhancedSynapse(const EnhancedSynapse&) = delete;
    EnhancedSynapse& operator=(const EnhancedSynapse&) = delete;
    EnhancedSynapse(EnhancedSynapse&&) noexcept;
    EnhancedSynapse& operator=(EnhancedSynapse&&) noexcept;
    
    // Core properties
    SynapseId getId() const;
    double getWeight() const;
    void setWeight(double weight);
    bool isEnabled() const;
    bool isPlastic() const;
    
    // Plasticity control
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // STDP parameters
    void setSTDPParameters(double ltpWeight, double ltdWeight, double tau);
    
    // Analysis
    double getStrengthScore() const;
    std::string getStateDescription() const;
    
    // Conversion
    std::shared_ptr<Synapse> getInternalSynapse();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<Brain> brain_;
};

/**
 * Enhanced Brain for Python - Pythonic brain interface with advanced features
 */
class EnhancedBrain {
public:
    EnhancedBrain(std::shared_ptr<Config> config = nullptr);
    ~EnhancedBrain();
    
    // Disable copying, enable moving
    EnhancedBrain(const EnhancedBrain&) = delete;
    EnhancedBrain& operator=(const EnhancedBrain&) = delete;
    EnhancedBrain(EnhancedBrain&&) noexcept;
    EnhancedBrain& operator=(EnhancedBrain&&) noexcept;
    
    // Configuration
    void configure(size_t neuronCount, size_t regionCount, float connectionProb);
    
    // Core operations
    void step();
    void step(double timestep);
    void step(SimulationStep step, double time);
    
    // Advanced simulation control
    void runForSteps(size_t steps, double timestep = 0.001);
    void runUntilSpikeCount(size_t targetSpikes, double timestep = 0.001);
    
    // Input/output
    void injectCurrentToAll(NeuronType type, double current);
    void injectToNeuron(NeuronId id, double current);
    
    // Analysis and monitoring
    size_t getTotalNeurons() const;
    size_t getTotalSynapses() const;
    size_t getActiveNeurons() const;
    size_t getFiringNeurons() const;
    double getAverageFiringRate() const;
    double getExcitationInhibitionRatio() const;
    size_t getTotalSpikes() const;
    
    // Development and learning
    void advanceDevelopment(double timestep);
    void applyReward(double reward, double predicted = 0.0);
    
    // Brain manipulation
    void addRegion(const std::string& name = "");
    std::shared_ptr<Neuron> getNeuronAtRegion(RegionId regionId, size_t index);
    std::vector<std::shared_ptr<Neuron>> getAllNeurons();
    std::string getSystemStatus() const;
    
    // State management
    void saveState(const std::string& filename);
    void loadState(const std::string& filename);
    
    // Conversion
    std::shared_ptr<Brain> getInternalBrain();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    friend class EnhancedNeuron;
    friend class EnhancedSynapse;
};

/**
 * Enhanced Configuration for Python - Advanced configuration with validation
 */
class EnhancedConfig {
public:
    EnhancedConfig();
    ~EnhancedConfig();
    
    // Disable copying, enable moving
    EnhancedConfig(const EnhancedConfig&) = delete;
    EnhancedConfig& operator=(const EnhancedConfig&) = delete;
    EnhancedConfig(EnhancedConfig&&) noexcept;
    EnhancedConfig& operator=(EnhancedConfig&&) noexcept;
    
    // Configuration operations
    void set(const std::string& key, int value);
    void set(const std::string& key, double value);
    void set(const std::string& key, const std::string& value);
    bool has(const std::string& key) const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
    // Validation
    bool validate() const;
    std::string getValidationError() const;
    
    // Configuration management
    void clear();
    std::vector<std::string> getKeys() const;
    std::string getSummary() const;
    
    // File operations
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    
    // Conversion
    std::shared_ptr<Config> getInternalConfig();
    
    // Factory methods
    static std::shared_ptr<EnhancedConfig> createDefault();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm