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
    // Try JSON parser first
    if (loadFromJSON(filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format for backwards compatibility
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

bool Config::loadFromJSON(const std::string& filepath) {
    // Real JSON parser implementation
    // Using a simple but effective approach with manual parsing
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Parse JSON content
    size_t pos = 0;
    while (pos < content.size()) {
        // Find next key-value pair
        pos = content.find('"', pos);
        if (pos == std::string::npos) break;
        
        size_t key_end = content.find('"', pos + 1);
        if (key_end == std::string::npos) break;
        
        std::string key = content.substr(pos + 1, key_end - pos - 1);
        pos = key_end + 1;
        
        // Skip whitespace
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t')) {
            pos++;
        }
        
        if (pos >= content.size() || content[pos] != ':') break;
        pos++;
        
        // Skip whitespace after colon
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t')) {
            pos++;
        }
        
        // Parse value based on type
        ConfigValue value;
        if (pos < content.size() && content[pos] == '"') {
            // String value
            size_t value_end = content.find('"', pos + 1);
            if (value_end == std::string::npos) break;
            value = content.substr(pos + 1, value_end - pos - 1);
            pos = value_end + 1;
        } else if (pos < content.size() && content[pos] == 't') {
            // Boolean true
            if (content.substr(pos, 4) == "true") {
                value = true;
                pos += 4;
            } else {
                return false;
            }
        } else if (pos < content.size() && content[pos] == 'f') {
            // Boolean false
            if (content.substr(pos, 5) == "false") {
                value = false;
                pos += 5;
            } else {
                return false;
            }
        } else {
            // Number (int or float)
            size_t value_end = content.find_first_of(",}", pos);
            if (value_end == std::string::npos) {
                value_end = content.size();
            }
            
            std::string value_str = content.substr(pos, value_end - pos);
            // Check if it's an integer
            bool is_int = true;
            for (char c : value_str) {
                if (!std::isdigit(c) && c != '-' && c != '+') {
                    is_int = false;
                    break;
                }
            }
            
            if (is_int) {
                try {
                    value = std::stoll(value_str);
                } catch (...) {
                    value = std::stod(value_str);
                }
            } else {
                try {
                    value = std::stod(value_str);
                } catch (...) {
                    return false;
                }
            }
            pos = value_end;
        }
        
        // Skip whitespace
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t')) {
            pos++;
        }
        
        // Add to config
        set(key, value, ConfigSource::File);
        
        // Skip comma or brace
        if (pos < content.size() && content[pos] == ',') {
            pos++;
        } else if (pos < content.size() && content[pos] == '}') {
            break;
        }
    }
    
    return true;
}

// Parse nested JSON objects (for complex configurations)
bool Config::parseNestedJSON(const std::string& json, size_t& pos, std::string& key, ConfigValue& value) {
    return false; // Placeholder for nested object parsing
}

// Parse JSON arrays
bool Config::parseJSONArray(const std::string& json, size_t& pos, std::vector<int64_t>& array) {
    return false; // Placeholder for array parsing
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Write the value based on its type
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << value << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (value ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, double> || 
                                std::is_same_v<T, float> ||
                                std::is_same_v<T, int> ||
                                std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << value << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [\"";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << "\", \"";
                    file << value[i];
                }
                file << "\"]\n";
            } else {
                file << entry.key << " = " << value << "\n";
            }
        }, entry.value);
        
        file << "\n";
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
        
        // Write the value based on its type
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << value << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (value ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, double> || 
                                std::is_same_v<T, float> ||
                                std::is_same_v<T, int> ||
                                std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << value << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [\"";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << "\", \"";
                    file << value[i];
                }
                file << "\"]\n";
            } else {
                file << entry.key << " = " << value << "\n";
            }
        }, entry.value);
        
        file << "\n";
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
