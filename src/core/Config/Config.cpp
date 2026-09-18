// Config implementation with JSON support
struct Config::Impl {
    std::map<std::string, ConfigValue> config;
    std::shared_ptr<ConfigValidator> validator;
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->validator = std::make_shared<ConfigValidator>();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // First try to load as JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing config
        pImpl->config.clear();
        
        // Parse JSON
        fromJson(jsonData);
        
        // Validate
        validate();
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        // If JSON parsing fails, try fallback to simple key=value format
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
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Try to parse value based on content
                ConfigValue configValue;
                if (value == "true") {
                    configValue = true;
                } else if (value == "false") {
                    configValue = false;
                } else if (value[0] == '"' || value[0] == '\'') {
                    configValue = value.substr(1, value.length() - 2);
                } else {
                    // Try numeric
                    try {
                        size_t posFloat;
                        float f = std::stof(value, &posFloat);
                        if (posFloat == value.length()) {
                            configValue = f;
                        } else {
                            size_t posInt;
                            long long i = std::stoll(value, &posInt);
                            if (posInt == value.length()) {
                                configValue = i;
                            } else {
                                configValue = value;
                            }
                        }
                    } catch (...) {
                        configValue = value;
                    }
                }
                
                pImpl->config[key] = configValue;
            }
        }
        
        return true;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Same implementation as before
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
    
    // Save as JSON
    nlohmann::json jsonData;
    
    for (const auto& pair : pImpl->config) {
        jsonData[pair.first] = pair.second;
    }
    
    file << jsonData.dump(2); // Pretty print with 2-space indentation
    
    return true;
}

bool Config::validate() {
    if (!pImpl->validator) {
        return true;
    }
    
    ValidationResult result = pImpl->validator->validate(pImpl->config);
    pImpl->validationErrors = result.errors;
    
    return result.valid;
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

void Config::applyDefaults() {
    if (pImpl->validator) {
        pImpl->validator->applyDefaults(pImpl->config);
    }
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->config.size() << " entries):\n";
    for (const auto& pair : pImpl->config) {
        oss << "  " << pair.first << " = [";
        oss << ConfigValidator::configValueToString(pair.second) << "]\n";
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

// Template instantiations for common types
// nlm namespace end
}