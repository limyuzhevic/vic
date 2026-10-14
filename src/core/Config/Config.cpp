#include <iomanip>

using json = nlohmann::json;

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

    // Try to parse as JSON first
    try {
        json j;
        file >> j;
        clear();
        
        // Parse JSON and validate
        if (j.is_object()) {
            for (const auto& [key, value] : j.items()) {
                validateKey(key);
                ConfigValue configValue;
                try {
                    configValue = jsonToConfigValue(value);
                    set(key, configValue, ConfigSource::File);
                } catch (const std::exception& e) {
                    throw ConfigParseError("Failed to parse JSON value for key '" + key + "': " + std::string(e.what()));
                }
            }
        } else {
            throw ConfigParseError("Invalid JSON format: root must be an object");
        }
        return true;
    } catch (const json::parse_error& e) {
        // Fall back to simple key=value format
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
        return true;
    } catch (const ConfigParseError& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigParseError("Error parsing config file: " + std::string(e.what()));
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
    // Determine file format based on extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    
    if (ext == "json" || ext == "JSON") {
        return saveToJSON(filepath);
    } else {
        return saveToSimpleFormat(filepath);
    }
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

ConfigValue Config::jsonToConfigValue(const json& j) {
    if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number()) {
        double val = j.get<double>();
        // Check for NaN
        if (std::isnan(val)) {
            throw ConfigParseError("Configuration value cannot be NaN");
        }
        return val;
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        return jsonToConfigVector(j);
    }
    
    throw ConfigParseError("Unsupported JSON value type");
}

ConfigValue Config::jsonToConfigVector(const json& j) {
    if (!j.empty()) {
        // Check first element type to determine vector type
        if (j[0].is_number_integer()) {
            std::vector<int> vec = j.get<std::vector<int>>();
            return vec;
        } else if (j[0].is_number()) {
            // Check for NaN in any element
            for (const auto& element : j) {
                double val = element.get<double>();
                if (std::isnan(val)) {
                    throw ConfigParseError("Configuration array values cannot be NaN");
                }
            }
            std::vector<double> vec = j.get<std::vector<double>>();
            return vec;
        } else if (j[0].is_boolean()) {
            // Booleans in arrays aren't standard JSON, but we handle them
            std::vector<bool> vec = j.get<std::vector<bool>>();
            return vec;
        } else if (j[0].is_string()) {
            std::vector<std::string> vec = j.get<std::vector<std::string>>();
            return vec;
        }
    }
    
    throw ConfigParseError("Unsupported JSON array element type");
}

void Config::validateKey(const std::string& key) {
    if (key.empty()) {
        throw ConfigParseError("Configuration key cannot be empty");
    }
    if (key.find('.') != std::string::npos) {
        throw ConfigParseError("Invalid key format: key cannot contain dots ('.')");
    }
    if (key.find(' ') != std::string::npos) {
        throw ConfigParseError("Invalid key format: key cannot contain spaces");
    }
}

void Config::validateValue(const ConfigValue& value) {
    std::visit([this](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, double>) {
            if (std::isnan(val)) {
                throw ConfigValidationError("Configuration value cannot be NaN");
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& element : val) {
                if (std::isnan(element)) {
                    throw ConfigValidationError("Configuration array values cannot be NaN");
                }
            }
        }
    }, value);
}

void Config::saveToJSON(const std::string& filepath) const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = configValueToJson(entry.value);
        
        if (!entry.description.empty()) {
            j[entry.key + "_description"] = entry.description;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileError("Cannot open file for writing: " + filepath);
    }
    
    file << j.dump(2);  // Pretty print with 2-space indentation
}

void Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << configValueToSimpleFormat(entry.value) << "\n\n";
    }
}

json Config::configValueToJson(const ConfigValue& value) const {
    return std::visit([](auto&& val) -> json {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int>) {
            return val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return val;
        } else if constexpr (std::is_same_v<T, double>) {
            return val;
        } else if constexpr (std::is_same_v<T, bool>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return val;
        } else {
            throw ConfigParseError("Unsupported configuration value type");
        }
    }, value);
}

std::string Config::configValueToSimpleFormat(const ConfigValue& value) const {
    return std::visit([](auto&& val) -> std::string {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return std::to_string(val);
        } else if constexpr (std::is_same_v<T, double>) {
            // Format to avoid scientific notation for reasonable values
            std::stringstream ss;
            ss << std::fixed << std::setprecision(6) << val;
            return ss.str();
        } else if constexpr (std::is_same_v<T, bool>) {
            return val ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + val + "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << val[i];
            }
            ss << "]";
            return ss.str();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << std::fixed << std::setprecision(6) << val[i];
            }
            ss << "]";
            return ss.str();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << val[i] << "\"";
            }
            ss << "]";
            return ss.str();
        } else {
            return "PLACEHOLDER_VALUE";
        }
    }, value);
}
} // namespace nlm
