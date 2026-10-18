} // namespace nlm
// Simple YAML parsing support
bool Config::isYamlContent(const std::string& content) const {
    // Simple YAML detection - look for YAML-like patterns
    // Basic YAML has indentation, maps with colons, lists with dashes
    bool hasIndentation = false;
    bool hasColon = false;
    bool hasDash = false;
    
    std::istringstream iss(content);
    std::string line;
    while (std::getline(iss, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        // Check for YAML patterns
        if (!line.empty() && (line[0] == ' ' || line[0] == '\t')) hasIndentation = true;
        if (line.find(':') != std::string::npos) hasColon = true;
        if (line.find("- ") != std::string::npos) hasDash = true;
    }
    
    return hasIndentation || hasColon || hasDash;
}

bool Config::loadFromYaml(const std::string& content, const std::string& filepath) {
    std::istringstream iss(content);
    std::string line;
    size_t indentLevel = 0;
    std::string currentKey;
    std::string currentSection;
    
    try {
        while (std::getline(iss, line)) {
            size_t originalLength = line.length();
            line = trim(line);
            
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Calculate indentation level
            size_t currentIndent = 0;
            while (currentIndent < originalLength && (line[currentIndent] == ' ' || line[currentIndent] == '\t')) {
                currentIndent++;
            }
            
            // Handle nested structures
            if (currentIndent > indentLevel) {
                currentSection = currentKey;
            } else if (currentIndent < indentLevel) {
                // Go back up one level
                size_t lastDot = currentSection.find_last_of('.');
                if (lastDot != std::string::npos) {
                    currentSection = currentSection.substr(0, lastDot);
                } else {
                    currentSection = "";
                }
            }
            
            indentLevel = currentIndent;
            
            // Check for key-value pair
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = trim(line.substr(0, colonPos));
                std::string value = trim(line.substr(colonPos + 1));
                
                // Construct full key
                std::string fullKey = currentSection.empty() ? key : currentSection + "." + key;
                
                // Remove quotes from value
                if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                        (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Set the value
                set(fullKey, value, ConfigSource::File);
            } else if (line.find("-") == 0 && line.size() > 1 && std::isspace(static_cast<unsigned char>(line[1]))) {
                // Handle list items
                std::string listItem = trim(line.substr(1));
                // For now, treat as string value
                set(currentKey, listItem, ConfigSource::File);
            }
        }
        
        clearLastError();
        return true;
        
    } catch (const std::exception& e) {
        pImpl->lastError = std::string("Error parsing YAML content: ") + e.what();
        return false;
    }
}

bool Config::loadFromJson(const std::string& content, const std::string& filepath) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(content);
        
        // Validate JSON structure
        if (!validateJSONStructure(jsonData)) {
            return false;
        }
        
        // Convert JSON to configuration entries
        clear();
        parseJSON(jsonData, ConfigSource::File);
        
        clearLastError();
        return true;
        
    } catch (const nlohmann::json::exception& e) {
        pImpl->lastError = std::string("JSON parsing error: ") + e.what();
        return false;
    } catch (const std::exception& e) {
        pImpl->lastError = std::string("Error parsing JSON content: ") + e.what();
        return false;
    }
}

