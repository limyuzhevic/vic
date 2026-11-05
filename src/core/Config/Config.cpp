#include "Config.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <cctype>
#include <unordered_map>
#include <set>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <limits>

namespace nlm {

// Helper functions for string manipulation
std::string Config::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && std::isspace(str[start])) {
        start++;
    }
    
    while (end > start && std::isspace(str[end - 1])) {
        end--;
    }
    
    return str.substr(start, end - start);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string Config::escapeJSON(const std::string& str) {
    std::string result;
    result.reserve(str.size() + 2);
    
    for (char c : str) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    result += "\\u" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

std::string Config::escapeYAML(const std::string& str) {
    // Simple YAML escaping
    std::string result;
    
    // Check if string needs quoting
    bool needsQuoting = false;
    for (char c : str) {
        if (std::isspace(c) || c == ':' || c == '{' || c == '}' || c == '[' || c == ']' ||
            c == ',' || c == '#' || c == '-' || c == '?' || c == '|' || c == '>' ||
            c == "'"[0] || c == '"'[0] || c == '`'[0]) {
            needsQuoting = true;
            break;
        }
    }
    
    if (needsQuoting) {
        // Escape special YAML characters
        result += "\"";
        for (char c : str) {
            switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20) {
                        result += "\\x" + std::to_string(static_cast<int>(c));
                    } else {
                        result += c;
                    }
                    break;
            }
        }
        result += "\"";
    } else {
        result = str;
    }
    
    return result;
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigGroup> groups;
    std::map<std::string, ConfigTemplate> templates;
    std::map<std::string, ConfigProfile> profiles;
    std::shared_ptr<Config> parentConfig;
    
    Impl() {
        createDefaultProfile();
    }
    
    void createDefaultProfile() {
        ConfigProfile defaultProfile("default");
        
        // Create common groups
        ConfigGroup basicGroup("basic");
        basicGroup.setValue("random_seed", 42);
        basicGroup.setValue("simulation_timestep", 0.001);
        basicGroup.setValue("neuron_count", 1000);
        basicGroup.setValue("region_count", 1);
        basicGroup.setValue("connection_probability", 0.1);
        
        ConfigGroup plasticityGroup("plasticity");
        plasticityGroup.setValue("plasticity_learning_rate", 0.01);
        plasticityGroup.setValue("stdp_ltp_weight", 0.01);
        plasticityGroup.setValue("stdp_ltd_weight", 0.012);
        plasticityGroup.setValue("stdp_time_constant", 20.0);
        
        ConfigGroup developmentGroup("development");
        developmentGroup.setValue("synaptogenesis_rate", 0.001);
        developmentGroup.setValue("pruning_rate", 0.0001);
        
        ConfigGroup neuromodulationGroup("neuromodulation");
        neuromodulationGroup.setValue("dopamine_baseline", 0.1);
        neuromodulationGroup.setValue("curiosity_baseline", 0.0);
        neuromodulationGroup.setValue("novelty_baseline", 0.0);
        
        ConfigGroup rewardGroup("reward");
        rewardGroup.setValue("reward_discount_factor", 0.99);
        
        ConfigGroup environmentGroup("environment");
        environmentGroup.setValue("environment_name", "GridWorld");
        environmentGroup.setValue("environment_width", 10);
        environmentGroup.setValue("environment_height", 10);
        
        ConfigGroup loggingGroup("logging");
        loggingGroup.setValue("log_level", std::string("INFO"));
        loggingGroup.setValue("log_to_file", false);
        loggingGroup.setValue("log_filename", std::string("nlm.log"));
        
        ConfigGroup simulationGroup("simulation");
        simulationGroup.setValue("max_simulation_steps", 10000);
        simulationGroup.setValue("simulation_time_limit", 0.0);
        
        ConfigGroup visualizationGroup("visualization");
        visualizationGroup.setValue("visualization_enabled", false);
        visualizationGroup.setValue("visualization_update_rate", 30.0);
        
        // Add groups to profile
        defaultProfile.addGroup(basicGroup);
        defaultProfile.addGroup(plasticityGroup);
        defaultProfile.addGroup(developmentGroup);
        defaultProfile.addGroup(neuromodulationGroup);
        defaultProfile.addGroup(rewardGroup);
        defaultProfile.addGroup(environmentGroup);
        defaultProfile.addGroup(loggingGroup);
        defaultProfile.addGroup(simulationGroup);
        defaultProfile.addGroup(visualizationGroup);
        
        profiles["default"] = defaultProfile;
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept = default;

Config& Config::operator=(Config&& other) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::string format = detectFormat(filepath);
    
    if (format == "JSON") {
        return loadFromJSON(filepath);
    } else if (format == "YAML") {
        return loadFromYAML(filepath);
    } else {
        // Fall back to simple key=value format
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        int lineNum = 0;
        while (std::getline(file, line)) {
            lineNum++;
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    }
}

bool Config::loadFromJSON(const std::string& jsonString) {
    // Simple JSON parsing implementation
    // In a real implementation, you would use a proper JSON library like nlohmann/json
    // For now, we'll implement basic JSON parsing
    
    try {
        clear();
        
        // Basic JSON parsing - find key-value pairs
        size_t pos = 0;
        while (pos < jsonString.size()) {
            // Skip whitespace
            while (pos < jsonString.size() && std::isspace(jsonString[pos])) {
                pos++;
            }
            
            if (pos >= jsonString.size()) break;
            
            // Find key (string in quotes)
            if (jsonString[pos] == '\"') {
                size_t keyEnd = jsonString.find('\"', pos + 1);
                if (keyEnd == std::string::npos) {
                    throw ConfigParseException("Invalid JSON: unclosed string at position " + std::to_string(pos));
                }
                
                std::string key = jsonString.substr(pos + 1, keyEnd - pos - 1);
                pos = keyEnd + 1;
                
                // Skip whitespace
                while (pos < jsonString.size() && std::isspace(jsonString[pos])) {
                    pos++;
                }
                
                if (pos >= jsonString.size() || jsonString[pos] != ':') {
                    throw ConfigParseException("Invalid JSON: expected ':' after key at position " + std::to_string(pos));
                }
                
                pos++; // Skip ':'
                
                // Skip whitespace
                while (pos < jsonString.size() && std::isspace(jsonString[pos])) {
                    pos++;
                }
                
                // Find value
                size_t valueStart = pos;
                int bracketDepth = 0;
                int braceDepth = 0;
                bool inString = false;
                char quoteChar = '\0';
                
                for (; pos < jsonString.size(); pos++) {
                    char c = jsonString[pos];
                    
                    if (!inString) {
                        if (c == '\"') {
                            inString = true;
                            quoteChar = c;
                        } else if (c == '[') {
                            bracketDepth++;
                        } else if (c == ']') {
                            bracketDepth--;
                        } else if (c == '{') {
                            braceDepth++;
                        } else if (c == '}') {
                            braceDepth--;
                        } else if (c == ',' && bracketDepth == 0 && braceDepth == 0) {
                            break; // End of value
                        }
                    } else {
                        if (c == quoteChar && jsonString[pos - 1] != '\\') {
                            inString = false;
                        }
                    }
                }
                
                if (pos >= jsonString.size()) {
                    throw ConfigParseException("Invalid JSON: unclosed value for key " + key);
                }
                
                std::string valueStr = jsonString.substr(valueStart, pos - valueStart);
                
                // Parse value based on type
                ConfigValue configValue = parseJSONValue(valueStr);
                set(key, configValue, ConfigSource::File);
                
                // Skip comma
                pos++;
            }
        }
        
        return true;
    } catch (const ConfigException& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigParseException(std::string("JSON parsing error: ") + e.what());
    }
}

bool Config::loadFromYAML(const std::string& yamlString) {
    // Simple YAML parsing implementation
    // In a real implementation, you would use a proper YAML library like yaml-cpp
    // For now, we'll implement basic YAML parsing
    
    try {
        clear();
        
        // Basic YAML parsing
        // This is a simplified implementation - a real one would handle complex YAML structures
        
        // Split into lines
        std::vector<std::string> lines;
        std::stringstream ss(yamlString);
        std::string line;
        
        while (std::getline(ss, line)) {
            line = trim(line);
            if (!line.empty() && line[0] != '#') {
                lines.push_back(line);
            }
        }
        
        // Parse simple key-value pairs
        for (const std::string& line : lines) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Parse value
                ConfigValue configValue = parseYAMLValue(value);
                set(key, configValue, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const ConfigException& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigParseException(std::string("YAML parsing error: ") + e.what());
    }
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
    std::string format = detectFormat(filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "JSON") {
        file << saveToJSON();
    } else if (format == "YAML") {
        file << saveToYAML();
    } else {
        // Simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << configValueToString(entry.value) << "\n\n";
        }
    }
    
    return true;
}

std::string Config::saveToJSON() const {
    std::stringstream json;
    json << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            json << ",\n";
        }
        first = false;
        
        json << "  \"" << escapeJSON(entry.key) << "\": " << configValueToJSON(entry.value);
    }
    
    json << "\n}\n";
    return json.str();
}

