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
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Config file not found: " + filepath + " (using defaults)");
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
            
            // Try to parse different types in order
            bool parsed = false;
            
            // Try integer
            size_t numEnd;
            try {
                int64_t intVal = std::stoll(value, &numEnd);
                if (numEnd == value.size()) {
                    set(key, intVal, ConfigSource::File);
                    parsed = true;
                }
            } catch (...) {}
            
            // Try double if not integer
            if (!parsed) {
                try {
                    double doubleVal = std::stod(value, &numEnd);
                    if (numEnd == value.size()) {
                        set(key, doubleVal, ConfigSource::File);
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try boolean
            if (!parsed) {
                std::string lowerValue = toLower(value);
                if (lowerValue == "true" || lowerValue == "false") {
                    bool boolVal = (lowerValue == "true");
                    set(key, boolVal, ConfigSource::File);
                    parsed = true;
                }
            }
            
            // Try vector (simple form: [1,2,3])
            if (!parsed && value.size() >= 2 && value.front() == '[' && value.back() == ']') {
                // Extract content between brackets
                std::string content = value.substr(1, value.size() - 2);
                std::vector<double> vec;
                std::string token;
                std::istringstream tokenStream(content);
                
                while (std::getline(tokenStream, token, ',')) {
                    token = trim(token);
                    try {
                        double val = std::stod(token);
                        vec.push_back(val);
                    } catch (...) {
                        break;  // If parsing fails, abandon the whole vector
                    }
                }
                
                if (!vec.empty() && !tokenStream.peek()) {
                    set(key, vec, ConfigSource::File);
                    parsed = true;
                }
            }
            
            // Fall back to string if nothing else worked
            if (!parsed) {
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from: " + filepath);
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO(std::to_string(argc) + " command line arguments received");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                NLM_LOG_INFO("Setting config from command line: " + key + " = " + value);
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Malformed command line argument (missing '='): " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            NLM_LOG_INFO("Setting config from command line: " + key + " = " + value);
            set(key, value, ConfigSource::CommandLine);
        } else {
            NLM_LOG_WARNING("Unrecognized command line argument (should start with - or --): " + arg);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by NLM Brain System" << std::endl;
    file << "# " << filepath << std::endl;
    file << std::endl;
    
    // Write each entry
    for (const auto& entry : pImpl->entries) {
        // Write description as comment if available
        if (!entry.description.empty()) {
            file << "# " << entry.description << std::endl;
        }
        
        // Write the key-value pair
        file << entry.key << " = ";
        
        // Use std::visit to write the actual value based on its type
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, int>) {
                file << val;
            } else if constexpr (std::is_same_v<T, double>) {
                // Use reasonable precision for doubles
                file.precision(6);
                file << std::fixed << val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << val[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file.precision(6);
                    file << std::fixed << val[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << val[i] << "\"";
                }
                file << "]";
            } else {
                // Fallback for unknown types
                file << "UNKNOWN_TYPE_" << static_cast<int>(entry.source);
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    file << std::endl;
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
