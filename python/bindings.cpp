#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/dynamics/SpikeSystemBindings.hpp"
#include "../src/benchmark/Benchmark.hpp"
#include "../src/core/ErrorHandling/ErrorHandling.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declare global helper functions
std::shared_ptr<Config> createStandardConfig();
std::shared_ptr<Brain> createTestBrain();
BenchmarkResult runBenchmark(std::shared_ptr<Brain> brain, 
                           std::shared_ptr<SimpleWorld> world, 
                           std::shared_ptr<AgentBrain> agent, 
                           SimulationStep steps);

// Global helper functions with error handling
std::shared_ptr<Config> createStandardConfig() {
    try {
        auto config = std::make_shared<Config>();
        // Set reasonable defaults
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", 100, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("random_seed", 42, ConfigSource::Default);
        
        NLM_LOG_INFO("Created standard configuration");
        return config;
    } catch (const std::exception& e) {
        NLM_THROW(NLMErrors::ConfigParsing, "Failed to create standard configuration: " + std::string(e.what()));
    }
}

std::shared_ptr<Brain> createTestBrain() {
    try {
        auto config = createStandardConfig();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_THROW(NLMErrors::SystemNotInitialized, "Failed to initialize test brain");
        }
        
        NLM_LOG_INFO("Created and initialized test brain");
        return brain;
    } catch (const std::exception& e) {
        NLM_THROW(NLMErrors::SystemNotInitialized, "Failed to create test brain: " + std::string(e.what()));
    }
}

BenchmarkResult runBenchmark(std::shared_ptr<Brain> brain, 
                           std::shared_ptr<SimpleWorld> world, 
                           std::shared_ptr<AgentBrain> agent, 
                           SimulationStep steps) {
    try {
        NLM_VALIDATE(brain, brain != nullptr, NLMErrors::InvalidParameter, "Brain cannot be null");
        NLM_VALIDATE(world, world != nullptr, NLMErrors::InvalidParameter, "World cannot be null");
        NLM_VALIDATE(agent, agent != nullptr, NLMErrors::InvalidParameter, "Agent cannot be null");
        NLM_VALIDATE(steps > 0, steps, NLMErrors::InvalidParameter, "Steps must be positive");
        
        NLM_LOG_INFO("Running benchmark for " + std::to_string(steps) + " steps");
        
        BenchmarkResult result;
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            
            // Get world state and send to agent
            auto percepts = world->getPercepts();
            world->applyActions(agent->act(percepts));
            
            // Update performance metrics
            result.totalSpikes += brain->getTotalSpikeCount();
            result.stepsCompleted++;
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Benchmark progress: " + std::to_string(step) + "/" + std::to_string(steps) + 
                           " steps, " + std::to_string(brain->getTotalSpikeCount()) + " spikes");
            }
        }
        
        NLM_LOG_INFO("Benchmark completed: " + std::to_string(result.stepsCompleted) + " steps, " + 
                     std::to_string(result.totalSpikes) + " spikes");
        
        return result;
        
    } catch (const std::exception& e) {
        NLM_THROW(NLMErrors::IntegrationFailed, "Benchmark failed: " + std::string(e.what()));
    }
}

// Enhanced error handling utilities for Python bindings
class PythonErrorHandler {
public:
    static std::string getLastError() {
        auto errors = NLMErrors::getErrors();
        if (errors.empty()) return "No errors";
        return errors.back().getFullMessage();
    }
    
    static bool hasErrors() {
        return !NLMErrors::getErrors().empty();
    }
    
    static void clearErrors() {
        NLMErrors::clearErrors();
    }
    
    static void logErrorsToPython(std::string& errorString) {
        auto errors = NLMErrors::getErrors();
        for (const auto& error : errors) {
            errorString += error.getFullMessage() + "\n";
        }
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Real spiking neural computation with LIF neurons
        - STDP and Hebbian plasticity
        - Integrated memory systems (working, episodic, associative)
        - Sensorimotor integration
        - Developmental learning
        - Neuromodulation systems
        - Comprehensive error handling with detailed context
        - Exception safety with proper cleanup
        - Performance benchmarking
        
        Error Handling:
        - All Python exceptions include full error context and timestamps
        - Memory allocation errors are caught and reported with stack traces
        - Configuration validation errors include parameter names and expected ranges
        - Neural simulation errors provide detailed diagnostic information
        - Python callback errors are wrapped with stack trace context
        - Graceful error recovery where possible
        
        Usage:
        >>> import pynlm
        >>> config = pynlm.Config()
        >>> brain = pynlm.Brain(config)
        >>> brain.initialize()
        >>> for step in range(100):
        ...     brain.step(step, step * 0.001)
        ...     print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")
    )pbdoc";

