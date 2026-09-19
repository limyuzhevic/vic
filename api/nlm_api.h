// NLM C++ API - Programmatic Control Interface
// Provides C++ and Python API for NLM neural simulations

#ifndef NLM_API_HPP
#define NLM_API_HPP

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <variant>
#include <any>

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Metrics.hpp"

namespace nlm {

// Forward declarations
class Experiment;
class ExperimentRunner;

// Configuration management API
class ConfigAPI {
public:
    static std::shared_ptr<Config> createConfig(const std::string& filename = "");
    static bool loadConfig(std::shared_ptr<Config> config, const std::string& filename);
    static bool saveConfig(std::shared_ptr<Config> config, const std::string& filename);
    static bool validateConfig(std::shared_ptr<Config> config);
    static std::string exportConfig(std::shared_ptr<Config> config, const std::string& format = "json");
    static bool importConfig(std::shared_ptr<Config> config, const std::string& data, const std::string& format = "json");
    static std::string configDiff(std::shared_ptr<Config> config1, std::shared_ptr<Config> config2);
    static std::string getConfigSummary(std::shared_ptr<Config> config);
    
    static std::shared_ptr<Config> createStandardTemplate();
    static std::shared_ptr<Config> createResearchTemplate();
    static std::shared_ptr<Config> createPerformanceTemplate();
    static std::shared_ptr<Config> mergeConfigs(const std::vector<std::shared_ptr<Config>>& configs);
};

// Brain simulation API
class BrainAPI {
public:
    static std::shared_ptr<Brain> createBrain(const std::string& configFile = "");
    static bool initializeBrain(std::shared_ptr<Brain> brain);
    static bool runStep(std::shared_ptr<Brain> brain, size_t step, double time);
    static bool runSimulation(std::shared_ptr<Brain> brain, size_t steps, double timestep = 0.001);
    static bool saveBrainState(std::shared_ptr<Brain> brain, const std::string& filename);
    static bool loadBrainState(std::shared_ptr<Brain> brain, const std::string& filename);
    static void logBrainStatus(std::shared_ptr<Brain> brain);
    
    static double getTotalNeuronCount(std::shared_ptr<Brain> brain);
    static double getTotalSynapseCount(std::shared_ptr<Brain> brain);
    static size_t getTotalSpikeCount(std::shared_ptr<Brain> brain);
    static size_t getFiringNeuronCount(std::shared_ptr<Brain> brain);
    static size_t getMemoryUsage(std::shared_ptr<Brain> brain);
    static size_t getMemoryAccessCount(std::shared_ptr<Brain> brain);
    
    static std::vector<std::vector<double>> getPopulationActivity(std::shared_ptr<Brain> brain);
    static std::vector<std::vector<double>> getRegionalActivity(std::shared_ptr<Brain> brain);
};

// Experiment API
class ExperimentAPI {
public:
    static std::shared_ptr<Experiment> createExperiment(const std::string& name, const std::string& type);
    static bool addMetric(std::shared_ptr<Experiment> experiment, const std::string& metricName,
                         std::function<double(const Brain&, size_t)> metricFunc);
    static bool setExperimentParameters(std::shared_ptr<Experiment> experiment,
                                        const std::map<std::string, std::any>& parameters);
    static bool runExperiment(std::shared_ptr<Experiment> experiment,
                             std::shared_ptr<Brain> brain,
                             std::shared_ptr<ExperimentRunner> runner,
                             size_t steps);
    static bool saveExperimentResults(std::shared_ptr<Experiment> experiment,
                                     const std::string& filename);
    static bool loadExperimentResults(std::shared_ptr<Experiment> experiment,
                                     const std::string& filename);
    static std::string getExperimentInfo(std::shared_ptr<Experiment> experiment);
    