bool Config::loadFromKeyValue(const std::string& content, const std::string& filepath) {
    std::istringstream iss(content);
    std::string line;
    size_t lineNumber = 0;
    
    try {
        while (std::getline(iss, line)) {
            lineNumber++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Validate line length
            if (line.length() > 1024) { // 1KB line limit
                pImpl->lastError = "Line " + std::to_string(lineNumber) + " exceeds maximum length (1024 characters)";
                return false;
            }
            
            // Parse simple key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Validate key format
                if (key.empty()) {
                    pImpl->lastError = "Line " + std::to_string(lineNumber) + ": Empty key";
                    return false;
                }
                
                if (!isValidKey(key)) {
                    pImpl->lastError = "Line " + std::to_string(lineNumber) + ": Invalid key format \"" + key + "\"";
                    return false;
                }
                
                // Remove quotes if present
                if (value.size() >= 2 && ((value.front() == '\"' && value.back() == '\"') ||
                                        (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Validate value format
                if (!isValidValue(value)) {
                    pImpl->lastError = "Line " + std::to_string(lineNumber) + ": Invalid value format \"" + value + "\"";
                    return false;
                }
                
                // Set the value
                set(key, value, ConfigSource::File);
            } else {
                pImpl->lastError = "Line " + std::to_string(lineNumber) + ": Missing = separator";
                return false;
            }
        }
        
        clearLastError();
        return true;
        
    } catch (const std::exception& e) {
        pImpl->lastError = std::string("Error parsing key-value content: ") + e.what();
        return false;
    }
}

// Determine if content appears to be JSON
bool Config::isJsonContent(const std::string& content) const {
    // Trim whitespace
    std::string trimmed = content;
    size_t start = trimmed.find_first_not_of(" \t\r\n");
    size_t end = trimmed.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return false;
    trimmed = trimmed.substr(start, end - start + 1);
    
    // JSON must start with { or [
    return !trimmed.empty() && (trimmed[0] == '{' || trimmed[0] == '[');
}// Configuration merging
void Config::merge(const Config& other) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Merge entries from other config
    for (const auto& entry : other.pImpl->entries) {
        // Only override if the source is higher priority
        if (static_cast<int>(entry.source) > static_cast<int>(getSourcePriority(entry.source))) {
            set(entry.key, entry.value, entry.source);
        }
    }
    
    // Clear cache
    pImpl->cache.clear();
    pImpl->cacheDirty = true;
}

// Get source priority for merging
ConfigSource Config::getSourcePriority(ConfigSource source) {
    switch (source) {
        case ConfigSource::CommandLine:
            return ConfigSource::CommandLine;
        case ConfigSource::File:
            return ConfigSource::File;
        case ConfigSource::Runtime:
            return ConfigSource::Runtime;
        case ConfigSource::Default:
            return ConfigSource::Default;
        default:
            return ConfigSource::Default;
    }
}

// Clear cache implementation
void Config::clearCache() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->cache.clear();
    pImpl->cacheDirty = true;
}

// Get cached value implementation
template<typename T>
std::optional<T> Config::getCached(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->cache.find(key);
    if (it != pImpl->cache.end()) {
        return std::any_cast<std::optional<T>>(it->second);
    }
    
    return std::nullopt;
}

// Set cached value implementation
template<typename T>
void Config::setCached(const std::string& key, const T& value) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->cache[key] = value;
    pImpl->cacheDirty = false;
}

// Enhanced getOr implementation with better error handling
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    
    if (val.has_value()) {
        return val.value();
    }
    
    // Log missing key if error handling is enabled
    if (pImpl->lastError.empty()) {
        pImpl->lastError = "Configuration key not found: " + key;
    }
    
    return defaultValue;
}

// Configuration validation improvements
bool Config::validate(const std::string& key, const ConfigValue& value) const {
    std::vector<std::string> errors;
    
    // Check if value type is valid for the key
    if (!isValidKey(key)) {
        errors.push_back("Invalid key format: " + key);
    }
    
    // Type-specific validation
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            if (arg.length() > 1024) {
                errors.push_back("String value too long for key: " + key);
            }
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            if (std::isinf(arg) || std::isnan(arg)) {
                errors.push_back("Invalid numeric value (infinity or NaN) for key: " + key);
            }
            if (arg < -1e6 || arg > 1e6) {
                errors.push_back("Numeric value out of range for key: " + key);
            }
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            if (arg < -1000000 || arg > 1000000) {
                errors.push_back("Integer value out of range for key: " + key);
            }
        }
        
        if constexpr (std::is_same_v<T, bool>) {
            // Boolean values are always valid
        }
        
    }, value);
    
    // Update error message
    if (!errors.empty()) {
        pImpl->lastError = "Validation failed for key \"" + key + "\": " + 
                         (errors.size() > 1 ? ("Multiple errors: " + errors[0]) : errors[0]);
        return false;
    }
    
    pImpl->lastError.clear();
    return true;
}

// Get configuration errors
std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (!pImpl->lastError.empty()) {
        errors.push_back(pImpl->lastError);
    }
    
    return errors;
}

// Clear validation errors
void Config::clearValidationErrors() const {
    pImpl->lastError.clear();
}