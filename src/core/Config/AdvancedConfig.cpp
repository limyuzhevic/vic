//=============================================================
// ADVANCED CONFIGURATION SYSTEM IMPLEMENTATION
//=============================================================

#include "AdvancedConfig.hpp"
#include "../core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <algorithm>
#include <unordered_map>
#include <sstream>

using json = nlohmann::json;

namespace nlm {

struct AdvancedConfig::Impl {
    // File format parsers
    std::unique_ptr<JSONParser> jsonParser;
    std::unique_ptr<YAMLParser> yamlParser;
    
    // Configuration metadata
    std::string currentFilepath;
    std::string format;
    bool autoSaveEnabled;
    std::string autoSaveDirectory;
    int autoSaveInterval;
    
    // Validation state
    mutable std::vector<std::string> validationErrors;
    mutable bool validationDirty;
    
    // Configuration statistics
    size_t entryCount;
    std::map<std::string, int> typeDistribution;
    double memoryUsage;
    
    // Callbacks
    std::function<void()> configWatcherCallback;
    std::string watchedFilepath;
    
    Impl() : format("key=value"), autoSaveEnabled(false), autoSaveInterval(100), 
             validationDirty(true), entryCount(0), memoryUsage(0.0) {}
};

AdvancedConfig::AdvancedConfig() : pImpl(std::make_unique<Impl>()) {}

AdvancedConfig::~AdvancedConfig() = default;

bool AdvancedConfig::loadFromJSON(const std::string& filepath) {
    pImpl->jsonParser = std::make_unique<JSONParser>();
    return pImpl->jsonParser->load(filepath, *this);
}

bool AdvancedConfig::loadFromYAML(const std::string& filepath) {
    pImpl->yamlParser = std::make_unique<YAMLParser>();
    return pImpl->yamlParser->load(filepath, *this);
}

bool AdvancedConfig::loadFromTOML(const std::string& filepath) {
    // TODO: Implement TOML parser
    NLM_LOG_WARNING("TOML format not yet implemented");
    return false;
}

bool AdvancedConfig::saveToJSON(const std::string& filepath, bool pretty) const {
    pImpl->jsonParser = std::make_unique<JSONParser>();
    return pImpl->jsonParser->save(filepath, *this, pretty);
}

bool AdvancedConfig::saveToYAML(const std::string& filepath) const {
    pImpl->yamlParser = std::make_unique<YAMLParser>();
    return pImpl->yamlParser->save(filepath, *this);
}

bool AdvancedConfig::validateSchema() const {
    pImpl->validationErrors.clear();
    
    // Basic validation rules
    for (const auto& entry : getKeys()) {
        if (!ConfigUtils::validateKeyFormat(entry)) {
            pImpl->validationErrors.push_back("Invalid key format: " + entry);
        }
        
        auto value = get<ConfigValue>(entry);
        if (value && !ConfigUtils::validateValueType(entry, *value)) {
            pImpl->validationErrors.push_back("Invalid value type for key: " + entry);
        }
    }
    
    // Check for required keys
    static const std::vector<std::string> requiredKeys = {
        "brain.neuron_count",
        "simulation_timestep",
        "plasticity.stdp.enable"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            pImpl->validationErrors.push_back("Missing required key: " + key);
        }
    }
    
    pImpl->validationDirty = false;
    return pImpl->validationErrors.empty();
}

std::vector<std::string> AdvancedConfig::getValidationErrors() const {
    if (pImpl->validationDirty) {
        validateSchema();
    }
    return pImpl->validationErrors;
}

bool AdvancedConfig::mergeFromFile(const std::string& filepath) {
    // Detect file format by extension
    std::string ext = std::filesystem::path(filepath).extension().string();n    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return mergeFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return mergeFromYAML(filepath);
    } else {
        // Try to detect format
        return mergeFromString(getFileContent(filepath), getInferredFormat(filepath));
    }
}

bool AdvancedConfig::mergeFromString(const std::string& content) {
    std::string format = getInferredFormatFromContent(content);
    return ConfigUtils::configFromString(*this, content, format);
}

