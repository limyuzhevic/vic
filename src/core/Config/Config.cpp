#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

// Private implementation struct
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ValidationRule> validationRules;
    std::vector<std::function<bool(const ConfigValue&, const ConfigValue&)>> conditionalValidators;
    std::map<std::string, std::pair<std::string, std::function<bool(const ConfigValue&)>>> conditionalDependencies;
    std::vector<ValidationError> validationErrors;
    std::string version;
    std::vector<ConfigVersion> versionHistory;
    bool monitorEnabled = false;
    std::map<std::string, std::chrono::system_clock::time_point> lastModifiedTimes;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->version = "1.0.0";
    auto now = std::chrono::system_clock::now();
    pImpl->lastModifiedTimes["version"] = now;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Auto-detect file format
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromYAML(filepath);
    } else {
        // Default to simple key=value format
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
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

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            if (it->is_number_integer()) {
                set(key, it->get<int>(), ConfigSource::File);
            } else if (it->is_number_unsigned()) {
                set(key, static_cast<int64_t>(it->get<uint64_t>()), ConfigSource::File);
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), ConfigSource::File);
            } else if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::File);
            } else if (it->is_string()) {
                set(key, it->get<std::string>(), ConfigSource::File);
            } else if (it->is_array()) {
                // Check array type
                if (!it->empty()) {
                    if (std::all_of(it->begin(), it->end(), [](const auto& item) { return item.is_number_integer(); })) {
                        std::vector<int> vec;
                        for (const auto& item : *it) {
                            vec.push_back(item.get<int>());
                        }
                        set(key, vec, ConfigSource::File);
                    } else if (std::all_of(it->begin(), it->end(), [](const auto& item) { return item.is_number_float(); })) {
                        std::vector<double> vec;
                        for (const auto& item : *it) {
                            vec.push_back(item.get<double>());
                        }
                        set(key, vec, ConfigSource::File);
                    } else {
                        std::vector<std::string> vec;
                        for (const auto& item : *it) {
                            vec.push_back(item.get<std::string>());
                        }
                        set(key, vec, ConfigSource::File);
                    }
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        for (const auto& node : doc) {
            std::string key = node.first.as<std::string>();
            
            if (node.second.IsScalar()) {
                if (node.second.IsInt()) {
                    set(key, node.second.as<int>(), ConfigSource::File);
                } else if (node.second.IsDouble()) {
                    set(key, node.second.as<double>(), ConfigSource::File);
                } else if (node.second.IsBool()) {
                    set(key, node.second.as<bool>(), ConfigSource::File);
                } else {
                    set(key, node.second.as<std::string>(), ConfigSource::File);
                }
            } else if (node.second.IsSequence()) {
                // Check sequence type
                if (!node.second.empty()) {
                    if (std::all_of(node.second.begin(), node.second.end(), [](const auto& item) { return item.IsInt(); })) {
                        std::vector<int> vec;
                        for (const auto& item : node.second) {
                            vec.push_back(item.as<int>());
                        }
                        set(key, vec, ConfigSource::File);
                    } else if (std::all_of(node.second.begin(), node.second.end(), [](const auto& item) { return item.IsDouble(); })) {
                        std::vector<double> vec;
                        for (const auto& item : node.second) {
                            vec.push_back(item.as<double>());
                        }
                        set(key, vec, ConfigSource::File);
                    } else {
                        std::vector<std::string> vec;
                        for (const auto& item : node.second) {
                            vec.push_back(item.as<std::string>());
                        }
                        set(key, vec, ConfigSource::File);
                    }
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromBinary(const std::string& filepath) {
    // Binary format implementation would go here
    // For now, return false to indicate not implemented
    return false;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Auto-detect file format
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return saveToJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return saveToYAML(filepath);
    } else {
        // Default to simple format
        return saveToSimpleFile(filepath);
    }
}

bool Config::saveToSimpleFile(const std::string& filepath) const {
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

bool Config::saveToJSON(const std::string& filepath) const {
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            std::visit([&jsonData, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonData[entry.key] = arg;
                }
            }, entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(4) << jsonData << std::endl;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            std::visit([&emitter](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : arg) {
                        emitter << item;
                    }
                    emitter << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : arg) {
                        emitter << item;
                    }
                    emitter << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : arg) {
                        emitter << item;
                    }
                    emitter << YAML::EndSeq;
                }
            }, entry.value);
        }
        
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << emitter.c_str();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToBinary(const std::string& filepath) const {
    // Binary format implementation would go here
    // For now, return false to indicate not implemented
    return false;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        auto now = std::chrono::system_clock::now();
        pImpl->lastModifiedTimes[key] = now;
    } else {
        ConfigEntry newEntry;
        newEntry.key = key;
        newEntry.value = value;
        newEntry.source = source;
        newEntry.created = std::chrono::system_clock::now();
        newEntry.lastModified = newEntry.created;
        pImpl->entries.emplace_back(newEntry);
        pImpl->lastModifiedTimes[key] = newEntry.created;
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
    pImpl->lastModifiedTimes.erase(key);
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
    pImpl->lastModifiedTimes.clear();
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

// Validation implementation
bool Config::validate(bool treatWarningsAsErrors) {
    pImpl->validationErrors.clear();
    
    // Validate each entry
    for (const auto& entry : pImpl->entries) {
        validateEntry(entry, pImpl->validationErrors);
    }
    
    // Validate conditional dependencies
    validateConditionalDependencies(pImpl->validationErrors);
    
    // Filter by severity
    bool hasErrors = false;
    bool hasWarnings = false;
    
    for (const auto& error : pImpl->validationErrors) {
        if (error.result == ValidationResult::Error) {
            hasErrors = true;
        } else if (error.result == ValidationResult::Warning) {
            hasWarnings = true;
        }
    }
    
    if (hasWarnings && treatWarningsAsErrors) {
        hasErrors = true;
    }
    
    return !hasErrors;
}

std::vector<ValidationError> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

void Config::addValidationRule(const ValidationRule& rule) {
    pImpl->validationRules.push_back(rule);
}

void Config::clearValidationRules() {
    pImpl->validationRules.clear();
}

void Config::addConditionalValidation(const std::string& conditionKey, const std::string& dependentKey,
                                      std::function<bool(const ConfigValue&, const ConfigValue&)> validator) {
    pImpl->conditionalValidators.push_back([conditionKey, dependentKey, validator](const ConfigValue& condVal, const ConfigValue& depVal) {
        return validator(condVal, depVal);
    });
}

// Validation helper implementations
bool Config::validateRange(const ConfigValue& value, double min, double max) {
    return std::visit([min, max](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return arg >= min && arg <= max;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg >= min && arg <= max;
        } else {
            return false;
        }
    }, value);
}

bool Config::validatePattern(const std::string& value, const std::string& pattern) {
    try {
        std::regex regex(pattern);
        return std::regex_match(value, regex);
    } catch (const std::regex_error&) {
        return false;
    }
}

void Config::validateEntry(const ConfigEntry& entry, std::vector<ValidationError>& errors) const {
    // Check validation rules
    for (const auto& rule : pImpl->validationRules) {
        if (rule.key == entry.key) {
            if (!rule.validator(entry.value)) {
                ValidationError error;
                error.key = entry.key;
                error.message = rule.errorMessage;
                error.result = ValidationResult::Error;
                error.code = rule.code;
                errors.push_back(error);
            }
        }
    }
}

void Config::validateConditionalDependencies(std::vector<ValidationError>& errors) const {
    for (const auto& validator : pImpl->conditionalValidators) {
        // Implementation would need to track which entries are condition-dependent
        // For now, just a placeholder
    }
}

// Default configuration templates
ConfigTemplate Config::getPhase2Template() {
    ConfigTemplate template;
    template.name = "Phase 2 Template";
    template.description = "Base configuration for Phase 2 neural network setup";
    template.phase = "2";
    
    // Common Phase 2 parameters
    template.defaults["simulation.timeStep"] = 0.01;
    template.defaults["simulation.maxSteps"] = 1000;
    template.defaults["network.inputSize"] = 784;
    template.defaults["network.hiddenSize"] = 128;
    template.defaults["network.outputSize"] = 10;
    template.defaults["network.learningRate"] = 0.01;
    template.defaults["network.momentum"] = 0.9;
    
    // Validation rules
    ValidationRule rule;
    rule.key = "simulation.learningRate";
    rule.validator = [](const ConfigValue& v) {
        return std::visit([](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, double>) {
                return arg >= 0.0 && arg <= 1.0;
            } else {
                return false;
            }
        }, v);
    };
    rule.errorMessage = "Learning rate must be between 0.0 and 1.0";
    rule.code = "LR_RANGE";
    rule.required = true;
    template.validations.push_back(rule);
    
    return template;
}

