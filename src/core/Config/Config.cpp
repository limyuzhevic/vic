#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <mutex>

namespace nlm {

// Helper function to load from simple format
bool Config::loadFromSimpleFormat(std::ifstream& file) {
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
        file << entry.key << " = ";
        
        // Write the value based on its type
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
        file << "\n\n";
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
    // Validate the parameter before setting
    if (!validateParameter(key, value)) {
        NLM_LOG_WARNING("Invalid configuration parameter: " + key + " = " + std::visit([](auto&& arg) { return std::to_string(arg); }, value));
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, getParameterDescriptions().at(key));
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Convert string value and validate
    auto convertedValue = parseValueFromString(value, key);
    if (convertedValue) {
        // Validate the converted value
        if (!validateParameter(key, *convertedValue)) {
            NLM_LOG_WARNING("Invalid configuration parameter: " + key + " = " + value);
        }
        set(key, *convertedValue, source);
    } else {
        NLM_LOG_ERROR("Cannot convert configuration value to appropriate type: " + value);
    }
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

// Helper function to load from simple format
bool Config::loadFromSimpleFormat(std::ifstream& file) {
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

// Validate configuration
bool Config::validate() const {
    // Required parameters
    const std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    // Check for required parameters
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            NLM_LOG_ERROR("Missing required configuration parameter: " + key);
            return false;
        }
    }
    
    // Validate specific parameters
    if (has("neuron_count")) {
        auto neurons = get<int64_t>("neuron_count");
        if (neurons && (*neurons <= 0 || *neurons > 100000)) {
            NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(*neurons));
            return false;
        }
    }
    
    if (has("region_count")) {
        auto regions = get<int64_t>("region_count");
        if (regions && (*regions <= 0 || *regions > 100)) {
            NLM_LOG_ERROR("Invalid region_count: " + std::to_string(*regions));
            return false;
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestep = get<double>("simulation_timestep");
        if (timestep && (*timestep <= 0.0 || *timestep > 1.0)) {
            NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(*timestep));
            return false;
        }
    }
    
    return true;
}

// Get parameter descriptions for help
const std::unordered_map<std::string, std::string> Config::getParameterDescriptions() {
    return {
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Connection probability for random connectivity"},
        {"plasticity_learning_rate", "Base learning rate for plasticity"},
        {"stdp_ltp_weight", "STDP long-term potentiation weight"},
        {"stdp_ltd_weight", "STDP long-term depression weight"},
        {"stdp_time_constant", "STDP time constant (ms)"},
        {"development_synaptogenesis_rate", "Developmental synaptogenesis rate"},
        {"development_pruning_rate", "Developmental pruning rate"},
        {"dopamine_baseline", "Dopamine baseline value"},
        {"reward_discount_factor", "Reward discount factor"},
        {"environment_name", "Environment type name"},
        {"environment_width", "Environment width"},
        {"environment_height", "Environment height"},
        {"log_level", "Logging level (Debug, Info, Warning, Error, Critical)"},
        {"log_to_file", "Enable/disable file logging"},
        {"log_filename", "Log file name"},
        {"max_simulation_steps", "Maximum number of simulation steps"},
        {"simulation_time_limit", "Simulation time limit (0 = unlimited)"},
        {"visualization_enabled", "Enable/disable visualization"},
        {"visualization_update_rate", "Visualization update rate (Hz)"}
    };
}

// Validate specific parameter
bool Config::validateParameter(const std::string& key, const ConfigValue& value) const {
    // Parameter-specific validation
    if (key == "random_seed") {
        try {
            return std::get<int64_t>(value) >= 0;
        } catch (const std::bad_variant_access&) { return false; }
    }
    
    if (key == "neuron_count" || key == "region_count") {
        try {
            int64_t val = std::get<int64_t>(value);
            return val > 0 && val <= 100000;
        } catch (const std::bad_variant_access&) { return false; }
    }
    
    if (key == "simulation_timestep") {
        try {
            double val = std::get<double>(value);
            return val > 0.0 && val <= 1.0;
        } catch (const std::bad_variant_access&) { return false; }
    }
    
    if (key == "connection_probability") {
        try {
            double val = std::get<double>(value);
            return val >= 0.0 && val <= 1.0;
        } catch (const std::bad_variant_access&) { return false; }
    }
    
    if (key == "log_level") {
        try {
            std::string val = std::get<std::string>(value);
            return val == "Debug" || val == "Info" || val == "Warning" || val == "Error" || val == "Critical";
        } catch (const std::bad_variant_access&) { return false; }
    }
    
    return true;  // No specific validation for other parameters
}

// Convert string value to target type with validation
template<typename T>
std::optional<T> Config::parseValue(const std::string& str) {
    std::istringstream iss(str);
    T result;
    
    if (iss >> result) {
        // Check if there are any non-whitespace characters after the value
        char remaining;
        if (iss >> std::ws >> remaining) {
            return std::nullopt;  // Extra characters, not a valid number
        }
        return result;
    }
    
    return std::nullopt;
}