std::shared_ptr<AdvancedConfig> AdvancedConfig::createDefaultConfig() {
    auto config = std::make_shared<AdvancedConfig>();
    
    // Brain configuration
    config->set("brain.neuron_count", 1000);
    config->set("brain.region_count", 1);
    config->set("brain.connection_probability", 0.1f);
    config->set("brain.initial_weight_mean", 0.5f);
    config->set("brain.initial_weight_std", 0.1f);
    config->set("brain.v_thresh", -50.0f);
    config->set("brain.v_rest", -70.0f);
    config->set("brain.v_reset", -75.0f);
    config->set("brain.tau_mem", 20.0f);
    config->set("brain.tau_ref", 2.0f);
    
    // Plasticity configuration
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.stdp.learning_rate", 0.001f);
    config->set("plasticity.stdp.tau_plus", 20.0f);
    config->set("plasticity.stdp.tau_minus", 20.0f);
    config->set("plasticity.hebbian.enable", true);
    config->set("plasticity.structural.enable", true);
    config->set("plasticity.synaptogenesis_rate", 0.0001f);
    config->set("plasticity.pruning_rate", 0.00001f);
    
    // Neuromodulation configuration
    config->set("neuromod.dopamine.scale", 1.0f);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    
    // Memory configuration
    config->set("memory.working.capacity", 100);
    config->set("memory.episodic.max_episodes", 1000);
    config->set("memory.associative.max_associations", 5000);
    
    // Simulation configuration
    config->set("simulation.timestep", 0.001);
    config->set("simulation.max_steps", 10000);
    config->set("random_seed", 42u);
    
    return config;
}

std::shared_ptr<AdvancedConfig> AdvancedConfig::createMinimalConfig() {
    auto config = std::make_shared<AdvancedConfig>();
    
    // Minimal required configuration
    config->set("brain.neuron_count", 100);
    config->set("brain.region_count", 1);
    config->set("simulation.timestep", 0.001);
    config->set("plasticity.stdp.enable", true);
    
    return config;
}

std::shared_ptr<AdvancedConfig> AdvancedConfig::createResearchConfig() {
    auto config = std::make_shared<AdvancedConfig>();
    
    // Enhanced configuration for research
    config->set("brain.neuron_count", 10000);
    config->set("brain.region_count", 4);
    config->set("brain.connection_probability", 0.05f);
    config->set("plasticity.stdp.learning_rate", 0.01f);
    config->set("plasticity.structural.enable", true);
    config->set("plasticity.pruning_rate", 0.000001f);
    config->set("memory.episodic.max_episodes", 5000);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    
    // Advanced research parameters
    config->set("research.emergent_behaviors", true);
    config->set("research.development_stages", true);
    config->set("research.memory_consolidation", true);
    config->set("research.bistability_support", true);
    
    return config;
}

std::shared_ptr<AdvancedConfig> AdvancedConfig::createRealTimeConfig() {
    auto config = std::make_shared<AdvancedConfig>();
    
    // Configuration optimized for real-time performance
    config->set("brain.neuron_count", 500);
    config->set("brain.region_count", 2);
    config->set("simulation.timestep", 0.0001);
    config->set("simulation.max_steps", 1000);
    config->set("plasticity.stdp.enable", false); // Disable for speed
    config->set("memory.working.capacity", 50);
    config->set("neuromod.curiosity.enable", false); // Disable for speed
    
    return config;
}

void AdvancedConfig::setAutoSave(const std::string& directory, int intervalSteps) {
    pImpl->autoSaveEnabled = true;
    pImpl->autoSaveDirectory = directory;
    pImpl->autoSaveInterval = intervalSteps;
    
    // Create directory if it doesn't exist
    std::filesystem::create_directories(directory);
    NLM_LOG_INFO("Auto-save enabled: directory='" + directory + "', interval=" + std::to_string(intervalSteps) + " steps");
}

