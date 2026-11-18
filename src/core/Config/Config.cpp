// Configuration validation implementation

namespace nlm {

// ConfigSchema implementation
void ConfigSchema::addRule(const ConfigValidationRule& rule) {
    rules.push_back(rule);
}

ValidationResult ConfigSchema::validate(const Config& config, std::string& errorMessage) const {
    bool hasErrors = false;
    for (const auto& rule : rules) {
        ValidationResult result = ValidationResult::Success;
        
        // Check if required key exists
        if (rule.required && !config.has(rule.key)) {
            errorMessage += "Missing required key: " + rule.key + "\n";
            hasErrors = true;
            continue;
        }
        
        // If key doesn't exist and not required, skip validation
        if (!config.has(rule.key)) {
            continue;
        }
        
        // Validate the value
        std::optional<nlm::ConfigValue> value = config.get<nlm::ConfigValue>(rule.key);
        if (!value) {
            errorMessage += "Cannot get value for key: " + rule.key + "\n";
            hasErrors = true;
            continue;
        }
        
        result = rule.validator(*value);
        
        if (result != ValidationResult::Success) {
            errorMessage += "Validation failed for key '" + rule.key + "': " + rule.errorMessage + "\n";
            hasErrors = true;
        }
    }
    
    return hasErrors ? ValidationResult::InvalidType : ValidationResult::Success;
}

std::vector<std::string> ConfigSchema::getValidationErrors(const Config& config) const {
    std::vector<std::string> errors;
    std::string errorMessage;
    
    if (validate(config, errorMessage) != ValidationResult::Success) {
        // Split error message into lines
        size_t pos = 0;
        size_t newlinePos;
        while ((newlinePos = errorMessage.find('\n', pos)) != std::string::npos) {
            errors.push_back(errorMessage.substr(pos, newlinePos - pos));
            pos = newlinePos + 1;
        }
        if (pos < errorMessage.size()) {
            errors.push_back(errorMessage.substr(pos));
        }
    }
    
    return errors;
}

bool ConfigSchema::isValid(const Config& config) const {
    std::string errorMessage;
    return validate(config, errorMessage) == ValidationResult::Success;
}

std::vector<std::string> ConfigSchema::getRequiredKeys() const {
    std::vector<std::string> requiredKeys;
    for (const auto& rule : rules) {
        if (rule.required) {
            requiredKeys.push_back(rule.key);
        }
    }
    return requiredKeys;
}

size_t ConfigSchema::getErrorCount(const Config& config) const {
    std::vector<std::string> errors = getValidationErrors(config);
    return errors.size();
}

// Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first, then fallback to simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    nlohmann::json jsonData;
    try {
        jsonData = nlohmann::json::parse(file);
    } catch (...) {
        // If JSON parsing fails, try simple key=value format
        std::string line;
        while (std::getline(file, line)) {
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
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
    
    // Successfully parsed as JSON
    try {
        // Convert JSON to config entries
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            const auto& value = it.value();
            
            // Convert JSON value to ConfigValue
            if (value.is_string()) {
                set(key, value.get<std::string>(), ConfigSource::File);
            } else if (value.is_boolean()) {
                set(key, value.get<bool>(), ConfigSource::File);
            } else if (value.is_number_integer()) {
                // Try int64_t first, then int
                try {
                    set(key, value.get<int64_t>(), ConfigSource::File);
                } catch (...) {
                    set(key, value.get<int>(), ConfigSource::File);
                }
            } else if (value.is_number_float()) {
                set(key, value.get<double>(), ConfigSource::File);
            } else if (value.is_array()) {
                // Handle array types
                if (value.empty()) {
                    continue;
                }
                
                // Check array element type
                bool allInts = std::all_of(value.begin(), value.end(), 
                    [](const auto& v) { return v.is_number_integer(); });
                bool allDoubles = std::all_of(value.begin(), value.end(), 
                    [](const auto& v) { return v.is_number_float(); });
                bool allStrings = std::all_of(value.begin(), value.end(), 
                    [](const auto& v) { return v.is_string(); });
                
                if (allInts) {
                    std::vector<int64_t> intArray;
                    intArray.reserve(value.size());
                    for (const auto& v : value) {
                        intArray.push_back(v.get<int64_t>());
                    }
                    set(key, intArray, ConfigSource::File);
                } else if (allDoubles) {
                    std::vector<double> doubleArray;
                    doubleArray.reserve(value.size());
                    for (const auto& v : value) {
                        doubleArray.push_back(v.get<double>());
                    }
                    set(key, doubleArray, ConfigSource::File);
                } else if (allStrings) {
                    std::vector<std::string> stringArray;
                    stringArray.reserve(value.size());
                    for (const auto& v : value) {
                        stringArray.push_back(v.get<std::string>());
                    }
                    set(key, stringArray, ConfigSource::File);
                }
                // If mixed types, ignore the entry
            }
        }
    } catch (...) {
        // If JSON processing fails, return false
        return false;
    }
    
    return true;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first, then fallback to simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    nlohmann::json jsonData;
    try {
        jsonData = nlohmann::json::parse(file);
    } catch (...) {
        // If JSON parsing fails, try simple key=value format
        std::string line;
        while (std::getline(file, line)) {
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
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
    
    // Successfully parsed as JSON
    try {
        // Convert JSON to config entries
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            const auto& value = it.value();
            
            // Convert JSON value to ConfigValue
            if (value.is_string()) {
                set(key, value.get<std::string>(), ConfigSource::File);
            } else if (value.is_boolean()) {
                set(key, value.get<bool>(), ConfigSource::File);
            } else if (value.is_number_integer()) {
                // Try int64_t first, then int
                try {
                    set(key, value.get<int64_t>(), ConfigSource::File);
                } catch (...) {
                    set(key, value.get<int>(), ConfigSource::File);
                }
            } else if (value.is_number_float()) {
                set(key, value.get<double>(), ConfigSource::File);
            } else if (value.is_array()) {
                // Handle array types
                if (value.empty()) {
                    continue;
                }
                
                // Check array element type
                bool allInts = std::all_of(value.begin(), value.end(), 
                    [](const auto& v) { return v.is_number_integer(); });
                bool allDoubles = std::all_of(value.begin(), value.end(), 
                    [](const auto& v) { return v.is_number_float(); });
                bool allStrings = std::all_of(value.begin(), value.end(), 
                    [](const auto& v) { return v.is_string(); });
                
                if (allInts) {
                    std::vector<int64_t> intArray;
                    intArray.reserve(value.size());
                    for (const auto& v : value) {
                        intArray.push_back(v.get<int64_t>());
                    }
                    set(key, intArray, ConfigSource::File);
                } else if (allDoubles) {
                    std::vector<double> doubleArray;
                    doubleArray.reserve(value.size());
                    for (const auto& v : value) {
                        doubleArray.push_back(v.get<double>());
                    }
                    set(key, doubleArray, ConfigSource::File);
                } else if (allStrings) {
                    std::vector<std::string> stringArray;
                    stringArray.reserve(value.size());
                    for (const auto& v : value) {
                        stringArray.push_back(v.get<std::string>());
                    }
                    set(key, stringArray, ConfigSource::File);
                }
                // If mixed types, ignore the entry
            }
        }
    } catch (...) {
        // If JSON processing fails, return false
        return false;
    }
    
    return true;
}

// Configuration validation implementation

ConfigSchema::ConfigSchema() = default;

ConfigSchema::~ConfigSchema() = default;

// ConfigSchema implementation
void ConfigSchema::addRule(const ConfigValidationRule& rule) {
    rules.push_back(rule);
}

ValidationResult ConfigSchema::validate(const Config& config, std::string& errorMessage) const {
    bool hasErrors = false;
    for (const auto& rule : rules) {
        ValidationResult result = ValidationResult::Success;
        
        // Check if required key exists
        if (rule.required && !config.has(rule.key)) {
            errorMessage += "Missing required key: " + rule.key + "\n";
            hasErrors = true;
            continue;
        }
        
        // If key doesn't exist and not required, skip validation
        if (!config.has(rule.key)) {
            continue;
        }
        
        // Validate the value
        std::optional<nlm::ConfigValue> value = config.get<nlm::ConfigValue>(rule.key);
        if (!value) {
            errorMessage += "Cannot get value for key: " + rule.key + "\n";
            hasErrors = true;
            continue;
        }
        
        result = rule.validator(*value);
        
        if (result != ValidationResult::Success) {
            errorMessage += "Validation failed for key '" + rule.key + "': " + rule.errorMessage + "\n";
            hasErrors = true;
        }
    }
    
    return hasErrors ? ValidationResult::InvalidType : ValidationResult::Success;
}

std::vector<std::string> ConfigSchema::getValidationErrors(const Config& config) const {
    std::vector<std::string> errors;
    std::string errorMessage;
    
    if (validate(config, errorMessage) != ValidationResult::Success) {
        // Split error message into lines
        size_t pos = 0;
        size_t newlinePos;
        while ((newlinePos = errorMessage.find('\n', pos)) != std::string::npos) {
            errors.push_back(errorMessage.substr(pos, newlinePos - pos));
            pos = newlinePos + 1;
        }
        if (pos < errorMessage.size()) {
            errors.push_back(errorMessage.substr(pos));
        }
    }
    
    return errors;
}

bool ConfigSchema::isValid(const Config& config) const {
    std::string errorMessage;
    return validate(config, errorMessage) == ValidationResult::Success;
}

std::vector<std::string> ConfigSchema::getRequiredKeys() const {
    std::vector<std::string> requiredKeys;
    for (const auto& rule : rules) {
        if (rule.required) {
            requiredKeys.push_back(rule.key);
        }
    }
    return requiredKeys;
}

size_t ConfigSchema::getErrorCount(const Config& config) const {
    std::vector<std::string> errors = getValidationErrors(config);
    return errors.size();
}

Config::Impl::Impl(std::shared_ptr<Config> cfg)
    : config(cfg)
    , rng(nullptr)
    , developmentalStage(DevelopmentalStage::Initial)
    , nextRegionId(RegionId(1))
    , timestep(0.001)
    , currentStep(0)
    , currentTime(0.0)
    , totalSpikesThisStep(0)
    , totalSpikesTotal(0)
    , isResting(false)
    , stepsSinceLastEpisode(0)
    , replayInterval(100)      // Replay every 100 steps
    , consolidationInterval(1000)  // Consolidate every 1000 steps
{}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Use JSON format for saving configuration
    nlohmann::json jsonData;
    
    // Convert config entries to JSON format
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        const auto& value = entry.value;
        
        // Convert ConfigValue to nlohmann::json
        if (std::holds_alternative<int>(value)) {
            jsonData[key] = std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            jsonData[key] = std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            jsonData[key] = std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            jsonData[key] = std::get<bool>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            jsonData[key] = std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            jsonData[key] = std::get<std::vector<int>>(value);
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            jsonData[key] = std::get<std::vector<double>>(value);
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            jsonData[key] = std::get<std::vector<std::string>>(value);
        }
    }
    
    // Write JSON to file
    try {
        file << jsonData.dump(2);  // Pretty print with 2-space indentation
    } catch (...) {
        return false;
    }
    
    return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
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
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\\\"" << arg << "\\\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\\n";
    }
    return oss.str();
}

bool Config::validate() const {
    std::string errorMessage;
    return schema.validate(*this, errorMessage) == ValidationResult::Success;
}

std::string Config::getValidationErrors() const {
    std::vector<std::string> errors = schema.getValidationErrors(*this);
    if (errors.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < errors.size(); ++i) {
        if (i > 0) oss << "\\n";
        oss << errors[i];
    }
    return oss.str();
}

void Config::setSchema(const ConfigSchema& newSchema) {
    schema = newSchema;
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \\t\\r\\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \\t\\r\\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
