// Optimized Config System Implementation
// File: src/core/Config/OptimizedConfig.hpp
#pragma once

#include "Config.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <mutex>
#include <chrono>
#include <fstream>
#include <sstream>

namespace nlm {

class OptimizedConfig : public Config {
public:
    OptimizedConfig();
    ~OptimizedConfig() override;
    
    // Disable copying, enable moving
    OptimizedConfig(const OptimizedConfig&) = delete;
    OptimizedConfig& operator=(const OptimizedConfig&) = delete;
    OptimizedConfig(OptimizedConfig&&) noexcept;
    OptimizedConfig& operator=(OptimizedConfig&&) noexcept;
    
    // Optimized loading methods
    bool loadFromFile(const std::string& filepath) override;
    bool loadFromArgs(int argc, char** argv) override;
    bool loadFromEnvironment() override;
    bool loadFromDefaults() override;
    bool loadWithInheritance(const std::vector<std::string>& filePaths) override;
    
    // Optimized value access
    template<typename T>
    std::optional<T> get(const std::string& key) const override;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const override;
    
    template<typename T>
    std::optional<T> getNested(const std::string& keyPath) const override;
    
    // Optimized internal methods
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) override;
    
    // Performance optimizations
    void enableCache(bool enable);
    void enableFastAccess(bool enable);
    void setValueCacheSize(size_t max_cached_values);
    void setAccessPatternCacheSize(size_t max_cached_patterns);
    
    // Statistics and profiling
    struct PerformanceStats {
        double total_load_time_ms;
        double total_access_time_ms;
        size_t total_values_cached;
        size_t total_patterns_cached;
        size_t cache_hit_count;
        size_t cache_miss_count;
        double cache_hit_ratio;
    };
    
    PerformanceStats getPerformanceStats() const;
    std::string generatePerformanceReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Optimized JSON Parser
class OptimizedJsonParser {
public:
    struct ParseOptions {
        bool enable_comments = true;
        bool enable_trailing_commas = true;
        bool enable_unicode_escapes = true;
        bool enable_fast_path = true;  // Use optimized parsing for simple objects
        bool enable_vector_optimization = true;
        size_t min_object_size_for_optimization = 10;
    };
    
    static std::unordered_map<std::string, ConfigValue> parse(const std::string& content, 
                                                             const ParseOptions& options = ParseOptions());
    static std::unordered_map<std::string, ConfigValue> parseFile(const std::string& filepath,
                                                                 const ParseOptions& options = ParseOptions());
    static std::unordered_map<std::string, ConfigValue> parseStream(std::istream& stream,
                                                                   const ParseOptions& options = ParseOptions());
    
    // Fast path for simple key-value pairs
    static bool parseFast(const std::string& content, std::unordered_map<std::string, ConfigValue>& config);
    
    // Memory-efficient parsing
    static bool parseMemoryEfficient(const std::string& content, std::unordered_map<std::string, ConfigValue>& config);
    
    // Optimized value conversion
    static ConfigValue convertValue(const std::string& str, ConfigValue::variant_type type);
    static bool tryConvertValue(const std::string& str, ConfigValue& target);
};

// Optimized YAML Parser
class OptimizedYamlParser {
public:
    struct ParseOptions {
        bool enable_comments = true;
        bool enable_unicode_escapes = true;
        bool enable_multiline_strings = true;
        bool enable_fast_indentation = true;
        bool enable_string_pooling = true;
        bool enable_tag_handling = false;  // For future YAML 1.2 support
    };
    
    static std::unordered_map<std::string, ConfigValue> parse(const std::string& content,
                                                             const ParseOptions& options = ParseOptions());
    static std::unordered_map<std::string, ConfigValue> parseFile(const std::string& filepath,
                                                                 const ParseOptions& options = ParseOptions());
    
