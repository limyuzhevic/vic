// NLM Utilities - Core Helper Functions
//
// This module provides comprehensive utility functions to make the NLM codebase
// easier to use, extend, and maintain. These utilities are designed to support
// both simple use cases and advanced features for power users.

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <optional>
#include <functional>

// Forward declarations
namespace nlm {
    class Logger;
}

namespace nlm {
namespace utils {

// File system utilities
class FileSystem {
public:
    // Safe file operations with error handling
    static bool fileExists(const std::string& path);
    static bool directoryExists(const std::string& path);
    static bool createDirectory(const std::string& path, bool recursive = true);
    static bool removeFile(const std::string& path);
    static bool copyFile(const std::string& source, const std::string& destination);
    static std::string getFileExtension(const std::string& filename);
    static std::string getFileNameWithoutExtension(const std::string& filename);
    static std::string getParentDirectory(const std::string& path);
    static std::string getTempDirectory();
    static std::string generateUniqueFilename(const std::string& prefix = "", 
                                             const std::string& extension = "");
    
    // Directory operations
    static std::vector<std::string> getFilesInDirectory(const std::string& directory, 
                                                         const std::string& extension = "");
    static std::vector<std::string> getSubdirectories(const std::string& directory);
    static bool isDirectory(const std::string& path);
    
    // File content operations
    static std::string readFile(const std::string& filepath);
    static bool writeFile(const std::string& filepath, const std::string& content, 
                         bool append = false);
    static std::vector<std::string> readFileLines(const std::string& filepath);
    static bool writeFileLines(const std::string& filepath, 
                               const std::vector<std::string>& lines);
};

// String utilities
class StringUtils {
public:
    // String manipulation
    static std::string trim(const std::string& str, bool left = true, bool right = true);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string capitalize(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
    static size_t countOccurrences(const std::string& str, const std::string& substring);
    
    // String validation
    static bool isValidFilename(const std::string& filename);
    static bool isAlphaNumeric(const std::string& str);
    static bool isWhitespace(const std::string& str);
    
    // Formatting
    static std::string formatNumber(int number, const std::string& locale = "");
    static std::string formatFloat(float value, int precision = 6);
    static std::string formatPercentage(float value, int precision = 2);
    static std::string formatTime(double seconds);
    
    // Hash and encoding
    static std::string hashString(const std::string& str, const std::string& algorithm = "md5");
    static std::string urlEncode(const std::string& str);
    static std::string urlDecode(const std::string& str);
    static std::string base64Encode(const std::string& str);
    static std::string base64Decode(const std::string& str);
};

// Math utilities
class MathUtils {
public:
    // Basic mathematical operations
    static double clamp(double value, double min, double max);
    static double lerp(double a, double b, double t);
    static double mapValue(double value, double inMin, double inMax, double outMin, double outMax);
    static double smoothStep(double edge0, double edge1, double x);
    
    // Statistical functions
    static double mean(const std::vector<double>& values);
    static double median(const std::vector<double>& values);
    static double variance(const std::vector<double>& values);
    static double standardDeviation(const std::vector<double>& values);
    static double percentile(const std::vector<double>& values, double percentile);
    
    // Vector operations
    static double magnitude(const std::vector<double>& vector);
    static double dotProduct(const std::vector<double>& a, const std::vector<double>& b);
    static double crossProduct(const std::vector<double>& a, const std::vector<double>& b);
    static std::vector<double> normalize(const std::vector<double>& vector);
    static double distance(const std::vector<double>& a, const std::vector<double>& b);
    
    // Probability and random utilities
    static double randomDouble(double min = 0.0, double max = 1.0);
    static int randomInt(int min = 0, int max = 100);
    static bool randomBool(double probability = 0.5);
    static std::vector<int> randomSample(const std::vector<int>& population, 
                                        size_t sampleSize, bool withReplacement = false);
    
    // Array operations
    static std::vector<double> normalizeArray(const std::vector<double>& values, 
                                              double targetMean = 0.0, double targetStd = 1.0);
    static std::vector<double> smoothArray(const std::vector<double>& values, 
                                           size_t windowSize = 3);
    static double calculateMovingAverage(const std::vector<double>& values, 
                                        size_t windowSize);
    
