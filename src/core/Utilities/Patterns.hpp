#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <algorithm>
#include <stdexcept>

namespace nlm {
namespace Core {
namespace Utilities {

/**
 * Factory pattern utilities for neural network object creation
 */
template<typename T, typename... Args>
class Factory {
public:
    using CreatorFunc = std::function<std::unique_ptr<T>(Args...)>;
    
    template<typename U>
    static void registerType(const std::string& typeName) {
        creators[typeName] = [](Args... args) -> std::unique_ptr<T> {
            return std::make_unique<U>(std::forward<Args>(args)...);
        };
    }
    
    static std::unique_ptr<T> create(const std::string& typeName, Args... args) {
        auto it = creators.find(typeName);
        if (it == creators.end()) {
            throw std::runtime_error("Unknown type: " + typeName);
        }
        return it->second(std::forward<Args>(args)...);
    }
    
    static bool hasType(const std::string& typeName) {
        return creators.find(typeName) != creators.end();
    }
    
    static std::vector<std::string> getRegisteredTypes() {
        std::vector<std::string> types;
        types.reserve(creators.size());
        for (const auto& pair : creators) {
            types.push_back(pair.first);
        }
        return types;
    }
    
    static void clear() {
        creators.clear();
    }
    
protected:
    static std::map<std::string, CreatorFunc> creators;
};

template<typename T, typename... Args>
std::map<std::string, typename Factory<T, Args...>::CreatorFunc> Factory<T, Args...>::creators;

/**
 * Observer pattern utilities for neural network event handling
 */
template<typename T>
class Observer {
public:
    virtual ~Observer() = default;
    virtual void onEvent(const T& event) = 0;
    virtual bool matches(const std::string& eventType) const = 0;
    virtual int getPriority() const { return 0; }
};

template<typename T>
class Subject {
private:
    std::vector<std::unique_ptr<Observer<T>>> observers;
    
public:
    void attach(std::unique_ptr<Observer<T>> observer) {
        observers.push_back(std::move(observer));
        std::sort(observers.begin(), observers.end(),
            [](const std::unique_ptr<Observer<T>>& a, const std::unique_ptr<Observer<T>>& b) {
                return a->getPriority() > b->getPriority();  // Higher priority first
            });
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
    
    size_t observerCount() const {
        return observers.size();
    }
    
    void clear() {
        observers.clear();
    }
};

/**
 * Strategy pattern utilities for algorithm selection
 */
template<typename T, typename R>
class Strategy {
public:
    virtual ~Strategy() = default;
    virtual R execute(const T& input) = 0;
    virtual std::string getName() const = 0;
    virtual double getScore(const T& input) const { return 1.0; }
};

/**
 * Event types for neural network simulations
 */
struct NeuralEvent {
    std::string type;
    double timestamp;
    std::map<std::string, std::string> data;
    std::string source;
    int priority;
    
    NeuralEvent(const std::string& type, const std::string& source = "")
        : type(type), timestamp(0.0), source(source), priority(0) {}
};

/**
 * Configuration patterns for neural network components
 */
template<typename T>
class Configurable {
protected:
    std::map<std::string, std::string> config;
    
public:
    virtual ~Configurable() = default;
    
    void setConfig(const std::string& key, const std::string& value) {
        config[key] = value;
    }
    
    std::string getConfig(const std::string& key, const std::string& defaultValue = "") const {
        auto it = config.find(key);
        return it != config.end() ? it->second : defaultValue;
    }
    
    bool hasConfig(const std::string& key) const {
        return config.find(key) != config.end();
    }
    
    virtual void applyConfig() {
        // Default implementation - override in derived classes
    }
    
    void clearConfig() {
        config.clear();
    }
};

/**
 * Batch processing utilities
 */
class BatchProcessor {
public:
    template<typename T, typename R>
    static std::vector<R> processBatch(const std::vector<T>& items, 
                                      std::function<R(const T&)> processor,
                                      size_t batchSize = 100) {
        std::vector<R> results;
        results.reserve(items.size());
        
        for (size_t i = 0; i < items.size(); i += batchSize) {
            size_t end = std::min(i + batchSize, items.size());
            for (size_t j = i; j < end; ++j) {
                results.push_back(processor(items[j]));
            }
        }
        
        return results;
    }
    
