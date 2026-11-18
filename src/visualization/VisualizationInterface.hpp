// Enhanced Visualization Interface
// Provides comprehensive visualization and debugging tools for NLM

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <atomic>
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"

namespace nlm {

// Visualization modes
enum class VisualizationMode {
    Text,           // Text-based visualization
    Simple,         // Simple graphical visualization
    Advanced,       // Advanced with detailed metrics
    Network,        // Network graph visualization
    TimeSeries,     // Time series plots
    3D,             // 3D visualization
    Custom         // Custom visualization
};

// Debug information types
enum class DebugInfoType {
    Basic,          // Basic neuron and synapse information
    Plasticity,     // Plasticity rule information
    Memory,         // Memory system state
    Neuromodulation,// Neuromodulation signals
    Development,    // Developmental stage information
    Performance,    // Performance metrics
    Custom         // Custom debug information
};

// Visualization data structure
struct VisualizationData {
    double timestamp;                    // Simulation time
    std::vector<float> neuronStates;      // Neuron membrane potentials
    std::vector<int> neuronFiring;        // Neuron firing states
    std::vector<float> synapticWeights;   // Synaptic weights
    std::vector<int> neuronTypes;         // Neuron types (excitatory, inhibitory, etc.)
    std::vector<float> membranePotentials; // Membrane potentials
    std::vector<float> firingRates;       // Firing rates
    std::vector<float> adaptationVariables; // Adaptation variables
    
    // Memory system data
    float workingMemoryLoad;              // Working memory utilization
    std::vector<float> episodicMemoryTraces; // Episodic memory traces
    float associativeMemoryStrength;     // Associative memory strength
    
    // Neuromodulation data
    float dopamineLevel;                  // Dopamine concentration
    float curiosityLevel;                 // Curiosity level
    float noveltyLevel;                   // Novelty detection level
    float predictionError;                // Prediction error signal
    
    // Development data
    float developmentalAge;              // Current developmental age
    float plasticityModifier;             // Plasticity modifier
    float developmentalStage;            // Current developmental stage
    
    // Performance data
    size_t totalSpikeCount;               // Total spike count
    size_t firingNeuronCount;             // Number of firing neurons
    float averageFiringRate;              // Average firing rate
    float excitationInhibitionRatio;      // E/I balance
    
    // Planning and cognition data
    float planningConfidence;             // Planning confidence
    std::vector<float> conceptActivations; // Concept activation levels
    float attentionFocus;                 // Attention focus strength
    
    // Environment data
    std::vector<float> sensoryInput;      // Sensory input values
    std::vector<float> motorOutput;       // Motor output values
    float rewardSignal;                   // Reward signal
    
    VisualizationData() : timestamp(0.0), workingMemoryLoad(0.0), 
                         episodicMemoryStrength(0.0), dopamineLevel(0.0),
                         curiosityLevel(0.0), noveltyLevel(0.0), predictionError(0.0),
                         developmentalAge(0.0), plasticityModifier(0.0), developmentalStage(0.0),
                         totalSpikeCount(0), firingNeuronCount(0), averageFiringRate(0.0),
                         excitationInhibitionRatio(1.0), planningConfidence(0.0),
                         attentionFocus(0.0), rewardSignal(0.0) {}
};

// Visualization event types
enum class VisualizationEvent {
    NeuronSpike,           // Neuron spike event
    SynapseActivation,      // Synapse activation event
    PlasticityChange,       // Plasticity rule change
    MemoryUpdate,          // Memory system update
    NeuromodulationChange,  // Neuromodulation signal change
    DevelopmentChange,      // Developmental stage change
    RewardReceived,         // Reward received
    ActionExecuted,         // Action executed
    CustomEvent            // Custom event
};

// Visualization event handler
using VisualizationEventHandler = std::function<void(VisualizationEvent, const VisualizationData&)>;

// Main visualization interface
class VisualizationInterface {
public:
    VisualizationInterface();
    virtual ~VisualizationInterface();
    
    // Configure visualization
    void configure(VisualizationMode mode, const std::string& configFile = "");
    
    // Set visualization options
    void setOption(const std::string& option, const std::string& value);
    std::string getOption(const std::string& option) const;
    
    // Add event handler
    void addEventHandler(VisualizationEvent event, VisualizationEventHandler handler);
    
    // Remove event handler
    void removeEventHandler(VisualizationEvent event);
    
    // Clear all event handlers
    void clearEventHandlers();
    
