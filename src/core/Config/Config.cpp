// NLM (熙然) - Neural Learning Machine
// Enhanced Configuration Implementation with JSON support and validation
// Implementation of the enhanced Config class

#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp> // Note: In production, use actual JSON library

using namespace nlm;

// Include the local JSON parser if nlohmann/json is not available
#ifdef NLM_NO_EXTERNAL_JSON
#include "core/Config/JSONParser.hpp"
#else
#include <nlohmann/json.hpp>
#endif

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config(const std::string& schemaName) : schemaName_(schemaName) {
    pImpl = std::make_unique<Impl>();
    
    // Set up default values based on schema
    if (schemaName == "Phase2") {
        set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        set("simulation_timestep", 0.001, ConfigSource::Default);
        set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Set default descriptions
        setDescription("random_seed", "Random seed for reproducible experiments");
        setDescription("simulation_timestep", "Simulation timestep in seconds");
        setDescription("neuron_count", "Number of neurons in the brain");
        setDescription("region_count", "Number of brain regions");
        setDescription("connection_probability", "Connection probability for random connectivity");
        setDescription("stdp_ltp_weight", "STDP Long-Term Potentiation weight");
        setDescription("stdp_ltd_weight", "STDP Long-Term Depression weight");
        setDescription("stdp_tau", "STDP time constant");
        setDescription("synaptogenesis_rate", "Structural synaptogenesis rate");
        setDescription("pruning_rate", "Structural pruning rate");
    } else if (schemaName == "Phase1") {
        // Phase 1 defaults (from original config file)
        set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        set("simulation_timestep", 0.001, ConfigSource::Default);
        set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Plasticity settings
        set("plasticity_learning_rate", 0.01, ConfigSource::Default);
        set("stdp_ltp_weight", 0.01, ConfigSource::Default);
        set("stdp_ltd_weight", 0.012, ConfigSource::Default);
        set("stdp_time_constant", 20.0, ConfigSource::Default);
        
        // Development settings
        set("development_synaptogenesis_rate", 0.001, ConfigSource::Default);
        set("development_pruning_rate", 0.0001, ConfigSource::Default);
        
        // Neuromodulation settings
        set("dopamine_baseline", 0.1, ConfigSource::Default); // Note: Duplicate in original config
        set("reward_discount_factor", 0.99, ConfigSource::Default);
        
        // Environment settings
        set("environment_name", "GridWorld", ConfigSource::Default);
        set("environment_width", 10, ConfigSource::Default);
        set("environment_height", 10, ConfigSource::Default);
        
        // Logging settings
        set("log_level", "INFO", ConfigSource::Default);
        set("log_to_file", false, ConfigSource::Default);
        set("log_filename", "nlm.log", ConfigSource::Default);
        
        // Simulation settings
        set("max_simulation_steps", 10000, ConfigSource::Default);
        set("simulation_time_limit", 0.0, ConfigSource::Default);
        
        // Visualization settings
        set("visualization_enabled", false, ConfigSource::Default);
        set("visualization_update_rate", 30.0, ConfigSource::Default);
        
        // Set default descriptions
        setDescription("random_seed", "Random seed for reproducible experiments");
        setDescription("simulation_timestep", "Simulation timestep in seconds");
        setDescription("neuron_count", "Number of neurons in the brain");
        setDescription("region_count", "Number of brain regions");
        setDescription("connection_probability", "Connection probability for random connectivity");
        setDescription("plasticity_learning_rate", "Plasticity learning rate");
        setDescription("stdp_ltp_weight", "STDP Long-Term Potentiation weight");
        setDescription("stdp_ltd_weight", "STDP Long-Term Depression weight");
        setDescription("stdp_time_constant", "STDP time constant");
        setDescription("development_synaptogenesis_rate", "Development synaptogenesis rate");
        setDescription("development_pruning_rate", "Development pruning rate");
        setDescription("dopamine_baseline", "Dopamine baseline value");
        setDescription("reward_discount_factor", "Reward discount factor");
        setDescription("environment_name", "Environment name");
        setDescription("environment_width", "Environment width");
        setDescription("environment_height", "Environment height");
        setDescription("log_level", "Logging level");
        setDescription("log_to_file", "Whether to log to file");
        setDescription("log_filename", "Log file name");
        setDescription("max_simulation_steps", "Maximum simulation steps");
        setDescription("simulation_time_limit", "Simulation time limit");
        setDescription("visualization_enabled", "Whether visualization is enabled");
        setDescription("visualization_update_rate", "Visualization update rate");
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file type by extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".yaml" || ext == ".yml") {
        return loadFromJSON(filepath);
    } else {
        return loadFromArgsFile(filepath);
    }
}

