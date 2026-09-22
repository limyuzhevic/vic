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
    
    // Read entire file to check if it's JSON/YAML format
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.clear();
    file.seekg(0);
    
    // Check for JSON format (starts with { or [)
    bool isJson = false;
    bool isYaml = false;
    
    // Simple JSON detection
    if (!content.empty() && (content[0] == '{' || content[0] == '[')) {
        isJson = true;
    }
    
    // Simple YAML detection (basic check for key: value pattern)
    // YAML often has indentation and colon separators
    size_t colonPos = content.find(':');
    if (colonPos != std::string::npos && colonPos < content.find('\n')) {
        // Check if it looks like YAML key-value
        size_t lineEnd = content.find('\n', 0);
        if (lineEnd != std::string::npos && lineEnd > 10) {
            std::string firstLine = content.substr(0, lineEnd);
            if (firstLine.find(':') != std::string::npos) {
                isYaml = true;
            }
        }
    }
    
    try {
        if (isJson) {
            // Parse as JSON using nlohmann::json
            auto json = nlohmann::json::parse(content);
            clear(); // Clear existing entries
            
            for (auto it = json.begin(); it != json.end(); ++it) {
                std::string key = it.key();
                ConfigValue value;
                
                // Convert JSON value to ConfigValue
                if (it.value().is_string()) {
                    value = it.value().get<std::string>();
                } else if (it.value().is_number_integer()) {
                    value = it.value().get<int64_t>();
                } else if (it.value().is_number_float()) {
                    value = it.value().get<double>();
                } else if (it.value().is_boolean()) {
                    value = it.value().get<bool>();
                } else if (it.value().is_array()) {
                    // Handle arrays
                    if (!it.value().empty() && it.value()[0].is_string()) {
                        value = it.value().get<std::vector<std::string>>();
                    } else if (!it.value().empty() && it.value()[0].is_number_integer()) {
                        value = it.value().get<std::vector<int64_t>>();
                    } else if (!it.value().empty() && it.value()[0].is_number_float()) {
                        value = it.value().get<std::vector<double>>();
                    }
                }
                
                set(key, value, ConfigSource::File);
            }
            return true;
        } else if (isYaml) {
            // Simple YAML parser (basic key: value pairs)
            clear(); // Clear existing entries
            
            std::istringstream stream(content);
            std::string line;
            
            while (std::getline(stream, line)) {
                // Trim whitespace
                line = trim(line);
                
                // Skip empty lines and comments
                if (line.empty() || line[0] == '#' || line[0] == '%') {
                    continue;
                }
                
                // Look for key: value pattern
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos && colonPos > 0) {
                    std::string key = trim(line.substr(0, colonPos));
                    std::string valueStr = trim(line.substr(colonPos + 1));
                    
                    // Remove quotes if present
                    if (valueStr.size() >= 2 && 
                        ((valueStr.front() == '"' && valueStr.back() == '"') ||
                         (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                        valueStr = valueStr.substr(1, valueStr.size() - 2);
                    }
                    
                    // Try to parse value as different types
                    bool parsed = false;
                    ConfigValue value;
                    
                    // Try boolean
                    if (valueStr == "true") {
                        value = true;
                        parsed = true;
                    } else if (valueStr == "false") {
                        value = false;
                        parsed = true;
                    }
                    
                    // Try integer
                    if (!parsed) {
                        try {
                            size_t pos;
                            int64_t intVal = std::stoll(valueStr, &pos);
                            if (pos == valueStr.size()) {
                                value = intVal;
                                parsed = true;
                            }
                        } catch (...) {}
                    }
                    
                    // Try double
                    if (!parsed) {
                        try {
                            size_t pos;
                            double doubleVal = std::stod(valueStr, &pos);
                            if (pos == valueStr.size()) {
                                value = doubleVal;
                                parsed = true;
                            }
                        } catch (...) {}
                    }
                    
                    // Use as string if not parsed as other type
                    if (!parsed) {
                        value = valueStr;
                    }
                    
                    set(key, value, ConfigSource::File);
                }
            }
            return true;
        }
    } catch (...) {
        // If JSON parsing fails, fall back to simple parser
    }
    
    // Simple key=value parser for backward compatibility
    clear(); // Clear existing entries
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
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
