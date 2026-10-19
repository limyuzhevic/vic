#pragma once

/**
 * @file Config.hpp
 * @brief Configuration system for NLM framework
 * @author NLM Framework
 * @version 6.0
 * @date 2024
 * 
 * @details
 * The Config class provides a flexible configuration system for the NLM framework,
 * allowing runtime configuration management through multiple sources:
 * 
 * 1. **Default Configuration**: Built-in default values
 * 2. **File Configuration**: Loading from text configuration files
 * 3. **Command Line**: Parsing command line arguments
 * 4. **Runtime**: Dynamic configuration changes during execution
 * 
 * This configuration system supports various data types including primitive
 * types, strings, and arrays. It provides type-safe access to configuration
 * values with automatic conversion and default value support.
 * 
 * **Performance Optimizations Implemented**:
 * - O(1) fast access patterns using std::unordered_map
 * - Memory pool for ConfigEntry objects
 * - SIMD-accelerated numeric parsing
 * - Buffered I/O with lazy loading
 * - Thread-safe concurrent access with read-optimized patterns
 * - Value compression for numeric types
 * - Small string optimization
 * - Thread-local caching
 * 
 * **Advanced Configuration Features**:
 * - Configuration Profiles with inheritance
 * - Configuration Macros and templates
 * - Comprehensive validation system
 * - Automatic documentation generation
 * - Configuration diffing and merging
 * - Configuration history tracking
 * - Incremental loading and lazy evaluation
 * 
 * @section architecture Architecture Overview
 * The configuration system uses a variant-based approach to support multiple
 * data types while maintaining type safety. Each configuration entry consists
 * of:
 * - A key string for identification
 * - A value stored as a variant
 * - A source enum indicating where the value came from
 * - A description for documentation
 * 
 * **Fast Access Architecture**:
 * - Primary storage: std::unordered_map<std::string, ConfigEntry> for O(1) lookup
 * - Secondary cache: ConfigCache for frequently accessed values
 * - Thread-local cache for per-thread read optimization
 * - Memory pool for efficient ConfigEntry allocation
 * 
 * **Hierarchical Configuration**:
 * - Configuration profiles with parent-child relationships
 * - Configuration macros for preprocessor-style expansion
 * - Validation rules with error details
 * - Templates for reusable configuration patterns
 * 
 * **Persistence and Performance**:
 * - Buffered I/O with async file operations
 * - Lazy loading for large configuration files
 * - Incremental configuration loading
 * - Checksum validation for configuration files
 * 
 * This design enables:
 * - **Type Safety**: Safe access to configuration values
 * - **Flexibility**: Support for multiple data types
 * - **Transparency**: Tracking configuration sources
 * - **Persistence**: Saving configuration to files
 * - **Performance**: Optimized for both small and large configurations
 * - **Scalability**: Supports concurrent access patterns
 * 
 * @section usage Usage Examples
 * @code
 * // Create configuration with fast access
 * Config config;
 * 
 * // Set values with validation
 * config.set("neuron_count", 1000);
 * config.set("connection_probability", 0.1f);
 * config.set("simulation_timestep", 0.001);
 * 
 * // Fast O(1) access patterns
 * auto count = config.getFast<size_t>("neuron_count");
 * auto prob = config.getOrFast<float>("connection_probability", 0.1f);
 * 
 * // Configuration profiles
 * config.createProfile("production", {"neuron_count", "connection_probability"});
 * config.applyProfile("production");
 * 
 * // Load with lazy loading
 * config.loadFromFile("nlm_config.txt", true); // lazy = true
 * 
 * // Thread-safe concurrent reads
 * auto val1 = config.getConcurrently<int>("key1");
 * auto val2 = config.getConcurrently<double>("key2");
 * 
 * // Get values with type safety
 * auto count = config.get<size_t>("neuron_count");
 * float prob = config.getOr<float>("connection_probability", 0.1f);
 * 
 * // Load from file
 * config.loadFromFile("nlm_config.txt");
 * 
 * // Load from command line
 * config.loadFromArgs(argc, argv);
 * @endcode
 * 
 * @section file_format File Format
 * Configuration files use a simple key=value format:
 * @code
 * # Configuration for NLM Brain
 * neuron_count = 1000
 * connection_probability = 0.1
 * simulation_timestep = 0.001
 * reward_scale = 1.0
 * @endcode
 * 
 * Comments start with # or / and lines are trimmed of whitespace.
 * Values can be quoted strings, integers, floats, booleans, or arrays.
 * 
 * **Advanced File Formats**:
 * - JSON format: {"neuron_count": 1000, "connection_probability": 0.1}
 * - YAML format: neuron_count: 1000\nconnection_probability: 0.1
 * - TOML format: neuron_count = 1000\nconnection_probability = 0.1
 * 
 * @section thread_safety Thread Safety
 * The Config class provides **thread-safe concurrent read access**:
 * - Read operations are lock-free using RCU-style patterns
 * - Write operations use fine-grained locking
 * - Thread-local caching for improved read performance
 * - Memory pool with thread-local allocations
 * 
 * @section performance Performance Considerations
 * - O(1) fast access patterns with std::unordered_map
 * - Memory-efficient with small string optimization
 * - SIMD-accelerated numeric parsing
 * - Buffered I/O with async operations
 * - Lazy loading for large configuration files
 * - LRU cache for frequently accessed values
 * - Pre-allocation for common value types
 * - Optimized for small configuration sets (typical use case)
 * 
 * @note This implementation requires C++17 or later for modern C++ features.
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <list>
#include <functional>
#include <thread>
#include <span>
#include <numeric>
#include <execution>
#include <tuple>

namespace nlm {

/**
 * @brief Configuration value types
 * 
 * Variant type that stores different kinds of configuration values.
 * Supports primitive types, strings, and arrays for flexible configuration.
 * 
 * Supported types:
 * - Primitive: int, int64_t, double, bool
 * - Text: std::string
 * - Collections: std::vector<int>, std::vector<double>, std::vector<std::string>
 * 
 * The variant enables type-safe storage while maintaining flexibility
 * for configuration needs.
 * 
 * **Memory Optimizations**:
 * - Small string optimization for strings
 * - Numeric compression for integers and doubles
 * - Vector pre-allocation for common array types
 */
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

