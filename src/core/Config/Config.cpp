// Helper function implementations

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

std::string Config::parseSimpleKeyValue(const std::string& line) {
    size_t pos = line.find('=');
    if (pos != std::string::npos) {
        std::string value = line.substr(pos + 1);
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        return value;
    }
    return "";
}

ConfigValue Config::stringToConfigValue(const std::string& value) {
    // Try boolean
    if (value == "true") return true;
    if (value == "false") return false;
    
    // Try integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(value, &pos);
        if (pos == value.size()) return intVal;
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) return doubleVal;
    } catch (...) {}
    
    // Return as string
    return value;
}

std::string Config::configValueToString(const ConfigValue& value) {
    return std::visit([](const auto& val) -> std::string {
        if constexpr (std::is_same_v<decltype(val), int64_t>) {
            return std::to_string(val);
        } else if constexpr (std::is_same_v<decltype(val), double>) {
            return std::to_string(val);
        } else if constexpr (std::is_same_v<decltype(val), bool>) {
            return val ? "true" : "false";
        } else if constexpr (std::is_same_v<decltype(val), std::string>) {
            return val;
        } else if constexpr (std::is_same_v<decltype(val), std::vector<int64_t>>) {
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << val[i];
            }
            ss << "]";
            return ss.str();
        } else if constexpr (std::is_same_v<decltype(val), std::vector<double>>) {
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << val[i];
            }
            ss << "]";
            return ss.str();
        } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
            std::stringstream ss;
            ss << "[\"";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) ss << "\", ";
                ss << val[i];
            }
            ss << "\"]";
            return ss.str();
        } else {
            return "";
        }
    }, value);
}

FileFormat Config::detectFileFormat(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    // Convert to lowercase for comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") return FileFormat::JSON;
    if (extension == ".yaml" || extension == ".yml") return FileFormat::YAML;
    
    // For .cfg or unknown extensions, try to detect based on content
    // This is a simple heuristic - check if it starts with JSON structure or YAML structure
    std::ifstream file(filepath);
    if (!file.is_open()) return FileFormat::UNKNOWN;
    
    std::string firstLine;
    if (std::getline(file, firstLine)) {
        // Remove whitespace
        firstLine.erase(0, firstLine.find_first_not_of(" \t\r\n"));
        firstLine.erase(firstLine.find_last_not_of(" \t\r\n") + 1);
        
        // Check for JSON object start
        if (firstLine.size() >= 1 && (firstLine[0] == '{' || firstLine == "[")) {
            return FileFormat::JSON;
        }
        
        // Check for YAML typical start
        if (firstLine.find(':') != std::string::npos && 
            (firstLine.find(':') < firstLine.find(' '))) {
            return FileFormat::YAML;
        }
    }
    
    return FileFormat::SIMPLE;
}

// JSON parsing helper
#ifdef NLM_USE_NLOHMANN_JSON
ConfigValue Config::jsonValueToConfigValue(const json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_unsigned()) {
        return j.get<uint64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        // Handle array types
        if (!j.empty()) {
            // Check element types
            bool allInts = true, allDoubles = true, allStrings = true;
            for (const auto& element : j) {
                if (!element.is_number_integer()) allInts = false;
                if (!element.is_number_float()) allDoubles = false;
                if (!element.is_string()) allStrings = false;
            }
            
            if (allInts) {
                std::vector<int64_t> vec;
                vec.reserve(j.size());
                for (const auto& element : j) {
                    vec.push_back(element.get<int64_t>());
                }
                return vec;
            } else if (allDoubles) {
                std::vector<double> vec;
                vec.reserve(j.size());
                for (const auto& element : j) {
                    vec.push_back(element.get<double>());
                }
                return vec;
            } else if (allStrings) {
                std::vector<std::string> vec;
                vec.reserve(j.size());
                for (const auto& element : j) {
                    vec.push_back(element.get<std::string>());
                }
                return vec;
            }
        }
        // Fallback to string representation
        return j.dump();
    }
    return "";
}
#endif

// YAML parsing helper
#ifdef NLM_USE_YAML_CPP
ConfigValue Config::yamlValueToConfigValue(const YAML::Node& node) {
    if (node.IsScalar()) {
        const std::string value = node.Scalar();
        if (value == "true" || value == "false") {
            return value == "true";
        }
        try {
            // Try integer parsing
            size_t pos;
            int64_t intVal = std::stoll(value, &pos);
            if (pos == value.size()) return intVal;
            
            // Try double parsing
            double doubleVal = std::stod(value, &pos);
            if (pos == value.size()) return doubleVal;
            
            // If it's a string, return it
            return value;
        } catch (...) {
            return value;
        }
    } else if (node.IsSequence()) {
        // Check sequence element types
        bool allInts = true, allDoubles = true, allStrings = true;
        for (const auto& element : node) {
            if (element.IsScalar()) {
                const std::string value = element.Scalar();
                if (value == "true" || value == "false") {
                    allInts = allDoubles = allStrings = false;
                    continue;
                }
                try {
                    size_t pos;
                    int64_t intVal = std::stoll(value, &pos);
                    if (pos == value.size()) {
                        allDoubles = allStrings = false;
                    } else {
                        allInts = false;
                    }
                } catch (...) {
                    allInts = allDoubles = false;
                }
            } else {
                allInts = allDoubles = allStrings = false;
            }
        }
        
        if (allInts) {
            std::vector<int64_t> vec;
            for (const auto& element : node) {
                if (element.IsScalar()) {
                    vec.push_back(std::stoll(element.Scalar()));
                }
            }
            return vec;
        } else if (allDoubles) {
            std::vector<double> vec;
            for (const auto& element : node) {
                if (element.IsScalar()) {
                    vec.push_back(std::stod(element.Scalar()));
                }
            }
            return vec;
        } else if (allStrings) {
            std::vector<std::string> vec;
            for (const auto& element : node) {
                if (element.IsScalar()) {
                    vec.push_back(element.Scalar());
                }
            }
            return vec;
        }
        // Fallback to string representation
        std::stringstream ss;
        ss << node;
        return ss.str();
    } else if (node.IsMap()) {
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
    return "";
}
#endif