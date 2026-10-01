#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../performance/PerformanceProfiler.hpp"
#include <memory>
#include <string>
#include <chrono>

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

    // Visualization methods
    bool generateVisualization(const std::string& filename, const std::string& format = "png");
    bool exportBrainData(const std::string& filename, const std::string& format = "json");
    bool exportConnectivityMatrix(const std::string& filename, const std::string& format = "csv");
    bool exportTimeSeriesData(const std::string& filename, const std::string& format = "csv");
    void updateVisualization(double deltaTime);
    void renderVisualization();
    bool isVisualizationEnabled() const;
    void enableVisualization(bool enabled);

    // Visualization statistics
    struct VisualizationStats {
        size_t frameCount;
        double fps;
        double updateTime;
        double renderTime;
        double totalUpdateTime;
        double totalRenderTime;
        double lastUpdateTime;
        
        VisualizationStats()
            : frameCount(0), fps(0.0), updateTime(0.0), renderTime(0.0),
              totalUpdateTime(0.0), totalRenderTime(0.0), lastUpdateTime(0.0) {}
    };
    
    VisualizationStats getVisualizationStats() const;

    // Get available visualization types
    std::vector<std::string> getAvailableVisualizationTypes() const;
    std::vector<std::string> getAvailableExportFormats() const;

    // Set visualization preferences
    void setVisualizationPreference(const std::string& key, const std::string& value);
    std::string getVisualizationPreference(const std::string& key) const;

    // ========== PERFORMANCE MEASUREMENT ==========

    // Enable/disable performance profiling
    bool enablePerformanceProfiling(const std::string& sectionName = "brain_step");
    void disablePerformanceProfiling(const std::string& sectionName = "brain_step");
    void togglePerformanceProfiling(const std::string& sectionName = "brain_step");

    // Get performance statistics
    double getPerformanceTime(const std::string& sectionName = "brain_step") const;
    uint64_t getMemoryUsage() const;
    double getCPUUsage() const;
    double getFPS() const;

    // Profile specific brain operations
    void profileBrainStep(const std::string& operation = "general");
    void profileMemoryOperations();
    void profileNeuralActivity();
    void profilePlasticityUpdates();
    void profileMemoryUpdates();
    void profilePredictionSystem();
    void profileCognitionSystems();
    void profileNeuromodulation();

    // Get performance profile data
    std::vector<std::string> getActiveProfileSections() const;
    std::vector<std::string> getBottleneckSections(double threshold = 0.2) const;
    std::string getPerformanceSummary() const;

    // Performance configuration
    void configurePerformanceProfiling(std::chrono::milliseconds interval = std::chrono::milliseconds(100));
    void enableDetailedTiming(bool enable);
    void enableMemoryTracking(bool enable);
    void enableCPUTracking(bool enable);
    void enableIOTracking(bool enable);
    void enableGPUTracking(bool enable);

    // Performance validation
    bool validatePerformanceRequirements(const std::map<std::string, double>& requirements) const;
    std::string getPerformanceGrade() const;

    // Export performance data
    bool exportPerformanceData(const std::string& filename, const std::string& format = "json") const;
    bool exportBenchmarkResults(const std::string& filename, const std::string& format = "csv") const;

    // Performance analysis
    std::string analyzePerformanceTrends() const;
    std::vector<std::string> getPerformanceRecommendations() const;

    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
