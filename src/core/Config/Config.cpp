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

// Validate configuration
bool Config::validateConfig() const {
    std::vector<std::string> errors = getValidationErrors();
    return errors.empty();
}

// Validate specific key
template<typename T>
bool Config::validateKey(const std::string& key, const T& expectedValue) const {
    auto val = get<T>(key);
    return val.has_value() && val.value() == expectedValue;
}

// Get configuration errors
std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Critical validation rules
    if (has("simulation_timestep") && !get<double>("simulation_timestep").has_value()) {
        errors.push_back("Invalid simulation_timestep: must be a positive number");
    } else {
        double timestep = getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0.0) {
            errors.push_back("Invalid simulation_timestep: must be positive (current: " + 
                           std::to_string(timestep) + ")");
        } else if (timestep > 1.0) {
            errors.push_back("Warning: simulation_timestep is large (current: " + 
                           std::to_string(timestep) + ") - may affect simulation accuracy");
        }
    }
    
    if (has("neuron_count") && !get<int>("neuron_count").has_value()) {
        errors.push_back("Invalid neuron_count: must be an integer");
    } else {
        int neuronCount = getOr<int>("neuron_count", 1000);
        if (neuronCount <= 0) {
            errors.push_back("Invalid neuron_count: must be positive (current: " + 
                           std::to_string(neuronCount) + ")");
        } else if (neuronCount > 100000) {
            errors.push_back("Warning: neuron_count is very large (current: " + 
                           std::to_string(neuronCount) + ") - may affect performance");
        }
    }
    
    if (has("connection_probability") && !get<double>("connection_probability").has_value()) {
        errors.push_back("Invalid connection_probability: must be a number");
    } else {
        double connProb = getOr<double>("connection_probability", 0.1);
        if (connProb < 0.0 || connProb > 1.0) {
            errors.push_back("Invalid connection_probability: must be between 0.0 and 1.0 (current: " + 
                           std::to_string(connProb) + ")");
        }
    }
    
    if (has("dopamine_baseline") && !get<double>("dopamine_baseline").has_value()) {
        errors.push_back("Invalid dopamine_baseline: must be a number");
    } else {
        double dopamineBaseline = getOr<double>("dopamine_baseline", 0.1);
        if (dopamineBaseline < 0.0 || dopamineBaseline > 1.0) {
            errors.push_back("Invalid dopamine_baseline: should be between 0.0 and 1.0 (current: " + 
                           std::to_string(dopamineBaseline) + ")");
        }
    }
    
    if (has("environment_width") && !get<int>("environment_width").has_value()) {
        errors.push_back("Invalid environment_width: must be an integer");
    } else {
        int envWidth = getOr<int>("environment_width", 10);
        if (envWidth <= 0) {
            errors.push_back("Invalid environment_width: must be positive (current: " + 
                           std::to_string(envWidth) + ")");
        }
    }
    
    if (has("environment_height") && !get<int>("environment_height").has_value()) {
        errors.push_back("Invalid environment_height: must be an integer");
    } else {
        int envHeight = getOr<int>("environment_height", 10);
        if (envHeight <= 0) {
            errors.push_back("Invalid environment_height: must be positive (current: " + 
                           std::to_string(envHeight) + ")");
        }
    }
    
    if (has("max_simulation_steps") && !get<int>("max_simulation_steps").has_value()) {
        errors.push_back("Invalid max_simulation_steps: must be an integer");
    } else {
        int maxSteps = getOr<int>("max_simulation_steps", 10000);
        if (maxSteps <= 0) {
            errors.push_back("Invalid max_simulation_steps: must be positive (current: " + 
                           std::to_string(maxSteps) + ")");
        }
    }
    
    return errors;
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

// Validation method instantiations
template bool Config::validateKey<int>(const std::string&, const int&) const;
template bool Config::validateKey<double>(const std::string&, const double&) const;
template bool Config::validateKey<std::string>(const std::string&, const std::string&) const;
template bool Config::validateKey<bool>(const std::string&, const bool&) const;

} // namespace nlm
