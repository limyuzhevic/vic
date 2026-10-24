#include "ConfigFormat.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {
namespace config {

// JSON Parser implementation
bool JSONParser::canParse(const std::string& content) const {
    if (content.empty()) return false;
    
    // Trim whitespace
    std::string trimmed = trim(content);
    if (trimmed.empty()) return false;
    
    // Check if it's a valid JSON object
    try {
        auto json = nlohmann::json::parse(trimmed);
        return json.is_object();
    } catch (const nlohmann::json::parse_error&) {
        return false;
    }
}

std::vector<ConfigEntry> JSONParser::parse(const std::string& content) {
    std::vector<ConfigEntry> entries;
    
    try {
        auto json = nlohmann::json::parse(content);
        
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            // Convert JSON value to ConfigValue
            if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_unsigned()) {
                value = static_cast<int64_t>(it->get<size_t>());
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_array()) {
                // Convert array to appropriate vector type
                if (!it->empty()) {
                    if (std::all_of(it->begin(), it->end(), [](const nlohmann::json& val) { return val.is_number_integer(); })) {
                        std::vector<int> intArray;
                        for (const auto& val : *it) {
                            intArray.push_back(val.get<int64_t>());
                        }
                        value = intArray;
                    } else if (std::all_of(it->begin(), it->end(), [](const nlohmann::json& val) { return val.is_number_float(); })) {
                        std::vector<double> doubleArray;
                        for (const auto& val : *it) {
                            doubleArray.push_back(val.get<double>());
                        }
                        value = doubleArray;
                    } else if (std::all_of(it->begin(), it->end(), [](const nlohmann::json& val) { return val.is_string(); })) {
                        std::vector<std::string> stringArray;
                        for (const auto& val : *it) {
                            stringArray.push_back(val.get<std::string>());
                        }
                        value = stringArray;
                    } else {
                        // Mixed array - convert to generic string representation
                        value = it->dump();
                    }
                } else {
                    value = std::vector<int>(); // Empty array
                }
            } else {
                // Other types - convert to string representation
                value = it->dump();
            }
            
            entries.emplace_back(key, value, ConfigSource::JSON, "");
        }
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("JSON parsing failed: ") + e.what());
    }
    
    return entries;
}

// YAML Parser implementation
bool YAMLParser::canParse(const std::string& content) const {
    if (content.empty()) return false;
    
    // Trim whitespace
    std::string trimmed = trim(content);
    if (trimmed.empty()) return false;
    
    // Check if it's a valid YAML document
    try {
        auto yaml = YAML::Load(trimmed);
        return yaml.IsMap();
    } catch (const YAML::Exception&) {
        return false;
    }
}

std::vector<ConfigEntry> YAMLParser::parse(const std::string& content) {
    std::vector<ConfigEntry> entries;
    
    try {
        auto yaml = YAML::Load(content);
        
        for (const auto& pair : yaml) {
            std::string key = pair.first.Scalar();
            ConfigValue value;
            
            // Convert YAML value to ConfigValue
            if (pair.second.IsScalar()) {
                const YAML::Node& node = pair.second;
                if (node.IsInt()) {
                    value = node.as<int64_t>();
                } else if (node.IsFloat()) {
                    value = node.as<double>();
                } else if (node.IsBool()) {
                    value = node.as<bool>();
                } else {
                    // String scalar
                    value = node.Scalar();
                }
            } else if (pair.second.IsSequence()) {
                const YAML::Node& sequence = pair.second;
                if (sequence.size() > 0) {
                    if (std::all_of(sequence.begin(), sequence.end(), [](const YAML::Node& node) { return node.IsInt(); })) {
                        std::vector<int> intArray;
                        for (const auto& node : sequence) {
                            intArray.push_back(node.as<int64_t>());
                        }
                        value = intArray;
                    } else if (std::all_of(sequence.begin(), sequence.end(), [](const YAML::Node& node) { return node.IsFloat(); })) {
                        std::vector<double> doubleArray;
                        for (const auto& node : sequence) {
                            doubleArray.push_back(node.as<double>());
                        }
                        value = doubleArray;
                    } else if (std::all_of(sequence.begin(), sequence.end(), [](const YAML::Node& node) { return node.IsScalar(); })) {
                        std::vector<std::string> stringArray;
                        for (const auto& node : sequence) {
                            stringArray.push_back(node.Scalar());
                        }
                        value = stringArray;
                    } else {
                        // Mixed sequence - convert to string representation
                        std::ostringstream oss;
                        oss << sequence;
                        value = oss.str();
                    }
                } else {
                    value = std::vector<int>(); // Empty array
                }
            } else {
                // Other types - convert to string representation
                std::ostringstream oss;
                oss << pair.second;
                value = oss.str();
            }
            
            entries.emplace_back(key, value, ConfigSource::YAML, "");
        }
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("YAML parsing failed: ") + e.what());
    }
    
    return entries;
}

