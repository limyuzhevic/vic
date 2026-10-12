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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Detect file format based on extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == ".json") {
        success = loadFromJSONFile(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        success = loadFromYAMLFile(filepath);
    } else {
        // Default to simple key=value format
        success = loadFromSimpleFile(filepath);
    }
    
    if (success) {
        NLM_LOG_INFO("Loaded configuration from: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to load configuration from: " + filepath);
    }
    
    return success;
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

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to parse config line " + std::to_string(lineNum) + ": " + e.what());
                // Continue parsing other lines instead of failing completely
            }
        } else {
            NLM_LOG_WARNING("Invalid config format in line " + std::to_string(lineNum) + ": missing '='");
        }
    }
    
    return true;
}

bool Config::loadFromJSONFile(const std::string& filepath) {
#ifdef HAS_NLOHMANN_JSON
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        for (const auto& item : jsonData.items()) {
            std::string key = item.key();
            const nlohmann::json& value = item.value();
            
            // Convert JSON value to ConfigValue based on type
            if (value.is_string()) {
                set(key, value.get<std::string>(), ConfigSource::File);
            } else if (value.is_number_integer()) {
                set(key, value.get<int64_t>(), ConfigSource::File);
            } else if (value.is_number_float()) {
                set(key, value.get<double>(), ConfigSource::File);
            } else if (value.is_boolean()) {
                set(key, value.get<bool>(), ConfigSource::File);
            } else if (value.is_array()) {
                // Convert array to vector of strings (basic support)
                std::vector<std::string> vec;
                for (const auto& item : value) {
                    vec.push_back(item.get<std::string>());
                }
                set(key, vec, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Unsupported JSON type for key '" + key + "'")
                continue;
            }
        }
        
        NLM_LOG_INFO("Successfully loaded JSON config from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON config file: " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("JSON support not compiled in. Cannot load config file: " + filepath);
    return false;
#endif
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
#ifdef HAS_YAML_CPP
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        for (const auto& item : yamlData) {
            std::string key = item.first.as<std::string>();
            const YAML::Node& value = item.second;
            
            // Convert YAML node to ConfigValue based on type
            if (value.IsScalar()) {
                // Try to infer type from string content
                std::string scalarStr = value.as<std::string>();
                if (scalarStr == "true" || scalarStr == "false") {
                    set(key, scalarStr == "true", ConfigSource::File);
                } else if (std::all_of(scalarStr.begin(), scalarStr.end(), ::isdigit)) {
                    set(key, std::stoll(scalarStr), ConfigSource::File);
                } else if (std::any_of(scalarStr.begin(), scalarStr.end(), [](char c) { return c == '.'; })) {
                    set(key, std::stod(scalarStr), ConfigSource::File);
                } else {
                    set(key, scalarStr, ConfigSource::File);
                }
            } else if (value.IsSequence()) {
                std::vector<std::string> vec;
                for (const auto& item : value) {
                    vec.push_back(item.as<std::string>());
                }
                set(key, vec, ConfigSource::File);
            } else if (value.IsMap()) {
                // For nested maps, convert to JSON string representation
                std::string yamlStr = YAML::Dump(value);
                set(key, yamlStr, ConfigSource::File);
            }
        }
        
        NLM_LOG_INFO("Successfully loaded YAML config from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML config file: " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("YAML support not compiled in. Cannot load config file: " + filepath);
    return false;
#endif
}

bool Config::saveToJSONFile(const std::string& filepath) const {
#ifdef HAS_NLOHMANN_JSON
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            std::visit([&jsonData, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    // Basic support for int arrays
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    // Basic support for double arrays
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    // Basic support for string arrays
                    jsonData[entry.key] = arg;
                }
            }, entry.value);
        }
        
        file << jsonData.dump(2); // Pretty print with 2-space indentation
        NLM_LOG_INFO("Successfully saved JSON config to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to write JSON config file: " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("JSON support not compiled in. Cannot save config file: " + filepath);
    return false;
#endif
}

bool Config::saveToYAMLFile(const std::string& filepath) const {
#ifdef HAS_YAML_CPP
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            std::visit([&emitter](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    emitter << YAML::Value << arg;
                }
            }, entry.value);
        }
        
        emitter << YAML::EndMap;
        file << emitter.c_str();
        NLM_LOG_INFO("Successfully saved YAML config to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to write YAML config file: " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("YAML support not compiled in. Cannot save config file: " + filepath);
    return false;
#endif
}

// Enhanced validation system
bool Config::validate() const {
    NLM_LOG_INFO("Validating configuration...");
    
    bool allValid = true;
    
    // Validate known configuration keys
    for (const auto& entry : pImpl->entries) {
        bool valid = validateKeyValue(entry.key, entry.value);
        if (!valid) {
            NLM_LOG_ERROR("Validation failed for key: " + entry.key);
            allValid = false;
        }
    }
    
    // Check for required keys
    allValid &= validateRequiredKeys();
    
    // Validate dependencies between keys
    allValid &= validateKeyDependencies();
    
    if (allValid) {
        NLM_LOG_INFO("Configuration validation passed");
    } else {
        NLM_LOG_ERROR("Configuration validation failed");
    }
    
    return allValid;
}

bool Config::validateKeyValue(const std::string& key, const ConfigValue& value) const {
    // Basic type validation
    if (key == "neuron_count") {
        if (auto val = std::get_if<int64_t>(&value)) {
            if (*val <= 0) {
                NLM_LOG_ERROR("neuron_count must be positive, got: " + std::to_string(*val));
                return false;
            }
            if (*val > 100000) {
                NLM_LOG_WARNING("neuron_count is very large (" + std::to_string(*val) + ") - may impact performance");
            }
            return true;
        }
        NLM_LOG_ERROR("neuron_count must be an integer");
        return false;
    } else if (key == "region_count") {
        if (auto val = std::get_if<int64_t>(&value)) {
            if (*val <= 0) {
                NLM_LOG_ERROR("region_count must be positive, got: " + std::to_string(*val));
                return false;
            }
            return true;
        }
        NLM_LOG_ERROR("region_count must be an integer");
        return false;
    } else if (key == "connection_probability") {
        if (auto val = std::get_if<double>(&value)) {
            if (*val < 0.0 || *val > 1.0) {
                NLM_LOG_ERROR("connection_probability must be between 0.0 and 1.0, got: " + std::to_string(*val));
                return false;
            }
            return true;
        }
        NLM_LOG_ERROR("connection_probability must be a number");
        return false;
    } else if (key == "simulation_timestep") {
        if (auto val = std::get_if<double>(&value)) {
            if (*val <= 0.0) {
                NLM_LOG_ERROR("simulation_timestep must be positive, got: " + std::to_string(*val));
                return false;
            }
            if (*val > 1.0) {
                NLM_LOG_WARNING("simulation_timestep is large (" + std::to_string(*val) + ") - may impact accuracy");
            }
            return true;
        }
        NLM_LOG_ERROR("simulation_timestep must be a number");
        return false;
    } else if (key == "stdp_ltp_weight") {
        if (auto val = std::get_if<double>(&value)) {
            if (*val < 0.0) {
                NLM_LOG_ERROR("stdp_ltp_weight must be non-negative, got: " + std::to_string(*val));
                return false;
            }
            return true;
        }
        NLM_LOG_ERROR("stdp_ltp_weight must be a number");
        return false;
    } else if (key == "stdp_ltd_weight") {
        if (auto val = std::get_if<double>(&value)) {
            if (*val < 0.0) {
                NLM_LOG_ERROR("stdp_ltd_weight must be non-negative, got: " + std::to_string(*val));
                return false;
            }
            return true;
        }
        NLM_LOG_ERROR("stdp_ltd_weight must be a number");
        return false;
    } else if (key == "synaptogenesis_rate") {
        if (auto val = std::get_if<double>(&value)) {
            if (*val < 0.0) {
                NLM_LOG_ERROR("synaptogenesis_rate must be non-negative, got: " + std::to_string(*val));
                return false;
            }
            return true;
        }
        NLM_LOG_ERROR("synaptogenesis_rate must be a number");
        return false;
    } else if (key == "pruning_rate") {
        if (auto val = std::get_if<double>(&value)) {
            if (*val < 0.0) {
                NLM_LOG_ERROR("pruning_rate must be non-negative, got: " + std::to_string(*val));
                return false;
            }
            return true;
        }
        NLM_LOG_ERROR("pruning_rate must be a number");
        return false;
    }
    
    // Unknown keys pass validation
    return true;
}

bool Config::validateRequiredKeys() const {
    // Define required configuration keys
    std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count",
        "connection_probability"
    };
    
    bool allPresent = true;
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            NLM_LOG_ERROR("Required configuration key missing: " + key);
            allPresent = false;
        }
    }
    
    return allPresent;
}