ConfigTemplate Config::getPhase4Template() {
    ConfigTemplate template;
    template.name = "Phase 4 Template";
    template.description = "Configuration for Phase 4 reward-based learning";
    template.phase = "4";
    
    // Common Phase 4 parameters
    template.defaults["reward.discountFactor"] = 0.99;
    template.defaults["reward.explorationRate"] = 0.1;
    template.defaults["reward.optimizationAlgorithm"] = std::string("policy_gradient");
    template.defaults["reward.baselineEnabled"] = true;
    template.defaults["reward.entropyRegularization"] = 0.01;
    template.defaults["reward.clipRange"] = 0.2;
    
    // Validation rules
    ValidationRule rule;
    rule.key = "reward.discountFactor";
    rule.validator = [](const ConfigValue& v) {
        return std::visit([](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, double>) {
                return arg >= 0.0 && arg <= 1.0;
            } else {
                return false;
            }
        }, v);
    };
    rule.errorMessage = "Discount factor must be between 0.0 and 1.0";
    rule.code = "DF_RANGE";
    rule.required = true;
    template.validations.push_back(rule);
    
    return template;
}

ConfigTemplate Config::getPhase6Template() {
    ConfigTemplate template;
    template.name = "Phase 6 Template";
    template.description = "Configuration for Phase 6 hierarchical reinforcement learning";
    template.phase = "6";
    
    // Common Phase 6 parameters
    template.defaults["hierarchical.policyGatingEnabled"] = true;
    template.defaults["hierarchical.numMetaPolicies"] = 3;
    template.defaults["hierarchical.adaptationRate"] = 0.05;
    template.defaults["hierarchical.expertiseBalancing"] = std::string("e_greedy");
    template.defaults["hierarchical.decompositionDepth"] = 3;
    template.defaults["hierarchical.optionSelection"] = std::string("epsilon_greedy");
    
    // Validation rules
    ValidationRule rule;
    rule.key = "hierarchical.adaptationRate";
    rule.validator = [](const ConfigValue& v) {
        return std::visit([](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, double>) {
                return arg >= 0.0 && arg <= 1.0;
            } else {
                return false;
            }
        }, v);
    };
    rule.errorMessage = "Adaptation rate must be between 0.0 and 1.0";
    rule.code = "AR_RANGE";
    rule.required = true;
    template.validations.push_back(rule);
    
    return template;
}

