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
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // Try JSON first
    if (loadFromJSON(content, ConfigSource::File)) {
        return true;
    }
    
    // Try YAML second
    if (loadFromYAML(content, ConfigSource::File)) {
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

// Load from command line arguments
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

// Proper JSON parser implementation (simplified for NLM)
bool Config::loadFromJSON(const std::string& content, ConfigSource source) {
    // Simple JSON parser - handles basic key-value pairs
    // This is a simplified implementation for NLM
    
    std::istringstream stream(content);
    std::string line;
    bool in_object = false;
    size_t brace_count = 0;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        
        // Skip comments
        size_t comment_pos = line.find("//");
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
            line = trim(line);
        }
        
        // Track braces for object parsing
        for (char c : line) {
            if (c == '{') {
                brace_count++;
                in_object = true;
            } else if (c == '}') {
                brace_count--;
            }
        }
        
        if (!in_object) continue;
        
        // Simple parsing for key-value pairs (key: value format)
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = trim(line.substr(0, colon_pos));
            std::string value = trim(line.substr(colon_pos + 1));
            
            // Remove quotes from key
            if (key.size() >= 2 && key.front() == '"' && key.back() == '"') {
                key = key.substr(1, key.size() - 2);
            }
            
            // Remove trailing comma
            if (!value.empty() && value.back() == ',') {
                value = value.substr(0, value.size() - 1);
                value = trim(value);
            }
            
            // Convert string to appropriate type
            try {
                if (value.front() == '"' && value.back() == '"') {
                    // String value
                    set(key, value.substr(1, value.size() - 2), source);
                } else if (value == "true" || value == "false") {
                    // Boolean value
                    bool bool_val = (value == "true");
                    set(key, bool_val, source);
                } else if (value.find('.') != std::string::npos) {
                    // Double value
                    double double_val = std::stod(value);
                    set(key, double_val, source);
                } else {
                    // Integer value
                    int int_val = std::stoi(value);
                    set(key, int_val, source);
                }
            } catch (const std::exception&) {
                // If conversion fails, treat as string
                set(key, value, source);
            }
        }
    }
    
    return true;
}

// Proper YAML parser implementation (simplified for NLM)
bool Config::loadFromYAML(const std::string& content, ConfigSource source) {
    // Simple YAML parser - handles basic key-value pairs
    // This is a simplified implementation for NLM
    
    std::istringstream stream(content);
    std::string line;
    bool in_document = false;
    size_t indent_level = 0;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Calculate indentation
        size_t spaces = line.find_first_not_of(' ');
        if (spaces == std::string::npos) continue;
        
        // Skip list items (starting with -) for now
        if (line[spaces] == '-') {
            continue;
        }
        
        // Check if this is a key-value pair (contains ':')
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos && colon_pos > spaces) {
            std::string key = trim(line.substr(spaces, colon_pos - spaces));
            std::string value = trim(line.substr(colon_pos + 1));
            
            // Remove quotes from key
            if (key.size() >= 2 && key.front() == '"' && key.back() == '"') {
                key = key.substr(1, key.size() - 2);
            }
            
            // Remove trailing comma
            if (!value.empty() && value.back() == ',') {
                value = value.substr(0, value.size() - 1);
                value = trim(value);
            }
            
            // Convert string to appropriate type
            try {
                if (value.front() == '"' && value.back() == '"') {
                    // String value
                    set(key, value.substr(1, value.size() - 2), source);
                } else if (value == "true" || value == "false") {
                    // Boolean value
                    bool bool_val = (value == "true");
                    set(key, bool_val, source);
                } else if (value.find('.') != std::string::npos) {
                    // Double value
                    double double_val = std::stod(value);
                    set(key, double_val, source);
                } else {
                    // Integer value
                    int int_val = std::stoi(value);
                    set(key, int_val, source);
                }
            } catch (const std::exception&) {
                // If conversion fails, treat as string
                set(key, value, source);
            }
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write in JSON format (modern, readable)
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        // Write value based on type
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format double without trailing zeros
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << arg[j];
                    if (j < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    // Format double without trailing zeros
                    file << arg[j];
                    if (j < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << "\"" << arg[j] << "\"";
                    if (j < arg.size() - 1) file << ", ";
                }
                file << "]";
            }
        }, entry.value);
        
        // Add comma except for last entry
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    file << "}\n";
    
    return true;
}

const ConfigEntry& Config::getEntry(size_t index) const {
    if (index >= pImpl->entries.size()) {
        throw std::out_of_range("Entry index out of range");
    }
    return pImpl->entries[index];
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    return ConfigSource::Default;
}

// Set values
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

} // namespace nlm
