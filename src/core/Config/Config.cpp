// New helper functions

// Add getCurrentTimestamp function
std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Add validation functions
bool Config::validate() const {
    bool isValid = true;
    
    // Validate essential configuration parameters
    if (has("brain.neuron_count")) {
        auto neurons = get<size_t>("brain.neuron_count");
        if (neurons && *neurons <= 0) {
            NLM_LOG_ERROR("Invalid brain.neuron_count: must be positive");
            isValid = false;
        }
    }
    
    if (has("brain.connection_probability")) {
        auto connProb = get<double>("brain.connection_probability");
        if (connProb && (*connProb < 0.0 || *connProb > 1.0)) {
            NLM_LOG_ERROR("Invalid brain.connection_probability: must be between 0.0 and 1.0");
            isValid = false;
        }
    }
    
    if (has("experiment.max_steps")) {
        auto maxSteps = get<size_t>("experiment.max_steps");
        if (maxSteps && *maxSteps <= 0) {
            NLM_LOG_ERROR("Invalid experiment.max_steps: must be positive");
            isValid = false;
        }
    }
    
    if (has("brain.synapse_density")) {
        auto synapseDensity = get<double>("brain.synapse_density");
        if (synapseDensity && (*synapseDensity < 0.0 || *synapseDensity > 1.0)) {
            NLM_LOG_ERROR("Invalid brain.synapse_density: must be between 0.0 and 1.0");
            isValid = false;
        }
    }
    
    // Validate plasticity parameters
    if (has("plasticity.stdp.learning_rate")) {
        auto stdpLearningRate = get<double>("plasticity.stdp.learning_rate");
        if (stdpLearningRate && (*stdpLearningRate < 0.0 || *stdpLearningRate > 1.0)) {
            NLM_LOG_ERROR("Invalid plasticity.stdp.learning_rate: must be between 0.0 and 1.0");
            isValid = false;
        }
    }
    
    if (has("plasticity.hebbian.learning_rate")) {
        auto hebbianLearningRate = get<double>("plasticity.hebbian.learning_rate");
        if (hebbianLearningRate && (*hebbianLearningRate < 0.0 || *hebbianLearningRate > 1.0)) {
            NLM_LOG_ERROR("Invalid plasticity.hebbian.learning_rate: must be between 0.0 and 1.0");
            isValid = false;
        }
    }
    
    // Validate development stage
    if (has("development.stage")) {
        auto stage = get<std::string>("development.stage");
        if (stage) {
            std::string stageLower = toLower(*stage);
            const std::vector<std::string> validStages = {
                "initial", "criticalperiod", "maturation", "adult", "aging"
            };
            if (std::find(validStages.begin(), validStages.end(), stageLower) == validStages.end()) {
                NLM_LOG_ERROR("Invalid development.stage: must be one of Initial, CriticalPeriod, Maturation, Adult, Aging");
                isValid = false;
            }
        }
    }
    
    if (isValid) {
        NLM_LOG_INFO("Configuration validation passed");
    } else {
        NLM_LOG_WARN("Configuration validation failed. Check log for details.");
    }
    
    return isValid;
}

// Get validation errors
defs getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Validate essential configuration parameters
    if (has("brain.neuron_count")) {
        auto neurons = get<size_t>("brain.neuron_count");
        if (neurons && *neurons <= 0) {
            errors.push_back("brain.neuron_count: must be positive");
        }
    }
    
    if (has("brain.connection_probability")) {
        auto connProb = get<double>("brain.connection_probability");
        if (connProb && (*connProb < 0.0 || *connProb > 1.0)) {
            errors.push_back("brain.connection_probability: must be between 0.0 and 1.0");
        }
    }
    
    if (has("experiment.max_steps")) {
        auto maxSteps = get<size_t>("experiment.max_steps");
        if (maxSteps && *maxSteps <= 0) {
            errors.push_back("experiment.max_steps: must be positive");
        }
    }
    
    if (has("brain.synapse_density")) {
        auto synapseDensity = get<double>("brain.synapse_density");
        if (synapseDensity && (*synapseDensity < 0.0 || *synapseDensity > 1.0)) {
            errors.push_back("brain.synapse_density: must be between 0.0 and 1.0");
        }
    }
    
    // Validate plasticity parameters
    if (has("plasticity.stdp.learning_rate")) {
        auto stdpLearningRate = get<double>("plasticity.stdp.learning_rate");
        if (stdpLearningRate && (*stdpLearningRate < 0.0 || *stdpLearningRate > 1.0)) {
            errors.push_back("plasticity.stdp.learning_rate: must be between 0.0 and 1.0");
        }
    }
    
    if (has("plasticity.hebbian.learning_rate")) {
        auto hebbianLearningRate = get<double>("plasticity.hebbian.learning_rate");
        if (hebbianLearningRate && (*hebbianLearningRate < 0.0 || *hebbianLearningRate > 1.0)) {
            errors.push_back("plasticity.hebbian.learning_rate: must be between 0.0 and 1.0");
        }
    }
    
    // Validate development stage
    if (has("development.stage")) {
        auto stage = get<std::string>("development.stage");
        if (stage) {
            std::string stageLower = toLower(*stage);
            const std::vector<std::string> validStages = {
                "initial", "criticalperiod", "maturation", "adult", "aging"
            };
            if (std::find(validStages.begin(), validStages.end(), stageLower) == validStages.end()) {
                errors.push_back("development.stage: must be one of Initial, CriticalPeriod, Maturation, Adult, Aging");
            }
        }
    }
    
    return errors;
}

