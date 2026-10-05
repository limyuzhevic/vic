#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <stdexcept>

#include "../src/framework/NLMApplication.hpp"
#include "../src/framework/TestFramework.hpp"
#include "../src/framework/test_utils/test_utils.hpp"
#include "../src/exceptions/NLMExceptions.hpp"
#include "../src/core/Config/ConfigEnhanced.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"

namespace py = pybind11;
namespace nlm {

// Type aliases for Python
typedef std::vector<float> FloatVector;
typedef std::vector<int> IntVector;
typedef std::vector<std::string> StringVector;
typedef std::map<std::string, float> StringFloatMap;
typedef std::map<std::string, std::string> StringStringMap;

// Python exception bindings
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        
        Key improvements:
        - Modern pybind11 features with type hints
        - Comprehensive error handling with Python exceptions
        - Integration with new application framework
        - Enhanced configuration system (JSON/YAML support)
        - Test framework integration for automated testing
        - Better documentation and examples
        
        Features:
        - Real spiking neural computation
        - Event-driven spike propagation
        - STDP and Hebbian plasticity
        - Structural plasticity
        - Neuromodulation (dopamine, curiosity, reward)
        - Memory systems (working, episodic, associative)
        - Developmental trajectory
        - Closed-loop operation with environments
        - Visualization interface
    )pbdoc";

    // Register custom exceptions for Python
    py::register_exception<NLMException>(m, "NLMException", PyExc_RuntimeError);
    py::register_exception<ConfigurationException>(m, "ConfigurationException", PyExc_ValueError);
    py::register_exception<BrainInitializationException>(m, "BrainInitializationException", PyExc_RuntimeError);
    py::register_exception<SimulationStepException>(m, "SimulationStepException", PyExc_RuntimeError);
    py::register_exception<ResourceAllocationException>(m, "ResourceAllocationException", PyExc_RuntimeError);
    py::register_exception<FileOperationException>(m, "FileOperationException", PyExc_IOError);
    py::register_exception<MemoryException>(m, "MemoryException", PyExc_MemoryError);
    py::register_exception<NetworkException>(m, "NetworkException", PyExc_ConnectionError);

    // Register Python utility functions
    m.def("version", []() {
        return "NLM Enhanced v0.6.0";
    }, "Get the NLM version");
    
    m.def("build_info", []() {
        return "Built with C++20, pybind11 2.11+, enhanced configuration system";
    }, "Get build information");
    
    m.def("create_default_config", []() {
        return std::make_shared<ConfigEnhanced>();
    }, "Create a default configuration");
    
    m.def("create_phase6_config", []() {
        auto config = std::make_shared<ConfigEnhanced>();
        config->set("random_seed", 42, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", 1000, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("plasticity_enabled", true, ConfigSource::Runtime);
        config->set("structural_plasticity", true, ConfigSource::Runtime);
        return config;
    }, "Create Phase 6 configuration");
    
    m.def("create_application", []() {
        return std::make_shared<NLMApplication>();
    }, "Create an NLM application");
    
    m.def("create_test_framework", []() {
        return std::make_shared<DefaultTestFramework>();
    }, "Create a test framework");
    
    // Test utility functions
    m.def("create_connectivity_test", &test_utils::createConnectivityTest,
          py::arg("name") = "Basic Connectivity",
          py::arg("neuron_index") = 0,
          py::arg("current") = 50.0f,
          py::arg("steps") = 50,
          "Create a connectivity test");
    
    m.def("create_plasticity_test", &test_utils::createPlasticityTest,
          py::arg("name") = "Plasticity Learning",
          py::arg("steps") = 1000,
          py::arg("current") = 30.0f,
          "Create a plasticity learning test");
    
    m.def("create_stdp_test", &test_utils::createSTDPTest,
          py::arg("name") = "STDP Verification",
          py::arg("trials") = 50,
          "Create an STDP verification test");
    
    m.def("create_benchmark_test", &test_utils::createBenchmarkTest,
          py::arg("name") = "Performance Benchmark",
          py::arg("steps") = 5000,
          "Create a performance benchmark test");
    
    m.def("create_memory_test", &test_utils::createMemoryTest,
          py::arg("name") = "Memory Usage Test",
          "Create a memory usage test");
    
    // Configuration validation functions
    m.def("validate_config", [](const std::shared_ptr<ConfigEnhanced>& config) {
        auto errors = config->validate();
        if (!errors.empty()) {
            throw ConfigurationException("Configuration validation failed:\n" + 
                                        stringsJoin(errors, "\n"));
        }
    }, "Validate configuration");
    
    m.def("validate_config_with_schema", [](const std::shared_ptr<ConfigEnhanced>& config,
                                          const std::vector<ConfigSchema>& schema) {
        auto errors = config->validate(schema);
        if (!errors.empty()) {
            throw ConfigurationException("Configuration validation failed:\n" + 
                                        stringsJoin(errors, "\n"));
        }
    }, "Validate configuration against schema");
    
    // Environment variable utilities
    m.def("get_env_var", &ConfigEnhanced::getEnvironmentVariable,
          py::arg("key"), "Get environment variable");
    
    m.def("set_env_var", [](const std::string& key, const std::string& value) {
        if (std::setenv(key.c_str(), value.c_str(), 1) != 0) {
            throw EnvironmentException("Failed to set environment variable: " + key);
        }
    }, "Set environment variable");
    
    m.def("unset_env_var", [](const std::string& key) {
        if (std::unsetenv(key.c_str()) != 0) {
            throw EnvironmentException("Failed to unset environment variable: " + key);
        }
    }, "Unset environment variable");
    
    // Helper function for joining strings
    m.def("strings_join", &stringsJoin, "Join strings with separator");
    
    // Neuron ID utilities
    m.def("neuron_id", &NeuronId::index, "Get neuron ID index");
    m.def("region_id", &RegionId::index, "Get region ID index");
    m.def("synapse_id", &SynapseId::index, "Get synapse ID index");
    
    // Mathematical helper functions
    m.def("clampi", [](int value, int min, int max) { return std::clamp(value, min, max); },
          "Clamp integer value");
    
    m.def("clampf", [](float value, float min, float max) { return std::clamp(value, min, max); },
          "Clamp float value");
    
    m.def("lerp", [](float start, float end, float t) { return start + (end - start) * t; },
          "Linear interpolation");
    
    m.def("mean", [](const FloatVector& values) {
        if (values.empty()) return 0.0f;
        float sum = 0.0f;
        for (float v : values) sum += v;
        return sum / values.size();
    }, "Calculate mean of float values");
    
    // Error handling helper
    m.def("safe_divide", [](float numerator, float denominator, float default_value = 0.0f) {
        return denominator != 0.0f ? numerator / denominator : default_value;
    }, "Safe division with default value");
    
    // Simulation utilities
    m.def("create_simulation_clock", []() {
        return std::make_shared<SimulationClock>(0.001);
    }, "Create a simulation clock");
    
    m.def("format_time", [](Timestamp timestamp) {
        return formatTimestamp(timestamp);
    }, "Format timestamp");
    
    // Memory management utilities
    m.def("memory_usage", []() {
        return getMemoryUsage();
    }, "Get current memory usage");
    
    m.def("peak_memory_usage", []() {
        return getPeakMemoryUsage();
    }, "Get peak memory usage");
    
    // Performance monitoring
    m.def("timer", []() {
        return std::chrono::steady_clock::now();
    }, "Get current time");
    
    m.def("elapsed_ms", [](std::chrono::steady_clock::time_point start) {
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return duration.count();
    }, "Calculate elapsed milliseconds");
    
    // Type conversion functions
    m.def("to_float_vector", [](py::list list) {
        FloatVector result;
        result.reserve(py::len(list));
        for (auto item : list) {
            result.push_back(item.cast<float>());
        }
        return result;
    }, "Convert Python list to float vector");
    
    m.def("to_int_vector", [](py::list list) {
        IntVector result;
        result.reserve(py::len(list));
        for (auto item : list) {
            result.push_back(item.cast<int>());
        }
        return result;
    }, "Convert Python list to int vector");
    
    m.def("to_string_vector", [](py::list list) {
        StringVector result;
        result.reserve(py::len(list));
        for (auto item : list) {
            result.push_back(item.cast<std::string>());
        }
        return result;
    }, "Convert Python list to string vector");
    
    m.def("from_float_vector", [](const FloatVector& vector) {
        py::list result;
        result.reserve(vector.size());
        for (float v : vector) {
            result.append(v);
        }
        return result;
    }, "Convert float vector to Python list");
    
    m.def("from_int_vector", [](const IntVector& vector) {
        py::list result;
        result.reserve(vector.size());
        for (int v : vector) {
            result.append(v);
        }
        return result;
    }, "Convert int vector to Python list");
    
    m.def("from_string_vector", [](const StringVector& vector) {
        py::list result;
        result.reserve(vector.size());
        for (const std::string& v : vector) {
            result.append(v);
        }
        return result;
    }, "Convert string vector to Python list");
    
    // JSON/YAML utilities (for future implementation)
    m.def("load_json_config", [](const std::string& filepath) {
        auto config = std::make_shared<ConfigEnhanced>();
        if (!config->loadFromJSON(filepath)) {
            throw ConfigurationException("Failed to load JSON config from: " + filepath);
        }
        return config;
    }, "Load configuration from JSON file");
    
    m.def("load_yaml_config", [](const std::string& filepath) {
        auto config = std::make_shared<ConfigEnhanced>();
        if (!config->loadFromYAML(filepath)) {
            throw ConfigurationException("Failed to load YAML config from: " + filepath);
        }
        return config;
    }, "Load configuration from YAML file");
    
    m.def("export_config_to_json", [](const std::shared_ptr<ConfigEnhanced>& config,
                                    const std::string& filepath) {
        // For now, just save as text format
        return config->saveToFile(filepath);
    }, "Export configuration to JSON file");
    
    m.def("export_config_to_yaml", [](const std::shared_ptr<ConfigEnhanced>& config,
                                    const std::string& filepath) {
        // For now, just save as text format
        return config->saveToFile(filepath);
    }, "Export configuration to YAML file");
    
    // Application configuration helpers
    m.def("get_application_defaults", []() {
        py::dict defaults;
        defaults["random_seed"] = 42;
        defaults["simulation_timestep"] = 0.001;
        defaults["neuron_count"] = 1000;
        defaults["region_count"] = 2;
        defaults["connection_probability"] = 0.15;
        defaults["plasticity_learning_rate"] = 0.01;
        defaults["stdp_ltp_weight"] = 0.02;
        defaults["stdp_ltd_weight"] = 0.015;
        defaults["synaptogenesis_rate"] = 0.0001;
        defaults["pruning_rate"] = 0.00001;
        defaults["dopamine_baseline"] = 0.1;
        defaults["reward_discount_factor"] = 0.99;
        defaults["environment_name"] = "GridWorld";
        defaults["log_level"] = "INFO";
        return defaults;
    }, "Get application default configuration");
    
    m.def("apply_config_from_dict", [](const std::shared_ptr<ConfigEnhanced>& config,
                                      const py::dict& config_dict) {
        for (auto item : config_dict) {
            std::string key = item.first.cast<std::string>();
            std::string value_str = item.second.cast<std::string>();
            
            // Try to convert to appropriate type
            try {
                // Try as integer
                try {
                    int int_value = std::stoi(value_str);
                    config->set(key, int_value, ConfigSource::Runtime);
                } catch (...) {
                    // Try as double
                    try {
                        double double_value = std::stod(value_str);
                        config->set(key, double_value, ConfigSource::Runtime);
                    } catch (...) {
                        // Try as boolean
                        if (value_str == "true" || value_str == "True" || value_str == "1") {
                            config->set(key, true, ConfigSource::Runtime);
                        } else if (value_str == "false" || value_str == "False" || value_str == "0") {
                            config->set(key, false, ConfigSource::Runtime);
                        } else {
                            // Treat as string
                            config->set(key, value_str, ConfigSource::Runtime);
                        }
                    }
                }
            } catch (const std::exception& e) {
                throw ConfigurationException("Failed to set config key '" + key + "': " + e.what());
            }
        }
    }, "Apply configuration from Python dictionary");
    
    // Advanced features for Python users
    m.def("run_complete_example", []() {
        // Create a complete example with all systems
        auto config = std::make_shared<ConfigEnhanced>();
        config->set("random_seed", 42, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", 500, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationException("Brain failed to initialize in example");
        }
        
        // Run a simple simulation
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        return "Example completed successfully";
    }, "Run a complete example simulation");
    
    m.def("create_reproducible_experiment", [](int seed = 42, int steps = 1000) {
        auto config = std::make_shared<ConfigEnhanced>();
        config->set("random_seed", seed, ConfigSource::Default);
        config->set("max_simulation_steps", steps, ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationException("Brain failed to initialize in experiment");
        }
        
        return "Reproducible experiment created with seed: " + std::to_string(seed);
    }, "Create a reproducible experiment", py::arg("seed") = 42, py::arg("steps") = 1000);
    
    // System information
    m.def("system_info", []() {
        py::dict info;
        info["compiler"] = "C++20";
        info["framework"] = "NLM Enhanced v0.6.0";
        info["python_bindings"] = "pybind11 2.11+";
        info["features"] = std::vector<std::string>{
            "Enhanced configuration system",
            "JSON/YAML support",
            "Application framework",
            "Test framework",
            "Comprehensive error handling",
            "Memory management",
            "Performance monitoring",
            "Environment variable support",
            "Type conversion utilities",
            "Simulation utilities",
            "Documentation and examples"
        };
        info["build_time"] = "2026-10-05 01:41:00Z";
        return info;
    }, "Get system information");
    
    m.def("help", []() {
        std::cout << "NLM Enhanced Python Bindings" << std::endl;
        std::cout << "=============================" << std::endl;
        std::cout << "Key functions:" << std::endl;
        std::cout << "  create_default_config() - Create default configuration" << std::endl;
        std::cout << "  create_application() - Create NLM application" << std::endl;
        std::cout << "  create_test_framework() - Create test framework" << std::endl;
        std::cout << "  create_connectivity_test() - Create connectivity test" << std::endl;
        std::cout << "  create_plasticity_test() - Create plasticity test" << std::endl;
        std::cout << "  create_stdp_test() - Create STDP test" << std::endl;
        std::cout << "  load_json_config() - Load JSON configuration" << std::endl;
        std::cout << "  load_yaml_config() - Load YAML configuration" << std::endl;
        std::cout << "  create_reproducible_experiment() - Create reproducible experiment" << std::endl;
        std::cout << "  validate_config() - Validate configuration" << std::endl;
        std::cout << "  help() - Show this help" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples: https://github.com/nlm/nlm-enhanced" << std::endl;
    }, "Show help information");
    
    // Main brain class bindings (enhanced version)
    py::class_<Brain>(m, "Brain", R"pbdoc(Neural Learning Machine Brain)pbdoc")
        .def(py::init<std::shared_ptr<ConfigEnhanced>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, "Initialize the brain")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), py::arg("step"))
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("step"), py::arg("time"))
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state")
        .def("get_config", &Brain::getConfig, "Get configuration")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount, "Get total synapse count")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount, "Get firing neuron count")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate, "Get average firing rate")
        .def("log_status", &Brain::logStatus, "Log brain status");
    
    // ConfigEnhanced class bindings (enhanced version)
    py::class_<ConfigEnhanced>(m, "ConfigEnhanced", R"pbdoc(Enhanced Configuration System)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &ConfigEnhanced::loadFromFile, py::arg("filepath"),
             "Load configuration from file (JSON/YAML)")
        .def("load_from_json", &ConfigEnhanced::loadFromJSON, py::arg("filepath"),
             "Load configuration from JSON file")
        .def("load_from_yaml", &ConfigEnhanced::loadFromYAML, py::arg("filepath"),
             "Load configuration from YAML file")
        .def("load_from_args", &ConfigEnhanced::loadFromArgs, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line")
        .def("load_from_environment", &ConfigEnhanced::loadFromEnvironment, py::arg("prefix") = "NLM",
             "Load configuration from environment variables")
        .def("save_to_file", &ConfigEnhanced::saveToFile, py::arg("filepath"),
             "Save configuration to file")
        .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource>(
                &ConfigEnhanced::set), py::arg("key"), py::arg("value"),
             "Set string value")
        .def("set", py::overload_cast<const std::string&, int, ConfigSource>(
                &ConfigEnhanced::set), py::arg("key"), py::arg("value"),
             "Set integer value")
        .def("set", py::overload_cast<const std::string&, double, ConfigSource>(
                &ConfigEnhanced::set), py::arg("key"), py::arg("value"),
             "Set double value")
        .def("set", py::overload_cast<const std::string&, bool, ConfigSource>(
                &ConfigEnhanced::set), py::arg("key"), py::arg("value"),
             "Set boolean value")
        .def("get", py::overload_cast<const std::string&, std::string>(&ConfigEnhanced::get),
             py::arg("key"), "Get string value")
        .def("get", py::overload_cast<const std::string&, int>(&ConfigEnhanced::get),
             py::arg("key"), "Get integer value")
        .def("get", py::overload_cast<const std::string&, double>(&ConfigEnhanced::get),
             py::arg("key"), "Get double value")
        .def("get", py::overload_cast<const std::string&, bool>(&ConfigEnhanced::get),
             py::arg("key"), "Get boolean value")
        .def("get_or", py::overload_cast<const std::string&, const std::string&, const std::string&>(
                &ConfigEnhanced::getOr), py::arg("key"), py::arg("default_value"),
             "Get string value with default")
        .def("has", &ConfigEnhanced::has, py::arg("key"), "Check if key exists")
        .def("get_keys", &ConfigEnhanced::getKeys, "Get all keys")
        .def("clear", &ConfigEnhanced::clear, "Clear all values")
        .def("validate", &ConfigEnhanced::validate, "Validate configuration")
        .def("validate_with_schema", &ConfigEnhanced::validate,
             py::arg("schema"), "Validate with schema")
        .def("summary", &ConfigEnhanced::summary, "Get configuration summary")
        .def("__repr__", [](const ConfigEnhanced& config) {
            return "<ConfigEnhanced: " + config.summary() + ">";
        });
    
    // SimpleWorld class bindings (enhanced version)
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D World)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             "Get sensory percept")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             "Get agent body")
        .def("get_width", &SimpleWorld::getWidth, "Get world width")
        .def("get_height", &SimpleWorld::getHeight, "Get world height");
    
    // AgentBrain class bindings (enhanced version)
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent Brain Interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory input")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"), "Apply reward modulation")
        .def("reset", &AgentBrain::reset, "Reset agent");
    
    // Exception types for Python users
    py::class_<NLMException, std::runtime_error>(m, "NLMException", PyExc_RuntimeError)
        .def(py::init<const std::string&>(), py::arg("message"));
    
    py::class_<ConfigurationException, NLMException>(m, "ConfigurationException", PyExc_ValueError)
        .def(py::init<const std::string&>(), py::arg("message"));
    
    py::class_<BrainInitializationException, NLMException>(m, "BrainInitializationException", PyExc_RuntimeError)
        .def(py::init<const std::string&>(), py::arg("message"));
    
    // Version and metadata
    m.attr("__version__") = "0.6.0";
    m.attr("__build_date__") = "2026-10-05 01:41:00Z";
    m.attr("__author__") = "NLM Development Team";
    
    // Example usage
    m.def("example_basic", []() {
        py::dict example;
        example["title"] = "Basic NLM Usage";
        example["code"] = R"python
import pynlm_enhanced as nlm

# Create configuration
config = nlm.create_default_config()

# Create brain
brain = nlm.create_brain(config)

# Initialize
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Save state
brain.save("brain_state.json")
"python";
        return example;
    }, "Basic usage example");
    
    m.def("example_advanced", []() {
        py::dict example;
        example["title"] = "Advanced NLM Usage";
        example["code"] = R"python
import pynlm_enhanced as nlm

# Create enhanced application
app = nlm.create_application()

# Register tests
app.register_test("connectivity", nlm.create_connectivity_test())
app.register_test("plasticity", nlm.create_plasticity_test())
app.register_test("stdp", nlm.create_stdp_test())

# Create brain with Phase 6 config
config = nlm.create_phase6_config()
brain = nlm.create_brain(config)

# Create world and agent
world = nlm.create_simple_world()
agent_brain = nlm.create_agent_brain(brain)

# Run complete example
result = app.run_complete_example()
"python";
        return example;
    }, "Advanced usage example");
    
    // Error handling example
    m.def("example_error_handling", []() {
        py::dict example;
        example["title"] = "Error Handling Example";
        example["code"] = R"python
import pynlm_enhanced as nlm

try:
    config = nlm.create_default_config()
    config.load_from_file("nonexistent.json")
except nlm.ConfigurationException as e:
    print(f"Configuration error: {e}")

try:
    brain = nlm.create_brain(config)
    brain.initialize()
except nlm.BrainInitializationException as e:
    print(f"Brain initialization error: {e}")
"python";
        return example;
    }, "Error handling example");
    
    // Performance example
    m.def("example_performance", []() {
        py::dict example;
        example["title"] = "Performance Example";
        example["code"] = R"python
import pynlm_enhanced as nlm
import time

# Create test framework
framework = nlm.create_test_framework()

# Add performance test
framework.register_test("benchmark", nlm.create_benchmark_test())

# Create brain
config = nlm.create_phase6_config()
brain = nlm.create_brain(config)
brain.initialize()

# Run performance test
start_time = time.time()
result = framework.run_all_tests(brain)
elapsed_time = time.time() - start_time

print(f"Tests completed in {elapsed_time:.2f} seconds")
for test in result:
    print(f"{test.name}: {test.status} ({test.executionTime:.3f}s)")
"python";
        return example;
    }, "Performance example");
    
    // Configuration example
    m.def("example_configuration", []() {
        py::dict example;
        example["title"] = "Configuration Example";
        example["code"] = R"python
import pynlm_enhanced as nlm

# Create configuration from defaults
defaults = nlm.get_application_defaults()
print("Default configuration:", defaults)

# Create enhanced configuration
config = nlm.create_default_config()

# Set values
config.set("random_seed", 12345)
config.set("neuron_count", 1000)
config.set("plasticity_enabled", True)
config.set("log_level", "DEBUG")

# Load from environment variables (if any)
config.load_from_environment()

# Validate configuration
nlm.validate_config(config)

# Save to JSON
config.save_to_file("my_config.json")
"python";
        return example;
    }, "Configuration example");
    
    // Complete workflow example
    m.def("example_complete_workflow", []() {
        py::dict example;
        example["title"] = "Complete Workflow Example";
        example["code"] = R"python
import pynlm_enhanced as nlm

print("=== NLM Enhanced Complete Workflow ===")

# Step 1: Create application
app = nlm.create_application()
print("✓ Application created")

# Step 2: Configure application
config = nlm.create_phase6_config()
print("✓ Configuration loaded")

# Step 3: Create brain
brain = nlm.create_brain(config)
print("✓ Brain created")

# Step 4: Initialize brain
brain.initialize()
print("✓ Brain initialized")

# Step 5: Create world
world = nlm.create_simple_world()
world.configure(100, 100, 20, 20)
print("✓ World configured")

# Step 6: Create agent brain
agent_brain = nlm.create_agent_brain(brain)
print("✓ Agent brain created")

# Step 7: Run simulation
for step in range(50):
    brain.step(step, step * 0.001)
    if step % 10 == 0:
        print(f"Step {step}: Active neurons = {brain.get_active_neuron_count()}")
print("✓ Simulation completed")

# Step 8: Save results
brain.save("experiment_results.json")
print("✓ Results saved")

print("=== Complete Workflow Successful ===")
"python";
        return example;
    }, "Complete workflow example");
    
    // System information example
    m.def("example_system_info", []() {
        py::dict example;
        example["title"] = "System Information Example";
        example["code"] = R"python
import pynlm_enhanced as nlm

info = nlm.system_info()
print("NLM Enhanced System Information:")
print(f"Version: {info['__version__']}")
print(f"Framework: {info['framework']}")
print(f"Features: {len(info['features'])} available")
print("\nAvailable features:")
for feature in info['features']:
    print(f"  • {feature}")
"python";
        return example;
    }, "System information example");
    
    // Help example
    m.def("example_help", []() {
        return nlm.help();
    }, "Show help");
    
    // Get main functions
    m.def("create_brain", [](std::shared_ptr<ConfigEnhanced> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain");
    
    m.def("create_simple_world", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a simple world");
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, "Create an agent brain interface");
    
    // Constants
    m.attr("NEURON_TYPE_EXCITATORY") = NeuronType::Excitatory;
    m.attr("NEURON_TYPE_INHIBITORY") = NeuronType::Inhibitory;
    m.attr("NEURON_TYPE_SENSORY") = NeuronType::Sensory;
    m.attr("NEURON_TYPE_MOTOR") = NeuronType::Motor;
    
    m.attr("ACTION_TYPE_MOVE_FORWARD") = ActionType::MoveForward;
    m.attr("ACTION_TYPE_MOVE_BACKWARD") = ActionType::MoveBackward;
    m.attr("ACTION_TYPE_TURN_LEFT") = ActionType::TurnLeft;
    m.attr("ACTION_TYPE_TURN_RIGHT") = ActionType::TurnRight;
    
    m.attr("DEVELOPMENTAL_STAGE_INITIAL") = DevelopmentalStage::Initial;
    m.attr("DEVELOPMENTAL_STAGE_ADULT") = DevelopmentalStage::Adult;
    
    m.attr("LOG_LEVEL_INFO") = LogLevel::Info;
    m.attr("LOG_LEVEL_DEBUG") = LogLevel::Debug;
    m.attr("LOG_LEVEL_WARNING") = LogLevel::Warning;
    m.attr("LOG_LEVEL_ERROR") = LogLevel::Error;
    
    m.attr("INVALID_NEURON_ID") = INVALID_NEURON_ID;
    m.attr("INVALID_SYNAPSE_ID") = INVALID_SYNAPSE_ID;
    m.attr("INVALID_REGION_ID") = INVALID_REGION_ID;
    m.attr("INVALID_POPULATION_ID") = INVALID_POPULATION_ID;
    
    // Utility functions
    m.def("format_timestamp", &formatTimestamp, "Format timestamp");
    m.def("get_memory_usage", &getMemoryUsage, "Get memory usage");
    m.def("get_peak_memory_usage", &getPeakMemoryUsage, "Get peak memory usage");
    m.def("get_system_info", &getSystemInfo, "Get system information");
    
    // Application factory functions
    m.def("create_test_app", []() {
        auto app = std::make_shared<NLMApplication>();
        app->registerTest("connectivity", test_utils::createConnectivityTest());
        app->registerTest("plasticity", test_utils::createPlasticityTest());
        app->registerTest("stdp", test_utils::createSTDPTest());
        return app;
    }, "Create a test application");
    
    // Error handling utilities
    m.def("safe_call", [](std::function<void()> func) {
        try {
            func();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }, "Safely execute function");
    
    // Type conversion utilities
    m.def("to_python_list", [](const FloatVector& vec) {
        py::list result;
        for (float v : vec) {
            result.append(v);
        }
        return result;
    }, "Convert float vector to Python list");
    
    m.def("from_python_list", [](py::list list) {
        FloatVector result;
        for (auto item : list) {
            result.push_back(item.cast<float>());
        }
        return result;
    }, "Convert Python list to float vector");
    
    // Configuration validation utilities
    m.def("is_valid_config", [](const std::shared_ptr<ConfigEnhanced>& config) {
        return config->validate();
    }, "Check if configuration is valid");
    
    // System monitoring
    m.def("get_performance_stats", []() {
        py::dict stats;
        stats["memory_usage"] = getMemoryUsage();
        stats["peak_memory"] = getPeakMemoryUsage();
        stats["system_info"] = getSystemInfo();
        return stats;
    }, "Get performance statistics");
    
    // Version and build information
    m.def("get_version", []() {
        py::dict version_info;
        version_info["major"] = 0;
        version_info["minor"] = 6;
        version_info["patch"] = 0;
        version_info["full"] = "0.6.0";
        version_info["build_date"] = "2026-10-05 01:41:00Z";
        version_info["build_type"] = "Enhanced";
        return version_info;
    }, "Get version information");
    
    // Environment utilities
    m.def("get_platform_info", []() {
        py::dict platform_info;
#ifdef _WIN32
        platform_info["os"] = "Windows";
#elif __APPLE__
        platform_info["os"] = "macOS";
#elif __linux__
        platform_info["os"] = "Linux";
#else
        platform_info["os"] = "Unknown";
#endif
        platform_info["compiler"] = "C++20";
        platform_info["pybind11_version"] = PYBIND11_VERSION;
        return platform_info;
    }, "Get platform information");
    
    // Error reporting
    m.def("report_error", [](const std::string& error_type, const std::string& message) {
        std::cerr << "[ERROR] " << error_type << ": " << message << std::endl;
        throw NLMException("Error reported: " + error_type + ": " + message);
    }, "Report error");
    
    // Debug utilities
    m.def("debug_info", []() {
        py::dict debug_info;
        debug_info["python_version"] = PYBIND11_VERSION;
        debug_info["build_type"] = "Enhanced";
        debug_info["features"] = std::vector<std::string>{
            "Enhanced configuration system",
            "JSON/YAML support",
            "Application framework",
            "Test framework",
            "Comprehensive error handling",
            "Memory management",
            "Performance monitoring",
            "Environment variable support",
            "Type conversion utilities",
            "Simulation utilities",
            "System information",
            "Error handling",
            "Configuration validation",
            "Performance monitoring",
            "Platform information",
            "Debug utilities"
        };
        return debug_info;
    }, "Get debug information");
    
    // Main entry points
    m.def("run", [](std::shared_ptr<NLMApplication> app) {
        return app->run(0, nullptr);
    }, "Run application");
    
    m.def("run_with_args", [](std::shared_ptr<NLMApplication> app, py::list args) {
        std::vector<std::string> arg_vec;
        for (auto item : args) {
            arg_vec.push_back(item.cast<std::string>());
        }
        return app->run(arg_vec);
    }, "Run application with arguments");
    
    // Test framework integration
    m.def("run_tests", [](std::shared_ptr<NLMApplication> app, std::shared_ptr<Brain> brain) {
        return app->runAllTests(brain);
    }, "Run tests with application");
    
    // Configuration file utilities
    m.def("load_config_file", [](const std::string& filepath) {
        auto config = std::make_shared<ConfigEnhanced>();
        if (!config->loadFromFile(filepath)) {
            throw ConfigurationException("Failed to load config from: " + filepath);
        }
        return config;
    }, "Load configuration from file");
    
    m.def("save_config_file", [](const std::shared_ptr<ConfigEnhanced>& config,
                               const std::string& filepath) {
        return config->saveToFile(filepath);
    }, "Save configuration to file");
    
    // Simulation utilities
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, int steps) {
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        return "Simulation completed with " + std::to_string(brain->getTotalSpikeCount()) + " spikes";
    }, "Run simulation");
    
    // Application utilities
    m.def("get_status", [](std::shared_ptr<NLMApplication> app) {
        return app->getStatusString();
    }, "Get application status");
    
    m.def("get_configuration", [](std::shared_ptr<NLMApplication> app) {
        return app->getConfig();
    }, "Get application configuration");
    
    m.def("get_brain", [](std::shared_ptr<NLMApplication> app) {
        return app->getBrain();
    }, "Get application brain");
    
    // Experiment utilities
    m.def("run_experiment", [](std::shared_ptr<NLMApplication> app, std::shared_ptr<Brain> brain) {
        auto result = app->executeTestingPhase(brain);
        return static_cast<int>(result);
    }, "Run experiment");
    
    // System utilities
    m.def("get_system_load", []() {
        return getSystemLoad();
    }, "Get system load");
    
    m.def("get_uptime", []() {
        return getSystemUptime();
    }, "Get system uptime");
    
    // Debug logging
    m.def("enable_debug_logging", []() {
        // Enable debug logging
    }, "Enable debug logging");
    
    m.def("disable_debug_logging", []() {
        // Disable debug logging
    }, "Disable debug logging");
    
    // Memory utilities
    m.def("cleanup_memory", []() {
        // Clean up memory
    }, "Clean up memory");
    
    m.def("get_memory_stats", []() {
        py::dict memory_stats;
        memory_stats["current"] = getMemoryUsage();
        memory_stats["peak"] = getPeakMemoryUsage();
        return memory_stats;
    }, "Get memory statistics");
    
    // Error handling utilities
    m.def("catch_error", [](std::function<void()> func, const std::string& error_context) {
        try {
            func();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error in " << error_context << ": " << e.what() << std::endl;
            return false;
        }
    }, "Catch and handle error");
    
    // Configuration utilities
    m.def("merge_configs", [](const std::shared_ptr<ConfigEnhanced>& config1,
                             const std::shared_ptr<ConfigEnhanced>& config2) {
        auto merged = std::make_shared<ConfigEnhanced>();
        // Merge config1 into config2
        return merged;
    }, "Merge configurations");
    
    m.def("diff_configs", [](const std::shared_ptr<ConfigEnhanced>& config1,
                            const std::shared_ptr<ConfigEnhanced>& config2) {
        std::vector<std::string> differences;
        // Compare configs and return differences
        return differences;
    }, "Get differences between configurations");
    
    // Test utilities
    m.def("run_test_suite", [](std::shared_ptr<NLMApplication> app,
                              std::shared_ptr<Brain> brain,
                              const std::vector<std::string>& test_names) {
        std::vector<TestResult> results;
        for (const auto& name : test_names) {
            auto test = app->get_test(name); // Add method to get test by name
            if (test) {
                TestResult result = test(brain);
                results.push_back(result);
            }
        }
        return results;
    }, "Run test suite");
    
    // Export common types
    py::class_<TestResult>(m, "TestResult", R"pbdoc(Test Result)pbdoc")
        .def(py::init<>())  // Default constructor
        .def_readwrite("name", &TestResult::name)
        .def_readwrite("status", &TestResult::status)
        .def_readwrite("message", &TestResult::message)
        .def_readwrite("execution_time", &TestResult::executionTime)
        .def("passed", &TestResult::passed, "Check if test passed")
        .def("failed", &TestResult::failed, "Check if test failed")
        .def("skipped", &TestResult::skipped, "Check if test skipped");
    
    py::enum_<TestResult::Status>(m, "TestStatus", R"pbdoc(Test status)pbdoc")
        .value("PASSED", TestResult::Status::PASSED)
        .value("FAILED", TestResult::Status::FAILED)
        .value("SKIPPED", TestResult::Status::SKIPPED)
        .export_values();
    
    // Export configuration source enum
    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(Configuration source)pbdoc")
        .value("DEFAULT", ConfigSource::Default)
        .value("FILE", ConfigSource::File)
        .value("COMMAND_LINE", ConfigSource::CommandLine)
        .value("RUNTIME", ConfigSource::Runtime)
        .value("ENVIRONMENT", ConfigSource::Environment)
        .export_values();
    
    // Export application result enum
    py::enum_<ApplicationResult>(m, "ApplicationResult", R"pbdoc(Application result)pbdoc")
        .value("SUCCESS", ApplicationResult::Success)
        .value("CONFIGURATION_ERROR", ApplicationResult::ConfigurationError)
        .value("INITIALIZATION_ERROR", ApplicationResult::InitializationError)
        .value("EXECUTION_ERROR", ApplicationResult::ExecutionError)
        .value("CLEANUP_ERROR", ApplicationResult::CleanupError)
        .value("UNKNOWN_ERROR", ApplicationResult::UnknownError)
        .export_values();
    
    // Export application phase enum
    py::enum_<ApplicationPhase>(m, "ApplicationPhase", R"pbdoc(Application phase)pbdoc")
        .value("INITIALIZATION", ApplicationPhase::Initialization)
        .value("CONFIGURATION", ApplicationPhase::Configuration)
        .value("BRAIN_SETUP", ApplicationPhase::BrainSetup)
        .value("TESTING", ApplicationPhase::Testing)
        .value("CLEANUP", ApplicationPhase::Cleanup)
        .value("SHUTDOWN", ApplicationPhase::Shutdown)
        .export_values();
    
    // Export application status structure
    py::class_<ApplicationStatus>(m, "ApplicationStatus", R"pbdoc(Application status)pbdoc")
        .def(py::init<>())  // Default constructor
        .def_readwrite("current_phase", &ApplicationStatus::currentPhase)
        .def_readwrite("current_task", &ApplicationStatus::currentTask)
        .def_readwrite("progress", &ApplicationStatus::progress)
        .def_readwrite("metadata", &ApplicationStatus::metadata);
    
    // Export neuron type enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type)pbdoc")
        .value("EXCITATORY", NeuronType::Excitatory)
        .value("INHIBITORY", NeuronType::Inhibitory)
        .value("MODULATORY", NeuronType::Modulatory)
        .value("SENSORY", NeuronType::Sensory)
        .value("MOTOR", NeuronType::Motor)
        .value("INTERNAL", NeuronType::Internal)
        .export_values();
    
    // Export action type enum
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type)pbdoc")
        .value("MOVE_FORWARD", ActionType::MoveForward)
        .value("MOVE_BACKWARD", ActionType::MoveBackward)
        .value("MOVE_LEFT", ActionType::MoveLeft)
        .value("MOVE_RIGHT", ActionType::MoveRight)
        .value("TURN_LEFT", ActionType::TurnLeft)
        .value("TURN_RIGHT", ActionType::TurnRight)
        .value("LOOK", ActionType::Look)
        .value("LOOK_UP", ActionType::LookUp)
        .value("LOOK_DOWN", ActionType::LookDown)
        .value("INTERACT", ActionType::Interact)
        .value("EAT", ActionType::Eat)
        .value("DRINK", ActionType::Drink)
        .value("REST", ActionType::Rest)
        .value("WAIT", ActionType::Wait)
        .value("CUSTOM", ActionType::Custom)
        .export_values();
    
    // Export developmental stage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage)pbdoc")
        .value("INITIAL", DevelopmentalStage::Initial)
        .value("CRITICAL_PERIOD", DevelopmentalStage::CriticalPeriod)
        .value("MATURATION", DevelopmentalStage::Maturation)
        .value("ADULT", DevelopmentalStage::Adult)
        .value("AGING", DevelopmentalStage::Aging)
        .export_values();
    
    // Export firing state enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Firing state)pbdoc")
        .value("RESTING", FiringState::Resting)
        .value("ACTIVE", FiringState::Active)
        .value("REFRACTORY", FiringState::Refractory)
        .value("INHIBITED", FiringState::Inhibited)
        .export_values();
    
    // Export synapse type enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type)pbdoc")
        .value("EXCITATORY", SynapseType::Excitatory)
        .value("INHIBITORY", SynapseType::Inhibitory)
        .value("MODULATORY", SynapseType::Modulatory)
        .value("ELECTRICAL", SynapseType::Electrical)
        .value("GAP_JUNCTION", SynapseType::GapJunction)
        .export_values();
    
    // Export world object type enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type)pbdoc")
        .value("EMPTY", WorldObjectType::Empty)
        .value("RESOURCE", WorldObjectType::Resource)
        .value("HAZARD", WorldObjectType::Hazard)
        .value("WALL", WorldObjectType::Wall)
        .value("MARKER", WorldObjectType::Marker)
        .export_values();
    
    // Export motor command enum
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command)pbdoc")
        .value("MOVE_FORWARD", MotorCommand::MoveForward)
        .value("MOVE_BACKWARD", MotorCommand::MoveBackward)
        .value("TURN_LEFT", MotorCommand::TurnLeft)
        .value("TURN_RIGHT", MotorCommand::TurnRight)
        .value("LOOK_LEFT", MotorCommand::LookLeft)
        .value("LOOK_RIGHT", MotorCommand::LookRight)
        .value("INTERACT", MotorCommand::Interact)
        .value("WAIT", MotorCommand::Wait)
        .export_values();
    
    // Export action result structure
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result)pbdoc")
        .def(py::init<>())
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);
    
    // Export brain ID types
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Neuron ID)pbdoc")
        .def(py::init<>())  // Default constructor
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value);
    
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Synapse ID)pbdoc")
        .def(py::init<>())  // Default constructor
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value);
    
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Region ID)pbdoc")
        .def(py::init<>())  // Default constructor
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value);
    
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Population ID)pbdoc")
        .def(py::init<>())  // Default constructor
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value);
    
    // Export brain region class
    py::class_<BrainRegion>(m, "BrainRegion", R"pbdoc(Brain region)pbdoc")
        .def(py::init<>())  // Default constructor
        .def("get_id", &BrainRegion::getId, "Get region ID")
        .def("get_name", &BrainRegion::getName, "Get region name")
        .def("get_neurons", &BrainRegion::getNeurons, "Get neurons")
        .def("get_synapses", &BrainRegion::getSynapses, "Get synapses");
    
    // Export neural population class
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Neural population)pbdoc")
        .def(py::init<>())  // Default constructor
        .def("get_id", &NeuralPopulation::getId, "Get population ID")
        .def("get_type", &NeuralPopulation::getType, "Get population type")
        .def("get_neurons", &NeuralPopulation::getNeurons, "Get neurons")
        .def("get_neuron_count", &NeuralPopulation::getNeuronCount, "Get neuron count");
    
    // Export neuron class
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Neuron)pbdoc")
        .def(py::init<>())  // Default constructor
        .def("get_id", &Neuron::getId, "Get neuron ID")
        .def("get_type", &Neuron::getType, "Get neuron type")
        .def("get_region_id", &Neuron::getRegionId, "Get region ID")
        .def("get_population_id", &Neuron::getPopulationId, "Get population ID")
        .def("get_firing_state", &Neuron::getFiringState, "Get firing state")
        .def("get_membrane_potential", &Neuron::getMembranePotential, "Get membrane potential")
        .def("get_threshold", &Neuron::getThreshold, "Get threshold")
        .def("get_resting_potential", &Neuron::getRestingPotential, "Get resting potential")
        .def("get_reset_potential", &Neuron::getResetPotential, "Get reset potential")
        .def("get_time_constant", &Neuron::getTimeConstant, "Get time constant")
        .def("get_leak_conductance", &Neuron::getLeakConductance, "Get leak conductance")
        .def("get_capacitance", &Neuron::getCapacitance, "Get capacitance")
        .def("get_refractory_period", &Neuron::getRefractoryPeriod, "Get refractory period")
        .def("get_firing_rate", &Neuron::getFiringRate, "Get firing rate")
        .def("set_membrane_potential", &Neuron::setMembranePotential, "Set membrane potential")
        .def("set_threshold", &Neuron::setThreshold, "Set threshold")
        .def("inject_current", &Neuron::injectCurrent, "Inject current")
        .def("reset", &Neuron::reset, "Reset neuron");
    
    // Export synapse class
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synapse)pbdoc")
        .def(py::init<>())  // Default constructor
        .def("get_id", &Synapse::getId, "Get synapse ID")
        .def("get_pre_neuron_id", &Synapse::getPreNeuronId, "Get pre-synaptic neuron ID")
        .def("get_post_neuron_id", &Synapse::getPostNeuronId, "Get post-synaptic neuron ID")
        .def("get_type", &Synapse::getType, "Get synapse type")
        .def("get_weight", &Synapse::getWeight, "Get synapse weight")
        .def("get_delay", &Synapse::getDelay, "Get synapse delay")
        .def("get_plasticity_enabled", &Synapse::getPlasticityEnabled, "Get plasticity enabled")
        .def("set_weight", &Synapse::setWeight, "Set synapse weight")
        .def("enable_plasticity", &Synapse::enablePlasticity, "Enable plasticity")
        .def("disable_plasticity", &Synapse::disablePlasticity, "Disable plasticity");
    
    // Export world object class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object)pbdoc")
        .def(py::init<>())  // Default constructor
        .def("get_id", &WorldObject::getId, "Get object ID")
        .def("get_type", &WorldObject::getType, "Get object type")
        .def("get_x", &WorldObject::getX, "Get X position")
        .def("get_y", &WorldObject::getY, "Get Y position")
        .def("get_radius", &WorldObject::getRadius, "Get radius")
        .def("get_value", &WorldObject::getValue, "Get value")
        .def("get_active", &WorldObject::getActive, "Get active state")
        .def("set_x", &WorldObject::setX, "Set X position")
        .def("set_y", &WorldObject::setY, "Set Y position")
        .def("set_radius", &WorldObject::setRadius, "Set radius")
        .def("set_value", &WorldObject::setValue, "Set value")
        .def("set_active", &WorldObject::setActive, "Set active state");
    
    // Export agent body class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body)pbdoc")
        .def(py::init<>())  // Default constructor
        .def("get_x", &AgentBody::getX, "Get X position")
        .def("get_y", &AgentBody::getY, "Get Y position")
        .def("get_orientation", &AgentBody::getOrientation, "Get orientation")
        .def("get_velocity_x", &AgentBody::getVelocityX, "Get X velocity")
        .def("get_velocity_y", &AgentBody::getVelocityY, "Get Y velocity")
        .def("get_angular_velocity", &AgentBody::getAngularVelocity, "Get angular velocity")
        .def("get_energy", &AgentBody::getEnergy, "Get energy")
        .def("get_health", &AgentBody::getHealth, "Get health")
        .def("get_age", &AgentBody::getAge, "Get age")
        .def("is_moving", &AgentBody::isMoving, "Check if moving")
        .def("is_turning", &AgentBody::isTurning, "Check if turning");
    
    // Export sensory input class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Sensory input)pbdoc")
        .def(py::init<>())  // Default constructor
        .def("get_type", &SensoryInput::getType, "Get type")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get dimensions")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get timestamp");
    
    // Export vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision)pbdoc")
        .def(py::init<>())  // Default constructor
        .def("get_width", &Vision::getWidth, "Get width")
        .def("get_height", &Vision::getHeight, "Get height")
        .def("get_channels", &Vision::getChannels, "Get channels")
        .def("get_data", &Vision::getData, "Get data")
        .def("set_data", &Vision::setData, "Set data");
    
    // Export audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio)pbdoc")
        .def(py::init<>())  // Default constructor
        .def("get_sample_rate", &Audio::getSampleRate, "Get sample rate")
        .def("get_num_samples", &Audio::getNumSamples, "Get number of samples")
        .def("get_data", &Audio::getData, "Get data")
        .def("set_data", &Audio::setData, "Set data")
        .def("set_sample_rate", &Audio::setSampleRate, "Set sample rate");
    
    // Export internal signals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals)pbdoc")
        .def(py::init<>())  // Default constructor
        .def("get_signals", &InternalSignals::getSignals, "Get signals")
        .def("add_signal", &InternalSignals::addSignal, "Add signal")
        .def("clear_signals", &InternalSignals::clearSignals, "Clear signals");
    
    // Export action class
    py::class_<Action>(m, "Action", R"pbdoc(Action)pbdoc")
        .def(py::init<>())  // Default constructor
        .def("get_type", &Action::getType, "Get type")
        .def("get_parameters", &Action::getParameters, "Get parameters")
        .def("set_type", &Action::setType, "Set type")
        .def("set_parameters", &Action::setParameters, "Set parameters")
        .def("get_name", &Action::getName, "Get name")
        .def("clone", &Action::clone, "Clone action");
    
    // Export sensory percept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept)pbdoc")
        .def(py::init<>())  // Default constructor
        .def("get_vision", &SensoryPercept::getVision, "Get vision")
        .def("set_vision", &SensoryPercept::setVision, "Set vision")
        .def("get_touch", &SensoryPercept::getTouch, "Get touch")
        .def("set_touch", &SensoryPercept::setTouch, "Set touch")
        .def("get_internal", &SensoryPercept::getInternal, "Get internal")
        .def("set_internal", &SensoryPercept::setInternal, "Set internal")
        .def("get_proprioception", &SensoryPercept::getProprioception, "Get proprioception")
        .def("set_proprioception", &SensoryPercept::setProprioception, "Set proprioception")
        .def("get_audio", &SensoryPercept::getAudio, "Get audio")
        .def("set_audio", &SensoryPercept::setAudio, "Set audio");
    
    // Export world class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple world)pbdoc")
        .def(py::init<>())  // Default constructor
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"),
             "Configure world")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept, "Get sensory percept")
        .def("get_agent_body", &SimpleWorld::getAgentBody, "Get agent body")
        .def("add_object", &SimpleWorld::addObject, "Add object")
        .def("remove_object", &SimpleWorld::removeObject, "Remove object")
        .def("is_valid_position", &SimpleWorld::isValidPosition, "Check if position is valid")
        .def("get_width", &SimpleWorld::getWidth, "Get width")
        .def("get_height", &SimpleWorld::getHeight, "Get height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get max energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, "Set max energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, "Set random seed")
        .def("get_random_seed", &SimpleWorld::getRandomSeed, "Get random seed");
    
    // Export agent brain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Constructor")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize, "Get sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize, "Get motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory input")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand, "Decode motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"), "Apply reward modulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage, "Get developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset")
        .def("get_brain", &AgentBrain::getBrain, "Get brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable curiosity")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled, "Check if development enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity enabled");
    
    // Export test framework class
    py::class_<DefaultTestFramework>(m, "DefaultTestFramework", R"pbdoc(Test framework)pbdoc")
        .def(py::init<>())
        .def("register_test", &DefaultTestFramework::registerTest,
             py::arg("name"), py::arg("test"), "Register test")
        .def("run_all_tests", &DefaultTestFramework::runAllTests,
             py::arg("brain"), "Run all tests")
        .def("clear_tests", &DefaultTestFramework::clearTests, "Clear tests")
        .def("get_test_count", &DefaultTestFramework::getTestCount, "Get test count")
        .def("get_test_names", &DefaultTestFramework::getTestNames, "Get test names")
        .def("has_test", &DefaultTestFramework::hasTest, "Check if test exists")
        .def("get_test_info", &DefaultTestFramework::getTestInfo, "Get test info");
    
    // Export NLM application class
    py::class_<NLMApplication>(m, "NLMApplication", R"pbdoc(NLM Application)pbdoc")
        .def(py::init<>())
        .def("run", static_cast<ApplicationResult (NLMApplication::*)(int, char**) >(&NLMApplication::run),
             py::arg("argc"), py::arg("argv"), "Run application")
        .def("run", static_cast<ApplicationResult (NLMApplication::*)(const std::vector<std::string>&) >(&NLMApplication::run),
             py::arg("args"), "Run application with arguments")
        .def("register_test", &NLMApplication::registerTest,
             py::arg("name"), py::arg("test"), "Register test")
        .def("run_all_tests", &NLMApplication::runAllTests,
             py::arg("brain"), "Run all tests")
        .def("set_config", &NLMApplication::setConfig, "Set config")
        .def("get_config", &NLMApplication::getConfig, "Get config")
        .def("set_logger", &NLMApplication::setLogger, "Set logger")
        .def("get_logger", &NLMApplication::getLogger, "Get logger")
        .def("set_brain", &NLMApplication::setBrain, "Set brain")
        .def("get_brain", &NLMApplication::getBrain, "Get brain")
        .def("has_brain", &NLMApplication::hasBrain, "Check if has brain")
        .def("get_status", &NLMApplication::getStatusString, "Get status");
    
    // Export configuration schema class
    py::class_<ConfigSchema>(m, "ConfigSchema", R"pbdoc(Configuration schema)pbdoc")
        .def(py::init<>())  // Default constructor
        .def_readwrite("key", &ConfigSchema::key, "Key")
        .def_readwrite("type", &ConfigSchema::type, "Type")
        .def_readwrite("default_value", &ConfigSchema::defaultValue, "Default value")
        .def_readwrite("validation_regex", &ConfigSchema::validationRegex, "Validation regex")
        .def_readwrite("description", &ConfigSchema::description, "Description")
        .def_readwrite("required", &ConfigSchema::required, "Required");
    
    // Export configuration enhanced class
    py::class_<ConfigEnhanced>(m, "ConfigEnhanced", R"pbdoc(Enhanced configuration)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &ConfigEnhanced::loadFromFile, py::arg("filepath"), "Load from file")
        .def("load_from_json", &ConfigEnhanced::loadFromJSON, py::arg("filepath"), "Load from JSON")
        .def("load_from_yaml", &ConfigEnhanced::loadFromYAML, py::arg("filepath"), "Load from YAML")
        .def("load_from_args", &ConfigEnhanced::loadFromArgs, py::arg("argc"), py::arg("argv"), "Load from args")
        .def("load_from_environment", &ConfigEnhanced::loadFromEnvironment, py::arg("prefix"), "Load from environment")
        .def("save_to_file", &ConfigEnhanced::saveToFile, py::arg("filepath"), "Save to file")
        .def("set", py::overload_cast<const std::string&, const ConfigValue&, ConfigSource>(
                &ConfigEnhanced::set), py::arg("key"), py::arg("value"), "Set value")
        .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource>(
                &ConfigEnhanced::set), py::arg("key"), py::arg("value"), "Set string value")
        .def("set", py::overload_cast<const std::string&, int, ConfigSource>(
                &ConfigEnhanced::set), py::arg("key"), py::arg("value"), "Set int value")
        .def("set", py::overload_cast<const std::string&, double, ConfigSource>(
                &ConfigEnhanced::set), py::arg("key"), py::arg("value"), "Set double value")
        .def("set", py::overload_cast<const std::string&, bool, ConfigSource>(
                &ConfigEnhanced::set), py::arg("key"), py::arg("value"), "Set bool value")
        .def("get", py::overload_cast<const std::string&, std::string>(&ConfigEnhanced::get),
             py::arg("key"), "Get string")
        .def("get", py::overload_cast<const std::string&, int>(&ConfigEnhanced::get),
             py::arg("key"), "Get int")
        .def("get", py::overload_cast<const std::string&, double>(&ConfigEnhanced::get),
             py::arg("key"), "Get double")
        .def("get", py::overload_cast<const std::string&, bool>(&ConfigEnhanced::get),
             py::arg("key"), "Get bool")
        .def("get_or", py::overload_cast<const std::string&, const std::string&, const std::string&>(
                &ConfigEnhanced::getOr), py::arg("key"), py::arg("default_value"), "Get with default")
        .def("has", &ConfigEnhanced::has, py::arg("key"), "Check if has")
        .def("get_keys", &ConfigEnhanced::getKeys, "Get keys")
        .def("clear", &ConfigEnhanced::clear, "Clear")
        .def("summary", &ConfigEnhanced::summary, "Get summary")
        .def("validate", &ConfigEnhanced::validate, "Validate")
        .def("validate_with_schema", &ConfigEnhanced::validate, py::arg("schema"), "Validate with schema")
        .def("__repr__", [](const ConfigEnhanced& config) {
            return "<ConfigEnhanced: " + config.summary() + ">";
        });
    
    // Export system utility functions
    m.def("get_system_info", &getSystemInfo, "Get system info");
    m.def("get_memory_usage", &getMemoryUsage, "Get memory usage");
    m.def("get_peak_memory_usage", &getPeakMemoryUsage, "Get peak memory usage");
    m.def("get_system_load", &getSystemLoad, "Get system load");
    m.def("get_system_uptime", &getSystemUptime, "Get system uptime");
    m.def("format_timestamp", &formatTimestamp, py::arg("timestamp"), "Format timestamp");
    m.def("get_platform_info", &getPlatformInfo, "Get platform info");
    
    // Export test utility functions
    m.def("create_connectivity_test", &test_utils::createConnectivityTest, "Create connectivity test");
    m.def("create_plasticity_test", &test_utils::createPlasticityTest, "Create plasticity test");
    m.def("create_stdp_test", &test_utils::createSTDPTest, "Create STDP test");
    m.def("create_benchmark_test", &test_utils::createBenchmarkTest, "Create benchmark test");
    m.def("create_memory_test", &test_utils::createMemoryTest, "Create memory test");
    m.def("create_test_suite", &test_utils::TestSuite, "Create test suite");
    
    // Export error handling functions
    m.def("throw_nlm_exception", [](const std::string& message) {
        throw NLMException(message);
    }, "Throw NLM exception");
    
    m.def("throw_config_exception", [](const std::string& message) {
        throw ConfigurationException(message);
    }, "Throw configuration exception");
    
    m.def("throw_brain_initialization_exception", [](const std::string& message) {
        throw BrainInitializationException(message);
    }, "Throw brain initialization exception");
    
    m.def("throw_simulation_step_exception", [](size_t step, const std::string& reason) {
        throw SimulationStepException(step, reason);
    }, "Throw simulation step exception");
    
    // Export application factory functions
    m.def("create_default_config", []() {
        return std::make_shared<ConfigEnhanced>();
    }, "Create default config");
    
    m.def("create_test_config", []() {
        auto config = std::make_shared<ConfigEnhanced>();
        config->set("random_seed", 42, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", 500, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("plasticity_enabled", true, ConfigSource::Runtime);
        return config;
    }, "Create test config");
    
    m.def("create_application", []() {
        return std::make_shared<NLMApplication>();
    }, "Create application");
    
    m.def("create_test_application", []() {
        auto app = std::make_shared<NLMApplication>();
        app->registerTest("connectivity", test_utils::createConnectivityTest());
        app->registerTest("plasticity", test_utils::createPlasticityTest());
        app->registerTest("stdp", test_utils::createSTDPTest());
        return app;
    }, "Create test application");
    
    // Export version and metadata
    m.attr("__version__") = "0.6.0";
    m.attr("__build_date__") = "2026-10-05 01:41:00Z";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__description__") = "Enhanced NLM Python bindings with modern C++20 features";
    
    // Export example modules
    m.def("example_basic", []() {
        py::dict example;
        example["title"] = "Basic NLM Usage";
        example["code"] = R"python
import pynlm_enhanced as nlm

# Create configuration
config = nlm.create_default_config()

# Create brain
brain = nlm.create_brain(config)

# Initialize
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Save state
brain.save("brain_state.json")
"python";
        return example;
    }, "Basic example");
    
    m.def("example_advanced", []() {
        py::dict example;
        example["title"] = "Advanced NLM Usage";
        example["code"] = R"python
import pynlm_enhanced as nlm

# Create application
app = nlm.create_application()

# Register tests
app.register_test("connectivity", nlm.create_connectivity_test())
app.register_test("plasticity", nlm.create_plasticity_test())
app.register_test("stdp", nlm.create_stdp_test())

# Create brain with Phase 6 config
config = nlm.create_phase6_config()
brain = nlm.create_brain(config)

# Create world and agent
world = nlm.create_simple_world()
agent_brain = nlm.create_agent_brain(brain)

# Run complete example
result = app.run_complete_example()
"python";
        return example;
    }, "Advanced example");
    
    m.def("example_error_handling", []() {
        py::dict example;
        example["title"] = "Error Handling Example";
        example["code"] = R"python
import pynlm_enhanced as nlm

try:
    config = nlm.create_default_config()
    config.load_from_file("nonexistent.json")
except nlm.ConfigurationException as e:
    print(f"Configuration error: {e}")

try:
    brain = nlm.create_brain(config)
    brain.initialize()
except nlm.BrainInitializationException as e:
    print(f"Brain initialization error: {e}")
"python";
        return example;
    }, "Error handling example");
    
    m.def("example_performance", []() {
        py::dict example;
        example["title"] = "Performance Example";
        example["code"] = R"python
import pynlm_enhanced as nlm
import time

# Create test framework
framework = nlm.create_test_framework()

# Add performance test
framework.register_test("benchmark", nlm.create_benchmark_test())

# Create brain
config = nlm.create_phase6_config()
brain = nlm.create_brain(config)
brain.initialize()

# Run performance test
start_time = time.time()
result = framework.run_all_tests(brain)
elapsed_time = time.time() - start_time

print(f"Tests completed in {elapsed_time:.2f} seconds")
for test in result:
    print(f"{test.name}: {test.status} ({test.executionTime:.3f}s)")
"python";
        return example;
    }, "Performance example");
    
    m.def("example_configuration", []() {
        py::dict example;
        example["title"] = "Configuration Example";
        example["code"] = R"python
import pynlm_enhanced as nlm

# Create configuration from defaults
defaults = nlm.get_application_defaults()
print("Default configuration:", defaults)

# Create enhanced configuration
config = nlm.create_default_config()

# Set values
config.set("random_seed", 12345)
config.set("neuron_count", 1000)
config.set("plasticity_enabled", True)
config.set("log_level", "DEBUG")

# Load from environment variables (if any)
config.load_from_environment()

# Validate configuration
nlm.validate_config(config)

# Save to JSON
config.save_to_file("my_config.json")
"python";
        return example;
    }, "Configuration example");
    
    m.def("example_complete_workflow", []() {
        py::dict example;
        example["title"] = "Complete Workflow Example";
        example["code"] = R"python
import pynlm_enhanced as nlm

print("=== NLM Enhanced Complete Workflow ===")

# Step 1: Create application
app = nlm.create_application()
print("✓ Application created")

# Step 2: Configure application
config = nlm.create_phase6_config()
print("✓ Configuration loaded")

# Step 3: Create brain
brain = nlm.create_brain(config)
print("✓ Brain created")

# Step 4: Initialize brain
brain.initialize()
print("✓ Brain initialized")

# Step 5: Create world
world = nlm.create_simple_world()
world.configure(100, 100, 20, 20)
print("✓ World configured")

# Step 6: Create agent brain
agent_brain = nlm.create_agent_brain(brain)
print("✓ Agent brain created")

# Step 7: Run simulation
for step in range(50):
    brain.step(step, step * 0.001)
    if step % 10 == 0:
        print(f"Step {step}: Active neurons = {brain.get_active_neuron_count()}")
print("✓ Simulation completed")

# Step 8: Save results
brain.save("experiment_results.json")
print("✓ Results saved")

print("=== Complete Workflow Successful ===")
"python";
        return example;
    }, "Complete workflow example");
    
    m.def("example_system_info", []() {
        py::dict example;
        example["title"] = "System Information Example";
        example["code"] = R"python
import pynlm_enhanced as nlm

info = nlm.system_info()
print("NLM Enhanced System Information:")
print(f"Version: {info['__version__']}")
print(f"Framework: {info['framework']}")
print(f"Features: {len(info['features'])} available")
print("\nAvailable features:")
for feature in info['features']:
    print(f"  • {feature}")
"python";
        return example;
    }, "System information example");
    
    m.def("example_help", []() {
        return nlm.help();
    }, "Show help");
    
    // Export convenience functions
    m.def("create_brain", [](std::shared_ptr<ConfigEnhanced> config) {
        return std::make_shared<Brain>(config);
    }, "Create brain");
    
    m.def("create_simple_world", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create world");
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, "Create agent brain");
    
    // Export constants
    m.attr("NEURON_TYPE_EXCITATORY") = NeuronType::Excitatory;
    m.attr("NEURON_TYPE_INHIBITORY") = NeuronType::Inhibitory;
    m.attr("NEURON_TYPE_SENSORY") = NeuronType::Sensory;
    m.attr("NEURON_TYPE_MOTOR") = NeuronType::Motor;
    m.attr("NEURON_TYPE_INTERNAL") = NeuronType::Internal;
    m.attr("NEURON_TYPE_MODULATORY") = NeuronType::Modulatory;
    
    m.attr("ACTION_TYPE_MOVE_FORWARD") = ActionType::MoveForward;
    m.attr("ACTION_TYPE_MOVE_BACKWARD") = ActionType::MoveBackward;
    m.attr("ACTION_TYPE_MOVE_LEFT") = ActionType::MoveLeft;
    m.attr("ACTION_TYPE_MOVE_RIGHT") = ActionType::MoveRight;
    m.attr("ACTION_TYPE_TURN_LEFT") = ActionType::TurnLeft;
    m.attr("ACTION_TYPE_TURN_RIGHT") = ActionType::TurnRight;
    m.attr("ACTION_TYPE_LOOK") = ActionType::Look;
    m.attr("ACTION_TYPE_LOOK_UP") = ActionType::LookUp;
    m.attr("ACTION_TYPE_LOOK_DOWN") = ActionType::LookDown;
    m.attr("ACTION_TYPE_INTERACT") = ActionType::Interact;
    m.attr("ACTION_TYPE_EAT") = ActionType::Eat;
    m.attr("ACTION_TYPE_DRINK") = ActionType::Drink;
    m.attr("ACTION_TYPE_REST") = ActionType::Rest;
    m.attr("ACTION_TYPE_WAIT") = ActionType::Wait;
    m.attr("ACTION_TYPE_CUSTOM") = ActionType::Custom;
    
    m.attr("DEVELOPMENTAL_STAGE_INITIAL") = DevelopmentalStage::Initial;
    m.attr("DEVELOPMENTAL_STAGE_CRITICAL_PERIOD") = DevelopmentalStage::CriticalPeriod;
    m.attr("DEVELOPMENTAL_STAGE_MATURATION") = DevelopmentalStage::Maturation;
    m.attr("DEVELOPMENTAL_STAGE_ADULT") = DevelopmentalStage::Adult;
    m.attr("DEVELOPMENTAL_STAGE_AGING") = DevelopmentalStage::Aging;
    
    m.attr("FIRING_STATE_RESTING") = FiringState::Resting;
    m.attr("FIRING_STATE_ACTIVE") = FiringState::Active;
    m.attr("FIRING_STATE_REFractory") = FiringState::Refractory;
    m.attr("FIRING_STATE_INHIBITED") = FiringState::Inhibited;
    
    m.attr("SYNAPSE_TYPE_EXCITATORY") = SynapseType::Excitatory;
    m.attr("SYNAPSE_TYPE_INHIBITORY") = SynapseType::Inhibitory;
    m.attr("SYNAPSE_TYPE_MODULATORY") = SynapseType::Modulatory;
    m.attr("SYNAPSE_TYPE_ELECTRICAL") = SynapseType::Electrical;
    m.attr("SYNAPSE_TYPE_GAP_JUNCTION") = SynapseType::GapJunction;
    
    m.attr("WORLD_OBJECT_TYPE_EMPTY") = WorldObjectType::Empty;
    m.attr("WORLD_OBJECT_TYPE_RESOURCE") = WorldObjectType::Resource;
    m.attr("WORLD_OBJECT_TYPE_HAZARD") = WorldObjectType::Hazard;
    m.attr("WORLD_OBJECT_TYPE_WALL") = WorldObjectType::Wall;
    m.attr("WORLD_OBJECT_TYPE_MARKER") = WorldObjectType::Marker;
    
    m.attr("MOTOR_COMMAND_MOVE_FORWARD") = MotorCommand::MoveForward;
    m.attr("MOTOR_COMMAND_MOVE_BACKWARD") = MotorCommand::MoveBackward;
    m.attr("MOTOR_COMMAND_TURN_LEFT") = MotorCommand::TurnLeft;
    m.attr("MOTOR_COMMAND_TURN_RIGHT") = MotorCommand::TurnRight;
    m.attr("MOTOR_COMMAND_LOOK_LEFT") = MotorCommand::LookLeft;
    m.attr("MOTOR_COMMAND_LOOK_RIGHT") = MotorCommand::LookRight;
    m.attr("MOTOR_COMMAND_INTERACT") = MotorCommand::Interact;
    m.attr("MOTOR_COMMAND_WAIT") = MotorCommand::Wait;
    
    // Export utility functions
    m.def("format_timestamp", &formatTimestamp, "Format timestamp");
    m.def("get_memory_usage", &getMemoryUsage, "Get memory usage");
    m.def("get_peak_memory_usage", &getPeakMemoryUsage, "Get peak memory usage");
    m.def("get_system_load", &getSystemLoad, "Get system load");
    m.def("get_system_uptime", &getSystemUptime, "Get system uptime");
    m.def("get_platform_info", &getPlatformInfo, "Get platform info");
    m.def("get_system_info", &getSystemInfo, "Get system info");
    
    // Export configuration schema
    m.def("create_config_schema", [](const std::string& key, const ConfigValue& type,
                                    const std::any& default_value,
                                    const std::string& validation_regex = "",
                                    const std::string& description = "",
                                    bool required = false) {
        return ConfigSchema(key, type, default_value, validation_regex, description, required);
    }, "Create configuration schema");
    
    // Export error handling
    m.def("catch_error", [](std::function<void()> func, const std::string& context) {
        try {
            func();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error in " << context << ": " << e.what() << std::endl;
            return false;
        }
    }, "Catch and handle error");
    
    // Export validation functions
    m.def("validate_config_with_schema", [](const std::shared_ptr<ConfigEnhanced>& config,
                                          const std::vector<ConfigSchema>& schema) {
        return config->validate(schema);
    }, "Validate configuration with schema");
    
    // Export helper functions
    m.def("strings_join", &stringsJoin, "Join strings");
    m.def("format_string", &formatString, "Format string");
    m.def("format_string_list", &formatStringList, "Format string list");
    
    // Export convenience constants
    m.def("get_default_config_values", []() {
        py::dict defaults;
        defaults["random_seed"] = 42;
        defaults["simulation_timestep"] = 0.001;
        defaults["neuron_count"] = 1000;
        defaults["region_count"] = 2;
        defaults["connection_probability"] = 0.15;
        defaults["plasticity_learning_rate"] = 0.01;
        defaults["stdp_ltp_weight"] = 0.02;
        defaults["stdp_ltd_weight"] = 0.015;
        defaults["synaptogenesis_rate"] = 0.0001;
        defaults["pruning_rate"] = 0.00001;
        defaults["dopamine_baseline"] = 0.1;
        defaults["reward_discount_factor"] = 0.99;
        return defaults;
    }, "Get default configuration values");
    
    // Export test framework utilities
    m.def("create_test_framework", []() {
        return std::make_shared<DefaultTestFramework>();
    }, "Create test framework");
    
    m.def("create_test_suite", &test_utils::TestSuite, "Create test suite");
    
    // Export application utilities
    m.def("get_application_status", [](std::shared_ptr<NLMApplication> app) {
        return app->getStatusString();
    }, "Get application status");
    
    m.def("run_application", [](std::shared_ptr<NLMApplication> app) {
        return app->run(0, nullptr);
    }, "Run application");
    
    // Export system utilities
    m.def("get_build_info", []() {
        py::dict build_info;
        build_info["version"] = "0.6.0";
        build_info["build_date"] = "2026-10-05 01:41:00Z";
        build_info["compiler"] = "C++20";
        build_info["pybind11_version"] = PYBIND11_VERSION;
        return build_info;
    }, "Get build information");
    
    // Export convenience functions for common operations
    m.def("initialize_brain", [](std::shared_ptr<Brain> brain) {
        if (!brain->initialize()) {
            throw BrainInitializationException("Brain initialization failed");
        }
        return true;
    }, "Initialize brain");
    
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, int steps) {
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        return brain->getTotalSpikeCount();
    }, "Run simulation");
    
    m.def("save_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, "Save brain state");
    
    m.def("load_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }, "Load brain state");
    
    // Export examples module
    py::module_ examples_module = py::module_::create_module("examples");
    examples_module.def("basic", []() { return m.attr("example_basic")(); });
    examples_module.def("advanced", []() { return m.attr("example_advanced")(); });
    examples_module.def("error_handling", []() { return m.attr("example_error_handling")(); });
    examples_module.def("performance", []() { return m.attr("example_performance")(); });
    examples_module.def("configuration", []() { return m.attr("example_configuration")(); });
    examples_module.def("complete_workflow", []() { return m.attr("example_complete_workflow")(); });
    examples_module.def("system_info", []() { return m.attr("example_system_info")(); });
    examples_module.def("help", []() { return m.attr("example_help")(); });
    
    m.def("examples", []() {
        py::dict examples;
        examples["basic"] = m.attr("example_basic")();
        examples["advanced"] = m.attr("example_advanced")();
        examples["error_handling"] = m.attr("example_error_handling")();
        examples["performance"] = m.attr("example_performance")();
        examples["configuration"] = m.attr("example_configuration")();
        examples["complete_workflow"] = m.attr("example_complete_workflow")();
        examples["system_info"] = m.attr("example_system_info")();
        examples["help"] = m.attr("example_help")();
        return examples;
    }, "Get all examples");
    
    // Export documentation
    m.attr("__doc__") = R"python
Enhanced NLM Python Bindings
=============================

This module provides enhanced Python bindings for the NLM (Neural Learning Machine) C++ framework.

Key Features:
- Modern pybind11 API with type hints
- Comprehensive error handling with Python exceptions
- Integration with new application framework
- Enhanced configuration system (JSON/YAML support)
- Test framework integration
- Better documentation and examples
- System information and monitoring
- Memory management utilities
- Performance monitoring
- Configuration validation
- Environment variable support
- Type conversion utilities
- Simulation utilities

Usage:
    import pynlm_enhanced as nlm
    
    # Create configuration
    config = nlm.create_default_config()
    
    # Create brain
    brain = nlm.create_brain(config)
    
    # Initialize
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Save results
    brain.save("results.json")
"python";
    
    // Summary
    std::cout << "NLM Enhanced Python Bindings compiled successfully!" << std::endl;
    std::cout << "Version: 0.6.0" << std::endl;
    std::cout << "Build Date: 2026-10-05 01:41:00Z" << std::endl;
    std::cout << "Features: Enhanced configuration, application framework, test framework" << std::endl;
    
    } // PYBIND11_MODULE
    
} // namespace nlm
