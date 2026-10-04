# Enhanced NLM Python Utilities
# Additional helper classes and utilities for advanced NLM users

namespace nlm {
namespace PythonUtilities {

// Utility class for common NLM operations and data transformations
class NLMToolkit {
public:
    // Configuration validation utilities
    static bool validateBrainConfiguration(const std::shared_ptr<Config>& config) {
        if (!config) return false;
        
        return config->has("brain.neuron_count") && 
               config->has("brain.connection_probability") &&
               config->getOr<size_t>("brain.neuron_count", 0) > 0;
    }
    
    // Create standardized configuration profiles
    static std::shared_ptr<Config> createConfigurationProfile(const std::string& profile) {
        auto config = std::make_shared<Config>();
        
        if (profile == "minimal") {
            config->set("brain.neuron_count", static_cast<int64_t>(100));
            config->set("brain.connection_probability", 0.05f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
        } else if (profile == "balanced") {
            config->set("brain.neuron_count", static_cast<int64_t>(1000));
            config->set("brain.connection_probability", 0.1f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
        } else if (profile == "advanced") {
            config->set("brain.neuron_count", static_cast<int64_t>(5000));
            config->set("brain.connection_probability", 0.15f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.dopamine.scale", 1.0f);
            config->set("neuromod.prediction_error.scale", 0.5f);
        }
        
        return config;
    }
    
    // Data export/import utilities
    static std::string exportBrainStateToJSON(const std::shared_ptr<Brain>& brain) {
        if (!brain) return "{}";
        
        std::string json = "{";
        json += "\"total_neurons\": " + std::to_string(brain->getTotalNeuronCount()) + ",";
        json += "\"total_synapses\": " + std::to_string(brain->getTotalSynapseCount()) + ",";
        json += "\"active_neurons\": " + std::to_string(brain->getActiveNeuronCount()) + ",";
        json += "\"firing_neurons\": " + std::to_string(brain->getFiringNeuronCount()) + ",";
        json += "\"total_spikes\": " + std::to_string(brain->getTotalSpikeCount()) + ",";
        json += "\"avg_firing_rate\": " + std::to_string(brain->getAverageFiringRate()) + ",";
        json += "\"e_i_ratio\": " + std::to_string(brain->getExcitationInhibitionRatio()) + "}";
        
        return json;
    }
    
    // Performance profiling utilities
    static struct PerformanceProfile {
        double setupTime;
        double simulationTime;
        size_t stepsExecuted;
        float avgFiringRate;
        size_t peakMemoryUsage;
        float cpuUtilization;
    };
    
    static PerformanceProfile profileSimulation(std::shared_ptr<Brain> brain, int steps) {
        PerformanceProfile profile;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain for profiling");
        }
        
        auto setupEndTime = std::chrono::high_resolution_clock::now();
        profile.setupTime = std::chrono::duration<double>(setupEndTime - startTime).count();
        
        auto simStartTime = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
        }
        auto simEndTime = std::chrono::high_resolution_clock::now();
        
        profile.simulationTime = std::chrono::duration<double>(simEndTime - simStartTime).count();
        profile.stepsExecuted = steps;
        profile.avgFiringRate = brain->getAverageFiringRate();
        profile.peakMemoryUsage = 0; // Would need platform-specific memory tracking
        profile.cpuUtilization = 0.0f; // Would need platform-specific CPU tracking
        
        return profile;
    }
    
    // Comparison utilities
    static std::string generatePerformanceReport(const PerformanceProfile& profile) {
        std::string report = "Performance Profile:\n";
        report += "  Setup Time: " + std::to_string(profile.setupTime) + "s\n";
        report += "  Simulation Time: " + std::to_string(profile.simulationTime) + "s\n";
        report += "  Steps Executed: " + std::to_string(profile.stepsExecuted) + "\n";
        report += "  Average Firing Rate: " + std::to_string(profile.avgFiringRate) + " Hz\n";
        report += "  Peak Memory Usage: " + std::to_string(profile.peakMemoryUsage) + " MB\n";
        report += "  CPU Utilization: " + std::to_string(profile.cpuUtilization) + "%\n";
        
        // Calculate efficiency metrics
        if (profile.setupTime + profile.simulationTime > 0.0) {
            double efficiency = static_cast<double>(profile.stepsExecuted) / (profile.setupTime + profile.simulationTime);
            report += "  Efficiency: " + std::to_string(efficiency) + " steps/second\n";
        }
        
        return report;
    }
    
    // Error recovery utilities
    static bool recoverFromBrainError(std::shared_ptr<Brain>& brain, const std::string& errorContext) {
        try {
            NLM_LOG_WARNING(std::string("Attempting recovery from brain error in context: ") + errorContext);
            
            if (!brain) {
                NLM_LOG_ERROR("Cannot recover: brain is null");
                return false;
            }
            
            // Reset brain state
            brain->reset();
            
            // Re-initialize with existing configuration
            auto config = brain->getConfig();
            if (config) {
                brain = std::make_shared<Brain>(config);
                if (!brain->initialize()) {
                    NLM_LOG_ERROR("Failed to re-initialize brain after recovery");
                    return false;
                }
                
                NLM_LOG_INFO("Brain successfully recovered from error");
                return true;
            }
            
            NLM_LOG_ERROR("No configuration available for brain recovery");
            return false;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception during brain recovery: ") + e.what());
            return false;
        }
    }
    
    // Batch processing utilities
    static std::vector<std::shared_ptr<Brain>> batchCreateBrains(size_t count, const std::shared_ptr<Config>& baseConfig) {
        std::vector<std::shared_ptr<Brain>> brains;
        
        for (size_t i = 0; i < count; ++i) {
            try {
                auto config = std::make_shared<Config>(*baseConfig);
                // Add small variation to each config for diversity
                config->set("random_seed", static_cast<int64_t>(i + 1000));
                
                auto brain = std::make_shared<Brain>(config);
                if (brain->initialize()) {
                    brains.push_back(brain);
                } else {
                    NLM_LOG_WARNING(std::string("Failed to initialize brain in batch ") + std::to_string(i));
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Exception creating brain in batch ") + std::to_string(i) + ": " + e.what());
            }
        }
        
        return brains;
    }
    
    // Data aggregation utilities
    static std::string aggregateBrainMetrics(const std::vector<std::shared_ptr<Brain>>& brains) {
        if (brains.empty()) return "{}";
        
        size_t totalNeurons = 0;
        size_t totalSynapses = 0;
        size_t totalActiveNeurons = 0;
        size_t totalFiringNeurons = 0;
        size_t totalSpikes = 0;
        float totalFiringRate = 0.0f;
        
        for (const auto& brain : brains) {
            if (brain) {
                totalNeurons += brain->getTotalNeuronCount();
                totalSynapses += brain->getTotalSynapseCount();
                totalActiveNeurons += brain->getActiveNeuronCount();
                totalFiringNeurons += brain->getFiringNeuronCount();
                totalSpikes += brain->getTotalSpikeCount();
                totalFiringRate += brain->getAverageFiringRate();
            }
        }
        
        size_t brainCount = brains.size();
        
        std::string json = "{";
        json += "\"brain_count\": " + std::to_string(brainCount) + ",";
        json += "\"total_neurons\": " + std::to_string(totalNeurons) + ",";
        json += "\"total_synapses\": " + std::to_string(totalSynapses) + ",";
        json += "\"avg_active_neurons\": " + std::to_string(static_cast<float>(totalActiveNeurons) / brainCount) + ",";
        json += "\"avg_firing_neurons\": " + std::to_string(static_cast<float>(totalFiringNeurons) / brainCount) + ",";
        json += "\"total_spikes\": " + std::to_string(totalSpikes) + ",";
        json += "\"avg_firing_rate\": " + std::to_string(totalFiringRate / brainCount) + "}";
        
        return json;
    }
};

// Diagnostic utilities for troubleshooting
namespace Diagnostic {
    // Collect comprehensive system information
    static std::string collectSystemInfo() {
        std::string info = "NLM System Diagnostics:\n";
        info += "  Operating System: " + std::string(SYSTEM_INFO::os_name) + "\n";
        info += "  Architecture: " + std::string(SYSTEM_INFO::arch) + "\n";
        info += "  Processor Count: " + std::to_string(SYSTEM_INFO::cpu_count) + "\n";
        info += "  Available Memory: " + std::to_string(SYSTEM_INFO::total_memory / (1024 * 1024)) + " MB\n";
        info += "  Python Version: " + std::string(PYTHON_INFO::version) + "\n";
        info += "  NLM Version: " + std::string(NLM_INFO::version) + "\n";
        info += "  Build Type: " + std::string(BUILD_INFO::type) + "\n";
        
        return info;
    }
    
    // Validate system requirements
    static std::string validateSystemRequirements() {
        std::string errors = "";
        
        // Check Python requirements
        if (PYTHON_INFO::version < "3.8") {
            errors += "  - Python 3.8+ required (found: " + std::string(PYTHON_INFO::version) + ")\n";
        }
        
        // Check memory requirements
        if (SYSTEM_INFO::total_memory < 512 * 1024 * 1024) {  // 512 MB minimum
            errors += "  - Minimum 512MB RAM required (found: " + 
                     std::to_string(SYSTEM_INFO::total_memory / (1024 * 1024)) + " MB)\n";
        }
        
        // Check CPU requirements
        if (SYSTEM_INFO::cpu_count < 2) {
            errors += "  - At least 2 CPU cores required (found: " + std::to_string(SYSTEM_INFO::cpu_count) + ")\n";
        }
        
        if (!errors.empty()) {
            return "System requirements validation failed:\n" + errors;
        }
        
        return "System requirements validation passed.\n";
    }
    
    // Check NLM installation
    static std::string validateNLMInstallation() {
        std::string issues = "";
        
        // Check if we can create a basic brain
        try {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                issues += "  - Failed to initialize basic brain\n";
            }
        } catch (const std::exception& e) {
            issues += std::string("  - Exception during basic brain creation: ") + e.what() + "\n";
        }
        
        // Check if we can create world
        try {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(10, 10, 8, 8);
            world->reset();
        } catch (const std::exception& e) {
            issues += std::string("  - Exception during world creation: ") + e.what() + "\n";
        }
        
        if (!issues.empty()) {
            return "NLM installation validation failed:\n" + issues;
        }
        
        return "NLM installation validation passed.\n";
    }
}}

// Advanced simulation utilities
namespace AdvancedSimulation {
    // Run parameter sweep for brain optimization
    static std::vector<std::tuple<float, float, float, float>> optimizeBrainParameters(
        const std::shared_ptr<Config>& baseConfig,
        const std::vector<float>& neuronCounts,
        const std::vector<float>& connectionProbabilities,
        const std::vector<float>& stdpWeights,
        const std::vector<float>& hebbianWeights) {
        
        std::vector<std::tuple<float, float, float, float>> results;
        
        for (float neuronCount : neuronCounts) {
            for (float connProb : connectionProbabilities) {
                for (float stdpWeight : stdpWeights) {
                    for (float hebbianWeight : hebbianWeights) {
                        try {
                            auto config = std::make_shared<Config>(*baseConfig);
                            config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
                            config->set("brain.connection_probability", connProb);
                            config->set("plasticity.stdp.learning_rate", stdpWeight);
                            config->set("plasticity.hebbian.learning_rate", hebbianWeight);
                            
                            auto brain = std::make_shared<Brain>(config);
                            if (brain->initialize()) {
                                // Run quick test simulation
                                for (int step = 0; step < 100; ++step) {
                                    brain->step(step);
                                }
                                
                                float finalFiringRate = brain->getAverageFiringRate();
                                results.emplace_back(neuronCount, connProb, stdpWeight, hebbianWeight, finalFiringRate);
                            }
                        } catch (const std::exception& e) {
                            // Skip invalid parameter combinations
                            continue;
                        }
                    }
                }
            }
        }
        
        return results;
    }
    
