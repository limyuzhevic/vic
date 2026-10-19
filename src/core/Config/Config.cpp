// ============================================================================
// Batch Operations Implementation
// ============================================================================

void Config::batchSet(const std::vector<std::pair<std::string, ConfigValue>>& values, ConfigSource source) {
    for (const auto& pair : values) {
        if (pair.first.empty()) {
            throw std::invalid_argument("Configuration key cannot be empty");
        }
        set(pair.first, pair.second, source);
    }
}

void Config::batchRemove(const std::vector<std::string>& keys) {
    for (const auto& key : keys) {
        remove(key);
    }
}

template<typename T>
std::unordered_map<std::string, T> Config::batchGet(const std::vector<std::string>& keys) const {
    std::unordered_map<std::string, T> result;
    for (const auto& key : keys) {
        auto val = get<T>(key);
        if (val.has_value()) {
            result[key] = val.value();
        }
    }
    return result;
}

void Config::batchUpdate(const std::unordered_map<std::string, ConfigValue>& updates, ConfigSource source) {
    for (const auto& pair : updates) {
        if (!has(pair.first)) {
            throw std::invalid_argument("Key does not exist: " + pair.first);
        }
        set(pair.first, pair.second, source);
    }
}

// ============================================================================
// Scripting Support Implementation
// ============================================================================

std::vector<std::string> Config::executeScript(const std::string& script) {
    std::vector<std::string> output;
    std::string processedScript = scriptVar(script);
    
    // Simple script parsing (could be extended with more sophisticated parsing)
    std::vector<std::string> tokens;
    std::stringstream ss(processedScript);
    std::string token;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
    
    for (size_t i = 0; i < tokens.size(); ) {
        if (tokens[i] == "set" && i + 2 < tokens.size()) {
            std::string key = tokens[i + 1];
            std::string value = tokens[i + 2];
            
            // Try to determine type
            ConfigValue configValue = stringToConfigValue(value);
            set(key, configValue, ConfigSource::Runtime);
            output.push_back("Set " + key + " = " + value);
            i += 3;
        } else if (tokens[i] == "get" && i + 1 < tokens.size()) {
            std::string key = tokens[i + 1];
            auto val = get<std::string>(key);
            if (val.has_value()) {
                output.push_back(key + " = " + val.value());
            } else {
                output.push_back("Key not found: " + key);
            }
            i += 2;
        } else if (tokens[i] == "remove" && i + 1 < tokens.size()) {
            std::string key = tokens[i + 1];
            remove(key);
            output.push_back("Removed " + key);
            i += 2;
        } else if (tokens[i] == "clear") {
            clear();
            output.push_back("Cleared all configuration");
            i++;
        } else if (tokens[i] == "update" && i + 2 < tokens.size()) {
            std::string key = tokens[i + 1];
            std::string value = tokens[i + 2];
            
            if (!has(key)) {
                output.push_back("Key does not exist: " + key);
                i += 3;
                continue;
            }
            
            ConfigValue configValue = stringToConfigValue(value);
            set(key, configValue, ConfigSource::Runtime);
            output.push_back("Updated " + key + " = " + value);
            i += 3;
        } else {
            output.push_back("Unknown command: " + tokens[i]);
            i++;
        }
    }
    
    return output;
}

std::string Config::scriptVar(const std::string& script) const {
    std::string result = script;
    size_t pos = 0;
    
    while ((pos = result.find("${", pos)) != std::string::npos) {
        size_t endPos = result.find("}", pos + 2);
        if (endPos == std::string::npos) {
            break; // Malformed variable reference
        }
        
        std::string varName = result.substr(pos + 2, endPos - pos - 2);
        auto val = get<std::string>(varName);
        std::string replacement = val.has_value() ? val.value() : "";
        
        result.replace(pos, endPos - pos + 1, replacement);
        pos += replacement.length();
    }
    
    return result;
}

