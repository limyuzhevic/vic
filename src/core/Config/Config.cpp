// Configuration presets

// Research configuration for experimental setups
std::shared_ptr<Config> createResearchConfig() {
    auto config = std::make_shared<Config>();
    
    // Enable experimental features
    config->set("experimental.enabled", true);
    config->set("experimental.max_neuron_count", 10000);
    config->set("experimental.plasticity.enabled", true);
    config->set("experimental.neuromodulation.enabled", true);
    
    // Simulation parameters
    config->set("simulation.learning_rate", 0.001);
    config->set("simulation.noise_level", 0.1);
    config->set("simulation.dt", 0.01);
    config->set("simulation.run_time", 1000.0);
    
    // Memory settings
    config->set("memory.working_memory.capacity", 1000);
    config->set("memory.episodic.enabled", true);
    
    // Observation settings
    config->set("sensory.observation_rate", 100);
    config->set("sensory.noise_level", 0.05);
    
    return config;
}

// Production configuration for deployment
std::shared_ptr<Config> createProductionConfig() {
    auto config = std::make_shared<Config>();
    
    // Production safety and stability
    config->set("safety.checkpoints.enabled", true);
    config->set("safety.validation.enabled", true);
    config->set("safety.error_handling.level", "strict");
    config->set("production.performance.optimization", true);
    
    // Core simulation parameters
    config->set("simulation.learning_rate", 0.0001);
    config->set("simulation.noise_level", 0.0);
    config->set("simulation.dt", 0.005);
    config->set("simulation.run_time", 3600.0);
    
    // Memory management
    config->set("memory.working_memory.capacity", 10000);
    config->set("memory.episodic.enabled", false);
    config->set("memory.semantic.enabled", true);
    config->set("memory.procedural.enabled", true);
    
    // Output and monitoring
    config->set("output.logging.level", "info");
    config->set("output.metrics.enabled", true);
    config->set("output.visualization.enabled", false);
    
    return config;
}

// Simulation configuration for different scenarios
std::shared_ptr<Config> createSimulationConfig(const std::string& scenarioName) {
    auto config = std::make_shared<Config>();
    
    if (scenarioName == "simple") {
        config->set("simulation.learning_rate", 0.01);
        config->set("simulation.noise_level", 0.0);
        config->set("brain.neuron_count", 100);
        config->set("plasticity.enabled", false);
    } else if (scenarioName == "complex") {
        config->set("simulation.learning_rate", 0.001);
        config->set("simulation.noise_level", 0.1);
        config->set("brain.neuron_count", 10000);
        config->set("plasticity.enabled", true);
        config->set("neuromodulation.enabled", true);
        config->set("memory.episodic.enabled", true);
    } else if (scenarioName == "minimal") {
        config->set("simulation.learning_rate", 0.1);
        config->set("simulation.noise_level", 0.0);
        config->set("brain.neuron_count", 10);
        config->set("plasticity.enabled", false);
        config->set("neuromodulation.enabled", false);
        config->set("memory.episodic.enabled", false);
    }
    
    // Common settings
    config->set("simulation.dt", 0.01);
    config->set("simulation.run_time", 100.0);
    
    return config;
}

// Performance optimization configuration
std::shared_ptr<Config> createPerformanceConfig() {
    auto config = std::make_shared<Config>();
    
    // High performance settings
    config->set("performance.threading.enabled", true);
    config->set("performance.threading.num_threads", static_cast<int>(std::thread::hardware_concurrency()));
    config->set("performance.memory.pooling", true);
    config->set("performance.compression.enabled", true);
    config->set("performance.parallel.simulation", true);
    
    // Simulation optimization
    config->set("simulation.batch_size", 1000);
    config->set("simulation.async_processing", true);
    config->set("simulation.precompute.enabled", true);
    config->set("simulation.optimization.level", "aggressive");
    
    // Memory optimization
    config->set("memory.pools.enabled", true);
    config->set("memory.compression.threshold", 1024);
    config->set("memory.shared.enabled", true);
    
    // Output optimization
    config->set("output.compression.enabled", true);
    config->set("output.async.write", true);
    
    return config;
}

// Configuration diff and comparison
std::vector<std::string> Config::diff(const Config& other) const {
    std::vector<std::string> differences;
    
    // Check entries in this config not in other
    for (const auto& entry : pImpl->entries) {
        if (!other.has(entry.key)) {
            differences.push_back("  + " + entry.key + " = " + ConfigValidator::getStringValue(entry.value));
        }
    }
    
    // Check entries in other not in this
    for (const auto& entry : other.pImpl->entries) {
        if (!has(entry.key)) {
            differences.push_back("  - " + entry.key + " = " + ConfigValidator::getStringValue(entry.value));
        } else {
            // Check if values differ
            auto value1 = getValue(entry.key);
            auto value2 = other.getValue(entry.key);
            if (value1 != value2) {
                differences.push_back("  ~ " + entry.key + ": " + 
                                     ConfigValidator::getStringValue(value1) + 
                                     " != " + ConfigValidator::getStringValue(value2));
            }
        }
    }
    
    return differences;
}

// Implementation of ConfigBuilder functions

// Python bindings support
namespace pybind11 {
    template<typename T>
    void bindConfig(pybind11::module_& m) {
        m.def("validate_config", [](const std::shared_ptr<Config>& config) {
            return config->validate();
        }, "Validate configuration");
        
        m.def("get_config_metadata", [](const std::shared_ptr<Config>& config, const std::string& key) {
            return config->getParameterInfo(key);
        }, "Get parameter metadata");
        
        m.def("create_config_from_dict", [](const std::map<std::string, std::any>& dict) {
            auto config = std::make_shared<Config>();
            for (const auto& pair : dict) {
                // Type conversion from std::any to ConfigValue
                if (std::holds_alternative<int>(pair.second)) {
                    config->set(pair.first, std::any_cast<int>(pair.second));
                } else if (std::holds_alternative<double>(pair.second)) {
                    config->set(pair.first, std::any_cast<double>(pair.second));
                } else if (std::holds_alternative<bool>(pair.second)) {
                    config->set(pair.first, std::any_cast<bool>(pair.second));
                } else if (std::holds_alternative<std::string>(pair.second)) {
                    config->set(pair.first, std::any_cast<std::string>(pair.second));
                }
            }
            return config;
        }, "Create configuration from dictionary");
        
        m.def("get_config_presets", []() {
            std::map<std::string, std::shared_ptr<Config>> presets;
            presets["research"] = createResearchConfig();
            presets["production"] = createProductionConfig();
            presets["performance"] = createPerformanceConfig();
            return presets;
        }, "Get available configuration presets");
    }
}