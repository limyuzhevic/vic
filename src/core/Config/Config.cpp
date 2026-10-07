#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <set>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema_() {
    loadDefaultSchema();
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
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string valueStr = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            // Parse value from string
            auto parsedValue = parseValueFromString(key, valueStr);
            if (parsedValue.has_value()) {
                set(key, parsedValue.value(), ConfigSource::File);
            } else {
                // If parsing fails, treat as string
                set(key, valueStr, ConfigSource::File);
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
                
                auto parsedValue = parseValueFromString(key, value);
                if (parsedValue.has_value()) {
                    set(key, parsedValue.value(), ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            auto parsedValue = parseValueFromString(key, value);
            if (parsedValue.has_value()) {
                set(key, parsedValue.value(), ConfigSource::CommandLine);
            }
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
        file << "# Category: " << entry.metadata.category << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        file << entry.key << " = " << getExampleValue(entry.key) << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        // Return default value if key exists in schema
        if (isKeyInSchema(key)) {
            T defaultValue = std::get<T>(getDefaultValue(key));
            return defaultValue;
        }
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

bool Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Check if key is in schema
    if (!isKeyInSchema(key)) {
        // Unknown key, still allow it for backward compatibility
        auto& entry = getEntry(key);
        entry.value = value;
        entry.source = source;
        return true;
    }
    
    // Get validation rules for the key
    ConfigValidationRules validation = schema_.getValidation(key);
    
    // Validate the value
    if (!validation.validate(value)) {
        return false;
    }
    
    auto& entry = getEntry(key);
    entry.value = value;
    entry.source = source;
    return true;
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

bool Config::isValid(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return true; // Unknown key is always valid for backward compatibility
    }
    
    return it->isValid();
}

std::string Config::getValidationError(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "";
    }
    
    return it->getValidationError();
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

std::vector<std::string> Config::getValidKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.isValid()) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:" << std::endl;
    oss << "Total entries: " << pImpl->entries.size() << std::endl;
    oss << "Valid entries: " << std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.isValid(); }) << std::endl;
    
    if (!pImpl->entries.empty()) {
        oss << std::endl;
        oss << "Entries:" << std::endl;
        for (const auto& entry : pImpl->entries) {
            oss << "  " << entry.key << " = [";
            
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else {
                    oss << arg;
                }
            }, entry.value);
            
            oss << "] (" << static_cast<int>(entry.source) << ") ";
            if (!entry.isValid()) {
                oss << "[INVALID]";
            }
            oss << std::endl;
        }
    }
    
    return oss.str();
}

