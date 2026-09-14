// Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key
                if (key.empty()) {
                    NLM_LOG_ERROR("Error: Empty key in command line argument: " + arg);
                    return false;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARN("Warning: Invalid command line argument format (missing '='): " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            if (arg.length() == 1) {
                NLM_LOG_ERROR("Error: Invalid command line argument: -");
                return false;
            }
            
            std::string key = arg.substr(1);
            if (key.empty()) {
                NLM_LOG_ERROR("Error: Empty key in command line argument: " + arg);
                return false;
            }
            
            if (i + 1 < argc) {
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_ERROR("Error: Missing value for command line argument: -" + key);
                return false;
            }
        } else {
            NLM_LOG_WARN("Warning: Unknown command line argument format: " + arg);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Validate filepath
    if (filepath.empty()) {
        NLM_LOG_ERROR("Error: Empty file path provided for saving");
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Error: Cannot open file for writing: " + filepath);
        return false;
    }
    
    // Check if we have entries to save
    if (pImpl->entries.empty()) {
        NLM_LOG_WARN("Warning: No configuration entries to save to " + filepath);
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Successfully saved configuration to file: " + filepath);
    return true;
}

void Config::processJSON(const nlohmann::json& json, ConfigSource source) {
    if (!json.is_object()) {
        NLM_LOG_ERROR("Error: JSON data must be an object, got " + json.type_name());
        return;
    }
    
    for (const auto& [key, value] : json.items()) {
        if (key.empty()) {
            NLM_LOG_ERROR("Error: Empty key in JSON object");
            continue;
        }
        
        if (value.is_object() || value.is_array()) {
            // For nested objects or arrays, flatten the structure
            processNestedJSON(value, key, source);
        } else {
            setConfigEntryFromJSON(key, value, source);
        }
    }
}

void Config::processNestedJSON(const nlohmann::json& json, const std::string& prefix, ConfigSource source) {
    if (json.is_array()) {
        for (size_t i = 0; i < json.size(); ++i) {
            std::string key = prefix + "[" + std::to_string(i) + "]";
            if (json[i].is_object() || json[i].is_array()) {
                processNestedJSON(json[i], key + ".", source);
            } else {
                setConfigEntryFromJSON(key, json[i], source);
            }
        }
    } else if (json.is_object()) {
        for (const auto& [key, value] : json.items()) {
            if (key.empty()) {
                NLM_LOG_ERROR("Error: Empty key in nested JSON object");
                continue;
            }
            
            std::string newKey = prefix + "." + key;
            if (value.is_object() || value.is_array()) {
                processNestedJSON(value, newKey, source);
            } else {
                setConfigEntryFromJSON(newKey, value, source);
            }
        }
    }
}

void Config::setConfigEntryFromJSON(const std::string& key, const nlohmann::json& value, ConfigSource source) {
    ConfigValue configValue;
    
    if (value.is_string()) {
        configValue = value.get<std::string>();
    } else if (value.is_number_integer()) {
        // Check if it fits into int64_t
        try {
            int64_t val = value.get<int64_t>();
            configValue = val;
        } catch (const std::out_of_range&) {
            NLM_LOG_ERROR("Error: Integer value " + value.dump() + " for key " + key + " is too large for int64_t");
            configValue = 0; // Default value
        }
    } else if (value.is_number_unsigned()) {
        // Check if it fits into uint64_t
        try {
            uint64_t val = value.get<uint64_t>();
            configValue = val;
        } catch (const std::out_of_range&) {
            NLM_LOG_ERROR("Error: Unsigned integer value " + value.dump() + " for key " + key + " is too large for uint64_t");
            configValue = 0; // Default value
        }
    } else if (value.is_number()) {
        // Check if it's actually an integer
        if (value.is_number_integer()) {
            // Check range
            double intpart;
            if (std::modf(value.get<double>(), &intpart) == 0.0 && 
                value.get<double>() >= static_cast<double>(std::numeric_limits<int64_t>::min()) &&
                value.get<double>() <= static_cast<double>(std::numeric_limits<int64_t>::max())) {
                configValue = static_cast<int64_t>(value.get<double>());
            } else {
                NLM_LOG_ERROR("Error: Integer value " + value.dump() + " for key " + key + " is out of range for int64_t");
                configValue = 0; // Default value
            }
        } else {
            // Double value
            double dval = value.get<double>();
            if (std::isfinite(dval)) {
                configValue = dval;
            } else {
                NLM_LOG_ERROR("Error: Invalid (non-finite) double value " + value.dump() + " for key " + key);
                configValue = 0.0f; // Default value
            }
        }
    } else if (value.is_boolean()) {
        configValue = value.get<bool>();
    } else if (value.is_array()) {
        // Handle arrays - for now just store as string representation
        std::vector<std::string> strArray;
        for (size_t i = 0; i < value.size(); ++i) {
            if (value[i].is_string()) {
                strArray.push_back(value[i].get<std::string>());
            } else if (value[i].is_number()) {
                strArray.push_back(std::to_string(value[i].get<double>()));
            } else {
                strArray.push_back(value[i].dump());
            }
        }
        configValue = strArray;
    } else {
        // Fallback to string representation
        configValue = value.dump();
    }
    
    set(key, configValue, source);
}

} // namespace nlm