/**
 * @brief Configuration source enumeration
 * 
 * Indicates where a configuration value originated from.
 * This helps track the precedence and source of configuration values.
 */
enum class ConfigSource {
public:
    /** Default configuration - built into the application */
    Default,
    
    /** File configuration - loaded from a configuration file */
    File,
    
    /** Command line configuration - parsed from command line arguments */
    CommandLine,
    
    /** Runtime configuration - changed during execution */
    Runtime,
    
    /** Profile configuration - applied from named profile */
    Profile,
    
    /** Macro configuration - expanded from configuration macro */
    Macro
};

/**
 * @brief Configuration format enumeration
 * 
 * Indicates the file format of a configuration source.
 */
enum class ConfigFormat {
public:
    /** Plain text key=value format */
    Text,
    
    /** JSON format */
    JSON,
    
    /** YAML format */
    YAML,
    
    /** TOML format */
    TOML
};

/**
 * @brief Configuration profile metadata
 * 
 * Contains metadata about a configuration profile including its source,
 * inheritance, and description.
 */
struct ConfigProfile {
    std::string name;
    std::vector<std::string> keys;
    std::string description;
    ConfigSource source;
    std::chrono::steady_clock::time_point created;
    std::unordered_map<std::string, std::string> inheritance;
    std::unordered_map<std::string, std::string> macros;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
    std::string parentProfile;
    bool isActive;
    
    ConfigProfile() : source(ConfigSource::Default), created(std::chrono::steady_clock::now()),
                     isActive(false) {}
    
    ConfigProfile(const std::string& name, const std::vector<std::string>& keys, 
                  const std::string& description = "", ConfigSource source = ConfigSource::Profile)
        : name(name), keys(keys), description(description), source(source),
          created(std::chrono::steady_clock::now()), isActive(false) {}
};

/**
 * @brief Configuration macro definition
 * 
 * Defines a configuration macro that can be expanded to multiple values.
 */
struct ConfigMacro {
    std::string name;
    std::string definition;
    std::vector<std::string> parameters;
    std::string description;
    ConfigSource source;
    
    ConfigMacro() : source(ConfigSource::Macro) {}
    
    ConfigMacro(const std::string& name, const std::string& definition,
                const std::vector<std::string>& params = {},
                const std::string& description = "", ConfigSource source = ConfigSource::Macro)
        : name(name), definition(definition), parameters(params),
          description(description), source(source) {}
};

/**
 * @brief Single configuration entry
 * 
 * Represents a single configuration key-value pair with metadata.
 * Used internally by the Config class to manage configuration state.
 * 
 * Each entry contains:
 * - Key: Unique identifier for the configuration parameter
 * - Value: The actual configuration value
 * - Source: Where the value originated
 * - Description: Human-readable documentation
 * 
 * **Memory Optimizations**:
 * - Small string optimization for keys and descriptions
 * - Inline storage for frequently used values
 * - Custom allocator for efficient memory pooling
 */
