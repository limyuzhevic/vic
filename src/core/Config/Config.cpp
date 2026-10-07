#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <nlohmann/json.hpp>

namespace nlm {

using json = nlohmann::json;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigurationTemplate> templates;
    std::map<std::string, std::vector<ValidationConstraint>> validationConstraints;
    std::map<std::string, std::chrono::system_clock::time_point> modificationTimes;
    size_t nextTemplateId;
    std::string currentSessionId;
    bool enableJsonParsing;
    bool enableYamlParsing;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->nextTemplateId = 0;
    pImpl->currentSessionId = generateID();
    pImpl->enableJsonParsing = true;
    pImpl->enableYamlParsing = false;
    
    // Load default templates
    initializeDefaultTemplates();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultTemplates() {
    // Neural network configuration template
    ConfigurationTemplate neuralNetTemplate("neural_network", "Standard neural network configuration for basic simulations");
    neuralNetTemplate.defaultValues = {
        {"neuron_count", 1000},
        {"region_count", 1},
        {"connection_probability", 0.1},
        {"plasticity_rate", 0.05},
        {"simulation_timestep", 0.001},
        {"replay_interval", 100},
        {"consolidation_interval", 1000}
    };
    neuralNetTemplate.descriptions = {
        {"neuron_count", "Number of neurons in the network"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Probability of connection between neurons"},
        {"plasticity_rate", "Rate of synaptic plasticity"},
        {"simulation_timestep", "Duration of each simulation step"},
        {"replay_interval", "Interval for memory replay in simulation steps"},
        {"consolidation_interval", "Interval for memory consolidation in simulation steps"}
    };
    pImpl->templates["neural_network"] = neuralNetTemplate;
    
    // Development configuration template
    ConfigurationTemplate developmentTemplate("development", "Configuration for developmental stages and plasticity");
    developmentTemplate.defaultValues = {
        {"enable_plasticity", true},
        {"plasticity_decay_rate", 0.001},
        {"critical_period_length", 3000},
        {"structural_plasticity_threshold", 0.7},
        {"developmental_stage", "Initial"}
    };
    developmentTemplate.descriptions = {
        {"enable_plasticity", "Enable synaptic plasticity"},
        {"plasticity_decay_rate", "Rate at which plasticity decreases over time"},
        {"critical_period_length", "Duration of critical period"},
        {"structural_plasticity_threshold", "Threshold for structural plasticity"},
        {"developmental_stage", "Current developmental stage"}
    };
    pImpl->templates["development"] = developmentTemplate;
    
    // Memory configuration template
    ConfigurationTemplate memoryTemplate("memory_systems", "Configuration for memory systems");
    memoryTemplate.defaultValues = {
        {"enable_working_memory", true},
        {"enable_episodic_memory", true},
        {"enable_associative_memory", true},
        {"working_memory_capacity", 128},
        {"episodic_capacity", 1024},
        {"memory_replay_frequency", 0.1},
        {"memory_consolidation_threshold", 0.8}
    };
    memoryTemplate.descriptions = {
        {"enable_working_memory", "Enable working memory system"},
        {"enable_episodic_memory", "Enable episodic memory system"},
        {"enable_associative_memory", "Enable associative memory system"},
        {"working_memory_capacity", "Capacity of working memory"},
        {"episodic_capacity", "Capacity of episodic memory"},
        {"memory_replay_frequency", "Frequency of memory replay"},
        {"memory_consolidation_threshold", "Threshold for memory consolidation"}
    };
    pImpl->templates["memory_systems"] = memoryTemplate;
    
    // Neuromodulation configuration template
    ConfigurationTemplate neuromodTemplate("neuromodulation", "Configuration for neuromodulation systems");
    neuromodTemplate.defaultValues = {
        {"dopamine_baseline", 0.1},
        {"dopamine_sensitivity", 0.8},
        {"curiosity_factor", 0.3},
        {"novelty_threshold", 0.2},
        {"reward_sensitivity", 1.0}
    };
    neuromodTemplate.descriptions = {
        {"dopamine_baseline", "Baseline dopamine level"},
        {"dopamine_sensitivity", "Sensitivity to dopamine signals"},
        {"curiosity_factor", "Factor for curiosity-driven exploration"},
        {"novelty_threshold", "Threshold for novelty detection"},
        {"reward_sensitivity", "Sensitivity to reward signals"}
    };
    pImpl->templates["neuromodulation"] = neuromodTemplate;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first, then fallback to simple key=value format
    if (pImpl->enableJsonParsing) {
        // Try JSON format
        std::ifstream file(filepath);
        if (file.is_open()) {
            try {
                json j;
                file >> j;
                
                // Clear existing entries
                pImpl->entries.clear();
                
                // Parse JSON
                for (auto it = j.begin(); it != j.end(); ++it) {
                    std::string key = it.key();
                    std::string valueStr = it.value().dump();
                    
                    // Remove surrounding quotes from string values
                    if (valueStr.size() >= 2 && valueStr.front() == '"' && valueStr.back() == '"') {
                        valueStr = valueStr.substr(1, valueStr.size() - 2);
                    }
                    
                    // Try to parse as different types
                    ConfigValue value;
                    
                    // Check if it looks like a boolean
                    if (it.value().is_boolean()) {
                        value = it.value().get<bool>();
                    }
                    // Check if it looks like a number
                    else if (it.value().is_number()) {
                        if (it.value().is_number_integer()) {
                            value = it.value().get<int64_t>();
                        } else {
                            value = it.value().get<double>();
                        }
                    }
                    // Check if it looks like an array
                    else if (it.value().is_array()) {
                        std::vector<double> vec = it.value().get<std::vector<double>>();
                        value = vec;
                    }
                    // Otherwise treat as string
                    else {
                        value = valueStr;
                    }
                    
                    set(key, value, ConfigSource::File);
                }
                
                file.close();
                return true;
            } catch (const json::parse_error& e) {
                // JSON parsing failed, fall back to simple format
                file.close();
            }
        }
    }
    
    // Fallback to simple key=value format
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
        
        // Parse key=value pairs
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
    // Save in JSON format for better readability and tool support
    json j;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&j, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j[entry.key] = arg;
            }
        }, entry.value);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << std::setw(2) << j << std::endl;
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
    
    // Update modification time
    pImpl->modificationTimes[key] = std::chrono::system_clock::now();
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

