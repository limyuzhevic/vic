// NLM CLI Analysis - Custom metric analysis commands header
// Handles custom metric definition, statistical analysis, and experiment optimization

#ifndef NLM_CLI_ANALYSIS_HPP
#define NLM_CLI_ANALYSIS_HPP

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <random>
#include <cmath>
#include <complex>

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Metrics.hpp"

namespace nlm {
namespace cli {

class AnalysisCLI {
public:
    AnalysisCLI();
    
    int run(int argc, char** argv);
    
private:
    void initializeLogger();
    
    // Analysis commands
    int handleMetricsCommand(int argc, char** argv);
    int handleStatisticalAnalysisCommand(int argc, char** argv);
    int handleMachineLearningCommand(int argc, char** argv);
    int handleFeatureEngineeringCommand(int argc, char** argv);
    int handleOptimizationCommand(int argc, char** argv);
    int handleForecastingCommand(int argc, char** argv);
    int handleAnomalyDetectionCommand(int argc, char** argv);
    int handleCorrelationAnalysisCommand(int argc, char** argv);
    int handleClusteringCommand(int argc, char** argv);
    
    // Metric definition structure
    struct CustomMetric {
        std::string name;
        std::string description;
        std::function<double(const Brain&, size_t)> calculator;
        std::string unit;
        bool isDerived;
        std::vector<std::string> dependencies;
    };
    
    // Analysis result structures
    struct StatisticalResults {
        double mean;
        double median;
        double stddev;
        double min;
        double max;
        double correlation;
        double pvalue;
    };
    
    struct MLModel {
        std::string modelType;
        double accuracy;
        std::vector<std::string> features;
        double trainingTime;
    };
    
    struct FeatureSet {
        size_t featureCount;
        std::vector<std::string> derivedFeatures;
        double processingTime;
    };
    
    struct OptimizationResults {
        std::map<std::string, double> optimalParameters;
        double optimalScore;
        double improvement;
        size_t iterations;
        double convergenceTime;
    };
    
    struct ForecastResults {
        std::vector<double> forecastData;
        double modelAccuracy;
        double forecastConfidence;
        size_t forecastHorizon;
    };
    
    struct AnomalyResults {
        size_t anomalyCount;
        double anomalyThreshold;
        std::vector<size_t> anomalyIndices;
        std::vector<double> anomalySeverity;
    };
    
    struct CorrelationResults {
        double correlationCoefficient;
        double pValue;
        bool significance;
        std::string correlationType;
    };
    
    struct ClusteringResults {
        size_t numClusters;
        std::vector<size_t> clusterAssignments;
        std::vector<std::vector<double>> clusterCentroids;
        double clusterQuality;
    };
    
    // Helper functions
    std::vector<CustomMetric> createDefaultMetrics();
    void generateMetricReport(const std::vector<CustomMetric>& metrics,
                             const std::vector<std::vector<double>>& metricData,
                             const std::vector<double>& timestamps,
                             const std::string& outputFile);
    StatisticalResults performStatisticalAnalysis(const std::string& filename,
                                                 const std::string& analysisType);
    void generateStatisticalReport(const StatisticalResults& results,
                                  const std::string& outputFile);
    MLModel trainMachineLearningModel(const std::string& trainingData, const std::string& modelType);
    void saveMLModel(const MLModel& model, const std::string& outputFile);
    FeatureSet engineerFeatures(const std::string& inputMetrics);
    void saveFeatureSet(const FeatureSet& features, const std::string& outputFile);
    OptimizationResults runOptimization(const std::string& experimentConfig);
    void generateOptimizationReport(const OptimizationResults& results,
                                   const std::string& outputFile);
    ForecastResults runForecasting(const std::string& historicalData, size_t steps);
    void generateForecastReport(const ForecastResults& results,
                               const std::string& outputFile);
    AnomalyResults detectAnomalies(const std::string& dataFile, double threshold);
    void generateAnomalyReport(const AnomalyResults& results,
                               const std::string& outputFile);
    CorrelationResults performCorrelationAnalysis(const std::string& dataFile1,
                                                  const std::string& dataFile2,
                                                  const std::string& correlationType);
    void generateCorrelationReport(const CorrelationResults& results,
                                  const std::string& outputFile);
    ClusteringResults performClustering(const std::string& dataFile, size_t numClusters);
    void generateClusteringReport(const ClusteringResults& results,
                                 const std::string& outputFile);
    
    void printUsage();
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_ANALYSIS_HPP