struct ConfigEntry {
    /** Configuration key for identification */
    std::string key;
    
    /** Configuration value stored as variant */
    ConfigValue value;
    
    /** Source of this configuration value */
    ConfigSource source;
    
    /** Human-readable description of the configuration parameter */
    std::string description;
    
    /**
     * @brief Timestamp when entry was created/modified
     */
    std::chrono::steady_clock::time_point timestamp;
    
    /**
     * @brief Access count for LRU cache
     */
    mutable size_t accessCount;
    
    /**
     * @brief Checksum for validation
     */
    std::string checksum;
    
    /**
     * @brief Configuration metadata
     */
    std::unordered_map<std::string, std::string> metadata;
    
    /**
     * @brief Default constructor
     * Creates an empty configuration entry with default source.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(),
                   timestamp(std::chrono::steady_clock::now()), accessCount(0) {}
    
    /**
     * @brief Parameterized constructor
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of configuration
     * @param desc Description of the configuration parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc),
          timestamp(std::chrono::steady_clock::now()), accessCount(0) {}
    
    /**
     * @brief Equality comparison
     */
    bool operator==(const ConfigEntry& other) const {
        return key == other.key && value == other.value && source == other.source;
    }
};

/**
 * @brief Memory pool for ConfigEntry objects
 * 
 * Provides efficient allocation and deallocation of ConfigEntry objects
 * using a custom memory pool to reduce fragmentation and improve performance.
 */
class ConfigEntryPool {
public:
    struct Block {
        std::aligned_storage_t<sizeof(ConfigEntry), alignof(ConfigEntry)> data[64];  // 64 entries per block
        Block* next = nullptr;
    };
    
    ConfigEntryPool(size_t initialBlocks = 16) {
        for (size_t i = 0; i < initialBlocks; ++i) {
            Block* block = static_cast<Block*>(std::aligned_alloc(alignof(Block), sizeof(Block)));
            block->next = head;
            head = block;
        }
    }
    
    ~ConfigEntryPool() {
        Block* current = head;
        while (current) {
            Block* next = current->next;
            std::free(current);
            current = next;
        }
    }
    
    ConfigEntry* allocate() {
        if (!freeList.empty()) {
            ConfigEntry* entry = freeList.back();
            freeList.pop_back();
            return entry;
        }
        
        // Check if current block is full
        if (currentBlockIndex >= blockSize) {
            if (!head->next) {
                // Expand pool
                Block* newBlock = static_cast<Block*>(std::aligned_alloc(alignof(Block), sizeof(Block)));
                newBlock->next = head;
                head = newBlock;
            }
            currentBlock = head;
            currentBlockIndex = 0;
        }
        
        return reinterpret_cast<ConfigEntry*>(&currentBlock->data[currentBlockIndex++]);
    }
    
    void deallocate(ConfigEntry* entry) {
        // Find the block containing this entry
        Block* current = head;
        while (current) {
            auto* blockData = reinterpret_cast<ConfigEntry**>(current->data);
            for (size_t i = 0; i < blockSize; ++i) {
                if (blockData[i] == entry) {
                    freeList.push_back(entry);
                    return;
                }
            }
            current = current->next;
        }
    }
    
    size_t size() const { return totalAllocated - freeList.size(); }
    void clear() {
        freeList.clear();
        currentBlockIndex = 0;
    }
    
private:
    static constexpr size_t blockSize = 64;
    Block* head = nullptr;
    Block* currentBlock = nullptr;
    size_t currentBlockIndex = blockSize;
    std::vector<ConfigEntry*> freeList;
    std::atomic<size_t> totalAllocated{0};
};

/**
 * @class Config
 * @brief Configuration management class
 * 
 * The Config class provides comprehensive configuration management for the
 * NLM framework. It supports loading from multiple sources, type-safe value
 * access, and persistence to files.
 * 
 * **Performance Features**:
 * - O(1) fast access patterns using std::unordered_map
 * - Memory-efficient ConfigEntry pooling
 * - Thread-safe concurrent read access
 * - SIMD-accelerated numeric parsing
 * - Buffered I/O with async operations
 * - LRU cache for frequently accessed values
 * - Small string optimization
 * - Numeric compression for common types
 * 
 * **Advanced Features**:
 * - Configuration profiles with inheritance
 * - Configuration macros and templates
 * - Comprehensive validation system
 * - Automatic documentation generation
 * - Configuration diffing and merging
 * - Configuration history tracking
 * - Incremental loading and lazy evaluation
 * 
 * Key features:
 * - **Multiple sources**: Default, file, command line, runtime, profile, macro
 * - **Type safety**: Safe access to configuration values
 * - **Flexible types**: Support for primitives, strings, and arrays
 * - **Persistence**: Save/load configuration to/from files
 * - **Transparency**: Track configuration sources
 * - **Validation**: Automatic type checking and conversion
 * - **Performance**: Optimized for both small and large configurations
 * - **Scalability**: Supports concurrent access patterns
 * 
 * The class uses a pImpl pattern to hide implementation complexity
 * while providing a clean, stable public API.
 */
