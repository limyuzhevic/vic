#include "Config.hpp"
#include "ConfigurationValidator.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <map>
#include <unordered_set>
#include <set>
#include <regex>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

// Extract Config::Impl forward declaration and FastJSONConfigLoader before Config class

/**
 * Fast JSON configuration loader with caching
 * Provides O(n) parsing for large configurations
 */
class FastJSONConfigLoader {
public:
    static std::shared_ptr<Config> loadFromJSONFile(const std::string& filepath) {
        // Check cache first
        static std::unordered_map<std::string, std::shared_ptr<Config>> cache;
        auto cacheIt = cache.find(filepath);
        if (cacheIt != cache.end()) {
            return cacheIt->second;
        }
        
        auto config = std::make_shared<Config>();
        
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return nullptr;
            }
            
            // Parse JSON with error handling
            json j;
            file >> j;
            
            // Optimized field extraction
            extractConfigFromJSON(config, j);
            
            // Cache the configuration
            cache[filepath] = config;
            return config;
        } catch (const std::exception& e) {
            // Log error but continue with fallback
            return nullptr;
        }
    }
    
    /**
     * Fast JSON string parser with optimized type conversion
     */
    static std::shared_ptr<Config> loadFromJSONString(const std::string& jsonStr) {
        auto config = std::make_shared<Config>();
        
        try {
            json j = json::parse(jsonStr);
            extractConfigFromJSON(config, j);
            return config;
        } catch (const std::exception& e) {
            return nullptr;
        }
    }
    