std::string Config::scriptEval(const std::string& expression) {
    std::string processedExpr = scriptVar(expression);
    
    // Simple evaluation for numeric expressions
    // In a real implementation, this would use a proper expression parser
    try {
        // Try to evaluate as a simple arithmetic expression
        // This is a simplified implementation
        size_t plusPos = processedExpr.find('+');
        size_t minusPos = processedExpr.find('-');
        size_t mulPos = processedExpr.find('*');
        size_t divPos = processedExpr.find('/');
        
        if (plusPos != std::string::npos || minusPos != std::string::npos || 
            mulPos != std::string::npos || divPos != std::string::npos) {
            
            // Very simplified evaluation - not production ready
            // Just handle simple cases
            if (processedExpr.find('+') != std::string::npos) {
                size_t pos = processedExpr.find('+');
                std::string left = processedExpr.substr(0, pos);
                std::string right = processedExpr.substr(pos + 1);
                double result = std::stod(left) + std::stod(right);
                return std::to_string(result);
            } else if (processedExpr.find('-') != std::string::npos) {
                size_t pos = processedExpr.find('-');
                std::string left = processedExpr.substr(0, pos);
                std::string right = processedExpr.substr(pos + 1);
                double result = std::stod(left) - std::stod(right);
                return std::to_string(result);
            } else if (processedExpr.find('*') != std::string::npos) {
                size_t pos = processedExpr.find('*');
                std::string left = processedExpr.substr(0, pos);
                std::string right = processedExpr.substr(pos + 1);
                double result = std::stod(left) * std::stod(right);
                return std::to_string(result);
            } else if (processedExpr.find('/') != std::string::npos) {
                size_t pos = processedExpr.find('/');
                std::string left = processedExpr.substr(0, pos);
                std::string right = processedExpr.substr(pos + 1);
                double result = std::stod(left) / std::stod(right);
                return std::to_string(result);
            }
        }
        
        // If no operators found, return the value as-is (could be a variable)
        return processedExpr;
        
    } catch (const std::exception&) {
        throw std::runtime_error("Expression evaluation failed: " + expression);
    }
}

// ============================================================================
// Advanced Filtering Implementation
// ============================================================================

std::vector<std::string> Config::filterBySource(ConfigSource source) const {
    std::vector<std::string> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.push_back(entry.key);
        }
    }
    return result;
}

template<typename T>
std::vector<std::string> Config::filterByType() const {
    std::vector<std::string> result;
    for (const auto& entry : pImpl->entries) {
        try {
            std::get<T>(entry.value);
            result.push_back(entry.key);
        } catch (const std::bad_variant_access&) {
            // Not the requested type
        }
    }
    return result;
}

std::vector<std::string> Config::findKeys(const std::string& pattern) const {
    std::vector<std::string> result;
    for (const auto& entry : pImpl->entries) {
        if (matchWildcard(entry.key, pattern)) {
            result.push_back(entry.key);
        }
    }
    return result;
}

// ============================================================================
// Import/Export Implementation
// ============================================================================

std::string Config::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": " << escapeJSON(formatValue(entry.value));
        if (i < pImpl->entries.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    
    oss << "}\n";
    return oss.str();
}

bool Config::importFromJSON(const std::string& jsonString) {
    // Very simplified JSON parsing - in production would use a proper JSON library
    // This is just a basic implementation for demonstration
    if (jsonString.empty() || jsonString[0] != '{' || jsonString.back() != '}') {
        throw std::runtime_error("Invalid JSON format: must be object");
    }
    
    std::string content = jsonString.substr(1, jsonString.size() - 2);
    std::vector<std::string> pairs;
    
    // Very naive parsing - would use a proper JSON parser in production
    size_t pos = 0;
    while (pos < content.size()) {
        size_t commaPos = content.find(",", pos);
        std::string pair;
        if (commaPos != std::string::npos) {
            pair = content.substr(pos, commaPos - pos);
            pos = commaPos + 1;
        } else {
            pair = content.substr(pos);
            pos = content.size();
        }
        
        // Find key-value separator
        size_t colonPos = pair.find(":");
        if (colonPos == std::string::npos) {
            continue; // Invalid pair
        }
        
        std::string key = pair.substr(0, colonPos);
        // Trim whitespace
        key = trim(key);
        if (key.empty() || key[0] != '\"' || key.back() != '\"') {
            continue; // Invalid key format
        }
        key = key.substr(1, key.size() - 2); // Remove quotes
        
        std::string value = pair.substr(colonPos + 1);
        value = trim(value);
        
        // Parse value (simplified)
        ConfigValue configValue = stringToConfigValue(value);
        set(key, configValue, ConfigSource::File);
    }
    
    return true;
}