bool Config::loadFromJSON(const std::string& content) {
    try {
#ifdef NLM_USE_NLOHMANN_JSON
        auto json = nlohmann::json::parse(content);
        
        // Parse JSON and populate configuration
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            std::string valueStr = it.value().dump();
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            if (it.value().is_number_integer()) {
                configValue = it.value().get<int64_t>();
            } else if (it.value().is_number_float()) {
                configValue = it.value().get<double>();
            } else if (it.value().is_boolean()) {
                configValue = it.value().get<bool>();
            } else if (it.value().is_string()) {
                configValue = it.value().get<std::string>();
            } else if (it.value().is_array()) {
                // Handle array types
                auto array = it.value();
                if (array.empty()) {
                    configValue = std::vector<std::string>{};
                } else if (array[0].is_number_integer()) {
                    std::vector<int> intArray;
                    for (const auto& item : array) {
                        intArray.push_back(item.get<int>());
                    }
                    configValue = intArray;
                } else if (array[0].is_number_float()) {
                    std::vector<double> doubleArray;
                    for (const auto& item : array) {
                        doubleArray.push_back(item.get<double>());
                    }
                    configValue = doubleArray;
                } else {
                    std::vector<std::string> stringArray;
                    for (const auto& item : array) {
                        stringArray.push_back(item.get<std::string>());
                    }
                    configValue = stringArray;
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
#else
        // Use our simple JSON parser
        std::map<std::string, std::string> jsonConfig = parseJSONConfig(content);
        bool success = true;
        
        for (const auto& pair : jsonConfig) {
            if (!setFromString(pair.first, pair.second, ConfigSource::File)) {
                success = false;
            }
        }
        
        return success;
#endif
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromArgsFile(const std::string& filepath) {
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

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!setFromString(key, value, ConfigSource::CommandLine)) {
                    success = false;
                }
            } else {
                success = false; // Incomplete --key=value
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (!setFromString(key, value, ConfigSource::CommandLine)) {
                success = false;
            }
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine format by extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        // Save as JSON
        file << "{\n";
        
        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) file << ",\n";
            first = false;
            
            file << "  \"" << entry.key << "\": ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << std::fixed << std::setprecision(6) << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << std::fixed << std::setprecision(6) << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << arg[i] << "\"";
                    }
                    file << "]";
                }
            }, entry.value);
        }
        
        file << "\n}\n";
    } else {
        // Save as simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "\n\n";
        }
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
    setDescription(key, ""); // Clear description when setting via string
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    
    return ConfigSource::Default;
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
        oss << "] (" << static_cast<int>(entry.source) << ") - " << entry.description << "\n";
    }
    return oss.str();
}

ValidationResult Config::validate(const std::string& schemaName) const {
    // Convert current config to string map for validation
    std::map<std::string, std::string> configMap;
    
    for (const auto& entry : pImpl->entries) {
        configMap[entry.key] = valueToString(entry.value);
    }
    
    ValidationResult result;
    result.valid = ConfigValidator::validateConfig(configMap, schemaName);
    
    if (!result.valid) {
        result.errors = getSchemaValidationErrors(configMap, schemaName);
    }
    
    return result;
}

ValidationResult Config::validateWithDetailedErrors(const std::string& schemaName) const {
    ValidationResult result = validate(schemaName);
    
    if (result.hasErrors()) {
        result.errors = getSchemaValidationErrors(
            convertToConfigMap(), schemaName);
    }
    
    return result;
}

