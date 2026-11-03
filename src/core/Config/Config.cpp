#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <ctime>
#include <cmath>
#include <vector>
#include <map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Forward declarations for JSON parsing
static bool parseJSONFromContent(const std::string& content);
static std::string generateSimpleJSON(const std::vector<ConfigEntry>& entries);
static std::string configValueToString(const ConfigValue& value);

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try to parse as JSON first
    // Simple JSON detection: if file starts with { and contains "key": "value" format
    bool isJSON = false;
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        // Check for JSON-like structure with quotes around keys and values
        size_t quote_pos = content.find('"');
        if (quote_pos != std::string::npos && content.find('"', quote_pos + 1) != std::string::npos) {
            isJSON = true;
        }
    }
    
    if (isJSON) {
        // Parse as JSON
        return parseJSONFromContent(content);
    } else {
        // Fall back to simple key=value format (Phase 1 style)
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
    
    // Generate JSON content
    std::string json_content = generateSimpleJSON(pImpl->entries);
    file << json_content;
    
    return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
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

template bool Config::getOr<int>(const std::string&, const int&) const;
template bool Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template bool Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template bool Config::getOr<float>(const std::string&, const float&) const;
template bool Config::getOr<double>(const std::string&, const double&) const;

// Simple JSON parser implementation
bool parseJSONFromContent(const std::string& content) {
    // This is a simplified JSON parser that handles basic JSON structures.
    // For a production system, you would use a proper JSON library like nlohmann/json.
    
    std::string current = content;
    size_t pos = 0;
    
    // Remove whitespace and newlines
    current.erase(std::remove_if(current.begin(), current.end(), 
        [](char c) { return std::isspace(c); }), current.end());
    
    // Check if it's a JSON object
    if (current.empty() || current[0] != '{') {
        return false;
    }
    
    // Parse key-value pairs from {"key1": "value1", "key2": 42, "key3": true, ...}
    size_t start = 1; // Skip opening {
    size_t end = current.find('}');
    if (end == std::string::npos) {
        return false;
    }
    
    std::string pairs = current.substr(start, end - start);
    size_t pair_pos = 0;
    
    while (pair_pos < pairs.length()) {
        // Find next key
        size_t key_start = pairs.find('"', pair_pos);
        if (key_start == std::string::npos) break;
        
        size_t key_end = pairs.find('"', key_start + 1);
        if (key_end == std::string::npos) break;
        
        std::string key = pairs.substr(key_start + 1, key_end - key_start - 1);
        
        // Find colon
        size_t colon_pos = pairs.find(':', key_end);
        if (colon_pos == std::string::npos) break;
        
        size_t value_start = pairs.find_first_not_of(' ', colon_pos + 1);
        if (value_start == std::string::npos) break;
        
        // Parse value based on type
        ConfigValue value;
        
        if (value_start < pairs.length() && pairs[value_start] == '"') {
            // String value
            size_t value_end = pairs.find('"', value_start + 1);
            if (value_end == std::string::npos) break;
            std::string value_str = pairs.substr(value_start + 1, value_end - value_start - 1);
            value = value_str;
        } else if (value_start < pairs.length() && (std::isdigit(pairs[value_start]) || pairs[value_start] == '-' || pairs[value_start] == '+')) {
            // Number value
            size_t value_end = value_start;
            while (value_end < pairs.length() && (std::isdigit(pairs[value_end]) || pairs[value_end] == '.' || pairs[value_end] == '-' || pairs[value_end] == '+')) {
                value_end++;
            }
            std::string value_str = pairs.substr(value_start, value_end - value_start);
            try {
                // Try double first
                size_t decimal_pos = value_str.find('.');
                if (decimal_pos != std::string::npos) {
                    value = std::stod(value_str);
                } else {
                    // Try int64_t
                    try {
                        value = static_cast<int64_t>(std::stoll(value_str));
                    } catch (...) {
                        // Fall back to double
                        value = std::stod(value_str);
                    }
                }
            } catch (...) {
                value = value_str;
            }
        } else if (value_start < pairs.length() && pairs[value_start] == 't') {
            // Boolean true
            if (pairs.substr(value_start, 4) == "true") {
                value = true;
            } else {
                value = pairs.substr(value_start, 4);
            }
        } else if (value_start < pairs.length() && pairs[value_start] == 'f') {
            // Boolean false
            if (pairs.substr(value_start, 5) == "false") {
                value = false;
            } else {
                value = pairs.substr(value_start, 5);
            }
        } else {
            // Unknown type, skip
            size_t next_comma = pairs.find(',', value_start);
            if (next_comma == std::string::npos) break;
            pair_pos = next_comma + 1;
            continue;
        }
        
        // Set the value in config
        set(key, value, ConfigSource::File);
        
        // Move to next pair
        size_t next_comma = pairs.find(',', value_start);
        if (next_comma == std::string::npos) {
            // No more pairs
            break;
        }
        pair_pos = next_comma + 1;
    }
    
    return true;
}

std::string generateSimpleJSON(const std::vector<ConfigEntry>& entries) {
    std::ostringstream json;
    json << "{\n";
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& entry = entries[i];
        json << "  \"" << entry.key << "\": ";
        
        // Handle different value types
        std::visit([&json](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                json << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                json << (arg ? "true" : "false");
            } else if constexpr (std::is_arithmetic_v<T>) {
                json << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                json << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j) json << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        json << "\"" << arg[j] << "\"";
                    } else {
                        json << arg[j];
                    }
                }
                json << "]";
            } else {
                json << "null";
            }
        }, entry.value);
        
        if (i < entries.size() - 1) {
            json << ",\n";
        } else {
            json << "\n";
        }
    }
    json << "}\n";
    return json.str();
}

