#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigEntry> defaults;
    JsonSchema defaultSchema;
    size_t entryCount;
    size_t defaultCount;
    size_t fileCount;
    
    Impl() : entryCount(0), defaultCount(0), fileCount(0) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, const JsonSchema& schema) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData = nlohmann::json::parse(file);
        
        // Clear existing entries before loading
        clear();
        
        // Process JSON data
        if (jsonData.is_object()) {
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                ConfigSource source = schema.is_null() ? ConfigSource::File : ConfigSource::File;
                
                // Get description from schema if available
                std::string description = "";
                if (!schema.is_null() && schema.contains(key) && schema[key].contains("description")) {
                    description = schema[key]["description"].get<std::string>();
                }
                
                // Get value and convert to ConfigValue
                ConfigValue value;
                bool conversionSuccess = convertJsonToConfigValue(*it).has_value() ? 
                    (*it).is_null() ? ConfigValue(std::string()) : 
                    (*it).is_number_integer() ? ConfigValue(static_cast<int64_t>((*it).get<int64_t>())) :
                    (*it).is_number_unsigned() ? ConfigValue(static_cast<uint64_t>((*it).get<uint64_t>())) :
                    (*it).is_number_float() ? ConfigValue((*it).get<double>()) :
                    (*it).is_boolean() ? ConfigValue((*it).get<bool>()) :
                    (*it).is_string() ? ConfigValue((*it).get<std::string>()) :
                    (*it).is_array() ? convertJsonToConfigValue(*it).value() : ConfigValue(std::string());
                
                if (conversionSuccess) {
                    pImpl->entries.emplace_back(key, value, source, description);
                    pImpl->entryCount++;
                    if (source == ConfigSource::File) {
                        pImpl->fileCount++;
                    }
                }
            }
        }
        
        // Validate against schema if provided
        if (!schema.is_null()) {
            ValidationResult result = validate(schema);
            if (!result.valid) {
                // Log validation errors
                return false;
            }
        }
        
        return true;
    } catch (const nlohmann::json::exception& e) {
        return false;
    }
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
    
    nlohmann::json jsonData;
    
    // Convert all entries to JSON
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = convertConfigValueToJson(entry.value);
    }
    
    // Write JSON with pretty formatting
    file << jsonData.dump(2);
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    // Try to convert the stored ConfigValue to type T
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source,
                 const std::string& description, bool required,
                 const std::string& validationPattern,
                 const std::string& validationErrorMessage) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->description = description;
        it->required = required;
        it->validationPattern = validationPattern;
        it->validationErrorMessage = validationErrorMessage;
    } else {
        pImpl->entries.emplace_back(key, value, source, description, required,
                                    validationPattern, validationErrorMessage);
        pImpl->entryCount++;
        if (source == ConfigSource::File) {
            pImpl->fileCount++;
        }
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source,
                 const std::string& description, bool required,
                 const std::string& validationPattern,
                 const std::string& validationErrorMessage) {
    set(key, ConfigValue(value), source, description, required,
        validationPattern, validationErrorMessage);
}

void Config::set(const std::string& key, int value, ConfigSource source,
                 const std::string& description, bool required,
                 const std::string& validationPattern,
                 const std::string& validationErrorMessage) {
    set(key, ConfigValue(value), source, description, required,
        validationPattern, validationErrorMessage);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source,
                 const std::string& description, bool required,
                 const std::string& validationPattern,
                 const std::string& validationErrorMessage) {
    set(key, ConfigValue(value), source, description, required,
        validationPattern, validationErrorMessage);
}

void Config::set(const std::string& key, double value, ConfigSource source,
                 const std::string& description, bool required,
                 const std::string& validationPattern,
                 const std::string& validationErrorMessage) {
    set(key, ConfigValue(value), source, description, required,
        validationPattern, validationErrorMessage);
}

void Config::set(const std::string& key, bool value, ConfigSource source,
                 const std::string& description, bool required,
                 const std::string& validationPattern,
                 const std::string& validationErrorMessage) {
    set(key, ConfigValue(value), source, description, required,
        validationPattern, validationErrorMessage);
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
    pImpl->entryCount = pImpl->entries.size();
    pImpl->fileCount = countIf(pImpl->entries.begin(), pImpl->entries.end(),
                               [](const ConfigEntry& e) { return e.source == ConfigSource::File; });
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
    pImpl->entryCount = 0;
    pImpl->fileCount = 0;
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
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (entry.required) oss << " [REQUIRED]";
        oss << "\n";
    }
    return oss.str();
}

