// Additional common utilities for NLM tools

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include "core/Logger/Logger.hpp"

namespace nlm {

// Progress tracking for long-running operations
class ProgressTracker {
public:
    ProgressTracker(size_t total, const std::string& description);
    
    void update(size_t current, const std::string& message = "");
    void finish();
    
    bool isComplete() const;
    double getProgress() const;
    
    void setMessage(const std::string& message);
    
    void setMaxThreads(size_t maxThreads);
    void waitForComplete();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Result aggregation for batch operations
struct BatchResult {
    std::string scenarioName;
    std::string configFile;
    std::string outputDir;
    bool success;
    std::string errorMessage;
    double executionTime;
    std::map<std::string, double> metrics;
    time_t timestamp;
    
    BatchResult() : success(false), executionTime(0.0), timestamp(0) {}
};

// Thread pool for parallel execution
class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();
    
    template<typename F>
    void submit(F&& task);
    
    void waitForAll();
    size_t getQueueSize() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration parser for YAML/JSON (simplified)
class ConfigParser {
public:
    static std::map<std::string, std::string> parseSimpleConfig(const std::string& filepath);
    static std::string generateConfigTemplate(const std::map<std::string, std::string>& defaults);
    
    // Common configuration keys
    static const std::vector<std::string> getCommonKeys();
    static const std::vector<std::string> getLearningKeys();
    static const std::vector<std::string> getSimulationKeys();
};

// Result formatter for different output formats
class ResultFormatter {
public:
    enum class Format { TEXT, CSV, JSON, HTML, XML };
    
    static std::string formatResults(const std::vector<BatchResult>& results, Format format);
    static std::string formatProgress(size_t current, size_t total, 
                                     const std::string& description);
    
    static void saveResults(const std::string& filename, 
                           const std::vector<BatchResult>& results,
                           Format format = Format::TEXT);
};

} // namespace nlm