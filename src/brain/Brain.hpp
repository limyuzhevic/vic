#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
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

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
    // ========== ADVANCED ANALYSIS TOOLS ==========
    
    // Analyze network topology
    std::string analyzeNetworkTopology() const;
    
    // Detect performance bottlenecks
    std::vector<std::string> detectBottlenecks() const;
    
    // Generate comprehensive summary report
    std::string generateSummaryReport() const;
    
    // ========== PERFORMANCE AND MEMORY MANAGEMENT ==========
    
    // Automatic memory optimization
    bool optimizeMemory();
    
    // Compact memory to reduce footprint
    bool compactStorage();
    
    // Get current memory usage
    std::map<std::string, size_t> getMemoryUsage() const;
    
    // ========== ADVANCED SIMULATION FEATURES ==========
    
    // Run full episode with exploration
    bool runEpisode(size_t numSteps, bool explore = true);
    
    // Run reinforcement learning policy
    std::vector<MotorCommand> runPolicy(const std::vector<SensoryInput>& sensoryInputs);
    
    // ========== DATA MANAGEMENT AND EXPORT ==========
    
    // Export to standard neural network formats
    bool exportToNeuralNet(const std::string& filepath, const std::string& format = "npy") const;
    
    // Import from other neural network formats
    bool importFromNeuralNet(const std::string& filepath);
    
    // ========== BATCH PROCESSING ==========
    
    // Run multiple simulation trials
    std::vector<std::string> runMultipleTrials(size_t numTrials);
    
    // Run parallel simulations
    std::vector<std::string> runParallelSimulations(const std::vector<std::shared_ptr<Config>>& configs, size_t numWorkers);
    
    // ========== ADVANCED MONITORING AND DEBUGGING ==========
    
    // Start performance profiling
    void startProfiling();
    
    // Stop profiling and return results
    std::map<std::string, double> stopProfiling();
    
    // Get all performance metrics
    std::map<std::string, double> getAllMetrics() const;
    
    // ========== ADVANCED LEARNING FEATURES ==========
    
    // Learn from external experience data
    bool learnFromExperience(const std::vector<float>& data);
    
    // Apply curriculum learning
    bool applyCurriculumLearning(size_t steps);
    
    // Import brain state from other runs
    bool importBrainState(const std::vector<float>& stateData);
    
    // ========== ERROR RECOVERY AND RESILIENCE ==========
    
    // Save checkpoint
    bool saveCheckpoint(const std::string& filepath = "brain_checkpoint.bin") const;
    
    // Load checkpoint
    bool loadCheckpoint(const std::string& filepath = "brain_checkpoint.bin");
    
    // Restore episode state from checkpoint
    bool restoreEpisodeState();
    
    // Validate brain integrity
    std::vector<std::string> validateIntegrity() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
