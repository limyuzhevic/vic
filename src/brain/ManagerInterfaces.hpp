// Manager Interfaces for NLM Brain Refactoring
// Abstract base classes for all specialized managers
// This file provides the interface definitions for the dependency injection system

#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class Brain;
class Config;
class RandomGenerator;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class STDP;
class Hebbian;
class StructuralPlasticity;
class Synapse;

// ========== MEMORY MANAGER INTERFACE ==========

class MemoryManager {
public:
    virtual ~MemoryManager() = default;
    
    virtual void initialize(Brain* brain) = 0;
    virtual void update(float timestep) = 0;
    virtual void storeToNeuron(NeuronId neuronId, float activation) = 0;
    virtual void storeEpisode(const class EpisodicMemoryItem& episode) = 0;
    virtual void clear() = 0;
    
    // Working memory interface
    virtual NeuralWorkingMemory* getWorkingMemory() = 0;
    virtual size_t getWorkingMemoryCapacity() const = 0;
    virtual void setWorkingMemoryCapacity(size_t capacity) = 0;
    
    // Episodic memory interface
    virtual NeuralEpisodicMemory* getEpisodicMemory() = 0;
    virtual size_t getMaxEpisodes() const = 0;
    virtual void setMaxEpisodes(size_t maxEpisodes) = 0;
    virtual size_t getEpisodeCount() const = 0;
    
    // Associative memory interface
    virtual NeuralAssociativeMemory* getAssociativeMemory() = 0;
    virtual void associate(const std::vector<float>& pattern1, const std::vector<float>& pattern2, float strength) = 0;
    virtual std::vector<float> retrieve(const std::vector<float>& pattern) = 0;
    virtual float getAssociationStrength(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const = 0;
};

// ========== PLASTICITY MANAGER INTERFACE ==========

class PlasticityManager {
public:
    virtual ~PlasticityManager() = default;
    
    virtual void initialize(Brain* brain) = 0;
    virtual void update(float timestep) = 0;
    virtual void applyPlasticity(Synapse* synapse, float preSpikeTime, float postSpikeTime, float timestep) = 0;
    virtual void applySTDP(Synapse* synapse, const std::vector<float>& preSpikeHistory, const std::vector<float>& postSpikeHistory, float timestep) = 0;
    virtual void applyHebbian(Synapse* synapse, const std::vector<float>& preSpikeHistory, const std::vector<float>& postSpikeHistory, float timestep) = 0;
    virtual void applyStructuralPlasticity(Brain* brain, RandomGenerator& rng, float timestep) = 0;
    
    // STDP interface
    virtual STDP* getSTDP() = 0;
    virtual void configureSTDP(float ltpWeight, float ltdWeight, float tau) = 0;
    
    // Hebbian interface
    virtual Hebbian* getHebbian() = 0;
    
    // Structural plasticity interface
    virtual StructuralPlasticity* getStructuralPlasticity() = 0;
    virtual void setSynaptogenesisRate(float rate) = 0;
    virtual void setPruningRate(float rate) = 0;
    
    // Neuromodulation interface
    virtual void applyNeuromodulationFactor(float factor) = 0;
};

// ========== DEVELOPMENT MANAGER INTERFACE ==========

class DevelopmentManager {
public:
    virtual ~DevelopmentManager() = default;
    
    virtual void initialize(Brain* brain) = 0;
    virtual void update(Brain* brain, RandomGenerator& rng, float timestep) = 0;
    virtual void applyDevelopmentEffects(Brain* brain, float plasticityMod) = 0;
    
    // Development stage management
    virtual DevelopmentalStage getStage() const = 0;
    virtual void setStage(DevelopmentalStage stage) = 0;
    virtual void advanceStage() = 0;
    virtual bool isStageCompleted() const = 0;
    
    // Development system access
    virtual DevelopmentSystem* getDevelopmentSystem() = 0;
    
    // Plasticity modulation
    virtual float getPlasticityModulator() const = 0;
};

// ========== NEUROMODULATION MANAGER INTERFACE ==========

class NeuromodulationManager {
public:
    virtual ~NeuromodulationManager() = default;
    
    virtual void initialize(Brain* brain) = 0;
    virtual void update(float timestep) = 0;
    
