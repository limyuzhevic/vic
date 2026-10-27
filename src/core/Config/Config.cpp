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

// Enhanced validation and error handling
    bool validationErrors = false;
    std::vector<std::string> errorMessages;
    
    for (const auto& entry : pImpl->entries) {
        std::string error = validateConfigEntry(entry);
        if (!error.empty()) {
            errorMessages.push_back("Config entry \"" + entry.key + "\": " + error);
            validationErrors = true;
        }
    }
    
    if (validationErrors) {
        NLM_LOG_ERROR("Configuration validation failed:");
        for (const auto& error : errorMessages) {
            NLM_LOG_ERROR("  " + error);
        }
        return false;
    }
    
    return true;
}

// Enhanced loadFromArgs with validation and environment variable support
bool Config::loadFromArgs(int argc, char** argv) {
    bool hasConfigFile = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --config=<file> or --config <file>
        if (arg.substr(0, 8) == "--config" || arg == "-c") {
            std::string configFile;
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                configFile = argv[++i];
            }
            
            if (!configFile.empty()) {
                NLM_LOG_INFO("Config file specified via command line: " + configFile);
                // Load from file first
                if (loadFromFile(configFile)) {
                    hasConfigFile = true;
                }
            }
        }
        // Handle --env=PREFIX to load from environment variables
        else if (arg.substr(0, 6) == "--env=") {
            std::string prefix = arg.substr(6);
            loadFromEnvironment(prefix);
        }
        // Handle --validate-only flag
        else if (arg == "--validate-only" || arg == "-v") {
            NLM_LOG_INFO("Validating current configuration...");
            bool valid = validate();
            if (valid) {
                NLM_LOG_INFO("Configuration validation passed.");
                return true;
            } else {
                NLM_LOG_ERROR("Configuration validation failed.");
                return false;
            }
        }
        // Handle --config-summary flag
        else if (arg == "--config-summary" || arg == "-s") {
            NLM_LOG_INFO("=== Current Configuration Summary ===");
            NLM_LOG_INFO(summary());
            NLM_LOG_INFO("=== End of Summary ===");
            return true;
        }
        // Handle standard --key=value format
        else if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && arg.size() > 1 && arg[1] != '-') {
            if (i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    
    return !hasConfigFile;  // Return false if config file was specified but couldn't be loaded
}

// Enhanced saveToFile with better formatting and validation
bool Config::saveToFile(const std::string& filepath) const {
    // Validate configuration before saving
    bool valid = validate();
    if (!valid) {
        NLM_LOG_ERROR("Cannot save configuration: validation failed");
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated on: " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        // Write comment if available
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Write configuration entry with proper formatting
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "  # " << static_cast<int>(entry.source) << "\n\n";
    }
    
    return true;
}

// Validate configuration entries
std::string Config::validateConfigEntry(const ConfigEntry& entry) {
    // Type-specific validation
    if (entry.key == "neuron_count") {
        auto val = std::get_if<int64_t>(&entry.value);
        if (val && (*val <= 0 || *val > 1000000)) {
            return "must be between 1 and 1,000,000";
        }
    }
    else if (entry.key == "simulation_timestep") {
        auto val = std::get_if<double>(&entry.value);
        if (val && (*val <= 0.0 || *val > 1.0)) {
            return "must be between 0 and 1 second";
        }
    }
    else if (entry.key == "connection_probability") {
        auto val = std::get_if<float>(&entry.value);
        if (val && (*val < 0.0f || *val > 1.0f)) {
            return "must be between 0.0 and 1.0";
        }
    }
    else if (entry.key == "random_seed") {
        auto val = std::get_if<int64_t>(&entry.value);
        if (val && (*val < 0 || *val > 2147483647)) {
            return "must be between 0 and 2,147,483,647";
        }
    }
    
    return "";
}

// Validate entire configuration
bool Config::validate() const {
    std::vector<std::string> errors;
    
    // Check for required configuration entries
    const std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count", 
        "connection_probability",
        "simulation_timestep",
        "random_seed"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            errors.push_back("Missing required configuration: " + key);
        }
    }
    
    // Check for conflicting configuration entries
    if (has("neuron_count") && has("region_count")) {
        auto neuronCount = get<int64_t>("neuron_count");
        auto regionCount = get<int64_t>("region_count");
        if (neuronCount && regionCount && *neuronCount < *regionCount) {
            errors.push_back("neuron_count (" + std::to_string(*neuronCount) + ") must be >= region_count (" + std::to_string(*regionCount) + ")");
        }
    }
    
    // Check for valid ranges
    if (has("connection_probability")) {
        auto prob = get<double>("connection_probability");
        if (prob && (*prob < 0.0 || *prob > 1.0)) {
            errors.push_back("connection_probability must be between 0.0 and 1.0");
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestep = get<double>("simulation_timestep");
        if (timestep && (*timestep <= 0.0 || *timestep > 10.0)) {
            errors.push_back("simulation_timestep must be between 0 and 10 seconds");
        }
    }
    
    if (!errors.empty()) {
        NLM_LOG_ERROR("Configuration validation errors:");
        for (const auto& error : errors) {
            NLM_LOG_ERROR("  " + error);
        }
        return false;
    }
    
    return true;
}

// Load configuration from environment variables
void Config::loadFromEnvironment(const std::string& prefix) {
    std::string envPrefix = prefix;
    if (!envPrefix.empty() && envPrefix.back() != '_') {
        envPrefix += "_";
    }
    
    NLM_LOG_INFO("Loading configuration from environment variables with prefix: " + prefix);
    
    // Common environment variable mappings
    const std::vector<std::pair<std::string, std::string>> envMappings = {
        {"NLM_NEURON_COUNT", "neuron_count"},
        {"NLM_REGION_COUNT", "region_count"},
        {"NLM_CONNECTION_PROBABILITY", "connection_probability"},
        {"NLM_SIMULATION_TIMESTEP", "simulation_timestep"},
        {"NLM_RANDOM_SEED", "random_seed"},
        {"NLM_STDP_LTP_WEIGHT", "stdp_ltp_weight"},
        {"NLM_STDP_LTD_WEIGHT", "stdp_ltd_weight"},
        {"NLM_SYNAPTOGENESIS_RATE", "synaptogenesis_rate"},
        {"NLM_PRUNING_RATE", "pruning_rate"},
        {"NLM_DOPAMINE_BASELINE", "dopamine_baseline"},
        {"NLM_LOG_LEVEL", "log_level"}
    };
    
    for (const auto& mapping : envMappings) {
        const std::string& envVar = envPrefix + mapping.first;
        const std::string& configKey = mapping.second;
        
        char* envValue = std::getenv(envVar.c_str());
        if (envValue != nullptr && *envValue != '\0') {
            std::string value = envValue;
            set(configKey, value, ConfigSource::Environment);
            NLM_LOG_INFO("Loaded " + configKey + " from environment variable: " + envVar);
        }
    }
}

// Get current timestamp for configuration files
std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
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
