#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Forward declarations for JSON/YAML parsing
namespace {
    // Internal helper to add values from JSON parsing
    void addFromJSON(const std::string& key, const nlohmann::json& value, ConfigSource source, Config& config);
    
    // Internal helper to add values from YAML parsing
    void addFromYAML(const std::string& key, const YAML::Node& value, ConfigSource source, Config& config);
}

// Helper methods for parsing
bool Config::loadFromJSON(const std::string& content);
bool Config::loadFromYAML(const std::string& content);
bool Config::loadFromKeyValue(const std::string& content);

// Validation and utility methods
bool Config::validateConfig() const;
bool Config::hasValidTypes() const;
std::string Config::getConfigType(const std::string& key) const;
bool Config::isKeyPresent(const std::string& key) const;

bool Config::loadFromFile(const std::string& filepath) {
    // PHASE 2: Enhanced JSON/YAML parser with support for multiple formats
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Config file not found: " + filepath);
        return false;
    }
    
    // Try to detect file format
    std::string content;
    {
        std::stringstream buffer;
        buffer << file;
        content = buffer.str();
    }
    
    if (content.empty()) {
        NLM_LOG_WARNING("Config file is empty: " + filepath);
        return true; // Empty file is valid
    }
    
    // Determine format and parse accordingly
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    if (extension == "json") {
        return loadFromJSON(content);
    } else if (extension == "yaml" || extension == "yml") {
        return loadFromYAML(content);
    } else {
        // Default to key=value format for backward compatibility
        return loadFromKeyValue(content);
    }
}

// JSON parsing implementation
bool Config::loadFromJSON(const std::string& content) {
    try {
        auto json = nlohmann::json::parse(content);
        
        // Clear existing entries
        clear();
        
        // Parse JSON object
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            addFromJSON(key, *it, ConfigSource::File, *this);
        }
        
        NLM_LOG_INFO("Successfully loaded JSON configuration with " + std::to_string(pImpl->entries.size()) + " entries");
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in config file: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON config: " + std::string(e.what()));
        return false;
    }
}

// YAML parsing implementation
bool Config::loadFromYAML(const std::string& content) {
    try {
        auto yaml = YAML::Load(content);
        
        // Clear existing entries
        clear();
        
        // Parse YAML mapping
        for (const auto& it : yaml) {
            std::string key = it.first.as<std::string>();
            addFromYAML(key, it.second, ConfigSource::File, *this);
        }
        
        NLM_LOG_INFO("Successfully loaded YAML configuration with " + std::to_string(pImpl->entries.size()) + " entries");
        return true;
    } catch (const YAML::ParserException& e) {
        NLM_LOG_ERROR("YAML parse error in config file: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing YAML config: " + std::string(e.what()));
        return false;
    }
}

// Legacy key=value format parsing
bool Config::loadFromKeyValue(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        
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
        } else {
            NLM_LOG_WARNING("Invalid line in config file (line " + std::to_string(lineNumber) + "): " + line);
        }
    }
    
    NLM_LOG_INFO("Successfully loaded key=value configuration with " + std::to_string(pImpl->entries.size()) + " entries");
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Internal helper implementations
namespace {
    void addFromJSON(const std::string& key, const nlohmann::json& value, ConfigSource source, Config& config) {
        if (value.is_number_integer()) {
            config.set(key, value.get<int64_t>(), source);
        } else if (value.is_number_unsigned()) {
            config.set(key, value.get<uint64_t>(), source);
        } else if (value.is_number()) {
            config.set(key, value.get<double>(), source);
        } else if (value.is_boolean()) {
            config.set(key, value.get<bool>(), source);
        } else if (value.is_string()) {
            config.set(key, value.get<std::string>(), source);
        } else if (value.is_array()) {
            // Try to detect array type
            if (!value.empty()) {
                if (value[0].is_number_integer()) {
                    std::vector<int64_t> arr;
                    for (const auto& item : value) {
                        arr.push_back(item.get<int64_t>());
                    }
                    config.set(key, arr, source);
                } else if (value[0].is_number()) {
                    std::vector<double> arr;
                    for (const auto& item : value) {
                        arr.push_back(item.get<double>());
                    }
                    config.set(key, arr, source);
                } else {
                    std::vector<std::string> arr;
                    for (const auto& item : value) {
                        arr.push_back(item.get<std::string>());
                    }
                    config.set(key, arr, source);
                }
            }
        }
    }
    
