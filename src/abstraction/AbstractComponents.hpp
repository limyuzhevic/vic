#pragma once

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Types/Types.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class RandomGenerator;
class SimulationClock;
class Logger;

// Abstract base class for all brain implementations (updated)
class AbstractBrain {
public:
    virtual ~AbstractBrain() = default;
    
    // Core simulation methods
    virtual bool initialize() = 0;
    virtual void step(SimulationStep currentStep) = 0;
    virtual void step(SimulationStep currentStep, Timestamp currentTime) = 0;
    
    // Input/output methods
    virtual void receiveSensoryInput(const class SensoryInput& input) = 0;
    virtual void injectCurrent(NeuronId neuron, MembranePotential current) = 0;
    virtual void injectCurrentToNeurons(NeuronType type, MembranePotential current) = 0;
    virtual std::unique_ptr<class Action> produceAction() = 0;
    
    // System access methods
    virtual class SpikeSystem* getSpikeSystem() = 0;
    virtual const class SpikeSystem* getSpikeSystem() const = 0;
    
    // Plasticity access
    virtual class STDP* getSTDP() = 0;
    virtual class Hebbian* getHebbian() = 0;
    virtual class StructuralPlasticity* getStructuralPlasticity() = 0;
    
    // Memory access
    virtual class NeuralWorkingMemory* getWorkingMemory() = 0;
    virtual class NeuralEpisodicMemory* getEpisodicMemory() = 0;
    virtual class NeuralAssociativeMemory* getAssociativeMemory() = 0;
    
    // Prediction and cognition
    virtual class PredictionSystem* getPredictionSystem() = 0;
    virtual class NeuralPlanner* getPlanner() = 0;
    virtual class ConceptFormation* getConceptFormation() = 0;
    
    // Development
    virtual class DevelopmentSystem* getDevelopmentSystem() = 0;
    virtual DevelopmentalStage getDevelopmentalStage() const = 0;
    virtual void setDevelopmentalStage(DevelopmentalStage stage) = 0;
    
    // Neuromodulation
    virtual class Dopamine* getDopamine() = 0;
    virtual class Curiosity* getCuriosity() = 0;
    virtual class Novelty* getNovelty() = 0;
    virtual class PredictionError* getPredictionErrorSignal() = 0;
    
    // Statistics
    virtual float getExcitationInhibitionRatio() const = 0;
    virtual size_t getTotalSpikeCount() const = 0;
    virtual size_t getPendingSpikeEventCount() const = 0;
    virtual size_t getTotalNeuronCount() const = 0;
    virtual size_t getTotalSynapseCount() const = 0;
    virtual size_t getActiveNeuronCount() const = 0;
    virtual size_t getFiringNeuronCount() const = 0;
    virtual float getAverageFiringRate() const = 0;
    
    // Configuration
    virtual std::shared_ptr<const Config> getConfig() const = 0;
    virtual RandomGenerator* getRandomGenerator() = 0;
    
    // State management
    virtual void reset() = 0;
    virtual bool save(const std::string& filepath) const = 0;
    virtual bool load(const std::string& filepath) = 0;
    
    // Region management
    virtual RegionId addRegion(const std::string& name = "") = 0;
    virtual NeuralRegion* getRegion(RegionId id) = 0;
    virtual const NeuralRegion* getRegion(RegionId id) const = 0;
    virtual size_t getRegionCount() const = 0;
    
    // Operations
    virtual void applyNeuromodulation(const Neuromodulator& signal) = 0;
    virtual void updatePlasticity() = 0;
    virtual void develop() = 0;
    
    // Information
    virtual void logStatus() const = 0;
    
    // Identity
    virtual const char* getName() const = 0;
    virtual void setName(const std::string& name) = 0;
    
    // Plugin support
    virtual bool hasPlugin(const std::string& pluginName) const = 0;
    virtual void enablePlugin(const std::string& pluginName) = 0;
    virtual void disablePlugin(const std::string& pluginName) = 0;
};

// Abstract base class for all plasticity rules (updated)
class AbstractPlasticityRule {
public:
    virtual ~AbstractPlasticityRule() = default;
    
    // Core plasticity update
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Information
    virtual const char* getName() const = 0;
    virtual std::string getType() const = 0;
    
    // Configuration
    virtual void configure(const Config& config) = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual bool isEnabled() const = 0;
    
