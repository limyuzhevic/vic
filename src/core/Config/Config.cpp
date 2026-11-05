#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <numeric>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>

// Include JSON/YAML if available
#ifdef HAVE_JSON
#include <nlohmann/json.hpp>
#endif

namespace nlm {

// Configuration value type helpers
namespace ConfigValueHelper {
    template<typename T>
    constexpr bool is_numeric() {
        return std::is_integral_v<T> || std::is_floating_point_v<T>;
    }
    
    template<typename T>
    constexpr bool is_container() {
        return std::is_same_v<T, std::vector<int>> ||
               std::is_same_v<T, std::vector<int64_t>> ||
               std::is_same_v<T, std::vector<uint64_t>> ||
               std::is_same_v<T, std::vector<float>> ||
               std::is_same_v<T, std::vector<double>> ||
               std::is_same_v<T, std::vector<bool>> ||
               std::is_same_v<T, std::vector<std::string>>;
    }
    
    std::string valueToString(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (is_container<T>()) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << valueToString(arg[i]);
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
    
    ValidationResult validateType(const std::string& key, const ConfigValue& value) {
        // Basic validation - can be extended with schema validation
        if (std::holds_alternative<std::string>(value) && std::get<std::string>(value).empty()) {
            return ValidationResult::error("Empty string not allowed for key: " + key);
        }
        return ValidationResult::success();
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::function<ValidationResult(const std::string&, const ConfigValue&)>> validators;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Add default validators
    pImpl->validators["brain.neuron_count"] = [](const std::string&, const ConfigValue& val) {
        if (const int* v = std::get_if<int>(&val)) {
            return (*v > 0 && *v <= 1000000) 
                ? ValidationResult::success() 
                : ValidationResult::error("Neuron count must be between 1 and 1,000,000");
        }
        if (const int64_t* v = std::get_if<int64_t>(&val)) {
            return (*v > 0 && *v <= 1000000) 
                ? ValidationResult::success() 
                : ValidationResult::error("Neuron count must be between 1 and 1,000,000");
        }
        return ValidationResult::error("Neuron count must be an integer");
    };
    
    pImpl->validators["brain.synapse_density"] = [](const std::string&, const ConfigValue& val) {
        if (const double* v = std::get_if<double>(&val)) {
            return (*v >= 0.0 && *v <= 1.0) 
                ? ValidationResult::success() 
                : ValidationResult::error("Synapse density must be between 0.0 and 1.0");
        }
        return ValidationResult::error("Synapse density must be a number");
    };
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists first
    if (!std::filesystem::exists(filepath)) {
        throw ConfigError("Configuration file not found: " + filepath, ConfigErrorType::FileNotFound);
    }
    
    // Try to detect file format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError("Cannot open configuration file: " + filepath, ConfigErrorType::IoError);
    }
    
    // Read first line to detect format
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    // Check if it's JSON
    if (firstLine.find('{') != std::string::npos || firstLine.find('[') != std::string::npos) {
        // Try to parse as JSON
        try {
            return loadFromJsonFile(filepath);
        } catch (const std::exception& e) {
            throw ConfigError("Failed to parse JSON configuration: " + std::string(e.what()), ConfigErrorType::ParseError);
        }
    }
    
    // Otherwise try as YAML (basic detection)
    if (firstLine.find(":") != std::string::npos && firstLine.find("-") != std::string::npos) {
        // Try to parse as YAML
        try {
            return loadFromYamlFile(filepath);
        } catch (const std::exception& e) {
            throw ConfigError("Failed to parse YAML configuration: " + std::string(e.what()), ConfigErrorType::ParseError);
        }
    }
    
    // Default to simple key=value format for backward compatibility
    return loadFromSimpleFile(filepath);
}

bool Config::loadFromJsonFile(const std::string& filepath) {
#ifdef HAVE_JSON
    std::ifstream file(filepath);
    nlohmann::json jsonData;
    
    try {
        file >> jsonData;
    } catch (const nlohmann::json::parse_error& e) {
        throw ConfigError("JSON parse error: " + std::string(e.what()), ConfigErrorType::ParseError);
    }
    
    clear();
    
    // Recursively convert JSON to Config entries
    convertJsonToConfig(jsonData, "", ConfigSource::File);
    
    return true;
#else
    throw ConfigError("JSON support not available. Please install nlohmann/json.", ConfigErrorType::UnsupportedFormat);
#endif
}

bool Config::loadFromYamlFile(const std::string& filepath) {
#ifdef HAVE_YAML
    // Placeholder for YAML support
    // In a real implementation, this would use a YAML parser
    // For now, fall back to simple format
    return loadFromSimpleFile(filepath);
#else
    // Fall back to simple format
    return loadFromSimpleFile(filepath);
#endif
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
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
            
            // Validate key format (allow alphanumeric and dots)
            if (key.empty() || !std::all_of(key.begin(), key.end(),
                [](char c) { return std::isalnum(c) || c == '.' || c == '_'; })) {
                throw ConfigError("Invalid configuration key format at line " + std::to_string(lineNum) + ": " + key);
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Convert string to appropriate type
            ConfigValue configValue;
            bool converted = false;
            
            // Try to convert to boolean
            if (value == "true" || value == "false") {
                configValue = (value == "true");
                converted = true;
            }
            // Try to convert to integer
            else {
                try {
                    size_t posEnd;
                    int intVal = std::stoi(value);
                    configValue = intVal;
                    converted = true;
                } catch (...) {
                    try {
                        size_t posEnd;
                        int64_t int64Val = std::stoll(value);
                        configValue = int64Val;
                        converted = true;
                    } catch (...) {
                        try {
                            size_t posEnd;
                            double doubleVal = std::stod(value);
                            configValue = doubleVal;
                            converted = true;
                        } catch (...) {
                            // Keep as string
                            configValue = value;
                            converted = true;
                        }
                    }
                }
            }
            
            if (converted) {
                set(key, configValue, ConfigSource::File);
            }
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
        throw ConfigError("Cannot create configuration file: " + filepath, ConfigErrorType::IoError);
    }
    
    // Save in JSON format for better compatibility
    try {
        nlohmann::json jsonData;
        for (const auto& entry : pImpl->entries) {
            std::string path = entry.key;
            
            // Handle nested keys (e.g., "brain.neuron_count" -> brain: { neuron_count: value })
            nlohmann::json* current = &jsonData;
            size_t dotPos = path.find('.');
            while (dotPos != std::string::npos) {
                std::string segment = path.substr(0, dotPos);
                path = path.substr(dotPos + 1);
                if (!current->contains(segment)) {
                    (*current)[segment] = nlohmann::json::object();
                }
                current = &((*current)[segment]);
                dotPos = path.find('.');
            }
            
            // Set the value
            (*current)[path] = ConfigValueToJson(entry.value);
        }
        
        file << jsonData.dump(2); // 2-space indentation
        return true;
    } catch (const std::exception& e) {
        throw ConfigError("Failed to save configuration to JSON: " + std::string(e.what()), ConfigErrorType::IoError);
    }
}

nlohmann::json Config::ConfigValueToJson(const ConfigValue& value) {
    nlohmann::json result;
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, float>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<uint64_t>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<float>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            std::vector<bool> vec = arg;
            result = nlohmann::json(vec);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = arg;
        }
    }, value);
    return result;
}

