#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    std::string content = buffer.str();
    
    // Try to parse as JSON
    if (parseJSON(content)) {
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
    std::ifstream file2(filepath);
    if (!file2.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file2, line)) {
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

// Simple JSON parser implementation
bool Config::parseJSON(const std::string& json) {
    try {
        // Simple JSON parsing - this is a minimal implementation
        // A full JSON parser would use a proper library like nlohmann::json
        
        size_t pos = 0;
        skipWhitespace(json, pos);
        
        if (pos >= json.size() || json[pos] != '{') {
            return false;  // Not a JSON object
        }
        
        pos++;  // Skip {
        skipWhitespace(json, pos);
        
        while (pos < json.size() && json[pos] != '}') {
            // Parse key
            skipWhitespace(json, pos);
            if (pos >= json.size() || json[pos] != '"') {
                return false;
            }
            
            size_t keyStart = ++pos;
            while (pos < json.size() && json[pos] != '"') {
                pos++;
            }
            
            if (pos >= json.size()) {
                return false;
            }
            
            std::string key = json.substr(keyStart, pos - keyStart);
            pos++;  // Skip closing quote
            
            skipWhitespace(json, pos);
            if (pos >= json.size() || json[pos] != ':') {
                return false;
            }
            
            pos++;  // Skip :
            skipWhitespace(json, pos);
            
            // Parse value
            ConfigValue value = parseJSONValue(json, pos);
            if (value.index() == static_cast<size_t>(-1)) {  // Check for invalid variant (equivalent to std::variant_npos)
                return false;
            }
            
            set(key, value, ConfigSource::File);
            
            skipWhitespace(json, pos);
            if (pos < json.size() && json[pos] == ',') {
                pos++;  // Skip ,
                skipWhitespace(json, pos);
            } else if (pos < json.size() && json[pos] != '}') {
                return false;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

// Parse a single JSON value
Config::ConfigValue Config::parseJSONValue(const std::string& json, size_t& pos) {
    skipWhitespace(json, pos);
    
    if (pos >= json.size()) {
        return std::string();
    }
    
    char c = json[pos];
    
    if (c == '"') {
        // String
        pos++;  // Skip opening quote
        size_t start = pos;
        while (pos < json.size() && json[pos] != '"') {
            if (json[pos] == '\\') {
                pos++;  // Skip escaped character
                if (pos < json.size()) pos++;  // Skip the escaped character
            }
            pos++;
        }
        
        if (pos >= json.size()) {
            return std::string();
        }
        
        std::string result = json.substr(start, pos - start);
        pos++;  // Skip closing quote
        return result;
    } else if (c == 't' || c == 'f') {
        // Boolean
        if (json.substr(pos, 4) == "true") {
            pos += 4;
            return true;
        } else if (json.substr(pos, 5) == "false") {
            pos += 5;
            return false;
        }
    } else if (c == '-' || (c >= '0' && c <= '9')) {
        // Number
        size_t start = pos;
        while (pos < json.size() && (json[pos] == '.' || json[pos] == '-' || 
                                    (json[pos] >= '0' && json[pos] <= '9') ||
                                    json[pos] == 'e' || json[pos] == 'E' ||
                                    json[pos] == '+' || json[pos] == '-')) {
            pos++;
        }
        
        std::string numStr = json.substr(start, pos - start);
        
        // Try to parse as double first
        try {
            double d = std::stod(numStr);
            return d;
        } catch (...) {
            // If that fails, try as int64_t
            try {
                int64_t i = std::stoll(numStr);
                return i;
            } catch (...) {
                // Fall back to string
                return numStr;
            }
        }
    } else if (c == '[') {
        // Array
        pos++;  // Skip [
        std::vector<ConfigValue> array;
        
        skipWhitespace(json, pos);
        if (pos < json.size() && json[pos] == ']') {
            pos++;  // Skip ]
            return array;
        }
        
        while (true) {
                ConfigValue value = parseJSONValue(json, pos);
                if (value.index() == static_cast<size_t>(-1)) {
                    return std::vector<ConfigValue>();  // Error
                }
                array.push_back(value);
                
                skipWhitespace(json, pos);
                if (pos < json.size() && json[pos] == ',') {
                    pos++;  // Skip ,
                    skipWhitespace(json, pos);
                    continue;
                } else if (pos < json.size() && json[pos] == ']') {
                    pos++;  // Skip ]
                    return array;
                } else {
                    return std::vector<ConfigValue>();  // Error
                }
            }
    }
    
    return std::string();  // Error or empty
}

// Skip whitespace in JSON
void Config::skipWhitespace(const std::string& json, size_t& pos) {
    while (pos < json.size() && std::isspace(json[pos])) {
        pos++;
    }
}

// Helper function to get variant index (since C++23 doesn't have variant_npos)
size_t getVariantIndex(const ConfigValue& value) {
    return value.index();
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

    // Write header comment
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n\n";

    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        // Write key=value pairs
        file << entry.key << " = ";
        
        // Visit the variant to get the actual value
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                // For scalar types (int, int64_t, double, bool)
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
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

// Trim whitespace from both ends of a string
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Convert string to lowercase
static std::string toLower(const std::string& str) {
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
