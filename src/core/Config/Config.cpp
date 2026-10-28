#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

// Advanced features implementation

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    return ConfigSource::Default;
}

bool Config::isFileModified(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->source == ConfigSource::Runtime || it->source == ConfigSource::CommandLine;
    }
    return false;
}

std::vector<std::string> Config::getKeysFromSource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

bool Config::validate(const std::string& schemaFile) const {
    try {
        // Validate value types
        validateValueTypes();
        
        // Validate against schema if provided
        if (!schemaFile.empty()) {
            return validateSchema(schemaFile);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Configuration validation failed: " + std::string(e.what()));
        return false;
    }
}

void Config::validateValueTypes() const {
    for (const auto& entry : pImpl->entries) {
        validateEntryType(entry);
    }
}

void Config::validateEntryType(const ConfigEntry& entry) const {
    std::visit([this, &entry](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            if (arg < std::numeric_limits<int>::min() || arg > std::numeric_limits<int>::max()) {
                throw ValidationException("Value " + std::to_string(arg) + " for key '" + entry.key + "' is out of int range");
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (arg < std::numeric_limits<int64_t>::min() || arg > std::numeric_limits<int64_t>::max()) {
                throw ValidationException("Value " + std::to_string(arg) + " for key '" + entry.key + "' is out of int64_t range");
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::isnan(arg) || std::isinf(arg)) {
                throw ValidationException("Value " + std::to_string(arg) + " for key '" + entry.key + "' is NaN or infinite");
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            // Boolean values are always valid
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (arg.empty()) {
                NLM_LOG_WARNING("Empty string value for key '" + entry.key + "'");
            }
        }
    }, entry.value);
}

std::string Config::toJSON() const {
    #ifdef HAVE_NLOHMANN_JSON
    nlohmann::json json;
    toJSONImpl(json);
    return json.dump(2); // Pretty print with 2-space indentation
    #else
    NLM_LOG_ERROR("JSON export requested but nlohmann::json library not available");
    return "";
    #endif
}

std::string Config::toYAML() const {
    #ifdef HAVE_YAML_CPP
    YAML::Emitter emitter;
    toYAMLImpl(emitter);
    return emitter.c_str();
    #else
    NLM_LOG_ERROR("YAML export requested but yaml-cpp library not available");
    return "";
    #endif
}

void Config::toJSONImpl(nlohmann::json& json) const {
    for (const auto& entry : pImpl->entries) {
        json[entry.key] = entry.value;
    }
}

void Config::toYAMLImpl(YAML::Emitter& emitter) const {
    emitter << YAML::BeginMap;
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        emitter << YAML::Value << entry.value;
    }
    emitter << YAML::EndMap;
}

void Config::loadFromEnv(const std::string& prefix) {
    // This would typically be implemented by checking environment variables
    // For now, it's a placeholder for advanced functionality
    NLM_LOG_INFO("Loading configuration from environment variables with prefix: " + prefix);
    
    // Example implementation would iterate through environment variables
    // and load those starting with the given prefix
    // This is typically done in the main application, not in the Config class
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            // Add new entry
            pImpl->entries.push_back(entry);
            pImpl->entries.back().source = source;
        } else {
            // Update existing entry
            it->value = entry.value;
            it->source = source; // Override source with new source
        }
    }
}

void Config::exportKeyValue(std::ostream& output) const {
    for (const auto& entry : pImpl->entries) {
        output << entry.key << " = ";
        
        std::visit([&output](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                output << "\"" << arg << "\"";
            } else {
                output << arg;
            }
        }, entry.value);
        
        output << "\n";
    }
}

bool Config::importKeyValue(std::istream& input, ConfigSource source) {
    std::string line;
    bool success = true;
    
    while (std::getline(input, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, source);
        } else {
            NLM_LOG_WARNING("Invalid line in key=value import: " + line);
            success = false;
        }
    }
    
    return success;
}