    // Special functions
    static double sigmoid(double x);
    static double relu(double x);
    static double elu(double x, double alpha = 1.0);
    static double softplus(double x);
    static double gaussian(double x, double mean = 0.0, double sigma = 1.0);
};

// Memory management utilities
class MemoryUtils {
public:
    // Memory allocation and tracking
    static void* allocateMemory(size_t size, const std::string& debugName = "");
    static void deallocateMemory(void* ptr);
    static size_t getMemoryUsage(); // in bytes
    static size_t getPeakMemoryUsage(); // peak usage in bytes
    static void resetMemoryTracking();
    
    // Smart pointer utilities
    template<typename T, typename... Args>
    static std::unique_ptr<T> makeUnique(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
    
    template<typename T, typename U>
    static std::shared_ptr<T> staticPointerCast(std::shared_ptr<U>&& ptr) {
        return std::static_pointer_cast<T>(std::move(ptr));
    }
    
    template<typename T, typename U>
    static std::shared_ptr<T> dynamicPointerCast(std::shared_ptr<U>&& ptr) {
        return std::dynamic_pointer_cast<T>(std::move(ptr));
    }
    
    // Container utilities
    template<typename Container>
    static size_t safeSize(const Container& container) {
        try {
            return container.size();
        } catch (const std::exception&) {
            return 0;
        }
    }
    
    template<typename Container>
    static bool safeEmpty(const Container& container) {
        try {
            return container.empty();
        } catch (const std::exception&) {
            return true;
        }
    }
    
    template<typename Container>
    static auto safeBegin(const Container& container) {
        try {
            return container.begin();
        } catch (const std::exception&) {
            return container.begin();
        }
    }
    
    template<typename Container>
    static auto safeEnd(const Container& container) {
        try {
            return container.end();
        } catch (const std::exception&) {
            return container.end();
        }
    }
    
    // Resource management
    static std::string getResourceUsageInfo();
};

// Performance monitoring utilities
class PerformanceMonitor {
public:
    // Timing utilities
    class Timer {
    public:
        Timer();
        ~Timer();
        double elapsed() const; // in seconds
        void reset();
        std::string elapsedString() const;
        
    private:
        std::chrono::high_resolution_clock::time_point startTime;
    };
    
    // Performance metrics
    struct PerformanceMetrics {
        double cpuUsage;           // Percentage
        double memoryUsage;        // Percentage
        double memoryAvailable;   // Bytes
        double diskUsage;          // Percentage
        double networkUsage;       // Bytes per second
        size_t activeThreads;      // Number of active threads
        double responseTime;       // Milliseconds
        double throughput;         // Operations per second
    };
    
    // Static methods
    static PerformanceMetrics getPerformanceMetrics();
    static void startMonitoring();
    static void stopMonitoring();
    static bool isMonitoring();
    static std::string getPerformanceReport();
    
    // Benchmark utilities
    static void benchmark(const std::string& name, 
                         std::function<void()> operation,
                         int iterations = 100);
    static void benchmark(const std::string& name,
                         std::function<double()> operation,
                         int iterations = 100);
    static std::vector<double> profileOperation(std::function<void()> operation,
                                               size_t numSamples = 10);
    
    // Memory profiling
    static void startMemoryProfiling();
    static void stopMemoryProfiling();
    static std::string getMemoryProfileReport();
    
    // CPU profiling
    static void startCPUProfiling();
    static void stopCPUProfiling();
    static std::string getCPUProfileReport();
    
    // Event tracking
    static void trackEvent(const std::string& eventName, 
                          const std::unordered_map<std::string, std::string>& metadata = {});
    static std::string getEventLog();
    static void clearEventLog();
    
    // Performance thresholds
    static void setPerformanceThresholds(double maxCpuUsage = 80.0,
                                         double maxMemoryUsage = 80.0,
                                         double maxResponseTime = 100.0);
    static bool checkPerformanceThresholds();
    static std::string getPerformanceThresholdsInfo();
};

// Configuration utilities
class ConfigUtils {
public:
    // Configuration validation
    static bool validateConfiguration(const std::string& configFile);
    static std::string validateConfiguration(const std::unordered_map<std::string, std::string>& config);
    static std::string getConfigurationErrors(const std::string& configFile);
    
    // Configuration merging
    static std::unordered_map<std::string, std::string> mergeConfigurations(
        const std::unordered_map<std::string, std::string>& base,
        const std::unordered_map<std::string, std::string>& override);
    static std::unordered_map<std::string, std::string> mergeConfigurations(
        const std::string& file1, const std::string& file2);
    