ValidationResult Config::validate(const JsonSchema& schema) const {
    ValidationResult result;
    
    if (schema.is_null()) {
        return result;
    }
    
    // Check for required keys
    if (schema.contains("required") && schema["required"].is_array()) {
        std::vector<std::string> requiredKeys = schema["required"].get<std::vector<std::string>>();
        for (const auto& key : requiredKeys) {
            if (!has(key)) {
                result.addError("Missing required key: " + key);
            }
        }
    }
    
    // Validate each entry against schema
    for (const auto& entry : pImpl->entries) {
        validateEntryAgainstSchema(entry, schema, result);
    }
    
    return result;
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    bool success = false;
    
    // Environment variable processing would go here
    // For now, just return false as it's not implemented
    
    return success;
}

void Config::setDefault(const std::string& key, const ConfigValue& value,
                        const std::string& description, bool required) {
    auto it = std::find_if(pImpl->defaults.begin(), pImpl->defaults.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->defaults.end()) {
        it->value = value;
        it->description = description;
        it->required = required;
    } else {
        pImpl->defaults.emplace_back(key, value, ConfigSource::Default, description, required);
        pImpl->defaultCount++;
    }
}

void Config::setDefaultSchema(const JsonSchema& schema) {
    pImpl->defaultSchema = schema;
}

size_t Config::getEntryCount() const {
    return pImpl->entryCount;
}

size_t Config::getDefaultCount() const {
    return pImpl->defaultCount;
}

size_t Config::getFileCount() const {
    return pImpl->fileCount;
}

std::string Config::toString(const ConfigValue& value) {
    return std::visit([](auto&& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<T, int>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, bool>) {
            return v ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(v[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(v[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + v[i] + "\"";
            }
            result += "]";
            return result;
        } else {
            return "";
        }
    }, value);
}

std::optional<ConfigValue> Config::fromString(const std::string& str) {
    // Simple type detection - could be enhanced
    if (str.empty()) return std::nullopt;
    
    // Try different types
    try {
        if (str.find('.') != std::string::npos) {
            return std::stod(str);
        } else {
            size_t pos;
            int64_t intVal = std::stoll(str, &pos);
            if (pos == str.size()) {
                return intVal;
            }
        }
        
        if (str == "true" || str == "false") {
            return str == "true";
        }
        
        return str;
    } catch (...) {
        return std::nullopt;
    }
}

bool Config::validatePattern(const std::string& value, const std::string& pattern) {
    if (pattern.empty()) return true;
    
    std::regex regex(pattern);
    return std::regex_match(value, regex);
}

bool Config::validateRange(const ConfigValue& value, double min, double max) {
    return std::visit([min, max](auto&& v) -> bool {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_arithmetic_v<T>) {
            return static_cast<double>(v) >= min && static_cast<double>(v) <= max;
        } else {
            return false;
        }
    }, value);
}

bool Config::validateChoices(const ConfigValue& value, const std::vector<std::string>& choices) {
    return std::visit([&choices](auto&& v) -> bool {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return std::find(choices.begin(), choices.end(), v) != choices.end();
        } else {
            return false;
        }
    }, value);
}

nlohmann::json Config::getDefaultSchema() {
    nlohmann::json schema;
    
    // Basic NLM configuration schema
    schema["type"] = "object";
    schema["description"] = "NLM (Neural Learning Machine) default configuration";
    
    // Required keys
    nlohmann::json requiredKeys;
    requiredKeys.push_back("random_seed");
    requiredKeys.push_back("neuron_count");
    requiredKeys.push_back("region_count");
    schema["required"] = requiredKeys;
    
    // Define properties for each configuration key
    nlohmann::json properties;
    
    nlohmann::json random_seed;
    random_seed["type"] = "integer";
    random_seed["description"] = "Random seed for reproducible experiments";
    random_seed["minimum"] = 0;
    properties["random_seed"] = random_seed;
    
    nlohmann::json neuron_count;
    neuron_count["type"] = "integer";
    neuron_count["description"] = "Number of neurons in the brain";
    neuron_count["minimum"] = 1;
    neuron_count["maximum"] = 10000;
    properties["neuron_count"] = neuron_count;
    
    nlohmann::json region_count;
    region_count["type"] = "integer";
    region_count["description"] = "Number of brain regions";
    region_count["minimum"] = 1;
    region_count["maximum"] = 100;
    properties["region_count"] = region_count;
    
    nlohmann::json connection_probability;
    connection_probability["type"] = "number";
    connection_probability["description"] = "Connection probability for random connectivity";
    connection_probability["minimum"] = 0.0;
    connection_probability["maximum"] = 1.0;
    properties["connection_probability"] = connection_probability;
    
    nlohmann::json simulation_timestep;
    simulation_timestep["type"] = "number";
    simulation_timestep["description"] = "Simulation timestep in seconds";
    simulation_timestep["minimum"] = 0.0001;
    simulation_timestep["maximum"] = 0.1;
    properties["simulation_timestep"] = simulation_timestep;
    
    schema["properties"] = properties;
    
    return schema;
}