void AdvancedConfig::setConfigWatcher(const std::string& filepath, std::function<void()> callback) {
    pImpl->watchedFilepath = filepath;
    pImpl->configWatcherCallback = callback;
    NLM_LOG_INFO("Config watcher set for file: " + filepath);
}

std::string AdvancedConfig::getConfigSummary() const {
    std::ostringstream oss;
    oss << "=== NLM Advanced Configuration ===" << std::endl;
    oss << "Format: " << pImpl->format << std::endl;
    oss << "Total entries: " << getEntryCount() << std::endl;
    oss << "Memory usage: " << getMemoryUsage() << " KB" << std::endl;
    oss << "Type distribution: " << std::endl;
    
    for (const auto& pair : pImpl->typeDistribution) {
        oss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    oss << "Auto-save: " << (pImpl->autoSaveEnabled ? "enabled" : "disabled") << std::endl;
    if (pImpl->autoSaveEnabled) {
        oss << "  Directory: " << pImpl->autoSaveDirectory << std::endl;
        oss << "  Interval: " << pImpl->autoSaveInterval << " steps" << std::endl;
    }
    
    return oss.str();
}

std::string AdvancedConfig::toString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << "UNKNOWN_TYPE";
        }
    }, value);
    
    return oss.str();
}

bool AdvancedConfig::fromString(const std::string& str, ConfigValue& value) {
    // Try to detect type
    if (str.empty()) return false;
    
    if (str == "true" || str == "false") {
        value = (str == "true");
        return true;
    }
    
    if (str.front() == '"' && str.back() == '"') {
        value = str.substr(1, str.size() - 2);
        return true;
    }
    
    try {
        if (str.find('.') != std::string::npos) {
            value = std::stod(str);
        } else {
            // Try integer first
            size_t pos;
            long long_val = std::stoll(str, &pos);
            if (pos == str.size()) {
                value = long_val;
                return true;
            }
            
            // Try unsigned long long
            unsigned long long unsigned_val = std::stoull(str, &pos);
            if (pos == str.size()) {
                value = unsigned_val;
                return true;
            }
        }
    } catch (const std::exception&) {
        // Fall through to string conversion
        value = str;
        return true;
    }
    
    return false;
}

size_t AdvancedConfig::getEntryCount() const {
    pImpl->entryCount = Config::getKeys().size();
    return pImpl->entryCount;
}

std::map<std::string, int> AdvancedConfig::getTypeDistribution() const {
    pImpl->typeDistribution.clear();
    
    for (const auto& key : getKeys()) {
        auto value = get<ConfigValue>(key);
        if (value) {
            std::string typeName = getTypeName(*value);
            pImpl->typeDistribution[typeName]++;
        }
    }
    
    return pImpl->typeDistribution;
}

double AdvancedConfig::getMemoryUsage() const {
    // Estimate memory usage based on entry count and string sizes
    double totalSize = 0.0;
    
    for (const auto& entry : getKeys()) {
        auto value = get<ConfigValue>(entry);
        if (value) {
            totalSize += sizeof(ConfigEntry) + getTypeSize(*value);
        }
    }
    
    pImpl->memoryUsage = totalSize / 1024.0; // KB
    return pImpl->memoryUsage;
}

std::vector<AdvancedConfig::ConfigurationTemplate> AdvancedConfig::getAvailableTemplates() {
    return {
        {
            "default",
            "Standard configuration for basic simulations",
            getBrainDefaults(),
            {"brain.neuron_count", "simulation.timestep"}
        },
        {
            "research",
            "Configuration optimized for research and experimentation",
            getResearchDefaults(),
            {"brain.neuron_count"}
        },
        {
            "real_time",
            "Configuration optimized for real-time performance",
            getRealTimeDefaults(),
            {"brain.neuron_count", "simulation.timestep"}
        },
        {
            "minimal",
            "Minimal configuration with only essential parameters",
            getMinimalDefaults(),
            {"brain.neuron_count"}
        }
    };
}

bool AdvancedConfig::exportToXML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    file << "<configuration>" << std::endl;
    
    for (const auto& entry : getKeys()) {
        auto value = get<ConfigValue>(entry);
        if (value) {
            file << "  <entry key=\"" << entry << "\">";
            file << toString(*value);
            file << "</entry>" << std::endl;
        }
    }
    
    file << "</configuration>" << std::endl;
    file.close();
    return true;
}

bool AdvancedConfig::importFromXML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple XML parsing - could be enhanced
    std::string line;
    while (std::getline(file, line)) {
        // Parse <entry key="value">content</entry>
        size_t keyStart = line.find("key=\"");
        if (keyStart == std::string::npos) continue;
        
        keyStart += 6; // Skip "key=\""
        size_t keyEnd = line.find('"', keyStart);
        if (keyEnd == std::string::npos) continue;
        
        std::string key = line.substr(keyStart, keyEnd - keyStart);
        
        size_t contentStart = line.find('>', keyEnd);
        size_t contentEnd = line.find('<', keyStart);
        if (contentStart == std::string::npos || contentEnd == std::string::npos) continue;
        
        std::string content = line.substr(contentStart + 1, contentEnd - contentStart - 1);
        
        // Convert content back to value
        ConfigValue value;
        if (fromString(content, value)) {
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

void AdvancedConfig::batchSet(const std::map<std::string, std::string>& values) {
    for (const auto& pair : values) {
        ConfigValue value;
        if (fromString(pair.second, value)) {
            set(pair.first, value, ConfigSource::CommandLine);
        } else {
            NLM_LOG_WARNING("Failed to parse value for key '" + pair.first + "': " + pair.second);
        }
    }
}

std::map<std::string, std::string> AdvancedConfig::batchGet(const std::vector<std::string>& keys) const {
    std::map<std::string, std::string> result;
    
    for (const auto& key : keys) {
        auto value = get<ConfigValue>(key);
        if (value) {
            result[key] = toString(*value);
        }
    }
    
    return result;
}

bool AdvancedConfig::loadFromFile(const std::string& filepath) {
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Fall back to original key=value format
        return Config::loadFromFile(filepath);
    }
}

bool AdvancedConfig::saveToFile(const std::string& filepath) const {
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return saveToJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return saveToYAML(filepath);
    } else {
        // Fall back to original key=value format
        return Config::saveToFile(filepath);
    }
}

std::string AdvancedConfig::getInferredFormat(const std::string& filepath) {
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") return "json";
    if (ext == ".yaml" || ext == ".yml") return "yaml";
    if (ext == ".toml") return "toml";
    
    return "key=value";
}

std::string AdvancedConfig::getInferredFormatFromContent(const std::string& content) {
    // Simple detection based on content patterns
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        return "json";
    }
    
    if (content.find('- ') != std::string::npos && content.find(':') != std::string::npos) {
        return "yaml";
    }
    
    return "key=value";
}

std::string AdvancedConfig::getFileContent(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string AdvancedConfig::getTypeName(const ConfigValue& value) {
    std::string typeName;
    std::visit([&typeName](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) typeName = "string";
        else if constexpr (std::is_same_v<T, int>) typeName = "int";
        else if constexpr (std::is_same_v<T, int64_t>) typeName = "int64";
        else if constexpr (std::is_same_v<T, double>) typeName = "double";
        else if constexpr (std::is_same_v<T, bool>) typeName = "bool";
        else typeName = "unknown";
    }, value);
    return typeName;
}

double AdvancedConfig::getTypeSize(const ConfigValue& value) {
    double size = sizeof(ConfigEntry);
    std::visit([&size](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) size += sizeof(std::string);
        else if constexpr (std::is_same_v<T, int>) size += sizeof(int);
        else if constexpr (std::is_same_v<T, int64_t>) size += sizeof(int64_t);
        else if constexpr (std::is_same_v<T, double>) size += sizeof(double);
        else if constexpr (std::is_same_v<T, bool>) size += sizeof(bool);
    }, value);
    return size;
}

std::map<std::string, std::string> ConfigUtils::getBrainDefaults() {
    return {
        {"brain.neuron_count", "1000"},
        {"brain.region_count", "1"},
        {"brain.connection_probability", "0.1"},
        {"brain.initial_weight_mean", "0.5"},
        {"brain.initial_weight_std", "0.1"},
        {"brain.v_thresh", "-50.0"},
        {"brain.v_rest", "-70.0"},
        {"brain.v_reset", "-75.0"},
        {"brain.tau_mem", "20.0"},
        {"brain.tau_ref", "2.0"}
    };
}

std::map<std::string, std::string> ConfigUtils::getPlasticityDefaults() {
    return {
        {"plasticity.stdp.enable", "true"},
        {"plasticity.stdp.learning_rate", "0.001"},
        {"plasticity.stdp.tau_plus", "20.0"},
        {"plasticity.stdp.tau_minus", "20.0"},
        {"plasticity.hebbian.enable", "true"},
        {"plasticity.structural.enable", "true"},
        {"plasticity.synaptogenesis_rate", "0.0001"},
        {"plasticity.pruning_rate", "0.00001"}
    };
}

std::map<std::string, std::string> ConfigUtils::getMemoryDefaults() {
    return {
        {"memory.working.capacity", "100"},
        {"memory.episodic.max_episodes", "1000"},
        {"memory.associative.max_associations", "5000"}
    };
}

std::map<std::string, std::string> ConfigUtils::getNeuromodulationDefaults() {
    return {
        {"neuromod.dopamine.scale", "1.0"},
        {"neuromod.curiosity.enable", "true"},
        {"neuromod.novelty.enable", "true"}
    };
}

std::map<std::string, std::string> ConfigUtils::getResearchDefaults() {
    std::map<std::string, std::string> defaults = getBrainDefaults();
    std::map<std::string, std::string> plasticityDefaults = getPlasticityDefaults();
    std::map<std::string, std::string> memoryDefaults = getMemoryDefaults();
    std::map<std::string, std::string> neuromodDefaults = getNeuromodulationDefaults();
    
    // Merge with research-specific overrides
    defaults["brain.neuron_count"] = "10000";
    defaults["brain.region_count"] = "4";
    defaults["brain.connection_probability"] = "0.05";
    
    plasticityDefaults["plasticity.stdp.learning_rate"] = "0.01";
    plasticityDefaults["plasticity.synaptogenesis_rate"] = "0.00001";
    
    memoryDefaults["memory.episodic.max_episodes"] = "5000";
    
    neuromodDefaults["neuromod.curiosity.enable"] = "true";
    neuromodDefaults["neuromod.novelty.enable"] = "true";
    
    defaults["research.emergent_behaviors"] = "true";
    defaults["research.development_stages"] = "true";
    defaults["research.memory_consolidation"] = "true";
    defaults["research.bistability_support"] = "true";
    
    // Merge all maps
    std::map<std::string, std::string> result = defaults;
    result.insert(plasticityDefaults.begin(), plasticityDefaults.end());
    result.insert(memoryDefaults.begin(), memoryDefaults.end());
    result.insert(neuromodDefaults.begin(), neuromodDefaults.end());
    
    return result;
}

std::map<std::string, std::string> ConfigUtils::getRealTimeDefaults() {
    std::map<std::string, std::string> defaults = getBrainDefaults();
    std::map<std::string, std::string> plasticityDefaults = getPlasticityDefaults();
    std::map<std::string, std::string> memoryDefaults = getMemoryDefaults();
    std::map<std::string, std::string> neuromodDefaults = getNeuromodulationDefaults();
    
    // Optimize for real-time performance
    defaults["brain.neuron_count"] = "500";
    defaults["brain.region_count"] = "2";
    defaults["simulation.timestep"] = "0.0001";
    defaults["simulation.max_steps"] = "1000";
    
    plasticityDefaults["plasticity.stdp.enable"] = "false";
    memoryDefaults["memory.working.capacity"] = "50";
    neuromodDefaults["neuromod.curiosity.enable"] = "false";
    neuromodDefaults["neuromod.novelty.enable"] = "false";
    
    std::map<std::string, std::string> result = defaults;
    result.insert(plasticityDefaults.begin(), plasticityDefaults.end());
    result.insert(memoryDefaults.begin(), memoryDefaults.end());
    result.insert(neuromodDefaults.begin(), neuromodDefaults.end());
    
    return result;
}

std::map<std::string, std::string> ConfigUtils::getMinimalDefaults() {
    return {
        {"brain.neuron_count", "100"},
        {"brain.region_count", "1"},
        {"simulation.timestep", "0.001"},
        {"plasticity.stdp.enable", "true"}
    };
}

std::string ConfigUtils::configToString(const Config& config, const std::string& format) {
    std::ostringstream oss;
    
    if (format == "json") {
        oss << "{\n";
        for (size_t i = 0; i < config.getKeys().size(); ++i) {
            oss << "  \"" << config.getKeys()[i] << "\": \";
            auto value = config.get<std::string>(config.getKeys()[i]);
            if (value) {
                oss << *value;
            }
            oss << "\"";
            if (i < config.getKeys().size() - 1) oss << ",\n";
        }
        oss << "}\n";
    } else if (format == "yaml") {
        for (const auto& key : config.getKeys()) {
            oss << key << ": ";
            auto value = config.get<std::string>(key);
            if (value) {
                oss << *value << std::endl;
            }
        }
    } else {
        // Default key=value format
        for (size_t i = 0; i < config.getKeys().size(); ++i) {
            oss << config.getKeys()[i] << " = ";
            auto value = config.get<std::string>(config.getKeys()[i]);
            if (value) {
                oss << *value;
            }
            if (i < config.getKeys().size() - 1) oss << std::endl;
        }
    }
    
    return oss.str();
}

bool ConfigUtils::configFromString(Config& config, const std::string& content, const std::string& format) {
    if (format == "json") {
        // Parse JSON
        auto jsonData = json::parse(content);
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            config.set(it.key(), it.value().get<std::string>(), ConfigSource::File);
        }
        return true;
    } else if (format == "yaml") {
        // Parse YAML
        auto yamlData = YAML::Load(content);
        for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
            config.set(it.first.as<std::string>(), it.second.as<std::string>(), ConfigSource::File);
        }
        return true;
    } else {
        // Parse key=value format
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line)) {
            // Parse simple key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                          (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                config.set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
}

bool ConfigUtils::validateKeyFormat(const std::string& key) {
    if (key.empty()) return false;
    if (key.find('=') != std::string::npos) return false;
    if (key.find(' ') != std::string::npos) return false;
    
    // Validate section.key format
    size_t dotPos = key.find('.');
    if (dotPos != std::string::npos) {
        std::string section = key.substr(0, dotPos);
        std::string subfield = key.substr(dotPos + 1);
        
        if (section.empty() || subfield.empty()) return false;
        
        // Check for valid section names
        static const std::vector<std::string> validSections = {
            "brain", "plasticity", "neuromod", "memory", "simulation", "research"
        };
        
        if (std::find(validSections.begin(), validSections.end(), section) == validSections.end()) {
            return false;
        }
    }
    
    return true;
}

bool ConfigUtils::validateValueType(const std::string& key, const ConfigValue& value) {
    // Simple validation based on key prefix
    if (key.find("brain.") == 0) {
        // Brain parameters: int, float, string
        return std::holds_alternative<int>(value) || std::holds_alternative<float>(value) || 
               std::holds_alternative<std::string>(value);
    } else if (key.find("plasticity.") == 0) {
        // Plasticity parameters: bool, float
        return std::holds_alternative<bool>(value) || std::holds_alternative<float>(value);
    } else if (key.find("simulation.") == 0) {
        // Simulation parameters: int, double
        return std::holds_alternative<int>(value) || std::holds_alternative<double>(value);
    }
    
    return true; // Default to accepting any type
}

