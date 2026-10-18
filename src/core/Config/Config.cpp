#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <toml.hpp>

namespace nlm {

// Static members
std::string Config::environment_prefix = "NLM_";

// Implementation details
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::unique_ptr<ConfigSchema>> schemas;
    std::map<std::string, std::unique_ptr<ConfigValidator>> validators;
    std::map<std::string, ConfigMetadata> metadata_store;
    std::map<std::string, std::unique_ptr<ConfigNode>> node_store;
};

// ConfigNode implementations
ConfigNode* ConfigNode::getChild(const std::string& name) {
    auto it = children.find(name);
    return it != children.end() ? it->second.get() : nullptr;
}

const ConfigNode* ConfigNode::getChild(const std::string& name) const {
    auto it = children.find(name);
    return it != children.end() ? it->second.get() : nullptr;
}

ConfigNode* ConfigNode::addChild(const std::string& name) {
    auto child = std::make_unique<ConfigNode>(name);
    ConfigNode* ptr = child.get();
    children[name] = std::move(child);
    return ptr;
}

void ConfigNode::setValue(const std::any& val) {
    value = val;
}

// ConfigSchema implementations
void ConfigSchema::addField(const FieldSchema& field) {
    fields.push_back(field);
}

void ConfigSchema::addSubSchema(std::unique_ptr<ConfigSchema> schema) {
    sub_schemas[schema->name] = std::move(schema);
}

bool ConfigSchema::validate(const Config& config) const {
    for (const auto& field : fields) {
        if (field.required) {
            if (!config.has(field.name)) {
                return false;
            }
        }
        if (config.has(field.name)) {
            auto value = config.getHierarchical(field.name);
            if (!value.has_value()) {
                return false;
            }
            if (field.validator && !field.validator(value.value())) {
                return false;
            }
            if (field.range.has_value()) {
                if (!validateRange(value.value(), field.range)) {
                    return false;
                }
            }
        }
    }
    return true;
}

ConfigValue ConfigSchema::transform(const ConfigValue& value, const std::string& field) const {
    auto it = std::find_if(fields.begin(), fields.end(),
                          [&field](const FieldSchema& f) { return f.name == field; });n    if (it != fields.end() && it->transformer) {
        return it->transformer(value);
    }
    return value;
}

bool ConfigSchema::hasField(const std::string& field_name) const {
    return std::any_of(fields.begin(), fields.end(),
                      [&field_name](const FieldSchema& f) { return f.name == field_name; });
}

// ConfigValidator implementations
void ConfigValidator::addRule(const std::string& field_name,
                             const ValidationRule& rule,
                             const ValidationMessage& error_msg,
                             bool is_range_check) {
    ValidationRuleEntry entry;
    entry.field_name = field_name;
    entry.rule = rule;
    entry.error_message = error_msg;
    entry.is_range_check = is_range_check;
    rules.push_back(entry);
}

bool ConfigValidator::validate(const Config& config) const {
    for (const auto& rule : rules) {
        if (config.has(rule.field_name)) {
            auto value = config.getHierarchical(rule.field_name);
            if (!value.has_value()) {
                continue;
            }
            if (!rule.rule(value.value())) {
                return false;
            }
        }
    }
    return true;
}

std::vector<std::string> ConfigValidator::getValidationErrors(const Config& config) const {
    std::vector<std::string> errors;
    for (const auto& rule : rules) {
        if (config.has(rule.field_name)) {
            auto value = config.getHierarchical(rule.field_name);
            if (!value.has_value()) {
                continue;
            }
            if (!rule.rule(value.value())) {
                if (rule.error_message) {
                    errors.push_back(rule.error_message(value.value()));
                } else {
                    errors.push_back("Validation failed for field: " + rule.field_name);
                }
            }
        }
    }
    return errors;
}

