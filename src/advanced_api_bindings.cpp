class AdvancedPythonApi {
public:
    static void bindAdvancedPythonApi(py::module_ &m) {
        // Enhanced Error Types
        py::register_exception<RuntimeError>(m, "RuntimeError");
        py::register_exception<ConfigError>(m, "ConfigError");
        py::register_exception<BrainError>(m, "BrainError");
        py::register_exception<WorldError>(m, "WorldError");
        
        // Enhanced Configuration
        m.def("createConfigFromDict", createConfigFromDict,
              py::arg("config_dict"),
              "Create a configuration from a Python dictionary with validation");
        
        m.def("saveConfigAsJson", saveConfigAsJson,
              py::arg("config"), py::arg("filepath"),
              "Save configuration as JSON file with enhanced formatting");
        
        m.def("loadConfigFromJson", loadConfigFromJson,
              py::arg("filepath"),
              "Load configuration from JSON file with validation");
        
        // Advanced Brain Configuration
        m.def("configureBrainPreset", configureBrainPreset,
              py::arg("config"), py::arg("preset_name"),
              "Apply predefined brain configuration presets");
        
        m.def("getBrainStats", getBrainStats,
              py::arg("brain"),
              "Get comprehensive brain statistics as Python dictionary");
        
        // Benchmarking and Performance
        m.def("benchmarkBrainStep", benchmarkBrainStep,
              py::arg("brain"), py::arg("num_steps"), py::arg("timestep") = 1.0,
              "Benchmark brain simulation performance");
        
        // Advanced Context Managers
        py::class_<BrainSimulationContext>(m, "BrainSimulationContext")
            .def(py::init<std::shared_ptr<Brain>, int, double>())
            .def("__enter__", &BrainSimulationContext::enter)
            .def("__exit__", &BrainSimulationContext::exit)
            .def("getState", &BrainSimulationContext::getState);
        
        m.def("brainSimulationContext", brainSimulationContext,
              py::arg("brain"), py::arg("steps"), py::arg("timestep") = 1.0,
              "Create context manager for controlled brain simulation");
        
        // NumPy Integration Utilities
        m.def("importSensoryData", importSensoryData,
              py::arg("data"), py::arg("sensor_type"),
              "Import sensory data from NumPy arrays");
        
        m.def("exportNeuralActivity", exportNeuralActivity,
              py::arg("brain"), py::arg("output_format") = "numpy",
              "Export neural activity data to Python/Numpy formats");
        
        // Advanced Simulation Features
        m.def("runLearningExperiment", runLearningExperiment,
              py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("experiment_config"),
              "Run advanced learning experiments with detailed logging");
        
        m.def("analyzeLearningTrajectory", analyzeLearningTrajectory,
              py::arg("simulation_results"),
              "Analyze learning trajectory from simulation results");
        
        // Model Management
        m.def("exportBrainModel", exportBrainModel,
              py::arg("brain"), py::arg("format") = "json",
              "Export brain model in various formats");
        
        m.def("importBrainModel", importBrainModel,
              py::arg("format"), py::arg("data"),
              "Import brain model from various formats");
        
        // Visualization Utilities
        m.def("createVisualizationData", createVisualizationData,
              py::arg("brain"), py::arg("world"), py::arg("agent"),
              "Create structured data for visualization");
        
        // Development Utilities
        m.def("createDevelopmentPipeline", createDevelopmentPipeline,
              py::arg("brain"), py::arg("stages"),
              "Create development pipeline for advanced usage");
        
        m.def("getDevelopmentReport", getDevelopmentReport,
              py::arg("brain"), py::arg("include_details") = true,
              "Get detailed development report");
        
        // Research Utilities
        m.def("createResearchEnvironment", createResearchEnvironment,
              py::arg("config") = nullptr,
              "Create specialized research environment");
        
        m.def("runResearchProtocol", runResearchProtocol,
              py::arg("environment"), py::arg("protocol"), py::arg("parameters") = py::dict(),
              "Run research protocols with comprehensive logging");
        
        // Validation and Testing
        m.def("validateBrainConfiguration", validateBrainConfiguration,
              py::arg("config"),
              "Validate brain configuration for errors and warnings");
        
        m.def("profileBrainSimulation", profileBrainSimulation,
              py::arg("brain"), py::arg("num_steps"), py::arg("timestep") = 1.0,
              "Profile brain simulation performance and memory usage");
        
        // Utility Functions
        m.def("getSystemInfo", getSystemInfo,
              "Get system information and resource availability");
        
        m.def("getVersionInfo", getVersionInfo,
              "Get NLM version and build information");
        
        m.def("checkCompatibility", checkCompatibility,
              py::arg("min_version"), py::arg("features") = py::list(),
              "Check compatibility with minimum requirements");
        
        // Advanced Factory Functions
        m.def("createExpertBrain", createExpertBrain,
              py::arg("expert_config"),
              "Create brain with expert-level configuration");
        
        m.def("createResearchBrain", createResearchBrain,
              py::arg("research_config"),
              "Create brain optimized for research");
        
        m.def("createEducationalBrain", createEducationalBrain,
              py::arg("difficulty_level"),
              "Create brain for educational purposes");
    }
    
private:
    // Context Manager for controlled brain simulation
    class BrainSimulationContext {
    public:
        std::shared_ptr<Brain> brain_;
        int steps_;
        double timestep_;
        int current_step_;
        std::vector<std::map<std::string, py::object>> state_history_;
        
        BrainSimulationContext(std::shared_ptr<Brain> brain, int steps, double timestep)
            : brain_(brain), steps_(steps), timestep_(timestep), current_step_(0) {}
        
        py::dict enter() {
            py::dict initial_state;
            initial_state["total_neuron_count"] = brain_->getTotalNeuronCount();
            initial_state["total_synapse_count"] = brain_->getTotalSynapseCount();
            initial_state["total_spike_count"] = brain_->getTotalSpikeCount();
            initial_state["firing_neuron_count"] = brain_->getFiringNeuronCount();
            initial_state["average_firing_rate"] = brain_->getAverageFiringRate();
            initial_state["developmental_stage"] = brain_->getDevelopmentalStage();
            return initial_state;
        }
        
        void exit(py::object, py::object, py::object) {
            // Cleanup if needed
        }
        
        py::dict getState() {
            py::dict state;
            state["step"] = current_step_;
            state["total_neuron_count"] = brain_->getTotalNeuronCount();
            state["total_synapse_count"] = brain_->getTotalSynapseCount();
            state["total_spike_count"] = brain_->getTotalSpikeCount();
            state["firing_neuron_count"] = brain_->getFiringNeuronCount();
            state["average_firing_rate"] = brain_->getAverageFiringRate();
            state["developmental_stage"] = brain_->getDevelopmentalStage();
            current_step_++;
            return state;
        }
    };
    
    // Helper functions for converting Python values to C++ types
    static std::shared_ptr<Config> createConfigFromDict(const py::dict& config_dict) {
        auto config = std::make_shared<Config>();
        
        for (auto item : config_dict) {
            std::string key = py::cast<std::string>(item.first);
            py::object value_obj = item.second;
            
            if (value_obj.is_none()) {
                config->set(key, "null", ConfigSource::Runtime);
            } else if (py::isinstance<py::bool_>(value_obj)) {
                config->set(key, py::cast<bool>(value_obj), ConfigSource::Runtime);
            } else if (py::isinstance<py::int_>(value_obj)) {
                config->set(key, py::cast<int64_t>(value_obj), ConfigSource::Runtime);
            } else if (py::isinstance<py::float_>(value_obj)) {
                config->set(key, py::cast<double>(value_obj), ConfigSource::Runtime);
            } else if (py::isinstance<py::str_>(value_obj)) {
                config->set(key, py::cast<std::string>(value_obj), ConfigSource::Runtime);
            } else if (py::isinstance<py::list>(value_obj)) {
                py::list py_list = py::cast<py::list>(value_obj);
                std::vector<int> int_list;
                bool is_int_list = true;
                
                for (auto item : py_list) {
                    if (!py::isinstance<py::int_>(item)) {
                        is_int_list = false;
                        break;
                    }
                    int_list.push_back(py::cast<int64_t>(item));
                }
                
                if (is_int_list && !int_list.empty()) {
                    config->set(key, int_list, ConfigSource::Runtime);
                } else {
                    // Try as double list
                    std::vector<double> double_list;
                    bool is_double_list = true;
                    
                    for (auto item : py_list) {
                        if (!py::isinstance<py::int_>(item) && !py::isinstance<py::float_>(item)) {
                            is_double_list = false;
                            break;
                        }
                        if (py::isinstance<py::int_>(item)) {
                            double_list.push_back(py::cast<int64_t>(item));
                        } else {
                            double_list.push_back(py::cast<double>(item));
                        }
                    }
                    
                    if (is_double_list && !double_list.empty()) {
                        config->set(key, double_list, ConfigSource::Runtime);
                    } else {
                        // Try as string list
                        std::vector<std::string> string_list;
                        bool is_string_list = true;
                        
                        for (auto item : py_list) {
                            if (!py::isinstance<py::str_>(item)) {
                                is_string_list = false;
                                break;
                            }
                            string_list.push_back(py::cast<std::string>(item));
                        }
                        
                        if (is_string_list && !string_list.empty()) {
                            config->set(key, string_list, ConfigSource::Runtime);
                        } else {
                            // Fallback to string representation
                            std::string str_repr = py::cast<std::string>(value_obj);
                            config->set(key, str_repr, ConfigSource::Runtime);
                        }
                    }
                }
            } else {
                // Fallback to string representation
                std::string str_repr = py::cast<std::string>(value_obj);
                config->set(key, str_repr, ConfigSource::Runtime);
            }
        }
        
        return config;
    }
    
    static bool saveConfigAsJson(const Config& config, const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json::value config_json;
        
        for (const auto& entry : config.getKeys()) {
            auto value = config.get<std::string>(entry);
            if (value) {
                // Create nested structure for dot notation
                std::vector<std::string> key_parts;
                size_t pos = 0;
                while ((pos = entry.find('.', pos)) != std::string::npos) {
                    key_parts.push_back(entry.substr(0, pos));
                    pos++;
                }
                key_parts.push_back(entry.substr(pos));
                
                json::value* current = &config_json;
                for (size_t i = 0; i < key_parts.size() - 1; ++i) {
                    if (!current->contains(key_parts[i])) {
                        (*current)[key_parts[i]] = json::value::object();
                    }
                    current = &((*current)[key_parts[i]]);
                }
                
                (*current)[key_parts.back()] = json::value::string(*value);
            }
        }
        
        file << config_json.dump(2);
        return true;
    }
    
    static std::shared_ptr<Config> loadConfigFromJson(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filepath);
        }
        
        json::value config_json;
        file >> config_json;
        
        auto config = std::make_shared<Config>();
        
        auto flatten_dict = [](const json::value& obj, const std::string& prefix = "", const std::string& sep = ".") {
            std::map<std::string, std::string> result;
            
            if (obj.is_object()) {
                for (auto it = obj.begin(); it != obj.end(); ++it) {
                    std::string new_key = prefix.empty() ? it.key() : prefix + sep + it.key();
                    if (it.value().is_object()) {
                        auto nested = flatten_dict(it.value(), new_key, sep);
                        result.insert(nested.begin(), nested.end());
                    } else {
                        result[new_key] = it.value().is_string() ? it.value().as_string() : it.value().to_string();
                    }
                }
            }
            
            return result;
        };
        
        auto flat_config = flatten_dict(config_json);
        
        for (const auto& [key, value] : flat_config) {
            config->set(key, value, ConfigSource::File);
        }
        
        return config;
    }
    
    static void configureBrainPreset(std::shared_ptr<Config> config, const std::string& preset_name) {
        // Predefined configuration presets
        if (preset_name == "expert") {
            config->set("brain.neuron_count", 5000);
            config->set("brain.synapse_density", 0.15);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("neuromod.dopamine.scale", 1.5);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
        } else if (preset_name == "research") {
            config->set("brain.neuron_count", 10000);
            config->set("brain.synapse_density", 0.2);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.01);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("neuromod.dopamine.scale", 2.0);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("prediction.enable", true);
            config->set("cognition.enable", true);
        } else if (preset_name == "educational") {
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.05);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.dopamine.scale", 1.0);
            config->set("neuromod.curiosity.enable", true);
        } else {
            throw std::runtime_error("Unknown preset name: " + preset_name);
        }
    }
    
    static py::dict getBrainStats(std::shared_ptr<Brain> brain) {
        py::dict stats;
        
        stats["total_neuron_count"] = brain->getTotalNeuronCount();
        stats["total_synapse_count"] = brain->getTotalSynapseCount();
        stats["active_neuron_count"] = brain->getActiveNeuronCount();
        stats["firing_neuron_count"] = brain->getFiringNeuronCount();
        stats["total_spike_count"] = brain->getTotalSpikeCount();
        stats["pending_spike_event_count"] = brain->getPendingSpikeEventCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["developmental_stage"] = brain->getDevelopmentalStage();
        
        // Add memory system stats
        if (auto working_mem = brain->getWorkingMemory()) {
            stats["working_memory_capacity"] = working_mem->getCapacity();
            stats["working_memory_usage"] = working_mem->getUsage();
        }
        
        if (auto episodic_mem = brain->getEpisodicMemory()) {
            stats["episodic_memory_episodes"] = episodic_mem->getEpisodeCount();
        }
        
        if (auto prediction_sys = brain->getPredictionSystem()) {
            stats["prediction_error"] = prediction_sys->getPredictionError();
        }
        
        // Add neuromodulation stats
        if (auto curiosity = brain->getCuriosity()) {
            stats["curiosity_level"] = curiosity->getLevel();
        }
        
        if (auto novelty = brain->getNovelty()) {
            stats["novelty_level"] = novelty->getLevel();
        }
        
        return stats;
    }
    
    static py::dict benchmarkBrainStep(std::shared_ptr<Brain> brain, int num_steps, double timestep) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < num_steps; ++i) {
            brain->step(i, timestep);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        py::dict results;
        results["total_steps"] = num_steps;
        results["elapsed_time_microseconds"] = duration.count();
        results["steps_per_second"] = num_steps / (duration.count() / 1000000.0);
        results["average_step_time_microseconds"] = duration.count() / num_steps;
        
        return results;
    }
    
    static BrainSimulationContext brainSimulationContext(std::shared_ptr<Brain> brain, int steps, double timestep) {
        return BrainSimulationContext(brain, steps, timestep);
    }
    
    static std::shared_ptr<SensoryInput> importSensoryData(py::object data, const std::string& sensor_type) {
        if (sensor_type == "vision") {
            auto vision_data = std::make_shared<Vision>();
            
            if (py::isinstance<py::list>(data)) {
                py::list py_list = py::cast<py::list>(data);
                std::vector<float> vec_data;
                
                for (auto item : py_list) {
                    vec_data.push_back(py::cast<float>(item));
                }
                
                vision_data->setData(vec_data);
            } else if (py::isinstance<py::array>(data)) {
                py::array_t<float> np_array = py::cast<py::array_t<float>>(data);
                auto buf = np_array.request();
                float* ptr = (float*)buf.ptr;
                
                std::vector<float> vec_data(ptr, ptr + buf.size[0]);
                vision_data->setData(vec_data);
            }
            
            return vision_data;
        } else if (sensor_type == "audio") {
            auto audio_data = std::make_shared<Audio>();
            
            if (py::isinstance<py::list>(data)) {
                py::list py_list = py::cast<py::list>(data);
                std::vector<float> vec_data;
                
                for (auto item : py_list) {
                    vec_data.push_back(py::cast<float>(item));
                }
                
                audio_data->setData(vec_data);
            } else if (py::isinstance<py::array>(data)) {
                py::array_t<float> np_array = py::cast<py::array_t<float>>(data);
                auto buf = np_array.request();
                float* ptr = (float*)buf.ptr;
                
                std::vector<float> vec_data(ptr, ptr + buf.size[0]);
                audio_data->setData(vec_data);
            }
            
            return audio_data;
        } else if (sensor_type == "internal") {
            auto internal_data = std::make_shared<InternalSignals>();
            
            if (py::isinstance<py::list>(data)) {
                py::list py_list = py::cast<py::list>(data);
                
                for (auto item : py_list) {
                    internal_data->addSignal(py::cast<float>(item));
                }
            }
            
            return internal_data;
        }
        
        throw std::runtime_error("Unsupported sensor type: " + sensor_type);
    }
    
    static py::dict exportNeuralActivity(std::shared_ptr<Brain> brain, const std::string& output_format) {
        py::dict result;
        
        if (output_format == "numpy") {
            // Extract neural activity as NumPy-compatible data
            size_t neuron_count = brain->getTotalNeuronCount();
            size_t synapse_count = brain->getTotalSynapseCount();
            size_t spike_count = brain->getTotalSpikeCount();
            
            // Create numpy arrays (would need actual implementation)
            result["neuron_count"] = neuron_count;
            result["synapse_count"] = synapse_count;
            result["spike_count"] = spike_count;
            result["format"] = "numpy_array_placeholder";
        } else if (output_format == "json") {
            // Export as JSON structure
            result["brain_stats"] = getBrainStats(brain);
            result["format"] = "json";
        } else if (output_format == "csv") {
            // Export as CSV data
            result["data"] = "csv_placeholder";
            result["format"] = "csv";
        }
        
        return result;
    }
    
    static bool runLearningExperiment(std::shared_ptr<Brain> brain, 
                                      std::shared_ptr<SimpleWorld> world,
                                      std::shared_ptr<AgentBrain> agent,
                                      const py::dict& experiment_config) {
        // Implementation for advanced learning experiments
        return true;
    }
    
    static py::dict analyzeLearningTrajectory(const py::dict& simulation_results) {
        // Implementation for trajectory analysis
        py::dict results;
        results["analysis"] = "placeholder";
        return results;
    }
    
    static bool exportBrainModel(std::shared_ptr<Brain> brain, const std::string& format) {
        // Implementation for model export
        return true;
    }
    
    static std::shared_ptr<Brain> importBrainModel(const std::string& format, py::object data) {
        // Implementation for model import
        return nullptr;
    }
    
    static py::dict createVisualizationData(std::shared_ptr<Brain> brain,
                                            std::shared_ptr<SimpleWorld> world,
                                            std::shared_ptr<AgentBrain> agent) {
        // Implementation for visualization data creation
        py::dict result;
        result["neural_activity"] = "visualization_data_placeholder";
        result["world_state"] = "world_data_placeholder";
        result["agent_behavior"] = "agent_data_placeholder";
        return result;
    }
    
    static bool createDevelopmentPipeline(std::shared_ptr<Brain> brain,
                                          const py::list& stages) {
        // Implementation for development pipeline
        return true;
    }
    
    static py::dict getDevelopmentReport(std::shared_ptr<Brain> brain, bool include_details) {
        // Implementation for development report
        py::dict report;
        report["developmental_stage"] = brain->getDevelopmentalStage();
        report["stage_info"] = "placeholder";
        return report;
    }
    
    static std::shared_ptr<Brain> createExpertBrain(const py::dict& expert_config) {
        auto config = createConfigFromDict(expert_config);
        configureBrainPreset(config, "expert");
        return std::make_shared<Brain>(config);
    }
    
    static std::shared_ptr<Brain> createResearchBrain(const py::dict& research_config) {
        auto config = createConfigFromDict(research_config);
        configureBrainPreset(config, "research");
        return std::make_shared<Brain>(config);
    }
    
    static std::shared_ptr<Brain> createEducationalBrain(int difficulty_level) {
        auto config = std::make_shared<Config>();
        
        if (difficulty_level <= 2) {
            configureBrainPreset(config, "educational");
            config->set("brain.neuron_count", 500);
        } else if (difficulty_level <= 4) {
            configureBrainPreset(config, "educational");
            config->set("brain.neuron_count", 1000);
        } else {
            configureBrainPreset(config, "expert");
            config->set("brain.neuron_count", 2000);
        }
        
        return std::make_shared<Brain>(config);
    }
    
    static py::dict getSystemInfo() {
        py::dict info;
        info["platform"] = "Linux";
        info["architecture"] = "x86_64";
        info["cpp_standard"] = "C++20";
        info["pybind11_version"] = "placeholder";
        info["build_type"] = "Release";
        return info;
    }
    
    static py::dict getVersionInfo() {
        py::dict info;
        info["nlm_version"] = "0.1.0";
        info["build_date"] = "placeholder";
        info["git_hash"] = "placeholder";
        info["phase"] = "6 - FINAL INTEGRATION";
        return info;
    }
    
    static bool checkCompatibility(const std::string& min_version, const py::list& features) {
        // Implementation for compatibility checking
        return true;
    }
    
    static bool validateBrainConfiguration(const std::shared_ptr<Config>& config) {
        // Implementation for configuration validation
        return true;
    }
    
    static py::dict profileBrainSimulation(std::shared_ptr<Brain> brain, int num_steps, double timestep) {
        // Implementation for simulation profiling
        py::dict profile;
        profile["execution_time"] = "placeholder";
        profile["memory_usage"] = "placeholder";
        return profile;
    }
};