#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <chrono>
#include <ctime>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string description;
    std::string filename;
    std::map<std::string, std::string> metadata;
    std::vector<std::pair<std::string, std::string>> validationRules;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

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

        if (line.find('=') != std::string::npos) {
            std::string key = trim(line.substr(0, line.find('=')));
            std::string value = trim(line.substr(line.find('=') + 1));
            set(key, value, ConfigSource::File);
        } else if (line.find(':') != std::string::npos) {
            // Try YAML-like format
            std::string key = trim(line.substr(0, line.find(':')));
            std::string value = trim(line.substr(line.find(':') + 1));
            set(key, value, ConfigSource::File);
        }
    }

    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    // Enhanced command-line argument parsing
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
        // Handle --key (boolean flags)
        else if (arg[0] == '-' && arg.size() > 1) {
            std::string key = arg.substr(1);
            set(key, "true", ConfigSource::CommandLine);
        }
        // Handle JSON-like --key="value" --key="another value"
        else if (arg.find('"') != std::string::npos) {
            size_t keyStart = arg.find_first_of('-');
            if (keyStart != std::string::npos) {
                size_t quotePos = arg.find('"', keyStart);
                if (quotePos != std::string::npos) {
                    std::string key = arg.substr(keyStart + 1, quotePos - keyStart - 2);
                    size_t valueStart = arg.find('"', quotePos + 1);
                    if (valueStart != std::string::npos) {
                        size_t valueEnd = arg.find('"', valueStart + 1);
                        if (valueEnd != std::string::npos) {
                            std::string value = arg.substr(valueStart + 1, valueEnd - valueStart - 1);
                            set(key, value, ConfigSource::CommandLine);
                        }
                    }
                }
            }
        }
    }

    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    // Add metadata and comments
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated on: " << getTimestamp() << std::endl;
    file << "# Version: 1.0" << std::endl;
    file << std::endl;

    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << " (source: " << static_cast<int>(entry.source) << ")" << std::endl;
        file << entry.key << " = " << entry.value << std::endl;
        file << std::endl;
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
        updateLastModified();
    } else {
        pImpl->entries.emplace_back(key, value, source);
        updateLastModified();
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
    updateLastModified();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "=== NLM Configuration Summary ===" << std::endl;
    oss << "Loaded on: " << getTimestamp() << std::endl;
    oss << "Total entries: " << pImpl->entries.size() << std::endl;
    oss << std::endl;

    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << entry.key << std::endl;
        oss << "  Value: ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << std::endl;
        oss << "  Source: " << static_cast<int>(entry.source) << std::endl;
        oss << std::endl;
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

std::string Config::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void Config::updateLastModified() {
    pImpl->description = "Updated: " + getTimestamp();
}

void Config::addMetadata(const std::string& key, const std::string& value) {
    pImpl->metadata[key] = value;
}

std::string Config::getMetadata(const std::string& key) const {
    auto it = pImpl->metadata.find(key);
    if (it != pImpl->metadata.end()) {
        return it->second;
    }
    return "";
}

void Config::addValidationRule(const std::string& key, const std::string& rule) {
    pImpl->validationRules.emplace_back(key, rule);
}

bool Config::validate() const {
    for (const auto& rule : pImpl->validationRules) {
        auto val = get<std::string>(rule.first);
        if (!val.has_value()) {
            return false;
        }
        // TODO: Implement actual validation logic based on rule
    }
    return true;
}

std::string Config::exportToJSON() const {
    std::ostringstream oss;
    oss << "{" << std::endl;
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            oss << "," << std::endl;
        }
        oss << "  \"" << entry.key << "\": ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, float>) {
                oss << arg << "f";
            } else if constexpr (std::is_same_v<T, double>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            }
        }, entry.value);
        
        first = false;
    }
    
    oss << std::endl << "}" << std::endl;
    return oss.str();
}

bool Config::importFromJSON(const std::string& jsonStr) {
    // Simple JSON parser - in production, use a proper JSON library
    size_t pos = jsonStr.find("{");
    size_t end = jsonStr.rfind("}");
    if (pos == std::string::npos || end == std::string::npos) {
        return false;
    }
    
    std::string content = jsonStr.substr(pos + 1, end - pos - 1);
    
    // Parse key-value pairs (simplified)
    size_t keyStart = 0;
    size_t keyEnd;
    while ((keyEnd = content.find(":", keyStart)) != std::string::npos) {
        std::string key = content.substr(keyStart, keyEnd - keyStart);
        key = trim(key);
        // Remove quotes
        if (key.size() >= 2 && key.front() == '"' && key.back() == '"') {
            key = key.substr(1, key.size() - 2);
        }
        
        size_t valueStart = content.find('"', keyEnd + 1);
        if (valueStart == std::string::npos) {
            valueStart = keyEnd + 1;
        } else {
            size_t valueEnd = content.find('"', valueStart + 1);
            if (valueEnd != std::string::npos) {
                std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
                set(key, value, ConfigSource::File);
            }
        }
        
        keyStart = content.find(',', keyEnd) + 1;
        if (keyStart == 0) break;
    }
    
    return true;
}