// Config implementations
Config::Config() : pImpl(std::make_unique<Impl>()), root_node(std::make_unique<ConfigNode>()) {
    root_node->name = "root";
    pImpl->node_store["root"] = root_node.get();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::setEnvironmentPrefix(const std::string& prefix) {
    environment_prefix = prefix;
}

std::string Config::getEnvironmentPrefix() {
    return environment_prefix;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::filesystem::path path(filepath);
    if (path.extension() == ".yaml" || path.extension() == ".yml") {
        return loadFromYAML(filepath);
    } else if (path.extension() == ".json") {
        return loadFromJSON(filepath);
    } else if (path.extension() == ".toml") {
        return loadFromTOML(filepath);
    } else {
        // Try simple key=value format
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
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node root = YAML::LoadFile(filepath);
        return parseYAML(YAML::Dump(root), *this);
    } catch (const YAML::Exception& e) {
        throw ConfigFileError(std::string("YAML parse error: ") + e.what());
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        nlohmann::json j;
        file >> j;
        return parseJSON(j.dump(), *this);
    } catch (const std::exception& e) {
        throw ConfigFileError(std::string("JSON parse error: ") + e.what());
    }
}

bool Config::loadFromTOML(const std::string& filepath) {
    try {
        auto data = toml::parse_file(filepath);
        return parseTOML(toml::stringify(data), *this);
    } catch (const std::exception& e) {
        throw ConfigFileError(std::string("TOML parse error: ") + e.what());
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
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::loadFromEnvironment() {
    for (const auto& env : std::filesystem::environment()) {
        std::string key = env.first;
        std::string value = env.second;
        
        if (key.size() > environment_prefix.size() &&
            key.substr(0, environment_prefix.size()) == environment_prefix) {
            std::string config_key = key.substr(environment_prefix.size());
            set(config_key, value, ConfigSource::Environment);
        }
    }
    return true;
}

bool Config::loadFromDefaults() {
    // Load default configuration values
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", 1000LL, ConfigSource::Default);
    set("region_count", 2, ConfigSource::Default);
    set("connection_probability", 0.1, ConfigSource::Default);
    set("plasticity_learning_rate", 0.01, ConfigSource::Default);
    set("random_seed", 42LL, ConfigSource::Default);
    set("enable_plasticity", true, ConfigSource::Default);
    set("environment_name", std::string("GridWorld"), ConfigSource::Default);
    set("log_level", std::string("INFO"), ConfigSource::Default);
    set("max_simulation_steps", 10000LL, ConfigSource::Default);
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool pretty) const {
    std::filesystem::path path(filepath);
    if (path.extension() == ".yaml" || path.extension() == ".yml") {
        return saveToYAML(filepath, pretty);
    } else if (path.extension() == ".json") {
        return saveToJSON(filepath, pretty);
    } else if (path.extension() == ".toml") {
        return saveToTOML(filepath, pretty);
    } else {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]\n";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]\n";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << arg[i] << "\"";
                    }
                    file << "]\n";
                } else {
                    file << entry.key << " = " << arg << "\n";
                }
            }, entry.value);
            file << "\n";
        }
        return true;
    }
}

bool Config::saveToYAML(const std::string& filepath, bool pretty) const {
    YAML::Emitter out;
    if (pretty) {
        out.SetIndent(2);
    }
    
    out << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        out << YAML::Key << entry.key;
        
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                out << YAML::Value << arg;
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, double> ||
                                 std::is_same_v<T, bool>) {
                out << YAML::Value << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                out << YAML::Value << YAML::BeginSeq;
                for (const auto& item : arg) {
                    out << item;
                }
                out << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                out << YAML::Value << YAML::BeginSeq;
                for (const auto& item : arg) {
                    out << item;
                }
                out << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                out << YAML::Value << YAML::BeginSeq;
                for (const auto& item : arg) {
                    out << item;
                }
                out << YAML::EndSeq;
            }
        }, entry.value);
    }
    
    out << YAML::EndMap;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << out.c_str();
    return true;
}

bool Config::saveToJSON(const std::string& filepath, bool pretty) const {
    nlohmann::json j;
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = entry.value;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (pretty) {
        file << j.dump(4);
    } else {
        file << j.dump();
    }
    return true;
}

