#include "Config_Improved.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <unordered_set>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<std::string> errors;
    std::map<std::string, ConfigSource> sourceMap;
    std::map<std::string, bool> validationMap;
    std::vector<ConfigSchema> schema;
    bool compressed = false;
};

// Helper function to get JSON library
static json parseJsonFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }
    return json::parse(file);
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default schema
    pImpl->schema = getDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first, fallback to key=value
    if (loadFromJson(filepath)) {
        return true;
    }
    return loadFromKeyValue(filepath);
}

bool Config::loadFromJson(const std::string& filepath) {
    try {
        auto configJson = parseJsonFromFile(filepath);
        return parseJsonObject(configJson);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON config: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromKeyValue(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }

    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        
        // Skip empty lines and comments
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
            
            if (!setValidated(key, value, ConfigSource::File)) {
                NLM_LOG_WARNING("Invalid config value in " + filepath + ": line " + 
                              std::to_string(lineNum) + ": " + key + " = " + value);
            }
        } else {
            NLM_LOG_WARNING("Invalid config line in " + filepath + ": line " + 
                          std::to_string(lineNum) + ": " + line);
        }
    }
    
    return true;
}

bool Config::loadFromJsonString(const std::string& jsonStr) {
    try {
        auto configJson = json::parse(jsonStr);
        return parseJsonObject(configJson);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON string: " + std::string(e.what()));
        return false;
    }
}

bool Config::parseJsonObject(const json& jsonObject) {
    clearErrors();
    
    // Check for deprecated keys and migrate
    for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        const std::string& key = it.key();
        
        // Handle deprecated keys
        if (isKeyDeprecated(key)) {
            std::string newKey = getReplacementKey(key);
            NLM_LOG_INFO("Migrating deprecated config key: " + key + " -> " + newKey);
            
            // Parse the deprecated key's value
            auto parsedValue = parseJsonValue(it.value());
            if (parsedValue.has_value()) {
                setValidated(newKey, parsedValue.value(), ConfigSource::File);
            }
            continue;
        }
        
        // Parse the key's value
        auto parsedValue = parseJsonValue(it.value());
        if (parsedValue.has_value()) {
            if (!setValidated(key, parsedValue.value(), ConfigSource::File)) {
                // Error was already added by setValidated
            }
        }
    }
    
    return validateAll();
}

bool Config::parseLegacyFormat(const json& jsonLegacy) {
    // Handle legacy format where values are strings
    clearErrors();
    
    for (auto it = jsonLegacy.begin(); it != jsonLegacy.end(); ++it) {
        const std::string& key = it.key();
        std::string valueStr = it.value();
        
        if (isKeyDeprecated(key)) {
            std::string newKey = getReplacementKey(key);
            NLM_LOG_INFO("Migrating deprecated config key: " + key + " -> " + newKey);
            setValidated(newKey, valueStr, ConfigSource::File);
            continue;
        }
        
        setValidated(key, valueStr, ConfigSource::File);
    }
    
    return validateAll();
}

std::optional<ConfigValue> Config::parseJsonValue(const json& jsonValue) {
    try {
        if (jsonValue.is_number_integer()) {
            return jsonValue.get<int64_t>();
        } else if (jsonValue.is_number_float()) {
            return jsonValue.get<double>();
        } else if (jsonValue.is_boolean()) {
            return jsonValue.get<bool>();
        } else if (jsonValue.is_string()) {
            return jsonValue.get<std::string>();
        } else if (jsonValue.is_array()) {
            // Parse array
            std::vector<double> vec;
            for (const auto& item : jsonValue) {
                vec.push_back(item.get<double>());
            }
            return vec;
        } else if (jsonValue.is_null()) {
            return std::string(""); // Default empty string
        } else {
            NLM_LOG_WARNING("Unsupported JSON value type: " + jsonValue.dump());
            return std::nullopt;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON value: " + std::string(e.what()));
        return std::nullopt;
    }
    return std::nullopt;
}

nlohmann::json Config::convertToJson(const ConfigValue& value) const {
    if (std::holds_alternative<int64_t>(value)) {
        return std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        // Convert int vector to double array
        const auto& intVec = std::get<std::vector<int>>(value);
        json::array_t arr;
        for (int val : intVec) {
            arr.push_back(val);
        }
        return arr;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return std::get<std::vector<double>>(value);
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        // For string vectors, we'll return as array of strings
        const auto& strVec = std::get<std::vector<std::string>>(value);
        json::array_t arr;
        for (const auto& val : strVec) {
            arr.push_back(val);
        }
        return arr;
    }
    return nullptr;
}

bool Config::setValidated(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Check if key is valid
    if (!isValidJsonKey(key)) {
        addError(key, "Invalid configuration key format: " + key);
        return false;
    }
    
    // Check for deprecated key
    if (isKeyDeprecated(key)) {
        addError(key, "Deprecated configuration key: " + key + ". Use: " + 
                  getReplacementKey(key) + " instead.");
        return false;
    }
    
    // Find schema entry
    auto schemaIt = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
                                 [&key](const ConfigSchema& s) { return s.key == key; });
    
    // Validate against schema if exists
    if (schemaIt != pImpl->schema.end()) {
        if (!validateValue(key, value, *schemaIt)) {
            // Error was already added by validateValue
            return false;
        }
    }
    
    // Set the value
    set(key, value, source);
    
    // Validate the set value
    if (!isValid(key)) {
        addError(key, "Invalid value for configuration key: " + key);
        return false;
    }
    
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value, const ConfigSchema& schema) {
    // Check required field
    if (schema.required && (value == ConfigValue{} || (std::holds_alternative<std::string>(value) && 
        std::get<std::string>(value).empty()))) {
        addError(key, "Required configuration key is missing or empty: " + key);
        return false;
    }
    
    // Check range validation
    if (!schema.range.first == 0 && !schema.range.second == 0) {
        if (!validateRange(value, schema.range)) {
            addError(key, "Value for key " + key + " is out of range: " + 
                      std::to_string(schema.range.first) + " - " + 
                      std::to_string(schema.range.second));
            return false;
        }
    }
    
    return true;
}