std::string Config::exportToYAML() const {
    std::ostringstream oss;
    oss << "# Configuration\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << entry.key << ": " << formatValue(entry.value) << "\n";
        oss << "  # Source: " << static_cast<int>(entry.source) << "\n";
        oss << "  # Description: " << entry.description << "\n";
    }
    
    return oss.str();
}

bool Config::importFromYAML(const std::string& yamlString) {
    // Very simplified YAML parsing - in production would use a proper YAML library
    clear();
    
    std::istringstream iss(yamlString);
    std::string line;
    
    while (std::getline(iss, line)) {
        // Skip comments and empty lines
        if (line.empty() || line.find("#") == 0) {
            continue;
        }
        
        // Very naive YAML parsing - would use a proper YAML parser in production
        size_t colonPos = line.find(":");
        if (colonPos == std::string::npos) {
            continue; // Invalid YAML line
        }
        
        std::string key = line.substr(0, colonPos);
        // Trim whitespace
        key = trim(key);
        
        std::string value = line.substr(colonPos + 1);
        value = trim(value);
        
        // Handle quoted strings
        if (!value.empty() && value[0] == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }
        
        ConfigValue configValue = stringToConfigValue(value);
        set(key, configValue, ConfigSource::File);
    }
    
    return true;
}

std::vector<std::string> Config::diff(const Config& otherConfig) const {
    std::vector<std::string> differences;
    
    // Get all keys from both configurations
    std::vector<std::string> thisKeys = getKeys();
    std::vector<std::string> otherKeys = otherConfig.getKeys();
    
    // Check for keys that exist in this but not in other
    for (const auto& key : thisKeys) {
        if (!otherConfig.has(key)) {
            differences.push_back("Added: " + key + " (new key)");
        } else {
            // Get values from both configurations
            auto thisVal = get<std::string>(key);
            auto otherVal = otherConfig.get<std::string>(key);
            
            if (thisVal != otherVal) {
                std::string oldValue = otherVal.has_value() ? otherVal.value() : "<not present>";
                std::string newValue = thisVal.has_value() ? thisVal.value() : "<empty>";
                differences.push_back("Changed: " + key + "\n  From: " + oldValue + "\n  To: " + newValue);
            }
        }
    }
    
    // Check for keys that exist in other but not in this
    for (const auto& key : otherKeys) {
        if (!has(key)) {
            differences.push_back("Removed: " + key + " (deleted key)");
        }
    }
    
    return differences;
}

// ============================================================================
// Configuration Templates Implementation
// ============================================================================

bool Config::applyTemplate(const std::string& templateName) {
    // Predefined templates
    if (templateName == "production") {
        set("security.debug", false, ConfigSource::File);
        set("performance.max_connections", 100, ConfigSource::File);
        set("logging.level", "ERROR", ConfigSource::File);
        set("caching.enabled", true, ConfigSource::File);
        set("monitoring.enabled", true, ConfigSource::File);
        return true;
    } else if (templateName == "development") {
        set("security.debug", true, ConfigSource::File);
        set("performance.max_connections", 10, ConfigSource::File);
        set("logging.level", "DEBUG", ConfigSource::File);
        set("caching.enabled", false, ConfigSource::File);
        set("monitoring.enabled", false, ConfigSource::File);
        return true;
    } else if (templateName == "testing") {
        set("security.debug", true, ConfigSource::File);
        set("performance.max_connections", 5, ConfigSource::File);
        set("logging.level", "TRACE", ConfigSource::File);
        set("caching.enabled", false, ConfigSource::File);
        set("monitoring.enabled", false, ConfigSource::File);
        set("test.mock_data", true, ConfigSource::File);
        return true;
    } else if (templateName == "minimal") {
        // Empty template - only essential settings
        return true;
    }
    
    return false; // Template not found
}

