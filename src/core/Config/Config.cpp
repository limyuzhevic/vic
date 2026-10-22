#include "Config.hpp"
#include "ConfigValidation.hpp"
#include "Exceptions/Exceptions.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>

namespace nlm {

// Known configuration keys and their validation rules
static std::unordered_map<std::string, std::string> knownConfigKeys = {
    {"random_seed", "Random seed for reproducible experiments"},
    {"simulation_timestep", "Simulation timestep in seconds"},
    {"neuron_count", "Number of neurons in the brain"},
    {"region_count", "Number of brain regions"},
    {"connection_probability", "Connection probability for random connectivity"},
    {"plasticity_learning_rate", "Plasticity learning rate"},
    {"stdp_ltp_weight", "STDP LTP weight"},
    {"stdp_ltd_weight", "STDP LTD weight"},
    {"stdp_time_constant", "STDP time constant"},
    {"development_synaptogenesis_rate", "Development synaptogenesis rate"},
    {"development_pruning_rate", "Development pruning rate"},
    {"dopamine_baseline", "Dopamine baseline value"},
    {"reward_discount_factor", "Reward discount factor"},
    {"environment_name", "Environment name"},
    {"environment_width", "Environment width"},
    {"environment_height", "Environment height"},
    {"log_level", "Log level"},
    {"log_to_file", "Log to file"},
    {"log_filename", "Log filename"},
    {"max_simulation_steps", "Maximum simulation steps"},
    {"simulation_time_limit", "Simulation time limit"},
    {"visualization_enabled", "Visualization enabled"},
    {"visualization_update_rate", "Visualization update rate"}
};

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
        throw ConfigFileException(filepath, "Cannot open file");
    }
    
    std::string line;
    int lineNumber = 0;
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
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const ConfigException& e) {
                throw ConfigParseException(key, value, e.what());
            }
        } else {
            throw ConfigParseException("<unknown>", line, "Invalid format (missing '=')");
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
            } else {
                throw ConfigParseException(arg.substr(2), "", "Invalid format (missing '=')");
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
        throw ConfigFileException(filepath, "Cannot open file for writing");
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

// Implementation of ConfigValidation.hpp methods
void ConfigValidator::validateValue(const std::string& key, const ConfigValue& value, 
                                    const std::string& description) {
    // Basic validation for known keys
    if (knownConfigKeys.find(key) == knownConfigKeys.end()) {
        throw ConfigException("Unknown configuration key: " + key);
    }
    
    // Type-specific validation (could be extended)
    if (std::holds_alternative<int64_t>(value)) {
        int64_t val = std::get<int64_t>(value);
        if (key == "random_seed" && val < 0) {
            throw ConfigException("random_seed must be non-negative");
        }
    } else if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        if (key == "simulation_timestep" && val <= 0.0) {
            throw ConfigException("simulation_timestep must be positive");
        } else if (key == "neuron_count" && (val <= 0 || val > 1000000)) {
            throw ConfigException("neuron_count must be between 1 and 1,000,000");
        } else if (key == "region_count" && val <= 0) {
            throw ConfigException("region_count must be positive");
        } else if (key == "connection_probability" && (val < 0.0 || val > 1.0)) {
            throw ConfigException("connection_probability must be between 0.0 and 1.0");
        } else if (key == "plasticity_learning_rate" && val < 0.0) {
            throw ConfigException("plasticity_learning_rate must be non-negative");
        } else if (key == "environment_width" && val <= 0) {
            throw ConfigException("environment_width must be positive");
        } else if (key == "environment_height" && val <= 0) {
            throw ConfigException("environment_height must be positive");
    } else if (std::holds_alternative<bool>(value)) {
        // Boolean values are fine
    } else if (std::holds_alternative<std::string>(value)) {
        std::string val = std::get<std::string>(value);
        if (key == "environment_name" && val.empty()) {
            throw ConfigException("environment_name cannot be empty");
        } else if (key == "log_filename" && val.empty()) {
            throw ConfigException("log_filename cannot be empty");
        }
    }
}

void ConfigValidator::validateRange(const std::string& key, double value, 
                                    double min, double max, const std::string& description) {
    if (value < min || value > max) {
        throw ConfigException("Value " + std::to_string(value) + " for key '" + key + 
                            "' is out of range [" + std::to_string(min) + ", " + 
                            std::to_string(max) + "]");
    }
}

void ConfigValidator::validatePositive(const std::string& key, double value, 
                                      const std::string& description) {
    if (value <= 0.0) {
        throw ConfigException("Value " + std::to_string(value) + " for key '" + key + 
                            "' must be positive");
    }
}

void ConfigValidator::validateNonNegative(const std::string& key, double value, 
                                         const std::string& description) {
    if (value < 0.0) {
        throw ConfigException("Value " + std::to_string(value) + " for key '" + key + 
                            "' must be non-negative");
    }
}

void ConfigValidator::validateEnum(const std::string& key, int64_t value, 
                                  const std::string& description) {
    // Basic enum validation - could be extended
    if (key == "log_level" && (value < 0 || value > 4)) {
        throw ConfigException("log_level must be between 0 and 4 (0=Debug, 1=Info, 2=Warning, 3=Error, 4=Critical)");
    }
}

void ConfigValidator::validateString(const std::string& key, const std::string& value, 
                                    size_t minLength, size_t maxLength, const std::string& description) {
    if (value.size() < minLength) {
        throw ConfigException("Value for key '" + key + "' must be at least " + 
                            std::to_string(minLength) + " characters long");
    }
    if (value.size() > maxLength) {
        throw ConfigException("Value for key '" + key + "' cannot exceed " + 
                            std::to_string(maxLength) + " characters");
    }
}

// Implementation of ConfigHelper methods
std::string ConfigHelper::getValueDescription(const std::string& key) {
    auto it = knownConfigKeys.find(key);
    if (it != knownConfigKeys.end()) {
        return it->second;
    }
    return "Unknown configuration key";
}

std::string ConfigHelper::getErrorPrefix(const ConfigEntry& entry) {
    std::ostringstream oss;
    oss << "[Config " << static_cast<int>(entry.source) << "]";
    return oss.str();
}

void ConfigHelper::setValueWithValidation(const std::string& key, const std::string& value,
                                         ConfigSource source) {
    // This would be called from Config::set() after validation
    // For now, just a placeholder
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

