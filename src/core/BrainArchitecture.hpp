// Brain Architecture Refactoring - Improved Modularity

#ifndef NLM_BRAIN_ARCHITECTURE_HPP
#define NLM_BRAIN_ARCHITECTURE_HPP

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <functional>

namespace nlm {

// Forward declarations
class NeuralRegion;
class Neuron;
class Synapse;
class SpikeSystem;
class Config;
class RandomGenerator;
class MemorySystem;
class PredictionSystem;
class CognitiveSystem;
class NeuromodulationSystem;
class DevelopmentSystem;

// Brain Component Interface
class IBrainComponent {
public:
    virtual ~IBrainComponent() = default;
    virtual void initialize() = 0;
    virtual void update(double timestep) = 0;
    virtual void reset() = 0;
    virtual std::string getStatus() const = 0;
};

// Brain Configuration Manager
class BrainConfiguration {
public:
    struct Settings {
        size_t neuronCount = 1000;
        size_t regionCount = 1;
        float connectionProbability = 0.1f;
        float simulationTimestep = 0.001f;
        uint64_t randomSeed = 42;
        bool enableSTDP = true;
        bool enableHebbian = true;
        bool enableStructuralPlasticity = true;
        std::map<std::string, std::string> customSettings;
    };

    BrainConfiguration();
    explicit BrainConfiguration(const Settings& settings);

    void loadFromFile(const std::string& filename);
    void loadFromCommandLine(const std::map<std::string, std::string>& options);
    void applyDefaults();
    void validate() const;
    const Settings& getSettings() const { return settings_; }

    // Get individual settings
    size_t getNeuronCount() const { return settings_.neuronCount; }
    size_t getRegionCount() const { return settings_.regionCount; }
    float getConnectionProbability() const { return settings_.connectionProbability; }
    float getSimulationTimestep() const { return settings_.simulationTimestep; }
    uint64_t getRandomSeed() const { return settings_.randomSeed; }

private:
    Settings settings_;
    std::shared_ptr<Config> config_;
};

// Brain Component Base Classes
class NeuralCore : public IBrainComponent {
public:
    virtual void createRegions() = 0;
    virtual void setupConnectivity() = 0;
    virtual size_t getTotalNeuronCount() const = 0;
    virtual size_t getTotalSynapseCount() const = 0;
    virtual Neuron* getNeuron(NeuronId id) = 0;
    virtual Synapse* getSynapse(SynapseId id) = 0;
};

class PlasticityManager : public IBrainComponent {
public:
    virtual void enablePlasticity(bool stdp, bool hebbian, bool rewardModulated) = 0;
    virtual void applyPlasticity(double timestep) = 0;
    virtual float getPlasticityStrength() const = 0;
    virtual void resetPlasticity() = 0;
};

class MemoryManager : public IBrainComponent {
public:
    virtual void storeToWorkingMemory(NeuronId neuronId, float activation) = 0;
    virtual std::vector<NeuronId> getWorkingMemoryNeurons() const = 0;
    virtual void updateWorkingMemory(double timestep) = 0;
    virtual void storeEpisodicMemory(const std::string& episode) = 0;
    virtual void replayEpisodes() = 0;
};

class NeuromodulationController : public IBrainComponent {
public:
    virtual void update Neuromodulation(double timestep) = 0;
    virtual float getDopamineLevel() const = 0;
    virtual float getCuriosityLevel() const = 0;
    virtual void applyNeuromodulationToNeurons() = 0;
};

class DevelopmentManager : public IBrainComponent {
public:
    virtual void updateDevelopment(double timestep) = 0;
    virtual DevelopmentalStage getDevelopmentalStage() const = 0;
    virtual void applyDevelopmentEffects() = 0;
    virtual float getPlasticityModifier() const = 0;
};

// Main Brain Class - Simplified and Modular
class Brain {
public:
    Brain(const std::shared_ptr<BrainConfiguration>& config);
    ~Brain();

    // Initialization and setup
    bool initialize();
    void reset();

    // Core operations
    void step(SimulationStep step);
    void step(SimulationStep step, Timestamp time);

    // Component access
    NeuralCore* getNeuralCore() { return neuralCore_.get(); }
    PlasticityManager* getPlasticityManager() { return plasticityManager_.get(); }
    MemoryManager* getMemoryManager() { return memoryManager_.get(); }
    NeuromodulationController* getNeuromodulationController() { return neuromodulationController_.get(); }
    DevelopmentManager* getDevelopmentManager() { return developmentManager_.get(); }

    // Configuration access
    std::shared_ptr<const BrainConfiguration> getConfiguration() const { return config_; }

    // Statistics and monitoring
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getTotalSpikeCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;

    // Brain operations
    void logStatus() const;
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);

    // Agent interface
    std::shared_ptr<class AgentBrain> createAgent() const;
    void connectToAgent(std::shared_ptr<class AgentBrain> agent);

private:
    std::shared_ptr<BrainConfiguration> config_;

    // Brain components
    std::unique_ptr<NeuralCore> neuralCore_;
    std::unique_ptr<PlasticityManager> plasticityManager_;
    std::unique_ptr<MemoryManager> memoryManager_;
    std::unique_ptr<NeuromodulationController> neuromodulationController_;
    std::unique_ptr<DevelopmentManager> developmentManager_;

    // Supporting systems
    std::unique_ptr<SpikeSystem> spikeSystem_;
    std::unique_ptr<RandomGenerator> randomGenerator_;

    // Agent integration
    std::shared_ptr<class AgentBrain> agentBrain_;

    // Private helper methods
    void setupComponentIntegration();
    void setupSpikeSystem();
    void setupMemorySystems();
    void setupCognitiveSystems();
    void setupNeuromodulationSystems();
    void setupDevelopmentSystem();

    // Component-specific implementations
    class NeuralCoreImpl : public NeuralCore {
    public:
        explicit NeuralCoreImpl(std::shared_ptr<Brain> parentBrain);
        void initialize() override;
        void update(double timestep) override;
        void reset() override;
        std::string getStatus() const override;
        void createRegions() override;
        void setupConnectivity() override;
        size_t getTotalNeuronCount() const override;
        size_t getTotalSynapseCount() const override;
        Neuron* getNeuron(NeuronId id) override;
        Synapse* getSynapse(SynapseId id) override;

    private:
        std::shared_ptr<Brain> parentBrain_;
        std::vector<std::unique_ptr<NeuralRegion>> regions_;
        std::map<NeuronId, Neuron*> neuronMap_;
        std::map<SynapseId, Synapse*> synapseMap_;
    };

    // Similar implementations for other components...
};

} // namespace nlm

#endif // NLM_BRAIN_ARCHITECTURE_HPP
