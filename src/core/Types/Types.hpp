// Code Quality Improvements - Header File
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <functional>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace nlm {

// Type aliases for better code clarity
typedef int64_t Timestamp;
typedef double TimeStep;
typedef float Weight;
typedef int SimulationStep;

// Error codes for better error handling
enum class ErrorCode {
    Success = 0,
    InvalidArgument = 1,
    NotInitialized = 2,
    MemoryError = 3,
    ConfigurationError = 4,
    FileError = 5,
    NetworkError = 6,
    Unknown = 99
};

// Error information structure
struct ErrorInfo {
    ErrorCode code;
    std::string message;
    std::string context;
    std::map<std::string, std::string> details;
    
    ErrorInfo(ErrorCode c, const std::string& msg, const std::string& ctx = "")
        : code(c), message(msg), context(ctx) {}
    
    void addDetail(const std::string& key, const std::string& value) {
        details[key] = value;
    }
};

// Exception class for better error handling
class NLMException : public std::runtime_error {
public:
    NLMException(const ErrorInfo& errorInfo)
        : std::runtime_error(errorInfo.message), errorInfo_(errorInfo) {}
    
    const ErrorInfo& getErrorInfo() const { return errorInfo_; }
    
private:
    ErrorInfo errorInfo_;
};

// Debugging and profiling utilities
class DebugLogger {
public:
    enum class Level {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };
    
    struct LogEntry {
        Level level;
        Timestamp timestamp;
        std::string context;
        std::string message;
        std::map<std::string, std::string> metadata;
    };
    
    static DebugLogger& getInstance() {
        static DebugLogger instance;
        return instance;
    }
    
    void setLevel(Level level) { currentLevel_ = level; }
    Level getLevel() const { return currentLevel_; }
    
    void log(Level level, const std::string& context, const std::string& message);
    void logError(const ErrorInfo& error);
    
    void enableProfiling() { profilingEnabled_ = true; }
    void disableProfiling() { profilingEnabled_ = false; }
    bool isProfilingEnabled() const { return profilingEnabled_; }
    
    void startTimer(const std::string& operation);
    void stopTimer(const std::string& operation);
    double getElapsedTime(const std::string& operation) const;
    
    void exportLog(const std::string& filename) const;
    
private:
    DebugLogger() : currentLevel_(Level::INFO), profilingEnabled_(false) {}
    
    Level currentLevel_;
    bool profilingEnabled_;
    std::vector<LogEntry> logEntries_;
    std::map<std::string, std::chrono::high_resolution_clock::time_point> timers_;
    std::map<std::string, double> elapsedTimes_;
};

// Configuration validation and schema
class ConfigValidator {
public:
    struct ValidationRule {
        std::string key;
        std::string type; // "integer", "float", "string", "boolean", "array"
        std::string pattern; // Regex pattern for validation
        double minValue; // For numeric types
        double maxValue;
        bool required;
        std::vector<std::string> allowedValues;
        std::string description;
    };
    
    static bool validate(const std::map<std::string, std::string>& config,
                        const std::vector<ValidationRule>& rules,
                        std::vector<std::string>& errors);
    
    static std::string sanitizeString(const std::string& input);
    static bool isValidKey(const std::string& key);
    static bool isValidConfigName(const std::string& name);
};

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        std::string operation;
        Timestamp startTime;
        Timestamp endTime;
        double duration; // in milliseconds
        uint64_t memoryUsage; // in bytes
        std::map<std::string, std::string> metrics;
    };
    
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    void recordOperation(const std::string& operation, uint64_t memoryBefore = 0, uint64_t memoryAfter = 0);
    void addCustomMetric(const std::string& key, const std::string& value);
    
    double getAverageTime(const std::string& operation) const;
    uint64_t getPeakMemoryUsage() const;
    size_t getOperationCount(const std::string& operation) const;
    
    void exportMetrics(const std::string& filename) const;
    
    void clear();
    
private:
    PerformanceMonitor() = default;
    
    std::vector<PerformanceMetrics> metrics_;
    std::unordered_map<std::string, std::vector<double>> operationTimes_;
    std::map<std::string, size_t> operationCounts_;
};

// Utility functions for better code quality
namespace utils {
    // String utilities
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    std::string capitalize(const std::string& str);
    std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
    
    // Numeric utilities
    template<typename T>
    T clamp(T value, T min, T max);
    
    template<typename T>
    bool inRange(T value, T min, T max);
    
    // Array and container utilities
    template<typename T>
    size_t safeIndex(size_t index, size_t maxSize, size_t defaultIndex = 0);
    
    // Random number generation with better entropy
    class RandomGenerator {
    public:
        RandomGenerator() : seed_(std::chrono::high_resolution_clock::now().time_since_epoch().count()) {}
        RandomGenerator(uint64_t seed) : seed_(seed) {}
        
        void setSeed(uint64_t seed) { seed_ = seed; }
        uint64_t getSeed() const { return seed_; }
        
        float uniformReal(float min = 0.0f, float max = 1.0f);
        int uniformInt(int min = 0, int max = 100);
        double normalDistribution(double mean = 0.0, double stddev = 1.0);
        
    private:
        uint64_t seed_;
    };
    
    // File operations
    bool fileExists(const std::string& path);
    bool directoryExists(const std::string& path);
    std::string getFileExtension(const std::string& filename);
    std::string getFileNameWithoutExtension(const std::string& filename);
    
    // Math utilities for neural computations
    template<typename T>
    T sigmoid(T x);
    
