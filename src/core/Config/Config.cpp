#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <limits>
#include <cstring>
#include <cctype>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::string> groups;
    std::vector<std::string> requiredGroups;
    nlohmann::json schema;
    
    Impl() : schema(createDefaultSchema()) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return loadFromJsonFile(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        return loadFromYamlFile(filepath);
    } else if (extension == ".ini" || extension == ".cfg") {
        return loadFromIniFile(filepath);
    } else {
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromJson(const std::string& jsonStr) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonStr);
        clear();
        
        if (jsonData.contains("configuration")) {
            auto configArray = jsonData["configuration"];
            for (const auto& item : configArray) {
                ConfigEntry entry = ConfigEntry::fromJson(item);
                pImpl->entries.push_back(entry);
            }
            
            if (jsonData.contains("schema")) {
                pImpl->schema = jsonData["schema"];
            }
        } else if (jsonData.is_object()) {
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                ConfigSource source = ConfigSource::File;
                
                std::string type = "unknown";
                if (it->is_number_integer()) {
                    type = "int";
                } else if (it->is_number_unsigned()) {
                    type = "uint64_t";
                } else if (it->is_number()) {
                    type = "double";
                } else if (it->is_boolean()) {
                    type = "bool";
                } else if (it->is_string()) {
                    type = "string";
                } else if (it->is_array()) {
                    if (!it->empty() && it->front().is_number_integer()) {
                        type = "std::vector<int>";
                    } else if (!it->empty() && it->front().is_number()) {
                        type = "std::vector<double>";
                    } else {
                        type = "std::vector<std::string>";
                    }
                }
                
                ConfigValue value;
                if (type == "int") value = it->get<int>();
                else if (type == "uint64_t") value = it->get<uint64_t>();
                else if (type == "double") value = it->get<double>();
                else if (type == "bool") value = it->get<bool>();
                else if (type == "string") value = it->get<std::string>();
                else if (type == "std::vector<int>") value = it->get<std::vector<int>>();
                else if (type == "std::vector<double>") value = it->get<std::vector<double>>();
                else if (type == "std::vector<std::string>") value = it->get<std::vector<std::string>>();
                
                pImpl->entries.emplace_back(key, value, source, "", type);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYaml(const std::string& yamlStr) {
    try {
        YAML::Node yamlData = YAML::Load(yamlStr);
        clear();
        
        if (yamlData["configuration"]) {
            auto configArray = yamlData["configuration"];
            for (const auto& item : configArray) {
                ConfigEntry entry;
                entry.key = item["key"].as<std::string>();
                entry.description = item["description"].as<std::string>();
                entry.source = static_cast<ConfigSource>(item["source"].as<int>());
                entry.typeName = item["type"].as<std::string>();
                entry.isRequired = item["required"].as<bool>();
                
                if (item["value"]) {
                    auto value = item["value"];
                    if (value.IsScalar()) {
                        if (entry.typeName == "int") {
                            entry.value = value.as<int>();
                        } else if (entry.typeName == "double") {
                            entry.value = value.as<double>();
                        } else if (entry.typeName == "bool") {
                            entry.value = value.as<bool>();
                        } else if (entry.typeName == "string") {
                            entry.value = value.as<std::string>();
                        }
                    }
                }
                
                pImpl->entries.push_back(entry);
            }
        } else if (yamlData.IsMap()) {
            for (const auto& item : yamlData) {
                std::string key = item.first.as<std::string>();
                ConfigSource source = ConfigSource::File;
                
                std::string type = "unknown";
                if (item.second.IsScalar()) {
                    auto value = item.second;
                    if (value.IsInteger()) {
                        type = "int";
                    } else if (value.IsDouble()) {
                        type = "double";
                    } else if (value.IsBoolean()) {
                        type = "bool";
                    } else if (value.IsString()) {
                        type = "string";
                    }
                }
                
                ConfigValue configValue;
                if (type == "int") configValue = item.second.as<int>();
                else if (type == "double") configValue = item.second.as<double>();
                else if (type == "bool") configValue = item.second.as<bool>();
                else if (type == "string") configValue = item.second.as<std::string>();
                
                pImpl->entries.emplace_back(key, configValue, source, "", type);
            }
        }
        
        return true;
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("YAML parse error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromIni(const std::string& filepath) {
    return loadFromIniFile(filepath);
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return saveToJsonFile(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        return saveToYamlFile(filepath);
    } else if (extension == ".ini" || extension == ".cfg") {
        return saveToIniFile(filepath);
    } else {
        return saveToJsonFile(filepath);
    }
}

std::string Config::saveToJson() const {
    nlohmann::json jsonData;
    
    nlohmann::json configArray = nlohmann::json::array();
    for (const auto& entry : pImpl->entries) {
        configArray.push_back(entry.toJson());
    }
    
    jsonData["configuration"] = configArray;
    
    if (!pImpl->schema.empty()) {
        jsonData["schema"] = pImpl->schema;
    }
    
    return jsonData.dump(2);
}

std::string Config::saveToYaml() const {
    YAML::Emitter emitter;
    emitter.SetIndent(2);
    
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "configuration";
    emitter << YAML::Value << YAML::BeginSeq;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "key" << YAML::Value << entry.key;
        emitter << YAML::Key << "description" << YAML::Value << entry.description;
        emitter << YAML::Key << "source" << YAML::Value << static_cast<int>(entry.source);
        emitter << YAML::Key << "type" << YAML::Value << entry.typeName;
        emitter << YAML::Key << "required" << YAML::Value << entry.isRequired;
        
        if (std::holds_alternative<int>(entry.value)) {
            emitter << YAML::Key << "value" << YAML::Value << std::get<int>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            emitter << YAML::Key << "value" << YAML::Value << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            emitter << YAML::Key << "value" << YAML::Value << std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            emitter << YAML::Key << "value" << YAML::Value << std::get<std::string>(entry.value);
        }
        
        emitter << YAML::EndMap;
    }
    
    emitter << YAML::EndSeq;
    emitter << YAML::EndMap;
    
    return emitter.c_str();
}

bool Config::saveToJsonFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    std::string jsonStr = saveToJson();
    file << jsonStr;
    
    return true;
}

bool Config::saveToYamlFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    std::string yamlStr = saveToYaml();
    file << yamlStr;
    
    return true;
}

