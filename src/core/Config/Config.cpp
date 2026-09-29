#include "../src/core/Exceptions/NLMBaseException.hpp"
#include <stdexcept>
#include <filesystem>

namespace nlm {

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Check if file exists
        if (!std::filesystem::exists(filepath)) {
            throw FileNotFoundException(filepath);
        }
        
        // Check if file is readable
        if (!std::filesystem::is_regular_file(filepath) || !std::filesystem::status(filepath).permissions() & std::filesystem::perms::owner_read) {
            throw FilePermissionException(filepath);
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw FilePermissionException(filepath, "Could not open file for reading");
        }
        
        // Try to parse as JSON (if available)
        // For now, handle both simple key=value and JSON formats
        bool parsedSuccessfully = false;
        
        // First try to parse as JSON
        nlohmann::json jsonData;
        try {
            file >> jsonData;
            parsedSuccessfully = true;
            
            // Process JSON data
            if (jsonData.is_object()) {
                for (const auto& [key, value] : jsonData.items()) {
                    ConfigSource source = ConfigSource::File;
                    std::string description = "Loaded from JSON file";
                    
                    // Convert JSON value to ConfigValue
                    ConfigValue configValue;
                    if (value.is_string()) {
                        configValue = value.get<std::string>();
                    } else if (value.is_number_integer()) {
                        configValue = value.get<int>();
                    } else if (value.is_number_float()) {
                        configValue = value.get<double>();
                    } else if (value.is_boolean()) {
                        configValue = value.get<bool>();
                    } else if (value.is_array()) {
                        std::vector<double> vec;
                        for (const auto& item : value) {
                            if (item.is_number()) {
                                vec.push_back(item.get<double>());
                            }
                        }
                        configValue = vec;
                    }
                    
                    set(key, configValue, source);
                }
            }
        } catch (const std::exception&) {
            // JSON parsing failed, try simple key=value format
            file.clear();
            file.seekg(0, std::ios::beg);
        }
        
        // Try to parse as simple key=value format
        if (!parsedSuccessfully) {
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
                    
                    if (key.empty()) {
                        throw ValidationException("Empty key found at line " + std::to_string(lineNumber));
                    }
                    
                    // Remove quotes if present
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    try {
                        set(key, value, ConfigSource::File);
                    } catch (const std::exception& e) {
                        throw FileFormatException(filepath, 
                            "Parse error at line " + std::to_string(lineNumber) + ": " + std::string(e.what()));
                    }
                } else {
                    throw FileFormatException(filepath, 
                        "Invalid format at line " + std::to_string(lineNumber) + ": expected 'key=value' format");
                }
            }
        }
        
        return true;
        
    } catch (const FileNotFoundException& e) {
        return false;
    } catch (const FilePermissionException& e) {
        return false;
    } catch (const FileFormatException& e) {
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Check if filepath is valid
        if (filepath.empty()) {
            throw ValidationException("File path cannot be empty");
        }
        
        // Check if directory exists (if filepath includes directories)
        std::filesystem::path path(filepath);
        if (path.has_parent_path()) {
            std::filesystem::path parentDir = path.parent_path();
            if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
                throw FileNotFoundException("Parent directory does not exist: " + parentDir.string());
            }
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw FileWriteException(filepath, "Could not open file for writing");
        }
        
        // Set file permissions
        file << std::left;
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Serialize the value
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, double>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, int>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << arg[i] << "\"";
                    }
                    file << "]";
                } else {
                    file << "UNKNOWN_TYPE";
                }
            }, entry.value);
            
            file << "\n\n";
        }
        
        if (!file) {
            throw FileWriteException(filepath, "Error writing to file");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    try {
        if (key.empty()) {
            throw NullOrEmptyException("Key");
        }
        
        // Validate key format
        if (!std::all_of(key.begin(), key.end(), [](char c) {
            return std::isalnum(c) || c == '_' || c == '-' || c == '.';
        })) {
            throw ValidationException("Invalid key format: '" + key + "'. Only alphanumeric characters, underscores, hyphens, and dots are allowed.");
        }
        
        // Validate value based on type
        std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                // String validation
                if (val.length() > 10000) {
                    throw ValidationException("String value too long (max 10000 characters)");
                }
            } else if constexpr (std::is_same_v<T, double>) {
                // Double validation
                if (std::isinf(val) || std::isnan(val)) {
                    throw ValidationException("Invalid double value: infinity or NaN");
                }
            } else if constexpr (std::is_same_v<T, int>) {
                // Integer validation
                if (val < -2147483648 || val > 2147483647) {
                    throw ValidationException("Integer value out of range [-2147483648, 2147483647]");
                }
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                // Vector validation
                if (val.size() > 10000) {
                    throw ValidationException("Vector too large (max 10000 elements)");
                }
                for (const auto& item : val) {
                    if (std::isinf(item) || std::isnan(item)) {
                        throw ValidationException("Invalid vector element: infinity or NaN");
                    }
                }
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                // Vector validation
                if (val.size() > 10000) {
                    throw ValidationException("Vector too large (max 10000 elements)");
                }
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                // Vector validation
                if (val.size() > 10000) {
                    throw ValidationException("Vector too large (max 10000 elements)");
                }
                for (const auto& item : val) {
                    if (item.length() > 1000) {
                        throw ValidationException("String in vector too long (max 1000 characters)");
                    }
                }
            }
        }, value);
        
        // Set the value
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
        
    } catch (const FileFormatException& e) {
        // Re-throw format exceptions
        throw;
    } catch (const ValidationException& e) {
        // Re-throw validation exceptions
        throw;
    } catch (const std::exception& e) {
        throw ValidationException("Failed to set configuration key: " + std::string(e.what()));
    }
}

// Add other Config methods with error handling...

} // namespace nlm