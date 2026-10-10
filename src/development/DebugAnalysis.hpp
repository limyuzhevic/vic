#include "../performance/PerformanceMonitor.hpp"

namespace nlm {

// Forward declarations
class Config;

/**
 * Comprehensive Debug Analysis System for NLM
 * Provides deep insights into neural system behavior, performance, and topology
 */
class DebugAnalysis {
public:
    // Output format enumeration
    enum class OutputFormat {
        CONSOLE,
        JSON,
        CSV,
        VISUALIZATION
    };

    // Analysis flags structure
    struct AnalysisFlags {
        // Core debugging tools
        bool neuronAnalysis = false;           // Enhanced Neuron Analysis
        bool synapseAnalysis = false;          // Enhanced Synapse Analysis
        bool networkAnalysis = false;          // Network Analysis
        bool activityAnalysis = false;         // Neural activity profiling
        bool performanceAnalysis = false;      // System performance profiling
        bool memoryAnalysis = false;           // Memory usage analysis
        bool statisticsAnalysis = false;       // Statistical analysis
        bool eventTracing = false;             // Enhanced event tracing
        bool stateComparison = false;          // Brain state comparison
        bool patternAnalysis = false;          // Pattern recognition analysis
        bool bottleneckAnalysis = false;       // Bottleneck identification
        
        // Neuron-specific flags
        bool neuronStateInspection = false;     // Detailed neuron state
        bool synapticInputOutput = false;       // Synaptic flow analysis
        bool membranePotentialTracing = false;  // Membrane potential over time
        bool refractoryPeriodAnalysis = false;  // Refractory period statistics
        bool adaptationVariableTracking = false; // Adaptation variable dynamics
        
        // Synapse-specific flags
        bool weightChangeTracking = false;      // Synaptic weight dynamics
        bool eligibilityTraceAnalysis = false;  // Eligibility trace analysis
        bool plasticityRuleApplication = false; // Plasticity rule analysis
        bool delayAnalysis = false;             // Synaptic delay analysis
        bool connectivityAnalysis = false;      // Connectivity mapping
        
        // Network analysis flags
        bool connectivityStats = false;        // Network connectivity statistics
        bool regionalAnalysis = false;         // Regional analysis
        bool populationStats = false;          // Population statistics
        bool pathLengthCalculations = false;    // Shortest path analysis
        bool hubIdentification = false;        // Hub identification
        
        // Performance profiling flags
        bool cpuProfiling = false;              // CPU usage profiling
        bool memoryProfiling = false;           // Memory usage tracking
        bool simulationSpeedAnalysis = false;   // Simulation speed analysis
        bool bottleneckIdentification = false;  // Bottleneck identification
        bool resourceUtilization = false;       // Resource utilization metrics
        
        // State analysis flags
        bool brainStateComparison = false;      // Brain state comparison
        bool checkpointValidation = false;      // Checkpoint validation
        bool replayIntegrityAnalysis = false;    // Replay integrity analysis
        bool stateEvolutionTracking = false;    // State evolution tracking
        
        // Statistical analysis flags
        bool spikeTrainStatistics = false;      // Spike train statistics
        bool firingRateAnalysis = false;        // Firing rate analysis
        bool correlationAnalysis = false;       // Correlation analysis
        bool patternRecognition = false;        // Pattern recognition
        bool anomalyDetection = false;          // Anomaly detection
        
        // Visualization integration flags
        bool enhancedVisualization = false;     // Enhanced visualization interface
        bool realTimeMonitoring = false;        // Real-time monitoring
        bool dataExport = false;                // Data export for external tools
        bool interactiveDebugging = false;      // Interactive debugging tools
        
        AnalysisFlags() = default;
        AnalysisFlags(const std::map<std::string, std::string>& args);
    };

    // Core analysis results structures
    struct NeuronAnalysisResult {
        NeuronId id;
        NeuronType type;
        double membranePotential;
        double restingPotential;
        double threshold;
        double resetPotential;
        double leakConductance;
        double synapticConductance;
        double firingRate;
        std::string firingState;
        uint32_t refractoryRemaining;
        uint32_t refractoryPeriod;
        double adaptationVariable;
        double lastSpikeTime;
        double avgInput;
        double maxInput;
        double stdInput;
        double meanInterSpikeInterval;
        double firingCoefficientOfVariation;
        std::vector<double> recentMembranePotentials;
    };

    struct SynapseAnalysisResult {
        SynapseId id;
        NeuronId sourceNeuron;
        NeuronId destinationNeuron;
        double weight;
        double weightHistory;
        Delay delay;
        SynapseType type;
        bool isExcitatory;
        double eligibilityTrace;
        double efficacy;
        double avgPreSpikeTime;
        double avgPostSpikeTime;
        double stdPreSpikeTime;
        double stdPostSpikeTime;
        double avgWeightChange;
        double maxWeightChange;
        std::vector<double> recentWeights;
        std::vector<double> recentPreSpikes;
        std::vector<double> recentPostSpikes;
    };