void Config::applyTemplate(const ConfigTemplate& template) {
    clear();
    
    for (const auto& pair : template.defaults) {
        set(pair.first, pair.second, ConfigSource::Default);
    }
    
    for (const auto& rule : template.validations) {
        addValidationRule(rule);
    }
}

void Config::applyInheritance(const Config& parentConfig) {
    // Apply parent's configuration to this config
    for (const auto& parentEntry : parentConfig.pImpl->entries) {
        if (!has(parentEntry.key)) {
            set(parentEntry.key, parentEntry.value, parentEntry.source);
        }
    }
}

bool Config::resolveSubstitutions() {
    // Resolve macro substitutions in values
    // This is a simplified implementation
    bool changed = false;
    
    for (auto& entry : pImpl->entries) {
        std::string key = entry.key;
        std::string& valueStr = std::get<std::string>(entry.value);
        
        // Look for ${...} patterns
        size_t pos = 0;
        while ((pos = valueStr.find('$', pos)) != std::string::npos) {
            if (pos + 1 < valueStr.size() && valueStr[pos + 1] == '{') {
                size_t endPos = valueStr.find('}', pos + 2);
                if (endPos != std::string::npos) {
                    std::string macro = valueStr.substr(pos + 2, endPos - pos - 2);
                    auto it = pImpl->lastModifiedTimes.find(macro);
                    if (it != pImpl->lastModifiedTimes.end()) {
                        // Find macro value
                        for (const auto& e : pImpl->entries) {
                            if (e.key == macro) {
                                std::string macroValue;
                                if (std::holds_alternative<std::string>(e.value)) {
                                    macroValue = std::get<std::string>(e.value);
                                } else {
                                    std::ostringstream oss;
                                    oss << std::get<double>(e.value);
                                    macroValue = oss.str();
                                }
                                
                                valueStr.replace(pos, endPos - pos + 1, macroValue);
                                changed = true;
                                break;
                            }
                        }
                    }
                }
            }
            pos++;
        }
    }
    
    return changed;
}