void Config::loadDefaultSchema() {
    // Clear existing schema
    schema_ = ConfigSchema();
    
    // Register core configuration parameters
    
    // General parameters
    schema_.registerParam("random_seed", static_cast<int64_t>(42),
        ConfigValidationRules(true, "Random seed for reproducible results,
        [](const ConfigValue& v) {
            try {
                return std::get<int64_t>(v) >= 0;
            } catch (...) { return false; }
        }, "Seed must be a non-negative integer"),
        ConfigMetadata("general", "core", true, "42"),
        "Random seed for reproducible results");
    
    schema_.registerParam("simulation_timestep", 0.001,
        ConfigValidationRules(true, "Simulation timestep in seconds",
        [](const ConfigValue& v) {
            try {
                double t = std::get<double>(v);
                return t > 0.0 && t < 1.0;
            } catch (...) { return false; }
        }, "Timestep must be a positive value less than 1 second"),
        ConfigMetadata("general", "core", true, "0.001"),
        "Simulation timestep in seconds (0.0-1.0s)");
    
    schema_.registerParam("neuron_count", static_cast<int64_t>(500),
        ConfigValidationRules(true, "Number of neurons in the network",
        [](const ConfigValue& v) {
            try {
                int64_t n = std::get<int64_t>(v);
                return n >= 10 && n <= 10000;
            } catch (...) { return false; }
        }, "Neuron count must be between 10 and 10000"),
        ConfigMetadata("general", "core", true, "500"),
        "Number of neurons in the network (10-10000)");
    
    schema_.registerParam("region_count", static_cast<int64_t>(1),
        ConfigValidationRules(true, "Number of brain regions",
        [](const ConfigValue& v) {
            try {
                int64_t n = std::get<int64_t>(v);
                return n >= 1 && n <= 100;
            } catch (...) { return false; }
        }, "Region count must be between 1 and 100"),
        ConfigMetadata("general", "core", true, "1"),
        "Number of brain regions (1-100)");
    
    schema_.registerParam("connection_probability", 0.15f,
        ConfigValidationRules(true, "Probability of connection between neurons",
        [](const ConfigValue& v) {
            try {
                double p = std::get<double>(v);
                return p >= 0.0 && p <= 1.0;
            } catch (...) { return false; }
        }, "Connection probability must be between 0.0 and 1.0"),
        ConfigMetadata("general", "core", true, "0.15"),
        "Connection probability between neurons (0.0-1.0)");
    
    // Plasticity parameters
    schema_.registerParam("stdp_ltp_weight", 0.02f,
        ConfigValidationRules(true, "STDP LTP weight",
        [](const ConfigValue& v) {
            try {
                double w = std::get<double>(v);
                return w >= 0.0 && w <= 1.0;
            } catch (...) { return false; }
        }, "STDP LTP weight must be between 0.0 and 1.0"),
        ConfigMetadata("plasticity", "core", true, "0.02"),
        "STDP Long-Term Potentiation weight");
    
    schema_.registerParam("stdp_ltd_weight", 0.015f,
        ConfigValidationRules(true, "STDP LTD weight",
        [](const ConfigValue& v) {
            try {
                double w = std::get<double>(v);
                return w >= 0.0 && w <= 1.0;
            } catch (...) { return false; }
        }, "STDP LTD weight must be between 0.0 and 1.0"),
        ConfigMetadata("plasticity", "core", true, "0.015"),
        "STDP Long-Term Depression weight");
    
    schema_.registerParam("stdp_tau", 20.0f,
        ConfigValidationRules(true, "STDP time constant",
        [](const ConfigValue& v) {
            try {
                double t = std::get<double>(v);
                return t > 0.0 && t < 1000.0;
            } catch (...) { return false; }
        }, "STDP tau must be between 0.1 and 1000"),
        ConfigMetadata("plasticity", "core", true, "20.0"),
        "STDP time constant (steps)");
    
    schema_.registerParam("plasticity_factor", 1.0f,
        ConfigValidationRules(true, "Global plasticity scaling factor",
        [](const ConfigValue& v) {
            try {
                double f = std::get<double>(v);
                return f > 0.0 && f <= 5.0;
            } catch (...) { return false; }
        }, "Plasticity factor must be between 0.01 and 5.0"),
        ConfigMetadata("plasticity", "advanced", true, "1.0"),
        "Global plasticity scaling factor (0.01-5.0)");
    
    schema_.registerParam("hebbian_enabled", true,
        ConfigValidationRules(true, "Enable Hebbian plasticity"),
        ConfigMetadata("plasticity", "advanced", true, "true"),
        "Enable Hebbian plasticity");
    
    schema_.registerParam("stdp_enabled", true,
        ConfigValidationRules(true, "Enable STDP plasticity"),
        ConfigMetadata("plasticity", "advanced", true, "true"),
        "Enable STDP plasticity");
    
    schema_.registerParam("structural_plasticity_enabled", true,
        ConfigValidationRules(true, "Enable structural plasticity"),
        ConfigMetadata("plasticity", "advanced", true, "true"),
        "Enable structural plasticity (synaptogenesis/pruning)");
    
    // Development parameters
    schema_.registerParam("development_enabled", true,
        ConfigValidationRules(true, "Enable developmental processes"),
        ConfigMetadata("development", "advanced", true, "true"),
        "Enable developmental processes");
    
    schema_.registerParam("developmental_stage", std::string("Initial"),
        ConfigValidationRules(true, "Developmental stage",
        [](const ConfigValue& v) {
            try {
                std::string s = std::get<std::string>(v);
                return s == "Initial" || s == "CriticalPeriod" || 
                       s == "Maturation" || s == "Adult" || s == "Aging";
            } catch (...) { return false; }
        }, "Invalid developmental stage"),
        ConfigMetadata("development", "advanced", true, "Initial"),
        "Developmental stage (Initial|CriticalPeriod|Maturation|Adult|Aging)");
    
    schema_.registerParam("synaptogenesis_rate", 0.0001f,
        ConfigValidationRules(true, "Synaptogenesis rate",
        [](const ConfigValue& v) {
            try {
                double r = std::get<double>(v);
                return r >= 0.0 && r <= 0.01;
            } catch (...) { return false; }
        }, "Synaptogenesis rate must be between 0.0 and 0.01"),
        ConfigMetadata("development", "advanced", true, "0.0001"),
        "Synaptogenesis rate per timestep");
    
    schema_.registerParam("pruning_rate", 0.00001f,
        ConfigValidationRules(true, "Pruning rate",
        [](const ConfigValue& v) {
            try {
                double r = std::get<double>(v);
                return r >= 0.0 && r <= 0.001;
            } catch (...) { return false; }
        }, "Pruning rate must be between 0.0 and 0.001"),
        ConfigMetadata("development", "advanced", true, "0.00001"),
        "Pruning rate per timestep");
    
    // Neuromodulation parameters
    schema_.registerParam("reward_modulation_enabled", true,
        ConfigValidationRules(true, "Enable reward-based modulation"),
        ConfigMetadata("neuromodulation", "advanced", true, "true"),
        "Enable reward-based neuromodulation");
    
    schema_.registerParam("curiosity_enabled", true,
        ConfigValidationRules(true, "Enable curiosity-driven exploration"),
        ConfigMetadata("neuromodulation", "advanced", true, "true"),
        "Enable curiosity-driven exploration");
    
    schema_.registerParam("novelty_enabled", true,
        ConfigValidationRules(true, "Enable novelty detection"),
        ConfigMetadata("neuromodulation", "advanced", true, "true"),
        "Enable novelty detection");
    
    // Memory parameters
    schema_.registerParam("working_memory_enabled", true,
        ConfigValidationRules(true, "Enable working memory"),
        ConfigMetadata("memory", "advanced", true, "true"),
        "Enable working memory system");
    
    schema_.registerParam("episodic_memory_enabled", true,
        ConfigValidationRules(true, "Enable episodic memory"),
        ConfigMetadata("memory", "advanced", true, "true"),
        "Enable episodic memory system");
    
    schema_.registerParam("associative_memory_enabled", true,
        ConfigValidationRules(true, "Enable associative memory"),
        ConfigMetadata("memory", "advanced", true, "true"),
        "Enable associative memory system");
    
    // Debug parameters
    schema_.registerParam("debug_mode", false,
        ConfigValidationRules(true, "Enable debug mode"),
        ConfigMetadata("debug", "advanced", true, "false"),
        "Enable debug mode with additional output");
    
    schema_.registerParam("debug_neurons", false,
        ConfigValidationRules(true, "Enable per-neuron debug output"),
        ConfigMetadata("debug", "advanced", true, "false"),
        "Enable per-neuron debug output");
    
    schema_.registerParam("debug_plasticity", false,
        ConfigValidationRules(true, "Enable plasticity debug output"),
        ConfigMetadata("debug", "advanced", true, "false"),
        "Enable plasticity debug output");
    
    // Experiment mode
    schema_.registerParam("experiment_mode", std::string("full"),
        ConfigValidationRules(true, "Experiment mode",
        [](const ConfigValue& v) {
            try {
                std::string m = std::get<std::string>(v);
                return m == "basic" || m == "plasticity" || m == "stdp" || 
                       m == "replay" || m == "full";
            } catch (...) { return false; }
        }, "Invalid experiment mode"),
        ConfigMetadata("experiment", "advanced", true, "full"),
        "Experiment mode: basic|plasticity|stdp|replay|full");
    
    // Checkpoint parameters
    schema_.registerParam("checkpoint_path", std::string("./checkpoint.bin"),
        ConfigValidationRules(true, "Checkpoint file path"),
        ConfigMetadata("checkpoint", "advanced", true, "./checkpoint.bin"),
        "Path for saving checkpoint files");
    
    schema_.registerParam("checkpoint_load_path", std::string(""),
        ConfigValidationRules(true, "Checkpoint load path"),
        ConfigMetadata("checkpoint", "advanced", true, ""),
        "Path for loading checkpoint files (empty for new)");
}

