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
    // PLACEHOLDER - Phase 1 uses a simple key=value format with validation
    
    // Check if filepath is valid
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config::loadFromFile: Empty filepath provided");
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Config::loadFromFile: Failed to open file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Config::loadFromFile: Line " + std::to_string(lineNumber) + " has no '=' separator: " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_ERROR("Config::loadFromFile: Line " + std::to_string(lineNumber) + ": Empty key");
            return false;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Config::loadFromFile: Line " + std::to_string(lineNumber) + ": Error parsing key-value pair: " + e.what());
            return false;
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!argv) {
        NLM_LOG_ERROR("Config::loadFromArgs: argv is null");
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            NLM_LOG_ERROR("Config::loadFromArgs: argv[" + std::to_string(i) + "] is null");
            continue;
        }
        
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_ERROR("Config::loadFromArgs: Invalid --key=value format: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            if (!argv[i + 1]) {
                NLM_LOG_ERROR("Config::loadFromArgs: argv[" + std::to_string(i + 1) + "] is null");
                continue;
            }
            
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        } else {
            NLM_LOG_ERROR("Config::loadFromArgs: Invalid argument format: " + arg);
        }
    }
    return true;
}

// Enhanced CLI-specific options
void Config::loadAdvancedCLIOptions(int argc, char** argv) {
    if (!argv) {
        NLM_LOG_ERROR("Config::loadAdvancedCLIOptions: argv is null");
        return;
    }
    
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            NLM_LOG_ERROR("Config::loadAdvancedCLIOptions: argv[" + std::to_string(i) + "] is null");
            continue;
        }
        
        std::string arg = argv[i];
        
        // Logging Control Options
        if (arg.find("--log-level=") == 0) {
            std::string level = arg.substr(12);
            set("log_level", level, ConfigSource::CommandLine);
        } else if (arg.find("--log-file=") == 0) {
            std::string filepath = arg.substr(11);
            set("log_file", filepath, ConfigSource::CommandLine);
        } else if (arg.find("--log-format=") == 0) {
            std::string format = arg.substr(13);
            set("log_format", format, ConfigSource::CommandLine);
        }
        
        // Simulation Control Options
        else if (arg.find("--steps=") == 0) {
            try {
                int steps = std::stoi(arg.substr(8));
                set("simulation_steps", steps, ConfigSource::CommandLine);
            } catch (const std::exception&) {
                NLM_LOG_ERROR("Config::loadAdvancedCLIOptions: Invalid value for --steps: " + arg);
            }
        } else if (arg.find("--timestep=") == 0) {
            try {
                double dt = std::stod(arg.substr(11));
                set("simulation_timestep", dt, ConfigSource::CommandLine);
            } catch (const std::exception&) {
                NLM_LOG_ERROR("Config::loadAdvancedCLIOptions: Invalid value for --timestep: " + arg);
            }
        } else if (arg.find("--time=") == 0) {
            try {
                double time = std::stod(arg.substr(7));
                set("simulation_time", time, ConfigSource::CommandLine);
            } catch (const std::exception&) {
                NLM_LOG_ERROR("Config::loadAdvancedCLIOptions: Invalid value for --time: " + arg);
            }
        } else if (arg.find("--stop-at=") == 0) {
            std::string condition = arg.substr(10);
            set("simulation_stop_condition", condition, ConfigSource::CommandLine);
        }
        
        // Experiment Control Options
        else if (arg.find("--experiment=") == 0) {
            std::string experiment = arg.substr(13);
            set("experiment_name", experiment, ConfigSource::CommandLine);
        } else if (arg.find("--trial=") == 0) {
            try {
                int trial = std::stoi(arg.substr(8));
                set("experiment_trial", trial, ConfigSource::CommandLine);
            } catch (const std::exception&) {
                NLM_LOG_ERROR("Config::loadAdvancedCLIOptions: Invalid value for --trial: " + arg);
            }
        } else if (arg.find("--random-seed=") == 0) {
            try {
                uint64_t seed = std::stoull(arg.substr(14));
                set("random_seed", static_cast<int64_t>(seed), ConfigSource::CommandLine);
            } catch (const std::exception&) {
                NLM_LOG_ERROR("Config::loadAdvancedCLIOptions: Invalid value for --random-seed: " + arg);
            }
        }
        
        // Output Control Options
        else if (arg.find("--output-format=") == 0) {
            std::string format = arg.substr(17);
            set("output_format", format, ConfigSource::CommandLine);
        } else if (arg.find("--output-file=") == 0) {
            std::string filepath = arg.substr(15);
            set("output_file", filepath, ConfigSource::CommandLine);
        } else if (arg == "--quiet") {
            set("output_quiet", true, ConfigSource::CommandLine);
        } else if (arg == "--verbose") {
            set("output_verbose", true, ConfigSource::CommandLine);
        }
        
        // Debugging & Analysis Options
        else if (arg == "--debug") {
            set("debug_mode", true, ConfigSource::CommandLine);
        } else if (arg == "--profile") {
            set("enable_profiling", true, ConfigSource::CommandLine);
        } else if (arg == "--benchmark") {
            set("run_benchmark", true, ConfigSource::CommandLine);
        } else if (arg == "--validate") {
            set("validate_only", true, ConfigSource::CommandLine);
        } else if (arg == "--dry-run") {
            set("dry_run", true, ConfigSource::CommandLine);
        }
        
        // Visualization & Monitoring Options
        else if (arg == "--monitor") {
            set("enable_monitor", true, ConfigSource::CommandLine);
        } else if (arg.find("--watch=") == 0) {
            try {
                int interval = std::stoi(arg.substr(8));
                set("monitor_interval", interval, ConfigSource::CommandLine);
            } catch (const std::exception&) {
                NLM_LOG_ERROR("Config::loadAdvancedCLIOptions: Invalid value for --watch: " + arg);
            }
        } else if (arg == "--neural-visualization") {
            set("enable_neural_visualization", true, ConfigSource::CommandLine);
        } else if (arg == "--memory-visualization") {
            set("enable_memory_visualization", true, ConfigSource::CommandLine);
        }
        
        // Batch Processing Options
        else if (arg == "--batch") {
            set("batch_mode", true, ConfigSource::CommandLine);
        } else if (arg.find("--batch-config=") == 0) {
            std::string filepath = arg.substr(15);
            set("batch_config_file", filepath, ConfigSource::CommandLine);
        } else if (arg.find("--parallel=") == 0) {
            try {
                int parallel = std::stoi(arg.substr(11));
                set("parallel_simulations", parallel, ConfigSource::CommandLine);
            } catch (const std::exception&) {
                NLM_LOG_ERROR("Config::loadAdvancedCLIOptions: Invalid value for --parallel: " + arg);
            }
        }
    }
}

