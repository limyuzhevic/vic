#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// JSON support (nlohmann::json)
#ifdef __has_include
#if __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
#else
#error "nlohmann/json.hpp not found. Please install nlohmann/json library."
#endif
#else
#include <nlohmann/json.hpp>
#endif

// YAML support (yaml-cpp)
#ifdef __has_include
#if __has_include(<yaml-cpp/yaml.h>)
#include <yaml-cpp/yaml.h>
#else
#error "yaml-cpp/yaml.h not found. Please install yaml-cpp library."
#endif
#else
#include <yaml-cpp/yaml.h>
#endif

#include <unordered_set>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ValidationRule> validationRules;
    ConfigOptions options;
    mutable std::vector<std::string> validationErrors;
    std::string lastError;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Set default validation rules for common configuration keys
    addValidationRule({"random_seed", 
        [](const ConfigValue& val) {
            if (std::holds_alternative<int>(val)) {
                return std::get<int>(val) >= 0;
            }
            return false;
        }, 
        "Random seed must be a non-negative integer"});
    
    addValidationRule({"simulation_timestep", 
        [](const ConfigValue& val) {
            if (std::holds_alternative<double>(val)) {
                return std::get<double>(val) > 0.0;
            }
            return false;
        }, 
        "Simulation timestep must be a positive number"});
    
    addValidationRule({"neuron_count", 
        [](const ConfigValue& val) {
            if (std::holds_alternative<int>(val)) {
                return std::get<int>(val) > 0;
            }
            return false;
        }, 
        "Neuron count must be a positive integer"});
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Configuration file format parsing implementations
namespace {
    // Helper to parse simple key=value format
    bool parsePlainText(const std::string& content, std::vector<ConfigEntry>& entries) {
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line)) {
            line = Config::trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = Config::trim(line.substr(0, pos));
                std::string value = Config::trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                entries.emplace_back(key, Config::stringToConfigValue(value), ConfigSource::File);
            }
        }
        return true;
    }
    
    // Check if a string is valid JSON
    bool isJSON(const std::string& content) {
        try {
            nlohmann::json::parse(content);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Check if a string is valid YAML
    bool isYAML(const std::string& content) {
        try {
            YAML::Load(content);
            return true;
        } catch (...) {
            return false;
        }
    }
}

bool Config::loadFromFile(const std::string& filepath, ConfigFileFormat format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    std::vector<ConfigEntry> entries;
    bool success = false;
    
    switch (format) {
        case ConfigFileFormat::PlainText:
            success = parsePlainText(content, entries);
            break;
        case ConfigFileFormat::JSON:
            success = parseJSON(content, entries);
            break;
        case ConfigFileFormat::YAML:
            success = parseYAML(content, entries);
            break;
    }
    
    if (success && pImpl->options.mergeOnLoad) {
        // Merge with existing entries
        for (const auto& entry : entries) {
            set(entry.key, entry.value, entry.source);
        }
    } else if (success) {
        pImpl->entries = std::move(entries);
        validate();
    }
    
    return success;
}

bool Config::loadFromJSON(const std::string& jsonString) {
    std::vector<ConfigEntry> entries;
    if (parseJSON(jsonString, entries)) {
        pImpl->entries = std::move(entries);
        validate();
        return true;
    }
    return false;
}

bool Config::loadFromYAML(const std::string& yamlString) {
    std::vector<ConfigEntry> entries;
    if (parseYAML(yamlString, entries)) {
        pImpl->entries = std::move(entries);
        validate();
        return true;
    }
    return false;
}

bool Config::saveToFile(const std::string& filepath, ConfigFileFormat format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    switch (format) {
        case ConfigFileFormat::PlainText:
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                file << entry.key << " = " << configValueToString(entry.value) << "\n\n";
            }
            break;
        case ConfigFileFormat::JSON:
            {
                nlohmann::json j;
                for (const auto& entry : pImpl->entries) {
                    j[entry.key] = configValueToString(entry.value);
                }
                file << j.dump(2);
            }
            break;
        case ConfigFileFormat::YAML:
            {
                YAML::Emitter emitter;
                emitter << YAML::BeginMap;
                for (const auto& entry : pImpl->entries) {
                    emitter << YAML::Key << entry.key;
                    emitter << YAML::Value << configValueToString(entry.value);
                }
                emitter << YAML::EndMap;
                file << emitter.c_str();
            }
            break;
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

// Preset profile definitions
namespace {
    PresetProfile createBasicProfile() {
        PresetProfile profile;
        profile.name = "basic";
        profile.type = PresetProfile::ProfileType::Basic;
        profile.description = "Beginner-friendly configuration for simple experiments";
        profile.useCase = "Getting started with NLM, running basic simulations";
        
        // Add basic configuration entries
        profile.entries = {
            {"random_seed", 42, ConfigSource::Default, "Random seed for reproducible experiments"},
            {"simulation_timestep", 0.001, ConfigSource::Default, "Simulation timestep in seconds"},
            {"neuron_count", 100, ConfigSource::Default, "Number of neurons in the brain"},
            {"region_count", 1, ConfigSource::Default, "Number of brain regions"},
            {"connection_probability", 0.05, ConfigSource::Default, "Connection probability for random connectivity"},
            {"log_level", std::string("INFO"), ConfigSource::Default, "Logging level"},
            {"max_simulation_steps", 1000, ConfigSource::Default, "Maximum simulation steps"},
        };
        
        return profile;
    }
    
    PresetProfile createAdvancedProfile() {
        PresetProfile profile;
        profile.name = "advanced";
        profile.type = PresetProfile::ProfileType::Advanced;
        profile.description = "Advanced configuration for complex experiments";
        profile.useCase = "Research-grade simulations with optimization and monitoring";
        
        profile.entries = {
            {"random_seed", 12345, ConfigSource::Default, "Random seed for reproducible experiments"},
            {"simulation_timestep", 0.0005, ConfigSource::Default, "High-resolution simulation timestep"},
            {"neuron_count", 1000, ConfigSource::Default, "Number of neurons in the brain"},
            {"region_count", 4, ConfigSource::Default, "Number of brain regions for modular organization"},
            {"connection_probability", 0.1, ConfigSource::Default, "Connection probability for random connectivity"},
            {"plasticity_learning_rate", 0.02, ConfigSource::Default, "Plasticity learning rate"},
            {"stdp_ltp_weight", 0.02, ConfigSource::Default, "STDP LTP weight"},
            {"stdp_ltd_weight", 0.022, ConfigSource::Default, "STDP LTD weight"},
            {"stdp_time_constant", 20.0, ConfigSource::Default, "STDP time constant"},
            {"development_synaptogenesis_rate", 0.002, ConfigSource::Default, "Synaptogenesis rate"},
            {"development_pruning_rate", 0.0002, ConfigSource::Default, "Pruning rate"},
            {"dopamine_baseline", 0.1, ConfigSource::Default, "Dopamine baseline level"},
            {"reward_discount_factor", 0.99, ConfigSource::Default, "Reward discount factor"},
            {"environment_name", std::string("GridWorld"), ConfigSource::Default, "Environment type"},
            {"log_level", std::string("DEBUG"), ConfigSource::Default, "Debug logging level"},
            {"log_to_file", true, ConfigSource::Default, "Log to file for analysis"},
            {"log_filename", std::string("nlm_advanced.log"), ConfigSource::Default, "Log filename"},
            {"max_simulation_steps", 50000, ConfigSource::Default, "Maximum simulation steps"},
            {"simulation_time_limit", 0.0, ConfigSource::Default, "Simulation time limit (0 = no limit)"},
            {"visualization_enabled", true, ConfigSource::Default, "Enable visualization"},
            {"visualization_update_rate", 30.0, ConfigSource::Default, "Visualization update rate"},
        };
        
        return profile;
    }
    
    PresetProfile createResearchProfile() {
        PresetProfile profile;
        profile.name = "research";
        profile.type = PresetProfile::ProfileType::Research;
        profile.description = "Research-grade configuration for scientific studies";
        profile.useCase = "Controlled experiments with full parameter control and reproducibility";
        
        profile.entries = {
            {"random_seed", 2024, ConfigSource::Default, "Research-grade random seed (year-based)"},
            {"simulation_timestep", 0.0001, ConfigSource::Default, "High-precision simulation timestep for research"},
            {"neuron_count", 5000, ConfigSource::Default, "Large-scale neural population for research"},
            {"region_count", 10, ConfigSource::Default, "Multiple regions for complex network studies"},
            {"connection_probability", 0.15, ConfigSource::Default, "Research-optimized connectivity density"},
            {"plasticity_learning_rate", 0.015, ConfigSource::Default, "Balanced plasticity rate for research"},
            {"stdp_ltp_weight", 0.015, ConfigSource::Default, "Research-standard STDP weights"},
            {"stdp_ltd_weight", 0.018, ConfigSource::Default, "Research-standard STDP weights"},
            {"stdp_time_constant", 15.0, ConfigSource::Default, "Research-optimized STDP time constant"},
            {"development_synaptogenesis_rate", 0.001, ConfigSource::Default, "Controlled synaptogenesis for research"},
            {"development_pruning_rate", 0.00005, ConfigSource::Default, "Research-based pruning rate"},
            {"dopamine_baseline", 0.05, ConfigSource::Default, "Controlled neuromodulation baseline"},
            {"reward_discount_factor", 0.995, ConfigSource::Default, "Research-appropriate reward discounting"},
            {"environment_name", std::string("ComplexEnvironment"), ConfigSource::Default, "Research-grade environment"},
            {"log_level", std::string("TRACE"), ConfigSource::Default, "Detailed logging for research analysis"},
            {"log_to_file", true, ConfigSource::Default, "Complete research logging"},
            {"log_filename", std::string("nlm_research_$(date + %Y%m%d_%H%M%S).log"), ConfigSource::Default, "Timestamped research logs"},
            {"max_simulation_steps", 100000, ConfigSource::Default, "Extended research simulations"},
            {"simulation_time_limit", 3600.0, ConfigSource::Default, "1-hour time limit for research"},
            {"visualization_enabled", true, ConfigSource::Default, "Advanced visualization for research"},
            {"visualization_update_rate", 60.0, ConfigSource::Default, "High-frequency research visualization"},
            {"checkpoint_interval", 5000, ConfigSource::Default, "Regular checkpointing for research"},
            {"replay_interval", 1000, ConfigSource::Default, "Memory replay for research"},
            {"enable_development", true, ConfigSource::Default, "Full developmental stages for research"},
            {"enable_curiosity", true, ConfigSource::Default, "Curiosity-driven exploration for research"},
            {"enable_novelty", true, ConfigSource::Default, "Novelty detection for research"},
        };
        
        return profile;
    }
    
    std::unordered_map<std::string, PresetProfile> initializePresetProfiles() {
        std::unordered_map<std::string, PresetProfile> presets;
        presets["basic"] = createBasicProfile();
        presets["advanced"] = createAdvancedProfile();
        presets["research"] = createResearchProfile();
        return presets;
    }
    
    const std::unordered_map<std::string, PresetProfile>& getPresetProfiles() {
        static const std::unordered_map<std::string, PresetProfile> presets = initializePresetProfiles();
        return presets;
    }
}

std::vector<std::string> Config::getAvailablePresets() {
    std::vector<std::string> presets;
    const auto& profiles = getPresetProfiles();
    for (const auto& pair : profiles) {
        presets.push_back(pair.first);
    }
    return presets;
}

PresetProfile Config::loadPreset(const std::string& presetName) {
    const auto& profiles = getPresetProfiles();
    auto it = profiles.find(presetName);
    if (it != profiles.end()) {
        return it->second;
    }
    
    // Return empty profile if not found
    PresetProfile emptyProfile;
    emptyProfile.name = presetName;
    emptyProfile.type = PresetProfile::ProfileType::Basic;
    return emptyProfile;
}

bool Config::applyPreset(const std::string& presetName) {
    PresetProfile preset = loadPreset(presetName);
    if (preset.name.empty()) {
        pImpl->lastError = "Preset not found: " + presetName;
        return false;
    }
    
    clear();
    for (const auto& entry : preset.entries) {
        set(entry.key, entry.value, ConfigSource::Default);
    }
    
    return true;
}

bool Config::parseJSON(const std::string& json, std::vector<ConfigEntry>& entries) {
    try {
        auto j = nlohmann::json::parse(json);
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Extract description from JSON comment if available
            std::string description = "";
            if (j.contains(key + "_description")) {
                description = j[key + "_description"].get<std::string>();
            }
            
            // Convert JSON value to ConfigValue
            ConfigValue value;
            const auto& jsonVal = *it;
            
            if (jsonVal.is_string()) {
                value = jsonVal.get<std::string>();
            } else if (jsonVal.is_number_integer()) {
                value = jsonVal.get<int64_t>();
            } else if (jsonVal.is_number_float()) {
                value = jsonVal.get<double>();
            } else if (jsonVal.is_boolean()) {
                value = jsonVal.get<bool>();
            } else if (jsonVal.is_array()) {
                // Convert array to vector based on element types
                std::vector<double> doubleArray;
                bool allNumbers = true;
                for (const auto& elem : jsonVal) {
                    if (elem.is_number()) {
                        doubleArray.push_back(elem.get<double>());
                    } else {
                        allNumbers = false;
                        break;
                    }
                }
                if (allNumbers && !doubleArray.empty()) {
                    // Check if it's int array
                    bool allInts = true;
                    for (double d : doubleArray) {
                        if (d != static_cast<int>(d)) {
                            allInts = false;
                            break;
                        }
                    }
                    if (allInts) {
                        std::vector<int> intArray;
                        for (double d : doubleArray) {
                            intArray.push_back(static_cast<int>(d));
                        }
                        value = intArray;
                    } else {
                        value = doubleArray;
                    }
                } else {
                    // Try string array
                    std::vector<std::string> stringArray;
                    for (const auto& elem : jsonVal) {
                        if (elem.is_string()) {
                            stringArray.push_back(elem.get<std::string>());
                        } else {
                            stringArray.clear();
                            break;
                        }
                    }
                    if (!stringArray.empty()) {
                        value = stringArray;
                    } else {
                        pImpl->lastError = "Unsupported JSON array type for key: " + key;
                        return false;
                    }
                }
            } else {
                pImpl->lastError = "Unsupported JSON value type for key: " + key;
                return false;
            }
            
            entries.emplace_back(key, value, ConfigSource::File, description);
        }
        return true;
    } catch (const nlohmann::json::exception& e) {
        pImpl->lastError = "JSON parsing error: " + std::string(e.what());
        return false;
    }
}

bool Config::parseYAML(const std::string& yaml, std::vector<ConfigEntry>& entries) {
    try {
        auto doc = YAML::Load(yaml);
        
        for (const auto& pair : doc) {
            std::string key = pair.first.as<std::string>();
            
            // Extract description from YAML comment if available
            std::string description = "";
            if (doc[key + "_description"]) {
                description = doc[key + "_description"].as<std::string>();
            }
            
            // Convert YAML value to ConfigValue
            ConfigValue value;
            const auto& yamlVal = pair.second;
            
            if (yamlVal.Type() == YAML::NodeType::Scalar) {
                // Try different types
                try {
                    value = yamlVal.as<bool>();
                } catch (...) {
                    try {
                        value = yamlVal.as<int>();
                    } catch (...) {
                        try {
                            value = yamlVal.as<double>();
                        } catch (...) {
                            value = yamlVal.as<std::string>();
                        }
                    }
                }
            } else if (yamlVal.Type() == YAML::NodeType::Sequence) {
                // Try to determine array type
                std::vector<int> intArray;
                bool allInts = true;
                for (const auto& elem : yamlVal) {
                    try {
                        intArray.push_back(elem.as<int>());
                    } catch (...) {
                        allInts = false;
                        break;
                    }
                }
                if (allInts && !intArray.empty()) {
                    value = intArray;
                } else {
                    std::vector<double> doubleArray;
                    bool allDoubles = true;
                    for (const auto& elem : yamlVal) {
                        try {
                            doubleArray.push_back(elem.as<double>());
                        } catch (...) {
                            allDoubles = false;
                            break;
                        }
                    }
                    if (allDoubles && !doubleArray.empty()) {
                        value = doubleArray;
                    } else {
                        std::vector<std::string> stringArray;
                        bool allStrings = true;
                        for (const auto& elem : yamlVal) {
                            try {
                                stringArray.push_back(elem.as<std::string>());
                            } catch (...) {
                                allStrings = false;
                                break;
                            }
                        }
                        if (allStrings && !stringArray.empty()) {
                            value = stringArray;
                        } else {
                            pImpl->lastError = "Unsupported YAML array type for key: " + key;
                            return false;
                        }
                    }
                }
            } else {
                pImpl->lastError = "Unsupported YAML value type for key: " + key;
                return false;
            }
            
            entries.emplace_back(key, value, ConfigSource::File, description);
        }
        return true;
    } catch (const YAML::Exception& e) {
        pImpl->lastError = "YAML parsing error: " + std::string(e.what());
        return false;
    }
}

void Config::addValidationRule(const ValidationRule& rule) {
    pImpl->validationRules.push_back(rule);
}

void Config::clearValidationRules() {
    pImpl->validationRules.clear();
    pImpl->validationErrors.clear();
}

template<typename T>
void Config::setValidated(const std::string& key, const T& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
    if (pImpl->options.validateOnSet) {
        validate();
    }
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
    pImpl->validationErrors.clear();
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
    pImpl->validationErrors.clear();
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
    
    if (pImpl->options.validateOnSet) {
        validate();
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

void Config::validate() const {
    pImpl->validationErrors.clear();
    
    for (const auto& rule : pImpl->validationRules) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&rule](const ConfigEntry& e) { return e.key == rule.key; });
        
        if (it != pImpl->entries.end()) {
            if (!rule.validator(it->value)) {
                pImpl->validationErrors.push_back(rule.errorMessage);
            }
        }
    }
}

