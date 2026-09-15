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
    
    // Try to parse as JSON first
    file.clear();
    file.seekg(0, std::ios::beg);
    
    // Simple JSON parser (lightweight, no external dependencies)
    std::string content((std::istreambuf_iterator<char>(file)), 
                       std::istreambuf_iterator<char>());
    file.close();
    
    if (content.empty()) {
        return true; // Empty file is OK
    }
    
    // Check if it looks like JSON (starts with { or [)
    bool isJson = false;
    size_t i = 0;
    while (i < content.size() && std::isspace(content[i])) i++;
    if (i < content.size() && (content[i] == '{' || content[i] == '[')) {
        isJson = true;
    }
    
    if (isJson) {
        // Parse JSON format
        return loadFromJson(content);
    } else {
        // Parse simple key=value format (backward compatibility)
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line)) {
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
    
    // Save in simple key=value format with comments
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        // Convert value back to string based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        file << "\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> parseJsonValue(const std::string& jsonStr) {
    // Simple JSON value parser for basic types
    // Remove whitespace
    std::string trimmed = jsonStr;
    size_t start = trimmed.find_first_not_of(" \t\r\n");
    size_t end = trimmed.find_last_not_of(" \t\r\n");
    if (start == std::string::npos || end == std::string::npos) {
        return std::nullopt;
    }
    trimmed = trimmed.substr(start, end - start + 1);
    
    try {
        if constexpr (std::is_same_v<T, std::string>) {
            if (trimmed.size() >= 2 && trimmed.front() == '"' && trimmed.back() == '"') {
                return trimmed.substr(1, trimmed.size() - 2);
            }
            return trimmed;
        } else if constexpr (std::is_same_v<T, bool>) {
            if (trimmed == "true" || trimmed == "1") return true;
            if (trimmed == "false" || trimmed == "0") return false;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            size_t pos;
            int64_t val = std::stoll(trimmed, &pos);
            if (pos != trimmed.size()) throw std::invalid_argument("Extra characters");
            return static_cast<T>(val);
        } else if constexpr (std::is_same_v<T, double>) {
            size_t pos;
            double val = std::stod(trimmed, &pos);
            if (pos != trimmed.size()) throw std::invalid_argument("Extra characters");
            return val;
        }
    } catch (const std::exception&) {
        // Return nullopt on parse error
    }
    return std::nullopt;
}

bool Config::loadFromJson(const std::string& jsonStr) {
    // Simple JSON parser for basic structures (objects and arrays)
    std::istringstream stream(jsonStr);
    std::string line;
    std::string currentKey;
    bool inObject = false;
    bool inArray = false;
    
    clear(); // Start fresh
    
    while (std::getline(stream, line)) {
        // Remove whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue; // Empty line
        
        line = line.substr(start);
        
        // Check for opening/closing braces and commas
        size_t bracePos = line.find('{');
        size_t bracketPos = line.find('[');
        size_t closeBracePos = line.find('}');
        size_t closeBracketPos = line.find(']');
        
        if (bracePos != std::string::npos) inObject = true;
        if (bracketPos != std::string::npos) inArray = true;
        
        if (closeBracePos != std::string::npos) inObject = false;
        if (closeBracketPos != std::string::npos) inArray = false;
        
        // Skip if we're inside nested structures
        if (inObject || inArray) continue;
        
        // Skip comments
        size_t commentPos = line.find("//");
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
            // Remove trailing whitespace
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
        }
        
        // Skip braces
        if (line.empty() || line[0] == '{' || line[0] == '}') continue;
        
        // Parse key:value pair
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;
        
        std::string key = trim(line.substr(0, colonPos));
        std::string valueStr = trim(line.substr(colonPos + 1));
        
        // Remove trailing comma if present
        if (!valueStr.empty() && valueStr.back() == ',') {
            valueStr.pop_back();
            valueStr = trim(valueStr);
        }
        
        // Try to determine value type and parse
        auto intVal = parseJsonValue<int>(valueStr);
        if (intVal.has_value()) {
            set(key, *intVal, ConfigSource::File);
            continue;
        }
        
        auto doubleVal = parseJsonValue<double>(valueStr);
        if (doubleVal.has_value()) {
            set(key, *doubleVal, ConfigSource::File);
            continue;
        }
        
        auto boolVal = parseJsonValue<bool>(valueStr);
        if (boolVal.has_value()) {
            set(key, *boolVal, ConfigSource::File);
            continue;
        }
        
        auto stringVal = parseJsonValue<std::string>(valueStr);
        if (stringVal.has_value()) {
            set(key, *stringVal, ConfigSource::File);
            continue;
        }
        
        // If all parsing failed, store as string
        set(key, valueStr, ConfigSource::File);
    }
    
    return true;
}

// Helper function to parse nested JSON structures
std::string Config::extractStringValue(const std::string& jsonStr) {
    std::string result;
    bool inQuotes = false;
    bool escapeNext = false;
    
    for (char c : jsonStr) {
        if (escapeNext) {
            result += c;
            escapeNext = false;
        } else if (c == '\\') {
            escapeNext = true;
        } else if (c == '"') {
            inQuotes = !inQuotes;
        } else if (!inQuotes && (c == ' ' || c == '\t' || c == '\n' || c == '\r')) {
            // Skip whitespace outside quotes
            continue;
        } else {
            result += c;
        }
    }
    return result;
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