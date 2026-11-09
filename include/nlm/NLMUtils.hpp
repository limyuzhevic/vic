#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <chrono>
#include <thread>
#include <cmath>

#include "nlm/Brain.hpp"
#include "nlm/AgentBrain.hpp"
#include "nlm/SimpleWorld.hpp"

namespace nlm {

class NLMUtils {
public:
    // Configuration file utilities
    class ConfigUtils {
    public:
        static std::shared_ptr<Config> loadConfig(const std::string& filename);
        static bool saveConfig(const Config& config, const std::string& filename);
        static std::shared_ptr<Config> mergeConfigs(const Config& base, const std::string& overrideFile);
        static void printConfig(const Config& config);
        static std::string getConfigSummary(const Config& config);
    };

    // Visualization utilities
    class VisualizationUtils {
    public:
        static void createVisualization(const Brain& brain, const std::string& outputFile);
        static void plotNeuralActivity(const Brain& brain, const std::string& outputFile);
        static void visualizeConnections(const Brain& brain, const std::string& outputFile);
        static std::string generateHTMLVisualization(const Brain& brain);
    };

    // Analysis utilities
    class AnalysisUtils {
    public:
        static double calculateFiringRate(const Brain& brain);
        static double calculateExcitatoryInhibitionRatio(const Brain& brain);
        static std::vector<float> analyzeNeuronActivations(const Brain& brain);
        static void performStatisticalAnalysis(const Brain& brain, std::ostream& output);
        static std::string generateBrainReport(const Brain& brain);
    };

    // Export/import utilities
    class StateUtils {
    public:
        static bool saveBrainState(const Brain& brain, const std::string& filename);
        static bool loadBrainState(Brain& brain, const std::string& filename);
        static bool exportExperimentData(const Brain& brain, const std::string& filename);
        static bool importExperimentData(Brain& brain, const std::string& filename);
        static std::string generateStateFileHeader(const Brain& brain);
    };

    // Benchmark utilities
    class BenchmarkUtils {
    public:
        struct BenchmarkResults {
            double simulationTime;
            size_t neuronsProcessed;
            size_t spikesGenerated;
            double firingRate;
            double energyConsumption;
            double learningRate;
        };
        
        static BenchmarkResults benchmarkSimulation(const Brain& brain, size_t steps);
        static void runPerformanceProfile(const Brain& brain, size_t steps);
        static std::string formatBenchmarkResults(const BenchmarkResults& results);
        static void compareBrains(const std::vector<std::shared_ptr<Brain>>& brains, const std::string& outputFile);
    };

    // Simulation control utilities
    class SimulationUtils {
    public:
        static void runControlledSimulation(const Brain& brain, size_t steps, double timestep);
        static bool runEpisode(const Brain& brain, const SimpleWorld& world, const AgentBrain& agent);
        static void pauseSimulation();
        static void resumeSimulation();
        static bool isSimulationPaused();
        static void setSimulationSpeed(double speed);
        static double getSimulationSpeed();
    };

    // Debugging utilities
    class DebugUtils {
    public:
        static void enableDebugLogging(const std::string& logFile);
        static void disableDebugLogging();
        static void setLogLevel(int level);
        static void printSystemStatus(const Brain& brain);
        static void analyzeMemoryUsage(const Brain& brain);
        static void printPerformanceMetrics(const Brain& brain);
        static std::string captureBrainSnapshot(const Brain& brain);
    };

    // Advanced analysis utilities
    class AdvancedAnalysis {
    public:
        static std::vector<std::vector<float>> calculateStateSpace(const Brain& brain, size_t steps);
        static double calculateEntropy(const std::vector<float>& data);
        static std::vector<float> computePrincipalComponents(const std::vector<std::vector<float>>& data);
        static std::map<size_t, size_t> analyzeSpikeTimeIntervals(const Brain& brain);
        static double computeNetworkClustering(const Brain& brain);
        static std::vector<std::string> detectBehaviorPatterns(const Brain& brain);
    };

    // Experiment utilities
    class ExperimentUtils {
    public:
        struct ExperimentResult {
            std::string experimentName;
            double successRate;
            double learningRate;
            double performance;
            std::vector<double> metrics;
            std::string timestamp;
        };
        
        static ExperimentResult runLearningExperiment(const Brain& brain, const std::string& experimentType);
        static std::vector<ExperimentResult> runBatchExperiments(const std::vector<std::string>& experimentTypes);
        static bool saveExperimentResults(const std::vector<ExperimentResult>& results, const std::string& filename);
        static std::vector<ExperimentResult> loadExperimentResults(const std::string& filename);
        static std::string generateExperimentSummary(const std::vector<ExperimentResult>& results);
    };

    // Neural network analysis utilities
    class NetworkAnalysis {
    public:
        static std::vector<std::vector<float>> computeAdjacencyMatrix(const Brain& brain);
        static std::vector<std::vector<float>> computeFunctionalConnectivity(const Brain& brain);
        static double computeSmallWorldCoefficient(const Brain& brain);
        static std::vector<size_t> findHubNodes(const Brain& brain);
        static std::vector<std::pair<size_t, size_t>> findCriticalPaths(const Brain& brain);
        static std::vector<std::vector<float>> computeCommunityStructure(const Brain& brain);
    };

    // Utility functions
    static std::string generateTimestamp();
    static std::string formatTime(double seconds);
    static std::string formatNumber(double number);
    static std::string truncateString(const std::string& str, size_t maxLength);
    static std::vector<std::string> splitString(const std::string& str, char delimiter);
    static std::string joinStrings(const std::vector<std::string>& strings, const std::string& delimiter);
    static bool fileExists(const std::string& filename);
    static std::string getFileExtension(const std::string& filename);
    static std::string getFileBaseName(const std::string& filename);
};