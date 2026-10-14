# Configuration System Error Handling and Validation

## Critical Issues Identified

The NLM configuration system has significant gaps in error handling and validation:

1. **No input validation** in Config.set() methods
2. **Poor error messages** for type mismatches
3. **No configuration validation** for required parameters
4. **Inadequate file I/O error handling**
5. **Missing exception types** for different error categories

## Implementation Plan

### 1. Enhanced Error Handling in Config.cpp

#### 1.1 Custom Exception Hierarchy
**Create comprehensive exception classes** in `src/core/Config/Config.hpp`:

```cpp
namespace nlm {

// Base configuration exception
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message) : std::runtime_error(message) {}
};

// Configuration file errors
class ConfigFileException : public ConfigException {
public:
    ConfigFileException(const std::string& filepath, const std::string& reason)
        : ConfigException("Configuration file error in \"" + filepath + "\": " + reason) {}
};

// Configuration validation errors
class ConfigValidationException : public ConfigException {
public:
    ConfigValidationException(const std::string& key, const std::string& reason)
        : ConfigException("Configuration validation error for key \"" + key + "\": " + reason) {}
};

// Configuration type errors
class ConfigTypeException : public ConfigException {
public:
    ConfigTypeException(const std::string& key, const std::string& expectedType, const std::string& actualType)
        : ConfigException("Type error for key \"" + key + "\": expected " + expectedType + ", got " + actualType) {}
};

// Configuration source errors
class ConfigSourceException : public ConfigException {
public:
    ConfigSourceException(const std::string& key, const std::string& source, const std::string& reason)
        : ConfigException("Source error for key \"" + key + "\" from " + source + ": " + reason) {}
};

} // namespace nlm
```

#### 1.2 Enhanced Config.set() Methods with Validation
**Update `src/core/Config/Config.cpp`**:

**Current**: Simple type conversion with try-catch
**Improved**: Comprehensive validation with specific error messages

```cpp
// Enhanced string setting with validation
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        throw ConfigException("Configuration key cannot be empty");
    }
    
    if (!std::isalpha(key[0]) && key[0] != '_') {
        throw ConfigException("Configuration key must start with letter or underscore: '" + key + "'");
    }
    
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '.') {
            throw ConfigException("Invalid character in configuration key '" + key + "'");
        }
    }
    
    // Validate value
    if (value.empty() && key.find("required") != std::string::npos) {
        throw ConfigValidationException(key, "Required configuration parameter cannot be empty");
    }
    
    set(key, ConfigValue(value), source);
}

// Enhanced integer setting with range validation
void Config::set(const std::string& key, int value, ConfigSource source) {
    // Validate key format
    if (key.empty()) {
        throw ConfigException("Configuration key cannot be empty");
    }
    
    // Validate value range for common configuration parameters
    if (key == "brain.neuron_count" && (value <= 0 || value > 100000)) {
        throw ConfigValidationException(key, "Neuron count must be between 1 and 100000");
    }
    
    if (key == "brain.connection_probability" && (value < 0 || value > 1)) {
        throw ConfigValidationException(key, "Connection probability must be between 0 and 1");
    }
    
    set(key, ConfigValue(value), source);
}

// Enhanced double setting with validation
void Config::set(const std::string& key, double value, ConfigSource source) {
    // Validate key format
    if (key.empty()) {
        throw ConfigException("Configuration key cannot be empty");
    }
    
    // Validate value ranges
    if (key == "brain.simulation_timestep" && (value <= 0.0 || value > 1.0)) {
        throw ConfigValidationException(key, "Simulation timestep must be between 0.0 and 1.0 seconds");
    }
    
    if (key == "brain.v_thresh" && (value < -100.0 || value > 50.0)) {
        throw ConfigValidationException(key, "Neuron threshold must be between -100.0 and 50.0 mV");
    }
    
    set(key, ConfigValue(value), source);
}

// Enhanced boolean setting
void Config::set(const std::string& key, bool value, ConfigSource source) {
    // Boolean values don't need range validation, but validate key
    if (key.empty()) {
        throw ConfigException("Configuration key cannot be empty");
    }
    
    // Log boolean settings for debugging
    if (key.find("enable") != std::string::npos || key.find("enabled") != std::string::npos) {
        NLM_LOG_INFO("Configuration enabled: " + key);
    }
    
    set(key, ConfigValue(value), source);
}

// Enhanced vector setting with validation
void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        throw ConfigException("Configuration key cannot be empty");
    }
    
    // Validate vector size
    if (value.size() > 10000) {
        throw ConfigValidationException(key, "Vector size cannot exceed 10000 elements");
    }
    
    // Validate individual elements
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] < -1000000 || value[i] > 1000000) {
            throw ConfigValidationException(key, "Element at index " + std::to_string(i) + " out of range");
        }
    }
    
    set(key, ConfigValue(value), source);
}
```

