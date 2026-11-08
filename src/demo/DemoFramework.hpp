// NLM Demo Framework - Base classes and utilities for advanced demo functionality
// Created as part of NLM codebase improvements

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <iomanip>
#include <sstream>
#include <atomic>
#include <mutex>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

// Demo result structure for comprehensive reporting
struct DemoResult {
    std::string demoName;
    double executionTime;
    bool success;
    std::string errorMessage;
    std::map<std::string, std::string> metrics;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    
    DemoResult(const std::string& name) : demoName(name), success(false), executionTime(0.0) {
        startTime = std::chrono::system_clock::now();
    }
    
    void markSuccess() {
        success = true;
        endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        executionTime = duration.count() / 1000.0;
    }
    
    void markFailure(const std::string& error) {
        success = false;
        errorMessage = error;
        endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        executionTime = duration.count() / 1000.0;
    }
};

// Base demo runner class for creating modular demos
class BaseDemo {
public:
    virtual ~BaseDemo() = default;
    
    // Core demo interface
    virtual DemoResult run(const std::map<std::string, std::string>& config) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::vector<std::string> getAvailableTests() const = 0;
    
    // Configuration management
    virtual void loadConfig(const std::string& configFile) = 0;
    virtual void applyCommandLineArgs(int argc, char** argv) = 0;
    
    // Utility functions
    virtual void printBanner() = 0;
    virtual void setupLogging() = 0;
    virtual void cleanup() = 0;
}; 

// Advanced CLI argument parser for NLM demos
class DemoCLI {
public:
    struct Options {
        bool help = false;
        bool verbose = false;
        bool quiet = false;
        bool jsonOutput = false;
        std::string configFile = "";
        std::string outputFile = "";
        std::string logLevel = "info";
        std::string testFilter = "";
        double timeStep = 0.001;
        int neuronCount = 500;
        int maxSteps = 1000;
        bool enableCheckpoint = false;
        bool enableReplay = false;
        bool enableDevelopment = false;
        std::vector<std::string> customArgs;
    };
    
    static Options parseArguments(int argc, char** argv);
    static void printHelp();
    static std::string formatDuration(double seconds);
    static std::string formatMemorySize(size_t bytes);
    static std adicionalesJson exportResults(const std::vector<DemoResult>& results);
};

// Metrics collection system for comprehensive monitoring
class MetricsCollector {
public:
    struct Metric {
        std::string name;
        double value;
        std::string unit;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, std::string> tags;
        
        Metric(const std::string& n, double v, const std::string& u = "") 
            : name(n), value(v), unit(u), timestamp(std::chrono::system_clock::now()) {}
    };
    
    void recordMetric(const std::string& name, double value, const std::string& unit = "",
                     const std::map<std::string, std::string>& tags = {});
    std::vector<Metric> getMetrics() const;
    void clear();
    double getAverage(const std::string& name) const;
    double getMin(const std::string& name) const;
    double getMax(const std::string& name) const;
    size_t getCount(const std::string& name) const;
    
private:
    std::vector<Metric> metrics_;
    mutable std::mutex mutex_;
};

// Advanced configuration manager with JSON support
class AdvancedConfig {
public:
    static std::shared_ptr<Config> loadJsonConfig(const std::string& filepath);
    static std::shared_ptr<Config> loadCommandLineConfig(int argc, char** argv);
    static std::shared_ptr<Config> mergeConfigs(const std::shared_ptr<Config>& base,
                                               const std::map<std::string, std::string>& overrides);
    static void saveResultsJson(const std::string& filepath, const DemoResult& result);
    static std::vector<DemoResult> loadResultsJson(const std::string& filepath);
    static std::string formatConfigForDisplay(const std::shared_ptr<Config>& config);
};

// Progress tracking and visualization
class ProgressTracker {
public:
    ProgressTracker(const std::string& description, int totalSteps);
    
    void update(int currentStep, const std::string& status = "");
    void increment(const std::string& status = "");
    void complete();
    
    std::string getProgressBar(double progress) const;
    std::string getStatusLine() const;
    
private:
    int totalSteps_;
    int currentStep_;
    std::string description_;
    std::chrono::system_clock::time_point startTime_;
    mutable std::mutex mutex_;
};

// Enhanced brain interface for demos with additional monitoring
class DemoBrainWrapper {
public:
    DemoBrainWrapper(std::shared_ptr<Brain> brain);
    
    // Core functionality with metrics
    bool step(SimulationStep step, double time);
    
    // Metrics collection
    size_t getTotalSpikeCount() const;
    size_t getFiringNeuronCount() const;
    double getAverageFiringRate() const;
    size_t getWorkingMemoryNeurons() const;
    size_t getEpisodicMemoryCount() const;
    
    // Monitoring
    std::map<std::string, double> getSystemMetrics() const;
    double getMemoryUsage() const;
    double getCPULoad() const;
    
    // Advanced features
    void enableRecording(const std::string& filepath);
    void disableRecording();
    bool isRecording() const;
    
    // Configuration
    void applyConfig(const std::shared_ptr<Config>& config);
    
private:
    std::shared_ptr<Brain> brain_;
    MetricsCollector metrics_;
    bool recording_ = false;
    std::string recordingFile_;
    mutable std::mutex mutex_;
};

// Output formatter for different output formats
class OutputFormatter {
public:
    enum class Format { Text, Json, Csv, Html };
    
    static std::string format(const DemoResult& result, Format format = Format::Text);
    static std::string format(const std::vector<DemoResult>& results, Format format = Format::Text);
    static std::string formatMetrics(const MetricsCollector& metrics, Format format = Format::Text);
    static void writeToFile(const std::string& content, const std::string& filepath);
    static std::string escapeHtml(const std::string& text);
};

} // namespace nlm