    // Run comparative analysis
    static std::string comparativeAnalysis(const std::vector<std::shared_ptr<Brain>>& brains, const std::string& analysisType = "performance") {
        if (brains.empty()) return "No brains to analyze";
        
        std::string analysis = "Comparative Analysis:\n";
        analysis += "  Number of brains analyzed: " + std::to_string(brains.size()) + "\n";
        
        if (analysisType == "performance") {
            float totalNeurons = 0.0f;
            float totalSynapses = 0.0f;
            float totalSpikes = 0.0f;
            float totalFiringRate = 0.0f;
            
            for (const auto& brain : brains) {
                if (brain) {
                    totalNeurons += brain->getTotalNeuronCount();
                    totalSynapses += brain->getTotalSynapseCount();
                    totalSpikes += brain->getTotalSpikeCount();
                    totalFiringRate += brain->getAverageFiringRate();
                }
            }
            
            size_t count = brains.size();
            analysis += "  Average neuron count: " + std::to_string(totalNeurons / count) + "\n";
            analysis += "  Average synapse count: " + std::to_string(totalSynapses / count) + "\n";
            analysis += "  Average total spikes: " + std::to_string(totalSpikes / count) + "\n";
            analysis += "  Average firing rate: " + std::to_string(totalFiringRate / count) + " Hz\n";
            
        } else if (analysisType == "memory") {
            // Memory analysis (would need platform-specific memory tracking)
            analysis += "  Memory analysis requires platform-specific tools\n";
        }
        
        return analysis;
    }
    