// ConfigManager implementation
ConfigManager::ConfigManager() {
    // Register default parsers
    registerParser(createJSONParser());
    registerParser(createYAMLParser());
}

ConfigManager::~ConfigManager() = default;

void ConfigManager::registerParser(std::unique_ptr<ConfigParser> parser) {
    if (parser) {
        parsers_.push_back(std::move(parser));
    }
}

bool ConfigManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.good()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return loadFromString(content, "auto");
}

bool ConfigManager::loadFromString(const std::string& content, const std::string& format) {
    try {
        entries_.clear();
        validationErrors_.clear();
        
        if (format == "auto") {
            std::string detectedFormat = detectFormat(content);
            if (detectedFormat.empty()) {
                return false;
            }
            
            for (const auto& parser : parsers_) {
                if (parser->getFormatName() == detectedFormat && parser->canParse(content)) {
                    auto parsedEntries = parser->parse(content);
                    entries_.insert(entries_.end(), parsedEntries.begin(), parsedEntries.end());
                    break;
                }
            }
        } else {
            for (const auto& parser : parsers_) {
                if (parser->getFormatName() == format && parser->canParse(content)) {
                    auto parsedEntries = parser->parse(content);
                    entries_.insert(entries_.end(), parsedEntries.begin(), parsedEntries.end());
                    break;
                }
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        validationErrors_.push_back(std::string("Failed to parse configuration: ") + e.what());
        return false;
    }
}

bool ConfigManager::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.good()) {
        return false;
    }
    
    std::string content;
    if (!saveToString(content)) {
        return false;
    }
    
    file << content;
    return file.good();
}

bool ConfigManager::saveToString(std::string& output) const {
    try {
        // Convert to JSON format
        nlohmann::json jsonObj;
        
        for (const auto& entry : entries_) {
            std::visit([&](const auto& val) {
                jsonObj[entry.key] = val;
            }, entry.value);
        }
        
        output = jsonObj.dump(2); // Pretty print with 2-space indentation
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::string ConfigManager::detectFormat(const std::string& content) const {
    if (content.empty()) return "";
    
    std::string trimmed = trim(content);
    if (trimmed.empty()) return "";
    
    // Check for JSON format
    try {
        auto json = nlohmann::json::parse(trimmed);
        if (json.is_object()) {
            return "JSON";
        }
    } catch (...) {}
    
    // Check for YAML format
    try {
        auto yaml = YAML::Load(trimmed);
        if (yaml.IsMap()) {
            return "YAML";
        }
    } catch (...) {}
    
    return "";
}

bool ConfigManager::validate() const {
    validationErrors_.clear();
    
    // Basic validation
    for (const auto& entry : entries_) {
        if (entry.key.empty()) {
            validationErrors_.push_back("Empty key found");
            continue;
        }
        
        // Check for duplicate keys
        auto it = std::find_if(entries_.begin(), entries_.end(),
                              [&](const ConfigEntry& e) { return e.key == entry.key && &e != &entry; });
        if (it != entries_.end()) {
            validationErrors_.push_back("Duplicate key: " + entry.key);
        }
        
        // Type validation (basic)
        if (entry.value.index() == 0) { // int
            // OK
        } else if (entry.value.index() == 1) { // int64_t
            // OK
        } else if (entry.value.index() == 2) { // double
            // OK
        } else if (entry.value.index() == 3) { // bool
            // OK
        } else if (entry.value.index() == 4) { // string
            // OK
        } else if (entry.value.index() == 5) { // vector<int>
            // OK
        } else if (entry.value.index() == 6) { // vector<double>
            // OK
        } else if (entry.value.index() == 7) { // vector<string>
            // OK
        }
    }
    
    return validationErrors_.empty();
}

std::vector<std::string> ConfigManager::getValidationErrors() const {
    return validationErrors_;
}

ConfigEntry* ConfigManager::findEntry(const std::string& key) {
    auto it = std::find_if(entries_.begin(), entries_.end(),
                          [&](const ConfigEntry& e) { return e.key == key; });
    return it != entries_.end() ? &*it : nullptr;
}

const ConfigEntry* ConfigManager::findEntry(const std::string& key) const {
    auto it = std::find_if(entries_.begin(), entries_.end(),
                          [&](const ConfigEntry& e) { return e.key == key; });
    return it != entries_.end() ? &*it : nullptr;
}

void ConfigManager::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    ConfigEntry entry(key, value, source, "");
    
    // Check if entry already exists
    auto it = std::find_if(entries_.begin(), entries_.end(),
                          [&](const ConfigEntry& e) { return e.key == key; });
    
    if (it != entries_.end()) {
        // Update existing entry
        it->value = value;
        it->source = source;
    } else {
        // Add new entry
        entries_.push_back(entry);
    }
}

bool ConfigManager::has(const std::string& key) const {
    return findEntry(key) != nullptr;
}

std::vector<std::string> ConfigManager::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(entries_.size());
    
    for (const auto& entry : entries_) {
        keys.push_back(entry.key);
    }
    
    return keys;
}

