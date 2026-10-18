// Add missing helper functions

// Add isNumber function if not already present
bool Config::isNumber(const std::string& str) {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') {
        start = 1;
    }
    
    bool hasDecimal = false;
    for (size_t i = start; i < str.length(); ++i) {
        if (str[i] == '.') {
            if (hasDecimal) return false;
            hasDecimal = true;
        } else if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

// Add validation and utility functions

// Enhanced validation for Config value types
bool Config::validateKeyFormat(const std::string& key) {
    if (key.empty()) return false;
    if (key.find_first_of("\n\r\t") != std::string::npos) return false;
    
    // Check for valid characters (alphanumeric, dots, underscores)
    for (char c : key) {
        if (!std::isalnum(c) && c != '.' && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

// Type-safe value getter with enhanced error handling
template<typename T>
T Config::getWithFallback(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    }
    return defaultValue;
}

// Enhanced set method with validation
void Config::setValidated(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (validateKeyFormat(key) && validateValue(key, value)) {
        set(key, value, source);
    } else {
        // Log validation error or handle as appropriate
        // For now, we'll silently accept invalid values with warnings
    }
}

// Configuration factory methods
Config Config::createDefaultConfig() {
    Config config;
    
    // Set reasonable defaults for neural simulation
    config.set("simulation.timestep", 0.01, ConfigSource::Default);
    config.set("simulation.duration", 10000, ConfigSource::Default);
    config.set("neuron.threshold", -55.0, ConfigSource::Default);
    config.set("neuron.restingpotential", -70.0, ConfigSource::Default);
    config.set("neuron.refractoryperiod", 10, ConfigSource::Default);
    config.set("synapse.weight", 1.0, ConfigSource::Default);
    config.set("synapse.delay", 1, ConfigSource::Default);
    config.set("plasticity.hebbianrate", 0.01, ConfigSource::Default);
    config.set("plasticity.stdprate", 0.01, ConfigSource::Default);
    
    return config;
}

Config Config::createMinimalConfig() {
    Config config;
    
    // Minimal viable configuration
    config.set("simulation.timestep", 0.01, ConfigSource::Default);
    config.set("neuron.threshold", -55.0, ConfigSource::Default);
    config.set("neuron.restingpotential", -70.0, ConfigSource::Default);
    
    return config;
}

// Utility for batch operations
void Config::setBatch(const std::vector<std::pair<std::string, ConfigValue>>& values, ConfigSource source) {
    for (const auto& pair : values) {
        set(pair.first, pair.second, source);
    }
}

// Configuration merging utilities
void Config::applyOverrides(const Config& overrides) {
    for (const auto& entry : overrides.pImpl->entries) {
        set(entry.key, entry.value, entry.source);
    }
}

// Get configuration statistics
ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    
    for (const auto& entry : pImpl->entries) {
        stats.entriesBySource[static_cast<int>(entry.source)]++;
        
        // Count by type
        std::visit([&stats](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                stats.stringValues++;
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, int64_t>) {
                stats.numericValues++;
            } else if constexpr (std::is_same_v<T, bool>) {
                stats.boolValues++;
            } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                               std::is_same_v<T, std::vector<double>> ||
                               std::is_same_v<T, std::vector<std::string>>) {
                stats.arrayValues++;
            }
        }, entry.value);
    }
    
    return stats;
}

// Configuration preset management
const std::unordered_map<std::string, Config> Config::PRESETS = {
    {"default", createDefaultConfig()},
    {"minimal", createMinimalConfig()},
    {"fast", createFastSimulationConfig()},
    {"accurate", createAccurateSimulationConfig()}
};

Config Config::createFastSimulationConfig() {
    Config config = createDefaultConfig();
    
    // Fast simulation settings
    config.set("neuron.refractoryperiod", 2, ConfigSource::Default);
    config.set("synapse.delay", 0, ConfigSource::Default);
    config.set("plasticity.hebbianrate", 0.05, ConfigSource::Default);
    config.set("simulation.timestep", 0.005, ConfigSource::Default);
    
    return config;
}

