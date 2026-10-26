#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <ctime>
#include <numeric>
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <mutex>
#include <unordered_set>
#include <set>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigEntry> history;  // Change history for undo
    std::unordered_map<std::string, size_t> keyVersions;  // Track versions per key
    ConfigChangeCallback changeCallback;
    bool frozen;
    std::chrono::steady_clock::time_point creationTime;
    
    Impl() : frozen(false), creationTime(std::chrono::steady_clock::now()) {}
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
            
            // Check if this is a comment line (e.g., "# random_seed = 42")
            if (key.empty() || (key.size() > 0 && std::isalpha(key[0]))) {
                // Try to determine type and convert
                if (value.empty()) continue;
                
                try {
                    if (value.find('.') != std::string::npos) {
                        // Could be float or string
                        size_t pos2 = value.find(' ');
                        if (pos2 != std::string::npos) {
                            // Has spaces, likely a string
                            setWithDescription(key, value, "File loaded configuration", ConfigSource::File);
                        } else {
                            double doubleVal = std::stod(value);
                            setWithDescription(key, doubleVal, "File loaded configuration", ConfigSource::File);
                        }
                    } else {
                        // Try int first, then bool, then string
                        try {
                            int64_t intVal = std::stoll(value);
                            setWithDescription(key, intVal, "File loaded configuration", ConfigSource::File);
                        } catch (...) {
                            if (value == "true" || value == "false") {
                                bool boolVal = (value == "true");
                                setWithDescription(key, boolVal, "File loaded configuration", ConfigSource::File);
                            } else {
                                setWithDescription(key, value, "File loaded configuration", ConfigSource::File);
                            }
                        }
                    }
                } catch (...) {
                    // If conversion fails, store as string
                    setWithDescription(key, value, "File loaded configuration (unconverted)", ConfigSource::File);
                }
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
                setWithDescription(key, value, "Command line argument", ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setWithDescription(key, value, "Command line argument", ConfigSource::CommandLine);
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
    file << "# Saved from programmatic configuration\n";
    file << "# Generated: " << std::ctime(&std::time(nullptr));
    
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        switch (entry.value.index()) {
            case 0: { // int
                file << entry.key << " = " << std::get<int>(entry.value);
                break;
            }
            case 1: { // int64_t
                file << entry.key << " = " << std::get<int64_t>(entry.value);
                break;
            }
            case 2: { // double
                file << entry.key << " = " << std::get<double>(entry.value);
                break;
            }
            case 3: { // bool
                file << entry.key << " = " << (std::get<bool>(entry.value) ? "true" : "false");
                break;
            }
            case 4: { // std::string
                file << entry.key << " = \"" << std::get<std::string>(entry.value) << "\"";
                break;
            }
            case 5: { // std::vector<int>
                const auto& vec = std::get<std::vector<int>>(entry.value);
                file << entry.key << " = [";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << vec[i];
                }
                file << "]";
                break;
            }
            case 6: { // std::vector<double>
                const auto& vec = std::get<std::vector<double>>(entry.value);
                file << entry.key << " = [";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << vec[i];
                }
                file << "]";
                break;
            }
            case 7: { // std::vector<std::string>
                const auto& vec = std::get<std::vector<std::string>>(entry.value);
                file << entry.key << " = [\"";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << "\", \"";
                    file << vec[i];
                }
                file << "\"]";
                break;
            }
        }
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
    if (pImpl->frozen && source == ConfigSource::Runtime) {
        throw std::runtime_error("Configuration is frozen. Cannot set runtime values.");
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigValue oldValue;
    bool isNew = false;
    
    if (it != pImpl->entries.end()) {
        oldValue = it->value;
    } else {
        isNew = true;
    }
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        
        // Add to history
        if (oldValue != value) {
            pImpl->history.emplace_back(key, oldValue, ConfigSource::Undo, "Undo " + key);
            pImpl->keyVersions[key]++;
            
            // Call callback if set
            if (pImpl->changeCallback) {
                pImpl->changeCallback(key, oldValue, value);
            }
        }
    } else {
        pImpl->entries.emplace_back(key, value, source);
        
        // Add to history for new entries
        pImpl->history.emplace_back(key, ConfigValue(), ConfigSource::Undo, "Add " + key);
        pImpl->keyVersions[key] = 1;
        
        // Call callback if set
        if (pImpl->changeCallback) {
            pImpl->changeCallback(key, ConfigValue(), value);
        }
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setWithDescription(const std::string& key, const ConfigValue& value, const std::string& description, ConfigSource source) {
    set(key, value, source);
    
    // Update the description for this key
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        it->description = description;
    }
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Save for undo
        pImpl->history.emplace_back(key, it->value, ConfigSource::Undo, "Remove " + key);
        
        pImpl->entries.erase(it);
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<ConfigEntry> Config::getEntries() const {
    return pImpl->entries;
}

void Config::clear() {
    // Save undo for all entries
    for (const auto& entry : pImpl->entries) {
        pImpl->history.emplace_back(entry.key, entry.value, ConfigSource::Undo, "Clear " + entry.key);
    }
    
    pImpl->entries.clear();
    pImpl->frozen = false;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:";
    oss << "\n  Total entries: " << pImpl->entries.size();
    
    std::unordered_map<ConfigSource, size_t> sourceCounts;
    for (const auto& entry : pImpl->entries) {
        sourceCounts[entry.source]++;
    }
    
    oss << "\n  By source:";
    oss << "\n    Default: " << sourceCounts[ConfigSource::Default];
    oss << "\n    File: " << sourceCounts[ConfigSource::File];
    oss << "\n    Command Line: " << sourceCounts[ConfigSource::CommandLine];
    oss << "\n    Runtime: " << sourceCounts[ConfigSource::Runtime];
    
    oss << "\n  Configuration age: " << getConfigurationAge() << " seconds";
    oss << "\n  Frozen: " << (isFrozen() ? "yes" : "no");
    
    oss << "\n  Memory estimate: " << estimateMemoryUsage() << " bytes";
    
    return oss.str();
}

ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalKeys = pImpl->entries.size();
    stats.runtimeKeys = 0;
    stats.fileKeys = 0;
    stats.defaultKeys = 0;
    stats.commandLineKeys = 0;
    stats.memoryUsageMB = estimateMemoryUsage() / (1024 * 1024);
    stats.lastAccess = std::chrono::steady_clock::now();
    
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default:
                stats.defaultKeys++;
                break;
            case ConfigSource::File:
                stats.fileKeys++;
                break;
            case ConfigSource::CommandLine:
                stats.commandLineKeys++;
                break;
            case ConfigSource::Runtime:
                stats.runtimeKeys++;
                break;
        }
    }
    
    return stats;
}

