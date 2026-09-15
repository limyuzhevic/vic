// Enhanced Configuration Implementation

EnhancedConfig::EnhancedConfig() : currentMode_("auto") {
    // Set up enhanced defaults for Phase 6
    setDefaults();
}

bool EnhancedConfig::loadWithValidation(const std::string& filepath) {
    bool loaded = Config::loadFromFile(filepath);
    
    if (loaded) {
        // Validate configuration after loading
        if (!validateConfiguration(currentMode_)) {
            NLM_LOG_WARNING("Configuration validation failed");
            // Don't fail completely, just warn
        }
        
        // Apply enhanced defaults for any missing keys
        setDefaults();
        
        // Log configuration report
        NLM_LOG_INFO(getConfigurationReport());
    }
    
    return loaded;
}

void EnhancedConfig::applyToBrain(std::shared_ptr<Brain> brain) const {
    if (!brain) return;
    
    // Apply configuration settings to brain
    if (has("random_seed")) {
        brain->getConfig()->set("random_seed", getOr<int64_t>("random_seed", 42));
    }
    
    if (has("neuron_count")) {
        brain->getConfig()->set("neuron_count", getOr<int64_t>("neuron_count", 500));
    }
    
    if (has("region_count")) {
        brain->getConfig()->set("region_count", getOr<int64_t>("region_count", 1));
    }
    
    if (has("connection_probability")) {
        brain->getConfig()->set("connection_probability", getOr<float>("connection_probability", 0.1f));
    }
    
    NLM_LOG_INFO("Applied enhanced configuration to brain");
}

void EnhancedConfig::applyToAgentBrain(std::shared_ptr<AgentBrain> agentBrain) const {
    if (!agentBrain) return;
    
    // Apply agent-specific configuration
    if (has("curiosity_threshold")) {
        // Note: This would require adding a setter to AgentBrain
        // For now, we'll log the configuration
        float curiosityThreshold = getOr<float>("curiosity_threshold", 0.3f);
        NLM_LOG_INFO("Agent curiosity threshold: " + std::to_string(curiosityThreshold));
    }
    
    if (has("novelty_threshold")) {
        float noveltyThreshold = getOr<float>("novelty_threshold", 0.5f);
        NLM_LOG_INFO("Agent novelty threshold: " + std::to_string(noveltyThreshold));
    }
    
    // Enable/disable features based on configuration
    bool enableRewardModulation = getOr<bool>("enable_reward_modulation", true);
    bool enableStructuralPlasticity = getOr<bool>("enable_structural_plasticity", true);
    bool enableDevelopment = getOr<bool>("enable_development", true);
    bool enableCuriosity = getOr<bool>("enable_curiosity", true);
    
    agentBrain->enableRewardModulation(enableRewardModulation);
    agentBrain->enableStructuralPlasticity(enableStructuralPlasticity);
    agentBrain->enableDevelopment(enableDevelopment);
    agentBrain->enableCuriosity(enableCuriosity);
    
    NLM_LOG_INFO("Applied enhanced configuration to AgentBrain");
}

void EnhancedConfig::applyToWorld(std::shared_ptr<SimpleWorld> world) const {
    if (!world) return;
    
    // Apply world configuration
    if (has("world_width")) {
        int width = getOr<int>("world_width", 64);
        // Note: This would require adding setters to SimpleWorld
        NLM_LOG_INFO("World width: " + std::to_string(width));
    }
    
    if (has("world_height")) {
        int height = getOr<int>("world_height", 64);
        NLM_LOG_INFO("World height: " + std::to_string(height));
    }
    
    NLM_LOG_INFO("Applied enhanced configuration to world");
}