// Helper function to convert config value to string representation
std::string configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_arithmetic_v<T>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                             std::is_same_v<T, std::vector<double>> ||
                             std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ", ";
                if constexpr (std::is_same_v<T, std::vector<std::string>) {
                    oss << "\"" << arg[i] << "\"";
                } else {
                    oss << arg[i];
                }
            }
            oss << "]";
        } else {
            oss << "null";
        }
    }, value);
    return oss.str();
}

return oss.str();
}

// Advanced configuration methods implementation

void Config::setBrainParameters(int neuronCount, int regionCount) {
    if (neuronCount <= 0 || regionCount <= 0) {
        return; // Invalid parameters
    }
    set("brain.neuron_count", neuronCount, ConfigSource::Runtime);
    set("brain.region_count", regionCount, ConfigSource::Runtime);
}

void Config::setPlasticityParameters(float stdpLTP, float stdpLTD, float stdpTimeConstant) {
    set("plasticity.stdp.ltp_weight", stdpLTP, ConfigSource::Runtime);
    set("plasticity.stdp.ltd_weight", stdpLTD, ConfigSource::Runtime);
    set("plasticity.stdp.time_constant", stdpTimeConstant, ConfigSource::Runtime);
}

void Config::setNeuromodulationParameters(float dopamineScale, float curiosityRate, float noveltyThreshold) {
    set("neuromod.dopamine.scale", dopamineScale, ConfigSource::Runtime);
    set("neuromod.curiosity.rate", curiosityRate, ConfigSource::Runtime);
    set("neuromod.novelty.threshold", noveltyThreshold, ConfigSource::Runtime);
}

void Config::setDevelopmentParameters(bool enableDevelopment, float maturationRate) {
    set("development.enabled", enableDevelopment, ConfigSource::Runtime);
    set("development.maturation_rate", maturationRate, ConfigSource::Runtime);
}

void Config::setPerformanceParameters(int maxSteps, float timestep, bool enableOptimization) {
    set("simulation.max_steps", maxSteps, ConfigSource::Runtime);
    set("simulation.timestep", timestep, ConfigSource::Runtime);
    set("performance.enable_optimization", enableOptimization, ConfigSource::Runtime);
}

void Config::setCustomSynapseParameters(float minWeight, float maxWeight, float initialWeightMean, float initialWeightStd) {
    set("synapse.min_weight", minWeight, ConfigSource::Runtime);
    set("synapse.max_weight", maxWeight, ConfigSource::Runtime);
    set("synapse.initial_weight_mean", initialWeightMean, ConfigSource::Runtime);
    set("synapse.initial_weight_std", initialWeightStd, ConfigSource::Runtime);
}

void Config::setStructuralPlasticityParameters(float synaptogenesisRate, float pruningRate, float connectionProbability) {
    set("structural.synaptogenesis_rate", synaptogenesisRate, ConfigSource::Runtime);
    set("structural.pruning_rate", pruningRate, ConfigSource::Runtime);
    set("structural.connection_probability", connectionProbability, ConfigSource::Runtime);
}