    static std::shared_ptr<ExperimentRunner> createExperimentRunner();
    static bool runExperimentSuite(std::shared_ptr<ExperimentRunner> runner,
                                  std::shared_ptr<Brain> brain,
                                  const std::vector<std::shared_ptr<Experiment>>& experiments,
                                  size_t steps);
};

// Data analysis API
class AnalysisAPI {
public:
    static std::vector<double> calculateStatistics(const std::vector<double>& data);
    static double calculateMean(const std::vector<double>& data);
    static double calculateMedian(const std::vector<double>& data);
    static double calculateStddev(const std::vector<double>& data);
    static double calculateCorrelation(const std::vector<double>& data1, const std::vector<double>& data2);
    static std::vector<size_t> detectAnomalies(const std::vector<double>& data, double threshold = 3.0);
    static std::vector<size_t> performClustering(const std::vector<std::vector<double>>& data, size_t numClusters);
    static std::vector<double> performPCA(const std::vector<std::vector<double>>& data, size_t numComponents = 2);
    
    static std::vector<double> smoothData(const std::vector<double>& data, size_t windowSize = 5);
    static std::vector<double> differentiateData(const std::vector<double>& data, double dt = 0.001);
    static std::vector<double> integrateData(const std::vector<double>& data, double dt = 0.001);
    static std::vector<double> normalizeData(const std::vector<double>& data, double mean = 0.0, double stddev = 1.0);
};

// Neural network operations API
class NeuralAPI {
public:
    static std::vector<std::vector<double>> matrixMultiply(const std::vector<std::vector<double>>& A,
                                                          const std::vector<std::vector<double>>& B);
    static std::vector<std::vector<double>> matrixAdd(const std::vector<std::vector<double>>& A,
                                                     const std::vector<std::vector<double>>& B);
    static std::vector<std::vector<double>> matrixTranspose(const std::vector<std::vector<double>>& matrix);
    static double matrixDeterminant(const std::vector<std::vector<double>>& matrix);
    static std::vector<std::vector<double>> matrixInverse(const std::vector<std::vector<double>>& matrix);
    
    static double dotProduct(const std::vector<double>& a, const std::vector<double>& b);
    static double vectorNorm(const std::vector<double>& vector, double p = 2.0);
    static std::vector<double> vectorAdd(const std::vector<double>& a, const std::vector<double>& b);
    static std::vector<double> vectorSubtract(const std::vector<double>& a, const std::vector<double>& b);
    
    static std::vector<double> sigmoid(const std::vector<double>& x);
    static std::vector<double> relu(const std::vector<double>& x);
    static std::vector<double> tanh(const std::vector<double>& x);
    static std::vector<double> softmax(const std::vector<double>& x);
    
    static std::vector<std::vector<double>> convolution2D(const std::vector<std::vector<double>>& input,
                                                         const std::vector<std::vector<double>>& kernel,
                                                         size_t stride = 1, size_t padding = 0);
    
    static double crossEntropyLoss(const std::vector<double>& predictions,
                                  const std::vector<double>& targets);
    static double meanSquaredError(const std::vector<double>& predictions,
                                  const std::vector<double>& targets);
};

// Pipeline automation API
class PipelineAPI {
public:
    struct PipelineStep {
        std::string name;
        std::function<bool(std::shared_ptr<Config>, std::shared_ptr<Brain>, std::any&)> function;
        std::map<std::string, std::any> parameters;
    };
    
    static bool addPipelineStep(std::vector<PipelineStep>& pipeline,
                               const std::string& name,
                               std::function<bool(std::shared_ptr<Config>, std::shared_ptr<Brain>, std::any&)> function);
    static bool executePipeline(std::vector<PipelineStep>& pipeline,
                               std::shared_ptr<Config> config,
                               std::shared_ptr<Brain> brain);
    static bool savePipeline(const std::vector<PipelineStep>& pipeline, const std::string& filename);
    static bool loadPipeline(std::vector<PipelineStep>& pipeline, const std::string& filename);
    static void printPipeline(const std::vector<PipelineStep>& pipeline);
};

// Batch processing API
class BatchAPI {
public:
    struct BatchParameters {
        size_t numExperiments;
        std::vector<std::string> experimentTypes;
        std::vector<size_t> steps;
        std::string outputDirectory;
        bool parallelExecution;
        size_t numProcesses;
    };
    