void Config::convertJsonToConfig(const nlohmann::json& json, const std::string& prefix, ConfigSource source) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = prefix.empty() ? it.key() : (prefix + "." + it.key());
        
        if (it->is_object()) {
            convertJsonToConfig(*it, key, source);
        } else if (it->is_array()) {
            // Handle arrays - store as JSON string for now
            set(key, it->dump(), source);
        } else {
            // Convert JSON value to ConfigValue
            ConfigValue configValue = jsonToConfigValue(*it);
            set(key, configValue, source);
        }
    }
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& json) {
    if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_number_integer()) {
        // Check if it fits in int
        try {
            long long val = json.get<long long>();
            if (val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max()) {
                return static_cast<int>(val);
            } else {
                return val;
            }
        } catch (...) {
            return json.get<double>();
        }
    } else if (json.is_number_float()) {
        return json.get<double>();
    } else if (json.is_string()) {
        return json.get<std::string>();
    } else {
        // Fallback to string representation
        return json.dump();
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the value before setting
    if (!validateKeyValue(key, value)) {
        throw ConfigError("Validation failed for key: " + key);
    }
    
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

bool Config::validateKeyValue(const std::string& key, const ConfigValue& value) {
    // Check custom validators
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end()) {
        ValidationResult result = validatorIt->second(key, value);
        if (!result.valid) {
            throw ConfigError(result.errorMessage, ConfigErrorType::ValidationError);
        }
    }
    
    // Type-specific validation
    ValidationResult basicResult = ConfigValueHelper::validateType(key, value);
    if (!basicResult.valid) {
        throw ConfigError(basicResult.errorMessage, ConfigErrorType::ValidationError);
    }
    
    return true;
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
                                 std::is_same_v<T, std::vector<int64_t>> ||
                                 std::is_same_v<T, std::vector<uint64_t>> ||
                                 std::is_same_v<T, std::vector<float>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<bool>> ||
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

} // namespace nlm