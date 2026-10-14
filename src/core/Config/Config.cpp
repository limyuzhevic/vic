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

// Add includes for file parsing
#include <cctype>
#include <algorithm>
#include <map>
#include <sstream>
#include <fstream>
#include <regex>

// Simple JSON parser (basic support)
bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Simple JSON parsing - looks for "key": "value" patterns
    std::regex jsonPattern(R"("([^"]*)"\s*:\s*("[^"]*"|[^,}]+))");
    std::smatch match;
    
    std::string::const_iterator searchStart(content.cbegin());
    while (std::regex_search(searchStart, content.cend(), match, jsonPattern)) {
        if (match.size() >= 3) {
            std::string key = trim(match[1].str());
            std::string valueStr = trim(match[2].str());
            
            // Remove quotes from value
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            set(key, valueStr, ConfigSource::File);
        }
        
        searchStart = match.suffix().first;
    }
    
    return true;
}

// Simple YAML parser (basic support)
bool Config::loadFromYAMLFile(const std::string& filepath) {
    return loadFromKeyValueFile(filepath); // For now, use the same parser
}

// Enhanced key=value file parser with better error handling
bool Config::loadFromKeyValueFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with improved error handling
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            // Warning: invalid line format
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            // Warning: empty key
            continue;
        }
        
        // Parse value to appropriate type
        ConfigValue parsedValue = parseValue(value);
        set(key, parsedValue, ConfigSource::File);
    }
    
    return success;
}

// Parse string value to appropriate ConfigValue type
ConfigValue Config::parseValue(const std::string& valueStr) {
    std::string trimmed = trim(valueStr);
    
    // Check for boolean values
    if (trimmed == "true" || trimmed == "false") {
        return trimmed == "true";
    }
    
    // Check for integer values
    try {
        if (trimmed.find('.') == std::string::npos && trimmed.find('e') == std::string::npos) {
            size_t pos;
            int intVal = std::stoi(trimmed, &pos);
            if (pos == trimmed.size()) {
                return intVal;
            }
        }
    } catch (...) {}
    
    // Check for double values
    try {
        size_t pos;
        double doubleVal = std::stod(trimmed, &pos);
        if (pos == trimmed.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Return as string (default)
    return trimmed;
}

// Enhanced Config methods implementation

const ConfigEntry* Config::getEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return &*it;
    }
    return nullptr;
}

void Config::setWithCategory(const std::string& key, const ConfigValue& value, 
                            ConfigSource source, ConfigCategory category, 
                            const std::string& description) {
    std::string keyWithCategory = "[" + std::to_string(static_cast<int>(category)) + "]" + key;
    set(keyWithCategory, value, source);
    
    // Could store category separately if needed
    // For now, it's encoded in the key name
}

bool Config::setValidated(const std::string& key, const ConfigValue& value, 
                         ConfigSource source, const std::string& validator) {
    if (!validateKey(key)) {
        return false;
    }
    
    // Simple validation examples
    if (validator == "positive_int") {
        if (std::holds_alternative<int>(value)) {
            if (std::get<int>(value) <= 0) {
                return false;
            }
        } else if (std::holds_alternative<int64_t>(value)) {
            if (std::get<int64_t>(value) <= 0) {
                return false;
            }
        } else {
            return false;
        }
    }
    
    set(key, value, source);
    return true;
}

std::vector<ConfigEntry> Config::getByCategory(ConfigCategory category) const {
    std::vector<ConfigEntry> result;
    std::string categoryPrefix = "[" + std::to_string(static_cast<int>(category)) + "]";
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(categoryPrefix) == 0) {
            result.push_back(entry);
        }
    }
    
    return result;
}

std::vector<std::string> Config::validateAll() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        // Basic validation examples
        if (entry.key.find("neuron_count") != std::string::npos) {
            if (std::holds_alternative<int>(entry.value)) {
                if (std::get<int>(entry.value) <= 0) {
                    errors.push_back("neuron_count must be positive, got: " + std::to_string(std::get<int>(entry.value)));
                }
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                if (std::get<int64_t>(entry.value) <= 0) {
                    errors.push_back("neuron_count must be positive, got: " + std::to_string(std::get<int64_t>(entry.value)));
                }
            }
        }
    }
    
    return errors;
}