std::string ConfigManager::getFormatSummary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << entries_.size() << " entries):" << std::endl;
    oss << "Format: JSON" << std::endl;
    oss << "Sources:" << std::endl;
    
    std::map<ConfigSource, int> sourceCount;
    for (const auto& entry : entries_) {
        sourceCount[entry.source]++;
    }
    
    for (const auto& pair : sourceCount) {
        oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << " entries" << std::endl;
    }
    
    return oss.str();
}

// Helper functions
std::string ConfigManager::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool ConfigManager::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool ConfigManager::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool ConfigManager::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool ConfigManager::isAlphanumeric(char c) {
    return isDigit(c) || isAlpha(c);
}

std::string ConfigManager::normalizeKey(const std::string& key) {
    std::string result = key;
    // Convert to lowercase
    std::transform(result.begin(), result.end(), result.begin(),
                  [](unsigned char c) { return std::tolower(c); });
    // Remove spaces
    result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
    return result;
}

// ConfigProfile implementation
ConfigProfile::ConfigProfile() = default;

void ConfigProfile::addProfile(const Profile& profile) {
    profiles_[profile.name] = profile;
}

bool ConfigProfile::loadProfilesFromFile(const std::string& filepath) {
    // In a real implementation, this would load profiles from a JSON/YAML file
    // For now, just add some example profiles
    Profile defaultProfile;
    defaultProfile.name = "default";
    defaultProfile.description = "Default NLM configuration";
    profiles_["default"] = defaultProfile;
    
    Profile plasticityProfile;
    plasticityProfile.name = "plasticity";
    plasticityProfile.description = "Configuration optimized for plasticity experiments";
    profiles_["plasticity"] = plasticityProfile;
    
    return true;
}

bool ConfigProfile::saveProfilesToFile(const std::string& filepath) const {
    // In a real implementation, this would save profiles to a JSON/YAML file
    return false;
}

Profile* ConfigProfile::getProfile(const std::string& name) {
    auto it = profiles_.find(name);
    return it != profiles_.end() ? &it->second : nullptr;
}

const Profile* ConfigProfile::getProfile(const std::string& name) const {
    auto it = profiles_.find(name);
    return it != profiles_.end() ? &it->second : nullptr;
}

bool ConfigProfile::hasProfile(const std::string& name) const {
    return profiles_.find(name) != profiles_.end();
}

