#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <cctype>
#include <regex>
#include <iomanip>

namespace nlm {

struct ConfigSchema::Impl {
    std::vector<ConfigSchemaEntry> entries;
    std::vector<ConfigGroup> groups;
    std::unordered_map<std::string, size_t> entryIndex;
    std::unordered_map<std::string, size_t> groupIndex;
};

ConfigSchema::ConfigSchema() : pImpl(std::make_unique<Impl>()) {}

ConfigSchema::~ConfigSchema() = default;

ConfigSchema& ConfigSchema::addEntry(const ConfigSchemaEntry& entry) {
    pImpl->entries.push_back(entry);
    pImpl->entryIndex[entry.name] = pImpl->entries.size() - 1;
    return *this;
}

ConfigSchema& ConfigSchema::addGroup(const ConfigGroup& group) {
    pImpl->groups.push_back(group);
    pImpl->groupIndex[group.name] = pImpl->groups.size() - 1;
    
    for (const auto& entry : group.entries) {
        std::string prefixedName = group.name + "." + entry.name;
        ConfigSchemaEntry prefixedEntry = entry;
        prefixedEntry.name = prefixedName;
        addEntry(prefixedEntry);
    }
    
    for (const auto& subgroup : group.subgroups) {
        addGroup(subgroup.second);
    }
    
    return *this;
}

const ConfigSchemaEntry* ConfigSchema::getEntry(const std::string& name) const {
    auto it = pImpl->entryIndex.find(name);
    if (it != pImpl->entryIndex.end()) {
        return &pImpl->entries[it->second];
    }
    return nullptr;
}

const ConfigGroup* ConfigSchema::getGroup(const std::string& name) const {
    auto it = pImpl->groupIndex.find(name);
    if (it != pImpl->groupIndex.end()) {
        return &pImpl->groups[it->second];
    }
    return nullptr;
}

ValidationResult ConfigSchema::validate(const std::string& key, const ConfigValue& value) const {
    ValidationResult result;
    
    const ConfigSchemaEntry* entry = getEntry(key);
    if (!entry) {
        result.addError("Unknown configuration key: " + key);
        return result;
    }
    
    return result;
}

bool ConfigSchema::saveToFile(const std::string& filepath, bool isJSON) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        nlohmann::json entryJson;
        entryJson["name"] = entry.name;
        entryJson["description"] = entry.description;
        entryJson["category"] = static_cast<int>(entry.category);
        entryJson["required"] = entry.required;
        entryJson["type"] = entry.type;
        
        if (entry.defaultValue.has_value()) {
            entryJson["default"] = ConfigValueConverter::toJSON(entry.defaultValue.value());
        }
        
        json["entries"].push_back(entryJson);
    }
    
    for (const auto& group : pImpl->groups) {
        nlohmann::json groupJson;
        groupJson["name"] = group.name;
        groupJson["description"] = group.description;
        groupJson["hasSchema"] = group.hasSchema;
        
        json["groups"].push_back(groupJson);
    }
    
    file << json.dump(2);
    return true;
}

bool ConfigSchema::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json json;
    try {
        file >> json;
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    }
    
    clear();
    
    if (json.contains("entries")) {
        for (const auto& entryJson : json["entries"]) {
            ConfigSchemaEntry entry;
            entry.name = entryJson.value("name", "");
            entry.description = entryJson.value("description", "");
            entry.category = static_cast<ConfigCategory>(entryJson.value("category", 0));
            entry.required = entryJson.value("required", false);
            entry.type = entryJson.value("type", "auto");
            
            if (entryJson.contains("default")) {
                entry.defaultValue = ConfigValueConverter::fromJSON(entryJson["default"]);
            }
            
            addEntry(entry);
        }
    }
    
    if (json.contains("groups")) {
        for (const auto& groupJson : json["groups"]) {
            ConfigGroup group;
            group.name = groupJson.value("name", "");
            group.description = groupJson.value("description", "");
            group.hasSchema = groupJson.value("hasSchema", false);
            
            pImpl->groups.push_back(group);
            pImpl->groupIndex[group.name] = pImpl->groups.size() - 1;
        }
    }
    
    return true;
}

void ConfigSchema::clear() {
    pImpl->entries.clear();
    pImpl->groups.clear();
    pImpl->entryIndex.clear();
    pImpl->groupIndex.clear();
}

std::string ConfigSchema::summary() const {
    std::ostringstream oss;
    oss << "Configuration Schema (" << pImpl->entries.size() << " entries, " 
        << pImpl->groups.size() << " groups):\n";
    
    oss << "\nEntries:";
    for (const auto& entry : pImpl->entries) {
        oss << "\n  " << entry.name << " (" << static_cast<int>(entry.category) << ")";
        if (entry.required) oss << " [REQUIRED]";
        oss << " - " << entry.description;
    }
    
    oss << "\n\nGroups:";
    for (const auto& group : pImpl->groups) {
        oss << "\n  " << group.name << " - " << group.description;
        if (group.hasSchema) oss << " [HAS_SCHEMA]";
    }
    
    return oss.str();
}

