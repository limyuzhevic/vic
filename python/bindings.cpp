// Advanced utility functions for Python bindings

namespace nlm::utilities {

    // Brain simulation utilities
    class SimulationUtilities {
    public:
        // Create a test brain with specified characteristics
        static std::shared_ptr<Brain> createTestBrain(int neuron_count = 100, int region_count = 1) {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
            config->set("brain.synapse_density", 0.1, ConfigSource::Default);
            config->set("brain.region_count", static_cast<int64_t>(region_count), ConfigSource::Default);
            config->set("connection_probability", 0.05, ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            return brain;
        }

        // Create a complete agent setup
        static std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> 
        createAgentSetup(int world_width = 20, int world_height = 20, 
                        int vision_width = 8, int vision_height = 8) {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto world = std::make_shared<SimpleWorld>();
            world->configure(static_cast<float>(world_width), static_cast<float>(world_height),
                            vision_width, vision_height);
            world->reset();
            world->setAgentStart(10.0f, 10.0f);
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            
            return std::make_tuple(brain, world, agent);
        }

        // Run simulation with detailed statistics
        static py::dict runSimulationWithStats(std::shared_ptr<Brain> brain, SimulationStep num_steps,
                                              std::shared_ptr<SimpleWorld> world = nullptr,
                                              std::shared_ptr<AgentBrain> agent = nullptr) {
            py::dict stats;
            
            stats["start_time"] = world ? world->getSimulationTime() : 0.0;
            stats["neuron_count"] = brain->getTotalNeuronCount();
            stats["synapse_count"] = brain->getTotalSynapseCount();
            
            // Run simulation
            for (SimulationStep step = 0; step < num_steps; ++step) {
                if (world && agent) {
                    world->update(0.1);
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                }
                brain->step(step);
                
                if (step % 100 == 0) {
                    stats["step_" + std::to_string(step)] = brain->getFiringNeuronCount();
                }
            }
            
            stats["end_time"] = world ? world->getSimulationTime() : 0.0;
            stats["total_spikes"] = brain->getTotalSpikeCount();
            stats["final_firing_rate"] = brain->getAverageFiringRate();
            
            if (agent) {
                stats["final_curiosity"] = agent->getCuriosityLevel();
                stats["final_novelty"] = agent->getNoveltyLevel();
                stats["final_prediction_error"] = agent->getPredictionError();
            }
            
            return stats;
        }

        // Save and load brain with backup
        static bool saveBrainWithBackup(const Brain& brain, const std::string& filepath,
                                        bool create_backup = true) {
            try {
                if (create_backup && std::filesystem::exists(filepath)) {
                    std::string backup_path = filepath + ".backup";
                    std::filesystem::copy_file(filepath, backup_path, std::filesystem::copy_options::overwrite_existing);
                }
                return brain.save(filepath);
            } catch (const std::exception& e) {
                std::cerr << "Error saving brain: " << e.what() << std::endl;
                return false;
            }
        }

        static bool loadBrainWithBackup(std::shared_ptr<Brain> brain, const std::string& filepath,
                                        bool restore_from_backup = false) {
            try {
                std::string target_path = filepath;
                if (restore_from_backup && std::filesystem::exists(filepath + ".backup")) {
                    target_path = filepath + ".backup";
                }
                return brain->load(target_path);
            } catch (const std::exception& e) {
                std::cerr << "Error loading brain: " << e.what() << std::endl;
                return false;
            }
        }

        // Brain comparison utilities
        static py::dict compareBrains(const Brain& brain1, const Brain& brain2) {
            py::dict comparison;
            
            comparison["neuron_count_match"] = brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount();
            comparison["synapse_count_match"] = brain1.getTotalSynapseCount() == brain2.getTotalSynapseCount();
            comparison["firing_neurons_diff"] = std::abs(static_cast<int>(brain1.getFiringNeuronCount() - brain2.getFiringNeuronCount()));
            comparison["average_firing_rate_diff"] = std::abs(brain1.getAverageFiringRate() - brain2.getAverageFiringRate());
            comparison["total_spikes_diff"] = std::abs(static_cast<int>(brain1.getTotalSpikeCount() - brain2.getTotalSpikeCount()));
            comparison["developmental_stage_match"] = brain1.getDevelopmentalStage() == brain2.getDevelopmentalStage();
            
            return comparison;
        }

        // Statistical analysis utilities
        static py::dict analyzeBrainStatistics(const Brain& brain) {
            py::dict analysis;
            
            size_t neuron_count = brain.getTotalNeuronCount();
            size_t firing_count = brain.getFiringNeuronCount();
            size_t active_count = brain.getActiveNeuronCount();
            size_t total_spikes = brain.getTotalSpikeCount();
            float avg_firing_rate = brain.getAverageFiringRate();
            float e_i_ratio = brain.getExcitationInhibitionRatio();
            
            analysis["total_neurons"] = neuron_count;
            analysis["firing_neurons"] = firing_count;
            analysis["active_neurons"] = active_count;
            analysis["firing_percentage"] = (neuron_count > 0) ? (firing_count * 100.0 / neuron_count) : 0.0;
            analysis["active_percentage"] = (neuron_count > 0) ? (active_count * 100.0 / neuron_count) : 0.0;
            analysis["total_spikes"] = total_spikes;
            analysis["average_firing_rate"] = avg_firing_rate;
            analysis["excitement_inhibition_ratio"] = e_i_ratio;
            analysis["is_stable"] = (avg_firing_rate < 100.0) && (e_i_ratio >= 0.1 && e_i_ratio <= 10.0);
            
            // Calculate firing rate statistics
            float max_reasonable_rate = 100.0; // Hz
            analysis["firing_rate_status"] = (avg_firing_rate > max_reasonable_rate) ? "high" : 
                                           (avg_firing_rate > 10.0) ? "moderate" : "low";
            
            // Calculate E/I ratio status
            analysis["ei_ratio_status"] = (e_i_ratio > 10.0) ? "high_excitation" : 
                                         (e_i_ratio < 0.1) ? "high_inhibition" : "balanced";
            
            return analysis;
        }

        // Configuration utilities
        static py::dict getConfigurationInfo(const Config& config) {
            py::dict info;
            
            info["has_neuron_count"] = config.has("brain.neuron_count");
            info["has_synapse_density"] = config.has("brain.synapse_density");
            info["has_plasticity_settings"] = config.has("plasticity.stdp.enable");
            info["has_neuromodulation_settings"] = config.has("neuromod.dopamine.scale");
            
            auto keys = config.getKeys();
            py::list key_list;
            for (const auto& key : keys) {
                key_list.append(key);
            }
            info["configuration_keys"] = key_list;
            
            return info;
        }

        // World utilities
        static py::dict analyzeWorldState(const SimpleWorld& world) {
            py::dict analysis;
            
            analysis["world_width"] = world.getWidth();
            analysis["world_height"] = world.getHeight();
            analysis["simulation_time"] = world.getSimulationTime();
            analysis["max_energy"] = world.getMaxEnergy();
            analysis["agent_x"] = world.getAgentBody().x;
            analysis["agent_y"] = world.getAgentBody().y;
            analysis["agent_energy"] = world.getAgentBody().energy;
            analysis["agent_health"] = world.getAgentBody().health;
            analysis["agent_age"] = world.getAgentBody().age;
            analysis["is_moving"] = world.getAgentBody().isMoving;
            analysis["is_turning"] = world.getAgentBody().isTurning;
            
            // Get sensory information
            auto percept = world.getSensoryPercept();
            analysis["vision_size"] = percept.getVisionWidth() * percept.getVisionHeight();
            analysis["touch_size"] = percept.getTouch().size();
            analysis["internal_size"] = percept.getInternal().size();
            analysis["proprioception_size"] = percept.getProprioception().size();
            
            return analysis;
        }

        // Performance benchmarking
        static py::dict benchmarkBrainPerformance(std::shared_ptr<Brain> brain, SimulationStep num_steps,
                                                  int num_runs = 3) {
            py::dict results;
            std::vector<double> times;
            std::vector<size_t> spike_counts;
            
            for (int run = 0; run < num_runs; ++run) {
                auto start_time = std::chrono::high_resolution_clock::now();
                
                for (SimulationStep step = 0; step < num_steps; ++step) {
                    brain->step(step);
                }
                
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration<double>(end_time - start_time);
                times.push_back(duration.count());
                spike_counts.push_back(brain->getTotalSpikeCount());
                
                // Reset brain for next run
                brain->reset();
                brain->initialize();
            }
            
            double total_time = 0.0;
            double total_spikes = 0.0;
            for (size_t i = 0; i < times.size(); ++i) {
                total_time += times[i];
                total_spikes += spike_counts[i];
            }
            
            results["runs_completed"] = num_runs;
            results["steps_per_run"] = num_steps;
            results["average_time_per_run"] = total_time / num_runs;
            results["total_time"] = total_time;
            results["average_spikes_per_run"] = total_spikes / num_runs;
            results["spikes_per_second"] = total_spikes / total_time;
            
            // Calculate standard deviation
            double time_variance = 0.0;
            double spike_variance = 0.0;
            for (size_t i = 0; i < times.size(); ++i) {
                time_variance += (times[i] - (total_time / num_runs)) * (times[i] - (total_time / num_runs));
                spike_variance += (spike_counts[i] - (total_spikes / num_runs)) * (spike_counts[i] - (total_spikes / num_runs));
            }
            
            results["time_std_dev"] = std::sqrt(time_variance / num_runs);
            results["spike_std_dev"] = std::sqrt(spike_variance / num_runs);
            
            return results;
        }
    };
}

// Python bindings for advanced utilities
void bindAdvancedUtilities(py::module_& m) {
    // Bind simulation utilities
    py::class_<nlm::utilities::SimulationUtilities>(m, "SimulationUtilities")
        .def_static("create_test_brain", &nlm::utilities::SimulationUtilities::createTestBrain,
                    py::arg("neuron_count") = 100, py::arg("region_count") = 1,
                    "Create a test brain with specified characteristics")
        .def_static("create_agent_setup", &nlm::utilities::SimulationUtilities::createAgentSetup,
                    py::arg("world_width") = 20, py::arg("world_height") = 20,
                    py::arg("vision_width") = 8, py::arg("vision_height") = 8,
                    "Create a complete agent setup")
        .def_static("run_simulation_with_stats", &nlm::utilities::SimulationUtilities::runSimulationWithStats,
                    py::arg("brain"), py::arg("num_steps"), py::arg("world") = nullptr, py::arg("agent") = nullptr,
                    "Run simulation with detailed statistics")
        .def_static("save_brain_with_backup", &nlm::utilities::SimulationUtilities::saveBrainWithBackup,
                    py::arg("brain"), py::arg("filepath"), py::arg("create_backup") = true,
                    "Save brain with optional backup")
        .def_static("load_brain_with_backup", &nlm::utilities::SimulationUtilities::loadBrainWithBackup,
                    py::arg("brain"), py::arg("filepath"), py::arg("restore_from_backup") = false,
                    "Load brain with optional backup restoration")
        .def_static("compare_brains", &nlm::utilities::SimulationUtilities::compareBrains,
                    py::arg("brain1"), py::arg("brain2"), "Compare two brains")
        .def_static("analyze_brain_statistics", &nlm::utilities::SimulationUtilities::analyzeBrainStatistics,
                    py::arg("brain"), "Analyze brain statistics")
        .def_static("get_configuration_info", &nlm::utilities::SimulationUtilities::getConfigurationInfo,
                    py::arg("config"), "Get configuration information")
        .def_static("analyze_world_state", &nlm::utilities::SimulationUtilities::analyzeWorldState,
                    py::arg("world"), "Analyze world state")
        .def_static("benchmark_brain_performance", &nlm::utilities::SimulationUtilities::benchmarkBrainPerformance,
                    py::arg("brain"), py::arg("num_steps"), py::arg("num_runs") = 3,
                    "Benchmark brain performance");
    
    // Convenience functions
    m.def("create_simple_test", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
        config->set("brain.synapse_density", 0.1, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a simple test brain");
    
    m.def("create_complete_test_setup", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 8, 8);
        world->reset();
        world->setAgentStart(10.0f, 10.0f);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create complete test setup");
    
    m.def("run_quick_simulation", [](SimulationStep num_steps) {
        auto brain = std::make_shared<Brain>(std::make_shared<Config>());
        brain->initialize();
        
        for (SimulationStep step = 0; step < num_steps; ++step) {
            brain->step(step);
        }
        
        py::dict results;
        results["steps_completed"] = num_steps;
        results["total_spikes"] = brain->getTotalSpikeCount();
        results["firing_neurons"] = brain->getFiringNeuronCount();
        results["average_firing_rate"] = brain->getAverageFiringRate();
        
        return results;
    }, py::arg("num_steps"), "Run quick simulation");
    
    m.def("create_agent_with_features", [](const std::string& agent_name) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Enable all features
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return agent;
    }, py::arg("agent_name") = "default_agent", "Create agent with all features enabled");
}

// Main binding function with utilities
PYBIND11_MODULE(pynlm, m) {
    // Set module documentation
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Advanced Python Bindings
        ----------------------------------------------------
        Enhanced Python bindings with advanced utilities and utilities.
        
        This module provides advanced Python bindings with:
        - Comprehensive simulation utilities
        - Brain analysis and comparison tools
        - Performance benchmarking
        - Configuration validation
        - World analysis utilities
        - Backup and restore functionality
        - Context managers for simulation control
        - Enhanced error handling
        - Logging and debugging tools
        
        Key Features:
        - Advanced simulation utilities for complex scenarios
        - Brain analysis and statistical tools
        - Performance benchmarking and optimization
        - Comprehensive configuration management
        - World state analysis and monitoring
        - Backup and restore with safety checks
        - Context managers for controlled simulation execution
        - Enhanced Pythonic API design
        
        Usage Examples:
        ```python
        import pynlm
        
        # Create test setup
        brain, world, agent = pynlm.create_complete_test_setup()
        
        # Run simulation with statistics
        stats = pynlm.SimulationUtilities.run_simulation_with_stats(brain, 1000, world, agent)
        
        # Analyze brain statistics
        analysis = pynlm.SimulationUtilities.analyze_brain_statistics(brain)
        
        # Benchmark performance
        benchmark = pynlm.SimulationUtilities.benchmark_brain_performance(brain, 500)
        
        # Create agent with features
        agent = pynlm.create_agent_with_features("my_agent")
        ```
    )pbdoc";
    
    // Register exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    
    // Register enhanced logger
    py::class_<nlm::utilities::EnhancedPythonLogger>(m, "AdvancedLogger")
        .def("info", &nlm::utilities::EnhancedPythonLogger::info, "Log informational message")
        .def("warn", &nlm::utilities::EnhancedPythonLogger::warn, "Log warning message")
        .def("error", &nlm::utilities::EnhancedPythonLogger::error, "Log error message")
        .def("debug", &nlm::utilities::EnhancedPythonLogger::debug, "Log debug message")
        .def("log_to_file", &nlm::utilities::EnhancedPythonLogger::logToFile,
             py::arg("filepath"), py::arg("level") = "INFO",
             "Log messages to file")
        .def("log_to_callback", &nlm::utilities::EnhancedPythonLogger::logToCallback,
             py::arg("callback"), py::arg("level") = "INFO",
             "Log messages to Python callback");
    
    // Set global logger
    m.attr("log") = &nlm::utilities::EnhancedPythonLogger::getInstance();
    
    // Register version information
    m.attr("__version__") = "0.1.0-advanced";
    m.attr("__author__") = "NLM Authors";
    m.attr("__license__") = "MIT";
    m.attr("__description__") = "Advanced Python bindings for NLM neural simulation framework";
    
    // Register basic bindings
    bindValidationAndRuntime(m);
    bindAdvancedUtilities(m);
    
    // Register enhanced bindings
    bindEnhancedConfig(m);
    bindEnhancedBrain(m);
    bindEnhancedAgentBrain(m);
    bindEnhancedSimpleWorld(m);
    
    // Register core bindings (basic functionality)
    bindNeuronId(m);
    bindSynapseId(m);
    bindRegionId(m);
    bindPopulationId(m);
    
    bindNeuronType(m);
    bindSynapseType(m);
    bindDevelopmentalStage(m);
    bindFiringState(m);
    
    bindActionType(m);
    bindMotorCommand(m);
    bindWorldObjectType(m);
    
    // Register basic class bindings
    bindSensoryInput(m);
    bindVision(m);
    bindAudio(m);
    bindInternalSignals(m);
    bindAction(m);
    bindWorldObject(m);
    bindAgentBody(m);
    bindActionResult(m);
    bindSensoryPercept(m);
    
    // Register utility functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create agent brain interface");
    
    // Register invalid ID sentinel values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Register context manager
    py::class_<nlm::SimulationStepContext>(m, "SimulationStepContext")
        .def(py::init<std::shared_ptr<Brain>, SimulationStep, SimulationStep>())
        .def("__enter__", &nlm::SimulationStepContext::__enter__)
        .def("__exit__", &nlm::SimulationStepContext::__exit__);
    
    // Register example utilities
    m.def("example_simple_brain", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        for (int i = 0; i < 10; ++i) {
            brain->step(i);
        }
        return brain->getTotalNeuronCount();
    }, "Example function: create and run a simple brain");
    
    m.def("example_agent_simulation", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        for (int i = 0; i < 50; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
        }
        
        return agent->getCuriosityLevel();
    }, "Example function: run a complete agent simulation");
}

} // namespace nlm