    static BatchParameters createBatchParameters(size_t numExperiments = 10);
    static bool addExperimentToBatch(BatchParameters& params,
                                    const std::string& type,
                                    size_t steps);
    static bool executeBatch(const BatchParameters& params);
    static bool aggregateBatchResults(const std::string& outputDirectory,
                                     const std::string& outputFile);
    static bool exportBatchResults(const BatchParameters& params,
                                  const std::string& format = "json");
};

// Configuration templates API
class TemplateAPI {
public:
    static std::shared_ptr<Config> getTemplate(const std::string& templateName);
    static bool addTemplate(const std::string& name, std::shared_ptr<Config> config);
    static bool removeTemplate(const std::string& name);
    static bool updateTemplate(const std::string& name, std::shared_ptr<Config> config);
    static std::vector<std::string> listTemplates();
    static std::shared_ptr<Config> mergeTemplateConfigs(const std::vector<std::string>& templateNames);
    
    static std::shared_ptr<Config> createNeurodevelopmentalTemplate();
    static std::shared_ptr<Config> createCognitiveTemplate();
    static std::shared_ptr<Config> createLearningTemplate();
    static std::shared_ptr<Config> createMemoryTemplate();
};

// Performance monitoring API
class PerformanceAPI {
public:
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        double gpuUsage;
        double networkUsage;
        double diskUsage;
        std::chrono::system_clock::time_point timestamp;
    };
    
    static PerformanceMetrics getPerformanceMetrics();
    static std::vector<PerformanceMetrics> monitorPerformance(size_t duration, double interval);
    static double calculatePerformanceScore(const PerformanceMetrics& metrics);
    static bool savePerformanceMetrics(const std::vector<PerformanceMetrics>& metrics,
                                      const std::string& filename);
    static bool loadPerformanceMetrics(std::vector<PerformanceMetrics>& metrics,
                                       const std::string& filename);
    
    static void optimizeForPerformance(std::shared_ptr<Config> config);
    static void optimizeForEnergy(std::shared_ptr<Config> config);
    static void optimizeForAccuracy(std::shared_ptr<Config> config);
};

// Visualization and plotting API
class VisualizationAPI {
public:
    static bool createSpikeRasterPlot(const std::vector<std::vector<size_t>>& spikeTrains,
                                     const std::string& outputFile,
                                     const std::string& title = "Spike Raster Plot");
    static bool createFiringRatePlot(const std::vector<double>& firingRates,
                                    const std::string& outputFile,
                                    const std::string& title = "Firing Rate");
    static bool createSynapseWeightPlot(const std::vector<std::vector<double>>& weightHistory,
                                       const std::string& outputFile,
                                       const std::string& title = "Synapse Weight Evolution");
    static bool createBrainActivityPlot(const std::vector<std::vector<double>>& activityData,
                                       const std::string& outputFile,
                                       const std::string& title = "Brain Activity");
    static bool create3DNeuronVisualization(std::shared_ptr<Brain> brain,
                                           const std::string& outputFile);
    static bool createNetworkConnectivityPlot(std::shared_ptr<Brain> brain,
                                             const std::string& outputFile,
                                             const std::string& title = "Network Connectivity");
    
    static bool exportDataForExternalVisualization(const std::string& dataFile,
                                                   const std::string& format = "json");
    static bool importVisualizationConfig(const std::string& configFile);
};

// Utility functions API
class UtilityAPI {
public:
    static std::string generateUniqueId();
    static size_t hashString(const std::string& str);
    static double currentTime();
    static void sleep(double seconds);
    static std::string formatString(const std::string& format, ...);
    static std::string getPlatformInfo();
    static std::string getVersionInfo();
    
    static bool fileExists(const std::string& filename);
    static bool createDirectory(const std::string& path);
    static std::vector<std::string> listFiles(const std::string& directory);
    static bool removeFile(const std::string& filename);
    static bool copyFile(const std::string& source, const std::string& destination);
    
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    static std::string base64Encode(const std::string& data);
    static std::string base64Decode(const std::string& data);
    
    static std::string getCurrentWorkingDirectory();
    static bool changeWorkingDirectory(const std::string& path);
    static std::vector<std::string> getEnvironmentVariables();
    static std::string getEnvironmentVariable(const std::string& name, const std::string& defaultValue = "");
    static bool setEnvironmentVariable(const std::string& name, const std::string& value);
};

// Main API function
inline void initializeNLM() {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

inline void finalizeNLM() {
    Logger::clearGlobal();
}

} // namespace nlm

#endif // NLM_API_HPP