bool Config::validateRange(const ConfigValue& value, const std::pair<double, double>& range) const {
    if (std::holds_alternative<int64_t>(value)) {
        double val = static_cast<double>(std::get<int64_t>(value));
        return val >= range.first && val <= range.second;
    } else if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        return val >= range.first && val <= range.second;
    }
    // String and other types don't have numeric ranges
    return true;
}

bool Config::isValid(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false; // Key doesn't exist
    }
    
    // Basic type validation
    const auto& value = it->value;
    
    if (std::holds_alternative<int64_t>(value)) {
        return true;
    } else if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        return !std::isnan(val) && !std::isinf(val);
    } else if (std::holds_alternative<bool>(value)) {
        return true;
    } else if (std::holds_alternative<std::string>(value)) {
        const std::string& str = std::get<std::string>(value);
        return !str.empty();
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        return true;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return true;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return true;
    }
    
    return false;
}

void Config::addError(const std::string& key, const std::string& error) {
    pImpl->errors.push_back(key + ": " + error);
}

void Config::clearErrors() {
    pImpl->errors.clear();
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->errors;
}

bool Config::isValidJsonKey(const std::string& key) {
    // Check for valid characters
    return !key.empty() && std::all_of(key.begin(), key.end(),
        [](char c) { return std::isalnum(c) || c == '_' || c == '-'; });
}

std::string Config::getTypeName(const std::type_info& typeInfo) {
    if (typeInfo == typeid(int64_t)) return "int64_t";
    if (typeInfo == typeid(double)) return "double";
    if (typeInfo == typeid(bool)) return "bool";
    if (typeInfo == typeid(std::string)) return "string";
    if (typeInfo == typeid(std::vector<int>)) return "vector<int>";
    if (typeInfo == typeid(std::vector<double>)) return "vector<double>";
    if (typeInfo == typeid(std::vector<std::string>)) return "vector<string>";
    return "unknown";
}

bool Config::isKeyDeprecated(const std::string& key) {
    // Deprecated keys list
    static const std::unordered_set<std::string> deprecatedKeys = {
        "dopamine_baseline",
        "plasticity_learning_rate",
        "stdp_tau",
        "synaptogenesis_rate",
        "pruning_rate"
    };
    return deprecatedKeys.find(key) != deprecatedKeys.end();
}

std::string Config::getReplacementKey(const std::string& deprecatedKey) {
    // Map of deprecated keys to their replacements
    static const std::map<std::string, std::string> replacements = {
        {"dopamine_baseline", "neuromodulation_baseline"},
        {"plasticity_learning_rate", "plasticity_learning_rate"},
        {"stdp_tau", "stdp_time_constant"},
        {"synaptogenesis_rate", "development_synaptogenesis_rate"},
        {"pruning_rate", "development_pruning_rate"}
    };
    
    auto it = replacements.find(deprecatedKey);
    return it != replacements.end() ? it->second : deprecatedKey;
}