std::shared_ptr<Config> Config::createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Set default values for NLM Phase 6
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Plasticity settings
    config->set("plasticity.learning_rate", 0.01f, ConfigSource::Default);
    config->set("plasticity.stdp.enable", true, ConfigSource::Default);
    config->set("plasticity.stdp.ltp_weight", 0.01f, ConfigSource::Default);
    config->set("plasticity.stdp.ltd_weight", 0.012f, ConfigSource::Default);
    config->set("plasticity.stdp.tau", 20.0f, ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
    config->set("plasticity.structural.enable", true, ConfigSource::Default);
    
    // Neuromodulation settings
    config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
    config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
    config->set("neuromod.novelty.enable", true, ConfigSource::Default);
    
    // Development settings
    config->set("development.synaptogenesis_rate", 0.001f, ConfigSource::Default);
    config->set("development.pruning_rate", 0.0001f, ConfigSource::Default);
    
    // Memory settings
    config->set("memory.working_capacity", 1000, ConfigSource::Default);
    config->set("memory.episodic.max_episodes", 1000, ConfigSource::Default);
    
    // Prediction settings
    config->set("prediction.error_threshold", 0.1f, ConfigSource::Default);
    
    // Cognition settings
    config->set("cognition.planning.depth", 5, ConfigSource::Default);
    config->set("cognition.attention.inhibition_strength", 0.5f, ConfigSource::Default);
    
    // Environment settings
    config->set("environment.name", "GridWorld", ConfigSource::Default);
    config->set("environment.width", 20, ConfigSource::Default);
    config->set("environment.height", 20, ConfigSource::Default);
    
    // Logging settings
    config->set("logging.level", "INFO", ConfigSource::Default);
    config->set("logging.to_file", false, ConfigSource::Default);
    config->set("logging.filename", "nlm.log", ConfigSource::Default);
    
    // Simulation settings
    config->set("simulation.max_steps", 10000, ConfigSource::Default);
    config->set("simulation.time_limit", 0.0, ConfigSource::Default);
    
    config->addMetadata("version", "0.1.0");
    config->addMetadata("phase", "6");
    config->addMetadata("description", "Phase 6: Final Integration - All Systems Connected");
    
    return config;
}

void Config::setDefaultValues() {
    // Set all configuration parameters to their defaults
    clear();
    
    // Core simulation parameters
    set("random_seed", 42, ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", 1000, ConfigSource::Default);
    set("region_count", 2, ConfigSource::Default);
    set("connection_probability", 0.1, ConfigSource::Default);
    
    // Plasticity system
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    set("hebbian_learning_rate", 0.01f, ConfigSource::Default);
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Neuromodulation
    set("dopamine_scale", 1.0f, ConfigSource::Default);
    set("curiosity_enabled", true, ConfigSource::Default);
    set("novelty_enabled", true, ConfigSource::Default);
    
    // Development
    set("developmental_stage_initial_plasticity", 1.0f, ConfigSource::Default);
    
    // Memory
    set("working_memory_capacity", 1000, ConfigSource::Default);
    set("episodic_memory_max_episodes", 1000, ConfigSource::Default);
    
    // Prediction
    set("prediction_error_threshold", 0.1f, ConfigSource::Default);
    
    // Cognition
    set("planning_depth", 5, ConfigSource::Default);
    set("attention_inhibition_strength", 0.5f, ConfigSource::Default);
    
    // Environment
    set("world_width", 20, ConfigSource::Default);
    set("world_height", 20, ConfigSource::Default);
    set("vision_width", 8, ConfigSource::Default);
    set("vision_height", 8, ConfigSource::Default);
    
    // Logging
    set("log_level", "INFO", ConfigSource::Default);
    set("log_to_file", false, ConfigSource::Default);
    
    // Performance
    set("max_simulation_steps", 10000, ConfigSource::Default);
    set("checkpoint_interval", 1000, ConfigSource::Default);
    
    updateLastModified();
}

std::string Config::getDescription() const {
    return pImpl->description;
}

std::string Config::getFilename() const {
    return pImpl->filename;
}

void Config::setFilename(const std::string& filename) {
    pImpl->filename = filename;
}

std::string Config::getLastModified() const {
    return pImpl->description; // Currently stores timestamp in description
}

ConfigEntry::ConfigEntry(const std::string& key, const ConfigValue& value, ConfigSource source)
    : key(key), value(value), source(source) {
    description = "Generated entry for key: " + key;
}

const std::string& ConfigEntry::getKey() const {
    return key;
}

const ConfigValue& ConfigEntry::getValue() const {
    return value;
}

ConfigSource ConfigEntry::getSource() const {
    return source;
}

const std::string& ConfigEntry::getDescription() const {
    return description;
}

void ConfigEntry::setDescription(const std::string& desc) {
    description = desc;
}

} // namespace nlm