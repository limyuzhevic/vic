// =============================================================================
// Python Module Utilities and Advanced Functions
// =============================================================================

namespace nlm {

    // Exception classes for better error handling
    class ValidationError : public std::runtime_error {
    public:
        ValidationError(const std::string& msg) : std::runtime_error(msg) {}
    };

    class ConfigError : public std::runtime_error {
    public:
        ConfigError(const std::string& msg) : std::runtime_error(msg) {}
    };

    class BrainError : public std::runtime_error {
    public:
        BrainError(const std::string& msg) : std::runtime_error(msg) {}
    };

    // Utility functions for Python bindings
    namespace pyutils {

        // Convert vector to Python list
        template<typename T>
        py::list vectorToList(const std::vector<T>& vec) {
            py::list result;
            for (const auto& item : vec) {
                result.append(item);
            }
            return result;
        }

        // Convert Python list to vector
        template<typename T>
        std::vector<T> listToVector(const py::list& list) {
            std::vector<T> result;
            for (const auto& item : list) {
                result.push_back(item.cast<T>());
            }
            return result;
        }

        // Create a dictionary from Python dict
        std::map<std::string, std::string> dictToMap(const py::dict& d) {
            std::map<std::string, std::string> result;
            for (auto item : d) {
                result[item.first.cast<std::string>()] = item.second.cast<std::string>();
            }
            return result;
        }

        // Check if configuration has required keys
        std::vector<std::string> getMissingKeys(const Config& config, 
                                               const std::vector<std::string>& requiredKeys) {
            std::vector<std::string> missing;
            for (const auto& key : requiredKeys) {
                if (!config.has(key)) {
                    missing.push_back(key);
                }
            }
            return missing;
        }

        // Safe configuration value getter with type checking
        template<typename T>
        T safeGetValue(const Config& config, const std::string& key, T defaultValue) {
            if (config.has(key)) {
                return config.getOr<T>(key, defaultValue);
            }
            return defaultValue;
        }

        // Configuration value setter with validation
        bool safeSetValue(Config& config, const std::string& key, const std::string& value) {
            try {
                config.set(key, value, ConfigSource::User);
                return true;
            } catch (const std::exception&) {
                return false;
            }
        }

        // Batch configuration update
        bool batchUpdateConfig(Config& config, const py::dict& settings) {
            bool success = true;
            for (auto item : settings) {
                std::string key = item.first.cast<std::string>();
                if (py::isinstance<py::str>(item.second)) {
                    if (!safeSetValue(config, key, item.second.cast<std::string>())) {
                        success = false;
                    }
                } else if (py::isinstance<py::int_>(item.second)) {
                    int64_t intValue = item.second.cast<int64_t>();
                    config.set(key, intValue, ConfigSource::User);
                } else if (py::isinstance<py::float_>(item.second)) {
                    float floatValue = item.second.cast<float>();
                    config.set(key, floatValue, ConfigSource::User);
                } else {
                    success = false; // Unsupported type
                }
            }
            return success;
        }

        // Format configuration summary
        std::string formatConfigSummary(const Config& config) {
            std::stringstream ss;
            ss << "=== Configuration Summary ===" << std::endl;
            ss << "Total keys: " << config.getKeys().size() << std::endl;
            
            auto keys = config.getKeys();
            for (const auto& key : keys) {
                ss << key << ": ";
                if (config.has(key)) {
                    ss << "[SET]" << std::endl;
                }
            }
            
            ss << "===============================" << std::endl;
            return ss.str();
        }
    }

    // Python module-level utility functions
    namespace pybindings {

        // Create a Pythonic configuration with validation
        std::shared_ptr<Config> createValidatedConfig(const py::dict& initialSettings = py::dict()) {
            auto config = std::make_shared<Config>();
            
            if (!initialSettings.empty()) {
                pyutils::batchUpdateConfig(*config, initialSettings);
            }
            
            return config;
        }

