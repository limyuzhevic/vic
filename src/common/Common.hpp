#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>

namespace nlm {
namespace Common {

/**
 * Common data types and definitions for neural network simulations
 */
using Timestamp = double;
using Timestep = double;
using SimulationStep = uint64_t;
using NeuronId = uint32_t;
using RegionId = uint32_t;
using SynapseId = uint64_t;

/**
 * Basic scalar types with enhanced properties
 */
struct Scalar {
    double value;
    std::string unit;
    double min;
    double max;
    
    Scalar(double val = 0.0, const std::string& unitStr = "") 
        : value(val), unit(unitStr), min(-std::numeric_limits<double>::max()), 
          max(std::numeric_limits<double>::max()) {}
    
    Scalar(double val, double min, double max, const std::string& unitStr = "")
        : value(val), unit(unitStr), min(min), max(max) {}
    
    bool isValid() const {
        return value >= min && value <= max;
    }
    
    double clamp() const {
        return std::max(min, std::min(value, max));
    }
};

/**
 * Vector types with enhanced functionality
 */
struct Vector3 {
    double x, y, z;
    
    Vector3(double x = 0.0, double y = 0.0, double z = 0.0) : x(x), y(y), z(z) {}
    
    double magnitude() const {
        return std::sqrt(x*x + y*y + z*z);
    }
    
    Vector3 normalize() const {
        double mag = magnitude();
        return mag > 0.0 ? Vector3(x/mag, y/mag, z/mag) : Vector3();
    }
    
    double dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
};

struct Vector2 {
    double x, y;
    
    Vector2(double x = 0.0, double y = 0.0) : x(x), y(y) {}
    
    double magnitude() const {
        return std::sqrt(x*x + y*y);
    }
    
    Vector2 normalize() const {
        double mag = magnitude();
        return mag > 0.0 ? Vector2(x/mag, y/mag) : Vector2();
    }
    
    double dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }
};

/**
 * Configuration helpers and type conversions
 */
class ConfigHelper {
public:
    static std::string sanitizeKey(const std::string& key);
    static std::string formatKey(const std::string& prefix, const std::string& suffix);
    static std::string getNestedKey(const std::string& parent, const std::string& child);
    static std::vector<std::string> splitKey(const std::string& key);
    static std::string joinKeys(const std::vector<std::string>& keys);
    static bool isValidKeyFormat(const std::string& key);
    static std::string getDefaultUnit(const std::string& type);
    static double convertUnits(double value, const std::string& fromUnit, const std::string& toUnit);
};

/**
 * Error handling utilities
 */
enum class ErrorLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

struct ErrorInfo {
    ErrorLevel level;
    std::string code;
    std::string message;
    std::string context;
    Timestamp timestamp;
    std::vector<std::string> stackTrace;
    std::map<std::string, std::string> metadata;
    
    ErrorInfo(ErrorLevel lvl, const std::string& code, const std::string& msg,
              const std::string& ctx = "") 
        : level(lvl), code(code), message(msg), context(ctx), 
          timestamp(std::chrono::duration<double>::zero()) {}
};

class ErrorHandler {
public:
    using ErrorCallback = std::function<void(const ErrorInfo&)>;
    
    static void registerCallback(ErrorCallback callback);
    static void unregisterCallback(ErrorCallback callback);
    static void logError(const ErrorInfo& error);
    static void logError(ErrorLevel level, const std::string& code,
                         const std::string& message, const std::string& context = "");
    static std::vector<ErrorInfo> getRecentErrors(size_t maxCount = 100);
    static void clearErrors();
    static size_t getErrorCount();
    static std::string formatError(const ErrorInfo& error);
    static std::string formatErrorChain(const std::vector<ErrorInfo>& errors);
    static void setErrorLevel(ErrorLevel level);
    static ErrorLevel getErrorLevel();
};

/**
 * Performance monitoring utilities
 */
struct PerformanceMetrics {
    Timestamp startTime;
    Timestamp endTime;
    double cpuTime;
    double memoryUsage;
    size_t operationsCount;
    std::map<std::string, double> customMetrics;
    
    PerformanceMetrics() : cpuTime(0.0), memoryUsage(0.0), operationsCount(0) {}
    
    double getDuration() const {
        return endTime - startTime;
    }
    
    double getOperationsPerSecond() const {
        double duration = getDuration();
        return duration > 0.0 ? operationsCount / duration : 0.0;
    }
};

class PerformanceMonitor {
public:
    using MetricCallback = std::function<void(const PerformanceMetrics&)>;
    
    static void startMeasurement(const std::string& operation);
    static void stopMeasurement(const std::string& operation);
    static PerformanceMetrics getMetrics(const std::string& operation);
    static std::map<std::string, PerformanceMetrics> getAllMetrics();
    static void clearMetrics();
    static void registerMetricCallback(MetricCallback callback);
    static void unregisterMetricCallback(MetricCallback callback);
    static std::string formatMetrics(const PerformanceMetrics& metrics);
    static std::string generatePerformanceReport();
};

/**
 * Memory management utilities
 */
class MemoryManager {
public:
    static void* allocate(size_t size, const std::string& purpose = "");
    static void deallocate(void* ptr);
    static void* alignedAllocate(size_t size, size_t alignment, const std::string& purpose = "");
    static void alignedDeallocate(void* ptr);
    static size_t getAllocationCount();
    static size_t getTotalAllocated();
    static void printMemoryStats();
    static void enableTracking(bool enable);
    static bool isTrackingEnabled();
    static double getFragmentationRatio();
    template<typename T>
    static std::vector<T*> allocateArray(size_t count);
    template<typename T>
    static void deallocateArray(std::vector<T*>& pointers);
    static void optimizeMemory();
};

/**
 * Factory pattern base classes for object creation
 */
template<typename T>
class Factory {
public:
    virtual ~Factory() = default;
    virtual std::unique_ptr<T> create() = 0;
    virtual std::unique_ptr<T> create(const std::string& config) = 0;
};

/**
 * Observer pattern for event handling
 */
template<typename T>
class Observer {
public:
    virtual ~Observer() = default;
    virtual void onEvent(const T& event) = 0;
    virtual bool matches(const std::string& eventType) const = 0;
};

/**
 * Subject class for observer pattern
 */
template<typename T>
class Subject {
private:
    std::vector<std::unique_ptr<Observer<T>>> observers;
    
public:
    void attach(std::unique_ptr<Observer<T>> observer) {
        observers.push_back(std::move(observer));
    }
    
    void detach(Observer<T>* observer) {
        observers.erase(std::remove_if(observers.begin(), observers.end(),
            [observer](const std::unique_ptr<Observer<T>>& obs) {
                return obs.get() == observer;
            }), observers.end());
    }
    
    void notify(const T& event) {
        for (auto& observer : observers) {
            if (observer->matches(event.type)) {
                observer->onEvent(event);
            }
        }
    }
};

/**
 * Event types for neural network simulations
 */
struct NeuralNetworkEvent {
    std::string type;
    Timestamp timestamp;
    std::map<std::string, std::string> data;
    std::string source;
    
    NeuralNetworkEvent(const std::string& type, const std::string& source = "")
        : type(type), timestamp(std::chrono::duration<double>::zero()), source(source) {}
};

/**
 * Strategy pattern for algorithm selection
 */
template<typename T>
class Strategy {
public:
    virtual ~Strategy() = default;
    virtual T execute(const std::vector<T>& inputs) = 0;
    virtual std::string getName() const = 0;
};

} // namespace Common
} // namespace nlm