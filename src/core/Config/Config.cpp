#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <memory>

// Include JSON and YAML libraries if available
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

#ifdef HAVE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace nlm {

// Forward declarations for JSON/YAML processing
#ifdef HAVE_NLOHMANN_JSON
using json = nlohmann::json;
#endif

#ifdef HAVE_YAML_CPP
using YAML = YAML::Node;
#endif

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // JSON/YAML support
#ifdef HAVE_NLOHMANN_JSON
    std::unordered_map<std::string, json> jsonCache;
#endif
#ifdef HAVE_YAML_CPP
    std::unordered_map<std::string, YAML> yamlCache;
#endif
    
    // Configuration schema
    std::unordered_map<std::string, std::string> typeHints;
    
    // Environment variable integration
    std::unordered_map<std::string, std::string> envVarOverrides;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with common type hints
    initializeTypeHints();
}

Config::~Config() = default;

void Config::initializeTypeHints() {
    // Common NLM configuration type hints
    pImpl->typeHints = {
        {"brain.neuron_count", "int"},
        {"brain.synapse_density", "float"},
        {"brain.v_thresh", "float"},
        {"brain.v_rest", "float"},
        {"brain.v_reset", "float"},
        {"brain.tau_mem", "float"},
        {"brain.tau_ref", "float"},
        {"plasticity.stdp.enable", "bool"},
        {"plasticity.stdp.learning_rate", "float"},
        {"plasticity.hebbian.enable", "bool"},
        {"neuromod.dopamine.scale", "float"},
        {"neuromod.curiosity.enable", "bool"},
        {"neuromod.novelty.enable", "bool"},
        {"development.initial_plasticity", "float"},
        {"checkpoint.save_interval", "int"},
        {"checkpoint.max_saves", "int"},
        {"random_seed", "int64_t"},
        {"simulation_timestep", "double"}
    };
}

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format based on extension
    std::string ext = getFileExtension(filepath);
    
    if (ext == "json") {
        return loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAVE_NLOHMANN_JSON
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        
        // Recursively process JSON and add entries
        processJSONObject(j, "", ConfigSource::File);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading JSON config from " << filepath << ": " << e.what() << std::endl;
        return false;
    }
#else
    std::cerr << "JSON support not available. Install nlohmann/json to enable JSON configuration." << std::endl;
    return false;
#endif
}

bool Config::loadFromYAML(const std::string& filepath) {
#ifdef HAVE_YAML_CPP
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        YAML yamlDoc = YAML::Load(file);
        
        // Process YAML document recursively
        processYAMLNode(yamlDoc, "", ConfigSource::File);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading YAML config from " << filepath << ": " << e.what() << std::endl;
        return false;
    }
#else
    std::cerr << "YAML support not available. Install yaml-cpp to enable YAML configuration." << std::endl;
    return false;
#endif
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == ';') {
            continue;
        }
        
        // Parse key=value pairs with support for various formats
        if (parseSimpleLine(line, lineNumber)) {
            continue;
        }
        
        // Try to parse as JSON if it looks like JSON
        if (line.find('{') != std::string::npos && line.find('}') != std::string::npos) {
            try {
                json j = json::parse(line);
                processJSONLine(j, lineNumber);
                continue;
            } catch (...) {
                // Not JSON, continue with other parsing attempts
            }
        }
        
        // Try to parse as YAML if it looks like YAML
        if (line.find(':') != std::string::npos) {
            try {
                YAML yamlNode = YAML::Load(line);
                if (yamlNode.IsMap()) {
                    processYAMLNode(yamlNode, lineNumber);
                    continue;
                }
            } catch (...) {
                // Not YAML, continue
            }
        }
        
        std::cerr << "Warning: Could not parse line " << lineNumber << ": " << line << std::endl;
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // First, load from environment variables
    loadFromEnvironmentVariables();
    
    // Then process command line arguments
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
    std::string ext = getFileExtension(filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (ext == "json") {
        saveToJSON(file);
    } else if (ext == "yaml" || ext == "yml") {
        saveToYAML(file);
    } else {
        // Default to simple key=value format
        saveToSimpleFormat(file);
    }
    
    return true;
}

