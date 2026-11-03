// NLM C++ Utilities and Advanced Commands
// Power-user utilities for working with the NLM brain simulation framework

#ifndef NLM_UTILITIES_HPP
#define NLM_UTILITIES_HPP

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "environment/Environment.hpp"
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <chrono>

namespace nlm {

// Utility functions for configuration management
class ConfigUtilities {
public:
    // Export configuration to various formats
    static bool exportToJSON(const Config& config, const std::string& filepath);
    static bool exportToYAML(const Config& config, const std::string& filepath);
    static bool exportToINI(const Config& config, const std::string& filepath);
    
    // Import configuration from various formats
    static bool importFromJSON(Config& config, const std::string& filepath);
    static bool importFromYAML(Config& config, const std::string& filepath);
    static bool importFromINI(Config& config, const std::string& filepath);
    
    // Configuration validation
    static std::vector<std::string> validateConfig(const Config& config);
    static bool checkRequiredKeys(const Config& config, const std::vector<std::string>& requiredKeys);
    
    // Configuration statistics
    static std::string getConfigStats(const Config& config);
    static std::string getMemoryUsage(const Config& config);
    
    // Configuration comparison
    static bool compareConfigs(const Config& config1, const Config& config2);
    static double configDifferenceScore(const Config& config1, const Config& config2);
    
    // Safe configuration operations
    static bool safeSet(Config& config, const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    static bool safeGetString(const Config& config, const std::string& key, std::string& value, const std::string& defaultValue = "");
    static bool safeGetInt(const Config& config, const std::string& key, int& value, int defaultValue = 0);
    static bool safeGetDouble(const Config& config, const std::string& key, double& value, double defaultValue = 0.0);
    static bool safeGetBool(const Config& config, const std::string& key, bool& value, bool defaultValue = false);
};

// Utility functions for brain management
class BrainUtilities {
public:
    // Brain state management
    static bool saveBrainState(const std::shared_ptr<Brain>& brain, const std::string& filepath);
    static bool loadBrainState(std::shared_ptr<Brain>& brain, const std::string& filepath);
    static bool backupBrainState(const std::shared_ptr<Brain>& brain, const std::string& backupPath);
    
    // Brain analysis and diagnostics
    static std::string analyzeBrainState(const std::shared_ptr<Brain>& brain);
    static std::string getBrainHealthReport(const std::shared_ptr<Brain>& brain);
    static std::string getPerformanceMetrics(const std::shared_ptr<Brain>& brain);
    
    // Brain validation
    static std::vector<std::string> validateBrain(const std::shared_ptr<Brain>& brain);
    static bool checkBrainIntegrity(const std::shared_ptr<Brain>& brain);
    static bool checkMemoryIntegrity(const std::shared_ptr<Brain>& brain);
    
    // Brain utilities
    static size_t countActiveNeurons(const std::shared_ptr<Brain>& brain);
    static size_t countTotalSpikes(const std::shared_ptr<Brain>& brain);
    static double getAverageFiringRate(const std::shared_ptr<Brain>& brain);
    static std::string getDevelopmentalStageString(const std::shared_ptr<Brain>& brain);
};

// Utility functions for logging and monitoring
class LoggingUtilities {
public:
    // Log file management
    static bool setupLogFile(const std::string& filepath, LogLevel level = LogLevel::Info);
    static bool setupLogRotation(const std::string& baseFilename, size_t maxFileSizeMB = 100, int maxBackups = 10);
    static bool logToFile(const std::string& message, const std::string& level = "INFO");
    
    // Performance logging
    static void logPerformance(const std::string& operation, std::chrono::duration<double> duration, double memoryMB = 0.0);
    static void logBenchmark(const std::string& benchmarkName, const std::vector<double>& metrics);
    
    // Log analysis
    static std::string analyzeLogFile(const std::string& filepath);
    static std::string extractLogErrors(const std::string& filepath);
    static std::string extractLogPerformance(const std::string& filepath);
    
    // Log utilities
    static std::string formatLogMessage(const std::string& level, const std::string& message);
    static std::string getCurrentTimestamp();
    static std::string getLogLevelString(LogLevel level);
};

// Utility functions for simulation control
class SimulationUtilities {
public:
    // Simulation control
    static bool runSimulationPhase(const std::shared_ptr<Brain>& brain, const std::shared_ptr<SimpleWorld>& world, int steps);
    static bool runCustomSimulation(const std::shared_ptr<Brain>& brain, const std::shared_ptr<SimpleWorld>& world, 
                                   const std::vector<std::pair<std::string, std::string>>& stimuli, int steps);
    static bool runTrainingEpisode(const std::shared_ptr<Brain>& brain, const std::shared_ptr<SimpleWorld>& world, 
                                  int maxSteps, float minReward = 0.0f);
    
