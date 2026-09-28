// Brain System Interfaces
// This file defines abstract interfaces for all major brain systems
// enabling better separation of concerns and easier testing

#ifndef NLM_SYSTEM_INTERFACES_H
#define NLM_SYSTEM_INTERFACES_H

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Brain;

// Abstract interfaces for all major brain systems

// 1. Memory Systems Interfaces
class IWorkingMemory {
public:
    virtual ~IWorkingMemory() = default;
    virtual void initialize(Brain* brain) = 0;
    virtual void update(SimulationStep step, Timestamp time) = 0;
    virtual size_t getActiveTraces() const = 0;
    virtual float getWorkingMemory(SimulationStep step, Timestamp time) = 0;
};

class IEpisodicMemory {
public:
    virtual ~IEpisodicMemory() = default;
    virtual void initialize(Brain* brain) = 0;
    virtual void update(SimulationStep step, Timestamp time) = 0;
    virtual size_t getEpisodeCount() const = 0;
    virtual std::vector<NeuronId> getEpisodesForReplay(size_t maxEpisodes) = 0;
};

class IAssociativeMemory {
public:
    virtual ~IAssociativeMemory() = default;
    virtual void initialize(Brain* brain) = 0;
    virtual void update(SimulationStep step, Timestamp time) = 0;
    virtual float getAssociationStrength(NeuronId neuron1, NeuronId neuron2) const = 0;
    virtual void updateAssociation(NeuronId neuron1, NeuronId neuron2, float strength) = 0;
};

// 2. Plasticity Systems Interfaces
class IPlasticityRule {
public:
    virtual ~IPlasticityRule() = default;
    virtual void configure(float param1, float param2, float param3) = 0;
    virtual void update(NeuronId pre, NeuronId post, float preSpikeTime, float postSpikeTime) = 0;
    virtual float getWeightChange() const = 0;
};

class IPlasticitySystem {
public:
    virtual ~IPlasticitySystem() = default;
    virtual void initialize(Brain* brain) = 0;
    virtual void update(SimulationStep step, Timestamp time) = 0;
    virtual void enablePlasticity(bool hebbian, bool stdp, bool structural) = 0;
};

// 3. Neuromodulation Systems Interfaces
class INeuromodulator {
public:
    virtual ~INeuromodulator() = default;
    virtual void initialize(Brain* brain) = 0;
    virtual void update(SimulationStep step, Timestamp time) = 0;
    virtual float getSignal() const = 0;
    virtual void applyEffect(std::vector<Neuron*>& neurons) = 0;
};

class IDevelopmentSystem {
public:
    virtual ~IDevelopmentSystem() = default;
    virtual void initialize(Brain* brain) = 0;
    virtual void update(SimulationStep step, Timestamp time) = 0;
    virtual DevelopmentalStage getDevelopmentalStage() const = 0;
    virtual void advanceStage() = 0;
};

// 4. Cognition Systems Interfaces
class IAttentionSystem {
public:
    virtual ~IAttentionSystem() = default;
    virtual void initialize(Brain* brain) = 0;
    virtual void update(SimulationStep step, Timestamp time) = 0;
    virtual void setInhibitionStrength(float strength) = 0;
    virtual void setExcitationStrength(float strength) = 0;
    virtual NeuronId getAttendedNeuron() const = 0;
};

class IPlanningSystem {
public:
    virtual ~IPlanningSystem() = default;
    virtual void initialize(Brain* brain) = 0;
    virtual void update(SimulationStep step, Timestamp time) = 0;
    virtual void setPlanningDepth(size_t depth) = 0;
    virtual std::vector<NeuronId> getPlannedAction() = 0;
};

class IConceptFormationSystem {
public:
    virtual ~IConceptFormationSystem() = default;
    virtual void initialize(Brain* brain) = 0;
    virtual void update(SimulationStep step, Timestamp time) = 0;
    virtual std::vector<float> getConceptVector(NeuronId neuron) const = 0;
    virtual void updateConceptVector(NeuronId neuron, const std::vector<float>& concept) = 0;
};

// 5. Prediction Systems Interfaces
class IPredictionSystem {
public:
    virtual ~IPredictionSystem() = default;
    virtual void initialize(Brain* brain) = 0;
    virtual void update(SimulationStep step, Timestamp time) = 0;
    virtual float getPredictionError() const = 0;
    virtual std::vector<float> getPredictions() const = 0;
};

// 6. Performance Monitoring Interface
class IPerformanceMonitor {
public:
    virtual ~IPerformanceMonitor() = default;
    virtual void initialize(Brain* brain) = 0;
    virtual void update(SimulationStep step, Timestamp time) = 0;
    virtual float getAverageFiringRate() const = 0;
    virtual size_t getTotalSpikeCount() const = 0;
    virtual std::string getPerformanceReport() const = 0;
};

// 7. Configuration Interface
class IConfigurator {
public:
    virtual ~IConfigurator() = default;
    virtual void loadFromFile(const std::string& filepath) = 0;
    virtual void loadFromArgs(int argc, char** argv) = 0;
    virtual std::string getValue(const std::string& key) const = 0;
    virtual void setValue(const std::string& key, const std::string& value) = 0;
};

// System Factory Interface for dependency injection
class ISystemFactory {
public:
    virtual ~ISystemFactory() = default;
    virtual std::unique_ptr<IWorkingMemory> createWorkingMemory() = 0;
    virtual std::unique_ptr<IEpisodicMemory> createEpisodicMemory() = 0;
    virtual std::unique_ptr<IAssociativeMemory> createAssociativeMemory() = 0;
    virtual std::unique_ptr<IPlasticitySystem> createPlasticitySystem() = 0;
    virtual std::unique_ptr<IDevelopmentSystem> createDevelopmentSystem() = 0;
    virtual std::unique_ptr<IAttentionSystem> createAttentionSystem() = 0;
    virtual std::unique_ptr<IPlanningSystem> createPlanningSystem() = 0;
    virtual std::unique_ptr<IConceptFormationSystem> createConceptFormationSystem() = 0;
    virtual std::unique_ptr<IPredictionSystem> createPredictionSystem() = 0;
    virtual std::unique_ptr<IPerformanceMonitor> createPerformanceMonitor() = 0;
};

} // namespace nlm

#endif // NLM_SYSTEM_INTERFACES_H