bool Config::isValid() const {
    validate();
    return pImpl->validationErrors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    validate();
    return pImpl->validationErrors;
}

void Config::setOptions(const ConfigOptions& options) {
    pImpl->options = options;
}

ConfigOptions Config::getOptions() const {
    return pImpl->options;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "Validation: " << (isValid() ? "PASSED" : "FAILED") << "\n";
    oss << "Preset Profiles Available: " << getAvailablePresets().size() << "\n";
    oss << "File Format: " << static_cast<int>(pImpl->options.preferredFormat) << "\n";
    
    if (!pImpl->validationErrors.empty()) {
        oss << "\nValidation Errors:";
        for (const auto& error : pImpl->validationErrors) {
            oss << "\n  - " << error;
        }
    }
    
    oss << "\n\nEntries:";
    for (const auto& entry : pImpl->entries) {
        oss << "\n  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ") - " << entry.description;
    }
    
    return oss.str();
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

ConfigSource Config::getEntrySource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    
    return ConfigSource::Default;
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

std::string Config::configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
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
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

ConfigValue Config::stringToConfigValue(const std::string& str) {
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    try {
        if (str.find('.') != std::string::npos) {
            return std::stod(str);
        } else {
            return std::stoll(str);
        }
    } catch (...) {
        // Try to determine if it's an array format
        if (str.size() >= 2 && str.front() == '[' && str.back() == ']') {
            std::string content = str.substr(1, str.size() - 2);
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream stream(content);
            
            for (char c : content) {
                if (c == ',' && !token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                } else {
                    token.push_back(c);
                }
            }
            if (!token.empty()) {
                tokens.push_back(token);
            }
            
            // Try to parse as int array
            std::vector<int> intArray;
            bool allInts = true;
            for (const auto& token : tokens) {
                try {
                    intArray.push_back(std::stoi(trim(token)));
                } catch (...) {
                    allInts = false;
                    break;
                }
            }
            if (allInts && !intArray.empty()) {
                return intArray;
            }
            
            // Try to parse as double array
            std::vector<double> doubleArray;
            bool allDoubles = true;
            for (const auto& token : tokens) {
                try {
                    doubleArray.push_back(std::stod(trim(token)));
                } catch (...) {
                    allDoubles = false;
                    break;
                }
            }
            if (allDoubles && !doubleArray.empty()) {
                return doubleArray;
            }
            
            // Try to parse as string array
            std::vector<std::string> stringArray;
            for (const auto& token : tokens) {
                stringArray.push_back(trim(token));
            }
            if (!stringArray.empty()) {
                return stringArray;
            }
        }
        
        return str;
    }
}

} // namespace nlm

// Explicit template instantiations
template std::optional<int> nlm::Config::get<int>(const std::string&) const;
template std::optional<int64_t> nlm::Config::get<int64_t>(const std::string&) const;
template std::optional<double> nlm::Config::get<double>(const std::string&) const;
template std::optional<bool> nlm::Config::get<bool>(const std::string&) const;
template std::optional<std::string> nlm::Config::get<std::string>(const std::string&) const;

template int nlm::Config::getOr<int>(const std::string&, const int&) const;
template int64_t nlm::Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double nlm::Config::getOr<double>(const std::string&, const double&) const;
template bool nlm::Config::getOr<bool>(const std::string&, const bool&) const;
template std::string nlm::Config::getOr<std::string>(const std::string&, const std::string&) const;