std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    // Example validation rules
    if (has("random_seed")) {
        auto seed = get<int64_t>("random_seed");
        if (seed && seed < 0) {
            errors.push_back("random_seed must be non-negative");
        }
    }
    
    if (has("neuron_count")) {
        auto count = get<int64_t>("neuron_count");
        if (count && count > 1000000) {
            errors.push_back("neuron_count should not exceed 1,000,000");
        }
    }
    
    if (has("connection_probability")) {
        auto prob = get<double>("connection_probability");
        if (prob && (prob < 0.0 || prob > 1.0)) {
            errors.push_back("connection_probability must be between 0.0 and 1.0");
        }
    }
    
    return errors;
}

void Config::setChangeCallback(ConfigChangeCallback callback) {
    pImpl->changeCallback = callback;
}

void Config::freeze() {
    pImpl->frozen = true;
}

bool Config::isFrozen() const {
    return pImpl->frozen;
}

std::string Config::getValueType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "not_found";
    }
    
    switch (it->value.index()) {
        case 0: return "int";
        case 1: return "int64_t";
        case 2: return "double";
        case 3: return "bool";
        case 4: return "string";
        case 5: return "vector<int>";
        case 6: return "vector<double>";
        case 7: return "vector<string>";
        default: return "unknown";
    }
}