    // Update visualization with current simulation data
    void update(const VisualizationData& data);
    
    // Get current visualization data
    VisualizationData getCurrentData() const;
    
    // Start/stop visualization
    void start();
    void stop();
    bool isRunning() const;
    
    // Save visualization data
    bool saveVisualizationData(const std::string& filepath) const;
    
    // Load visualization data
    bool loadVisualizationData(const std::string& filepath);
    
    // Generate report
    std::string generateReport() const;
    
    // Export data for external tools
    bool exportData(const std::string& format, const std::string& filepath) const;
    
    // Interactive control
    virtual void handleInput(const std::string& input);
    virtual bool hasInteractiveMode() const;
    
    // Debug mode
    void enableDebugMode(bool enable);
    bool isDebugModeEnabled() const;
    
    // Event filtering
    void addEventFilter(VisualizationEvent event, std::function<bool(const VisualizationData&)> filter);
    void removeEventFilter(VisualizationEvent event);
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    std::string getPerformanceStats() const;
    
    // Data recording
    void startRecording(const std::string& filepath);
    void stopRecording();
    bool isRecording() const;
    
    // Replay functionality
    bool loadReplayData(const std::string& filepath);
    void startReplay(double speed = 1.0);
    void stopReplay();
    bool isReplaying() const;
    double getReplayTime() const;
    
    // Analysis tools
    std::string analyzePattern(const std::string& patternType) const;
    std::vector<std::string> detectAnomalies() const;
    std::string correlateWithBehavior(const std::string& behaviorMetric) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    void emitEvent(VisualizationEvent event, const VisualizationData& data);
    bool shouldFilterEvent(VisualizationEvent event, const VisualizationData& data) const;
};

// Specialized visualization classes

// Text-based visualization for terminal output
class TextVisualization : public VisualizationInterface {
public:
    TextVisualization();
    ~TextVisualization() override;
    
    void configure(VisualizationMode mode, const std::string& configFile = "") override;
    void update(const VisualizationData& data) override;
    void handleInput(const std::string& input) override;
    bool hasInteractiveMode() const override;
    
    // Text-specific options
    void setShowNeuronDetails(bool show);
    void setShowPlasticity(bool show);
    void setShowMemory(bool show);
    void setShowNeuromodulation(bool show);
    void setShowPerformance(bool show);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Simple graphical visualization using ASCII art
class SimpleVisualization : public VisualizationInterface {
public:
    SimpleVisualization();
    ~SimpleVisualization() override;
    
    void configure(VisualizationMode mode, const std::string& configFile = "") override;
    void update(const VisualizationData& data) override;
    void handleInput(const std::string& input) override;
    bool hasInteractiveMode() const override;
    
    // Simple visualization options
    void setShowBrainOutline(bool show);
    void setShowSynapses(bool show);
    void setShowNeurons(bool show);
    void setShowLabels(bool show);
    
    // Neural network visualization
    void visualizeNeuralNetwork(const VisualizationData& data);
    void visualizeSpikePropagation(const VisualizationData& data);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced visualization with detailed metrics
class AdvancedVisualization : public VisualizationInterface {
public:
    AdvancedVisualization();
    ~AdvancedVisualization() override;
    
    void configure(VisualizationMode mode, const std::string& configFile = "") override;
    void update(const VisualizationData& data) override;
    void handleInput(const std::string& input) override;
    bool hasInteractiveMode() const override;
    
    // Advanced features
    void enableHeatmap(bool enable);
    void enableTimeSeries(bool enable);
    void enableStatisticalAnalysis(bool enable);
    void enablePatternDetection(bool enable);
    
    // Detailed metrics
    void visualizeNeuronDynamics(const VisualizationData& data);
    void visualizePlasticityDynamics(const VisualizationData& data);
    void visualizeMemoryDynamics(const VisualizationData& data);
    void visualizeNeuromodulationDynamics(const VisualizationData& data);
    
    // Statistical plots
    void generateSpikeHistogram() const;
    void generateFiringRatePlot() const;
    void generateSynapticWeightDistribution() const;
    void generateMemoryUsageChart() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Network visualization for neural connectivity
class NetworkVisualization : public VisualizationInterface {
public:
    NetworkVisualization();
    ~NetworkVisualization() override;
    
    void configure(VisualizationMode mode, const std::string& configFile = "") override;
    void update(const VisualizationData& data) override;
    void handleInput(const std::string& input) override;
    bool hasInteractiveMode() const override;
    