ConfigValue ConfigValueConverter::fromJSON(const nlohmann::json& json) {
    if (json.is_null()) {
        return std::string("null");
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_number_integer()) {
        return json.get<int64_t>();
    } else if (json.is_number_unsigned()) {
        return json.get<uint64_t>();
    } else if (json.is_number()) {
        if (json.get<double>() == static_cast<int64_t>(json.get<double>())) {
            return json.get<int64_t>();
        }
        return json.get<double>();
    } else if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_array()) {
        std::vector<ConfigValue> vec;
        for (const auto& item : json) {
            vec.push_back(fromJSON(item));
        }
        
        if (!vec.empty()) {
            if (std::all_of(vec.begin(), vec.end(), [](const ConfigValue& v) { return std::holds_alternative<int>(v); })) {
                std::vector<int> intVec;
                intVec.reserve(vec.size());
                for (const auto& v : vec) intVec.push_back(std::get<int>(v));
                return intVec;
            } else if (std::all_of(vec.begin(), vec.end(), [](const ConfigValue& v) { return std::holds_alternative<double>(v); })) {
                std::vector<double> doubleVec;
                doubleVec.reserve(vec.size());
                for (const auto& v : vec) doubleVec.push_back(std::get<double>(v));
                return doubleVec;
            } else if (std::all_of(vec.begin(), vec.end(), [](const ConfigValue& v) { return std::holds_alternative<std::string>(v); })) {
                std::vector<std::string> stringVec;
                stringVec.reserve(vec.size());
                for (const auto& v : vec) stringVec.push_back(std::get<std::string>(v));
                return stringVec;
            }
        }
        
        return vec;
    }
    
    return std::string("unknown");
}

ConfigValue ConfigValueConverter::fromYAML(const YAML::Node& yaml) {
    if (!yaml) {
        return std::string("null");
    } else if (yaml.IsScalar()) {
        if (yaml.IsBoolean()) {
            return yaml.as<bool>();
        } else if (yaml.IsInteger()) {
            return yaml.as<int64_t>();
        } else if (yaml.IsDouble()) {
            return yaml.as<double>();
        } else if (yaml.IsString()) {
            return yaml.as<std::string>();
        }
    } else if (yaml.IsSequence()) {
        std::vector<ConfigValue> vec;
        for (const auto& item : yaml) {
            vec.push_back(fromYAML(item));
        }
        
        if (!vec.empty()) {
            if (std::all_of(vec.begin(), vec.end(), [](const ConfigValue& v) { return std::holds_alternative<int>(v); })) {
                std::vector<int> intVec;
                intVec.reserve(vec.size());
                for (const auto& v : vec) intVec.push_back(std::get<int>(v));
                return intVec;
            } else if (std::all_of(vec.begin(), vec.end(), [](const ConfigValue& v) { return std::holds_alternative<double>(v); })) {
                std::vector<double> doubleVec;
                doubleVec.reserve(vec.size());
                for (const auto& v : vec) doubleVec.push_back(std::get<double>(v));
                return doubleVec;
            } else if (std::all_of(vec.begin(), vec.end(), [](const ConfigValue& v) { return std::holds_alternative<std::string>(v); })) {
                std::vector<std::string> stringVec;
                stringVec.reserve(vec.size());
                for (const auto& v : vec) stringVec.push_back(std::get<std::string>(v));
                return stringVec;
            }
        }
        
        return vec;
    } else if (yaml.IsMap()) {
        std::ostringstream oss;
        oss << yaml;
        return oss.str();
    }
    
    return std::string("unknown");
}

nlohmann::json ConfigValueConverter::toJSON(const ConfigValue& value) {
    nlohmann::json json;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : arg) {
                json.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                json.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                json.push_back(item);
            }
        } else {
            json = "unsupported_type";
        }
    }, value);
    
    return json;
}

YAML::Node ConfigValueConverter::toYAML(const ConfigValue& value) {
    YAML::Node node;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else {
            node = "unsupported_type";
        }
    }, value);
    
    return node;
}

std::string ConfigValueConverter::toString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << arg;
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool ConfigValueConverter::toBool(const ConfigValue& value) {
    bool result = false;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, int>) {
            result = arg != 0;
        } else if constexpr (std::is_same_v<T, double>) {
            result = std::abs(arg) > 0.5;
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = arg == "true" || arg == "yes" || arg == "1";
        }
    }, value);
    return result;
}