std::string Config::saveToYAML() const {
    std::stringstream yaml;
    yaml << "# NLM Configuration File\n";
    yaml << "# Generated by NLM Configuration System\n\n";
    
    for (const auto& entry : pImpl->entries) {
        yaml << "# " << entry.description << "\n";
        yaml << entry.key << ": " << configValueToYAML(entry.value) << "\n";
    }
    
    return yaml.str();
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        // Try parent config if available
        if (pImpl->parentConfig) {
            return pImpl->parentConfig->get<T>(key);
        }
        return std::nullopt;
    }
    
    // Convert ConfigValue to T
    return convertConfigValue<T>(it->value);
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
        pImpl->entries.emplace_back(key, value, source, "");
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

void Config::set(const std::string& groupName, const std::string& key, const ConfigValue& value) {
    // Ensure group exists
    if (pImpl->groups.find(groupName) == pImpl->groups.end()) {
        pImpl->groups[groupName] = ConfigGroup(groupName);
    }
    
    pImpl->groups[groupName].setValue(key, value);
}

bool Config::has(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return true;
    }
    
    // Try parent config if available
    if (pImpl->parentConfig) {
        return pImpl->parentConfig->has(key);
    }
    
    return false;
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
    
    // Also include keys from groups
    for (const auto& groupPair : pImpl->groups) {
        auto groupKeys = groupPair.second.getKeys();
        keys.insert(keys.end(), groupKeys.begin(), groupKeys.end());
    }
    
    // Remove duplicates
    std::sort(keys.begin(), keys.end());
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
    
    return keys;
}