void ConfigUtils::mergeConfigs(Config& target, const Config& source) {
    for (const auto& key : source.getKeys()) {
        auto value = source.get<ConfigValue>(key);
        if (value) {
            target.set(key, *value, ConfigSource::File);
        }
    }
}

bool ConfigUtils::isSubset(const Config& subset, const Config& superset) {
    for (const auto& key : subset.getKeys()) {
        if (!superset.has(key)) {
            return false;
        }
        
        auto subsetValue = subset.get<ConfigValue>(key);
        auto superSetValue = superset.get<ConfigValue>(key);
        
        if (subsetValue && superSetValue && *subsetValue != *superSetValue) {
            return false;
        }
    }
    
    return true;
}

std::string ConfigUtils::analyzeConfig(const Config& config) {
    std::ostringstream oss;
    oss << "Configuration Analysis:" << std::endl;
    oss << "Total entries: " << config.getKeys().size() << std::endl;
    
    // Count active entries
    size_t activeCount = 0;
    for (const auto& key : config.getKeys()) {
        auto value = config.get<std::string>(key);
        if (value && !value->empty()) {
            activeCount++;
        }
    }
    
    oss << "Active entries: " << activeCount << std::endl;
    oss << "Inactive entries: " << (config.getKeys().size() - activeCount) << std::endl;
    
    // Find entries with default values
    oss << "Default value entries: ";
    size_t defaultCount = 0;
    for (const auto& key : config.getKeys()) {
        auto value = config.get<std::string>(key);
        if (value && (*value == "0" || *value == "false" || *value == "")) {
            defaultCount++;
        }
    }
    
    oss << defaultCount << std::endl;
    
    return oss.str();
}

size_t ConfigUtils::countActiveEntries(const Config& config) {
    size_t activeCount = 0;
    for (const auto& key : config.getKeys()) {
        auto value = config.get<std::string>(key);
        if (value && !value->empty()) {
            activeCount++;
        }
    }
    return activeCount;
}

std::vector<std::string> ConfigUtils::findUnusedEntries(const Config& config) {
    std::vector<std::string> unused;
    for (const auto& key : config.getKeys()) {
        auto value = config.get<std::string>(key);
        if (value && (*value == "0" || *value == "false" || *value == "")) {
            unused.push_back(key);
        }
    }
    return unused;
}

// JSON Parser implementation
class JSONParser {
public:
    bool load(const std::string& filepath, Config& config) {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;
        
        
        json j;
        file >> j;
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            config.set(it.key(), it.value().get<std::string>(), ConfigSource::File);
        }
        
        return true;
    }
    
    bool save(const std::string& filepath, const Config& config, bool pretty) const {
        std::ofstream file(filepath);
        if (!file.is_open()) return false;
        
        json j;
        for (const auto& key : config.getKeys()) {
            auto value = config.get<std::string>(key);
            if (value) {
                j[key] = *value;
            }
        }
        
        if (pretty) {
            file << j.dump(2);
        } else {
            file << j.dump();
        }
        
        return true;
    }
};

// YAML Parser implementation
class YAMLParser {
public:
    bool load(const std::string& filepath, Config& config) {
        YAML::Node y;
        try {
            y = YAML::LoadFile(filepath);
        } catch (const YAML::Exception& e) {
            NLM_LOG_ERROR("YAML parsing error: " + std::string(e.what()));
            return false;
        }
        
        for (auto it = y.begin(); it != y.end(); ++it) {
            std::string key = it.first.as<std::string>();
            std::string value = it.second.as<std::string>();
            config.set(key, value, ConfigSource::File);
        }
        
        return true;
    }
    
    bool save(const std::string& filepath, const Config& config) const {
        YAML::Emitter y;
        y << YAML::BeginMap;
        
        for (const auto& key : config.getKeys()) {
            auto value = config.get<std::string>(key);
            if (value) {
                y << YAML::Key << key;
                y << YAML::Value << *value;
            }
        }
        
        y << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) return false;
        
        file << y.c_str();
        return true;
    }
};

} // namespace nlm
