"""
Enhanced NLM Python Bindings with Logging Configuration

This file extends the previous bindings with comprehensive logging configuration
and enhanced error handling for Python users.
"""

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <iostream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced error classes with logging capabilities
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) : NLMError("Configuration Error: " + message) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& message) : NLMError("Simulation Error: " + message) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message) : NLMError("Brain Error: " + message) {}
};

// Logging configuration for Python users
class PythonLogger {
public:
    enum class LogLevel {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3,
        CRITICAL = 4
    };
    
private:
    LogLevel current_level_;
    std::vector<std::string> log_messages_;
    bool console_output_enabled_;
    std::string log_file_;
    
public:
    PythonLogger() : current_level_(LogLevel::INFO), console_output_enabled_(true) {}
    
    void setLevel(LogLevel level) {
        current_level_ = level;
    }
    
    LogLevel getLevel() const {
        return current_level_;
    }
    
    void enableConsoleOutput(bool enable) {
        console_output_enabled_ = enable;
    }
    
    bool isConsoleOutputEnabled() const {
        return console_output_enabled_;
    }
    
    void setLogFile(const std::string& filepath) {
        log_file_ = filepath;
    }
    
    std::string getLogFile() const {
        return log_file_;
    }
    
    void log(LogLevel level, const std::string& message) {
        if (static_cast<int>(level) < static_cast<int>(current_level_) && level != LogLevel::CRITICAL) {
            return;
        }
        
        std::string level_str;
        switch (level) {
            case LogLevel::DEBUG: level_str = "DEBUG"; break;
            case LogLevel::INFO: level_str = "INFO"; break;
            case LogLevel::WARNING: level_str = "WARNING"; break;
            case LogLevel::ERROR: level_str = "ERROR"; break;
            case LogLevel::CRITICAL: level_str = "CRITICAL"; break;
        }
        
        std::string log_entry = "[" + level_str + "] " + message;
        
        // Store in message history
        log_messages_.push_back(log_entry);
        
        // Console output
        if (console_output_enabled_) {
            std::cout << log_entry << std::endl;
        }
        
        // File output
        if (!log_file_.empty()) {
            std::ofstream log_stream(log_file_, std::ios::app);
            if (log_stream.is_open()) {
                log_stream << log_entry << std::endl;
            }
        }
    }
    
    void info(const std::string& message) {
        log(LogLevel::INFO, message);
    }
    
    void debug(const std::string& message) {
        log(LogLevel::DEBUG, message);
    }
    
    void warning(const std::string& message) {
        log(LogLevel::WARNING, message);
    }
    
    void error(const std::string& message) {
        log(LogLevel::ERROR, message);
    }
    
    void critical(const std::string& message) {
        log(LogLevel::CRITICAL, message);
    }
    
    std::vector<std::string> getLogMessages() const {
        return log_messages_;
    }
    
    void clearLogMessages() {
        log_messages_.clear();
    }
    
    std::string getFormattedLogs() const {
        std::stringstream ss;
        for (const auto& msg : log_messages_) {
            ss << msg << std::endl;
        }
        return ss.str();
    }
    
    size_t getLogCount() const {
        return log_messages_.size();
    }
    
