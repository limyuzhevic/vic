#pragma once

/**
 * Configuration Optimizer Module
 * 
 * Advanced neural network configuration optimization for neural network simulation with:
 * - Configuration loading optimization
 * - Schema validation optimization
 * - Configuration caching
 * - Lazy loading
 * - Configuration compression
 * 
 * Features:
 * - Optimized configuration loading with caching
 * - Schema validation and optimization
 * - Compression for reduced memory usage
 * - Lazy loading of configuration components
 * - Configuration profiling and optimization
 */

#ifndef NLM_CONFIG_OPTIMIZER_HPP
#define NLM_CONFIG_OPTIMIZER_HPP

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <iomanip>
#include <filesystem>

#include "Performance.hpp"
#include "MemoryOptimizer.hpp"
#include "ProcessingOptimizer.hpp"

namespace nlm {

/**
 * Configuration schema field type
 */
enum class ConfigFieldType {
    INTEGER,
    FLOAT,
    STRING,
    BOOLEAN,
    VECTOR,
    MAP,
    STRUCT,
    ARRAY,
    UNION,
    ANY
};

/**
 * Configuration schema validation error
 */
struct ConfigValidationError {
    std::string path;
    std::string message;
    std::string expectedType;
    std::string actualType;
    bool isFatal;
};

/**
 * Configuration cache entry
 */
struct ConfigCacheEntry {
    std::string key;
    std::string value;
    std::chrono::steady_clock::time_point timestamp;
    size_t accessCount;
    size_t memorySize;
};

/**
 * Configuration compression statistics
 */
struct CompressionStats {
    size_t originalSize;
    size_t compressedSize;
    double compressionRatio;
    std::chrono::steady_clock::time_point compressionTime;
};

/**
 * Configuration loading statistics
 */
struct LoadingStats {
    size_t totalLoads;
    size_t successfulLoads;
    size_t failedLoads;
    double averageLoadTime;
    std::chrono::steady_clock::time_point lastLoadTime;
};

/**
 * Configuration optimizer implementing:
 * - Configuration loading optimization
 * - Schema validation optimization
 * - Configuration caching
 * - Lazy loading
 * - Configuration compression
 */
class ConfigOptimizer {
public:
    /**
     * Construct configuration optimizer with configuration
     * @param enableCaching Enable configuration caching
     * @param enableCompression Enable configuration compression
     * @param enableLazyLoading Enable lazy loading
     * @param cacheSize Maximum cache size in entries
     * @param compressionLevel Compression level (0-9)
     */
    explicit ConfigOptimizer(bool enableCaching = true,
                           bool enableCompression = true,
                           bool enableLazyLoading = true,
                           size_t cacheSize = 1024,
                           int compressionLevel = 6);
    
    ~ConfigOptimizer();
    
    // Disable copying and moving
    ConfigOptimizer(const ConfigOptimizer&) = delete;
    ConfigOptimizer& operator=(const ConfigOptimizer&) = delete;
    ConfigOptimizer(ConfigOptimizer&&) = delete;
    ConfigOptimizer& operator=(ConfigOptimizer&&) = delete;
    
    /**
     * Initialize configuration optimization system
     * @return true if initialization succeeded
     */
    bool initialize();
    
    /**
     * Finalize configuration optimization system
     */
    void finalize();
    
    /**
     * Load configuration from file with optimization
     * @param filepath Path to configuration file
     * @param[out] configData Loaded configuration data
     * @return true if loading succeeded
     */
    bool loadConfig(const std::string& filepath, std::string& configData);
    
    /**
     * Load configuration from string with optimization
     * @param configString Configuration string
     * @param[out] configData Loaded configuration data
     * @return true if loading succeeded
     */
    bool loadConfigFromString(const std::string& configString, std::string& configData);
    
    /**
     * Validate configuration against schema
     * @param configData Configuration data to validate
     * @param schema Schema to validate against
     * @param[out] errors Validation errors
     * @return true if configuration is valid
     */
    bool validateConfig(const std::string& configData, const std::string& schema,
                       std::vector<ConfigValidationError>& errors);
    
    /**
     * Validate configuration JSON structure
     * @param configData Configuration data to validate
     * @param[out] errors Validation errors
     * @return true if configuration is valid
     */
    bool validateJSONConfig(const std::string& configData,
                           std::vector<ConfigValidationError>& errors);
    