class Config {
public:
    /**
     * @brief Default constructor
     * @details
     * Creates an empty configuration object with default values.
     * The configuration starts with no entries, using default values
     * when needed.
     * 
     * Initializes:
     * - Fast lookup table (unordered_map)
     * - Memory pool for ConfigEntry objects
     * - Thread-local cache
     * - Performance optimization flags
     */
    Config();
    
    /**
     * @brief Destructor
     * Cleans up implementation details and resources.
     */
    ~Config();
    
    // Disable copying for thread safety
    /**
     * @brief Disable copy constructor
     * Prevents multiple objects from sharing configuration state.
     * 
     * @param other Another Config object (cannot be used)
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disable copy assignment operator
     * Prevents assignment between configuration objects.
     * 
     * @param other Another Config object (cannot be used)
     * @return Reference to this object (not used)
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor
     * @param other Another Config object to move from
     * @details
     * Efficiently transfers configuration ownership from another object.
     * The source object is left in a valid but unspecified state.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * @param other Another Config object to move from
     * @return Reference to this object
     * @details
     * Efficiently transfers configuration ownership from another object.
     * The source object is left in a valid but unspecified state.
     * Self-assignment is handled correctly.
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file with lazy loading option
     * @param filepath Path to configuration file
     * @param lazy Whether to load only metadata and defer full loading
     * @return bool True if file was successfully loaded and parsed
     * @details
     * Loads configuration from a text/JSON/YAML/TOML file with key=value format.
     * Supports multiple file formats and provides incremental loading.
     * 
     * **Performance Features**:
     * - Buffered I/O for faster file reading
     * - Lazy loading for large files
     * - Checksum validation for file integrity
     * - Async parsing for better performance
     */
    bool loadFromFile(const std::string& filepath, bool lazy = false);
    
    /**
     * @brief Load configuration from JSON/YAML/TOML content
     * @param content File content as string
     * @param format Format of the content
     * @param lazy Whether to parse lazily
     * @return bool True if content was successfully parsed
     * @details
     * Parses configuration from JSON, YAML, or TOML format.
     * Provides fast parsing with SIMD acceleration where possible.
     */
    bool loadFromContent(const std::string& content, ConfigFormat format = ConfigFormat::Text, bool lazy = false);
    
    /**
     * @brief Load configuration from command line arguments
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     * @return bool True if arguments were successfully parsed
     * @details
     * Parses command line arguments in two formats:
     * - `--key=value` for long options
     * - `-k value` for short options
     * 
     * **Performance Optimizations**:
     * - SIMD-friendly string operations
     * - Fast type detection
     * - Pre-allocated buffers for parsing
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file with compression option
     * @param filepath Path to output configuration file
     * @param format Output file format
     * @param compress Whether to compress the output
     * @return bool True if file was successfully created and written
     * @details
     * Saves configuration to a text/JSON/YAML/TOML file.
     * Supports compression for large configurations.
     * 
     * **Performance Features**:
     * - Buffered I/O for faster writing
     * - Async file operations
     * - Format-specific optimizations
     * - Delta compression for incremental saves
     */
    bool saveToFile(const std::string& filepath, ConfigFormat format = ConfigFormat::Text, bool compress = false) const;
    
    /**
     * @brief Get configuration value with fast O(1) access
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @return std::optional<T> Value if key exists and type matches, nullopt otherwise
     * @details
     * Fast O(1) access using std::unordered_map for optimal performance.
     * Uses thread-local cache for even faster read access.
     * 
     * **Performance Features**:
     * - O(1) lookup using unordered_map
     * - Thread-local caching
     * - SIMD-friendly type checking
     * - Pre-allocated temporary buffers
     */
    template<typename T>
    std::optional<T> getFast(const std::string& key) const;
    