    template<typename T, typename R>
    static std::vector<R> processParallel(const std::vector<T>& items,
                                         std::function<R(const T&)> processor,
                                         size_t threadCount = std::thread::hardware_concurrency()) {
        std::vector<R> results;
        results.reserve(items.size());
        
        if (threadCount <= 1 || items.size() < threadCount * 10) {
            // Use sequential processing for small batches
            for (const auto& item : items) {
                results.push_back(processor(item));
            }
        } else {
            // Use parallel processing
            std::vector<std::future<R>> futures;
            futures.reserve(items.size());
            
            for (const auto& item : items) {
                futures.push_back(std::async(std::launch::async, processor, item));
            }
            
            for (auto& future : futures) {
                results.push_back(future.get());
            }
        }
        
        return results;
    }
};

/**
 * Cache utilities
 */
template<typename K, typename V>
class SimpleCache {
private:
    std::map<K, std::pair<V, double>> cache;  // value + timestamp
    double ttl;  // Time to live in seconds
    size_t maxSize;
    
public:
    SimpleCache(double ttlSeconds = 300.0, size_t maxSize = 1000)
        : ttl(ttlSeconds), maxSize(maxSize) {}
    
    void put(const K& key, const V& value) {
        if (cache.size() >= maxSize) {
            // Remove oldest entry
            auto oldest = cache.begin();
            cache.erase(oldest);
        }
        
        cache[key] = {value, std::chrono::duration<double>::zero()};
    }
    
    bool get(const K& key, V& value) {
        auto it = cache.find(key);
        if (it == cache.end()) return false;
        
        // Check TTL
        double age = std::chrono::duration<double>::zero(); // TODO: Implement proper time tracking
        if (age > ttl) {
            cache.erase(it);
            return false;
        }
        
        value = it->second.first;
        return true;
    }
    
    bool contains(const K& key) const {
        return cache.find(key) != cache.end();
    }
    
    void remove(const K& key) {
        cache.erase(key);
    }
    
    void clear() {
        cache.clear();
    }
    
    size_t size() const {
        return cache.size();
    }
    
    bool isEmpty() const {
        return cache.empty();
    }
};

/**
 * Performance monitoring utilities
 */
class PerformanceProfiler {
private:
    struct ProfileData {
        std::string operation;
        double duration;
        size_t callCount;
        double totalDuration;
        
        ProfileData(const std::string& op) 
            : operation(op), duration(0.0), callCount(0), totalDuration(0.0) {}
    };
    
    std::map<std::string, ProfileData> profiles;
    bool enabled;
    
public:
    PerformanceProfiler() : enabled(true) {}
    
    void start(const std::string& operation) {
        if (!enabled) return;
        
        auto it = profiles.find(operation);
        if (it == profiles.end()) {
            profiles[operation] = ProfileData(operation);
        }
    }
    
    void stop(const std::string& operation, double duration) {
        if (!enabled) return;
        
        auto it = profiles.find(operation);
        if (it != profiles.end()) {
            ProfileData& data = it->second;
            data.duration = duration;
            data.callCount++;
            data.totalDuration += duration;
        }
    }
    
    double getAverageTime(const std::string& operation) const {
        auto it = profiles.find(operation);
        if (it == profiles.end() || it->second.callCount == 0) return 0.0;
        return it->second.totalDuration / it->second.callCount;
    }
    
    size_t getCallCount(const std::string& operation) const {
        auto it = profiles.find(operation);
        return it != profiles.end() ? it->second.callCount : 0;
    }
    
    void reset(const std::string& operation) {
        auto it = profiles.find(operation);
        if (it != profiles.end()) {
            it->second = ProfileData(operation);
        }
    }
    
    void setEnabled(bool enable) {
        enabled = enable;
    }
    
    bool isEnabled() const {
        return enabled;
    }
    
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== Performance Profile Report ===\n";
        
        for (const auto& pair : profiles) {
            const ProfileData& data = pair.second;
            if (data.callCount > 0) {
                oss << "Operation: " << data.operation << "\n"
                    << "  Calls: " << data.callCount << "\n"
                    << "  Total duration: " << data.totalDuration << "s\n"
                    << "  Average duration: " << data.totalDuration / data.callCount << "s\n"
                    << "  Last duration: " << data.duration << "s\n";
            }
        }
        
        return oss.str();
    }
};

} // namespace Utils
} // namespace Core
} // namespace nlm