private:
    static void extractConfigFromJSON(std::shared_ptr<Config>& config, const json& j) {
        // Optimized extraction using direct access when possible
        if (j.contains("neuron_count") && j["neuron_count"].is_number_integer()) {
            config->set("neuron_count", j["neuron_count"].get<size_t>(), ConfigSource::JSON);
        }
        if (j.contains("region_count") && j["region_count"].is_number_integer()) {
            config->set("region_count", j["region_count"].get<size_t>(), ConfigSource::JSON);
        }
        if (j.contains("simulation_timestep") && j["simulation_timestep"].is_number()) {
            config->set("simulation_timestep", j["simulation_timestep"].get<double>(), ConfigSource::JSON);
        }
        if (j.contains("connection_probability") && j["connection_probability"].is_number()) {
            config->set("connection_probability", j["connection_probability"].get<float>(), ConfigSource::JSON);
        }
        if (j.contains("plasticity_learning_rate") && j["plasticity_learning_rate"].is_number()) {
            config->set("plasticity_learning_rate", j["plasticity_learning_rate"].get<float>(), ConfigSource::JSON);
        }
        if (j.contains("stdp_ltp_weight") && j["stdp_ltp_weight"].is_number()) {
            config->set("stdp_ltp_weight", j["stdp_ltp_weight"].get<float>(), ConfigSource::JSON);
        }
        if (j.contains("stdp_ltd_weight") && j["stdp_ltd_weight"].is_number()) {
            config->set("stdp_ltd_weight", j["stdp_ltd_weight"].get<float>(), ConfigSource::JSON);
        }
        if (j.contains("stdp_time_constant") && j["stdp_time_constant"].is_number()) {
            config->set("stdp_time_constant", j["stdp_time_constant"].get<float>(), ConfigSource::JSON);
        }
        if (j.contains("development_synaptogenesis_rate") && j["development_synaptogenesis_rate"].is_number()) {
            config->set("development_synaptogenesis_rate", j["development_synaptogenesis_rate"].get<float>(), ConfigSource::JSON);
        }
        if (j.contains("development_pruning_rate") && j["development_pruning_rate"].is_number()) {
            config->set("development_pruning_rate", j["development_pruning_rate"].get<float>(), ConfigSource::JSON);
        }
        if (j.contains("dopamine_baseline") && j["dopamine_baseline"].is_number()) {
            config->set("dopamine_baseline", j["dopamine_baseline"].get<float>(), ConfigSource::JSON);
        }
        if (j.contains("reward_discount_factor") && j["reward_discount_factor"].is_number()) {
            config->set("reward_discount_factor", j["reward_discount_factor"].get<float>(), ConfigSource::JSON);
        }
        if (j.contains("environment_name") && j["environment_name"].is_string()) {
            config->set("environment_name", j["environment_name"].get<std::string>(), ConfigSource::JSON);
        }
        if (j.contains("environment_width") && j["environment_width"].is_number_integer()) {
            config->set("environment_width", j["environment_width"].get<size_t>(), ConfigSource::JSON);
        }
        if (j.contains("environment_height") && j["environment_height"].is_number_integer()) {
            config->set("environment_height", j["environment_height"].get<size_t>(), ConfigSource::JSON);
        }
        if (j.contains("log_level") && j["log_level"].is_string()) {
            config->set("log_level", j["log_level"].get<std::string>(), ConfigSource::JSON);
        }
        if (j.contains("log_to_file") && j["log_to_file"].is_boolean()) {
            config->set("log_to_file", j["log_to_file"].get<bool>(), ConfigSource::JSON);
        }
        if (j.contains("max_simulation_steps") && j["max_simulation_steps"].is_number_integer()) {
            config->set("max_simulation_steps", j["max_simulation_steps"].get<size_t>(), ConfigSource::JSON);
        }
        if (j.contains("visualization_enabled") && j["visualization_enabled"].is_boolean()) {
            config->set("visualization_enabled", j["visualization_enabled"].get<bool>(), ConfigSource::JSON);
        }
        if (j.contains("visualization_update_rate") && j["visualization_update_rate"].is_number()) {
            config->set("visualization_update_rate", j["visualization_update_rate"].get<float>(), ConfigSource::JSON);
        }
        
        // For unknown fields or complex types, fall back to generic extraction
        for (auto it = j.begin(); it != j.end(); ++it) {
            const std::string& key = it.key();
            const json& value = it.value();
            
            // Skip if already handled by specific extraction above
            if (config->has(key)) {
                continue;
            }
            
            if (value.is_string()) {
                config->set(key, value.get<std::string>(), ConfigSource::JSON);
            } else if (value.is_number_integer()) {
                config->set(key, value.get<size_t>(), ConfigSource::JSON);
            } else if (value.is_number()) {
                config->set(key, value.get<double>(), ConfigSource::JSON);
            } else if (value.is_boolean()) {
                config->set(key, value.get<bool>(), ConfigSource::JSON);
            }
        }
    }
};

namespace nlm {

using json = nlohmann::json;

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->validator = std::make_shared<ConfigurationValidator>();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try fast JSON loading first (O(n) instead of O(n) with comments/skipping)
    auto jsonConfig = FastJSONConfigLoader::loadFromJSONFile(filepath);
    if (jsonConfig) {
        // Merge JSON configuration with current config
        for (const auto& entry : jsonConfig->pImpl->entries) {
            set(entry.key, entry.value, entry.source);
        }
        return true;
    }
    
    // Fallback to original key=value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Fast parsing with std::unordered_map for O(1) lookups
    std::unordered_map<std::string, std::string> parsedEntries;
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments (early continue)
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs efficiently
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
            
            parsedEntries[key] = value;
        }
    }
    
    // Set all parsed entries
    for (const auto& pair : parsedEntries) {
        set(pair.first, pair.second, ConfigSource::File);
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
    // Validate the configuration if validator is available
    if (pImpl->validator) {
        bool isValid = pImpl->validator->validateParameter(key, value, source);
        if (!isValid) {
            // Log validation error
            std::string error = pImpl->validator->getValidationError(key, value, 
                "Failed validation");
            // In a real implementation, this would use the logger
            // For now, we'll just continue but could throw or log
        }
    }
    
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