#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigConstraint> constraints;
    std::unordered_map<std::string, ConfigMetadata> metadata;
    std::unordered_map<std::string, std::string> descriptions;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first
    auto ext = filepath.substr(filepath.find_last_of('.') + 1);
    if (ext == "json" || ext == "JSON") {
        return fromJSON(filepath);
    }
    
    // Fall back to simple key=value format
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
            
            if (!loadFromString(key + "=" + value)) {
                return false;
            }
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Use the improved parseArgs method
    auto parsed = parseArgs(argc, argv);
    
    for (const auto& [key, value] : parsed.knownArgs) {
        if (!set(key, value, ConfigSource::CommandLine)) {
            return false;
        }
    }
    
    return true;
}

bool Config::loadFromString(const std::string& configString) {
    // Parse key=value format
    size_t pos = configString.find('=');
    if (pos == std::string::npos) {
        return false;
    }
    
    std::string key = trim(configString.substr(0, pos));
    std::string value = trim(configString.substr(pos + 1));
    
    // Try to determine type from value
    if (value.empty()) return false;
    
    if (value[0] == '"' && value.back() == '"') {
        return setString(key, value.substr(1, value.size() - 2));
    } else if (value[0] == '\'' && value.back() == '\'') {
        return setString(key, value.substr(1, value.size() - 2));
    } else if (value == "true" || value == "false") {
        bool boolVal = (value == "true");
        return setBool(key, boolVal);
    } else {
        try {
            // Try integer
            size_t endpos;
            long long intVal = std::stoll(value, &endpos);
            if (endpos == value.size()) {
                return setInt(key, static_cast<int>(intVal));
            }
            
            // Try double
            double doubleVal = std::stod(value, &endpos);
            if (endpos == value.size()) {
                return setDouble(key, doubleVal);
            }
            
            // Fall back to string
            return setString(key, value);
        } catch (...) {
            return setString(key, value);
        }
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try JSON format first
    auto ext = filepath.substr(filepath.find_last_of('.') + 1);
    if (ext == "json" || ext == "JSON") {
        return fromJSON(toJSON());
    }
    
    // Fall back to simple key=value format
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
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
            // For vectors, just write placeholder
            file << "[INT_VECTOR]";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            file << "[DOUBLE_VECTOR]";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            file << "[STRING_VECTOR]";
        }
        
        file << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto val = getRaw(key);
    if (!val.has_value()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(val.value());
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::optional<ConfigValue> Config::getRaw(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return it->value;
}

bool Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source, const std::string& description) {
    // Validate the value
    if (!validateValue(value, key)) {
        return false;
    }
    
    // Validate constraints
    if (!validateConstraints(key, value)) {
        return false;
    }
    
    // Find existing entry or create new one
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, description);
    }
    
    return true;
}

bool Config::setString(const std::string& key, const std::string& value,
                       ConfigSource source, const std::string& description) {
    return set(key, ConfigValue(value), source, description);
}

bool Config::setInt(const std::string& key, int value,
                    ConfigSource source, const std::string& description) {
    return set(key, ConfigValue(value), source, description);
}

bool Config::setDouble(const std::string& key, double value,
                       ConfigSource source, const std::string& description) {
    return set(key, ConfigValue(value), source, description);
}

bool Config::setBool(const std::string& key, bool value,
                     ConfigSource source, const std::string& description) {
    return set(key, ConfigValue(value), source, description);
}

bool Config::setStringVector(const std::string& key, const std::vector<std::string>& values,
                             ConfigSource source, const std::string& description) {
    return set(key, ConfigValue(values), source, description);
}

void Config::addConstraint(const ConfigConstraint& constraint) {
    pImpl->constraints.push_back(constraint);
}

void Config::addMetadata(const std::string& key, const ConfigMetadata& metadata) {
    pImpl->metadata[key] = metadata;
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        // Validate the value type
        if (!validateValue(entry.value, entry.key)) {
            result.addError("Invalid value type for key '" + entry.key + "'");
        }
        
        // Validate constraints
        if (!validateConstraints(entry.key, entry.value)) {
            result.addError("Constraint violation for key '" + entry.key + "'");
        }
    }
    
    // Check required fields
    for (const auto& [key, metadata] : pImpl->metadata) {
        if (metadata.required && !has(key)) {
            result.addError("Required key '" + key + "' is missing");
        }
    }
    
    return result;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it);
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    
    return keys;
}

