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

// Set default agent brain sensory dimensions
void Config::setVisionDimensions(size_t width, size_t height) {
    set("agent_brain.vision_width", static_cast<int64_t>(width));
    set("agent_brain.vision_height", static_cast<int64_t>(height));
}

// Set research mode
void Config::setResearchMode(bool enabled, const std::string& observationLevel, bool exportData) {
    set("research.mode", enabled);
    set("research.observation_level", observationLevel);
    set("research.data_export", exportData);
}

void Config::setTouchCount(size_t count) {
    set("agent_brain.touch_count", static_cast<int64_t>(count));
}

void Config::setInternalSignalCount(size_t count) {
    set("agent_brain.internal_count", static_cast<int64_t>(count));
}

void Config::setProprioceptionCount(size_t count) {
    set("agent_brain.proprioception_count", static_cast<int64_t>(count));
}

void Config::setMotorActionCount(size_t count) {
    set("agent_brain.motor_action_count", static_cast<int64_t>(count));
}

// Get agent brain sensory dimensions
size_t Config::getVisionWidth() const {
    auto val = get<int64_t>("agent_brain.vision_width");
    return val.has_value() ? static_cast<size_t>(val.value()) : 16;
}

size_t Config::getVisionHeight() const {
    auto val = get<int64_t>("agent_brain.vision_height");
    return val.has_value() ? static_cast<size_t>(val.value()) : 16;
}

size_t Config::getTouchCount() const {
    auto val = get<int64_t>("agent_brain.touch_count");
    return val.has_value() ? static_cast<size_t>(val.value()) : 8;
}

size_t Config::getInternalSignalCount() const {
    auto val = get<int64_t>("agent_brain.internal_count");
    return val.has_value() ? static_cast<size_t>(val.value()) : 4;
}

size_t Config::getProprioceptionCount() const {
    auto val = get<int64_t>("agent_brain.proprioception_count");
    return val.has_value() ? static_cast<size_t>(val.value()) : 6;
}

size_t Config::getMotorActionCount() const {
    auto val = get<int64_t>("agent_brain.motor_action_count");
    return val.has_value() ? static_cast<size_t>(val.value()) : 6;
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

// Helper function to convert string to value type
template<typename T>
T Config::fromString(const std::string& str) {
    std::istringstream iss(str);
    T value;
    iss >> value;
    return value;
}

// Helper function to get nested key value
double Config::getNestedValue(const std::string& key, const Config& config) {
    // Try simple key first
    auto val = config.get<double>(key);
    if (val.has_value()) return val.value();
    
    // Try with dot notation
    size_t pos = key.find('.');
    if (pos != std::string::npos) {
        std::string prefix = key.substr(0, pos);
        std::string suffix = key.substr(pos + 1);
        
        // Try to find matching prefix
        for (const auto& entry : config.getKeys()) {
            if (entry.find(prefix) == 0) {
                std::string fullKey = entry;
                auto nestedVal = config.get<double>(fullKey);
                if (nestedVal.has_value()) {
                    return nestedVal.value();
                }
            }
        }
    }
    
    return 0.0;
}

// Function to convert config to JSON string
std::string Config::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) oss << ",";
        oss << "\n";
    }
    
    oss << "}\n";
    return oss.str();
}

// Function to convert config to YAML string
std::string Config::toYAML() const {
    std::ostringstream oss;
    oss << "# NLM Configuration (YAML)\n";
    oss << "# Generated from Config object\n\n";
    
    std::map<std::string, std::string> grouped;
    
    for (const auto& entry : pImpl->entries) {
        // Group by prefix for cleaner YAML
        size_t pos = entry.key.find('.');
        std::string prefix = (pos != std::string::npos) ? entry.key.substr(0, pos) : "general";
        std::string suffix = (pos != std::string::npos) ? entry.key.substr(pos + 1) : entry.key;
        
        std::ostringstream valueStream;
        std::visit([&valueStream](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStream << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStream << (arg ? "true" : "false");
            } else {
                valueStream << arg;
            }
        }, entry.value);
        
        if (grouped.find(prefix) == grouped.end()) {
            grouped[prefix] = "\n" + suffix + ": " + valueStream.str();
        } else {
            grouped[prefix] += "\n  " + suffix + ": " + valueStream.str();
        }
    }
    
    for (const auto& pair : grouped) {
        oss << pair.first << ":\s*" << pair.second;
    }
    
    return oss.str();
}

// Function to merge config from another config object
void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            // Add new entry
            pImpl->entries.push_back(entry);
        } else {
            // Override existing entry
            it->value = entry.value;
            it->source = entry.source;
        }
    }
}

// Function to get config difference
std::diff Config::diff(const Config& other) const {
    diff diff;
    
    // Check entries in this config
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& otherEntry : other.pImpl->entries) {
            if (entry.key == otherEntry.key) {
                found = true;
                if (entry.value != otherEntry.value) {
                    diff.differences.push_back(
                        {entry.key, entry.value, otherEntry.value}
                    );
                }
                break;
            }
        }
        
        if (!found) {
            diff.onlyInFirst.push_back(entry.key);
        }
    }
    
    // Check entries only in other
    for (const auto& otherEntry : other.pImpl->entries) {
        bool found = false;
        for (const auto& entry : pImpl->entries) {
            if (entry.key == otherEntry.key) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            diff.onlyInSecond.push_back(otherEntry.key);
        }
    }
    
    return diff;
}

} // namespace nlm

// Template function implementations

template<typename T>
T Config::fromString(const std::string& str) {
    std::istringstream iss(str);
    T value;
    iss >> value;
    return value;
}
