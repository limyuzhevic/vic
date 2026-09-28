    #include <fstream>
    #include <sstream>
    #include <algorithm>
    #include <filesystem>
    #include <stdexcept>

namespace nlm {

    // Simple configuration file parser supporting both:
    // 1. Key=value format (legacy)
    // 2. JSON format with support for basic types
    bool parseConfigValue(const std::string& valueStr, ConfigValue& outValue) {
        try {
            // Try to detect type based on content
            std::string trimmed = trim(valueStr);
            
            // Boolean detection
            if (trimmed == "true" || trimmed == "false") {
                outValue = (trimmed == "true");
                return true;
            }
            
            // Integer detection
            bool isInteger = true;
            for (char c : trimmed) {
                if (!std::isdigit(c) && c != '-') {
                    isInteger = false;
                    break;
                }
            }
            if (isInteger && !trimmed.empty()) {
                try {
                    outValue = std::stoll(trimmed);
                } catch (const std::exception&) {
                    return false;
                }
                return true;
            }
            
            // Float detection
            bool isFloat = true;
            int decimalPoints = 0;
            for (char c : trimmed) {
                if (!std::isdigit(c) && c != '-' && c != '.') {
                    isFloat = false;
                    break;
                }
                if (c == '.') decimalPoints++;
            }
            if (isFloat && decimalPoints <= 1 && !trimmed.empty()) {
                try {
                    outValue = std::stod(trimmed);
                } catch (const std::exception&) {
                    return false;
                }
                return true;
            }
            
            // String (already quoted or unquoted)
            outValue = trimmed;
            return true;
        } catch (...) {
            return false;
        }
    }

    bool Config::loadFromFile(const std::string& filepath) {
        try {
            // Implement proper configuration file parser
            // Supports both JSON and legacy key=value formats
            if (filepath.empty() || !std::filesystem::exists(filepath)) {
                return false;
            }
            
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            // Try to parse as JSON first
            nlohmann::json jsonData;
            try {
                file >> jsonData;
            } catch (...) {
                // If JSON parsing fails, fall back to legacy format
                file.clear();
                file.seekg(0);
                
                std::string line;
                while (std::getline(file, line)) {
                    // Skip empty lines and comments
                    line = trim(line);
                    if (line.empty() || line[0] == '#' || line[0] == '/') {
                        continue;
                    }
                    
                    // Parse key=value pairs
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
                        
                        parseConfigValue(value, jsonData[key]);
                    }
                }
                return true;
            }
            
            // Parse JSON object
            if (jsonData.is_object()) {
                for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                    const std::string& key = it.key();
                    ConfigValue value;
                    
                    // Parse JSON value to ConfigValue
                    if (it.value().is_string()) {
                        value = it.value().get<std::string>();
                    } else if (it.value().is_number_integer()) {
                        value = it.value().get<int64_t>();
                    } else if (it.value().is_number_float()) {
                        value = it.value().get<double>();
                    } else if (it.value().is_boolean()) {
                        value = it.value().get<bool>();
                    } else if (it.value().is_array()) {
                        std::vector<double> doubleArray;
                        doubleArray.reserve(it.value().size());
                        for (const auto& v : it.value()) {
                            doubleArray.push_back(v.get<double>());
                        }
                        value = doubleArray;
                    }
                    
                    set(key, value, ConfigSource::File);
                }
            }
            
            return true;
        } catch (...) {
            return false;
        }
    }

    bool Config::loadFromArgs(int argc, char** argv) {
        try {
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
        } catch (...) {
            return false;
        }
    }

    bool Config::saveToFile(const std::string& filepath) const {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            file << "{\n";
            bool first = true;
            for (const auto& entry : pImpl->entries) {
                if (!first) {
                    file << ",\n";
                }
                first = false;
                
                file << "  \"" << entry.key << "\": ";
                std::visit([&file](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        file << "\"" << arg << "\"";
                    } else if constexpr (std::is_same_v<T, bool>) {
                        file << (arg ? "true" : "false");
                    } else if constexpr (std::is_same_v<T, double>) {
                        file << arg;
                    } else if constexpr (std::is_same_v<T, int64_t>) {
                        file << arg;
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        file << "[";
                        const auto& vec = arg;
                        for (size_t i = 0; i < vec.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << vec[i];
                        }
                        file << "]";
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "[";
                        const auto& vec = arg;
                        for (size_t i = 0; i < vec.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << "\"" << vec[i] << "\"";
                        }
                        file << "]";
                    }
                }, entry.value);
            }
            file << "\n}\n";
            
            return true;
        } catch (...) {
            return false;
        }
    }

    template<typename T>
    std::optional<T> Config::get(const std::string& key) const {
        try {
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
        } catch (...) {
            return std::nullopt;
        }
    }

    template<typename T>
    T Config::getOr(const std::string& key, const T& defaultValue) const {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    }

    void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
        if (key.empty()) {
            return;
        }
        
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
        if (!key.empty()) {
            set(key, ConfigValue(value), source);
        }
    }

    void Config::set(const std::string& key, int value, ConfigSource source) {
        if (!key.empty()) {
            set(key, ConfigValue(value), source);
        }
    }

    void Config::set(const std::string& key, double value, ConfigSource source) {
        if (!key.empty()) {
            set(key, ConfigValue(value), source);
        }
    }

    void Config::set(const std::string& key, bool value, ConfigSource source) {
        if (!key.empty()) {
            set(key, ConfigValue(value), source);
        }
    }

    bool Config::has(const std::string& key) const {
        if (key.empty()) return false;
        
        return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    }

    void Config::remove(const std::string& key) {
        if (key.empty()) return;
        
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
            if (!entry.key.empty()) {
                keys.push_back(entry.key);
            }
        }
        return keys;
    }

    void Config::clear() {
        pImpl->entries.clear();
    }

    std::string Config::summary() const {
        try {
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
        } catch (...) {
            return "Error generating configuration summary.";
        }
    }

    std::string Config::trim(const std::string& str) {
        try {
            size_t start = str.find_first_not_of(" \t\r\n");
            if (start == std::string::npos) return "";
            size_t end = str.find_last_not_of(" \t\r\n");
            return str.substr(start, end - start + 1);
        } catch (...) {
            return str;
        }
    }

    std::string Config::toLower(const std::string& str) {
        try {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            return result;
        } catch (...) {
            return str;
        }
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

} // namespace nlm