void Config::loadFromEnvironmentVariables() {
    // Common environment variable prefixes for NLM
    const std::vector<std::string> prefixes = {
        "NLM_", "BRAIN_", "NEURAL_", "SYNAPSE_"
    };
    
    for (const auto& prefix : prefixes) {
        for (const auto& entry : pImpl->typeHints) {
            std::string envVar = prefix + entry.first;
            char* envValue = std::getenv(envVar.c_str());
            if (envValue != nullptr) {
                pImpl->envVarOverrides[entry.first] = std::string(envValue);
                set(entry.first, std::string(envValue), ConfigSource::Runtime);
            }
        }
    }
}

void Config::processJSONLine(const json& j, int lineNumber) {
#ifdef HAVE_NLOHMANN_JSON
    try {
        // Convert JSON to string representation
        std::string jsonStr = j.dump();
        size_t pos = jsonStr.find(':');
        if (pos != std::string::npos) {
            std::string key = trim(jsonStr.substr(0, pos));
            std::string value = trim(jsonStr.substr(pos + 1));
            set(key, value, ConfigSource::File);
        }
    } catch (...) {
        // Ignore JSON parsing errors for line-by-line processing
    }
#endif
}

void Config::processYAMLNode(const YAML& node, int lineNumber) {
#ifdef HAVE_YAML_CPP
    try {
        // Handle single key-value pair
        if (node.IsScalar() && node.Present()) {
            // Can't process single scalar easily, skip
            return;
        }
        
        // Process nested structure
        if (node.IsMap()) {
            for (const auto& pair : node) {
                std::string key = pair.first.as<std::string>();
                std::string fullKey = (lineNumber > 0) ? 
                    std::to_string(lineNumber) + "." + key : key;
                
                if (pair.second.IsScalar()) {
                    set(fullKey, pair.second.as<std::string>(), ConfigSource::File);
                } else {
                    // Recursively process nested structures
                    processYAMLNode(pair.second, lineNumber);
                }
            }
        }
    } catch (...) {
        // Ignore YAML parsing errors
    }
#endif
}

void Config::processJSONObject(const json& j, const std::string& prefix, ConfigSource source) {
#ifdef HAVE_NLOHMANN_JSON
    if (j.is_object()) {
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
            processJSONValue(it.value(), key, source);
        }
    } else if (j.is_array()) {
        // Process array elements with indexed keys
        for (size_t i = 0; i < j.size(); ++i) {
            std::string key = prefix + "[" + std::to_string(i) + "]";
            processJSONValue(j[i], key, source);
        }
    }
#endif
}

void Config::processJSONValue(const json& value, const std::string& key, ConfigSource source) {
#ifdef HAVE_NLOHMANN_JSON
    if (value.is_null()) {
        set(key, "null", source);
    } else if (value.is_boolean()) {
        set(key, value.get<bool>() ? "true" : "false", source);
    } else if (value.is_number_integer()) {
        set(key, std::to_string(value.get<int64_t>()), source);
    } else if (value.is_number_unsigned()) {
        set(key, std::to_string(value.get<uint64_t>()), source);
    } else if (value.is_number_float()) {
        set(key, std::to_string(value.get<double>()), source);
    } else if (value.is_string()) {
        set(key, value.get<std::string>(), source);
    } else if (value.is_array()) {
        // Convert array to string representation
        std::string arrayStr = "[";
        for (size_t i = 0; i < value.size(); ++i) {
            if (i > 0) arrayStr += ", ";
            arrayStr += value[i].dump();
        }
        arrayStr += "]";
        set(key, arrayStr, source);
    } else {
        // For objects, we could process them recursively
        // For now, store as JSON string
        set(key, value.dump(), source);
    }
#endif
}

void Config::processYAMLNode(const YAML& node, const std::string& prefix, ConfigSource source) {
#ifdef HAVE_YAML_CPP
    if (node.IsMap()) {
        for (const auto& pair : node) {
            std::string key = prefix.empty() ? pair.first.as<std::string>() : 
                            prefix + "." + pair.first.as<std::string>();
            processYAMLValue(pair.second, key, source);
        }
    } else if (node.IsSequence()) {
        // Process sequence elements with indexed keys
        for (size_t i = 0; i < node.size(); ++i) {
            std::string key = prefix + "[" + std::to_string(i) + "]";
            processYAMLValue(node[i], key, source);
        }
    }
#endif
}

