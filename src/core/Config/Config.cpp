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

}

// Helper function to parse boolean values
bool parseBool(const std::string& str) {
    std::string lower = toLower(trim(str));
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
}

// Helper function to parse integer values
int parseInt(const std::string& str) {
    return std::stoi(str);
}

// Helper function to parse double values
double parseDouble(const std::string& str) {
    return std::stod(str);
}

// Helper function to parse string values
std::string parseString(const std::string& str) {
    return str;
}

void Config::initializeDefaults() {
    clear();
    
    // ===== BRAIN CONFIGURATION =====
    set("neuron_count", 1000, ConfigSource::Default);
    set("region_count", 1, ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("random_seed", 42, ConfigSource::Default);
    
    // ===== NEUROMODULATION CONFIGURATION =====
    set("dopamine_enabled", true, ConfigSource::Default);
    set("curiosity_enabled", true, ConfigSource::Default);
    set("novelty_enabled", true, ConfigSource::Default);
    set("prediction_error_enabled", true, ConfigSource::Default);
    set("dopamine_level", 0.5f, ConfigSource::Default);
    set("curiosity_threshold", 0.3f, ConfigSource::Default);
    set("novelty_sensitivity", 0.5f, ConfigSource::Default);
    
    // ===== MEMORY SYSTEM CONFIGURATION =====
    set("working_memory_capacity", 100, ConfigSource::Default);
    set("episodic_memory_max_episodes", 1000, ConfigSource::Default);
    set("associative_memory_size", 500, ConfigSource::Default);
    set("memory_consolidation_interval", 1000, ConfigSource::Default);
    set("memory_replay_enabled", true, ConfigSource::Default);
    set("memory_replay_frequency", 100, ConfigSource::Default);
    
    // ===== PREDICTION SYSTEM CONFIGURATION =====
    set("prediction_enabled", true, ConfigSource::Default);
    set("prediction_horizon", 10, ConfigSource::Default);
    set("prediction_error_weight", 0.5f, ConfigSource::Default);
    set("prediction_learning_rate", 0.01f, ConfigSource::Default);
    
    // ===== COGNITION SYSTEM CONFIGURATION =====
    set("attention_enabled", true, ConfigSource::Default);
    set("attention_inhibition_strength", 0.5f, ConfigSource::Default);
    set("attention_excitation_strength", 1.5f, ConfigSource::Default);
    set("concept_formation_enabled", true, ConfigSource::Default);
    set("concept_embedding_dim", 64, ConfigSource::Default);
    set("planner_enabled", true, ConfigSource::Default);
    set("planner_planning_depth", 5, ConfigSource::Default);
    
    // ===== DEVELOPMENT SYSTEM CONFIGURATION =====
    set("development_enabled", true, ConfigSource::Default);
    set("developmental_stage", 0, ConfigSource::Default);
    set("critical_period_enabled", true, ConfigSource::Default);
    set("maturation_rate", 0.01f, ConfigSource::Default);
    set("plasticity_decay_rate", 0.001f, ConfigSource::Default);
    
    // ===== AGENT BRAIN ADVANCED FEATURES =====
    set("agent_brain_feature_extraction", true, ConfigSource::Default);
    set("agent_brain_cross_modal_fusion", true, ConfigSource::Default);
    set("agent_brain_temporal_smoothing", true, ConfigSource::Default);
    set("agent_brain_value_learning", true, ConfigSource::Default);
    set("agent_brain_curiosity_driven_exploration", true, ConfigSource::Default);
    set("agent_brain_experience_collection", true, ConfigSource::Default);
    set("agent_brain_action_statistics", true, ConfigSource::Default);
    set("agent_brain_learning_rate", 0.01f, ConfigSource::Default);
    set("agent_brain_exploration_epsilon", 0.1f, ConfigSource::Default);
    set("agent_brain_discount_factor", 0.95f, ConfigSource::Default);
    set("agent_brain_max_experience_history", 1000, ConfigSource::Default);
    set("agent_brain_novelty_decay", 0.99f, ConfigSource::Default);
    set("agent_brain_curiosity_threshold", 0.3f, ConfigSource::Default);
    
    // ===== PERFORMANCE AND OPTIMIZATION =====
    set("performance_profiling_enabled", false, ConfigSource::Default);
    set("performance_profiling_output", "profile.txt", ConfigSource::Default);
    set("performance_multithreading_enabled", false, ConfigSource::Default);
    set("performance_thread_count", 4, ConfigSource::Default);
    set("performance_spike_system_opt", "auto", ConfigSource::Default);
    
    // ===== CHECKPOINT AND PERSISTENCE =====
    set("checkpoint_enabled", true, ConfigSource::Default);
    set("checkpoint_dir", "./checkpoints", ConfigSource::Default);
    set("checkpoint_frequency", 1000, ConfigSource::Default);
    set("checkpoint_compression", "balanced", ConfigSource::Default);
    set("auto_save_enabled", true, ConfigSource::Default);
    set("auto_save_interval", 100, ConfigSource::Default);
    
    // ===== LOGGING AND DEBUGGING =====
    set("logging_level", "Info", ConfigSource::Default);
    set("log_to_console", true, ConfigSource::Default);
    set("log_to_file", false, ConfigSource::Default);
    set("log_file", "nlm.log", ConfigSource::Default);
    set("debug_mode", false, ConfigSource::Default);
    set("debug_output_file", "debug.txt", ConfigSource::Default);
    
    // ===== EXPERIMENT CONTROL =====
    set("experiment_enabled", true, ConfigSource::Default);
    set("experiment_max_steps", 10000, ConfigSource::Default);
    set("experiment_save_metrics", true, ConfigSource::Default);
    set("experiment_metrics_file", "metrics.csv", ConfigSource::Default);
    set("experiment_random_seed", 42, ConfigSource::Default);
    
    // ===== ADVANCED FEATURE FLAGS =====
    set("advanced_features_enabled", true, ConfigSource::Default);
    set("research_mode_enabled", false, ConfigSource::Default);
    set("debug_neural_dynamics", false, ConfigSource::Default);
    set("enable_structural_plasticity", true, ConfigSource::Default);
    set("enable_stdp_learning", true, ConfigSource::Default);
    set("enable_hebbian_learning", true, ConfigSource::Default);
    set("enable_development_dynamics", true, ConfigSource::Default);
    
    // ===== VALIDATION AND INTEGRATION =====
    set("validate_memory_integrations", true, ConfigSource::Default);
    set("validate_neuromodulation_integrations", true, ConfigSource::Default);
    set("validate_prediction_integrations", true, ConfigSource::Default);
    set("validate_cognition_integrations", true, ConfigSource::Default);
    set("validate_development_integrations", true, ConfigSource::Default);
}

std::string Config::getConfigurationHelp() {
    std::ostringstream oss;
    oss << "NLM Configuration Options:\n\n";
    oss << "=== BRAIN CONFIGURATION ===\n";
    oss << "  --neuron_count=<n>          Number of neurons (default: 1000)\n";
    oss << "  --region_count=<n>          Number of brain regions (default: 1)\n";
    oss << "  --connection_probability=<p> Connection probability between neurons (default: 0.1)\n";
    oss << "  --simulation_timestep=<t>    Simulation timestep in seconds (default: 0.001)\n";
    oss << "  --random_seed=<s>            Random seed for reproducibility (default: 42)\n\n";
    
    oss << "=== NEUROMODULATION CONFIGURATION ===\n";
    oss << "  --dopamine_enabled=<b>       Enable dopamine neuromodulation (default: true)\n";
    oss << "  --curiosity_enabled=<b>      Enable curiosity-driven exploration (default: true)\n";
    oss << "  --novelty_enabled=<b>        Enable novelty detection (default: true)\n";
    oss << "  --prediction_error_enabled=<b> Enable prediction error signaling (default: true)\n";
    oss << "  --dopamine_level=<l>         Initial dopamine level (default: 0.5)\n";
    oss << "  --curiosity_threshold=<t>    Curiosity threshold for exploration (default: 0.3)\n";
    oss << "  --novelty_sensitivity=<s>    Novelty sensitivity factor (default: 0.5)\n\n";
    
    oss << "=== MEMORY SYSTEM CONFIGURATION ===\n";
    oss << "  --working_memory_capacity=<n>  Working memory capacity (default: 100)\n";
    oss << "  --episodic_memory_max_episodes=<n> Maximum episodic memory episodes (default: 1000)\n";
    oss << "  --associative_memory_size=<n>  Associative memory pattern count (default: 500)\n";
    oss << "  --memory_consolidation_interval=<n> Steps between memory consolidation (default: 1000)\n";
    oss << "  --memory_replay_enabled=<b>   Enable memory replay (default: true)\n";
    oss << "  --memory_replay_frequency=<n> Replay frequency in steps (default: 100)\n\n";
    
    oss << "=== AGENT BRAIN ADVANCED FEATURES ===\n";
    oss << "  --agent_brain_feature_extraction=<b> Enable advanced feature extraction (default: true)\n";
    oss << "  --agent_brain_cross_modal_fusion=<b> Enable cross-modal fusion (default: true)\n";
    oss << "  --agent_brain_value_learning=<b> Enable value-based learning (default: true)\n";
    oss << "  --agent_brain_curiosity_driven_exploration=<b> Enable curiosity-driven exploration (default: true)\n";
    oss << "  --agent_brain_experience_collection=<b> Enable experience collection (default: true)\n";
    oss << "  --agent_brain_learning_rate=<l> Agent learning rate (default: 0.01)\n";
    oss << "  --agent_brain_exploration_epsilon=<e> Exploration rate (default: 0.1)\n";
    oss << "  --agent_brain_discount_factor=<d> Discount factor for future rewards (default: 0.95)\n\n";
    
    oss << "=== PERFORMANCE AND OPTIMIZATION ===\n";
    oss << "  --performance_profiling_enabled=<b> Enable performance profiling (default: false)\n";
    oss << "  --performance_profiling_output=<f> Profiling output file (default: profile.txt)\n";
    oss << "  --performance_multithreading_enabled=<b> Enable multithreading (default: false)\n";
    oss << "  --performance_thread_count=<n> Number of threads (default: 4)\n\n";
    
    oss << "=== CHECKPOINT AND PERSISTENCE ===\n";
    oss << "  --checkpoint_enabled=<b>       Enable checkpointing (default: true)\n";
    oss << "  --checkpoint_dir=<d>          Checkpoint directory (default: ./checkpoints)\n";
    oss << "  --checkpoint_frequency=<n>     Checkpoint frequency in steps (default: 1000)\n";
    oss << "  --auto_save_enabled=<b>       Enable automatic saving (default: true)\n";
    oss << "  --auto_save_interval=<n>      Auto-save interval in steps (default: 100)\n\n";
    
    oss << "=== EXPERIMENT CONTROL ===\n";
    oss << "  --experiment_max_steps=<n>     Maximum simulation steps (default: 10000)\n";
    oss << "  --experiment_random_seed=<s>   Random seed for experiments (default: 42)\n\n";
    
    oss << "=== ADVANCED FEATURE FLAGS ===\n";
    oss << "  --advanced_features_enabled=<b>    Enable all advanced features (default: true)\n";
    oss << "  --research_mode_enabled=<b>        Enable research mode (default: false)\n";
    oss << "  --debug_mode=<b>                   Enable debug output (default: false)\n";
    oss << "  --enable_structural_plasticity=<b> Enable structural plasticity (default: true)\n";
    oss << "  --enable_stdp_learning=<b>         Enable STDP learning (default: true)\n";
    oss << "  --enable_hebbian_learning=<b>      Enable Hebbian learning (default: true)\n\n";
    
    oss << "=== HELP ===\n";
    oss << "  --help                         Show this help message\n";
    oss << "  --version                      Show version information\n";
    oss << "  --validate-config              Validate current configuration and show warnings\n";
    
    return oss.str();
}

bool Config::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Validate brain configuration
    auto neuronCount = get<size_t>("neuron_count");
    if (neuronCount && neuronCount.value() == 0) {
        errors.push_back("neuron_count must be greater than 0");
    }
    
    auto connectionProb = get<double>("connection_probability");
    if (connectionProb && (connectionProb.value() < 0.0 || connectionProb.value() > 1.0)) {
        errors.push_back("connection_probability must be between 0.0 and 1.0");
    }
    
    auto timestep = get<double>("simulation_timestep");
    if (timestep && timestep.value() <= 0.0) {
        errors.push_back("simulation_timestep must be greater than 0.0");
    }
    
    // Validate memory configuration
    auto workMemCap = get<size_t>("working_memory_capacity");
    if (workMemCap && workMemCap.value() == 0) {
        errors.push_back("working_memory_capacity must be greater than 0");
    }
    
    auto maxEpisodes = get<size_t>("episodic_memory_max_episodes");
    if (maxEpisodes && maxEpisodes.value() == 0) {
        errors.push_back("episodic_memory_max_episodes must be greater than 0");
    }
    
    // Validate performance configuration
    auto threadCount = get<size_t>("performance_thread_count");
    if (threadCount && threadCount.value() == 0) {
        errors.push_back("performance_thread_count must be greater than 0");
    }
    
    // Validate checkpoint configuration
    auto checkpointFreq = get<size_t>("checkpoint_frequency");
    if (checkpointFreq && checkpointFreq.value() == 0) {
        errors.push_back("checkpoint_frequency must be greater than 0");
    }
    
    return errors;
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