Config Config::createAccurateSimulationConfig() {
    Config config = createDefaultConfig();
    
    // Accurate simulation settings
    config.set("neuron.refractoryperiod", 20, ConfigSource::Default);
    config.set("synapse.delay", 5, ConfigSource::Default);
    config.set("plasticity.hebbianrate", 0.001, ConfigSource::Default);
    config.set("plasticity.stdprate", 0.002, ConfigSource::Default);
    config.set("simulation.timestep", 0.02, ConfigSource::Default);
    
    return config;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // IMPROVEMENT: Support JSON/YAML format with validation
    // IMPROVEMENT: Add proper error handling and reporting
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Enhanced file format detection
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    if (content.empty()) {
        return false;
    }
    
    // Detect file format
    bool isJson = false, isYaml = false;
    if (content.find('{') != std::string::npos || content.find('[') != std::string::npos) {
        isJson = true;
    }
    if (content.find(": ") != std::string::npos || 
        content.find(":\t") != std::string::npos ||
        content.find("\n\s+- \w+") != std::string::npos) {
        isYaml = true;
    }
    
    if (isJson) {
        return loadFromJson(content);
    } else if (isYaml) {
        return loadFromYaml(content);
    } else {
        // Fallback to simple key=value format
        return loadFromSimpleFormat(content);
    }
}