    // Network-specific options
    void setShowConnections(bool show);
    void setShowNeuronTypes(bool show);
    void setShowSynapseWeights(bool show);
    void setShowPathways(bool show);
    
    // Network analysis
    void visualizeNetworkTopology(const VisualizationData& data);
    void visualizeInformationFlow(const VisualizationData& data);
    void visualizeCommunityStructure(const VisualizationData& data);
    
    // Interaction
    void highlightPathway(const std::string& pathway);
    void filterByNeuronType(NeuronType type);
    void highlightActiveNeurons(const std::vector<size_t>& neuronIndices);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Time series visualization
class TimeSeriesVisualization : public VisualizationInterface {
public:
    TimeSeriesVisualization();
    ~TimeSeriesVisualization() override;
    
    void configure(VisualizationMode mode, const std::string& configFile = "") override;
    void update(const VisualizationData& data) override;
    void handleInput(const std::string& input) override;
    bool hasInteractiveMode() const override;
    
    // Time series options
    void addTimeSeries(const std::string& name, std::vector<float> data);
    void removeTimeSeries(const std::string& name);
    void clearTimeSeries();
    
    // Time series visualization
    void plotTimeSeries(const std::string& name, const std::vector<float>& data) const;
    void plotMultipleTimeSeries(const std::map<std::string, std::vector<float>>& data) const;
    void plotCorrelationMatrix(const std::map<std::string, std::vector<float>>& data) const;
    
    // Real-time updates
    void enableRealTimeUpdates(bool enable);
    void setUpdateInterval(double interval);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Debug visualization for development and troubleshooting
class DebugVisualization : public VisualizationInterface {
public:
    DebugVisualization();
    ~DebugVisualization() override;
    
    void configure(VisualizationMode mode, const std::string& configFile = "") override;
    void update(const VisualizationData& data) override;
    void handleInput(const std::string& input) override;
    bool hasInteractiveMode() const override;
    
    // Debug features
    void enableMemoryDebugging(bool enable);
    void enablePlasticityDebugging(bool enable);
    void enableNeuromodulationDebugging(bool enable);
    void enablePerformanceDebugging(bool enable);
    void enableNetworkDebugging(bool enable);
    
    // Debug tools
    void visualizeMemoryAllocation(const VisualizationData& data);
    void visualizePlasticityRules(const VisualizationData& data);
    void visualizeNeuromodulationSignals(const VisualizationData& data);
    void visualizePerformanceCounters(const VisualizationData& data);
    void visualizeNetworkConnectivity(const VisualizationData& data);
    
    // Error detection
    void detectMemoryLeaks() const;
    void detectPerformanceBottlenecks() const;
    void detectPlasticityInstabilities() const;
    void detectNeuromodulationAnomalies() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Factory function for creating visualization instances
std::unique_ptr<VisualizationInterface> createVisualization(VisualizationMode mode);

// Helper functions for visualization data extraction
VisualizationData extractVisualizationData(std::shared_ptr<Brain> brain,
                                             std::shared_ptr<AgentBrain> agent,
                                             std::shared_ptr<SimpleWorld> world,
                                             double timestamp);

// Data processors for visualization
class VisualizationDataProcessor {
public:
    VisualizationDataProcessor();
    ~VisualizationDataProcessor();
    
    // Data extraction
    static VisualizationData extractFromBrain(std::shared_ptr<Brain> brain);
    static VisualizationData extractFromAgent(std::shared_ptr<AgentBrain> agent);
    static VisualizationData extractFromWorld(std::shared_ptr<SimpleWorld> world);
    static VisualizationData extractFromCompleteState(std::shared_ptr<Brain> brain,
                                                      std::shared_ptr<AgentBrain> agent,
                                                      std::shared_ptr<SimpleWorld> world);
    
    // Data processing
    static VisualizationData normalizeData(const VisualizationData& data);
    static VisualizationData filterData(const VisualizationData& data,
                                        const std::vector<std::string>& filters);
    static VisualizationData transformData(const VisualizationData& data,
                                           const std::string& transformation);
    
    // Analysis
    static std::vector<std::string> analyzePattern(const VisualizationData& data,
                                                  const std::string& patternType);
    static std::vector<std::string> detectAnomalies(const VisualizationData& data);
    static std::string correlateWithBehavior(const VisualizationData& data,
                                            const std::string& behaviorMetric);
    
    // Configuration
    static void configureProcessing(const std::string& configFile);
    static std::string getProcessingConfig() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