// Merge configuration from another config
bool Config::merge(const Config& other) {
    bool hasChanges = false;
    
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            // Add new entry
            pImpl->entries.push_back(entry);
            hasChanges = true;
            NLM_LOG_DEBUG("Merged configuration entry: " + entry.key);
        } else if (it->source == ConfigSource::Default) {
            // Override default with higher priority source
            it->value = entry.value;
            it->source = entry.source;
            hasChanges = true;
            NLM_LOG_DEBUG("Overrode default configuration entry: " + entry.key);
        } else {
            // Don't override existing non-default entries
            NLM_LOG_DEBUG("Skipped configuration entry (already set): " + entry.key);
        }
    }
    
    if (hasChanges) {
        NLM_LOG_INFO("Configuration merge completed with changes");
    } else {
        NLM_LOG_INFO("Configuration merge completed (no changes)");
    }
    
    return hasChanges;
}

// Get configuration by category
defs getByCategory(const std::string& category) const {
    std::vector<ConfigEntry> categoryEntries;
    std::string categoryLower = toLower(category);
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(categoryLower + ".") == 0) {
            categoryEntries.push_back(entry);
        }
    }
    
    return categoryEntries;
}

// Get configuration statistics
std::string Config::getStatistics() const {
    std::ostringstream oss;
    
    oss << "Configuration Statistics:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    
    // Count by source
    std::map<ConfigSource, size_t> sourceCount;
    for (const auto& entry : pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    oss << "  Entries by source:\n";
    for (const auto& pair : sourceCount) {
        oss << "    " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
    }
    
    // Count by data type
    std::map<std::string, size_t> typeCount;
    for (const auto& entry : pImpl->entries) {
        std::string typeName;
        if (std::holds_alternative<int>(entry.value)) {
            typeName = "int";
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            typeName = "int64_t";
        } else if (std::holds_alternative<double>(entry.value)) {
            typeName = "double";
        } else if (std::holds_alternative<bool>(entry.value)) {
            typeName = "bool";
        } else if (std::holds_alternative<std::string>(entry.value)) {
            typeName = "string";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            typeName = "vector<int>";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            typeName = "vector<double>";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            typeName = "vector<string>";
        }
        
        typeCount[typeName]++;
    }
    
    oss << "  Entries by type:\n";
    for (const auto& pair : typeCount) {
        oss << "    " << pair.first << ": " << pair.second << "\n";
    }
    
    return oss.str();
}

// Explicit template instantiations

// Add missing method implementations
void Config::setDefault(const std::string& key, const ConfigValue& value) {
    set(key, value, ConfigSource::Default);
}

void Config::setDefault(const std::string& key, const std::string& value) {
    setDefault(key, ConfigValue(value));
}

void Config::setDefault(const std::string& key, int value) {
    setDefault(key, ConfigValue(value));
}

void Config::setDefault(const std::string& key, double value) {
    setDefault(key, ConfigValue(value));
}

void Config::setDefault(const std::string& key, bool value) {
    setDefault(key, ConfigValue(value));
}

// Enhanced getOr method with default source detection
void Config::setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Basic validation
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set configuration with empty key");
        return;
    }
    
    // Apply validation based on key prefix
    std::string keyLower = toLower(key);
    
    if (keyLower.find("brain.neuron_count") == 0 && std::holds_alternative<size_t>(value)) {
        size_t neurons = std::get<size_t>(value);
        if (neurons <= 0) {
            NLM_LOG_ERROR("brain.neuron_count must be positive");
            return;
        }
    }
    
    if (keyLower.find("brain.connection_probability") == 0 && std::holds_alternative<double>(value)) {
        double prob = std::get<double>(value);
        if (prob < 0.0 || prob > 1.0) {
            NLM_LOG_ERROR("brain.connection_probability must be between 0.0 and 1.0");
            return;
        }
    }
    
    // If validation passes, set the value
    set(key, value, source);
}

// Get configuration by section
std::vector<ConfigEntry> Config::getSection(const std::string& section) const {
    std::vector<ConfigEntry> sectionEntries;
    std::string sectionLower = toLower(section);
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(sectionLower + ".") == 0) {
            sectionEntries.push_back(entry);
        }
    }
    
    return sectionEntries;
}

// Print configuration with formatting
void Config::printConfig() const {
    NLM_LOG_INFO("=== NLM Configuration ===");
    NLM_LOG_INFO("Total entries: " + std::to_string(pImpl->entries.size()));
    
    // Group by source
    std::map<ConfigSource, std::vector<ConfigEntry>> entriesBySource;
    for (const auto& entry : pImpl->entries) {
        entriesBySource[entry.source].push_back(entry);
    }
    
    // Print each source group
    for (const auto& pair : entriesBySource) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Source " + std::to_string(static_cast<int>(pair.first)) + ":");
        
        for (const auto& entry : pair.second) {
            NLM_LOG_INFO("  " + entry.key + " = " + getConfigValueString(entry.value));
        }
    }
}

// Get configuration value as string
std::string Config::getConfigValueString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << '"' << arg << '"';
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}