// Helper function to get log level from string
LogLevel Config::getLogLevel(const std::string& levelStr) {
    std::string level = toLower(levelStr);
    if (level == "debug") return LogLevel::Debug;
    if (level == "info") return LogLevel::Info;
    if (level == "warning") return LogLevel::Warning;
    if (level == "error") return LogLevel::Error;
    if (level == "critical") return LogLevel::Critical;
    return LogLevel::Info; // Default
}

// Plugin configuration implementation
bool Config::configurePlugin(const std::string& pluginName, const std::string& config) {
    set("plugin_" + pluginName + "_config", config, ConfigSource::Runtime);
    NLM_LOG_INFO("Config::configurePlugin: Configured plugin " + pluginName + " with config: " + config);
    return true;
}

std::string Config::getPluginConfig(const std::string& pluginName, const std::string& configKey) const {
    std::string fullKey = "plugin_" + pluginName + "_config_" + configKey;
    auto val = get<std::string>(fullKey);
    return val.has_value() ? val.value() : "";
}

bool Config::loadPluginConfig(const std::string& pluginName, const std::string& configFile) {
    std::string filepath = configFile.empty() ? ("plugins/" + pluginName + ".json") : configFile;
    if (loadFromFile(filepath)) {
        NLM_LOG_INFO("Config::loadPluginConfig: Loaded plugin config from " + filepath);
        return true;
    }
    return false;
}

bool Config::savePluginConfig(const std::string& pluginName, const std::string& configFile) {
    std::string filepath = configFile.empty() ? ("plugins/" + pluginName + ".json") : configFile;
    if (saveToFile(filepath)) {
        NLM_LOG_INFO("Config::savePluginConfig: Saved plugin config to " + filepath);
        return true;
    }
    return false;
}

// Helper function to check boolean config value
bool Config::getBool(const std::string& key, bool defaultValue) const {
    auto val = get<bool>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Helper function to get string config value
std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    auto val = get<std::string>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Helper function to get integer config value
int Config::getInt(const std::string& key, int defaultValue) const {
    auto val = get<int>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Helper function to get double config value
double Config::getDouble(const std::string& key, double defaultValue) const {
    auto val = get<double>(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Check if filepath is valid
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config::saveToFile: Empty filepath provided");
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Config::saveToFile: Failed to open file for writing: " + filepath);
        return false;
    }
    
    try {
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Config::saveToFile: Exception writing to file: " + e.what());
        return false;
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
