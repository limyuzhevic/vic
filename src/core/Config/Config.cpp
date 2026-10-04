#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <functional>
#include <regex>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Implementation of advanced methods
std::string Config::getAdvancedConfigSummary() const {
    std::ostringstream oss;
    oss << "=== Advanced Configuration Summary ===\n";
    oss << "Advanced Features Enabled: " << (hasAdvancedFeatures() ? "Yes" : "No") << "\n\n";
    
    // Get all advanced configuration keys
    auto advancedKeys = getAllAdvancedKeys();
    
    oss << "Advanced Configuration Parameters:\n";
    for (const auto& key : advancedKeys) {
        if (auto value = get<std::string>(key)) {
            oss << "  " << key << " = \"" << *value << "\"\n";
        } else if (auto value = get<int>(key)) {
            oss << "  " << key << " = " << *value << "\n";
        } else if (auto value = get<double>(key)) {
            oss << "  " << key << " = " << *value << "\n";
        } else if (auto value = get<bool>(key)) {
            oss << "  " << key << " = " << (*value ? "true" : "false") << "\n";
        }
    }
    
    oss << "\n=== Brain Configuration ===\n";
    oss << "Brain Type: " << getOr<std::string>("brain_type", "Default") << "\n";
    oss << "Neurons: " << getOr<size_t>("neuron_count", 0) << "\n";
    oss << "Regions: " << getOr<size_t>("region_count", 0) << "\n";
    oss << "Connection Probability: " << getOr<float>("connection_probability", 0.0f) << "\n\n";
    
    oss << "=== Memory Configuration ===\n";
    oss << "Working Memory Capacity: " << getOr<size_t>("working_memory_capacity", 0) << "\n";
    oss << "Working Memory Decay Rate: " << getOr<float>("working_memory_decay_rate", 0.0f) << "\n";
    oss << "Episodic Memory Max Episodes: " << getOr<size_t>("episodic_memory_max_episodes", 0) << "\n";
    oss << "Episodic Memory Consolidation Threshold: " << getOr<float>("episodic_memory_consolidation_threshold", 0.0f) << "\n\n";
    
    oss << "=== Prediction Configuration ===\n";
    oss << "Prediction Depth: " << getOr<int>("prediction_depth", 0) << "\n";
    oss << "Prediction Confidence Threshold: " << getOr<float>("prediction_confidence_threshold", 0.0f) << "\n";
    oss << "Prediction Error Smoothing: " << getOr<float>("prediction_error_smoothing", 0.0f) << "\n\n";
    
    oss << "=== Neuromodulation Configuration ===\n";
    oss << "Dopamine Plasticity Factor: " << getOr<float>("dopamine_plasticity_factor", 0.0f) << "\n";
    oss << "Novelty Detection Threshold: " << getOr<float>("novelty_detection_threshold", 0.0f) << "\n";
    oss << "Curiosity Exploration Weight: " << getOr<float>("curiosity_exploration_weight", 0.0f) << "\n";
    oss << "Serotonin Mood Influence: " << getOr<float>("serotonin_mood_influence", 0.0f) << "\n";
    oss << "Norepinephrine Arousal Threshold: " << getOr<float>("norepinephrine_arousal_threshold", 0.0f) << "\n";
    oss << "Acetylcholine Attention Gain: " << getOr<float>("acetylcholine_attention_gain", 0.0f) << "\n\n";
    
    return oss.str();
}

std::vector<std::string> Config::getAllAdvancedKeys() const {
    std::vector<std::string> advancedKeys;
    
    // Advanced brain configuration keys
    advancedKeys.insert(advancedKeys.end(), {
        "brain_type",
        "neuron_count",
        "region_count",
        "connection_probability"
    });
    
    // Advanced memory configuration keys
    advancedKeys.insert(advancedKeys.end(), {
        "working_memory_capacity",
        "working_memory_decay_rate",
        "episodic_memory_max_episodes",
        "episodic_memory_consolidation_threshold"
    });
    
    // Advanced prediction configuration keys
    advancedKeys.insert(advancedKeys.end(), {
        "prediction_depth",
        "prediction_confidence_threshold",
        "prediction_error_smoothing"
    });
    
    // Advanced cognition configuration keys
    advancedKeys.insert(advancedKeys.end(), {
        "planner_planning_depth",
        "concept_formation_stability_threshold",
        "attention_inhibition_strength",
        "attention_excitation_strength"
    });
    
    // Advanced neuromodulation configuration keys
    advancedKeys.insert(advancedKeys.end(), {
        "dopamine_plasticity_factor",
        "novelty_detection_threshold",
        "curiosity_exploration_weight",
        "serotonin_mood_influence",
        "norepinephrine_arousal_threshold",
        "acetylcholine_attention_gain"
    });
    
    // Advanced development configuration keys
    advancedKeys.insert(advancedKeys.end(), {
        "development_stage_transitions",
        "developmental_plasticity_multiplier",
        "maturation_rate"
    });
    
    // Advanced checkpoint configuration keys
    advancedKeys.insert(advancedKeys.end(), {
        "checkpoint_path",
        "checkpoint_compression_level",
        "checkpoint_max_files",
        "checkpoint_auto_save"
    });
    
    // Advanced performance configuration keys
    advancedKeys.insert(advancedKeys.end(), {
        "memory_pool_size",
        "event_queue_capacity",
        "parallel_processing_enabled",
        "simd_vectorization_enabled"
    });
    
    // Advanced replay configuration keys
    advancedKeys.insert(advancedKeys.end(), {
        "replay_memory_count",
        "replay_confidence_threshold",
        "replay_time_step_interval"
    });
    
    // Advanced rest configuration keys
    advancedKeys.insert(advancedKeys.end(), {
        "rest_period_duration",
        "rest_serotonin_threshold",
        "rest_dopamine_threshold",
        "rest_norepinephrine_threshold",
        "rest_acetylcholine_threshold",
        "rest_development_acceleration"
    });
    
    // Advanced event system configuration keys
    advancedKeys.insert(advancedKeys.end(), {
        "event_queue_max_size",
        "event_processing_interval",
        "event_priority_level"
    });
    
    // Advanced memory consolidation configuration keys
    advancedKeys.insert(advancedKeys.end(), {
        "memory_consolidation_threshold",
        "memory_consolidation_decay",
        "memory_replay_strength"
    });
    
    // Remove duplicates and return
    std::sort(advancedKeys.begin(), advancedKeys.end());
    advancedKeys.erase(std::unique(advancedKeys.begin(), advancedKeys.end()), advancedKeys.end());
    return advancedKeys;
}

