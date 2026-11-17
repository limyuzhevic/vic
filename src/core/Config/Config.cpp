#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // JSON schema for validation
    std::string jsonSchema;
    
    Impl() : jsonSchema(getDefaultSchema()) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file type based on extension
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json") {
        // Load as JSON format
        return loadFromJsonFile(filepath);
    } else if (extension == "yaml" || extension == "yml") {
        // TODO: Implement YAML parsing
        // For now, treat as JSON
        return loadFromJsonFile(filepath);
    } else {
        // Load as simple key=value format (backward compatibility)
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
}

bool Config::loadFromJsonFile(const std::string& filepath) {
    return loadFromJsonFileInternal(filepath);
}

// JSON file loading implementation
bool Config::loadFromJsonFileInternal(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    return loadFromJsonStringInternal(std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()));
}

// Load from JSON string
bool Config::loadFromJsonStringInternal(const std::string& jsonStr) {
    try {
        auto json = nlohmann::json::parse(jsonStr);
        
        // Clear existing configuration
        clear();
        
        // Parse JSON object
        if (json.is_object()) {
            for (auto it = json.begin(); it != json.end(); ++it) {
                std::string key = it.key();
                
                // Convert JSON value to ConfigValue
                ConfigValue value;
                if (it.value().is_null()) {
                    value = std::string("null");
                } else if (it.value().is_boolean()) {
                    value = it.value().get<bool>();
                } else if (it.value().is_number_integer()) {
                    value = it.value().get<int64_t>();
                } else if (it.value().is_number_unsigned()) {
                    value = it.value().get<uint64_t>();
                } else if (it.value().is_number()) {
                    value = it.value().get<double>();
                } else if (it.value().is_string()) {
                    value = it.value().get<std::string>();
                } else if (it.value().is_array()) {
                    // Convert array to vector (for now as strings)
                    std::vector<std::string> arr;
                    for (const auto& item : it.value()) {
                        if (item.is_string()) {
                            arr.push_back(item.get<std::string>());
                        } else if (item.is_number()) {
                            arr.push_back(std::to_string(item.get<double>()));
                        } else {
                            arr.push_back("[unknown]");
                        }
                    }
                    value = arr;
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return false;
    }
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

// Helper functions
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

// Template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

// Include JSON library after Config.hpp
#include <nlohmann/json.hpp>

// JSON schema for NLM configuration
const std::string DEFAULT_CONFIG_SCHEMA = R"(
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "NLM Configuration",
  "type": "object",
  "properties": {
    "random_seed": {
      "type": "integer",
      "description": "Random seed for reproducible experiments",
      "default": 42
    },
    "simulation_timestep": {
      "type": "number",
      "description": "Simulation timestep in seconds",
      "default": 0.001
    },
    "neuron_count": {
      "type": "integer",
      "description": "Number of neurons in the brain",
      "default": 1000
    },
    "region_count": {
      "type": "integer",
      "description": "Number of brain regions",
      "default": 1
    },
    "connection_probability": {
      "type": "number",
      "description": "Connection probability for random connectivity",
      "default": 0.1
    },
    "plasticity_learning_rate": {
      "type": "number",
      "description": "Plasticity learning rate",
      "default": 0.01
    },
    "stdp_ltp_weight": {
      "type": "number",
      "description": "STDP Long-Term Potentiation weight",
      "default": 0.01
    },
    "stdp_ltd_weight": {
      "type": "number",
      "description": "STDP Long-Term Depression weight",
      "default": 0.012
    },
    "stdp_time_constant": {
      "type": "number",
      "description": "STDP time constant",
      "default": 20.0
    },
    "development_synaptogenesis_rate": {
      "type": "number",
      "description": "Developmental synaptogenesis rate",
      "default": 0.001
    },
    "development_pruning_rate": {
      "type": "number",
      "description": "Developmental pruning rate",
      "default": 0.0001
    },
    "dopamine_baseline": {
      "type": "number",
      "description": "Dopamine baseline level",
      "default": 0.1
    },
    "reward_discount_factor": {
      "type": "number",
      "description": "Reward discount factor",
      "default": 0.99
    },
    "environment_name": {
      "type": "string",
      "description": "Environment name",
      "default": "GridWorld"
    },
    "environment_width": {
      "type": "integer",
      "description": "Environment width",
      "default": 10
    },
    "environment_height": {
      "type": "integer",
      "description": "Environment height",
      "default": 10
    },
    "log_level": {
      "type": "string",
      "description": "Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)",
      "default": "INFO"
    },
    "log_to_file": {
      "type": "boolean",
      "description": "Whether to log to file",
      "default": false
    },
    "log_filename": {
      "type": "string",
      "description": "Log filename",
      "default": "nlm.log"
    },
    "max_simulation_steps": {
      "type": "integer",
      "description": "Maximum simulation steps",
      "default": 10000
    },
    "simulation_time_limit": {
      "type": "number",
      "description": "Simulation time limit in seconds",
      "default": 0.0
    },
    "visualization_enabled": {
      "type": "boolean",
      "description": "Whether visualization is enabled",
      "default": false
    },
    "visualization_update_rate": {
      "type": "number",
      "description": "Visualization update rate in Hz",
      "default": 30.0
    }
  },
  "required": []
}
";