    // Configuration template management
    static bool createConfigTemplate(const std::string& templateFile,
                                     const std::string& outputFile);
    static std::unordered_map<std::string, std::string> loadConfigFromTemplate(
        const std::string& templateFile);
    static bool validateConfigAgainstTemplate(
        const std::unordered_map<std::string, std::string>& config,
        const std::string& templateFile);
    
    // Configuration backup and restore
    static bool backupConfiguration(const std::string& configFile, 
                                    const std::string& backupDir = "backups");
    static bool restoreConfiguration(const std::string& configFile,
                                     const std::string& backupFile);
    static std::vector<std::string> listBackups(const std::string& backupDir);
    
    // Configuration comparison
    static bool compareConfigurations(const std::string& configFile1,
                                     const std::string& configFile2);
    static std::string diffConfigurations(const std::string& configFile1,
                                          const std::string& configFile2);
    
    // Configuration statistics
    static std::string getConfigurationStatistics(const std::string& configFile);
    static std::string getConfigurationHealthReport(const std::string& configFile);
    
    // Configuration export/import
    static bool exportConfiguration(const std::string& configFile,
                                    const std::string& exportFile);
    static bool importConfiguration(const std::string& configFile,
                                    const std::string& exportFile);
    static std::string getConfigurationVersion(const std::string& configFile);
    static bool updateConfigurationVersion(const std::string& configFile,
                                           const std::string& version);
};

// Error handling utilities
class ErrorUtils {
public:
    // Error types
    enum class ErrorCode {
        SUCCESS = 0,
        FILE_NOT_FOUND = 1,
        INVALID_ARGUMENT = 2,
        OUT_OF_MEMORY = 3,
        NETWORK_ERROR = 4,
        TIMEOUT = 5,
        PERMISSION_DENIED = 6,
        INVALID_FORMAT = 7,
        UNKNOWN_ERROR = 8
    };
    
    // Error handling
    static std::string getErrorMessage(ErrorCode code);
    static std::string getSystemErrorMessage();
    static bool throwOnError(ErrorCode code, const std::string& message = "");
    static bool logAndContinue(ErrorCode code, const std::string& message = "");
    
    // Error recovery
    static bool attemptRecovery(ErrorCode code, 
                               std::function<bool()> recoveryAction,
                               int maxAttempts = 3);
    static std::vector<ErrorCode> getRecentErrors();
    static void clearErrorHistory();
    
    // Error validation
    static bool isRecoverable(ErrorCode code);
    static bool isCritical(ErrorCode code);
    static std::string getErrorCategory(ErrorCode code);
};

// Template utilities
class TemplateUtils {
public:
    // Template processing
    static std::string processTemplate(const std::string& templateContent,
                                       const std::unordered_map<std::string, std::string>& variables);
    static std::string processTemplate(const std::string& templateFile,
                                       const std::unordered_map<std::string, std::string>& variables);
    
    // Template generation
    static bool generateFromTemplate(const std::string& templateFile,
                                     const std::string& outputFile,
                                     const std::unordered_map<std::string, std::string>& variables);
    static std::unordered_map<std::string, std::string> extractTemplateVariables(
        const std::string& templateContent);
    
    // Template validation
    static bool validateTemplate(const std::string& templateContent);
    static std::vector<std::string> getTemplateVariables(const std::string& templateContent);
    static bool validateTemplateVariables(const std::string& templateContent,
                                          const std::unordered_map<std::string, std::string>& variables);
    
    // Template caching
    static void cacheTemplate(const std::string& name, const std::string& content);
    static std::string getCachedTemplate(const std::string& name);
    static bool isTemplateCached(const std::string& name);
    static void clearTemplateCache();
};

// Registry and factory utilities
class RegistryUtils {
public:
    // Plugin registry
    static bool registerPlugin(const std::string& name,
                               std::function<void*()> factory,
                               const std::string& version = "1.0.0");
    static bool unregisterPlugin(const std::string& name);
    static void* createPlugin(const std::string& name);
    static bool destroyPlugin(void* instance);
    static std::vector<std::string> listPlugins();
    
    // Factory registry
    template<typename T>
    static bool registerFactory(const std::string& name,
                                std::function<std::unique_ptr<T>()> factory);
    
    template<typename T>
    static std::unique_ptr<T> createObject(const std::string& name);
    