bool Config::validateAll() const {
    for (const auto& entry : pImpl->entries) {
        if (!entry.isValid()) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    for (const auto& entry : pImpl->entries) {
        if (!entry.isValid()) {
            errors.push_back("Invalid value for key '" + entry.key + "': " + entry.getValidationError());
        }
    }
    return errors;
}

std::string Config::getValueTypeName(const ConfigValue& value) {
    std::string typeName;
    std::visit([&typeName](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) typeName = "int";
        else if constexpr (std::is_same_v<T, int64_t>) typeName = "int64_t";
        else if constexpr (std::is_same_v<T, double>) typeName = "double";
        else if constexpr (std::is_same_v<T, bool>) typeName = "bool";
        else if constexpr (std::is_same_v<T, std::string>) typeName = "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) typeName = "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) typeName = "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) typeName = "vector<string>";
        else typeName = "unknown";
    }, value);
    return typeName;
}

std::optional<ConfigValue> Config::parseValueFromString(const std::string& key, const std::string& str) {
    std::string trimmed = trim(str);
    
    // Try to determine the type based on key or content
    bool isBool = false;
    if (trimmed == "true" || trimmed == "false" || trimmed == "yes" || trimmed == "no") {
        isBool = true;
    }
    
    // Try different types
    int64_t intVal;
    if (isInteger(trimmed, intVal)) {
        return ConfigValue(intVal);
    }
    
    double doubleVal;
    if (isDouble(trimmed, doubleVal)) {
        return ConfigValue(doubleVal);
    }
    
    if (isBool) {
        bool boolVal = (trimmed == "true" || trimmed == "yes");
        return ConfigValue(boolVal);
    }
    
    // Try as string
    return ConfigValue(trimmed);
}