void Config::applyDefaults(int phase) {
    // Clear current configuration
    clear();
    
    // Apply default values based on phase
    switch (phase) {
        case 1:
            // Phase 1: Skeleton
            set("neuron_count", 1000, ConfigSource::Default);
            set("region_count", 2, ConfigSource::Default);
            set("connection_probability", 0.1, ConfigSource::Default);
            break;
            
        case 2:
            // Phase 2: Real Neural Computation
            set("neuron_count", 500, ConfigSource::Default);
            set("region_count", 1, ConfigSource::Default);
            set("connection_probability", 0.15, ConfigSource::Default);
            set("stdp_ltp_weight", 0.02, ConfigSource::Default);
            set("stdp_ltd_weight", 0.015, ConfigSource::Default);
            set("stdp_time_constant", 20.0, ConfigSource::Default);
            break;
            
        case 3:
            // Phase 3: Integration
            set("neuron_count", 2000, ConfigSource::Default);
            set("region_count", 4, ConfigSource::Default);
            set("connection_probability", 0.2, ConfigSource::Default);
            set("development_synaptogenesis_rate", 0.001, ConfigSource::Default);
            set("development_pruning_rate", 0.0001, ConfigSource::Default);
            set("dopamine_baseline", 0.1, ConfigSource::Default);
            break;
            
        case 4:
            // Phase 4: Advanced Cognition
            set("neuron_count", 5000, ConfigSource::Default);
            set("region_count", 8, ConfigSource::Default);
            set("connection_probability", 0.25, ConfigSource::Default);
            set("prediction_time_constant", 0.1, ConfigSource::Default);
            set("concept_formulation_threshold", 0.7, ConfigSource::Default);
            break;
            
        case 5:
            // Phase 5: Performance
            set("neuron_count", 10000, ConfigSource::Default);
            set("region_count", 16, ConfigSource::Default);
            set("connection_probability", 0.3, ConfigSource::Default);
            set("simd_enabled", true, ConfigSource::Default);
            set("parallel_enabled", true, ConfigSource::Default);
            break;
            
        case 6:
            // Phase 6: Final Integration
            set("neuron_count", 20000, ConfigSource::Default);
            set("region_count", 32, ConfigSource::Default);
            set("connection_probability", 0.35, ConfigSource::Default);
            set("development_critical_period_multiplier", 1.5, ConfigSource::Default);
            set("neuromodulation_strength", 1.0, ConfigSource::Default);
            break;
            
        default:
            // Phase 1 defaults
            set("neuron_count", 1000, ConfigSource::Default);
            set("region_count", 2, ConfigSource::Default);
            set("connection_probability", 0.1, ConfigSource::Default);
            break;
    }
}

std::string Config::getFileContent(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "File not found: " + filepath;
    }
    
    std::stringstream content;
    content << file.rdbuf();
    return content.str();
}

bool Config::isFileFormatSupported(const std::string& filepath) {
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    }
    
    static const std::vector<std::string> supportedExtensions = {
        ".cfg", ".txt", ".json", ".jsonl", ".yaml", ".yml"
    };
    
    return std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.end();
}

std::vector<std::string> Config::getSupportedFileExtensions() {
    return {
        ".cfg", ".txt", ".json", ".jsonl", ".yaml", ".yml"
    };
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Override with same source or lower priority
        if (source == ConfigSource::Runtime || entry.source == ConfigSource::Runtime) {
            set(entry.key, entry.value, source);
        }
    }
}

std::shared_ptr<Config> Config::clone() const {
    auto cloned = std::make_shared<Config>();
    cloned->pImpl->entries = pImpl->entries;
    return cloned;
}

