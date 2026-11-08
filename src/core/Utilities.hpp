// NLM Utility Functions and Command-Line Tools
// This file provides advanced utilities for NLM users, including configuration
// management, simulation utilities, and analysis tools.

#ifndef NLM_UTILITIES_H
#define NLM_UTILITIES_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <random>
#include <functional>
#include <cmath>

namespace nlm {
namespace utils {

// File and directory operations
class FileUtils {
public:
    static bool fileExists(const std::string& path);
    static std::string getFileExtension(const std::string& filename);
    static std::string getFileNameWithoutExtension(const std::string& filename);
    static std::string getCurrentTimestamp();
    static std::string formatNumber(double number, int precision = 6);
    static std::string joinPath(const std::vector<std::string>& paths);
    static std::vector<std::string> splitPath(const std::string& path);
    static bool createDirectory(const std::string& path);
    static bool deleteFile(const std::string& path);
};

// Configuration utilities
class ConfigUtils {
public:
    static std::string generateRandomSeed();
    static std::string formatConfigValue(const ConfigValue& value);
    static std::string configValueToString(const ConfigValue& value);
    static bool validateConfig(const Config& config);
    static std::string getConfigSummary(const Config& config);
    static std::vector<std::string> getRequiredConfigKeys();
    static bool isConfigComplete(const Config& config);
};

// Simulation utilities
class SimulationUtils {
public:
    static double calculateFiringRate(size_t spikeCount, double durationSeconds);
    static double calculateAverageFiringRate(const std::vector<double>& firingRates);
    static double calculateStandardDeviation(const std::vector<double>& values);
    static double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    static std::vector<double> movingAverage(const std::vector<double>& data, size_t windowSize);
    static std::vector<double> exponentialMovingAverage(const std::vector<double>& data, double alpha);
    static std::string formatTime(double seconds);
    static std::string formatDuration(double seconds);
};

// Analysis utilities
class AnalysisUtils {
public:
    struct AnalysisResult {
        double mean;
        double median;
        double stddev;
        double min;
        double max;
        double range;
        size_t count;
        std::string statsString() const;
    };
    
    static AnalysisResult analyzeVector(const std::vector<double>& data);
    static std::vector<size_t> findPeaks(const std::vector<double>& data, double threshold = 0.0);
    static size_t countAboveThreshold(const std::vector<double>& data, double threshold);
    static double calculateKurtosis(const std::vector<double>& data);
    static double calculateSkewness(const std::vector<double>& data);
    static std::string getQualityGrade(double value, double thresholdHigh = 0.9, double thresholdLow = 0.5);
};

// Command-line utility classes
class CommandLineParser {
public:
    struct Argument {
        std::string name;
        std::string value;
        bool hasValue;
        bool operator()() const { return hasValue; }
    };
    
    static std::vector<Argument> parseArguments(int argc, char** argv);
    static std::string findArgumentValue(const std::vector<Argument>& args, const std::string& name);
    static bool hasArgument(const std::vector<Argument>& args, const std::string& name);
};

class ConfigurationManager {
public:
    static bool saveConfigToJSON(const Config& config, const std::string& filepath);
    static bool loadConfigFromJSON(Config& config, const std::string& filepath);
    static bool saveConfigToYAML(const Config& config, const std::string& filepath);
    static bool loadConfigFromYAML(Config& config, const std::string& filepath);
    static bool validateConfigFile(const std::string& filepath);
    static std::string getConfigTemplate(const std::string& format = "json");
};

class SimulationRunner {
public:
    static int runSimulation(const Config& config, const std::string& outputFile = "");
    static bool runBenchmark(const Config& config, size_t iterations = 100);
    static bool validateConfiguration(const Config& config);
    static bool exportResults(const std::string& resultsFile, const std::string& outputDir = ".");
    static std::string generateReport(const Config& config, const std::string& outputFile = "");
};

// Progress and monitoring utilities
class ProgressReporter {
public:
    static void showProgress(size_t current, size_t total, const std::string& label = "Progress");
    static void showProgressBar(size_t current, size_t total, size_t width = 50);
    static void setProgressFrequency(size_t steps);
    static void clearProgress();
};

class MetricsCollector {
public:
    struct Metrics {
        size_t steps;
        double avgFiringRate;
        double maxFiringRate;
        double totalSpikes;
        double avgMembranePotential;
        double avgSynapticInput;
        double learningProgress;
        std::vector<double> firingRateHistory;
        std::vector<double> membranePotentialHistory;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        
        std::string toCSV() const;
        std::string toJSON() const;
        void reset();
    };
    
    static void recordStep(size_t step, const Brain& brain);
    static Metrics getCurrentMetrics();
    static void saveMetrics(const std::string& filepath, const Metrics& metrics);
    static void loadMetrics(Metrics& metrics, const std::string& filepath);
    static std::string generateMetricsReport(const Metrics& metrics);
};

// Version and build information
class BuildInfo {
public:
    static const char* getVersion();
    static const char* getBuildDate();
    static const char* getBuildTime();
    static const char* getBuildType();
    static const char* getCompilerInfo();
    static const char* getPlatformInfo();
    static std::string getFullVersionInfo();
};

// Error handling and reporting
class ErrorHandler {
public:
    enum class ErrorCode {
        SUCCESS = 0,
        INVALID_ARGUMENT = 1,
        FILE_NOT_FOUND = 2,
        INVALID_FORMAT = 3,
        CONFIGURATION_ERROR = 4,
        SIMULATION_ERROR = 5,
        MEMORY_ERROR = 6,
        UNKNOWN_ERROR = 7
    };
    
    static std::string errorCodeToString(ErrorCode code);
    static ErrorCode getLastError();
    static void setLastError(ErrorCode code, const std::string& message);
    static std::string getLastErrorMessage();
    static bool isRecoveryPossible(ErrorCode code);
    static void handleError(ErrorCode code, const std::string& context = "");
};

// High-level convenience functions
namespace convenience {
    // Simple brain setup and simulation
    std::shared_ptr<Brain> createSimpleBrain(int neuronCount = 500, int regionCount = 1);
    std::shared_ptr<Brain> createBrainWithConnectivity(int neuronCount = 500, double connectionProb = 0.15);
    std::shared_ptr<Brain> createBrainWithPlasticity(bool hebbian = true, bool stdp = true, bool structural = false);
    
    // Quick simulation runs
    bool runConnectivityTest(std::shared_ptr<Brain> brain, size_t steps = 50);
    bool runPlasticityTest(std::shared_ptr<Brain> brain, size_t steps = 1000);
    bool runSTDPTest(std::shared_ptr<Brain> brain, size_t trials = 50);
    
    // Configuration presets
    Config createDefaultConfig();
    Config createBenchmarkConfig();
    Config createResearchConfig();
    Config createRealisticConfig();
    
    // Simple analysis
    std::string getBrainStatistics(const Brain& brain);
    std::string getLearningSummary(const Brain& brain);
    bool exportBrainState(const Brain& brain, const std::string& filepath);
    bool importBrainState(std::shared_ptr<Brain>& brain, const std::string& filepath);
}

// Inline helper functions
inline std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

inline std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

inline bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

inline bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline std::string generateRandomSeed() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 999999999);
    return std::to_string(dis(gen));
}

} // namespace utils
} // namespace nlm

#endif // NLM_UTILITIES_H
