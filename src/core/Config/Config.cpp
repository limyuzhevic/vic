#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

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

} // namespace nlm

// ==============================================================================
// ADVANCED CONFIGURATION (NLM Phase 8)
// ==============================================================================

// Configuration merger
void Config::merge(const Config& other, bool overwrite) {
    NLM_LOG_INFO("Merging configurations: overwrite=" + std::string(overwrite ? "true" : "false"));
    
    for (const auto& entry : other.pImpl->entries) {
        if (overwrite || !has(entry.key)) {
            set(entry.key, entry.value, entry.source);
            NLM_LOG_INFO("  Merged key: " + entry.key);
        }
    }
}

bool Config::validate(std::string& errors, std::string& warnings) const {
    NLM_LOG_INFO("Validating configuration");
    
    errors.clear();
    warnings.clear();
    
    bool isValid = true;
    
    // Basic validation rules
    for (const auto& entry : pImpl->entries) {
        const std::string& key = entry.key;
        
        // Check for empty keys
        if (key.empty()) {
            errors += "Empty configuration key found\n";
            isValid = false;
        }
        
        // Validate specific key patterns
        if (key.find("brain.") == 0) {
            // Brain-related validation
            if (key == "brain.neuronCount" || key == "brain.regionCount") {
                auto value = get<size_t>(key);
                if (value && *value == 0) {
                    warnings += "Zero " + key + " may cause issues\n";
                } else if (value && *value > 100000) {
                    errors += key + " too large (max 100000)\n";
                    isValid = false;
                }
            }
        }
        
        else if (key.find("plasticity.") == 0) {
            // Plasticity validation
            if (key == "plasticity.stdpLTPWeight" || key == "plasticity.stdpLTDWeight") {
                auto value = get<double>(key);
                if (value && (*value < 0.0 || *value > 1.0)) {
                    warnings += key + " outside [0.0, 1.0] range\n";
                }
            }
        }
        
        else if (key.find("memory.") == 0) {
            // Memory validation
            if (key == "memory.workingMemoryCapacity" || key == "memory.episodicCapacity") {
                auto value = get<size_t>(key);
                if (value && *value > 10000) {
                    warnings += key + " very large (consider reducing)\n";
                }
            }
        }
    }
    
    if (isValid && warnings.empty()) {
        NLM_LOG_INFO("Configuration validation passed");
    }
    
    return isValid;
}

void Config::optimize() {
    NLM_LOG_INFO("Optimizing configuration");
    
    // Optimization logic
    // 1. Remove redundant entries
    std::vector<ConfigEntry> toKeep;
    std::vector<std::string> seenKeys;
    
    for (auto it = pImpl->entries.rbegin(); it != pImpl->entries.rend(); ++it) {
        const auto& entry = *it;
        if (std::find(seenKeys.begin(), seenKeys.end(), entry.key) == seenKeys.end()) {
            seenKeys.push_back(entry.key);
            toKeep.push_back(entry);
        }
    }
    
    // Reverse back to original order
    std::reverse(toKeep.begin(), toKeep.end());
    pImpl->entries = toKeep;
    
    NLM_LOG_INFO("Configuration optimized: " + std::to_string(pImpl->entries.size()) + " entries");
}

std::unique_ptr<Config> Config::createAdvancedConfig(const std::string& presetName) const {
    NLM_LOG_INFO("Creating advanced configuration: " + presetName);
    
    auto advancedConfig = std::make_unique<Config>();
    
    // Add common advanced settings
    advancedConfig->set("brain.neuronCount", 5000);
    advancedConfig->set("brain.regionCount", 3);
    advancedConfig->set("brain.connectionProbability", 0.15f);
    
    // Memory settings
    advancedConfig->set("memory.workingMemoryCapacity", 500);
    advancedConfig->set("memory.episodicCapacity", 5000);
    advancedConfig->set("memory.associativeCapacity", 2000);
    
    // Plasticity settings
    advancedConfig->set("plasticity.stdpLTPWeight", 0.02f);
    advancedConfig->set("plasticity.stdpLTDWeight", 0.03f);
    advancedConfig->set("plasticity.hebbianLearningRate", 0.002f);
    
    // Prediction settings
    advancedConfig->set("prediction.planningDepth", 8);
    advancedConfig->set("prediction.predictionHorizon", 20);
    
    // Neuromodulation settings
    advancedConfig->set("neuromodulation.dopamineBaseline", 0.15f);
    advancedConfig->set("neuromodulation.curiosityScale", 1.2f);
    advancedConfig->set("neuromodulation.noveltyThreshold", 0.3f);
    
    // Development settings
    advancedConfig->set("development.criticalPeriodDuration", 300.0f);
    advancedConfig->set("development.synaptogenesisRate", 0.0005f);
    advancedConfig->set("development.pruningRate", 0.00002f);
    
    // Performance settings
    advancedConfig->set("performance.simulationTimestep", 0.002);
    advancedConfig->set("performance.checkpointInterval", 10000);
    advancedConfig->set("performance.maxMemoryUsage", 512 * 1024 * 1024); // 512MB
    
    // Reproducibility
    advancedConfig->set("random.seed", 12345);
    advancedConfig->set("random.algorithm", "mt19937");
    
    NLM_LOG_INFO("Advanced configuration created: " + presetName);
    return advancedConfig;
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}
