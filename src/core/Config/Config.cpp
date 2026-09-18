// Forward declarations for parser implementations
namespace {
    bool parseKeyValueFormat(const std::string& filepath, nlm::Config& config);
    bool parseJsonFormat(const std::string& filepath, nlm::Config& config);
    bool parseYamlFormat(const std::string& filepath, nlm::Config& config);
    
    // Helper functions
    std::string detectFileFormat(const std::string& filepath);
    bool parseVectorString(const std::string& str, std::vector<double>& out);
    bool parseVectorString(const std::string& str, std::vector<int>& out);
    bool parseVectorString(const std::string& str, std::vector<std::string>& out);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format based on extension
    std::string format = detectFileFormat(filepath);
    
    bool success = false;
    if (format == "json") {
        NLM_LOG_INFO("Loading configuration from JSON file: " + filepath);
        success = parseJsonFormat(filepath, *this);
    } else if (format == "yaml" || format == "yml") {
        NLM_LOG_INFO("Loading configuration from YAML file: " + filepath);
        success = parseYamlFormat(filepath, *this);
    } else {
        // Default to key=value format for backward compatibility
        NLM_LOG_INFO("Loading configuration from key=value file: " + filepath);
        success = parseKeyValueFormat(filepath, *this);
    }
    
    return success;
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
        NLM_LOG_ERROR("Failed to save configuration to file: " + filepath);
        return false;
    }
    
    // Write configuration in simple key=value format
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Write key=value pair
        file << entry.key << " = ";
        
        // Handle different value types for proper serialization
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                // Quote strings
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                // Write int vector
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                // Write double vector with controlled precision
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                // Write string vector
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                // Handle primitive types (int, int64_t, double, bool)
                if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Configuration saved to file: " + filepath);
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

// Namespace for configuration parser implementations
namespace {

// Helper: Detect file format based on extension
std::string detectFileFormat(const std::string& filepath) {
    if (filepath.size() < 4) return "keyvalue";
    
    std::string ext = filepath.substr(filepath.size() - 3);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") return "json";
    if (ext == "yaml" || ext == "yml") return "yaml";
    
    // Default to key=value format
    return "keyvalue";
}

// Helper: Parse vector string like "[1.0, 2.5, 3.0]" or "1,2,3"
bool parseVectorString(const std::string& str, std::vector<double>& out) {
    out.clear();
    std::string content = str;
    
    // Remove brackets if present
    if (!content.empty() && content.front() == '[' && content.back() == ']') {
        content = content.substr(1, content.size() - 2);
    }
    
    std::stringstream ss(content);
    std::string token;
    while (std::getline(ss, token, ',')) {
        token = trim(token);
        if (token.empty()) continue;
        
        try {
            size_t pos;
            double value = std::stod(token, &pos);
            out.push_back(value);
        } catch (const std::exception&) {
            return false;
        }
    }
    
    return !out.empty();
}

bool parseVectorString(const std::string& str, std::vector<int>& out) {
    out.clear();
    std::string content = str;
    
    // Remove brackets if present
    if (!content.empty() && content.front() == '[' && content.back() == ']') {
        content = content.substr(1, content.size() - 2);
    }
    
    std::stringstream ss(content);
    std::string token;
    while (std::getline(ss, token, ',')) {
        token = trim(token);
        if (token.empty()) continue;
        
        try {
            size_t pos;
            int value = std::stoi(token, &pos);
            out.push_back(value);
        } catch (const std::exception&) {
            return false;
        }
    }
    
    return !out.empty();
}

bool parseVectorString(const std::string& str, std::vector<std::string>& out) {
    out.clear();
    std::string content = str;
    
    // Remove brackets if present
    if (!content.empty() && content.front() == '[' && content.back() == ']') {
        content = content.substr(1, content.size() - 2);
    }
    
    std::stringstream ss(content);
    std::string token;
    while (std::getline(ss, token, ',')) {
        token = trim(token);
        if (token.empty()) continue;
        
        // Remove quotes if present
        if (token.size() >= 2 && ((token.front() == '"' && token.back() == '"') ||
                                 (token.front() == '\'' && token.back() == '\''))) {
            token = token.substr(1, token.size() - 2);
        }
        
        out.push_back(token);
    }
    
    return !out.empty();
}

// Key-value format parser (Phase 1 implementation with enhancements)
bool parseKeyValueFormat(const std::string& filepath, nlm::Config& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        line = config.trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = config.trim(line.substr(0, pos));
            std::string value = config.trim(line.substr(pos + 1));
            
            if (key.empty()) {
                NLM_LOG_WARNING("Empty key at line " + std::to_string(lineNumber));
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            config.set(key, value, nlm::ConfigSource::File);
            
            // Validate basic value types
            if (value == "true" || value == "false") {
                // Boolean value set
            } else if (value.find('.') != std::string::npos) {
                // Try to parse as double
                try {
                    double d = std::stod(value);
                    config.set(key, d, nlm::ConfigSource::File);
                } catch (...) {
                    // Keep as string if parsing fails
                }
            } else {
                // Try to parse as int
                try {
                    size_t pos;
                    int i = std::stoi(value, &pos);
                    if (pos == value.size()) {
                        config.set(key, i, nlm::ConfigSource::File);
                    }
                } catch (...) {
                    // Keep as string
                }
            }
        } else {
            NLM_LOG_WARNING("Invalid line " + std::to_string(lineNumber) + ": no '=' found");
        }
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(config.getKeys().size()) + 
                 " configuration entries from key=value file: " + filepath);
    return true;
}

// JSON format parser (placeholder - would need nlohmann/json or similar)
bool parseJsonFormat(const std::string& filepath, nlm::Config& config) {
    NLM_LOG_WARNING("JSON parsing not yet implemented, falling back to key=value format");
    return parseKeyValueFormat(filepath, config);
}

// YAML format parser (placeholder - would need yaml-cpp)
bool parseYamlFormat(const std::string& filepath, nlm::Config& config) {
    NLM_LOG_WARNING("YAML parsing not yet implemented, falling back to key=value format");
    return parseKeyValueFormat(filepath, config);
}

} // end namespace

// Explicit template instantiations
namespace nlm {
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

}} // namespace nlm
