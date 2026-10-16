#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <stdexcept>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, size_t> keyIndexMap;  // For O(1) lookups
    size_t nextIndex;  // Track next index for insertion
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->nextIndex = 0;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file format based on extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == ".json") {
        success = loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        success = loadFromYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        success = loadFromSimpleFile(filepath);
    }
    
    if (!success) {
        NLM_LOG_ERROR("Failed to load configuration from file: " + filepath);
    }
    
    return success;
}

bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAVE_NLOHMANN_JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        auto json = nlohmann::json::parse(file);
        
        for (auto& [key, value] : json.items()) {
            ConfigValue configValue;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Handle arrays
                std::vector<double> doubleArray;
                for (auto& item : value) {
                    if (item.is_number()) {
                        doubleArray.push_back(item.get<double>());
                    } else {
                        throw std::runtime_error("JSON array contains non-numeric values");
                    }
                }
                configValue = doubleArray;
            } else {
                throw std::runtime_error("Unsupported JSON value type for key: " + key);
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("JSON parsing error: ") + e.what());
        return false;
    }
#else
    NLM_LOG_WARNING("JSON support not available - compile with nlohmann_json to enable JSON parsing");
    return loadFromSimpleFile(filepath);
#endif
}

bool Config::loadFromYAML(const std::string& filepath) {
#ifdef HAVE_YAML_CPP
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node doc = YAML::Load(file);
        
        for (const auto& node : doc) {
            std::string key = node.first.as<std::string>();
            const YAML::Node& value = node.second;
            
            ConfigValue configValue;
            
            if (value.IsScalar()) {
                // Try to infer type
                if (value.IsInteger()) {
                    configValue = value.as<int64_t>();
                } else if (value.IsDouble()) {
                    configValue = value.as<double>();
                } else if (value.IsBoolean()) {
                    configValue = value.as<bool>();
                } else {
                    configValue = value.as<std::string>();
                }
            } else if (value.IsSequence()) {
                // Handle arrays
                std::vector<double> doubleArray;
                for (const auto& item : value) {
                    if (item.IsScalar()) {
                        doubleArray.push_back(item.as<double>());
                    }
                }
                configValue = doubleArray;
            } else {
                throw std::runtime_error("Unsupported YAML value type for key: " + key);
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("YAML parsing error: ") + e.what());
        return false;
    }
#else
    NLM_LOG_WARNING("YAML support not available - compile with yaml-cpp to enable YAML parsing");
    return loadFromSimpleFile(filepath);
#endif
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs (including complex types)
        if (parseSimpleLine(line)) {
            continue;
        }
        
        // Parse array assignments: key = [1,2,3,4]
        if (parseArrayLine(line)) {
            continue;
        }
        
        // Parse object assignments: key = {value1,value2}
        if (parseObjectLine(line)) {
            continue;
        }
        
        NLM_LOG_WARNING("Invalid configuration line " + std::to_string(lineNumber) + ": " + line);
    }
    
    return true;
}

bool Config::parseSimpleLine(const std::string& line) {
    // Check if line contains '= '
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return false;
    }
    
    std::string key = trim(line.substr(0, pos));
    std::string valueStr = trim(line.substr(pos + 1));
    
    if (key.empty()) {
        return false;
    }
    
    // Remove quotes from value
    if ((valueStr.front() == '\"' && valueStr.back() == '\"') || 
        (valueStr.front() == '\'' && valueStr.back() == '\'')) {
        valueStr = valueStr.substr(1, valueStr.size() - 2);
    }
    
    // Determine value type and set
    if (isBooleanString(valueStr)) {
        bool value = (valueStr == "true" || valueStr == "1");
        set(key, value, ConfigSource::File);
    } else if (isIntegerString(valueStr)) {
        try {
            int64_t value = std::stoll(valueStr);
            set(key, value, ConfigSource::File);
        } catch (...) {
            // Fall back to string
            set(key, valueStr, ConfigSource::File);
        }
    } else if (isFloatString(valueStr)) {
        try {
            double value = std::stod(valueStr);
            set(key, value, ConfigSource::File);
        } catch (...) {
            // Fall back to string
            set(key, valueStr, ConfigSource::File);
        }
    } else {
        set(key, valueStr, ConfigSource::File);
    }
    
    return true;
}

bool Config::parseArrayLine(const std::string& line) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return false;
    }
    
    std::string key = trim(line.substr(0, pos));
    std::string valueStr = trim(line.substr(pos + 1));
    
    // Check if it's an array: [value1, value2, ...]
    if (valueStr.front() != '[' || valueStr.back() != ']') {
        return false;
    }
    
    std::string inner = valueStr.substr(1, valueStr.size() - 2);
    std::vector<double> values;
    
    // Parse comma-separated values
    std::stringstream ss(inner);
    std::string item;
    while (std::getline(ss, item, ',')) {
        item = trim(item);
        if (item.empty()) continue;
        
        try {
            double value = std::stod(item);
            values.push_back(value);
        } catch (...) {
            // If we can't parse as number, treat as string array
            std::vector<std::string> strValues;
            std::stringstream ss2(inner);
            std::string strItem;
            while (std::getline(ss2, strItem, ',')) {
                strValues.push_back(trim(strItem));
            }
            set(key, strValues, ConfigSource::File);
            return true;
        }
    }
    
    set(key, values, ConfigSource::File);
    return true;
}

bool Config::parseObjectLine(const std::string& line) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return false;
    }
    
    std::string key = trim(line.substr(0, pos));
    std::string valueStr = trim(line.substr(pos + 1));
    
    // Check if it's an object: {value1,value2,...}
    if (valueStr.front() != '{' || valueStr.back() != '}') {
        return false;
    }
    
    std::string inner = valueStr.substr(1, valueStr.size() - 2);
    std::vector<std::string> values;
    
    // Parse comma-separated values
    std::stringstream ss(inner);
    std::string item;
    while (std::getline(ss, item, ',')) {
        values.push_back(trim(item));
    }
    
    set(key, values, ConfigSource::File);
    return true;
}

bool Config::isBooleanString(const std::string& str) const {
    std::string lower = toLower(str);
    return lower == "true" || lower == "false" || lower == "1" || lower == "0";
}

bool Config::isIntegerString(const std::string& str) const {
    if (str.empty() || (str[0] == '-' && str.size() == 1)) {
        return false;
    }
    
    std::string::const_iterator it = str.begin();
    while (it != str.end() && std::isdigit(*it)) {
        ++it;
    }
    
    return it == str.end() || 
           (*it == '-' && std::next(it) != str.end() && std::isdigit(*(std::next(it))));
}

bool Config::isFloatString(const std::string& str) const {
    if (str.empty()) return false;
    
    std::string::const_iterator it = str.begin();
    bool foundDot = false;
    while (it != str.end() && (std::isdigit(*it) || *it == '.' || *it == '-')) {
        if (*it == '.') {
            if (foundDot) return false;  // Multiple dots not allowed
            foundDot = true;
        }
        ++it;
    }
    
    return it == str.end();
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
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == ".json") {
        success = saveToJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        success = saveToYAML(filepath);
    } else {
        success = saveToSimpleFile(filepath);
    }
    
    if (!success) {
        NLM_LOG_ERROR("Failed to save configuration to file: " + filepath);
    }
    
    return success;
}

bool Config::saveToJSON(const std::string& filepath) const {
#ifdef HAVE_NLOHMANN_JSON
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json json;
        
        for (const auto& entry : pImpl->entries) {
            json[entry.key] = visit_json_value(entry.value);
        }
        
        file << std::setw(2) << json << std::endl;
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("JSON writing error: ") + e.what());
        return false;
    }
#else
    return saveToSimpleFile(filepath);
#endif
}

bool Config::saveToYAML(const std::string& filepath) const {
#ifdef HAVE_YAML_CPP
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginDoc;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            emitter << YAML::Value << visit_yaml_value(entry.value);
        }
        
        emitter << YAML::EndDoc;
        file << emitter.c_str();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("YAML writing error: ") + e.what());
        return false;
    }
#else
    return saveToSimpleFile(filepath);
#endif
}

bool Config::saveToSimpleFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        // Add description as comment
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Write key=value pair with proper formatting
        file << entry.key << " = ";
        
        visit_ostream_value(entry.value, file);
        
        file << "\n\n";
    }
    
    return true;
}

// Helper functions for JSON/YAML conversion
template<typename T>
nlohmann::json to_json(const T& value) {
    return value;
}

nlohmann::json Config::visit_json_value(const ConfigValue& value) const {
    return std::visit([](const auto& val) -> nlohmann::json {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return val;
        } else if constexpr (std::is_same_v<T, double>) {
            return val;
        } else if constexpr (std::is_same_v<T, bool>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            nlohmann::json jsonArray = nlohmann::json::array();
            for (auto item : val) {
                jsonArray.push_back(item);
            }
            return jsonArray;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return val;
        } else {
            return nlohmann::json(nullptr);
        }
    }, value);
}

YAML::Node Config::visit_yaml_value(const ConfigValue& value) const {
    return std::visit([](const auto& val) -> YAML::Node {
        YAML::Node node;
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            node = val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            node = val;
        } else if constexpr (std::is_same_v<T, double>) {
            node = val;
        } else if constexpr (std::is_same_v<T, bool>) {
            node = val;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            YAML::Node array = YAML::Node(YAML::NodeType::Sequence);
            for (auto item : val) {
                array.push_back(item);
            }
            node = array;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            YAML::Node array = YAML::Node(YAML::NodeType::Sequence);
            for (auto item : val) {
                array.push_back(item);
            }
            node = array;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            YAML::Node array = YAML::Node(YAML::NodeType::Sequence);
            for (auto item : val) {
                array.push_back(item);
            }
            node = array;
        }
        return node;
    }, value);
}

void Config::visit_ostream_value(const ConfigValue& value, std::ostream& os) const {
    std::visit([&os](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            os << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, double>) {
            os << val;
        } else if constexpr (std::is_same_v<T, bool>) {
            os << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            os << "[";
            bool first = true;
            for (auto item : val) {
                if (!first) os << ", ";
                os << item;
                first = false;
            }
            os << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            os << "[";
            bool first = true;
            for (auto item : val) {
                if (!first) os << ", ";
                os << item;
                first = false;
            }
            os << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            os << "[";
            bool first = true;
            for (auto item : val) {
                if (!first) os << ", ";
                os << "\"" << item << "\"";
                first = false;
            }
            os << "]";
        }
    }, value);
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->keyIndexMap.find(key);
    if (it == pImpl->keyIndexMap.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const ConfigEntry& entry = pImpl->entries[index];
    
    try {
        return std::get<T>(entry.value);
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
    // Remove from old index if exists
    auto it = pImpl->keyIndexMap.find(key);
    if (it != pImpl->keyIndexMap.end()) {
        size_t oldIndex = it->second;
        if (oldIndex < pImpl->entries.size()) {
            // Mark as deleted
            pImpl->entries[oldIndex].key.clear();
        }
    }
    
    // Find first available slot for insertion
    size_t insertIndex = pImpl->nextIndex++;
    while (insertIndex < pImpl->entries.size() && !pImpl->entries[insertIndex].key.empty()) {
        insertIndex++;
    }
    
    if (insertIndex >= pImpl->entries.size()) {
        pImpl->entries.emplace_back(key, value, source);
        pImpl->keyIndexMap[key] = pImpl->entries.size() - 1;
    } else {
        pImpl->entries[insertIndex].key = key;
        pImpl->entries[insertIndex].value = value;
        pImpl->entries[insertIndex].source = source;
        pImpl->keyIndexMap[key] = insertIndex;
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
    return pImpl->keyIndexMap.find(key) != pImpl->keyIndexMap.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->keyIndexMap.find(key);
    if (it != pImpl->keyIndexMap.end()) {
        size_t index = it->second;
        if (index < pImpl->entries.size()) {
            pImpl->entries[index].key.clear();  // Mark as deleted
            pImpl->keyIndexMap.erase(it);
        }
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        if (!entry.key.empty()) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->keyIndexMap.clear();
    pImpl->nextIndex = 0;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << getKeys().size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) continue;
        
        oss << "  " << entry.key << " = ";
        visit_ostringstream_value(entry.value, oss);
        oss << " (" << static_cast<int>(entry.source) << ")";
        
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
    }
    
    return oss.str();
}

void Config::visit_ostringstream_value(const ConfigValue& value, std::ostringstream& oss) const {
    std::visit([&oss](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, double>) {
            oss << val;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            oss << "[";
            bool first = true;
            for (auto item : val) {
                if (!first) oss << ", ";
                oss << item;
                first = false;
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            bool first = true;
            for (auto item : val) {
                if (!first) oss << ", ";
                oss << item;
                first = false;
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            bool first = true;
            for (auto item : val) {
                if (!first) oss << ", ";
                oss << "\"" << item << "\"";
                first = false;
            }
            oss << "]";
        }
    }, value);
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
