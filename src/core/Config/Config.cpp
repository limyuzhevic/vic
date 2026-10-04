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

// Implementation of cognitive system configuration methods
void Config::initializePhase6Defaults() {
    // Prediction System
    set(ConfigKeys::PREDICTION_ENABLED, true, ConfigSource::Default);
    set(ConfigKeys::PREDICTION_LEARNING_RATE, 0.01f, ConfigSource::Default);
    set(ConfigKeys::PREDICTION_CONFIDENCE_THRESHOLD, 0.7f, ConfigSource::Default);
    set(ConfigKeys::PREDICTION_HISTORY_SIZE, 1000, ConfigSource::Default);
    
    // Concept Formation
    set(ConfigKeys::CONCEPT_ENABLED, true, ConfigSource::Default);
    set(ConfigKeys::CONCEPT_FORMATION_THRESHOLD, 0.75f, ConfigSource::Default);
    set(ConfigKeys::CONCEPT_STABILITY_THRESHOLD, 0.7f, ConfigSource::Default);
    set(ConfigKeys::CONCEPT_STABILITY_WINDOW, 5, ConfigSource::Default);
    set(ConfigKeys::CONCEPT_MAX_CONCEPTS, 100, ConfigSource::Default);
    set(ConfigKeys::CONCEPT_GENERALIZATION_THRESHOLD, 0.8f, ConfigSource::Default);
    
    // Working Memory
    set(ConfigKeys::WORKING_MEMORY_ENABLED, true, ConfigSource::Default);
    set(ConfigKeys::WORKING_MEMORY_CAPACITY, 200, ConfigSource::Default);
    set(ConfigKeys::WORKING_MEMORY_DECAY_RATE, 0.01f, ConfigSource::Default);
    set(ConfigKeys::WORKING_MEMORY_REINFORCEMENT_STRENGTH, 0.5f, ConfigSource::Default);
    
    // Episodic Memory
    set(ConfigKeys::EPISODIC_MEMORY_ENABLED, true, ConfigSource::Default);
    set(ConfigKeys::EPISODIC_MEMORY_MAX_EPISODES, 10000, ConfigSource::Default);
    set(ConfigKeys::EPISODIC_MEMORY_CONSOLIDATION_THRESHOLD, 0.3f, ConfigSource::Default);
    set(ConfigKeys::EPISODIC_MEMORY_REPLAY_INTERVAL, 100, ConfigSource::Default);
    
    // Attention System
    set(ConfigKeys::ATTENTION_ENABLED, true, ConfigSource::Default);
    set(ConfigKeys::ATTENTION_INHIBITION_STRENGTH, 0.5f, ConfigSource::Default);
    set(ConfigKeys::ATTENTION_EXCITATION_STRENGTH, 1.5f, ConfigSource::Default);
    set(ConfigKeys::ATTENTION_NOISE_LEVEL, 0.1f, ConfigSource::Default);
    
    // Planner
    set(ConfigKeys::PLANNER_ENABLED, true, ConfigSource::Default);
    set(ConfigKeys::PLANNER_PLANNING_DEPTH, 3, ConfigSource::Default);
    set(ConfigKeys::PLANNER_ACTION_DIVERSITY, 10, ConfigSource::Default);
    set(ConfigKeys::PLANNER_EXPLORATION_RATE, 0.3f, ConfigSource::Default);
    set(ConfigKeys::PLANNER_CONSERVATION_BIAS, 0.7f, ConfigSource::Default);
    
    // Neuromodulation
    set(ConfigKeys::NEUROMODULATION_ENABLED, true, ConfigSource::Default);
    set(ConfigKeys::DOPAMINE_BASELINE, 0.1f, ConfigSource::Default);
    set(ConfigKeys::DOPAMINE_RELEASE_RATE, 1.0f, ConfigSource::Default);
    set(ConfigKeys::DOPAMINE_DECAY_RATE, 0.1f, ConfigSource::Default);
    set(ConfigKeys::CURIOUSITY_BASELINE, 0.1f, ConfigSource::Default);
    set(ConfigKeys::NOVELTY_SENSITIVITY, 1.0f, ConfigSource::Default);
    
    // Development
    set(ConfigKeys::DEVELOPMENT_ENABLED, true, ConfigSource::Default);
    set(ConfigKeys::DEVELOPMENT_INITIAL_PLATONICITY, 1.0f, ConfigSource::Default);
    set(ConfigKeys::DEVELOPMENT_MATURATION_RATE, 0.01f, ConfigSource::Default);
    set(ConfigKeys::DEVELOPMENT_CRITICAL_PERIOD, 300.0f, ConfigSource::Default);
    
    // Performance
    set(ConfigKeys::VERBOSE_LOGGING, false, ConfigSource::Default);
    set(ConfigKeys::ENABLE_PERFORMANCE_METRICS, true, ConfigSource::Default);
    set(ConfigKeys::METRICS_EXPORT_INTERVAL, 1000, ConfigSource::Default);
    
    // Integration
    set(ConfigKeys::ENABLE_BRAIN_LOOP_LOGGING, false, ConfigSource::Default);
    set(ConfigKeys::BRAIN_LOOP_TIMEOUT, 100, ConfigSource::Default);
    set(ConfigKeys::ENABLE_EXPERIMENT_MODE, false, ConfigSource::Default);
}

