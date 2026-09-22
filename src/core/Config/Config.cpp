#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Configuration created");
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    NLM_LOG_INFO("Loading configuration from file: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
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
            
            // Parse value based on content
            bool success = setFromString(key, value);
            if (!success) {
                NLM_LOG_WARNING("Failed to parse value for key " + key + " at line " + std::to_string(lineNum) +
                              ": " + value);
            }
        } else {
            NLM_LOG_WARNING("Invalid configuration line (no '=') at line " + std::to_string(lineNum) +
                          ": " + line);
        }
    }
    
    NLM_LOG_INFO("Configuration loaded successfully: " + std::to_string(pImpl->entries.size()) + " entries");
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Loading configuration from command line arguments");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                bool success = setFromString(key, value);
                if (!success) {
                    NLM_LOG_WARNING("Failed to parse command line argument --" + key + "=" + value);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            bool success = setFromString(key, value);
            if (!success) {
                NLM_LOG_WARNING("Failed to parse command line argument -" + key + " " + value);
            }
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    NLM_LOG_INFO("Saving configuration to file: " + filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Properly format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format double without unnecessary trailing zeros
                std::stringstream ss;
                ss.precision(6);
                ss << std::fixed << arg;
                file << ss.str();
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    file << "# End of configuration\n";
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
        NLM_LOG_WARNING("Type mismatch when getting key " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    } else {
        NLM_LOG_DEBUG("Using default value for key " + key + ": " + std::to_string(defaultValue));
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated configuration key: " + key);
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_INFO("Added new configuration key: " + key);
    }
}

bool Config::setFromString(const std::string& key, const std::string& value) {
    // Try different types in order
    try {
        if (value == "true" || value == "false") {
            set(key, std::string(value) == "true", ConfigSource::File);
            return true;
        } else if (value.find('.') != std::string::npos) {
            // Try double first
            size_t pos;
            double doubleVal = std::stod(value, &pos);
            if (pos == value.size()) {
                set(key, doubleVal, ConfigSource::File);
                return true;
            }
        } else {
            // Try int64_t
            size_t pos;
            int64_t intVal = std::stoll(value, &pos);
            if (pos == value.size()) {
                set(key, intVal, ConfigSource::File);
                return true;
            }
        }
    } catch (const std::exception&) {
        // Value doesn't match any expected type
    }
    
    // Default to string
    set(key, value, ConfigSource::File);
    return true;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    NLM_LOG_INFO("Removing configuration key: " + key);
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
    NLM_LOG_INFO("Clearing all configuration entries");
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
