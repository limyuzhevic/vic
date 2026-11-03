// Advanced brain state analysis and debugging utilities
// Provides tools for examining and understanding NLM brain operation

#ifndef ADVANCED_TOOLS_HPP
#define ADVANCED_TOOLS_HPP

#include "../brain/Brain.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

namespace nlm {

/**
 * Advanced brain analyzer for examining and understanding brain operation
 */
class AdvancedBrainAnalyzer {
public:
    AdvancedBrainAnalyzer(std::shared_ptr<Brain> brain);
    ~AdvancedBrainAnalyzer();
    
    /**
     * Analyze current brain state comprehensively
     */
    struct AnalysisReport {
        // Basic statistics
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        float averageFiringRate;
        float excitationInhibitionRatio;
        
        // Memory system status
        bool workingMemoryActive;
        size_t workingMemoryTraces;
        bool episodicMemoryActive;
        size_t episodicMemoryEpisodes;
        
        // Neuromodulation levels
        float dopamineLevel;
        float noveltyLevel;
        float curiosityLevel;
        float predictionError;
        
        // Development status
        DevelopmentalStage developmentalStage;
        float plasticityModifier;
        
        // Network properties
        size_t regionCount;
        float averageSynapticWeight;
        float synapticWeightVariance;
        
        // Activity patterns
        float memoryActivityLevel;
        float predictionAccuracy;
        float learningRate;
        
        // Stability indicators
        float networkStability;
        float memoryRetention;
        float adaptability;
    };
    
    AnalysisReport analyzeCurrentState() const;
    
    /**
     * Generate visualization data for brain state
     */
    struct VisualizationData {
        // Neuron positions and states
        std::vector<std::pair<float, float>> neuronPositions;
        std::vector<float> neuronActivities;
        std::vector<std::string> neuronTypes;
        
        // Synaptic connections
        std::vector<std::tuple<size_t, size_t, float>> synapses;
        std::vector<float> synapticWeights;
        
        // Memory representations
        std::vector<std::vector<float>> memoryPatterns;
        std::vector<float> memoryActivations;
        
        // Temporal patterns
        std::vector<std::vector<float>> spikeHistory;
        std::vector<std::vector<float>> activityTrajectories;
    };
    
    VisualizationData generateVisualizationData() const;
    
    /**
     * Compare brain state at two different times
     */
    struct ComparisonResult {
        float stateDifference;
        float activitySimilarity;
        float connectivityChange;
        float memoryChange;
        std::string trendAnalysis;
    };
    
    ComparisonResult compareStates(const AnalysisReport& previousReport, 
                                   const AnalysisReport& currentReport) const;
    
    /**
     * Detect and report anomalies in brain operation
     */
    struct AnomalyReport {
        bool hasAnomalies;
        std::vector<std::string> anomalyTypes;
        std::vector<std::string> descriptions;
        float severity;
        std::string recommendation;
    };
    
    AnomalyReport detectAnomalies() const;
    
    /**
     * Generate learning progress report
     */
    struct LearningProgressReport {
        float learningRate;
        float convergenceSpeed;
        float stability;
        std::vector<float> memoryRetentionCurve;
        std::vector<float> predictionAccuracyCurve;
        std::string overallAssessment;
    };
    
    LearningProgressReport generateLearningReport() const;
    
    /**
     * Export brain state for external analysis
     */
    bool exportForExternalAnalysis(const std::string& filepath) const;
    
    /**
     * Generate detailed neural network topology
     */
    struct NetworkTopology {
        size_t totalRegions;
        size_t totalPopulations;
        std::unordered_map<size_t, size_t> regionPopulationCounts;
        std::unordered_map<size_t, size_t> populationNeuronCounts;
        std::unordered_map<size_t, NeuronType> populationTypes;
        std::unordered_map<size_t, size_t> synapticDensity;  // synapses per neuron
        float connectionProbability;
    };
    
    NetworkTopology analyzeNetworkTopology() const;
    
    /**
     * Generate performance benchmarks
     */
    struct PerformanceBenchmark {
        double wallClockTimePerStep;
        double cpuUtilization;
        double memoryUsage;
        size_t spikesProcessedPerStep;
        size_t neuronsActivePerStep;
        float plasticityOperationsPerStep;
    };
    
    PerformanceBenchmark benchmarkPerformance(size_t steps) const;
    
    /**
     * Generate system diagnostic information
     */
    struct SystemDiagnostics {
        std::string systemStatus;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        std::vector<std::string> performanceIssues;
        std::string recommendation;
    };
    
    SystemDiagnostics getDiagnostics() const;
    
    /**
     * Generate complete analysis report
     */
    std::string generateCompleteReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // ADVANCED_TOOLS_HPP