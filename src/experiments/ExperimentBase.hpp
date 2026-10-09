#ifndef NLM_EXPERIMENT_BASE_H
#define NLM_EXPERIMENT_BASE_H

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <sstream>
#include <fstream>

namespace nlm {

class Metric {
public:
    std::string name;
    double value;
    double timestamp;
    
    Metric(const std::string& name, double value, double timestamp = 0.0)
        : name(name), value(value), timestamp(timestamp) {}
};

class BaseExperiment {
public:
    BaseExperiment(const std::string& name, std::shared_ptr<Logger> logger = nullptr)
        : name_(name), logger_(logger) {}
    
    virtual ~BaseExperiment() = default;
    
    // Core interface - all experiments implement these
    virtual std::string getName() const { return name_; }
    virtual void setup(const Config& config) = 0;
    virtual bool run() = 0;
    virtual void cleanup() = 0;
    
    // Common utilities - reuse across all experiments
    void setLogger(std::shared_ptr<Logger> logger) { logger_ = logger; }
    void addMetric(const std::string& name, double value);
    double getMetric(const std::string& name) const;
    const std::vector<Metric>& getAllMetrics() const { return metrics_; }
    
    // Configuration utilities
    template<typename T>
    void setConfig(const std::string& key, T value) {
        config_[key] = std::to_string(value);
    }
    
    template<typename T>
    T getConfig(const std::string& key) const {
        auto it = config_.find(key);
        if (it == config_.end()) {
            throw std::runtime_error("Configuration key not found: " + key);
        }
        return std::stod(it->second);
    }
    
    // Progress logging
    void logProgress(const std::string& message);
    void logProgress(const std::string& format, ...);
    
    // Results saving
    virtual void saveResults(const std::string& filename);
    virtual void saveResultsCSV(const std::string& filename) = 0;
    virtual void saveResultsJSON(const std::string& filename) = 0;
    
    // Helper methods for common experiment patterns
    void runEpisodes(size_t numEpisodes, std::function<void(size_t, double)> episodeFunc);
    void runSteps(size_t numSteps, std::function<void(size_t, double)> stepFunc);
    
protected:
    std::string name_;
    std::shared_ptr<Logger> logger_;
    std::unordered_map<std::string, std::string> config_;
    std::vector<Metric> metrics_;
    std::ofstream logFile_;
    bool logEnabled_ = false;
};

} // namespace nlm

#endif // NLM_EXPERIMENT_BASE_H
