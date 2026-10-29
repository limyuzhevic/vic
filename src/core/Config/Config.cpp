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
    // Save configuration in JSON format for better structure and readability
    return saveToJson(filepath);
}

bool Config::loadFromJsonString(const std::string& jsonString) {
    // For now, save to a temporary file and load from it
    // This is a simplified implementation - in production, use a proper JSON parser
    std::string tempFile = "temp_config.json";
    std::ofstream temp(tempFile);
    if (!temp.is_open()) return false;
    temp << jsonString;
    temp.close();
    
    bool result = loadFromFile(tempFile);
    std::remove(tempFile.c_str());
    return result;
}

std::string Config::saveToJsonString() const {
    // Create a temporary file and save to it, then read content
    std::string tempFile = "temp_config_save.json";
    if (!saveToJson(tempFile)) return "";
    
    std::ifstream file(tempFile);
    if (!file.is_open()) return "";
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    std::remove(tempFile.c_str());
    return content;
}

bool Config::validate(const std::vector<ValidationRule>& rules) const {
    std::vector<std::string> errors;
    
    for (const auto& rule : rules) {
        if (!has(rule.key)) {
            errors.push_back("Missing required key: " + rule.key);
            continue;
        }
        
        std::optional<ConfigValue> value = getValue(rule.key);
        if (!value) {
            errors.push_back("Could not get value for key: " + rule.key);
            continue;
        }
        
        if (!rule.validator(*value)) {
            errors.push_back(rule.errorMessage + " (key: " + rule.key + ")");
        }
    }
    
    if (!errors.empty()) {
        std::ostringstream oss;
        oss << "Configuration validation failed:";
        for (const auto& error : errors) {
            oss << "\n  - " << error;
        }
        NLM_LOG_ERROR(oss.str());
    }
    
    return errors.empty();
}

void Config::addValidationRule(const ValidationRule& rule) {
    // Store validation rules in the implementation struct
    if (pImpl) {
        pImpl->validationRules.push_back(rule);
    }
}

void Config::clearValidationRules() {
    if (pImpl) {
        pImpl->validationRules.clear();
    }
}

std::optional<ConfigValue> Config::getValue(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return it->value;
}

std::string Config::loadFromKeyValue(const std::string& content) {
    // Original key=value parser implementation
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
    
    return true;
}

// JSON utility functions
namespace JsonUtils {
    bool parseBool(const std::string& str) {
        if (str == "true") return true;
        if (str == "false") return false;
        return false;
    }
    
    std::string escapeString(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '\"': result += "\\\"; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (c < 32 || c > 126) {
                        result += "\\u" + std::to_string((unsigned int)c);
                    } else {
                        result += c;
                    }
                    break;
            }
        }
        return result;
    }
    
    std::string formatNumber(double value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
}

// Explicit template instantiations
template std::optional<int> nlm::Config::get<int>(const std::string&) const;
template std::optional<int64_t> nlm::Config::get<int64_t>(const std::string&) const;
template std::optional<double> nlm::Config::get<double>(const std::string&) const;
template std::optional<bool> nlm::Config::get<bool>(const std::string&) const;
template std::optional<std::string> nlm::Config::get<std::string>(const std::string&) const;

template int nlm::Config::getOr<int>(const std::string&, const int&) const;
template int64_t nlm::Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double nlm::Config::getOr<double>(const std::string&, const double&) const;
template bool nlm::Config::getOr<bool>(const std::string&, const bool&) const;
template std::string nlm::Config::getOr<std::string>(const std::string&, const std::string&) const;
