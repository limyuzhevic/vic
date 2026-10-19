#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <map>
#include <functional>
#include <atomic>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Brain;
class Config;
class VisualizationBackend;
class DataMonitor;

/**
 * @brief Real-time data monitor for brain state metrics
 */
class RealTimeMonitor {
public:
    struct BrainMetrics {
        // Neural metrics
        float avgFiringRate;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        size_t totalSpikeCount;
        float excitationInhibitionRatio;
        
        // Memory metrics
        float workingMemoryLoad;
        float episodicMemoryUtilization;
        size_t activeWorkingMemoryTraces;
        size_t storedEpisodicEpisodes;
        
        // Neuromodulation metrics
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        float predictionError;
        
        // Plasticity metrics
        float stdpWeightChange;
        float hebbianLearningRate;
        float structuralPlasticityRate;
        
        // Development metrics
        float developmentalProgress;
        float plasticityMaturation;
        
        // Prediction metrics
        float predictionAccuracy;
        float confidenceLevel;
        float planningHorizon;
        
        // Performance metrics
        float simulationStepTime;
        float memoryUsageMB;
        float cpuUsagePercent;
        size_t spikeEventsPerSecond;
        
        Timestamp timestamp;
    };
    
    using MetricsCallback = std::function<void(const BrainMetrics& metrics)>;
    using AlertCallback = std::function<void(const std::string& message, float severity)>;
    
    RealTimeMonitor();
    ~RealTimeMonitor();
    
    // Configuration
    bool initialize(std::shared_ptr<Config> config);
    void shutdown();
    
    // Monitoring control
    void startMonitoring(double updateIntervalHz = 30.0);
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Data collection
    void updateMetrics(std::shared_ptr<Brain> brain);
    void recordEvent(const std::string& eventType, const std::string& details = "");
    void addCustomMetric(const std::string& name, float value);
    
    // Callbacks
    void setMetricsCallback(MetricsCallback callback);
    void setAlertCallback(AlertCallback callback);
    
    // Alert system
    void triggerAlert(const std::string& message, float severity = 1.0f,
                     const std::string& category = "General");
    
    // Metrics queries
    BrainMetrics getLatestMetrics() const;
    std::vector<BrainMetrics> getMetricsHistory(size_t maxCount = 1000) const;
    std::vector<std::string> getActiveAlerts() const;
    
    // Statistical analysis
    float getMetricMean(const std::string& metricName) const;
    float getMetricStdDev(const std::string& metricName) const;
    float getMetricMax(const std::string& metricName) const;
    float getMetricMin(const std::string& metricName) const;
    
    // Event management
    void clearOldEvents(double keepSeconds = 300.0); // Keep last 5 minutes
    std::vector<std::pair<Timestamp, std::string>> getRecentEvents(size_t maxCount = 100) const;
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    void setPerformanceThresholds(float cpuThreshold = 90.0f,
                                   float memoryThreshold = 1024.0f,
                                   float spikeRateThreshold = 1000.0f);
    
    // Configuration
    void setUpdateInterval(double intervalHz);
    double getUpdateInterval() const;
    
    // Export capabilities
    bool exportMetrics(const std::string& filepath, const std::string& format = "csv");
    bool exportEvents(const std::string& filepath, const std::string& format = "json");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Comprehensive visualization system for neural simulations
 */
class VisualizationSystem {
public:
    enum class VisualizationType {
        NETWORK,
        ACTIVITY,
        MEMORY,
        NEUROMODULATION,
        PLASTICITY,
        DEVELOPMENT,
        PREDICTION,
        PLANNING,
        SOCIAL_LEARNING,
        CONTINUAL_LEARNING,
        PERFORMANCE,
        CUSTOM
    };
    
    enum class BackendType {
        OPENGL,
        VULKAN,
        SOFTWARE_RENDERER,
        PYTHON_PLOTTING,
        WEBGL,
        SVG,
        CUSTOM
    };
    
    using VisualizationCallback = std::function<void(std::shared_ptr<VisualizationBackend> backend)>;
    
    VisualizationSystem();
    ~VisualizationSystem();
    
    // Initialization
    bool initialize(std::shared_ptr<Config> config, BackendType backend = BackendType::SOFTWARE_RENDERER);
    void shutdown();
    