    /**
     * @brief Get configuration value with fast access and default fallback
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist or type mismatch
     * @return T Retrieved value or default
     * @details
     * Fast path with cache lookup and SIMD-friendly operations.
     * Optimized for frequently accessed configuration values.
     */
    template<typename T>
    T getOrFast(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Get configuration value with concurrent access support
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @return std::optional<T> Value if key exists and type matches, nullopt otherwise
     * @details
     * Thread-safe read access without blocking other readers.
     * Uses RCU-style read-copy-update patterns.
     */
    template<typename T>
    std::optional<T> getConcurrently(const std::string& key) const;
    
    /**
     * @brief Get configuration value with concurrent access and default fallback
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist or type mismatch
     * @return T Retrieved value or default
     * @details
     * Thread-safe read access with default fallback.
     * Optimized for concurrent read-heavy workloads.
     */
    template<typename T>
    T getOrConcurrently(const std::string& key, const T& defaultValue) const;
    
    // Standard API methods (for backward compatibility)
    /**
     * @brief Get configuration value with type safety
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @return std::optional<T> Value if key exists and type matches, nullopt otherwise
     * @details
     * Retrieves a configuration value with compile-time type checking.
     * If the key doesn't exist or the stored type doesn't match, returns nullopt.
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist or type mismatch
     * @return T Retrieved value or default
     * @details
     * Retrieves a configuration value, returning a default if the key
     * doesn't exist or the type doesn't match.
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Configuration Setting Methods
    /**
     * @brief Set configuration value with validation
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of configuration (default: Runtime)
     * @details
     * Sets or updates a configuration value with comprehensive validation.
     * Triggers update callbacks and invalidates cache.
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration string value
     * @param key Configuration key
     * @param value String value
     * @param source Source of configuration (default: Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration integer value
     * @param key Configuration key
     * @param value Integer value
     * @param source Source of configuration (default: Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration double value
     * @param key Configuration key
     * @param value Double value
     * @param source Source of configuration (default: Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration boolean value
     * @param key Configuration key
     * @param value Boolean value
     * @param source Source of configuration (default: Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration array value
     * @tparam T Array element type
     * @param key Configuration key
     * @param values Array values
     * @param source Source of configuration (default: Runtime)
     */
    template<typename T>
    void set(const std::string& key, const std::vector<T>& values, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration Profile Methods
    /**
     * @brief Create a new configuration profile
     * @param profile Configuration profile to create
     * @return bool True if profile was created successfully
     */
    bool createProfile(const ConfigProfile& profile);
    
    /**
     * @brief Delete a configuration profile
     * @param profileName Name of the profile to delete
     * @return bool True if profile was deleted
     */
    bool deleteProfile(const std::string& profileName);
    
    /**
     * @brief Apply a configuration profile
     * @param profileName Name of the profile to apply
     * @param overwrite Whether to overwrite existing values
     * @return bool True if profile was applied
     */
    bool applyProfile(const std::string& profileName, bool overwrite = true);
    
    /**
     * @brief Get all configuration profile names
     * @return std::vector<std::string> List of profile names
     */
    std::vector<std::string> getProfileNames() const;
    
    /**
     * @brief Get profile information
     * @param profileName Name of the profile
     * @return std::optional<ConfigProfile> Profile information if found
     */
    std::optional<ConfigProfile> getProfile(const std::string& profileName) const;
    
    // Configuration Macro Methods
    /**
     * @brief Define a configuration macro
     * @param macro Configuration macro to define
     * @return bool True if macro was defined
     */
    bool defineMacro(const ConfigMacro& macro);
    
    /**
     * @brief Expand a configuration macro
     * @param macroName Name of the macro to expand
     * @param parameters Macro parameters (key-value pairs)
     * @return std::unordered_map<std::string, ConfigValue> Expanded configuration
     */
    std::unordered_map<std::string, ConfigValue> expandMacro(const std::string& macroName, 
                                                           const std::unordered_map<std::string, std::string>& parameters = {});
    
    /**
     * @brief Get all macro names
     * @return std::vector<std::string> List of macro names
     */
    std::vector<std::string> getMacroNames() const;
    
    // Configuration Validation Methods
    /**
     * @brief Add a validation rule for a configuration key
     * @param key Configuration key to validate
     * @param validator Validation function
     * @details
     * Adds a custom validation rule that will be called when the key is set.
     * Validation rules can be added for any configuration key.
     */
    void addValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator);
    
    /**
     * @brief Remove a validation rule
     * @param key Configuration key to remove validation for
     */
    void removeValidator(const std::string& key);
    
    /**
     * @brief Validate a configuration key-value pair
     * @param key Configuration key
     * @param value Configuration value
     * @return bool True if value is valid
     */
    bool validate(const std::string& key, const ConfigValue& value) const;
    
    /**
     * @brief Get validation error message
     * @param key Configuration key
     * @param value Configuration value
     * @return std::string Error message if validation fails, empty string otherwise
     */
    std::string getValidationError(const std::string& key, const ConfigValue& value) const;
    
    // Configuration Management Methods
    /**
     * @brief Get configuration diff between this and another config
     * @param other Other configuration to compare with
     * @return std::vector<std::pair<std::string, std::pair<ConfigValue, ConfigValue>>> Differences
     */
    std::vector<std::pair<std::string, std::pair<ConfigValue, ConfigValue>>> getDiff(const Config& other) const;
    
    /**
     * @brief Merge another configuration into this one
     * @param other Other configuration to merge
     * @param overwrite Whether to overwrite existing values
     */
    void merge(const Config& other, bool overwrite = true);
    
    /**
     * @brief Prune unused configuration entries
     * @param threshold Minimum access count to keep entry
     */
    void pruneEntries(size_t threshold = 1);
    
    /**
     * @brief Clean up stale entries
     * @param maxAge Maximum age in seconds for entries to keep
     */
    void cleanupStaleEntries(std::chrono::seconds maxAge = std::chrono::hours(24));
    
    // Performance and Statistics Methods
    /**
     * @brief Get configuration statistics
     * @return std::unordered_map<std::string, size_t> Statistics
     */
    std::unordered_map<std::string, size_t> getStatistics() const;
    
    /**
     * @brief Enable/disable caching
     * @param enable Whether to enable caching
     */
    void setCacheEnabled(bool enable);
    
    /**
     * @brief Enable/disable fast path
     * @param enable Whether to enable fast path
     */
    void setFastPathEnabled(bool enable);
    
    /**
     * @brief Enable/disable validation on set
     * @param enable Whether to enable validation on set
     */
    void setValidationOnSetEnabled(bool enable);
    
    /**
     * @brief Get memory pool statistics
     * @return std::unordered_map<std::string, size_t> Pool statistics
     */
    std::unordered_map<std::string, size_t> getMemoryPoolStats() const;
    
    // Standard API methods (backward compatibility)
    /**
     * @brief Check if configuration key exists
     * @param key Configuration key to check
     * @return bool True if key exists in configuration
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * @return std::vector<std::string> List of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     */
    void clear();
    
    /**
     * @brief Generate configuration summary
     * @return std::string Human-readable configuration summary
     */
    std::string summary() const;
    
    // Callback and Event System
    /**
     * @brief Add a configuration change callback
     * @param callback Callback function to execute on configuration changes
     */
    void addChangeCallback(std::function<void(const std::string&, const ConfigValue&)> callback);
    
    /**
     * @brief Remove a configuration change callback
     * @param callback Callback function to remove
     */
    void removeChangeCallback(std::function<void(const std::string&, const ConfigValue&)> callback);
    
    /**
     * @brief Get the number of active callbacks
     * @return size_t Number of active callbacks
     */
    size_t getCallbackCount() const;
    
private:
    /**
     * @brief Implementation details for Config class
     * @details
     * Private implementation class encapsulating all complex internal state
     * of the configuration system, following the pImpl pattern for clean API design.
     * 
     * The implementation manages:
     * - Fast lookup table (unordered_map)
     * - Memory pool for ConfigEntry objects
     * - Thread-local cache
     * - Configuration profiles and macros
     * - Validation rules
     * - Update callbacks
     * - Performance optimization flags
     * - I/O buffers and streams
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @name Internal helper methods
     * Private helper methods for configuration processing
     */
    ///@{
    
    /**
     * @brief Trim whitespace from string
     * @param str Input string
     * @return std::string String with leading/trailing whitespace removed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * @param str Input string
     * @return std::string Lowercase version of input
     */
    static std::string toLower(const std::string& str);
    
    /**
     * @brief Compute checksum for a configuration value
     * @param value Configuration value
     * @return std::string Checksum string
     */
    static std::string computeChecksum(const ConfigValue& value);
    
    /**
     * @brief Parse configuration value from string
     * @param str String representation
     * @return std::optional<ConfigValue> Parsed value or nullopt on failure
     */
    static std::optional<ConfigValue> parseValue(const std::string& str);
    
    /**
     * @brief Format configuration value to string
     * @param value Configuration value
     * @return std::string Formatted string
     */
    static std::string formatValue(const ConfigValue& value);
    ///@}
};

// Explicit template instantiations for common types
/**
 * @brief Explicit template instantiations
 * @details
 * Explicitly instantiates common template methods to reduce compilation time
 * and ensure they are available when needed.
 */
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Fast access template instantiations
template std::optional<int> Config::getFast<int>(const std::string&) const;
template std::optional<int64_t> Config::getFast<int64_t>(const std::string&) const;
template std::optional<double> Config::getFast<double>(const std::string&) const;
template std::optional<bool> Config::getFast<bool>(const std::string&) const;
template std::optional<std::string> Config::getFast<std::string>(const std::string&) const;

template int Config::getOrFast<int>(const std::string&, const int&) const;
template int64_t Config::getOrFast<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOrFast<double>(const std::string&, const double&) const;
template bool Config::getOrFast<bool>(const std::string&, const bool&) const;
template std::string Config::getOrFast<std::string>(const std::string&, const std::string&) const;

// Concurrent access template instantiations
template std::optional<int> Config::getConcurrently<int>(const std::string&) const;
template std::optional<int64_t> Config::getConcurrently<int64_t>(const std::string&) const;
template std::optional<double> Config::getConcurrently<double>(const std::string&) const;
template std::optional<bool> Config::getConcurrently<bool>(const std::string&) const;
template std::optional<std::string> Config::getConcurrently<std::string>(const std::string&) const;

template int Config::getOrConcurrently<int>(const std::string&, const int&) const;
template int64_t Config::getOrConcurrently<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOrConcurrently<double>(const std::string&, const double&) const;
template bool Config::getOrConcurrently<bool>(const std::string&, const bool&) const;
template std::string Config::getOrConcurrently<std::string>(const std::string&, const std::string&) const;

// Array set template instantiations
template void Config::set<int>(const std::string&, const std::vector<int>&, ConfigSource);
template void Config::set<double>(const std::string&, const std::vector<double>&, ConfigSource);
template void Config::set<std::string>(const std::string&, const std::vector<std::string>&, ConfigSource);

} // namespace nlm

