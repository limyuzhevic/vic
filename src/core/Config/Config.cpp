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

bool Config::loadFromFile(const std::string& filepath) {
    // Implement proper JSON/YAML parser (Phase 2)
    // For now, support both JSON and key=value formats
    
    // Try to detect if it's a JSON file (contains { or starts with [)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Check if it looks like JSON (contains {, [, or " followed by :, at beginning or around content)
    bool looksLikeJSON = false;
    
    // Remove whitespace for checking
    std::string compact;
    for (char c : content) {
        if (!std::isspace(c)) compact += c;
    }
    
    // Check for JSON patterns
    if (!compact.empty()) {
        if (compact[0] == '{' || compact[0] == '[') {
            looksLikeJSON = true;
        } else {
            // Check for key:value pairs (JSON style)
            size_t pos = compact.find('"');
            while (pos != std::string::npos) {
                size_t endQuote = compact.find('"', pos + 1);
                if (endQuote != std::string::npos) {
                    std::string key = compact.substr(pos + 1, endQuote - pos - 1);
                    size_t colonPos = compact.find(':', endQuote);
                    if (colonPos != std::string::npos) {
                        // Check if next non-whitespace is { or [
                        size_t next = colonPos + 1;
                        while (next < compact.size() && std::isspace(compact[next])) next++;
                        if (next < compact.size() && (compact[next] == '{' || compact[next] == '[')) {
                            looksLikeJSON = true;
                            break;
                        }
                    }
                }
                pos = compact.find('"', endQuote + 1);
            }
        }
    }
    
    if (looksLikeJSON) {
        // Try to parse as JSON
        return loadFromJSONFile(filepath);
    } else {
        // Parse as simple key=value format (Phase 1)
        return loadFromKeyValueFile(filepath);
    }
}

bool Config::loadFromKeyValueFile(const std::string& filepath) {
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

// Simple JSON parser placeholder (Phase 2)
bool Config::loadFromJSONFile(const std::string& filepath) {
    // TODO: Implement proper JSON parsing with nlohmann/json or similar
    // For now, return false to fall back to key=value format
    // This is a placeholder implementation
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Simple heuristic check for JSON structure
    // This is a very basic implementation - real JSON parsing should be done with a proper library
    bool isValidJSON = false;
    
    // Basic JSON structure checks
    if (!content.empty()) {
        // Check for balanced braces/brackets
        int braceCount = 0;
        int bracketCount = 0;
        bool inString = false;
        char prevChar = '\0';
        
        for (char c : content) {
            if (c == '"' && prevChar != '\\') {
                inString = !inString;
            } else if (!inString) {
                if (c == '{') braceCount++;
                else if (c == '}') braceCount--;
                else if (c == '[') bracketCount++;
                else if (c == ']') bracketCount--;
            }
            prevChar = c;
        }
        
        if ((braceCount == 0 && bracketCount == 0) || (braceCount == 1 && bracketCount == 0) || 
            (braceCount == 0 && bracketCount == 1)) {
            isValidJSON = true;
        }
    }
    
    if (!isValidJSON) {
        // Fall back to key=value format
        return loadFromKeyValueFile(filepath);
    }
    
    // TODO: Implement actual JSON parsing
    // For now, just log and treat as plain text
    NLM_LOG_WARNING("JSON file loaded but parser not fully implemented: " + filepath);
    
    // For now, try to extract key-value pairs from JSON-like content
    // This is a very simple approach and won't work for all JSON
    
    // Extract key-value pairs from content
    size_t pos = 0;
    while (pos < content.size()) {
        // Find a key (string in quotes followed by :)
        pos = content.find('"', pos);
        if (pos == std::string::npos) break;
        
        size_t endKey = content.find('"', pos + 1);
        if (endKey == std::string::npos) break;
        
        std::string key = content.substr(pos + 1, endKey - pos - 1);
        pos = endKey + 1;
        
        // Skip whitespace
        while (pos < content.size() && std::isspace(content[pos])) pos++;
        
        // Check for colon
        if (pos < content.size() && content[pos] == ':') {
            pos++;
            
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            
            // Extract value
            if (content[pos] == '"') {
                // String value
                size_t endValue = content.find('"', pos + 1);
                if (endValue != std::string::npos) {
                    std::string value = content.substr(pos + 1, endValue - pos - 1);
                    set(key, value, ConfigSource::File);
                    pos = endValue + 1;
                }
            } else if (content[pos] == '{' || content[pos] == '[') {
                // Complex value - for now skip
                pos++;
                // Skip until matching closing brace/bracket
                int depth = 1;
                while (pos < content.size() && depth > 0) {
                    if (content[pos] == '{' || content[pos] == '[') depth++;
                    else if (content[pos] == '}' || content[pos] == ']') depth--;
                    pos++;
                }
            } else {
                // Simple value (number, boolean, null)
                size_t endValue = pos;
                while (endValue < content.size() && content[endValue] != ',' && content[endValue] != '}' && content[endValue] != ']') {
                    endValue++;
                }
                std::string value = content.substr(pos, endValue - pos);
                trim(value);
                set(key, value, ConfigSource::File);
                pos = endValue;
            }
            
            // Skip comma
            while (pos < content.size() && (std::isspace(content[pos]) || content[pos] == ',')) pos++;
        }
    }
    
    return !pImpl->entries.empty();
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
