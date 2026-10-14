#include "BrainSystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Neuron Processing System
class NeuronalSystem : public SpecializedBrainSystem {
public:
    NeuronalSystem(SystemPriority priority = SystemPriority::Critical);
    
    bool initialize(Brain* brain) override;
    void update(double timestep) override;
    void reset() override;
    std::string getName() const override { return "NeuronalSystem"; }
    bool isEnabled() const override { return true; }
    
    // Execute neuronal dynamics (LIF model, spike detection)
    void processNeuronalDynamics(SimulationStep step, Timestamp time);
    void detectAndScheduleSpikes(SimulationStep step, Timestamp time);
    void processDelayedSpikes(SimulationStep step, Timestamp time);
    void deliverSynapticInput(const class DelayedSpikeEvent& event);
    
    // Statistics and monitoring
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
};

// Memory Integration System
class MemorySystem : public SpecializedBrainSystem {
public:
    MemorySystem(SystemPriority priority = SystemPriority::High);
    
    bool initialize(Brain* brain) override;
    void update(double timestep) override;
    void reset() override;
    std::string getName() const override { return "MemorySystem"; }
    bool isEnabled() const override { return true; }
    
    // Working memory operations
    void updateWorkingMemory(double timestep);
    void storeSpikeActivity(const class SpikeEvent& spike, double membranePotential);
    
    // Episodic memory operations
    void updateEpisodicMemory(SimulationStep step, float reward);
    void captureCurrentState(SimulationStep step, float reward);
    
    // Replay operations
    void replayMemories(SimulationStep step, size_t count);
    
    // Statistics
    size_t getWorkingMemoryTraceCount() const;
    size_t getEpisodicMemoryEpisodeCount() const;
};

// Prediction and Cognition System
class PredictionSystem : public SpecializedBrainSystem {
public:
    PredictionSystem(SystemPriority priority = SystemPriority::High);
    
    bool initialize(Brain* brain) override;
    void update(double timestep) override;
    void reset() override;
    std::string getName() const override { return "PredictionSystem"; }
    bool isEnabled() const override { return true; }
    
    // Prediction operations
    void updatePrediction(SimulationStep step);
    void updateAttention(double timestep);
    void updateConceptFormation();
    void processCompetition(std::vector<NeuronId>& competitors);
    
    // Planning operations
    void updatePlanning(SimulationStep step);
};

// Neuromodulation System
class NeuromodulationSystem : public SpecializedBrainSystem {
public:
    NeuromodulationSystem(SystemPriority priority = SystemPriority::High);
    
    bool initialize(Brain* brain) override;
    void update(double timestep) override;
    void reset() override;
    std::string getName() const override { return "NeuromodulationSystem"; }
    bool isEnabled() const override { return true; }
    
    // Neuromodulation operations
    void updateNovelty(double timestep);
    void updateCuriosity(double timestep);
    void updateDopamine(double timestep);
    float getDopamineLevel() const;
    float getPlasticityFactor() const;
    
    // Apply neuromodulation effects
    void applyDopamineEffects(SimulationStep step);
    void applyCuriosityEffects(double timestep);
};

// Plasticity System
class PlasticitySystem : public SpecializedBrainSystem {
public:
    PlasticitySystem(SystemPriority priority = SystemPriority::Medium);
    
    bool initialize(Brain* brain) override;
    void update(double timestep) override;
    void reset() override;
    std::string getName() const override { return "PlasticitySystem"; }
    bool isEnabled() const override { return true; }
    
    // Plasticity operations
    void applySTDP(SimulationStep step);
    void applyHebbian(SimulationStep step);
    void applyStructuralPlasticity(SimulationStep step);
    void updateDevelopment(SimulationStep step);
    
    // Development effects
    void updateDevelopmentalStage(SimulationStep step);
    float getPlasticityModulation() const;
};

// Development System
class DevelopmentSystem : public SpecializedBrainSystem {
public:
    DevelopmentSystem(SystemPriority priority = SystemPriority::Medium);
    
    bool initialize(Brain* brain) override;
    void update(double timestep) override;
    void reset() override;
    std::string getName() const override { return "DevelopmentSystem"; }
    bool isEnabled() const override { return true; }
    
    // Development operations
    void applyDevelopment(SimulationStep step, double timestep);
    void updatePlasticityRates(float plasticityMod);
    
    DevelopmentalStage getStage() const { return stage; }
    void setStage(DevelopmentalStage newStage) { stage = newStage; }
    
private:
    DevelopmentalStage stage;
};

// Checkpoint System
class CheckpointSystem : public SpecializedBrainSystem {
public:
    CheckpointSystem(SystemPriority priority = SystemPriority::Low);
    
    bool initialize(Brain* brain) override;
    void update(double timestep) override;
    void reset() override;
    std::string getName() const override { return "CheckpointSystem"; }
    bool isEnabled() const override { return true; }
    
    // Checkpoint operations
    void manageCheckpoints(SimulationStep step, Timestamp time);
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Statistics
    size_t getCheckpointCount() const;
};

// Brain Orchestrator - Coordinates all systems
class BrainOrchestrator : public StepProcessor {
public:
    BrainOrchestrator(std::shared_ptr<Config> config);
    ~BrainOrchestrator();
    
    // Initialize all systems
    bool initialize(Brain* brain);
    
    // Execute the main step loop
    void executeStep(SimulationStep step, Timestamp time) override;
    
    // Individual system access
    NeuronalSystem* getNeuronalSystem();
    MemorySystem* getMemorySystem();
    PredictionSystem* getPredictionSystem();
    NeuromodulationSystem* getNeuromodulationSystem();
    PlasticitySystem* getPlasticitySystem();
    DevelopmentSystem* getDevelopmentSystem();
    CheckpointSystem* getCheckpointSystem();
    
    // System management
    void addSystem(std::unique_ptr<SpecializedBrainSystem> system);
    void removeSystem(const std::string& name);
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getTotalSpikeCount() const;
    float getExcitationInhibitionRatio() const;
    
    // Configuration
    std::shared_ptr<const Config> getConfig() const { return config; }
    
    // System chaining for delayed operations
    void setSystemHandler(SystemHandler* handler);
    bool handleSystemUpdates(Brain* brain, SimulationStep step, Timestamp time);
    
private:
    std::shared_ptr<Config> config;
    Brain* brainRef = nullptr;
    
    // All brain systems
    std::vector<std::unique_ptr<SpecializedBrainSystem>> systems;
    
    // Specialized system accessors
    std::unique_ptr<NeuronalSystem> neuronalSystem;
    std::unique_ptr<MemorySystem> memorySystem;
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<NeuromodulationSystem> neuromodulationSystem;
    std::unique_ptr<PlasticitySystem> plasticitySystem;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    std::unique_ptr<CheckpointSystem> checkpointSystem;
    
    // Chain of responsibility for system updates
    SystemHandler* systemHandler = nullptr;
    
    // Update order based on priorities
    void organizeSystemsByPriority();
};

} // namespace nlm
