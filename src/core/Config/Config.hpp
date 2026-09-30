#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <numeric>
#include <functional>
#include <unordered_map>
#include <chrono>
#include "Performance/PerformanceMonitoring.hpp"

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Batch operation result structure
struct BatchOperationResult {
    size_t operationsCompleted;
    double totalTime;
    std::vector<double> operationTimes;
    bool success;
    std::string errorMessage;
};

// Performance monitoring for config operations
struct ConfigPerformanceStats {
    PerformanceCounter loadCounter;
    PerformanceCounter saveCounter;
    PerformanceCounter batchLoadCounter;
    PerformanceCounter batchSaveCounter;
    Timer avgLoadTimer;
    Timer avgSaveTimer;
    Timer avgBatchLoadTimer;
    Timer avgBatchSaveTimer;
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // ========== BATCH OPERATIONS ==========
    
    // Batch load multiple configuration files
    BatchOperationResult batchLoadFiles(const std::vector<std::string>& filepaths,
        bool parallel = true, bool merge = true);
    
    // Batch save multiple configuration files
    BatchOperationResult batchSaveFiles(const std::vector<std::string>& filepaths,
        bool parallel = true);
    
    // Batch get values from multiple keys
    std::vector<std::optional<ConfigValue>> batchGet(const std::vector<std::string>& keys) const;
    
    // Batch set values for multiple keys
    BatchOperationResult batchSet(const std::vector<std::pair<std::string, ConfigValue>>& keyValues,
        ConfigSource source = ConfigSource::Runtime, bool parallel = true);
    
    // Performance monitoring methods
    void recordPerformanceStat(const std::string& name, double time);
    void recordPerformanceCounter(const std::string& name, size_t count, double timePerOp = 0.0);
    void printPerformanceReport() const;
    void resetPerformanceStats();
    
    // Memory optimization utilities
    void preAllocateMemory();
    void optimizeMemoryAccess();
    size_t getCurrentMemoryUsage() const;
    
    // Progress tracking
    void startLongOperation(const std::string& description, size_t totalSteps);
    void advanceProgress(size_t steps = 1);
    std::string getProgressStatus() const;
    
    // Benchmarking
    struct BenchmarkResult {
        std::string operationName;
        size_t iterations;
        double totalTime;
        double avgTime;
        double minTime;
        double maxTime;
        size_t operationsPerSecond;
    };
    
    BenchmarkResult benchmarkOperation(const std::string& name,
        std::function<void()> operation, size_t iterations = 100);
    
    // SIMD/parallel processing
    static void processConfigEntriesSIMD(const std::vector<ConfigEntry*>& entries,
        const std::function<void(ConfigEntry*)>& operation);
    
    static void processConfigValuesSIMD(const std::vector<ConfigValue>& values,
        const std::function<void(ConfigValue&)>& operation);
    
    // Thread pool for parallel operations
    static ThreadPool& getThreadPool(size_t numThreads = std::thread::hardware_concurrency());
    
    // Cache management for frequently accessed data
    void cacheConfigEntries();
    void clearConfigCache();
    const ConfigEntry* getCachedConfigEntry(const std::string& key) const;
    
    // Compressed data operations
    bool compressState(const std::string& filepath, float compressionLevel = 0.5f) const;
    bool decompressState(std::string& filepath, float compressionLevel = 0.5f);
    
    // Enhanced initialization with batch setup
    bool initializeWithBatchSetup(const std::vector<std::string>& configStrings,
        bool merge = true);
    
    // Statistics and reporting
    size_t getEntryCount() const;
    std::vector<ConfigEntry> getEntries() const;
    std::string getSourceSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Performance monitoring
    mutable ConfigPerformanceStats performanceStats_;
    
    // Progress tracking
    mutable ProgressTracker progressTracker_;
    
    // Memory cache
    mutable std::unordered_map<std::string, ConfigEntry*> configCache_;
    mutable std::mutex cacheMutex_;
    
    // Thread pool for batch operations
    static thread_local ThreadPool threadPool_;
    
    // SIMD optimization flags
    static constexpr bool USE_SIMD = true;
    static constexpr bool USE_VECTORIZATION = true;
    static constexpr bool USE_PARALLEL = true;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