#### 1.3 Enhanced Input Validation for get() Methods
**Improve error handling in template get() methods**:

```cpp
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Validate key format
    if (key.empty()) {
        throw ConfigException("Configuration key cannot be empty");
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt; // Key doesn't exist - return optional for backward compatibility
    }
    
    try {
        // Attempt type conversion
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        // Provide detailed error information
        std::string actualType = getTypeName(it->value);
        std::string expectedType = getTypeName<T>();
        throw ConfigTypeException(key, expectedType, actualType);
    }
}

// Helper to get type name for better error messages
template<typename T>
std::string Config::getTypeName() const {
    if constexpr (std::is_same_v<T, int>) return "int";
    if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
    if constexpr (std::is_same_v<T, double>) return "double";
    if constexpr (std::is_same_v<T, bool>) return "bool";
    if constexpr (std::is_same_v<T, std::string>) return "std::string";
    if constexpr (std::is_same_v<T, std::vector<int>>) return "std::vector<int>";
    if constexpr (std::is_same_v<T, std::vector<double>>) return "std::vector<double>";
    if constexpr (std::is_same_v<T, std::vector<std::string>>) return "std::vector<std::string>";
    return "unknown";
}
```

#### 1.4 Enhanced File I/O Error Handling
**Improve loadFromFile() and saveToFile()**:

```cpp
bool Config::loadFromFile(const std::string& filepath) {
    // Validate filepath
    if (filepath.empty()) {
        throw ConfigFileException("", "Filepath cannot be empty");
    }
    
    if (filepath.length() > 4096) {
        throw ConfigFileException(filepath, "Filepath too long (max 4096 characters)");
    }
    
    try {
        // Try JSON first, then fallback to simple key=value format
        bool success = false;
        
        // First try to parse as JSON
        success = loadFromJSON(filepath);
        if (!success) {
            // If JSON fails, try simple key=value format
            success = loadFromSimpleFormat(filepath);
        }
        
        return success;
    } catch (const ConfigException& e) {
        // Re-throw configuration-specific exceptions
        throw;
    } catch (const std::exception& e) {
        // Wrap other exceptions
        throw ConfigFileException(filepath, std::string("Parse error: ") + e.what());
    } catch (...) {
        // Catch-all for unexpected errors
        throw ConfigFileException(filepath, "Unknown error occurred");
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    // Validate filepath
    if (filepath.empty()) {
        throw ConfigFileException("", "Filepath cannot be empty");
    }
    
    if (filepath.length() > 4096) {
        throw ConfigFileException(filepath, "Filepath too long (max 4096 characters)");
    }
    
    // Check if filepath is writable (Unix check)
    struct stat buffer;
    if (stat(filepath.c_str(), &buffer) == 0) {
        if (!(buffer.st_mode & S_IWUSR)) {
            throw ConfigFileException(filepath, "File exists and is not writable");
        }
        if (access(filepath.c_str(), W_OK) != 0) {
            throw ConfigFileException(filepath, "File is not writable");
        }
    }
    
    std::ofstream file(filepath, std::ios::trunc);
    if (!file.is_open()) {
        throw ConfigFileException(filepath, "Cannot open file for writing");
    }
    
    try {
        // Save configuration with validation
        for (const auto& entry : pImpl->entries) {
            // Validate entry before saving
            if (entry.key.empty()) {
                throw ConfigException("Cannot save entry with empty key");
            }
            
            // Add description as comment if available
            if (!entry.description.empty()) {
                file << "# " << entry.description << "\n";
            }
            
            // Save key=value pair
            file << entry.key << " = ";
            
            // Convert ConfigValue to string
            file << valueToString(entry.value);
            
            file << "\n\n";  // Empty line between entries
        }
        
        file.flush();
        
        // Verify file was written successfully
        if (file.fail()) {
            throw ConfigFileException(filepath, "Error writing to file");
        }
        
        return true;
    } catch (const ConfigException& e) {
        // Re-throw configuration-specific exceptions
        throw;
    } catch (const std::exception& e) {
        // Close file and wrap other exceptions
        file.close();
        throw ConfigFileException(filepath, std::string("Write error: ") + e.what());
    } catch (...) {
        // Catch-all for unexpected errors
        file.close();
        throw ConfigFileException(filepath, "Unknown error occurred while writing");
    }
}
```

