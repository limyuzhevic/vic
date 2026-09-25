#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <variant>
#include <optional>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <format>
#include <any>
#include <spdlog/spdlog.h>

// Forward declarations for external parsers
namespace nlohmann {
    template<typename> class basic_json;
    using json = basic_json<...>;
}

namespace YAML {
    class Node;
}

namespace nlm {

// Configuration value types with validation support
class ConfigValue;

// Configuration index statistics for performance monitoring
struct ConfigIndexStats {
    size_t totalEntries = 0;
    size_t keyIndexSize = 0;
    size_t sourceIndexSize = 0;
    size_t typeIndexSize = 0;
    size_t lruSize = 0;
    double rebuildFrequency = 0.0;
};

// Index entry for LRU tracking
struct LRUEntry {
    size_t entryIndex;
    std::chrono::system_clock::time_point lastAccessTime;
};

// Main configuration class with pimpl and thread safety
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Factory method
    static std::unique_ptr<Config> create(ConfigSource source = ConfigSource::Default);
    
    // Load from file (JSON/YAML/simple format)
    bool loadFromFile(const std::string& filepath, bool merge = false);
    
    // Load from string (JSON/YAML/simple format)
    bool loadFromString(const std::string& content, bool merge = false);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv, bool merge = false);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::map<std::string, std::string>& envVars = {});
    
    // Save to file (JSON/YAML format)
    bool saveToFile(const std::string& filepath, bool format = false) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Type-safe get methods
    const ConfigValue& getRaw(const std::string& key) const;
    bool has(const std::string& key) const;
    
    // Set values
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source, 
             const std::string& description, const std::function<bool(const T&)>& validator = nullptr);
    
    // Configuration validation and management
    bool validate() const;
    bool applyProfile(const ConfigProfile& profile);
    bool inheritFrom(const Config& parent, const std::vector<std::string>& keys = {});
    
    // Configuration operations
    void remove(const std::string& key);
    void clear();
    std::vector<std::string> getKeys() const;
    
    // Index-based methods for improved performance
    // Key-based index O(1) lookups
    std::optional<ConfigValue> getValue(const std::string& key) const;
    std::optional<ConfigSource> getSource(const std::string& key) const;
    std::optional<std::string> getTypeName(const std::string& key) const;
    
    // Source-based index for filtering (returns all keys with specified source)
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;
    
    // Type-based index for filtering (returns all keys with specified type)
    std::vector<std::string> getKeysByType(const std::string& typeName) const;
    std::vector<ConfigEntry> getEntriesByType(const std::string& typeName) const;
    
    // Combined queries
    std::vector<ConfigEntry> query(const std::function<bool(const ConfigEntry&)>& predicate) const;
    std::vector<ConfigEntry> queryBySourceAndType(ConfigSource source, const std::string& typeName) const;
    
    // Index statistics and management
    ConfigIndexStats getIndexStats() const;
    void rebuildIndexes();
    void cleanupLRU();
    
    // Configuration hierarchy and snapshots
    void addParent(Config& parent);
    void removeParent(const Config& parent);
    bool saveSnapshot(const std::string& filename) const;
    bool restoreSnapshot(const std::string& filename);
    
    // Change notification system
    void addListener(std::shared_ptr<ConfigChangeListener> listener);
    void removeListener(std::shared_ptr<ConfigChangeListener> listener);
    
    // Configuration summary and debugging
    std::string summary() const;
    std::string toJson() const;
    std::string toYaml() const;
    std::string getSourceSummary() const;
    
    std::string toSimple() const;
    
    // Type conversion utilities
    static std::string serializeValue(const ConfigValue& value);
    static ConfigValue deserializeValue(const std::string& str, const std::string& typeName);
    static bool isInteger(const std::string& str);
    static bool isDouble(const std::string& str);
    static std::string ConfigSourceToString(ConfigSource source);
    
    // Parser implementations (private)
    bool loadFromJson(const std::string& content, bool merge);
    bool loadFromYaml(const std::string& content, bool merge);
    
    ConfigValue parseJsonValue(const nlohmann::json& json);
    ConfigValue parseYamlValue(const YAML::Node& node);
    ConfigValue inferType(const std::string& value);
};

// Configuration parser utility
class ConfigParser {
public:
    static std::unique_ptr<Config> parseFile(const std::string& filepath);
    static std::unique_ptr<Config> parseString(const std::string& content, const std::string& format = "auto");
    static std::unique_ptr<Config> parseArgs(int argc, char** argv);
    static std::unique_ptr<Config> parseEnvironment(const std::map<std::string, std::string>& envVars = {});
    
    // Configuration serialization
    static std::string serializeToJson(const Config& config);
    static std::string serializeToYaml(const Config& config);
    static std::string serializeToSimple(const Config& config);
    
    // Format detection
    static std::string detectFormat(const std::string& content);
    
private:
    std::unique_ptr<Impl> pImpl;
    
    static bool isJson(const std::string& content);
    static bool isYaml(const std::string& content);
    static bool isSimpleFormat(const std::string& content);
};

// Configuration factory for creating config instances
class ConfigFactory {
public:
    static std::unique_ptr<Config> createDefaultConfig();
    static std::unique_ptr<Config> createConfigFromDefaults(const std::map<std::string, ConfigValue>& defaults);
    static std::unique_ptr<Config> createConfigFromFile(const std::string& filepath, bool merge = false);
    static std::unique_ptr<Config> createConfigFromArgs(int argc, char** argv, bool merge = false);
    
    // Profile management
    static void registerProfile(const ConfigProfile& profile);
    static std::optional<ConfigProfile> getProfile(const std::string& name);
    static bool applyProfileToConfig(const std::string& profileName, Config& config);
    
    // Template management
    static void registerTemplate(const std::string& name, const ConfigProfile& template_);
    static std::optional<ConfigProfile> getTemplate(const std::string& name);
    static std::unique_ptr<Config> createFromTemplate(const std::string& templateName);
};

// Convenience functions
inline std::unique_ptr<Config> makeConfig() {
    return Config::create();
}

inline std::unique_ptr<Config> makeConfigFromFile(const std::string& filepath) {
    return ConfigFactory::createConfigFromFile(filepath);
}

} // namespace nlm