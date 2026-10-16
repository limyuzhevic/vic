#pragma once

#include "PluginBase.hpp"
#include "../../brain/Neuron.hpp"

namespace nlm {
namespace plugin {

// LIF (Leaky Integrate-and-Fire) Neuron Type Plugin
class LIFNeuronPlugin : public NeuronTypePlugin {
public:
    LIFNeuronPlugin();
    ~LIFNeuronPlugin() override;
    
    const std::string& getNeuronTypeName() const override;
    float getDefaultThreshold() const override;
    float getDefaultRestingPotential() const override;
    float getDefaultMembraneTimeConstant() const override;
    float getDefaultRefractoryPeriod() const override;
    
    std::unique_ptr<class Neuron> createNeuron(NeuronId id) override;
    bool updateNeuronState(class Neuron& neuron, TimestepDuration dt) override;
    bool checkForSpike(class Neuron& neuron) const override;
    void resetNeuron(class Neuron& neuron) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// AdaptiveExponential Neuron Type Plugin
class AdaptiveExponentialNeuronPlugin : public NeuronTypePlugin {
public:
    AdaptiveExponentialNeuronPlugin();
    ~AdaptiveExponentialNeuronPlugin() override;
    
    const std::string& getNeuronTypeName() const override;
    float getDefaultThreshold() const override;
    float getDefaultRestingPotential() const override;
    float getDefaultMembraneTimeConstant() const override;
    float getDefaultRefractoryPeriod() const override;
    
    std::unique_ptr<class Neuron> createNeuron(NeuronId id) override;
    bool updateNeuronState(class Neuron& neuron, TimestepDuration dt) override;
    bool checkForSpike(class Neuron& neuron) const override;
    void resetNeuron(class Neuron& neuron) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Standard plasticity rule plugin
class STDPPlugin : public PlasticityRulePlugin {
public:
    STDPPlugin();
    ~STDPPlugin() override;
    
    const std::string& getRuleName() const override;
    const std::string& getRuleDescription() const override;
    PlasticityRuleType getSupportedRuleType() const override;
    
    bool updatePlasticity(class Synapse& synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) override;
    bool applyWeightChange(class Synapse& synapse, SynapticWeight delta) override;
    float getEligibilityTrace(class Synapse& synapse) const override;
    void updateEligibilityTrace(class Synapse& synapse, float delta, TimestepDuration dt) override;
    void resetEligibilityTrace(class Synapse& synapse) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Hebbian plasticity rule plugin
class HebbianPlugin : public PlasticityRulePlugin {
public:
    HebbianPlugin();
    ~HebbianPlugin() override;
    
    const std::string& getRuleName() const override;
    const std::string& getRuleDescription() const override;
    PlasticityRuleType getSupportedRuleType() const override;
    
    bool updatePlasticity(class Synapse& synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) override;
    bool applyWeightChange(class Synapse& synapse, SynapticWeight delta) override;
    float getEligibilityTrace(class Synapse& synapse) const override;
    void updateEligibilityTrace(class Synapse& synapse, float delta, TimestepDuration dt) override;
    void resetEligibilityTrace(class Synapse& synapse) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Dopamine neuromodulator plugin
class DopaminePlugin : public NeuromodulatorPlugin {
public:
    DopaminePlugin();
    ~DopaminePlugin() override;
    
    const std::string& getModulatorName() const override;
    NeuromodulatorType getModulatorType() const override;
    float getDefaultConcentration() const override;
    
    float getEffectOnPlasticity() const override;
    float getEffectOnExcitation() const override;
    float getEffectOnInhibition() const override;
    
    bool updateModulatorLevel(TimestepDuration dt) override;
    void applyModulatoryEffects(class Brain& brain) override;
    void resetModulator() override;
    
    // Reward signaling
    void signalReward(float reward) override;
    void signalRewardPredictionError(float error) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin neuromodulator plugin
class SerotoninPlugin : public NeuromodulatorPlugin {
public:
    SerotoninPlugin();
    ~SerotoninPlugin() override;
    
    const std::string& getModulatorName() const override;
    NeuromodulatorType getModulatorType() const override;
    float getDefaultConcentration() const override;
    
    float getEffectOnPlasticity() const override;
    float getEffectOnExcitation() const override;
    float getEffectOnInhibition() const override;
    
    bool updateModulatorLevel(TimestepDuration dt) override;
    void applyModulatoryEffects(class Brain& brain) override;
    void resetModulator() override;
    
    // Mood-related functions
    void signalMoodChange(float moodLevel) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// WorkingMemory plugin
class WorkingMemoryPlugin : public MemorySystemPlugin {
public:
    WorkingMemoryPlugin();
    ~WorkingMemoryPlugin() override;
    
    const std::string& getMemorySystemName() const override;
    MemorySystemType getMemorySystemType() const override;
    size_t getMaxCapacity() const override;
    
    bool storeMemory(const MemoryEntry& entry) override;
    bool retrieveMemory(MemoryId id, MemoryEntry& entry) const override;
    bool updateMemory(const MemoryEntry& entry) override;
    bool deleteMemory(MemoryId id) override;
    
    bool clearMemory() override;
    size_t getMemoryUsage() const override;
    float getMemoryUtilization() const override;
    std::vector<MemoryId> getAllMemoryIds() const override;
    
    // Working memory specific functions
    bool isMemoryActive(MemoryId id) const override;
    void updateMemoryActivity(MemoryId id, float activity) override;
    float getMemoryPriority(MemoryId id) const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// EpisodicMemory plugin
class EpisodicMemoryPlugin : public MemorySystemPlugin {
public:
    EpisodicMemoryPlugin();
    ~EpisodicMemoryPlugin() override;
    
    const std::string& getMemorySystemName() const override;
    MemorySystemType getMemorySystemType() const override;
    size_t getMaxCapacity() const override;
    
    bool storeMemory(const MemoryEntry& entry) override;
    bool retrieveMemory(MemoryId id, MemoryEntry& entry) const override;
    bool updateMemory(const MemoryEntry& entry) override;
    bool deleteMemory(MemoryId id) override;
    
    bool clearMemory() override;
    size_t getMemoryUsage() const override;
    float getMemoryUtilization() const override;
    std::vector<MemoryId> getAllMemoryIds() const override;
    
    // Episodic memory specific functions
    bool storeEpisode(const Episode& episode) override;
    bool retrieveEpisode(EpisodeId id, Episode& episode) const override;
    bool updateEpisode(const Episode& episode) override;
    bool deleteEpisode(EpisodeId id) override;
    
    std::vector<EpisodeId> getRecentEpisodes(size_t count) const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

}}} // namespace nlm::plugin