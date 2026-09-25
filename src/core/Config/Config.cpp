#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "../core/Logger/Logger.hpp"

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
        NLM_LOG_WARNING("Could not open config file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Try to parse as JSON first
    bool parsed = parseJSON(content);
    if (!parsed) {
        // Fallback to simple key=value format for backward compatibility
        parsed = parseSimpleFormat(content);
    }
    
    if (!parsed) {
        NLM_LOG_WARNING("Could not parse config file: " + filepath);
    }
    
    return parsed;
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
        NLM_LOG_WARNING("Could not save config file: " + filepath);
        return false;
    }
    
    // Write JSON format for better interoperability
    file << "{\n";
    
    size_t entryCount = pImpl->entries.size();
    for (size_t i = 0; i < entryCount; ++i) {
        const auto& entry = pImpl->entries[i];
        
        // Add description comment
        if (!entry.description.empty()) {
            file << "  // " << entry.description << "\n";
        }
        
        // Write key-value pair
        file << "  \"" << entry.key << "\": ";
        
        // Handle different value types
        if (std::holds_alternative<int>(entry.value)) {
            file << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            file << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            file << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            file << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            file << "\"" << std::get<std::string>(entry.value) << "\"";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            const auto& arr = std::get<std::vector<int>>(entry.value);
            file << "[";
            for (size_t j = 0; j < arr.size(); ++j) {
                if (j > 0) file << ", ";
                file << arr[j];
            }
            file << "]";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            const auto& arr = std::get<std::vector<double>>(entry.value);
            file << "[";
            for (size_t j = 0; j < arr.size(); ++j) {
                if (j > 0) file << ", ";
                file << arr[j];
            }
            file << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            const auto& arr = std::get<std::vector<std::string>>(entry.value);
            file << "[";
            for (size_t j = 0; j < arr.size(); ++j) {
                if (j > 0) file << ", ";
                file << "\"" << arr[j] << "\"";
            }
            file << "]";
        }
        
        file << ",\n";
    }
    
    // Remove trailing comma
    if (!pImpl->entries.empty()) {
        file.seekp(-2, std::ios_base::end); // Go back 2 characters: ",\n"
        file << "\n";
    }
    
    file << "}\n";
    
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