int ConfigValueConverter::toInt(const ConfigValue& value) {
    int result = 0;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = static_cast<int>(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            result = static_cast<int>(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg ? 1 : 0;
        } else if constexpr (std::is_same_v<T, std::string>) {
            try {
                result = std::stoi(arg);
            } catch (...) {
                result = 0;
            }
        }
    }, value);
    return result;
}

double ConfigValueConverter::toDouble(const ConfigValue& value) {
    double result = 0.0;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = static_cast<double>(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = static_cast<double>(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg ? 1.0 : 0.0;
        } else if constexpr (std::is_same_v<T, std::string>) {
            try {
                result = std::stod(arg);
            } catch (...) {
                result = 0.0;
            }
        }
    }, value);
    return result;
}

bool ConfigValueConverter::isValidType(const std::string& typeName) {
    static const std::vector<std::string> validTypes = {
        "int", "int64_t", "double", "bool", "string",
        "std::vector<int>", "std::vector<int64_t>", "std::vector<double>",
        "std::vector<bool>", "std::vector<std::string>", "auto"
    };
    
    return std::find(validTypes.begin(), validTypes.end(), typeName) != validTypes.end();
}

std::vector<std::string> ConfigValueConverter::getSupportedTypes() {
    return {
        "int", "int64_t", "double", "bool", "string",
        "std::vector<int>", "std::vector<int64_t>", "std::vector<double>",
        "std::vector<bool>", "std::vector<std::string>"
    };
}

ValidationResult ConfigValueConverter::validate(const std::string& typeName, const ConfigValue& value) {
    ValidationResult result;
    
    if (!isValidType(typeName) && typeName != "auto") {
        result.addError("Invalid type specification: " + typeName);
        return result;
    }
    
    return result;
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigSource> sources;
    std::unordered_map<std::string, std::string> keyCategories;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultSchema() {
    schema.addEntry(ConfigSchemaEntry(
        "random_seed", "Random seed for reproducible experiments",
        ConfigCategory::System, std::nullopt, true, "int64_t"
    ));
    
    schema.addEntry(ConfigSchemaEntry(
        "simulation_timestep", "Simulation timestep in seconds",
        ConfigCategory::System, std::nullopt, true, "double"
    ));
    
    schema.addEntry(ConfigSchemaEntry(
        "neuron_count", "Number of neurons in the brain",
        ConfigCategory::Neural, std::nullopt, true, "int64_t"
    ));
    
    schema.addEntry(ConfigSchemaEntry(
        "region_count", "Number of brain regions",
        ConfigCategory::Neural, std::nullopt, true, "int64_t"
    ));
    
    schema.addEntry(ConfigSchemaEntry(
        "connection_probability", "Connection probability for random connectivity",
        ConfigCategory::Neural, std::optional<ConfigValue>(0.1), false, "double"
    ));
    
    schema.addEntry(ConfigSchemaEntry(
        "stdp_ltp_weight", "STDP LTP weight",
        ConfigCategory::Plasticity, std::optional<ConfigValue>(0.02), false, "double"
    ));
    
    schema.addEntry(ConfigSchemaEntry(
        "stdp_ltd_weight", "STDP LTD weight",
        ConfigCategory::Plasticity, std::optional<ConfigValue>(0.015), false, "double"
    ));
    
    schema.addEntry(ConfigSchemaEntry(
        "stdp_tau", "STDP time constant",
        ConfigCategory::Plasticity, std::optional<ConfigValue>(20.0), false, "double"
    ));
    
    schema.addEntry(ConfigSchemaEntry(
        "synaptogenesis_rate", "Synaptogenesis rate",
        ConfigCategory::Development, std::optional<ConfigValue>(0.0001), false, "double"
    ));
    
    schema.addEntry(ConfigSchemaEntry(
        "pruning_rate", "Pruning rate",
        ConfigCategory::Development, std::optional<ConfigValue>(0.00001), false, "double"
    ));
    
    schema.addEntry(ConfigSchemaEntry(
        "log_level", "Log level",
        ConfigCategory::Logging, std::optional<ConfigValue>(std::string("INFO")), false, "string"
    ));
    
    schema.addEntry(ConfigSchemaEntry(
        "log_to_file", "Log to file",
        ConfigCategory::Logging, std::optional<ConfigValue>(false), false, "bool"
    ));
    
    schema.addEntry(ConfigSchemaEntry(
        "environment_name", "Environment name",
        ConfigCategory::Environment, std::optional<ConfigValue>(std::string("GridWorld")), false, "string"
    ));
    
    schema.addEntry(ConfigSchemaEntry(
        "environment_width", "Environment width",
        ConfigCategory::Environment, std::optional<ConfigValue>(10), false, "int64_t"
    ));
    
    schema.addEntry(ConfigSchemaEntry(
        "environment_height", "Environment height",
        ConfigCategory::Environment, std::optional<ConfigValue>(10), false, "int64_t"
    ));
}

void Config::setSchema(const ConfigSchema& newSchema) {
    schema = newSchema;
}

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFileAuto(filepath);
}