void Config::createTemplate(const std::string& templateName, const std::string& description) {
    // Templates are stored in a static map - in production would use a proper
    // persistent storage mechanism like a database or file system
    static std::unordered_map<std::string, std::string> templates;
    
    if (templates.find(templateName) != templates.end()) {
        throw std::runtime_error("Template already exists: " + templateName);
    }
    
    templates[templateName] = description;
}

std::unordered_map<std::string, std::string> Config::listTemplates() const {
    static std::unordered_map<std::string, std::string> templates = {
        {"production", "Production-ready configuration with security and performance settings"},
        {"development", "Development configuration with debug and verbose settings"},
        {"testing", "Test configuration with mock data and isolated settings"},
        {"minimal", "Minimal configuration with only essential settings"}
    };
    
    return templates;
}

// ============================================================================
// Helper Functions Implementation
// ============================================================================

bool Config::matchWildcard(const std::string& str, const std::string& pattern) {
    // Very simplified wildcard matching - in production would use a more robust algorithm
    size_t starPos = pattern.find('*');
    if (starPos == std::string::npos) {
        // No wildcards, direct comparison
        return str == pattern;
    }
    
    // Handle * pattern
    size_t prefixEnd = starPos;
    std::string prefix = pattern.substr(0, prefixEnd);
    size_t suffixStart = starPos + 1;
    std::string suffix = pattern.substr(suffixStart);
    
    // Check if string starts with prefix and ends with suffix
    if (str.size() < prefix.size() + suffix.size()) {
        return false;
    }
    
    if (str.substr(0, prefix.size()) != prefix) {
        return false;
    }
    
    if (str.substr(str.size() - suffix.size()) != suffix) {
        return false;
    }
    
    return true;
}

std::string Config::escapeJSON(const std::string& input) {
    std::string output;
    for (char c : input) {
        switch (c) {
            case '\\': output += "\\\\"; break;
            case '\"': output += "\\\""; break;
            case '\b': output += "\\b"; break;
            case '\f': output += "\\f"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default:
                if (c < 32 || c > 126) {
                    output += "\\u";
                    output += "0123456789ABCDEF"[(c >> 12) & 0xF];
                    output += "0123456789ABCDEF"[(c >> 8) & 0xF];
                    output += "0123456789ABCDEF"[(c >> 4) & 0xF];
                    output += "0123456789ABCDEF"[c & 0xF];
                } else {
                    output += c;
                }
                break;
        }
    }
    return output;
}

std::vector<std::string> Config::parseExpression(const std::string& expression) {
    std::vector<std::string> tokens;
    std::string token;
    
    for (char c : expression) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else if (c == '(' || c == ')' || c == '+' || c == '-' || c == '*' || c == '/') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(std::string(1, c));
        } else {
            token += c;
        }
    }
    
    if (!token.empty()) {
        tokens.push_back(token);
    }
    
    return tokens;
}

ConfigValue Config::stringToConfigValue(const std::string& value) {
    if (value.empty()) {
        return std::string("");
    }
    
    // Check for boolean
    std::string lower = toLower(value);
    if (lower == "true" || lower == "false") {
        return lower == "true";
    }
    
    // Check for integer
    bool isInt = true;
    size_t pos = 0;
    if (value[0] == '-') {
        pos = 1;
    }
    for (; pos < value.size(); ++pos) {
        if (!std::isdigit(static_cast<unsigned char>(value[pos]))) {
            isInt = false;
            break;
        }
    }
    if (isInt) {
        try {
            return std::stoi(value);
        } catch (const std::exception&) {
            // Fall through to double check
        }
    }
    
    // Check for double
    bool isDouble = true;
    int decimalCount = 0;
    pos = 0;
    if (value[0] == '-') {
        pos = 1;
    }
    for (; pos < value.size(); ++pos) {
        if (std::isdigit(static_cast<unsigned char>(value[pos]))) {
            continue;
        } else if (value[pos] == '.' && decimalCount == 0) {
            decimalCount++;
        } else {
            isDouble = false;
            break;
        }
    }
    if (isDouble && decimalCount > 0) {
        try {
            return std::stod(value);
        } catch (const std::exception&) {
            // Fall through to string
        }
    }
    
    // Default to string
    return value;
}

} // namespace nlm