std::shared_ptr<Config> Config::copy() const {
    auto copy = std::make_shared<Config>();
    copy->pImpl->entries = pImpl->entries;
    copy->pImpl->frozen = pImpl->frozen;
    return copy;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            // Add new entry
            pImpl->entries.push_back(entry);
        } else {
            // Override existing entry
            it->value = entry.value;
            it->source = entry.source;
            it->description = entry.description;
        }
    }
}

bool Config::setDefaultFromFile(const std::string& filepath) {
    // First, backup current defaults
    std::vector<ConfigEntry> savedDefaults;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == ConfigSource::Default) {
            savedDefaults.push_back(entry);
        }
    }
    
    // Clear current entries
    clear();
    
    // Load from file (this will be file source)
    bool success = loadFromFile(filepath);
    if (!success) {
        // Restore defaults on failure
        for (const auto& entry : savedDefaults) {
            pImpl->entries.push_back(entry);
        }
        return false;
    }
    
    // Mark all as Default source
    for (auto& entry : pImpl->entries) {
        entry.source = ConfigSource::Default;
    }
    
    return true;
}

void Config::resetToDefaults() {
    // Create a minimal default config
    clear();
    
    // Set default values (based on original default.cfg)
    setWithDescription("random_seed", int64_t(42), "Default random seed for reproducibility", ConfigSource::Default);
    setWithDescription("simulation_timestep", 0.001, "Default simulation timestep in seconds", ConfigSource::Default);
    setWithDescription("neuron_count", int64_t(1000), "Number of neurons in the brain", ConfigSource::Default);
    setWithDescription("region_count", int64_t(2), "Number of brain regions", ConfigSource::Default);
    setWithDescription("connection_probability", 0.1, "Connection probability for random connectivity", ConfigSource::Default);
    setWithDescription("plasticity_learning_rate", 0.01, "Plasticity learning rate", ConfigSource::Default);
    setWithDescription("stdp_ltp_weight", 0.01, "STDP long-term potentiation weight", ConfigSource::Default);
    setWithDescription("stdp_ltd_weight", 0.012, "STDP long-term depression weight", ConfigSource::Default);
    setWithDescription("stdp_time_constant", 20.0, "STDP time constant", ConfigSource::Default);
    setWithDescription("development_synaptogenesis_rate", 0.001, "Synaptogenesis rate during development", ConfigSource::Default);
    setWithDescription("development_pruning_rate", 0.0001, "Pruning rate during development", ConfigSource::Default);
    setWithDescription("dopamine_baseline", 0.1, "Dopamine baseline level", ConfigSource::Default);
    setWithDescription("reward_discount_factor", 0.99, "Reward discount factor", ConfigSource::Default);
    setWithDescription("environment_name", "GridWorld", "Environment name", ConfigSource::Default);
    setWithDescription("environment_width", int64_t(10), "Environment width", ConfigSource::Default);
    setWithDescription("environment_height", int64_t(10), "Environment height", ConfigSource::Default);
    setWithDescription("log_level", "INFO", "Log level", ConfigSource::Default);
    setWithDescription("log_to_file", false, "Enable file logging", ConfigSource::Default);
    setWithDescription("log_filename", "nlm.log", "Log filename", ConfigSource::Default);
    setWithDescription("max_simulation_steps", int64_t(10000), "Maximum simulation steps", ConfigSource::Default);
    setWithDescription("simulation_time_limit", 0.0, "Simulation time limit", ConfigSource::Default);
    setWithDescription("visualization_enabled", false, "Enable visualization", ConfigSource::Default);
    setWithDescription("visualization_update_rate", 30.0, "Visualization update rate", ConfigSource::Default);
}

