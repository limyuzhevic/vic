#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <map>
#include <set>

namespace nlm {

// Configuration validation rules
struct ConfigValidationRules {
    std::map<std::string, std::function<bool(const ConfigValue&)>> validators;
    std::set<std::string> requiredKeys;
    std::map<std::string, ConfigValue> defaults;
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<ConfigValidationRules> validationRules;
    
    Impl() : validationRules(std::make_unique<ConfigValidationRules>()) {
        // Initialize validation rules
        initializeValidationRules();
    }
    
    void initializeValidationRules() {
        // Brain configuration validation
        validationRules->validators["brain.neuron_count"] = 
            [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0 && std::get<int64_t>(v) <= 100000; };
        
        validationRules->validators["brain.region_count"] = 
            [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0 && std::get<int64_t>(v) <= 100; };
        
        validationRules->validators["brain.connection_probability"] = 
            [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; };
        
        // Plasticity configuration validation
        validationRules->validators["plasticity.stdp.learning_rate"] = 
            [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 0.1; };
        
        validationRules->validators["plasticity.hebbian.learning_rate"] = 
            [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 0.1; };
        
        // Neuromodulation validation
        validationRules->validators["neuromod.dopamine.scale"] = 
            [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0; };
        
        validationRules->validators["neuromod.curiosity.threshold"] = 
            [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; };
        
        // Memory validation
        validationRules->validators["memory.working_memory.capacity"] = 
            [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; };
        
        // Development validation
        validationRules->validators["development.initial_plasticity_rate"] = 
            [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; };
        
        // Simulation validation
        validationRules->validators["simulation.timestep"] = 
            [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0 && std::get<double>(v) <= 0.1; };
        
        // Required keys
        validationRules->requiredKeys = {
            "brain.neuron_count",
            "brain.connection_probability",
            "plasticity.stdp.learning_rate",
            "neuromod.curiosity.enable"
        };
        
        // Default values for missing keys
        validationRules->defaults = {
            {"brain.region_count", int64_t(1)},
            {"brain.initial_weight_mean", double(0.5)},
            {"brain.initial_weight_std", double(0.1)},
            {"brain.v_thresh", double(-50.0)},
            {"brain.v_rest", double(-70.0)},
            {"brain.v_reset", double(-75.0)},
            {"brain.tau_mem", double(20.0)},
            {"brain.tau_ref", double(2.0)},
            {"plasticity.stdp.tau_plus", double(20.0)},
            {"plasticity.stdp.tau_minus", double(20.0)},
            {"plasticity.hebbian.enable", bool(true)},
            {"plasticity.structural.enable", bool(true)},
            {"neuromod.dopamine.scale", double(1.0)},
            {"neuromod.curiosity.enable", bool(true)},
            {"neuromod.novelty.enable", bool(true)},
            {"neuromod.curiosity.threshold", double(0.5)},
            {"memory.working_memory.capacity", int64_t(100)},
            {"memory.episodic.max_episodes", int64_t(1000)},
            {"development.initial_plasticity_rate", double(1.0)},
            {"development.critical_period_length", double(300.0)},
            {"simulation.timestep", double(0.001)},
            {"simulation.max_steps", int64_t(10000)}
        };
    }
    
    bool validate() const {
        std::vector<std::string> errors;
        
        // Check required keys
        for (const auto& requiredKey : validationRules->requiredKeys) {
            if (!hasKey(requiredKey)) {
                errors.push_back("Missing required key: " + requiredKey);
            }
        }
        
        // Validate all entries
        for (const auto& entry : entries) {
            auto it = validationRules->validators.find(entry.key);
            if (it != validationRules->validators.end()) {
                if (!it->second(entry.value)) {
                    errors.push_back("Invalid value for key '" + entry.key + "': " + configValueToString(entry.value));
                }
            }
        }
        
        // Check for inconsistencies
        auto neuronCount = get<int64_t>("brain.neuron_count");
        auto regionCount = get<int64_t>("brain.region_count");
        if (neuronCount && regionCount && *neuronCount < *regionCount) {
            errors.push_back("brain.neuron_count must be >= brain.region_count");
        }
        
        return errors.empty();
    }
    
    std::vector<std::string> getValidationErrors() const {
        std::vector<std::string> errors;
        
        for (const auto& entry : entries) {
            auto it = validationRules->validators.find(entry.key);
            if (it != validationRules->validators.end()) {
                if (!it->second(entry.value)) {
                    errors.push_back("Invalid value for key '" + entry.key + "': " + configValueToString(entry.value));
                }
            }
        }
        
        return errors;
    }
    
    std::string configValueToString(const ConfigValue& value) const {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
    
    bool hasKey(const std::string& key) const {
        return std::any_of(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    }
    
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        auto it = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == entries.end()) {
            return std::nullopt;
        }
        
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to load as JSON first (modern approach)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    std::string content = buffer.str();
    
    // Try JSON parsing first
    try {
        auto json = nlohmann::json::parse(content);
        clear(); // Clear existing config
        
        // Convert JSON to Config entries
        for (const auto& item : json.items()) {
            const std::string& key = item.key();
            
            if (item.is_boolean()) {
                set(key, item.get<bool>(), ConfigSource::File);
            } else if (item.is_number_integer()) {
                set(key, item.get<int64_t>(), ConfigSource::File);
            } else if (item.is_number_float()) {
                set(key, item.get<double>(), ConfigSource::File);
            } else if (item.is_string()) {
                set(key, item.get<std::string>(), ConfigSource::File);
            } else if (item.is_array()) {
                // Handle arrays - currently only support string arrays
                std::vector<std::string> strArray;
                for (const auto& element : item) {
                    strArray.push_back(element.get<std::string>());
                }
                set(key, ConfigValue(strArray), ConfigSource::File);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error&) {
        // JSON parsing failed, try old format
        file.open(filepath);
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