// Forward declarations for helper classes
namespace nlm {
    class ConfigValidator;
    class ConfigCache;
    class FastConfigLoader;
}

/**
 * @brief Single configuration entry
 * 
 * Represents a single configuration key-value pair with metadata.
 * Used internally by the Config class to manage configuration state.
 * 
 * Each entry contains:
 * - Key: Unique identifier for the configuration parameter
 * - Value: The actual configuration value
 * - Source: Where the value originated
 * - Description: Human-readable documentation
 */
struct ConfigEntry {
    /** Configuration key for identification */
    std::string key;
    
    /** Configuration value stored as variant */
    ConfigValue value;
    
    /** Source of this configuration value */
    ConfigSource source;
    
    /** Human-readable description of the configuration parameter */
    std::string description;
    
    /**
     * @brief Default constructor
     * Creates an empty configuration entry with default source.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Parameterized constructor
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of configuration
     * @param desc Description of the configuration parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @class Config
 * @brief Configuration management class
 * 
 * The Config class provides comprehensive configuration management for the
 * NLM framework. It supports loading from multiple sources, type-safe value
 * access, and persistence to files.
 * 
 * Key features:
 * - **Multiple sources**: Default, file, command line, runtime
 * - **Type safety**: Safe access to configuration values
 * - **Flexible types**: Support for primitives, strings, and arrays
 * - **Persistence**: Save/load configuration to/from files
 * - **Transparency**: Track configuration sources
 * - **Validation**: Automatic type checking and conversion
 * 
 * The class uses a pImpl pattern to hide implementation complexity
 * while providing a clean, stable public API.
 */
class Config {
public:
    /**
     * @brief Default constructor
     * @details
     * Creates an empty configuration object with default values.
     * The configuration starts with no entries, using default values
     * when needed.
     */
    Config();
    
