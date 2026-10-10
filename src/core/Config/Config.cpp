namespace nlm {

// Helper function to parse vector from string
std::vector<int> parseVectorInt(const std::string& str) {
    std::vector<int> result;
    std::stringstream ss(str);
    std::string token;
    bool first = true;
    
    while (std::getline(ss, token, ',')) {
        // Trim whitespace
        size_t start = token.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            if (!first && !token.empty()) {
                result.push_back(0);
            }
            continue;
        }
        size_t end = token.find_last_not_of(" \t\r\n");
        token = token.substr(start, end - start + 1);
        
        if (token.empty()) {
            if (!first) {
                result.push_back(0);
            }
            continue;
        }
        
        try {
            // Handle potential whitespace and comments
            size_t comment_pos = token.find('#');
            if (comment_pos != std::string::npos) {
                token = token.substr(0, comment_pos);
                // Trim again after removing comment
                start = token.find_first_not_of(" \t\r\n");
                if (start == std::string::npos) {
                    if (!first) {
                        result.push_back(0);
                    }
                    continue;
                }
                end = token.find_last_not_of(" \t\r\n");
                token = token.substr(start, end - start + 1);
                if (token.empty()) {
                    if (!first) {
                        result.push_back(0);
                    }
                    continue;
                }
            }
            
            // Parse the integer
            int value = std::stoi(token);
            result.push_back(value);
        } catch (const std::exception&) {
            return {};
        }
        
        first = false;
    }
    
    return result;
}

std::vector<double> parseVectorDouble(const std::string& str) {
    std::vector<double> result;
    std::stringstream ss(str);
    std::string token;
    bool first = true;
    
    while (std::getline(ss, token, ',')) {
        // Trim whitespace
        size_t start = token.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            if (!first && !token.empty()) {
                result.push_back(0.0);
            }
            continue;
        }
        size_t end = token.find_last_not_of(" \t\r\n");
        token = token.substr(start, end - start + 1);
        
        if (token.empty()) {
            if (!first) {
                result.push_back(0.0);
            }
            continue;
        }
        
        try {
            // Handle potential whitespace and comments
            size_t comment_pos = token.find('#');
            if (comment_pos != std::string::npos) {
                token = token.substr(0, comment_pos);
                // Trim again after removing comment
                start = token.find_first_not_of(" \t\r\n");
                if (start == std::string::npos) {
                    if (!first) {
                        result.push_back(0.0);
                    }
                    continue;
                }
                end = token.find_last_not_of(" \t\r\n");
                token = token.substr(start, end - start + 1);
                if (token.empty()) {
                    if (!first) {
                        result.push_back(0.0);
                    }
                    continue;
                }
            }
            
            // Parse the double
            double value = std::stod(token);
            result.push_back(value);
        } catch (const std::exception&) {
            return {};
        }
        
        first = false;
    }
    
    return result;
}

std::vector<std::string> parseVectorString(const std::string& str) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;
    bool first = true;
    
    while (std::getline(ss, token, ',')) {
        // Trim whitespace
        size_t start = token.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            if (!first && !token.empty()) {
                result.push_back(";");
            }
            continue;
        }
        size_t end = token.find_last_not_of(" \t\r\n");
        token = token.substr(start, end - start + 1);
        
        if (token.empty()) {
            if (!first) {
                result.push_back(";");
            }
            continue;
        }
        
        try {
            // Handle potential whitespace and comments
            size_t comment_pos = token.find('#');
            if (comment_pos != std::string::npos) {
                token = token.substr(0, comment_pos);
                // Trim again after removing comment
                start = token.find_first_not_of(" \t\r\n");
                if (start == std::string::npos) {
                    if (!first) {
                        result.push_back(";");
                    }
                    continue;
                }
                end = token.find_last_not_of(" \t\r\n");
                token = token.substr(start, end - start + 1);
                if (token.empty()) {
                    if (!first) {
                        result.push_back(";");
                    }
                    continue;
                }
            }
            
            // Handle quoted strings
            std::string value;
            if ((token.front() == '"' && token.back() == '"') ||
                (token.front() == '\'' && token.back() == '\'')) {
                value = token.substr(1, token.size() - 2);
                // Unescape quotes and backslashes
                size_t pos = 0;
                while ((pos = value.find('\\\\', pos)) != std::string::npos) {
                    value.replace(pos, 1, "\\\\");
                    pos += 2;
                }
                pos = 0;
                while ((pos = value.find('\\"', pos)) != std::string::npos) {
                    value.replace(pos, 2, "\"");
                    pos += 1;
                }
            } else {
                value = token;
            }
            
            result.push_back(value);
        } catch (const std::exception&) {
            return {};
        }
        
        first = false;
    }
    
    return result;
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigError> errors;
    ErrorRecoveryMode errorRecoveryMode;
    size_t successfulParses;
    size_t errorCount;
    
    Impl() : errors(), errorRecoveryMode(ErrorRecoveryMode::Lenient), 
             successfulParses(0), errorCount(0) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFile(filepath, ErrorRecoveryMode::Lenient);
}

