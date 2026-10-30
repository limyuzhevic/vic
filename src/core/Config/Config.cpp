#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>  // JSON library for parsing

// Convert ConfigValue to JSON value
static nlohmann::json configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return nlohmann::json::parse("\"" + arg + "\"");
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int> ||
                             std::is_same_v<T, int64_t> ||
                             std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return nlohmann::json(arg);
        } else {
            return nlohmann::json();
        }
    }, value);
}

// Convert JSON value to ConfigValue
static ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue) {
    if (jsonValue.is_string()) {
        // Try to parse as boolean first for true/false strings
        std::string str = jsonValue.get<std::string>();
        if (str == "true") return true;
        if (str == "false") return false;
        return str;
    } else if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>();
    } else if (jsonValue.is_number_integer()) {
        return jsonValue.get<int64_t>();
    } else if (jsonValue.is_number_unsigned()) {
        return jsonValue.get<uint64_t>();
    } else if (jsonValue.is_number()) {
        return jsonValue.get<double>();
    } else if (jsonValue.is_array()) {
        // Determine array type
        if (!jsonValue.empty()) {
            if (jsonValue[0].is_number_integer()) {
                std::vector<int64_t> vec;
                vec.reserve(jsonValue.size());
                for (const auto& item : jsonValue) {
                    vec.push_back(item.get<int64_t>());
                }
                return vec;
            } else if (jsonValue[0].is_number()) {
                std::vector<double> vec;
                vec.reserve(jsonValue.size());
                for (const auto& item : jsonValue) {
                    vec.push_back(item.get<double>());
                }
                return vec;
            } else if (jsonValue[0].is_string()) {
                std::vector<std::string> vec;
                vec.reserve(jsonValue.size());
                for (const auto& item : jsonValue) {
                    vec.push_back(item.get<std::string>());
                }
                return vec;
            }
        }
        return std::vector<int64_t>(); // Empty int array as default
    }
    
    // Fallback to string representation
    return jsonValue.dump();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    try {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Check if it's JSON (starts with { or [ or has key-value pairs in JSON format)
        bool isJson = false;
        if (content.find('{') != std::string::npos || content.find('[') != std::string::npos) {
            try {
                auto jsonData = nlohmann::json::parse(content);
                
                // If it's a JSON object, process it
                if (jsonData.is_object()) {
                    isJson = true;
                    for (const auto& [key, value] : jsonData.items()) {
                        ConfigValue configValue = jsonToConfigValue(value);
                        set(key, configValue, ConfigSource::File);
                    }
                }
            } catch (const nlohmann::json::parse_error&) {
                // Not valid JSON, fall back to simple format
                isJson = false;
            }
        }
        
        // If not JSON or JSON parsing failed, use simple key=value format
        if (!isJson) {
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
        }
        
        return true;
        
    } catch (const std::exception& e) {
        // Fallback to simple format if anything goes wrong
        file.clear();
        file.seekg(0);
        
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
            } else {
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

// Validate configuration parameters
bool Config::validate() const {
    // Validate critical configuration parameters
    bool isValid = true;
    
    // Check for required parameters
    if (!has("random_seed")) {
        NLM_LOG_WARNING("Missing required parameter: random_seed");
        isValid = false;
    }
    
    if (!has("neuron_count") || getOr<size_t>("neuron_count", 0) < 10) {
        NLM_LOG_WARNING("Invalid neuron_count: must be >= 10");
        isValid = false;
    }
    
    if (!has("simulation_timestep") || getOr<double>("simulation_timestep", 0.0) <= 0.0) {
        NLM_LOG_WARNING("Invalid simulation_timestep: must be > 0");
        isValid = false;
    }
    
    if (!has("region_count") || getOr<size_t>("region_count", 0) < 1) {
        NLM_LOG_WARNING("Invalid region_count: must be >= 1");
        isValid = false;
    }
    
    // Check for reasonable bounds
    if (has("neuron_count")) {
        size_t neuronCount = getOr<size_t>("neuron_count", 0);
        if (neuronCount > 1000000) {
            NLM_LOG_WARNING("Very large neuron_count may impact performance");
        }
    }
    
    if (has("max_simulation_steps")) {
        size_t maxSteps = getOr<size_t>("max_simulation_steps", 0);
        if (maxSteps > 10000000) {
            NLM_LOG_WARNING("Very large max_simulation_steps may require significant time");
        }
    }
    
    return isValid;
}

// Merge configuration from another config
void Config::merge(const Config& other) {
    // Merge all entries from other config
    for (const auto& entry : other.pImpl->entries) {
        // Use runtime source for merged entries
        set(entry.key, entry.value, ConfigSource::Runtime);
    }
}

// Get configuration statistics
std::string Config::getStatistics() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:" << std::endl;
    oss << "Total entries: " << pImpl->entries.size() << std::endl;
    
    // Count entries by source
    size_t defaultCount = 0, fileCount = 0, cliCount = 0, runtimeCount = 0;
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default: defaultCount++; break;
            case ConfigSource::File: fileCount++; break;
            case ConfigSource::CommandLine: cliCount++; break;
            case ConfigSource::Runtime: runtimeCount++; break;
        }
    }
    
    oss << "Default entries: " << defaultCount << std::endl;
    oss << "File entries: " << fileCount << std::endl;
    oss << "Command line entries: " << cliCount << std::endl;
    oss << "Runtime entries: " << runtimeCount << std::endl;
    
    // Show memory usage estimate
    size_t totalMemory = pImpl->entries.size() * (sizeof(ConfigEntry) + 100); // Rough estimate
    oss << "Estimated memory usage: " << (totalMemory / 1024) << " KB" << std::endl;
    
    return oss.str();
}

// Clear and reset configuration
void Config::clear() {
    pImpl->entries.clear();
}

} // namespace nlm