bool Config::validateKey(const std::string& key) {
    // Basic key validation
    if (key.empty() || key.find(' ') != std::string::npos) {
        return false;
    }
    
    // Check for valid characters
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
            return false;
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

// Helper functions for ConfigValue conversion
std::string Config::valueToString(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else {
            return std::to_string(arg);
        }
    }, value);
}

std::string Config::valueToString(const std::vector<int>& values) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << values[i];
    }
    ss << "]";
    return ss.str();
}

std::string Config::valueToString(const std::vector<double>& values) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << values[i];
    }
    ss << "]";
    return ss.str();
}

std::string Config::valueToString(const std::vector<std::string>& values) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << "\"" << values[i] << "\"";
    }
    ss << "]";
    return ss.str();
}

// Configuration methods implementation

const ConfigEntry* Config::getEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return &*it;
    }
    return nullptr;
}

void Config::setWithCategory(const std::string& key, const ConfigValue& value, 
                            ConfigSource source, ConfigCategory category, 
                            const std::string& description) {
    std::string keyWithCategory = "[" + std::to_string(static_cast<int>(category)) + "]" + key;
    set(keyWithCategory, value, source);
    
    // Could store category separately if needed
    // For now, it's encoded in the key name
}

bool Config::setValidated(const std::string& key, const ConfigValue& value, 
                         ConfigSource source, const std::string& validator) {
    if (!validateKey(key)) {
        return false;
    }
    
    // Simple validation examples
    if (validator == "positive_int") {
        if (std::holds_alternative<int>(value)) {
            if (std::get<int>(value) <= 0) {
                return false;
            }
        } else if (std::holds_alternative<int64_t>(value)) {
            if (std::get<int64_t>(value) <= 0) {
                return false;
            }
        } else {
            return false;
        }
    }
    
    set(key, value, source);
    return true;
}

std::vector<ConfigEntry> Config::getByCategory(ConfigCategory category) const {
    std::vector<ConfigEntry> result;
    std::string categoryPrefix = "[" + std::to_string(static_cast<int>(category)) + "]";
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(categoryPrefix) == 0) {
            result.push_back(entry);
        }
    }
    
    return result;
}

std::vector<std::string> Config::validateAll() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        // Basic validation examples
        if (entry.key.find("neuron_count") != std::string::npos) {
            if (std::holds_alternative<int>(entry.value)) {
                if (std::get<int>(entry.value) <= 0) {
                    errors.push_back("neuron_count must be positive, got: " + std::to_string(std::get<int>(entry.value)));
                }
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                if (std::get<int64_t>(entry.value) <= 0) {
                    errors.push_back("neuron_count must be positive, got: " + std::to_string(std::get<int64_t>(entry.value)));
                }
            }
        }
    }
    
    return errors;
}