void Config::modifyRuntime(const std::string& key, const ConfigValue& value) {
    set(key, value, ConfigSource::Runtime);
}

bool Config::overrideConfig(const std::string& key, const ConfigValue& value) {
    if (has(key)) {
        set(key, value, ConfigSource::Runtime);
        return true;
    }
    return false;
}

bool Config::addTemplate(const ConfigurationTemplate& template) {
    if (pImpl->templates.find(template.name) != pImpl->templates.end()) {
        return false; // Template already exists
    }
    
    ConfigurationTemplate newTemplate = template;
    newTemplate.name = "template_" + std::to_string(pImpl->nextTemplateId++) + "_" + template.name;
    pImpl->templates[newTemplate.name] = newTemplate;
    return true;
}

bool Config::removeTemplate(const std::string& name) {
    auto it = pImpl->templates.find(name);
    if (it == pImpl->templates.end()) {
        return false;
    }
    
    pImpl->templates.erase(it);
    return true;
}

std::vector<std::string> Config::getTemplateNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->templates.size());
    for (const auto& pair : pImpl->templates) {
        names.push_back(pair.first);
    }
    return names;
}

bool Config::applyTemplate(const std::string& templateName) {
    auto it = pImpl->templates.find(templateName);
    if (it == pImpl->templates.end()) {
        return false;
    }
    
    const ConfigurationTemplate& template = it->second;
    
    // Apply template defaults
    for (const auto& pair : template.defaultValues) {
        const std::string& key = pair.first;
        const ConfigValue& value = pair.second;
        
        // Only set if key doesn't already exist or has Default source
        if (!has(key) || getEntry(key)->source == ConfigSource::Default) {
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

ConfigurationTemplate* Config::getTemplate(const std::string& name) {
    auto it = pImpl->templates.find(name);
    if (it == pImpl->templates.end()) {
        return nullptr;
    }
    return &it->second;
}

const ConfigurationTemplate* Config::getTemplate(const std::string& name) const {
    auto it = pImpl->templates.find(name);
    if (it == pImpl->templates.end()) {
        return nullptr;
    }
    return &it->second;
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult keyResult = validateKey(entry.key);
        if (!keyResult.valid) {
            result.valid = false;
            result.errorMessage += "Key '" + entry.key + "': " + keyResult.errorMessage + "\n";
        }
        
        // Add warnings
        for (const std::string& warning : keyResult.warnings) {
            result.warnings.push_back("Key '" + entry.key + "': " + warning);
        }
    }
    
    return result;
}

ValidationResult Config::validateKey(const std::string& key) const {
    ValidationResult result;
    
    // Check if key has validation constraints
    auto constraintIt = pImpl->validationConstraints.find(key);
    if (constraintIt != pImpl->validationConstraints.end()) {
        for (const auto& constraint : constraintIt->second) {
            ValidationResult constraintResult = constraint(key, get<ConfigValue>(key).value_or(ConfigValue()));
            if (!constraintResult.valid) {
                result.valid = false;
                result.errorMessage += constraintResult.errorMessage + "; ";
            }
            
            for (const std::string& warning : constraintResult.warnings) {
                result.warnings.push_back(warning);
            }
        }
    }
    
    // Additional validation based on key name
    if (key == "neuron_count") {
        auto valueOpt = get<size_t>(key);
        if (valueOpt.has_value()) {
            if (*valueOpt == 0) {
                result.valid = false;
                result.errorMessage += "Neuron count cannot be zero; ";
            } else if (*valueOpt > 100000) {
                result.warnings.push_back("Very high neuron count may cause performance issues");
            }
        }
    } else if (key == "simulation_timestep") {
        auto valueOpt = get<double>(key);
        if (valueOpt.has_value()) {
            if (*valueOpt <= 0.0) {
                result.valid = false;
                result.errorMessage += "Simulation timestep must be positive; ";
            } else if (*valueOpt > 0.1) {
                result.warnings.push_back("Large simulation timestep may reduce accuracy");
            }
        }
    } else if (key == "connection_probability") {
        auto valueOpt = get<double>(key);
        if (valueOpt.has_value()) {
            if (*valueOpt < 0.0 || *valueOpt > 1.0) {
                result.valid = false;
                result.errorMessage += "Connection probability must be between 0 and 1; ";
            }
        }
    }
    
    return result;
}

void Config::addValidationConstraint(const std::string& key, const ValidationConstraint& constraint) {
    pImpl->validationConstraints[key].push_back(constraint);
}

void Config::clearValidationConstraints(const std::string& key) {
    pImpl->validationConstraints.erase(key);
}

bool Config::exportConfig(const std::string& filepath, bool includeRuntime) const {
    // Export to JSON format
    json j;
    
    for (const auto& entry : pImpl->entries) {
        // Skip runtime entries if not requested
        if (!includeRuntime && entry.source == ConfigSource::Runtime) {
            continue;
        }
        
        std::visit([&j, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j[entry.key] = arg;
            }
        }, entry.value);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << std::setw(2) << j << std::endl;
    return true;
}

bool Config::importConfig(const std::string& filepath) {
    return loadFromFile(filepath);
}

bool Config::importConfigFromJSON(const std::string& jsonData) {
    try {
        json j = json::parse(jsonData);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse JSON
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            std::string valueStr = it.value().dump();
            
            // Remove surrounding quotes from string values
            if (valueStr.size() >= 2 && valueStr.front() == '"' && valueStr.back() == '"') {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            // Try to parse as different types
            ConfigValue value;
            
            // Check if it looks like a boolean
            if (it.value().is_boolean()) {
                value = it.value().get<bool>();
            }
            // Check if it looks like a number
            else if (it.value().is_number()) {
                if (it.value().is_number_integer()) {
                    value = it.value().get<int64_t>();
                } else {
                    value = it.value().get<double>();
                }
            }
            // Check if it looks like an array
            else if (it.value().is_array()) {
                std::vector<double> vec = it.value().get<std::vector<double>>();
                value = vec;
            }
            // Otherwise treat as string
            else {
                value = valueStr;
            }
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const json::parse_error& e) {
        return false;
    }
}

std::string Config::exportConfigToJSON(bool includeRuntime) const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        // Skip runtime entries if not requested
        if (!includeRuntime && entry.source == ConfigSource::Runtime) {
            continue;
        }
        
        std::visit([&j, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j[entry.key] = arg;
            }
        }, entry.value);
    }
    
    return j.dump(2);
}