    // Neuromodulator access
    virtual Dopamine* getDopamine() = 0;
    virtual Curiosity* getCuriosity() = 0;
    virtual Novelty* getNovelty() = 0;
    virtual PredictionError* getPredictionErrorSignal() = 0;
    
    // Neuromodulation effects
    virtual void applyToPlasticity(float factor) = 0;
    virtual void applyToExcitability(Brain* brain, float level) = 0;
};

// ========== COGNITION MANAGER INTERFACE ==========

class CognitionManager {
public:
    virtual ~CognitionManager() = default;
    
    virtual void initialize(Brain* brain) = 0;
    virtual void update(float timestep) = 0;
    
    // Cognitive system access
    virtual NeuralPlanner* getPlanner() = 0;
    virtual ConceptFormation* getConceptFormation() = 0;
    virtual AttentionalSelection* getAttention() = 0;
    
    // Planning and attention
    virtual std::vector<float> evaluateActionSequence(const std::vector<float>& sequence) = 0;
    virtual void processCompetition(const std::vector<NeuronId>& competitors) = 0;
    virtual void setPlanningDepth(int depth) = 0;
    virtual void setInhibitionStrength(float strength) = 0;
    virtual void setExcitationStrength(float strength) = 0;
};

// ========== PREDICTION MANAGER INTERFACE ==========

class PredictionManager {
public:
    virtual ~PredictionManager() = default;
    
    virtual void initialize(Brain* brain) = 0;
    virtual void update(float timestep) = 0;
    
    // Prediction system access
    virtual PredictionSystem* getPredictionSystem() = 0;
    
    // Prediction error handling
    virtual float computePredictionError(const std::vector<float>& prediction, const std::vector<float>& actual) = 0;
    virtual void updatePrediction(const std::vector<float>& input) = 0;
    virtual std::vector<float> getCurrentPrediction() const = 0;
};

// ========== STATISTICS MANAGER INTERFACE ==========

class StatisticsManager {
public:
    virtual ~StatisticsManager() = default;
    
    virtual void initialize(Brain* brain) = 0;
    virtual void update(float timestep) = 0;
    virtual void recordEvent(const std::string& eventType, float value = 0.0f) = 0;
    
    // Statistics accessors
    virtual float getExcitationInhibitionRatio() const = 0;
    virtual size_t getTotalSpikeCount() const = 0;
    virtual size_t getPendingSpikeEventCount() const = 0;
    virtual size_t getTotalNeuronCount() const = 0;
    virtual size_t getTotalSynapseCount() const = 0;
    virtual size_t getActiveNeuronCount() const = 0;
    virtual size_t getFiringNeuronCount() const = 0;
    virtual float getAverageFiringRate() const = 0;
    virtual float getMemoryUtilization() const = 0;
    
    // Event and time tracking
    virtual size_t getStepCount() const = 0;
    virtual float getCurrentTime() const = 0;
    virtual float getAverageSpikeFrequency() const = 0;
};

// ========== CONFIGURATION MANAGER INTERFACE ==========

class ConfigurationManager {
public:
    virtual ~ConfigurationManager() = default;
    
    virtual void initialize(Brain* brain) = 0;
    virtual void update(float timestep) = 0;
    
    // Configuration access
    virtual std::shared_ptr<const Config> getConfig() const = 0;
    virtual std::shared_ptr<Config> getConfig() = 0;
    
    // Parameter management
    virtual bool hasParameter(const std::string& key) const = 0;
    virtual float getFloat(const std::string& key, float defaultValue = 0.0f) const = 0;
    virtual int64_t getInt(const std::string& key, int64_t defaultValue = 0) const = 0;
    virtual std::string getString(const std::string& key, const std::string& defaultValue = "") const = 0;
    virtual std::vector<float> getVector(const std::string& key) const = 0;
    
    virtual void setFloat(const std::string& key, float value) = 0;
    virtual void setInt(const std::string& key, int64_t value) = 0;
    virtual void setString(const std::string& key, const std::string& value) = 0;
    
    // Configuration validation and validation
    virtual bool validate() const = 0;
    virtual std::string getValidationErrors() const = 0;
    
    // Configuration persistence
    virtual bool save(const std::string& filepath) const = 0;
    virtual bool load(const std::string& filepath) = 0;
};

} // namespace nlm
