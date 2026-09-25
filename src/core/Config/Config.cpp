#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

// Global variables for configuration
std::string Config::globalConfigDir_ = "configs/";
std::vector<ConfigSource> Config::sourcePrecedence_ = {
    ConfigSource::CommandLine,
    ConfigSource::File,
    ConfigSource::Preset,
    ConfigSource::Default,
    ConfigSource::Runtime
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigPreset> presets;
    std::map<std::string, std::string> defaultTypes;
    std::map<std::string, std::string> defaultDescriptions;
    std::map<std::string, std::string> defaultValidationRules;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize default types
    pImpl->defaultTypes = {
        {"random_seed", "int64"},
        {"simulation_timestep", "double"},
        {"neuron_count", "int64"},
        {"region_count", "int64"},
        {"connection_probability", "float"},
        {"stdp_ltp_weight", "float"},
        {"stdp_ltd_weight", "float"},
        {"stdp_tau", "float"},
        {"synaptogenesis_rate", "float"},
        {"pruning_rate", "float"},
        {"plasticity_learning_rate", "float"},
        {"development_synaptogenesis_rate", "float"},
        {"development_pruning_rate", "float"},
        {"dopamine_baseline", "float"},
        {"reward_discount_factor", "float"},
        {"log_level", "string"},
        {"log_to_file", "bool"},
        {"environment_name", "string"},
        {"environment_width", "int"},
        {"environment_height", "int"},
        {"vision_width", "int"},
        {"vision_height", "int"}
    };
    
    // Initialize default descriptions
    pImpl->defaultDescriptions = {
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Connection probability for random connectivity"},
        {"stdp_ltp_weight", "STDP long-term potentiation weight"},
        {"stdp_ltd_weight", "STDP long-term depression weight"},
        {"stdp_tau", "STDP time constant"},
        {"synaptogenesis_rate", "Structural plasticity synaptogenesis rate"},
        {"pruning_rate", "Structural plasticity pruning rate"},
        {"plasticity_learning_rate", "Plasticity learning rate"},
        {"development_synaptogenesis_rate", "Development synaptogenesis rate"},
        {"development_pruning_rate", "Development pruning rate"},
        {"dopamine_baseline", "Baseline dopamine level"},
        {"reward_discount_factor", "Reward discount factor"},
        {"log_level", "Log level (DEBUG, INFO, WARNING, ERROR)"},
        {"log_to_file", "Whether to log to file"},
        {"environment_name", "Environment name"},
        {"environment_width", "Environment width"},
        {"environment_height", "Environment height"},
        {"vision_width", "Vision width in pixels"},
        {"vision_height", "Vision height in pixels"}
    };
    
    // Initialize default validation rules
    pImpl->defaultValidationRules = {
        {"random_seed", ">=0"},
        {"simulation_timestep", "(0,1.0]"},
        {"neuron_count", ">=0"},
        {"region_count", ">=0"},
        {"connection_probability", "[0.0,1.0]"},
        {"stdp_ltp_weight", ">=0.0"},
        {"stdp_ltd_weight", ">=0.0"},
        {"stdp_tau", ">=0.0"},
        {"synaptogenesis_rate", ">=0.0"},
        {"pruning_rate", ">=0.0"},
        {"plasticity_learning_rate", ">=0.0"},
        {"development_synaptogenesis_rate", ">=0.0"},
        {"development_pruning_rate", ">=0.0"},
        {"dopamine_baseline", "[-1.0,1.0]"},
        {"reward_discount_factor", "[0.0,1.0]"},
        {"log_to_file", ""},
        {"environment_width", ">=1"},
        {"environment_height", ">=1"},
        {"vision_width", ">=1"},
        {"vision_height", ">=1"}
    };
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
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
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
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
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Enhanced CLI\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
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
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        std::string type = pImpl->defaultTypes.count(key) ? pImpl->defaultTypes[key] : "string";
        std::string description = pImpl->defaultDescriptions.count(key) ? pImpl->defaultDescriptions[key] : "";
        
        ConfigEntry newEntry(key, value, source, description, type);
        pImpl->entries.push_back(newEntry);
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<bool>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::validate() const {
    bool allValid = true;
    
    for (const auto& entry : pImpl->entries) {
        std::string error = validateValue(entry.key, entry.value);
        if (!error.empty()) {
            allValid = false;
        }
    }
    
    return allValid;
}

bool Config::validateKey(const std::string& key) const {
    if (pImpl->defaultTypes.find(key) == pImpl->defaultTypes.end()) {
        return true;
    }
    return true;
}

std::string Config::validateValue(const std::string& key, const ConfigValue& value) const {
    if (pImpl->defaultValidationRules.find(key) != pImpl->defaultValidationRules.end()) {
        std::string rule = pImpl->defaultValidationRules[key];
        
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>> ||
                                 std::is_same_v<T, std::vector<bool>>) {
                oss << "[]";
            } else {
                oss << arg;
            }
        }, value);
        
        std::string valueStr = oss.str();
        
        if (rule.find(">=") != std::string::npos) {
            size_t pos = rule.find(">=");
            double threshold = std::stod(rule.substr(pos + 2));
            
            try {
                double val = std::stod(valueStr);
                if (val < threshold) {
                    return "Value must be >= " + std::to_string(threshold);
                }
            } catch (...) {
                return "Value must be a number";
            }
        }
        
        return "";
    }
    
    return "";
}

std::string Config::getType(const std::string& key) const {
    if (pImpl->defaultTypes.find(key) != pImpl->defaultTypes.end()) {
        return pImpl->defaultTypes[key];
    }
    return "string";
}

bool Config::isRequired(const std::string& key) const {
    return pImpl->defaultDescriptions.find(key) != pImpl->defaultDescriptions.end();
}

std::vector<ConfigPreset> Config::getAvailablePresets() {
    return getDefaultPresets();
}

bool Config::applyPreset(const std::string& presetName, ConfigSource source) {
    // Simple preset implementation
    if (presetName == "fast") {
        set("random_seed", "42", source);
        set("simulation_timestep", "0.001", source);
        set("neuron_count", "500", source);
        set("region_count", "1", source);
        set("connection_probability", "0.15", source);
        return true;
    } else if (presetName == "medium") {
        set("random_seed", "123", source);
        set("simulation_timestep", "0.001", source);
        set("neuron_count", "1000", source);
        set("region_count", "2", source);
        set("connection_probability", "0.1", source);
        return true;
    } else if (presetName == "high") {
        set("random_seed", "456", source);
        set("simulation_timestep", "0.0005", source);
        set("neuron_count", "2000", source);
        set("region_count", "3", source);
        set("connection_probability", "0.05", source);
        return true;
    }
    return false;
}

ConfigFormat Config::getFormatFromExtension(const std::string& filename) {
    if (filename.empty()) return ConfigFormat::Unknown;
    
    std::string lower = toLower(filename);
    
    if (endsWith(lower, ".json")) {
        return ConfigFormat::JSON;
    } else if (endsWith(lower, ".yaml") || endsWith(lower, ".yml")) {
        return ConfigFormat::YAML;
    } else if (endsWith(lower, ".cfg") || endsWith(lower, ".txt")) {
        return ConfigFormat::CFG;
    }
    
    return ConfigFormat::Unknown;
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

std::string Config::getStatistics() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
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
