void ConfigSchema::addParameter(const ParameterInfo& param) {
    parameters.push_back(param);
    paramMap[param.name] = parameters.size() - 1;
}

const ParameterInfo* ConfigSchema::getParameter(const std::string& name) const {
    auto it = paramMap.find(name);
    if (it != paramMap.end()) {
        return &parameters[it->second];
    }
    return nullptr;
}

std::vector<std::string> ConfigSchema::getRequiredParameters() const {
    std::vector<std::string> required;
    for (const auto& param : parameters) {
        if (param.required) {
            required.push_back(param.name);
        }
    }
    return required;
}

std::vector<ParameterInfo> ConfigSchema::getParametersByCategory(const std::string& category) const {
    std::vector<ParameterInfo> result;
    for (const auto& param : parameters) {
        if (param.category == category) {
            result.push_back(param);
        }
    }
    return result;
}

std::vector<ParameterInfo> ConfigSchema::getOrderedParameters() const {
    std::vector<ParameterInfo> result = parameters;
    std::sort(result.begin(), result.end(),
        [](const ParameterInfo& a, const ParameterInfo& b) { return a.order < b.order; });
    return result;
}

std::vector<ConfigError> ConfigSchema::validate(const std::map<std::string, std::any>& config) const {
    std::vector<ConfigError> errors;
    
    for (const auto& param : parameters) {
        auto it = config.find(param.name);
        
        // Check required parameters
        if (param.required && it == config.end()) {
            errors.emplace_back(ConfigErrorType::MissingDependency, param.name, "present", "missing",
                "Required parameter '" + param.name + "' is missing");
        }
        
        if (it == config.end()) {
            continue; // Skip validation for non-required, missing parameters
        }
        
        // Check parameter type
        const std::any& value = it->second;
        
        switch (param.type) {
            case ParameterType::Int:
                if (value.type() != typeid(int)) {
                    errors.emplace_back(ConfigErrorType::TypeMismatch, param.name, "int", getTypeName(value.type()),
                        "Parameter '" + param.name + "' should be an integer");
                }
                break;
            case ParameterType::Double:
                if (value.type() != typeid(double) && value.type() != typeid(float)) {
                    errors.emplace_back(ConfigErrorType::TypeMismatch, param.name, "float/double", getTypeName(value.type()),
                        "Parameter '" + param.name + "' should be a floating point number");
                }
                break;
            case ParameterType::Bool:
                if (value.type() != typeid(bool)) {
                    errors.emplace_back(ConfigErrorType::TypeMismatch, param.name, "bool", getTypeName(value.type()),
                        "Parameter '" + param.name + "' should be a boolean");
                }
                break;
            case ParameterType::String:
                if (value.type() != typeid(std::string)) {
                    errors.emplace_back(ConfigErrorType::TypeMismatch, param.name, "string", getTypeName(value.type()),
                        "Parameter '" + param.name + "' should be a string");
                }
                break;
            case ParameterType::Choice:
                if (param.choice && value.type() == typeid(std::string)) {
                    const std::string& strValue = std::any_cast<const std::string&>(value);
                    bool valid = false;
                    for (const auto& choice : param.choice->choices) {
                        if (strValue == choice) {
                            valid = true;
                            break;
                        }
                    }
                    if (!valid) {
                        errors.emplace_back(ConfigErrorType::TypeMismatch, param.name, "one of choices", strValue,
                            "Parameter '" + param.name + "' must be one of: " + 
                            formatChoices(param.choice->choices));
                    }
                }
                break;
        }
    }
    
    return errors;
}

std::string ConfigSchema::getDocumentation() const {
    std::ostringstream oss;
    oss << "NLM Configuration Schema\n";
    oss << "==========================\n\n";
    
    for (const auto& param : getOrderedParameters()) {
        oss << "Parameter: " << param.name << "\n";
        oss << "  Type: " << parameterTypeToString(param.type) << "\n";
        oss << "  Description: " << param.description << "\n";
        
        if (param.required) {
            oss << "  Required: Yes\n";
        }
        
        if (param.range) {
            oss << "  Range: " << param.range->min << " to " << param.range->max << "\n";
        }
        
        if (param.choice) {
            oss << "  Choices: " << formatChoices(param.choice->choices) << "\n";
        }
        
        if (!param.tags.empty()) {
            oss << "  Tags: " << joinStrings(param.tags, ", ") << "\n";
        }
        
        if (!param.category.empty()) {
            oss << "  Category: " << param.category << "\n";
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

ConfigSchema ConfigSchema::createQuickSchema() {
    ConfigSchema schema;
    
    // Quick preset parameters
    schema.addParameter(ParameterInfo("mode", "Operating mode", ParameterType::Choice,
        std::nullopt, ChoiceSpec({"quick", "balanced", "research"}), std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"performance"}, "quick", 0, ""));
    
    schema.addParameter(ParameterInfo("performance", "Performance level (0-100)", ParameterType::Range,
        RangeSpec("0", "100"), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"speed", "quality"}, "95", 1, "%"));
    
    schema.addParameter(ParameterInfo("memory", "Memory allocation in MB", ParameterType::Int,
        RangeSpec("512", "8192"), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"resources"}, "4096", 2, "MB"));
    
    schema.addParameter(ParameterInfo("threads", "Number of threads to use", ParameterType::Int,
        RangeSpec("1", "16"), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"performance"}, "8", 3, ""));
    
    schema.addParameter(ParameterInfo("enable_cuda", "Enable CUDA acceleration", ParameterType::Bool,
        std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::vector<std::string>{"hardware"},
        "false", 4, ""));
    
    return schema;
}