void Config::applyAdvancedConfig(const AdvancedConfig& advConfig) {
    clear();
    
    // Apply advanced configuration values
    set("stdp_ltp_weight", advConfig.stdp_ltp_weight, ConfigSource::Default);
    set("stdp_ltd_weight", advConfig.stdp_ltd_weight, ConfigSource::Default);
    set("stdp_time_constant", advConfig.stdp_time_constant, ConfigSource::Default);
    set("hebbian_learning_rate", advConfig.hebbian_learning_rate, ConfigSource::Default);
    set("reward_modulation_strength", advConfig.reward_modulation_strength, ConfigSource::Default);
    
    set("synaptogenesis_rate", advConfig.synaptogenesis_rate, ConfigSource::Default);
    set("pruning_rate", advConfig.pruning_rate, ConfigSource::Default);
    set("critical_period_multiplier", advConfig.critical_period_multiplier, ConfigSource::Default);
    set("maturation_rate", advConfig.maturation_rate, ConfigSource::Default);
    
    set("dopamine_baseline", advConfig.dopamine_baseline, ConfigSource::Default);
    set("curiosity_strength", advConfig.curiosity_strength, ConfigSource::Default);
    set("novelty_threshold", advConfig.novelty_threshold, ConfigSource::Default);
    set("prediction_error_decay", advConfig.prediction_error_decay, ConfigSource::Default);
    
    set("working_memory_capacity", static_cast<int64_t>(advConfig.working_memory_capacity), ConfigSource::Default);
    set("episodic_memory_capacity", static_cast<int64_t>(advConfig.episodic_memory_capacity), ConfigSource::Default);
    set("associative_memory_patterns", static_cast<int64_t>(advConfig.associative_memory_patterns), ConfigSource::Default);
    
    set("prediction_time_constant", advConfig.prediction_time_constant, ConfigSource::Default);
    set("confidence_threshold", advConfig.confidence_threshold, ConfigSource::Default);
    
    set("simd_enabled", advConfig.simd_enabled, ConfigSource::Default);
    set("parallel_enabled", advConfig.parallel_enabled, ConfigSource::Default);
    set("vectorization_width", static_cast<int64_t>(advConfig.vectorization_width), ConfigSource::Default);
    
    set("visualization_enabled", advConfig.visualization_enabled, ConfigSource::Default);
    set("visualization_update_rate", advConfig.visualization_update_rate, ConfigSource::Default);
    
    set("log_level", advConfig.log_level, ConfigSource::Default);
    set("log_to_file", advConfig.log_to_file, ConfigSource::Default);
    set("log_filename", advConfig.log_filename, ConfigSource::Default);
    
    set("environment_name", advConfig.environment_name, ConfigSource::Default);
    set("environment_width", static_cast<int64_t>(advConfig.environment_width), ConfigSource::Default);
    set("environment_height", static_cast<int64_t>(advConfig.environment_height), ConfigSource::Default);
    
    set("max_simulation_steps", static_cast<int64_t>(advConfig.max_simulation_steps), ConfigSource::Default);
    set("simulation_time_limit", advConfig.simulation_time_limit, ConfigSource::Default);
    set("timestep", advConfig.timestep, ConfigSource::Default);
    
    set("max_energy", advConfig.max_energy, ConfigSource::Default);
    set("energy_decay_rate", advConfig.energy_decay_rate, ConfigSource::Default);
    set("health_degradation_rate", advConfig.health_degradation_rate, ConfigSource::Default);
    
    set("track_development_stages", advConfig.track_development_stages, ConfigSource::Default);
    set("enable_structural_plasticity", advConfig.enable_structural_plasticity, ConfigSource::Default);
    set("enable_curiosity", advConfig.enable_curiosity, ConfigSource::Default);
    set("enable_reward_modulation", advConfig.enable_reward_modulation, ConfigSource::Default);
}