bool Config::validateKeyDependencies() const {
    // Validate dependencies between keys
    bool allValid = true;
    
    // Example dependency: if region_count > 1, connection_probability should be reasonable
    if (has("region_count") && has("connection_probability")) {
        auto regionCount = get<int64_t>("region_count");
        auto connectionProb = get<double>("connection_probability");
        
        if (regionCount && connectionProb) {
            if (*regionCount > 10 && *connectionProb > 0.5) {
                NLM_LOG_WARNING("High region count (" + std::to_string(*regionCount) + ") with high connection probability (" + 
                              std::to_string(*connectionProb) + ") - may create very dense connectivity");
            }
        }
    }
    
    return allValid;
}

// Enhanced CLI handling with environment variable support
bool Config::loadFromArgsEnhanced(int argc, char** argv) {
    bool success = loadFromArgs(argc, argv);
    
    // Expand environment variables in values
    for (auto& entry : pImpl->entries) {
        if (entry.source == ConfigSource::CommandLine) {
            expandEnvironmentVariables(entry.value);
        }
    }
    
    return success;
}

void Config::expandEnvironmentVariables(ConfigValue& value) {
    std::string* strPtr = std::get_if<std::string>(&value);
    if (strPtr) {
        std::string& str = *strPtr;
        size_t pos = 0;
        while ((pos = str.find('${', pos)) != std::string::npos) {
            size_t endPos = str.find('}', pos);
            if (endPos != std::string::npos) {
                std::string envVar = str.substr(pos + 2, endPos - pos - 2);
                const char* envValue = std::getenv(envVar.c_str());
                if (envValue) {
                    str.replace(pos, endPos - pos + 1, envValue);
                } else {
                    NLM_LOG_WARNING("Environment variable not found: " + envVar);
                }
                pos += strlen(envValue);
            } else {
                break;
            }
        }
    }
}