std::string Config::getAsString(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return "";
    }
    
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            const auto& vec = arg;
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            const auto& vec = arg;
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::fixed << std::setprecision(6) << vec[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            const auto& vec = arg;
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << vec[i] << "\"";
            }
            oss << "]";
        }
    }, it->value);
    
    return oss.str();
}

std::string Config::getWithUnits(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "N/A";
    }
    
    std::string baseValue = getAsString(key);
    
    // Add units based on key
    if (key == "simulation_timestep") {
        return baseValue + " seconds";
    } else if (key == "connection_probability" || key == "dopamine_baseline" || key == "reward_discount_factor") {
        return baseValue + " (0.0-1.0)";
    } else if (key == "neuron_count" || key == "region_count" || key == "max_simulation_steps") {
        return baseValue + " neurons/regions/steps";
    } else if (key == "stdp_time_constant") {
        return baseValue + " milliseconds";
    } else if (key == "environment_width" || key == "environment_height") {
        return baseValue + " cells";
    } else if (key == "visualization_update_rate") {
        return baseValue + " Hz";
    }
    
    return baseValue;
}

bool Config::validateRequiredKeys(const std::vector<std::string>& requiredKeys) const {
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            return false;
        }
    }
    return true;
}

double Config::getConfigurationAge() const {
    auto now = std::chrono::steady_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->creationTime);
    return static_cast<double>(age.count());
}

std::string Config::exportConfig(const std::string& format) const {
    if (format == "json") {
        std::ostringstream oss;
        oss << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            oss << "  \"" << entry.key << "\": ";
            
            switch (entry.value.index()) {
                case 0: // int
                    oss << std::get<int>(entry.value);
                    break;
                case 1: // int64_t
                    oss << std::get<int64_t>(entry.value);
                    break;
                case 2: // double
                    oss << std::fixed << std::setprecision(6) << std::get<double>(entry.value);
                    break;
                case 3: // bool
                    oss << (std::get<bool>(entry.value) ? "true" : "false");
                    break;
                case 4: // std::string
                    oss << "\"" << std::get<std::string>(entry.value) << "\"";
                    break;
            }
            
            if (i < pImpl->entries.size() - 1) {
                oss << ",\n";
            } else {
                oss << "\n";
            }
        }
        oss << "}\n";
        return oss.str();
    } else if (format == "ini") {
        // Simple INI format
        std::ostringstream oss;
        oss << "# NLM Configuration\n";
        oss << "# Exported at: " << std::ctime(&std::time(nullptr));
        
        for (const auto& entry : pImpl->entries) {
            if (!entry.description.empty()) {
                oss << "# " << entry.description << "\n";
            }
            
            oss << "[" << toLower(entry.key) << "]\n";
            
            switch (entry.value.index()) {
                case 0: oss << "value = " << std::get<int>(entry.value) << "\n"; break;
                case 1: oss << "value = " << std::get<int64_t>(entry.value) << "\n"; break;
                case 2: oss << "value = " << std::get<double>(entry.value) << "\n"; break;
                case 3: oss << "value = " << (std::get<bool>(entry.value) ? "true" : "false") << "\n"; break;
                case 4: oss << "value = " << std::get<std::string>(entry.value) << "\n"; break;
            }
            oss << "\n";
        }
        
        return oss.str();
    }
    
    return "# Unsupported format: " + format + "\n";
}

