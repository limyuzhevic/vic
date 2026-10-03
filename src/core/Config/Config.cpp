#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <yaml-cpp/yaml.h>
#include <toml.hpp>
#include <iostream>
#include <map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigConstraint> schema;
    std::map<std::string, std::vector<std::string>> keysBySource;
    std::string lastError;
    
    // Default configuration schema
    void initializeDefaultSchema() {
        // Basic NLM configuration parameters
        addSchema(ConfigConstraint("random_seed", ConfigType::Integer, "Random seed for reproducible results"));
        addSchema(ConfigConstraint("simulation_timestep", ConfigType::Float, "Time step for simulation (seconds)"));
        addSchema(ConfigConstraint("neuron_count", ConfigType::Integer, "Number of neurons in the network"));
        addSchema(ConfigConstraint("region_count", ConfigType::Integer, "Number of neural regions"));
        addSchema(ConfigConstraint("connection_probability", ConfigType::Float, "Probability of creating a connection between neurons"));
        
        // STDP parameters
        addSchema(ConfigConstraint("stdp_ltp_weight", ConfigType::Float, "STDP long-term potentiation weight"));
        addSchema(ConfigConstraint("stdp_ltd_weight", ConfigType::Float, "STDP long-term depression weight"));
        addSchema(ConfigConstraint("stdp_tau", ConfigType::Float, "STDP time constant (ms)"));
        
        // Structural plasticity parameters
        addSchema(ConfigConstraint("synaptogenesis_rate", ConfigType::Float, "Rate of new synapse formation"));
        addSchema(ConfigConstraint("pruning_rate", ConfigType::Float, "Rate of synapse pruning"));
        
        // Memory system parameters
        addSchema(ConfigConstraint("working_memory_capacity", ConfigType::Integer, "Capacity of working memory"));
        addSchema(ConfigConstraint("episodic_memory_max_episodes", ConfigType::Integer, "Maximum number of episodic memory episodes"));
        
        // Prediction system parameters
        addSchema(ConfigConstraint("prediction_confidence_threshold", ConfigType::Float, "Confidence threshold for predictions"));
        
        // Cognitive system parameters
        addSchema(ConfigConstraint("planning_depth", ConfigType::Integer, "Depth of planning horizon"));
        addSchema(ConfigConstraint("attention_inhibition_strength", ConfigType::Float, "Strength of attentional inhibition"));
        addSchema(ConfigConstraint("attention_excitation_strength", ConfigType::Float, "Strength of attentional excitation"));
        
        // Neuromodulation parameters
        addSchema(ConfigConstraint("novelty_threshold", ConfigType::Float, "Novelty detection threshold"));
        addSchema(ConfigConstraint("curiosity_gain", ConfigType::Float, "Curiosity signal gain"));
        
        // Performance parameters
        addSchema(ConfigConstraint("checkpoint_interval", ConfigType::Integer, "Simulation steps between checkpoints"));
        addSchema(ConfigConstraint("replay_interval", ConfigType::Integer, "Simulation steps between memory replay"));
        addSchema(ConfigConstraint("consolidation_interval", ConfigType::Integer, "Simulation steps between memory consolidation"));
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->initializeDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    ConfigFileFormat format = detectFileFormat(filepath);
    
    switch (format) {
        case ConfigFileFormat::SimpleKeyValue:
            return loadFromSimpleFile(filepath);
        case ConfigFileFormat::JSON:
            return loadFromJSON(filepath);
        case ConfigFileFormat::YAML:
            return loadFromYAML(filepath);
        case ConfigFileFormat::TOML:
            return loadFromTOML(filepath);
        case ConfigFileFormat::Unknown:
        default:
            // Try simple format as fallback
            return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open JSON file: " + filepath;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    rapidjson::Document doc;
    rapidjson::ParseResult result = doc.Parse(buffer.str().c_str());
    
    if (!result) {
        pImpl->lastError = "Invalid JSON format in file: " + filepath;
        return false;
    }
    
    // Clear existing entries
    clear();
    
    // Parse JSON object
    if (doc.IsObject()) {
        for (rapidjson::Value::MemberIterator it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
            std::string key = it->name.GetString();
            ConfigValue value;
            
            if (it->value.IsString()) {
                value = std::string(it->value.GetString());
            } else if (it->value.IsInt()) {
                value = it->value.GetInt();
            } else if (it->value.IsInt64()) {
                value = it->value.GetInt64();
            } else if (it->value.IsDouble()) {
                value = it->value.GetDouble();
            } else if (it->value.IsBool()) {
                value = it->value.GetBool();
            } else if (it->value.IsArray()) {
                // Handle arrays
                const rapidjson::Value& array = it->value;
                if (array.Empty()) {
                    continue;
                }
                
                // Determine array type from first element
                if (array[0].IsInt()) {
                    std::vector<int> vec;
                    vec.reserve(array.Size());
                    for (rapidjson::SizeType i = 0; i < array.Size(); ++i) {
                        vec.push_back(array[i].GetInt());
                    }
                    value = vec;
                } else if (array[0].IsDouble()) {
                    std::vector<double> vec;
                    vec.reserve(array.Size());
                    for (rapidjson::SizeType i = 0; i < array.Size(); ++i) {
                        vec.push_back(array[i].GetDouble());
                    }
                    value = vec;
                } else if (array[0].IsString()) {
                    std::vector<std::string> vec;
                    vec.reserve(array.Size());
                    for (rapidjson::SizeType i = 0; i < array.Size(); ++i) {
                        vec.push_back(array[i].GetString());
                    }
                    value = vec;
                }
            }
            
            if (value.index() != std::variant_npos) {
                set(key, value, ConfigSource::File, "Loaded from JSON file");
            }
        }
    }
    
    return true;
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open YAML file: " + filepath;
        return false;
    }
    
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        // Clear existing entries
        clear();
        
        if (doc.IsMap()) {
            for (const auto& pair : doc) {
                std::string key = pair.first.as<std::string>();
                ConfigValue value;
                
                if (pair.second.IsScalar()) {
                    const YAML::Node& node = pair.second;
                    if (node.IsBool()) {
                        value = node.as<bool>();
                    } else if (node.IsInt()) {
                        value = node.as<int>();
                    } else if (node.IsFloat()) {
                        value = node.as<double>();
                    } else if (node.IsString()) {
                        value = node.as<std::string>();
                    }
                } else if (pair.second.IsSequence()) {
                    // Handle arrays
                    const YAML::Node& array = pair.second;
                    if (array.empty()) {
                        continue;
                    }
                    
                    // Determine array type from first element
                    if (array[0].IsInt()) {
                        std::vector<int> vec;
                        for (const auto& item : array) {
                            vec.push_back(item.as<int>());
                        }
                        value = vec;
                    } else if (array[0].IsFloat()) {
                        std::vector<double> vec;
                        for (const auto& item : array) {
                            vec.push_back(item.as<double>());
                        }
                        value = vec;
                    } else if (array[0].IsString()) {
                        std::vector<std::string> vec;
                        for (const auto& item : array) {
                            vec.push_back(item.as<std::string>());
                        }
                        value = vec;
                    }
                }
                
                if (value.index() != std::variant_npos) {
                    set(key, value, ConfigSource::File, "Loaded from YAML file");
                }
            }
        }
    } catch (const std::exception& e) {
        pImpl->lastError = "Error parsing YAML file " + filepath + ": " + e.what();
        return false;
    }
    
    return true;
}

