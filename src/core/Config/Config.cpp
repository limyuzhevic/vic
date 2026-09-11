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
    
    // Try JSON format first (more modern)
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Try to parse as JSON
    if (parseJson(content)) {
        NLM_LOG_INFO("Successfully loaded configuration from JSON file: " + filepath);
        return true;
    }
    
    // Try YAML format
    std::ifstream file2(filepath);
    if (parseYaml(file2)) {
        NLM_LOG_INFO("Successfully loaded configuration from YAML file: " + filepath);
        return true;
    }
    
    // Fall back to simple key=value format
    file2.close();
    file2.open(filepath);
    
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
    
    if (!pImpl->entries.empty()) {
        NLM_LOG_INFO("Successfully loaded configuration from simple file: " + filepath);
        return true;
    }
    
    return false;
}

bool Config::parseJson(const std::string& content) {
    try {
        // Simple JSON parsing - handle objects with key-value pairs
        size_t pos = 0;
        while (pos < content.size()) {
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) {
                pos++;
            }
            
            if (pos >= content.size()) break;
            
            // Check for opening brace
            if (content[pos] == '{') {
                pos++;
                while (pos < content.size() && content[pos] != '}') {
                    // Parse key
                    while (pos < content.size() && std::isspace(content[pos])) {
                        pos++;
                    }
                    
                    if (pos >= content.size()) break;
                    
                    if (content[pos] == '"') {
                        size_t keyStart = ++pos;
                        while (pos < content.size() && content[pos] != '"') {
                            pos++;
                        }
                        std::string key = content.substr(keyStart, pos - keyStart);
                        
                        // Skip to colon
                        while (pos < content.size() && content[pos] != ':') {
                            pos++;
                        }
                        if (pos >= content.size()) break;
                        pos++;
                        
                        // Skip whitespace
                        while (pos < content.size() && std::isspace(content[pos])) {
                            pos++;
                        }
                        
                        // Parse value
                        size_t valueStart = pos;
                        if (content[pos] == '"') {
                            // String value
                            while (pos < content.size() && content[pos] != '"') {
                                pos++;
                            }
                            std::string value = content.substr(valueStart, pos - valueStart);
                            set(key, value, ConfigSource::File);
                            pos++;
                        } else if (content[pos] == 't' || content[pos] == 'f') {
                            // Boolean value
                            while (pos < content.size() && content[pos] != ',' && content[pos] != '}') {
                                pos++;
                            }
                            // For now, just skip boolean parsing
                            // TODO: Implement proper boolean parsing
                        } else if (content[pos] == 'n') {
                            // Null value
                            while (pos < content.size() && content[pos] != ',' && content[pos] != '}') {
                                pos++;
                            }
                        } else {
                            // Number or other
                            while (pos < content.size() && content[pos] != ',' && content[pos] != '}') {
                                pos++;
                            }
                        }
                        
                        // Skip comma or closing brace
                        if (pos < content.size() && content[pos] == ',') {
                            pos++;
                        }
                    }
                }
            } else {
                break;
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::parseYaml(std::ifstream& file) {
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line.substr(0, 2) == "# ") {
            continue;
        }
        
        // Try to parse YAML key: value format
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos && colonPos > 0) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Remove quotes from value if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Handle YAML list format [value1, value2, ...]
            if (value.size() >= 2 && value.front() == '[' && value.back() == ']') {
                // Parse as list - for now, treat as string
                set(key, value, ConfigSource::File);
            } else {
                set(key, value, ConfigSource::File);
            }
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
    
    // Write header comment
    file << "# NLM Configuration File\n";
    file << "# Generated on " << __DATE__ << " at " << __TIME__ << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Visit the variant to get the value as string
        std::visit([&file, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            file << entry.key << " = ";
            
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                // For primitive types (int, int64_t, double)
                file << arg;
            }
            file << "\n\n";
        }, entry.value);
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