std::vector<std::string> Config::getAvailableSchemas() {
    return ConfigSchemaRegistry::getAvailableSchemas();
}

std::string Config::generateConfigTemplate(const std::string& schemaName) {
    return ConfigValidator::generateConfigTemplate(schemaName);
}

bool Config::validateConfig(const std::map<std::string, std::string>& config, const std::string& schemaName) {
    return ConfigValidator::validateConfig(config, schemaName);
}

// Helper function implementations
bool Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Try to parse as different types
    if (isNumeric(valueStr)) {
        try {
            if (valueStr.find('.') != std::string::npos) {
                double val = std::stod(valueStr);
                set(key, val, source);
            } else {
                int64_t val = std::stoll(valueStr);
                set(key, val, source);
            }
            return true;
        } catch (...) {
            return false;
        }
    } else if (valueStr == "true" || valueStr == "false") {
        bool val = (valueStr == "true");
        set(key, val, source);
        return true;
    } else {
        set(key, valueStr, source);
        return true;
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

bool Config::isNumeric(const std::string& str) {
    try {
        std::stod(str);
        return true;
    } catch (...) {
        return false;
    }
}

std::string Config::convertToConfigFormat(const std::string& jsonValue) {
    // Basic JSON to config format conversion
    if (jsonValue.empty() || jsonValue == "null") return "";
    if (jsonValue == "true" || jsonValue == "false") return jsonValue;
    if (jsonValue.front() == '"' && jsonValue.back() == '"') {
        return jsonValue.substr(1, jsonValue.size() - 2);
    }
    return jsonValue;
}

std::string Config::convertFromConfigFormat(const std::string& configValue) {
    // Basic config format to JSON conversion
    if (configValue.empty()) return "null";
    if (configValue == "true" || configValue == "false") return configValue;
    if (configValue.find(',') != std::string::npos || configValue.find('[') != std::string::npos) {
        return "[]"; // Simple array representation
    }
    return "\"" + configValue + "\"";
}

std::map<std::string, std::string> Config::parseSimpleConfig(const std::string& content) {
    std::map<std::string, std::string> config;
    
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            config[key] = value;
        }
    }
    
    return config;
}

std::map<std::string, std::string> Config::parseJSONConfig(const std::string& content) {
    // Use JSONConfigParser from config_validator.hpp
    return JSONConfigParser::parse(content);
}

std::vector<std::string> Config::getSchemaValidationErrors(const std::map<std::string, std::string>& config, const std::string& schemaName) const {
    return ConfigValidator::getValidationErrors(config, schemaName);
}

std::map<std::string, std::string> Config::convertToConfigMap() const {
    std::map<std::string, std::string> configMap;
    
    for (const auto& entry : pImpl->entries) {
        configMap[entry.key] = valueToString(entry.value);
    }
    
    return configMap;
}

std::string Config::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::fixed << std::setprecision(6) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        }
    }, value);
    
    return oss.str();
}

bool Config::isIntType(const ConfigValue& value) {
    return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
}

bool Config::isDoubleType(const ConfigValue& value) {
    return std::holds_alternative<double>(value);
}

bool Config::isStringType(const ConfigValue& value) {
    return std::holds_alternative<std::string>(value);
}

bool Config::isBoolType(const ConfigValue& value) {
    return std::holds_alternative<bool>(value);
}

bool Config::isVectorType(const ConfigValue& value) {
    return std::holds_alternative<std::vector<int>>(value) ||
           std::holds_alternative<std::vector<double>>(value) ||
           std::holds_alternative<std::vector<std::string>>(value);
}

std::string Config::getTypeName(const ConfigValue& value) {
    if (isIntType(value)) return "int";
    if (isDoubleType(value)) return "float";
    if (isStringType(value)) return "string";
    if (isBoolType(value)) return "bool";
    if (isVectorType(value)) return "vector";
    return "unknown";
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

} // namespace nlm
