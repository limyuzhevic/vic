#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    try {
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        clear();
        
        for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
            const std::string& key = it.key();
            const nlohmann::json& value = it.value();
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_unsigned()) {
                configValue = value.get<uint64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Handle arrays - convert to vector based on element type
                if (!value.empty() && value[0].is_number_integer()) {
                    std::vector<int64_t> intArray;
                    for (const auto& item : value) {
                        intArray.push_back(item.get<int64_t>());
                    }
                    configValue = intArray;
                } else if (!value.empty() && value[0].is_number_float()) {
                    std::vector<double> doubleArray;
                    for (const auto& item : value) {
                        doubleArray.push_back(item.get<double>());
                    }
                    configValue = doubleArray;
                } else if (!value.empty() && value[0].is_string()) {
                    std::vector<std::string> stringArray;
                    for (const auto& item : value) {
                        stringArray.push_back(item.get<std::string>());
                    }
                    configValue = stringArray;
                } else {
                    // Default to vector<double>
                    std::vector<double> doubleArray;
                    for (const auto& item : value) {
                        doubleArray.push_back(item.get<double>());
                    }
                    configValue = doubleArray;
                }
            } else {
                // Fallback to string representation
                configValue = value.dump();
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception&) {
        // Fall back to simple key=value format
        file.clear();
        file.seekg(0, std::ios::beg);
    }
    
    // Fallback to simple key=value format
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
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

// Explicit template instantiations
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

// Add a validation function
void Config::addValidator(const std::function<bool(const std::string&, const ConfigValue&)>& validator) {
    pImpl->validators.push_back(validator);
}

// Set description for a configuration key
void Config::setDescription(const std::string& key, const std::string& description) {
    pImpl->descriptions[key] = description;
}

// Get description for a configuration key
std::string Config::getDescription(const std::string& key) const {
    auto it = pImpl->descriptions.find(key);
    if (it != pImpl->descriptions.end()) {
        return it->second;
    }
    return "";
}

// Validate configuration
bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        // Validate against all validators
        for (const auto& validator : pImpl->validators) {
            if (!validator(entry.key, entry.value)) {
                return false;
            }
        }
    }
    return true;
}

// Generate configuration documentation
std::string Config::generateDocumentation() const {
    std::ostringstream oss;
    oss << "NLM Configuration Documentation" << std::endl;
    oss << "=============================" << std::endl;
    oss << "Generated: " << getCurrentDateTime() << std::endl;
    oss << std::endl;
    
    oss << "Available Configuration Keys:" << std::endl;
    oss << "--------------------------" << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << entry.key << std::endl;
        
        // Get description
        std::string description = getDescription(entry.key);
        if (!description.empty()) {
            oss << "  Description: " << description << std::endl;
        }
        
        // Show value
        oss << "  Current Value: ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << std::endl;
        
        // Show source
        oss << "  Source: " << static_cast<int>(entry.source) << std::endl;
        oss << std::endl;
    }
    
    oss << "Configuration Examples:" << std::endl;
    oss << "----------------------" << std::endl;
    oss << "// Configuration file example:" << std::endl;
    oss << "// nlm_config.cfg" << std::endl;
    oss << "random_seed = 42" << std::endl;
    oss << "simulation_timestep = 0.001" << std::endl;
    oss << "neuron_count = 1000" << std::endl;
    oss << "region_count = 1" << std::endl;
    oss << "connection_probability = 0.15" << std::endl;
    oss << "stdp_ltp_weight = 0.02" << std::endl;
    oss << "stdp_ltd_weight = 0.015" << std::endl;
    oss << "synaptogenesis_rate = 0.0001" << std::endl;
    oss << "pruning_rate = 0.00001" << std::endl;
    oss << std::endl;
    
    oss << "Command line overrides:" << std::endl;
    oss << "--------------------" << std::endl;
    oss << "nlm_main --config myconfig.cfg --neuron-count 500 --region-count 2" << std::endl;
    oss << "nlm_main --stdp_ltp_weight=0.03 --stdp_ltd_weight=0.02" << std::endl;
    oss << "nlm_main --steps 10000 --save checkpoint.bin" << std::endl;
    
    return oss.str();
}

// Get current date/time string
std::string Config::getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::tm tm_time;
    std::tm tm_time_local;
    
    // Thread-safe localtime conversion
    std::istringstream ss(std::to_string(time_t));
    if (ss >> std::get_time(&tm_time_local, "%Y-%m-%d %H:%M:%S")) {
        tm_time = tm_time_local;
    } else {
        // Fallback
        if (std::localtime_r(&time_t, &tm_time) == nullptr) {
            std::memset(&tm_time, 0, sizeof(tm_time));
        }
    }
    
    std::ostringstream oss;
    oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// Generate JSON schema for configuration validation
std::string Config::generateJSONSchema() const {
    nlohmann::json schema;
    schema["$schema"] = "http://json-schema.org/draft-07/schema#";
    schema["title"] = "NLM Configuration";
    schema["description"] = "Neural Learning Machine Configuration Schema";
    schema["type"] = "object";
    
    nlohmann::json properties = nlohmann::json::object();
    
    for (const auto& entry : pImpl->entries) {
        nlohmann::json prop;
        prop["type"] = getTypeName(entry.value);
        prop["description"] = getDescription(entry.key);
        prop["default"] = getDefaultValue(entry.value);
        properties[entry.key] = prop;
    }
    
    schema["properties"] = properties;
    schema["required"] = getRequiredKeys();
    
    return schema.dump(2);
}

// Get type name as string
std::string Config::getTypeName(const ConfigValue& value) const {
    std::string typeName;
    std::visit([&typeName](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            typeName = "integer";
        } else if constexpr (std::is_same_v<T, int64_t>) {
            typeName = "integer";
        } else if constexpr (std::is_same_v<T, double>) {
            typeName = "number";
        } else if constexpr (std::is_same_v<T, bool>) {
            typeName = "boolean";
        } else if constexpr (std::is_same_v<T, std::string>) {
            typeName = "string";
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            typeName = "array";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            typeName = "array";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            typeName = "array";
        } else {
            typeName = "string";
        }
    }, value);
    return typeName;
}

// Get default value as string
std::string Config::getDefaultValue(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

// Get required keys
std::vector<std::string> Config::getRequiredKeys() const {
    // For now, return empty vector (no required keys by default)
    // This can be extended to support configuration schema validation
    return {};
}

} // namespace nlm
