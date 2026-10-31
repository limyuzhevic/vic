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
    bool showHelp = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle help flag
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
            continue;
        }
        
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
    
    if (showHelp) {
        printHelp();
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
        // Update existing entry but preserve its description
        it->value = value;
        it->source = source;
    } else {
        // Add new entry with default description based on key
        std::string description = "Configuration parameter: " + key;
        if (key.find("neuron") != std::string::npos) description = "Number of neurons in brain";
        else if (key.find("synapse") != std::string::npos) description = "Synapse connectivity settings";
        else if (key.find("plasticity") != std::string::npos) description = "Plasticity learning parameters";
        else if (key.find("dopamine") != std::string::npos) description = "Dopamine neuromodulation level";
        else if (key.find("curiosity") != std::string::npos) description = "Curiosity-driven exploration level";
        else if (key.find("reward") != std::string::npos) description = "Reward prediction error";
        else if (key.find("development") != std::string::npos) description = "Developmental plasticity rates";
        
        pImpl->entries.emplace_back(key, value, source, description);
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
        oss << "] (" << static_cast<int>(entry.source) << ") " << entry.description << "\n";
    }
    return oss.str();
}

void Config::printHelp() const {
    std::cout << "=== NLM Configuration Help ===\n" << std::endl;
    std::cout << "Configuration Options:\n" << std::endl;
    
    // Brain Configuration
    std::cout << "Brain Configuration:\n";
    std::cout << "  --neuron_count=<int>          Total number of neurons (default: 1000)\n";
    std::cout << "  --region_count=<int>           Number of brain regions (default: 1)\n";
    std::cout << "  --connection_probability=<float> Connection probability (default: 0.1)\n";
    std::cout << "  --simulation_timestep=<float>   Simulation timestep in seconds (default: 0.001)\n";
    std::cout << std::endl;
    
    // Plasticity Configuration
    std::cout << "Plasticity Configuration:\n";
    std::cout << "  --stdp_ltp_weight=<float>       STDP LTP weight (default: 0.01)\n";
    std::cout << "  --stdp_ltd_weight=<float>       STDP LTD weight (default: 0.012)\n";
    std::cout << "  --stdp_time_constant=<float>    STDP time constant (default: 20.0)\n";
    std::cout << "  --plasticity_learning_rate=<float> Overall plasticity rate (default: 0.01)\n";
    std::cout << std::endl;
    
    // Development Configuration
    std::cout << "Development Configuration:\n";
    std::cout << "  --development_synaptogenesis_rate=<float> Synaptogenesis rate (default: 0.001)\n";
    std::cout << "  --development_pruning_rate=<float>   Pruning rate (default: 0.0001)\n";
    std::cout << std::endl;
    
    // Neuromodulation Configuration
    std::cout << "Neuromodulation Configuration:\n";
    std::cout << "  --dopamine_baseline=<float>     Dopamine baseline level (default: 0.1)\n";
    std::cout << "  --curiosity_baseline=<float>    Curiosity baseline (default: 0.5)\n";
    std::cout << "  --novelty_threshold=<float>     Novelty detection threshold (default: 0.3)\n";
    std::cout << "  --reward_discount_factor=<float> Reward discount factor (default: 0.99)\n";
    std::cout << std::endl;
    
    // Memory Configuration
    std::cout << "Memory Configuration:\n";
    std::cout << "  --working_memory_capacity=<int>  Working memory capacity (default: 100)\n";
    std::cout << "  --episodic_memory_max_episodes=<int> Max episodic memory episodes (default: 1000)\n";
    std::cout << "  --replay_interval=<int>         Replay every N steps (default: 100)\n";
    std::cout << "  --consolidation_interval=<int>  Memory consolidation interval (default: 1000)\n";
    std::endl;
    
    // Cognition Configuration
    std::cout << "Cognition Configuration:\n";
    std::cout << "  --planning_depth=<int>           Planning lookahead depth (default: 5)\n";
    std::cout << "  --attention_inhibition_strength=<float> Attention inhibition (default: 0.5)\n";
    std::cout << "  --attention_excitation_strength=<float> Attention excitation (default: 1.5)\n";
    std::cout << "  --concept_learning_rate=<float>  Concept formation learning rate (default: 0.01)\n";
    std::endl;
    
    // Environment Configuration
    std::cout << "Environment Configuration:\n";
    std::cout << "  --environment_name=<string>      Environment name (default: GridWorld)\n";
    std::cout << "  --environment_width=<int>        Environment width (default: 10)\n";
    std::cout << "  --environment_height=<int>       Environment height (default: 10)\n";
    std::endl;
    
    // Performance Configuration
    std::cout << "Performance Configuration:\n";
    std::cout << "  --checkpoint_dir=<string>        Directory for checkpoints (default: ./checkpoints)\n";
    std::cout << "  --checkpoint_interval=<int>      Checkpoint every N steps (default: 1000)\n";
    std::cout << "  --enable_memory_pools=<bool>     Enable memory pools (default: true)\n";
    std::cout << "  --enable_event_queues=<bool>     Enable event queues (default: true)\n";
    std::cout << "  --enable_parallel_processing=<bool> Enable parallel processing (default: false)\n";
    std::cout << "  --enable_simd_vectorization=<bool> Enable SIMD vectorization (default: true)\n";
    std::endl;
    
    std::cout << "Usage Examples:\n" << std::endl;
    std::cout << "  # Create a small brain with custom settings\n";
    std::cout << "  nlm --neuron_count=500 --region_count=1 --connection_probability=0.05\n" << std::endl;
    std::cout << "  # Enable all learning systems with specific neuromodulation\n";
    std::cout << "  nlm --dopamine_baseline=0.5 --curiosity_baseline=0.8 --novelty_threshold=0.2\n" << std::endl;
    std::cout << "  # Create checkpointing and detailed logging\n";
    std::cout << "  nlm --checkpoint_dir=./mybrain --checkpoint_interval=500 --log_level=DEBUG\n" << std::endl;
    std::cout << "  # Run with maximum memory capacity for long simulations\n";
    std::cout << "  nlm --working_memory_capacity=1000 --episodic_memory_max_episodes=5000\n" << std::endl;
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

void Config::printHelp() const {
    std::cout << "=== NLM Configuration Help ===" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM Configuration Options:" << std::endl;
    std::cout << std::endl;
    
    // Brain Configuration
    std::cout << "Brain Configuration:" << std::endl;
    std::cout << "  --neuron_count=<int>          Total number of neurons (default: 1000)" << std::endl;
    std::cout << "  --region_count=<int>           Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --connection_probability=<float> Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --simulation_timestep=<float>   Simulation timestep in seconds (default: 0.001)" << std::endl;
    std::cout << std::endl;
    
    // Plasticity Configuration
    std::cout << "Plasticity Configuration:" << std::endl;
    std::cout << "  --stdp_ltp_weight=<float>       STDP LTP weight (default: 0.01)" << std::endl;
    std::cout << "  --stdp_ltd_weight=<float>       STDP LTD weight (default: 0.012)" << std::endl;
    std::cout << "  --stdp_time_constant=<float>    STDP time constant (default: 20.0)" << std::endl;
    std::cout << "  --plasticity_learning_rate=<float> Overall plasticity rate (default: 0.01)" << std::endl;
    std::cout << std::endl;
    
    // Development Configuration
    std::cout << "Development Configuration:" << std::endl;
    std::cout << "  --development_synaptogenesis_rate=<float> Synaptogenesis rate (default: 0.001)" << std::endl;
    std::cout << "  --development_pruning_rate=<float>   Pruning rate (default: 0.0001)" << std::endl;
    std::cout << std::endl;
    
    // Neuromodulation Configuration
    std::cout << "Neuromodulation Configuration:" << std::endl;
    std::cout << "  --dopamine_baseline=<float>     Dopamine baseline level (default: 0.1)" << std::endl;
    std::cout << "  --curiosity_baseline=<float>    Curiosity baseline (default: 0.5)" << std::endl;
    std::cout << "  --novelty_threshold=<float>     Novelty detection threshold (default: 0.3)" << std::endl;
    std::cout << "  --reward_discount_factor=<float> Reward discount factor (default: 0.99)" << std::endl;
    std::cout << std::endl;
    
    // Memory Configuration
    std::cout << "Memory Configuration:" << std::endl;
    std::cout << "  --working_memory_capacity=<int>  Working memory capacity (default: 100)" << std::endl;
    std::cout << "  --episodic_memory_max_episodes=<int> Max episodic memory episodes (default: 1000)" << std::endl;
    std::cout << "  --replay_interval=<int>         Replay every N steps (default: 100)" << std::endl;
    std::cout << "  --consolidation_interval=<int>  Memory consolidation interval (default: 1000)" << std::endl;
    std::cout << std::endl;
    
    // Cognition Configuration
    std::cout << "Cognition Configuration:" << std::endl;
    std::cout << "  --planning_depth=<int>           Planning lookahead depth (default: 5)" << std::endl;
    std::cout << "  --attention_inhibition_strength=<float> Attention inhibition (default: 0.5)" << std::endl;
    std::cout << "  --attention_excitation_strength=<float> Attention excitation (default: 1.5)" << std::endl;
    std::cout << "  --concept_learning_rate=<float>  Concept formation learning rate (default: 0.01)" << std::endl;
    std::cout << std::endl;
    
    // Environment Configuration
    std::cout << "Environment Configuration:" << std::endl;
    std::cout << "  --environment_name=<string>      Environment name (default: GridWorld)" << std::endl;
    std::cout << "  --environment_width=<int>        Environment width (default: 10)" << std::endl;
    std::cout << "  --environment_height=<int>       Environment height (default: 10)" << std::endl;
    std::cout << std::endl;
    
    // Performance Configuration
    std::cout << "Performance Configuration:" << std::endl;
    std::cout << "  --checkpoint_dir=<string>        Directory for checkpoints (default: ./checkpoints)" << std::endl;
    std::cout << "  --checkpoint_interval=<int>      Checkpoint every N steps (default: 1000)" << std::endl;
    std::cout << "  --enable_memory_pools=<bool>     Enable memory pools (default: true)" << std::endl;
    std::cout << "  --enable_event_queues=<bool>     Enable event queues (default: true)" << std::endl;
    std::cout << "  --enable_parallel_processing=<bool> Enable parallel processing (default: false)" << std::endl;
    std::cout << "  --enable_simd_vectorization=<bool> Enable SIMD vectorization (default: true)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Usage Examples:" << std::endl;
    std::cout << "  nlm --neuron_count=500 --region_count=1 --connection_probability=0.05" << std::endl;
    std::cout << "  nlm --dopamine_baseline=0.5 --curiosity_baseline=0.8 --novelty_threshold=0.2" << std::endl;
    std::cout << "  nlm --checkpoint_dir=./mybrain --checkpoint_interval=500 --log_level=DEBUG" << std::endl;
    std::cout << "  nlm --working_memory_capacity=1000 --episodic_memory_max_episodes=5000" << std::endl;
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