std::string Config::generateDocumentation() const {
    std::ostringstream oss;
    oss << "NLM Configuration Documentation\n";
    oss << "==============================\n\n";
    oss << "Generated on: " << getCurrentTimestamp() << "\n\n";
    
    oss << "Configuration Entries:\n";
    oss << "-------------------\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << entry.key << "\n";
        oss << "  Type: " << getConfigValueType(entry.value) << "\n";
        oss << "  Value: ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            }
        }, entry.value);
        
        oss << "\n  Source: " << static_cast<int>(entry.source) << "\n";
        oss << "  Description: " << entry.description << "\n\n";
    }
    
    oss << "\nConfiguration Templates:\n";
    oss << "------------------------\n";
    
    for (const auto& pair : pImpl->templates) {
        const ConfigurationTemplate& template = pair.second;
        oss << "Name: " << template.name << "\n";
        oss << "  Description: " << template.description << "\n";
        oss << "  Enabled: " << (template.enabled ? "Yes" : "No") << "\n";
        oss << "  Default Values:\n";
        for (const auto& pair : template.defaultValues) {
            oss << "    " << pair.first << ": ";
            oss << getConfigValueAsString(pair.second) << "\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string Config::generateTemplateDocumentation(const std::string& templateName) const {
    auto it = pImpl->templates.find(templateName);
    if (it == pImpl->templates.end()) {
        return "Template not found: " + templateName;
    }
    
    const ConfigurationTemplate& template = it->second;
    std::ostringstream oss;
    
    oss << "Template Documentation: " << template.name << "\n";
    oss << "======================\n\n";
    oss << "Description: " << template.description << "\n\n";
    oss << "Default Values:\n";
    
    for (const auto& pair : template.defaultValues) {
        oss << "  " << pair.first << ": " << getConfigValueAsString(pair.second) << "\n";
        
        // Find description
        auto descIt = template.descriptions.find(pair.first);
        if (descIt != template.descriptions.end()) {
            oss << "    " << descIt->second << "\n";
        }
    }
    
    oss << "\nValidation Constraints:\n";
    if (template.constraints.empty()) {
        oss << "  No constraints defined\n";
    } else {
        for (const auto& pair : template.constraints) {
            oss << "  " << pair.first << ": Custom validation\n";
        }
    }
    
    return oss.str();
}

bool Config::hasChangesSince(const std::chrono::system_clock::time_point& since) const {
    for (const auto& pair : pImpl->modificationTimes) {
        if (pair.second > since) {
            return true;
        }
    }
    return false;
}

std::string Config::getChangeSummary(const std::chrono::system_clock::time_point& since) const {
    std::ostringstream oss;
    oss << "Configuration changes since " << getTimestampString(since) << ":\n";
    
    int changes = 0;
    for (const auto& pair : pImpl->modificationTimes) {
        if (pair.second > since) {
            changes++;
            oss << "  - " << pair.first << " modified at " << getTimestampString(pair.second) << "\n";
        }
    }
    
    if (changes == 0) {
        oss << "  No changes\n";
    } else {
        oss << "  Total changes: " << changes << "\n";
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
    
    // Remove modification time
    pImpl->modificationTimes.erase(key);
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
    pImpl->modificationTimes.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " << pImpl->templates.size() << " templates):\n";
    
    // Count entries by source
    int fileCount = 0, commandLineCount = 0, runtimeCount = 0, defaultCount = 0;
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::File: fileCount++; break;
            case ConfigSource::CommandLine: commandLineCount++; break;
            case ConfigSource::Runtime: runtimeCount++; break;
            case ConfigSource::Default: defaultCount++; break;
        }
    }
    
    oss << "  From file: " << fileCount << "\n";
    oss << "  From command line: " << commandLineCount << "\n";
    oss << "  From runtime: " << runtimeCount << "\n";
    oss << "  Default: " << defaultCount << "\n";
    
    return oss.str();
}

