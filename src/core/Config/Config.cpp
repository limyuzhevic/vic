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

// Configuration validation
bool Config::validate() const {
    // Validate known parameter ranges
    auto validateValue = [](const ConfigEntry& entry) -> bool {
        std::string key = entry.key;
        double value = 0.0;
        
        // Extract value as double
        std::visit([&](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) {
                value = static_cast<double>(val);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                value = static_cast<double>(val);
            } else if constexpr (std::is_same_v<T, double>) {
                value = val;
            } else if constexpr (std::is_same_v<T, bool>) {
                value = val ? 1.0 : 0.0;
            }
        }, entry.value);
        
        // Validate ranges for known parameters
        if (key == "working_memory_size") {
            return value >= 0 && value <= 10000;
        } else if (key == "max_episodic_episodes") {
            return value >= 0 && value <= 10000;
        } else if (key == "critical_period_start") {
            return value >= 0 && value <= 1000.0; // milliseconds
        } else if (key == "maturation_rate") {
            return value >= 0 && value <= 1.0;
        } else if (key == "dopamine_scale") {
            return value >= 0 && value <= 10.0;
        } else if (key == "curiosity_sensitivity") {
            return value >= 0 && value <= 10.0;
        } else if (key == "prediction_horizon") {
            return value >= 1 && value <= 100;
        } else if (key == "prediction_confidence_threshold") {
            return value >= 0 && value <= 1.0;
        } else if (key == "synaptogenesis_probability") {
            return value >= 0 && value <= 1.0;
        } else if (key == "pruning_threshold") {
            return value >= 0 && value <= 1.0;
        } else if (key == "aging_factor") {
            return value >= 0 && value <= 1.0;
        } else if (key == "inhibition_strength") {
            return value >= 0 && value <= 100.0;
        } else if (key == "excitation_strength") {
            return value >= 0 && value <= 100.0;
        } else if (key == "pattern_discovery_threshold") {
            return value >= 0 && value <= 1.0;
        } else if (key == "planning_depth") {
            return value >= 1 && value <= 20;
        } else if (key == "planning_complexity") {
            return value >= 1 && value <= 100;
        } else if (key == "learning_rate") {
            return value >= 0 && value <= 1.0;
        } else if (key == "decay_constant") {
            return value >= 0 && value <= 1.0;
        }
        
        // For unknown keys, assume valid
        return true;
    };
    
    return std::all_of(pImpl->entries.begin(), pImpl->entries.end(), validateValue);
}

// Convenience methods for setting related parameters
void Config::setMemorySystemSize(size_t workingMemorySize, size_t maxEpisodicEpisodes) {
    set("working_memory_size", static_cast<int>(workingMemorySize));
    set("max_episodic_episodes", static_cast<int>(maxEpisodicEpisodes));
}

void Config::setDevelopmentParameters(double criticalPeriodStart, double maturationRate) {
    set("critical_period_start", criticalPeriodStart);
    set("maturation_rate", maturationRate);
}

void Config::setNeuromodulationParameters(float dopamineScale, float curiositySensitivity) {
    set("dopamine_scale", dopamineScale);
    set("curiosity_sensitivity", curiositySensitivity);
}

void Config::setPredictionSystemParameters(int horizon, float confidenceThreshold) {
    set("prediction_horizon", horizon);
    set("prediction_confidence_threshold", confidenceThreshold);
}

void Config::setStructuralPlasticityParameters(float synaptogenesisProb, float pruningThreshold) {
    set("synaptogenesis_probability", synaptogenesisProb);
    set("pruning_threshold", pruningThreshold);
}

void Config::setDevelopmentStages(const std::vector<double>& stageTransitions, float agingFactor) {
    set("developmental_stage_transitions", std::vector<double>(stageTransitions));
    set("aging_factor", agingFactor);
}

void Config::setAttentionSystemParameters(float inhibitionStrength, float excitationStrength) {
    set("inhibition_strength", inhibitionStrength);
    set("excitation_strength", excitationStrength);
}

void Config::setConceptFormationParameters(float patternDiscoveryThreshold) {
    set("pattern_discovery_threshold", patternDiscoveryThreshold);
}

void Config::setPlanningParameters(int maxDepth, int complexity) {
    set("planning_depth", maxDepth);
    set("planning_complexity", complexity);
}

void Config::setLearningParameters(float learningRate, float decayConstant) {
    set("learning_rate", learningRate);
    set("decay_constant", decayConstant);
}

// Batch setter methods
void Config::setAllMemoryParameters(const std::string& workingMemoryKey, const std::string& episodicMemoryKey) {
    // This is a convenience for bulk loading from file
    // Users should use setMemorySystemSize for programmatic use
    set(workingMemoryKey, "PLACEHOLDER_VALUE");
    set(episodicMemoryKey, "PLACEHOLDER_VALUE");
}

void Config::setAllDevelopmentParameters(const std::string& criticalPeriodKey, const std::string& maturationKey) {
    set(criticalPeriodKey, "PLACEHOLDER_VALUE");
    set(maturationKey, "PLACEHOLDER_VALUE");
}

void Config::setAllNeuromodulationParameters(const std::string& dopamineKey, const std::string& curiosityKey) {
    set(dopamineKey, "PLACEHOLDER_VALUE");
    set(curiosityKey, "PLACEHOLDER_VALUE");
}

void Config::setAllPredictionParameters(const std::string& horizonKey, const std::string& confidenceKey) {
    set(horizonKey, "PLACEHOLDER_VALUE");
    set(confidenceKey, "PLACEHOLDER_VALUE");
}

void Config::setAllStructuralPlasticityParameters(const std::string& synaptogenesisKey, const std::string& pruningKey) {
    set(synaptogenesisKey, "PLACEHOLDER_VALUE");
    set(pruningKey, "PLACEHOLDER_VALUE");
}

void Config::setAllDevelopmentStageParameters(const std::string& transitionsKey, const std::string& agingKey) {
    set(transitionsKey, "PLACEHOLDER_VALUE");
    set(agingKey, "PLACEHOLDER_VALUE");
}

void Config::setAllAttentionParameters(const std::string& inhibitionKey, const std::string& excitationKey) {
    set(inhibitionKey, "PLACEHOLDER_VALUE");
    set(excitationKey, "PLACEHOLDER_VALUE");
}

void Config::setAllConceptParameters(const std::string& patternDiscoveryKey) {
    set(patternDiscoveryKey, "PLACEHOLDER_VALUE");
}

void Config::setAllPlanningParameters(const std::string& depthKey, const std::string& complexityKey) {
    set(depthKey, "PLACEHOLDER_VALUE");
    set(complexityKey, "PLACEHOLDER_VALUE");
}

void Config::setAllLearningParameters(const std::string& learningRateKey, const std::string& decayConstantKey) {
    set(learningRateKey, "PLACEHOLDER_VALUE");
    set(decayConstantKey, "PLACEHOLDER_VALUE");
}
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
