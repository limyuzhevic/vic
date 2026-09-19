// Parse JSON to Config
void Config::parseJsonToConfig(const nlohmann::json& json, Config& config) {
    if (json.is_object()) {
        for (const auto& [key, value] : json.items()) {
            // Check if value is an object (nested config)
            if (value.is_object()) {
                // Create nested ConfigNode
                ConfigNode node(value.value(".name", ""));
                // In a full implementation, we would create nested structures
                // For now, treat as complex value
                config.set(key, config_utils::jsonToConfigValue(value), ConfigSource::File);
            } else {
                config.set(key, config_utils::jsonToConfigValue(value), ConfigSource::File);
            }
        }
    }
}

// Parse YAML to Config
void Config::parseYamlToConfig(const YAML::Node& yaml, Config& config) {
    if (yaml.IsMap()) {
        for (const auto& pair : yaml) {
            if (pair.first.IsScalar() && pair.second.IsDefined()) {
                std::string key = pair.first.Scalar();
                config.set(key, config_utils::yamlToConfigValue(pair.second), ConfigSource::File);
            }
        }
    }
}

// Convert Config to JSON
nlohmann::json Config::configToJson(const Config& config) {
    nlohmann::json json;
    for (const auto& entry : config.getEntries()) {
        json[entry.key] = config_utils::configValueToJson(entry.value);
    }
    return json;
}

// Convert Config to YAML
YAML::Node Config::configToYaml(const Config& config) {
    YAML::Node yaml;
    for (const auto& entry : config.getEntries()) {
        yaml[entry.key] = config_utils::configValueToYaml(entry.value);
    }
    return yaml;
}

bool Config::hasNested(const std::string& path) const {
    // Simple implementation: check for exact key match
    // In a full implementation, this would parse dot-separated path
    return has(path);
}

std::optional<ConfigValue> Config::getNested(const std::string& path) const {
    // Simple implementation: check for exact key match
    if (has(path)) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&path](const ConfigEntry& e) { return e.key == path; });
        if (it != pImpl->entries.end()) {
            return it->value;
        }
    }
    return std::nullopt;
}

void Config::setNested(const std::string& path, const ConfigValue& value, ConfigSource source) {
    // Simple implementation: treat path as key
    set(path, value, source);
}

void Config::addEntry(const std::string& key, const ConfigValue& value, const std::string& description,
                      const std::string& category, const ConfigOptions& options) {
    pImpl->entries.emplace_back(key, value, ConfigSource::Default, description, category);
}

std::vector<ConfigEntry> Config::getEntries() const {
    return pImpl->entries;
}

void Config::setEntryDescription(const std::string& key, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        it->description = description;
    }
}

void Config::setEntryCategory(const std::string& key, const std::string& category) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        it->category = category;
    }
}

bool Config::validate(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    // In a full implementation, we would check against the options validator
    // For now, just check if the value is valid (not empty)
    return !std::holds_alternative<std::monostate>(it->value);
}

// Config function implementations for backward compatibility
bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string targetFormat = format;
    if (targetFormat.empty()) {
        targetFormat = config_utils::detectFormat(filepath);
    }
    
    if (targetFormat == "json") {
        nlohmann::json jsonData = configToJson(*this);
        file << jsonData.dump(4);
    } else if (targetFormat == "yaml" || targetFormat == "yml") {
        YAML::Node yamlData = configToYaml(*this);
        file << yamlData;
    } else {
        // Default to INI-like format with comments
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << entry.value << "\n";
        }
    }
    
    return true;
}

// Config function implementations for backward compatibility
bool Config::loadFromFile(const std::string& filepath, const std::string& format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string detectedFormat = format;
    if (detectedFormat == "auto") {
        detectedFormat = config_utils::detectFormat(filepath);
    }
    
    clear();
    
    if (detectedFormat == "json") {
        nlohmann::json jsonData;
        try {
            file >> jsonData;
            parseJsonToConfig(jsonData, *this);
        } catch (const std::exception& e) {
            // Fallback to simple format
            file.clear();
            file.seekg(0, std::ios::beg);
            return loadFromSimpleFile(filepath);
        }
    } else if (detectedFormat == "yaml" || detectedFormat == "yml") {
        try {
            YAML::Node yamlData = YAML::Load(file);
            parseYamlToConfig(yamlData, *this);
        } catch (const std::exception& e) {
            // Fallback to simple format
            file.clear();
            file.seekg(0, std::ios::beg);
            return loadFromSimpleFile(filepath);
        }
    } else {
        // Unknown format, try simple parsing
        return loadFromSimpleFile(filepath);
    }
    
    return true;
}

// Config function implementations for backward compatibility
bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear();
    
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

} // namespace nlm