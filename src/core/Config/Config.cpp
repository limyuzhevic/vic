#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Enhanced JSON parsing with better error handling and type support
bool tryParseJSON(const std::string& content, const std::string& filepath) {
    try {
        auto json = nlohmann::json::parse(content);
        
        if (!json.is_object()) {
            NLM_LOG_WARNING("JSON config file is not a valid object: " + filepath);
            return false;
        }
        
        for (const auto& [key, value] : json.items()) {
            try {
                setConfigFromJSONValue(key, value);
            } catch (const std::exception& e) {
                NLM_LOG_WARNING(std::string("Error parsing JSON key \") + key + "\": " + e.what());
            }
        }
        
        NLM_LOG_INFO("Successfully loaded JSON config from: " + filepath);
        return true;
        
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_WARNING(std::string("JSON parse error in " + filepath + ": ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_WARNING(std::string("Error parsing JSON from " + filepath + ": ") + e.what());
        return false;
    }
}

// Enhanced YAML parsing with better error handling
bool tryParseYAML(const std::string& content, const std::string& filepath) {
    try {
        auto yaml = YAML::Load(content);
        
        if (!yaml.IsMap()) {
            NLM_LOG_WARNING("YAML config file is not a valid map: " + filepath);
            return false;
        }
        
        for (const auto& node : yaml) {
            try {
                setConfigFromYAMLValue(node.first.as<std::string>(), node.second);
            } catch (const std::exception& e) {
                NLM_LOG_WARNING(std::string("Error parsing YAML key \") + node.first.as<std::string>() + "\": " + e.what());
            }
        }
        
        NLM_LOG_INFO("Successfully loaded YAML config from: " + filepath);
        return true;
        
    } catch (const YAML::ParserException& e) {
        NLM_LOG_WARNING(std::string("YAML parse error in " + filepath + ": ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_WARNING(std::string("Error parsing YAML from " + filepath + ": ") + e.what());
        return false;
    }
}

// Helper method to extract value from JSON node based on type
void setConfigFromJSONValue(const std::string& key, const nlohmann::json& value) {
    if (value.is_string()) {
        set(key, value.get<std::string>(), ConfigSource::File);
    } else if (value.is_number_integer()) {
        set(key, value.get<int64_t>(), ConfigSource::File);
    } else if (value.is_number_unsigned()) {
        set(key, value.get<uint64_t>(), ConfigSource::File);
    } else if (value.is_number_float()) {
        set(key, value.get<double>(), ConfigSource::File);
    } else if (value.is_boolean()) {
        set(key, value.get<bool>(), ConfigSource::File);
    } else if (value.is_array()) {
        // Handle arrays of different types
        if (!value.empty()) {
            // Try to determine array type from first element
            const auto& first = value[0];
            if (first.is_number_integer()) {
                std::vector<int64_t> intArray;
                for (const auto& item : value) {
                    intArray.push_back(item.get<int64_t>());
                }
                set(key, intArray, ConfigSource::File);
            } else if (first.is_number_float()) {
                std::vector<double> doubleArray;
                for (const auto& item : value) {
                    doubleArray.push_back(item.get<double>());
                }
                set(key, doubleArray, ConfigSource::File);
            } else if (first.is_string()) {
                std::vector<std::string> stringArray;
                for (const auto& item : value) {
                    stringArray.push_back(item.get<std::string>());
                }
                set(key, stringArray, ConfigSource::File);
            }
        }
    }
}

// Helper method to extract value from YAML node based on type
void setConfigFromYAMLValue(const std::string& key, const YAML::Node& value) {
    if (value.IsScalar()) {
        // Determine scalar type
        if (value.IsString()) {
            set(key, value.as<std::string>(), ConfigSource::File);
        } else if (value.IsInteger()) {
            set(key, value.as<int64_t>(), ConfigSource::File);
        } else if (value.IsFloat()) {
            set(key, value.as<double>(), ConfigSource::File);
        } else if (value.IsBoolean()) {
            set(key, value.as<bool>(), ConfigSource::File);
        }
    } else if (value.IsSequence()) {
        // Handle YAML sequences (arrays)
        if (!value.IsNull()) {
            // Try to determine array type from first element
            if (value.size() > 0) {
                const auto& first = value[0];
                if (first.IsScalar()) {
                    if (first.IsString()) {
                        std::vector<std::string> stringArray;
                        for (size_t i = 0; i < value.size(); ++i) {
                            stringArray.push_back(value[i].as<std::string>());
                        }
                        set(key, stringArray, ConfigSource::File);
                    } else if (first.IsInteger()) {
                        std::vector<int64_t> intArray;
                        for (size_t i = 0; i < value.size(); ++i) {
                            intArray.push_back(value[i].as<int64_t>());
                        }
                        set(key, intArray, ConfigSource::File);
                    } else if (first.IsFloat()) {
                        std::vector<double> doubleArray;
                        for (size_t i = 0; i < value.size(); ++i) {
                            doubleArray.push_back(value[i].as<double>());
                        }
                        set(key, doubleArray, ConfigSource::File);
                    }
                }
            }
        }
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // PHASE 2: Enhanced file loading with JSON/YAML support
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Cannot open config file: " + filepath);
        return false;
    }
    
    // Try to detect file format and parse accordingly
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    if (content.empty()) {
        NLM_LOG_WARNING("Config file is empty: " + filepath);
        return false;
    }
    
    // Try JSON first
    if (tryParseJSON(content, filepath)) {
        return true;
    }
    
    // Try YAML second
    if (tryParseYAML(content, filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format
    NLM_LOG_INFO("Using legacy key=value format for config file: " + filepath);
    std::istringstream iss(content);
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(iss, line)) {
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
            NLM_LOG_WARNING("Skipping malformed line " + std::to_string(lineNumber) + ": " + line);
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
        // Handle --file=file.json format (config file)
        else if (arg.find("--file=") == 0) {
            std::string filepath = arg.substr(7);
            NLM_LOG_INFO("Loading additional config from file: " + filepath);
            tryParseJSONFile(filepath);
        }
        // Handle --env=VAR_NAME format (environment variable)
        else if (arg.find("--env=") == 0) {
            std::string varName = arg.substr(6);
            setFromEnvironmentVariable(varName);
        }
        // Handle --config-key=value and --config-description="description" format
        else if (arg.find("--config-key=") == 0) {
            std::string key = arg.substr(13);
            // Look for description in next argument
            if (i + 1 < argc && std::string(argv[i + 1]).find("--config-description=") == 0) {
                std::string desc = argv[++i];
                desc = desc.substr(22); // Remove --config-description= prefix
                // Add config entry with description
                set(key, ConfigValue(""), ConfigSource::CommandLine);
                // Find and update the entry with description (this is a simplified approach)
                for (auto& entry : pImpl->entries) {
                    if (entry.key == key) {
                        entry.description = desc;
                        break;
                    }
                }
            }
        }
    }
    return true;
}

// Enhanced configuration loading with advanced features

// Helper method to load JSON file with error handling
bool tryParseJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING(std::string("Cannot open config file: ") + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return tryParseJSON(content, filepath);
}

// Helper method to set value from environment variable
void setFromEnvironmentVariable(const std::string& varName) {
    const char* envValue = std::getenv(varName.c_str());
    if (envValue != nullptr) {
        std::string value = envValue;
        set(varName, value, ConfigSource::CommandLine);
        NLM_LOG_INFO(std::string("Loaded environment variable ") + varName + ": " + value);
    } else {
        NLM_LOG_WARNING(std::string("Environment variable not found: ") + varName);
    }
}

// Helper method to validate configuration
bool validate() const {
    // Validate critical configuration parameters
    std::vector<std::string> criticalKeys = {
        "neuron_count",
        "simulation_timestep",
        "random_seed"
    };
    
    std::vector<std::string> missingKeys;
    for (const auto& key : criticalKeys) {
        if (!has(key)) {
            missingKeys.push_back(key);
        }
    }
    
    if (!missingKeys.empty()) {
        NLM_LOG_WARNING(std::string("Missing critical configuration keys: ") + 
                       joinStrings(missingKeys, ", "));
        return false;
    }
    
    return true;
}

// Helper method to join strings
std::string joinStrings(const std::vector<std::string>& strings, const std::string& separator) const {
    std::ostringstream oss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) {
            oss << separator;
        }
        oss << strings[i];
    }
    return oss.str();
}

// Helper method to get configuration value with type conversion and validation
template<typename T>
std::optional<T> getValidated(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value.has_value()) {
        // Additional validation can be added here based on the type
        if constexpr (std::is_integral_v<T>) {
            if (value.value() <= 0) {
                NLM_LOG_WARNING(std::string("Invalid value for key ") + key + ": " + 
                              std::to_string(value.value()) + " (must be positive)");
                return defaultValue;
            }
        } else if constexpr (std::is_floating_point_v<T>) {
            if (value.value() < 0.0) {
                NLM_LOG_WARNING(std::string("Invalid value for key ") + key + ": " + 
                              std::to_string(value.value()) + " (must be non-negative)");
                return defaultValue;
            }
        }
    }
    return value;
}

// Helper method to print configuration summary with validation
void printSummary() const {
    NLM_LOG_INFO("=== Configuration Summary ===");
    
    // Print all configuration entries
    for (const auto& entry : pImpl->entries) {
        std::string valueStr;
        std::visit([&valueStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = "\"" + arg + "\"";
            } else {
                valueStr = std::to_string(arg);
            }
        }, entry.value);
        
        NLM_LOG_INFO(std::string("  ") + entry.key + " = " + valueStr + 
                    std::string(" (source: ") + std::to_string(static_cast<int>(entry.source)) + ")");
    }
    
    // Print validation status
    NLM_LOG_INFO(std::string("Configuration validation: ") + 
                (validate() ? "PASSED" : "FAILED"));
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
std::optional<T> Config::getValidated(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value.has_value()) {
        // Additional validation can be added here based on the type
        if constexpr (std::is_integral_v<T>) {
            if (value.value() <= 0) {
                NLM_LOG_WARNING(std::string("Invalid value for key ") + key + ": " + 
                              std::to_string(value.value()) + " (must be positive)");
                return defaultValue;
            }
        } else if constexpr (std::is_floating_point_v<T>) {
            if (value.value() < 0.0) {
                NLM_LOG_WARNING(std::string("Invalid value for key ") + key + ": " + 
                              std::to_string(value.value()) + " (must be non-negative)");
                return defaultValue;
            }
        }
    }
    return value;
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
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

// Validation template instantiations
template std::optional<int> Config::getValidated<int>(const std::string&, const int&) const;
template std::optional<int64_t> Config::getValidated<int64_t>(const std::string&, const int64_t&) const;
template std::optional<double> Config::getValidated<double>(const std::string&, const double&) const;
template std::optional<bool> Config::getValidated<bool>(const std::string&, const bool&) const;
template std::optional<std::string> Config::getValidated<std::string>(const std::string&, const std::string&) const;
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm
