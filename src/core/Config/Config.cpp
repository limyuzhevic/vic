#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// Include external parsers when available
#ifdef HAS_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif
#ifdef HAS_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file format by extension
    std::string ext = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos);
    }
    
    // Try JSON parser first
    if (ext == ".json" || ext == ".JSON" || ext.empty()) {
        return loadFromJSON(filepath);
    }
    // Try YAML parser
    else if (ext == ".yaml" || ext == ".yml" || ext == ".YAML") {
        return loadFromYAML(filepath);
    }
    // Fall back to simple key=value format
    else {
        return loadFromSimpleFormat(filepath);
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// JSON helper functions
static bool parseJSONValue(const nlohmann::json& j, ConfigValue& value) {
    if (j.is_number_integer()) {
        value = j.get<int64_t>();
        return true;
    } else if (j.is_number_unsigned()) {
        value = j.get<uint64_t>();
        return true;
    } else if (j.is_number_float()) {
        value = j.get<double>();
        return true;
    } else if (j.is_boolean()) {
        value = j.get<bool>();
        return true;
    } else if (j.is_string()) {
        value = j.get<std::string>();
        return true;
    } else if (j.is_array()) {
        // Handle arrays - determine element type
        if (j.empty()) {
            value = std::vector<int>();
            return true;
        }
        // Check if all elements are numbers
        bool allInt = std::all_of(j.begin(), j.end(), [](const auto& item) { return item.is_number_integer(); });
        if (allInt) {
            std::vector<int> intArray;
            for (const auto& item : j) intArray.push_back(item.get<int>());
            value = intArray;
            return true;
        }
        // Check if all elements are floats
        bool allFloat = std::all_of(j.begin(), j.end(), [](const auto& item) { return item.is_number_float(); });
        if (allFloat) {
            std::vector<double> floatArray;
            for (const auto& item : j) floatArray.push_back(item.get<double>());
            value = floatArray;
            return true;
        }
        // Check if all elements are strings
        bool allString = std::all_of(j.begin(), j.end(), [](const auto& item) { return item.is_string(); });
        if (allString) {
            std::vector<std::string> stringArray;
            for (const auto& item : j) stringArray.push_back(item.get<std::string>());
            value = stringArray;
            return true;
        }
        // Fallback to string array
        std::vector<std::string> fallbackArray;
        for (const auto& item : j) fallbackArray.push_back(item.dump());
        value = fallbackArray;
        return true;
    }
    // Unsupported type
    return false;
}

// YAML helper functions
static bool parseYAMLValue(const YAML::Node& node, ConfigValue& value) {
    if (node.IsScalar()) {
        std::string str = node.Scalar();
        // Try to convert to appropriate type
        if (str == "true" || str == "false") {
            value = str == "true";
            return true;
        }
        try {
            // Try integer
            size_t pos;
            int intVal = std::stoi(str, &pos);
            if (pos == str.size()) {
                value = intVal;
                return true;
            }
        } catch (...) {}
        
        try {
            // Try double
            size_t pos;
            double doubleVal = std::stod(str, &pos);
            if (pos == str.size()) {
                value = doubleVal;
                return true;
            }
        } catch (...) {}
        
        // Use as string
        value = str;
        return true;
    } else if (node.IsSequence()) {
        // Handle arrays
        if (node.size() == 0) {
            value = std::vector<int>();
            return true;
        }
        // Determine element type
        bool allInt = std::all_of(node.begin(), node.end(), [](const YAML::Node& n) { return n.IsScalar() && n.as<std::string>().find('.') == std::string::npos; });
        if (allInt) {
            std::vector<int> intArray;
            for (const auto& item : node) intArray.push_back(item.as<int>());
            value = intArray;
            return true;
        }
        
        bool allFloat = std::all_of(node.begin(), node.end(), [](const YAML::Node& n) { return n.IsScalar(); });
        if (allFloat) {
            std::vector<double> floatArray;
            for (const auto& item : node) floatArray.push_back(item.as<double>());
            value = floatArray;
            return true;
        }
        
        bool allString = std::all_of(node.begin(), node.end(), [](const YAML::Node& n) { return n.IsScalar(); });
        if (allString) {
            std::vector<std::string> stringArray;
            for (const auto& item : node) stringArray.push_back(item.as<std::string>());
            value = stringArray;
            return true;
        }
        
        // Fallback to string array
        std::vector<std::string> fallbackArray;
        for (const auto& item : node) fallbackArray.push_back(item.as<std::string>());
        value = fallbackArray;
        return true;
    } else if (node.IsMap()) {
        // For maps, convert to string representation
        std::string mapStr = "{";
        bool first = true;
        for (const auto& item : node) {
            if (!first) mapStr += ", ";
            mapStr += "\"" + item.first.as<std::string>() + "\": \"" + item.second.as<std::string>() + "\"";
            first = false;
        }
        mapStr += "}";
        value = mapStr;
        return true;
    }
    // Unsupported type
    return false;
}

bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAS_NLOHMANN_JSON
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open JSON file: " + filepath);
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        
        if (!j.is_object()) {
            NLM_LOG_ERROR("JSON file is not an object: " + filepath);
            return false;
        }
        
        for (const auto& item : j.items()) {
            const std::string& key = item.key();
            ConfigValue value;
            if (parseJSONValue(item.value(), value)) {
                set(key, value, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Failed to parse JSON value for key: " + key);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("JSON parser not available. Install nlohmann/json for JSON support.");
    return false;
#endif
}

bool Config::loadFromYAML(const std::string& filepath) {
#ifdef HAS_YAML_CPP
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open YAML file: " + filepath);
            return false;
        }
        
        YAML::Node doc;
        file >> doc;
        
        if (!doc.IsMap()) {
            NLM_LOG_ERROR("YAML file is not a mapping: " + filepath);
            return false;
        }
        
        for (const auto& item : doc) {
            const std::string& key = item.first.as<std::string>();
            ConfigValue value;
            if (parseYAMLValue(item.second, value)) {
                set(key, value, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Failed to parse YAML value for key: " + key);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing YAML file " + filepath + ": " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("YAML parser not available. Install yaml-cpp for YAML support.");
    return false;
#endif
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
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

// Save configuration in different formats based on file extension
bool Config::saveToFile(const std::string& filepath) const {
    // Try to detect file format by extension
    std::string ext = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos);
    }
    
    // Try JSON format
    if (ext == ".json" || ext == ".JSON" || ext.empty()) {
        return saveToJSON(filepath);
    }
    // Try YAML format
    else if (ext == ".yaml" || ext == ".yml" || ext == ".YAML") {
        return saveToYAML(filepath);
    }
    // Fall back to simple key=value format
    else {
        return saveToSimpleFormat(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
#ifdef HAS_NLOHMANN_JSON
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open JSON file for writing: " + filepath);
            return false;
        }
        
        nlohmann::json j = toJSON();
        file << j.dump(2); // Pretty print with 2-space indentation
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error writing JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("JSON parser not available. Install nlohmann/json for JSON support.");
    return false;
#endif
}

bool Config::saveToYAML(const std::string& filepath) const {
#ifdef HAS_YAML_CPP
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open YAML file for writing: " + filepath);
            return false;
        }
        
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            if (entry.description.empty()) {
                emitter << YAML::Key << entry.key;
            } else {
                emitter << YAML::Comment(entry.description.c_str());
                emitter << YAML::Key << entry.key;
            }
            
            // Convert value to YAML node
            YAML::Node yamlNode = fromConfigValue(entry.value);
            emitter << yamlNode;
        }
        
        emitter << YAML::EndMap;
        file << emitter.c_str();
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error writing YAML file " + filepath + ": " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("YAML parser not available. Install yaml-cpp for YAML support.");
    return false;
#endif
}

bool Config::saveToSimpleFormat(const std::string& filepath) const {
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

// Helper to convert ConfigValue to nlohmann::json
nlohmann::json Config::toJSON() const {
#ifdef HAS_NLOHMANN_JSON
    nlohmann::json j;
    for (const auto& entry : pImpl->entries) {
        std::visit([&j, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j[entry.key] = arg;
            } else {
                // For unknown types, convert to string
                j[entry.key] = "unknown_type:" + std::to_string(entry.source);
            }
        }, entry.value);
    }
    return j;
#else
    return nlohmann::json();
#endif
}

// Helper to convert YAML::Node to ConfigValue
YAML::Node Config::fromConfigValue(const ConfigValue& value) {
#ifdef HAS_YAML_CPP
    return std::visit([](auto&& arg) -> YAML::Node {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) {
            return YAML::Node(arg);
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            return YAML::Node(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return YAML::Node(arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return YAML::Node(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            YAML::Node seq = YAML::Node(YAML::NodeType::Sequence);
            for (const auto& item : arg) {
                seq.push_back(item);
            }
            return seq;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            YAML::Node seq = YAML::Node(YAML::NodeType::Sequence);
            for (const auto& item : arg) {
                seq.push_back(item);
            }
            return seq;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            YAML::Node seq = YAML::Node(YAML::NodeType::Sequence);
            for (const auto& item : arg) {
                seq.push_back(item);
            }
            return seq;
        } else {
            // For unknown types, return as string
            return YAML::Node("unknown:" + std::to_string(static_cast<int>(value)));
        }
    }, value);
#else
    return YAML::Node();
#endif
}

std::string Config::toJSON() const {
    std::ostringstream oss;
    nlohmann::json j = toJSON();
    oss << j.dump(2); // Pretty print
    return oss.str();
}

std::string Config::toYAML() const {
    std::ostringstream oss;
#ifdef HAS_YAML_CPP
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.description.empty()) {
            emitter << YAML::Key << entry.key;
        } else {
            emitter << YAML::Comment(entry.description.c_str());
            emitter << YAML::Key << entry.key;
        }
        
        YAML::Node yamlNode = fromConfigValue(entry.value);
        emitter << yamlNode;
    }
    
    emitter << YAML::EndMap;
    oss << emitter.c_str();
#else
    oss << "# YAML output requires yaml-cpp library\n";
    oss << "# Configuration entries:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "# " << entry.key << " = " << entry.description << "\n";
        oss << "# Type: " << getTypeName(entry.value) << "\n";
    }
#endif
    return oss.str();
}

std::string Config::getTypeName(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) {
            return "integer";
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            return "float";
        } else if constexpr (std::is_same_v<T, bool>) {
            return "boolean";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "string";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return "int_array";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return "float_array";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return "string_array";
        } else {
            return "unknown";
        }
    }, value);
}

// Save configuration in different formats based on file extension
bool Config::saveToFile(const std::string& filepath) const {
    // Try to detect file format by extension
    std::string ext = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos);
    }
    
    // Try JSON format
    if (ext == ".json" || ext == ".JSON" || ext.empty()) {
        return saveToJSON(filepath);
    }
    // Try YAML format
    else if (ext == ".yaml" || ext == ".yml" || ext == ".YAML") {
        return saveToYAML(filepath);
    }
    // Fall back to simple key=value format
    else {
        return saveToSimpleFormat(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
#ifdef HAS_NLOHMANN_JSON
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open JSON file for writing: " + filepath);
            return false;
        }
        
        nlohmann::json j = toJSON();
        file << j.dump(2); // Pretty print with 2-space indentation
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error writing JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("JSON parser not available. Install nlohmann/json for JSON support.");
    return false;
#endif
}

bool Config::saveToYAML(const std::string& filepath) const {
#ifdef HAS_YAML_CPP
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open YAML file for writing: " + filepath);
            return false;
        }
        
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            if (entry.description.empty()) {
                emitter << YAML::Key << entry.key;
            } else {
                emitter << YAML::Comment(entry.description.c_str());
                emitter << YAML::Key << entry.key;
            }
            
            // Convert value to YAML node
            YAML::Node yamlNode = fromConfigValue(entry.value);
            emitter << yamlNode;
        }
        
        emitter << YAML::EndMap;
        file << emitter.c_str();
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error writing YAML file " + filepath + ": " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("YAML parser not available. Install yaml-cpp for YAML support.");
    return false;
#endif
}

bool Config::saveToSimpleFormat(const std::string& filepath) const {
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