void Config::setMemorySystemParameters(size_t workingMemoryCapacity, size_t episodicMemoryCapacity, size_t associativeMemoryCapacity) {
    set("memory.working_capacity", static_cast<int64_t>(workingMemoryCapacity), ConfigSource::Runtime);
    set("memory.episodic_capacity", static_cast<int64_t>(episodicMemoryCapacity), ConfigSource::Runtime);
    set("memory.associative_capacity", static_cast<int64_t>(associativeMemoryCapacity), ConfigSource::Runtime);
}

void Config::setPredictionSystemParameters(float predictionHorizon, float predictionAccuracyThreshold) {
    set("prediction.horizon", predictionHorizon, ConfigSource::Runtime);
    set("prediction.accuracy_threshold", predictionAccuracyThreshold, ConfigSource::Runtime);
}

void Config::setCognitiveSystemParameters(float attentionStrength, float planningDepth, float conceptFormationRate) {
    set("cognition.attention_strength", attentionStrength, ConfigSource::Runtime);
    set("cognition.planning_depth", planningDepth, ConfigSource::Runtime);
    set("cognition.concept_formation_rate", conceptFormationRate, ConfigSource::Runtime);
}

bool Config::validateConfiguration() const {
    // Basic validation rules
    if (pImpl->entries.empty()) {
        return false; // No configuration
    }
    
    // Check for required brain parameters
    bool hasNeuronCount = has("brain.neuron_count");
    bool hasRegionCount = has("brain.region_count");
    
    if (!hasNeuronCount || !hasRegionCount) {
        return false; // Essential parameters missing
    }
    
    // Validate parameter ranges
    if (has("brain.neuron_count")) {
        int64_t neuronCount = get<int64_t>("brain.neuron_count").value_or(0);
        if (neuronCount <= 0 || neuronCount > 100000) {
            return false; // Invalid neuron count
        }
    }
    
    if (has("brain.region_count")) {
        int64_t regionCount = get<int64_t>("brain.region_count").value_or(0);
        if (regionCount <= 0 || regionCount > 100) {
            return false; // Invalid region count
        }
    }
    
    // Check for consistency
    if (has("brain.neuron_count") && has("brain.region_count")) {
        int64_t neuronCount = get<int64_t>("brain.neuron_count").value_or(0);
        int64_t regionCount = get<int64_t>("brain.region_count").value_or(0);
        if (neuronCount / regionCount < 10) {
            return false; // Too few neurons per region
        }
    }
    
    return true;
}

std::string Config::getConfigurationError() const {
    std::string errors;
    
    if (pImpl->entries.empty()) {
        errors += "No configuration entries found.\n";
    }
    
    // Check for essential parameters
    bool hasNeuronCount = has("brain.neuron_count");
    bool hasRegionCount = has("brain.region_count");
    
    if (!hasNeuronCount) {
        errors += "Missing essential parameter: brain.neuron_count\n";
    }
    
    if (!hasRegionCount) {
        errors += "Missing essential parameter: brain.region_count\n";
    }
    
    // Validate parameter ranges
    if (has("brain.neuron_count")) {
        int64_t neuronCount = get<int64_t>("brain.neuron_count").value_or(0);
        if (neuronCount <= 0) {
            errors += "Invalid brain.neuron_count: must be positive\n";
        } else if (neuronCount > 100000) {
            errors += "Invalid brain.neuron_count: too large (> 100,000)\n";
        }
    }
    
    if (has("brain.region_count")) {
        int64_t regionCount = get<int64_t>("brain.region_count").value_or(0);
        if (regionCount <= 0) {
            errors += "Invalid brain.region_count: must be positive\n";
        } else if (regionCount > 100) {
            errors += "Invalid brain.region_count: too large (> 100)\n";
        }
    }
    
    if (has("brain.neuron_count") && has("brain.region_count")) {
        int64_t neuronCount = get<int64_t>("brain.neuron_count").value_or(0);
        int64_t regionCount = get<int64_t>("brain.region_count").value_or(0);
        if (neuronCount / regionCount < 10) {
            errors += "Configuration inconsistency: insufficient neurons per region (should be at least 10)\n";
        }
    }
    
    return errors.empty() ? "Configuration is valid." : errors;
}

