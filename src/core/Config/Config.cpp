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
    // Load configuration from JSON file
    // For now, support both simple key=value format and JSON format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try to parse as JSON first
    try {
        // Check if it's likely JSON (starts with { or [)
        if (!content.empty() && (content.find('{') == 0 || content.find('[') == 0)) {
            return loadFromJSON(content);
        }
    } catch (...) {
        // JSON parsing failed, fall back to simple format
    }
    
    // Fallback to simple key=value format
    file.clear();
    file.seekg(0);
    
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

bool Config::loadFromJSON(const std::string& jsonContent) {
    // Simple JSON parsing for basic configuration
    // This is a minimal implementation that handles common JSON structures
    // For production, consider using a proper JSON library
    
    // Remove whitespace and comments
    std::string cleaned;
    bool inString = false;
    char quoteChar = 0;
    
    for (size_t i = 0; i < jsonContent.size(); ++i) {
        char c = jsonContent[i];
        
        // Skip comments
        if (!inString && c == '/' && i + 1 < jsonContent.size() && 
            (jsonContent[i + 1] == '/' || jsonContent[i + 1] == '*')) {
            while (i + 1 < jsonContent.size() && 
                   !(jsonContent[i + 1] == '\n' && jsonContent[i] == '*')) {
                i++;
            }
            continue;
        }
        
        if (!inString && (c == ' ' || c == '\t' || c == '\n' || c == '\r')) {
            // Skip whitespace
            continue;
        }
        
        if (c == '"' || c == '\'') {
            if (!inString) {
                inString = true;
                quoteChar = c;
                cleaned += c;
            } else if (quoteChar == c) {
                inString = false;
                quoteChar = 0;
                cleaned += c;
            } else {
                cleaned += c;
            }
        } else if (c == ',' && !inString) {
            cleaned += ' ';
        } else {
            cleaned += c;
        }
    }
    
    // Now parse the cleaned content
    // Very simple parser - for production use a proper JSON library
    size_t pos = 0;
    while (pos < cleaned.size()) {
        // Find next key (either "key": or key:)
        size_t keyPos = cleaned.find('"', pos);
        if (keyPos == std::string::npos) {
            keyPos = cleaned.find_first_of(':', pos);
        }
        
        if (keyPos == std::string::npos) {
            break;
        }
        
        // Extract key
        std::string key;
        size_t valuePos;
        
        if (cleaned[keyPos] == '"') {
            // JSON quoted key
            size_t endQuote = cleaned.find('"', keyPos + 1);
            if (endQuote == std::string::npos) break;
            key = cleaned.substr(keyPos + 1, endQuote - keyPos - 1);
            valuePos = cleaned.find(':', endQuote);
            if (valuePos == std::string::npos) break;
        } else {
            // Unquoted key (simplified)
            size_t endKey = cleaned.find(':', keyPos);
            if (endKey == std::string::npos) break;
            key = cleaned.substr(keyPos, endKey - keyPos);
            valuePos = endKey;
        }
        
        // Skip whitespace after key
        while (valuePos < cleaned.size() && 
               (cleaned[valuePos] == ' ' || cleaned[valuePos] == '\t' || 
                cleaned[valuePos] == '\n' || cleaned[valuePos] == '\r')) {
            valuePos++;
        }
        
        if (valuePos >= cleaned.size() || cleaned[valuePos] != ':') {
            break;
        }
        
        valuePos++; // Skip colon
        
        // Skip whitespace after colon
        while (valuePos < cleaned.size() && 
               (cleaned[valuePos] == ' ' || cleaned[valuePos] == '\t' || 
                cleaned[valuePos] == '\n' || cleaned[valuePos] == '\r')) {
            valuePos++;
        }
        
        // Extract value (simplified)
        std::string value;
        size_t endValue = cleaned.find_first_of(",}", valuePos);
        if (endValue == std::string::npos) {
            endValue = cleaned.size();
        }
        
        value = cleaned.substr(valuePos, endValue - valuePos);
        
        // Trim value
        value = trim(value);
        
        // Remove surrounding quotes from value
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Parse numeric values
        bool isNumeric = false;
        double doubleVal = 0.0;
        if (std::isdigit(value[0]) || (value[0] == '-' && value.size() > 1 && std::isdigit(value[1]))) {
            try {
                size_t posAfter = 0;
                doubleVal = std::stod(value, &posAfter);
                if (posAfter == value.size()) {
                    isNumeric = true;
                }
            } catch (...) {
                isNumeric = false;
            }
        }
        
        // Parse boolean values
        bool isBool = false;
        bool boolVal = false;
        if (value == "true") {
            boolVal = true;
            isBool = true;
        } else if (value == "false") {
            boolVal = false;
            isBool = true;
        }
        
        // Set the configuration value
        if (isNumeric) {
            // Use double as base type and convert to appropriate type later
            set(key, doubleVal, ConfigSource::File);
        } else if (isBool) {
            set(key, boolVal, ConfigSource::File);
        } else {
            set(key, value, ConfigSource::File);
        }
        
        pos = endValue + 1;
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
        file << entry.key << " = ";
        
        // Serialize the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || 
                               std::is_same_v<T, int64_t> ||
                               std::is_same_v<T, double>) {
                file << arg;
            } else {
                // For other types (not currently supported), use string conversion
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
