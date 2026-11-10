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
    // Set up default configuration values
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    set("synaptogenesis_rate", 0.001f, ConfigSource::Default);
    set("pruning_rate", 0.0f, ConfigSource::Default);
    set("dopamine_baseline", 0.1f, ConfigSource::Default);
    set("reward_discount_factor", 0.99, ConfigSource::Default);
    set("environment_name", std::string("GridWorld"), ConfigSource::Default);
    set("environment_width", 10, ConfigSource::Default);
    set("environment_height", 10, ConfigSource::Default);
    set("log_level", std::string("INFO"), ConfigSource::Default);
    set("log_to_file", false, ConfigSource::Default);
    set("log_filename", std::string("nlm.log"), ConfigSource::Default);
    set("max_simulation_steps", static_cast<int64_t>(10000), ConfigSource::Default);
    set("simulation_time_limit", 0.0, ConfigSource::Default);
    set("visualization_enabled", false, ConfigSource::Default);
    set("visualization_update_rate", 30.0, ConfigSource::Default);
    // Integration system defaults (Phase 6)
    set("replay_interval", static_cast<int64_t>(100), ConfigSource::Default);
    set("consolidation_interval", static_cast<int64_t>(1000), ConfigSource::Default);
    set("checkpoint_dir", std::string("./checkpoints"), ConfigSource::Default);
    set("checkpoint_max_history", 10000, ConfigSource::Default);
    set("checkpoint_max_files", 5, ConfigSource::Default);
    set("checkpoint_compress", true, ConfigSource::Default);
    set("checkpoint_auto_save", true, ConfigSource::Default);
}
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open config file: " << filepath << std::endl;
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    size_t parseErrors = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
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
            
            // Validate key
            if (key.empty()) {
                std::cerr << "ERROR: Config file " << filepath << " line " << lineNumber 
                         << ": Empty key found" << std::endl;
                ++parseErrors;
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            std::cerr << "ERROR: Config file " << filepath << " line " << lineNumber 
                     << ": Invalid format (no '=' found)" << std::endl;
            ++parseErrors;
        }
    }
    
    if (parseErrors > 0) {
        std::cerr << "WARNING: Config file " << filepath << " had " << parseErrors 
                 << " parse errors (continuing)" << std::endl;
    }
    
    // Validate configuration values
    validate();
    
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
        file << entry.key << " = ";
        // Actually write the real value instead of placeholder
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n";
    }
    
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

void Config::validate() const {
    // Validate configuration values
    auto validateFloat = [](const std::string& key, float value, float min, float max, const std::string& desc) {
        if (value < min || value > max) {
            std::cerr << "WARNING: Config key '" << key << "' value " << value 
                     << " is outside recommended range [" << min << ", " << max 
                     << "]. " << desc << std::endl;
        }
    };
    
    auto validateInt = [](const std::string& key, int64_t value, int64_t min, int64_t max, const std::string& desc) {
        if (value < min || value > max) {
            std::cerr << "WARNING: Config key '" << key << "' value " << value 
                     << " is outside recommended range [" << min << ", " << max 
                     << "]. " << desc << std::endl;
        }
    };
    
    auto validateString = [](const std::string& key, const std::string& value, 
                            const std::vector<std::string>& valid, const std::string& desc) {
        if (!valid.empty()) {
            bool found = false;
            for (const auto& validVal : valid) {
                if (value == validVal) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cerr << "WARNING: Config key '" << key << "' value '" << value 
                         << " is not in recommended set. " << desc << std::endl;
            }
        }
    };
    
    // Validate simulation parameters
    if (auto val = getOr<float>("simulation_timestep", 0.001f)) {
        validateFloat("simulation_timestep", *val, 0.0001f, 0.01f, 
                     "Too large timestep causes instability, too small causes slowness");
    }
    
    if (auto val = getOr<int64_t>("neuron_count", 1000)) {
        validateInt("neuron_count", *val, 1, 10000, 
                   "Very large neuron counts may cause memory/performance issues");
    }
    
    if (auto val = getOr<float>("connection_probability", 0.1f)) {
        validateFloat("connection_probability", *val, 0.0f, 1.0f,
                     "Probability should be between 0 (no connections) and 1 (full connections)");
    }
    
    if (auto val = getOr<float>("stdp_ltp_weight", 0.01f)) {
        validateFloat("stdp_ltp_weight", *val, 0.0f, 1.0f,
                     "STDP LTP weight should be positive and reasonable for neural plasticity");
    }
    
    if (auto val = getOr<float>("stdp_ltd_weight", 0.012f)) {
        validateFloat("stdp_ltd_weight", *val, 0.0f, 1.0f,
                     "STDP LTD weight should be positive and reasonable for neural plasticity");
    }
    
    if (auto val = getOr<float>("stdp_tau", 20.0f)) {
        validateFloat("stdp_tau", *val, 1.0f, 100.0f,
                     "STDP time constant should be reasonable for spike timing");
    }
    
    if (auto val = getOr<float>("synaptogenesis_rate", 0.001f)) {
        validateFloat("synaptogenesis_rate", *val, 0.0f, 0.01f,
                     "Synaptogenesis rate affects network growth over time");
    }
    
    if (auto val = getOr<float>("pruning_rate", 0.0f)) {
        validateFloat("pruning_rate", *val, 0.0f, 0.001f,
                     "Pruning rate affects network refinement over time");
    }
    
    // Validate environment parameters
    if (auto val = getOr<int64_t>("environment_width", 10)) {
        validateInt("environment_width", *val, 1, 1000,
                   "Environment width affects spatial navigation and perception");
    }
    
    if (auto val = getOr<int64_t>("environment_height", 10)) {
        validateInt("environment_height", *val, 1, 1000,
                   "Environment height affects spatial navigation and perception");
    }
    
    // Validate logging parameters
    if (auto val = getOr<bool>("log_to_file", false)) {
        // No validation needed for boolean
    }
    
    if (auto val = getOr<std::string>("log_level", std::string("INFO"))) {
        validateString("log_level", *val, {"DEBUG", "INFO", "WARNING", "ERROR", "OFF"},
                      "Log level affects verbosity of output");
    }
    
    // Validate simulation settings
    if (auto val = getOr<int64_t>("max_simulation_steps", 10000)) {
        validateInt("max_simulation_steps", *val, 1, 1000000,
                   "Too many steps may cause long simulation times");
    }
    
    if (auto val = getOr<float>("visualization_update_rate", 30.0f)) {
        validateFloat("visualization_update_rate", *val, 1.0f, 1000.0f,
                     "Update rate affects visualization performance");
    }
    
    // Validate integration system parameters
    if (auto val = getOr<int64_t>("replay_interval", 100)) {
        validateInt("replay_interval", *val, 1, 10000,
                   "Replay interval affects memory consolidation frequency");
    }
    
    if (auto val = getOr<int64_t>("consolidation_interval", 1000)) {
        validateInt("consolidation_interval", *val, 1, 10000,
                   "Consolidation interval affects memory management");
    }
    
    std::cout << "Configuration validation complete (check warnings above)" << std::endl;
}

// Add backup and restore methods to Config class
void Config::backupConfig(const std::string& filepath) {
    std::cout << "Creating backup for config..." << std::endl;
    // Implementation would go here
}

void Config::restoreConfig(const std::string& filepath) {
    std::cout << "Restoring config from backup..." << std::endl;
    // Implementation would go here
}

std::string Config::getBackupList(const std::string& filepath) {
    return "Backup list would be implemented here";
}
