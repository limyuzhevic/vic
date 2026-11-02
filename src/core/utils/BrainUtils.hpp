// Brain Utilities and Helper Classes
// Provides common functionality used across the brain modules

#pragma once

#include <algorithm>
#include <cmath>
#include <chrono>
#include <random>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Logging utilities
class BrainLogger {
public:
    static BrainLogger& getInstance() {
        static BrainLogger instance;
        return instance;
    }
    
    void log(const std::string& message, LogLevel level = LogLevel::Info) {
        // Integration with existing logging system
        // This could be enhanced to support different output formats
        std::cout << "[BRAIN] " << message << std::endl;
    }
    
    void logDebug(const std::string& message) {
        log(message, LogLevel::Debug);
    }
    
    void logInfo(const std::string& message) {
        log(message, LogLevel::Info);
    }
    
    void logWarning(const std::string& message) {
        log(message, LogLevel::Warning);
    }
    
    void logError(const std::string& message) {
        log(message, LogLevel::Error);
    }
    
private:
    BrainLogger() {}
    BrainLogger(const BrainLogger&) = delete;
    BrainLogger& operator=(const BrainLogger&) = delete;
};

// Mathematical utilities
class MathUtils {
public:
    // Vector operations
    static float dotProduct(const std::vector<float>& a, const std::vector<float>& b) {
        float result = 0.0f;
        size_t size = std::min(a.size(), b.size());
        for (size_t i = 0; i < size; ++i) {
            result += a[i] * b[i];
        }
        return result;
    }
    
    static float magnitude(const std::vector<float>& v) {
        float result = 0.0f;
        for (float x : v) {
            result += x * x;
        }
        return std::sqrt(result);
    }
    
    static float normalize(std::vector<float>& v) {
        float mag = magnitude(v);
        if (mag > 0.0f) {
            for (float& x : v) {
                x /= mag;
            }
        }
        return mag;
    }
    
    // Statistical operations
    static float mean(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = 0.0f;
        for (float v : values) sum += v;
        return sum / values.size();
    }
    
    static float variance(const std::vector<float>& values) {
        if (values.size() < 2) return 0.0f;
        float m = mean(values);
        float sum = 0.0f;
        for (float v : values) {
            float diff = v - m;
            sum += diff * diff;
        }
        return sum / (values.size() - 1);
    }
    
    static float standardDeviation(const std::vector<float>& values) {
        return std::sqrt(variance(values));
    }
    
    // Clamping and normalization
    static float clamp(float value, float min, float max) {
        return std::max(min, std::min(max, value));
    }
    
    static bool isWithin(float value, float target, float tolerance) {
        return std::abs(value - target) <= tolerance;
    }
    
    // Easing functions for smooth transitions
    static float easeInOut(float t) {
        return t * t * (3.0f - 2.0f * t);  // Smoothstep
    }
    
    // Random utilities
    static float uniformRandom(float min, float max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(min, max);
        return dist(gen);
    }
    
    static int uniformRandomInt(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }
    
    // Timing utilities
    static double getCurrentTime() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration<double>(duration).count();
    }
    
    // String utilities
    static std::string format(const char* format, ...) {
        // Simple implementation - in production, consider using std::format
        va_list args;
        va_start(args, format);
        // This is a simplified version - for production use a more robust formatter
        std::string result = format;
        va_end(args);
        return result;
    }
    
    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    // Performance monitoring
    class PerformanceTimer {
    public:
        PerformanceTimer(const std::string& name) : name_(name) {
            start_time_ = MathUtils::getCurrentTime();
        }
        
        ~PerformanceTimer() {
            double elapsed = MathUtils::getCurrentTime() - start_time_;
            BrainLogger::getInstance().logDebug(name_ + " took " + std::to_string(elapsed) + " seconds");
        }
        
        double elapsed() const {
            return MathUtils::getCurrentTime() - start_time_;
        }
        
    private:
        std::string name_;
        double start_time_;
    };
};

// Event system for decoupled communication
class EventManager {
public:
    template<typename EventType>
    class Event {
    public:
        using EventCallback = std::function<void(const EventType&)>;
        
        void addListener(EventCallback callback) {
            listeners_.push_back(callback);
        }
        
        void removeListener(EventCallback callback) {
            listeners_.erase(std::remove(listeners_.begin(), listeners_.end(), callback), listeners_.end());
        }
        
        void fire(const EventType& event) {
            for (auto& callback : listeners_) {
                callback(event);
            }
        }
        
        size_t listenerCount() const {
            return listeners_.size();
        }
        
    private:
        std::vector<EventCallback> listeners_;
    };
    
    // Common event types
    struct SpikeEvent {
        NeuronId neuronId;
        Timestamp timestamp;
        NeuronType neuronType;
    };
    
    struct RewardEvent {
        float reward;
        float predictedReward;
        NeuronId sourceNeuron;
        Timestamp timestamp;
    };
    
    struct MemoryUpdateEvent {
        std::string memoryType;
        float importance;
        Timestamp timestamp;
    };
    
    // Singleton instance
    static EventManager& getInstance() {
        static EventManager instance;
        return instance;
    }
    
    // Predefined event channels
    Event<SpikeEvent> spikeChannel;
    Event<RewardEvent> rewardChannel;
    Event<MemoryUpdateEvent> memoryChannel;
    
private:
    EventManager() {}
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;
};

// Configuration utilities
class ConfigUtils {
public:
    // Validate configuration values
    static bool validateRange(float value, float min, float max) {
        return value >= min && value <= max;
    }
    
    static bool validatePositive(float value) {
        return value >= 0.0f;
    }
    