Config Config::getDefaultConfig() {
    Config config;
    
    // Add default values for common parameters
    config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config.set("connection_probability", 0.15f, ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config.set("stdp_tau", 20.0f, ConfigSource::Default);
    config.set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config.set("pruning_rate", 0.00001f, ConfigSource::Default);
    config.set("environment_name", "GridWorld", ConfigSource::Default);
    config.set("log_level", "INFO", ConfigSource::Default);
    config.set("visualization_enabled", false, ConfigSource::Default);
    
    return config;
}

Config Config::getProductionConfig() {
    Config config = getDefaultConfig();
    
    // Optimize for production performance
    config.set("neuron_count", static_cast<int64_t>(10000), ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
    config.set("connection_probability", 0.2f, ConfigSource::Default);
    config.set("log_level", "WARNING", ConfigSource::Default);
    config.set("max_simulation_steps", static_cast<int64_t>(100000), ConfigSource::Default);
    config.set("visualization_enabled", false, ConfigSource::Default);
    config.set("log_to_file", true, ConfigSource::Default);
    
    return config;
}

Config Config::getDebugConfig() {
    Config config = getDefaultConfig();
    
    // Enable debugging features
    config.set("random_seed", static_cast<int64_t>(123456), ConfigSource::Default);
    config.set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config.set("connection_probability", 0.1f, ConfigSource::Default);
    config.set("log_level", "DEBUG", ConfigSource::Default);
    config.set("log_to_file", true, ConfigSource::Default);
    config.set("visualization_enabled", true, ConfigSource::Default);
    config.set("max_simulation_steps", static_cast<int64_t>(5000), ConfigSource::Default);
    config.set("dopamine_baseline", 0.1f, ConfigSource::Default);
    
    return config;
}

// Helper function implementations
void Config::flattenJSON(const nlohmann::json& json, const std::string& prefix, ConfigSource source) {
    if (json.is_object()) {
        for (const auto& item : json.items()) {
            std::string key = prefix.empty() ? item.key() : prefix + "." + item.key();
            flattenJSON(item.value(), key, source);
        }
    } else if (json.is_array()) {
        for (size_t i = 0; i < json.size(); ++i) {
            std::string key = prefix + "[" + std::to_string(i) + "]";
            flattenJSON(json[i], key, source);
        }
    } else {
        // Convert JSON value to string representation for storage
        std::string valueStr;
        if (json.is_string()) {
            valueStr = json.get<std::string>();
        } else if (json.is_number_integer()) {
            valueStr = std::to_string(json.get<int64_t>());
        } else if (json.is_number_unsigned()) {
            valueStr = std::to_string(json.get<uint64_t>());
        } else if (json.is_number_float()) {
            valueStr = std::to_string(json.get<double>());
        } else if (json.is_boolean()) {
            valueStr = json.get<bool>() ? "true" : "false";
        } else if (json.is_null()) {
            valueStr = "";
        }
        
        set(prefix, valueStr, source);
    }
}

void Config::flattenYAML(const YAML::Node& node, const std::string& prefix, ConfigSource source) {
    if (node.IsMap()) {
        for (const auto& item : node) {
            std::string key = prefix.empty() ? item.first.Scalar() : prefix + "." + item.first.Scalar();
            flattenYAML(item.second, key, source);
        }
    } else if (node.IsSequence()) {
        for (size_t i = 0; i < node.size(); ++i) {
            std::string key = prefix + "[" + std::to_string(i) + "]";
            flattenYAML(node[i], key, source);
        }
    } else {
        // Convert YAML value to string representation for storage
        std::string valueStr;
        if (node.IsScalar()) {
            valueStr = node.Scalar();
        } else if (node.IsNull()) {
            valueStr = "";
        }
        
        set(prefix, valueStr, source);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        auto json = nlohmann::json::parse(file);
        
        // Flatten the JSON structure to key=value pairs
        // Handle nested objects by converting to dot notation
        flattenJSON(json, "", ConfigSource::File);
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON file '" + filepath + "': " + e.what());
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node yaml = YAML::LoadFile(filepath);
        
        // Flatten the YAML structure to key=value pairs
        flattenYAML(yaml, "", ConfigSource::File);
        
        return true;
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML file '" + filepath + "': " + e.what());
        return false;
    }
}

bool Config::validateSchema(const std::string& schema) const {
    // This would typically use a JSON/YAML schema validator
    // For now, it's a placeholder for advanced validation
    NLM_LOG_INFO("Schema validation requested: " + schema + " (placeholder)");
    return true;
}

// Explicit template instantiations
namespace {
    // Helper to avoid duplicate instantiations
    struct ConfigTemplator {
        ConfigTemplator() {
            // These instantiations are needed for the template methods to work
            Config config;
            (void)config.get<int>("test");
            (void)config.get<int64_t>("test");
            (void)config.get<double>("test");
            (void)config.get<bool>("test");
            (void)config.get<std::string>("test");
            (void)config.getOr<int>("test", 0);
            (void)config.getOr<int64_t>("test", 0);
            (void)config.getOr<double>("test", 0.0);
            (void)config.getOr<bool>("test", false);
            (void)config.getOr<std::string>("test", "");
        }
    };
    
    // Force instantiation at program startup
    static ConfigTemplator configTemplator;
}

} // namespace nlm