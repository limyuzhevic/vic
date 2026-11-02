// Brain Utilities Implementation
// Contains implementations for utility classes and helper functions

#include "BrainUtils.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// BrainLogger implementation
void BrainLogger::log(const std::string& message, LogLevel level) {
    // Integration with existing NLM logging system
    const char* levelStr[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
    std::cout << "[BRAIN " << levelStr[static_cast<int>(level)] << "] " 
              << message << std::endl;
    
    // Forward to existing NLM logger
    NLM_LOG_INFO("Utility: " + message);
}

// EventManager implementation - template functions are defined in header

// ConfigUtils implementation - template functions are defined in header

// StatisticsCollector implementation
void StatisticsCollector::printReport() const {
    BrainLogger::getInstance().logInfo("Performance Statistics Report:");
    for (const auto& pair : stats_) {
        const auto& s = pair.second;
        BrainLogger::getInstance().logInfo("  " + pair.first + ": " +
            std::to_string(s.min) + " to " + std::to_string(s.max) + " " +
            std::to_string(s.mean) + " ± " + std::to_string(s.stddev) + " " +
            std::to_string(s.count) + " samples");
    }
}

// ResourceManager implementation - template functions are defined in header

// Additional utility implementations
namespace Detail {
    
    // Thread-safe counter implementation
    class ThreadSafeCounter {
    public:
        ThreadSafeCounter() : count_(0) {}
        
        void increment() {
            std::lock_guard<std::mutex> lock(mutex_);
            count_++;
        }
        
        size_t get() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return count_;
        }
        
        void reset() {
            std::lock_guard<std::mutex> lock(mutex_);
            count_ = 0;
        }
        
    private:
        mutable std::mutex mutex_;
        size_t count_;
    };
    
    // LRU cache implementation
    template<typename Key, typename Value>
    class LRUCache {
    public:
        LRUCache(size_t capacity) : capacity_(capacity) {}
        
        bool get(const Key& key, Value& value) {
            auto it = cache_.find(key);
            if (it == cache_.end()) return false;
            
            // Move to end (most recently used)
            value = it->second.first;
            access_order_.erase(it->second.second);
            access_order_.push_back(key);
            it->second.second = --access_order_.end();
            
            return true;
        }
        
        void put(const Key& key, const Value& value) {
            auto it = cache_.find(key);
            if (it != cache_.end()) {
                // Update existing entry
                access_order_.erase(it->second.second);
            } else if (cache_.size() >= capacity_) {
                // Remove least recently used
                Key lru_key = access_order_.front();
                cache_.erase(lru_key);
                access_order_.pop_front();
            }
            
            cache_[key] = std::make_pair(value, --access_order_.end());
            access_order_.push_back(key);
        }
        
        size_t size() const {
            return cache_.size();
        }
        
        void clear() {
            cache_.clear();
            access_order_.clear();
        }
        
    private:
        std::unordered_map<Key, std::pair<Value, std::list<Key>::iterator>> cache_;
        std::list<Key> access_order_;
        size_t capacity_;
    };
    
} // namespace Detail

// Global utility instances for backward compatibility
namespace {
    BrainLogger& getGlobalLogger() {
        return BrainLogger::getInstance();
    }
    
    StatisticsCollector& getGlobalStatistics() {
        return StatisticsCollector::getInstance();
    }
    
    EventManager& getGlobalEventManager() {
        return EventManager::getInstance();
    }
    
    ResourceManager& getGlobalResourceManager() {
        return ResourceManager::getInstance();
    }
}

// Convenience functions for common operations
void initializeBrainUtils() {
    // Initialize any global utility state
    getGlobalEventManager();
    getGlobalResourceManager();
    getGlobalStatistics();
    
    BrainLogger::getInstance().logInfo("Brain utilities initialized");
}

void cleanupBrainUtils() {
    // Cleanup global utility state
    getGlobalResourceManager().cleanup();
    BrainLogger::getInstance().logInfo("Brain utilities cleaned up");
}

// Template function for logging with variable arguments
std::string formatMessage(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    
    // Simple formatting implementation
    std::string result;
    size_t format_pos = 0;
    size_t arg_index = 0;
    
    while (format_pos < format.size()) {
        if (format[format_pos] == '%') {
            format_pos++;
            if (format_pos < format.size() && format[format_pos] == 'd') {
                int arg = va_arg(args, int);
                result += std::to_string(arg);
                format_pos++;
            } else if (format_pos < format.size() && format[format_pos] == 'f') {
                float arg = static_cast<float>(va_arg(args, double));
                result += std::to_string(arg);
                format_pos++;
            } else if (format_pos < format.size() && format[format_pos] == 's') {
                const char* arg = va_arg(args, const char*);
                result += std::string(arg);
                format_pos++;
            } else {
                result += "%";
            }
        } else {
            result += format[format_pos];
            format_pos++;
        }
    }
    
    va_end(args);
    return result;
}

// Performance monitoring helper
class ScopedTimer {
public:
    ScopedTimer(const std::string& operation_name) 
        : operation_name_(operation_name) 
        , start_time_(std::chrono::high_resolution_clock::now()) {}
    
    ~ScopedTimer() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_);
        
        StatisticsCollector::getInstance().record(operation_name_ + "_duration", duration.count());
        BrainLogger::getInstance().logDebug(operation_name_ + " took " + std::to_string(duration.count()) + " ms");
    }
    
private:
    std::string operation_name_;
    std::chrono::high_resolution_clock::time_point start_time_;
};

// Macro for easy timing
#define TIMER_SCOPE(operation_name) nlm::ScopedTimer timer_##__LINE__(operation_name)

} // namespace nlm