void Config::merge(const Config& other, MergeStrategy strategy) {
    switch (strategy) {
        case MergeStrategy::Override:
            for (const auto& entry : other.pImpl->entries) {
                set(entry.key, entry.value, entry.source);
            }
            break;
        case MergeStrategy::Merge:
            for (const auto& entry : other.pImpl->entries) {
                if (!has(entry.key)) {
                    set(entry.key, entry.value, entry.source);
                }
            }
            break;
        case MergeStrategy::KeepExisting:
            // Keep existing values, ignore new ones
            break;
        case MergeStrategy::DeepMerge:
            for (const auto& entry : other.pImpl->entries) {
                if (has(entry.key)) {
                    // For simplicity, always override in deep merge
                    set(entry.key, entry.value, entry.source);
                } else {
                    set(entry.key, entry.value, entry.source);
                }
            }
            break;
    }
}

void Config::incrementVersion(const std::string& description) {
    std::istringstream iss(pImpl->version);
    int major, minor, patch;
    char dot;
    
    if (iss >> major >> dot >> minor >> dot >> patch) {
        patch++;
    } else {
        major = 1;
        minor = 0;
        patch = 1;
    }
    
    std::ostringstream oss;
    oss << major << "." << minor << "." << patch;
    pImpl->version = oss.str();
    
    ConfigVersion version;
    version.version = pImpl->version;
    version.description = description;
    version.timestamp = std::chrono::system_clock::now();
    version.changes.clear(); // Could track actual changes
    
    pImpl->versionHistory.push_back(version);
    pImpl->lastModifiedTimes["version"] = version.timestamp;
}

std::string Config::getVersion() const {
    return pImpl->version;
}

std::vector<ConfigVersion> Config::getVersionHistory() const {
    return pImpl->versionHistory;
}

bool Config::hasDiff(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return true;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it == other.pImpl->entries.end()) {
            return true;
        }
        
        if (it->value != entry.value) {
            return true;
        }
    }
    
    return false;
}

