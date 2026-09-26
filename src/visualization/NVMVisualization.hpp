#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <functional>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;
class Synapse;
class MemorySystem;
class NeuromodulationSystem;
class PlasticitySystem;
class DevelopmentSystem;

// Visualization data structures
struct NeuronData {
    NeuronId id;
    float x, y, z; // Position in 3D space
    float activityLevel; // Current firing rate (Hz)
    float membranePotential; // Current membrane potential (mV)
    float threshold; // Firing threshold (mV)
    float lastSpikeTime; // Timestamp of last spike
    std::vector<float> synapticWeights; // Incoming synaptic weights
    float colorR, colorG, colorB; // Visualization color
    bool isActive; // Current firing state
    bool isInRefractory; // Refractory period status
};

struct SynapseData {
    SynapseId id;
    NeuronId preNeuronId;
    NeuronId postNeuronId;
    float weight; // Synaptic strength (-1 to 1)
    float delay; // Transmission delay (ms)
    float lastUpdateTime; // Last weight update timestamp
    std::string type; // "excitatory", "inhibitory", "modulatory"
    float colorR, colorG, colorB; // Visualization color
    float activity; // Current activity level
};

struct NetworkTopology {
    std::vector<NeuronData> neurons;
    std::vector<SynapseData> synapses;
    std::map<RegionId, std::vector<NeuronId>> regions; // Neurons by region
    std::map<PopulationId, std::vector<NeuronId>> populations; // Neurons by population
};

struct MemoryTrace {
    MemoryId id;
    std::vector<float> activationPattern; // Pattern of active neurons
    float strength; // Memory strength (0-1)
    float age; // Age in time steps
    float decayRate; // Decay rate
    Timestamp creationTime; // When memory was formed
    Timestamp lastAccessTime; // Last time accessed
    std::string type; // "working", "episodic", "semantic"
    float consolidationProgress; // 0-1 for consolidation
};

struct NeuromodulationData {
    std::string type; // "dopamine", "acetylcholine", "serotonin", etc.
    float level; // Current level (normalized)
    float baseline; // Baseline level
    float targetLevel; // Target level
    float learningRate; // Learning rate modulation
    float noveltySignal; // Novelty detection signal
    float predictionError; // Prediction error signal
    float curiosity; // Curiosity level (0-1)
    std::vector<float> timeSeries; // Time series data
    std::vector<Timestamp> timePoints; // Corresponding timestamps
};

struct PlasticityData {
    std::string type; // "hebbian", "stdp", "reward_modulated", etc.
    float learningRate; // Current learning rate
    float eligibilityTrace; // Eligibility trace value
    float rewardPrediction; // Reward prediction
    float valueFunction; // Value function estimate
    std::vector<float> weights; // Synaptic weights
    std::vector<float> eligibilityTraces; // For each synapse
    std::vector<float> rewardSignals; // Reward signals over time
};

struct DevelopmentData {
    int stage; // Current developmental stage (0-100)
    float maturationProgress; // Overall maturation progress (0-1)
    std::vector<float> structuralPlasticity; // Synaptogenesis and pruning
    std::vector<float> experienceDependence; // Experience-dependent changes
    std::vector<float> connectivityMaturation; // Connectivity maturation
    std::vector<float> neuronMaturation; // Neuron maturation
    std::vector<Timestamp> experienceTimestamps; // Experience timestamps
    std::vector<std::string> experiences; // Recorded experiences
};

struct PerformanceMetrics {
    double cpuUsage; // CPU utilization percentage
    double memoryUsage; // Memory utilization (MB)
    double networkUsage; // Network bandwidth usage
    double renderingFrameTime; // Time per frame (ms)
    double updateTime; // Time per update (ms)
    double renderTime; // Time per render (ms)
    double fps; // Frames per second
    double dataProcessingTime; // Data processing time (ms)
    std::map<std::string, double> customMetrics; // Custom metrics
};

struct LearningCurve {
    std::vector<Timestamp> timePoints;
    std::vector<float> performanceValues; // Performance metric values
    std::vector<float> explorationValues; // Exploration levels
    std::vector<float> exploitationValues; // Exploitation levels
    std::vector<float> curiosityValues; // Curiosity levels
    std::string metricName; // Name of the metric being tracked
};