// Preset configurations
void Config::applyPreset(const std::string& presetName) {
    std::transform(presetName.begin(), presetName.end(), presetName.begin(), ::tolower);
    
    if (presetName == "minimal") {
        clear();
        set("neuron_count", 100, ConfigSource::Default);
        set("region_count", 1, ConfigSource::Default);
        set("connection_probability", 0.1, ConfigSource::Default);
        set("simulation_timestep", 0.001, ConfigSource::Default);
        NLM_LOG_INFO("Applied 'minimal' preset configuration");
        
    } else if (presetName == "small") {
        clear();
        set("neuron_count", 500, ConfigSource::Default);
        set("region_count", 1, ConfigSource::Default);
        set("connection_probability", 0.15, ConfigSource::Default);
        set("simulation_timestep", 0.001, ConfigSource::Default);
        set("stdp_ltp_weight", 0.02, ConfigSource::Default);
        set("stdp_ltd_weight", 0.015, ConfigSource::Default);
        NLM_LOG_INFO("Applied 'small' preset configuration");
        
    } else if (presetName == "medium") {
        clear();
        set("neuron_count", 1000, ConfigSource::Default);
        set("region_count", 2, ConfigSource::Default);
        set("connection_probability", 0.1, ConfigSource::Default);
        set("simulation_timestep", 0.001, ConfigSource::Default);
        set("stdp_ltp_weight", 0.02, ConfigSource::Default);
        set("stdp_ltd_weight", 0.015, ConfigSource::Default);
        set("synaptogenesis_rate", 0.0001, ConfigSource::Default);
        set("pruning_rate", 0.00001, ConfigSource::Default);
        NLM_LOG_INFO("Applied 'medium' preset configuration");
        
    } else if (presetName == "large") {
        clear();
        set("neuron_count", 5000, ConfigSource::Default);
        set("region_count", 4, ConfigSource::Default);
        set("connection_probability", 0.05, ConfigSource::Default);
        set("simulation_timestep", 0.001, ConfigSource::Default);
        set("stdp_ltp_weight", 0.01, ConfigSource::Default);
        set("stdp_ltd_weight", 0.012, ConfigSource::Default);
        set("synaptogenesis_rate", 0.00005, ConfigSource::Default);
        set("pruning_rate", 0.000005, ConfigSource::Default);
        NLM_LOG_INFO("Applied 'large' preset configuration");
        
    } else if (presetName == "learning") {
        clear();
        set("neuron_count", 1000, ConfigSource::Default);
        set("region_count", 2, ConfigSource::Default);
        set("connection_probability", 0.1, ConfigSource::Default);
        set("simulation_timestep", 0.001, ConfigSource::Default);
        set("stdp_ltp_weight", 0.02, ConfigSource::Default);
        set("stdp_ltd_weight", 0.015, ConfigSource::Default);
        set("synaptogenesis_rate", 0.0001, ConfigSource::Default);
        set("pruning_rate", 0.00001, ConfigSource::Default);
        set("replay_interval", 100, ConfigSource::Default);
        set("consolidation_interval", 1000, ConfigSource::Default);
        NLM_LOG_INFO("Applied 'learning' preset configuration");
        
    } else if (presetName == "development") {
        clear();
        set("neuron_count", 2000, ConfigSource::Default);
        set("region_count", 3, ConfigSource::Default);
        set("connection_probability", 0.08, ConfigSource::Default);
        set("simulation_timestep", 0.001, ConfigSource::Default);
        set("stdp_ltp_weight", 0.015, ConfigSource::Default);
        set("stdp_ltd_weight", 0.012, ConfigSource::Default);
        set("synaptogenesis_rate", 0.0002, ConfigSource::Default);
        set("pruning_rate", 0.00002, ConfigSource::Default);
        set("enable_development", true, ConfigSource::Default);
        set("development_stage", "CriticalPeriod", ConfigSource::Default);
        NLM_LOG_INFO("Applied 'development' preset configuration");
        
    } else {
        throw NLMConfigurationError("Unknown preset configuration: " + presetName);
    }
}