ConfigSchema ConfigSchema::createBalancedSchema() {
    ConfigSchema schema;
    
    // Balanced preset parameters
    schema.addParameter(ParameterInfo("mode", "Operating mode", ParameterType::Choice,
        std::nullopt, ChoiceSpec({"quick", "balanced", "research"}), std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"performance"}, "balanced", 0, ""));
    
    schema.addParameter(ParameterInfo("performance", "Performance level (0-100)", ParameterType::Range,
        RangeSpec("0", "100"), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"speed", "quality"}, "75", 1, "%"));
    
    schema.addParameter(ParameterInfo("memory", "Memory allocation in MB", ParameterType::Int,
        RangeSpec("1024", "16384"), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"resources", "memory"}, "8192", 2, "MB"));
    
    schema.addParameter(ParameterInfo("threads", "Number of threads to use", ParameterType::Int,
        RangeSpec("2", "16"), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"performance", "parallelism"}, "8", 3, ""));
    
    schema.addParameter(ParameterInfo("enable_cuda", "Enable CUDA acceleration", ParameterType::Bool,
        std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::vector<std::string>{"hardware"},
        "false", 4, ""));
    
    schema.addParameter(ParameterInfo("verbose", "Enable verbose logging", ParameterType::Bool,
        std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::vector<std::string>{"debug"},
        "false", 5, ""));
    
    return schema;
}

ConfigSchema ConfigSchema::createResearchSchema() {
    ConfigSchema schema;
    
    // Research preset parameters
    schema.addParameter(ParameterInfo("mode", "Operating mode", ParameterType::Choice,
        std::nullopt, ChoiceSpec({"quick", "balanced", "research"}), std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"accuracy"}, "research", 0, ""));
    
    schema.addParameter(ParameterInfo("performance", "Performance level (0-100)", ParameterType::Range,
        RangeSpec("0", "100"), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"accuracy", "quality"}, "60", 1, "%"));
    
    schema.addParameter(ParameterInfo("memory", "Memory allocation in MB", ParameterType::Int,
        RangeSpec("16384", "65536"), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"resources", "memory", "accuracy"}, "32768", 2, "MB"));
    
    schema.addParameter(ParameterInfo("threads", "Number of threads to use", ParameterType::Int,
        RangeSpec("4", "32"), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"parallelism", "accuracy"}, "16", 3, ""));
    
    schema.addParameter(ParameterInfo("enable_cuda", "Enable CUDA acceleration", ParameterType::Bool,
        std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::vector<std::string>{"hardware", "performance"},
        "true", 4, ""));
    
    schema.addParameter(ParameterInfo("verbose", "Enable verbose logging", ParameterType::Bool,
        std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::vector<std::string>{"debug", "analysis"},
        "true", 5, ""));
    
    schema.addParameter(ParameterInfo("checkpoint_interval", "Checkpoint interval in seconds", ParameterType::Int,
        RangeSpec("300", "7200"), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"recovery", "research"}, "1800", 6, "s"));
    
    return schema;
}

std::string ConfigSchema::formatChoices(const std::vector<std::string>& choices) const {
    std::ostringstream oss;
    for (size_t i = 0; i < choices.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << choices[i] << "\"";
    }
    return oss.str();
}

std::string ConfigSchema::joinStrings(const std::vector<std::string>& strings, const std::string& delimiter) const {
    std::ostringstream oss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) oss << delimiter;
        oss << strings[i];
    }
    return oss.str();
}

std::string ConfigSchema::getTypeName(const std::type_info& type) const {
    std::string name = type.name();
    
    // Simplify type names
    if (name.find("int") != std::string::npos && name.find("64") != std::string::npos) {
        return "int64_t";
    } else if (name.find("int") != std::string::npos) {
        return "int";
    } else if (name.find("double") != std::string::npos) {
        return "double";
    } else if (name.find("float") != std::string::npos) {
        return "float";
    } else if (name.find("bool") != std::string::npos) {
        return "bool";
    } else if (name.find("string") != std::string::npos) {
        return "string";
    } else if (name.find("vector") != std::string::npos) {
        return "vector";
    }
    
    return name;
}

std::string ConfigSchema::parameterTypeToString(ParameterType type) const {
    switch (type) {
        case ParameterType::Int: return "int";
        case ParameterType::Int64: return "int64_t";
        case ParameterType::Float: return "float";
        case ParameterType::Double: return "double";
        case ParameterType::Bool: return "bool";
        case ParameterType::String: return "string";
        case ParameterType::StringList: return "string[]";
        case ParameterType::IntList: return "int[]";
        case ParameterType::FloatList: return "float[]";
        case ParameterType::Choice: return "choice";
        case ParameterType::Range: return "range";
        case ParameterType::Vector2D: return "vector2d";
        case ParameterType::Vector3D: return "vector3d";
        case ParameterType::Matrix: return "matrix";
        case ParameterType::Custom: return "custom";
        default: return "unknown";
    }
}