void Config::loadAdvancedConfigFromFile(const std::string& filepath) {
    // Try to load as JSON first (placeholder for future JSON parser)
    // For now, use the simple file loading method
    loadFromFile(filepath);
}

void Config::saveAdvancedConfigToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    file << "# Advanced NLM Configuration\n";
    file << "# Generated by advanced configuration system\n\n";
    
    // Save brain configuration
    if (has("brain_type")) {
        file << "brain_type = \"" << getOr<std::string>("brain_type", "") << "\"\n";
    }
    if (has("neuron_count")) {
        file << "neuron_count = " << getOr<size_t>("neuron_count", 0) << "\n";
    }
    if (has("region_count")) {
        file << "region_count = " << getOr<size_t>("region_count", 0) << "\n";
    }
    if (has("connection_probability")) {
        file << "connection_probability = " << getOr<float>("connection_probability", 0.0f) << "\n";
    }
    
    file << "\n";
    
    // Save memory configuration
    if (has("working_memory_capacity")) {
        file << "working_memory_capacity = " << getOr<size_t>("working_memory_capacity", 0) << "\n";
    }
    if (has("working_memory_decay_rate")) {
        file << "working_memory_decay_rate = " << getOr<float>("working_memory_decay_rate", 0.0f) << "\n";
    }
    if (has("episodic_memory_max_episodes")) {
        file << "episodic_memory_max_episodes = " << getOr<size_t>("episodic_memory_max_episodes", 0) << "\n";
    }
    if (has("episodic_memory_consolidation_threshold")) {
        file << "episodic_memory_consolidation_threshold = " << getOr<float>("episodic_memory_consolidation_threshold", 0.0f) << "\n";
    }
    
    file << "\n";
    
    // Save all other advanced configuration
    auto advancedKeys = getAllAdvancedKeys();
    for (const auto& key : advancedKeys) {
        if (has(key)) {
            // Skip keys already saved
            if (key == "brain_type" || key == "neuron_count" || key == "region_count" || 
                key == "connection_probability" || key == "working_memory_capacity" ||
                key == "working_memory_decay_rate" || key == "episodic_memory_max_episodes" ||
                key == "episodic_memory_consolidation_threshold") {
                continue;
            }
            
            file << key << " = ";
            if (auto value = get<std::string>(key)) {
                file << "\"" << *value << "\"";
            } else if (auto value = get<int>(key)) {
                file << *value;
            } else if (auto value = get<double>(key)) {
                file << *value;
            } else if (auto value = get<bool>(key)) {
                file << (*value ? "true" : "false");
            } else if (auto value = get<size_t>(key)) {
                file << *value;
            }
            file << "\n";
        }
    }
    
    file.close();
}

// Legacy method kept for backward compatibility
bool Config::loadFromFile(const std::string& filepath) {
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

// Advanced validation implementations
bool Config::validateAdvancedSetting(const std::string& key, const std::string& value) const {
    // Common validation patterns for different parameter types
    if (key == "brain_type" || key == "checkpoint_path") {
        return !value.empty() && value.length() <= 256;
    }
    if (key == "connection_probability" || key == "working_memory_decay_rate" ||
        key == "prediction_confidence_threshold" || key == "prediction_error_smoothing" ||
        key == "concept_formation_stability_threshold" || key == "attention_inhibition_strength" ||
        key == "attention_excitation_strength" || key == "dopamine_plasticity_factor" ||
        key == "novelty_detection_threshold" || key == "curiosity_exploration_weight" ||
        key == "serotonin_mood_influence" || key == "norepinephrine_arousal_threshold" ||
        key == "acetylcholine_attention_gain" || key == "development_stage_transitions" ||
        key == "developmental_plasticity_multiplier" || key == "maturation_rate" ||
        key == "episodic_memory_consolidation_threshold") {
        double val = std::stod(value);
        return val >= 0.0 && val <= 10.0;
    }
    return true;
}

bool Config::validateAdvancedSetting(const std::string& key, size_t value) const {
    if (key == "neuron_count" || key == "region_count" || key == "working_memory_capacity" ||
        key == "episodic_memory_max_episodes" || key == "checkpoint_max_files" ||
        key == "memory_pool_size" || key == "event_queue_capacity" || key == "replay_memory_count" ||
        key == "event_queue_max_size" || key == "event_processing_interval" || key == "event_priority_level") {
        return value <= 1000000;
    }
    if (key == "rest_period_duration") {
        return value <= 86400; // 24 hours
    }
    return true;
}

bool Config::validateAdvancedSetting(const std::string& key, float value) const {
    if (key == "connection_probability") {
        return value >= 0.0f && value <= 1.0f;
    }
    return validateAdvancedSetting(key, static_cast<double>(value));
}

bool Config::validateAdvancedSetting(const std::string& key, bool value) const {
    // All boolean values are valid
    return true;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

} // namespace nlm
