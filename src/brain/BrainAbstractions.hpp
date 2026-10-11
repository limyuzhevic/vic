#ifndef BRAIN_ABSTRACTIONS_HPP
#define BRAIN_ABSTRACTIONS_HPP

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralPopulation.hpp"
#include "brain/NeuralRegion.hpp"

namespace nlm {

// Abstract base class for brain computational units
class BrainUnit {
public:
    virtual ~BrainUnit() = default;
    virtual void initialize() = 0;
    virtual void step(SimulationStep step) = 0;
    virtual void reset() = 0;
    virtual std::string getType() const = 0;
};

// Abstract factory for creating brain components
class BrainComponentFactory {
public:
    virtual ~BrainComponentFactory() = default;
    virtual std::unique_ptr<Neuron> createNeuron(NeuronId id) = 0;
    virtual std::unique_ptr<Synapse> createSynapse() = 0;
    virtual std::unique_ptr<NeuralRegion> createRegion(const std::string& name = "") = 0;
    virtual std::unique_ptr<NeuralPopulation> createPopulation() = 0;
};

// Abstract brain architecture interface
class IBrainArchitecture {
public:
    virtual ~IBrainArchitecture() = default;
    virtual void initialize() = 0;
    virtual void step(SimulationStep step, Timestamp time = 0) = 0;
    virtual void reset() = 0;
    virtual void addRegion(std::unique_ptr<NeuralRegion> region) = 0;
    virtual NeuralRegion* getRegion(RegionId id) = 0;
    virtual const NeuralRegion* getRegion(RegionId id) const = 0;
    virtual size_t getRegionCount() const = 0;
    virtual void connectRegions(RegionId source, RegionId target, float weight = 0.0f) = 0;
};

// Abstract brain implementation interface
class IBrainImplementation {
public:
    virtual ~IBrainImplementation() = default;
    virtual void initialize(const Config& config) = 0;
    virtual void step(SimulationStep step, Timestamp time = 0) = 0;
    virtual void reset() = 0;
    virtual float getExcitationInhibitionRatio() const = 0;
    virtual size_t getTotalNeuronCount() const = 0;
    virtual size_t getTotalSynapseCount() const = 0;
    virtual size_t getActiveNeuronCount() const = 0;
    virtual size_t getFiringNeuronCount() const = 0;
    virtual float getAverageFiringRate() const = 0;
};

// Abstract neuromodulation interface
class INeuromodulationSystem {
public:
    virtual ~INeuromodulationSystem() = default;
    virtual void applySignal(const Neuromodulator& signal) = 0;
    virtual void update(SimulationStep step) = 0;
    virtual void reset() = 0;
    virtual float getModulationLevel(const std::string& type) const = 0;
};

// Abstract memory system interface
class IMemorySystem {
public:
    virtual ~IMemorySystem() = default;
    virtual void update(SimulationStep step) = 0;
    virtual void reset() = 0;
    virtual void store(const MemoryItem& item) = 0;
    virtual MemoryItem retrieve(const MemoryId& id) = 0;
    virtual std::vector<MemoryId> getRecentMemories() const = 0;
};

// Abstract plasticity system interface
class IPlasticitySystem {
public:
    virtual ~IPlasticitySystem() = default;
    virtual void update(SimulationStep step, Timestamp time) = 0;
    virtual void reset() = 0;
    virtual void applyPlasticity() = 0;
    virtual void configurePlasticity(const PlasticityConfig& config) = 0;
};

// Abstract development system interface
class IDevelopmentSystem {
public:
    virtual ~IDevelopmentSystem() = default;
    virtual void update(SimulationStep step) = 0;
    virtual void reset() = 0;
    virtual DevelopmentalStage getCurrentStage() const = 0;
    virtual void setStage(DevelopmentalStage stage) = 0;
};

// Abstract sensory interface
class ISensorySystem {
public:
    virtual ~ISensorySystem() = default;
    virtual void processInput(const SensoryInput& input) = 0;
    virtual std::vector<float> getSensoryActivity() const = 0;
    virtual void reset() = 0;
};

// Abstract motor interface
class IMotorSystem {
public:
    virtual ~IMotorSystem() = default;
    virtual std::unique_ptr<Action> produceAction() = 0;
    virtual void update(SimulationStep step) = 0;
    virtual void reset() = 0;
};

// Abstract prediction system interface
class IPredictionSystem {
public:
    virtual ~IPredictionSystem() = default;
    virtual void update(SimulationStep step) = 0;
    virtual void reset() = 0;
    virtual float getPredictionError() const = 0;
    virtual void setPrediction(const std::vector<float>& prediction) = 0;
};

// Abstract cognition system interface
class ICognitionSystem {
public:
    virtual ~ICognitionSystem() = default;
    virtual std::unique_ptr<Action> planAction() = 0;
    virtual void update(SimulationStep step) = 0;
    virtual void reset() = 0;
    virtual std::vector<float> getAttentionWeights() const = 0;
};

// Abstract environment interface
class IEnvironment {
public:
    virtual ~IEnvironment() = default;
    virtual void update(SimulationStep step) = 0;
    virtual SensoryInput getObservation() const = 0;
    virtual void applyAction(const Action& action) = 0;
    virtual float getReward() const = 0;
    virtual bool isDone() const = 0;
    virtual void reset() = 0;
};

} // namespace nlm

#endif // BRAIN_ABSTRACTIONS_HPP