    // Control
    void startVisualization(double fps = 60.0);
    void stopVisualization();
    bool isRunning() const;
    
    // Data source
    void setBrainData(std::shared_ptr<Brain> brain);
    void setMonitor(std::shared_ptr<RealTimeMonitor> monitor);
    
    // Visualization management
    bool addVisualization(VisualizationType type, const std::string& name = "");
    bool removeVisualization(VisualizationType type);
    bool updateVisualization(VisualizationType type);
    
    // Backend management
    bool setBackend(BackendType backend);
    BackendType getBackendType() const;
    std::vector<std::string> getAvailableBackends() const;
    
    // Camera and view control
    void setCameraPosition(float x, float y, float z);
    void setCameraTarget(float x, float y, float z);
    void setZoom(float zoom);
    void rotateCamera(float yaw, float pitch);
    
    // Interaction
    void handleMouseClick(int x, int y, bool leftButton);
    void handleMouseMove(int x, int y);
    void handleScroll(int delta);
    
    // Export
    bool exportVisualization(const std::string& filepath, const std::string& format = "png");
    bool exportAnimation(const std::string& filepath, double duration, double fps = 30.0);
    
    // Configuration
    void setVisualizationScale(float scale);
    void setUpdateFrequency(double hz);
    void enableAntialiasing(bool enable);
    void setColorScheme(const std::string& scheme);
    
    // Callbacks
    void setVisualizationReadyCallback(VisualizationCallback callback);
    
    // Statistics
    size_t getTotalNeuronCount() const;
    float getRenderingFPS() const;
    size_t getMemoryUsage() const;
    