void Config::processYAMLValue(const YAML& value, const std::string& key, ConfigSource source) {
#ifdef HAVE_YAML_CPP
    if (!value) {
        set(key, "null", source);
    } else if (value.IsScalar()) {
        set(key, value.as<std::string>(), source);
    } else if (value.IsMap()) {
        // Recursively process nested structure
        processYAMLNode(value, key, source);
    } else if (value.IsSequence()) {
        // Convert sequence to string representation
        std::string seqStr = "[";
        for (size_t i = 0; i < value.size(); ++i) {
            if (i > 0) seqStr += ", ";
            seqStr += value[i].as<std::string>();
        }
        seqStr += "]";
        set(key, seqStr, source);
    }
#endif
}

std::string Config::getFileExtension(const std::string& filename) const {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    std::string ext = filename.substr(dotPos + 1);
    // Convert to lowercase
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

void Config::saveToJSON(std::ostream& output) const {
#ifdef HAVE_NLOHMANN_JSON
    json j;
    
    // Convert configuration entries to JSON
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        std::string value = entry.value;
        
        // Parse value to appropriate type based on type hint
        auto typeIt = pImpl->typeHints.find(key);
        if (typeIt != pImpl->typeHints.end()) {
            std::string type = typeIt->second;
            if (type == "int") {
                try {
                    j[key] = std::stoi(value);
                } catch (...) {
                    j[key] = value;
                }
            } else if (type == "float" || type == "double") {
                try {
                    j[key] = std::stod(value);
                } catch (...) {
                    j[key] = value;
                }
            } else if (type == "bool") {
                j[key] = (value == "true" || value == "1" || value == "yes");
            } else if (type == "int64_t") {
                try {
                    j[key] = std::stoll(value);
                } catch (...) {
                    j[key] = value;
                }
            } else {
                j[key] = value;
            }
        } else {
            // Try to infer type from value
            if (value == "true" || value == "false") {
                j[key] = (value == "true");
            } else {
                try {
                    // Try integer
                    size_t pos;
                    int intVal = std::stoi(value, &pos);
                    if (pos == value.size()) {
                        j[key] = intVal;
                        continue;
                    }
                } catch (...) {}
                
                try {
                    // Try double
                    size_t pos;
                    double doubleVal = std::stod(value, &pos);
                    if (pos == value.size()) {
                        j[key] = doubleVal;
                        continue;
                    }
                } catch (...) {}
                
                // Store as string
                j[key] = value;
            }
        }
    }
    
    output << j.dump(2); // Pretty print with 2-space indentation
#endif
}

void Config::saveToYAML(std::ostream& output) const {
#ifdef HAVE_YAML_CPP
    YAML yamlDoc;
    
    // Convert configuration entries to YAML
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        std::string value = entry.value;
        
        // Try to determine YAML node type
        auto typeIt = pImpl->typeHints.find(key);
        if (typeIt != pImpl->typeHints.end()) {
            std::string type = typeIt->second;
            if (type == "bool") {
                yamlDoc[key] = (value == "true" || value == "1" || value == "yes");
            } else if (type == "int" || type == "int64_t") {
                try {
                    yamlDoc[key] = std::stoll(value);
                } catch (...) {
                    yamlDoc[key] = value;
                }
            } else if (type == "float" || type == "double") {
                try {
                    yamlDoc[key] = std::stod(value);
                } catch (...) {
                    yamlDoc[key] = value;
                }
            } else {
                yamlDoc[key] = value;
            }
        } else {
            yamlDoc[key] = value;
        }
    }
    
    YAML::Emitter emitter(output);
    emitter.SetIndent(2);
    emitter.Emit(yamlDoc);
#endif
}

void Config::saveToSimpleFormat(std::ostream& output) const {
    for (const auto& entry : pImpl->entries) {
        output << "# " << entry.description << "\n";
        output << entry.key << " = " << entry.value << "\n\n";
    }
}

bool Config::parseSimpleLine(const std::string& line, int lineNumber) {
    // Try to parse as key=value
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
        return true;
    }
    
    // Try to parse as JSON
    if (line.find('{') != std::string::npos && line.find('}') != std::string::npos) {
        try {
            json j = json::parse(line);
            processJSONLine(j, lineNumber);
            return true;
        } catch (...) {
            // Not JSON
        }
    }
    
    // Try to parse as YAML
    if (line.find(':') != std::string::npos && line.find(' ') != std::string::npos) {
        try {
            YAML yamlNode = YAML::Load(line);
            if (yamlNode.IsMap()) {
                processYAMLNode(yamlNode, lineNumber);
                return true;
            }
        } catch (...) {
            // Not YAML
        }
    }
    
    return false;
}

// Template method implementations
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

// Explicit template instantiations
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

} // namespace nlm