std::vector<std::string> Config::getKeys(ConfigSource source) const {
    std::vector<std::string> keys;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigSource::Default;
    }
    
    return it->source;
}

std::string Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "";
    }
    
    return it->description;
}

std::optional<ConfigMetadata> Config::getMetadata(const std::string& key) const {
    auto it = pImpl->metadata.find(key);
    if (it == pImpl->metadata.end()) {
        return std::nullopt;
    }
    
    return it->second;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->constraints.clear();
    pImpl->metadata.clear();
    pImpl->descriptions.clear();
}

std::string Config::summary() const {
    std::stringstream ss;
    
    ss << "Configuration Summary:" << std::endl;
    ss << "====================" << std::endl;
    ss << std::endl;
    
    // Group by source
    std::unordered_map<ConfigSource, std::vector<const ConfigEntry*>> grouped;
    for (const auto& entry : pImpl->entries) {
        grouped[entry.source].push_back(&entry);
    }
    
    for (const auto& [source, entries] : grouped) {
        ss << "Source: " << static_cast<int>(source) << std::endl;
        for (const auto* entry : entries) {
            ss << "  " << entry->key << " = ";
            
            if (std::holds_alternative<int>(entry->value)) {
                ss << std::get<int>(entry->value);
            } else if (std::holds_alternative<int64_t>(entry->value)) {
                ss << std::get<int64_t>(entry->value);
            } else if (std::holds_alternative<double>(entry->value)) {
                ss << std::get<double>(entry->value);
            } else if (std::holds_alternative<bool>(entry->value)) {
                ss << (std::get<bool>(entry->value) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(entry->value)) {
                ss << "\"" << std::get<std::string>(entry->value) << "\"";
            } else {
                ss << "[UNSUPPORTED_TYPE]";
            }
            
            if (!entry->description.empty()) {
                ss << "  # " << entry->description;
            }
            
            ss << std::endl;
        }
        ss << std::endl;
    }
    
    return ss.str();
}

std::string Config::toJSON() const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        
        if (std::holds_alternative<int>(entry.value)) {
            j[key] = std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            j[key] = std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            j[key] = std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            j[key] = std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            j[key] = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            j[key] = std::get<std::vector<int>>(entry.value);
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            j[key] = std::get<std::vector<double>>(entry.value);
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            j[key] = std::get<std::vector<std::string>>(entry.value);
        }
    }
    
    return j.dump(2);
}

bool Config::fromJSON(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        for (auto& [key, value] : j.items()) {
            if (value.is_boolean()) {
                setBool(key, value.get<bool>());
            } else if (value.is_number_integer()) {
                setInt(key, value.get<int>());
            } else if (value.is_number_float()) {
                setDouble(key, value.get<double>());
            } else if (value.is_string()) {
                setString(key, value.get<std::string>());
            } else if (value.is_array()) {
                // Determine element type
                if (!value.empty()) {
                    if (std::all_of(value.begin(), value.end(), 
                                   [](const json& v) { return v.is_number_integer(); })) {
                        std::vector<int> intArray;
                        for (const auto& v : value) {
                            intArray.push_back(v.get<int>());
                        }
                        setStringVector(key, intArray); // This won't work, need better vector type
                    } else if (std::all_of(value.begin(), value.end(), 
                                          [](const json& v) { return v.is_number_float(); })) {
                        std::vector<double> doubleArray;
                        for (const auto& v : value) {
                            doubleArray.push_back(v.get<double>());
                        }
                        setStringVector(key, doubleArray); // This won't work either
                    } else if (std::all_of(value.begin(), value.end(), 
                                          [](const json& v) { return v.is_string(); })) {
                        std::vector<std::string> stringArray;
                        for (const auto& v : value) {
                            stringArray.push_back(v.get<std::string>());
                        }
                        setStringVector(key, stringArray);
                    }
                }
            }
        }
        
        return true;
    } catch (const json::parse_error& e) {
        return false;
    }
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Only replace if the other source has higher priority
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end() || entry.source > it->source) {
            if (it != pImpl->entries.end()) {
                *it = entry;
            } else {
                pImpl->entries.push_back(entry);
            }
        }
    }
}