void Config::migrateFromLegacy(const Config& legacyConfig) {
    for (const auto& entry : legacyConfig.pImpl->entries) {
        if (isKeyDeprecated(entry.key)) {
            std::string newKey = getReplacementKey(entry.key);
            set(newKey, entry.value, entry.source);
        }
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
                if (!setValidated(key, value, ConfigSource::CommandLine)) {
                    NLM_LOG_WARNING("Invalid command line config: " + key + " = " + value);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (!setValidated(key, value, ConfigSource::CommandLine)) {
                NLM_LOG_WARNING("Invalid command line config: " + key + " = " + value);
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for writing: " + filepath);
        return false;
    }
    
    json configJson = json::object();
    
    for (const auto& entry : pImpl->entries) {
        configJson[entry.key] = convertToJson(entry.value);
    }
    
    file << configJson.dump(2);
    return true;
}

bool Config::saveToKeyValue(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for writing: " + filepath);
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
    clearErrors();
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

std::vector<ConfigSchema> Config::getDefaultSchema() {
    return {
        {"random_seed", "int64_t", "Random seed for reproducible experiments", "42", {0, 2147483647}, true},
        {"simulation_timestep", "double", "Simulation timestep in seconds", "0.001", {1e-9, 1.0}, true},
        {"neuron_count", "int64_t", "Number of neurons in the brain", "1000", {1, 1000000}, true},
        {"region_count", "int64_t", "Number of brain regions", "2", {1, 100}, true},
        {"connection_probability", "double", "Connection probability for random connectivity", "0.1", {0.0, 1.0}, true},
        {"plasticity_learning_rate", "double", "Plasticity learning rate", "0.01", {0.0, 1.0}, true},
        {"stdp_ltp_weight", "double", "STDP LTP weight", "0.01", {0.0, 1.0}, true},
        {"stdp_ltd_weight", "double", "STDP LTD weight", "0.012", {0.0, 1.0}, true},
        {"stdp_time_constant", "double", "STDP time constant in milliseconds", "20.0", {1.0, 1000.0}, true},
        {"development_synaptogenesis_rate", "double", "Synaptogenesis rate", "0.001", {0.0, 1.0}, true},
        {"development_pruning_rate", "double", "Pruning rate", "0.0001", {0.0, 1.0}, true},
        {"dopamine_baseline", "double", "Dopamine baseline level", "0.1", {-10.0, 10.0}, true},
        {"reward_discount_factor", "double", "Reward discount factor", "0.99", {0.0, 1.0}, true},
        {"environment_name", "string", "Environment name", "GridWorld", {0, 0}, true},
        {"environment_width", "int64_t", "Environment width", "10", {1, 1000}, true},
        {"environment_height", "int64_t", "Environment height", "10", {1, 1000}, true},
        {"log_level", "string", "Log level", "INFO", {0, 0}, true},
        {"log_to_file", "bool", "Whether to log to file", "false", {0, 0}, true},
        {"log_filename", "string", "Log filename", "nlm.log", {0, 0}, true},
        {"max_simulation_steps", "int64_t", "Maximum simulation steps", "10000", {1, 1000000000}, true},
        {"simulation_time_limit", "double", "Simulation time limit in seconds", "0.0", {0.0, 86400.0}, true},
        {"visualization_enabled", "bool", "Whether visualization is enabled", "false", {0, 0}, true},
        {"visualization_update_rate", "double", "Visualization update rate in Hz", "30.0", {1.0, 1000.0}, true}
    };
}

bool Config::validateAll() const {
    clearErrors();
    
    for (const auto& entry : pImpl->entries) {
        if (!isValid(entry.key)) {
            addError(entry.key, "Invalid configuration value for key: " + entry.key);
        }
        
        // Additional schema validation would be performed here
        // In a full implementation, we would validate against the schema
    }
    
    return pImpl->errors.empty();
}

std::string Config::exportToJson() const {
    json configJson = json::object();
    
    for (const auto& entry : pImpl->entries) {
        configJson[entry.key] = convertToJson(entry.value);
    }
    
    return configJson.dump(2);
}

bool Config::importFromJson(const std::string& jsonStr) {
    try {
        auto configJson = json::parse(jsonStr);
        return parseJsonObject(configJson);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON string: " + std::string(e.what()));
        return false;
    }
}

void Config::mergeFrom(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Override with other's values if key exists
        if (has(entry.key)) {
            set(entry.key, entry.value, entry.source);
        } else {
            pImpl->entries.push_back(entry);
        }
    }
}

Config::Stats Config::getStats() const {
    Stats stats;
    stats.totalEntries = pImpl->entries.size();
    
    size_t validCount = 0;
    for (const auto& entry : pImpl->entries) {
        if (isValid(entry.key)) {
            validCount++;
        }
    }
    stats.validEntries = validCount;
    stats.invalidEntries = stats.totalEntries - validCount;
    
    // Collect unique sources
    std::unordered_set<ConfigSource> sources;
    for (const auto& entry : pImpl->entries) {
        sources.insert(entry.source);
    }
    stats.sources = std::vector<std::string>(sources.begin(), sources.end());
    
    return stats;
}

int Config::getHealthScore() const {
    if (pImpl->entries.empty()) {
        return 0;
    }
    
    int score = 0;
    int maxScore = 0;
    
    // Check for required keys
    for (const auto& schema : pImpl->schema) {
        maxScore += 10; // Each schema entry contributes to max score
        
        if (has(schema.key)) {
            score += 10;
        }
    }
    
    // Check for valid values
    for (const auto& entry : pImpl->entries) {
        maxScore += 5; // Each entry contributes to max score
        if (isValid(entry.key)) {
            score += 5;
        }
    }
    
    // Check for errors
    maxScore += pImpl->errors.size() * 5;
    
    // Deduct points for errors
    score -= pImpl->errors.size() * 5;
    
    // Ensure score is between 0 and 100
    int healthScore = std::max(0, std::min(100, score));
    return healthScore;
}

std::vector<std::string> Config::getDeprecatedKeys() const {
    std::vector<std::string> deprecatedKeys;
    for (const auto& entry : pImpl->entries) {
        if (isKeyDeprecated(entry.key)) {
            deprecatedKeys.push_back(entry.key);
        }
    }
    return deprecatedKeys;
}

std::vector<std::pair<std::string, std::string>> Config::getDeprecatedKeyReplacements() const {
    std::vector<std::pair<std::string, std::string>> replacements;
    for (const auto& entry : pImpl->entries) {
        if (isKeyDeprecated(entry.key)) {
            replacements.emplace_back(entry.key, getReplacementKey(entry.key));
        }
    }
    return replacements;
}

bool Config::loadSchemaFromJson(const std::string& filepath) {
    try {
        auto schemaJson = parseJsonFromFile(filepath);
        
        if (!schemaJson.is_array()) {
            NLM_LOG_ERROR("Schema file must be a JSON array");
            return false;
        }
        
        pImpl->schema.clear();
        
        for (const auto& item : schemaJson) {
            if (item.is_object()) {
                ConfigSchema schema;
                schema.key = item.value("key", "");
                schema.type = item.value("type", "");
                schema.description = item.value("description", "");
                schema.defaultValue = item.value("defaultValue", "");
                
                if (item.contains("range") && item["range"].is_array() && item["range"].size() >= 2) {
                    schema.range.first = item["range"][0].get<double>();
                    schema.range.second = item["range"][1].get<double>();
                }
                
                schema.required = item.value("required", false);
                
                if (!schema.key.empty() && !schema.type.empty()) {
                    pImpl->schema.push_back(schema);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load schema from JSON: " + std::string(e.what()));
        return false;
    }
}

bool Config::compressConfiguration() {
    // Simple compression implementation
    // In a real implementation, we would use more sophisticated compression
    if (pImpl->entries.size() < 10) {
        return false; // Not worth compressing for small configurations
    }
    
    // Remove duplicate entries
    std::sort(pImpl->entries.begin(), pImpl->entries.end(),
              [](const ConfigEntry& a, const ConfigEntry& b) { return a.key < b.key; });
    auto last = std::unique(pImpl->entries.begin(), pImpl->entries.end(),
                            [](const ConfigEntry& a, const ConfigEntry& b) { return a.key == b.key; });
    pImpl->entries.erase(last, pImpl->entries.end());
    
    pImpl->compressed = true;
    return true;
}

bool Config::decompressConfiguration() {
    pImpl->compressed = false;
    return true;
}

bool Config::backupConfiguration(const std::string& backupPath) const {
    std::string backupFile = backupPath + ".backup.json";
    return saveToFile(backupFile);
}

bool Config::restoreConfiguration(const std::string& backupPath) {
    std::string backupFile = backupPath + ".backup.json";
    if (!saveToFile(backupFile)) {
        return false;
    }
    
    clear();
    return loadFromFile(backupFile);
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (otherIt == other.pImpl->entries.end()) {
            return false;
        }
        
        if (entry.value != otherIt->value) {
            return false;
        }
    }
    
    return true;
}

std::string Config::getFingerprint() const {
    std::string fingerprint = "config_";
    for (const auto& entry : pImpl->entries) {
        fingerprint += entry.key + "=" + std::to_string(std::hash<ConfigValue>()(entry.value));
    }
    return std::to_string(std::hash<std::string>()(fingerprint));
}

// Explicit template instantiations
namespace nlm {
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
}

} // namespace nlm