    template<typename T>
    T relu(T x);
    
    template<typename T>
    T leakyRelu(T x, T alpha = 0.01f);
    
    template<typename T>
    T deriveSigmoid(T x);
    
    template<typename T>
    T deriveReLU(T x);
    
    template<typename T>
    T gaussian(T x, T mean, T sigma);
} // namespace utils

// Constants for better code maintainability
namespace constants {
    // Default configuration values
    constexpr int DEFAULT_NEURON_COUNT = 1000;
    constexpr int DEFAULT_REGION_COUNT = 2;
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    constexpr float DEFAULT_SIMULATION_TIMESTEP = 0.001f;
    constexpr int DEFAULT_RANDOM_SEED = 42;
    
    // Neural network parameters
    constexpr float DEFAULT_V_TH = -50.0f;  // Threshold potential
    constexpr float DEFAULT_V_REST = -70.0f; // Resting potential
    constexpr float DEFAULT_V_RESET = -75.0f; // Reset potential
    constexpr float DEFAULT_TAU_MEM = 20.0f; // Membrane time constant
    constexpr float DEFAULT_TAU_REF = 2.0f; // Refractory period
    
    // Plasticity parameters
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
    constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;
    constexpr float DEFAULT_HEBBIAN_LEARNING_RATE = 0.01f;
    constexpr float DEFAULT_STRUCTURED Plasticity_RATE = 0.0001f;
    
    // Development parameters
    constexpr float DEFAULT_DEVELOPMENT_SYNAPTOGENESIS_RATE = 0.001f;
    constexpr float DEFAULT_DEVELOPMENT_PRUNING_RATE = 0.0001f;
    
    // Neuromodulation parameters
    constexpr float DEFAULT_DOPAMINE_BASELINE = 0.1f;
    constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.5f;
    
    // World parameters
    constexpr int DEFAULT_WORLD_WIDTH = 20;
    constexpr int DEFAULT_WORLD_HEIGHT = 20;
    constexpr float DEFAULT_VISION_WIDTH = 8.0f;
    constexpr float DEFAULT_VISION_HEIGHT = 8.0f;
    
    // Debug and logging
    constexpr utils::DebugLogger::Level DEFAULT_LOG_LEVEL = utils::DebugLogger::Level::INFO;
    constexpr bool DEFAULT_VERBOSE_LOGGING = false;
    constexpr bool DEFAULT_ENABLE_PROFILING = false;
    constexpr size_t DEFAULT_LOG_BUFFER_SIZE = 1024;
    
    // Performance
    constexpr size_t DEFAULT_PERFORMANCE_SAMPLE_SIZE = 100;
    constexpr double DEFAULT_PERFORMANCE_THRESHOLD = 100.0; // milliseconds
    
    // Memory management
    constexpr size_t DEFAULT_MEMORY_POOL_SIZE = 1024 * 1024; // 1MB
    constexpr float MEMORY_POOL_GROWTH_FACTOR = 1.5f;
    
    // Safety limits
    constexpr size_t MAX_NEURON_COUNT = 100000;
    constexpr size_t MAX_SYNAPSE_COUNT = 1000000;
    constexpr float MAX_WEIGHT = 100.0f;
    constexpr float MIN_WEIGHT = -100.0f;
    constexpr float MAX_SIMULATION_STEPS = 1000000;
} // namespace constants

// Forward declarations
class Brain;
class Config;
class Environment;

// Abstract base class for pluggable components
class PluggableComponent {
public:
    virtual ~PluggableComponent() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
};

// Plugin manager for extensibility
class PluginManager {
public:
    static PluginManager& getInstance() {
        static PluginManager instance;
        return instance;
    }
    
    bool loadPlugin(const std::string& path);
    bool unloadPlugin(const std::string& name);
    bool isPluginLoaded(const std::string& name) const;
    
    std::vector<std::string> getLoadedPlugins() const;
    std::vector<std::string> getAvailablePlugins() const;
    
    template<typename T>
    std::shared_ptr<T> getPlugin(const std::string& name);
    
    bool registerComponent(std::shared_ptr<PluggableComponent> component);
    std::vector<std::shared_ptr<PluggableComponent>> getComponents() const;
    
private:
    PluginManager() = default;
    
    std::map<std::string, std::shared_ptr<PluggableComponent>> components_;
    std::map<std::string, std::string> pluginPaths_;
};

// Logging utilities
#define NLM_LOG_ERROR(msg) DebugLogger::getInstance().log(DebugLogger::Level::ERROR, "NLM", msg)
#define NLM_LOG_WARNING(msg) DebugLogger::getInstance().log(DebugLogger::Level::WARNING, "NLM", msg)
#define NLM_LOG_INFO(msg) DebugLogger::getInstance().log(DebugLogger::Level::INFO, "NLM", msg)
#define NLM_LOG_DEBUG(msg) DebugLogger::getInstance().log(DebugLogger::Level::DEBUG, "NLM", msg)

#define NLM_THROW_ERROR(code, msg, context) \
    do { \
        ErrorInfo error(code, msg, context); \
        throw NLMException(error); \
    } while(0)

#define NLM_CHECK(condition, code, msg) \
    do { \
        if (!(condition)) { \
            NLM_THROW_ERROR(code, msg, #condition); \
        } \
    } while(0)

#define NLM_CHECK_RANGE(value, min, max, code, msg) \
    do { \
        if (!(value >= min && value <= max)) { \
            NLM_THROW_ERROR(code, msg, "Range check failed"); \
        } \
    } while(0)

} // namespace nlm