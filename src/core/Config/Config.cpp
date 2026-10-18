// Apply configuration preset
    bool Config::applyPreset(ConfigPreset preset) {
        std::cout << "Applying preset: " << preset << std::endl;
        
        switch (preset) {
            case ConfigPreset::RESEARCH:
                return applyResearchPreset(*this);
            case ConfigPreset::PERFORMANCE:
                return applyPerformancePreset(*this);
            case ConfigPreset::EDUCATION:
                return applyEducationPreset(*this);
            case ConfigPreset::BALANCED:
                return applyBalancedPreset(*this);
            case ConfigPreset::CUSTOM:
                std::cout << "Custom preset - using current configuration" << std::endl;
                return true;
            default:
                std::cout << "Unknown preset, using defaults" << std::endl;
                return true;
        }
    }

    // Enhanced preset application methods
    bool Config::applyResearchPreset(Config& config) {
        std::cout << "Applying RESEARCH preset: Detailed logging, validation enabled" << std::endl;
        
        // Set research-specific values
        config.set("log_level", std::string("DEBUG"), ConfigSource::Preset);
        config.set("log_to_file", true, ConfigSource::Preset);
        config.set("log_filename", std::string("nlm_research.log"), ConfigSource::Preset);
        config.set("max_simulation_steps", static_cast<int64_t>(50000), ConfigSource::Preset);
        config.set("enable_validation", true, ConfigSource::Preset);
        config.set("validation_frequency", 100, ConfigSource::Preset);
        
        // Enhance neural parameters for research
        config.set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Preset);
        config.set("stdp_ltp_weight", 0.015f, ConfigSource::Preset);
        config.set("stdp_ltd_weight", 0.012f, ConfigSource::Preset);
        config.set("synaptogenesis_rate", 0.002f, ConfigSource::Preset);
        
        return true;
    }
    
    bool Config::applyPerformancePreset(Config& config) {
        std::cout << "Applying PERFORMANCE preset: Optimized for speed, reduced logging" << std::endl;
        
        // Set performance-specific values
        config.set("log_level", std::string("WARN"), ConfigSource::Preset);
        config.set("log_to_file", false, ConfigSource::Preset);
        config.set("max_simulation_steps", static_cast<int64_t>(1000), ConfigSource::Preset);
        config.set("enable_validation", false, ConfigSource::Preset);
        config.set("parallel_threads", 8, ConfigSource::Preset);
        config.set("enable_simd", true, ConfigSource::Preset);
        
        // Optimize neural parameters for performance
        config.set("neuron_count", static_cast<int64_t>(200), ConfigSource::Preset);
        config.set("stdp_ltp_weight", 0.005f, ConfigSource::Preset);
        config.set("stdp_ltd_weight", 0.006f, ConfigSource::Preset);
        config.set("synaptogenesis_rate", 0.0001f, ConfigSource::Preset);
        
        return true;
    }
    
    bool Config::applyEducationPreset(Config& config) {
        std::cout << "Applying EDUCATION preset: Simplified configuration, verbose output" << std::endl;
        
        // Set education-specific values
        config.set("log_level", std::string("INFO"), ConfigSource::Preset);
        config.set("log_to_file", true, ConfigSource::Preset);
        config.set("log_filename", std::string("nlm_education.log"), ConfigSource::Preset);
        config.set("max_simulation_steps", static_cast<int64_t>(5000), ConfigSource::Preset);
        config.set("enable_validation", true, ConfigSource::Preset);
        config.set("record_neurons", true, ConfigSource::Preset);
        config.set("record_synapses", true, ConfigSource::Preset);
        config.set("record_memories", true, ConfigSource::Preset);
        
        // Simplified neural parameters
        config.set("neuron_count", static_cast<int64_t>(500), ConfigSource::Preset);
        config.set("stdp_ltp_weight", 0.01f, ConfigSource::Preset);
        config.set("stdp_ltd_weight", 0.01f, ConfigSource::Preset);
        
        return true;
    }
    
    bool Config::applyBalancedPreset(Config& config) {
        std::cout << "Applying BALANCED preset: Balanced configuration" << std::endl;
        
        // Set balanced-specific values
        config.set("log_level", std::string("INFO"), ConfigSource::Preset);
        config.set("log_to_file", false, ConfigSource::Preset);
        config.set("max_simulation_steps", static_cast<int64_t>(10000), ConfigSource::Preset);
        config.set("enable_validation", true, ConfigSource::Preset);
        config.set("parallel_threads", 4, ConfigSource::Preset);
        
        // Moderate neural parameters
        config.set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Preset);
        config.set("stdp_ltp_weight", 0.01f, ConfigSource::Preset);
        config.set("stdp_ltd_weight", 0.012f, ConfigSource::Preset);
        config.set("synaptogenesis_rate", 0.001f, ConfigSource::Preset);
        
        return true;
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