    // Custom visualization builders
    template<typename T>
    bool registerVisualizationBuilder(VisualizationType type, 
                                     std::function<std::shared_ptr<T>()> builder) {
        // Implementation would register builder for creating visualization components
        return true;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Neural network visualization with neuron spikes, synaptic weights, and connectivity
 */
class NeuralNetworkVisualizer {
public:
    struct NeuronVisualization {
        NeuronId id;
        float x, y, z;           // Position in 3D space
        float radius;            // Visual size based on firing rate
        float colorR, colorG, colorB; // Color based on neuron type
        float opacity;           // Based on activity level
        bool isActive;
        float recentActivity;    // Recent firing history
        float membranePotential; // Current membrane potential
    };
    
    struct SynapseVisualization {
        SynapseId id;
        NeuronId sourceId;
        NeuronId targetId;
        float weight;            // Visual thickness based on weight
        float colorR, colorG, colorB; // Color based on synapse type
        float opacity;
        Delay delay;             // Delay visualization
        bool isActive;
    };
    
    struct NetworkLayout {
        std::string algorithm;   // "force-directed", "radial", "grid", "custom"
        float repulsionStrength;
        float attractionStrength;
        float damping;
        float equilibriumDistance;
    };
    
    NeuralNetworkVisualizer();
    ~NeuralNetworkVisualizer();
    
    // Configuration
    void setLayout(const NetworkLayout& layout);
    void setMinDistance(float minDist);
    void setMaxDistance(float maxDist);
    void setShowSynapticWeights(bool show);
    void setShowConnectionStrength(bool show);
    void setColorByNeuronType(bool colorByType);
    void setColorByFiringRate(bool colorByFiring);
    void setShowSpines(bool show);
    
    // Data update
    void updateNeurons(const std::vector<NeuronVisualization>& neurons);
    void updateSynapses(const std::vector<SynapseVisualization>& synapses);
    void updateNetworkTopology(const std::shared_ptr<Brain>& brain);
    
    // Computation
    std::vector<float> computeForceDirectedLayout(const std::vector<NeuronVisualization>& neurons);
    std::vector<float> computeRadialLayout(const std::vector<NeuronVisualization>& neurons,
                                           RegionId centerRegion);
    std::vector<float> computeGridLayout(const std::vector<NeuronVisualization>& neurons,
                                         float gridSize);
    
    // Analysis
    float calculateNetworkDensity() const;
    float calculateClusteringCoefficient() const;
    std::vector<NeuronId> find communities() const;
    float calculateModularity() const;
    
    // Export
    bool exportLayout(const std::string& filepath);
    bool saveNetworkGraph(const std::string& filepath);
    
    // Callbacks
    using LayoutCompleteCallback = std::function<void(const std::vector<float>& positions)>;
    void setLayoutCompleteCallback(LayoutCompleteCallback callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Memory system visualization (working memory activation, episodic memory replay)
 */
class MemorySystemVisualizer {
public:
    struct WorkingMemoryViz {
        size_t traceId;
        float activationLevel;
        float importance;
        float decayRate;
        std::vector<float> contentPattern; // Pattern vector
        Timestamp creationTime;
        Timestamp lastAccessTime;
        float x, y, z;           // Position in memory space
        float radius;            // Size based on importance/activation
        float colorR, colorG, colorB; // Color based on content type
    };
    
    struct EpisodicMemoryViz {
        size_t episodeId;
        Timestamp startTime;
        Timestamp endTime;
        float replayStrength;
        std::vector<float> spatialPattern;
        std::vector<float> temporalPattern;
        float behavioralOutcome;
        std::string contextDescription;
        float x, y, z;           // Replay position
        float size;              // Based on significance
        float colorR, colorG, colorB; // Color based on emotional valence
    };
    
    struct MemoryTimeSeries {
        Timestamp timestamp;
        float workingMemoryLoad;
        float episodicMemoryUtilization;
        float memoryReplayRate;
        float consolidationProgress;
    };
    
    MemorySystemVisualizer();
    ~MemorySystemVisualizer();
    
    // Configuration
    void setShowWorkingMemoryTrails(bool show);
    void setShowEpisodicReplay(bool show);
    void setShowMemoryTimeSeries(bool show);
    void setMemoryTimeWindow(double timeWindowSeconds);
    void setReplayAnimationSpeed(float speed);
    
    // Data update
    void updateWorkingMemory(const std::vector<WorkingMemoryViz>& traces);
    void updateEpisodicMemory(const std::vector<EpisodicMemoryViz>& episodes);
    void updateMemoryTimeSeries(const std::vector<MemoryTimeSeries>& series);
    
    // Analysis
    float calculateMemoryCapacityUtilization() const;
    float calculateMemoryConsolidationRate() const;
    float calculateReplayEfficiency() const;
    std::vector<size_t> findMemoryClusters() const;
    
    // Simulation
    void simulateMemoryReplay(double timeStep);
    void animateMemoryConsolidation(double timeStep);
    
    // Export
    bool exportMemoryGraph(const std::string& filepath);
    bool exportTimeSeries(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Neuromodulation visualization (dopamine levels, curiosity signals)
 */
class NeuromodulationVisualizer {
public:
    struct NeuromodulatorSignal {
        std::string name;
        float level;             // Current level (0-1)
        float targetLevel;       // Target level
        float rateOfChange;      // Rate of increase/decrease
        float history[10];       // Last 10 samples
        float x, y, z;           // Position in neuromodulation space
        float radius;            // Size based on level
        float colorR, colorG, colorB; // Color based on neuromodulator type
        Timestamp timestamp;
    };
    
    struct ModulationEffect {
        std::string effect;      // "plasticity", "attention", "memory", "motivation"
        float magnitude;         // Effect strength
        float duration;          // Duration in seconds
        float startTime;         // When effect started
        float x, y, z;           // Location of effect
        float influenceRadius;   // Spatial influence
        float colorIntensity;
    };
    
    NeuromodulationVisualizer();
    ~NeuromodulationVisualizer();
    
    // Configuration
    void setShowModulationFields(bool show);
    void setShowModulationArrows(bool show);
    void setShowEffectZones(bool show);
    void setTimeScale(float scale); // For slow/fast neuromodulation dynamics
    
    // Data update
    void updateNeuromodulators(const std::vector<NeuromodulatorSignal>& signals);
    void updateModulationEffects(const std::vector<ModulationEffect>& effects);
    
    // Analysis
    float calculateModulationBalance() const;
    float calculateNoveltySurprise() const;
    std::vector<std::string> detectModulationPatterns() const;
    
    // Simulation
    void simulateModulationDynamics(double timeStep);
    void updateModulationFields();
    
    // Export
    bool exportModulationData(const std::string& filepath);
    bool exportSignalTimeSeries(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Plasticity visualization (STDP traces, Hebbian learning curves)
 */
class PlasticityVisualizer {
public:
    struct STDPTrace {
        Timestamp spikeTime;     // Time of spike
        float traceValue;        // Synaptic modification trace value
        float decayRate;         // Trace decay rate
        float x, y, z;           // Position in plasticity space
        float colorR, colorG, colorB; // Color based on LTP/LTD
        float opacity;
        bool isPositive;         // LTP vs LTD
    };
    
    struct HebbianLearningCurve {
        Timestamp time;          // Time point
        float learningRate;      // Current learning rate
        float weightChange;      // Weight modification
        float correlationStrength; // Pattern correlation
        float x, y, z;           // Position in learning space
        float size;              // Based on learning intensity
        float colorR, colorG, colorB; // Color based on learning phase
    };
    
    struct PlasticityRuleViz {
        std::string ruleName;    // "STDP", "Hebbian", "Reward-modulated"
        float effectiveness;      // How well rule is performing
        float adaptationRate;    // Rate of rule adaptation
        float x, y, z;           // Position in rule space
        float radius;            // Size based on effectiveness
        float colorR, colorG, colorB;
    };
    
    PlasticityVisualizer();
    ~PlasticityVisualizer();
    
    // Configuration
    void setShowSTDPTraces(bool show);
    void setShowHebbianCurves(bool show);
    void setShowPlasticityRules(bool show);
    void setLearningCurveTimeWindow(double timeWindow);
    void setTraceDecayVisualization(bool showDecay);
    
    // Data update
    void updateSTDPTraces(const std::vector<STDPTrace>& traces);
    void updateHebbianCurves(const std::vector<HebbianLearningCurve>& curves);
    void updatePlasticityRules(const std::vector<PlasticityRuleViz>& rules);
    
    // Analysis
    float calculatePlasticityBalance() const;
    float calculateLearningEfficiency() const;
    float calculateAdaptationSpeed() const;
    std::vector<std::string> detectPlasticityPatterns() const;
    
    // Simulation
    void simulateTraceDecay(double timeStep);
    void animateLearningCurves(double timeStep);
    
    // Export
    bool exportPlasticityData(const std::string& filepath);
    bool exportLearningHistory(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Development system visualization (developmental stages, maturation)
 */
class DevelopmentVisualizer {
public:
    struct DevelopmentalStage {
        DevelopmentalStage stage; // Current stage (Initial, CriticalPeriod, etc.)
        float progress;           // Progress through stage (0-1)
        float x, y, z;            // Position in developmental space
        float radius;             // Size based on maturity
        float colorR, colorG, colorB; // Color based on stage
        std::string description;  // Human-readable description
    };
    
    struct MaturationCurve {
        Timestamp time;           // Time point
        float neuralExcitability; // Overall neural excitability
        float plasticityRate;     // Current plasticity rate
        float myelination;        // Myelin development
        float synapseDensity;     // Synaptic density
        float x, y, z;            // Position in maturation space
        float size;               // Based on maturity level
        float colorR, colorG, colorB;
    };
    
    DevelopmentVisualizer();
    ~DevelopmentVisualizer();
    
    // Configuration
    void setShowDevelopmentalStages(bool show);
    void setShowMaturationCurves(bool show);
    void setDevelopmentTimeScale(float scale);
    void setMaturationAnimationSpeed(float speed);
    
    // Data update
    void updateDevelopmentalStages(const std::vector<DevelopmentalStage>& stages);
    void updateMaturationCurves(const std::vector<MaturationCurve>& curves);
    
    // Analysis
    float calculateDevelopmentalProgress() const;
    float calculateMaturationLevel() const;
    std::vector<std::string> detectDevelopmentalPatterns() const;
    
    // Simulation
    void simulateDevelopment(double timeStep);
    void animateMaturation(double timeStep);
    
    // Export
    bool exportDevelopmentData(const std::string& filepath);
    bool exportMaturationHistory(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Prediction system visualization (prediction errors, confidence intervals)
 */
class PredictionVisualizer {
public:
    struct PredictionError {
        Timestamp time;           // Time of prediction error
        float errorMagnitude;     // Size of prediction error
        float errorType;          // "sensory", "memory", "action"
        float confidence;         // Confidence in prediction
        float x, y, z;            // Position in prediction error space
        float radius;             // Size based on error magnitude
        float colorR, colorG, colorB; // Color based on error type
    };
    
    struct ConfidenceInterval {
        Timestamp time;           // Time point
        float lowerBound;         // Lower confidence bound
        float upperBound;         // Upper confidence bound
        float predictedValue;     // Predicted mean value
        float width;              // Interval width (uncertainty)
        float x, y, z;            // Position in confidence space
        float size;               // Based on uncertainty
        float colorR, colorG, colorB; // Color based on confidence level
    };
    
    struct PlanningTree {
        size_t nodeId;
        std::string action;       // Action being considered
        float value;              // Expected value
        float uncertainty;        // Uncertainty in value
        float x, y, z;            // 3D position in planning space
        float size;               // Based on expected value
        float colorR, colorG, colorB;
        std::vector<size_t> children; // Child nodes in decision tree
    };
    
    PredictionVisualizer();
    ~PredictionVisualizer();
    
    // Configuration
    void setShowPredictionErrors(bool show);
    void setShowConfidenceIntervals(bool show);
    void setShowPlanningTrees(bool show);
    void setPlanningTreeDepth(size_t depth);
    void setErrorTimeWindow(double timeWindow);
    
    // Data update
    void updatePredictionErrors(const std::vector<PredictionError>& errors);
    void updateConfidenceIntervals(const std::vector<ConfidenceInterval>& intervals);
    void updatePlanningTrees(const std::vector<PlanningTree>& trees);
    
    // Analysis
    float calculatePredictionAccuracy() const;
    float calculatePredictionUncertainty() const;
    float calculatePlanningEfficiency() const;
    std::vector<std::string> detectPredictionPatterns() const;
    
    // Simulation
    void simulatePredictionUpdates(double timeStep);
    void animatePlanningTrees(double timeStep);
    
    // Export
    bool exportPredictionData(const std::string& filepath);
    bool exportPlanningHistory(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Python integration APIs for real-time visualization and monitoring
 */
namespace python {

/**
 * @brief Python binding for RealTimeMonitor
 */
void bindRealTimeMonitor(py::module& m);

/**
 * @brief Python binding for VisualizationSystem
 */
void bindVisualizationSystem(py::module& m);

/**
 * @brief Python binding for NeuralNetworkVisualizer
 */
void bindNeuralNetworkVisualizer(py::module& m);

/**
 * @brief Python binding for MemorySystemVisualizer
 */
void bindMemorySystemVisualizer(py::module& m);

/**
 * @brief Python binding for NeuromodulationVisualizer
 */
void bindNeuromodulationVisualizer(py::module& m);

/**
 * @brief Python binding for PlasticityVisualizer
 */
void bindPlasticityVisualizer(py::module& m);

/**
 * @brief Python binding for DevelopmentVisualizer
 */
void bindDevelopmentVisualizer(py::module& m);

/**
 * @brief Python binding for PredictionVisualizer
 */
void bindPredictionVisualizer(py::module& m);

/**
 * @brief Python visualization utilities
 */
namespace utils {

/**
 * @brief Create a real-time plotting dashboard
 */
py::dict createDashboard(std::shared_ptr<RealTimeMonitor> monitor,
                         const std::vector<std::string>& metrics,
                         double updateInterval = 1.0);

/**
 * @brief Export visualization data to Python formats
 */
py::dict exportToPythonFormat(std::shared_ptr<VisualizationSystem> visualizer,
                              const std::string& format = "numpy");

/**
 * @brief Create interactive visualization with matplotlib
 */
py::object createMatplotlibVisualization(std::shared_ptr<Brain> brain,
                                         std::shared_ptr<RealTimeMonitor> monitor,
                                         const py::dict& options = py::dict());

/**
 * @brief Create web-based visualization interface
 */
std::string createWebInterface(std::shared_ptr<VisualizationSystem> visualizer,
                               const std::string& host = "localhost",
                               int port = 8080);

/**
 * @brief Performance profiling for visualization
 */
py::dict profileVisualization(std::shared_ptr<VisualizationSystem> visualizer,
                              double duration = 10.0,
                              const py::dict& options = py::dict());

} // namespace utils

} // namespace python

} // namespace nlm