// JSON parser - supports basic JSON objects with string keys and simple values
bool Config::parseJSON(const std::string& content) {
    try {
        nlohmann::json j = nlohmann::json::parse(content);
        
        for (const auto& [key, value] : j.items()) {
            ConfigValue configValue;
            
            if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_unsigned()) {
                configValue = static_cast<int64_t>(value.get<uint64_t>());
            } else if (value.is_number()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_array()) {
                // Handle arrays
                std::vector<std::variant<int, int64_t, double, bool, std::string>> arrayValue;
                for (const auto& item : value) {
                    if (item.is_number_integer()) {
                        arrayValue.push_back(item.get<int64_t>());
                    } else if (item.is_number_unsigned()) {
                        arrayValue.push_back(static_cast<int64_t>(item.get<uint64_t>()));
                    } else if (item.is_number()) {
                        arrayValue.push_back(item.get<double>());
                    } else if (item.is_boolean()) {
                        arrayValue.push_back(item.get<bool>());
                    } else if (item.is_string()) {
                        arrayValue.push_back(item.get<std::string>());
                    } else {
                        return false; // Unsupported type
                    }
                }
                
                // Convert to appropriate vector type
                bool allInts = true;
                bool allDoubles = true;
                bool allStrings = true;
                
                for (const auto& item : arrayValue) {
                    if (std::holds_alternative<int>(item) || std::holds_alternative<int64_t>(item)) {
                        allDoubles = false;
                        allStrings = false;
                    } else if (std::holds_alternative<double>(item)) {
                        allInts = false;
                        allStrings = false;
                    } else if (std::holds_alternative<bool>(item)) {
                        allInts = false;
                        allDoubles = false;
                        allStrings = false;
                    } else if (std::holds_alternative<std::string>(item)) {
                        allInts = false;
                        allDoubles = false;
                    } else {
                        allInts = false;
                        allDoubles = false;
                        allStrings = false;
                    }
                }
                
                if (allInts) {
                    std::vector<int> intArray;
                    for (const auto& item : arrayValue) {
                        if (std::holds_alternative<int>(item)) {
                            intArray.push_back(std::get<int>(item));
                        } else if (std::holds_alternative<int64_t>(item)) {
                            intArray.push_back(static_cast<int>(std::get<int64_t>(item)));
                        }
                    }
                    configValue = intArray;
                } else if (allDoubles) {
                    std::vector<double> doubleArray;
                    for (const auto& item : arrayValue) {
                        if (std::holds_alternative<double>(item)) {
                            doubleArray.push_back(std::get<double>(item));
                        } else if (std::holds_alternative<int>(item)) {
                            doubleArray.push_back(static_cast<double>(std::get<int>(item)));
                        } else if (std::holds_alternative<int64_t>(item)) {
                            doubleArray.push_back(static_cast<double>(std::get<int64_t>(item)));
                        }
                    }
                    configValue = doubleArray;
                } else if (allStrings) {
                    std::vector<std::string> stringArray;
                    for (const auto& item : arrayValue) {
                        if (std::holds_alternative<std::string>(item)) {
                            stringArray.push_back(std::get<std::string>(item));
                        }
                    }
                    configValue = stringArray;
                } else {
                    return false; // Mixed type arrays not supported
                }
            } else {
                return false; // Unsupported JSON type
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const nlohmann::json::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

// Parse simple key=value format (backward compatibility)
bool Config::parseSimpleFormat(const std::string& content) {
    size_t pos = 0;
    while (pos < content.size()) {
        size_t lineEnd = content.find('\n', pos);
        if (lineEnd == std::string::npos) {
            lineEnd = content.size();
        }
        
        std::string line = content.substr(pos, lineEnd - pos);
        pos = lineEnd + 1;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = trim(line.substr(0, eqPos));
            std::string valueStr = trim(line.substr(eqPos + 1));
            
            // Remove quotes if present
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            // Try to parse value based on content
            ConfigValue value;
            
            // Try integer
            bool parsed = false;
            try {
                size_t posEnd;
                int64_t intVal = std::stoll(valueStr, &posEnd);
                if (posEnd == valueStr.size()) {
                    value = intVal;
                    parsed = true;
                }
            } catch (...) {}
            
            // Try unsigned integer
            if (!parsed) {
                try {
                    size_t posEnd;
                    uint64_t uintVal = std::stoull(valueStr, &posEnd);
                    if (posEnd == valueStr.size()) {
                        value = static_cast<int64_t>(uintVal);
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try floating point
            if (!parsed) {
                try {
                    size_t posEnd;
                    double doubleVal = std::stod(valueStr, &posEnd);
                    if (posEnd == valueStr.size()) {
                        value = doubleVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try boolean
            if (!parsed) {
                if (valueStr == "true") {
                    value = true;
                    parsed = true;
                } else if (valueStr == "false") {
                    value = false;
                    parsed = true;
                }
            }
            
            // Use as string if not parsed
            if (!parsed) {
                value = valueStr;
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

void Config::skipWhitespace(const std::string& str, size_t& pos) {
    while (pos < str.size() && std::isspace(str[pos])) {
        pos++;
    }
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

bool Config::validate() const {
    bool valid = true;
    
    // Check required keys
    std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            NLM_LOG_WARNING("Missing required config key: " + key);
            valid = false;
        }
    }
    
    // Validate value ranges
    if (has("neuron_count")) {
        int64_t neuronCount = getOr<int64_t>("neuron_count", 500);
        if (neuronCount < 10 || neuronCount > 100000) {
            NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount));
            valid = false;
        }
    }
    
    if (has("region_count")) {
        int64_t regionCount = getOr<int64_t>("region_count", 1);
        if (regionCount < 1 || regionCount > 100) {
            NLM_LOG_ERROR("Invalid region_count: " + std::to_string(regionCount));
            valid = false;
        }
    }
    
    if (has("connection_probability")) {
        float connProb = getOr<float>("connection_probability", 0.15f);
        if (connProb < 0.0f || connProb > 1.0f) {
            NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(connProb));
            valid = false;
        }
    }
    
    return valid;
}

std::string Config::getDescription(const std::string& key) {
    const std::unordered_map<std::string, std::string> descriptions = {
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Connection probability for random connectivity"},
        {"plasticity_learning_rate", "Learning rate for plasticity rules"},
        {"stdp_ltp_weight", "STDP long-term potentiation weight"},
        {"stdp_ltd_weight", "STDP long-term depression weight"},
        {"stdp_time_constant", "STDP time constant (ms)"},
        {"synaptogenesis_rate", "Structural plasticity synaptogenesis rate"},
        {"pruning_rate", "Structural plasticity pruning rate"},
        {"dopamine_baseline", "Baseline dopamine level"},
        {"reward_discount_factor", "Reward discount factor for reinforcement learning"},
        {"environment_name", "Environment name"},
        {"environment_width", "Environment width in cells"},
        {"environment_height", "Environment height in cells"},
        {"log_level", "Logging level (DEBUG, INFO, WARNING, ERROR)"},
        {"log_to_file", "Whether to log to file"},
        {"log_filename", "Log file name"},
        {"max_simulation_steps", "Maximum simulation steps"},
        {"visualization_enabled", "Whether visualization is enabled"},
        {"visualization_update_rate", "Visualization update rate (Hz)"}
    };
    
    auto it = descriptions.find(key);
    if (it != descriptions.end()) {
        return it->second;
    }
    
    return "Configuration parameter: " + key;
}

// Explicit template instantiations
// Note: Config::get and Config::getOr are already defined above

} // namespace nlm
