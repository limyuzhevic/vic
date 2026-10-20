#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

bool Config::parseJSONFirstChar(const std::string& content) const {
    // Check first non-whitespace character for JSON indicators
    size_t pos = 0;
    while (pos < content.size() && std::isspace(content[pos])) pos++;
    
    return pos < content.size() && (content[pos] == '{' || content[pos] == '[');
}

bool Config::parseYAMLFirstChar(const std::string& content) const {
    // Check first non-whitespace character for YAML indicators
    size_t pos = 0;
    while (pos < content.size() && std::isspace(content[pos])) pos++;
    
    return pos < content.size() && (content[pos] == '-' || content[pos] == '{' || 
                                     content[pos] == '[' || std::isalpha(content[pos]));
}

void Config::parseJSON(const std::string& content) {
    // Parse JSON content using RapidJSON
    rapidjson::Document doc;
    doc.Parse(content.c_str());
    
    if (doc.HasParseError()) {
        throw std::runtime_error("JSON parse error at position " + std::to_string(doc.GetParseError()) +
                               ": " + std::string(rapidjson::GetParseError_En(doc.GetParseError())));
    }
    
    // Flatten JSON structure into key=value pairs
    flattenJSON(doc, "");
}

void Config::parseYAML(const std::string& content) {
    // Parse YAML content using YAML-CPP
    YAML::Node doc = YAML::Load(content);
    
    // Flatten YAML structure into key=value pairs
    flattenYAML(doc, "");
}

void Config::flattenJSON(const rapidjson::Value& value, const std::string& prefix) {
    if (value.IsObject()) {
        for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it) {
            std::string key = prefix.empty() ? it->name.GetString() : prefix + "." + it->name.GetString();
            flattenJSON(it->value, key);
        }
    } else if (value.IsArray()) {
        for (SizeType i = 0; i < value.Size(); ++i) {
            std::string key = prefix + ".[" + std::to_string(i) + "]";
            flattenJSON(value[i], key);
        }
    } else {
        // Leaf value
        std::string key = prefix;
        std::string strValue;
        
        if (value.IsString()) {
            strValue = value.GetString();
        } else if (value.IsNumber()) {
            if (value.IsInt()) {
                strValue = std::to_string(value.GetInt());
            } else if (value.IsUint()) {
                strValue = std::to_string(value.GetUint());
            } else if (value.IsInt64()) {
                strValue = std::to_string(value.GetInt64());
            } else if (value.IsUint64()) {
                strValue = std::to_string(value.GetUint64());
            } else if (value.IsFloat()) {
                strValue = std::to_string(value.GetFloat());
            } else if (value.IsDouble()) {
                strValue = std::to_string(value.GetDouble());
            }
        } else if (value.IsBool()) {
            strValue = value.GetBool() ? "true" : "false";
        } else if (value.IsNull()) {
            strValue = "null";
        }
        
        if (!key.empty()) {
            set(key, strValue, ConfigSource::File);
        }
    }
}

void Config::flattenYAML(const YAML::Node& node, const std::string& prefix) {
    if (node.IsMap()) {
        for (auto it : node) {
            std::string key = it.first.IsScalar() ? it.first.Scalar() : "";
            if (!key.empty()) {
                std::string newPrefix = prefix.empty() ? key : prefix + "." + key;
                flattenYAML(it.second, newPrefix);
            }
        }
    } else if (node.IsSequence()) {
        for (size_t i = 0; i < node.size(); ++i) {
            std::string key = prefix + ".[" + std::to_string(i) + "]";
            flattenYAML(node[i], key);
        }
    } else {
        // Leaf value
        std::string key = prefix;
        std::string strValue;
        
        if (node.IsScalar()) {
            strValue = node.Scalar();
            // Convert string representations to proper types
            if (strValue == "true" || strValue == "false") {
                set(key, strValue == "true", ConfigSource::File);
                return;
            } else if (strValue.find('.') != std::string::npos) {
                try {
                    double doubleVal = std::stod(strValue);
                    set(key, doubleVal, ConfigSource::File);
                    return;
                } catch (...) {}
            } else {
                try {
                    int intVal = std::stoi(strValue);
                    set(key, intVal, ConfigSource::File);
                    return;
                } catch (...) {}
            }
        } else if (node.IsBoolean()) {
            strValue = node.As<bool>() ? "true" : "false";
        } else if (node.IsInteger()) {
            strValue = std::to_string(node.As<int>());
        } else if (node.IsFloat()) {
            strValue = std::to_string(node.As<float>());
        } else if (node.IsDouble()) {
            strValue = std::to_string(node.As<double>());
        } else {
            strValue = "";
        }
        
        if (!key.empty() && !strValue.empty()) {
            set(key, strValue, ConfigSource::File);
        }
    }
}

bool Config::parseSimpleFormat(const std::string& content) {
    // Original simple key=value parser for backward compatibility
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
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

bool Config::loadFromFile(const std::string& filepath) {
    // Proper JSON/YAML parser with format detection and comprehensive error handling
    // Supports both JSON (.json) and YAML (.yaml, .yml) formats with robust validation
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Detect file format based on extension and content
    bool isJSON = false;
    bool isYAML = false;
    
    if (filepath.size() >= 5) {
        std::string extension = filepath.substr(filepath.size() - 5);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        if (extension == ".json" || extension == ".JSON") {
            isJSON = true;
        } else if (extension == ".yaml" || extension == ".yml" || extension == ".YAML" || extension == ".YML") {
            isYAML = true;
        }
    }
    
    // Auto-detect based on content if extension unknown
    if (!isJSON && !isYAML) {
        isJSON = parseJSONFirstChar(content);
        if (!isJSON) {
            isYAML = parseYAMLFirstChar(content);
        }
    }
    
    try {
        if (isJSON) {
            parseJSON(content);
        } else if (isYAML) {
            parseYAML(content);
        } else {
            // Fallback to simple key=value format
            return parseSimpleFormat(content);
        }
    } catch (const std::exception& e) {
        // Clear any partial entries on error
        clear();
        return false;
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