std::vector<std::string> Config::getKeysInGroup(const std::string& groupName) const {
    auto it = pImpl->groups.find(groupName);
    if (it != pImpl->groups.end()) {
        return it->second.getKeys();
    }
    return std::vector<std::string>();
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->groups.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary\n";
    oss << "===================\n\n";
    oss << "Configuration Entries: " << pImpl->entries.size() << "\n";
    oss << "Configuration Groups: " << pImpl->groups.size() << "\n\n";
    
    oss << "Groups:\n";
    for (const auto& groupPair : pImpl->groups) {
        oss << "  " << groupPair.first << ": ";
        auto keys = groupPair.second.getKeys();
        oss << keys.size() << " keys\n";
    }
    
    oss << "\nEntries:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = " << configValueToString(entry.value) << " (" << static_cast<int>(entry.source) << ")\n";
    }
    
    oss << "\nParent Config: " << (pImpl->parentConfig ? "Yes" : "No") << "\n";
    
    return oss.str();
}

bool Config::validate(const ConfigSchema& schema, std::vector<std::string>& errors) const {
    return schema.validate(*this, errors);
}

void Config::applyTemplate(const ConfigTemplate& template_) {
    template_.applyTo(*this);
}

void Config::applyProfile(const ConfigProfile& profile) {
    // Apply groups from profile
    for (const auto& group : profile.groups) {
        if (pImpl->groups.find(group.getName()) == pImpl->groups.end()) {
            pImpl->groups[group.getName()] = group;
        } else {
            // Merge with existing group
            auto& existingGroup = pImpl->groups[group.getName()];
            for (const auto& key : group.getKeys()) {
                if (!existingGroup.hasValue(key)) {
                    existingGroup.setValue(key, group.getValue(key));
                }
            }
        }
    }
    
    // Apply templates from profile
    for (const auto& template_ : profile.templates) {
        if (pImpl->templates.find(template_.getName()) == pImpl->templates.end()) {
            pImpl->templates[template_.getName()] = template_;
        }
    }
}