bool Config::saveToTOML(const std::string& filepath, bool pretty) const {
    toml::value v;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to toml value
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                v[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t>) {
                v[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                v[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                v[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                v[entry.key] = toml::array(arg.begin(), arg.end());
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                v[entry.key] = toml::array(arg.begin(), arg.end());
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                v[entry.key] = toml::array(arg.begin(), arg.end());
            }
        }, entry.value);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << v;
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto normalized_key = normalizeKey(key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalized_key](const ConfigEntry& e) { return normalizeKey(e.key) == normalized_key; });
    
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

template<typename T>
T Config::getRequired(const std::string& key) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        throw ConfigValidationError("Required configuration key not found: " + key);
    }
    return val.value();
}

template<typename T>
std::optional<T> Config::getHierarchical(const std::string& dot_key) const {
    std::vector<std::string> parts;
    splitKey(dot_key, parts);
    
    const ConfigNode* current = root_node.get();
    if (!current) {
        return std::nullopt;
    }
    
    for (const auto& part : parts) {
        current = current->getChild(part);
        if (!current) {
            return std::nullopt;
        }
    }
    
    if (!current->hasValue()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<T>(current->value);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

void Config::set(const std::string& key, const ConfigValue& value,
                 ConfigSource source, const ConfigMetadata& metadata) {
    std::string normalized_key = normalizeKey(key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalized_key](const ConfigEntry& e) { return normalizeKey(e.key) == normalized_key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->metadata = metadata;
    } else {
        pImpl->entries.emplace_back(key, value, source, "", metadata);
        pImpl->metadata_store[normalized_key] = metadata;
        
        // Also update hierarchical structure
        std::vector<std::string> parts;
        splitKey(key, parts);
        ConfigNode* current = root_node.get();
        
        for (size_t i = 0; i < parts.size(); ++i) {
            const std::string& part = parts[i];
            ConfigNode* child = current->getChild(part);
            
            if (!child) {
                child = current->addChild(part);
                pImpl->node_store[part] = child;
            }
            
            current = child;
            
            if (i == parts.size() - 1) {
                current->setValue(value);
            }
        }
    }
}

void Config::set(const std::string& key, const std::string& value,
                 ConfigSource source, const ConfigMetadata& metadata) {
    set(key, ConfigValue(value), source, metadata);
}

void Config::set(const std::string& key, int value,
                 ConfigSource source, const ConfigMetadata& metadata) {
    set(key, ConfigValue(value), source, metadata);
}

void Config::set(const std::string& key, double value,
                 ConfigSource source, const ConfigMetadata& metadata) {
    set(key, ConfigValue(value), source, metadata);
}

void Config::set(const std::string& key, bool value,
                 ConfigSource source, const ConfigMetadata& metadata) {
    set(key, ConfigValue(value), source, metadata);
}

void Config::setHierarchical(const std::string& dot_key, const ConfigValue& value,
                            ConfigSource source, const ConfigMetadata& metadata) {
    std::vector<std::string> parts;
    splitKey(dot_key, parts);
    
    ConfigNode* current = root_node.get();
    if (!current) {
        return;
    }
    
    for (size_t i = 0; i < parts.size(); ++i) {
        const std::string& part = parts[i];
        ConfigNode* child = current->getChild(part);
        
        if (!child) {
            child = current->addChild(part);
            pImpl->node_store[part] = child;
        }
        
        current = child;
        
        if (i == parts.size() - 1) {
            current->setValue(value);
        }
    }
    
    set(dot_key, value, source, metadata);
}

const ConfigMetadata& Config::getMetadata(const std::string& key) const {
    std::string normalized_key = normalizeKey(key);
    auto it = pImpl->metadata_store.find(normalized_key);
    static const ConfigMetadata empty_metadata;
    return it != pImpl->metadata_store.end() ? it->second : empty_metadata;
}

void Config::setMetadata(const std::string& key, const ConfigMetadata& metadata) {
    std::string normalized_key = normalizeKey(key);
    pImpl->metadata_store[normalized_key] = metadata;
}

bool Config::validate(const ConfigSchema& schema) const {
    return schema.validate(*this);
}

bool Config::validate(const ConfigValidator& validator) const {
    return validator.validate(*this);
}

std::vector<std::string> Config::getValidationErrors(const ConfigSchema& schema) const {
    std::vector<std::string> errors;
    // Schema validation errors would need to be tracked
    return errors;
}

std::vector<std::string> Config::getValidationErrors(const ConfigValidator& validator) const {
    return validator.getValidationErrors(*this);
}

bool Config::has(const std::string& key) const {
    std::string normalized_key = normalizeKey(key);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalized_key](const ConfigEntry& e) { return normalizeKey(e.key) == normalized_key; });
}