    // Optimized YAML specific parsing
    static bool parseYamlScalar(const std::string& content, size_t& pos, ConfigValue& value);
    static bool parseYamlString(const std::string& content, size_t& pos, std::string& value);
    static bool parseYamlNumber(const std::string& content, size_t& pos, ConfigValue& value);
    static bool parseYamlBoolean(const std::string& content, size_t& pos, ConfigValue& value);
    static bool parseYamlArray(const std::string& content, size_t& pos, ConfigValue& value);
    static bool parseYamlMapping(const std::string& content, size_t& pos, std::unordered_map<std::string, ConfigValue>& value);
};

// Value cache for frequently accessed configurations
class ValueCache {
public:
    struct CacheEntry {
        std::string key;
        ConfigValue value;
        ConfigSource source;
        std::chrono::steady_clock::time_point access_time;
        size_t access_count;
        double access_time_ms; // Average access time
    };
    
    ValueCache(size_t max_entries = 1000);
    ~ValueCache();
    
    // Cache management
    void put(const std::string& key, const ConfigValue& value, ConfigSource source);
    std::optional<ConfigValue> get(const std::string& key);
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    void clear();
    
    // Cache statistics
    size_t size() const;
    size_t capacity() const;
    void setCapacity(size_t capacity);
    double getHitRatio() const;
    size_t getHitCount() const;
    size_t getMissCount() const;
    
    // Pattern-based access caching
    void cacheAccessPattern(const std::string& pattern, const std::vector<std::string>& keys);
    std::vector<std::string> getCachedKeys(const std::string& pattern) const;
    
    // Performance optimization
    void optimizeForFrequentAccess(const std::vector<std::string>& hot_keys);
    void enableAdaptiveCaching(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // LRU eviction
    void evictLRU();
    void updateAccessTime(const std::string& key);
};

// Fast path accessor for nested key access
class FastPathAccessor {
public:
    FastPathAccessor();
    ~FastPathAccessor();
    
    // Compile-time optimized path generation
    template<typename T>
    std::optional<T> getNestedOptimized(const std::string& keyPath) const;
    
    // Runtime optimized path caching
    void cachePath(const std::string& keyPath);
    std::vector<std::string> getPathComponents(const std::string& keyPath) const;
    
    // Pattern matching for common access patterns
    static bool isHotPattern(const std::string& keyPath);
    static std::string normalizePattern(const std::string& keyPath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Optimized validation engine
class OptimizedValidator {
public:
    OptimizedValidator();
    ~OptimizedValidator();
    
    // Batch validation for performance
    bool validateAll(const std::vector<ConfigEntry>& entries) const;
    std::vector<std::string> validateBatch(const std::vector<ConfigEntry>& entries) const;
    
    // Optimized validation rules
    void addRangeRule(const std::string& key, double min, double max);
    void addPatternRule(const std::string& key, const std::regex& pattern);
    void addDependencyRule(const std::string& key, const std::string& dependency);
    
    // Fast validation for common patterns
    bool validateHotKeys(const std::vector<std::string>& hot_keys) const;
    std::vector<std::string> validateBatchHotKeys(const std::vector<std::string>& hot_keys) const;
    
    // Performance optimization
    void enableParallelValidation(bool enable);
    void setValidationThreadCount(size_t threads);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Statistics and profiling for optimized config
class ConfigProfiler {
public:
    struct ParseStats {
        double total_time_ms;
        size_t total_bytes_processed;
        size_t file_count;
        size_t object_count;
        size_t array_count;
        double avg_time_per_object_ms;
        double avg_time_per_byte_ns;
    };
    
    struct AccessStats {
        double total_time_ms;
        size_t total_accesses;
        size_t cache_hits;
        size_t cache_misses;
        double avg_access_time_ns;
        std::unordered_map<std::string, size_t> key_access_counts;
    };
    
    ConfigProfiler();
    ~ConfigProfiler();
    
    // Profiling methods
    void recordParseTime(double duration_ms, size_t bytes_processed, size_t object_count);
    void recordAccessTime(const std::string& key, double duration_ns, bool cache_hit);
    void recordValidationTime(double duration_ms, size_t rule_count);
    
    // Statistics
    ParseStats getParseStats() const;
    AccessStats getAccessStats() const;
    double getParseThroughputMBps() const;
    double getAccessThroughputOpsPerSec() const;
    
    // Reporting
    std::string generatePerformanceReport() const;
    void savePerformanceReport(const std::string& filename) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
