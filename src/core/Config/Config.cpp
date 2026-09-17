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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first, then fallback to simple key=value format
    std::string line;
    bool hasJsonBraces = false;
    std::vector<std::string> jsonLines;
    
    // Check if this is a JSON file (starts with '{')
    if (file.peek() == '{') {
        hasJsonBraces = true;
        // Read entire file content for JSON parsing
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        // Simple JSON parser - only handles basic key=value pairs in JSON format
        std::string content = buffer.str();
        size_t pos = 0;
        while (pos < content.length()) {
            pos = content.find('"', pos);
            if (pos == std::string::npos) break;
            
            size_t keyEnd = content.find('"', pos + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(pos + 1, keyEnd - pos - 1);
            pos = content.find(':', keyEnd);
            if (pos == std::string::npos) break;
            
            // Skip whitespace after colon
            pos++;
            while (pos < content.length() && std::isspace(content[pos])) pos++;
            
            // Parse JSON value
            if (content[pos] == '"') {
                size_t valEnd = content.find('"', pos + 1);
                if (valEnd == std::string::npos) break;
                std::string value = content.substr(pos + 1, valEnd - pos - 1);
                set(key, value, ConfigSource::File);
                pos = valEnd + 1;
            } else if (content[pos] == 't' || content[pos] == 'f') {
                // Boolean value (true/false)
                size_t valEnd = content.find_first_of(",}", pos);
                if (valEnd == std::string::npos) break;
                std::string valStr = content.substr(pos, valEnd - pos);
                bool boolVal = (valStr == "true");
                set(key, boolVal, ConfigSource::File);
                pos = valEnd;
            } else if (content[pos] == 'n') {
                // null value
                set(key, std::string("null"), ConfigSource::File);
                pos++;
                while (pos < content.length() && content[pos] != ',' && content[pos] != '}') pos++;
            } else if (content[pos] == '0' || content[pos] == '1' || content[pos] == '2' ||
                      content[pos] == '3' || content[pos] == '4' || content[pos] == '5' ||
                      content[pos] == '6' || content[pos] == '7' || content[pos] == '8' ||
                      content[pos] == '9' || content[pos] == '-') {
                // Number (integer or float)
                size_t valEnd = content.find_first_of(",}", pos);
                if (valEnd == std::string::npos) break;
                std::string numStr = content.substr(pos, valEnd - pos);
                try {
                    // Try integer first
                    size_t posAfter;
                    int64_t intVal = std::stoll(numStr, &posAfter);
                    if (posAfter == numStr.length()) {
                        set(key, intVal, ConfigSource::File);
                    } else {
                        // Try float
                        double floatVal = std::stod(numStr);
                        set(key, floatVal, ConfigSource::File);
                    }
                } catch (...) {
                    // Keep as string if parsing fails
                    set(key, numStr, ConfigSource::File);
                }
                pos = valEnd;
            }
            
            // Skip to next key or end
            while (pos < content.length() && content[pos] != '"') pos++;
        }
    }
    
    // If not JSON, try simple key=value format
    if (!hasJsonBraces) {
        file.clear();
        file.seekg(0);
        
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
                
                // Parse boolean values
                if (value == "true") {
                    set(key, true, ConfigSource::File);
                } else if (value == "false") {
                    set(key, false, ConfigSource::File);
                } else if (value == "null" || value == "NULL") {
                    set(key, std::string("null"), ConfigSource::File);
                } else {
                    // Try to parse as number (integer or float)
                    size_t posAfter;
                    bool parsed = false;
                    
                    // Try integer
                    try {
                        int64_t intVal = std::stoll(value, &posAfter);
                        if (posAfter == value.length()) {
                            set(key, intVal, ConfigSource::File);
                            parsed = true;
                        }
                    } catch (...) {}
                    
                    if (!parsed) {
                        // Try float
                        try {
                            double floatVal = std::stod(value);
                            set(key, floatVal, ConfigSource::File);
                            parsed = true;
                        } catch (...) {}
                    }
                    
                    if (!parsed) {
                        // Keep as string
                        set(key, value, ConfigSource::File);
                    }
                }
            }
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