#### 1.5 Enhanced Configuration Validation
**Add comprehensive validation methods**:

```cpp
// Validate configuration completeness
bool Config::validate() const {
    std::vector<std::string> errors;
    
    // Check for required parameters
    std::vector<std::string> requiredKeys = {
        "brain.neuron_count",
        "brain.region_count", 
        "brain.connection_probability"
    };
    
    for (const std::string& key : requiredKeys) {
        if (!has(key)) {
            errors.push_back("Missing required parameter: " + key);
        }
    }
    
    // Validate specific parameters
    auto neuronCount = get<int64_t>("brain.neuron_count");
    if (neuronCount && (*neuronCount <= 0 || *neuronCount > 1000000)) {
        errors.push_back("brain.neuron_count must be between 1 and 1000000");
    }
    
    auto connectionProb = get<double>("brain.connection_probability");
    if (connectionProb && (*connectionProb < 0.0 || *connectionProb > 1.0)) {
        errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
    }
    
    auto timestep = get<double>("brain.simulation_timestep");
    if (timestep && (*timestep <= 0.0 || *timestep > 1.0)) {
        errors.push_back("brain.simulation_timestep must be between 0.0 and 1.0");
    }
    
    if (!errors.empty()) {
        std::string errorMsg = "Configuration validation failed:\n";
        for (const std::string& error : errors) {
            errorMsg += "  - " + error + "\n";
        }
        throw ConfigValidationException("general", errorMsg);
    }
    
    return true;
}

// Validate configuration for specific use case
bool Config::validateForSimulation() const {
    try {
        // Check for simulation-critical parameters
        if (!has("brain.neuron_count")) {
            throw ConfigValidationException("brain.neuron_count", "Required for simulation");
        }
        
        if (!has("brain.simulation_timestep")) {
            throw ConfigValidationException("brain.simulation_timestep", "Required for simulation");
        }
        
        // Validate neuron count is reasonable
        auto neuronCount = get<int64_t>("brain.neuron_count");
        if (!neuronCount || *neuronCount == 0) {
            throw ConfigValidationException("brain.neuron_count", "Must be greater than 0");
        }
        
        if (*neuronCount > 1000000) {
            throw ConfigValidationException("brain.neuron_count", "Too large (max 1000000)");
        }
        
        // Validate timestep
        auto timestep = get<double>("brain.simulation_timestep");
        if (!timestep || *timestep <= 0.0) {
            throw ConfigValidationException("brain.simulation_timestep", "Must be positive");
        }
        
        if (*timestep > 1.0) {
            throw ConfigValidationException("brain.simulation_timestep", "Too large (max 1.0)");
        }
        
        return true;
    } catch (const ConfigException& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigException(std::string("Unexpected validation error: ") + e.what());
    }
}
```

#### 1.6 Enhanced Summary Method with Validation
**Improve summary() method to include validation info**:

```cpp
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    // Check if configuration is valid
    bool isValid = true;
    std::string validationError;
    
    try {
        validate();
    } catch (const ConfigException& e) {
        isValid = false;
        validationError = e.what();
    }
    
    oss << "  Status: " << (isValid ? "VALID" : "INVALID") << std::endl;
    if (!isValid) {
        oss << "  Error: " << validationError << std::endl;
    }
    
    oss << "  Entry count: " << pImpl->entries.size() << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}
```

### 2. Python Error Handling Integration

#### 2.1 Custom Exception Classes for Python
**Create Python exception hierarchy** in `python/bindings.cpp`:

```cpp
// Register custom exception classes with Python
py::register_exception<nlm::ConfigException>(m, "NLMConfigError");
py::register_exception<nlm::ConfigFileException>(m, "NLMConfigFileError");
py::register_exception<nlm::ConfigValidationException>(m, "NLMConfigValidationError");
py::register_exception<nlm::ConfigTypeException>(m, "NLMConfigTypeError");

// Additional general exceptions
py::register_exception<std::runtime_error>(m, "RuntimeError");
py::register_exception<std::invalid_argument>(m, "ArgumentError");
py::register_exception<std::out_of_range>(m, "RangeError");
py::register_exception<std::logic_error>(m, "LogicError");
```

#### 2.2 Enhanced Python Binding Error Handling
**Wrap Python bindings with better error handling**:

```cpp
// Enhanced Config bindings with error handling
py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
    .def(py::init<>())
    .def("loadFromFile", [](Config& self, const std::string& filepath) {
        try {
            bool success = self.loadFromFile(filepath);
            if (!success) {
                throw nlm::ConfigFileException(filepath, "File not found or invalid format");
            }
            return success;
        } catch (const nlm::ConfigException& e) {
            // Re-throw NLM exceptions
            throw;
        } catch (const std::exception& e) {
            // Wrap other exceptions
            throw nlm::ConfigException(std::string("Error loading config file: ") + e.what());
        }
    }, py::arg("filepath"), "Load configuration from file")
    
    .def("set", [](Config& self, const std::string& key, const std::string& value) {
        try {
            self.set(key, value);
        } catch (const nlm::ConfigException& e) {
            throw;
        } catch (const std::exception& e) {
            throw nlm::ConfigException(std::string("Error setting configuration: ") + e.what());
        }
    }, py::arg("key"), py::arg("value"), "Set configuration value")
    
    .def("getOr", [](Config& self, const std::string& key, const std::string& defaultValue) {
        try {
            auto result = self.get<std::string>(key);
            return result.has_value() ? result.value() : defaultValue;
        } catch (const nlm::ConfigException& e) {
            throw;
        } catch (const std::exception& e) {
            throw nlm::ConfigException(std::string("Error getting configuration: ") + e.what());
        }
    }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default")
    
    .def("validate", [](Config& self) {
        try {
            return self.validate();
        } catch (const nlm::ConfigException& e) {
            throw;
        } catch (const std::exception& e) {
            throw nlm::ConfigException(std::string("Error validating configuration: ") + e.what());
        }
    }, "Validate configuration and return True if valid");
```

#### 2.3 Python Exception Translation
**Create wrapper functions for better exception handling**:

```cpp
// Exception translation utilities
namespace {
    
    template<typename ExceptionType>
    void translateException(py::object& exc) {
        try {
            throw;
        } catch (const nlm::ConfigException& e) {
            exc = py::cast(nlm::ConfigException(e.what()));
        } catch (const std::runtime_error& e) {
            exc = py::cast(std::runtime_error(e.what()));
        } catch (const std::invalid_argument& e) {
            exc = py::cast(std::invalid_argument(e.what()));
        } catch (const std::out_of_range& e) {
            exc = py::cast(std::out_of_range(e.what()));
        } catch (const std::exception& e) {
            exc = py::cast(std::runtime_error(std::string("NLM error: ") + e.what()));
        } catch (...) {
            exc = py::cast(std::runtime_error("Unknown NLM error"));
        }
    }
    
} // anonymous namespace

// Exception translation for functions that can throw multiple exception types
template<typename F>
auto withExceptionTranslation(F&& func) {
    return [func = std::forward<F>(func)](auto&&... args) -> decltype(auto) {
        py::object exc;
        try {
            return func(std::forward<decltype(args)>(args)...);
        } catch (...) {
            translateException(exc);
            PyErr_SetObject(exc.ptr(), exc.ptr());
            throw py::error_already_set();
        }
    };
}
```

### 3. System Integration Error Handling

#### 3.1 Enhanced Brain Initialization Error Handling
**Improve Brain initialization with better error handling**:

```cpp
bool Brain::initialize() {
    try {
        // Validate configuration before initialization
        if (pImpl->config) {
            pImpl->config->validate();
        }
        
        // Check if required systems are available
        if (!pImpl->spikeSystem) {
            NLM_LOG_ERROR("Spike system not initialized");
            return false;
        }
        
        // Initialize other systems
        if (!pImpl->spikeSystem->initialize()) {
            NLM_LOG_ERROR("Failed to initialize spike system");
            return false;
        }
        
        // Initialize memory systems if available
        if (pImpl->workingMemory) {
            if (!pImpl->workingMemory->initialize(this)) {
                NLM_LOG_ERROR("Failed to initialize working memory");
                return false;
            }
        }
        
        // Similar checks for other systems...
        
        return true;
    } catch (const nlm::ConfigException& e) {
        NLM_LOG_ERROR("Configuration error during initialization: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error during initialization: " + std::string(e.what()));
        return false;
    } catch (...) {
        NLM_LOG_ERROR("Unknown error during initialization");
        return false;
    }
}
```

#### 3.2 Enhanced System Connection Validation
**Add validation for system integration**:

```cpp
void Brain::validateSystemConnections() const {
    try {
        // Check if memory systems are connected to neural processing
        if (pImpl->workingMemory && !pImpl->workingMemory->isInitialized()) {
            NLM_LOG_WARNING("Working memory initialized but not connected to neural processing");
        }
        
        if (pImpl->episodicMemory && !pImpl->episodicMemory->isInitialized()) {
            NLM_LOG_WARNING("Episodic memory initialized but not connected to neural processing");
        }
        
        // Check if prediction system is integrated
        if (pImpl->predictionSystem && !pImpl->predictionSystem->isActive()) {
            NLM_LOG_WARNING("Prediction system initialized but not active in brain loop");
        }
        
        // Check if cognitive systems are connected
        if (pImpl->planner && !pImpl->planner->isInitialized()) {
            NLM_LOG_WARNING("Neural planner initialized but not connected to action selection");
        }
        
        // Check neuromodulation integration
        if (pImpl->dopamine && !pImpl->dopamine->isActive()) {
            NLM_LOG_WARNING("Dopamine system initialized but not active");
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error validating system connections: " + std::string(e.what()));
    }
}
```

## Implementation Files

### Core Files to Modify
- `src/core/Config/Config.hpp` - Add exception classes and validation methods
- `src/core/Config/Config.cpp` - Implement enhanced error handling and validation

### Python Bindings to Update
- `python/bindings.cpp` - Add exception registration and error handling
- `python/nlm_exceptions.py` (if created) - Custom Python exception classes

## Implementation Timeline

### Week 1: Exception Classes and Basic Validation
- Create custom exception hierarchy
- Add basic validation methods to Config class
- Implement key validation in set() methods

### Week 2: Enhanced File I/O and Type Validation
- Improve loadFromFile() and saveToFile() error handling
- Add comprehensive type validation
- Implement configuration validation methods

### Week 3: Python Integration
- Add exception registration to Python bindings
- Create Python wrapper functions with error handling
- Update documentation with error handling information

### Week 4: System Integration Validation
- Add system connection validation to Brain class
- Implement validation for Phase 4+ system integration
- Create testing for error handling scenarios

## Testing Strategy

### Unit Tests
1. **Config validation tests**: Test validation methods with various inputs
2. **Error message tests**: Verify error messages are informative
3. **Exception tests**: Test exception types and inheritance
4. **File I/O tests**: Test file loading/saving error scenarios

### Integration Tests
1. **Configuration loading tests**: Test complete configuration workflows
2. **Brain initialization tests**: Test error handling during brain setup
3. **Python binding tests**: Test Python exception handling

## Expected Improvements

### After Implementation
1. **Better Error Messages**: Clear, actionable error messages
2. **Input Validation**: Early detection of invalid inputs
3. **Exception Safety**: Proper exception handling throughout system
4. **Python Integration**: Seamless Python error handling
5. **System Validation**: Early detection of integration issues

### Benefits
1. **Easier Debugging**: Developers can quickly identify and fix issues
2. **Better User Experience**: Users get clear guidance on what's wrong
3. **Improved Reliability**: Early validation prevents runtime errors
4. **Better Testing**: Comprehensive test coverage for error scenarios
5. **Documentation**: Error messages serve as documentation

## Risk Mitigation

### High Risks
1. **Breaking Existing Code**: Use backward-compatible exception types
2. **Performance Overhead**: Optimize validation for production use
3. **Documentation Overload**: Focus on critical error scenarios

### Mitigation Strategies
1. **Gradual Rollout**: Add validation incrementally
2. **Performance Profiling**: Monitor impact on critical paths
3. **Error Message Guidelines**: Follow consistent documentation standards

## Conclusion

The configuration system and Python bindings currently lack comprehensive error handling and validation. Implementing the improvements outlined above will significantly enhance the robustness, usability, and developer experience of the NLM system.

**The recommended approach**:

1. **First**: Add exception classes and basic validation (Week 1)
2. **Then**: Improve file I/O and type validation (Week 2)  
3. **Finally**: Integrate with Python and validate system connections (Weeks 3-4)

This incremental approach ensures that each improvement builds on the previous one, creating a robust foundation for the NLM system.

**Immediate Action Items**:
1. Create custom exception hierarchy for configuration errors
2. Add validation to key configuration methods
3. Implement better file I/O error handling
4. Integrate error handling with Python bindings
5. Add system connection validation

The improvements will transform NLM from a system with poor error handling into a **robust, well-validated system** that provides clear guidance to users and developers.