bool Config::hasHierarchical(const std::string& dot_key) const {
    std::vector<std::string> parts;
    splitKey(dot_key, parts);
    
    const ConfigNode* current = root_node.get();
    if (!current) {
        return false;
    }
    
    for (const auto& part : parts) {
        current = current->getChild(part);
        if (!current) {
            return false;
        }
    }
    
    return current && current->hasValue();
}

void Config::remove(const std::string& key) {
    std::string normalized_key = normalizeKey(key);
    
    // Remove from entries
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&normalized_key](const ConfigEntry& e) { return normalizeKey(e.key) == normalized_key; }),
        pImpl->entries.end()
    );
    
    pImpl->metadata_store.erase(normalized_key);
    
    // Remove from hierarchical structure
    std::vector<std::string> parts;
    splitKey(key, parts);
    
    ConfigNode* current = root_node.get();
    if (!current) {
        return;
    }
    
    for (size_t i = 0; i < parts.size() - 1 && current; ++i) {
        const std::string& part = parts[i];
        ConfigNode* child = current->getChild(part);
        if (child) {
            current = child;
        } else {
            current = nullptr;
            break;
        }
    }
    
    if (current) {
        const std::string& last_part = parts.back();
        if (current->hasChild(last_part)) {
            current->children.erase(last_part);
            pImpl->node_store.erase(last_part);
        }
    }
}

void Config::removeHierarchical(const std::string& dot_key) {
    remove(dot_key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getHierarchicalKeys() const {
    std::vector<std::string> keys;
    
    std::function<void(ConfigNode*, std::string)> collect_keys = [&](ConfigNode* node, std::string prefix) {
        if (node->hasValue()) {
            if (!prefix.empty()) {
                keys.push_back(prefix);
            }
        }
        
        for (const auto& child_pair : node->children) {
            collect_keys(child_pair.second.get(), 
                        prefix.empty() ? child_pair.first : prefix + "." + child_pair.first);
        }
    };
    
    collect_keys(root_node.get(), "");
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->metadata_store.clear();
    root_node = std::make_unique<ConfigNode>();
    root_node->name = "root";
    pImpl->node_store.clear();
    pImpl->node_store["root"] = root_node.get();
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

std::string Config::summaryHierarchical() const {
    std::ostringstream oss;
    oss << "Hierarchical Configuration:\n";
    
    std::function<void(ConfigNode*, std::string, int)> print_node = [&](ConfigNode* node, std::string prefix, int level) {
        if (node->hasValue()) {
            oss << std::string(level * 2, ' ') << prefix << ": ";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, node->value);
            oss << "\n";
        }
        
        for (const auto& child_pair : node->children) {
            print_node(child_pair.second.get(),
                      child_pair.first,
                      level + 1);
        }
    };
    
    print_node(root_node.get(), "", 0);
    return oss.str();
}

void Config::merge(const Config& other, bool override_source) {
    for (const auto& other_entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&other_entry](const ConfigEntry& e) { return e.key == other_entry.key; });
        
        if (it != pImpl->entries.end()) {
            if (override_source) {
                it->value = other_entry.value;
                it->source = other_entry.source;
                it->metadata = other_entry.metadata;
            } else if (it->source == ConfigSource::Default) {
                it->value = other_entry.value;
                it->source = other_entry.source;
            }
        } else {
            pImpl->entries.push_back(other_entry);
            pImpl->metadata_store[other_entry.key] = other_entry.metadata;
        }
    }
}