std::chrono::system_clock::time_point Config::getModificationTime(const std::string& key) const {
    auto it = pImpl->modificationTimes.find(key);
    if (it == pImpl->modificationTimes.end()) {
        return std::chrono::system_clock::time_point();
    }
    return it->second;
}

ConfigEntry* Config::getEntry(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return nullptr;
    }
    return &(*it);
}

const ConfigEntry* Config::getEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return nullptr;
    }
    return &(*it);
}

const std::vector<ConfigEntry>& Config::getAllEntries() const {
    return pImpl->entries;
}

// Static helper methods
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

std::string Config::escapeJSON(const std::string& str) {
    json j = str;
    return j.dump();
}

std::string Config::generateID() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto epoch = now_ms.time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    return std::to_string(value.count());
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Config::getTimestampString(const std::chrono::system_clock::time_point& time) {
    auto in_time_t = std::chrono::system_clock::to_time_t(time);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Config::getConfigValueType(const ConfigValue& value) {
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "string";
        } else if constexpr (std::is_same_v<T, bool>) {
            return "boolean";
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return "integer";
        } else if constexpr (std::is_same_v<T, double>) {
            return "float";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return "integer array";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return "float array";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return "string array";
        }
        return "unknown";
    }, value);
    return "unknown";
}

std::string Config::getConfigValueAsString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        }
    }, value);
    return oss.str();
}

} // namespace nlm

// Explicit template instantiations
namespace nlm {
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

template ConfigValue::ConfigValue(const int&);
template ConfigValue::ConfigValue(const int64_t&);
template ConfigValue::ConfigValue(const double&);
template ConfigValue::ConfigValue(const bool&);
template ConfigValue::ConfigValue(const std::string&);
template ConfigValue::ConfigValue(const std::vector<int>&);
template ConfigValue::ConfigValue(const std::vector<double>&);
template ConfigValue::ConfigValue(const std::vector<std::string>&);
} // namespace nlm

// Include JSON library
#include <nlohmann/json.hpp>