bool Config::loadFromJson(const std::string& jsonContent) {
    bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save in JSON format for better interoperability
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << arg[j];
                    if (j < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << arg[j];
                    if (j < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << "\"" << arg[j] << "\"";
                    if (j < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << "null";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    file << "}\n";
    
    return true;
}

bool Config::loadFromYaml(const std::string& yamlContent) {
    // TODO: Implement full YAML parsing
    // IMPROVEMENT: Use a proper YAML library or implement parser
    
    // Enhanced parsing for YAML-like content
    try {
        std::istringstream stream(yamlContent);
        std::string line;
        std::string currentSection;
        
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Check for section headers
            if (line.find(':') != std::string::npos && 
                line.find(':') < line.length() - 1 &&
                !isspace(line[line.find(':') + 1])) {
                // New section
                size_t colon = line.find(':');
                currentSection = trim(line.substr(0, colon));
                continue;
            }
            
            // Parse key-value pairs
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string key = trim(line.substr(0, colon));
                std::string value = trim(line.substr(colon + 1));
                
                // Validate key with section if present
                std::string fullKey = currentSection.empty() ? key : currentSection + "." + key;
                
                // Try to parse value based on type
                if (value.empty()) {
                    set(fullKey, std::string(""), ConfigSource::File);
                } else if (value == "true" || value == "false") {
                    set(fullKey, value == "true", ConfigSource::File);
                } else if (isNumber(value)) {
                    if (value.find('.') != std::string::npos) {
                        set(fullKey, std::stod(value), ConfigSource::File);
                    } else {
                        set(fullKey, std::stoll(value), ConfigSource::File);
                    }
                } else {
                    // Remove quotes if present
                    if ((value.front() == '"' && value.back() == '"') ||
                        (value.front() == '\'' && value.back() == '\'')) {
                        value = value.substr(1, value.size() - 2);
                    }
                    set(fullKey, value, ConfigSource::File);
                }
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& content) {
    // Original simple format parsing with improvements
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with enhanced format support
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Handle different value formats
            if (value.empty()) {
                set(key, std::string(""), ConfigSource::File);
            } else if (value == "true" || value == "false") {
                set(key, value == "true", ConfigSource::File);
            } else if (isNumber(value)) {
                try {
                    size_t decimalPos = value.find('.');
                    if (decimalPos != std::string::npos) {
                        set(key, std::stod(value), ConfigSource::File);
                    } else {
                        set(key, std::stoll(value), ConfigSource::File);
                    }
                } catch (...) {
                    // Fallback to string
                    set(key, value, ConfigSource::File);
                }
            } else {
                // Remove quotes if present
                if ((value.front() == '"' && value.back() == '"') ||
                    (value.front() == '\'' && value.back() == '\'')) {
                    value = value.substr(1, value.size() - 2);
                }
                set(key, value, ConfigSource::File);
            }
        } else if (line.find(':') != std::string::npos && 
                   line.find(':') < line.length() - 1) {
            // Handle section.key format
            std::string key = line.substr(0, line.find(':'));
            std::string value = trim(line.substr(line.find(':') + 1));
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::isNumber(const std::string& str) {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') {
        start = 1;
    }
    
    bool hasDecimal = false;
    for (size_t i = start; i < str.length(); ++i) {
        if (str[i] == '.') {
            if (hasDecimal) return false;
            hasDecimal = true;
        } else if (!std::isdigit(str[i])) {
            return false;
        }
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
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
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

// Validation methods
    bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
        // IMPROVEMENT: Add validation for key format and value types
        // Validate key format
        if (key.empty() || key.find_first_of("\n\r\t") != std::string::npos) {
            return false;
        }
        
        // Validate value type based on key prefix
        std::string lowerKey = toLower(key);
        
        // Check for section-specific validation
        if (lowerKey.find("neuron.") == 0) {
            // Neuron-related parameters
            return validateNeuronConfig(key, value);
        } else if (lowerKey.find("synapse.") == 0) {
            // Synapse-related parameters
            return validateSynapseConfig(key, value);
        } else if (lowerKey.find("simulation.") == 0) {
            // Simulation parameters
            return validateSimulationConfig(key, value);
        }
        
        return true; // Default validation passes
    }
    
    bool Config::validateNeuronConfig(const std::string& key, const ConfigValue& value) const {
        // Validate neuron-specific parameters
        std::string lowerKey = toLower(key);
        
        if (lowerKey == "neuron.threshold") {
            double threshold = std::get<double>(value);
            return threshold > -100.0 && threshold < 100.0; // Realistic range
        } else if (lowerKey == "neuron.restingpotential") {
            double resting = std::get<double>(value);
            return resting > -100.0 && resting < 0.0; // Typical resting potential
        } else if (lowerKey == "neuron.refractoryperiod") {
            int64_t period = std::get<int64_t>(value);
            return period > 0 && period < 10000; // Reasonable range
        }
        
        return true;
    }
    
    bool Config::validateSynapseConfig(const std::string& key, const ConfigValue& value) const {
        // Validate synapse-specific parameters
        std::string lowerKey = toLower(key);
        
        if (lowerKey == "synapse.weight") {
            double weight = std::get<double>(value);
            return weight >= -10.0 && weight <= 10.0; // Reasonable weight range
        } else if (lowerKey == "synapse.delay") {
            int64_t delay = std::get<int64_t>(value);
            return delay >= 0 && delay < 1000; // Reasonable delay range
        }
        
        return true;
    }
    
    bool Config::validateSimulationConfig(const std::string& key, const ConfigValue& value) const {
        // Validate simulation-specific parameters
        std::string lowerKey = toLower(key);
        
        if (lowerKey == "simulation.timestep") {
            double dt = std::get<double>(value);
            return dt > 0.0 && dt < 0.1; // Reasonable timestep
        } else if (lowerKey == "simulation.duration") {
            int64_t duration = std::get<int64_t>(value);
            return duration > 0 && duration < 1000000; // Reasonable duration
        }
        
        return true;
    }
    
    std::vector<ConfigEntry> Config::getFilteredEntries(ConfigSource source) const {
        std::vector<ConfigEntry> filtered;
        for (const auto& entry : pImpl->entries) {
            if (entry.source == source) {
                filtered.push_back(entry);
            }
        }
        return filtered;
    }
    
    bool Config::mergeConfig(const Config& otherConfig) {
        // IMPROVEMENT: Add config merging with precedence
        bool changed = false;
        
        for (const auto& entry : otherConfig.pImpl->entries) {
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                 [&entry](const ConfigEntry& e) { return e.key == entry.key; });
            
            if (it == pImpl->entries.end()) {
                // Add new entry
                pImpl->entries.push_back(entry);
                changed = true;
            } else if (entry.source == ConfigSource::CommandLine) {
                // Command line overrides file/command line
                it->value = entry.value;
                it->source = entry.source;
                changed = true;
            } else if (entry.source == ConfigSource::File && it->source == ConfigSource::Default) {
                // File overrides default
                it->value = entry.value;
                it->source = entry.source;
                changed = true;
            }
        }
        
        return changed;
    }
    
    std::string Config::getConfigSummary(ConfigSource source) const {
        std::ostringstream oss;
        oss << "Configuration from " << static_cast<int>(source) << " (";
        
        int count = 0;
        for (const auto& entry : pImpl->entries) {
            if (entry.source == source) {
                count++;
            }
        }
        
        oss << count << " entries):\n";
        
        for (const auto& entry : pImpl->entries) {
            if (entry.source == source) {
                oss << "  " << entry.key << " = [";
                std::visit([&oss](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        oss << "\"" << arg << "\"";
                    } else {
                        oss << arg;
                    }
                }, entry.value);
                oss << "]\n";
            }
        }
        
        return oss.str();
    }