Config Config::diff(const Config& other) const {
    Config diff_config;
    
    // Find entries in this config but not in other
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& other_entry : other.pImpl->entries) {
            if (entry.key == other_entry.key) {
                found = true;
                if (entry.value != other_entry.value) {
                    diff_config.set(entry.key, entry.value, entry.source);
                }
                break;
            }
        }
        if (!found) {
            diff_config.set(entry.key, entry.value, entry.source);
        }
    }
    
    // Find entries in other but not in this
    for (const auto& other_entry : other.pImpl->entries) {
        bool found = false;
        for (const auto& entry : pImpl->entries) {
            if (entry.key == other_entry.key) {
                found = true;
                break;
            }
        }
        if (!found) {
            diff_config.set(other_entry.key, other_entry.value, other_entry.source);
        }
    }
    
    return diff_config;
}

bool Config::isSubset(const Config& other) const {
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& other_entry : other.pImpl->entries) {
            if (entry.key == other_entry.key) {
                found = true;
                // Values don't need to be equal for subset check
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

void Config::addSchema(const std::string& name, std::unique_ptr<ConfigSchema> schema) {
    pImpl->schemas[name] = std::move(schema);
}

ConfigSchema* Config::getSchema(const std::string& name) {
    auto it = pImpl->schemas.find(name);
    return it != pImpl->schemas.end() ? it->second.get() : nullptr;
}

const ConfigSchema* Config::getSchema(const std::string& name) const {
    auto it = pImpl->schemas.find(name);
    return it != pImpl->schemas.end() ? it->second.get() : nullptr;
}

void Config::addValidator(const std::string& name, std::unique_ptr<ConfigValidator> validator) {
    pImpl->validators[name] = std::move(validator);
}

ConfigValidator* Config::getValidator(const std::string& name) {
    auto it = pImpl->validators.find(name);
    return it != pImpl->validators.end() ? it->second.get() : nullptr;
}

const ConfigValidator* Config::getValidator(const std::string& name) const {
    auto it = pImpl->validators.find(name);
    return it != pImpl->validators.end() ? it->second.get() : nullptr;
}

// Static helper methods
bool Config::parseYAML(const std::string& content, Config& config) {
    try {
        YAML::Node root = YAML::Load(content);
        for (const auto& pair : root) {
            std::string key = pair.first.as<std::string>();
            ConfigValue value;
            
            if (pair.second.IsScalar()) {
                if (pair.second.IsSequence()) {
                    std::vector<int> vec;
                    for (const auto& item : pair.second) {
                        vec.push_back(item.as<int>());
                    }
                    value = vec;
                } else if (pair.second.IsScalar()) {
                    std::string str_val = pair.second.as<std::string>();
                    
                    // Try to infer type
                    try {
                        int int_val = std::stoi(str_val);
                        value = int_val;
                    } catch (...) {
                        try {
                            double double_val = std::stod(str_val);
                            value = double_val;
                        } catch (...) {
                            try {
                                if (str_val == "true" || str_val == "false") {
                                    bool bool_val = (str_val == "true");
                                    value = bool_val;
                                } else {
                                    value = str_val;
                                }
                            } catch (...) {
                                value = str_val;
                            }
                        }
                    }
                }
            }
            
            config.set(key, value, ConfigSource::File);
        }
        return true;
    } catch (const YAML::Exception& e) {
        throw ConfigParseError(std::string("YAML parse error: ") + e.what());
    }
}

bool Config::parseJSON(const std::string& content, Config& config) {
    try {
        nlohmann::json j = nlohmann::json::parse(content);
        for (const auto& pair : j.items()) {
            std::string key = pair.key();
            ConfigValue value;
            
            if (pair.value().is_array()) {
                if (pair.value().empty()) {
                    value = std::vector<std::string>{};
                } else if (pair.value()[0].is_number_integer()) {
                    std::vector<int> vec;
                    for (const auto& item : pair.value()) {
                        vec.push_back(item.get<int>());
                    }
                    value = vec;
                } else if (pair.value()[0].is_number_float()) {
                    std::vector<double> vec;
                    for (const auto& item : pair.value()) {
                        vec.push_back(item.get<double>());
                    }
                    value = vec;
                } else {
                    std::vector<std::string> vec;
                    for (const auto& item : pair.value()) {
                        vec.push_back(item.get<std::string>());
                    }
                    value = vec;
                }
            } else if (pair.value().is_boolean()) {
                value = pair.value().get<bool>();
            } else if (pair.value().is_number_integer()) {
                value = pair.value().get<int64_t>();
            } else if (pair.value().is_number_float()) {
                value = pair.value().get<double>();
            } else {
                value = pair.value().get<std::string>();
            }
            
            config.set(key, value, ConfigSource::File);
        }
        return true;
    } catch (const std::exception& e) {
        throw ConfigParseError(std::string("JSON parse error: ") + e.what());
    }
}

bool Config::parseTOML(const std::string& content, Config& config) {
    try {
        auto data = toml::parse(content);
        for (const auto& pair : data) {
            std::string key = pair.first.as<std::string>();
            ConfigValue value;
            
            if (auto* p = std::get_if<toml::integer>(&pair.second)) {
                value = static_cast<int64_t>(*p);
            } else if (auto* p = std::get_if<toml::floating_point>(&pair.second)) {
                value = *p;
            } else if (auto* p = std::get_if<toml::boolean>(&pair.second)) {
                value = *p;
            } else if (auto* p = std::get_if<toml::string>(&pair.second)) {
                value = *p;
            } else if (auto* p = std::get_if<toml::array>(&pair.second)) {
                // Convert array to appropriate vector type
                std::vector<std::string> str_vec;
                for (const auto& item : **p) {
                    if (auto* s = std::get_if<std::string>(&item)) {
                        str_vec.push_back(*s);
                    } else if (auto* i = std::get_if<toml::integer>(&item)) {
                        str_vec.push_back(std::to_string(static_cast<int64_t>(*i)));
                    } else if (auto* f = std::get_if<toml::floating_point>(&item)) {
                        str_vec.push_back(std::to_string(*f));
                    } else if (auto* b = std::get_if<toml::boolean>(&item)) {
                        str_vec.push_back(*b ? "true" : "false");
                    }
                }
                value = str_vec;
            }
            
            config.set(key, value, ConfigSource::File);
        }
        return true;
    } catch (const std::exception& e) {
        throw ConfigParseError(std::string("TOML parse error: ") + e.what());
    }
}

std::string Config::getEnvVarName(const std::string& key) {
    return environment_prefix + key;
}

std::string Config::convertEnvVarName(const std::string& key) {
    std::string result;
    bool capitalize_next = false;
    
    for (char c : key) {
        if (c == '_' || c == '-') {
            capitalize_next = true;
        } else if (capitalize_next) {
            result += std::toupper(c);
            capitalize_next = false;
        } else {
            result += c;
        }
    }
    
    return environment_prefix + result;
}

void Config::splitKey(const std::string& dot_key, std::vector<std::string>& parts) {
    std::stringstream ss(dot_key);
    std::string part;
    while (std::getline(ss, part, '.')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
}

std::string Config::joinKey(const std::vector<std::string>& parts) {
    std::string result;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            result += ".";
        }
        result += parts[i];
    }
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

std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string Config::snakeToCamel(const std::string& s) {
    std::string result;
    bool capitalize_next = true;
    
    for (char c : s) {
        if (c == '_') {
            capitalize_next = true;
        } else if (capitalize_next) {
            result += std::toupper(c);
            capitalize_next = false;
        } else {
            result += c;
        }
    }
    
    return result;
}

std::string Config::camelToSnake(const std::string& s) {
    std::string result;
    for (size_t i = 0; i < s.size(); ++i) {
        if (i > 0 && std::isupper(s[i])) {
            result += '_';
        }
        result += std::tolower(s[i]);
    }
    return result;
}

std::string Config::normalizeKey(const std::string& key) {
    return toLower(key);
}

// Explicit template instantiations
// Config.hpp template instantiations

template std::optional<int> Config::get<int>(const std::string&) const;
template int Config::getOr<int>(const std::string&, const int&) const;
template int Config::getRequired<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template int64_t Config::getRequired<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template double Config::getRequired<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template bool Config::getRequired<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
template std::string Config::getRequired<std::string>(const std::string&) const;

} // namespace nlm