nlohmann::json Config::getPhase2Schema() {
    nlohmann::json schema = getDefaultSchema();
    
    // Add Phase 2 specific properties
    nlohmann::json stdp_ltp_weight;
    stdp_ltp_weight["type"] = "number";
    stdp_ltp_weight["description"] = "STDP long-term potentiation weight";
    stdp_ltp_weight["minimum"] = 0.0;
    stdp_ltp_weight["maximum"] = 1.0;
    schema["properties"]["stdp_ltp_weight"] = stdp_ltp_weight;
    
    nlohmann::json stdp_ltd_weight;
    stdp_ltd_weight["type"] = "number";
    stdp_ltd_weight["description"] = "STDP long-term depression weight";
    stdp_ltd_weight["minimum"] = 0.0;
    stdp_ltd_weight["maximum"] = 1.0;
    schema["properties"]["stdp_ltd_weight"] = stdp_ltd_weight;
    
    return schema;
}

nlohmann::json Config::getPhase6Schema() {
    nlohmann::json schema = getPhase2Schema();
    
    // Add Phase 6 specific properties
    nlohmann::json enable_checkpointing;
    enable_checkpointing["type"] = "boolean";
    enable_checkpointing["description"] = "Enable checkpointing system";
    schema["properties"]["enable_checkpointing"] = enable_checkpointing;
    
    nlohmann::json enable_replay;
    enable_replay["type"] = "boolean";
    enable_replay["description"] = "Enable memory replay system";
    schema["properties"]["enable_replay"] = enable_replay;
    
    nlohmann::json enable_development;
    enable_development["type"] = "boolean";
    enable_development["description"] = "Enable developmental system";
    schema["properties"]["enable_development"] = enable_development;
    
    nlohmann::json enable_curiosity;
    enable_curiosity["type"] = "boolean";
    enable_curiosity["description"] = "Enable curiosity-driven exploration";
    schema["properties"]["enable_curiosity"] = enable_curiosity;
    
    nlohmann::json reward_discount_factor;
    reward_discount_factor["type"] = "number";
    reward_discount_factor["description"] = "Reward discount factor for learning";
    reward_discount_factor["minimum"] = 0.0;
    reward_discount_factor["maximum"] = 1.0;
    schema["properties"]["reward_discount_factor"] = reward_discount_factor;
    
    return schema;
}

// Template instantiations for get and getOr

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

// Type-specific getter implementations

std::optional<int> getInt(const Config& config, const std::string& key) {
    return config.get<int>(key);
}

std::optional<double> getDouble(const Config& config, const std::string& key) {
    return config.get<double>(key);
}

std::optional<bool> getBool(const Config& config, const std::string& key) {
    return config.get<bool>(key);
}

std::optional<std::string> getString(const Config& config, const std::string& key) {
    return config.get<std::string>(key);
}

// Type-specific setter implementations

void setInt(Config& config, const std::string& key, int value,
            ConfigSource source, const std::string& description, bool required) {
    config.set(key, value, source, description, required);
}

void setDouble(Config& config, const std::string& key, double value,
               ConfigSource source, const std::string& description, bool required) {
    config.set(key, value, source, description, required);
}

void setBool(Config& config, const std::string& key, bool value,
             ConfigSource source, const std::string& description, bool required) {
    config.set(key, value, source, description, required);
}

void setString(Config& config, const std::string& key, const std::string& value,
               ConfigSource source, const std::string& description, bool required) {
    config.set(key, value, source, description, required);
}

} // namespace nlm