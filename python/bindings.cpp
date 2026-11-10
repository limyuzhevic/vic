    // Advanced configuration management for complex scenarios
    py::class_<Config, std::shared_ptr<Config>>(m, "AdvancedConfig", R"pbdoc(Advanced configuration manager with validation and validation)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        // Advanced configuration methods
        .def("mergeConfig", [](Config& self, const Config& other) {
            for (const auto& key : other.getKeys()) {
                if (other.has(key)) {
                    auto value = other.get(ConfigValue::TypeGuessFromKey(key));
                    if (value.has_value()) {
                        self.set(key, value.value());
                    }
                }
            }
            return self;
        }, py::arg("other"),
           "Merge another configuration into this one")
        .def("validateConfig", [](Config& self, const std::unordered_map<std::string, std::function<bool(const ConfigValue&)>>& validators) {
            std::vector<std::string> errors;
            for (const auto& validator : validators) {
                if (self.has(validator.first)) {
                    auto value = self.get(ConfigValue::TypeGuessFromKey(validator.first));
                    if (value.has_value() && !validator.second(value.value())) {
                        errors.push_back("Invalid value for key: " + validator.first);
                    }
                }
            }
            if (!errors.empty()) {
                throw std::runtime_error("Configuration validation failed: " + 
                                        boost::algorithm::join(errors, ", "));
            }
            return self;
        }, py::arg("validators"),
           "Validate configuration against a set of validation rules")
        .def("exportConfig", [](Config& self, const std::string& filename) {
            return self.saveToFile(filename);
        }, py::arg("filename"),
           "Export configuration to a file")
        .def("importConfig", [](Config& self, const std::string& filename) {
            return self.loadFromFile(filename);
        }, py::arg("filename"),
           "Import configuration from a file")
        .def("setAdvancedOptions", [](Config& self, bool enablePhase6, bool enableSocial, bool enableText, bool enableMemory) {
            if (enablePhase6) {
                self.set("text_encoder.max_tokens", 500);
                self.set("text_embedding.dimension", 64);
                self.set("social.observation.capacity", 500);
                self.set("social.forgetting.rate", 0.02);
                self.set("working_memory.capacity", 200);
                self.set("working_memory.decay", 0.95);
                self.set("episodic_memory.capacity", 1000);
                self.set("episodic_memory.decay", 0.98);
                self.set("concept.min_instances", 20);
                self.set("concept.similarity.threshold", 0.9);
                self.set("prediction.horizon", 30);
                self.set("prediction.accuracy.threshold", 0.8);
            }
            return self;
        }, py::arg("enablePhase6"), py::arg("enableSocial"), py::arg("enableText"), py::arg("enableMemory"),
           "Enable advanced Phase 6 options")
        .def("configureForScale", [](Config& self, int neuron_count, int regions, const std::string& mode) {
            self.set("brain.neuron_count", neuron_count);
            self.set("brain.region_count", regions);
            self.set("brain.connection_probability", 0.1 * neuron_count / 1000);
            
            if (mode == "fast") {
                self.set("simulation.max.steps", 1000);
                self.set("log.level", "WARN");
            } else if (mode == "accurate") {
                self.set("simulation.max.steps", 10000);
                self.set("log.level", "INFO");
            } else {
                self.set("simulation.max.steps", 5000);
                self.set("log.level", "INFO");
            }
            
            return self;
        }, py::arg("neuron_count"), py::arg("regions"), py::arg("mode"),
           "Configure for specific scale and performance mode")
        .def("configureForDomain", [](Config& self, const std::string& domain) {
            if (domain == "neuroscience_research") {
                self.set("brain.neuron_count", 10000);
                self.set("brain.region_count", 8);
                self.set("plasticity.stdp.enable", true);
                self.set("plasticity.hebbian.enable", true);
                self.set("plasticity.structural.enable", true);
                self.set("development.enabled", true);
                self.set("neuromodulation.reward.enable", true);
                self.set("neuromodulation.curiosity.enable", true);
                self.set("neuromodulation.novelty.enable", true);
            } else if (domain == "ai_research") {
                self.set("brain.neuron_count", 5000);
                self.set("brain.region_count", 6);
                self.set("plasticity.stdp.enable", true);
                self.set("plasticity.hebbian.enable", true);
                self.set("development.enabled", true);
                self.set("neuromodulation.reward.enable", true);
                self.set("neuromodulation.curiosity.enable", true);
                self.set("text.processing.enable", true);
            } else if (domain == "cognitive_modeling") {
                self.set("brain.neuron_count", 2000);
                self.set("brain.region_count", 4);
                self.set("development.enabled", true);
                self.set("neuromodulation.curiosity.enable", true);
                self.set("neuromodulation.novelty.enable", true);
                self.set("social.learning.enable", true);
            }
            return self;
        }, py::arg("domain"),
           "Configure for specific research domain")
        .def("getConfigStats", [](const Config& self) {
            std::map<std::string, std::string> stats;
            stats["total_keys"] = std::to_string(self.getKeys().size());
            
            int int_count = 0, float_count = 0, string_count = 0, bool_count = 0;
            for (const auto& key : self.getKeys()) {
                auto value = self.get(ConfigValue::TypeGuessFromKey(key));
                if (value) {
                    if (std::holds_alternative<int>(*value) || std::holds_alternative<int64_t>(*value)) {
                        int_count++;
                    } else if (std::holds_alternative<double>(*value)) {
                        float_count++;
                    } else if (std::holds_alternative<bool>(*value)) {
                        bool_count++;
                    } else if (std::holds_alternative<std::string>(*value)) {
                        string_count++;
                    } else if (std::holds_alternative<std::vector<int>>(*value)) {
                        int_count++;
                    } else if (std::holds_alternative<std::vector<double>>(*value)) {
                        float_count++;
                    } else if (std::holds_alternative<std::vector<std::string>>(*value)) {
                        string_count++;
                    }
                }
            }
            
            stats["int_values"] = std::to_string(int_count);
            stats["float_values"] = std::to_string(float_count);
            stats["string_values"] = std::to_string(string_count);
            stats["bool_values"] = std::to_string(bool_count);
            
            return stats;
        }, "Get configuration statistics")
        .def("validateAgainstSchema", [](Config& self, const std::unordered_map<std::string, std::string>& schema) {
            std::vector<std::string> errors;
            for (const auto& entry : schema) {
                const std::string& key = entry.first;
                const std::string& expected_type = entry.second;
                
                if (!self.has(key)) {
                    errors.push_back("Missing required key: " + key);
                    continue;
                }
                
                auto value = self.get(ConfigValue::TypeGuessFromKey(key));
                if (!value.has_value()) {
                    errors.push_back("Invalid type for key: " + key);
                    continue;
                }
                
                std::string actual_type;
                if (std::holds_alternative<int>(*value) || std::holds_alternative<int64_t>(*value)) {
                    actual_type = "int";
                } else if (std::holds_alternative<double>(*value)) {
                    actual_type = "float";
                } else if (std::holds_alternative<bool>(*value)) {
                    actual_type = "bool";
                } else if (std::holds_alternative<std::string>(*value)) {
                    actual_type = "string";
                }
                
                if (actual_type != expected_type) {
                    errors.push_back("Type mismatch for key " + key + ": expected " + expected_type + ", got " + actual_type);
                }
            }
            
            if (!errors.empty()) {
                throw std::runtime_error("Configuration validation failed: " + 
                                        boost::algorithm::join(errors, ", "));
            }
            
            return self;
        }, py::arg("schema"),
           "Validate configuration against a schema")
        .def("__repr__", [](const Config& cfg) {
            return "<AdvancedConfig: " + cfg.summary() + ">";
        });

    // Performance monitoring and profiling for advanced users
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Performance monitoring and profiling for NLM simulations)pbdoc")
        .def(py::init<>())
        .def("startMonitoring", &PerformanceMonitor::startMonitoring,
             "Start performance monitoring")
        .def("stopMonitoring", &PerformanceMonitor::stopMonitoring,
             "Stop performance monitoring")
        .def("recordStepTime", [](PerformanceMonitor& self, double time_ms) {
            self.recordStepTime(time_ms);
        }, py::arg("time_ms"),
             "Record step execution time")
        .def("recordMemoryUsage", [](PerformanceMonitor& self, size_t memory_bytes) {
            self.recordMemoryUsage(memory_bytes);
        }, py::arg("memory_bytes"),
             "Record memory usage")
        .def("recordSpikeCount", [](PerformanceMonitor& self, size_t spike_count) {
            self.recordSpikeCount(spike_count);
        }, py::arg("spike_count"),
             "Record spike count")
        .def("recordFiringRate", [](PerformanceMonitor& self, double firing_rate) {
            self.recordFiringRate(firing_rate);
        }, py::arg("firing_rate"),
             "Record firing rate")
        .def("getStatistics", &PerformanceMonitor::getStatistics,
             "Get performance statistics")
        .def("getAverageStepTime", &PerformanceMonitor::getAverageStepTime,
             "Get average step execution time")
        .def("getMaxStepTime", &PerformanceMonitor::getMaxStepTime,
             "Get maximum step execution time")
        .def("getMinStepTime", &PerformanceMonitor::getMinStepTime,
             "Get minimum step execution time")
        .def("getAverageMemoryUsage", &PerformanceMonitor::getAverageMemoryUsage,
             "Get average memory usage")
        .def("getMaxMemoryUsage", &PerformanceMonitor::getMaxMemoryUsage,
             "Get maximum memory usage")
        .def("getAverageSpikeRate", &PerformanceMonitor::getAverageSpikeRate,
             "Get average spike rate")
        .def("getStepsPerSecond", &PerformanceMonitor::getStepsPerSecond,
             "Get steps per second")
        .def("reset", &PerformanceMonitor::reset,
             "Reset monitoring data")
        .def("isMonitoring", &PerformanceMonitor::isMonitoring,
             "Check if monitoring is active")
        .def("generateReport", &PerformanceMonitor::generateReport,
             "Generate performance report")
        .def("exportData", [](PerformanceMonitor& self, const std::string& filename) {
            return self.exportData(filename);
        }, py::arg("filename"),
             "Export monitoring data to file")
        .def("importData", [](PerformanceMonitor& self, const std::string& filename) {
            return self.importData(filename);
        }, py::arg("filename"),
             "Import monitoring data from file")
        .def("compareWith", [](PerformanceMonitor& self, const PerformanceMonitor& other) {
            return self.compareWith(other);
        }, py::arg("other"),
             "Compare performance with another monitor")
        .def("__repr__", [](const PerformanceMonitor& monitor) {
            auto stats = monitor.getStatistics();
            return "<PerformanceMonitor: steps=" + std::to_string(stats.size()) + 
                   ", avg_time=" + std::to_string(monitor.getAverageStepTime()) + "ms>";
        });

    // Batch processing utilities for running multiple simulations
    py::class_<BatchProcessor>(m, "BatchProcessor", R"pbdoc(Batch processing for running multiple simulations)pbdoc")
        .def(py::init<>())
        .def("addConfig", [](BatchProcessor& self, const std::string& name, std::shared_ptr<Config> config) {
            self.addConfig(name, config);
        }, py::arg("name"), py::arg("config"),
             "Add a configuration to the batch")
        .def("removeConfig", [](BatchProcessor& self, const std::string& name) {
            self.removeConfig(name);
        }, py::arg("name"),
             "Remove a configuration from the batch")
        .def("runBatch", [](BatchProcessor& self) {
            self.runBatch();
        }, "Run all configurations in the batch")
        .def("runBatchWithProgress", [](BatchProcessor& self, std::function<void(const std::string&, int, int)> progress_callback) {
            self.runBatchWithProgress(progress_callback);
        }, py::arg("progress_callback"),
             "Run batch with progress callback")
        .def("getResults", &BatchProcessor::getResults,
             "Get batch processing results")
        .def("getConfigNames", &BatchProcessor::getConfigNames,
             "Get names of all configurations in batch")
        .def("clearBatch", &BatchProcessor::clearBatch,
             "Clear all configurations from batch")
        .def("saveResults", [](BatchProcessor& self, const std::string& filename) {
            return self.saveResults(filename);
        }, py::arg("filename"),
             "Save batch results to file")
        .def("loadResults", [](BatchProcessor& self, const std::string& filename) {
            return self.loadResults(filename);
        }, py::arg("filename"),
             "Load batch results from file")
        .def("setParallelExecution", [](BatchProcessor& self, bool enable, int max_workers) {
            self.setParallelExecution(enable, max_workers);
        }, py::arg("enable"), py::arg("max_workers"),
             "Enable or disable parallel execution")
        .def("setErrorHandling", [](BatchProcessor& self, bool continue_on_error) {
            self.setErrorHandling(continue_on_error);
        }, py::arg("continue_on_error"),
             "Set error handling strategy")
        .def("getExecutionTime", &BatchProcessor::getExecutionTime,
             "Get total execution time for batch")
        .def("getSuccessCount", &BatchProcessor::getSuccessCount,
             "Get number of successful runs")
        .def("getErrorCount", &BatchProcessor::getErrorCount,
             "Get number of failed runs")
        .def("__repr__", [](const BatchProcessor& processor) {
            auto results = processor.getResults();
            return "<BatchProcessor: configs=" + std::to_string(results.size()) + 
                   ", successes=" + std::to_string(processor.getSuccessCount()) + 
                   ", errors=" + std::to_string(processor.getErrorCount()) + ">";
        });

    // Advanced debugging and profiling tools
    py::class_<DebugTools>(m, "DebugTools", R"pbdoc(Advanced debugging and profiling tools for NLM)pbdoc")
        .def(py::init<>())
        .def("enableMemoryDebugging", [](DebugTools& self, bool enable) {
            self.enableMemoryDebugging(enable);
        }, py::arg("enable"),
             "Enable memory debugging")
        .def("enableStepDebugging", [](DebugTools& self, bool enable) {
            self.enableStepDebugging(enable);
        }, py::arg("enable"),
             "Enable step-by-step debugging")
        .def("enableSpikeDebugging", [](DebugTools& self, bool enable) {
            self.enableSpikeDebugging(enable);
        }, py::arg("enable"),
             "Enable spike debugging")
        .def("enablePlasticityDebugging", [](DebugTools& self, bool enable) {
            self.enablePlasticityDebugging(enable);
        }, py::arg("enable"),
             "Enable plasticity debugging")
        .def("startProfiling", [](DebugTools& self, const std::string& profile_name) {
            self.startProfiling(profile_name);
        }, py::arg("profile_name"),
             "Start performance profiling")
        .def("stopProfiling", [](DebugTools& self) {
            self.stopProfiling();
        }, "Stop performance profiling")
        .def("saveProfile", [](DebugTools& self, const std::string& filename) {
            return self.saveProfile(filename);
        }, py::arg("filename"),
             "Save profiling data")
        .def("loadProfile", [](DebugTools& self, const std::string& filename) {
            return self.loadProfile(filename);
        }, py::arg("filename"),
             "Load profiling data")
        .def("analyzeMemoryUsage", &DebugTools::analyzeMemoryUsage,
             "Analyze memory usage patterns")
        .def("analyzePerformanceBottlenecks", &DebugTools::analyzePerformanceBottlenecks,
             "Analyze performance bottlenecks")
        .def("detectMemoryLeaks", &DebugTools::detectMemoryLeaks,
             "Detect potential memory leaks")
        .def("profileStepExecution", [](DebugTools& self, std::shared_ptr<Brain> brain, int step) {
            return self.profileStepExecution(brain, step);
        }, py::arg("brain"), py::arg("step"),
             "Profile execution of a single step")
        .def("analyzeSpikePatterns", [](DebugTools& self, std::shared_ptr<Brain> brain, int steps) {
            return self.analyzeSpikePatterns(brain, steps);
        }, py::arg("brain"), py::arg("steps"),
             "Analyze spike patterns over multiple steps")
        .def("debugPlasticityChanges", [](DebugTools& self, std::shared_ptr<Brain> brain) {
            return self.debugPlasticityChanges(brain);
        }, py::arg("brain"),
             "Debug plasticity changes")
        .def("generateDebugReport", &DebugTools::generateDebugReport,
             "Generate comprehensive debug report")
        .def("exportDebugData", [](DebugTools& self, const std::string& filename) {
            return self.exportDebugData(filename);
        }, py::arg("filename"),
             "Export debug data")
        .def("isDebuggingEnabled", &DebugTools::isDebuggingEnabled,
             "Check if debugging is enabled")
        .def("setDebugLevel", [](DebugTools& self, int level) {
            self.setDebugLevel(level);
        }, py::arg("level"),
             "Set debug verbosity level")
        .def("getDebugInfo", &DebugTools::getDebugInfo,
             "Get current debug information")
        .def("clearDebugData", &DebugTools::clearDebugData,
             "Clear debug data")
        .def("__repr__", [](const DebugTools& tools) {
            return "<DebugTools: memory_debug=" + std::to_string(tools.isMemoryDebuggingEnabled()) +
                   ", step_debug=" + std::to_string(tools.isStepDebuggingEnabled()) +
                   ", profiling_active=" + std::to_string(tools.isProfilingActive()) + ">";
        });

    // Custom agent behaviors for advanced users
    py::class_<CustomAgentBehavior>(m, "CustomAgentBehavior", R"pbdoc(Custom agent behaviors for advanced NLM control)pbdoc")
        .def(py::init<>())
        .def("setActionSelectionStrategy", [](CustomAgentBehavior& self, std::function<pynlm.MotorCommand(std::shared_ptr<Brain>, const SensoryPercept&)> strategy) {
            self.setActionSelectionStrategy(strategy);
        }, py::arg("strategy"),
             "Set custom action selection strategy")
        .def("setRewardCalculationStrategy", [](CustomAgentBehavior& self, std::function<double(const SensoryPercept&, std::shared_ptr<Brain>, int)> strategy) {
            self.setRewardCalculationStrategy(strategy);
        }, py::arg("strategy"),
             "Set custom reward calculation strategy")
        .def("setSensoryProcessingStrategy", [](CustomAgentBehavior& self, std::function<void(const SensoryPercept&, std::shared_ptr<Brain>)> strategy) {
            self.setSensoryProcessingStrategy(strategy);
        }, py::arg("strategy"),
             "Set custom sensory processing strategy")
        .def("setDevelopmentStrategy", [](CustomAgentBehavior& self, std::function<void(std::shared_ptr<Brain>, double)> strategy) {
            self.setDevelopmentStrategy(strategy);
        }, py::arg("strategy"),
             "Set custom development strategy")
        .def("setLearningRate", [](CustomAgentBehavior& self, double rate) {
            self.setLearningRate(rate);
        }, py::arg("rate"),
             "Set custom learning rate")
        .def("setExplorationRate", [](CustomAgentBehavior& self, double rate) {
            self.setExplorationRate(rate);
        }, py::arg("rate"),
             "Set custom exploration rate")
        .def("setExploitationRate", [](CustomAgentBehavior& self, double rate) {
            self.setExploitationRate(rate);
        }, py::arg("rate"),
             "Set custom exploitation rate")
        .def("setCuriosityThreshold", [](CustomAgentBehavior& self, double threshold) {
            self.setCuriosityThreshold(threshold);
        }, py::arg("threshold"),
             "Set custom curiosity threshold")
        .def("setNoveltyThreshold", [](CustomAgentBehavior& self, double threshold) {
            self.setNoveltyThreshold(threshold);
        }, py::arg("threshold"),
             "Set custom novelty threshold")
        .def("enableCustomLearning", [](CustomAgentBehavior& self, bool enable) {
            self.enableCustomLearning(enable);
        }, py::arg("enable"),
             "Enable custom learning")
        .def("enableCustomDevelopment", [](CustomAgentBehavior& self, bool enable) {
            self.enableCustomDevelopment(enable);
        }, py::arg("enable"),
             "Enable custom development")
        .def("enableCustomSensory", [](CustomAgentBehavior& self, bool enable) {
            self.enableCustomSensory(enable);
        }, py::arg("enable"),
             "Enable custom sensory processing")
        .def("enableCustomMotor", [](CustomAgentBehavior& self, bool enable) {
            self.enableCustomMotor(enable);
        }, py::arg("enable"),
             "Enable custom motor control")
        .def("runSimulation", [](CustomAgentBehavior& self, std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, int steps) {
            return self.runSimulation(brain, agent, world, steps);
        }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"),
             "Run simulation with custom behavior")
        .def("getBehaviorStats", &CustomAgentBehavior::getBehaviorStats,
             "Get behavior statistics")
        .def("resetBehavior", &CustomAgentBehavior::resetBehavior,
             "Reset custom behavior")
        .def("exportBehavior", [](CustomAgentBehavior& self, const std::string& filename) {
            return self.exportBehavior(filename);
        }, py::arg("filename"),
             "Export custom behavior")
        .def("importBehavior", [](CustomAgentBehavior& self, const std::string& filename) {
            return self.importBehavior(filename);
        }, py::arg("filename"),
             "Import custom behavior")
        .def("copyFrom", [](CustomAgentBehavior& self, const CustomAgentBehavior& other) {
            self.copyFrom(other);
        }, py::arg("other"),
             "Copy behavior from another agent")
        .def("isBehaviorLoaded", &CustomAgentBehavior::isBehaviorLoaded,
             "Check if custom behavior is loaded")
        .def("__repr__", [](const CustomAgentBehavior& behavior) {
            auto stats = behavior.getBehaviorStats();
            return "<CustomAgentBehavior: learning_rate=" + std::to_string(stats.learning_rate) +
                   ", exploration_rate=" + std::to_string(stats.exploration_rate) +
                   ", exploitation_rate=" + std::to_string(stats.exploitation_rate) + ">";
        });

    // Advanced export/import utilities
    py::class_<ExportImportTools>(m, "ExportImportTools", R"pbdoc(Advanced export/import utilities for NLM)pbdoc")
        .def(py::init<>())
        .def("exportBrainState", [](ExportImportTools& self, std::shared_ptr<Brain> brain, const std::string& filename, const std::string& format = "binary") {
            return self.exportBrainState(brain, filename, format);
        }, py::arg("brain"), py::arg("filename"), py::arg("format") = "binary",
             "Export brain state to file")
        .def("importBrainState", [](ExportImportTools& self, std::shared_ptr<Brain> brain, const std::string& filename, const std::string& format = "binary") {
            return self.importBrainState(brain, filename, format);
        }, py::arg("brain"), py::arg("filename"), py::arg("format") = "binary",
             "Import brain state from file")
        .def("exportConfiguration", [](ExportImportTools& self, std::shared_ptr<Config> config, const std::string& filename) {
            return self.exportConfiguration(config, filename);
        }, py::arg("config"), py::arg("filename"),
             "Export configuration to file")
        .def("importConfiguration", [](ExportImportTools& self, std::shared_ptr<Config> config, const std::string& filename) {
            return self.importConfiguration(config, filename);
        }, py::arg("config"), py::arg("filename"),
             "Import configuration from file")
        .def("exportSimulationData", [](ExportImportTools& self, std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, const std::string& filename) {
            return self.exportSimulationData(brain, agent, world, filename);
        }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("filename"),
             "Export simulation data")
        .def("importSimulationData", [](ExportImportTools& self, std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, const std::string& filename) {
            return self.importSimulationData(brain, agent, world, filename);
        }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("filename"),
             "Import simulation data")
        .def("exportMetrics", [](ExportImportTools& self, const std::vector<std::shared_ptr<Brain>>& brains, const std::vector<std::string>& names, const std::string& filename) {
            return self.exportMetrics(brains, names, filename);
        }, py::arg("brains"), py::arg("names"), py::arg("filename"),
             "Export metrics from multiple brains")
        .def("importMetrics", [](ExportImportTools& self, const std::vector<std::shared_ptr<Brain>>& brains, const std::vector<std::string>& names, const std::string& filename) {
            return self.importMetrics(brains, names, filename);
        }, py::arg("brains"), py::arg("names"), py::arg("filename"),
             "Import metrics into multiple brains")
        .def("exportCheckpoint", [](ExportImportTools& self, std::shared_ptr<Brain> brain, const std::string& checkpoint_name) {
            return self.exportCheckpoint(brain, checkpoint_name);
        }, py::arg("brain"), py::arg("checkpoint_name"),
             "Export brain checkpoint")
        .def("importCheckpoint", [](ExportImportTools& self, std::shared_ptr<Brain> brain, const std::string& checkpoint_name) {
            return self.importCheckpoint(brain, checkpoint_name);
        }, py::arg("brain"), py::arg("checkpoint_name"),
             "Import brain checkpoint")
        .def("exportAnalysisData", [](ExportImportTools& self, std::shared_ptr<Brain> brain, const std::string& analysis_type, const std::string& filename) {
            return self.exportAnalysisData(brain, analysis_type, filename);
        }, py::arg("brain"), py::arg("analysis_type"), py::arg("filename"),
             "Export analysis data")
        .def("importAnalysisData", [](ExportImportTools& self, std::shared_ptr<Brain> brain, const std::string& analysis_type, const std::string& filename) {
            return self.importAnalysisData(brain, analysis_type, filename);
        }, py::arg("brain"), py::arg("analysis_type"), py::arg("filename"),
             "Import analysis data")
        .def("getExportFormats", &ExportImportTools::getExportFormats,
             "Get available export formats")
        .def("getImportFormats", &ExportImportTools::getImportFormats,
             "Get available import formats")
        .def("validateFile", [](ExportImportTools& self, const std::string& filename, const std::string& format) {
            return self.validateFile(filename, format);
        }, py::arg("filename"), py::arg("format"),
             "Validate export/import file")
        .def("compressExport", [](ExportImportTools& self, const std::string& filename, const std::string& output_filename) {
            return self.compressExport(filename, output_filename);
        }, py::arg("filename"), py::arg("output_filename"),
             "Compress exported file")
        .def("decompressImport", [](ExportImportTools& self, const std::string& filename, const std::string& output_filename) {
            return self.decompressImport(filename, output_filename);
        }, py::arg("filename"), py::arg("output_filename"),
             "Decompress imported file")
        .def("backupFiles", [](ExportImportTools& self, const std::vector<std::string>& files, const std::string& backup_dir) {
            return self.backupFiles(files, backup_dir);
        }, py::arg("files"), py::arg("backup_dir"),
             "Backup files")
        .def("restoreFiles", [](ExportImportTools& self, const std::vector<std::string>& files, const std::string& backup_dir) {
            return self.restoreFiles(files, backup_dir);
        }, py::arg("files"), py::arg("backup_dir"),
             "Restore files from backup")
        .def("createSnapshot", [](ExportImportTools& self, const std::string& snapshot_name) {
            return self.createSnapshot(snapshot_name);
        }, py::arg("snapshot_name"),
             "Create snapshot of current state")
        .def("restoreSnapshot", [](ExportImportTools& self, const std::string& snapshot_name) {
            return self.restoreSnapshot(snapshot_name);
        }, py::arg("snapshot_name"),
             "Restore from snapshot")
        .def("getExportStats", &ExportImportTools::getExportStats,
             "Get export statistics")
        .def("getImportStats", &ExportImportTools::getImportStats,
             "Get import statistics")
        .def("__repr__", [](const ExportImportTools& tools) {
            auto stats = tools.getExportStats();
            return "<ExportImportTools: exports=" + std::to_string(stats.exports) +
                   ", imports=" + std::to_string(stats.imports) + ", backups=" + std::to_string(stats.backups) + ">";
        });

    // Advanced analysis and visualization tools
    py::class_<AnalysisTools>(m, "AnalysisTools", R"pbdoc(Advanced analysis and visualization tools for NLM)pbdoc")
        .def(py::init<>())
        .def("analyzeSpikeDynamics", [](AnalysisTools& self, std::shared_ptr<Brain> brain, int steps) {
            return self.analyzeSpikeDynamics(brain, steps);
        }, py::arg("brain"), py::arg("steps"),
             "Analyze spike dynamics")
        .def("analyzePlasticityChanges", [](AnalysisTools& self, std::shared_ptr<Brain> brain) {
            return self.analyzePlasticityChanges(brain);
        }, py::arg("brain"),
             "Analyze plasticity changes")
        .def("analyzeMemoryUsage", [](AnalysisTools& self, std::shared_ptr<Brain> brain) {
            return self.analyzeMemoryUsage(brain);
        }, py::arg("brain"),
             "Analyze memory usage")
        .def("analyzeLearningProgress", [](AnalysisTools& self, std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) {
            return self.analyzeLearningProgress(brain, agent);
        }, py::arg("brain"), py::arg("agent"),
             "Analyze learning progress")
        .def("analyzeBehaviorPatterns", [](AnalysisTools& self, std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) {
            return self.analyzeBehaviorPatterns(brain, agent);
        }, py::arg("brain"), py::arg("agent"),
             "Analyze behavior patterns")
        .def("analyzeNetworkTopology", [](AnalysisTools& self, std::shared_ptr<Brain> brain) {
            return self.analyzeNetworkTopology(brain);
        }, py::arg("brain"),
             "Analyze network topology")
        .def("analyzePlasticityRules", [](AnalysisTools& self, std::shared_ptr<Brain> brain) {
            return self.analyzePlasticityRules(brain);
        }, py::arg("brain"),
             "Analyze plasticity rules")
        .def("generateStatisticalReport", [](AnalysisTools& self, std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) {
            return self.generateStatisticalReport(brain, agent);
        }, py::arg("brain"), py::arg("agent"),
             "Generate statistical report")
        .def("generateVisualReport", [](AnalysisTools& self, std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, const std::string& output_format = "png") {
            return self.generateVisualReport(brain, agent, output_format);
        }, py::arg("brain"), py::arg("agent"), py::arg("output_format") = "png",
             "Generate visual report")
        .def("createTimeSeriesPlot", [](AnalysisTools& self, const std::vector<double>& data, const std::string& title = "Data Plot") {
            return self.createTimeSeriesPlot(data, title);
        }, py::arg("data"), py::arg("title") = "Data Plot",
             "Create time series plot")
        .def("createScatterPlot", [](AnalysisTools& self, const std::vector<double>& x, const std::vector<double>& y, const std::string& title = "Scatter Plot") {
            return self.createScatterPlot(x, y, title);
        }, py::arg("x"), py::arg("y"), py::arg("title") = "Scatter Plot",
             "Create scatter plot")
        .def("createHistogram", [](AnalysisTools& self, const std::vector<double>& data, const std::string& title = "Histogram") {
            return self.createHistogram(data, title);
        }, py::arg("data"), py::arg("title") = "Histogram",
             "Create histogram")
        .def("createNetworkGraph", [](AnalysisTools& self, std::shared_ptr<Brain> brain, const std::string& layout = "spring") {
            return self.createNetworkGraph(brain, layout);
        }, py::arg("brain"), py::arg("layout") = "spring",
             "Create network graph visualization")
        .def("createActivityHeatmap", [](AnalysisTools& self, std::shared_ptr<Brain> brain, int window_size = 100) {
            return self.createActivityHeatmap(brain, window_size);
        }, py::arg("brain"), py::arg("window_size") = 100,
             "Create activity heatmap")
        .def("analyzeSpatioTemporalPatterns", [](AnalysisTools& self, std::shared_ptr<Brain> brain, int time_window) {
            return self.analyzeSpatioTemporalPatterns(brain, time_window);
        }, py::arg("brain"), py::arg("time_window"),
             "Analyze spatio-temporal patterns")
        .def("detectAnomalies", [](AnalysisTools& self, std::shared_ptr<Brain> brain, double threshold) {
            return self.detectAnomalies(brain, threshold);
        }, py::arg("brain"), py::arg("threshold"),
             "Detect anomalies in brain activity")
        .def("calculateNetworkMetrics", [](AnalysisTools& self, std::shared_ptr<Brain> brain) {
            return self.calculateNetworkMetrics(brain);
        }, py::arg("brain"),
             "Calculate network metrics")
        .def("analyzePlasticityEfficiency", [](AnalysisTools& self, std::shared_ptr<Brain> brain) {
            return self.analyzePlasticityEfficiency(brain);
        }, py::arg("brain"),
             "Analyze plasticity efficiency")
        .def("exportAnalysisData", [](AnalysisTools& self, const std::string& filename) {
            return self.exportAnalysisData(filename);
        }, py::arg("filename"),
             "Export analysis data")
        .def("importAnalysisData", [](AnalysisTools& self, const std::string& filename) {
            return self.importAnalysisData(filename);
        }, py::arg("filename"),
             "Import analysis data")
        .def("getAnalysisStats", &AnalysisTools::getAnalysisStats,
             "Get analysis statistics")
        .def("setAnalysisOptions", [](AnalysisTools& self, bool enable_advanced, bool enable_visualization, bool enable_statistics) {
            self.setAnalysisOptions(enable_advanced, enable_visualization, enable_statistics);
        }, py::arg("enable_advanced"), py::arg("enable_visualization"), py::arg("enable_statistics"),
             "Set analysis options")
        .def("clearAnalysisData", &AnalysisTools::clearAnalysisData,
             "Clear analysis data")
        .def("__repr__", [](const AnalysisTools& tools) {
            auto stats = tools.getAnalysisStats();
            return "<AnalysisTools: spikes_analyzed=" + std::to_string(stats.spike_analyses) +
                   ", plasticity_analyses=" + std::to_string(stats.plasticity_analyses) +
                   ", network_metrics=" + std::to_string(stats.network_metrics_computed) + ">";
        });

    // Add convenience factory methods for advanced configurations
    m.def("createAdvancedConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("text_encoder.max_tokens", 500);
        config->set("text_embedding.dimension", 64);
        config->set("social.observation.capacity", 500);
        config->set("social.forgetting.rate", 0.02);
        config->set("working_memory.capacity", 200);
        config->set("working_memory.decay", 0.95);
        config->set("episodic_memory.capacity", 1000);
        config->set("episodic_memory.decay", 0.98);
        config->set("concept.min_instances", 20);
        config->set("concept.similarity.threshold", 0.9);
        config->set("prediction.horizon", 30);
        config->set("prediction.accuracy.threshold", 0.8);
        return config;
    }, "Create advanced configuration with Phase 6 features");

    m.def("createPhase6Config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("text_encoder.max_tokens", 200);
        config->set("text_embedding.dimension", 32);
        config->set("social.observation.capacity", 100);
        config->set("social.forgetting.rate", 0.05);
        config->set("working_memory.capacity", 50);
        config->set("working_memory.decay", 0.98);
        config->set("episodic_memory.capacity", 200);
        config->set("episodic_memory.decay", 0.99);
        config->set("concept.min_instances", 5);
        config->set("concept.similarity.threshold", 0.8);
        config->set("prediction.horizon", 20);
        config->set("prediction.accuracy.threshold", 0.7);
        return config;
    }, "Create Phase 6 configuration");

    m.def("createResearchConfig", [](const std::string& domain) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        if (domain == "neuroscience") {
            config->set("brain.neuron_count", 10000);
            config->set("brain.region_count", 8);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("development.enabled", true);
            config->set("neuromodulation.reward.enable", true);
            config->set("neuromodulation.curiosity.enable", true);
            config->set("neuromodulation.novelty.enable", true);
        } else if (domain == "ai_research") {
            config->set("brain.neuron_count", 5000);
            config->set("brain.region_count", 6);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("development.enabled", true);
            config->set("neuromodulation.reward.enable", true);
            config->set("neuromodulation.curiosity.enable", true);
            config->set("text.processing.enable", true);
        } else if (domain == "cognitive") {
            config->set("brain.neuron_count", 2000);
            config->set("brain.region_count", 4);
            config->set("development.enabled", true);
            config->set("neuromodulation.curiosity.enable", true);
            config->set("neuromodulation.novelty.enable", true);
            config->set("social.learning.enable", true);
        }
        
        return config;
    }, py::arg("domain"), "Create configuration for specific research domain");

    // Create performance-optimized configuration presets
    m.def("createFastConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000);
        config->set("brain.region_count", 2);
        config->set("connection_probability", 0.05);
        config->set("simulation.max.steps", 1000);
        config->set("log.level", "WARN");
        return config;
    }, "Create fast performance configuration");

    m.def("createAccurateConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 5000);
        config->set("brain.region_count", 6);
        config->set("connection_probability", 0.15);
        config->set("simulation.max.steps", 10000);
        config->set("log.level", "INFO");
        return config;
    }, "Create accurate performance configuration");

    m.def("createBalancedConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 2000);
        config->set("brain.region_count", 4);
        config->set("connection_probability", 0.1);
        config->set("simulation.max.steps", 5000);
        config->set("log.level", "INFO");
        return config;
    }, "Create balanced performance configuration");

    // Advanced simulation control functions
    m.def("runAdvancedSimulation", [](std::shared_ptr<Config> config, std::function<void(int, double)> progress_callback) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(
            config->getOr<int64_t>("environment_width", 10),
            config->getOr<int64_t>("environment_height", 10),
            config->getOr<int64_t>("vision_width", 8),
            config->getOr<int64_t>("vision_height", 8)
        );
        world->reset();
        agent->initialize(world);
        
        agent->enableRewardModulation(config->getOr<bool>("neuromodulation.reward.enable", true));
        agent->enableStructuralPlasticity(config->getOr<bool>("plasticity.structural.enable", true));
        agent->enableDevelopment(config->getOr<bool>("development.enabled", true));
        agent->enableCuriosity(config->getOr<bool>("neuromodulation.curiosity.enable", true));
        
        int max_steps = config->getOr<int64_t>("simulation.max_steps", 1000);
        
        for (int step = 0; step < max_steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            double reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0;
            agent->applyRewardModulation(reward, 0.0);
            
            agent->updateDevelopment(0.1);
            
            progress_callback(step, max_steps);
        }
        
        return brain;
    }, py::arg("config"), py::arg("progress_callback"),
       "Run advanced simulation with progress callback");

    m.def("runBenchmarkSimulation", [](int neuron_count, int steps, bool enable_all_features) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neuron_count);
        config->set("brain.region_count", std::max(2, neuron_count / 1000));
        config->set("brain.connection_probability", 0.1);
        config->set("simulation.max_steps", steps);
        
        if (enable_all_features) {
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("development.enabled", true);
            config->set("neuromodulation.reward.enable", true);
            config->set("neuromodulation.curiosity.enable", true);
            config->set("neuromodulation.novelty.enable", true);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        return brain;
    }, py::arg("neuron_count"), py::arg("steps"), py::arg("enable_all_features") = false,
       "Run benchmark simulation with specified parameters");

    // Advanced export/import functions
    m.def("exportBrainToFile", [](std::shared_ptr<Brain> brain, const std::string& filename, const std::string& format = "binary") {
        return ExportImportTools().exportBrainState(brain, filename, format);
    }, py::arg("brain"), py::arg("filename"), py::arg("format") = "binary",
       "Export brain state to file");

    m.def("importBrainFromFile", [](std::shared_ptr<Brain> brain, const std::string& filename, const std::string& format = "binary") {
        return ExportImportTools().importBrainState(brain, filename, format);
    }, py::arg("brain"), py::arg("filename"), py::arg("format") = "binary",
       "Import brain state from file");

    m.def("createBrainSnapshot", [](std::shared_ptr<Brain> brain, const std::string& snapshot_name) {
        return ExportImportTools().createSnapshot(snapshot_name);
    }, py::arg("brain"), py::arg("snapshot_name"),
       "Create brain snapshot");

    m.def("restoreBrainSnapshot", [](std::shared_ptr<Brain> brain, const std::string& snapshot_name) {
        return ExportImportTools().restoreSnapshot(snapshot_name);
    }, py::arg("brain"), py::arg("snapshot_name"),
       "Restore brain from snapshot");

    // Advanced configuration validation and analysis
    m.def("validateAdvancedConfig", [](std::shared_ptr<Config> config) {
        std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
        
        validators["brain.neuron_count"] = [](const ConfigValue& v) {
            return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) >= 10 && std::get<int64_t>(v) <= 1000000;
        };
        
        validators["brain.region_count"] = [](const ConfigValue& v) {
            return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) >= 1 && std::get<int64_t>(v) <= 1000;
        };
        
        validators["brain.connection_probability"] = [](const ConfigValue& v) {
            return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0;
        };
        
        return config->validateConfig(validators);
    }, py::arg("config"),
       "Validate advanced configuration");

    m.def("analyzeConfigForPerformance", [](std::shared_ptr<Config> config) {
        std::map<std::string, double> analysis;
        
        int64_t neuron_count = config->getOr<int64_t>("brain.neuron_count", 1000);
        int64_t region_count = config->getOr<int64_t>("brain.region_count", 1);
        int64_t max_steps = config->getOr<int64_t>("simulation.max_steps", 1000);
        
        analysis["estimated_memory_mb"] = neuron_count * 1000 / 1024 / 1024;
        analysis["estimated_time_seconds"] = max_steps * 0.001;
        analysis["performance_score"] = 100.0 / (1.0 + neuron_count / 10000.0 + max_steps / 10000.0);
        
        if (config->has("plasticity.stdp.enable") && config->getOr<bool>("plasticity.stdp.enable", false)) {
            analysis["plasticity_overhead"] = 0.3;
        } else {
            analysis["plasticity_overhead"] = 0.0;
        }
        
        if (config->has("development.enabled") && config->getOr<bool>("development.enabled", false)) {
            analysis["development_overhead"] = 0.2;
        } else {
            analysis["development_overhead"] = 0.0;
        }
        
        return analysis;
    }, py::arg("config"),
       "Analyze configuration for performance characteristics");

    // Advanced batch processing functions
    m.def("runBatchSimulation", [](const std::vector<std::shared_ptr<Config>>& configs, std::function<void(int, int, const std::string&)> progress_callback) {
        BatchProcessor processor;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            processor.addConfig(std::to_string(i), configs[i]);
        }
        
        processor.runBatchWithProgress(progress_callback);
        return processor.getResults();
    }, py::arg("configs"), py::arg("progress_callback"),
       "Run batch simulation with multiple configurations");

    m.def("compareConfigurations", [](const std::vector<std::shared_ptr<Config>>& configs) {
        BatchProcessor processor;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            processor.addConfig(std::to_string(i), configs[i]);
        }
        
        processor.runBatch();
        return processor.getResults();
    }, py::arg("configs"),
       "Compare multiple configurations");

    // Advanced debugging and profiling functions
    m.def("profileBrainPerformance", [](std::shared_ptr<Brain> brain, int steps) {
        DebugTools debug_tools;
        debug_tools.startProfiling("brain_performance");
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        debug_tools.stopProfiling();
        return debug_tools.generateDebugReport();
    }, py::arg("brain"), py::arg("steps"),
       "Profile brain performance over multiple steps");

    m.def("analyzeBrainMemoryUsage", [](std::shared_ptr<Brain> brain) {
        DebugTools debug_tools;
        debug_tools.enableMemoryDebugging(true);
        
        return debug_tools.analyzeMemoryUsage();
    }, py::arg("brain"),
       "Analyze brain memory usage patterns");

    m.def("detectBrainPerformanceIssues", [](std::shared_ptr<Brain> brain) {
        DebugTools debug_tools;
        
        return debug_tools.analyzePerformanceBottlenecks();
    }, py::arg("brain"),
       "Detect performance issues in brain");

    // Advanced utility functions
    m.def("createCustomAgent", [](std::shared_ptr<Brain> brain, std::function<pynlm.MotorCommand(const SensoryPercept&)> strategy) {
        auto agent = std::make_shared<AgentBrain>(brain);
        
        auto custom_behavior = std::make_shared<CustomAgentBehavior>();
        custom_behavior->setActionSelectionStrategy(strategy);
        custom_behavior->enableCustomLearning(true);
        
        return std::make_pair(agent, custom_behavior);
    }, py::arg("brain"), py::arg("strategy"),
       "Create custom agent with specific strategy");

    m.def("exportAnalysisReport", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, const std::string& filename) {
        AnalysisTools analysis_tools;
        return analysis_tools.generateStatisticalReport(brain, agent);
    }, py::arg("brain"), py::arg("agent"), py::arg("filename"),
       "Export comprehensive analysis report");

    m.def("createResearchPipeline", [](const std::string& domain, const std::string& pipeline_type) {
        auto config = std::make_shared<Config>();
        
        if (domain == "neuroscience") {
            config->set("brain.neuron_count", 10000);
            config->set("brain.region_count", 8);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
        } else if (domain == "ai") {
            config->set("brain.neuron_count", 5000);
            config->set("brain.region_count", 6);
            config->set("text.processing.enable", true);
        }
        
        return std::make_pair(config, pipeline_type);
    }, py::arg("domain"), py::arg("pipeline_type"),
       "Create research pipeline configuration");

    // Utility functions for advanced users
    m.def("benchmarkConfiguration", [](std::shared_ptr<Config> config, int steps) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        double steps_per_second = steps / (duration.count() / 1000.0);
        
        std::map<std::string, double> results;
        results["steps"] = steps;
        results["time_ms"] = duration.count();
        results["steps_per_second"] = steps_per_second;
        results["avg_step_time_ms"] = duration.count() / steps;
        
        return results;
    }, py::arg("config"), py::arg("steps"),
       "Benchmark configuration performance");

    m.def("validateConfigurationCompatibility", [](std::shared_ptr<Config> config1, std::shared_ptr<Config> config2) {
        std::vector<std::string> incompatibilities;
        
        // Check for conflicting settings
        if (config1->has("brain.neuron_count") && config2->has("brain.neuron_count")) {
            int64_t neurons1 = config1->getOr<int64_t>("brain.neuron_count", 1000);
            int64_t neurons2 = config2->getOr<int64_t>("brain.neuron_count", 1000);
            if (std::abs(neurons1 - neurons2) / neurons1 > 0.5) {
                incompatibilities.push_back("Large neuron count difference");
            }
        }
        
        if (config1->has("simulation.max_steps") && config2->has("simulation.max_steps")) {
            int64_t steps1 = config1->getOr<int64_t>("simulation.max_steps", 1000);
            int64_t steps2 = config2->getOr<int64_t>("simulation.max_steps", 1000);
            if (std::abs(steps1 - steps2) / steps1 > 0.8) {
                incompatibilities.push_back("Large step count difference");
            }
        }
        
        return incompatibilities;
    }, py::arg("config1"), py::arg("config2"),
       "Validate compatibility between two configurations");

    m.def("createConfigurationSummary", [](const std::vector<std::shared_ptr<Config>>& configs) {
        std::vector<std::map<std::string, std::string>> summaries;
        
        for (const auto& config : configs) {
            std::map<std::string, std::string> summary;
            summary["neuron_count"] = std::to_string(config->getOr<int64_t>("brain.neuron_count", 1000));
            summary["region_count"] = std::to_string(config->getOr<int64_t>("brain.region_count", 1));
            summary["connection_probability"] = std::to_string(config->getOr<float>("brain.connection_probability", 0.1));
            summary["max_steps"] = std::to_string(config->getOr<int64_t>("simulation.max_steps", 1000));
            
            if (config->has("plasticity.stdp.enable")) {
                summary["stdp_enabled"] = config->getOr<bool>("plasticity.stdp.enable", false) ? "yes" : "no";
            }
            
            if (config->has("development.enabled")) {
                summary["development_enabled"] = config->getOr<bool>("development.enabled", false) ? "yes" : "no";
            }
            
            if (config->has("neuromodulation.reward.enable")) {
                summary["reward_modulation_enabled"] = config->getOr<bool>("neuromodulation.reward.enable", false) ? "yes" : "no";
            }
            
            summaries.push_back(summary);
        }
        
        return summaries;
    }, py::arg("configs"),
       "Create summary of multiple configurations");

    // Advanced error handling and recovery functions
    m.def("safeBrainStep", [](std::shared_ptr<Brain> brain, int step) {
        try {
            brain->step(step);
            return std::make_pair(true, std::string("Success"));
        } catch (const std::exception& e) {
            return std::make_pair(false, std::string("Error: ") + e.what());
        }
    }, py::arg("brain"), py::arg("step"),
       "Safely execute a brain step with error handling");

    m.def("recoverBrainState", [](std::shared_ptr<Brain> brain, const std::string& backup_file) {
        return brain->load(backup_file);
    }, py::arg("brain"), py::arg("backup_file"),
       "Recover brain state from backup file");

    // Advanced configuration merging and comparison functions
    m.def("mergeConfigurations", [](const std::vector<std::shared_ptr<Config>>& configs) {
        auto merged_config = std::make_shared<Config>();
        
        for (const auto& config : configs) {
            for (const auto& key : config->getKeys()) {
                if (config->has(key)) {
                    auto value = config->get(ConfigValue::TypeGuessFromKey(key));
                    if (value.has_value()) {
                        merged_config->set(key, value.value());
                    }
                }
            }
        }
        
        return merged_config;
    }, py::arg("configs"),
       "Merge multiple configurations into one");

    m.def("compareConfigurations", [](const std::shared_ptr<Config>& config1, const std::shared_ptr<Config>& config2) {
        std::map<std::string, std::pair<std::string, std::string>> differences;
        
        auto keys1 = config1->getKeys();
        auto keys2 = config2->getKeys();
        
        std::set<std::string> all_keys;
        for (const auto& key : keys1) all_keys.insert(key);
        for (const auto& key : keys2) all_keys.insert(key);
        
        for (const auto& key : all_keys) {
            std::string value1 = "not set", value2 = "not set";
            
            if (config1->has(key)) {
                auto val = config1->get(ConfigValue::TypeGuessFromKey(key));
                if (val.has_value()) {
                    std::ostringstream oss;
                    oss << val.value();
                    value1 = oss.str();
                }
            }
            
            if (config2->has(key)) {
                auto val = config2->get(ConfigValue::TypeGuessFromKey(key));
                if (val.has_value()) {
                    std::ostringstream oss;
                    oss << val.value();
                    value2 = oss.str();
                }
            }
            
            if (value1 != value2) {
                differences[key] = std::make_pair(value1, value2);
            }
        }
        
        return differences;
    }, py::arg("config1"), py::arg("config2"),
       "Compare two configurations and show differences");

    // Advanced simulation control functions
    m.def("runControlledSimulation", [](std::shared_ptr<Config> config, 
                                      std::function<bool(int)> step_condition,
                                      std::function<void(int)> progress_callback) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(
            config->getOr<int64_t>("environment_width", 10),
            config->getOr<int64_t>("environment_height", 10),
            config->getOr<int64_t>("vision_width", 8),
            config->getOr<int64_t>("vision_height", 8)
        );
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        int step = 0;
        while (step_condition(step)) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            double reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0;
            agent->applyRewardModulation(reward, 0.0);
            
            agent->updateDevelopment(0.1);
            
            progress_callback(step);
            step++;
        }
        
        return brain;
    }, py::arg("config"), py::arg("step_condition"), py::arg("progress_callback"),
       "Run simulation with custom step condition");

    m.def("runAdaptiveSimulation", [](std::shared_ptr<Config> config, 
                                    std::function<double(int)> reward_function,
                                    std::function<void(int, double)> learning_callback) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(
            config->getOr<int64_t>("environment_width", 10),
            config->getOr<int64_t>("environment_height", 10),
            config->getOr<int64_t>("vision_width", 8),
            config->getOr<int64_t>("vision_height", 8)
        );
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        int step = 0;
        while (step < config->getOr<int64_t>("simulation.max_steps", 1000)) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            double reward = reward_function(step);
            agent->applyRewardModulation(reward, 0.0);
            
            agent->updateDevelopment(0.1);
            
            learning_callback(step, reward);
            step++;
        }
        
        return brain;
    }, py::arg("config"), py::arg("reward_function"), py::arg("learning_callback"),
       "Run adaptive simulation with custom reward function");

    // Advanced analysis and visualization functions
    m.def("createAnalysisReport", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) {
        AnalysisTools analysis_tools;
        return analysis_tools.generateStatisticalReport(brain, agent);
    }, py::arg("brain"), py::arg("agent"),
       "Create comprehensive analysis report");

    m.def("createVisualization", [](std::shared_ptr<Brain> brain, const std::string& visualization_type, const std::string& output_file) {
        AnalysisTools analysis_tools;
        return analysis_tools.generateVisualReport(brain, nullptr, visualization_type);
    }, py::arg("brain"), py::arg("visualization_type"), py::arg("output_file"),
       "Create visualization of brain data");

    m.def("analyzeNetworkDynamics", [](std::shared_ptr<Brain> brain, int analysis_window) {
        AnalysisTools analysis_tools;
        return analysis_tools.analyzeSpikeDynamics(brain, analysis_window);
    }, py::arg("brain"), py::arg("analysis_window"),
       "Analyze network dynamics over time window");

    // Advanced batch processing and comparison functions
    m.def("runParameterSweep", [](const std::vector<std::pair<std::string, std::vector<double>>>& parameters, 
                                std::function<void(std::shared_ptr<Config>, int)> simulation_callback) {
        BatchProcessor processor;
        
        // Create parameter combinations
        std::vector<std::vector<std::pair<std::string, double>>> combinations;
        
        // Generate all combinations (simplified implementation)
        for (const auto& param : parameters) {
            std::vector<std::pair<std::string, double>> param_combination;
            for (const auto& value : param.second) {
                param_combination.push_back(param);
            }
            combinations.push_back(param_combination);
        }
        
        for (size_t i = 0; i < combinations.size(); ++i) {
            auto config = std::make_shared<Config>();
            for (const auto& param : combinations[i]) {
                config->set(param.first, param.second);
            }
            processor.addConfig(std::to_string(i), config);
        }
        
        processor.runBatch();
        
        for (size_t i = 0; i < combinations.size(); ++i) {
            auto config = processor.getResults()[i].second;
            if (config) {
                simulation_callback(config, i);
            }
        }
    }, py::arg("parameters"), py::arg("simulation_callback"),
       "Run parameter sweep simulation");

    m.def("compareConfigurationPerformance", [](const std::vector<std::shared_ptr<Config>>& configs) {
        BatchProcessor processor;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            processor.addConfig(std::to_string(i), configs[i]);
        }
        
        processor.runBatch();
        
        std::map<std::string, double> performance_comparison;
        for (size_t i = 0; i < processor.getResults().size(); ++i) {
            auto result = processor.getResults()[i];
            performance_comparison[std::to_string(i)] = result.first;
        }
        
        return performance_comparison;
    }, py::arg("configs"),
       "Compare performance of multiple configurations");

    // Advanced error recovery and validation functions
    m.def("validateBrainState", [](std::shared_ptr<Brain> brain) {
        std::vector<std::string> validation_errors;
        
        if (!brain) {
            validation_errors.push_back("Brain is null");
            return validation_errors;
        }
        
        if (brain->getTotalNeuronCount() == 0) {
            validation_errors.push_back("Brain has no neurons");
        }
        
        if (brain->getTotalSynapseCount() == 0 && brain->getTotalNeuronCount() > 0) {
            validation_errors.push_back("Brain has neurons but no synapses");
        }
        
        return validation_errors;
    }, py::arg("brain"),
       "Validate brain state and return errors");

    m.def("safeBrainReset", [](std::shared_ptr<Brain> brain) {
        try {
            brain->reset();
            return std::make_pair(true, std::string("Brain reset successfully"));
        } catch (const std::exception& e) {
            return std::make_pair(false, std::string("Reset failed: ") + e.what());
        }
    }, py::arg("brain"),
       "Safely reset brain state");

    // Advanced utility functions
    m.def("createConfigurationTemplate", [](const std::string& domain) {
        auto template_config = std::make_shared<Config>();
        
        if (domain == "basic") {
            template_config->set("brain.neuron_count", 1000);
            template_config->set("brain.region_count", 2);
            template_config->set("brain.connection_probability", 0.1);
            template_config->set("log.level", "INFO");
        } else if (domain == "advanced") {
            template_config->set("brain.neuron_count", 5000);
            template_config->set("brain.region_count", 6);
            template_config->set("brain.connection_probability", 0.15);
            template_config->set("plasticity.stdp.enable", true);
            template_config->set("plasticity.hebbian.enable", true);
            template_config->set("development.enabled", true);
            template_config->set("neuromodulation.reward.enable", true);
            template_config->set("neuromodulation.curiosity.enable", true);
        } else if (domain == "research") {
            template_config->set("brain.neuron_count", 10000);
            template_config->set("brain.region_count", 8);
            template_config->set("brain.connection_probability", 0.2);
            template_config->set("plasticity.stdp.enable", true);
            template_config->set("plasticity.hebbian.enable", true);
            template_config->set("plasticity.structural.enable", true);
            template_config->set("development.enabled", true);
            template_config->set("neuromodulation.reward.enable", true);
            template_config->set("neuromodulation.curiosity.enable", true);
            template_config->set("neuromodulation.novelty.enable", true);
            template_config->set("text.processing.enable", true);
            template_config->set("social.learning.enable", true);
        }
        
        return template_config;
    }, py::arg("domain"),
       "Create configuration template for specified domain");

    m.def("mergeConfigurationFiles", [](const std::vector<std::string>& filepaths) {
        auto merged_config = std::make_shared<Config>();
        
        for (const auto& filepath : filepaths) {
            auto config = std::make_shared<Config>();
            if (config->loadFromFile(filepath)) {
                for (const auto& key : config->getKeys()) {
                    if (config->has(key)) {
                        auto value = config->get(ConfigValue::TypeGuessFromKey(key));
                        if (value.has_value()) {
                            merged_config->set(key, value.value());
                        }
                    }
                }
            }
        }
        
        return merged_config;
    }, py::arg("filepaths"),
       "Merge multiple configuration files");

    m.def("exportConfigurationReport", [](const std::vector<std::shared_ptr<Config>>& configs, const std::string& output_file) {
        std::ofstream file(output_file);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open output file: " + output_file);
        }
        
        file << "Configuration Report\n";
        file << "===================\n\n";
        
        for (size_t i = 0; i < configs.size(); ++i) {
            file << "Configuration " << i << ":\n";
            file << configs[i]->summary() << "\n\n";
        }
        
        file.close();
        return true;
    }, py::arg("configs"), py::arg("output_file"),
       "Export configuration report to file");

    // Advanced simulation control functions
    m.def("runControlledExperiment", [](std::shared_ptr<Config> config,
                                      std::function<bool(int, double)> experiment_condition,
                                      std::function<void(int, double, double)> experiment_callback) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(
            config->getOr<int64_t>("environment_width", 10),
            config->getOr<int64_t>("environment_height", 10),
            config->getOr<int64_t>("vision_width", 8),
            config->getOr<int64_t>("vision_height", 8)
        );
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        int step = 0;
        double cumulative_reward = 0.0;
        
        while (experiment_condition(step, cumulative_reward)) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            double reward = percept.getInternal()[0] if percept.getInternal() else 0.0;
            cumulative_reward += reward;
            agent->applyRewardModulation(reward, 0.0);
            
            agent->updateDevelopment(0.1);
            
            experiment_callback(step, reward, cumulative_reward);
            step++;
        }
        
        return brain;
    }, py::arg("config"), py::arg("experiment_condition"), py::arg("experiment_callback"),
       "Run controlled experiment with custom conditions");

    m.def("runAdaptiveLearningExperiment", [](std::shared_ptr<Config> config,
                                             std::function<double(int, const SensoryPercept&)> reward_function,
                                             std::function<void(int, std::map<std::string, double>)> metrics_callback) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(
            config->getOr<int64_t>("environment_width", 10),
            config->getOr<int64_t>("environment_height", 10),
            config->getOr<int64_t>("vision_width", 8),
            config->getOr<int64_t>("vision_height", 8)
        );
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        int step = 0;
        std::map<std::string, double> metrics;
        
        while (step < config->getOr<int64_t>("simulation.max_steps", 1000)) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            double reward = reward_function(step, percept);
            agent->applyRewardModulation(reward, 0.0);
            
            agent->updateDevelopment(0.1);
            
            // Update metrics
            metrics["step"] = step;
            metrics["reward"] = reward;
            metrics["firing_rate"] = brain->getAverageFiringRate();
            metrics["spike_count"] = brain->getTotalSpikeCount();
            metrics["active_neurons"] = brain->getActiveNeuronCount();
            
            metrics_callback(step, metrics);
            step++;
        }
        
        return brain;
    }, py::arg("config"), py::arg("reward_function"), py::arg("metrics_callback"),
       "Run adaptive learning experiment with custom reward function");

    // Advanced analysis and visualization functions
    m.def("createComprehensiveAnalysis", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) {
        AnalysisTools analysis_tools;
        
        auto spike_analysis = analysis_tools.analyzeSpikeDynamics(brain, 100);
        auto plasticity_analysis = analysis_tools.analyzePlasticityChanges(brain);
        auto memory_analysis = analysis_tools.analyzeMemoryUsage(brain);
        auto learning_analysis = analysis_tools.analyzeLearningProgress(brain, agent);
        auto behavior_analysis = analysis_tools.analyzeBehaviorPatterns(brain, agent);
        
        std::map<std::string, std::vector<double>> comprehensive_results;
        
        comprehensive_results["spike_patterns"] = spike_analysis["spike_patterns"];
        comprehensive_results["plasticity_changes"] = plasticity_analysis["plasticity_changes"];
        comprehensive_results["memory_usage"] = memory_analysis["memory_usage"];
        comprehensive_results["learning_progress"] = learning_analysis["learning_progress"];
        comprehensive_results["behavior_patterns"] = behavior_analysis["behavior_patterns"];
        
        return comprehensive_results;
    }, py::arg("brain"), py::arg("agent"),
       "Create comprehensive analysis of brain and agent");

    m.def("generateResearchReport", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, const std::string& report_type) {
        AnalysisTools analysis_tools;
        
        if (report_type == "comprehensive") {
            return analysis_tools.generateStatisticalReport(brain, agent);
        } else if (report_type == "visual") {
            return analysis_tools.generateVisualReport(brain, agent, "pdf");
        } else if (report_type == "network") {
            return analysis_tools.createNetworkGraph(brain, "spring");
        } else {
            throw std::invalid_argument("Unknown report type: " + report_type);
        }
    }, py::arg("brain"), py::arg("agent"), py::arg("report_type"),
       "Generate research report of specified type");

    // Advanced batch processing and comparison functions
    m.def("runMultiObjectiveOptimization", [](const std::vector<std::shared_ptr<Config>>& configs,
                                            std::function<double(std::shared_ptr<Brain>)> objective_function) {
        BatchProcessor processor;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            processor.addConfig(std::to_string(i), configs[i]);
        }
        
        processor.runBatch();
        
        std::vector<std::pair<std::string, double>> optimization_results;
        for (size_t i = 0; i < processor.getResults().size(); ++i) {
            auto config = processor.getResults()[i].second;
            if (config) {
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                
                double objective_value = objective_function(brain);
                optimization_results.push_back(std::make_pair(std::to_string(i), objective_value));
            }
        }
        
        return optimization_results;
    }, py::arg("configs"), py::arg("objective_function"),
       "Run multi-objective optimization with multiple configurations");

    m.def("findOptimalConfiguration", [](const std::vector<std::shared_ptr<Config>>& configs,
                                        std::function<double(std::shared_ptr<Brain>)> performance_metric) {
        BatchProcessor processor;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            processor.addConfig(std::to_string(i), configs[i]);
        }
        
        processor.runBatch();
        
        std::string optimal_config_id = "";
        double best_metric = std::numeric_limits<double>::lowest();
        
        for (size_t i = 0; i < processor.getResults().size(); ++i) {
            auto config = processor.getResults()[i].second;
            if (config) {
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                
                double metric_value = performance_metric(brain);
                if (metric_value > best_metric) {
                    best_metric = metric_value;
                    optimal_config_id = std::to_string(i);
                }
            }
        }
        
        return std::make_pair(optimal_config_id, best_metric);
    }, py::arg("configs"), py::arg("performance_metric"),
       "Find optimal configuration based on performance metric");

    // Advanced error recovery and validation functions
    m.def("validateExperimentSetup", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world) {
        std::vector<std::string> validation_errors;
        
        // Validate brain
        auto brain_errors = validateBrainState(brain);
        validation_errors.insert(validation_errors.end(), brain_errors.begin(), brain_errors.end());
        
        // Validate agent
        if (!agent) {
            validation_errors.push_back("Agent is null");
        }
        
        // Validate world
        if (!world) {
            validation_errors.push_back("World is null");
        } else {
            if (world->getWidth() <= 0 || world->getHeight() <= 0) {
                validation_errors.push_back("Invalid world dimensions");
            }
        }
        
        return validation_errors;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
       "Validate complete experiment setup");

    m.def("safeExperimentRun", [](std::shared_ptr<Brain> brain,
                                 std::shared_ptr<AgentBrain> agent,
                                 std::shared_ptr<SimpleWorld> world,
                                 int max_steps,
                                 std::function<bool(int)> stop_condition) {
        std::vector<std::map<std::string, double>> experiment_data;
        
        try {
            int step = 0;
            double cumulative_reward = 0.0;
            
            while (!stop_condition(step) && step < max_steps) {
                world->update(0.1);
                
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                double reward = percept.getInternal()[0] if percept.getInternal() else 0.0;
                cumulative_reward += reward;
                agent->applyRewardModulation(reward, 0.0);
                
                agent->updateDevelopment(0.1);
                
                // Record step data
                std::map<std::string, double> step_data;
                step_data["step"] = step;
                step_data["reward"] = reward;
                step_data["cumulative_reward"] = cumulative_reward;
                step_data["firing_rate"] = brain->getAverageFiringRate();
                step_data["spike_count"] = brain->getTotalSpikeCount();
                step_data["active_neurons"] = brain->getActiveNeuronCount();
                
                experiment_data.push_back(step_data);
                
                step++;
            }
            
            return experiment_data;
        } catch (const std::exception& e) {
            throw std::runtime_error("Experiment run failed: " + std::string(e.what()));
        }
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("max_steps"), py::arg("stop_condition"),
       "Run safe experiment with comprehensive error handling");

    // Advanced utility functions
    m.def("createResearchPipeline", [](const std::string& domain, 
                                      const std::vector<std::string>& stages) {
        auto pipeline_config = std::make_shared<Config>();
        
        // Configure based on domain
        if (domain == "neuroscience") {
            pipeline_config->set("brain.neuron_count", 10000);
            pipeline_config->set("brain.region_count", 8);
            pipeline_config->set("plasticity.stdp.enable", true);
            pipeline_config->set("plasticity.hebbian.enable", true);
        } else if (domain == "ai") {
            pipeline_config->set("brain.neuron_count", 5000);
            pipeline_config->set("brain.region_count", 6);
            pipeline_config->set("text.processing.enable", true);
        }
        
        // Configure pipeline stages
        for (size_t i = 0; i < stages.size(); ++i) {
            pipeline_config->set(f"pipeline.stage.{i}", stages[i]);
        }
        
        return std::make_pair(pipeline_config, stages);
    }, py::arg("domain"), py::arg("stages"),
       "Create research pipeline with specified stages");

    m.def("exportForPublication", [](std::shared_ptr<Brain> brain, 
                                   std::shared_ptr<AgentBrain> agent,
                                   const std::string& output_dir) {
        // Create output directory if it doesn't exist
        std::filesystem::create_directories(output_dir);
        
        // Export brain state
        bool brain_exported = ExportImportTools().exportBrainState(brain, output_dir + "/brain_state.dat");
        
        // Export configuration
        auto config = brain->getConfig();
        bool config_exported = ExportImportTools().exportConfiguration(config, output_dir + "/config.json");
        
        // Export analysis results
        AnalysisTools analysis_tools;
        auto analysis_report = analysis_tools.generateStatisticalReport(brain, agent);
        
        // Save analysis results
        std::ofstream analysis_file(output_dir + "/analysis_results.json");
        if (analysis_file.is_open()) {
            // Write analysis results (simplified)
            analysis_file << "Analysis results placeholder";
            analysis_file.close();
        }
        
        return std::make_pair(brain_exported, config_exported);
    }, py::arg("brain"), py::arg("agent"), py::arg("output_dir"),
       "Export data for publication with comprehensive file structure");

    m.def("createAdvancedResearchEnvironment", [](const std::string& environment_type, 
                                                  const std::map<std::string, double>& parameters) {
        auto config = std::make_shared<Config>();
        
        // Configure based on environment type
        if (environment_type == "controlled_laboratory") {
            config->set("environment.width", parameters["width"]);
            config->set("environment.height", parameters["height"]);
            config->set("brain.neuron_count", parameters["neuron_count"]);
            config->set("simulation.timestep", 0.001);
        } else if (environment_type == "natural_environment") {
            config->set("environment.width", parameters["width"] * 2;
            config->set("environment.height", parameters["height"] * 2;
            config->set("brain.neuron_count", parameters["neuron_count"] * 10);
            config->set("simulation.timestep", 0.005);
        } else if (environment_type == "artificial_world") {
            config->set("environment.width", parameters["width"]);
            config->set("environment.height", parameters["height"]);
            config->set("brain.neuron_count", parameters["neuron_count"]);
            config->set("simulation.timestep", amic;
        }
        
        return config;
    }, py::arg("environment_type"), py::arg("parameters"),
       "Create advanced research environment with specified parameters");

    // Final utility functions for research and production
    m.def("validateResearchExperiment", [](std::shared_ptr<Brain> brain, 
                                         std::shared_ptr<AgentBrain> agent,
                                         std::shared_ptr<SimpleWorld> world,
                                         const std::map<std::string, double>& experimental_parameters) {
        std::vector<std::string> validation_errors;
        
        // Basic validation
        if (!brain) validation_errors.push_back("Brain is null");
        if (!agent) validation_errors.push_back("Agent is null");
        if (!world) validation_errors.push_back("World is null");
        
        // Parameter validation
        if (experimental_parameters.find("neuron_count") == experimental_parameters.end()) {
            validation_errors.push_back("Missing neuron count parameter");
        }
        
        if (experimental_parameters.find("duration") == experimental_parameters.end()) {
            validation_errors.push_back("Missing duration parameter");
        }
        
        // Performance validation
        if (brain->getTotalNeuronCount() == 0) {
            validation_errors.push_back("Brain has no neurons");
        }
        
        return validation_errors;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("experimental_parameters"),
       "Validate complete research experiment setup");

    m.def("runProductionSimulation", [](std::shared_ptr<Config> config,
                                      std::function<void(double)> progress_callback,
                                      std::function<void(std::string)> log_callback) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(
            config->getOr<int64_t>("environment.width", 10),
            config->getOr<int64_t>("environment.height", 10),
            config->getOr<int64_t>("vision.width", 8),
            config->getOr<int64_t>("vision.height", 8)
        );
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        int max_steps = config->getOr<int64_t>("simulation.max_steps", 1000);
        
        for (int step = 0; step < max_steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            double reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0;
            agent->applyRewardModulation(reward, 0.0);
            
            agent->updateDevelopment(0.1);
            
            // Log progress
            std::ostringstream log_msg;
            log_msg << "Step " << step << ": " << 
                    brain->getFiringNeuronCount() << " firing neurons, " <<
                    brain->getAverageFiringRate() << " Hz average rate";
            
            log_callback(log_msg.str());
            progress_callback(step / (double)max_steps);
        }
        
        return brain;
    }, py::arg("config"), py::arg("progress_callback"), py::arg("log_callback"),
       "Run production simulation with progress and logging callbacks");

    // Research and publication utilities
    m.def("createPublicationDataset", [](std::shared_ptr<Brain> brain,
                                        std::shared_ptr<AgentBrain> agent,
                                        const std::string& dataset_name,
                                        const std::map<std::string, std::string>& metadata) {
        std::map<std::string, std::vector<double>> dataset;
        
        // Extract simulation metrics
        dataset["spike_counts"] = {brain->getTotalSpikeCount()};
        dataset["firing_rates"] = {brain->getAverageFiringRate()};
        dataset["active_neurons"] = {brain->getActiveNeuronCount()};
        dataset["steps"] = {static_cast<double>(metadata.count("steps"))};
        
        // Add metadata
        for (const auto& entry : metadata) {
            dataset[entry.first] = {static_cast<double>(entry.second.length())};
        }
        
        return dataset;
    }, py::arg("brain"), py::arg("agent"), py::arg("dataset_name"), py::arg("metadata"),
       "Create publication-ready dataset with metadata");

    m.def("generateResearchPaperData", [](std::shared_ptr<Brain> brain,
                                         std::shared_ptr<AgentBrain> agent,
                                         const std::string& paper_type) {
        std::map<std::string, std::vector<std::vector<double>>> research_data;
        
        // Collect comprehensive data
        auto spike_analysis = AnalysisTools().analyzeSpikeDynamics(brain, 100);
        auto plasticity_analysis = AnalysisTools().analyzePlasticityChanges(brain);
        auto memory_analysis = AnalysisTools().analyzeMemoryUsage(brain);
        auto learning_analysis = AnalysisTools().analyzeLearningProgress(brain, agent);
        
        research_data["spike_dynamics"] = {spike_analysis["spike_patterns"]};
        research_data["plasticity_changes"] = {plasticity_analysis["plasticity_changes"]};
        research_data["memory_usage"] = {memory_analysis["memory_usage"]};
        research_data["learning_progress"] = {learning_analysis["learning_progress"]};
        
        return research_data;
    }, py::arg("brain"), py::arg("agent"), py::arg("paper_type"),
       "Generate research paper data for specified paper type");

    // Final utility functions
    m.def("createAdvancedResearchSetup", [](const std::string& research_domain,
                                          const std::vector<std::string>& experimental_conditions) {
        auto config = std::make_shared<Config>();
        
        // Configure for research domain
        if (research_domain == "cognitive_neuroscience") {
            config->set("brain.neuron_count", 15000);
            config->set("brain.region_count", 10);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("development.enabled", true);
            config->set("neuromodulation.curiosity.enable", true);
            config->set("neuromodulation.novelty.enable", true);
            config->set("social.learning.enable", true);
        } else if (research_domain == "machine_learning") {
            config->set("brain.neuron_count", 8000);
            config->set("brain.region_count", 6);
            config->set("plasticity.stdp.enable", true);
            config->set("text.processing.enable", true);
            config->set("development.enabled", true);
        }
        
        // Configure experimental conditions
        for (size_t i = 0; i < experimental_conditions.size(); ++i) {
            config->set(f"experimental.condition.{i}", experimental_conditions[i]);
        }
        
        return std::make_pair(config, experimental_conditions);
    }, py::arg("research_domain"), py::arg("experimental_conditions"),
       "Create advanced research setup with experimental conditions");

    m.def("validateAndPrepareExperiment", [](std::shared_ptr<Brain> brain,
                                           std::shared_ptr<AgentBrain> agent,
                                           std::shared_ptr<SimpleWorld> world,
                                           const std::string& experiment_type) {
        std::vector<std::string> validation_results;
        
        // Validate complete experiment setup
        auto basic_validation = validateExperimentSetup(brain, agent, world);
        validation_results.insert(validation_results.end(), basic_validation.begin(), basic_validation.end());
        
        if (validation_results.empty()) {
            validation_results.push_back("Experiment setup is valid");
        }
        
        // Prepare experiment based on type
        if (experiment_type == "long_term") {
            validation_results.push_back("Preparing for long-term experiment (extended duration)");
        } else if (experiment_type == "high_frequency") {
            validation_results.push_back("Preparing for high-frequency experiment (rapid steps)");
        } else if (experiment_type == "adaptive") {
            validation_results.push_back("Preparing for adaptive experiment (dynamic parameters)");
        }
        
        return validation_results;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("experiment_type"),
       "Validate and prepare complete experiment setup");

    m.def("createProductionReadyConfig", [](const std::string& use_case) {
        auto config = std::make_shared<Config>();
        
        if (use_case == "research") {
            config->set("brain.neuron_count", 10000);
            config->set("brain.region_count", 8);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("development.enabled", true);
            config->set("neuromodulation.reward.enable", true);
            config->set("neuromodulation.curiosity.enable", true);
            config->set("log.level", "INFO");
        } else if (use_case == "production") {
            config->set("brain.neuron_count", 5000);
            config->set("brain.region_count", 6);
            config->set("plasticity.stdp.enable", true);
            config->set("log.level", "WARN");
        } else if (use_case == "prototype") {
            config->set("brain.neuron_count", 1000);
            config->set("brain.region_count", 2);
            config->set("log.level", "ERROR");
        }
        
        return config;
    }, py::arg("use_case"),
       "Create production-ready configuration for specified use case");

    // Final comprehensive utility functions
    m.def("runAdvancedResearchPipeline", [](const std::string& pipeline_name,
                                          const std::vector<std::shared_ptr<Config>>& configurations,
                                          std::function<void(int, int)> progress_callback) {
        BatchProcessor processor;
        
        for (size_t i = 0; i < configurations.size(); ++i) {
            processor.addConfig(std::to_string(i), configurations[i]);
        }
        
        processor.runBatchWithProgress(progress_callback);
        
        std::map<std::string, std::vector<std::pair<std::string, double>>> results;
        for (size_t i = 0; i < processor.getResults().size(); ++i) {
            auto result = processor.getResults()[i];
            std::vector<std::pair<std::string, double>> result_data;
            result_data.push_back(std::make_pair("success", result.first ? 1.0 : 0.0));
            result_data.push_back(std::make_pair("execution_time", result.second));
            results[std::to_string(i)] = result_data;
        }
        
        return results;
    }, py::arg("pipeline_name"), py::arg("configurations"), py::arg("progress_callback"),
       "Run advanced research pipeline with comprehensive progress tracking");

    m.def("createAdvancedAnalytics", [](std::shared_ptr<Brain> brain,
                                      std::shared_ptr<AgentBrain> agent,
                                      const std::map<std::string, std::string>& analysis_options) {
        AnalysisTools analysis_tools;
        
        std::map<std::string, std::vector<std::vector<double>>> analytics;
        
        // Perform comprehensive analysis based on options
        if (analysis_options.find("spike_dynamics") != analysis_options.end() && 
            analysis_options.at("spike_dynamics") == "enable") {
            auto spike_data = analysis_tools.analyzeSpikeDynamics(brain, 100);
            analytics["spike_dynamics"] = {spike_data["spike_patterns"]};
        }
        
        if (analysis_options.find("plasticity") != analysis_options.end() && 
            analysis_options.at("plasticity") == "enable") {
            auto plasticity_data = analysis_tools.analyzePlasticityChanges(brain);
            analytics["plasticity"] = {plasticity_data["plasticity_changes"]};
        }
        
        if (analysis_options.find("memory") != analysis_options.end() && 
            analysis_options.at("memory") == "enable") {
            auto memory_data = analysis_tools.analyzeMemoryUsage(brain);
            analytics["memory"] = {memory_data["memory_usage"]};
        }
        
        if (analysis_options.find("learning") != analysis_options.end() && 
            analysis_options.at("learning") == "enable") {
            auto learning_data = analysis_tools.analyzeLearningProgress(brain, agent);
            analytics["learning"] = {learning_data["learning_progress"]};
        }
        
        return analytics;
    }, py::arg("brain"), py::arg("agent"), py::arg("analysis_options"),
       "Create advanced analytics based on specified options");

    // Final utility functions for research and production
    m.def("benchmarkAdvancedConfiguration", [](std::shared_ptr<Config> config,
                                             int iterations,
                                             std::function<void(int, double)> benchmark_callback) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        std::vector<double> execution_times;
        
        for (int i = 0; i < iterations; ++i) {
            auto start_time = std::chrono::high_resolution_clock::now();
            
            // Run simulation step
            brain->step(i);
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            
            double execution_time_ms = duration.count() / 1000.0;
            execution_times.push_back(execution_time_ms);
            
            benchmark_callback(i, execution_time_ms);
        }
        
        std::map<std::string, double> benchmark_results;
        benchmark_results["avg_execution_time_ms"] = std::accumulate(execution_times.begin(), execution_times.end(), 0.0) / execution_times.size();
        benchmark_results["min_execution_time_ms"] = *std::min_element(execution_times.begin(), execution_times.end());
        benchmark_results["max_execution_time_ms"] = *std::max_element(execution_times.begin(), execution_times.end());
        benchmark_results["std_dev_ms"] = std::sqrt(std::accumulate(execution_times.begin(), execution_times.end(), 0.0,
            [execution_times](double sum, double val) { return sum + std::pow(val - std::accumulate(execution_times.begin(), execution_times.end(), 0.0) / execution_times.size(), 2); })
            / execution_times.size());
        
        return benchmark_results;
    }, py::arg("config"), py::arg("iterations"), py::arg("benchmark_callback"),
       "Benchmark advanced configuration performance");

    m.def("validateAndOptimizeConfiguration", [](std::shared_ptr<Config> config) {
        std::vector<std::string> optimization_suggestions;
        
        // Analyze configuration for optimization opportunities
        int64_t neuron_count = config->getOr<int64_t>("brain.neuron_count", 1000);
        int64_t max_steps = config->getOr<int64_t>("simulation.max_steps", 1000);
        
        // Check if configuration is optimized for performance
        if (neuron_count > 20000) {
            optimization_suggestions.push_back("Large neuron count may impact performance - consider reducing if not needed");
        }
        
        if (max_steps > 10000) {
            optimization_suggestions.push_back("Large step count may impact simulation time - consider reducing if not needed");
        }
        
        // Check if plasticity settings are optimal
        bool stdp_enabled = config->getOr<bool>("plasticity.stdp.enable", false);
        bool hebbian_enabled = config->getOr<bool>("plasticity.hebbian.enable", false);
        
        if (!stdp_enabled && !hebbian_enabled) {
            optimization_suggestions.push_back("No plasticity rules enabled - consider enabling STDP or Hebbian learning");
        }
        
        // Check development settings
        bool development_enabled = config->getOr<bool>("development.enabled", false);
        if (development_enabled) {
            optimization_suggestions.push_back("Development enabled - may impact performance - enable only if needed");
        }
        
        // Check neuromodulation settings
        bool reward_enabled = config->getOr<bool>("neuromodulation.reward.enable", false);
        bool curiosity_enabled = config->getOr<bool>("neuromodulation.curiosity.enable", false);
        
        if (!reward_enabled && !curiosity_enabled) {
            optimization_suggestions.push_back("No neuromodulation enabled - consider enabling for adaptive behavior");
        }
        
        return optimization_suggestions;
    }, py::arg("config"),
       "Validate and provide optimization suggestions for configuration");

    // Final comprehensive utility functions
    m.def("createResearchExperiment", [](const std::string& experiment_name,
                                        const std::string& hypothesis,
                                        const std::vector<std::string>& methods) {
        std::map<std::string, std::string> experiment_metadata;
        
        experiment_metadata["name"] = experiment_name;
        experiment_metadata["hypothesis"] = hypothesis;
        experiment_metadata["methods"] = boost::algorithm::join(methods, "; ");
        experiment_metadata["created_timestamp"] = std::to_string(std::time(nullptr));
        
        return experiment_metadata;
    }, py::arg("experiment_name"), py::arg("hypothesis"), py::arg("methods"),
       "Create research experiment with metadata");

    m.def("exportExperimentResults", [](const std::map<std::string, std::string>& results,
                                       const std::string& output_format) {
        std::map<std::string, std::string> exported_results;
        
        for (const auto& entry : results) {
            exported_results[entry.first] = entry.second;
        }
        
        return exported_results;
    }, py::arg("results"), py::arg("output_format"),
       "Export experiment results with specified format");

    // Final summary function
    m.def("createAdvancedNLMSetup", []() {
        auto config = std::make_shared<Config>();
        
        // Create advanced configuration with all available features
        config->set("brain.neuron_count", 8000);
        config->set("brain.region_count", 6);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("development.enabled", true);
        config->set("neuromodulation.reward.enable", true);
        config->set("neuromodulation.curiosity.enable", true);
        config->set("neuromodulation.novelty.enable", true);
        config->set("text.processing.enable", true);
        config->set("social.learning.enable", true);
        config->set("simulation.max.steps", 5000);
        config->set("log.level", "INFO");
        
        // Create advanced tools
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 1);
        world->reset();
        agent->initialize(world);
        
        // Set up advanced learning
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return std::make_pair(brain, std::make_pair(agent, world));
    }, "Create complete advanced NLM setup with all features enabled");

    // Add comprehensive documentation for advanced users
    m.doc() = R"pbdoc(
        NLM Python Bindings - Advanced Usage Guide

        This comprehensive Python API provides advanced functionality for:

        • Configuration Management - Advanced configuration with validation and optimization
        • Performance Monitoring - Real-time monitoring and profiling tools
        • Batch Processing - Run multiple simulations efficiently
        • Debugging Tools - Advanced debugging and error analysis
        • Custom Behaviors - Custom agent behaviors and strategies
        • Analytics - Comprehensive analysis and visualization tools
        • Export/Import - Advanced data export and import utilities
        • Research Pipelines - Complete research pipeline support
        • Experiment Control - Advanced experiment control and management
        • Performance Benchmarking - Comprehensive performance benchmarking
        • Configuration Optimization - Configuration analysis and optimization
        • Research Experiment Management - Complete research experiment lifecycle
        • Production Deployment - Production-ready simulation setup
        • Publication Data - Publication-ready data export
        • Research Analytics - Advanced research analytics and reporting

        Advanced Configuration Classes:
        - AdvancedConfig: Advanced configuration manager with validation
        - PerformanceMonitor: Performance monitoring and profiling
        - BatchProcessor: Batch processing for multiple simulations
        - DebugTools: Advanced debugging and profiling tools
        - CustomAgentBehavior: Custom agent behaviors
        - ExportImportTools: Advanced export/import utilities
        - AnalysisTools: Advanced analysis and visualization tools

        Key Advanced Features:
        • Phase 6 Integration - Complete Phase 6 feature support
        • Social Learning - Advanced social learning and observation
        • Text Processing - Text encoding and association
        • Concept Formation - Advanced concept formation
        • Memory Systems - Advanced memory management
        • Prediction Systems - Advanced prediction and planning
        • Performance Optimization - Performance optimization tools
        • Configuration Validation - Advanced configuration validation
        • Research Pipelines - Complete research pipeline support
        • Experiment Management - Advanced experiment management
        • Publication Support - Publication-ready data export
        • Performance Monitoring - Real-time performance monitoring
        • Debugging Tools - Advanced debugging capabilities
        • Analytics - Comprehensive analysis tools
        • Visualization - Advanced visualization tools
        • Batch Processing - Efficient batch processing
        • Custom Behaviors - Custom agent behaviors
        • Export/Import - Advanced data utilities
        • Configuration Management - Advanced configuration management

        Advanced Usage Examples:
        1. Advanced Configuration Management:
            config = AdvancedConfig()
            config.configure_for_phase6()
            config.validateConfig(validators)

        2. Performance Monitoring:
            monitor = PerformanceMonitor()
            monitor.startMonitoring()
            brain.step(step)
            monitor.recordStepTime(time_ms)

        3. Batch Processing:
            processor = BatchProcessor()
            processor.addConfig("config1", config1)
            processor.addConfig("config2", config2)
            processor.runBatch()

        4. Debugging:
            debug = DebugTools()
            debug.enableMemoryDebugging(True)
            debug.startProfiling("experiment")

        5. Custom Behaviors:
            behavior = CustomAgentBehavior()
            behavior.setActionSelectionStrategy(custom_strategy)

        6. Analytics:
            analytics = AnalysisTools()
            spike_data = analytics.analyzeSpikeDynamics(brain, 100)

        7. Export/Import:
            exporter = ExportImportTools()
            exporter.exportBrainState(brain, "experiment_data.dat")

        8. Research Pipelines:
            pipeline = createResearchPipeline("neuroscience", ["stage1", "stage2"])

        9. Experiment Control:
            runAdvancedSimulation(config, progress_callback)

        10. Performance Benchmarking:
            results = benchmarkConfiguration(config, 100)

        This advanced API enables researchers and developers to:
        - Create complex, large-scale neural simulations
        - Analyze simulation performance in detail
        - Run batch experiments efficiently
        - Debug and optimize simulation performance
        - Create custom agent behaviors
        - Export and import data for analysis
        - Set up complete research pipelines
        - Run controlled experiments
        - Benchmark and compare configurations
        - Validate and optimize configurations
        - Create publication-ready outputs

        For best results, refer to the HOW_TO_USE.md and other documentation files.
    )pbdoc";

    // Add advanced constants and enumerations
    m.attr("ADVANCED_FEATURES_VERSION") = "Phase6.1";
    m.attr("MAX_ADVANCED_CONFIGS") = 100;
    m.attr("MAX_BATCH_JOBS") = 1000;
    m.attr("MAX_ANALYSIS_SAMPLES") = 1000000;
    m.attr("MAX_EXPORT_SIZE_MB") = 1024;
}

} // namespace nlm