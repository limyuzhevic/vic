// python/bindings_config.cpp

void bindConfig(py::module_& m) {
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file)
             
             Args:
                 filepath: Path to the JSON configuration file
                 
             Returns:
                 bool: True if configuration was loaded successfully, false otherwise
                 
             Example:
                 >>> config = pynlm.Config()
                 >>> config.loadFromFile("config.json")
                 >>> print(config.has("brain.neuron_count"))
                 True
            )")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(Load configuration from command line arguments)
           
           Args:
               argc: Number of command line arguments
               argv: Command line argument array
               
           Example:
               >>> config = pynlm.Config()
               >>> config.loadFromArgs(len(sys.argv), sys.argv)
           )")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file)
             
             Args:
                 filepath: Path where the JSON configuration will be saved
                 
             Returns:
                 bool: True if configuration was saved successfully, false otherwise
                 
             Example:
                 >>> config = pynlm.Config()
                 >>> config.set("brain.neuron_count", 1000)
                 >>> config.saveToFile("my_config.json")
            )")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists)
             
             Args:
                 key: Configuration key to check (e.g., "brain.neuron_count")
                 
             Returns:
                 bool: True if the key exists, False otherwise
                 
             Example:
                 >>> config = pynlm.Config()
                 >>> config.has("brain.neuron_count")
                 True
            )")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys)
             
             Returns:
                 list: List of all configuration keys
                 
             Example:
                 >>> config = pynlm.Config()
                 >>> config.set("brain.neuron_count", 1000)
                 >>> keys = config.getKeys()
                 >>> print("neuron_count" in keys)
                 True
            )")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries)
             
             Example:
                 >>> config = pynlm.Config()
                 >>> config.set("test", 42)
                 >>> config.clear()
                 >>> config.has("test")
                 False
            )")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration)
             
             Returns:
                 str: Summary of configuration key-value pairs
                 
             Example:
                 >>> config = pynlm.Config()
                 >>> config.set("brain.neuron_count", 500)
                 >>> summary = config.summary()
                 >>> print(summary)
                 Config with 1 keys
            )")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Validation and convenience methods for Config
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration)
    
    Returns:
        Config: A new configuration with default values
        
    Example:
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
    )")
    .def("validateConfiguration", [](const Config& cfg) -> bool {
        // Basic validation - check if essential brain keys exist
        bool hasNeuronCount = cfg.has("brain.neuron_count");
        bool hasSynapseDensity = cfg.has("brain.synapse_density");
        return hasNeuronCount && hasSynapseDensity;
    }, py::arg("config"), R"pbdoc(Validate configuration for brain creation)
    
    Args:
        config: Configuration to validate
        
    Returns:
        bool: True if configuration is valid for brain creation
        
    Example:
        >>> config = pynlm.createDefaultConfig()
        >>> config.set("brain.neuron_count", 1000)
        >>> if pynlm.validateConfiguration(config):
        ...     print("Configuration is valid")
    )")
    .def("getRecommendedConfig", [](const std::string& useCase) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        if (useCase == "simple") {
            config->set("brain.neuron_count", 100, nlm::ConfigSource::Default);
            config->set("brain.synapse_density", 0.1, nlm::ConfigSource::Default);
        } else if (useCase == "complex") {
            config->set("brain.neuron_count", 1000, nlm::ConfigSource::Default);
            config->set("brain.synapse_density", 0.2, nlm::ConfigSource::Default);
            config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
        }
        
        return config;
    }, py::arg("use_case"), R"pbdoc(Get recommended configuration for a use case)
    
    Args:
        use_case: Use case type ("simple" or "complex")
        
    Returns:
        Config: Recommended configuration for the specified use case
        
    Example:
        >>> config = pynlm.getRecommendedConfig("simple")
        >>> brain = pynlm.createBrain(config)
    )")
    .def("mergeConfigurations", [](const std::shared_ptr<Config>& base, 
                                  const std::shared_ptr<Config>& override) -> std::shared_ptr<Config> {
        auto merged = std::make_shared<Config>(*base);
        
        // Simple merge - override configs take precedence
        auto keys = override->getKeys();
        for (const auto& key : keys) {
            auto value = override->get<ConfigValue>(key);
            if (value.has_value()) {
                merged->set(key, value.value(), nlm::ConfigSource::Runtime);
            }
        }
        
        return merged;
    }, py::arg("base_config"), py::arg("override_config"), R"pbdoc(Merge two configurations)
    
    Args:
        base_config: Base configuration
        override_config: Override configuration (takes precedence)
        
    Returns:
        Config: Merged configuration
        
    Example:
        >>> base = pynlm.createDefaultConfig()
        >>> base.set("brain.neuron_count", 1000)
        >>> override = pynlm.createDefaultConfig()
        >>> override.set("brain.neuron_count", 500)
        >>> merged = pynlm.mergeConfigurations(base, override)
        >>> merged.getOr("brain.neuron_count", 0)
        500
    )");
}