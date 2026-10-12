#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first (Phase 2 feature)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check if file looks like JSON (starts with {)
    char firstChar;
    file.get(firstChar);
    file.clear();
    file.seekg(0);
    
    if (firstChar == '{') {
        // Try JSON parsing
        try {
            return parseJsonFromFile(filepath);
        } catch (const std::exception&) {
            // Fall back to simple format
            file.clear();
            file.seekg(0);
        }
    }
    
    // Parse simple key=value format
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

bool Config::parseJsonFromFile(const std::string& filepath) {
    // Simple JSON parser for configuration files
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), 
                        std::istreambuf_iterator<char>());
    
    // Basic JSON parsing - parse object keys and values
    size_t pos = content.find('{');
    if (pos == std::string::npos) {
        return false;
    }
    
    pos++; // Skip {
    size_t end = content.rfind('}');
    if (end == std::string::npos) {
        return false;
    }
    
    std::string object = content.substr(pos, end - pos);
    
    // Parse key-value pairs
    size_t keyStart = 0;
    while (keyStart < object.size()) {
        // Find next key
        size_t colon = object.find(':', keyStart);
        if (colon == std::string::npos) {
            break;
        }
        
        // Find key start
        size_t quote = object.rfind('"', colon - 1);
        if (quote == std::string::npos) {
            break;
        }
        
        std::string key = object.substr(quote + 1, colon - quote - 2);
        
        // Find value
        size_t valueStart = object.find_first_not_of(' ', colon + 1);
        if (valueStart == std::string::npos) {
            break;
        }
        
        // Parse value based on type
        if (object[valueStart] == '"') {
            // String value
            size_t valueEnd = object.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) {
                break;
            }
            std::string value = object.substr(valueStart + 1, valueEnd - valueStart - 1);
            set(key, value, ConfigSource::File);
            keyStart = valueEnd + 2;
        } else if (object[valueStart] == 't' || object[valueStart] == 'f') {
            // Boolean value
            if (object.substr(valueStart, 5) == "true") {
                set(key, true, ConfigSource::File);
                keyStart = valueStart + 4;
            } else if (object.substr(valueStart, 4) == "false") {
                set(key, false, ConfigSource::File);
                keyStart = valueStart + 5;
            } else {
                keyStart = colon + 1;
            }
        } else if (object[valueStart] == '-' || std::isdigit(object[valueStart])) {
            // Number (int or double)
            size_t numEnd = valueStart;
            while (numEnd < object.size() && (std::isdigit(object[numEnd]) || 
                                              object[numEnd] == '.' || 
                                              object[numEnd] == '-' || 
                                              object[numEnd] == 'e' || 
                                              object[numEnd] == 'E')) {
                numEnd++;
            }
            std::string numStr = object.substr(valueStart, numEnd - valueStart);
            
            // Try to parse as double first
            try {
                double doubleVal = std::stod(numStr);
                // Check if it's an integer
                if (numStr.find('.') == std::string::npos && numStr.find('e') == std::string::npos && 
                    numStr.find('E') == std::string::npos) {
                    // Try to parse as int64_t
                    try {
                        int64_t intVal = std::stoll(numStr);
                        set(key, intVal, ConfigSource::File);
                    } catch (...) {
                        set(key, doubleVal, ConfigSource::File);
                    }
                } else {
                    set(key, doubleVal, ConfigSource::File);
                }
            } catch (...) {
                // Fall back to string
                set(key, numStr, ConfigSource::File);
            }
            
            keyStart = numEnd;
        } else {
            // Skip unknown value
            keyStart = colon + 1;
        }
        
        // Skip comma and whitespace
        keyStart = object.find_first_not_of(", \t\n\r", keyStart);
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
    
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format to avoid scientific notation
                file.precision(6);
                file << std::fixed << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "["; // TODO: Proper array serialization
                // For now, skip arrays
                file << "\]";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    file << "}\n";
    
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

// Helper functions for JSON parsing
std::string Config::extractJsonString(const std::string& text, size_t startPos, size_t& endPos) {
    size_t quote = text.find('"', startPos);
    if (quote == std::string::npos) {
        endPos = text.size();
        return "";
    }
    
    size_t valueEnd = text.find('"', quote + 1);
    if (valueEnd == std::string::npos) {
        endPos = text.size();
        return "";
    }
    
    endPos = valueEnd + 1;
    return text.substr(quote + 1, valueEnd - quote - 1);
}

std::string Config::extractJsonNumber(const std::string& text, size_t startPos, size_t& endPos) {
    size_t numEnd = startPos;
    while (numEnd < text.size() && (std::isdigit(text[numEnd]) || 
                                    text[numEnd] == '.' || 
                                    text[numEnd] == '-' || 
                                    text[numEnd] == 'e' || 
                                    text[numEnd] == 'E')) {
        numEnd++;
    }
    
    endPos = numEnd;
    return text.substr(startPos, numEnd - startPos);
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