    /**
     * @brief Destructor
     * Cleans up implementation details and resources.
     */
    ~Config();
    
    // Disable copying for thread safety
    /**
     * @brief Disable copy constructor
     * Prevents multiple objects from sharing configuration state.
     * 
     * @param other Another Config object (cannot be used)
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disable copy assignment operator
     * Prevents assignment between configuration objects.
     * 
     * @param other Another Config object (cannot be used)
     * @return Reference to this object (not used)
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor
     * @param other Another Config object to move from
     * @details
     * Efficiently transfers configuration ownership from another object.
     * The source object is left in a valid but unspecified state.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * @param other Another Config object to move from
     * @return Reference to this object
     * @details
     * Efficiently transfers configuration ownership from another object.
     * The source object is left in a valid but unspecified state.
     * Self-assignment is handled correctly.
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * @param filepath Path to configuration file
     * @return bool True if file was successfully loaded and parsed
     * @details
     * Loads configuration from a text file with key=value format.
     * File format details are described in the file header.
     * 
     * @warning File must be readable and in correct format.
     * @post Configuration entries are added with File source.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     * @return bool True if arguments were successfully parsed
     * @details
     * Parses command line arguments in two formats:
     * - `--key=value` for long options
     * - `-k value` for short options
     * 
     * Supported types include integers, floats, booleans, and strings.
     * Arguments are added with CommandLine source.
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * @param filepath Path to output configuration file
     * @return bool True if file was successfully created and written
     * @details
     * Saves configuration to a text file in key=value format.
     * Includes comments for each entry description.
     * 
     * @note Values are written as strings for simplicity.
     * @warning File may be overwritten if it already exists.
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @return std::optional<T> Value if key exists and type matches, nullopt otherwise
     * @details
     * Retrieves a configuration value with compile-time type checking.
     * If the key doesn't exist or the stored type doesn't match, returns nullopt.
     * 
     * @tparam T Must be one of the supported ConfigValue types
     * @param key Unique identifier for the configuration parameter
     * @return Optional containing the value, or empty if not found/incompatible
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist or type mismatch
     * @return T Retrieved value or default
     * @details
     * Retrieves a configuration value, returning a default if the key
     * doesn't exist or the type doesn't match. This is the preferred
     * method for accessing optional configuration parameters.
     * 
     * @tparam T Must be one of the supported ConfigValue types
     * @param key Configuration key to look up
     * @param defaultValue Fallback value when key is not found
     * @return Retrieved value or provided default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @name Configuration Setting Methods
     * Methods for setting configuration values
     */
    ///@{ 
    
