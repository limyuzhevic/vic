#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "Logger.hpp"

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
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        lineNum++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config format at line " + std::to_string(lineNum) + ": missing '=' in \"" + line + "\"");
            hasErrors = true;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '\"' && value.back() == '\"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Validate key is not empty
        if (key.empty()) {
            NLM_LOG_WARNING("Invalid config at line " + std::to_string(lineNum) + ": empty key");
            hasErrors = true;
            continue;
        }
        
        // Validate value is not empty for non-optional keys
        if (value.empty() && key != "simulation_time_limit" && key != "environment_name" && key != "log_filename") {
            NLM_LOG_WARNING("Invalid config at line " + std::to_string(lineNum) + ": empty value for key \"" + key + "\"");
            hasErrors = true;
            continue;
        }
        
        set(key, value, ConfigSource::File);
        
        // Validate critical configuration values
        validateConfigKey(key, value);
    }
    
    if (hasErrors) {
        NLM_LOG_WARNING("Config file " + filepath + " has " + std::to_string(lineNum) + " lines with parsing errors");
    } else {
        NLM_LOG_INFO("Successfully loaded config file: " + filepath);
    }
    
    return !hasErrors;
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
        std::remove_if(pImpl->entries.begin(),
                      pImpl->entries.end(),
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

void Config::validateConfigKey(const std::string& key, const std::string& value) const {
    // Validate known configuration keys based on their expected types and ranges
    if (key == "random_seed") {
        try {
            uint64_t seed = std::stoull(value);
            if (seed == 0) {
                NLM_LOG_WARNING("Warning: random_seed should be non-zero for true randomness");
            }
        } catch (const std::exception&) {
            NLM_LOG_WARNING("Warning: random_seed value \"" + value + "\" is not a valid unsigned integer");
        }
    } else if (key == "simulation_timestep") {
        try {
            double timestep = std::stod(value);
            if (timestep <= 0.0) {
                NLM_LOG_ERROR("Error: simulation_timestep must be positive, got " + value);
            } else if (timestep > 1.0) {
                NLM_LOG_WARNING("Warning: simulation_timestep " + value + " is very large (simulations will be slow)");
            }
        } catch (const std::exception&) {
            NLM_LOG_WARNING("Warning: simulation_timestep value \"" + value + "\" is not a valid number");
        }
    } else if (key == "neuron_count") {
        try {
            uint64_t count = std::stoull(value);
            if (count == 0) {
                NLM_LOG_WARNING("Warning: neuron_count is zero, brain will have no neurons");
            } else if (count > 100000) {
                NLM_LOG_WARNING("Warning: neuron_count " + value + " is very large, simulation may be slow");
            }
        } catch (const std::exception&) {
            NLM_LOG_WARNING("Warning: neuron_count value \"" + value + "\" is not a valid unsigned integer");
        }
    } else if (key == "connection_probability") {
        try {
            double prob = std::stod(value);
            if (prob < 0.0 || prob > 1.0) {
                NLM_LOG_WARNING("Warning: connection_probability should be between 0.0 and 1.0, got " + value);
            }
        } catch (const std::exception&) {
            NLM_LOG_WARNING("Warning: connection_probability value \"" + value + "\" is not a valid number");
        }
    } else if (key == "plasticity_learning_rate") {
        try {
            double rate = std::stod(value);
            if (rate < 0.0) {
                NLM_LOG_WARNING("Warning: plasticity_learning_rate should be non-negative, got " + value);
            } else if (rate > 1.0) {
                NLM_LOG_WARNING("Warning: plasticity_learning_rate " + value + " is very high (fast learning)");
            }
        } catch (const std::exception&) {
            NLM_LOG_WARNING("Warning: plasticity_learning_rate value \"" + value + "\" is not a valid number");
        }
    } else if (key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || key == "stdp_time_constant") {
        // These are STD parameters, warn if values are unusual
        try {
            double val = std::stod(value);
            if (val < 0.0 && key != "stdp_time_constant") {
                NLM_LOG_WARNING("Warning: " + key + " should be positive, got " + value);
            }
        } catch (const std::exception&) {
            NLM_LOG_WARNING("Warning: " + key + " value \"" + value + "\" is not a valid number");
        }
    } else if (key == "environment_width" || key == "environment_height") {
        try {
            uint64_t size = std::stoull(value);
            if (size == 0) {
                NLM_LOG_WARNING("Warning: " + key + " is zero, environment will be invalid");
            } else if (size > 1000) {
                NLM_LOG_WARNING("Warning: " + key + " " + value + " is very large, environment may be too big");
            }
        } catch (const std::exception&) {
            NLM_LOG_WARNING("Warning: " + key + " value \"" + value + "\" is not a valid unsigned integer");
        }
    }
}

std::string Config::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) const {
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