bool Config::importConfig(const std::string& configString, const std::string& format) {
    // Clear current configuration
    clear();
    
    if (format == "json") {
        // Simple JSON parsing (not robust)
        size_t pos = 0;
        while (pos < configString.size()) {
            size_t keyStart = configString.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = configString.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = configString.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            size_t valueStart = configString.find('"', keyEnd + 1);
            if (valueStart == std::string::npos) break;
            
            size_t colon = configString.find(':', valueStart);
            if (colon == std::string::npos) break;
            
            size_t comma = configString.find(',', colon);
            if (comma == std::string::npos) {
                // Last entry
                size_t brace = configString.find('}', colon);
                if (brace == std::string::npos) break;
                
                std::string valueStr = configString.substr(colon + 1, brace - colon - 1);
                // Remove quotes if present
                if (valueStr.front() == '"' && valueStr.back() == '"') {
                    valueStr = valueStr.substr(1, valueStr.size() - 2);
                }
                
                try {
                    if (valueStr.find('.') != std::string::npos) {
                        double val = std::stod(valueStr);
                        set(key, val, ConfigSource::Runtime);
                    } else {
                        int64_t val = std::stoll(valueStr);
                        set(key, val, ConfigSource::Runtime);
                    }
                } catch (...) {
                    set(key, valueStr, ConfigSource::Runtime);
                }
                break;
            }
            
            std::string valueStr = configString.substr(colon + 1, comma - colon - 1);
            
            try {
                if (valueStr.find('.') != std::string::npos) {
                    double val = std::stod(valueStr);
                    set(key, val, ConfigSource::Runtime);
                } else {
                    int64_t val = std::stoll(valueStr);
                    set(key, val, ConfigSource::Runtime);
                }
            } catch (...) {
                set(key, valueStr, ConfigSource::Runtime);
            }
            
            pos = comma + 1;
        }
    } else {
        // Try simple parsing for INI-like format
        size_t pos = 0;
        while (pos < configString.size()) {
            size_t bracketStart = configString.find('[', pos);
            if (bracketStart == std::string::npos) break;
            
            size_t bracketEnd = configString.find(']', bracketStart + 1);
            if (bracketEnd == std::string::npos) break;
            
            std::string key = configString.substr(bracketStart + 1, bracketEnd - bracketStart - 1);
            toLower(key);
            
            size_t equals = configString.find('=', bracketEnd + 1);
            if (equals == std::string::npos) break;
            
            size_t newline = configString.find('\n', equals + 1);
            if (newline == std::string::npos) newline = configString.size();
            
            std::string valueStr = configString.substr(equals + 1, newline - equals - 1);
            trim(valueStr);
            
            // Remove quotes if present
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            try {
                if (valueStr.find('.') != std::string::npos) {
                    double val = std::stod(valueStr);
                    set(key, val, ConfigSource::Runtime);
                } else if (valueStr == "true" || valueStr == "false") {
                    bool val = (valueStr == "true");
                    set(key, val, ConfigSource::Runtime);
                } else {
                    int64_t val = std::stoll(valueStr);
                    set(key, val, ConfigSource::Runtime);
                }
            } catch (...) {
                set(key, valueStr, ConfigSource::Runtime);
            }
            
            pos = newline + 1;
        }
    }
    
    return true;
}

std::vector<ConfigEntry> Config::getHistory(const std::string& key, size_t maxEntries) const {
    std::vector<ConfigEntry> result;
    
    for (auto it = pImpl->history.rbegin(); it != pImpl->history.rend() && result.size() < maxEntries; ++it) {
        if (it->key == key) {
            result.push_back(*it);
        }
    }
    
    // Reverse to get chronological order
    std::reverse(result.begin(), result.end());
    return result;
}

