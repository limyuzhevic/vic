#pragma once

#include <atomic>
#include <string>
#include <chrono>
#include <vector>
#include <mutex>
#include <unordered_map>

namespace nlm {

class AtomicCounter {
private:
    std::atomic<uint64_t> value_;

public:
    AtomicCounter() : value_(0) {}
    AtomicCounter(uint64_t initial) : value_(initial) {}
    
    uint64_t increment() {
        return ++value_;
    }
    
    uint64_t increment(uint64_t amount) {
        return value_ += amount;
    }
    
    uint64_t decrement() {
        return --value_;
    }
    
    uint64_t decrement(uint64_t amount) {
        return value_ -= amount;
    }
    
    uint64_t get() const {
        return value_.load();
    }
    
    void set(uint64_t newValue) {
        value_.store(newValue);
    }
    
    operator uint64_t() const {
        return get();
    }
};

class PerformanceCounter {
private:
    struct CounterData {
        std::string name;
        std::atomic<uint64_t> value;
        std::chrono::duration<double> lastUpdate;
        uint64_t resetCount = 0;
        std::string component;
    };

    std::unordered_map<std::string, CounterData> counters_;
    mutable std::mutex countersMutex_;
    bool trackReset_ = false;

public:
    PerformanceCounter() = default;

    // Counter management
    void increment(const std::string& name, uint64_t amount = 1,
                  const std::string& component = "core") {
        std::lock_guard<std::mutex> lock(countersMutex_);
        if (counters_.find(name) == counters_.end()) {
            counters_[name] = {name, 0, std::chrono::high_resolution_clock::now(), 0, component};
        }
        counters_[name].value.fetch_add(amount);
        counters_[name].lastUpdate = std::chrono::high_resolution_clock::now();
    }

    void decrement(const std::string& name, uint64_t amount = 1,
                  const std::string& component = "core") {
        std::lock_guard<std::mutex> lock(countersMutex_);
        if (counters_.find(name) == counters_.end()) {
            counters_[name] = {name, 0, std::chrono::high_resolution_clock::now(), 0, component};
        }
        counters_[name].value.fetch_sub(amount);
        counters_[name].lastUpdate = std::chrono::high_resolution_clock::now();
    }

    void set(const std::string& name, uint64_t value,
            const std::string& component = "core") {
        std::lock_guard<std::mutex> lock(countersMutex_);
        if (counters_.find(name) == counters_.end()) {
            counters_[name] = {name, value, std::chrono::high_resolution_clock::now(), 0, component};
        } else {
            counters_[name].value.store(value);
            counters_[name].lastUpdate = std::chrono::high_resolution_clock::now();
        }
    }

    uint64_t get(const std::string& name) const {
        std::lock_guard<std::mutex> lock(countersMutex_);
        if (counters_.find(name) != counters_.end()) {
            return counters_.at(name).value.load();
        }
        return 0;
    }

    bool exists(const std::string& name) const {
        std::lock_guard<std::mutex> lock(countersMutex_);
        return counters_.find(name) != counters_.end();
    }

    void remove(const std::string& name) {
        std::lock_guard<std::mutex> lock(countersMutex_);
        counters_.erase(name);
    }

    void reset(const std::string& name) {
        std::lock_guard<std::mutex> lock(countersMutex_);
        if (counters_.find(name) != counters_.end()) {
            counters_[name].value.store(0);
            if (trackReset_) {
                counters_[name].resetCount++;
            }
            counters_[name].lastUpdate = std::chrono::high_resolution_clock::now();
        }
    }

    // Batch operations
    void resetAll() {
        std::lock_guard<std::mutex> lock(countersMutex_);
        for (auto& [name, data] : counters_) {
            data.value.store(0);
            if (trackReset_) {
                data.resetCount++;
            }
            data.lastUpdate = std::chrono::high_resolution_clock::now();
        }
    }

    std::vector<std::string> getCounterNames() const {
        std::lock_guard<std::mutex> lock(countersMutex_);
        std::vector<std::string> names;
        names.reserve(counters_.size());
        for (const auto& pair : counters_) {
            names.push_back(pair.first);
        }
        return names;
    }

    // Statistics and analysis
    struct CounterStats {
        std::string name;
        uint64_t currentValue;
        uint64_t resetCount;
        std::chrono::duration<double> lastUpdate;
        std::chrono::duration<double> age; // Time since last update
        std::string component;
    };

    std::vector<CounterStats> getAllStats() const {
        std::lock_guard<std::mutex> lock(countersMutex_);
        std::vector<CounterStats> stats;
        auto now = std::chrono::high_resolution_clock::now();
        
        for (const auto& [name, data] : counters_) {
            CounterStats stat;
            stat.name = data.name;
            stat.currentValue = data.value.load();
            stat.resetCount = data.resetCount;
            stat.lastUpdate = data.lastUpdate;
            stat.age = now - data.lastUpdate;
            stat.component = data.component;
            stats.push_back(stat);
        }
        return stats;
    }

    uint64_t getTotalCount() const {
        std::lock_guard<std::mutex> lock(countersMutex_);
        uint64_t total = 0;
        for (const auto& [name, data] : counters_) {
            total += data.value.load();
        }
        return total;
    }

    // Configuration
    void enableResetTracking(bool enable) {
        trackReset_ = enable;
    }

    // Reporting
    std::string generateReport() const {
        std::lock_guard<std::mutex> lock(countersMutex_);
        std::stringstream ss;
        
        ss << "Performance Counters Report\n";
        ss << "===========================\n\n";
        
        auto stats = getAllStats();
        if (stats.empty()) {
            ss << "No counters found.\n";
            return ss.str();
        }
        
        ss << "Total Counters: " << stats.size() << "\n";
        ss << "Total Count Value: " << getTotalCount() << "\n\n";
        
        ss << std::left << std::setw(20) << "Counter Name"
           << std::setw(15) << "Value"
           << std::setw(15) << "Resets"
           << std::setw(15) << "Age (s)"
           << std::setw(15) << "Component\n";
        
        ss << std::string(80, '-') << "\n";
        
        for (const auto& stat : stats) {
            ss << std::left << std::setw(20) << stat.name
               << std::setw(15) << stat.currentValue
               << std::setw(15) << stat.resetCount
               << std::setw(15) << stat.age.count()
               << std::setw(15) << stat.component << "\n";
        }
        
        return ss.str();
    }

    std::string generateJSONReport() const {
        std::lock_guard<std::mutex> lock(countersMutex_);
        std::stringstream ss;
        
        ss << "{\n";
        ss << "  \"counters\": [\n";
        
        auto stats = getAllStats();
        for (size_t i = 0; i < stats.size(); ++i) {
            const auto& stat = stats[i];
            ss << "    {\n";
            ss << "      \"name\": \"" << stat.name << "\",\n";
            ss << "      \"value\": " << stat.currentValue << ",\n";
            ss << "      \"resets\": " << stat.resetCount << ",\n";
            ss << "      \"lastUpdate\": " << stat.lastUpdate.count() << ",\n";
            ss << "      \"ageSeconds\": " << stat.age.count() << ",\n";
            ss << "      \"component\": \"" << stat.component << "\"\n";
            if (i < stats.size() - 1) ss << "    },\n";
            else ss << "    }\n";
        }
        ss << "  ],\n";
        ss << "  \"totalCount\": " << getTotalCount() << "\n";
        ss << "}\n";
        
        return ss.str();
    }
};

} // namespace nlm