bool Config::validateValue(const ConfigValue& value, const std::string& key) const {
    // Check constraints
    if (!validateConstraints(key, value)) {
        return false;
    }
    
    // Check metadata if available
    auto metadata = getMetadata(key);
    if (metadata.has_value()) {
        const auto& meta = metadata.value();
        
        // Check type
        if (meta.type != "unknown") {
            bool typeOk = false;
            
            if (meta.type == "int") {
                typeOk = std::holds_alternative<int>(value);
            } else if (meta.type == "int64") {
                typeOk = std::holds_alternative<int64_t>(value);
            } else if (meta.type == "double") {
                typeOk = std::holds_alternative<double>(value);
            } else if (meta.type == "bool") {
                typeOk = std::holds_alternative<bool>(value);
            } else if (meta.type == "string") {
                typeOk = std::holds_alternative<std::string>(value);
            }
            
            if (!typeOk) {
                return false;
            }
        }
        
        // Check range for numeric types
        if (meta.type == "int" || meta.type == "int64" || meta.type == "double") {
            double val = 0.0;
            if (std::holds_alternative<int>(value)) {
                val = std::get<int>(value);
            } else if (std::holds_alternative<int64_t>(value)) {
                val = static_cast<double>(std::get<int64_t>(value));
            } else if (std::holds_alternative<double>(value)) {
                val = std::get<double>(value);
            }
            
            if (val < meta.range.first || val > meta.range.second) {
                return false;
            }
        }
        
        // Check allowed values
        if (!meta.allowedValues.empty()) {
            std::string strVal;
            if (std::holds_alternative<std::string>(value)) {
                strVal = std::get<std::string>(value);
            } else {
                // Convert to string for comparison
                std::stringstream ss;
                ss << value;
                strVal = ss.str();
            }
            
            if (std::find(meta.allowedValues.begin(), meta.allowedValues.end(), strVal) 
                == meta.allowedValues.end()) {
                return false;
            }
        }
    }
    
    return true;
}

bool Config::validateConstraints(const std::string& key, const ConfigValue& value) const {
    for (const auto& constraint : pImpl->constraints) {
        if (constraint.key == key && !constraint.validator(value)) {
            return false;
        }
    }
    
    return true;
}

std::string Config::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && std::isspace(str[start])) {
        start++;
    }
    
    while (end > start && std::isspace(str[end - 1])) {
        end--;
    }
    
    return str.substr(start, end - start);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string Config::toJsonString(const std::string& str) {
    return escapeJsonString(str);
}

std::string Config::escapeJsonString(const std::string& str) {
    std::stringstream ss;
    ss << "\"";
    
    for (char c : str) {
        switch (c) {
            case '"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '/': ss << "\\/"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') 
                       << static_cast<int>(c);
                } else {
                    ss << c;
                }
                break;
        }
    }
    
    ss << "\"";
    return ss.str();
}

bool Config::isNumeric(const ConfigValue& value) {
    return std::holds_alternative<int>(value) || 
           std::holds_alternative<int64_t>(value) || 
           std::holds_alternative<double>(value);
}

bool Config::isString(const ConfigValue& value) {
    return std::holds_alternative<std::string>(value);
}

bool Config::isBoolean(const ConfigValue& value) {
    return std::holds_alternative<bool>(value);
}

bool Config::isVector(const ConfigValue& value) {
    return std::holds_alternative<std::vector<int>>(value) ||
           std::holds_alternative<std::vector<double>>(value) ||
           std::holds_alternative<std::vector<std::string>>(value);
}

template<typename T>
T Config::convertToType(const ConfigValue& value) {
    if constexpr (std::is_same_v<T, int>) {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        } else if (std::holds_alternative<double>(value)) {
            return static_cast<int>(std::get<double>(value));
        }
    } else if constexpr (std::is_same_v<T, double>) {
        if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<double>(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        }
    }
    
    throw std::bad_variant_access();
}

