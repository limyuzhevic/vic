#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::vector<std::string>> categoryIndex;
    std::map<std::string, std::vector<std::string>> subcategoryIndex;
};

// Initialize default configuration values
defineDefaultConfigValues(Config& config) {
    // General settings
    config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default, "Random seed for reproducibility", "general", "seed");
    config.set("simulation_timestep", 0.001, ConfigSource::Default, "Simulation timestep in ms", "performance", "timestep");
    config.set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default, "Number of neurons in network", "neural", "basic");
    config.set("region_count", static_cast<int64_t>(1), ConfigSource::Default, "Number of neural regions", "neural", "basic");
    config.set("connection_probability", 0.15f, ConfigSource::Default, "Probability of synaptic connections", "neural", "connectivity");
    config.set("max_neurons_per_region", static_cast<int64_t>(2000), ConfigSource::Default, "Maximum neurons per region", "neural", "advanced");
    
    // Performance optimization
    config.set("use_multithreading", true, ConfigSource::Default, "Enable multi-threaded processing", "performance", "optimization");
    config.set("thread_count", static_cast<int64_t>(std::thread::hardware_concurrency()), ConfigSource::Default, "Number of threads to use", "performance", "optimization");
    config.set("use_simd", true, ConfigSource::Default, "Enable SIMD instructions", "performance", "optimization");
    config.set("use_memory_pool", true, ConfigSource::Default, "Use memory pooling for performance", "performance", "optimization");
    config.set("min_simulation_steps", static_cast<int64_t>(1), ConfigSource::Default, "Minimum steps for simulation", "performance", "basic");
    config.set("max_simulation_steps", static_cast<int64_t>(1000000), ConfigSource::Default, "Maximum steps for simulation", "performance", "basic");
    
    // STDP parameters
    config.set("stdp_ltp_weight", 0.02f, ConfigSource::Default, "STDP LTP weight factor", "learning", "stdp");
    config.set("stdp_ltd_weight", 0.015f, ConfigSource::Default, "STDP LTD weight factor", "learning", "stdp");
    config.set("stdp_tau", 20.0f, ConfigSource::Default, "STDP time constant (ms)", "learning", "stdp");
    config.set("stdp_window_size", 50.0f, ConfigSource::Default, "STDP window size (ms)", "learning", "stdp");
    config.set("stdp_trace_decay", 0.01f, ConfigSource::Default, "STDP trace decay rate", "learning", "stdp");
    
    // Hebbian plasticity
    config.set("hebbian_learning_rate", 0.01f, ConfigSource::Default, "Hebbian learning rate", "learning", "hebbian");
    config.set("hebbian_threshold", 0.1f, ConfigSource::Default, "Hebbian activation threshold", "learning", "hebbian");
    
    // Structural plasticity
    config.set("synaptogenesis_rate", 0.0001f, ConfigSource::Default, "Rate of new synapse formation", "learning", "structural");
    config.set("pruning_rate", 0.00001f, ConfigSource::Default, "Rate of synapse removal", "learning", "structural");
    config.set("max_synapses_per_neuron", static_cast<int64_t>(1000), ConfigSource::Default, "Maximum synapses per neuron", "learning", "structural");
    
    // Memory systems
    config.set("use_working_memory", true, ConfigSource::Default, "Enable working memory system", "memory", "memory_systems");
    config.set("working_memory_capacity", static_cast<int64_t>(100), ConfigSource::Default, "Working memory capacity (items)", "memory", "memory_systems");
    config.set("use_episodic_memory", true, ConfigSource::Default, "Enable episodic memory system", "memory", "memory_systems");
    config.set("episodic_memory_capacity", static_cast<int64_t>(1000), ConfigSource::Default, "Episodic memory capacity (events)", "memory", "memory_systems");
    config.set("use_associative_memory", true, ConfigSource::Default, "Enable associative memory system", "memory", "memory_systems");
    
    // Development system
    config.set("enable_development", false, ConfigSource::Default, "Enable developmental stages", "development", "development");
    config.set("maturation_rate", 0.001f, ConfigSource::Default, "Rate of neural maturation", "development", "development");
    config.set("pruning_threshold", 0.1f, ConfigSource::Default, "Synaptic strength threshold for pruning", "development", "development");
    
    // Neuromodulation
    config.set("enable_neuromodulation", false, ConfigSource::Default, "Enable neuromodulation system", "neuromodulation", "neuromodulation");
    config.set("dopamine_level", 0.5f, ConfigSource::Default, "Baseline dopamine level", "neuromodulation", "neuromodulation");
    config.set("curiosity_strength", 0.3f, ConfigSource::Default, "Curiosity-driven exploration strength", "neuromodulation", "neuromodulation");
    config.set("novelty_threshold", 0.7f, ConfigSource::Default, "Novelty detection threshold", "neuromodulation", "neuromodulation");
    
    // Experimental features
    config.set("enable_curiosity", false, ConfigSource::Default, "Enable curiosity-driven exploration", "experimental", "experiment");
    config.set("prediction_error_weight", 0.5f, ConfigSource::Default, "Prediction error weighting factor", "experimental", "experiment");
    
    // Debugging and logging
    config.set("log_level", "info", ConfigSource::Default, "Log level (debug, info, warning, error, critical)", "debugging", "verbosity");
    config.set("enable_trace", false, ConfigSource::Default, "Enable detailed trace logging", "debugging", "debug");
    config.set("log_file", "", ConfigSource::Default, "Log output file (empty for console)", "debugging", "logging");
    config.set("performance_profiling", false, ConfigSource::Default, "Enable performance profiling", "debugging", "debug");
    
    // Session management
    config.set("checkpoint_interval", static_cast<int64_t>(10000), ConfigSource::Default, "Steps between checkpoints", "session", "checkpoint");
    config.set("max_memory_checkpoints", static_cast<int64_t>(10), ConfigSource::Default, "Maximum checkpoints to keep in memory", "session", "checkpoint");
    config.set("auto_save_interval", static_cast<int64_t>(0), ConfigSource::Default, "Auto-save interval in steps (0=disabled)", "session", "checkpoint");
    config.set("checkpoint_format", "binary", ConfigSource::Default, "Checkpoint file format (binary/text)", "session", "checkpoint");
    
    // Input/Output
    config.set("input_file", "", ConfigSource::Default, "Input data file path", "input", "basic");
    config.set("output_dir", "", ConfigSource::Default, "Output directory for results", "output", "basic");
    config.set("export_weights", false, ConfigSource::Default, "Export synaptic weights to file", "output", "basic");
    config.set("verbose_output", false, ConfigSource::Default, "Enable verbose output mode", "debugging", "verbosity");
    config.set("quiet_mode", false, ConfigSource::Default, "Suppress all non-essential output", "debugging", "verbosity");
    
    // Advanced neuron parameters
    config.set("neuron_threshold", -55.0f, ConfigSource::Default, "Neuron firing threshold (mV)", "neural", "advanced");
    config.set("neuron_reset_potential", -70.0f, ConfigSource::Default, "Neuron reset potential (mV)", "neural", "advanced");
    config.set("neuron_leak_coefficient", 0.1f, ConfigSource::Default, "Neuron membrane leak coefficient", "neural", "advanced");
    config.set("neuron_refractory_period", 2.0f, ConfigSource::Default, "Neuron refractory period (ms)", "neural", "advanced");
    config.set("initial_membrane_potential", -60.0f, ConfigSource::Default, "Initial membrane potential (mV)", "neural", "advanced");
    
    // Advanced synaptic parameters
    config.set("synaptic_delay_min", 1, ConfigSource::Default, "Minimum synaptic delay (ms)", "neural", "advanced");
    config.set("synaptic_delay_max", 10, ConfigSource::Default, "Maximum synaptic delay (ms)", "neural", "advanced");
    config.set("synaptic_noise_level", 0.0f, ConfigSource::Default, "Synaptic transmission noise level", "neural", "advanced");
    config.set("plastic_synapse_prob", 0.3f, ConfigSource::Default, "Probability of plastic synapses", "learning", "plasticity");
    config.set("static_synapse_prob", 0.7f, ConfigSource::Default, "Probability of static synapses", "learning", "plasticity");
}

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <thread>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::vector<std::string>> categoryIndex;
    std::map<std::string, std::vector<std::string>> subcategoryIndex;
};