    /**
     * Compress configuration data
     * @param configData Original configuration data
     * @param[out] compressedData Compressed configuration data
     * @return Compression statistics
     */
    CompressionStats compressConfig(const std::string& configData,
                                   std::string& compressedData);
    
    /**
     * Decompress configuration data
     * @param compressedData Compressed configuration data
     * @param[out] configData Decompressed configuration data
     * @return true if decompression succeeded
     */
    bool decompressConfig(const std::string& compressedData, std::string& configData);
    
    /**
     * Get cached configuration
     * @param key Cache key
     * @param[out] configData Cached configuration data
     * @return true if configuration found in cache
     */
    bool getCachedConfig(const std::string& key, std::string& configData);
    
    /**
     * Cache configuration
     * @param key Cache key
     * @param configData Configuration data to cache
     */
    void cacheConfig(const std::string& key, const std::string& configData);
    
    /**
     * Clear configuration cache
     */
    void clearConfigCache();
    
    /**
     * Enable lazy loading for specific configuration sections
     * @param section Configuration section to lazy load
     */
    void enableLazyLoading(const std::string& section);
    
    /**
     * Disable lazy loading for specific configuration sections
     * @param section Configuration section to disable lazy loading for
     */
    void disableLazyLoading(const std::string& section);
    
    /**
     * Check if lazy loading is enabled for a section
     * @param section Configuration section to check
     * @return true if lazy loading is enabled
     */
    bool isLazyLoadingEnabled(const std::string& section) const;
    
    /**
     * Get configuration loading statistics
     * @return Loading statistics
     */
    LoadingStats getLoadingStats() const;
    
    /**
     * Get compression statistics
     * @return Compression statistics
     */
    CompressionStats getCompressionStats() const;
    
    /**
     * Generate configuration optimization report
     * @return Formatted optimization report
     */
    std::string generateReport() const;
    
    /**
     * Get optimal compression level for configuration size
     * @param configSize Size of configuration data
     * @return Optimal compression level (0-9)
     */
    static int getOptimalCompressionLevel(size_t configSize);
    
    /**
     * Profile configuration loading performance
     * @param filepath Path to configuration file
     * @param iterations Number of iterations to profile
     * @return Average load time in milliseconds
     */
    double profileLoadPerformance(const std::string& filepath, size_t iterations = 10);
    
    /**
     * Profile configuration validation performance
     * @param configData Configuration data to validate
     * @param iterations Number of iterations to profile
     * @return Average validation time in milliseconds
     */
    double profileValidationPerformance(const std::string& configData, size_t iterations = 100);

private:
    // Internal helper classes and methods
    struct ConfigSchema {
        std::string name;
        std::map<std::string, ConfigFieldType> fields;
        std::string description;
        bool isValid;
    };
    
    // Simplified compression utilities
    static std::string simpleCompress(const std::string& data);
    static std::string simpleDecompress(const std::string& data);
    
    // Schema validation utilities
    bool validateFieldType(const std::string& path, const std::string& value,
                         ConfigFieldType expectedType, ConfigValidationError& error) const;
    
    // Cache management
    void cacheEvictionPolicy();
    
    // JSON parsing utilities
    static std::string minifyJSON(const std::string& json);
    static std::string extractJSONField(const std::string& json, const std::string& field);
    
    // Configuration validation
    bool validateRequiredFields(const std::string& json, const ConfigSchema& schema,
                               std::vector<ConfigValidationError>& errors) const;
    
    // Lazy loading implementation
    bool loadLazyConfig(const std::string& section, std::string& configData);
    
    // State variables
    bool initialized_;
    bool cachingEnabled_;
    bool compressionEnabled_;
    bool lazyLoadingEnabled_;
    size_t cacheSize_;
    int compressionLevel_;
    
    // Configuration cache
    std::unordered_map<std::string, ConfigCacheEntry> configCache_;
    mutable std::mutex cacheMutex_;
    
    // Lazy loading configuration
    std::set<std::string> lazyLoadSections_;
    mutable std::mutex lazyLoadMutex_;
    
    // Statistics
    mutable std::mutex statsMutex_;
    LoadingStats loadingStats_;
    CompressionStats compressionStats_;
    
    // Pre-compiled schemas
    std::vector<ConfigSchema> compiledSchemas_;
};

} // namespace nlm

#endif // NLM_CONFIG_OPTIMIZER_HPP