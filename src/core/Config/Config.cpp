#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values
    // Cognitive system defaults
    pImpl->entries.emplace_back(CognitiveConfig::NEURAL_PLANNER_DEPTH, 
        DefaultConfig::DEFAULT_NEURAL_PLANNER_DEPTH, 
        ConfigSource::Default, "NeuralPlanner planning depth (max steps to consider)");
    pImpl->entries.emplace_back(CognitiveConfig::NEURAL_PLANNER_CONFIDENCE,
        DefaultConfig::DEFAULT_NEURAL_PLANNER_CONFIDENCE,
        ConfigSource::Default, "NeuralPlanner confidence threshold for accepting plans");
    pImpl->entries.emplace_back(CognitiveConfig::NEURAL_PLANNER_MAX_CACHE_SIZE,
        DefaultConfig::DEFAULT_NEURAL_PLANNER_MAX_CACHE_SIZE,
        ConfigSource::Default, "NeuralPlanner maximum cache size for planning sequences");
    
    pImpl->entries.emplace_back(CognitiveConfig::CONCEPT_FORMATION_MIN_PATTERN_SIZE,
        DefaultConfig::DEFAULT_CONCEPT_FORMATION_MIN_PATTERN_SIZE,
        ConfigSource::Default, "ConceptFormation minimum pattern size for concept formation");
    pImpl->entries.emplace_back(CognitiveConfig::CONCEPT_FORMATION_MAX_PATTERN_SIZE,
        DefaultConfig::DEFAULT_CONCEPT_FORMATION_MAX_PATTERN_SIZE,
        ConfigSource::Default, "ConceptFormation maximum pattern size for concept formation");
    pImpl->entries.emplace_back(CognitiveConfig::CONCEPT_FORMATION_SIMILARITY_THRESHOLD,
        DefaultConfig::DEFAULT_CONCEPT_FORMATION_SIMILARITY_THRESHOLD,
        ConfigSource::Default, "ConceptFormation similarity threshold for pattern matching");
    pImpl->entries.emplace_back(CognitiveConfig::CONCEPT_FORMATION_MIN_REINFORCEMENT,
        DefaultConfig::DEFAULT_CONCEPT_FORMATION_MIN_REINFORCEMENT,
        ConfigSource::Default, "ConceptFormation minimum reinforcement for concept retention");
    
    pImpl->entries.emplace_back(CognitiveConfig::SELF_MODEL_CAPABILITY_BASELINE,
        DefaultConfig::DEFAULT_SELF_MODEL_CAPABILITY_BASELINE,
        ConfigSource::Default, "SelfModel baseline capability level (0-1)");
    pImpl->entries.emplace_back(CognitiveConfig::SELF_MODEL_BODY_AWARENESS_THRESHOLD,
        DefaultConfig::DEFAULT_SELF_MODEL_BODY_AWARENESS_THRESHOLD,
        ConfigSource::Default, "SelfModel body awareness threshold for action prediction");
    pImpl->entries.emplace_back(CognitiveConfig::SELF_MODEL_PREDICTION_ERROR_THRESHOLD,
        DefaultConfig::DEFAULT_SELF_MODEL_PREDICTION_ERROR_THRESHOLD,
        ConfigSource::Default, "SelfModel prediction error threshold for self-generated events");
    
    pImpl->entries.emplace_back(CognitiveConfig::SOCIAL_LEARNING_MAX_OBSERVATIONS,
        DefaultConfig::DEFAULT_SOCIAL_LEARNING_MAX_OBSERVATIONS,
        ConfigSource::Default, "SocialLearning maximum number of observations to store");
    pImpl->entries.emplace_back(CognitiveConfig::SOCIAL_LEARNING_IMITATION_CONFIDENCE,
        DefaultConfig::DEFAULT_SOCIAL_LEARNING_IMITATION_CONFIDENCE,
        ConfigSource::Default, "SocialLearning confidence threshold for action imitation");
    pImpl->entries.emplace_back(CognitiveConfig::SOCIAL_LEARNING_COMMUNICATION_ENABLED,
        DefaultConfig::DEFAULT_SOCIAL_LEARNING_COMMUNICATION_ENABLED,
        ConfigSource::Default, "SocialLearning whether communication learning is enabled");
    
    // Memory system defaults
    pImpl->entries.emplace_back(MemoryConfig::WORKING_MEMORY_CAPACITY,
        DefaultConfig::DEFAULT_WORKING_MEMORY_CAPACITY,
        ConfigSource::Default, "WorkingMemory capacity (number of active traces)");
    pImpl->entries.emplace_back(MemoryConfig::WORKING_MEMORY_DECAY_RATE,
        DefaultConfig::DEFAULT_WORKING_MEMORY_DECAY_RATE,
        ConfigSource::Default, "WorkingMemory decay rate for memory traces");
    pImpl->entries.emplace_back(MemoryConfig::WORKING_MEMORY_REINFORCEMENT_FACTOR,
        DefaultConfig::DEFAULT_WORKING_MEMORY_REINFORCEMENT_FACTOR,
        ConfigSource::Default, "WorkingMemory reinforcement factor for memory strengthening");
    pImpl->entries.emplace_back(MemoryConfig::WORKING_MEMORY_COMPETITION_STRENGTH,
        DefaultConfig::DEFAULT_WORKING_MEMORY_COMPETITION_STRENGTH,
        ConfigSource::Default, "WorkingMemory competition strength between traces");
    
    pImpl->entries.emplace_back(MemoryConfig::EPISODIC_MEMORY_MAX_EPISODES,
        DefaultConfig::DEFAULT_EPISODIC_MEMORY_MAX_EPISODES,
        ConfigSource::Default, "EpisodicMemory maximum number of episodes to store");
    pImpl->entries.emplace_back(MemoryConfig::EPISODIC_MEMORY_REPLAY_ENABLED,
        DefaultConfig::DEFAULT_EPISODIC_MEMORY_REPLAY_ENABLED,
        ConfigSource::Default, "EpisodicMemory whether episode replay is enabled");
    pImpl->entries.emplace_back(MemoryConfig::EPISODIC_MEMORY_CONSOLIDATION_THRESHOLD,
        DefaultConfig::DEFAULT_EPISODIC_MEMORY_CONSOLIDATION_THRESHOLD,
        ConfigSource::Default, "EpisodicMemory relevance threshold for consolidation");
    pImpl->entries.emplace_back(MemoryConfig::EPISODIC_MEMORY_SIMILARITY_THRESHOLD,
        DefaultConfig::DEFAULT_EPISODIC_MEMORY_SIMILARITY_THRESHOLD,
        ConfigSource::Default, "EpisodicMemory similarity threshold for retrieval");
    
    pImpl->entries.emplace_back(MemoryConfig::ASSOCIATIVE_MEMORY_MAX_ASSOCIATIONS,
        DefaultConfig::DEFAULT_ASSOCIATIVE_MEMORY_MAX_ASSOCIATIONS,
        ConfigSource::Default, "AssociativeMemory maximum number of associations to store");
    pImpl->entries.emplace_back(MemoryConfig::ASSOCIATIVE_MEMORY_MAX_PATTERNS,
        DefaultConfig::DEFAULT_ASSOCIATIVE_MEMORY_MAX_PATTERNS,
        ConfigSource::Default, "AssociativeMemory maximum number of patterns to store");
    pImpl->entries.emplace_back(MemoryConfig::ASSOCIATIVE_MEMORY_SPREAD_STEPS,
        DefaultConfig::DEFAULT_ASSOCIATIVE_MEMORY_SPREAD_STEPS,
        ConfigSource::Default, "AssociativeMemory number of steps for activation spreading");
    pImpl->entries.emplace_back(MemoryConfig::ASSOCIATIVE_MEMORY_DECAY_RATE,
        DefaultConfig::DEFAULT_ASSOCIATIVE_MEMORY_DECAY_RATE,
        ConfigSource::Default, "AssociativeMemory decay rate for association strength");
    
    // Prediction system defaults
    pImpl->entries.emplace_back(PredictionConfig::PREDICTION_MODEL_TYPE,
        DefaultConfig::DEFAULT_PREDICTION_MODEL_TYPE,
        ConfigSource::Default, "PredictionSystem type of prediction model (bayesian, gaussian, etc.)");
    pImpl->entries.emplace_back(PredictionConfig::PREDICTION_UPDATE_RATE,
        DefaultConfig::DEFAULT_PREDICTION_UPDATE_RATE,
        ConfigSource::Default, "PredictionSystem rate at which predictions are updated");
    pImpl->entries.emplace_back(PredictionConfig::PREDICTION_CONSISTENCY_THRESHOLD,
        DefaultConfig::DEFAULT_PREDICTION_CONSISTENCY_THRESHOLD,
        ConfigSource::Default, "PredictionSystem consistency threshold for accepting predictions");
    pImpl->entries.emplace_back(PredictionConfig::PREDICTION_CONFIDENCE_THRESHOLD,
        DefaultConfig::DEFAULT_PREDICTION_CONFIDENCE_THRESHOLD,
        ConfigSource::Default, "PredictionSystem confidence threshold for prediction reliability");
    pImpl->entries.emplace_back(PredictionConfig::PREDICTION_MAX_HISTORY,
        DefaultConfig::DEFAULT_PREDICTION_MAX_HISTORY,
        ConfigSource::Default, "PredictionSystem maximum prediction history to keep");
    pImpl->entries.emplace_back(PredictionConfig::PREDICTION_LEARNING_RATE,
        DefaultConfig::DEFAULT_PREDICTION_LEARNING_RATE,
        ConfigSource::Default, "PredictionSystem learning rate for model updates");
}

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