    // Run simulation benchmark
    static std::string runSimulationBenchmark(const std::shared_ptr<Brain>& brain, int steps) {
        if (!brain) return "Invalid brain for benchmarking";
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (!brain->initialize()) {
            return "Failed to initialize brain for benchmarking";
        }
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        
        std::string benchmark = "Simulation Benchmark:\n";
        benchmark += "  Steps executed: " + std::to_string(steps) + "\n";
        benchmark += "  Total time: " + std::to_string(duration) + "s\n";
        benchmark += "  Steps per second: " + std::to_string(steps / duration) + "\n";
        benchmark += "  Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz\n";
        
        return benchmark;
    }
}}

// Export helper functions for Python
namespace PythonExport {
    void exportToolkitFunctions(py::module& m) {
        m.def("validate_brain_configuration", &NLMToolkit::validateBrainConfiguration,
              py::arg("config"), "Validate brain configuration");
        
        m.def("create_configuration_profile", &NLMToolkit::createConfigurationProfile,
              py::arg("profile"), "Create configuration profile");
        
        m.def("export_brain_state", &NLMToolkit::exportBrainStateToJSON,
              py::arg("brain"), "Export brain state to JSON");
        
        m.def("profile_simulation", &NLMToolkit::profileSimulation,
              py::arg("brain"), py::arg("steps"), "Profile simulation performance");
        
        m.def("recover_from_brain_error", &NLMToolkit::recoverFromBrainError,
              py::arg("brain"), py::arg("error_context"), "Recover from brain error");
        
        m.def("batch_create_brains", &NLMToolkit::batchCreateBrains,
              py::arg("count"), py::arg("base_config"), "Batch create multiple brains");
        
        m.def("aggregate_brain_metrics", &NLMToolkit::aggregateBrainMetrics,
              py::arg("brains"), "Aggregate metrics from multiple brains");
    }
    