bool Config::saveToIniFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::string valueStr = configValueToString(entry.value);
        file << valueStr << "\n\n";
    }
    
    return true;
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
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
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
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

bool Config::loadFromJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return loadFromJson(buffer.str());
}

bool Config::loadFromYamlFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open YAML file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return loadFromYaml(buffer.str());
}

bool Config::loadFromIniFile(const std::string& filepath) {
    return loadFromSimpleFile(filepath);
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
        NLM_LOG_WARNING("Type mismatch for key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value,
                 ConfigSource source,
                 const std::string& description,
                 const std::string& type,
                 bool required,
                 const std::string& validationRegex,
                 double minValue,
                 double maxValue,
                 const std::vector<std::string>& allowedValues) {
    std::string normalizedKey = normalizeKey(key);
    
    std::string entryType = type;
    if (entryType.empty()) {
        entryType = getTypeNameFromValue(value);
    }
    
    ConfigEntry entry(normalizedKey, value, source, description, entryType, required,
                     validationRegex, minValue, maxValue, allowedValues);
    
    if (!validateEntry(entry)) {
        NLM_LOG_ERROR("Invalid configuration entry for key: " + key);
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
    
    if (it != pImpl->entries.end()) {
        *it = entry;
    } else {
        pImpl->entries.push_back(entry);
    }
}

void Config::set(const std::string& key, const std::string& value,
                 ConfigSource source,
                 const std::string& description,
                 const std::string& type,
                 bool required,
                 const std::string& validationRegex,
                 double minValue,
                 double maxValue,
                 const std::vector<std::string>& allowedValues) {
    ConfigValue configValue = parseValue(value);
    set(key, configValue, source, description, type, required, validationRegex,
        minValue, maxValue, allowedValues);
}

void Config::set(const std::string& key, int value,
                 ConfigSource source,
                 const std::string& description,
                 const std::string& type,
                 bool required,
                 double minValue,
                 double maxValue) {
    std::string normalizedKey = normalizeKey(key);
    std::string entryType = type.empty() ? "int" : type;
    
    ConfigEntry entry(normalizedKey, value, source, description, entryType, required,
                     "", minValue, maxValue);
    
    if (!validateEntry(entry)) {
        NLM_LOG_ERROR("Invalid configuration entry for key: " + key);
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
    
    if (it != pImpl->entries.end()) {
        *it = entry;
    } else {
        pImpl->entries.push_back(entry);
    }
}

void Config::set(const std::string& key, double value,
                 ConfigSource source,
                 const std::string& description,
                 const std::string& type,
                 bool required,
                 double minValue,
                 double maxValue) {
    std::string normalizedKey = normalizeKey(key);
    std::string entryType = type.empty() ? "double" : type;
    
    ConfigEntry entry(normalizedKey, value, source, description, entryType, required,
                     "", minValue, maxValue);
    
    if (!validateEntry(entry)) {
        NLM_LOG_ERROR("Invalid configuration entry for key: " + key);
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
    
    if (it != pImpl->entries.end()) {
        *it = entry;
    } else {
        pImpl->entries.push_back(entry);
    }
}

void Config::set(const std::string& key, bool value,
                 ConfigSource source,
                 const std::string& description,
                 const std::string& type,
                 bool required) {
    std::string normalizedKey = normalizeKey(key);
    std::string entryType = type.empty() ? "bool" : type;
    
    ConfigEntry entry(normalizedKey, value, source, description, entryType, required);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
    
    if (it != pImpl->entries.end()) {
        *it = entry;
    } else {
        pImpl->entries.push_back(entry);
    }
}

bool Config::has(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
}

bool Config::isValid(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
    
    if (it == pImpl->entries.end()) return false;
    
    return validateEntry(*it);
}

bool Config::isComplete() const {
    return std::all_of(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.isRequired ? e.validate() : true; });
}

void Config::remove(const std::string& key) {
    std::string normalizedKey = normalizeKey(key);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; }),
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

std::vector<std::string> Config::getRequiredKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.isRequired) {
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
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        oss << configValueToString(entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (entry.isRequired) oss << " [REQUIRED]";
        oss << "\n";
    }
    return oss.str();
}

std::string Config::summaryWithValidation() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "Valid: " << getValidEntries().size() << "/" << pImpl->entries.size() << "\n";
    oss << "Required: " << getRequiredKeys().size() << "\n";
    oss << "Missing: " << getMissingEntries().size() << "\n";
    oss << "\nEntries:\n";
    
    for (const auto& entry : pImpl->entries) {
        std::string status = entry.validate() ? "✓" : "✗";
        oss << "  " << status << " " << entry.key << " = [" << configValueToString(entry.value) << "]";
        if (entry.isRequired) oss << " [REQUIRED]";
        oss << "\n";
    }
    
    return oss.str();
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            if (entry.source == ConfigSource::CommandLine || it->source != ConfigSource::CommandLine) {
                *it = entry;
            }
        } else {
            pImpl->entries.push_back(entry);
        }
    }
}