    static std::vector<std::string> listFactories();
    
    // Component registry
    template<typename Interface, typename Implementation>
    static bool registerComponent();
    
    template<typename Interface>
    static std::unique_ptr<Interface> createComponent(const std::string& name);
    
    static std::vector<std::string> listComponents();
};

// Debug and development utilities
class DebugUtils {
public:
    // Debugging
    static void enableDebugMode(bool enable = true);
    static bool isDebugMode();
    static void setDebugLevel(int level);
    static int getDebugLevel();
    static std::string getDebugInfo();
    
    // Stack traces
    static std::string getStackTrace();
    static std::string getCurrentStackTrace();
    
    // Memory debugging
    static void enableMemoryLeakDetection(bool enable = true);
    static bool isMemoryLeakDetectionEnabled();
    static std::string getMemoryLeaks();
    static void clearMemoryLeaks();
    
    // Performance debugging
    static void enablePerformanceProfiling(bool enable = true);
    static bool isPerformanceProfilingEnabled();
    static std::string getPerformanceProfile();
    static void clearPerformanceProfile();
    
    // Logging utilities
    static void setLogFile(const std::string& logfile);
    static std::string getLogFile();
    static void enableConsoleLogging(bool enable = true);
    static bool isConsoleLoggingEnabled();
    static std::string getLogHistory();
    static void clearLogHistory();
    
    // Validation utilities
    static bool validateAllModules();
    static std::string getValidationReport();
    static bool runSelfTest();
    static std::string getSelfTestReport();
};

// Main utility functions for easy access
namespace {
    // Global logger for utilities
    inline std::shared_ptr<Logger> getLogger() {
        static std::shared_ptr<Logger> logger;
        if (!logger) {
            logger = std::make_shared<Logger>();
        }
        return logger;
    }
}

// Convenience functions
inline std::string formatTime(double seconds) {
    return MathUtils::formatTime(seconds);
}

inline double clamp(double value, double min, double max) {
    return MathUtils::clamp(value, min, max);
}

inline std::string trim(const std::string& str) {
    return StringUtils::trim(str);
}

inline bool fileExists(const std::string& path) {
    return FileSystem::fileExists(path);
}

inline bool directoryExists(const std::string& path) {
    return FileSystem::directoryExists(path);
}

inline std::vector<std::string> readFileLines(const std::string& filepath) {
    return FileSystem::readFileLines(filepath);
}

inline bool writeFileLines(const std::string& filepath, 
                          const std::vector<std::string>& lines) {
    return FileSystem::writeFileLines(filepath, lines);
}

inline std::string getTempDirectory() {
    return FileSystem::getTempDirectory();
}

inline std::string generateUniqueFilename(const std::string& prefix, const std::string& extension) {
    return FileSystem::generateUniqueFilename(prefix, extension);
}

inline double randomDouble(double min, double max) {
    return MathUtils::randomDouble(min, max);
}

inline std::vector<double> randomSample(const std::vector<double>& population, 
                                       size_t sampleSize) {
    return MathUtils::randomSample(population, sampleSize);
}

inline std::string getResourceUsageInfo() {
    return MemoryUtils::getResourceUsageInfo();
}

inline PerformanceMonitor::Timer createTimer() {
    return PerformanceMonitor::Timer();
}

inline void benchmark(const std::string& name, std::function<void()> operation, 
                     int iterations = 100) {
    PerformanceMonitor::benchmark(name, operation, iterations);
}

inline bool validateConfiguration(const std::string& configFile) {
    return ConfigUtils::validateConfiguration(configFile);
}

inline std::string getConfigurationErrors(const std::string& configFile) {
    return ConfigUtils::getConfigurationErrors(configFile);
}

} // namespace utils

} // namespace nlm

// Include implementation files (they would be in separate .cpp files)
// #include "Utils/FileSystem.cpp"
// #include "Utils/StringUtils.cpp"
// #include "Utils/MathUtils.cpp"
// #include "Utils/MemoryUtils.cpp"
// #include "Utils/PerformanceMonitor.cpp"
// #include "Utils/ConfigUtils.cpp"
// #include "Utils/ErrorUtils.cpp"
// #include "Utils/TemplateUtils.cpp"
// #include "Utils/RegistryUtils.cpp"
// #include "Utils/DebugUtils.cpp"

#endif // NLM_UTILS_H