    struct NetworkAnalysisResult {
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        double connectivityDensity;
        double averageDegree;
        double clusteringCoefficient;
        double characteristicPathLength;
        double efficiency;
        std::map<RegionId, double> regionalConnectivity;
        std::map<PopulationId, double> populationConnectivity;
        std::vector<NeuronId> hubNeurons;
        std::vector<std::pair<NeuronId, NeuronId>> shortestPaths;
        std::map<std::string, double> topologicalMetrics;
    };

    struct ActivityAnalysisResult {
        std::vector<double> firingRates;
        std::vector<double> spikeCounts;
        double averageFiringRate;
        double maxFiringRate;
        double minFiringRate;
        double stdFiringRate;
        double totalSpikes;
        double spikeFrequency;
        std::vector<double> autocorrelation;
        std::vector<double> powerSpectrum;
        std::map<NeuronType, double> typeSpecificFiring;
        std::map<RegionId, double> regionalFiringRates;
    };

    struct PerformanceAnalysisResult {
        double cpuUsage;
        double memoryUsage;
        double memoryUtilization;
        double simulationSpeed;
        double stepsPerSecond;
        double simulationEfficiency;
        std::chrono::duration<double> totalSimulationTime;
        std::chrono::duration<double> analysisTime;
        std::map<std::string, double> resourceMetrics;
        std::vector<double> cpuHistory;
        std::vector<double> memoryHistory;
        std::vector<double> speedHistory;
    };

    struct StateAnalysisResult {
        double stateComparisonScore;
        std::vector<std::string> checkpointIssues;
        bool replayIntegrityValid;
        std::vector<std::string> replayIssues;
        std::vector<std::map<NeuronId, double>> stateEvolution;
        double stateStability;
        std::vector<double> driftMetrics;
        std::map<std::string, double> consistencyChecks;
    };

    struct StatisticalAnalysisResult {
        std::vector<double> spikeTrainStats;
        std::vector<double> firingRateStats;
        std::vector<double> correlationMatrix;
        std::vector<double> patternScores;
        std::vector<double> anomalyScores;
        std::map<std::string, double> summaryStatistics;
        std::vector<std::string> detectedPatterns;
        std::vector<std::string> detectedAnomalies;
    };

    // Main constructor
    DebugAnalysis(const std::shared_ptr<Brain>& brain,
                 const std::shared_ptr<Config>& config,
                 const AnalysisFlags& flags);

    // Core analysis methods
    void analyzeNeurons(std::vector<NeuronAnalysisResult>& results) const;
    void analyzeSynapses(std::vector<SynapseAnalysisResult>& results) const;
    void analyzeNetwork(NetworkAnalysisResult& result) const;
    void analyzeActivity(ActivityAnalysisResult& result) const;
    void analyzePerformance(PerformanceAnalysisResult& result) const;
    void analyzeState(StateAnalysisResult& result) const;
    void analyzeStatistics(StatisticalAnalysisResult& result) const;
    
    // Enhanced analysis methods
    void analyzeNeuronState(const Neuron* neuron, NeuronAnalysisResult& result) const;
    void analyzeSynapseState(const Synapse* synapse, SynapseAnalysisResult& result) const;
    void analyzeNetworkTopology(NetworkAnalysisResult& result) const;
    void profileActivity(ActivityAnalysisResult& result) const;
    void profilePerformance(PerformanceAnalysisResult& result) const;
    void analyzeMemoryUsage(std::map<std::string, double>& memoryMetrics) const;
    void analyzeStatisticalProperties(StatisticalAnalysisResult& result) const;
    void traceEvents(std::vector<std::string>& eventLog) const;
    
    // State comparison and tracking
    void compareWithState(const std::string& filepath, StateAnalysisResult& result) const;
    void trackStateEvolution(std::vector<std::map<NeuronId, double>>& evolution) const;
    
    // Pattern recognition and anomaly detection
    void performPatternAnalysis(std::vector<std::string>& patterns, StatisticalAnalysisResult& stats) const;
    void detectAnomalies(std::vector<std::string>& anomalies, StatisticalAnalysisResult& stats) const;
    
    // Output methods
    void outputToConsole(const NeuronAnalysisResult& result) const;
    void outputToJSON(const NeuronAnalysisResult& result, std::ostringstream& json) const;
    void outputToCSV(const NeuronAnalysisResult& result, std::ostringstream& csv) const;
    
    void outputToConsole(const SynapseAnalysisResult& result) const;
    void outputToJSON(const SynapseAnalysisResult& result, std::ostringstream& json) const;
    void outputToCSV(const SynapseAnalysisResult& result, std::ostringstream& csv) const;
    