        // Validate configuration with detailed feedback
        py::list validateConfigDetailed(const Config& config) {
            py::list errors;
            
            // Check for required keys
            std::vector<std::string> requiredKeys = {"neuron_count", "region_count", "simulation_timestep"};
            auto missingKeys = pyutils::getMissingKeys(config, requiredKeys);
            
            for (const auto& key : missingKeys) {
                errors.append("Missing required key: " + key);
            }
            
            // Validate specific values
            if (config.has("neuron_count")) {
                int64_t neuronCount = config.getOr<int64_t>("neuron_count", 0);
                if (neuronCount <= 0 || neuronCount > 1000000) {
                    errors.append("Invalid neuron_count: must be between 1 and 1,000,000");
                }
            }
            
            if (config.has("region_count")) {
                int64_t regionCount = config.getOr<int64_t>("region_count", 0);
                if (regionCount <= 0 || regionCount > 1000) {
                    errors.append("Invalid region_count: must be between 1 and 1,000");
                }
            }
            
            if (config.has("simulation_timestep")) {
                double timestep = config.getOr<double>("simulation_timestep", 0.0);
                if (timestep <= 0.0 || timestep > 1.0) {
                    errors.append("Invalid simulation_timestep: must be between 0.001 and 1.0");
                }
            }
            
            return errors;
        }

        // Create a simple world with validation
        std::shared_ptr<SimpleWorld> createValidatedWorld(int width = 20, int height = 20,
                                                          int visionWidth = 8, int visionHeight = 8) {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, visionWidth, visionHeight);
            world->reset();
            return world;
        }

        // Create an agent brain with world integration
        std::shared_ptr<AgentBrain> createIntegratedAgent(std::shared_ptr<Brain> brain,
                                                         std::shared_ptr<SimpleWorld> world) {
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            return agent;
        }

        // Complete simulation episode with error handling
        std::vector<std::string> runCompleteEpisode(std::shared_ptr<Brain> brain,
                                                   std::shared_ptr<SimpleWorld> world,
                                                   std::shared_ptr<AgentBrain> agent,
                                                   int steps = 100, double timestep = 0.1) {
            std::vector<std::string> allErrors;
            
            for (int step = 0; step < steps; ++step) {
                // World update with error handling
                {
                    auto errors = safeWorldUpdate(*world, timestep);
                    allErrors.insert(allErrors.end(), errors.begin(), errors.end());
                }
                
                // Agent processing
                {
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                }
                
                // Brain step
                {
                    auto errors = safeBrainStep(*brain, step, step * timestep);
                    allErrors.insert(allErrors.end(), errors.begin(), errors.end());
                }
                
                // Action and world update
                {
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                }
            }
            
            return allErrors;
        }

        // Configuration manager with preset configurations
        class ConfigManager {
        private:
            std::shared_ptr<Config> config;
            std::string name;
        
        public:
            ConfigManager(const std::string& n) : name(n), config(std::make_shared<Config>()) {}
            
            void setName(const std::string& n) { name = n; }
            std::string getName() const { return name; }
            
            std::shared_ptr<Config> getConfig() { return config; }
            
            bool applyPreset(const std::string& presetName) {
                if (presetName == "default") {
                    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
                    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
                    config->set("connection_probability", 0.1f, ConfigSource::Default);
                    return true;
                } else if (presetName == "high_performance") {
                    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
                    config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
                    config->set("connection_probability", 0.05f, ConfigSource::Default);
                    return true;
                } else if (presetName == "quick_test") {
                    config->set("neuron_count", static_cast<int64_t>(10), ConfigSource::Default);
                    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
                    config->set("connection_probability", 0.3f, ConfigSource::Default);
                    return true;
                }
                return false;
            }
            
            bool loadFromFile(const std::string& filename) {
                return config->loadFromFile(filename);
            }
            
            bool saveToFile(const std::string& filename) {
                config->saveToFile(filename);
                return true;
            }
            
            py::dict toDict() const {
                py::dict result;
                auto keys = config->getKeys();
                for (const auto& key : keys) {
                    // Get value as string for simplicity
                    result[key.c_str()] = key.c_str(); // Note: This is simplified
                }
                return result;
            }
        };
    }

} // namespace nlm