    /**
     * @brief Set configuration value
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of configuration (default: Runtime)
     * @details
     * Sets or updates a configuration value. If the key already exists,
     * its value and source are updated. Otherwise, a new entry is created.
     * 
     * @param key Unique identifier for the configuration parameter
     * @param value Configuration value to store
     * @param source Origin of this configuration value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration string value
     * @param key Configuration key
     * @param value String value
     * @param source Source of configuration (default: Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration integer value
     * @param key Configuration key
     * @param value Integer value
     * @param source Source of configuration (default: Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration double value
     * @param key Configuration key
     * @param value Double value
     * @param source Source of configuration (default: Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration boolean value
     * @param key Configuration key
     * @param value Boolean value
     * @param source Source of configuration (default: Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    ///@}
    
    /**
     * @brief Check if configuration key exists
     * @param key Configuration key to check
     * @return bool True if key exists in configuration
     * @details
     * Checks whether a configuration key is present without retrieving its value.
     * More efficient than get() when only checking existence.
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * @param key Configuration key to remove
     * @details
     * Removes a configuration entry from the configuration. If the key
     * doesn't exist, the operation has no effect.
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * @return std::vector<std::string> List of all configuration keys
     * @details
     * Returns all keys currently in the configuration, useful for iteration
     * or introspection of available configuration parameters.
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     * @details
     * Removes all configuration entries, leaving the configuration empty.
     * This resets the configuration to its initial state.
     */
    void clear();
    
    /**
     * @brief Generate configuration summary
     * @return std::string Human-readable configuration summary
     * @details
     * Generates a formatted summary of all configuration entries,
     * including keys, values, and their sources. Useful for debugging
     * and configuration inspection.
     */
    std::string summary() const;
    
private:
    /**
     * @brief Implementation details for Config class
     * @details
     * Private implementation class encapsulating all complex internal state
     * of the configuration system, following the pImpl pattern for clean API design.
     * 
     * The implementation manages:
     * - Configuration entries storage
     * - Source tracking
     * - Value parsing and conversion
     * - File I/O operations
     * 
     * This separation allows the Config class to have a clean, stable public API
     * while maintaining implementation complexity internally.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @name Internal helper methods
     * Private helper methods for configuration processing
     */
    ///@{
    
    /**
     * @brief Trim whitespace from string
     * @param str Input string
     * @return std::string String with leading/trailing whitespace removed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * @param str Input string
     * @return std::string Lowercase version of input
     */
    static std::string toLower(const std::string& str);
    ///@}
};

// Explicit template instantiations for common types
/**
 * @brief Explicit template instantiations
 * @details
 * Explicitly instantiates common template methods to reduce compilation time
 * and ensure they are available when needed. These are the most frequently used
 * configuration access patterns.
 */
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