std::vector<std::string> ConfigProfile::getProfileNames() const {
    std::vector<std::string> names;
    names.reserve(profiles_.size());
    
    for (const auto& pair : profiles_) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<ConfigEntry> ConfigProfile::resolveProfile(const std::string& profileName) const {
    std::vector<ConfigEntry> resolvedEntries;
    
    auto profileIt = profiles_.find(profileName);
    if (profileIt == profiles_.end()) {
        return resolvedEntries;
    }
    
    const Profile& profile = profileIt->second;
    
    // If profile has a parent, inherit from parent first
    if (!profile.parentProfile.empty()) {
        auto parentIt = profiles_.find(profile.parentProfile);
        if (parentIt != profiles_.end()) {
            const Profile& parentProfile = parentIt->second;
            std::vector<ConfigEntry> parentEntries = resolveProfile(parentProfile.name);
            resolvedEntries.insert(resolvedEntries.end(), parentEntries.begin(), parentEntries.end());
        }
    }
    
    // Add profile's own entries (override parent entries)
    resolvedEntries.insert(resolvedEntries.end(), profile.entries.begin(), profile.entries.end());
    
    return resolvedEntries;
}

// ConfigTemplate implementation
ConfigTemplate::ConfigTemplate() = default;

void ConfigTemplate::addTemplate(const Template& template) {
    templates_[template.name] = template;
    categoryMap_[template.category].push_back(template.name);
}

bool ConfigTemplate::loadTemplatesFromFile(const std::string& filepath) {
    // In a real implementation, this would load templates from a JSON/YAML file
    // For now, just add some example templates
    Template defaultTemplate;
    defaultTemplate.name = "default";
    defaultTemplate.description = "Default NLM configuration template";
    defaultTemplate.category = "basic";
    templates_["default"] = defaultTemplate;
    categoryMap_["basic"].push_back("default");
    
    Template plasticityTemplate;
    plasticityTemplate.name = "plasticity";
    plasticityTemplate.description = "Plasticity experiment template";
    plasticityTemplate.category = "advanced";
    templates_["plasticity"] = plasticityTemplate;
    categoryMap_["advanced"].push_back("plasticity");
    
    return true;
}

bool ConfigTemplate::saveTemplatesToFile(const std::string& filepath) const {
    // In a real implementation, this would save templates to a JSON/YAML file
    return false;
}

Template* ConfigTemplate::getTemplate(const std::string& name) {
    auto it = templates_.find(name);
    return it != templates_.end() ? &it->second : nullptr;
}

const Template* ConfigTemplate::getTemplate(const std::string& name) const {
    auto it = templates_.find(name);
    return it != templates_.end() ? &it->second : nullptr;
}

bool ConfigTemplate::hasTemplate(const std::string& name) const {
    return templates_.find(name) != templates_.end();
}

std::vector<std::string> ConfigTemplate::getTemplateNames() const {
    std::vector<std::string> names;
    names.reserve(templates_.size());
    
    for (const auto& pair : templates_) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<std::string> ConfigTemplate::getTemplateCategories() const {
    std::vector<std::string> categories;
    for (const auto& pair : categoryMap_) {
        categories.push_back(pair.first);
    }
    return categories;
}

// Schema implementations
namespace schemas {

ConfigSchema getDefaultSchema() {
    ConfigSchema schema;
    
    ConfigSchema::Field neuronCountField;
    neuronCountField.name = "neuron_count";
    neuronCountField.type = "int";
    neuronCountField.required = true;
    neuronCountField.defaultValue = size_t(1000);
    neuronCountField.description = "Number of neurons in the network";
    schema.addField(neuronCountField);
    
    ConfigSchema::Field timestepField;
    timestepField.name = "simulation_timestep";
    timestepField.type = "double";
    timestepField.required = true;
    timestepField.defaultValue = 0.001;
    timestepField.description = "Simulation time step in seconds";
    schema.addField(timestepField);
    
    ConfigSchema::Field regionCountField;
    regionCountField.name = "region_count";
    regionCountField.type = "int";
    regionCountField.required = false;
    regionCountField.defaultValue = size_t(1);
    regionCountField.description = "Number of neural regions";
    schema.addField(regionCountField);
    
    ConfigSchema::Field connectionProbField;
    connectionProbField.name = "connection_probability";
    connectionProbField.type = "double";
    connectionProbField.required = false;
    connectionProbField.defaultValue = 0.1;
    connectionProbField.description = "Probability of connection between neurons";
    schema.addField(connectionProbField);
    
    return schema;
}

ConfigSchema getPlasticitySchema() {
    ConfigSchema schema = getDefaultSchema();
    
    ConfigSchema::Field stdpLtpField;
    stdpLtpField.name = "stdp_ltp_weight";
    stdpLtpField.type = "double";
    stdpLtpField.required = false;
    stdpLtpField.defaultValue = 0.025;
    stdpLtpField.description = "STDP long-term potentiation weight";
    schema.addField(stdpLtpField);
    
    ConfigSchema::Field stdpLtdField;
    stdpLtdField.name = "stdp_ltd_weight";
    stdpLtdField.type = "double";
    stdpLtdField.required = false;
    stdpLtdField.defaultValue = 0.02;
    stdpLtdField.description = "STDP long-term depression weight";
    schema.addField(stdpLtdField);
    
    ConfigSchema::Field stdpTauField;
    stdpTauField.name = "stdp_tau";
    stdpTauField.type = "double";
    stdpTauField.required = false;
    stdpTauField.defaultValue = 20.0;
    stdpTauField.description = "STDP time constant in milliseconds";
    schema.addField(stdpTauField);
    
    ConfigSchema::Field synaptogenesisField;
    synaptogenesisField.name = "synaptogenesis_rate";
    synaptogenesisField.type = "double";
    synaptogenesisField.required = false;
    synaptogenesisField.defaultValue = 0.0002;
    synaptogenesisField.description = "Synaptogenesis rate";
    schema.addField(synaptogenesisField);
    
    ConfigSchema::Field pruningField;
    pruningField.name = "pruning_rate";
    pruningField.type = "double";
    pruningField.required = false;
    pruningField.defaultValue = 0.00002;
    pruningField.description = "Pruning rate";
    schema.addField(pruningField);
    
    return schema;
}

ConfigSchema getPerformanceSchema() {
    ConfigSchema schema = getDefaultSchema();
    
    ConfigSchema::Field neuronCountField;
    neuronCountField.name = "neuron_count";
    neuronCountField.type = "int";
    neuronCountField.required = false;
    neuronCountField.defaultValue = size_t(500);
    neuronCountField.description = "Number of neurons for performance testing";
    schema.addField(neuronCountField);
    
    ConfigSchema::Field connectionProbField;
    connectionProbField.name = "connection_probability";
    connectionProbField.type = "double";
    connectionProbField.required = false;
    connectionProbField.defaultValue = 0.05;
    connectionProbField.description = "Connection probability for performance testing";
    schema.addField(connectionProbField);
    
    return schema;
}

ConfigSchema getDevelopmentSchema() {
    ConfigSchema schema = getDefaultSchema();
    
    ConfigSchema::Field neuronCountField;
    neuronCountField.name = "neuron_count";
    neuronCountField.type = "int";
    neuronCountField.required = false;
    neuronCountField.defaultValue = size_t(1000);
    neuronCountField.description = "Number of neurons for developmental studies";
    schema.addField(neuronCountField);
    
    ConfigSchema::Field developmentalStageField;
    developmentalStageField.name = "developmental_stage";
    developmentalStageField.type = "int";
    developmentalStageField.required = false;
    developmentalStageField.defaultValue = 0;
    developmentalStageField.description = "Developmental stage (0=initial, 1=critical, 2=maturation, 3=adult)";
    schema.addField(developmentalStageField);
    
    return schema;
}

ConfigSchema getLearningSchema() {
    ConfigSchema schema = getDefaultSchema();
    
    ConfigSchema::Field curiosityField;
    curiosityField.name = "curiosity_level";
    curiosityField.type = "double";
    curiosityField.required = false;
    curiosityField.defaultValue = 0.5;
    curiosityField.description = "Curiosity level for exploration behavior";
    schema.addField(curiosityField);
    
    ConfigSchema::Field noveltyField;
    noveltyField.name = "novelty_threshold";
    noveltyField.type = "double";
    noveltyField.required = false;
    noveltyField.defaultValue = 0.1;
    noveltyField.description = "Novelty detection threshold";
    schema.addField(noveltyField);
    
    ConfigSchema::Field rewardField;
    rewardField.name = "reward_strength";
    rewardField.type = "double";
    rewardField.required = false;
    rewardField.defaultValue = 1.0;
    rewardField.description = "Reward prediction error strength";
    schema.addField(rewardField);
    
    return schema;
}

} // namespace schemas

} // namespace config
} // namespace nlm