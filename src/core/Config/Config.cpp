#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format based on extension
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    // Convert to lowercase for comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return loadFromJSONFile(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        return loadFromYAMLFile(filepath);
    } else {
        // Default to simple key=value format
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON configuration file: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        for (auto& [key, value] : jsonData.items()) {
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Convert array to vector of appropriate type
                if (!value.empty()) {
                    if (value[0].is_string()) {
                        std::vector<std::string> strArray;
                        for (const auto& item : value) {
                            strArray.push_back(item.get<std::string>());
                        }
                        configValue = strArray;
                    } else if (value[0].is_number_integer()) {
                        std::vector<int64_t> intArray;
                        for (const auto& item : value) {
                            intArray.push_back(item.get<int64_t>());
                        }
                        configValue = intArray;
                    } else if (value[0].is_number_float()) {
                        std::vector<double> doubleArray;
                        for (const auto& item : value) {
                            doubleArray.push_back(item.get<double>());
                        }
                        configValue = doubleArray;
                    }
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON configuration file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    // TODO: Implement YAML support when YAML library is available
    NLM_LOG_WARNING("YAML configuration format is not yet implemented. Falling back to simple format.");
    return loadFromSimpleFile(filepath);
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    size_t lineNumber = 0;
    std::string line;
    while (std::getline(file, line)) {
        lineNumber++;
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
        } else {
            NLM_LOG_WARNING("Skipping invalid line in config file " + filepath + ": " + std::to_string(lineNumber));
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from: " + filepath);
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

bool Config::validate() const {
    // Get default configuration
    auto defaults = getDefaultConfig();
    
    bool isValid = true;
    
    for (const auto& defaultEntry : defaults) {
        const std::string& key = defaultEntry.first;
        const ConfigValue& defaultValue = defaultEntry.second;
        
        if (!has(key)) {
            NLM_LOG_WARNING("Missing configuration key: " + key);
            isValid = false;
        }
    }
    
    // Additional validation rules
    if (has("neuron_count")) {
        auto neuronCountOpt = get<int64_t>("neuron_count");
        if (neuronCountOpt.has_value()) {
            int64_t neuronCount = neuronCountOpt.value();
            if (neuronCount < 10 || neuronCount > 10000) {
                NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount) + ". Must be between 10 and 10000.");
                isValid = false;
            }
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestepOpt = get<double>("simulation_timestep");
        if (timestepOpt.has_value()) {
            double timestep = timestepOpt.value();
            if (timestep <= 0.0 || timestep > 1.0) {
                NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(timestep) + ". Must be between 0.001 and 1.0.");
                isValid = false;
            }
        }
    }
    
    if (has("connection_probability")) {
        auto probOpt = get<double>("connection_probability");
        if (probOpt.has_value()) {
            double prob = probOpt.value();
            if (prob < 0.0 || prob > 1.0) {
                NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(prob) + ". Must be between 0.0 and 1.0.");
                isValid = false;
            }
        }
    }
    
    return isValid;
}

std::map<std::string, ConfigValue> Config::getDefaultConfig() {
    std::map<std::string, ConfigValue> defaults;
    
    // Brain configuration
    defaults["random_seed"] = int64_t(42);
    defaults["neuron_count"] = int64_t(500);
    defaults["region_count"] = int64_t(1);
    defaults["connection_probability"] = 0.15f;
    defaults["brain.v_thresh"] = -50.0;
    defaults["brain.v_rest"] = -70.0;
    defaults["brain.v_reset"] = -75.0;
    defaults["brain.tau_mem"] = 20.0;
    defaults["brain.tau_ref"] = 2.0;
    
    // Plasticity configuration
    defaults["plasticity.stdp.enable"] = true;
    defaults["plasticity.stdp.learning_rate"] = 0.001f;
    defaults["plasticity.stdp.tau_plus"] = 20.0f;
    defaults["plasticity.stdp.tau_minus"] = 20.0f;
    defaults["plasticity.hebbian.enable"] = true;
    defaults["plasticity.hebbian.learning_rate"] = 0.001f;
    defaults["plasticity.structural.enable"] = true;
    defaults["plasticity.structural.synaptogenesis_rate"] = 0.0001f;
    defaults["plasticity.structural.pruning_rate"] = 0.00001f;
    
    // Neuromodulation configuration
    defaults["neuromod.dopamine.scale"] = 1.0f;
    defaults["neuromod.curiosity.enable"] = true;
    defaults["neuromod.curiosity.exploration_strength"] = 0.5f;
    defaults["neuromod.novelty.enable"] = true;
    defaults["neuromod.novelty.sensitivity"] = 0.5f;
    
    // Development configuration
    defaults["development.synaptogenesis_rate"] = 0.001f;
    defaults["development.pruning_rate"] = 0.0001f;
    defaults["development.maturation_rate"] = 0.01f;
    
    // Environment configuration
    defaults["environment.name"] = std::string("SimpleWorld");
    defaults["environment.width"] = 10;
    defaults["environment.height"] = 10;
    defaults["environment.vision_width"] = 8;
    defaults["environment.vision_height"] = 8;
    
    // Logging configuration
    defaults["log_level"] = std::string("INFO");
    defaults["log_to_file"] = false;
    defaults["log_filename"] = std::string("nlm.log");
    
    // Simulation configuration
    defaults["max_simulation_steps"] = int64_t(10000);
    defaults["simulation_time_limit"] = 0.0;
    defaults["visualization_enabled"] = false;
    defaults["visualization_update_rate"] = 30.0f;
    
    return defaults;
}

// Convert ConfigValue to JSON
nlohmann::json configValueToJSON(const ConfigValue& value) {
    nlohmann::json result;
    
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = arg;
        }
    }, value);
    
    return result;
}

void Config::saveToJSONFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save configuration to JSON file: " + filepath);
        return;
    }
    
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = configValueToJSON(entry.value);
    }
    
    file << jsonData.dump(2); // Pretty print with 2-space indentation
    
    NLM_LOG_INFO("Saved configuration to JSON file: " + filepath);
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