std::string Config::getExampleValue(const std::string& key) {
    // Return example values for common keys
    if (key == "random_seed") return "42";
    if (key == "simulation_timestep") return "0.001";
    if (key == "neuron_count") return "500";
    if (key == "region_count") return "1";
    if (key == "connection_probability") return "0.15";
    if (key == "plasticity_factor") return "1.0";
    if (key == "developmental_stage") return "Initial";
    if (key == "checkpoint_path") return "./checkpoint.bin";
    
    // Return appropriate empty example based on key
    if (key.find("enabled") != std::string::npos) return "true";
    
    return "VALUE";
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

bool Config::isInteger(const std::string& str, int64_t& result) {
    try {
        size_t pos;
        int64_t val = std::stoll(str, &pos);
        if (pos == str.size()) {
            result = val;
            return true;
        }
    } catch (...) {}
    return false;
}

bool Config::isDouble(const std::string& str, double& result) {
    try {
        size_t pos;
        double val = std::stod(str, &pos);
        if (pos == str.size()) {
            result = val;
            return true;
        }
    } catch (...) {}
    return false;
}

bool Config::isBoolean(const std::string& str, bool& result) {
    std::string lower = toLower(trim(str));
    if (lower == "true" || lower == "yes" || lower == "1") {
        result = true;
        return true;
    } else if (lower == "false" || lower == "no" || lower == "0") {
        result = false;
        return true;
    }
    return false;
}

ConfigEntry& Config::getEntry(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return *it;
    }
    
    // Create new entry if not found
    pImpl->entries.emplace_back(key, ConfigValue(), ConfigSource::Default);
    return pImpl->entries.back();
}

const ConfigEntry& Config::getEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return *it;
    }
    
    // Return default entry for backward compatibility
    static ConfigEntry dummyEntry;
    return dummyEntry;
}

bool Config::isKeyInSchema(const std::string& key) const {
    return schema_.hasParam(key);
}

ConfigValue Config::getDefaultValue(const std::string& key) const {
    return schema_.getDefault(key);
}

} // namespace nlm