    void exportDiagnosticFunctions(py::module& m) {
        m.def("collect_system_info", &Diagnostic::collectSystemInfo,
              "Collect system diagnostic information");
        
        m.def("validate_system_requirements", &Diagnostic::validateSystemRequirements,
              "Validate system requirements for NLM");
        
        m.def("validate_nlm_installation", &Diagnostic::validateNLMInstallation,
              "Validate NLM installation");
    }
    
    void exportAdvancedSimulationFunctions(py::module& m) {
        m.def("optimize_brain_parameters", &AdvancedSimulation::optimizeBrainParameters,
              py::arg("base_config"), py::arg("neuron_counts"),
              py::arg("connection_probabilities"), py::arg("stdp_weights"),
              py::arg("hebbian_weights"), "Optimize brain parameters");
        
        m.def("comparative_analysis", &AdvancedSimulation::comparativeAnalysis,
              py::arg("brains"), py::arg("analysis_type") = "performance",
              "Run comparative analysis on multiple brains");
        
        m.def("run_simulation_benchmark", &AdvancedSimulation::runSimulationBenchmark,
              py::arg("brain"), py::arg("steps"), "Run simulation benchmark");
    }
}}

// Error handling utilities
namespace ErrorHandling {
    // Global error handler for Python
    class PythonErrorHandler {
    public:
        static void setErrorHandler(std::function<void(const std::string&)> handler) {
            errorHandler_ = handler;
        }
        
        static void handleError(const std::string& message, const std::string& context = "") {
            std::string fullMessage = message;
            if (!context.empty()) {
                fullMessage += " (Context: " + context + ")";
            }
            
            NLM_LOG_ERROR(fullMessage);
            
            if (errorHandler_) {
                errorHandler_(fullMessage);
            }
        }
        
        static void handleWarning(const std::string& message, const std::string& context = "") {
            std::string fullMessage = message;
            if (!context.empty()) {
                fullMessage += " (Context: " + context + ")";
            }
            
            NLM_LOG_WARNING(fullMessage);
        }
        
        static void handleInfo(const std::string& message, const std::string& context = "") {
            std::string fullMessage = message;
            if (!context.empty()) {
                fullMessage += " (Context: " + context + ")";
            }
            
            NLM_LOG_INFO(fullMessage);
        }
        
        static void clearErrorHandler() {
            errorHandler_ = nullptr;
        }
    
    private:
        static std::function<void(const std::string&)> errorHandler_;
    };
    
    std::function<void(const std::string&)> ErrorHandler::errorHandler_ = nullptr;
    
    // Exception safety utilities
    namespace ExceptionSafety {
        // RAII wrapper for exception-safe resource management
        template<typename T>
        class ExceptionSafeResource {
        public:
            explicit ExceptionSafeResource(T resource) : resource_(resource) {}
            
            ~ExceptionSafeResource() {
                try {
                    cleanup();
                } catch (...) {
                    // Log cleanup exception but don't propagate
                    NLM_LOG_ERROR("Exception during resource cleanup");
                }
            }
            