struct NetworkActivityPattern {
    std::vector<float> firingRates; // Firing rates of neurons
    std::vector<float> phases; // Phase information for oscillators
    std::vector<float> coherence; // Network coherence values
    std::vector<std::pair<NeuronId, NeuronId>> connections; // Functional connections
    std::string patternType; // Type of activity pattern
    float synchrony; // Degree of synchrony (0-1)
    float informationFlow; // Information flow strength
};

struct SystemIntegrationData {
    PerformanceMetrics performance; // System performance metrics
    std::vector<LearningCurve> learningCurves; // Learning progress over time
    std::vector<NetworkActivityPattern> activityPatterns; // Network activity patterns
    std::map<std::string, float> crossSystemCorrelations; // Correlations between systems
    std::vector<std::pair<Timestamp, std::string>> checkpointData; // Checkpoint information
    std::vector<std::pair<Timestamp, double>> resourceUtilization; // Resource utilization over time
};

// Base visualization interface
class VisualizationComponent {
public:
    virtual ~VisualizationComponent() = default;
    
    // Initialize the visualization component
    virtual bool initialize() = 0;
    
    // Update visualization data
    virtual void update(double deltaTime) = 0;
    
    // Render the visualization
    virtual void render() = 0;
    
    // Handle input events
    virtual void handleInput(const std::string& eventType, const std::map<std::string, float>& parameters) = 0;
    
    // Check if component is active
    virtual bool isActive() const = 0;
    
    // Set component visibility
    virtual void setVisible(bool visible) = 0;
    
    // Get component name
    virtual std::string getName() const = 0;
    
    // Get component position and size
    virtual void getBounds(float& x, float& y, float& width, float& height) const = 0;
    
    // Set component position and size
    virtual void setBounds(float x, float y, float width, float height) = 0;
};

// Core visualization framework
class NVMVisualization : public VisualizationComponent {
public:
    NVMVisualization();
    ~NVMVisualization() override;
    
    // Initialize with brain data
    bool initialize(const Brain& brain) override;
    
    // Update all visualizations
    void update(double deltaTime) override;
    
    // Render all visualizations
    void render() override;
    
    // Handle input events
    void handleInput(const std::string& eventType, const std::map<std::string, float>& parameters) override;
    
    // Check if visualization is active
    bool isActive() const override;
    
    // Set visibility
    void setVisible(bool visible) override;
    
    // Get component name
    std::string getName() const override;
    
    // Get component bounds
    void getBounds(float& x, float& y, float& width, float& height) const override;
    
    // Set component bounds
    void setBounds(float x, float y, float width, float height) override;
    
    // Add a visualization module
    void addVisualizationModule(std::unique_ptr<VisualizationComponent> module);
    
    // Remove a visualization module
    void removeVisualizationModule(const std::string& name);
    
    // Get a visualization module
    VisualizationComponent* getVisualizationModule(const std::string& name);
    
    // Update visualization data from brain
    void updateVisualizationData(const Brain& brain);
    
    // Export visualization data
    bool exportData(const std::string& filename, const std::string& format = "json");
    
    // Import visualization data
    bool importData(const std::string& filename, const std::string& format = "json");
    
    // Get current visualization data
    const NetworkTopology& getNetworkTopology() const;
    const std::vector<MemoryTrace>& getMemoryTraces() const;
    const std::vector<NeuromodulationData>& getNeuromodulationData() const;
    const std::vector<PlasticityData>& getPlasticityData() const;
    const std::vector<DevelopmentData>& getDevelopmentData() const;
    const SystemIntegrationData& getSystemIntegrationData() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    
    // Get update rate
    double getUpdateRate() const;
    
    // Enable/disable real-time updates
    void setRealTimeUpdates(bool enabled);
    
    // Check if real-time updates are enabled
    bool getRealTimeUpdates() const;
    
    // Add a data source
    void addDataSource(const std::string& name, const std::function<void(double)>& callback);
    
    // Remove a data source
    void removeDataSource(const std::string& name);
    
    // Set visualization theme
    void setTheme(const std::string& theme);
    
    // Get current theme
    std::string getTheme() const;
    
    // Enable/disable animations
    void setAnimationsEnabled(bool enabled);
    
    // Check if animations are enabled
    bool getAnimationsEnabled() const;
    
    // Set visualization quality
    void setQuality(const std::string& quality);
    
    // Get current quality setting
    std::string getQuality() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm