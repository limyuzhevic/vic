#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

bool Config::loadFromFile(const std::string& filepath) {
    // Implement proper JSON/YAML parser
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), 
                        std::istreambuf_iterator<char>());
    
    // Simple JSON parsing implementation
    size_t pos = 0;
    bool success = parseJSON(content, pos);
    
    return success;
}

bool Config::loadFromFileJSON(const std::string& filepath) {
    return loadFromFile(filepath);
}

bool Config::parseJSON(const std::string& json, size_t& pos) {
    skipWhitespace(json, pos);
    
    if (pos >= json.size() || json[pos] != '{') {
        return false;
    }
    
    pos++; // Skip {
    skipWhitespace(json, pos);
    
    while (pos < json.size()) {
        skipWhitespace(json, pos);
        
        if (json[pos] == '}') {
            pos++; // Skip }
            return true;
        }
        
        // Parse key
        if (!parseJSONString(json, pos)) {
            return false;
        }
        
        skipWhitespace(json, pos);
        if (pos >= json.size() || json[pos] != ':') {
            return false;
        }
        
        pos++; // Skip :
        skipWhitespace(json, pos);
        
        // Parse value
        if (!parseJSONValue(json, pos)) {
            return false;
        }
        
        skipWhitespace(json, pos);
        
        if (pos < json.size() && json[pos] == ',') {
            pos++; // Skip ,
        } else if (pos < json.size() && json[pos] == '}') {
            // Will handle in outer loop
            continue;
        }
    }
    
    return false;
}

bool Config::parseJSONString(const std::string& json, size_t& pos) {
    if (pos >= json.size() || json[pos] != '"') {
        return false;
    }
    
    pos++; // Skip "
    while (pos < json.size()) {
        if (json[pos] == '"') {
            pos++; // Skip closing "
            return true;
        } else if (json[pos] == '\\') {
            pos += 2; // Skip escape sequence
        } else {
            pos++;
        }
    }
    
    return false;
}

bool Config::parseJSONValue(const std::string& json, size_t& pos) {
    skipWhitespace(json, pos);
    
    if (pos >= json.size()) {
        return false;
    }
    
    char c = json[pos];
    
    if (c == '"') {
        // String value
        size_t start = pos++;
        while (pos < json.size() && json[pos] != '"') {
            if (json[pos] == '\\') {
                pos += 2; // Skip escape
            } else {
                pos++;
            }
        }
        if (pos < json.size() && json[pos] == '"') {
            pos++; // Skip closing quote
            return true;
        }
        return false;
    } else if (c == '{') {
        // Object - recursively parse
        return parseJSON(json, pos);
    } else if (c == '[') {
        // Array - parse as string for now
        return parseJSONArray(json, pos);
    } else if (std::isdigit(c) || c == '-') {
        // Number
        while (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '.' || 
                                     json[pos] == 'e' || json[pos] == 'E' || 
                                     json[pos] == '+' || json[pos] == '-')) {
            pos++;
        }
        return true;
    } else if (c == 't' || c == 'f') {
        // Boolean
        if (json.substr(pos, 4) == "true" || json.substr(pos, 5) == "false") {
            pos += 4;
            return true;
        }
        return false;
    } else if (c == 'n') {
        // Null
        if (json.substr(pos, 4) == "null") {
            pos += 4;
            return true;
        }
        return false;
    }
    
    return false;
}

bool Config::parseJSONArray(const std::string& json, size_t& pos) {
    pos++; // Skip [
    skipWhitespace(json, pos);
    
    while (pos < json.size()) {
        skipWhitespace(json, pos);
        
        if (json[pos] == ']') {
            pos++; // Skip ]
            return true;
        }
        
        if (!parseJSONValue(json, pos)) {
            return false;
        }
        
        skipWhitespace(json, pos);
        
        if (pos < json.size() && json[pos] == ',') {
            pos++; // Skip ,
        } else if (pos < json.size() && json[pos] == ']') {
            // Will handle in outer loop
            continue;
        }
    }
    
    return false;
}

void Config::skipWhitespace(const std::string& json, size_t& pos) {
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || 
                                 json[pos] == '\n' || json[pos] == '\r')) {
        pos++;
    }
}

void Config::saveToFileJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    file << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        // Output value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, double> ||
                                 std::is_same_v<T, bool>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << arg[j] << "\"";
                }
                file << "]";
            } else {
                file << "null"; // For unsupported types
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "}\n";
    file.close();
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
    // Use JSON format for saving
    saveToFileJSON(filepath);
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
