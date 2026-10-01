#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <random>
#include <chrono>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Initialize with default values for all NLM systems
    Impl() {
        // ===== NEURAL CORE SYSTEM =====
        setDefaultCoreParameters();
        
        // ===== MEMORY SYSTEMS =====
        setDefaultMemoryParameters();
        
        // ===== PREDICTION SYSTEM =====
        setDefaultPredictionParameters();
        
        // ===== COGNITIVE SYSTEMS =====
        setDefaultCognitionParameters();
        
        // ===== DEVELOPMENT SYSTEM =====
        setDefaultDevelopmentParameters();
        
        // ===== NEUROMODULATION SYSTEMS =====
        setDefaultNeuromodulationParameters();
        
        // ===== AGENT BRAIN INTERFACE =====
        setDefaultAgentParameters();
        
        // ===== INTEGRATION & BEHAVIOR =====
        setDefaultIntegrationParameters();
        
        // ===== PERFORMANCE & MONITORING =====
        setDefaultPerformanceParameters();
        
        // ===== LOGGING & DEBUGGING =====
        setDefaultLoggingParameters();
    }
    
    void setDefaultCoreParameters() {
        // Neuron dynamics
        set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default, "Total number of neurons in the brain");
        set("region_count", static_cast<int64_t>(1), ConfigSource::Default, "Number of brain regions");
        set("connection_probability", 0.1f, ConfigSource::Default, "Probability of forming synaptic connections between neurons");
        set("simulation_timestep", 0.001, ConfigSource::Default, "Time step duration for neural simulation (seconds)");
        set("random_seed", static_cast<uint64_t>(42), ConfigSource::Default, "Random seed for reproducible experiments");
        
        // Neural dynamics parameters
        set("lif_tau", 0.02f, ConfigSource::Default, "Membrane time constant for LIF neurons (seconds)");
        set("lif_v_reset", -0.06f, ConfigSource::Default, "Reset voltage for LIF neurons (V)");
        set("lif_v_th", -0.04f, ConfigSource::Default, "Threshold voltage for LIF neurons (V)");
        set("lif_v_rest", -0.06f, ConfigSource::Default, "Resting voltage for LIF neurons (V)");
        set("lif_refractory_period", 2.0f, ConfigSource::Default, "Refractory period after spike (ms)");
        
        // Spike system parameters
        set("spike_delay_min", 1, ConfigSource::Default, "Minimum synaptic delay in simulation steps");
        set("spike_delay_max", 10, ConfigSource::Default, "Maximum synaptic delay in simulation steps");
        set("spike_reliability", 0.99f, ConfigSource::Default, "Reliability of spike transmission (0-1)");
    }
    
    void setDefaultMemoryParameters() {
        // Working memory
        set("working_memory_capacity", static_cast<size_t>(100), ConfigSource::Default, "Capacity of working memory (number of neuron activations)");
        set("working_memory_decay_rate", 0.1f, ConfigSource::Default, "Decay rate for working memory traces per timestep");
        set("working_memory_reinforcement_threshold", 0.5f, ConfigSource::Default, "Activation threshold for strengthening working memory traces");
        
        // Episodic memory
        set("episodic_memory_max_episodes", 1000u, ConfigSource::Default, "Maximum number of episodic memory episodes");
        set("episodic_memory_consolidation_interval", 100u, ConfigSource::Default, "Steps between memory consolidation events");
        set("episodic_memory_replay_interval", 100u, ConfigSource::Default, "Steps between memory replay events");
        set("episodic_memory_replay_episodes", 3u, ConfigSource::Default, "Number of episodes to replay during consolidation");
        set("episodic_memory_consolidation_strength", 0.3f, ConfigSource::Default, "Strength of memory consolidation (0-1)");
        set("episodic_memory_pruning_threshold", 0.1f, ConfigSource::Default, "Activation threshold for removing weak episodic memories");
        
        // Associative memory
        set("associative_memory_learning_rate", 0.01f, ConfigSource::Default, "Learning rate for associative memory formation");
        set("associative_memory_similarity_threshold", 0.7f, ConfigSource::Default, "Similarity threshold for pattern matching");
        set("associative_memory_max_associations", 500u, ConfigSource::Default, "Maximum number of associative memory associations");
    }
    
    void setDefaultPredictionParameters() {
        set("prediction_system_history_size", 100u, ConfigSource::Default, "Size of prediction history buffer");
        set("prediction_update_frequency", 10u, ConfigSource::Default, "How often prediction system is updated (simulation steps)");
        set("prediction_confidence_threshold", 0.8f, ConfigSource::Default, "Confidence threshold for accepting predictions");
        set("prediction_error_decay", 0.95f, ConfigSource::Default, "Decay rate for prediction error signal");
        set("prediction_learning_rate", 0.005f, ConfigSource::Default, "Learning rate for prediction system training");
    }
    
    void setDefaultCognitionParameters() {
        // Planning
        set("neural_planner_depth", 5u, ConfigSource::Default, "Planning depth (how many steps ahead)");
        set("neural_planner_confidence_threshold", 0.6f, ConfigSource::Default, "Minimum confidence to accept plan");
        set("neural_planner_action_quality_history", 10u, ConfigSource::Default, "Number of action outcomes to track for quality assessment");
        
        // Concept formation
        set("concept_formation_stability_threshold", 0.8f, ConfigSource::Default, "Stability threshold for concept formation");
        set("concept_formation_prototype_learning_rate", 0.02f, ConfigSource::Default, "Learning rate for prototype formation");
        set("concept_formation_similarity_measure", "euclidean", ConfigSource::Default, "Similarity measure: euclidean, cosine, or correlation");
        
        // Attention
        set("attentional_selection_inhibition_strength", 0.5f, ConfigSource::Default, "Strength of lateral inhibition in attention system");
        set("attentional_selection_excitation_strength", 1.5f, ConfigSource::Default, "Strength of excitation in attention system");
        set("attentional_selection_competition_threshold", 0.7f, ConfigSource::Default, "Threshold for attention competition winner selection");
        
        // Self-model
        set("self_model_update_frequency", 100u, ConfigSource::Default, "How often self-model is updated");
        set("self_model_confidence_decay", 0.98f, ConfigSource::Default, "Decay rate for self-model confidence");
    }
    
    void setDefaultDevelopmentParameters() {
        set("development_stage_initial", 0, ConfigSource::Default, "Duration of initial development stage (steps)");
        set("development_stage_critical_period", 100, ConfigSource::Default, "Duration of critical period (steps)");
        set("development_stage_maturation", 300, ConfigSource::Default, "Duration of maturation stage (steps)");
        set("development_stage_adult", 900, ConfigSource::Default, "Duration of adult stage (steps)");
        set("development_stage_aging", 2000, ConfigSource::Default, "Duration of aging stage (steps)");
        
        // Synaptogenesis
        set("synaptogenesis_rate_initial", 0.0001f, ConfigSource::Default, "Initial rate of new synapse formation");
        set("synaptogenesis_rate_critical", 0.00008f, ConfigSource::Default, "Rate during critical period");
        set("synaptogenesis_rate_mature", 0.00005f, ConfigSource::Default, "Rate during maturation");
        
        // Pruning
        set("pruning_rate_initial", 0.00001f, ConfigSource::Default, "Initial rate of synapse pruning");
        set("pruning_rate_critical", 0.00002f, ConfigSource::Default, "Rate during critical period");
        set("pruning_rate_mature", 0.00003f, ConfigSource::Default, "Rate during maturation");
        
        // Maturation
        set("maturation_plasticity_modifier", 0.5f, ConfigSource::Default, "Plasticity modifier during maturation (0-1)");
    }
    
    void setDefaultNeuromodulationParameters() {
        // Dopamine
        set("dopamine_initial_level", 0.0f, ConfigSource::Default, "Initial dopamine level");
        set("dopamine_learning_rate", 0.01f, ConfigSource::Default, "Learning rate for dopamine system");
        set("dopamine_plasticity_factor_max", 2.0f, ConfigSource::Default, "Maximum plasticity factor modulation by dopamine");
        set("dopamine_plasticity_factor_min", 0.1f, ConfigSource::Default, "Minimum plasticity factor modulation by dopamine");
        
        // Novelty
        set("novelty_sensitivity", 0.5f, ConfigSource::Default, "Sensitivity to novel sensory input");
        set("novelty_decay_rate", 0.99f, ConfigSource::Default, "Decay rate for novelty signal");
        
        // Curiosity
        set("curiosity_explore_threshold", 0.3f, ConfigSource::Default, "Curiosity threshold for exploration");
        set("curiosity_max_random_action_chance", 0.3f, ConfigSource::Default, "Maximum chance of random action due to curiosity");
        
        // Prediction error
        set("prediction_error_initial", 0.0f, ConfigSource::Default, "Initial prediction error value");
        set("prediction_error_learning_rate", 0.1f, ConfigSource::Default, "Learning rate for prediction error system");
    }
    
    void setDefaultAgentParameters() {
        // Agent behavior
        set("agent_energy_initial", 100.0f, ConfigSource::Default, "Initial energy level");
        set("agent_energy_decay_rate", 0.01f, ConfigSource::Default, "Rate of energy decay");
        set("agent_health_initial", 1.0f, ConfigSource::Default, "Initial health level");
        set("agent_health_loss_per_step", 0.01f, ConfigSource::Default, "Health loss per simulation step");
        
        // Motor system
        set("motor_move_speed", 3.0f, ConfigSource::Default, "Movement speed");
        set("motor_turn_speed", 2.0f, ConfigSource::Default, "Turning speed");
        set("motor_action_cost_move", 0.01f, ConfigSource::Default, "Energy cost for moving");
        set("motor_action_cost_turn", 0.005f, ConfigSource::Default, "Energy cost for turning");
        
        // Sensory processing
        set("sensory_vision_range", 8.0f, ConfigSource::Default, "Vision sensor range");
        set("sensory_vision_fov", 90.0f, ConfigSource::Default, "Vision field of view in degrees");
        set("sensory_vision_resolution", 16, ConfigSource::Default, "Vision resolution (width/height)");
        set("sensory_touch_range", 1.5f, ConfigSource::Default, "Touch sensor range");
    }
    
    void setDefaultIntegrationParameters() {
        // Integration frequency
        set("integration_update_frequency", 1u, ConfigSource::Default, "How often agent integration occurs (simulation steps)");
        set("integration_brain_steps_per_action", 10u, ConfigSource::Default, "Number of brain steps per agent action");
        
        // Memory integration
        set("integration_store_frequency", 10u, ConfigSource::Default, "How often working memory is updated from sensory input");
        set("integration_recall_frequency", 5u, ConfigSource::Default, "How often working memory is recalled to influence planning");
        
        // Cognitive integration
        set("integration_attention_frequency", 1u, ConfigSource::Default, "How often attention system is updated");
        set("integration_planning_frequency", 5u, ConfigSource::Default, "How often planning is updated");
        
        // Development integration
        set("integration_development_frequency", 100u, ConfigSource::Default, "How often development system is updated");
    }
    
    void setDefaultPerformanceParameters() {
        // Performance optimization
        set("performance_enable_memory_pools", true, ConfigSource::Default, "Enable memory pool optimization");
        set("performance_enable_event_queues", true, ConfigSource::Default, "Enable event queue optimization");
        set("performance_enable_sparse_connectivity", true, ConfigSource::Default, "Enable sparse connectivity optimization");
        set("performance_enable_parallel_processing", false, ConfigSource::Default, "Enable parallel processing (experimental)");
        set("performance_enable_simd", true, ConfigSource::Default, "Enable SIMD vectorization");
        set("performance_max_memory_mb", 512u, ConfigSource::Default, "Maximum memory usage in MB");
        
        // Profiling
        set("performance_enable_profiling", false, ConfigSource::Default, "Enable performance profiling");
        set("performance_profile_interval", 10000u, ConfigSource::Default, "Interval between performance profile snapshots");
        set("performance_profile_output_file", "performance_profile.csv", ConfigSource::Default, "File for performance profile output");
    }
    
    void setDefaultLoggingParameters() {
        // Logging
        set("logging_level", "INFO", ConfigSource::Default, "Logging level: DEBUG, INFO, WARN, ERROR, CRITICAL");
        set("logging_enable_console", true, ConfigSource::Default, "Enable console logging");
        set("logging_enable_file", false, ConfigSource::Default, "Enable file logging");
        set("logging_file_path", "nlm.log", ConfigSource::Default, "Path to log file");
        set("logging_log_brain_step", true, ConfigSource::Default, "Log detailed brain step information");
        set("logging_log_agent_step", true, ConfigSource::Default, "Log detailed agent step information");
        set("logging_log_memory_stats", false, ConfigSource::Default, "Log memory system statistics");
    }
    
    // Helper method to add a parameter with description
    template<typename T>
    void addParameter(const std::string& key, const T& value, ConfigSource source, const std::string& description) {
        set(key, ConfigValue(value), source, description);
    }
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