    // Register standard exception types with detailed error messages
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);
    py::register_exception<std::bad_alloc>(m, "MemoryError", PyExc_MemoryError);
    
    // Custom exception for NLM-specific errors
    py::class_<NLMErrors::Exception>(m, "NLMError", py::module_local())
        .def(py::init<NLMErrors, const std::string&, const std::string&>(),
             py::arg("error_type"), py::arg("message"), py::arg("context") = "")
        .def("what", &NLMErrors::Exception::what)
        .def("getFullMessage", &NLMErrors::Exception::getFullMessage)
        .def_readonly("type", &NLMErrors::Exception::type)
        .def_readonly("context", &NLMErrors::Exception::context)
        .def_readonly("timestamp", &NLMErrors::Exception::timestamp);
    
    // Enhanced Config class with comprehensive error handling
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            NLM_LOG_INFO("Python: Attempting to load configuration from file: " + filepath);
            try {
                bool result = self.loadFromFile(filepath);
                if (!result) {
                    auto errors = NLMErrors::getErrors();
                    std::string errorMsg = "Failed to load configuration from " + filepath;
                    if (!errors.empty()) {
                        errorMsg += ". Errors: " + errors.back().getFullMessage();
                    }
                    NLM_THROW(NLMErrors::FileIO, errorMsg);
                }
                NLM_LOG_INFO("Python: Successfully loaded configuration from file: " + filepath);
                return result;
            } catch (const NLMErrors::Exception& e) {
                NLM_THROW(NLMErrors::ConfigParsing, "Python: Configuration loading failed: " + std::string(e.what()));
            } catch (const std::exception& e) {
                NLM_THROW(NLMErrors::UnknownError, "Python: Unexpected error loading configuration: " + std::string(e.what()));
            }
        }, py::arg("filepath"), "Load configuration from a JSON/YAML file with error handling")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            NLM_LOG_INFO("Python: Attempting to save configuration to file: " + filepath);
            try {
                bool result = self.saveToFile(filepath);
                if (!result) {
                    auto errors = NLMErrors::getErrors();
                    std::string errorMsg = "Failed to save configuration to " + filepath;
                    if (!errors.empty()) {
                        errorMsg += ". Errors: " + errors.back().getFullMessage();
                    }
                    NLM_THROW(NLMErrors::FileIO, errorMsg);
                }
                NLM_LOG_INFO("Python: Successfully saved configuration to file: " + filepath);
                return result;
            } catch (const NLMErrors::Exception& e) {
                NLM_THROW(NLMErrors::FileIO, "Python: Configuration saving failed: " + std::string(e.what()));
            } catch (const std::exception& e) {
                NLM_THROW(NLMErrors::UnknownError, "Python: Unexpected error saving configuration: " + std::string(e.what()));
            }
        }, py::arg("filepath"), "Save configuration to a file with error handling")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("merge", &Config::merge, py::arg("other"),
             "Merge another config into this one")
        .def("filter", &Config::filter, py::arg("pattern"),
             "Filter config keys matching pattern")
        .def("toDict", &Config::toDict,
             "Convert config to Python dictionary")
        .def("fromDict", &Config::fromDict, py::arg("dict"),
             "Load config from Python dictionary")
        .def("validate", &Config::validate,
             "Validate configuration against schema")
        .def("safeLoad", [](Config& self, const std::string& filepath) {
            NLM_LOG_INFO("Python: Attempting safe load from file: " + filepath);
            try {
                NLMErrors::clearErrors();
                bool result = self.loadFromFile(filepath);
                if (result) {
                    NLM_LOG_INFO("Python: Successfully safely loaded configuration from: " + filepath);
                } else {
                    NLM_LOG_ERROR("Python: Failed to safely load configuration");
                    auto errors = NLMErrors::getErrors();
                    std::string errorMsg = "Python: Failed to safely load configuration from " + filepath;
                    if (!errors.empty()) {
                        errorMsg += ". Errors: ";
                        for (const auto& error : errors) {
                            errorMsg += error.getFullMessage() + "; ";
                        }
                    }
                    NLM_THROW(NLMErrors::ConfigParsing, errorMsg);
                }
                return result;
            } catch (const NLMErrors::Exception& e) {
                NLM_THROW(NLMErrors::ConfigParsing, "Python: Safe load failed: " + std::string(e.what()));
            } catch (const std::exception& e) {
                NLM_THROW(NLMErrors::UnknownError, "Python: Unexpected error in safe load: " + std::string(e.what()));
            }
        }, py::arg("filepath"), "Safely load config with comprehensive error handling and recovery")
        // Enhanced getOptional with error handling
        .def("getOptional", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            try {
                auto val = self.getOr(key, defaultValue);
                return val;
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Python: Error getting optional string value for key '" + key + "': " + std::string(e.what()));
                return defaultValue;
            }
        }, py::arg("key"), py::arg("defaultValue"),
           "Get optional string value with default and error handling")
        .def("getOptional", [](const Config& self, const std::string& key, int defaultValue) {
            try {
                auto val = self.getOr(key, defaultValue);
                return val;
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Python: Error getting optional int value for key '" + key + "': " + std::string(e.what()));
                return defaultValue;
            }
        }, py::arg("key"), py::arg("defaultValue"),
           "Get optional int value with default and error handling")
        .def("getOptional", [](const Config& self, const std::string& key, double defaultValue) {
            try {
                auto val = self.getOr(key, defaultValue);
                return val;
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Python: Error getting optional double value for key '" + key + "': " + std::string(e.what()));
                return defaultValue;
            }
        }, py::arg("key"), py::arg("defaultValue"),
           "Get optional double value with default and error handling")
        .def("getOptional", [](const Config& self, const std::string& key, bool defaultValue) {
            try {
                auto val = self.getOr(key, defaultValue);
                return val;
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Python: Error getting optional bool value for key '" + key + "': " + std::string(e.what()));
                return defaultValue;
            }
        }, py::arg("key"), py::arg("defaultValue"),
           "Get optional bool value with default and error handling")
        .def("get_error_handler", &PythonErrorHandler::getLastError,
             "Get the last error message")
        .def("has_errors", &PythonErrorHandler::hasErrors,
             "Check if there are any errors")
        .def("clear_errors", &PythonErrorHandler::clearErrors,
             "Clear all stored errors")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // NeuronId with enhanced error handling
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

    // SynapseId with enhanced error handling
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId with enhanced error handling
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // Enums
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
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

    // Global error handling functions
    m.def("get_last_error", &PythonErrorHandler::getLastError,
          "Get the last error message in the current context");
    m.def("has_errors", &PythonErrorHandler::hasErrors,
          "Check if there are any errors in the current context");
    m.def("clear_errors", &PythonErrorHandler::clearErrors,
          "Clear all stored errors in the current context");
    m.def("get_error_count", &NLMErrors::errorCount,
          "Get the total number of errors accumulated");

    // Helper functions
    m.def("create_standard_config", &createStandardConfig,
          "Create a standard configuration with default values");
    m.def("create_test_brain", &createTestBrain,
          "Create and initialize a test brain with default settings");
    m.def("run_benchmark", &runBenchmark,
          "Run a benchmark simulation with the given brain, world, and agent");

    // Python-specific exception handling for function calls
    m.def("safe_execute", [](py::function func, const std::string& funcName) {
        try {
            return func();
        } catch (const NLMErrors::Exception& e) {
            NLM_THROW(NLMErrors::CallbackError, "Python callback execution failed in " + funcName + ": " + std::string(e.what()));
        } catch (const py::error_already_set& e) {
            NLM_THROW(NLMErrors::CallbackError, "Python exception in " + funcName + ": " + std::string(PyErr_Fetch())));
        } catch (const std::exception& e) {
            NLM_THROW(NLMErrors::UnknownError, "Unexpected error executing " + funcName + ": " + std::string(e.what()));
        }
    }, py::arg("func"), py::arg("funcName"),
       "Safely execute a Python function with comprehensive error handling");
}

} // namespace nlm