    void addFromYAML(const std::string& key, const YAML::Node& value, ConfigSource source, Config& config) {
        if (value.IsScalar()) {
            if (value.IsInteger()) {
                config.set(key, value.as<int64_t>(), source);
            } else if (value.IsDouble()) {
                config.set(key, value.as<double>(), source);
            } else if (value.IsBoolean()) {
                config.set(key, value.as<bool>(), source);
            } else {
                config.set(key, value.as<std::string>(), source);
            }
        } else if (value.IsSequence()) {
            // Try to detect sequence type
            if (!value.empty()) {
                if (value[0].IsInteger()) {
                    std::vector<int64_t> arr;
                    for (const auto& item : value) {
                        arr.push_back(item.as<int64_t>());
                    }
                    config.set(key, arr, source);
                } else if (value[0].IsDouble()) {
                    std::vector<double> arr;
                    for (const auto& item : value) {
                        arr.push_back(item.as<double>());
                    }
                    config.set(key, arr, source);
                } else {
                    std::vector<std::string> arr;
                    for (const auto& item : value) {
                        arr.push_back(item.as<std::string>());
                    }
                    config.set(key, arr, source);
                }
            }
        }
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Helper method implementations
bool Config::validateConfig() const {
    bool isValid = true;
    
    // Check for required configuration keys
    std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            NLM_LOG_WARNING("Missing required configuration key: " + key);
            isValid = false;
        }
    }
    
    // Validate specific value ranges
    auto neuronCountOpt = get<int64_t>("neuron_count");
    if (neuronCountOpt.has_value() && neuronCountOpt.value() <= 0) {
        NLM_LOG_ERROR("Invalid neuron_count: must be positive");
        isValid = false;
    }
    
    auto regionCountOpt = get<int64_t>("region_count");
    if (regionCountOpt.has_value() && regionCountOpt.value() <= 0) {
        NLM_LOG_ERROR("Invalid region_count: must be positive");
        isValid = false;
    }
    
    auto timestepOpt = get<double>("simulation_timestep");
    if (timestepOpt.has_value() && (timestepOpt.value() <= 0.0 || timestepOpt.value() > 1.0)) {
        NLM_LOG_ERROR("Invalid simulation_timestep: must be between 0 and 1");
        isValid = false;
    }
    
    return isValid;
}

bool Config::hasValidTypes() const {
    bool isValid = true;
    
    // Check that configuration values have the expected types
    for (const auto& entry : pImpl->entries) {
        const auto& key = entry.key;
        const auto& value = entry.value;
        
        std::visit([&](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            
            // Type-specific validation
            if (key == "random_seed" && !std::is_same_v<T, int64_t>) {
                NLM_LOG_WARNING("random_seed should be int64_t, got " + std::string(typeid(T).name()));
            } else if (key == "simulation_timestep" && !std::is_same_v<T, double>) {
                NLM_LOG_WARNING("simulation_timestep should be double, got " + std::string(typeid(T).name()));
            } else if (key == "neuron_count" && !std::is_same_v<T, int64_t>) {
                NLM_LOG_WARNING("neuron_count should be int64_t, got " + std::string(typeid(T).name()));
            } else if (key == "region_count" && !std::is_same_v<T, int64_t>) {
                NLM_LOG_WARNING("region_count should be int64_t, got " + std::string(typeid(T).name()));
            } else if (key == "connection_probability" && !std::is_same_v<T, double>) {
                NLM_LOG_WARNING("connection_probability should be double, got " + std::string(typeid(T).name()));
            }
        }, value);
    }
    
    return isValid;
}

std::string Config::getConfigType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "NOT_FOUND";
    }
    
    const auto& value = it->value;
    
    return std::visit([](auto&& val) -> std::string {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            return "INT";
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return "UINT";
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            return "FLOAT";
        } else if constexpr (std::is_same_v<T, bool>) {
            return "BOOL";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "STRING";
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            return "INT_ARRAY";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return "FLOAT_ARRAY";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return "STRING_ARRAY";
        } else {
            return "UNKNOWN";
        }
    }, value);
}

bool Config::isKeyPresent(const std::string& key) const {
    return has(key);
}

} // namespace nlm