ConfigGroup& Config::getGroup(const std::string& groupName) {
    if (pImpl->groups.find(groupName) == pImpl->groups.end()) {
        pImpl->groups[groupName] = ConfigGroup(groupName);
    }
    return pImpl->groups[groupName];
}

const ConfigGroup& Config::getGroup(const std::string& groupName) const {
    auto it = pImpl->groups.find(groupName);
    if (it != pImpl->groups.end()) {
        return it->second;
    }
    throw ConfigException("Group not found: " + groupName);
}

ConfigTemplate& Config::getTemplate(const std::string& templateName) {
    if (pImpl->templates.find(templateName) == pImpl->templates.end()) {
        pImpl->templates[templateName] = ConfigTemplate(templateName);
    }
    return pImpl->templates[templateName];
}

const ConfigTemplate& Config::getTemplate(const std::string& templateName) const {
    auto it = pImpl->templates.find(templateName);
    if (it != pImpl->templates.end()) {
        return it->second;
    }
    throw ConfigException("Template not found: " + templateName);
}

ConfigProfile& Config::getProfile(const std::string& profileName) {
    if (pImpl->profiles.find(profileName) == pImpl->profiles.end()) {
        pImpl->profiles[profileName] = ConfigProfile(profileName);
    }
    return pImpl->profiles[profileName];
}

const ConfigProfile& Config::getProfile(const std::string& profileName) const {
    auto it = pImpl->profiles.find(profileName);
    if (it != pImpl->profiles.end()) {
        return it->second;
    }
    throw ConfigException("Profile not found: " + profileName);
}

void Config::setParentConfig(const std::shared_ptr<Config>& parent) {
    pImpl->parentConfig = parent;
}

std::shared_ptr<Config> Config::getParentConfig() const {
    return pImpl->parentConfig;
}

std::string Config::detectFormat(const std::string& filepath) {
    if (filepath.size() < 5) {
        return "UNKNOWN";
    }
    
    std::string ext = filepath.substr(filepath.size() - 4);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return "JSON";
    } else if (ext == ".yaml" || ext == ".yml") {
        return "YAML";
    }
    
    // Check file content for JSON/YAML format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "UNKNOWN";
    }
    
    std::string firstLine;
    std::getline(file, firstLine);
    file.close();
    
    firstLine = trim(firstLine);
    
    if (firstLine.find('{') == 0) {
        return "JSON";
    } else if (firstLine.find('[') == 0) {
        return "JSON_ARRAY";
    } else if (firstLine.find('#') == std::string::npos && firstLine.find(' ') != std::string::npos) {
        // Heuristic for key=value format
        size_t pos = firstLine.find('=');
        if (pos != std::string::npos && pos > 0) {
            return "KEY_VALUE";
        }
    }
    
    return "UNKNOWN";
}

std::shared_ptr<Config> Config::createDefaultConfig() {
    auto config = std::make_shared<Config>();
    config->applyProfile(config->getProfile("default"));
    return config;
}

std::shared_ptr<Config> Config::createFromJSON(const std::string& jsonString) {
    auto config = std::make_shared<Config>();
    config->loadFromJSON(jsonString);
    return config;
}

std::shared_ptr<Config> Config::createFromYAML(const std::string& yamlString) {
    auto config = std::make_shared<Config>();
    config->loadFromYAML(yamlString);
    return config;
}

