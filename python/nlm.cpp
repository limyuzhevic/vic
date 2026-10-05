// NLM Python Bindings - Main Module
// Simplified and improved Python API for NLM neural simulation framework

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        A comprehensive Python API for the NLM neural simulation framework.
        
        This API provides access to spiking neural networks with brain-inspired
        learning, development, and cognitive capabilities. The bindings are
        designed to be both beginner-friendly and powerful for advanced users.
        
        Key Features:
        - Brain simulation with event-driven spike dynamics
        - Agent-environment interaction with complete control loop
        - Integrated memory, prediction, and cognition systems
        - Pythonic interfaces with context managers and properties
        - Advanced simulation utilities and performance monitoring
        - Comprehensive error handling and validation
        
        Quick Start:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> for step in range(100):
            ...     brain.step(step)
        
        Advanced Usage:
            >>> with pynlm.run_training_episode(brain, world, agent, 500) as results:
            ...     print(f"Training complete with {len(results)} steps")
    )pbdoc";

    // Error types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    py::register_exception<std::logic_error>(m, "LogicError");

    // Configuration class with enhanced Pythonic interface
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        The Config class manages simulation parameters and settings.
        It provides methods for loading from files, command line, and
        programmatic setup. Configuration can be accessed and modified
        using key-value pairs.
        
        Configuration keys follow dot notation for nested parameters:
        - brain.neuron_count: Number of neurons
        - brain.synapse_density: Connection density  
        - plasticity.stdp.learning_rate: STDP learning rate
        - neuromodulation.dopamine.scale: Dopamine scaling factor
        
        Examples:
            >>> config = Config()
            >>> config.loadFromFile("config.json")
            >>> config.set("brain.neuron_count", 1000)
            >>> config["plasticity.rate"] = 0.01
            >>> if "brain.neuron_count" in config:
            ...     print(config["brain.neuron_count"])
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"), "Check if configuration key exists")
        .def("__contains__", &Config::has,
             "Check if key exists using 'in' operator")
        .def("__getitem__", [](Config& self, const std::string& key) {
            if (self.has(key)) {
                return py::cast(self.has(key));
            } else {
                PyErr_SetString(PyExc_KeyError, ("Key not found: " + key).c_str());
                throw py::error_already_set();
            }
        }, py::arg("key"), "Get configuration value using indexing")
        .def("__setitem__", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set integer configuration")
        .def("__setitem__", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set float configuration")
        .def("__setitem__", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration")
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set string configuration")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("getOr", py::overload_cast<const std::string&, int64_t>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value or return default")
        .def("getOr", py::overload_cast<const std::string&, double>(&Config::getOr),
             py::arg("key"), py::arg("default_value"))
        .def("getOr", py::overload_cast<const std::string&, bool>(&Config::getOr),
             py::arg("key"), py::arg("default_value"))
        .def("getOr", py::overload_cast<const std::string&, std::string>(&Config::getOr),
             py::arg("key"), py::arg("default_value"))
        .def("set", py::overload_cast<const std::string&, int64_t>(&Config::set),
             py::arg("key"), py::arg("value"), "Set integer value")
        .def("set", py::overload_cast<const std::string&, double>(&Config::set),
             py::arg("key"), py::arg("value"), "Set float value")
        .def("set", py::overload_cast<const std::string&, bool>(&Config::set),
             py::arg("key"), py::arg("value"), "Set boolean value")
        .def("set", py::overload_cast<const std::string&, std::string>(&Config::set),
             py::arg("key"), py::arg("value"), "Set string value")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get configuration summary")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        .def("__len__", [](const Config& self) {
            return self.getKeys().size();
        }, "Get number of configuration entries");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration object");

    // Simulation utilities
    m.def("run_simulation", 
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
           std::shared_ptr<AgentBrain> agent, int steps, double timestep = 0.1) {
            std::vector<std::map<std::string, py::object>> results;
            results.reserve(steps);
            
            for (int step = 0; step < steps; ++step) {
                world->update(timestep);
                
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                brain->step(step, timestep);
                
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                auto internal = percept.getInternal();
                float reward = internal.empty() ? 0.0f : internal[0];
                agent->applyRewardModulation(reward, 0.0f);
                agent->updateDevelopment(timestep);
                
                std::map<std::string, py::object> step_result;
                step_result["step"] = step;
                step_result["time"] = world->getSimulationTime();
                step_result["spike_count"] = brain->getTotalSpikeCount();
                step_result["firing_neurons"] = brain->getFiringNeuronCount();
                step_result["average_firing_rate"] = brain->getAverageFiringRate();
                step_result["action_type"] = static_cast<int>(action.getType());
                step_result["curiosity_level"] = agent->getCuriosityLevel();
                step_result["novelty_level"] = agent->getNoveltyLevel();
                step_result["prediction_error"] = agent->getPredictionError();
                step_result["neuromodulation_level"] = agent->getNeuromodulationLevel();
                
                results.push_back(step_result);
            }
            
            return results;
        }, py::arg("brain"), py::arg("world"), py::arg("agent"),
            py::arg("steps"), py::arg("timestep") = 0.1,
            R"pbdoc(Run a complete simulation loop.
            
            Args:
                brain: Brain object for neural computation
                world: World object for environment simulation  
                agent: AgentBrain object connecting brain to world
                steps: Number of simulation steps to run
                timestep: Time step duration in seconds
                
            Returns:
                List of dictionaries, one for each step, containing simulation metrics
            )pbdoc");

    m.def("run_training_episode",
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
           std::shared_ptr<AgentBrain> agent, int steps, double timestep = 0.1,
           bool verbose = false) {
            std::vector<std::map<std::string, py::object>> results;
            results.reserve(steps);
            
            // Enable all subsystems for training
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
            
            if (verbose) {
                std::cout << "Starting training episode..." << std::endl;
            }
            
            for (int step = 0; step < steps; ++step) {
                world->update(timestep);
                
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                brain->step(step, timestep);
                
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                auto internal = percept.getInternal();
                float reward = internal.empty() ? 0.0f : internal[0];
                agent->applyRewardModulation(reward, 0.0f);
                agent->updateDevelopment(timestep);
                
                std::map<std::string, py::object> step_result;
                step_result["step"] = step;
                step_result["time"] = world->getSimulationTime();
                step_result["spike_count"] = brain->getTotalSpikeCount();
                step_result["firing_neurons"] = brain->getFiringNeuronCount();
                step_result["average_firing_rate"] = brain->getAverageFiringRate();
                step_result["action_type"] = static_cast<int>(action.getType());
                step_result["curiosity_level"] = agent->getCuriosityLevel();
                step_result["novelty_level"] = agent->getNoveltyLevel();
                step_result["prediction_error"] = agent->getPredictionError();
                step_result["neuromodulation_level"] = agent->getNeuromodulationLevel();
                step_result["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
                
                results.push_back(step_result);
            }
            
            return results;
        }, py::arg("brain"), py::arg("world"), py::arg("agent"),
            py::arg("steps"), py::arg("timestep") = 0.1, py::arg("verbose") = false,
        R"pbdoc(Run a complete training episode.
        
        Enables all major learning subsystems for training.
        
        Args:
            brain: Brain object for neural computation
            world: World object for environment simulation  
            agent: AgentBrain object connecting brain to world
            steps: Number of simulation steps to run
            timestep: Time step duration in seconds
            verbose: Print progress information if True
            
        Returns:
            List of dictionaries with training metrics
        )pbdoc");

    // Performance monitoring
    m.def("profile_operation",
        [](std::function<void()> operation, std::string name) {
            auto start = std::chrono::high_resolution_clock::now();
            operation();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            return std::make_pair(name, duration.count());
        }, py::arg("operation"), py::arg("name"),
        R"pbdoc(Profile a computation and return timing.
        
        Args:
            operation: Function to profile
            name: Name for the operation
            
        Returns:
            Tuple of (operation_name, duration_ms)
        )pbdoc");

    // Data analysis utilities
    m.def("analyze_simulation_results",
        [](const std::vector<std::map<std::string, py::object>>& results) {
            std::map<std::string, py::object> analysis;
            
            if (results.empty()) {
                return analysis;
            }
            
            std::vector<double> spike_counts, firing_rates, curiosity_levels;
            std::vector<int> action_types;
            
            for (const auto& result : results) {
                auto it = result.find("spike_count");
                if (it != result.end()) {
                    spike_counts.push_back(it->second.cast<double>());
                }
                
                it = result.find("average_firing_rate");
                if (it != result.end()) {
                    firing_rates.push_back(it->second.cast<double>());
                }
                
                it = result.find("curiosity_level");
                if (it != result.end()) {
                    curiosity_levels.push_back(it->second.cast<double>());
                }
                
                it = result.find("action_type");
                if (it != result.end()) {
                    action_types.push_back(it->second.cast<int>());
                }
            }
            
            if (!spike_counts.empty()) {
                double sum = 0, sum_sq = 0;
                for (double val : spike_counts) {
                    sum += val;
                    sum_sq += val * val;
                }
                double mean = sum / spike_counts.size();
                double variance = (sum_sq / spike_counts.size()) - (mean * mean);
                double stddev = std::sqrt(variance);
                
                analysis["spike_count_mean"] = mean;
                analysis["spike_count_std"] = stddev;
                analysis["spike_count_max"] = *std::max_element(spike_counts.begin(), spike_counts.end());
                analysis["spike_count_min"] = *std::min_element(spike_counts.begin(), spike_counts.end());
            }
            
            if (!firing_rates.empty()) {
                double sum = 0;
                for (double val : firing_rates) sum += val;
                analysis["firing_rate_mean"] = sum / firing_rates.size();
            }
            
            if (!curiosity_levels.empty()) {
                double sum = 0;
                for (double val : curiosity_levels) sum += val;
                analysis["curiosity_mean"] = sum / curiosity_levels.size();
                analysis["curiosity_max"] = *std::max_element(curiosity_levels.begin(), curiosity_levels.end());
            }
            
            if (!action_types.empty()) {
                std::map<int, int> action_counts;
                for (int action : action_types) {
                    action_counts[action]++;
                }
                analysis["action_distribution"] = action_counts;
            }
            
            analysis["total_steps"] = static_cast<int>(results.size());
            analysis["num_episodes"] = static_cast<int>(
                results.empty() ? 0 : results[0]["episode"].cast<int>() + 1
            );
            
            return analysis;
        }, py::arg("results"),
        R"pbdoc(Analyze simulation results and compute statistics.
        
        Args:
            results: List of result dictionaries from simulation functions
            
        Returns:
            Dictionary of analysis results including statistics
        )pbdoc");

    // Context managers for resource management
    py::class_<BrainContextManager>(m, "BrainContextManager", 
        R"pbdoc(Context manager for Brain objects.)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("__enter__", [](BrainContextManager& self) {
            self.config_->set("brain.neuron_count", 1000);
            self.config_->set("brain.synapse_density", 0.1);
            self.brain_ = std::make_shared<Brain>(self.config_);
            self.brain_->initialize();
            return self.brain_.get();
        })
        .def("__exit__", [](BrainContextManager&, const std::exception_ptr&) {
            // Cleanup logic
        });

    py::class_<ConfigContextManager>(m, "ConfigContextManager",
        R"pbdoc(Context manager for Config objects.)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("__enter__", [](ConfigContextManager& self) {
            return self.config_.get();
        })
        .def("__exit__", [](ConfigContextManager&, const std::exception_ptr&) {
            // Cleanup logic
        });

    // Convenience context manager functions
    m.def("with_brain",
        [](const std::shared_ptr<Config>& config,
           std::function<py::object(Brain*)> func) {
            BrainContextManager mgr(config);
            return func(mgr.brain_.get());
        }, py::arg("config"), py::arg("func"),
        R"pbdoc(Execute function with managed Brain context.
        
        Args:
            config: Configuration for brain creation
            func: Function to execute with brain
            
        Returns:
            Result of function execution
        )pbdoc");

    m.def("with_config",
        [](const std::shared_ptr<Config>& config,
           std::function<py::object(Config*)> func) {
            ConfigContextManager mgr(config);
            return func(mgr.config_.get());
        }, py::arg("config"), py::arg("func"),
        R"pbdoc(Execute function with managed Config context.
        
        Args:
            config: Config object to manage
            func: Function to execute with config
            
        Returns:
            Result of function execution
        )pbdoc");

    // Legacy factory functions (for backward compatibility)
    m.def("createBrain", 
        [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            return std::make_shared<Brain>(config);
        }, py::arg("config"),
        R"pbdoc(Legacy factory function for creating Brain.
        
        Args:
            config: Configuration object
            
        Returns:
            New Brain instance
        )pbdoc");

    // Re-export core types
    m.attr("core") = py::module::import("nlm_core");

} // namespace nlm

} // namespace nlm
