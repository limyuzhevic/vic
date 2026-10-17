// Visualization and Monitoring System for NLM
// Extensible system for monitoring neural activity, performance, and experiments

#ifndef NLM_VISUALIZATION_H
#define NLM_VISUALIZATION_H

#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <vector>
#include <chrono>
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <atomic>

namespace nlm {

// Forward declarations
class VisualizationManager;

// Performance monitoring data structures
struct PerformanceMetrics {
    std::chrono::system_clock::time_point timestamp;
    size_t totalSteps;
    size_t totalNeurons;
    size_t totalSynapses;
    size_t firingNeurons;
    size_t totalSpikeCount;
    float averageFiringRate;
    float excitationInhibitionRatio;
    size_t activeMemoryTraces;
    size_t episodicMemoryEpisodes;
    size_t interRegionConnections;
    size_t developmentalStage;
    
    PerformanceMetrics() : totalSteps(0), totalNeurons(0), totalSynapses(0), 
                          firingNeurons(0), totalSpikeCount(0), 
                          averageFiringRate(0.0f), excitationInhibitionRatio(0.0f),
                          activeMemoryTraces(0), episodicMemoryEpisodes(0),
                          interRegionConnections(0), developmentalStage(0) {}
};

// Neural activity visualization data
struct NeuralActivitySnapshot {
    std::chrono::system_clock::time_point timestamp;
    std::vector<float> neuronActivity;      // Activity levels for each neuron
    std::vector<float> synapticStrengths;    // Weight of each synapse
    std::vector<int> firingCounts;          // Recent firing counts
    std::vector<std::string> neuronTypes;   // Types of neurons
    std::vector<int> regionIds;            // Which region each neuron belongs to
    
    NeuralActivitySnapshot(size_t neuronCount) 
        : neuronActivity(neuronCount, 0.0f), 
          synapticStrengths(neuronCount, 0.0f),
          firingCounts(neuronCount, 0),
          regionIds(neuronCount, -1) {}
};

// Memory activity visualization
struct MemoryActivitySnapshot {
    std::chrono::system_clock::time_point timestamp;
    
    // Working memory
    std::vector<int> workingMemoryNeurons;     // Active working memory traces
    std::vector<float> workingMemoryActivations; // Activation levels
    
    // Episodic memory
    size_t episodicMemoryCapacity;
    size_t episodicMemoryCurrentSize;
    std::vector<float> episodicMemoryImportance; // Importance scores
    
    // Associative memory
    size_t associativeMemoryCapacity;
    size_t associativeMemoryCurrentSize;
    
    // Neuromodulation levels
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
};

// Development progress tracking
struct DevelopmentProgress {
    std::chrono::system_clock::time_point timestamp;
    DevelopmentalStage stage;
    float plasticityRate;
    float synaptogenesisProgress;
    float pruningProgress;
    float maturationProgress;
    float stabilityLevel;
    
    DevelopmentProgress() : stage(DevelopmentalStage::Initial), 
                           plasticityRate(1.0f), synaptogenesisProgress(0.0f),
                           pruningProgress(0.0f), maturationProgress(0.0f),
                           stabilityLevel(0.0f) {}
};

// Neural event data for visualization
struct NeuralEvent {
    enum EventType {
        Spike,
        SynapseCreation,
        SynapsePruning,
        PlasticityChange,
        MemoryStorage,
        MemoryRecall,
        DevelopmentTransition,
        NeuromodulationChange,
        RewardReceived,
        ActionExecuted
    };
    
    std::chrono::system_clock::time_point timestamp;
    EventType type;
    std::string description;
    std::unordered_map<std::string, std::string> metadata;
    
    NeuralEvent(EventType t, const std::string& desc) 
        : timestamp(std::chrono::system_clock::now()), type(t), description(desc) {}
};

// Main visualization and monitoring manager
class VisualizationManager {
private:
    std::shared_ptr<Logger> logger;
    
    // Data collection
    std::vector<PerformanceMetrics> performanceHistory;
    std::vector<NeuralActivitySnapshot> activityHistory;
    std::vector<MemoryActivitySnapshot> memoryHistory;
    std::vector<DevelopmentProgress> developmentHistory;
    std::vector<NeuralEvent> eventLog;
    
    // Sampling configuration
    size_t performanceSamplingInterval;    // Steps between performance samples
    size_t activitySamplingInterval;       // Steps between activity snapshots
    size_t eventSamplingInterval;          // Steps between event samples
    