std::string EnhancedConfig::getConfigurationReport() const {
    std::stringstream report;
    report << "=== Enhanced Configuration Report ===" << std::endl;
    report << "Current mode: " << currentMode_ << std::endl;
    report << "Number of configuration entries: " << getKeys().size() << std::endl;
    
    // List all configuration entries
    auto keys = getKeys();
    for (const auto& key : keys) {
        std::optional<ConfigValue> value = get(key);
        if (value.has_value()) {
            report << "  " << key << ": ";
            
            // Convert value to string based on type
            std::visit([&report](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    report << arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    report << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    report << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    report << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::string>) {
                    report << '"' << arg << '"';
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    report << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) report << ", ";
                        report << arg[i];
                    }
                    report << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    report << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) report << ", ";
                        report << arg[i];
                    }
                    report << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    report << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) report << ", ";
                        report << '"' << arg[i] << '"';
                    }
                    report << "]";
                } else {
                    report << "<unknown type>";
                }
            }, value.value());
            report << std::endl;
        }
    }
    
    return report.str();
}

bool EnhancedConfig::validateConfiguration(const std::string& mode) const {
    validationErrors_.clear();
    
    // Validate based on mode
    if (mode == "auto" || mode == "phase6") {
        // Phase 6 specific validations
        if (has("neuron_count") && getOr<int64_t>("neuron_count", 500) <= 0) {
            validationErrors_.push_back("neuron_count must be positive");
        }
        
        if (has("connection_probability") && 
            (getOr<float>("connection_probability", 0.1f) < 0.0f || 
             getOr<float>("connection_probability", 0.1f) > 1.0f)) {
            validationErrors_.push_back("connection_probability must be between 0.0 and 1.0");
        }
        
        if (has("curiosity_threshold") &&
            (getOr<float>("curiosity_threshold", 0.3f) < 0.0f ||
             getOr<float>("curiosity_threshold", 0.3f) > 1.0f)) {
            validationErrors_.push_back("curiosity_threshold must be between 0.0 and 1.0");
        }
    }
    
    // General validations
    if (has("simulation_timestep") && getOr<double>("simulation_timestep", 0.001) <= 0.0) {
        validationErrors_.push_back("simulation_timestep must be positive");
    }
    
    return validationErrors_.empty();
}

void EnhancedConfig::setDefaults() {
    // Set Phase 6 enhanced defaults if not already set
    
    if (!has("random_seed")) {
        set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    }
    
    if (!has("simulation_timestep")) {
        set("simulation_timestep", 0.001, ConfigSource::Default);
    }
    
    if (!has("neuron_count")) {
        set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    }
    
    if (!has("region_count")) {
        set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    }
    
    if (!has("connection_probability")) {
        set("connection_probability", 0.1f, ConfigSource::Default);
    }
    
    // Enhanced Phase 6 defaults
    if (!has("enable_checkpointing")) {
        set("enable_checkpointing", true, ConfigSource::Default);
    }
    
    if (!has("enable_replay")) {
        set("enable_replay", true, ConfigSource::Default);
    }
    
    if (!has("enable_development")) {
        set("enable_development", true, ConfigSource::Default);
    }
    
    if (!has("development_stage")) {
        set("development_stage", "adult", ConfigSource::Default);
    }
    
    if (!has("dopamine_ltp_factor")) {
        set("dopamine_ltp_factor", 0.02f, ConfigSource::Default);
    }
    
    if (!has("dopamine_ltd_factor")) {
        set("dopamine_ltd_factor", 0.015f, ConfigSource::Default);
    }
    
    if (!has("curiosity_exploration_rate")) {
        set("curiosity_exploration_rate", 0.3f, ConfigSource::Default);
    }
    
    if (!has("novelty_detection_sensitivity")) {
        set("novelty_detection_sensitivity", 0.5f, ConfigSource::Default);
    }
    
    if (!has("working_memory_size")) {
        set("working_memory_size", 1000, ConfigSource::Default);
    }
    
    if (!has("episodic_memory_capacity")) {
        set("episodic_memory_capacity", 100, ConfigSource::Default);
    }
    
    if (!has("semantic_memory_capacity")) {
        set("semantic_memory_capacity", 1000, ConfigSource::Default);
    }
}