Config::ParsedArgs Config::parseArgs(int argc, char** argv,
                                    const std::vector<std::string>& knownKeys) {
    ParsedArgs result;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Check if key is known
                bool isKnown = std::find(knownKeys.begin(), knownKeys.end(), key) != knownKeys.end();
                if (isKnown) {
                    result.knownArgs.push_back({key, value});
                } else {
                    result.unknownArgs.push_back(arg);
                }
            } else {
                result.unknownArgs.push_back(arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Check if key is known
            bool isKnown = std::find(knownKeys.begin(), knownKeys.end(), key) != knownKeys.end();
            if (isKnown) {
                result.knownArgs.push_back({key, value});
            } else {
                result.unknownArgs.push_back(arg);
                // Put the value back
                i--;
            }
        }
        // Handle --flag format (boolean flags)
        else if (arg.substr(0, 2) == "--" && arg.find('=') == std::string::npos) {
            std::string key = arg.substr(2);
            
            // Check if key is known
            bool isKnown = std::find(knownKeys.begin(), knownKeys.end(), key) != knownKeys.end();
            if (isKnown) {
                result.flags.push_back({key, "true"});
            } else {
                result.unknownArgs.push_back(arg);
            }
        } else {
            result.unknownArgs.push_back(arg);
        }
    }
    
    return result;
}

// ConfigHelpers implementation
namespace ConfigHelpers {
    const std::string CommonKeys::RANDOM_SEED = "random_seed";
    const std::string CommonKeys::SIMULATION_TIMESTEP = "simulation_timestep";
    const std::string CommonKeys::NEURON_COUNT = "neuron_count";
    const std::string CommonKeys::REGION_COUNT = "region_count";
    const std::string CommonKeys::CONNECTION_PROBABILITY = "connection_probability";
    const std::string CommonKeys::STDp_LTP_WEIGHT = "stdp_ltp_weight";
    const std::string CommonKeys::STDp_LTD_WEIGHT = "stdp_ltd_weight";
    const std::string CommonKeys::STDp_TAU = "stdp_tau";
    const std::string CommonKeys::SYNAPTOGENESIS_RATE = "synaptogenesis_rate";
    const std::string CommonKeys::PRUNING_RATE = "pruning_rate";
    const std::string CommonKeys::MAX_ENERGY = "max_energy";
    const std::string CommonKeys::ENERGY_DECAY_RATE = "energy_decay_rate";
    const std::string CommonKeys::WORLD_WIDTH = "world_width";
    const std::string CommonKeys::WORLD_HEIGHT = "world_height";
    const std::string CommonKeys::VISION_WIDTH = "vision_width";
    const std::string CommonKeys::VISION_HEIGHT = "vision_height";
    const std::string CommonKeys::AUDIO_ENABLED = "audio_enabled";
    const std::string CommonKeys::AUDIO_SAMPLE_RATE = "audio_sample_rate";
    const std::string CommonKeys::ENABLE_DEBUG_LOGGING = "enable_debug_logging";
    const std::string CommonKeys::ENABLE_PERFORMANCE_MEASURING = "enable_performance_measuring";
    const std::string CommonKeys::CHECKPOINT_INTERVAL = "checkpoint_interval";
    const std::string CommonKeys::MAX_EPISODES = "max_episodes";
    const std::string CommonKeys::MAX_STEPS_PER_EPISODE = "max_steps_per_episode";
    const std::string CommonKeys::REWARD_SCALE = "reward_scale";
    const std::string CommonKeys::NOISE_LEVEL = "noise_level";
    