bool Config::loadFromFile(const std::string& filepath, ErrorRecoveryMode mode) {
    // Reset error tracking
    pImpl->errors.clear();
    pImpl->successfulParses = 0;
    pImpl->errorCount = 0;
    pImpl->errorRecoveryMode = mode;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        // Log file open error
        pImpl->errors.emplace_back(0, "Failed to open file: " + filepath, "");
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        std::string trimmedLine = trim(line);
        if (trimmedLine.empty() || trimmedLine[0] == '#' || trimmedLine[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = trimmedLine.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(trimmedLine.substr(0, pos));
            std::string value = trim(trimmedLine.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate key is not empty
            if (key.empty()) {
                std::string error = "Empty key found";
                pImpl->errors.emplace_back(lineNumber, error, line);
                pImpl->errorCount++;
                
                if (pImpl->errorRecoveryMode == ErrorRecoveryMode::Strict) {
                    return false;
                }
                continue;
            }
            
            // Parse vector types
            bool parsed = false;
            if (key.find("vector_") != std::string::npos) {
                if (key.find("_int") != std::string::npos) {
                    auto vec = parseVectorInt(value);
                    if (!vec.empty()) {
                        set(key, ConfigValue(vec), ConfigSource::File);
                        pImpl->successfulParses++;
                        parsed = true;
                    }
                } else if (key.find("_double") != std::string::npos) {
                    auto vec = parseVectorDouble(value);
                    if (!vec.empty()) {
                        set(key, ConfigValue(vec), ConfigSource::File);
                        pImpl->successfulParses++;
                        parsed = true;
                    }
                } else if (key.find("_string") != std::string::npos) {
                    auto vec = parseVectorString(value);
                    if (!vec.empty()) {
                        set(key, ConfigValue(vec), ConfigSource::File);
                        pImpl->successfulParses++;
                        parsed = true;
                    }
                } else {
                    set(key, ConfigValue(value), ConfigSource::File);
                    pImpl->successfulParses++;
                    parsed = true;
                }
            } else if (key.find("_int") != std::string::npos) {
                // Handle vector<int> type without "vector_" prefix
                auto vec = parseVectorInt(value);
                if (!vec.empty()) {
                    set(key, ConfigValue(vec), ConfigSource::File);
                    pImpl->successfulParses++;
                    parsed = true;
                }
            } else if (key.find("_double") != std::string::npos) {
                // Handle vector<double> type without "vector_" prefix
                auto vec = parseVectorDouble(value);
                if (!vec.empty()) {
                    set(key, ConfigValue(vec), ConfigSource::File);
                    pImpl->successfulParses++;
                    parsed = true;
                }
            } else if (key.find("_string") != std::string::npos) {
                // Handle vector<string> type without "vector_" prefix
                auto vec = parseVectorString(value);
                if (!vec.empty()) {
                    set(key, ConfigValue(vec), ConfigSource::File);
                    pImpl->successfulParses++;
                    parsed = true;
                }
            } else {
                set(key, ConfigValue(value), ConfigSource::File);
                pImpl->successfulParses++;
                parsed = true;
            }
            
            if (!parsed) {
                std::string error = "Failed to parse value for key: " + key;
                pImpl->errors.emplace_back(lineNumber, error, line);
                pImpl->errorCount++;
                
                if (pImpl->errorRecoveryMode == ErrorRecoveryMode::Strict) {
                    return false;
                }
            }
        } else {
            // Malformed line - no equals sign
            std::string error = "Malformed line - expected format: key=value";
            pImpl->errors.emplace_back(lineNumber, error, line);
            pImpl->errorCount++;
            
            if (pImpl->errorRecoveryMode == ErrorRecoveryMode::Strict) {
                return false;
            }
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
        
        // Serialize the value based on its type
        std::visit([&file, &key = entry.key](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, int> || 
                          std::is_same_v<T, int64_t> || 
                          std::is_same_v<T, double> || 
                          std::is_same_v<T, bool>) {
                // For primitive types, use standard to_string
                file << key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, std::string>) {
                // For strings, quote them with double quotes
                // Escape any existing double quotes and backslashes
                std::string escaped = arg;
                size_t pos = 0;
                while ((pos = escaped.find('"', pos)) != std::string::npos) {
                    escaped.insert(pos++, "\\\\");
                    pos++;
                }
                pos = 0;
                while ((pos = escaped.find('\\\\', pos)) != std::string::npos) {
                    escaped.insert(pos++, "\\\\");
                    pos++;
                }
                file << key << " = \"" << escaped << "\"\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                // For int vectors, serialize as comma-separated values
                file << key << " = ";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ",";
                    file << arg[i];
                }
                file << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                // For double vectors, serialize as comma-separated values
                file << key << " = ";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ",";
                    file << arg[i];
                }
                file << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                // For string vectors, serialize as comma-separated quoted values
                file << key << " = ";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ",";
                    // Escape quotes in each string
                    std::string escaped = arg[i];
                    size_t pos = 0;
                    while ((pos = escaped.find('"', pos)) != std::string::npos) {
                        escaped.insert(pos++, "\\\\");
                        pos++;
                    }
                    pos = 0;
                    while ((pos = escaped.find('\\\\', pos)) != std::string::npos) {
                        escaped.insert(pos++, "\\\\");
                        pos++;
                    }
                    file << "\"" << escaped << "\"";
                }
                file << "\n";
            }
        }, entry.value);
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
    
    // Try direct variant access for scalar types
    if constexpr (std::is_same_v<T, int> || 
                  std::is_same_v<T, int64_t> || 
                  std::is_same_v<T, double> || 
                  std::is_same_v<T, bool> || 
                  std::is_same_v<T, std::string>) {
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
        try {
            const auto& vec = std::get<std::vector<int>>(it->value);
            return vec;
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        try {
            const auto& vec = std::get<std::vector<double>>(it->value);
            return vec;
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        try {
            const auto& vec = std::get<std::vector<std::string>>(it->value);
            return vec;
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    } else {
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

// Vector-specific set overloads
void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
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
    pImpl->errors.clear();
    pImpl->successfulParses = 0;
    pImpl->errorCount = 0;
}

bool Config::setErrorRecoveryMode(ErrorRecoveryMode mode) {
    pImpl->errorRecoveryMode = mode;
    return true;
}

ErrorRecoveryMode Config::getErrorRecoveryMode() const {
    return pImpl->errorRecoveryMode;
}

    size_t Config::getSuccessfulParses() const {
        return pImpl->successfulParses;
    }

    size_t Config::getErrorCount() const {
        return pImpl->errorCount;
    }

    const std::vector<ConfigError>& Config::getErrors() const {
        return pImpl->errors;
    }

    std::string Config::getErrorSummary() const {
        std::ostringstream oss;
        oss << "Configuration Error Summary:\n";
        oss << "  Successful parses: " << pImpl->successfulParses << "\n";
        oss << "  Errors: " << pImpl->errorCount << "\n";
        oss << "  Error recovery mode: " <<
            (pImpl->errorRecoveryMode == ErrorRecoveryMode::Strict ? "Strict" : "Lenient") << "\n";

        if (!pImpl->errors.empty()) {
            oss << "\nDetailed errors:\n";
            for (const auto& error : pImpl->errors) {
                oss << "  Line " << error.lineNumber << ": " << error.errorMessage << "\n";
                oss << "    Content: \"" << error.lineContent << "\"\n";
            }
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
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm
