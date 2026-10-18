// Error handling and logging improvements for NLM
// Provides comprehensive error classes and logging utilities
#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <chrono>
#include <sstream>

namespace nlm {

// Error classes
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) 
        : std::runtime_error("NLM Error: " + message) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message)
        : NLMError("Configuration Error: " + message) {}
};

class InitializationError : public NLMError {
public:
    explicit InitializationError(const std::string& message)
        : NLMError("Initialization Error: " + message) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& message)
        : NLMError("Simulation Error: " + message) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message)
        : NLMError("Memory Error: " + message) {}
};

// Log levels
enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

// Formatted string utilities
class StringUtils {
public:
    static std::string format(const char* format, ...);
    static std::string toString(double value, int precision = 6);
    static std::string toString(float value, int precision = 6);
    static std::string toString(int64_t value);
    static std::string toString(size_t value);
    static std::string toString(bool value);
};

// Performance timing utilities
class PerformanceTimer {
public:
    PerformanceTimer();
    double elapsed() const;
    void reset();
private:
    std::chrono::high_resolution_clock::time_point start_;
};

// Memory pool utilities (for performance)
template<typename T>
class MemoryPool {
public:
    MemoryPool(size_t capacity = 1000);
    T* allocate();
    void deallocate(T* ptr);
    size_t capacity() const { return capacity_; }
    size_t used() const { return used_; }
    double utilization() const { return static_cast<double>(used_) / capacity_; }
private:
    T* pool_;
    size_t capacity_;
    size_t used_;
    size_t head_;
};

// Array utilities
class ArrayUtils {
public:
    template<typename T>
    static void safeCopy(const T* src, T* dst, size_t count, const T& defaultValue);
    
    template<typename T>
    static void safeFill(T* array, T value, size_t count);
    
    template<typename T>
    static T clamp(T value, T min, T max);
    
    template<typename T>
    static bool isNaN(const T& value);
    
    template<typename T>
    static bool isInfinity(const T& value);
    
    template<typename T>
    static T mean(const T* array, size_t count);
    
    template<typename T>
    static T variance(const T* array, size_t count, T mean);
};

// Validation utilities
class ValidationUtils {
public:
    static bool isValidNeuronType(NeuronType type);
    static bool isValidSynapseType(SynapseType type);
    static bool isValidActionType(ActionType type);
    static bool isValid DevelopmentalStage(DevelopmentalStage stage);
    static bool isValidLogLevel(LogLevel level);
    static bool isPositive(double value);
    static bool isNonNegative(double value);
    static bool isInRange(double value, double min, double max);
    static bool isFinite(double value);
    static bool isNormal(double value);
    
    template<typename T>
    static bool isValidIndex(T index, T size);
    
    static std::string validateConfig(const Config& config);
    static std::string validateBrainState(const Brain* brain);
};

// Callback utilities
class CallbackManager {
public:
    using Callback = std::function<void()>;
    
    CallbackManager();
    void addCallback(const Callback& callback);
    void removeCallback(const Callback& callback);
    void executeAll();
    void clear();
    size_t count() const { return callbacks_.size(); }
private:
    std::vector<Callback> callbacks_;
};

// Thread safety utilities
class ThreadUtils {
public:
    static void memoryBarrier();
    static void threadYield();
    static size_t getCPUCount();
    static bool isMultiCore();
};

// Version information
namespace Version {
    constexpr int MAJOR = 0;
    constexpr int MINOR = 1;
    constexpr int PATCH = 0;
    constexpr const char* STRING = "0.1.0";
    constexpr const char* FULL = "NLM 0.1.0";
}

// Platform detection
namespace Platform {
#ifdef _WIN32
    constexpr bool WINDOWS = true;
    constexpr bool LINUX = false;
    constexpr bool MACOS = false;
#elif __linux__
    constexpr bool WINDOWS = false;
    constexpr bool LINUX = true;
    constexpr bool MACOS = false;
#elif __APPLE__
    constexpr bool WINDOWS = false;
    constexpr bool LINUX = false;
    constexpr bool MACOS = true;
#else
    constexpr bool WINDOWS = false;
    constexpr bool LINUX = false;
    constexpr bool MACOS = false;
#endif

constexpr const char* getPlatformString();
}

// Build configuration
namespace Build {
#ifdef NDEBUG
    constexpr bool DEBUG = false;
#else
    constexpr bool DEBUG = true;
#endif

#ifdef NLM_ENABLE_ASSERTIONS
    constexpr bool ASSERTIONS = true;
#else
    constexpr bool ASSERTIONS = false;
#endif

#ifdef NLM_ENABLE_PROFILING
    constexpr bool PROFILING = true;
#else
    constexpr bool PROFILING = false;
#endif

constexpr bool ENABLE_EXCEPTIONS = true;
}

} // namespace nlm
