#ifndef NLM_UTILS_H
#define NLM_UTILS_H

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <functional>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <limits>
#include <cassert>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace nlm {

// Forward declarations for common types
using Timestamp = double;
using TimestepDuration = double;
using SimulationStep = uint64_t;
using SynapticWeight = float;
using NeuronId = uint64_t;
using RegionId = uint64_t;
using PopulationId = uint64_t;
using SynapseId = uint64_t;
using Delay = int;

// Common constants
static constexpr float EPSILON = 1e-6f;
static constexpr float PI = 3.14159265358979323846f;

// Safe math functions
template<typename T>
inline T clamp(T value, T min, T max) {
    return std::max(min, std::min(max, value));
}

template<typename T>
inline bool isNear(T a, T b, T epsilon = EPSILON) {
    return std::abs(a - b) < epsilon;
}

// Common error handling macros
#define NLM_VALIDATE(condition, message) \
    do { \
        if (!(condition)) { \
            Logger::error(message, __FILE__, __LINE__, __func__); \
            return false; \
        } \
    } while(false)

#define NLM_THROW_IF(condition, message) \
    do { \
        if (!(condition)) { \
            throw std::runtime_error(message); \
        } \
    } while(false)

// Exception classes
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error(message) {}
};

class ConfigurationException : public NLMException {
public:
    explicit ConfigurationException(const std::string& message) 
        : NLMException("Configuration Error: " + message) {}
};

class InitializationException : public NLMException {
public:
    explicit InitializationException(const std::string& message) 
        : NLMException("Initialization Error: " + message) {}
};

// Memory management utilities
class MemoryPool {
public:
    template<typename T>
    static std::unique_ptr<T> allocate(std::unique_ptr<T> ptr) {
        return ptr;
    }
    
    template<typename T, typename... Args>
    static std::unique_ptr<T> make(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
};

// Template for common observer pattern registration
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update() = 0;
};

// Event-driven system
class EventSystem {
public:
    template<typename T>
    using Handler = std::function<void(T)>;
    
    template<typename T>
    void registerHandler(Handler<T> handler) {
        handlers_.push_back(handler);
    }
    
    template<typename T>
    void notify(const T& event) {
        for (auto& handler : handlers_) {
            handler(event);
        }
    }
    
    void clear() {
        handlers_.clear();
    }
    
private:
    std::vector<Handler<void>> handlers_;
};

// Template for common initialization patterns
class Initializable {
public:
    virtual ~Initializable() = default;
    virtual bool initialize() = 0;
    virtual bool isInitialized() const = 0;
    virtual void reset() = 0;
};

// Utility for performance timing
class Timer {
public:
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start_).count();
    }
    
    void reset() {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_;
};

// Statistical utilities
struct Statistics {
    static double mean(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return sum / values.size();
    }
    
    static float variance(const std::vector<float>& values, float mean) {
        if (values.empty()) return 0.0f;
        float sum = 0.0f;
        for (float val : values) {
            sum += (val - mean) * (val - mean);
        }
        return sum / values.size();
    }
    
    static float standardDeviation(const std::vector<float>& values, float mean) {
        return std::sqrt(variance(values, mean));
    }
    
    static float min(const std::vector<float>& values) {
        if (values.empty()) return std::numeric_limits<float>::max();
        return *std::min_element(values.begin(), values.end());
    }
    
    static float max(const std::vector<float>& values) {
        if (values.empty()) return std::numeric_limits<float>::lowest();
        return *std::max_element(values.begin(), values.end());
    }
};

// Thread-safe container
template<typename T>
class ThreadSafeContainer {
public:
    void push(const T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        container_.push(item);
    }
    
    bool try_pop(T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (container_.empty()) return false;
        item = container_.front();
        container_.pop();
        return true;
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return container_.size();
    }
    
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return container_.empty();
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!container_.empty()) {
            container_.pop();
        }
    }
    
private:
    std::queue<T> container_;
    mutable std::mutex mutex_;
};

// Helper function for string formatting
inline std::string format(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    // Get required buffer size
    int size = std::snprintf(nullptr, 0, format, args) + 1;
    
    std::vector<char> buffer(size);
    std::vsnprintf(buffer.data(), size, format, args);
    
    va_end(args);
    
    return std::string(buffer.data(), buffer.size() - 1);
}

} // namespace nlm

#endif // NLM_UTILS_H
