#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stdexcept>

namespace nlm {

using json = nlohmann::json;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string schema;
    std::map<std::string, std::string> descriptions;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with common configuration options
    initializeSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Try to parse as JSON first
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json configJson;
        try {
            configJson = json::parse(file);
            return loadFromJson(configJson);
        } catch (const json::parse_error&) {
            // Fall back to simple key=value format
            file.clear();
            file.seekg(0);
        }
        
        return loadFromSimpleFormat(file);
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading config file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        // First, try to load from simple --key=value and -key value formats
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
        
        // Try to load from JSON file specified by --config
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg.substr(0, 8) == "--config") {
                std::string configFile;
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
                if (!configFile.empty()) {
                    loadFromFile(configFile);
                }
                break;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading config from args: " << e.what() << std::endl;
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        json configJson;
        
        for (const auto& entry : pImpl->entries) {
            configJson[entry.key] = entry.value;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << configJson.dump(2); // Pretty print with 2-space indent
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving config to file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            return std::nullopt;
        }
        
        return std::get<T>(it->value);
        
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    } catch (const std::exception& e) {
        std::cerr << "Error getting config key " << key << ": " << e.what() << std::endl;
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    }
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    try {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
        
        // Validate if schema is present
        if (!pImpl->schema.empty()) {
            validateEntry(key, value);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error setting config key " << key << ": " << e.what() << std::endl;
        throw;
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

bool Config::loadFromJson(const json& configJson) {
    for (auto it = configJson.begin(); it != configJson.end(); ++it) {
        std::string key = it.key();
        ConfigValue value;
        
        if (it->is_string()) {
            value = it->get<std::string>();
        } else if (it->is_number_integer()) {
            value = it->get<int64_t>();
        } else if (it->is_number_float()) {
            value = it->get<double>();
        } else if (it->is_boolean()) {
            value = it->get<bool>();
        } else if (it->is_array()) {
            // Handle array types
            std::vector<std::string> arrayValues;
            for (const auto& item : *it) {
                arrayValues.push_back(item.get<std::string>());
            }
            value = arrayValues;
        }
        
        set(key, value, ConfigSource::File);
    }
    
    return true;
}

bool Config::loadFromSimpleFormat(std::ifstream& file) {
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

void Config::validateEntry(const std::string& key, const ConfigValue& value) {
    // Validate based on schema if present
    if (pImpl->schema.empty()) {
        return;
    }
    
    // Basic validation for common keys
    if (key == "random_seed") {
        auto val = std::get_if<int64_t>(&value);
        if (val && (*val < 0 || *val > 1000000000)) {
            throw std::invalid_argument("random_seed must be between 0 and 1000000000");
        }
    } else if (key == "neuron_count") {
        auto val = std::get_if<int64_t>(&value);
        if (val && (*val < 1 || *val > 10000)) {
            throw std::invalid_argument("neuron_count must be between 1 and 10000");
        }
    } else if (key == "region_count") {
        auto val = std::get_if<int64_t>(&value);
        if (val && (*val < 1 || *val > 100)) {
            throw std::invalid_argument("region_count must be between 1 and 100");
        }
    } else if (key == "connection_probability") {
        auto val = std::get_if<double>(&value);
        if (val && (*val < 0.0 || *val > 1.0)) {
            throw std::invalid_argument("connection_probability must be between 0.0 and 1.0");
        }
    } else if (key == "simulation_timestep") {
        auto val = std::get_if<double>(&value);
        if (val && (*val <= 0.0 || *val > 0.1)) {
            throw std::invalid_argument("simulation_timestep must be between 0.0001 and 0.1");
        }
    } else if (key == "stdp_ltp_weight") {
        auto val = std::get_if<double>(&value);
        if (val && (*val < 0.0 || *val > 1.0)) {
            throw std::invalid_argument("stdp_ltp_weight must be between 0.0 and 1.0");
        }
    } else if (key == "stdp_ltd_weight") {
        auto val = std::get_if<double>(&value);
        if (val && (*val < 0.0 || *val > 1.0)) {
            throw std::invalid_argument("stdp_ltd_weight must be between 0.0 and 1.0");
        }
    } else if (key == "synaptogenesis_rate") {
        auto val = std::get_if<double>(&value);
        if (val && (*val < 0.0 || *val > 1.0)) {
            throw std::invalid_argument("synaptogenesis_rate must be between 0.0 and 1.0");
        }
    } else if (key == "pruning_rate") {
        auto val = std::get_if<double>(&value);
        if (val && (*val < 0.0 || *val > 1.0)) {
            throw std::invalid_argument("pruning_rate must be between 0.0 and 1.0");
        }
    }
}

void Config::initializeSchema() {
    // Initialize with common configuration options and descriptions
    pImpl->descriptions = {
        {"random_seed", "Random number generator seed (0-1000000000)"},
        {"simulation_timestep", "Time step for simulation (0.0001-0.1 seconds)"},
        {"neuron_count", "Total number of neurons (1-10000)"},
        {"region_count", "Number of brain regions (1-100)"},
        {"connection_probability", "Probability of synapse formation (0.0-1.0)"},
        {"stdp_ltp_weight", "STDP LTP weight factor (0.0-1.0)"},
        {"stdp_ltd_weight", "STDP LTD weight factor (0.0-1.0)"},
        {"stdp_tau", "STDP trace decay time constant (1.0-100.0)"},
        {"synaptogenesis_rate", "Rate of new synapse formation (0.0-1.0)"},
        {"pruning_rate", "Rate of synapse elimination (0.0-1.0)"},
        {"replay_interval", "Steps between memory replay (1-100000)"},
        {"consolidation_interval", "Steps between memory consolidation (1-1000000)"},
        {"checkpoint_dir", "Directory for checkpoint files"},
        {"planning_depth", "Depth for neural planning (1-20)"},
        {"inhibition_strength", "Attention inhibition strength (0.0-1.0)"},
        {"excitation_strength", "Attention excitation strength (0.0-10.0)"},
        {"developmental_stage", "Current developmental stage (0-3)"}
    };
}

std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(first, end - first + 1);
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