bool Config::validateSchema() const {
    return validateAgainstSchema(pImpl->entries, pImpl->schema);
}

std::string Config::getSchema() const {
    return pImpl->schema.dump(2);
}

Config Config::createDefaultConfig() {
    Config config;
    
    config.set("neuron_count", 1000, ConfigSource::Default, 
              "Number of neurons in the network", "int", false, 1, 10000);
    config.set("region_count", 1, ConfigSource::Default,
              "Number of neural regions", "int", false, 1, 10);
    config.set("connection_probability", 0.1, ConfigSource::Default,
              "Probability of creating connections between neurons", "double", false, 0.0, 1.0);
    config.set("simulation_timestep", 0.001, ConfigSource::Default,
              "Time step for simulation (seconds)", "double", false, 1e-6, 0.1);
    
    config.set("working_memory_capacity", 100, ConfigSource::Default,
              "Capacity of working memory in neurons", "int", false, 1, 10000);
    config.set("episodic_memory_max_episodes", 1000, ConfigSource::Default,
              "Maximum number of episodes in episodic memory", "int", false, 10, 100000);
    
    config.set("stdp_ltp_weight", 0.01, ConfigSource::Default,
              "STDP long-term potentiation weight", "double", false, 0.0, 0.1);
    config.set("stdp_ltd_weight", 0.012, ConfigSource::Default,
              "STDP long-term depression weight", "double", false, 0.0, 0.1);
    config.set("stdp_tau", 20.0, ConfigSource::Default,
              "STDP time constant (ms)", "double", false, 1.0, 100.0);
    config.set("synaptogenesis_rate", 0.0001, ConfigSource::Default,
              "Rate of new synapse formation", "double", false, 0.0, 0.01);
    config.set("pruning_rate", 0.00001, ConfigSource::Default,
              "Rate of synapse removal", "double", false, 0.0, 0.001);
    
    config.set("dopamine_baseline", 0.1, ConfigSource::Default,
              "Baseline dopamine level", "double", false, 0.0, 1.0);
    config.set("curiosity_baseline", 0.1, ConfigSource::Default,
              "Baseline curiosity level", "double", false, 0.0, 1.0);
    
    config.set("planning_depth", 5, ConfigSource::Default,
              "Depth of planning horizon", "int", false, 1, 20);
    config.set("concept_formation_threshold", 0.5, ConfigSource::Default,
              "Threshold for concept formation", "double", false, 0.0, 1.0);
    
    config.set("plasticity_modulation_factor", 0.5, ConfigSource::Default,
              "Factor controlling plasticity reduction during development", "double", false, 0.0, 1.0);
    
    return config;
}