AdvancedConfig Config::getAdvancedConfig() const {
    AdvancedConfig advConfig;
    
    // Extract advanced configuration values
    advConfig.stdp_ltp_weight = getOr<float>("stdp_ltp_weight", 0.02f);
    advConfig.stdp_ltd_weight = getOr<float>("stdp_ltd_weight", 0.015f);
    advConfig.stdp_time_constant = getOr<float>("stdp_time_constant", 20.0f);
    advConfig.hebbian_learning_rate = getOr<float>("hebbian_learning_rate", 0.01f);
    advConfig.reward_modulation_strength = getOr<float>("reward_modulation_strength", 1.0f);
    
    advConfig.synaptogenesis_rate = getOr<float>("synaptogenesis_rate", 0.001f);
    advConfig.pruning_rate = getOr<float>("pruning_rate", 0.0001f);
    advConfig.critical_period_multiplier = getOr<float>("critical_period_multiplier", 1.5f);
    advConfig.maturation_rate = getOr<float>("maturation_rate", 0.1f);
    
    advConfig.dopamine_baseline = getOr<float>("dopamine_baseline", 0.1f);
    advConfig.curiosity_strength = getOr<float>("curiosity_strength", 0.5f);
    advConfig.novelty_threshold = getOr<float>("novelty_threshold", 0.7f);
    advConfig.prediction_error_decay = getOr<float>("prediction_error_decay", 0.95f);
    
    advConfig.working_memory_capacity = static_cast<size_t>(getOr<int64_t>("working_memory_capacity", 1000));
    advConfig.episodic_memory_capacity = static_cast<size_t>(getOr<int64_t>("episodic_memory_capacity", 5000));
    advConfig.associative_memory_patterns = static_cast<size_t>(getOr<int64_t>("associative_memory_patterns", 100));
    
    advConfig.prediction_time_constant = getOr<float>("prediction_time_constant", 0.1f);
    advConfig.confidence_threshold = getOr<float>("confidence_threshold", 0.8f);
    
    advConfig.simd_enabled = getOr<bool>("simd_enabled", true);
    advConfig.parallel_enabled = getOr<bool>("parallel_enabled", true);
    advConfig.vectorization_width = static_cast<size_t>(getOr<int64_t>("vectorization_width", 4));
    
    advConfig.visualization_enabled = getOr<bool>("visualization_enabled", false);
    advConfig.visualization_update_rate = getOr<float>("visualization_update_rate", 30.0f);
    
    advConfig.log_level = getOr<std::string>("log_level", "INFO");
    advConfig.log_to_file = getOr<bool>("log_to_file", false);
    advConfig.log_filename = getOr<std::string>("log_filename", "nlm.log");
    
    advConfig.environment_name = getOr<std::string>("environment_name", "GridWorld");
    advConfig.environment_width = static_cast<size_t>(getOr<int64_t>("environment_width", 10));
    advConfig.environment_height = static_cast<size_t>(getOr<int64_t>("environment_height", 10));
    
    advConfig.max_simulation_steps = static_cast<size_t>(getOr<int64_t>("max_simulation_steps", 10000));
    advConfig.simulation_time_limit = getOr<double>("simulation_time_limit", 0.0);
    advConfig.timestep = getOr<double>("timestep", 0.001);
    
    advConfig.max_energy = getOr<float>("max_energy", 100.0f);
    advConfig.energy_decay_rate = getOr<float>("energy_decay_rate", 0.01f);
    advConfig.health_degradation_rate = getOr<float>("health_degradation_rate", 0.001f);
    
    advConfig.track_development_stages = getOr<bool>("track_development_stages", true);
    advConfig.enable_structural_plasticity = getOr<bool>("enable_structural_plasticity", true);
    advConfig.enable_curiosity = getOr<bool>("enable_curiosity", true);
    advConfig.enable_reward_modulation = getOr<bool>("enable_reward_modulation", true);
    
    return advConfig;
}

ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    
    for (const auto& entry : pImpl->entries) {
        int sourceIndex = static_cast<int>(entry.source);
        if (sourceIndex >= 0 && sourceIndex < 4) {
            stats.entriesBySource[sourceIndex]++;
        }
    }
    
    // Find most used key (simplified)
    if (!pImpl->entries.empty()) {
        stats.mostUsedKey = pImpl->entries[0].key;
    }
    
    return stats;
}

std::vector<ConfigEntry> Config::search(const std::string& pattern) const {
    std::vector<ConfigEntry> results;
    std::string lowerPattern = toLower(pattern);
    
    for (const auto& entry : pImpl->entries) {
        if (toLower(entry.key).find(lowerPattern) != std::string::npos) {
            results.push_back(entry);
        }
    }
    
    return results;
}

std::vector<ConfigEntry> Config::getBySource(ConfigSource source) const {
    std::vector<ConfigEntry> results;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            results.push_back(entry);
        }
    }
    
    return results;
}