bool Config::undoLastChange(const std::string& key) {
    auto historyIt = std::find_if(pImpl->history.rbegin(), pImpl->history.rend(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (historyIt == pImpl->history.rend()) {
        return false; // No history found
    }
    
    // Get the previous value
    const ConfigEntry& entry = *historyIt;
    
    // Update current value
    auto currentIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (currentIt != pImpl->entries.end()) {
        currentIt->value = entry.value;
    }
    
    return true;
}

bool Config::resetToVersion(const std::string& key, size_t version) {
    auto historyIt = std::find_if(pImpl->history.begin(), pImpl->history.end(),
        [&key, &version](const ConfigEntry& e) { return e.key == key && pImpl->keyVersions[key] >= version; });
    
    if (historyIt == pImpl->history.end()) {
        return false; // Version not found
    }
    
    // Update current value
    auto currentIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (currentIt != pImpl->entries.end()) {
        currentIt->value = historyIt->value;
        currentIt->source = historyIt->source;
    }
    
    return true;
}

std::shared_ptr<Config> Config::createPreset(const std::string& presetName) {
    auto config = std::make_shared<Config>();
    
    if (presetName == "fast") {
        config->setWithDescription("neuron_count", int64_t(100), "Small brain for fast testing", ConfigSource::Default);
        config->setWithDescription("simulation_timestep", 0.01, "Coarser timestep for speed", ConfigSource::Default);
        config->setWithDescription("max_simulation_steps", int64_t(1000), "Fewer steps for testing", ConfigSource::Default);
        config->setWithDescription("plasticity_learning_rate", 0.05, "Higher learning rate", ConfigSource::Default);
    } else if (presetName == "accurate") {
        config->setWithDescription("neuron_count", int64_t(5000), "Larger brain for accuracy", ConfigSource::Default);
        config->setWithDescription("simulation_timestep", 0.001, "Fine timestep for accuracy", ConfigSource::Default);
        config->setWithDescription("max_simulation_steps", int64_t(50000), "More steps for convergence", ConfigSource::Default);
        config->setWithDescription("plasticity_learning_rate", 0.005, "Conservative learning rate", ConfigSource::Default);
    } else if (presetName == "balanced") {
        config->setWithDescription("neuron_count", int64_t(1000), "Medium brain size", ConfigSource::Default);
        config->setWithDescription("simulation_timestep", 0.001, "Standard timestep", ConfigSource::Default);
        config->setWithDescription("max_simulation_steps", int64_t(10000), "Standard steps", ConfigSource::Default);
        config->setWithDescription("plasticity_learning_rate", 0.01, "Balanced learning rate", ConfigSource::Default);
    } else if (presetName == "debug") {
        config->setWithDescription("neuron_count", int64_t(50), "Very small brain for debugging", ConfigSource::Default);
        config->setWithDescription("simulation_timestep", 0.01, "Coarse timestep", ConfigSource::Default);
        config->setWithDescription("max_simulation_steps", int64_t(100), "Very few steps", ConfigSource::Default);
        config->setWithDescription("plasticity_learning_rate", 0.1, "High learning rate for debugging", ConfigSource::Default);
    }
    
    return config;
}

std::vector<std::string> Config::getAvailablePresets() {
    return {"fast", "accurate", "balanced", "debug"};
}

size_t Config::estimateMemoryUsage() const {
    size_t total = 0;
    
    // Estimate for each entry
    for (const auto& entry : pImpl->entries) {
        total += entry.key.size();
        total += sizeof(ConfigValue);
        total += sizeof(ConfigSource);
        total += entry.description.size();
    }
    
    // Estimate for history
    for (const auto& entry : pImpl->history) {
        total += entry.key.size();
        total += sizeof(ConfigValue);
        total += sizeof(ConfigSource);
        total += entry.description.size();
    }
    
    return total;
}

std::string Config::Stats::formatMemoryUsage(size_t bytes) {
    if (bytes < 1024) return std::to_string(bytes) + " B";
    if (bytes < 1024 * 1024) return std::to_string(bytes / 1024) + " KB";
    if (bytes < 1024 * 1024 * 1024) return std::to_string(bytes / (1024 * 1024)) + " MB";
    return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
}

std::string Config::Stats::formatTime(double seconds) {
    if (seconds < 60) return std::to_string(static_cast<int>(seconds)) + "s";
    if (seconds < 3600) return std::to_string(static_cast<int>(seconds / 60)) + "m " + 
                                   std::to_string(static_cast<int>(seconds) % 60) + "s";
    return std::to_string(static_cast<int>(seconds / 3600)) + "h " +
           std::to_string(static_cast<int>((seconds / 60) % 60)) + "m";
}

std::string Config::Stats::formatCount(size_t count) {
    if (count < 1000) return std::to_string(count);
    if (count < 1000000) return std::to_string(count / 1000) + "K";
    return std::to_string(count / 1000000) + "M";
}

} // namespace nlm
