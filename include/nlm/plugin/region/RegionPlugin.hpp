#pragma once

#include "NLMPlugin.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @class RegionPlugin
 * @brief Plugin interface for custom brain region types
 */
class RegionPlugin : public virtual NLMPlugin {
public:
    virtual ~RegionPlugin() = default;
    
    // Type check
    bool isRegionPlugin() const override { return true; }
    
    // Region factory methods
    virtual void* getRegionFactory() override {
        return static_cast<void*>(this);
    }
    
    // Abstract plugin interface for regions
    virtual NeuralRegion* createRegion(const std::string& name = "") = 0;
    virtual NeuralRegion* createRegion(RegionId id, const std::string& name = "") = 0;
    virtual bool canCreateRegion(const std::string& regionType) const = 0;
    virtual std::vector<std::string> getSupportedRegionTypes() const = 0;
    
    // Region configuration
    virtual bool configureRegion(NeuralRegion& region, const std::string& config) = 0;
    virtual std::string getRegionConfig(const NeuralRegion& region) const = 0;
};

/**
 * @class NeuronPlugin
 * @brief Plugin interface for custom neuron types
 */
class NeuronPlugin : public virtual NLMPlugin {
public:
    virtual ~NeuronPlugin() = default;
    
    // Type check
    bool isNeuronPlugin() const override { return true; }
    
    // Neuron factory methods
    virtual void* getNeuronFactory() override {
        return static_cast<void*>(this);
    }
    
    // Abstract plugin interface for neurons
    virtual Neuron* createNeuron(NeuronId id) = 0;
    virtual Neuron* createNeuron(NeuronId id, NeuronType type) = 0;
    virtual bool canCreateNeuron(const std::string& neuronType) const = 0;
    virtual std::vector<std::string> getSupportedNeuronTypes() const = 0;
    
    // Neuron configuration
    virtual bool configureNeuron(Neuron& neuron, const std::string& config) = 0;
    virtual std::string getNeuronConfig(const Neuron& neuron) const = 0;
};

/**
 * @class PlasticityPlugin
 * @brief Plugin interface for custom plasticity rules
 */
class PlasticityPlugin : public virtual NLMPlugin {
public:
    virtual ~PlasticityPlugin() = default;
    
    // Type check
    bool isPlasticityPlugin() const override { return true; }
    
    // Plasticity factory methods
    virtual void* getPlasticityFactory() override {
        return static_cast<void*>(this);
    }
    
    // Abstract plugin interface for plasticity
    virtual void* createPlasticityRule() = 0;
    virtual bool canCreatePlasticityRule(const std::string& ruleType) const = 0;
    virtual std::vector<std::string> getSupportedPlasticityRuleTypes() const = 0;
    
    // Plasticity configuration
    virtual bool configurePlasticityRule(void* rule, const std::string& config) = 0;
    virtual std::string getPlasticityRuleConfig(void* rule) const = 0;
};

/**
 * @class SensoryPlugin
 * @brief Plugin interface for custom sensory modalities
 */
class SensoryPlugin : public virtual NLMPlugin {
public:
    virtual ~SensoryPlugin() = default;
    
    // Type check
    bool isSensoryPlugin() const override { return true; }
    
    // Sensory factory methods
    virtual void* getSensoryFactory() override {
        return static_cast<void*>(this);
    }
    
    // Abstract plugin interface for sensory
    virtual void* createSensoryModality() = 0;
    virtual bool canCreateSensoryModality(const std::string& modalityType) const = 0;
    virtual std::vector<std::string> getSupportedSensoryModalityTypes() const = 0;
    
    // Sensory configuration
    virtual bool configureSensoryModality(void* modality, const std::string& config) = 0;
    virtual std::string getSensoryModalityConfig(void* modality) const = 0;
};

/**
 * @class MemoryPlugin
 * @brief Plugin interface for custom memory systems
 */
class MemoryPlugin : public virtual NLMPlugin {
public:
    virtual ~MemoryPlugin() = default;
    
    // Type check
    bool isMemoryPlugin() const override { return true; }
    
    // Memory factory methods
    virtual void* getMemoryFactory() override {
        return static_cast<void*>(this);
    }
    
    // Abstract plugin interface for memory
    virtual void* createMemorySystem() = 0;
    virtual bool canCreateMemorySystem(const std::string& memoryType) const = 0;
    virtual std::vector<std::string> getSupportedMemorySystemTypes() const = 0;
    
    // Memory configuration
    virtual bool configureMemorySystem(void* memory, const std::string& config) = 0;
    virtual std::string getMemorySystemConfig(void* memory) const = 0;
};

/**
 * @class NeuromodulationPlugin
 * @brief Plugin interface for custom neuromodulators
 */
class NeuromodulationPlugin : public virtual NLMPlugin {
public:
    virtual ~NeuromodulationPlugin() = default;
    
    // Type check
    bool isNeuromodulationPlugin() const override { return true; }
    
    // Neuromodulation factory methods
    virtual void* getNeuromodulationFactory() override {
        return static_cast<void*>(this);
    }
    
    // Abstract plugin interface for neuromodulation
    virtual void* createNeuromodulator() = 0;
    virtual bool canCreateNeuromodulator(const std::string& neuromodulatorType) const = 0;
    virtual std::vector<std::string> getSupportedNeuromodulatorTypes() const = 0;
    
    // Neuromodulation configuration
    virtual bool configureNeuromodulator(void* neuromodulator, const std::string& config) = 0;
    virtual std::string getNeuromodulatorConfig(void* neuromodulator) const = 0;
};

} // namespace nlm