// Config::Impl private helper
void initializeDefaultConfigValues(Config::Impl* impl) {
    ConfigEntry entries[] = {
        // General settings
        ConfigEntry("random_seed", int64_t(42), ConfigSource::Default, "Random seed for reproducibility", "general", "seed"),
        ConfigEntry("simulation_timestep", 0.001, ConfigSource::Default, "Simulation timestep in ms", "performance", "timestep"),
        ConfigEntry("neuron_count", int64_t(500), ConfigSource::Default, "Number of neurons in network", "neural", "basic"),
        ConfigEntry("region_count", int64_t(1), ConfigSource::Default, "Number of neural regions", "neural", "basic"),
        ConfigEntry("connection_probability", 0.15f, ConfigSource::Default, "Probability of synaptic connections", "neural", "connectivity"),
        ConfigEntry("max_neurons_per_region", int64_t(2000), ConfigSource::Default, "Maximum neurons per region", "neural", "advanced"),
        
        // Performance optimization
        ConfigEntry("use_multithreading", true, ConfigSource::Default, "Enable multi-threaded processing", "performance", "optimization"),
        ConfigEntry("thread_count", static_cast<int64_t>(std::thread::hardware_concurrency()), ConfigSource::Default, "Number of threads to use", "performance", "optimization"),
        ConfigEntry("use_simd", true, ConfigSource::Default, "Enable SIMD instructions", "performance", "optimization"),
        ConfigEntry("use_memory_pool", true, ConfigSource::Default, "Use memory pooling for performance", "performance", "optimization"),
        ConfigEntry("min_simulation_steps", int64_t(1), ConfigSource::Default, "Minimum steps for simulation", "performance", "basic"),
        ConfigEntry("max_simulation_steps", int64_t(1000000), ConfigSource::Default, "Maximum steps for simulation", "performance", "basic"),
        
        // STDP parameters
        ConfigEntry("stdp_ltp_weight", 0.02f, ConfigSource::Default, "STDP LTP weight factor", "learning", "stdp"),
        ConfigEntry("stdp_ltd_weight", 0.015f, ConfigSource::Default, "STDP LTD weight factor", "learning", "stdp"),
        ConfigEntry("stdp_tau", 20.0f, ConfigSource::Default, "STDP time constant (ms)", "learning", "stdp"),
        ConfigEntry("stdp_window_size", 50.0f, ConfigSource::Default, "STDP window size (ms)", "learning", "stdp"),
        ConfigEntry("stdp_trace_decay", 0.01f, ConfigSource::Default, "STDP trace decay rate", "learning", "stdp"),
        
        // Hebbian plasticity
        ConfigEntry("hebbian_learning_rate", 0.01f, ConfigSource::Default, "Hebbian learning rate", "learning", "hebbian"),
        ConfigEntry("hebbian_threshold", 0.1f, ConfigSource::Default, "Hebbian activation threshold", "learning", "hebbian"),
        
        // Structural plasticity
        ConfigEntry("synaptogenesis_rate", 0.0001f, ConfigSource::Default, "Rate of new synapse formation", "learning", "structural"),
        ConfigEntry("pruning_rate", 0.00001f, ConfigSource::Default, "Rate of synapse removal", "learning", "structural"),
        ConfigEntry("max_synapses_per_neuron", int64_t(1000), ConfigSource::Default, "Maximum synapses per neuron", "learning", "structural"),
        
        // Memory systems
        ConfigEntry("use_working_memory", true, ConfigSource::Default, "Enable working memory system", "memory", "memory_systems"),
        ConfigEntry("working_memory_capacity", int64_t(100), ConfigSource::Default, "Working memory capacity (items)", "memory", "memory_systems"),
        ConfigEntry("use_episodic_memory", true, ConfigSource::Default, "Enable episodic memory system", "memory", "memory_systems"),
        ConfigEntry("episodic_memory_capacity", int64_t(1000), ConfigSource::Default, "Episodic memory capacity (events)", "memory", "memory_systems"),
        ConfigEntry("use_associative_memory", true, ConfigSource::Default, "Enable associative memory system", "memory", "memory_systems"),
        
        // Development system
        ConfigEntry("enable_development", false, ConfigSource::Default, "Enable developmental stages", "development", "development"),
        ConfigEntry("maturation_rate", 0.001f, ConfigSource::Default, "Rate of neural maturation", "development", "development"),
        ConfigEntry("pruning_threshold", 0.1f, ConfigSource::Default, "Synaptic strength threshold for pruning", "development", "development"),
        
        // Neuromodulation
        ConfigEntry("enable_neuromodulation", false, ConfigSource::Default, "Enable neuromodulation system", "neuromodulation", "neuromodulation"),
        ConfigEntry("dopamine_level", 0.5f, ConfigSource::Default, "Baseline dopamine level", "neuromodulation", "neuromodulation"),
        ConfigEntry("curiosity_strength", 0.3f, ConfigSource::Default, "Curiosity-driven exploration strength", "neuromodulation", "neuromodulation"),
        ConfigEntry("novelty_threshold", 0.7f, ConfigSource::Default, "Novelty detection threshold", "neuromodulation", "neuromodulation"),
        
        // Experimental features
        ConfigEntry("enable_curiosity", false, ConfigSource::Default, "Enable curiosity-driven exploration", "experimental", "experiment"),
        ConfigEntry("prediction_error_weight", 0.5f, ConfigSource::Default, "Prediction error weighting factor", "experimental", "experiment"),
        
        // Debugging and logging
        ConfigEntry("log_level", std::string("info"), ConfigSource::Default, "Log level (debug, info, warning, error, critical)", "debugging", "verbosity"),
        ConfigEntry("enable_trace", false, ConfigSource::Default, "Enable detailed trace logging", "debugging", "debug"),
        ConfigEntry("log_file", std::string(""), ConfigSource::Default, "Log output file (empty for console)", "debugging", "logging"),
        ConfigEntry("performance_profiling", false, ConfigSource::Default, "Enable performance profiling", "debugging", "debug"),
        
        // Session management
        ConfigEntry("checkpoint_interval", int64_t(10000), ConfigSource::Default, "Steps between checkpoints", "session", "checkpoint"),
        ConfigEntry("max_memory_checkpoints", int64_t(10), ConfigSource::Default, "Maximum checkpoints to keep in memory", "session", "checkpoint"),
        ConfigEntry("auto_save_interval", int64_t(0), ConfigSource::Default, "Auto-save interval in steps (0=disabled)", "session", "checkpoint"),
        ConfigEntry("checkpoint_format", std::string("binary"), ConfigSource::Default, "Checkpoint file format (binary/text)", "session", "checkpoint"),
        
        // Input/Output
        ConfigEntry("input_file", std::string(""), ConfigSource::Default, "Input data file path", "input", "basic"),
        ConfigEntry("output_dir", std::string(""), ConfigSource::Default, "Output directory for results", "output", "basic"),
        ConfigEntry("export_weights", false, ConfigSource::Default, "Export synaptic weights to file", "output", "basic"),
        ConfigEntry("verbose_output", false, ConfigSource::Default, "Enable verbose output mode", "debugging", "verbosity"),
        ConfigEntry("quiet_mode", false, ConfigSource::Default, "Suppress all non-essential output", "debugging", "verbosity"),
        
        // Advanced neuron parameters
        ConfigEntry("neuron_threshold", -55.0f, ConfigSource::Default, "Neuron firing threshold (mV)", "neural", "advanced"),
        ConfigEntry("neuron_reset_potential", -70.0f, ConfigSource::Default, "Neuron reset potential (mV)", "neural", "advanced"),
        ConfigEntry("neuron_leak_coefficient", 0.1f, ConfigSource::Default, "Neuron membrane leak coefficient", "neural", "advanced"),
        ConfigEntry("neuron_refractory_period", 2.0f, ConfigSource::Default, "Neuron refractory period (ms)", "neural", "advanced"),
        ConfigEntry("initial_membrane_potential", -60.0f, ConfigSource::Default, "Initial membrane potential (mV)", "neural", "advanced"),
        
        // Advanced synaptic parameters
        ConfigEntry("synaptic_delay_min", 1, ConfigSource::Default, "Minimum synaptic delay (ms)", "neural", "advanced"),
        ConfigEntry("synaptic_delay_max", 10, ConfigSource::Default, "Maximum synaptic delay (ms)", "neural", "advanced"),
        ConfigEntry("synaptic_noise_level", 0.0f, ConfigSource::Default, "Synaptic transmission noise level", "neural", "advanced"),
        ConfigEntry("plastic_synapse_prob", 0.3f, ConfigSource::Default, "Probability of plastic synapses", "learning", "plasticity"),
        ConfigEntry("static_synapse_prob", 0.7f, ConfigSource::Default, "Probability of static synapses", "learning", "plasticity"),
    };
    
    impl->entries.assign(std::begin(entries), std::end(entries));
    
    // Update indices
    for (const auto& entry : impl->entries) {
        impl->categoryIndex[entry.category].push_back(entry.key);
        if (!entry.subcategory.empty()) {
            impl->subcategoryIndex[entry.subcategory].push_back(entry.key);
        }
    }
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeDefaultConfigValues(pImpl.get());
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
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
    // Build index of entries for faster lookup
    std::map<std::string, ConfigEntry*> keyToEntry;
    for (auto& entry : pImpl->entries) {
        keyToEntry[entry.key] = &entry;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
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
    
    // Save entries organized by categories
    std::map<std::string, std::vector<const ConfigEntry*>> categorizedEntries;
    for (const auto& entry : pImpl->entries) {
        categorizedEntries[entry.category].push_back(&entry);
    }
    
    for (const auto& [category, entries] : categorizedEntries) {
        file << "# " << category << " settings\n";
        for (const auto* entry : entries) {
            file << "# " << entry->description << " (source: " << static_cast<int>(entry->source) << ")\n";
            file << entry->key << " = " << "[value_not_shown]\n";
        }
        file << "\n";
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
    set(key, value, source, "", "general", "");
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description, const std::string& category, const std::string& subcategory) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    bool isNew = (it == pImpl->entries.end());
    
    if (isNew) {
        ConfigEntry newEntry(key, value, source, description, category, subcategory);
        pImpl->entries.push_back(newEntry);
        it = pImpl->entries.end() - 1;
        // Update indices
        pImpl->categoryIndex[category].push_back(key);
        if (!subcategory.empty()) {
            pImpl->subcategoryIndex[subcategory].push_back(key);
        }
    } else {
        it->value = value;
        it->source = source;
        if (!description.empty()) it->description = description;
        if (!category.empty() && it->category != category) {
            // Remove from old category index and add to new one
            for (auto catIt = pImpl->categoryIndex.begin(); catIt != pImpl->categoryIndex.end(); ++catIt) {
                auto& keys = catIt->second;
                keys.erase(std::remove(keys.begin(), keys.end(), key), keys.end());
            }
            pImpl->categoryIndex[category].push_back(key);
            it->category = category;
        }
    }
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        std::string category = it->category;
        std::string subcategory = it->subcategory;
        
        pImpl->entries.erase(it);
        
        // Update indices
        auto& catKeys = pImpl->categoryIndex[category];
        catKeys.erase(std::remove(catKeys.begin(), catKeys.end(), key), catKeys.end());
        
        if (!subcategory.empty()) {
            auto& subcatKeys = pImpl->subcategoryIndex[subcategory];
            subcatKeys.erase(std::remove(subcatKeys.begin(), subcatKeys.end(), key), subcatKeys.end());
        }
    }
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
    pImpl->categoryIndex.clear();
    pImpl->subcategoryIndex.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    // Organize by category
    std::map<std::string, std::vector<const ConfigEntry*>> categorizedEntries;
    for (const auto& entry : pImpl->entries) {
        categorizedEntries[entry.category].push_back(&entry);
    }
    
    for (const auto& [category, entries] : categorizedEntries) {
        oss << "\n=== " << category << " ===\n";
        for (const auto* entry : entries) {
            oss << "  " << entry->key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry->value);
            oss << "] (" << static_cast<int>(entry->source) << ") - " << entry->description << "\n";
        }
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

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
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
    // Build index of entries for faster lookup
    std::map<std::string, ConfigEntry*> keyToEntry;
    for (auto& entry : pImpl->entries) {
        keyToEntry[entry.key] = &entry;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
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
    
    // Save entries organized by categories
    std::map<std::string, std::vector<const ConfigEntry*>> categorizedEntries;
    for (const auto& entry : pImpl->entries) {
        categorizedEntries[entry.category].push_back(&entry);
    }
    
    for (const auto& [category, entries] : categorizedEntries) {
        file << "# " << category << " settings\n";
        for (const auto* entry : entries) {
            file << "# " << entry->description << " (source: " << static_cast<int>(entry->source) << ")\n";
            file << entry->key << " = " << "[value_not_shown]\n";
        }
        file << "\n";
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
    set(key, value, source, "", "general", "");
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description, const std::string& category, const std::string& subcategory) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    bool isNew = (it == pImpl->entries.end());
    
    if (isNew) {
        pImpl->entries.emplace_back(key, value, source, description, category, subcategory);
        // Update indices
        pImpl->categoryIndex[category].push_back(key);
        if (!subcategory.empty()) {
            pImpl->subcategoryIndex[subcategory].push_back(key);
        }
    } else {
        it->value = value;
        it->source = source;
        if (!description.empty()) it->description = description;
        if (!category.empty() && it->category != category) {
            // Remove from old category index and add to new one
            for (auto catIt = pImpl->categoryIndex.begin(); catIt != pImpl->categoryIndex.end(); ++catIt) {
                auto& keys = catIt->second;
                keys.erase(std::remove(keys.begin(), keys.end(), key), keys.end());
            }
            pImpl->categoryIndex[category].push_back(key);
        }
    }
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        std::string category = it->category;
        std::string subcategory = it->subcategory;
        
        pImpl->entries.erase(it);
        
        // Update indices
        auto& catKeys = pImpl->categoryIndex[category];
        catKeys.erase(std::remove(catKeys.begin(), catKeys.end(), key), catKeys.end());
        
        if (!subcategory.empty()) {
            auto& subcatKeys = pImpl->subcategoryIndex[subcategory];
            subcatKeys.erase(std::remove(subcatKeys.begin(), subcatKeys.end(), key), subcatKeys.end());
        }
    }
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
    pImpl->categoryIndex.clear();
    pImpl->subcategoryIndex.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    // Organize by category
    std::map<std::string, std::vector<const ConfigEntry*>> categorizedEntries;
    for (const auto& entry : pImpl->entries) {
        categorizedEntries[entry.category].push_back(&entry);
    }
    
    for (const auto& [category, entries] : categorizedEntries) {
        oss << "\n=== " << category << " ===\n";
        for (const auto* entry : entries) {
            oss << "  " << entry->key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry->value);
            oss << "] (" << static_cast<int>(entry->source) << ") - " << entry->description << "\n";
        }
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