            void cleanup() {
                if (resource_) {
                    // Perform cleanup
                    resource_.reset();
                }
            }
            
            T& get() { return *resource_; }
            const T& get() const { return *resource_; }
            
            bool valid() const { return resource_ != nullptr; }
            
            void reset() {
                cleanup();
                resource_ = nullptr;
            }
            
        private:
            std::shared_ptr<T> resource_;
        };
        
        // Exception-safe brain initialization
        static bool exceptionSafeBrainInit(std::shared_ptr<Brain>& brain, std::shared_ptr<Config> config) {
            try {
                brain = std::make_shared<Brain>(config);
                if (!brain->initialize()) {
                    return false;
                }
                return true;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Exception during brain initialization: ") + e.what());
                return false;
            }
        }
        
        // Exception-safe configuration loading
        static bool exceptionSafeConfigLoad(std::shared_ptr<Config>& config, const std::string& filepath) {
            try {
                config = std::make_shared<Config>();
                if (!config->loadFromFile(filepath)) {
                    return false;
                }
                return true;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Exception during config loading: ") + e.what());
                return false;
            }
        }
    };
    
    // Utility for Python integration
    namespace PythonIntegration {
        // Convert C++ exception to Python exception
        inline void setupPythonExceptionHandling() {
            // This would be called during pybind11 initialization
            // to set up proper exception translation
        }
        
        // Safe Python function registration
        template<typename Func>
        void registerPythonFunction(py::module& m, const std::string& name, Func&& func) {
            try {
                m.def(name, std::forward<Func>(func));
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Failed to register Python function ") + name + ": " + e.what());
            }
        }
        
        // Check Python availability of modules
        static bool checkPythonModules(const std::vector<std::string>& requiredModules) {
            for (const std::string& module : requiredModules) {
                try {
                    py::module::import(module.c_str());
                } catch (const py::error_already_set&) {
                    NLM_LOG_ERROR(std::string("Missing required Python module: ") + module);
                    return false;
                }
            }
            return true;
        }
    };
    
    // Export error handling functions
    void exportErrorHandlingFunctions(py::module& m) {
        m.def("set_error_handler", &ErrorHandling::PythonErrorHandler::setErrorHandler,
              py::arg("handler"), "Set global error handler");
        
        m.def("clear_error_handler", &ErrorHandling::PythonErrorHandler::clearErrorHandler,
              "Clear global error handler");
        
        m.def("handle_error", &ErrorHandling::PythonErrorHandler::handleError,
              py::arg("message"), py::arg("context") = "", "Handle error with context");
        
        m.def("handle_warning", &ErrorHandling::PythonErrorHandler::handleWarning,
              py::arg("message"), py::arg("context") = "", "Handle warning with context");
        
        m.def("handle_info", &ErrorHandling::PythonErrorHandler::handleInfo,
              py::arg("message"), py::arg("context") = "", "Handle info message with context");
    }
}}  // namespace ErrorHandling

// Main module initialization
PYBIND11_MODULE(pynlm_advanced, m) {
    m.doc() = R"pbdoc(
        Advanced NLM Python Module
        ---------------------------
        
        This module provides advanced Python utilities and functions for NLM:
        - Enhanced configuration and toolkit utilities
        - Diagnostic and error handling tools
        - Advanced simulation and analysis functions
        - Exception-safe resource management
        - Python-specific error handling
        
        Use this module for advanced NLM applications requiring additional
        functionality beyond the core NLM API.
    )pbdoc";
    
    // Export toolkit functions
    PythonExport::exportToolkitFunctions(m);
    
    // Export diagnostic functions
    PythonExport::exportDiagnosticFunctions(m);
    
    // Export advanced simulation functions
    PythonExport::exportAdvancedSimulationFunctions(m);
    
    // Export error handling functions
    ErrorHandling::exportErrorHandlingFunctions(m);
    
    // Re-export original API for compatibility
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Export constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Export enumerations
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    py::enum_<ActionType>(m, "ActionType")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();
    
    // Export additional utility functions
    m.def("generate_agent_name", []() -> std::string {
        static std::atomic<size_t> counter{0};
        return "Agent_" + std::to_string(++counter);
    }, "Generate unique agent name");
    
    m.def("validate_agent_configuration", [](const std::shared_ptr<Config>& config) -> bool {
        return PythonUtilities::NLMToolkit::validateBrainConfiguration(config);
    }, py::arg("config"), "Validate agent configuration");
    
    m.def("create_enhanced_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create enhanced agent brain");
}

} // namespace nlm
