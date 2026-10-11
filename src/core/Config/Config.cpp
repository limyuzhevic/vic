// Add missing includes for JSON parsing
#include <nlohmann/json.hpp>
#include <mutex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::mutex mutex;  // Thread safety for concurrent access
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Simple JSON parser implementation using C++ standard library
class SimpleJsonParser {
public:
    static bool parse(const std::string& json, nlohmann::json& result);
    static bool serialize(const nlohmann::json& json, std::string& result);
    
private:
    static bool parseValue(const std::string& str, nlohmann::json& result);
    static bool parseObject(const std::string& str, nlohmann::json& result);
    static bool parseArray(const std::string& str, nlohmann::json& result);
    static bool parseString(const std::string& str, nlohmann::json& result);
    static bool parseNumber(const std::string& str, nlohmann::json& result);
    static bool parseBool(const std::string& str, nlohmann::json& result);
    static bool parseNull(const std::string& str, nlohmann::json& result);
    
    static std::string trim(const std::string& str);
    static size_t findNextUnescaped(const std::string& str, size_t start, char delim);
};

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Try to parse as JSON first
    try {
        auto jsonData = nlohmann::json::parse(content);
        clear();
        
        // Recursively parse JSON and add to config
        parseJsonToConfig(jsonData, "");
        
        NLM_LOG_INFO("Successfully loaded config from JSON file: " + filepath);
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        // If JSON parsing fails, fall back to simple key=value format
        NLM_LOG_DEBUG("JSON parsing failed, falling back to simple format: " + std::string(e.what()));
        
        // Clear and retry with simple format
        clear();
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
        
        if (!pImpl->entries.empty()) {
            NLM_LOG_INFO("Successfully loaded config from simple file format: " + filepath);
            return true;
        }
    }
    
    return false;
}

void Config::parseJsonToConfig(const nlohmann::json& json, const std::string& prefix) {
    if (json.is_object()) {
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = prefix.empty() ? it.key() : (prefix + "." + it.key());
            parseJsonToConfig(*it, key);
        }
    } else if (json.is_array()) {
        // Handle arrays by creating indexed keys
        for (size_t i = 0; i < json.size(); ++i) {
            parseJsonToConfig(json[i], prefix + "[" + std::to_string(i) + "]");
        }
    } else {
        // Handle primitive values
        ConfigSource source = ConfigSource::File;
        
        if (json.is_number_integer()) {
            set(prefix, json.get<int>(), source);
        } else if (json.is_number_float()) {
            set(prefix, json.get<double>(), source);
        } else if (json.is_boolean()) {
            set(prefix, json.get<bool>(), source);
        } else if (json.is_string()) {
            set(prefix, json.get<std::string>(), source);
        } else if (json.is_null()) {
            set(prefix, std::string("null"), source);
        }
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
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
    // Try to save as JSON first
    try {
        nlohmann::json jsonData;
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Build JSON from config entries
        for (const auto& entry : pImpl->entries) {
            std::string currentPath = entry.key;
            nlohmann::json* currentJson = &jsonData;
            
            // Navigate to the right path (handle dot notation)
            size_t dotPos = currentPath.find('.');
            while (dotPos != std::string::npos) {
                std::string segment = currentPath.substr(0, dotPos);
                currentPath = currentPath.substr(dotPos + 1);
                
                if (!currentJson->contains(segment)) {
                    currentJson->emplace_json_object(segment);
                }
                
                currentJson = &((*currentJson)[segment]);
                dotPos = currentPath.find('.');
            }
            
            // Set the value based on type
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                
                if constexpr (std::is_same_v<T, int>) {
                    (*currentJson)[currentPath] = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    (*currentJson)[currentPath] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    (*currentJson)[currentPath] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    (*currentJson)[currentPath] = arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    (*currentJson)[currentPath] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    (*currentJson)[currentPath] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    (*currentJson)[currentPath] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    (*currentJson)[currentPath] = arg;
                }
            }, entry.value);
        }
        
        // Write JSON to file
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
            return false;
        }
        
        file << jsonData.dump(2); // Pretty print with 2-space indent
        NLM_LOG_INFO("Successfully saved config as JSON: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to save config as JSON: ") + e.what());
        // Fall back to simple format
    }
    
    // Fallback to simple key=value format
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        std::string valueStr;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = "\"" + arg + "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStr = arg ? "true" : "false";
            } else {
                valueStr = std::to_string(arg);
            }
        }, entry.value);
        
        file << entry.key << " = " << valueStr << "\n";
    }
    
    NLM_LOG_INFO("Successfully saved config in simple format: " + filepath);
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
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

} // namespace nlm