    // Simulation analysis
    static std::string analyzeSimulationResults(const std::shared_ptr<Brain>& brain, const std::shared_ptr<SimpleWorld>& world);
    static std::string getRewardHistory(const std::shared_ptr<SimpleWorld>& world);
    static std::string getActionStatistics(const std::shared_ptr<SimpleWorld>& world);
    
    // Simulation utilities
    static float calculateCumulativeReward(const std::shared_ptr<SimpleWorld>& world);
    static size_t countUniqueStates(const std::shared_ptr<Brain>& brain, const std::shared_ptr<SimpleWorld>& world);
    static std::string getSimulationSummary(const std::shared_ptr<Brain>& brain, const std::shared_ptr<SimpleWorld>& world);
};

// Utility functions for file management
class FileUtilities {
public:
    // File existence and properties
    static bool fileExists(const std::string& filepath);
    static bool isDirectory(const std::string& path);
    static uint64_t getFileSize(const std::string& filepath);
    static std::string getFileExtension(const std::string& filepath);
    
    // Directory operations
    static std::vector<std::string> listFiles(const std::string& directory, const std::string& extension = "");
    static bool createDirectory(const std::string& path);
    static bool deleteFile(const std::string& filepath);
    static bool copyFile(const std::string& source, const std::string& destination);
    
    // Path manipulation
    static std::string getAbsolutePath(const std::string& path);
    static std::string getRelativePath(const std::string& path, const std::string& base);
    static std::string sanitizeFilename(const std::string& filename);
    
    // File content operations
    static std::string readFile(const std::string& filepath);
    static bool writeFile(const std::string& filepath, const std::string& content);
    static std::vector<std::string> readLines(const std::string& filepath);
    static bool writeLines(const std::string& filepath, const std::vector<std::string>& lines);
};

// Command-line interface utilities
class CommandLineUtilities {
public:
    // Command-line argument parsing
    static std::vector<std::string> parseArguments(int argc, char** argv);
    static std::string getArgumentValue(const std::vector<std::string>& args, const std::string& flag);
    static bool hasArgument(const std::vector<std::string>& args, const std::string& flag);
    
    // Help and usage
    static void printHelp(const std::string& programName, const std::string& description);
    static void printVersion(const std::string& version);
    static void printUsage(const std::string& programName);
    
    // Command validation
    static bool validateCommand(const std::string& command);
    static bool validateFilePath(const std::string& filepath);
    static bool validateInteger(const std::string& value, int& result, int min = INT_MIN, int max = INT_MAX);
    static bool validateFloat(const std::string& value, double& result, double min = -std::numeric_limits<double>::max(), 
                            double max = std::numeric_limits<double>::max());
    
    // Error handling
    static void printError(const std::string& message);
    static void printWarning(const std::string& message);
    static void printInfo(const std::string& message);
    static void printSuccess(const std::string& message);
};

// Performance monitoring utilities
class PerformanceUtilities {
public:
    // Timing utilities
    static std::chrono::duration<double> measureTime(std::function<void()> func);
    static std::chrono::duration<double> startTimer();
    static std::chrono::duration<double> stopTimer(std::chrono::duration<double> start);
    
    // Memory monitoring
    static double getCurrentMemoryUsage();
    static double getPeakMemoryUsage();
    static void logMemoryUsage(const std::string& label = "Memory");
    
    // Performance profiling
    static void startProfiler(const std::string& label);
    static void stopProfiler(const std::string& label);
    static void printProfilerReport();
    static void clearProfiler();
    
    // Benchmark utilities
    static void runBenchmark(const std::string& benchmarkName, std::function<void()> func, int iterations = 100);
    static std::vector<double> benchmarkMultiple(const std::string& benchmarkName, std::function<void()> func, int iterations = 100);
};

// Debug and diagnostics utilities
class DebugUtilities {
public:
    // Memory debugging
    static void checkMemoryLeaks();
    static void checkMemoryCorruption();
    static void analyzeMemoryFragments();
    
    // Thread debugging
    static void checkThreadDeadlocks();
    static void checkThreadStarvation();
    static void analyzeThreadUsage();
    
    // Resource monitoring
    static void checkFileHandles();
    static void checkNetworkConnections();
    static void checkSystemResources();
    
    // Application state
    static void dumpApplicationState();
    static void dumpSystemInfo();
    static void dumpEnvironmentInfo();
    
    // Debug utilities
    static std::string getStackTrace();
    static void triggerDebugBreak();
    static void setDebugLevel(int level);
    static int getDebugLevel();
};

} // namespace nlm

#endif // NLM_UTILITIES_HPP