std::string Config::exportConfiguration(const std::string& format) const {
    if (format == "json") {
        return generateSimpleJSON(pImpl->entries);
    } else if (format == "ini") {
        // Generate INI-style format
        std::ostringstream ini;
        ini << "[BRAIN]\n";
        ini << "neuron_count = " << (has("brain.neuron_count") ? get<int64_t>("brain.neuron_count").value_or(0) : 1000) << "\n";
        ini << "region_count = " << (has("brain.region_count") ? get<int64_t>("brain.region_count").value_or(0) : 1) << "\n\n";
        
        ini << "[PLASTICITY]\n";
        ini << "stdp_ltp_weight = " << (has("plasticity.stdp.ltp_weight") ? get<double>("plasticity.stdp.ltp_weight").value_or(0.02) : 0.02) << "\n";
        ini << "stdp_ltd_weight = " << (has("plasticity.stdp.ltd_weight") ? get<double>("plasticity.stdp.ltd_weight").value_or(0.015) : 0.015) << "\n";
        ini << "stdp_time_constant = " << (has("plasticity.stdp.time_constant") ? get<double>("plasticity.stdp.time_constant").value_or(20.0) : 20.0) << "\n\n";
        
        ini << "[NEUROMODULATION]\n";
        ini << "dopamine_scale = " << (has("neuromod.dopamine.scale") ? get<double>("neuromod.dopamine.scale").value_or(1.0) : 1.0) << "\n";
        
        return ini.str();
    } else {
        return summary(); // Default to summary format
    }
}

bool Config::importConfiguration(const std::string& content, const std::string& format) {
    if (format == "json") {
        return parseJSONFromContent(content);
    } else if (format == "ini") {
        // Parse INI format
        clear();
        std::string section;
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty() || line[0] == ';') {
                continue;
            }
            
            if (line[0] == '[' && line[line.size() - 1] == ']') {
                section = line.substr(1, line.size() - 2);
                continue;
            }
            
            size_t equals_pos = line.find('=');
            if (equals_pos == std::string::npos) {
                continue;
            }
            
            std::string key = section + "." + line.substr(0, equals_pos);
            key.erase(0, key.find_first_not_of(" \t"));
            
            std::string value = line.substr(equals_pos + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } else {
        return loadFromFile(content);
    }
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto other_entry = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (other_entry == other.pImpl->entries.end() || other_entry->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

std::string Config::getConfigurationHash() const {
    return generateConfigurationHash(pImpl->entries);
}

bool Config::backupToFile(const std::string& filepath) const {
    return saveToFile(filepath);
}

bool Config::restoreFromBackup(const std::string& filepath) {
    return loadFromFile(filepath);
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCountBySource(ConfigSource source) const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            count++;
        }
    }
    return count;
}

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

double Config::getAverageComplexity() const {
    if (pImpl->entries.empty()) {
        return 0.0;
    }
    
    double total_complexity = 0.0;
    for (const auto& entry : pImpl->entries) {
        total_complexity += entry.key.length();
    }
    
    return total_complexity / pImpl->entries.size();
}

void Config::setConfigurationVersion(int version) {
    set("config.version", version, ConfigSource::Runtime);
}

int Config::getConfigurationVersion() const {
    return get<int>("config.version").value_or(1);
}

bool Config::isVersionCompatible(int minVersion) const {
    return getConfigurationVersion() >= minVersion;
}

std::vector<ConfigEntry> Config::getEntries() const {
    return pImpl->entries;
}

ConfigEntry Config::getEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it == pImpl->entries.end()) {
        return ConfigEntry(); // Return empty entry if not found
    }
    return *it;
}

// Implementation of helper functions

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

bool Config::isValidNumber(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), [](char c) {
        return std::isdigit(c) || c == '.' || c == '-' || c == '+';
    });
}

bool Config::isValidBoolean(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "false" || lower == "1" || lower == "0";
}

std::string Config::generateConfigurationHash(const std::vector<ConfigEntry>& entries) {
    std::string hash = "config_";
    for (const auto& entry : entries) {
        hash += entry.key + "_" + std::to_string(std::hash<std::string>{}(configValueToString(entry.value)));
    }
    return hash;
}

} // namespace nlm