bool Config::getPredictionEnabled() const {
    return getOr<bool>(ConfigKeys::PREDICTION_ENABLED, false);
}

float Config::getPredictionLearningRate() const {
    return getOr<float>(ConfigKeys::PREDICTION_LEARNING_RATE, 0.01f);
}

float Config::getPredictionConfidenceThreshold() const {
    return getOr<float>(ConfigKeys::PREDICTION_CONFIDENCE_THRESHOLD, 0.7f);
}

int Config::getPredictionHistorySize() const {
    return getOr<int>(ConfigKeys::PREDICTION_HISTORY_SIZE, 1000);
}

bool Config::getConceptFormationEnabled() const {
    return getOr<bool>(ConfigKeys::CONCEPT_ENABLED, false);
}

float Config::getConceptFormationThreshold() const {
    return getOr<float>(ConfigKeys::CONCEPT_FORMATION_THRESHOLD, 0.75f);
}

float Config::getConceptStabilityThreshold() const {
    return getOr<float>(ConfigKeys::CONCEPT_STABILITY_THRESHOLD, 0.7f);
}

size_t Config::getConceptStabilityWindow() const {
    return getOr<size_t>(ConfigKeys::CONCEPT_STABILITY_WINDOW, 5);
}

size_t Config::getConceptMaxConcepts() const {
    return getOr<size_t>(ConfigKeys::CONCEPT_MAX_CONCEPTS, 100);
}

bool Config::getWorkingMemoryEnabled() const {
    return getOr<bool>(ConfigKeys::WORKING_MEMORY_ENABLED, false);
}

size_t Config::getWorkingMemoryCapacity() const {
    return getOr<size_t>(ConfigKeys::WORKING_MEMORY_CAPACITY, 200);
}

float Config::getWorkingMemoryDecayRate() const {
    return getOr<float>(ConfigKeys::WORKING_MEMORY_DECAY_RATE, 0.01f);
}

bool Config::getEpisodicMemoryEnabled() const {
    return getOr<bool>(ConfigKeys::EPISODIC_MEMORY_ENABLED, false);
}

size_t Config::getEpisodicMemoryMaxEpisodes() const {
    return getOr<size_t>(ConfigKeys::EPISODIC_MEMORY_MAX_EPISODES, 10000);
}

float Config::getEpisodicMemoryConsolidationThreshold() const {
    return getOr<float>(ConfigKeys::EPISODIC_MEMORY_CONSOLIDATION_THRESHOLD, 0.3f);
}

size_t Config::getEpisodicMemoryReplayInterval() const {
    return getOr<size_t>(ConfigKeys::EPISODIC_MEMORY_REPLAY_INTERVAL, 100);
}

bool Config::getAttentionEnabled() const {
    return getOr<bool>(ConfigKeys::ATTENTION_ENABLED, false);
}

float Config::getAttentionInhibitionStrength() const {
    return getOr<float>(ConfigKeys::ATTENTION_INHIBITION_STRENGTH, 0.5f);
}

float Config::getAttentionExcitationStrength() const {
    return getOr<float>(ConfigKeys::ATTENTION_EXCITATION_STRENGTH, 1.5f);
}

bool Config::getPlannerEnabled() const {
    return getOr<bool>(ConfigKeys::PLANNER_ENABLED, false);
}

int Config::getPlannerPlanningDepth() const {
    return getOr<int>(ConfigKeys::PLANNER_PLANNING_DEPTH, 3);
}

size_t Config::getPlannerActionDiversity() const {
    return getOr<size_t>(ConfigKeys::PLANNER_ACTION_DIVERSITY, 10);
}

bool Config::getNeuromodulationEnabled() const {
    return getOr<bool>(ConfigKeys::NEUROMODULATION_ENABLED, false);
}

float Config::getDopamineBaseline() const {
    return getOr<float>(ConfigKeys::DOPAMINE_BASELINE, 0.1f);
}

bool Config::getDevelopmentEnabled() const {
    return getOr<bool>(ConfigKeys::DEVELOPMENT_ENABLED, false);
}

// Explicit template instantiations
} // namespace nlm

// Explicit template instantiations for the new getter methods
template std::optional<bool> Config::get<bool>(const std::string&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
