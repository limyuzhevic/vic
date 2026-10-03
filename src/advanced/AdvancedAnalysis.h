// ====================================================================
// ADVANCED ANALYSIS MODULE
// ====================================================================

#ifndef SRC_ADVANCED_ADVANCEDANALYSIS_H
#define SRC_ADVANCED_ADVANCEDANALYSIS_H

#include "brain/Brain.hpp"
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <numeric>
#include <set>

namespace nlm {

class AdvancedAnalysis {
public:
    struct NeuralConnectivityAnalysis {
        double connectionDensity;
        double clusteringCoefficient;
        double characteristicPathLength;
        double smallWorldness;
        std::map<std::string, double> communityStructure;
        std::vector<std::vector<double>> adjacencyMatrix;
        
        NeuralConnectivityAnalysis() : connectionDensity(0.0), clusteringCoefficient(0.0),
                                     characteristicPathLength(0.0), smallWorldness(1.0) {}
    };
    
    struct LearningDynamicsAnalysis {
        double learningRate;
        double plasticityStrength;
        double memoryConsolidation;
        double transferEfficiency;
        std::vector<double> learningCurve;
        std::vector<double> retentionCurve;
        std::map<std::string, double> adaptationMetrics;
        
        LearningDynamicsAnalysis() : learningRate(0.0), plasticityStrength(0.0),
                                    memoryConsolidation(0.0), transferEfficiency(0.0) {}
    };
    
    struct PerformanceMetrics {
        double simulationSpeed;
        double memoryEfficiency;
        double computationalComplexity;
        double scalabilityFactor;
        std::chrono::nanoseconds simulationTime;
        size_t neuronsProcessed;
        size_t synapsesAnalyzed;
        
        PerformanceMetrics() : simulationSpeed(0.0), memoryEfficiency(0.0),
                               computationalComplexity(0.0), scalabilityFactor(1.0) {}
    };
    
    struct AnomalyDetectionResult {
        bool anomalyDetected;
        std::string anomalyType;
        double anomalyScore;
        std::string description;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, double> contributingFactors;
        
        AnomalyDetectionResult() : anomalyDetected(false), anomalyScore(0.0) {}
    };
    
    struct PredictiveModel {
        std::string modelType;
        double accuracy;
        double confidence;
        std::vector<double> parameters;
        std::map<std::string, double> featureImportance;
        std::chrono::system_clock::time_point lastTrained;
        
        PredictiveModel() : accuracy(0.0), confidence(0.0) {}
    };
    
    // Neural connectivity analysis
    static NeuralConnectivityAnalysis analyzeConnectivity(std::shared_ptr<Brain> brain);
    
    // Learning dynamics analysis
    static LearningDynamicsAnalysis analyzeLearningDynamics(std::shared_ptr<Brain> brain,
                                                              const std::map<std::string, double>& parameters = {});
    
    // Performance analysis
    static PerformanceMetrics analyzePerformance(std::shared_ptr<Brain> brain,
                                                 const std::chrono::system_clock::time_point& startTime,
                                                 const std::chrono::system_clock::time_point& endTime);
    
    // Anomaly detection
    static AnomalyDetectionResult detectAnomalies(std::shared_ptr<Brain> brain,
                                                  const std::map<std::string, double>& historicalData = {});
    
    // Predictive modeling
    static PredictiveModel predictLearningOutcome(std::shared_ptr<Brain> brain,
                                                  const std::map<std::string, double>& conditions,
                                                  const std::string& predictionType = "performance");
    
    // Comprehensive analysis report
    static std::string generateAnalysisReport(std::shared_ptr<Brain> brain,
                                               const std::map<std::string, double>& analysisParameters = {});
    
    // Statistical analysis
    static std::map<std::string, double> computeStatisticalMeasures(const std::vector<double>& data);
    
    // Network topology analysis
    static std::map<std::string, double> analyzeNetworkTopology(std::shared_ptr<Brain> brain);
    
    // Learning efficiency analysis
    static std::map<std::string, double> analyzeLearningEfficiency(std::shared_ptr<Brain> brain,
                                                                      int simulationSteps);
    
    // Memory usage analysis
    static std::map<std::string, double> analyzeMemoryUsage(std::shared_ptr<Brain> brain);
    
    // Scalability analysis
    static std::map<std::string, double> analyzeScalability(std::vector<size_t> neuronCounts,
                                                               int simulationSteps);
    
    // Cross-validation for predictive models
    static std::map<std::string, double> crossValidatePredictiveModel(
        std::shared_ptr<Brain> brain,
        const std::vector<std::map<std::string, double>>& validationData,
        const std::string& modelType);
    
    // Anomaly clustering
    static std::map<std::string, std::vector<std::vector<double>>> clusterAnomalies(
        const std::vector<AnomalyDetectionResult>& anomalies);
    
    // Learning trajectory analysis
    static std::map<std::string, std::vector<double>> analyzeLearningTrajectory(
        std::shared_ptr<Brain> brain,
        int analysisWindow = 100);
    
    // Network resilience analysis
    static std::map<std::string, double> analyzeNetworkResilience(std::shared_ptr<Brain> brain,
                                                                     double failureRate = 0.1);
    
    // Developmental trajectory analysis
    static std::map<std::string, std::vector<double>> analyzeDevelopmentalTrajectory(
        std::shared_ptr<Brain> brain,
        int stages = 10);
};

} // namespace nlm

#endif // SRC_ADVANCED_ADVANCEDANALYSIS_H
