#pragma once

#include "Experiment.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Forward declarations
class Brain;
struct ExperimentResults;

class ExperimentBase {
public:
    ExperimentBase(const std::string& name);
    virtual ~ExperimentBase() = default;
    
    // Lifecycle management
    virtual bool initialize() = 0;
    virtual bool run() = 0;
    virtual void cleanup() = 0;
    virtual bool isRunning() const = 0;
    
    // Configuration management
    void setConfigValue(const std::string& key, const std::string& value);
    std::string getConfigValue(const std::string& key) const;
    void loadConfig(const std::string& filename);
    bool saveConfig(const std::string& filename) const;
    void validateConfig() const;
    
    // Brain initialization helpers
    bool initializeBrain();
    bool setupBrainFromConfig(Brain* brain) const;
    
    // Metrics collection interfaces
    template<typename T>
    void recordMetric(const std::string& name, const T& value);
    
    template<typename T>
    void recordMetric(const std::string& category, const std::string& name, const T& value);
    
    double getMetric(const std::string& name) const;
    double getMetric(const std::string& category, const std::string& name) const;
    
    bool hasMetric(const std::string& name) const;
    bool hasMetric(const std::string& category, const std::string& name) const;
    
    const std::unordered_map<std::string, double>& getAllMetrics() const;
    const std::unordered_map<std::string, std::unordered_map<std::string, double>>& getAllCategorizedMetrics() const;
    
    // Results management
    template<typename TResult>
    void storeResult(const std::string& identifier, const TResult& result);
    
    template<typename TResult>
    const TResult& getResult(const std::string& identifier) const;
    
    // Serialization methods
    std::string toCSV() const;
    std::string toJSON() const;
    
    // Metadata
    const std::string& getName() const;
    void setName(const std::string& name);
    uint64_t getSeed() const;
    void setSeed(uint64_t seed);
    std::string getTimestamp() const;
    
    // Status and progress
    float getProgress() const;
    void setProgress(float progress);
    std::string getStatus() const;
    void setStatus(const std::string& status);
    
    // Experiment metadata
    void addNote(const std::string& note);
    const std::vector<std::string>& getNotes() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void initializeImpl(const std::string& name);
};

// Template experiment results base class
class ExperimentResultsBase {
public:
    virtual ~ExperimentResultsBase() = default;
    
    virtual std::string toCSV() const = 0;
    virtual std::string toJSON() const = 0;
    virtual void reset() = 0;
    
    std::string getTimestamp() const;
    void setTimestamp(const std::string& timestamp);
    
    const std::string& getExperimentName() const;
    void setExperimentName(const std::string& name);
    
    uint64_t getSeed() const;
    void setSeed(uint64_t seed);
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
