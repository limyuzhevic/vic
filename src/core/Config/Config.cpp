// Config Manager implementation

void ConfigManager::addParser(std::unique_ptr<ConfigParser> parser) {
    parsers_.push_back(std::move(parser));
}

void ConfigManager::addSerializer(std::unique_ptr<ConfigSerializer> serializer) {
    serializers_.push_back(std::move(serializer));
}

std::unordered_map<std::string, std::string> ConfigManager::parse(const std::string& filename, const std::string& content) {
    for (const auto& parser : parsers_) {
        if (parser->canParse(filename)) {
            return parser->parse(content);
        }
    }
    
    // Fallback to key-value parser for unknown file types
    KeyValueParser fallback;
    return fallback.parse(content);
}

std::string ConfigManager::serialize(const std::vector<ConfigEntry>& entries, const std::string& filename) {
    for (const auto& serializer : serializers_) {
        if (serializer->canSerialize(filename)) {
            return serializer->serialize(entries);
        }
    }
    
    // Fallback to key-value serializer for unknown file types
    KeyValueSerializer fallback;
    return fallback.serialize(entries);
}

std::vector<std::string> ConfigManager::getAvailableParsers() const {
    std::vector<std::string> names;
    for (const auto& parser : parsers_) {
        names.push_back(parser->getName());
    }
    return names;
}

std::vector<std::string> ConfigManager::getAvailableSerializers() const {
    std::vector<std::string> names;
    for (const auto& serializer : serializers_) {
        names.push_back(serializer->getName());
    }
    return names;
}

// Config implementation details

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string filename;  // Track the original file for format detection
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Reset config before loading
    clear();
    pImpl->filename = filepath;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return importFromString(buffer.str());
}

bool Config::loadFromArgs(int argc, char** argv) {
    clear();
    
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
    
    // Use config manager to serialize
    ConfigManager& manager = getConfigManager();
    std::string content = manager.serialize(pImpl->entries, filepath);
    
    file << content;
    return file.good();
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, std::vector<int> value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, std::vector<double> value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, std::vector<std::string> value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

std::string Config::getValueType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "NOT_FOUND";
    }
    
    const std::type_info& typeInfo = it->value.type();
    
    if (typeInfo == typeid(int)) return "INT";
    if (typeInfo == typeid(int64_t)) return "INT64";
    if (typeInfo == typeid(double)) return "DOUBLE";
    if (typeInfo == typeid(bool)) return "BOOL";
    if (typeInfo == typeid(std::string)) return "STRING";
    if (typeInfo == typeid(std::vector<int>)) return "VECTOR_INT";
    if (typeInfo == typeid(std::vector<double>)) return "VECTOR_DOUBLE";
    if (typeInfo == typeid(std::vector<std::string>)) return "VECTOR_STRING";
    
    return "UNKNOWN";
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
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    oss << arg[j];
                    if (j < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    oss << arg[j];
                    if (j < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    oss << "\"" << arg[j] << "\"";
                    if (j < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (i < pImpl->entries.size() - 1) oss << "\n";
    }
    return oss.str();
}

std::string Config::exportToString() const {
    ConfigManager& manager = getConfigManager();
    return manager.serialize(pImpl->entries, "export.cfg");
}

bool Config::importFromString(const std::string& content) {
    ConfigManager& manager = getConfigManager();
    std::string filename = pImpl->filename.empty() ? "unknown.cfg" : pImpl->filename;
    
    auto parsed = manager.parse(filename, content);
    
    // Convert parsed data to ConfigEntry format
    for (const auto& pair : parsed) {
        // Try to determine the appropriate type from the string value
        std::string valueStr = pair.second;
        
        // Remove quotes if present
        if (valueStr.size() >= 2 && 
            ((valueStr.front() == '"' && valueStr.back() == '"') ||
             (valueStr.front() == '\'' && valueStr.back() == '\''))) {
            valueStr = valueStr.substr(1, valueStr.size() - 2);
        }
        
        try {
            // Try to parse as boolean
            if (valueStr == "true" || valueStr == "false") {
                bool boolVal = (valueStr == "true");
                set(pair.first, boolVal, ConfigSource::File);
            }
            // Try to parse as integer
            else if (std::all_of(valueStr.begin(), valueStr.end(), ::isdigit) ||
                    (valueStr.front() == '-' && std::all_of(valueStr.begin() + 1, valueStr.end(), ::isdigit))) {
                int intVal = std::stoi(valueStr);
                set(pair.first, intVal, ConfigSource::File);
            }
            // Try to parse as double
            else {
                size_t pos;
                try {
                    double doubleVal = std::stod(valueStr, &pos);
                    if (pos == valueStr.size()) {
                        set(pair.first, doubleVal, ConfigSource::File);
                    } else {
                        // Check if it's a quoted string
                        set(pair.first, valueStr, ConfigSource::File);
                    }
                } catch (...) {
                    set(pair.first, valueStr, ConfigSource::File);
                }
            }
        } catch (...) {
            // If parsing fails, treat as string
            set(pair.first, valueStr, ConfigSource::File);
        }
    }
    
    return true;
}

// ConfigManager singleton management
ConfigManager& Config::getConfigManager() {
    static ConfigManager configManager;
    
    // Register default parsers and serializers if not already registered
    static bool registered = false;
    if (!registered) {
        configManager.addParser(std::make_unique<KeyValueParser>());
        configManager.addSerializer(std::make_unique<KeyValueSerializer>());
        
        // Optional: Register JSON/YAML parsers when available
        // configManager.addParser(std::make_unique<JsonConfigParser>());
        // configManager.addSerializer(std::make_unique<JsonConfigSerializer>());
        // configManager.addParser(std::make_unique<YamlConfigParser>());
        // configManager.addSerializer(std::make_unique<YamlConfigSerializer>());
        
        registered = true;
    }
    
    return configManager;
}

// Template specializations and helper implementations

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

bool Config::isComment(const std::string& line) {
    return line.empty() || line[0] == '#' || line[0] == '/';
}

std::pair<std::string, std::string> Config::parseKeyValue(const std::string& line) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return {"", ""};
    }
    
    std::string key = trim(line.substr(0, pos));
    std::string value = trim(line.substr(pos + 1));
    
    // Remove surrounding quotes
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    return {key, value};
}

template<typename T>
T Config::convertStringToType(const std::string& value) {
    // This is a placeholder - implement actual type conversion
    // For now, just return a default value
    return T();
}

std::string Config::convertValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        oss << arg;
    }, value);
    return oss.str();
}

bool Config::isValidType(const ConfigValue& value, const std::type_info& typeInfo) const {
    return value.type() == typeInfo;
}

} // namespace nlm