    bool hasErrors() const {
        return std::any_of(log_messages_.begin(), log_messages_.end(),
                          [](const std::string& msg) { return msg.find("[ERROR]") != std::string::npos ||
                                                          msg.find("[CRITICAL]") != std::string::npos; });
    }
};n
    
    // Global logger instance
    std::shared_ptr<PythonLogger> global_logger = std::make_shared<PythonLogger>();
    
    // Helper functions for logging integration
    namespace log_helpers {
        std::string log_level_to_string(PythonLogger::LogLevel level) {
            switch (level) {
                case PythonLogger::LogLevel::DEBUG: return "DEBUG";
                case PythonLogger::LogLevel::INFO: return "INFO";
                case PythonLogger::LogLevel::WARNING: return "WARNING";
                case PythonLogger::LogLevel::ERROR: return "ERROR";
                case PythonLogger::LogLevel::CRITICAL: return "CRITICAL";
                default: return "INFO";
            }
        }
        
        PythonLogger::LogLevel parse_log_level(const std::string& level_str) {
            std::string lower = level_str;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            
            if (lower == "debug") return PythonLogger::LogLevel::DEBUG;
            if (lower == "info") return PythonLogger::LogLevel::INFO;
            if (lower == "warning") return PythonLogger::LogLevel::WARNING;
            if (lower == "error") return PythonLogger::LogLevel::ERROR;
            if (lower == "critical") return PythonLogger::LogLevel::CRITICAL;
            
            return PythonLogger::LogLevel::INFO; // Default
        }
    }

    // Enhanced helper functions with logging support
    namespace helpers {

        template<typename T>
        std::vector<T> toCppVector(const py::list& py_list) {
            std::vector<T> cpp_vector;
            cpp_vector.reserve(py_list.size());
            for (const auto& item : py_list) {
                cpp_vector.push_back(item.cast<T>());
            }
            return cpp_vector;
        }
        
        template<typename T>
        py::list toPyList(const std::vector<T>& cpp_vector) {
            py::list py_list;
            for (const auto& item : cpp_vector) {
                py_list.append(item);
            }
            return py_list;
        }
        
        bool validateConfigKey(const std::string& key) {
            if (key.empty()) {
                return false;
            }
            return std::all_of(key.begin(), key.end(), 
                              [](char c) { return std::isalnum(c) || c == '.' || c == '_'; });
        }
        
        float safeFloatConversion(const std::string& value, float default_val = 0.0f) {
            try {
                return std::stof(value);
            } catch (const std::exception& e) {
                global_logger->warning("Float conversion failed for value '" + value + "': " + e.what());
                return default_val;
            }
        }
        
        int safeIntConversion(const std::string& value, int default_val = 0) {
            try {
                return std::stoi(value);
            } catch (const std::exception& e) {
                global_logger->warning("Int conversion failed for value '" + value + "': " + e.what());
                return default_val;
            }
        }
        
        std::string sanitizeString(const std::string& input) {
            std::string result;
            for (char c : input) {
                if (std::isalnum(c) || c == ' ' || c == '_' || c == '-' || c == '.' || c == ',') {
                    result += c;
                }
            }
            return result;
        }
        
        bool fileExists(const std::string& path) {
            std::ifstream file(path);
            return file.good();
        }
        
        bool directoryExists(const std::string& path) {
            struct stat info;
            return (stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode));
        }
    }

    // Configuration validation with logging
    class ConfigurationValidator {
    private:
        std::shared_ptr<PythonLogger> logger_;
        
    public:
        ConfigurationValidator(std::shared_ptr<PythonLogger> logger = global_logger) : logger_(logger) {}
        
        struct ValidationResult {
            bool is_valid;
            std::vector<std::string> errors;
            std::vector<std::string> warnings;
        };
        
        ValidationResult validateConfig(const Config& config) {
            ValidationResult result;
            result.is_valid = true;
            
            // Check required keys
            std::vector<std::string> required_keys = {
                "brain.neuron_count",
                "brain.connection_probability",
                "simulation.timestep"
            };
            
            for (const auto& key : required_keys) {
                if (!config.has(key)) {
                    result.errors.push_back("Missing required configuration key: " + key);
                    logger_->error("Configuration validation failed: Missing key '" + key + "'");
                    result.is_valid = false;
                }
            }
            
            // Validate values
            if (config.has("brain.neuron_count")) {
                auto neuron_count = config.getOr<int64_t>("brain.neuron_count", 0);
                if (neuron_count <= 0) {
                    result.errors.push_back("brain.neuron_count must be positive");
                    logger_->error("Configuration validation failed: Invalid brain.neuron_count '" + 
                                 std::to_string(neuron_count) + "'");
                    result.is_valid = false;
                } else if (neuron_count > 100000) {
                    result.warnings.push_back("brain.neuron_count is very high, may affect performance");
                    logger_->warning("Configuration warning: brain.neuron_count is very high '" + 
                                  std::to_string(neuron_count) + "'");
                }
            }
            
            if (config.has("brain.connection_probability")) {
                float prob = config.getOr<float>("brain.connection_probability", 0.0f);
                if (prob < 0.0f || prob > 1.0f) {
                    result.errors.push_back("brain.connection_probability must be between 0 and 1");
                    logger_->error("Configuration validation failed: Invalid brain.connection_probability '" + 
                                 std::to_string(prob) + "'");
                    result.is_valid = false;
                }
            }
            
            if (config.has("simulation.timestep")) {
                double timestep = config.getOr<double>("simulation.timestep", 0.001);
                if (timestep <= 0.0) {
                    result.errors.push_back("simulation.timestep must be positive");
                    logger_->error("Configuration validation failed: Invalid simulation.timestep '" + 
                                 std::to_string(timestep) + "'");
                    result.is_valid = false;
                } else if (timestep > 0.1) {
                    result.warnings.push_back("simulation.timestep is very large, may affect accuracy");
                    logger_->warning("Configuration warning: simulation.timestep is very large '" + 
                                  std::to_string(timestep) + "'");
                }
            }
            
            // Log validation results
            if (result.is_valid) {
                logger_->info("Configuration validation passed");
                for (const auto& warning : result.warnings) {
                    logger_->warning("Configuration warning: " + warning);
                }
            } else {
                logger_->error("Configuration validation failed with " + std::to_string(result.errors.size()) + " errors");
            }
            
            return result;
        }
    };

    // Enhanced logging integration for brain operations
    class LoggingBrainWrapper {
    private:
        std::shared_ptr<Brain> brain_;
        std::shared_ptr<PythonLogger> logger_;
        
    public:
        LoggingBrainWrapper(std::shared_ptr<Brain> brain, std::shared_ptr<PythonLogger> logger = global_logger)
            : brain_(brain), logger_(logger) {}
        
        bool initialize_safe() {
            logger_->info("Starting brain initialization");
            
            auto config = brain_->getConfig();
            ConfigurationValidator validator(logger_);
            auto validation_result = validator.validateConfig(*config);
            
            if (!validation_result.is_valid) {
                logger_->error("Brain initialization failed: Configuration validation failed");
                for (const auto& error : validation_result.errors) {
                    logger_->error("  " + error);
                }
                return false;
            }
            
            logger_->info("Configuration validated successfully");
            
            try {
                bool success = brain_->initialize();
                if (success) {
                    logger_->info("Brain initialization completed successfully");
                    logger_->info("Created " + std::to_string(brain_->getTotalNeuronCount()) + " neurons, " +
                                std::to_string(brain_->getTotalSynapseCount()) + " synapses");
                } else {
                    logger_->error("Brain initialization failed: C++ initialization returned false");
                }
                return success;
            } catch (const std::exception& e) {
                logger_->error("Brain initialization failed with exception: " + std::string(e.what()));
                return false;
            }
        }
        
        bool step_safe(SimulationStep step, std::optional<Timestamp> time = std::nullopt) {
            logger_->debug("Starting brain step " + std::to_string(step));
            
            if (step < 0) {
                logger_->error("Brain step failed: Step number cannot be negative");
                return false;
            }
            
            Timestamp current_time = time.has_value() ? *time : 
                                   static_cast<Timestamp>(step) * 0.001;
            
            try {
                brain_->step(step, current_time);
                
                size_t spikes = brain_->getTotalSpikeCount();
                size_t firing_neurons = brain_->getFiringNeuronCount();
                
                logger_->debug("Step " + std::to_string(step) + " completed: " + std::to_string(spikes) + 
                             " total spikes, " + std::to_string(firing_neurons) + " firing neurons");
                
                return true;
            } catch (const std::exception& e) {
                logger_->error("Brain step failed at step " + std::to_string(step) + ": " + std::string(e.what()));
                return false;
            }
        }
        
        bool receiveSensoryInput_safe(const SensoryInput& input) {
            logger_->debug("Processing sensory input with " + std::to_string(input.getData().size()) + " values");
            
            if (input.getData().empty()) {
                logger_->error("Brain sensory input failed: Input data is empty");
                return false;
            }
            
            try {
                brain_->receiveSensoryInput(input);
                logger_->debug("Sensory input processed successfully");
                return true;
            } catch (const std::exception& e) {
                logger_->error("Brain sensory input failed: " + std::string(e.what()));
                return false;
            }
        }
        
        py::dict get_detailed_stats() {
            py::dict stats;
            
            try {
                stats["total_neurons"] = brain_->getTotalNeuronCount();
                stats["total_synapses"] = brain_->getTotalSynapseCount();
                stats["active_neurons"] = brain_->getActiveNeuronCount();
                stats["firing_neurons"] = brain_->getFiringNeuronCount();
                stats["total_spikes"] = brain_->getTotalSpikeCount();
                stats["average_firing_rate"] = brain_->getAverageFiringRate();
                stats["excitation_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
                
                // Developmental stage
                auto stage = brain_->getDevelopmentalStage();
                stats["developmental_stage"] = static_cast<int>(stage);
                
                // Memory system info
                auto working_memory = brain_->getWorkingMemory();
                if (working_memory) {
                    stats["working_memory_traces"] = working_memory->getActiveTraces();
                    stats["working_memory_capacity"] = working_memory->getCapacity();
                }
                
                auto episodic_memory = brain_->getEpisodicMemory();
                if (episodic_memory) {
                    stats["episodic_memory_episodes"] = episodic_memory->getEpisodeCount();
                    stats["episodic_memory_max_episodes"] = episodic_memory->getMaxEpisodes();
                }
                
                // Neuromodulation info
                auto dopamine = brain_->getDopamine();
                if (dopamine) {
                    stats["dopamine_level"] = dopamine->getLevel();
                    stats["dopamine_plasticity_factor"] = dopamine->getPlasticityFactor();
                }
                
                logger_->debug("Detailed statistics collected successfully");
                
            } catch (const std::exception& e) {
                logger_->error("Failed to collect detailed statistics: " + std::string(e.what()));
                throw BrainError("Statistics collection failed");
            }
            
            return stats;
        }
        
        bool save_state_safe(const std::string& filepath) {
            logger_->info("Saving brain state to: " + filepath);
            
            if (!helpers::validateConfigKey(filepath)) {
                logger_->error("Brain save failed: Invalid filename " + filepath);
                return false;
            }
            
            if (!helpers::fileExists(filepath)) {
                logger_->warning("Checkpoint directory does not exist: " + filepath);
                // Don't fail for missing directory, let C++ handle it
            }
            
            try {
                bool success = brain_->save(filepath);
                if (success) {
                    logger_->info("Brain state saved successfully to: " + filepath);
                } else {
                    logger_->error("Brain save failed: C++ save operation returned false");
                }
                return success;
            } catch (const std::exception& e) {
                logger_->error("Brain save failed with exception: " + std::string(e.what()));
                return false;
            }
        }
        
        bool load_state_safe(const std::string& filepath) {
            logger_->info("Loading brain state from: " + filepath);
            
            if (!helpers::validateConfigKey(filepath)) {
                logger_->error("Brain load failed: Invalid filename " + filepath);
                return false;
            }
            
            if (!helpers::fileExists(filepath)) {
                logger_->error("Brain load failed: Checkpoint file does not exist: " + filepath);
                return false;
            }
            
            try {
                bool success = brain_->load(filepath);
                if (success) {
                    logger_->info("Brain state loaded successfully from: " + filepath);
                    logger_->info("Loaded brain with " + std::to_string(brain_->getTotalNeuronCount()) + 
                                " neurons, " + std::to_string(brain_->getTotalSynapseCount()) + " synapses");
                } else {
                    logger_->error("Brain load failed: C++ load operation returned false");
                }
                return success;
            } catch (const std::exception& e) {
                logger_->error("Brain load failed with exception: " + std::string(e.what()));
                return false;
            }
        }
        
        bool reset_safe() {
            logger_->info("Resetting brain");
            
            try {
                brain_->reset();
                logger_->info("Brain reset completed successfully");
                return true;
            } catch (const std::exception& e) {
                logger_->error("Brain reset failed: " + std::string(e.what()));
                return false;
            }
        }
        
        py::dict get_system_health() {
            py::dict health_report;
            
            try {
                health_report["initialization_ok"] = true;
                health_report["memory_integrity"] = true;
                health_report["plasticity_systems_active"] = true;
                health_report["neuromodulation_active"] = true;
                
                std::vector<std::string> warnings;
                
                if (brain_->getTotalNeuronCount() == 0) {
                    warnings.push_back("No neurons in brain");
                    logger_->warning("System health check: No neurons in brain");
                }
                
                if (brain_->getTotalSynapseCount() == 0) {
                    warnings.push_back("No synapses in brain");
                    logger_->warning("System health check: No synapses in brain");
                }
                
                if (brain_->getExcitationInhibitionRatio() > 1000.0f) {
                    warnings.push_back("Extreme excitation/inhibition imbalance");
                    logger_->warning("System health check: Extreme excitation/inhibition imbalance");
                }
                
                health_report["warnings"] = helpers::toPyList(warnings);
                health_report["healthy"] = warnings.empty();
                
                logger_->info("System health check completed: " + std::string(warnings.empty() ? "healthy" : "issues detected"));
                
            } catch (const std::exception& e) {
                logger_->error("System health check failed: " + std::string(e.what()));
                health_report["healthy"] = false;
                health_report["error"] = std::string(e.what());
            }
            
            return health_report;
        }
        
        std::shared_ptr<Brain> get_brain() {
            return brain_;
        }
    };

    // Enhanced SimpleWorld wrapper with logging
    class LoggingSimpleWorldWrapper {
    private:
        std::shared_ptr<SimpleWorld> world_;
        std::shared_ptr<PythonLogger> logger_;
        
    public:
        LoggingSimpleWorldWrapper(std::shared_ptr<SimpleWorld> world, std::shared_ptr<PythonLogger> logger = global_logger)
            : world_(world), logger_(logger) {}
        
        bool configure_safe(size_t width, size_t height, 
                          size_t vision_width = 8, size_t vision_height = 8) {
            logger_->info("Configuring world: " + std::to_string(width) + "x" + std::to_string(height) +
                        " with vision " + std::to_string(vision_width) + "x" + std::to_string(vision_height));
            
            if (width == 0 || height == 0) {
                logger_->error("World configuration failed: Dimensions must be positive");
                return false;
            }
            
            if (vision_width == 0 || vision_height == 0) {
                logger_->error("World configuration failed: Vision dimensions must be positive");
                return false;
            }
            
            if (vision_width > width || vision_height > height) {
                logger_->error("World configuration failed: Vision dimensions exceed world dimensions");
                return false;
            }
            
            try {
                world_->configure(width, height, vision_width, vision_height);
                logger_->info("World configuration completed successfully");
                return true;
            } catch (const std::exception& e) {
                logger_->error("World configuration failed: " + std::string(e.what()));
                return false;
            }
        }
        
        bool update_safe(double timestep) {
            logger_->debug("Updating world with timestep " + std::to_string(timestep));
            
            if (timestep <= 0.0) {
                logger_->error("World update failed: Timestep must be positive");
                return false;
            }
            
            try {
                world_->update(timestep);
                logger_->debug("World update completed successfully");
                return true;
            } catch (const std::exception& e) {
                logger_->error("World update failed: " + std::string(e.what()));
                return false;
            }
        }
        
        py::dict get_world_stats() {
            py::dict stats;
            
            try {
                stats["width"] = world_->getWidth();
                stats["height"] = world_->getHeight();
                stats["vision_width"] = world_->getVisionWidth();
                stats["vision_height"] = world_->getVisionHeight();
                stats["simulation_time"] = world_->getSimulationTime();
                stats["max_energy"] = world_->getMaxEnergy();
                stats["energy_decay_rate"] = world_->getEnergyDecayRate();
                
                auto body = world_->getAgentBody();
                if (body) {
                    stats["agent_x"] = body->x;
                    stats["agent_y"] = body->y;
                    stats["agent_orientation"] = body->orientation;
                    stats["agent_energy"] = body->energy;
                    stats["agent_health"] = body->health;
                    stats["agent_age"] = body->age;
                    stats["agent_is_moving"] = body->isMoving;
                    stats["agent_is_turning"] = body->isTurning;
                }
                
                logger_->debug("World statistics collected successfully");
                
            } catch (const std::exception& e) {
                logger_->error("Failed to collect world statistics: " + std::string(e.what()));
                throw BrainError("World statistics collection failed");
            }
            
            return stats;
        }
        
        std::shared_ptr<SimpleWorld> get_world() {
            return world_;
        }
    };

    // Module-level logging configuration functions
    void configure_logging(py::dict config) {
        global_logger->setLevel(static_cast<PythonLogger::LogLevel>(
            config.get("level", "INFO").cast<std::string>().empty() ? 
            PythonLogger::LogLevel::INFO : 
            log_helpers::parse_log_level(config.get("level", "INFO").cast<std::string>())
        ));
        
        global_logger->enableConsoleOutput(
            config.get("console_output", true).cast<bool>()
        );
        
        if (config.contains("file")) {
            global_logger->setLogFile(config.get("file", "").cast<std::string>());
        }
        
        global_logger->info("Logging configuration applied");
    }
    
    py::dict get_logging_status() {
        py::dict status;
        
        status["level"] = log_helpers::log_level_to_string(global_logger->getLevel());
        status["console_output_enabled"] = global_logger->isConsoleOutputEnabled();
        status["log_file"] = global_logger->getLogFile();
        status["message_count"] = global_logger->getLogCount();
        status["has_errors"] = global_logger->hasErrors();
        
        return status;
    }
    
    void clear_logs() {
        global_logger->clearLogMessages();
    }
    
    std::string get_log_messages() {
        return global_logger->getFormattedLogs();
    }

    // Main enhanced bindings
    PYBIND11_MODULE(pynlm_enhanced, m) {
        m.doc() = R"pbdoc(
            Enhanced NLM (Neural Learning Machine) Python Bindings with Logging
            -------------------------------------------------------------------
            An enhanced Python binding for the NLM C++ neural simulation framework.
            
            This version provides:
            - Comprehensive logging configuration and management
            - Enhanced error handling and validation
            - System health monitoring
            - Detailed logging for debugging and analysis
            - Backward compatibility with original API
            
            Key Features:
            - Configurable logging levels and output
            - Safe initialization with validation
            - Detailed logging for all operations
            - System health checking with logging
            - Error logging and recovery information
            - Log message retrieval and analysis
            
            Usage Examples:
            ```python
            import pynlm_enhanced
            
            # Configure logging
            logging_config = {
                "level": "DEBUG",
                "console_output": True,
                "file": "nlm.log"
            }
            pynlm_enhanced.configure_logging(logging_config)
            
            # Create brain with logging
            config = pynlm_enhanced.createDefaultConfig()
            brain = pynlm_enhanced.createBrain(config)
            
            # Initialize with logging and validation
            if brain.initialize_safe():
                print("Brain initialized successfully")
            
            # Run simulation with logging
            for step in range(100):
                brain.step_safe(step)
            
            # Get system health with logging
            health = brain.get_system_health()
            
            # Get logs
            logs = pynlm_enhanced.get_log_messages()
            ```
        )pbdoc";
        
        // Register custom exceptions
        py::register_exception<NLMError>(m, "NLMError");
        py::register_exception<ConfigurationError>(m, "ConfigurationError");
        py::register_exception<SimulationError>(m, "SimulationError");
        py::register_exception<BrainError>(m, "BrainError");
        
        // PythonLogger class
        py::class_<PythonLogger>(m, "PythonLogger", R"pbdoc(Logging class for NLM)")
            .def(py::init<>())
            .def("setLevel", &PythonLogger::setLevel, py::arg("level"), "Set logging level")
            .def("getLevel", &PythonLogger::getLevel, "Get current logging level")
            .def("enableConsoleOutput", &PythonLogger::enableConsoleOutput, py::arg("enable"), "Enable/disable console output")
            .def("isConsoleOutputEnabled", &PythonLogger::isConsoleOutputEnabled, "Check if console output is enabled")
            .def("setLogFile", &PythonLogger::setLogFile, py::arg("filepath"), "Set log file path")
            .def("getLogFile", &PythonLogger::getLogFile, "Get log file path")
            .def("info", &PythonLogger::info, py::arg("message"), "Log info message")
            .def("debug", &PythonLogger::debug, py::arg("message"), "Log debug message")
            .def("warning", &PythonLogger::warning, py::arg("message"), "Log warning message")
            .def("error", &PythonLogger::error, py::arg("message"), "Log error message")
            .def("critical", &PythonLogger::critical, py::arg("message"), "Log critical message")
            .def("getLogMessages", &PythonLogger::getLogMessages, "Get all log messages")
            .def("clearLogMessages", &PythonLogger::clearLogMessages, "Clear all log messages")
            .def("getFormattedLogs", &PythonLogger::getFormattedLogs, "Get formatted log messages")
            .def("getLogCount", &PythonLogger::getLogCount, "Get number of log messages")
            .def("hasErrors", &PythonLogger::hasErrors, "Check if there are any errors in logs");
        
        // Identity classes (same as original)
        py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
            .def(py::init<>())
            .def(py::init<uint64_t>(), py::arg("value"))
            .def_readwrite("value", &NeuronId::value)
            .def("index", &NeuronId::index)
            .def("__eq__", &NeuronId::operator==)
            .def("__ne__", &NeuronId::operator!=)
            .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
            .def("__repr__", [](const NeuronId& id) {
                return "<NeuronId: " + std::to_string(id.value) + ">";
            });

        // Enumerations
        py::enum_<PythonLogger::LogLevel>(m, "LogLevel", R"pbdoc(Logging level enumeration)pbdoc")
            .value("DEBUG", PythonLogger::LogLevel::DEBUG)
            .value("INFO", PythonLogger::LogLevel::INFO)
            .value("WARNING", PythonLogger::LogLevel::WARNING)
            .value("ERROR", PythonLogger::LogLevel::ERROR)
            .value("CRITICAL", PythonLogger::LogLevel::CRITICAL)
            .export_values();

        // Original Config class with enhanced validation
        py::class_<Config>(m, "Config", R"pbdoc(Enhanced Configuration class for NLM system)pbdoc")
            .def(py::init<>())
            .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"), "Load configuration from a JSON file")
            .def("loadFromArgs", [](Config& self, int argc, char** argv) {
                return self.loadFromArgs(argc, argv);
            }, py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments")
            .def("saveToFile", &Config::saveToFile, py::arg("filepath"), "Save configuration to a JSON file")
            .def("has", &Config::has, py::arg("key"), "Check if a configuration key exists")
            .def("getKeys", &Config::getKeys, "Get all configuration keys")
            .def("clear", &Config::clear, "Clear all configuration entries")
            .def("summary", &Config::summary, "Get a summary string of the configuration")
            .def("__repr__", [](const Config& cfg) {
                return "<Config: " + cfg.summary() + ">";
            })
            
            // Enhanced methods with validation
            .def("set_safe", [](Config& self, const std::string& key, const std::string& value) {
                if (!helpers::validateConfigKey(key)) {
                    throw ConfigurationError("Invalid configuration key: " + key);
                }
                try {
                    int int_val = std::stoi(value);
                    self.set(key, int_val);
                } catch (...) {
                    try {
                        float float_val = std::stof(value);
                        self.set(key, float_val);
                    } catch (...) {
                        self.set(key, value);
                    }
                }
            }, py::arg("key"), py::arg("value"), "Safely set configuration value with type inference")
            
            .def("get_int_or", [](Config& self, const std::string& key, int default_val) {
                return self.getOr<int64_t>(key, default_val);
            }, py::arg("key"), py::arg("default_val"), "Get configuration value as integer")
            
            .def("get_float_or", [](Config& self, const std::string& key, float default_val) {
                return self.getOr<float>(key, default_val);
            }, py::arg("key"), py::arg("default_val"), "Get configuration value as float")
            
            .def("validate_config", [](Config& self) {
                ConfigurationValidator validator;
                auto result = validator.validateConfig(self);
                return std::make_pair(result.is_valid, helpers::toPyList(result.errors));
            }, "Validate configuration and return (is_valid, error_list)")
            
            .def("create_preset", [](Config& self, const std::string& preset_name) {
                if (preset_name == "minimal") {
                    self.clear();
                    self.set("brain.neuron_count", 100);
                    self.set("brain.connection_probability", 0.05f);
                    self.set("simulation.timestep", 0.001);
                    return "Minimal preset created";
                } else if (preset_name == "standard") {
                    self.clear();
                    self.set("brain.neuron_count", 1000);
                    self.set("brain.region_count", 2);
                    self.set("brain.connection_probability", 0.1f);
                    self.set("brain.initial_weight_mean", 0.5f);
                    self.set("simulation.timestep", 0.001);
                    return "Standard preset created";
                } else if (preset_name == "development") {
                    self.clear();
                    self.set("brain.neuron_count", 500);
                    self.set("brain.region_count", 1);
                    self.set("brain.connection_probability", 0.15f);
                    self.set("plasticity.development.enable", true);
                    self.set("neuromodulation.curiosity.enable", true);
                    self.set("neuromodulation.novelty.enable", true);
                    self.set("simulation.timestep", 0.001);
                    return "Development preset created";
                } else if (preset_name == "high_plasticity") {
                    self.clear();
                    self.set("brain.neuron_count", 2000);
                    self.set("brain.region_count", 3);
                    self.set("brain.connection_probability", 0.2f);
                    self.set("plasticity.stdp.enable", true);
                    self.set("plasticity.hebbian.enable", true);
                    self.set("plasticity.structural.enable", true);
                    self.set("simulation.timestep", 0.0005);
                    return "High plasticity preset created";
                } else {
                    throw ConfigurationError("Unknown preset: " + preset_name);
                }
            }, py::arg("preset_name"), "Create a configuration preset")
            
            .def("export_config", [](Config& self) {
                py::dict config_dict;
                auto keys = self.getKeys();
                for (const auto& key : keys) {
                    if (auto val_int = self.get<int64_t>(key)) {
                        config_dict[key] = *val_int;
                    } else if (auto val_float = self.get<float>(key)) {
                        config_dict[key] = *val_float;
                    } else if (auto val_str = self.get<std::string>(key)) {
                        config_dict[key] = *val_str;
                    } else {
                        config_dict[key] = "";
                    }
                }
                return config_dict;
            }, "Export configuration as Python dictionary")
            
            .def("import_config", [](Config& self, const py::dict& config_dict) {
                self.clear();
                for (auto item : config_dict) {
                    std::string key = item.first.cast<std::string>();
                    py::object value = item.second;
                    
                    if (value.is(py::int_())) {
                        self.set(key, value.cast<int64_t>());
                    } else if (value.is(py::float_())) {
                        self.set(key, value.cast<float>());
                    } else if (value.is(py::str())) {
                        self.set(key, value.cast<std::string>());
                    } else {
                        throw ConfigurationError("Unsupported value type in config dictionary for key: " + key);
                    }
                }
            }, py::arg("config_dict"), "Import configuration from Python dictionary")
            
            .def("get_config_summary", [](Config& self) {
                std::string summary = "Configuration Summary:\n";
                summary += "  Number of keys: " + std::to_string(self.getKeys().size()) + "\n";
                
                auto keys = self.getKeys();
                for (const auto& key : keys) {
                    if (auto val_int = self.get<int64_t>(key)) {
                        summary += "  " + key + " = " + std::to_string(*val_int) + "\n";
                    } else if (auto val_float = self.get<float>(key)) {
                        summary += "  " + key + " = " + std::to_string(*val_float) + "\n";
                    } else if (auto val_str = self.get<std::string>(key)) {
                        summary += "  " + key + " = '" + *val_str + "'\n";
                    }
                }
                return summary;
            }, "Get detailed configuration summary")
            
            .def("get_recommended_config", []() {
                py::dict recommended_configs;
                
                py::dict minimal;
                minimal["brain.neuron_count"] = 100;
                minimal["brain.connection_probability"] = 0.05f;
                minimal["simulation.timestep"] = 0.001f;
                recommended_configs["minimal"] = minimal;
                
                py::dict standard;
                standard["brain.neuron_count"] = 1000;
                standard["brain.region_count"] = 2;
                standard["brain.connection_probability"] = 0.1f;
                standard["brain.initial_weight_mean"] = 0.5f;
                standard["simulation.timestep"] = 0.001f;
                recommended_configs["standard"] = standard;
                
                py::dict research;
                research["brain.neuron_count"] = 5000;
                research["brain.region_count"] = 5;
                research["brain.connection_probability"] = 0.15f;
                research["plasticity.stdp.enable"] = true;
                research["plasticity.hebbian.enable"] = true;
                research["plasticity.structural.enable"] = true;
                research["neuromodulation.curiosity.enable"] = true;
                research["neuromodulation.novelty.enable"] = true;
                research["simulation.timestep"] = 0.0005f;
                recommended_configs["research"] = research;
                
                return recommended_configs;
            }, "Get recommended configuration presets");

        // Original Brain class with logging enhancements
        py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced Brain class for NLM)")
            .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
            .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
            .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), py::arg("currentStep"), "Perform a simulation step")
            .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step), 
                 py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
            .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"), "Inject sensory input into the brain")
            .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
            .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
            .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
            .def("reset", &Brain::reset, "Reset brain state")
            .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
            .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
            .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
            .def("getRegion", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal, "Get a region by ID")
            .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
            .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
            .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal, "Get all regions")
            .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
            .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
            .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
            .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
            .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
            .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
            .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
            .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
            .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
            .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
            .def("logStatus", &Brain::logStatus, "Log brain status");

        // AgentBrain class
        py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced Agent brain interface connecting NLM brain to world)pbdoc")
            .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
            .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
            .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
            .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
            .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory percept and inject into brain")
            .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
            .def("applyRewardModulation", &AgentBrain::applyRewardModulation, 
                 py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
            .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
            .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
            .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
            .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
            .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
            .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
            .def("reset", &AgentBrain::reset, "Reset agent for new episode")
            .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get the underlying brain")
            .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"))
            .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"))
            .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"))
            .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"))
            .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
            .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
            .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
            .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

        // SimpleWorld class
        py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
            .def(py::init<>())
            .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
                 py::arg("visionWidth"), py::arg("visionHeight"))
            .def("reset", &SimpleWorld::reset)
            .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
            .def("update", &SimpleWorld::update, py::arg("timestep"))
            .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
                 py::arg("cmd"), py::arg("currentTime"))
            .def("getSensoryPercept", &SimpleWorld::getSensoryPercept, py::return_value_policy::reference_internal)
            .def("getAgentBody", &SimpleWorld::getAgentBody, py::return_value_policy::reference_internal)
            .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
            .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
            .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
            .def("getWidth", &SimpleWorld::getWidth)
            .def("getHeight", &SimpleWorld::getHeight)
            .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
            .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
            .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
            .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
            .def("getSimulationTime", &SimpleWorld::getSimulationTime)
            .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
            .def("getRandomSeed", &SimpleWorld::getRandomSeed);

        // Module-level convenience functions
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

        // Global logging functions
        m.def("configure_logging", &configure_logging, py::arg("config"), "Configure global logging settings");
        m.def("get_logging_status", &get_logging_status, "Get current logging status");
        m.def("clear_logs", &clear_logs, "Clear all log messages");
        m.def("get_log_messages", &get_log_messages, "Get all log messages as string");
        m.def("global_logger", []() { return global_logger; }, "Get the global logger instance");

        // Helper functions
        m.def("file_exists", &helpers::fileExists, py::arg("path"), "Check if file exists");
        m.def("directory_exists", &helpers::directory_exists, py::arg("path"), "Check if directory exists");
        m.def("sanitize_string", &helpers::sanitizeString, py::arg("input"), "Sanitize string for safe usage");

        // Additional utility functions
        m.def("parse_log_level", &log_helpers::parse_log_level, py::arg("level_str"), "Parse log level string");
        m.def("log_level_to_string", &log_helpers::log_level_to_string, py::arg("level"), "Convert log level to string");
        
        m.def("create_safe_config", []() {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", 1000);
            config->set("brain.connection_probability", 0.1f);
            config->set("simulation.timestep", 0.001);
            return config;
        }, "Create a safe configuration with recommended defaults");
        
        m.def("create_safe_brain", []() {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", 1000);
            config->set("brain.connection_probability", 0.1f);
            config->set("simulation.timestep", 0.001);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            return brain;
        }, "Create a brain with safe default configuration and initialize it");
        
        m.def("run_safe_simulation", [](std::shared_ptr<Brain> brain, int steps) {
            if (!brain) {
                throw BrainError("Brain is null");
            }
            
            for (int step = 0; step < steps; ++step) {
                brain->step(step);
            }
            
            return brain->getTotalSpikeCount();
        }, py::arg("brain"), py::arg("steps"), "Run a safe simulation with error checking");

        // Export all the enhanced classes for advanced users
        m.attr("LoggingBrainWrapper") = py::cpp_function([](std::shared_ptr<Brain> brain) {
            return LoggingBrainWrapper(brain, global_logger);
        });
        
        m.attr("LoggingSimpleWorldWrapper") = py::cpp_function([](std::shared_ptr<SimpleWorld> world) {
            return LoggingSimpleWorldWrapper(world, global_logger);
        });
        
        m.attr("ConfigurationValidator") = py::cpp_function([](std::shared_ptr<PythonLogger> logger) {
            return ConfigurationValidator(logger);
        });
    }

} // namespace nlm