Config Config::createMinimalConfig() {
    Config config;
    
    config.set("neuron_count", 100, ConfigSource::Default,
              "Number of neurons in the network", "int", false, 10, 1000);
    config.set("simulation_timestep", 0.001, ConfigSource::Default,
              "Time step for simulation (seconds)", "double", false, 1e-6, 0.1);
    
    return config;
}

Config Config::loadFromEnvironment() {
    Config config;
    
    const char* envNeuronCount = std::getenv("NLM_NEURON_COUNT");
    if (envNeuronCount) {
        config.set("neuron_count", std::stoi(envNeuronCount), ConfigSource::Runtime);
    }
    
    const char* envTimestep = std::getenv("NLM_TIMESTEP");
    if (envTimestep) {
        config.set("simulation_timestep", std::stod(envTimestep), ConfigSource::Runtime);
    }
    
    const char* envSeed = std::getenv("NLM_SEED");
    if (envSeed) {
        config.set("random_seed", std::stoull(envSeed), ConfigSource::Runtime);
    }
    
    return config;
}

size_t Config::getTotalEntries() const {
    return pImpl->entries.size();
}

size_t Config::getRequiredEntries() const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
                         [](const ConfigEntry& e) { return e.isRequired; });
}

size_t Config::getValidEntries() const {
    return getValidEntries().size();
}

size_t Config::getMissingEntries() const {
    return getMissingEntries().size();
}

std::vector<ConfigEntry> Config::getValidEntries() const {
    std::vector<ConfigEntry> validEntries;
    for (const auto& entry : pImpl->entries) {
        if (validateEntry(entry)) {
            validEntries.push_back(entry);
        }
    }
    return validEntries;
}

std::vector<ConfigEntry> Config::getInvalidEntries() const {
    std::vector<ConfigEntry> invalidEntries;
    for (const auto& entry : pImpl->entries) {
        if (!validateEntry(entry)) {
            invalidEntries.push_back(entry);
        }
    }
    return invalidEntries;
}

std::vector<ConfigEntry> Config::getMissingEntries() const {
    std::vector<ConfigEntry> missingEntries;
    for (const auto& entry : pImpl->entries) {
        if (entry.isRequired && !has(entry.key)) {
            missingEntries.push_back(entry);
        }
    }
    return missingEntries;
}

void Config::addGroup(const std::string& groupName) {
    pImpl->groups[groupName] = "";
    pImpl->requiredGroups.push_back(groupName);
}

void Config::removeGroup(const std::string& groupName) {
    pImpl->groups.erase(groupName);
    pImpl->requiredGroups.erase(
        std::remove(pImpl->requiredGroups.begin(), pImpl->requiredGroups.end(), groupName),
        pImpl->requiredGroups.end()
    );
}

