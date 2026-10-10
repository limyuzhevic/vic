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
    
    // Visualization APIs
    void plotActivity(const std::string& outputPath = "activity_plot.png");
    void visualizeConnectivity(const std::string& outputPath = "connectivity_matrix.png");
    void generateReport(const std::string& outputPath = "brain_report.txt");
    void createAnimation(const std::string& outputPath = "brain_animation.gif", 
                        double duration = 10.0, int fps = 30);
    
    // Enhanced Monitoring APIs
    void startMonitoring();
    bool isMonitoring() const;
    std::vector<MonitoringStats> getMonitoringStats() const;
    void setMonitoringThresholds(const MonitoringThresholds& thresholds);
    std::vector<EventLogEntry> getEventLog(size_t limit = 100) const;
    
    // Advanced Analysis APIs
    std::vector<SpikingPatternAnalysis> analyzeSpikingPatterns() const;
    std::vector<ConnectivityMetrics> calculateConnectivityMetrics() const;
    std::vector<ReplayEvent> detectReplayEvents() const;
    std::vector<PlasticityEffect> measurePlasticityEffects(size_t steps = 1000) const;
    
    // Export and Import APIs
    bool exportState(const std::string& filepath, ExportFormat format = ExportFormat::Binary);
    bool importState(const std::string& filepath, ImportFormat format = ImportFormat::Auto);
    std::vector<std::vector<double>> generateSessionData(size_t steps) const;
    std::vector<double> analyzeSessionData(const std::vector<std::vector<double>>& data) const;
    
    // Debug and Diagnostics APIs
    std::vector<NeuronDiagnostics> getNeuronDiagnostics(NeuronId neuronId = NeuronId()) const;
    std::vector<RegionDiagnostics> getRegionDiagnostics(RegionId regionId = RegionId()) const;
    void tracePlasticityEvents(size_t maxEvents = 100, PlasticityEventType type = PlasticityEventType::All);
    std::vector<std::string> identifyBottlenecks() const;
    
    // Configuration and Validation APIs
    ConfigValidationResult validateAll() const;
    std::string generateReport() const;
    std::vector<ConfigComparison> compareWithBaseline(const std::string& baselineConfig) const;
    std::vector<ConfigOptimization> suggestOptimizations() const;
    
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
    
    // ========== ENHANCED VISUALIZATION APIs =========
    
    // Visualization APIs
    void plotActivity(const std::string& outputPath = "activity_plot.png");
    void visualizeConnectivity(const std::string& outputPath = "connectivity_matrix.png");
    void generateReport(const std::string& outputPath = "brain_report.txt");
    void createAnimation(const std::string& outputPath = "brain_animation.gif", 
                        double duration = 10.0, int fps = 30);
    
    // ========== ENHANCED MONITORING APIs =========
    
    // Enhanced Monitoring APIs
    void startMonitoring();
    bool isMonitoring() const;
    std::vector<MonitoringStats> getMonitoringStats() const;
    void setMonitoringThresholds(const MonitoringThresholds& thresholds);
    std::vector<EventLogEntry> getEventLog(size_t limit = 100) const;
    
    // ========== ADVANCED ANALYSIS APIs =========
    
    // Advanced Analysis APIs
    std::vector<SpikingPatternAnalysis> analyzeSpikingPatterns() const;
    std::vector<ConnectivityMetrics> calculateConnectivityMetrics() const;
    std::vector<ReplayEvent> detectReplayEvents() const;
    std::vector<PlasticityEffect> measurePlasticityEffects(size_t steps = 1000) const;
    
    // ========== EXPORT AND IMPORT APIs =========
    
    // Export and Import APIs
    bool exportState(const std::string& filepath, ExportFormat format = ExportFormat::Binary);
    bool importState(const std::string& filepath, ImportFormat format = ImportFormat::Auto);
    std::vector<std::vector<double>> generateSessionData(size_t steps) const;
    std::vector<double> analyzeSessionData(const std::vector<std::vector<double>>& data) const;
    
    // ========== DEBUG AND DIAGNOSTICS APIs =========
    
    // Debug and Diagnostics APIs
    std::vector<NeuronDiagnostics> getNeuronDiagnostics(NeuronId neuronId = NeuronId()) const;
    std::vector<RegionDiagnostics> getRegionDiagnostics(RegionId regionId = RegionId()) const;
    void tracePlasticityEvents(size_t maxEvents = 100, PlasticityEventType type = PlasticityEventType::All);
    std::vector<std::string> identifyBottlenecks() const;
    
    // ========== CONFIGURATION AND VALIDATION APIs =========
    
    // Configuration and Validation APIs
    ConfigValidationResult validateAll() const;
    std::string generateReport() const;
    std::vector<ConfigComparison> compareWithBaseline(const std::string& baselineConfig) const;
    std::vector<ConfigOptimization> suggestOptimizations() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