void Config::exportToPython(py::module& m) {
    // Export enhanced Config functionality to Python
    m.def("create_config", []() {
        return std::make_shared<Config>();
    }, "Create a new configuration object");
    
    m.def("create_config_from_preset", [](const std::string& preset) {
        auto config = std::make_shared<Config>();
        config->applyPreset(preset);
        return config;
    }, py::arg("preset"), "Create configuration from preset (minimal, small, medium, large, learning, development)");
    
    m.def("validate_config", [](std::shared_ptr<Config> config) {
        return config->validate();
    }, py::arg("config"), "Validate configuration and return True if valid");
    
    // Register exception handlers for Python
    py::register_exception<NLMConfigurationError>(m, "NLMConfigurationError", PyExc_KeyError);
    py::register_exception<NLMValidationError>(m, "NLMValidationError", PyExc_ValueError);
    py::register_exception<NLMSerializationError>(m, "NLMSerializationError", PyExc_IOError);
}

std::string Config::getFormatExtension() const {
    // Determine preferred output format
    if (pImpl->entries.empty()) {
        return ".cfg"; // Default to simple format for empty configs
    }
    
    // Check if we have complex nested structures that need JSON/YAML
    for (const auto& entry : pImpl->entries) {
        if (std::get_if<std::vector<std::string>>(&entry.value) ||
            std::get_if<std::vector<int>>(&entry.value) ||
            std::get_if<std::vector<double>>(&entry.value)) {
            return ".json"; // Prefer JSON for array data
        }
    }
    
    // Default to simple format
    return ".cfg";
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
