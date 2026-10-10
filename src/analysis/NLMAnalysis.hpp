// NLM Analysis and Export Headers
#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>

namespace nlm {

/**
 * Experiment result types
 */
typedef std::variant<
    Phase3ExperimentResult,
    Phase4ExperimentResult,
    Phase5ExperimentResult,
    Phase6IntegrationResult
> ExperimentResult;

/**
 * Analysis result types
 */
struct AnalysisResult {
    bool success;
    double performanceScore;
    double efficiencyScore;
    double learningScore;
    
    // Common data
    double totalReward;
    double avgFiringRate;
    double memoryEpisodes;
    double duration;
    size_t neuronCount;
    size_t regionCount;
    std::string developmentStage;
    double dopamineLevel;
    double noveltyLevel;
    double curiosityLevel;
};

/**
 * Performance metrics
 */
struct PerformanceMetrics {
    double avgFiringRate;
    size_t totalSpikes;
    double duration;
};

/**
 * Performance analysis result
 */
struct PerformanceAnalysisResult {
    double firingRate;
    size_t spikeCount;
    double efficiency;
    double throughput;
    double stability;
    double optimization;
};

/**
 * Memory metrics
 */
struct MemoryMetrics {
    bool workingMemoryActive;
    size_t workingMemoryCapacity;
    double workingMemoryLoad;
    size_t episodicMemoryEpisodes;
    size_t episodicMemoryCapacity;
    size_t semanticMemoryNodes;
    size_t proceduralMemoryTrails;
    size_t associativeMemoryConnections;
};

/**
 * Memory analysis result
 */
struct MemoryAnalysisResult {
    bool workingMemoryActive;
    size_t workingMemoryCapacity;
    double workingMemoryLoad;
    double workingMemoryScore;
    size_t episodicMemoryEpisodes;
    size_t episodicMemoryCapacity;
    double episodicMemoryScore;
    size_t semanticMemoryNodes;
    double semanticMemoryScore;
    size_t proceduralMemoryTrails;
    double proceduralMemoryScore;
    size_t associativeMemoryConnections;
};

/**
 * Neuromodulation metrics
 */
struct NeuromodulationMetrics {
    double dopamineLevel;
    double curiosityLevel;
    double noveltyLevel;
    double predictionError;
};

/**
 * Neuromodulation analysis result
 */
struct NeuromodulationAnalysisResult {
    double dopamineLevel;
    double dopamineScore;
    double curiosityLevel;
    double curiosityScore;
    double noveltyLevel;
    double noveltyScore;
    double predictionError;
    double predictionErrorScore;
};

/**
 * Export options
 */
struct ExportOptions {
    bool includeHeader;
    bool includeMetadata;
    bool includeStatistics;
    std::string fileFormat;
    std::string compression;
    std::map<std::string, std::string> customOptions;
};

/**
 * NLM Data Exporter
 */
class NLMDataExporter {
public:
    NLMDataExporter();
    ~NLMDataExporter();
    
    /**
     * Export experiment results to CSV
     */
    bool exportToCSV(const std::string& filePath, const std::vector<ExperimentResult>& results);
    
    /**
     * Export experiment results to JSON
     */
    bool exportToJSON(const std::string& filePath, const std::vector<ExperimentResult>& results);
    
    /**
     * Export experiment results to SQLite
     */
    bool exportToSQLite(const std::string& filePath, const std::vector<ExperimentResult>& results);
    
    /**
     * Export experiment results to Parquet
     */
    bool exportToParquet(const std::string& filePath, const std::vector<ExperimentResult>& results);
    
    /**
     * Set export options
     */
    void setExportOptions(const ExportOptions& options);
    
    /**
     * Get export options
     */
    ExportOptions getExportOptions() const;
    
private:
    ExportOptions _options;
};

/**
 * Experiment analyzer
 */
class ExperimentAnalyzer {
public:
    ExperimentAnalyzer();
    ~ExperimentAnalyzer();
    
    /**
     * Analyze experiment result
     */
    AnalysisResult analyzeExperiment(const ExperimentResult& result);
    
    /**
     * Analyze multiple experiment results
     */
    std::vector<AnalysisResult> analyzeExperiments(const std::vector<ExperimentResult>& results);
    
    /**
     * Save analysis results
     */
    void saveAnalysis(const std::vector<AnalysisResult>& analyses, const std::string& filePath);
    
private:
    double calculatePerformanceScore(const ExperimentResult& result);
    double calculateEfficiencyScore(const ExperimentResult& result);
    double calculateLearningScore(const ExperimentResult& result);
    int countSuccessful(const std::vector<AnalysisResult>& analyses);
    double calculateAverage(const std::vector<AnalysisResult>& analyses, const std::string& metric);
};

/**
 * Performance analyzer
 */
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();
    
    /**
     * Analyze performance metrics
     */
    PerformanceAnalysisResult analyzePerformance(const PerformanceMetrics& metrics);
    
    /**
     * Save results
     */
    void saveResults(const PerformanceAnalysisResult& results, const std::string& filePath);
    
private:
    double calculateEfficiency(const PerformanceMetrics& metrics);
    double calculateThroughput(const PerformanceMetrics& metrics);
    double calculateStability(const PerformanceMetrics& metrics);
    double calculateOptimization(const PerformanceMetrics& metrics);
};

/**
 * Memory analyzer
 */
class MemoryAnalyzer {
public:
    MemoryAnalyzer();
    ~MemoryAnalyzer();
    
    /**
     * Analyze memory metrics
     */
    MemoryAnalysisResult analyzeMemory(const MemoryMetrics& metrics);
    
    /**
     * Save results
     */
    void saveResults(const MemoryAnalysisResult& results, const std::string& filePath);
    
private:
    double calculateWorkingMemoryScore(const MemoryMetrics& metrics);
    double calculateEpisodicMemoryScore(const MemoryMetrics& metrics);
    double calculateSemanticMemoryScore(const MemoryMetrics& metrics);
    double calculateProceduralMemoryScore(const MemoryMetrics& metrics);
};

/**
 * Neuromodulation analyzer
 */
class NeuromodulationAnalyzer {
public:
    NeuromodulationAnalyzer();
    ~NeuromodulationAnalyzer();
    
    /**
     * Analyze neuromodulation metrics
     */
    NeuromodulationAnalysisResult analyzeNeuromodulation(const NeuromodulationMetrics& metrics);
    
    /**
     * Save results
     */
    void saveResults(const NeuromodulationAnalysisResult& results, const std::string& filePath);
    
private:
    double calculateDopamineScore(const NeuromodulationMetrics& metrics);
    double calculateCuriosityScore(const NeuromodulationMetrics& metrics);
    double calculateNoveltyScore(const NeuromodulationMetrics& metrics);
    double calculatePredictionErrorScore(const NeuromodulationMetrics& metrics);
};

/**
 * Export manager
 */
class ExportManager {
public:
    ExportManager();
    ~ExportManager();
    
    /**
     * Export data
     */
    bool exportData(const std::string& dataType, const std::string& filePath,
                   const std::vector<ExperimentResult>& results);
    
    /**
     * Set export options
     */
    void setExportOptions(const ExportOptions& options);
    
    /**
     * Get export options
     */
    ExportOptions getExportOptions() const;
    
private:
    NLMDataExporter _exporter;
};

} // namespace nlm