    // File output
    std::ofstream activityLogFile;
    std::ofstream performanceLogFile;
    std::ofstream eventLogFile;
    
    // Statistics
    size_t totalStepsProcessed;
    std::chrono::system_clock::time_point startTime;
    std::atomic<size_t> activeConnections;
    
    // Export format
    enum ExportFormat { 
        JSON, CSV, BINARY, PLAIN_TEXT, PYTHON
    };
    
public:
    VisualizationManager(std::shared_ptr<Logger> logger)
        : logger(logger),
          performanceSamplingInterval(100),
          activitySamplingInterval(10),
          eventSamplingInterval(50),
          totalStepsProcessed(0) {
        startTime = std::chrono::system_clock::now();
        activeConnections = 0;
    }
    
    ~VisualizationManager() {
        closeOutputFiles();
    }
    
    // Configuration methods
    void setPerformanceSamplingInterval(size_t interval) {
        performanceSamplingInterval = interval;
    }
    
    void setActivitySamplingInterval(size_t interval) {
        activitySamplingInterval = interval;
    }
    
    void setEventSamplingInterval(size_t interval) {
        eventSamplingInterval = interval;
    }
    
    void enableFileLogging(const std::string& directory);
    void setExportFormat(ExportFormat format) { exportFormat = format; }
    
    // Data collection methods
    void collectPerformanceData(const Brain& brain);
    void collectNeuralActivity(const Brain& brain);
    void collectMemoryActivity(const Brain& brain);
    void collectDevelopmentProgress(const Brain& brain);
    
    // Event logging
    void logNeuralEvent(NeuralEvent::EventType type, const std::string& description);
    void logNeuralEvent(NeuralEvent::EventType type, const std::string& description, 
                       const std::unordered_map<std::string, std::string>& metadata);
    
    // Data access methods
    std::vector<PerformanceMetrics> getPerformanceHistory() const { return performanceHistory; }
    std::vector<NeuralActivitySnapshot> getActivityHistory() const { return activityHistory; }
    std::vector<MemoryActivitySnapshot> getMemoryHistory() const { return memoryHistory; }
    std::vector<NeuralEvent> getEventLog() const { return eventLog; }
    
    // Statistics and analysis
    double getAverageFiringRate() const;
    size_t getMaxFiringNeurons() const;
    float getAveragePlasticity() const;
    size_t getTotalEventsLogged() const { return eventLog.size(); }
    std::chrono::duration<double> getUptime() const;
    
    // Visualization methods
    void printActivitySummary() const;
    void printPerformanceSummary() const;
    void printMemorySummary() const;
    void printDevelopmentSummary() const;
    
    // Export methods
    void exportToCSV(const std::string& filepath) const;
    void exportToJSON(const std::string& filepath) const;
    void exportToPython(const std::string& filepath) const;
    
    // Visualization integration
    void integrateWithBrain(Brain& brain);
    
    // Configuration export
    void exportConfig(const std::string& filepath) const;
    
private:
    ExportFormat exportFormat = ExportFormat::CSV;
    
    void closeOutputFiles() {
        if (activityLogFile.is_open()) activityLogFile.close();
        if (performanceLogFile.is_open()) performanceLogFile.close();
        if (eventLogFile.is_open()) eventLogFile.close();
    }
    
    std::string formatTimestamp(std::chrono::system_clock::time_point tp) const;
    std::string toJsonString(const std::string& value) const;
};

// Neural activity visualization utilities
class NeuralVisualizer {
public:
    static void printActivityHeatmap(const NeuralActivitySnapshot& snapshot);
    static void printRegionActivity(const NeuralActivitySnapshot& snapshot);
    static void printSynapticStrengths(const NeuralActivitySnapshot& snapshot);
    static void printFiringPatterns(const NeuralActivitySnapshot& snapshot);
    static std::string generateActivityPlot(const NeuralActivitySnapshot& snapshot);
    static std::string generateRegionPlot(const NeuralActivitySnapshot& snapshot);
};

// Performance visualization utilities
class PerformanceVisualizer {
public:
    static void printPerformanceChart(const std::vector<PerformanceMetrics>& history);
    static void printMemoryUsage(const std::vector<PerformanceMetrics>& history);
    static void printSpikeRate(const std::vector<PerformanceMetrics>& history);
    static void printPlasticityOverTime(const std::vector<DevelopmentProgress>& history);
    static std::string generatePerformanceReport(const std::vector<PerformanceMetrics>& history);
    static std::string generateMemoryReport(const std::vector<PerformanceMetrics>& history);
};

// Experiment management utilities
class ExperimentManager {
public:
    struct Experiment {
        std::string name;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        std::unordered_map<std::string, std::string> parameters;
        std::shared_ptr<VisualizationManager> visualizer;
        