// Helper method implementations
ConfigValue Config::parseJSONValue(const std::string& jsonStr) {
    std::string str = trim(jsonStr);
    
    if (str.empty()) {
        return std::string();
    }
    
    if (str[0] == '\"' && str.back() == '\"') {
        return str.substr(1, str.size() - 2);
    }
    
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    if (str == "null" || str == "nil") {
        return std::string();
    }
    
    // Try numeric parsing
    try {
        if (str.find('.') != std::string::npos || str.find('e') != std::string::npos || str.find('E') != std::string::npos) {
            return std::stod(str);
        } else {
            return std::stoll(str);
        }
    } catch (...) {
        // Fall back to string
        return str;
    }
}

ConfigValue Config::parseYAMLValue(const std::string& yamlStr) {
    std::string str = trim(yamlStr);
    
    if (str.empty()) {
        return std::string();
    }
    
    // Remove quotes if present
    if ((str.front() == '\"' && str.back() == '\"') ||
        (str.front() == '\'' && str.back() == '\'')) {
        str = str.substr(1, str.size() - 2);
    }
    
    // Try boolean parsing
    if (toLower(str) == "true" || toLower(str) == "yes" || toLower(str) == "on") {
        return true;
    }
    if (toLower(str) == "false" || toLower(str) == "no" || toLower(str) == "off") {
        return false;
    }
    
    // Try numeric parsing
    try {
        if (str.find('.') != std::string::npos || str.find('e') != std::string::npos || str.find('E') != std::string::npos) {
            return std::stod(str);
        } else {
            return std::stoll(str);
        }
    } catch (...) {
        // Fall back to string
        return str;
    }
}

std::string Config::configValueToJSON(const ConfigValue& value) {
    std::stringstream json;
    
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            json << "\"" << escapeJSON(val) << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            json << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            json << val;
        } else if constexpr (std::is_same_v<T, double>) {
            json << std::fixed << std::setprecision(6) << val;
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                           std::is_same_v<T, std::vector<double>> ||
                           std::is_same_v<T, std::vector<std::string>> ||
                           std::is_same_v<T, std::vector<bool>>) {
            json << "[";
            bool first = true;
            for (const auto& item : val) {
                if (!first) json << ", ";
                first = false;
                // Convert item to JSON string recursively
                json << configValueToJSON(ConfigValue(item));
            }
            json << "]";
        } else {
            json << "\"" << escapeJSON(val) << "\"";
        }
    }, value);
    
    return json.str();
}

std::string Config::configValueToYAML(const ConfigValue& value) {
    std::stringstream yaml;
    
    std::visit([&](const auto& val) {
        using T = std::decay_t<.decltype(val)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            yaml << escapeYAML(val);
        } else if constexpr (std::is_same_v<T, bool>) {
            yaml << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            yaml << val;
        } else if constexpr (std::is_same_v<T, double>) {
            yaml << std::fixed << std::setprecision(6) << val;
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                           std::is_same_v<T, std::vector<double>> ||
                           std::is_same_v<T, std::vector<std::string>> ||
                           std::is_same_v<T, std::vector<bool>>) {
            yaml << "[";
            bool first = true;
            for (const auto& item : val) {
                if (!first) yaml << ", ";
                first = false;
                // Convert item to YAML string recursively
                yaml << configValueToYAML(ConfigValue(item));
            }
            yaml << "]";
        } else {
            yaml << escapeYAML(val);
        }
    }, value);
    
    return yaml.str();
}

std::string Config::configValueToString(const ConfigValue& value) {
    std::stringstream ss;
    
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << val << "\"";
        } else {
            ss << val;
        }
    }, value);
    
    return ss.str();
}

template<typename T>
std::optional<T> Config::convertConfigValue(const ConfigValue& value) {
    return std::nullopt; // Base implementation - needs to be specialized
}

// Template specializations for common types
template<typename T>
std::optional<int> Config::convertConfigValue(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    }
    return std::nullopt;
}

template<typename T>
std::optional<double> Config::convertConfigValue(const ConfigValue& value) {
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    }
    return std::nullopt;
}

template<typename T>
std::optional<std::string> Config::convertConfigValue(const ConfigValue& value) {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    }
    return std::nullopt;
}