bool Config::loadFromTOML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open TOML file: " + filepath;
        return false;
    }
    
    try {
        auto doc = toml::parse_file(filepath);
        
        // Clear existing entries
        clear();
        
        for (const auto& [key, value] : doc) {
            ConfigValue configValue;
            
            if (std::holds_alternative<toml::integer>(value)) {
                configValue = std::get<toml::integer>(value);
            } else if (std::holds_alternative<toml::floating_point>(value)) {
                configValue = std::get<toml::floating_point>(value);
            } else if (std::holds_alternative<toml::boolean>(value)) {
                configValue = std::get<toml::boolean>(value);
            } else if (std::holds_alternative<toml::string>(value)) {
                configValue = std::get<toml::string>(value);
            } else if (std::holds_alternative<toml::array_of<int>>(value)) {
                std::vector<int> vec;
                for (const auto& item : std::get<toml::array_of<int>>(value)) {
                    vec.push_back(item);
                }
                configValue = vec;
            } else if (std::holds_alternative<toml::array_of<double>>(value)) {
                std::vector<double> vec;
                for (const auto& item : std::get<toml::array_of<double>>(value)) {
                    vec.push_back(item);
                }
                configValue = vec;
            } else if (std::holds_alternative<toml::array_of<std::string>>(value)) {
                std::vector<std::string> vec;
                for (const auto& item : std::get<toml::array_of<std::string>>(value)) {
                    vec.push_back(item);
                }
                configValue = vec;
            }
            
            if (configValue.index() != std::variant_npos) {
                set(key, configValue, ConfigSource::File, "Loaded from TOML file");
            }
        }
    } catch (const std::exception& e) {
        pImpl->lastError = "Error parsing TOML file " + filepath + ": " + e.what();
        return false;
    }
    
    return true;
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open config file: " + filepath;
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == '!') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            pImpl->lastError = "Invalid line format in config file " + filepath + ": " + line;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Convert value to appropriate type
        ConfigValue configValue;
        bool parsed = false;
        
        // Try to determine type
        if (value == "true" || value == "false") {
            configValue = (value == "true");
            parsed = true;
        } else {
            // Try integer
            try {
                size_t posInt;
                long long intValue = std::stoll(value, &posInt);
                if (posInt == value.size()) {
                    configValue = static_cast<int>(intValue);
                    parsed = true;
                }
            } catch (...) {}
            
            // Try float
            if (!parsed) {
                try {
                    size_t posFloat;
                    double floatValue = std::stod(value, &posFloat);
                    if (posFloat == value.size()) {
                        configValue = floatValue;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Treat as string
            if (!parsed) {
                configValue = value;
                parsed = true;
            }
        }
        
        if (parsed) {
            set(key, configValue, ConfigSource::File, "Loaded from config file", filepath, lineNumber);
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
                
                // Convert value to appropriate type
                ConfigValue configValue;
                bool parsed = false;
                
                if (value == "true" || value == "false") {
                    configValue = (value == "true");
                    parsed = true;
                } else {
                    // Try integer
                    try {
                        size_t posInt;
                        long long intValue = std::stoll(value, &posInt);
                        if (posInt == value.size()) {
                            configValue = static_cast<int>(intValue);
                            parsed = true;
                        }
                    } catch (...) {}
                    
                    // Try float
                    if (!parsed) {
                        try {
                            size_t posFloat;
                            double floatValue = std::stod(value, &posFloat);
                            if (posFloat == value.size()) {
                                configValue = floatValue;
                                parsed = true;
                            }
                        } catch (...) {}
                    }
                    
                    // Treat as string
                    if (!parsed) {
                        configValue = value;
                        parsed = true;
                    }
                }
                
                if (parsed) {
                    set(key, configValue, ConfigSource::CommandLine, "Command line argument");
                }
            }
        }
        // Handle -key value format (simplified - currently only supports --key=value)
        else if (arg[0] == '-' && arg.size() > 1) {
            // Simple boolean flag
            std::string key = arg.substr(1);
            set(key, true, ConfigSource::CommandLine, "Command line flag");
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath, ConfigFileFormat format) const {
    switch (format) {
        case ConfigFileFormat::SimpleKeyValue:
            return saveToSimpleFile(filepath);
        case ConfigFileFormat::JSON:
            return saveToJSON(filepath);
        case ConfigFileFormat::YAML:
            return saveToYAML(filepath);
        case ConfigFileFormat::TOML:
            return saveToTOML(filepath);
        default:
            return saveToSimpleFile(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    
    writer.StartObject();
    
    for (const auto& entry : pImpl->entries) {
        writer.Key(entry.key.c_str());
        
        const ConfigValue& value = entry.value;
        if (std::holds_alternative<int>(value)) {
            writer.Int(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            writer.Int64(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            writer.Double(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            writer.Bool(std::get<bool>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            writer.String(std::get<std::string>(value).c_str());
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            writer.StartArray();
            for (int item : std::get<std::vector<int>>(value)) {
                writer.Int(item);
            }
            writer.EndArray();
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            writer.StartArray();
            for (double item : std::get<std::vector<double>>(value)) {
                writer.Double(item);
            }
            writer.EndArray();
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            writer.StartArray();
            for (const std::string& item : std::get<std::vector<std::string>>(value)) {
                writer.String(item.c_str());
            }
            writer.EndArray();
        }
    }
    
    writer.EndObject();
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << buffer.GetString();
    return true;
}

bool Config::saveToYAML(const std::string& filepath) const {
    YAML::Emitter emitter;
    emitter.SetIndent(2);
    emitter.SetMapFormat(YAML::EmitterStyle::Flow);
    
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        
        const ConfigValue& value = entry.value;
        if (std::holds_alternative<int>(value)) {
            emitter << std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            emitter << static_cast<int>(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            emitter << std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            emitter << std::get<bool>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            emitter << std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            emitter << YAML::BeginSeq;
            for (int item : std::get<std::vector<int>>(value)) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            emitter << YAML::BeginSeq;
            for (double item : std::get<std::vector<double>>(value)) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            emitter << YAML::BeginSeq;
            for (const std::string& item : std::get<std::vector<std::string>>(value)) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        }
    }
    
    emitter << YAML::EndMap;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << emitter.c_str();
    return true;
}

bool Config::saveToTOML(const std::string& filepath) const {
    toml::table doc;
    
    for (const auto& entry : pImpl->entries) {
        if (std::holds_alternative<int>(entry.value)) {
            doc[entry.key] = std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            doc[entry.key] = std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            doc[entry.key] = std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            doc[entry.key] = std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            doc[entry.key] = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            std::vector<toml::value> vec;
            for (int item : std::get<std::vector<int>>(entry.value)) {
                vec.push_back(item);
            }
            doc[entry.key] = vec;
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            std::vector<toml::value> vec;
            for (double item : std::get<std::vector<double>>(entry.value)) {
                vec.push_back(item);
            }
            doc[entry.key] = vec;
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            std::vector<toml::value> vec;
            for (const std::string& item : std::get<std::vector<std::string>>(entry.value)) {
                vec.push_back(item);
            }
            doc[entry.key] = vec;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << doc;
    return true;
}

bool Config::saveToSimpleFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << configValueToString(entry.value) << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source,
                 const std::string& description, const std::string& filepath, int lineNumber) {
    // Validate against schema if available
    for (const auto& constraint : pImpl->schema) {
        if (constraint.key == key) {
            validateConstraint(constraint, value);
            break;
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->filepath = filepath;
        it->lineNumber = lineNumber;
    } else {
        pImpl->entries.emplace_back(key, value, source, description, filepath, lineNumber);
    }
    
    pImpl->keysBySource[source].push_back(key);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source,
                 const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, int value, ConfigSource source,
                 const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, double value, ConfigSource source,
                 const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, bool value, ConfigSource source,
                 const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::addSchema(const ConfigConstraint& constraint) {
    pImpl->schema.push_back(constraint);
}

bool Config::validateSchema() const {
    std::vector<ConfigValidationError> errors = getSchemaValidationErrors();
    return errors.empty();
}

std::vector<ConfigValidationError> Config::getSchemaValidationErrors() const {
    std::vector<ConfigValidationError> errors;
    
    for (const auto& constraint : pImpl->schema) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&constraint](const ConfigEntry& e) { return e.key == constraint.key; });n
        
        if (it == pImpl->entries.end()) {
            // Missing required field (if constraint specifies)
            if (constraint.defaultValue.has_value()) {
                continue; // Has default, so OK
            } else {
                errors.emplace_back("Missing required configuration key: " + constraint.key, constraint.key);
            }
        } else {
            // Validate existing field
            if (!validateConstraint(constraint, it->value)) {
                errors.emplace_back("Configuration validation failed for key: " + constraint.key, constraint.key);
            }
        }
    }
    
    return errors;
}

bool Config::validateConstraint(const ConfigConstraint& constraint, const ConfigValue& value) const {
    if (!constraint.validate(value)) {
        return false;
    }
    
    if (!constraint.checkRange(value)) {
        return false;
    }
    
    if (!constraint.checkAllowedValues(value)) {
        return false;
    }
    
    return true;
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
    
    // Remove from keysBySource
    for (auto& [source, keys] : pImpl->keysBySource) {
        keys.erase(std::remove(keys.begin(), keys.end(), key), keys.end());
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<ConfigEntry> Config::getEntries() const {
    return pImpl->entries;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->keysBySource.clear();
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
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.filepath.empty()) {
            oss << " from " << entry.filepath;
            if (entry.lineNumber >= 0) {
                oss << ":" << entry.lineNumber;
            }
        }
        oss << "\n";
    }
    return oss.str();
}

std::string Config::getStatistics() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    oss << "  Sources:\n";
    for (const auto& [source, keys] : pImpl->keysBySource) {
        oss << "    " << static_cast<int>(source) << ": " << keys.size() << " entries\n";
    }
    oss << "  Schema constraints: " << pImpl->schema.size() << "\n";
    oss << "  Validation errors: " << getSchemaValidationErrors().size() << "\n";
    
    return oss.str();
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Don't overwrite existing entries with higher priority sources
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });n
        
        if (it == pImpl->entries.end() || static_cast<int>(source) > static_cast<int>(it->source)) {
            pImpl->entries.push_back(entry);
            pImpl->entries.back().source = source;
            pImpl->keysBySource[source].push_back(entry.key);
        }
    }
}

ConfigFileFormat Config::detectFileFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return ConfigFileFormat::Unknown;
    }
    
    std::string firstLine;
    std::getline(file, firstLine);
    
    // Check for common file format signatures
    if (firstLine.find('{') != std::string::npos && firstLine.find('}') != std::string::npos) {
        return ConfigFileFormat::JSON;
    }
    
    if (firstLine.find('[') != std::string::npos && firstLine.find(']') != std::string::npos) {
        return ConfigFileFormat::YAML;
    }
    
    if (firstLine.find("[[") != std::string::npos && firstLine.find("]] = ") != std::string::npos) {
        return ConfigFileFormat::TOML;
    }
    
    if (firstLine.find('=') != std::string::npos) {
        return ConfigFileFormat::SimpleKeyValue;
    }
    
    return ConfigFileFormat::Unknown;
}

std::map<ConfigSource, std::vector<std::string>> Config::getKeysBySource() const {
    return pImpl->keysBySource;
}

std::string Config::exportToJSON() const {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    
    writer.StartObject();
    
    for (const auto& entry : pImpl->entries) {
        writer.Key(entry.key.c_str());
        
        const ConfigValue& value = entry.value;
        if (std::holds_alternative<int>(value)) {
            writer.Int(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            writer.Int64(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            writer.Double(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            writer.Bool(std::get<bool>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            writer.String(std::get<std::string>(value).c_str());
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            writer.StartArray();
            for (int item : std::get<std::vector<int>>(value)) {
                writer.Int(item);
            }
            writer.EndArray();
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            writer.StartArray();
            for (double item : std::get<std::vector<double>>(value)) {
                writer.Double(item);
            }
            writer.EndArray();
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            writer.StartArray();
            for (const std::string& item : std::get<std::vector<std::string>>(value)) {
                writer.String(item.c_str());
            }
            writer.EndArray();
        }
    }
    
    writer.EndObject();
    
    return buffer.GetString();
}

std::string Config::exportToYAML() const {
    YAML::Emitter emitter;
    emitter.SetIndent(2);
    emitter.SetMapFormat(YAML::EmitterStyle::Flow);
    
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        
        const ConfigValue& value = entry.value;
        if (std::holds_alternative<int>(value)) {
            emitter << std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            emitter << static_cast<int>(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            emitter << std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            emitter << std::get<bool>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            emitter << std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            emitter << YAML::BeginSeq;
            for (int item : std::get<std::vector<int>>(value)) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            emitter << YAML::BeginSeq;
            for (double item : std::get<std::vector<double>>(value)) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            emitter << YAML::BeginSeq;
            for (const std::string& item : std::get<std::vector<std::string>>(value)) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        }
    }
    
    emitter << YAML::EndMap;
    
    return emitter.c_str();
}

std::string Config::exportToTOML() const {
    toml::table doc;
    
    for (const auto& entry : pImpl->entries) {
        if (std::holds_alternative<int>(entry.value)) {
            doc[entry.key] = std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            doc[entry.key] = std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            doc[entry.key] = std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            doc[entry.key] = std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            doc[entry.key] = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            std::vector<toml::value> vec;
            for (int item : std::get<std::vector<int>>(entry.value)) {
                vec.push_back(item);
            }
            doc[entry.key] = vec;
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            std::vector<toml::value> vec;
            for (double item : std::get<std::vector<double>>(entry.value)) {
                vec.push_back(item);
            }
            doc[entry.key] = vec;
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            std::vector<toml::value> vec;
            for (const std::string& item : std::get<std::vector<std::string>>(entry.value)) {
                vec.push_back(item);
            }
            doc[entry.key] = vec;
        }
    }
    
    std::stringstream buffer;
    buffer << doc;
    return buffer.str();
}

std::string Config::exportToINI() const {
    std::ostringstream oss;
    
    for (const auto& entry : pImpl->entries) {
        oss << "[" << entry.key << "]\n";
        oss << "value = " << configValueToString(entry.value) << "\n\n";
    }
    
    return oss.str();
}

bool Config::importFromJSON(const std::string& json) {
    try {
        rapidjson::Document doc;
        rapidjson::ParseResult result = doc.Parse(json.c_str());
        
        if (!result) {
            return false;
        }
        
        clear();
        
        if (doc.IsObject()) {
            for (rapidjson::Value::MemberIterator it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
                std::string key = it->name.GetString();
                ConfigValue value;
                
                if (it->value.IsString()) {
                    value = std::string(it->value.GetString());
                } else if (it->value.IsInt()) {
                    value = it->value.GetInt();
                } else if (it->value.IsInt64()) {
                    value = it->value.GetInt64();
                } else if (it->value.IsDouble()) {
                    value = it->value.GetDouble();
                } else if (it->value.IsBool()) {
                    value = it->value.GetBool();
                } else if (it->value.IsArray()) {
                    const rapidjson::Value& array = it->value;
                    if (array.Empty()) {
                        continue;
                    }
                    
                    if (array[0].IsInt()) {
                        std::vector<int> vec;
                        vec.reserve(array.Size());
                        for (rapidjson::SizeType i = 0; i < array.Size(); ++i) {
                            vec.push_back(array[i].GetInt());
                        }
                        value = vec;
                    } else if (array[0].IsDouble()) {
                        std::vector<double> vec;
                        vec.reserve(array.Size());
                        for (rapidjson::SizeType i = 0; i < array.Size(); ++i) {
                            vec.push_back(array[i].GetDouble());
                        }
                        value = vec;
                    } else if (array[0].IsString()) {
                        std::vector<std::string> vec;
                        vec.reserve(array.Size());
                        for (rapidjson::SizeType i = 0; i < array.Size(); ++i) {
                            vec.push_back(array[i].GetString());
                        }
                        value = vec;
                    }
                }
                
                if (value.index() != std::variant_npos) {
                    set(key, value, ConfigSource::Runtime, "Imported from JSON");
                }
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::importFromYAML(const std::string& yaml) {
    try {
        YAML::Node doc = YAML::Load(yaml);
        
        clear();
        
        if (doc.IsMap()) {
            for (const auto& pair : doc) {
                std::string key = pair.first.as<std::string>();
                ConfigValue value;
                
                if (pair.second.IsScalar()) {
                    const YAML::Node& node = pair.second;
                    if (node.IsBool()) {
                        value = node.as<bool>();
                    } else if (node.IsInt()) {
                        value = node.as<int>();
                    } else if (node.IsFloat()) {
                        value = node.as<double>();
                    } else if (node.IsString()) {
                        value = node.as<std::string>();
                    }
                } else if (pair.second.IsSequence()) {
                    const YAML::Node& array = pair.second;
                    if (array.empty()) {
                        continue;
                    }
                    
                    if (array[0].IsInt()) {
                        std::vector<int> vec;
                        for (const auto& item : array) {
                            vec.push_back(item.as<int>());
                        }
                        value = vec;
                    } else if (array[0].IsFloat()) {
                        std::vector<double> vec;
                        for (const auto& item : array) {
                            vec.push_back(item.as<double>());
                        }
                        value = vec;
                    } else if (array[0].IsString()) {
                        std::vector<std::string> vec;
                        for (const auto& item : array) {
                            vec.push_back(item.as<std::string>());
                        }
                        value = vec;
                    }
                }
                
                if (value.index() != std::variant_npos) {
                    set(key, value, ConfigSource::Runtime, "Imported from YAML");
                }
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::importFromTOML(const std::string& toml) {
    try {
        auto doc = toml::parse(toml);
        
        clear();
        
        for (const auto& [key, value] : doc) {
            ConfigValue configValue;
            
            if (std::holds_alternative<toml::integer>(value)) {
                configValue = std::get<toml::integer>(value);
            } else if (std::holds_alternative<toml::floating_point>(value)) {
                configValue = std::get<toml::floating_point>(value);
            } else if (std::holds_alternative<toml::boolean>(value)) {
                configValue = std::get<toml::boolean>(value);
            } else if (std::holds_alternative<toml::string>(value)) {
                configValue = std::get<toml::string>(value);
            } else if (std::holds_alternative<toml::array_of<int>>(value)) {
                std::vector<int> vec;
                for (const auto& item : std::get<toml::array_of<int>>(value)) {
                    vec.push_back(item);
                }
                configValue = vec;
            } else if (std::holds_alternative<toml::array_of<double>>(value)) {
                std::vector<double> vec;
                for (const auto& item : std::get<toml::array_of<double>>(value)) {
                    vec.push_back(item);
                }
                configValue = vec;
            } else if (std::holds_alternative<toml::array_of<std::string>>(value)) {
                std::vector<std::string> vec;
                for (const auto& item : std::get<toml::array_of<std::string>>(value)) {
                    vec.push_back(item);
                }
                configValue = vec;
            }
            
            if (configValue.index() != std::variant_npos) {
                set(key, configValue, ConfigSource::Runtime, "Imported from TOML");
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::importFromINI(const std::string& ini) {
    // Simplified INI parser
    std::istringstream stream(ini);
    std::string line;
    
    clear();
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        
        // Section header
        if (line[0] == '[' && line.back() == ']') {
            std::string section = line.substr(1, line.size() - 2);
            continue;
        }
        
        // Key-value pair
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        set(key, value, ConfigSource::Runtime, "Imported from INI");
    }
    
    return true;
}

std::string Config::configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    
    if (std::holds_alternative<int>(value)) {
        oss << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        oss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        oss << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        oss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        oss << "\"" << std::get<std::string>(value) << "\"";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        oss << "[";n
        const auto& vec = std::get<std::vector<int>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            oss << vec[i];
            if (i < vec.size() - 1) oss << ", ";
        }
        oss << "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        oss << "[";n
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            oss << vec[i];
            if (i < vec.size() - 1) oss << ", ";
        }
        oss << "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        oss << "[";n
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            oss << "\"" << vec[i] << "\"";
            if (i < vec.size() - 1) oss << ", ";
        }
        oss << "]";
    }
    
    return oss.str();
}

ConfigValue Config::stringToConfigValue(const std::string& str, ConfigType type) {
    ConfigValue result;
    
    switch (type) {
        case ConfigType::Integer:
            result = std::stoi(str);
            break;
        case ConfigType::Float:
            result = std::stod(str);
            break;
        case ConfigType::Boolean:
            result = (str == "true" || str == "1");
            break;
        case ConfigType::String:
            result = str;
            break;
        case ConfigType::IntVector:
        case ConfigType::FloatVector:
        case ConfigType::StringVector:
            // Parse as vector
            break;
        default:
            result = str;
    }
    
    return result;
}

} // namespace nlm
