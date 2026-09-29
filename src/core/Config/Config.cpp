#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::shared_ptr<ConfigJSONParser> jsonParser;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->jsonParser = std::make_shared<ConfigJSONParser>();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Use JSON parser for JSON/YAML files, fallback to simple format for .cfg files
    if (pImpl->jsonParser) {
        // Try JSON/YAML first
        bool success = pImpl->jsonParser->parseFromFile(filepath, *this).success;
        if (success) {
            return true;
        }
        
        // If JSON/YAML fails and it's a .cfg file, try simple format for backward compatibility
        if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".cfg") {
            return pImpl->jsonParser->parseSimpleFormatFromFile(filepath, *this);
        }
        
        return false;
    }
    return false;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Use JSON parser to load from args
    if (pImpl->jsonParser) {
        return pImpl->jsonParser->parseFromString("", *this).success;
    }
    return false;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Use JSON parser for JSON/YAML files, simple format for .cfg files
    if (pImpl->jsonParser) {
        // Try JSON/YAML first for JSON/YAML files
        if (filepath.size() >= 5 && (filepath.substr(filepath.size() - 5) == ".json" || 
                                    filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml" ||
                                    filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yml")) {
            return pImpl->jsonParser->saveToFile(*this, filepath);
        } else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".cfg") {
            // Use simple format for .cfg files
            return pImpl->jsonParser->saveToSimpleFormat(filepath);
        }
        
        return false;
    }
    return false;
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

std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    // Check for duplicate parameters
    std::map<std::string, size_t> keyCounts;
    for (const auto& entry : pImpl->entries) {
        keyCounts[entry.key]++;
    }
    
    for (const auto& [key, count] : keyCounts) {
        if (count > 1) {
            errors.push_back("Duplicate parameter: '" + key + "' appears multiple times");
        }
    }
    
    // Check for required parameters
    std::vector<std::string> requiredParams = {"neuron_count", "simulation_timestep"};
    for (const auto& param : requiredParams) {
        if (!has(param)) {
            errors.push_back("Missing required parameter: '" + param + "'");
        }
    }
    
    // Validate parameter types and values
    for (const auto& entry : pImpl->entries) {
        bool isValid = true;
        std::string errorMsg;
        
        // Type validation
        if (entry.key == "neuron_count" && !std::holds_alternative<int64_t>(entry.value) && 
            !std::holds_alternative<int>(entry.value)) {
            isValid = false;
            errorMsg = "Parameter 'neuron_count' must be an integer";
        } else if (entry.key == "simulation_timestep" && !std::holds_alternative<double>(entry.value)) {
            isValid = false;
            errorMsg = "Parameter 'simulation_timestep' must be a floating point number";
        } else if (entry.key == "connection_probability" && !std::holds_alternative<double>(entry.value)) {
            isValid = false;
            errorMsg = "Parameter 'connection_probability' must be a floating point number";
        } else if (entry.key == "random_seed" && !std::holds_alternative<int64_t>(entry.value) && 
                   !std::holds_alternative<int>(entry.value)) {
            isValid = false;
            errorMsg = "Parameter 'random_seed' must be an integer";
        } else if (entry.key == "log_to_file" && !std::holds_alternative<bool>(entry.value)) {
            isValid = false;
            errorMsg = "Parameter 'log_to_file' must be a boolean";
        }
        
        // Value validation
        if (isValid) {
            if (entry.key == "neuron_count") {
                int64_t neurons = std::holds_alternative<int64_t>(entry.value) ? 
                                  std::get<int64_t>(entry.value) : std::get<int>(entry.value);
                if (neurons <= 0) {
                    isValid = false;
                    errorMsg = "Parameter 'neuron_count' must be positive";
                }
            } else if (entry.key == "simulation_timestep") {
                double timestep = std::get<double>(entry.value);
                if (timestep <= 0.0) {
                    isValid = false;
                    errorMsg = "Parameter 'simulation_timestep' must be positive";
                }
            } else if (entry.key == "connection_probability") {
                double prob = std::get<double>(entry.value);
                if (prob < 0.0 || prob > 1.0) {
                    isValid = false;
                    errorMsg = "Parameter 'connection_probability' must be between 0.0 and 1.0";
                }
            }
        }
        
        if (!isValid) {
            errors.push_back("Parameter '" + entry.key + "': " + errorMsg);
        }
    }
    
    return errors;
}

bool Config::loadFromJSON(const std::string& jsonString) {
    if (pImpl->jsonParser) {
        return pImpl->jsonParser->loadFromJSON(jsonString);
    }
    return false;
}

std::string Config::saveToJSON() const {
    if (pImpl->jsonParser) {
        return pImpl->jsonParser->configToJSON(*this);
    }
    return "{}";
}

std::vector<std::string> Config::getParameterSchema() {
    return std::vector<std::string>{
        "neuron_count (int, required, > 0) - Total number of neurons",
        "region_count (int, optional) - Number of brain regions",
        "simulation_timestep (double, required, > 0) - Simulation timestep (seconds)",
        "connection_probability (double, optional, 0.0-1.0) - Probability of connection between neurons",
        "random_seed (int64_t, optional) - Random seed for reproducible experiments",
        "plasticity_learning_rate (double, optional) - General plasticity learning rate",
        "stdp_ltp_weight (double, optional) - STDP LTP weight",
        "stdp_ltd_weight (double, optional) - STDP LTD weight",
        "stdp_time_constant (double, optional) - STDP time constant (ms)",
        "synaptogenesis_rate (double, optional) - Synaptogenesis rate",
        "pruning_rate (double, optional) - Pruning rate",
        "dopamine_baseline (double, optional) - Dopamine baseline level",
        "reward_discount_factor (double, optional) - Reward discount factor",
        "environment_name (string, optional) - Environment name",
        "environment_width (int, optional) - Environment width",
        "environment_height (int, optional) - Environment height",
        "log_level (string, optional) - Log level (DEBUG|INFO|WARNING|ERROR|FATAL)",
        "log_to_file (bool, optional) - Log to file",
        "log_filename (string, optional) - Log filename",
        "max_simulation_steps (int64_t, optional) - Maximum simulation steps",
        "simulation_time_limit (double, optional) - Simulation time limit (seconds)",
        "visualization_enabled (bool, optional) - Enable visualization",
        "visualization_update_rate (double, optional) - Visualization update rate (Hz)"
    };
}

void Config::setJSONParser(std::shared_ptr<ConfigJSONParser> parser) {
    pImpl->jsonParser = parser;
}

std::shared_ptr<ConfigJSONParser> Config::getJSONParser() const {
    return pImpl->jsonParser;
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