template<typename T>
std::optional<bool> Config::convertConfigValue(const ConfigValue& value) {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    }
    return std::nullopt;
}

// Helper function to get all types available in ConfigValue
ConfigValue Config::getValueFromString(const std::string& str) {
    // Try different types in order of likelihood
    
    // Boolean
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    // Integer
    try {
        return std::stoll(str);
    } catch (...) {}
    
    // Double
    try {
        return std::stod(str);
    } catch (...) {}
    
    // String
    return str;
}

// Implementation of remaining helper functions
std::string Config::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && std::isspace(str[start])) {
        start++;
    }
    
    while (end > start && std::isspace(str[end - 1])) {
        end--;
    }
    
    return str.substr(start, end - start);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string Config::escapeJSON(const std::string& str) {
    std::string result;
    result.reserve(str.size() + 2);
    
    for (char c : str) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    result += "\\u" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

std::string Config::escapeYAML(const std::string& str) {
    // Simplified YAML escaping
    std::string result;
    
    // Check if string needs quoting
    bool needsQuoting = false;
    for (char c : str) {
        if (std::isspace(c) || c == ':' || c == '{' || c == '}' || c == '[' || c == ']' ||
            c == ',' || c == '#' || c == '-' || c == '?' || c == '|' || c == '>' ||
            c == "'"[0] || c == '"'[0] || c == '`'[0]) {
            needsQuoting = true;
            break;
        }
    }
    
    if (needsQuoting) {
        result += "\"";
        for (char c : str) {
            switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20) {
                        result += "\\x" + std::to_string(static_cast<int>(c));
                    } else {
                        result += c;
                    }
                    break;
            }
        }
        result += "\"";
    } else {
        result = str;
    }
    
    return result;
}

// Initialize default schema
void Config::initializeDefaultSchema() {
    // Create default schema for validation
    ConfigSchema defaultSchema("nlm_config", "1.0");
    
    defaultSchema.addEntry("random_seed", "int", "Random seed for reproducible experiments", true, 42);
    defaultSchema.addEntry("simulation_timestep", "double", "Simulation timestep in seconds", true, 0.001);
    defaultSchema.addEntry("neuron_count", "int", "Number of neurons in the brain", true, 1000);
    defaultSchema.addEntry("region_count", "int", "Number of brain regions", true, 1);
    defaultSchema.addEntry("connection_probability", "double", "Connection probability for random connectivity", true, 0.1);
    defaultSchema.addEntry("plasticity_learning_rate", "double", "Plasticity learning rate", false, 0.01);
    defaultSchema.addEntry("stdp_ltp_weight", "double", "STDP LTP weight", false, 0.01);
    defaultSchema.addEntry("stdp_ltd_weight", "double", "STDP LTD weight", false, 0.012);
    defaultSchema.addEntry("stdp_time_constant", "double", "STDP time constant", false, 20.0);
    defaultSchema.addEntry("synaptogenesis_rate", "double", "Synaptogenesis rate", false, 0.001);
    defaultSchema.addEntry("pruning_rate", "double", "Pruning rate", false, 0.0001);
    defaultSchema.addEntry("dopamine_baseline", "double", "Dopamine baseline level", false, 0.1);
    defaultSchema.addEntry("reward_discount_factor", "double", "Reward discount factor", false, 0.99);
    defaultSchema.addEntry("environment_name", "string", "Environment name", false, std::string("GridWorld"));
    defaultSchema.addEntry("environment_width", "int", "Environment width", false, 10);
    defaultSchema.addEntry("environment_height", "int", "Environment height", false, 10);
    defaultSchema.addEntry("log_level", "string", "Log level", false, std::string("INFO"));
    defaultSchema.addEntry("log_to_file", "bool", "Log to file", false, false);
    defaultSchema.addEntry("max_simulation_steps", "int", "Maximum simulation steps", false, 10000);
    defaultSchema.addEntry("visualization_enabled", "bool", "Enable visualization", false, false);
    
    // Store schema - in a real implementation, you would store it
    // For now, we'll just create it locally
}

void Config::createDefaultProfile() {
    // Default profile is created in Impl constructor
}