    void outputToConsole(const NetworkAnalysisResult& result) const;
    void outputToJSON(const NetworkAnalysisResult& result, std::ostringstream& json) const;
    void outputToCSV(const NetworkAnalysisResult& result, std::ostringstream& csv) const;
    
    void outputToConsole(const ActivityAnalysisResult& result) const;
    void outputToJSON(const ActivityAnalysisResult& result, std::ostringstream& json) const;
    void outputToCSV(const ActivityAnalysisResult& result, std::ostringstream& csv) const;
    
    void outputToConsole(const PerformanceAnalysisResult& result) const;
    void outputToJSON(const PerformanceAnalysisResult& result, std::ostringstream& json) const;
    void outputToCSV(const PerformanceAnalysisResult& result, std::ostringstream& csv) const;
    
    // Visualization integration
    void generateVisualizationData(const std::string& filepath) const;
    void exportForExternalTools(const std::string& filepath, OutputFormat format) const;
    
    // Event tracing and monitoring
    void setupEventTracing(size_t duration) const;
    void traceEvent(const std::string& event) const;
    void stopEventTracing() const;
    
    // Performance monitoring integration
    bool enablePerformanceMonitoring = false;           // Enable comprehensive performance monitoring
    PerformanceMonitor::MonitorFlags performanceMonitorFlags;  // Performance monitoring configuration
    mutable bool performanceMonitorInitialized;         // Performance monitor initialization state
    
    // Performance monitoring integration
    PerformanceMonitor performanceMonitor;              // Integrated performance monitor
    
    // Performance monitoring integration
    bool enablePerformanceMonitoring = false;           // Enable comprehensive performance monitoring
    PerformanceMonitor::MonitorFlags performanceMonitorFlags;  // Performance monitoring configuration
    mutable bool performanceMonitorInitialized;         // Performance monitor initialization state
    
    // Performance monitoring integration
    PerformanceMonitor performanceMonitor;              // Integrated performance monitor
    
// Performance monitoring integration
    bool enablePerformanceMonitoring = false;           // Enable comprehensive performance monitoring
    PerformanceMonitor::MonitorFlags performanceMonitorFlags;  // Performance monitoring configuration
    mutable bool performanceMonitorInitialized;         // Performance monitor initialization state
    
    // Performance monitoring integration
    PerformanceMonitor performanceMonitor;              // Integrated performance monitor
    
    // Performance monitoring utilities
    void startPerformanceMonitoring() const;
    void stopPerformanceMonitoring(PerformanceAnalysisResult& result) const;
    double measureCpuUsage() const;
    double measureMemoryUsage() const;
    
    // Utility methods
    static std::string formatMembranePotential(double potential) const;
    static std::string formatWeight(double weight) const;
    static std::string formatTime(double time) const;
    static std::string formatRate(double rate) const;
    
private:
    // Internal analysis helpers
    void calculateNeuronStatistics(const Neuron* neuron, NeuronAnalysisResult& result) const;
    void calculateSynapseStatistics(const Synapse* synapse, SynapseAnalysisResult& result) const;
    void calculateNetworkStatistics(NetworkAnalysisResult& result) const;
    void calculateActivityStatistics(ActivityAnalysisResult& result) const;
    void calculatePerformanceMetrics(PerformanceAnalysisResult& result) const;
    void calculateStateMetrics(StateAnalysisResult& result) const;
    void calculateStatisticalMetrics(StatisticalAnalysisResult& result) const;
    
    void collectNeuronMembranePotentialHistory(const Neuron* neuron, std::vector<double>& history) const;
    void collectSynapseWeightHistory(const Synapse* synapse, std::vector<double>& history) const;
    void collectNetworkTopologicalMetrics(NetworkAnalysisResult& result) const;
    void collectActivityCorrelation(ActivityAnalysisResult& result) const;
    void collectPerformanceMetrics(PerformanceAnalysisResult& result) const;
    
    // Performance monitoring state
    mutable bool performanceMonitoringActive;
    mutable std::chrono::time_point<std::chrono::high_resolution_clock> performanceStartTime;
    mutable double initialCpuUsage;
    mutable double initialMemoryUsage;
    
    // Data collection buffers
    mutable std::vector<std::string> eventTrace;
    mutable std::vector<double> cpuUsageHistory;
    mutable std::vector<double> memoryUsageHistory;
    mutable std::vector<double> speedHistory;
    
    // Internal state
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    AnalysisFlags flags;
    
    // Helper classes
    class NeuronStateAnalyzer;
    class SynapseStateAnalyzer;
    class NetworkTopologyAnalyzer;
    class ActivityProfiler;
    class PerformanceProfiler;
    class MemoryAnalyzer;
    class StatisticalAnalyzer;
    class EventTracer;
};

} // namespace nlm