        Experiment(const std::string& name) : name(name), visualizer(nullptr) {
            startTime = std::chrono::system_clock::now();
        }
    };
    
    static std::shared_ptr<Experiment> createExperiment(const std::string& name, 
                                                         const std::string& configFile);
    static void runExperiment(std::shared_ptr<Experiment> experiment, Brain& brain);
    static void saveExperimentData(std::shared_ptr<Experiment> experiment, 
                                  const std::string& outputPath);
    static void restoreExperiment(const std::string& outputPath, Brain& brain);
};

// Main interface for visualization and monitoring
class NLMVisualizer {
private:
    std::shared_ptr<Logger> logger;
    std::shared_ptr<VisualizationManager> visualManager;
    std::shared_ptr<ExperimentManager> experimentManager;
    
public:
    NLMVisualizer(std::shared_ptr<Logger> logger) 
        : logger(logger) {
        visualManager = std::make_shared<VisualizationManager>(logger);
        experimentManager = std::make_shared<ExperimentManager>();
    }
    
    void initialize() {
        logger->info("Initializing NLM Visualization System...");
        logger->info("Visualization and monitoring system ready.");
    }
    
    void enableFileOutput(const std::string& directory) {
        visualManager->enableFileLogging(directory);
        logger->info("File logging enabled for directory: " + directory);
    }
    
    void integrateWithBrain(Brain& brain) {
        visualManager->integrateWithBrain(brain);
        logger->info("NLM Visualization system integrated with brain.");
    }
    
    void logNeuralActivity(const Brain& brain) {
        visualManager->collectPerformanceData(brain);
        visualManager->collectNeuralActivity(brain);
        visualManager->collectMemoryActivity(brain);
        visualManager->collectDevelopmentProgress(brain);
    }
    
    void logEvent(NeuralEvent::EventType type, const std::string& description) {
        visualManager->logNeuralEvent(type, description);
    }
    
    void printStatusReport() const {
        visualManager->printActivitySummary();
        visualManager->printPerformanceSummary();
        visualManager->printMemorySummary();
        visualManager->printDevelopmentSummary();
    }
    
    void exportData(const std::string& directory, const std::string& prefix = "") {
        std::string activityFile = directory + "/" + prefix + "activity.csv";
        std::string performanceFile = directory + "/" + prefix + "performance.csv";
        std::string eventFile = directory + "/" + prefix + "events.csv";
        
        visualManager->exportToCSV(activityFile);
        visualManager->exportToCSV(performanceFile);
        visualManager->exportToCSV(eventFile);
        
        logger->info("Data exported to " + directory);
    }
    
    // Experiment management methods
    std::shared_ptr<ExperimentManager::Experiment> createExperiment(const std::string& name, 
                                                                     const std::string& configFile) {
        return experimentManager->createExperiment(name, configFile);
    }
    
    void runExperiment(std::shared_ptr<ExperimentManager::Experiment> experiment, Brain& brain) {
        experimentManager->runExperiment(experiment, brain);
    }
    
    void saveExperiment(std::shared_ptr<ExperimentManager::Experiment> experiment, 
                       const std::string& outputPath) {
        experimentManager->saveExperimentData(experiment, outputPath);
    }
    
    void restoreExperiment(const std::string& outputPath, Brain& brain) {
        experimentManager->restoreExperiment(outputPath, brain);
    }
    
    // Configuration export
    void exportConfig(const std::string& filepath) const {
        visualManager->exportConfig(filepath);
    }
    
    std::string getStatus() const {
        std::stringstream ss;
        ss << "NLM Visualization System Status:" << std::endl;
        ss << "  - Active visualizers: 3" << std::endl;
        ss << "  - Performance sampling: Every " << visualManager->performanceSamplingInterval << " steps" << std::endl;
        ss << "  - Activity sampling: Every " << visualManager->activitySamplingInterval << " steps" << std::endl;
        ss << "  - Event sampling: Every " << visualManager->eventSamplingInterval << " steps" << std::endl;
        ss << "  - Total data points collected: " << visualManager->performanceHistory.size() << std::endl;
        ss << "  - Uptime: " << visualManager->getUptime().count() << " seconds" << std::endl;
        return ss.str();
    }
};

} // namespace nlm

#endif // NLM_VISUALIZATION_H