bool Config::hasGroup(const std::string& groupName) const {
    return pImpl->groups.find(groupName) != pImpl->groups.end();
}

void Config::setGroup(const std::string& groupName, const std::string& key,
                      const ConfigValue& value) {
    if (!hasGroup(groupName)) {
        addGroup(groupName);
    }
    pImpl->groups[groupName] = key + "=" + configValueToString(value);
}

ConfigValue Config::getGroupValue(const std::string& groupName, const std::string& key) const {
    if (!hasGroup(groupName)) {
        return std::monostate();
    }
    return std::monostate();
}

nlohmann::json Config::createDefaultSchema() {
    nlohmann::json schema;
    schema["type"] = "object";
    schema["properties"] = nlohmann::json::object();
    schema["required"] = nlohmann::json::array();
    schema["additionalProperties"] = false;
    
    nlohmann::json definitions = nlohmann::json::object();
    nlohmann::json numberDef;
    numberDef["type"] = "number";
    numberDef["minimum"] = -std::numeric_limits<double>::max();
    numberDef["maximum"] = std::numeric_limits<double>::max();
    definitions["number"] = numberDef;
    
    nlohmann::json integerDef;
    integerDef["type"] = "integer";
    integerDef["minimum"] = -std::numeric_limits<int64_t>::max();
    integerDef["maximum"] = std::numeric_limits<int64_t>::max();
    definitions["integer"] = integerDef;
    
    nlohmann::json booleanDef;
    booleanDef["type"] = "boolean";
    definitions["boolean"] = booleanDef;
    
    nlohmann::json stringDef;
    stringDef["type"] = "string";
    definitions["string"] = stringDef;
    
    schema["definitions"] = definitions;
    
    return schema;
}

bool Config::validateAgainstSchema(const std::vector<ConfigEntry>& entries,
                                   const nlohmann::json& schema) {
    if (schema.empty()) return true;
    
    nlohmann::json instance = nlohmann::json::object();
    for (const auto& entry : entries) {
        instance[entry.key] = configValueToJson(entry.value);
    }
    
    for (const auto& entry : entries) {
        if (entry.isRequired && !instance.contains(entry.key)) {
            return false;
        }
    }
    
    return true;
}

std::string Config::getTypeNameFromValue(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "std::vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "std::vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "std::vector<std::string>";
    return "unknown";
}

ConfigValue Config::parseValue(const std::string& str) {
    if (str.empty()) return std::string();
    
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    char* endptr;
    double doubleVal = std::strtod(str.c_str(), &endptr);
    if (*endptr == '\0') {
        long longVal = std::strtoll(str.c_str(), &endptr, 10);
        if (*endptr == '\0') {
            return static_cast<int>(longVal);
        }
        return doubleVal;
    }
    
    return str;
}

std::string Config::configValueToString(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<int>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << vec[i] << "\"";
        }
        oss << "]";
        return oss.str();
    }
    return "";
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        return std::get<std::vector<int>>(value);
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return std::get<std::vector<double>>(value);
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return std::get<std::vector<std::string>>(value);
    }
    return nullptr;
}

bool Config::validateEntry(const ConfigEntry& entry) {
    if (entry.key.empty()) {
        NLM_LOG_ERROR("Configuration entry has empty key");
        return false;
    }
    
    if (!entry.validate()) {
        NLM_LOG_ERROR("Configuration entry validation failed for key: " + entry.key);
        return false;
    }
    
    std::string normalizedKey = normalizeKey(entry.key);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return normalizeKey(e.key) == normalizedKey; });
    
    if (it != pImpl->entries.end() && it != &entry) {
        NLM_LOG_WARNING("Duplicate configuration key: " + entry.key);
    }
    
    return true;
}

std::string Config::normalizeKey(const std::string& key) {
    std::string result = key;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    
    std::replace(result.begin(), result.end(), '-', '_');
    std::replace(result.begin(), result.end(), '.', '_');
    std::replace(result.begin(), result.end(), ' ', '_');
    
    return result;
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

bool Config::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

} // namespace nlm