    bool isPositive(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value) > 0;
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) > 0;
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) > 0;
        }
        return false;
    }
    
    bool isNonNegative(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value) >= 0;
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) >= 0;
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) >= 0;
        }
        return false;
    }
    
    bool isNonZero(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value) != 0;
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) != 0;
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) != 0;
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        }
        return false;
    }
    
    bool isInRange(const ConfigValue& value, double min, double max) {
        if (std::holds_alternative<int>(value)) {
            double val = std::get<int>(value);
            return val >= min && val <= max;
        } else if (std::holds_alternative<int64_t>(value)) {
            double val = static_cast<double>(std::get<int64_t>(value));
            return val >= min && val <= max;
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            return val >= min && val <= max;
        }
        return false;
    }
    
    bool isValidSeed(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value) >= 0;
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) >= 0;
        }
        return false;
    }
    
    std::unordered_map<std::string, ConfigValue> getDefaultConfig() {
        std::unordered_map<std::string, ConfigValue> defaults;
        
        defaults[CommonKeys::RANDOM_SEED] = 42;
        defaults[CommonKeys::SIMULATION_TIMESTEP] = 0.001;
        defaults[CommonKeys::NEURON_COUNT] = 500;
        defaults[CommonKeys::REGION_COUNT] = 1;
        defaults[CommonKeys::CONNECTION_PROBABILITY] = 0.15f;
        defaults[CommonKeys::STDp_LTP_WEIGHT] = 0.02f;
        defaults[CommonKeys::STDp_LTD_WEIGHT] = 0.015f;
        defaults[CommonKeys::STDp_TAU] = 20.0f;
        defaults[CommonKeys::SYNAPTOGENESIS_RATE] = 0.0001f;
        defaults[CommonKeys::PRUNING_RATE] = 0.00001f;
        defaults[CommonKeys::MAX_ENERGY] = 1.0f;
        defaults[CommonKeys::ENERGY_DECAY_RATE] = 0.01f;
        defaults[CommonKeys::WORLD_WIDTH] = 100.0f;
        defaults[CommonKeys::WORLD_HEIGHT] = 100.0f;
        defaults[CommonKeys::VISION_WIDTH] = 16;
        defaults[CommonKeys::VISION_HEIGHT] = 16;
        defaults[CommonKeys::AUDIO_ENABLED] = false;
        defaults[CommonKeys::AUDIO_SAMPLE_RATE] = 44100;
        defaults[CommonKeys::ENABLE_DEBUG_LOGGING] = false;
        defaults[CommonKeys::ENABLE_PERFORMANCE_MEASURING] = false;
        defaults[CommonKeys::CHECKPOINT_INTERVAL] = 100;
        defaults[CommonKeys::MAX_EPISODES] = 1000;
        defaults[CommonKeys::MAX_STEPS_PER_EPISODE] = 1000;
        defaults[CommonKeys::REWARD_SCALE] = 1.0f;
        defaults[CommonKeys::NOISE_LEVEL] = 0.01f;
        
        return defaults;
    }
    
    std::unordered_map<std::string, ConfigMetadata> getConfigMetadata() {
        std::unordered_map<std::string, ConfigMetadata> metadata;
        
        // Add metadata for each common key
        // Random seed
        {
            ConfigMetadata meta(CommonKeys::RANDOM_SEED, 
                               "Random seed for reproducible simulation");
            meta.type = "int64";
            meta.defaultValue = "42";
            meta.category = "simulation";
            meta.required = false;
            meta.validator = isValidSeed;
            metadata[CommonKeys::RANDOM_SEED] = meta;
        }
        
        // Simulation timestep
        {
            ConfigMetadata meta(CommonKeys::SIMULATION_TIMESTEP,
                               "Time step size for simulation (seconds)");
            meta.type = "double";
            meta.defaultValue = "0.001";
            meta.category = "simulation";
            meta.required = false;
            meta.validator = isPositive;
            meta.range = {1e-6, 1.0};
            metadata[CommonKeys::SIMULATION_TIMESTEP] = meta;
        }
        
        // Neuron count
        {
            ConfigMetadata meta(CommonKeys::NEURON_COUNT,
                               "Number of neurons in the brain");
            meta.type = "int64";
            meta.defaultValue = "500";
            meta.category = "brain";
            meta.required = false;
            meta.validator = isPositive;
            metadata[CommonKeys::NEURON_COUNT] = meta;
        }
        
        // And so on for other keys...
        
        return metadata;
    }
}

} // namespace nlm

// Template function definitions
namespace nlm {
template std::optional<int> Config::get<int>(const std::string& key) const;
template int Config::getOr<int>(const std::string& key, const int& defaultValue) const;
} // namespace nlm