    static bool validateNonZero(float value) {
        return value != 0.0f;
    }
    
    // Configuration presets
    static std::string getPresetName(const std::string& mode) {
        std::string lower = MathUtils::toLower(mode);
        if (lower == "fast") return "Fast Simulation Mode";
        if (lower == "accurate") return "High Accuracy Mode";
        if (lower == "balanced") return "Balanced Mode";
        if (lower == "adaptive") return "Adaptive Mode";
        return "Custom Mode";
    }
    
    // Merge configurations
    static std::string mergeConfigs(const std::string& base, const std::string& override) {
        // Simple implementation - for production, use proper JSON/YAML merging
        if (MathUtils::toLower(override).find("override") != std::string::npos) {
            return override;
        }
        return base;
    }
    
    // Get configuration value with type conversion
    template<typename T>
    static T getConfigValue(const std::unordered_map<std::string, std::string>& config, 
                           const std::string& key, T defaultValue) {
        auto it = config.find(key);
        if (it == config.end()) {
            return defaultValue;
        }
        
        // Convert string to target type (simplified)
        try {
            if constexpr (std::is_same_v<T, int>) {
                return std::stoi(it->second);
            } else if constexpr (std::is_same_v<T, float>) {
                return std::stof(it->second);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::stod(it->second);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return it->second;
            } else if constexpr (std::is_same_v<T, bool>) {
                std::string value = MathUtils::toLower(it->second);
                return value == "true" || value == "yes" || value == "1";
            }
        } catch (const std::exception&) {
            // Conversion failed, return default
        }
        
        return defaultValue;
    }
};

// Statistics collector for performance monitoring
class StatisticsCollector {
public:
    struct Stats {
        double min = 0.0;
        double max = 0.0;
        double mean = 0.0;
        double variance = 0.0;
        double stddev = 0.0;
        size_t count = 0;
        double sum = 0.0;
    };
    
    static StatisticsCollector& getInstance() {
        static StatisticsCollector instance;
        return instance;
    }
    
    void record(const std::string& name, double value) {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_[name].min = std::min(stats_[name].min, value);
        stats_[name].max = std::max(stats_[name].max, value);
        stats_[name].sum += value;
        stats_[name].count++;
        
        // Update running mean
        double old_mean = stats_[name].mean;
        stats_[name].mean = old_mean + (value - old_mean) / stats_[name].count;
        
        // Update variance
        stats_[name].variance = stats_[name].mean * stats_[name].mean;
        double m2 = 0.0;
        for (auto it = stats_.begin(); it != stats_.end(); ++it) {
            m2 += (it->second.mean - old_mean) * (it->second.mean - old_mean);
        }
        stats_[name].variance = m2 / stats_[name].count;
        stats_[name].stddev = std::sqrt(stats_[name].variance);
    }
    
    const Stats& getStats(const std::string& name) const {
        static Stats emptyStats;
        auto it = stats_.find(name);
        return it != stats_.end() ? it->second : emptyStats;
    }
    
    void reset(const std::string& name = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        if (name.empty()) {
            stats_.clear();
        } else {
            stats_.erase(name);
        }
    }
    
    void printReport() const {
        BrainLogger::getInstance().logInfo("Performance Statistics Report:");
        for (const auto& pair : stats_) {
            const auto& s = pair.second;
            BrainLogger::getInstance().logInfo("  " + pair.first + ": " +
                std::to_string(s.min) + " to " + std::to_string(s.max) + " " +
                std::to_string(s.mean) + " ± " + std::to_string(s.stddev) + " " +
                std::to_string(s.count) + " samples");
        }
    }
    
private:
    StatisticsCollector() {}
    StatisticsCollector(const StatisticsCollector&) = delete;
    StatisticsCollector& operator=(const StatisticsCollector&) = delete;
    
    mutable std::mutex mutex_;
    std::unordered_map<std::string, Stats> stats_;
};

// Resource manager for efficient allocation
class ResourceManager {
public:
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }
    
    template<typename T>
    class ResourcePool {
    public:
        ResourcePool(size_t capacity) : capacity_(capacity) {}
        
        T* allocate() {
            if (free_list_.empty()) {
                if (size_ >= capacity_) {
                    return nullptr; // Pool full
                }
                // Create new object
                objects_.push_back(std::make_unique<T>());
                free_list_.push_back(objects_.back().get());
            }
            
            T* obj = free_list_.back();
            free_list_.pop_back();
            allocated_.insert(obj);
            return obj;
        }
        
        void deallocate(T* obj) {
            auto it = allocated_.find(obj);
            if (it != allocated_.end()) {
                free_list_.push_back(obj);
                allocated_.erase(it);
            }
        }
        
        size_t allocatedCount() const {
            return allocated_.size();
        }
        
        size_t availableCount() const {
            return free_list_.size();
        }
        
    private:
        std::vector<std::unique_ptr<T>> objects_;
        std::vector<T*> free_list_;
        std::unordered_set<T*> allocated_;
        size_t capacity_;
        size_t size_ = 0;
    };
    
    // Get or create a resource pool
    template<typename T>
    ResourcePool<T>& getPool(const std::string& name, size_t capacity) {
        auto it = pools_.find(name);
        if (it == pools_.end()) {
            auto pool = std::make_unique<ResourcePool<T>>(capacity);
            pools_[name] = std::move(pool);
        }
        return *static_cast<ResourcePool<T>*>(pools_[name].get());
    }
    
    // Cleanup all pools
    void cleanup() {
        pools_.clear();
    }
    
private:
    ResourceManager() {}
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    std::unordered_map<std::string, std::unique_ptr<void>> pools_;
};

} // namespace nlm