    // Statistics
    virtual float getLearningRate() const = 0;
    virtual float getPlasticityStrength() const = 0;
    virtual size_t getUpdateCount() const = 0;
    
    // State management
    virtual void reset() = 0;
    virtual void save(std::ostream& stream) const = 0;
    virtual void load(std::istream& stream) = 0;
    
    // Plugin support
    virtual bool hasPlugin(const std::string& pluginName) const = 0;
    virtual void enablePlugin(const std::string& pluginName) = 0;
    virtual void disablePlugin(const std::string& pluginName) = 0;
};

// Abstract base class for neuromodulators (updated)
class AbstractNeuromodulator {
public:
    virtual ~AbstractNeuromodulator() = default;
    
    // Core neuromodulation
    virtual void update(TimestepDuration dt) = 0;
    virtual float getPlasticityFactor() const = 0;
    
    // Information
    virtual const char* getName() const = 0;
    virtual std::string getType() const = 0;
    
    // Concentration/state
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Configuration
    virtual void configure(const Config& config) = 0;
    
    // Effects
    virtual void signalReward(float reward) = 0;
    virtual void signalPredictionError(float error) = 0;
    virtual void signalNovelty(float novelty) = 0;
    
    // Statistics
    virtual float getExcitabilityModulator() const = 0;
    virtual float getInhibitionModulator() const = 0;
    virtual size_t getSignalCount() const = 0;
    
    // State management
    virtual void reset() = 0;
    virtual void save(std::ostream& stream) const = 0;
    virtual void load(std::istream& stream) = 0;
    
    // Plugin support
    virtual bool hasPlugin(const std::string& pluginName) const = 0;
    virtual void enablePlugin(const std::string& pluginName) = 0;
    virtual void disablePlugin(const std::string& pluginName) = 0;
};

// Abstract base class for development systems (updated)
class AbstractDevelopmentSystem {
public:
    virtual ~AbstractDevelopmentSystem() = default;
    
    // Core development
    virtual void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) = 0;
    
    // Information
    virtual const char* getName() const = 0;
    virtual std::string getType() const = 0;
    
    // Development stages
    virtual DevelopmentalStage getStage() const = 0;
    virtual void setStage(DevelopmentalStage stage) = 0;
    virtual const char* getStageName() const = 0;
    
    // Plasticity modulation
    virtual float getPlasticityModifier() const = 0;
    
    // Critical period
    virtual bool isCriticalPeriod() const = 0;
    virtual float getCriticalPeriodProgress() const = 0;
    
    // Configuration
    virtual void configure(const Config& config) = 0;
    
    // Statistics
    virtual double getDevelopmentalAge() const = 0;
    virtual void setDevelopmentalAge(double age) = 0;
    virtual size_t getUpdateCount() const = 0;
    
    // State management
    virtual void reset() = 0;
    virtual void save(std::ostream& stream) const = 0;
    virtual void load(std::istream& stream) = 0;
    
    // Plugin support
    virtual bool hasPlugin(const std::string& pluginName) const = 0;
    virtual void enablePlugin(const std::string& pluginName) = 0;
    virtual void disablePlugin(const std::string& pluginName) = 0;
};

// Abstract base class for memory systems (updated)
class AbstractMemorySystem {
public:
    virtual ~AbstractMemorySystem() = default;
    
    // Core memory operations
    virtual void initialize(Brain* brain) = 0;
    virtual void update(TimestepDuration dt) = 0;
    
    // Information
    virtual const char* getName() const = 0;
    virtual std::string getType() const = 0;
    
    // State management
    virtual void reset() = 0;
    virtual void save(std::ostream& stream) const = 0;
    virtual void load(std::istream& stream) = 0;
    
    // Capacity and limits
    virtual size_t getCapacity() const = 0;
    virtual void setCapacity(size_t capacity) = 0;
    
    // Statistics
    virtual size_t getStorageSize() const = 0;
    virtual size_t getUtilization() const = 0;
    virtual float getUtilizationRatio() const = 0;
    
    // Plugin support
    virtual bool hasPlugin(const std::string& pluginName) const = 0;
    virtual void enablePlugin(const std::string& pluginName) = 0;
    virtual void disablePlugin(const std::string& pluginName) = 0;
};

} // namespace nlm