std::vector<ConfigDiff> Config::getDiff(const Config& other) const {
    std::vector<ConfigDiff> diff;
    
    // Check for additions and updates in this config
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it == other.pImpl->entries.end()) {
            ConfigDiff d;
            d.key = entry.key;
            d.oldValue = {}; // Placeholder for null
            d.newValue = entry.value;
            d.source = entry.source;
            d.operation = "add";
            diff.push_back(d);
        } else if (it->value != entry.value) {
            ConfigDiff d;
            d.key = entry.key;
            d.oldValue = it->value;
            d.newValue = entry.value;
            d.source = entry.source;
            d.operation = "update";
            diff.push_back(d);
        }
    }
    
    // Check for removals
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it == pImpl->entries.end()) {
            ConfigDiff d;
            d.key = entry.key;
            d.oldValue = entry.value;
            d.newValue = {}; // Placeholder for null
            d.source = entry.source;
            d.operation = "remove";
            diff.push_back(d);
        }
    }
    
    return diff;
}

bool Config::applyPatch(const std::vector<ConfigDiff>& diff) {
    bool changed = false;
    
    for (const auto& d : diff) {
        if (d.operation == "add" || d.operation == "update") {
            set(d.key, d.newValue, d.source);
            changed = true;
        } else if (d.operation == "remove") {
            remove(d.key);
            changed = true;
        }
    }
    
    return changed;
}

bool Config::exportDiff(const std::string& filepath) const {
    // This would create a diff file
    // For now, just return false
    return false;
}

bool Config::importDiff(const std::string& filepath) {
    // This would import a diff file
    // For now, just return false
    return false;
}

bool Config::substituteMacros(const std::map<std::string, std::string>& macros) {
    bool changed = false;
    
    for (auto& entry : pImpl->entries) {
        if (std::holds_alternative<std::string>(entry.value)) {
            std::string& valueStr = std::get<std::string>(entry.value);
            
            // Substitute macros
            for (const auto& pair : macros) {
                std::string placeholder = "${" + pair.first + "}";
                size_t pos = 0;
                while ((pos = valueStr.find(placeholder, pos)) != std::string::npos) {
                    valueStr.replace(pos, placeholder.length(), pair.second);
                    changed = true;
                    pos += pair.second.length();
                }
            }
        }
    }
    
    if (changed) {
        auto now = std::chrono::system_clock::now();
        for (auto& entry : pImpl->entries) {
            if (std::holds_alternative<std::string>(entry.value)) {
                pImpl->lastModifiedTimes[entry.key] = now;
            }
        }
    }
    
    return changed;
}

bool Config::exportConfig(const std::string& filepath, const std::string& format) const {
    std::string ext = format;
    if (ext == "auto") {
        ext = filepath.substr(filepath.find_last_of('.') + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    } else {
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }
    
    if (ext == "json") {
        return saveToJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return saveToYAML(filepath);
    } else if (ext == "binary") {
        return saveToBinary(filepath);
    } else {
        // Default to simple format
        return saveToSimpleFile(filepath);
    }
}

bool Config::importConfig(const std::string& filepath, const std::string& format) {
    std::string ext = format;
    if (ext == "auto") {
        ext = filepath.substr(filepath.find_last_of('.') + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    } else {
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }
    
    if (ext == "json") {
        return loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromYAML(filepath);
    } else if (ext == "binary") {
        return loadFromBinary(filepath);
    } else {
        // Default to simple format
        return loadFromSimpleFile(filepath);
    }
}

void Config::setMonitorEnabled(bool enabled) {
    pImpl->monitorEnabled = enabled;
}

bool Config::isMonitorEnabled() const {
    return pImpl->monitorEnabled;
}

std::string Config::getValidationReport() const {
    std::ostringstream oss;
    oss << "=== Configuration Validation Report ===\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    
    if (pImpl->validationErrors.empty()) {
        oss << "All validations passed.\n";
    } else {
        oss << "Validation errors/warnings:\n";
        for (const auto& error : pImpl->validationErrors) {
            std::string severity = (error.result == ValidationResult::Error) ? "ERROR" :
                                  (error.result == ValidationResult::Warning) ? "WARNING" : "INFO";
            oss << "  [" << severity << "] Key: " << error.key << " - " << error.message << "\n";
        }
    }
    
    oss << "\n=== Current Configuration ===\n";
    oss << summary();
    
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