bool Config::loadFromJSON(const std::string& jsonString) {
    try {
        auto json = nlohmann::json::parse(jsonString);
        return loadFromJSONInternal(json, ConfigSource::File);
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlString) {
    try {
        auto yaml = YAML::Load(yamlString);
        return loadFromYAMLInternal(yaml, ConfigSource::File);
    } catch (const YAML::ParserException& e) {
        return false;
    }
}

bool Config::loadFromFileAuto(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        return loadFromJSON(content);
    } else if (content.find(':') != std::string::npos && 
               (content.find('-') != std::string::npos || content.find('"') != std::string::npos)) {
        return loadFromYAML(content);
    } else {
        std::ifstream simpleFile(filepath);
        if (!simpleFile.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(simpleFile, line)) {
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
        }
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool isJSON) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (isJSON) {
        nlohmann::json json;
        
        for (const auto& entry : pImpl->entries) {
            json[entry.key] = ConfigValueConverter::toJSON(entry.value);
            json[entry.key]["_source"] = static_cast<int>(entry.source);
            json[entry.key]["_description"] = entry.description;
        }
        
        file << json.dump(2);
    } else {
        YAML::Emitter yaml;
        
        for (const auto& entry : pImpl->entries) {
            yaml << YAML::BeginMap;
            yaml << YAML::Key << entry.key;
            yaml << YAML::Value << ConfigValueConverter::toYAML(entry.value);
            yaml << YAML::EndMap;
        }
        
        file << yaml.c_str();
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
    ValidationResult validation = validateValue(key, value);
    if (!validation.isValid) {
        std::ostringstream oss;
        oss << "Configuration validation failed for key \"" << key << "\":";
        for (const auto& error : validation.errors) {
            oss << " " << error;
        }
        throw std::runtime_error(oss.str());
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    pImpl->keyCategories[key] = getKeyCategory(key);
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

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    pImpl->keyCategories.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeys(ConfigCategory category) const {
    std::vector<std::string> keys;
    
    for (const auto& entry : pImpl->entries) {
        const ConfigSchemaEntry* schemaEntry = schema.getEntry(entry.key);
        if (schemaEntry && schemaEntry->category == category) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->keyCategories.clear();
}

std::string Config::getKeyCategory(const std::string& key) const {
    const ConfigSchemaEntry* entry = schema.getEntry(key);
    if (entry) {
        return std::to_string(static_cast<int>(entry->category));
    }
    return "0";
}

ValidationResult Config::validateValue(const std::string& key, const ConfigValue& value) const {
    return schema.validate(key, value);
}

void Config::applyDefaults() {
    for (const auto& entry : schema.getAllEntries()) {
        if (!has(entry.name) && entry.defaultValue.has_value()) {
            set(entry.name, entry.defaultValue.value(), ConfigSource::Default);
        }
    }
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& otherEntry : other.pImpl->entries) {
        if (!has(otherEntry.key) || source == ConfigSource::Inherited) {
            set(otherEntry.key, otherEntry.value, source);
        }
    }
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    std::unordered_map<std::string, std::vector<const ConfigEntry*>> groupedEntries;
    
    for (const auto& entry : pImpl->entries) {
        std::string category = (pImpl->keyCategories.find(entry.key) != pImpl->keyCategories.end()) 
            ? pImpl->keyCategories[entry.key] : "unknown";
        groupedEntries[category].push_back(&entry);
    }
    
    for (const auto& [category, entries] : groupedEntries) {
        oss << "\n  " << category << ":";
        for (const auto& entry : entries) {
            oss << "\n    " << entry->key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry->value);
            oss << "] (" << static_cast<int>(entry->source) << ")";
        }
    }
    
    return oss.str();
}

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

std::string Config::toCamelCase(const std::string& str) {
    std::string result;
    bool capitalizeNext = false;
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '_' || str[i] == '-') {
            capitalizeNext = true;
        } else if (capitalizeNext) {
            result += std::toupper(str[i]);
            capitalizeNext = false;
        } else {
            result += str[i];
        }
    }
    
    return result;
}

std::string Config::toPascalCase(const std::string& str) {
    std::string result;
    bool capitalizeNext = true;
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '_' || str[i] == '-') {
            capitalizeNext = true;
        } else if (capitalizeNext) {
            result += std::toupper(str[i]);
            capitalizeNext = false;
        } else {
            result += str[i];
        }
    }
    
    return result;
}

} // namespace nlm