bool Config::hasAny(const std::vector<std::string>& keys) const {
    for (const auto& key : keys) {
        if (has(key)) {
            return true;
        }
    }
    return false;
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto otherEntry = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                                       [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (otherEntry == other.pImpl->entries.end() || otherEntry->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

double Config::similarity(const Config& other) const {
    if (pImpl->entries.empty() && other.pImpl->entries.empty()) {
        return 1.0;
    }
    
    size_t matches = 0;
    size_t total = std::max(pImpl->entries.size(), other.pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        auto otherEntry = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                                       [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (otherEntry != other.pImpl->entries.end() && otherEntry->value == entry.value) {
            matches++;
        }
    }
    
    return static_cast<double>(matches) / total;
}

std::vector<std::string> Config::suggestOptimizations() const {
    std::vector<std::string> suggestions;
    
    // Check for obviously suboptimal values
    auto neuronCount = getOr<int64_t>("neuron_count", 0);
    if (neuronCount > 50000) {
        suggestions.push_back("Consider reducing neuron_count from " + std::to_string(neuronCount) + " to under 50000 for better performance");
    }
    
    auto connectionProb = getOr<double>("connection_probability", 0.0);
    if (connectionProb > 0.5) {
        suggestions.push_back("connection_probability of " + std::to_string(connectionProb) + " is very high, consider reducing for better efficiency");
    }
    
    return suggestions;
}

std::string Config::generateOptimalConfig(int phase) const {
    std::stringstream ss;
    ss << "# Optimal configuration for Phase " << phase << "\n";
    
    // Generate optimal values based on phase
    switch (phase) {
        case 1:
            ss << "neuron_count = 1000\n";
            ss << "region_count = 2\n";
            ss << "connection_probability = 0.1\n";
            break;
            
        case 2:
            ss << "neuron_count = 500\n";
            ss << "region_count = 1\n";
            ss << "connection_probability = 0.15\n";
            ss << "stdp_ltp_weight = 0.02\n";
            ss << "stdp_ltd_weight = 0.015\n";
            ss << "stdp_time_constant = 20.0\n";
            break;
            
        case 3:
            ss << "neuron_count = 2000\n";
            ss << "region_count = 4\n";
            ss << "connection_probability = 0.2\n";
            ss << "development_synaptogenesis_rate = 0.001\n";
            ss << "development_pruning_rate = 0.0001\n";
            ss << "dopamine_baseline = 0.1\n";
            break;
            
        case 4:
            ss << "neuron_count = 5000\n";
            ss << "region_count = 8\n";
            ss << "connection_probability = 0.25\n";
            ss << "prediction_time_constant = 0.1\n";
            ss << "confidence_threshold = 0.8\n";
            break;
            
        case 5:
            ss << "neuron_count = 10000\n";
            ss << "region_count = 16\n";
            ss << "connection_probability = 0.3\n";
            ss << "simd_enabled = true\n";
            ss << "parallel_enabled = true\n";
            break;
            
        case 6:
            ss << "neuron_count = 20000\n";
            ss << "region_count = 32\n";
            ss << "connection_probability = 0.35\n";
            ss << "development_critical_period_multiplier = 1.5\n";
            ss << "neuromodulation_strength = 1.0\n";
            break;
            
        default:
            ss << "neuron_count = 1000\n";
            ss << "region_count = 2\n";
            ss << "connection_probability = 0.1\n";
            break;
    }
    
    return ss.str();
}

template<typename T>
T Config::getSafe(const std::string& key, const T& defaultValue, 
                 const std::string& validator) const {
    if (!validator.empty()) {
        // Simple validation could be added here
    }
    return getOr<T>(key, defaultValue);
}

std::vector<std::pair<std::string, std::pair<ConfigValue, ConfigValue>>> 
Config::diff(const Config& other) const {
    std::vector<std::pair<std::string, std::pair<ConfigValue, ConfigValue>>> differences;
    
    // Check for differences in this config vs other
    for (const auto& entry : pImpl->entries) {
        auto otherEntry = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                                       [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (otherEntry != other.pImpl->entries.end() && otherEntry->value != entry.value) {
            differences.push_back({entry.key, {entry.value, otherEntry->value}});
        }
    }
    
    // Check for entries in other that aren't in this
    for (const auto& entry : other.pImpl->entries) {
        if (!has(entry.key)) {
            differences.push_back({entry.key, {ConfigValue(), entry.value}});
        }
    }
    
    return differences;
}

std::string Config::exportToJSON() const {
    std::stringstream ss;
    ss << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        ss << "  \"" << entry.key << "\": ";
        
        ss << valueToString(entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            ss << ",\n";
        } else {
            ss << "\n";
        }
    }
    
    ss << "}\n";
    return ss.str();
}

std::string Config::exportToYAML() const {
    std::stringstream ss;
    
    for (const auto& entry : pImpl->entries) {
        ss << entry.key << ": " << valueToString(entry.value) << "\n";
    }
    
    return ss.str();
}

bool Config::importFromJSON(const std::string& json) {
    // Simple JSON parsing - looks for "key": "value" patterns
    std::regex jsonPattern(R"("([^"]*)"\s*:\s*("[^"]*"|[^,}]+))");
    std::smatch match;
    
    std::string::const_iterator searchStart(json.cbegin());
    while (std::regex_search(searchStart, json.cend(), match, jsonPattern)) {
        if (match.size() >= 3) {
            std::string key = trim(match[1].str());
            std::string valueStr = trim(match[2].str());
            
            // Remove quotes from value
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            set(key, valueStr, ConfigSource::File);
        }
        
        searchStart = match.suffix().first;
    }
    
    return true;
}

bool Config::importFromYAML(const std::string& yaml) {
    return loadFromKeyValueFile("") || importFromJSON(yaml);
}

bool Config::isValid() const {
    return validateAll().empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    return validateAll();
}

void Config::addValidationRule(const std::string& key, const std::string& validator) {
    // Validation rules could be stored and applied during validation
    // For now, this is a placeholder for future validation framework
}

// File format parsers implementation

bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Simple JSON parsing - looks for "key": "value" patterns
    std::regex jsonPattern(R"("([^"]*)"\s*:\s*("[^"]*"|[^,}]+))");
    std::smatch match;
    
    std::string::const_iterator searchStart(content.cbegin());
    while (std::regex_search(searchStart, content.cend(), match, jsonPattern)) {
        if (match.size() >= 3) {
            std::string key = trim(match[1].str());
            std::string valueStr = trim(match[2].str());
            
            // Remove quotes from value
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            set(key, valueStr, ConfigSource::File);
        }
        
        searchStart = match.suffix().first;
    }
    
    return true;
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    return loadFromKeyValueFile(filepath);
}

bool Config::loadFromKeyValueFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with improved error handling
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            // Warning: invalid line format
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            // Warning: empty key
            continue;
        }
        
        // Parse value to appropriate type
        ConfigValue parsedValue = parseValue(value);
        set(key, parsedValue, ConfigSource::File);
    }
    
    return success;
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

// Static method implementations

std::map<int, std::vector<std::string>> Config::getPhaseConfigKeys() {
    return {
        {1, {"neuron_count", "region_count", "connection_probability"}},
        {2, {"neuron_count", "region_count", "connection_probability", "stdp_ltp_weight", "stdp_ltd_weight"}},
        {3, {"neuron_count", "region_count", "connection_probability", "development_synaptogenesis_rate"}},
        {4, {"neuron_count", "region_count", "connection_probability", "prediction_time_constant"}},
        {5, {"neuron_count", "region_count", "connection_probability", "simd_enabled"}},
        {6, {"neuron_count", "region_count", "connection_probability", "development_critical_period_multiplier"}}
    };
}

std::string Config::getPhaseDocumentation(int phase) {
    switch (phase) {
        case 1:
            return "Phase 1: Project skeleton with core types and configuration";
        case 2:
            return "Phase 2: Real neural computation with LIF neurons, STDP, and Hebbian plasticity";
        case 3:
            return "Phase 3: Integration with world interaction and basic learning";
        case 4:
            return "Phase 4: Advanced cognitive systems including prediction and planning";
        case 5:
            return "Phase 5: Performance optimizations including SIMD and parallel processing";
        case 6:
            return "Phase 6: Final integration of all systems into a coherent artificial brain";
        default:
            return "Unknown phase";
    }
}

} // namespace nlm
