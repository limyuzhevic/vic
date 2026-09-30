#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {
namespace Core {
namespace Utilities {

/**
 * Neural network specific validation and utility functions
 */
class BrainUtils {
public:
    // String formatting for neural network objects
    static std::string formatRegionName(const std::string& prefix, size_t id);
    static std::string formatPopulationName(const std::string& regionName, const std::string& type);
    static std::string formatNeuronName(size_t regionId, size_t populationId, size_t neuronId);
    static std::string formatConnectionName(size_t sourceId, size_t targetId);
    static std::string formatSynapseName(size_t sourceId, size_t targetId);

    // String formatting for logging and reporting
    static std::string formatCount(size_t count, const std::string& singular, const std::string& plural);
    static std::string formatPercentage(double ratio, int precision = 2);
    static std::string formatTime(double seconds);
    static std::string formatMemory(size_t bytes);
    static std::string formatWeight(double weight);
    static std::string formatThreshold(double threshold);
    static std::string formatLearningRate(double rate);

    // Configuration parameter validation
    static bool isValidNeuronCount(size_t neuronCount);
    static bool isValidRegionCount(size_t regionCount);
    static bool isValidConnectionProbability(double probability);
    static bool isValidTimestep(double timestep);
    static bool isValidReplayInterval(size_t interval);
    static bool isValidConsolidationInterval(size_t interval);

    // Neural network parameter validation
    static bool isValidSTDPParameters(float ltpWeight, float ltdWeight, float tau);
    static bool isValidPlasticityParameters(float rate, float min, float max);
    static bool isValidLearningRate(float rate);
    static bool isValidThreshold(float threshold);
    static bool isValidWeight(float weight);

    // Vector operations for neural networks
    static size_t divideEvenly(size_t total, size_t parts);
    static std::vector<size_t> distributeEvenly(size_t total, size_t parts);
    static std::vector<size_t> createNeuronsPerRegion(size_t totalNeurons, size_t regionCount);

    // Neuron and population validation
    static bool isValidNeuronId(size_t id);
    static bool isValidPopulationId(size_t id);
    static bool isValidRegionId(size_t id);

    // Container operations for neural networks
    static std::vector<std::pair<size_t, size_t>> createRegionConnections(size_t regionCount);
    static std::vector<std::pair<size_t, size_t>> createCompleteConnectionGraph(size_t neuronCount);
    static std::vector<size_t> createRandomPermutation(size_t n);

    // Statistics and reporting
    static std::string generateStatistics(size_t totalNeurons, size_t totalRegions, 
                                           size_t totalPopulations, size_t totalSynapses);
    static std::string generateMemoryStatistics(size_t capacity, size_t used);
    static std::string generatePerformanceStatistics(double duration, size_t steps);

    // Configuration key generation
    static std::string createConfigKey(const std::string& prefix, const std::string& suffix);
    static std::string getNestedConfigKey(const std::vector<std::string>& path);
    static std::string normalizeConfigKey(const std::string& key);

    // Error message formatting
    static std::string formatError(const std::string& context, const std::string& operation);
    static std::string formatErrorWithValue(const std::string& context, const std::string& operation,
                                           const std::string& value);
    static std::string formatErrorWithRange(const std::string& context, const std::string& operation,
                                           double value, double min, double max);

    // Debug and validation messages
    static std::string createValidationMessage(const std::string& field, const std::string& rule);
    static std::string createRangeValidationMessage(const std::string& field, double value,
                                                   double min, double max);
    static std::string createExistenceValidationMessage(const std::string& field);
    static std::string createTypeValidationMessage(const std::string& field, const std::string& type);

    // Neural network specific string operations
    static std::string getNeuronTypeString(int type);
    static std::string getRegionStatusString(bool active);
    static std::string getPopulationStatusString(bool active);
    static std::string getFiringStateString(int state);
    static std::string getDevelopmentStageString(int stage);

    // Memory and resource validation
    static bool isValidMemoryCapacity(size_t capacity);
    static bool isValidPopulationSize(size_t size);
    static bool isValidSynapticWeight(double weight);

    // Mathematical utilities for neural networks
    static double calculateConnectionDensity(size_t totalPossible, size_t actual);
    static double calculateFiringRate(size_t spikes, double duration);
    static double calculateSimilarity(const std::vector<double>& a, const std::vector<double>& b);
    static double calculateDistance(const std::vector<double>& a, const std::vector<double>& b);

    // Configuration validation utilities
    static bool validateConfigKeyFormat(const std::string& key);
    static bool validateConfigValueType(const ConfigValue& value);
    static bool isValidConfigSource(int source);

    // Neural network initialization validation
    static bool validateInitializationParameters(size_t neuronCount, size_t regionCount,
                                                 float connectionProbability);

    // Statistics calculation
    static double calculateMean(const std::vector<double>& values);
    static double calculateStandardDeviation(const std::vector<double>& values, double mean);
    static double calculateCoefficientOfVariation(const std::vector<double>& values);

    // Resource management validation
    static bool isResourceAvailable(size_t required, size_t available, double threshold = 0.8);
    static std::string formatResourceStatus(size_t used, size_t total);

    // Complex network visualization and analysis
    static std::string generateGraphvizDotFormat(const std::vector<NeuralRegion*>& regions,
                                                 const std::vector<InterRegionConnection>& connections,
                                                 const std::string& graphName = "NLM_Graph");
    static std::string generateConnectivityMatrix(const std::vector<NeuralRegion*>& regions);
    static std::string generateRegionHierarchy(const std::vector<NeuralRegion*>& regions);
    static std::string generateNeutronTopologyReport(const std::vector<NeuralRegion*>& regions);
    static std::string generatePlastictyAnalysisReport(const std::vector<NeuralRegion*>& regions);
    static std::string generateActivityHeatmap(const std::vector<NeuralRegion*>& regions,
                                               size_t timeSteps = 100);
    static bool exportNetworkToGraphML(const std::vector<NeuralRegion*>& regions,
                                      const std::vector<InterRegionConnection>& connections,
                                      const std::string& filepath);
    static bool exportNetworkToGML(const std::vector<NeuralRegion*>& regions,
                                   const std::vector<InterRegionConnection>& connections,
                                   const std::string& filepath);
    static std::string generateCustomVisualization(const std::vector<NeuralRegion*>& regions,
                                                    const std::map<std::string, std::string>& options);
};

} // namespace Utils
} // namespace Core
} // namespace nlm