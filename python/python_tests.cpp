// Comprehensive Python test file for NLM

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "../src/core/Logger/PythonLoggingIntegration.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pynlm_python_tests, m) {
    m.doc() = R"pbdoc(
        NLM Python Binding Tests Module
        -------------------------------
        Comprehensive test suite for NLM Python bindings.
        
        This module provides extensive tests for all Python bindings
        including configuration, sensory input, actions, world simulation,
        and brain functionality.
    )pbdoc";
    
    // Import the main pynlm module functionality
    import pybind11::module;
    
    // Re-export main classes and functions from pynlm
    m.def("createDefaultConfig", []() {
        return nlm::createDefaultConfig();
    }, "Create a default configuration object");
    
    m.def("createBrain", [](std::shared_ptr<nlm::Config> config) {
        return nlm::createBrain(config);
    }, "Create a brain with configuration");
    
    m.def("createSimpleWorld", []() {
        return nlm::createSimpleWorld();
    }, "Create a simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<nlm::Brain> brain) {
        return nlm::createAgentBrain(brain);
    }, "Create an agent brain interface");
    
    // Register all enumerations
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Internal", nlm::NeuronType::Internal);
    
    py::enum_<nlm::ActionType>(m, "ActionType")
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("MoveLeft", nlm::ActionType::MoveLeft)
        .value("MoveRight", nlm::ActionType::MoveRight)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("Look", nlm::ActionType::Look)
        .value("LookUp", nlm::ActionType::LookUp)
        .value("LookDown", nlm::ActionType::LookDown)
        .value("Interact", nlm::ActionType::Interact)
        .value("Eat", nlm::ActionType::Eat)
        .value("Drink", nlm::ActionType::Drink)
        .value("Rest", nlm::ActionType::Rest)
        .value("Wait", nlm::ActionType::Wait)
        .value("Custom", nlm::ActionType::Custom);
    
    py::enum_<nlm::WorldObjectType>(m, "WorldObjectType")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker);
    
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging);
    
    py::enum_<nlm::LogLevel>(m, "LogLevel")
        .value("DEBUG", nlm::LogLevel::Debug)
        .value("INFO", nlm::LogLevel::Info)
        .value("WARNING", nlm::LogLevel::Warning)
        .value("ERROR", nlm::LogLevel::Error)
        .value("CRITICAL", nlm::LogLevel::Critical);
    
    // Test helper functions
    m.def("run_all_tests", []() {
        // Run all Python binding tests
        test_pybindings::runAll();
        return "All tests completed successfully";
    }, "Run all Python binding tests");
    
    m.def("test_configuration", []() {
        test_pybindings::test_configuration_operations();
        return "Configuration tests passed";
    }, "Run configuration tests");
    
    m.def("test_sensory", []() {
        test_pybindings::test_sensory_bindings();
        return "Sensory tests passed";
    }, "Run sensory input tests");
    
    m.def("test_objects", []() {
        test_pybindings::test_object_bindings();
        return "Object tests passed";
    }, "Run object tests");
    
    m.def("test_brain", []() {
        test_pybindings::test_brain_bindings();
        return "Brain tests passed";
    }, "Run brain tests");
    
    m.def("test_world", []() {
        test_pybindings::test_world_bindings();
        return "World tests passed";
    }, "Run world tests");
    
    m.def("test_error_handling", []() {
        test_pybindings::test_error_handling();
        return "Error handling tests passed";
    }, "Run error handling tests");
    
    m.def("test_logging", []() {
        test_pybindings::test_logging_integration();
        return "Logging tests passed";
    }, "Run logging tests");
    
    // Example usage functions
    m.def("example_simple_brain", []() {
        // Example: Create and run a simple brain
        auto config = nlm::createDefaultConfig();
        config->set("brain.neuron_count", static_cast<int64_t>(100));
        config->set("brain.region_count", static_cast<int64_t>(1));
        
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        // Run a few steps
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain->step(step);
        }
        
        return std::make_tuple(brain->getTotalNeuronCount(), brain->getTotalSpikeCount());
    }, "Example: Create and run a simple brain");
    
    m.def("example_agent_world", []() {
        // Example: Create an agent in a world
        auto config = nlm::createDefaultConfig();
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        auto agent = nlm::createAgentBrain(brain);
        auto world = nlm::createSimpleWorld();
        world->configure(20, 20, 8, 8);
        world->reset();
        agent->initialize(world);
        
        // Run a simple episode
        for (int step = 0; step < 20; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(world->getSimulationTime(), brain->getTotalSpikeCount());
    }, "Example: Run an agent in a world");
    
    m.def("example_configuration", []() {
        // Example: Create and save a configuration
        auto config = nlm::createDefaultConfig();
        
        // Configure with custom parameters
        config->set("brain.neuron_count", static_cast<int64_t>(500));
        config->set("brain.region_count", static_cast<int64_t>(3));
        config->set("brain.connection_probability", 0.2f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 2.0f);
        
        // Save to file
        config->saveToFile("test_config.txt");
        
        // Load back
        auto config2 = nlm::createDefaultConfig();
        config2->loadFromFile("test_config.txt");
        
        return std::make_tuple(config2->getOr<int>("brain.neuron_count", 0),
                               config2->getOr<double>("brain.connection_probability", 0.0));
    }, "Example: Create, configure, save, and load configuration");
    
    // Utility functions
    m.def("get_version", []() {
        return std::string("NLM Python Bindings v0.1.0 (Phase 6 - Enhanced)");
    }, "Get version information");
    
    m.def("test_all_bindings", []() {
        test_pybindings::runAll();
        return "All Python bindings tests completed successfully";
    }, "Run comprehensive Python bindings tests");
    
    // Export logger functions
    m.def("configure_logging", &nlm::python_logging::PythonLogger::configure, "Configure logging from Python dictionary");
    m.def("get_logging_stats", &nlm::python_logging::PythonLogger::getLogStatistics, "Get current logging statistics");
    m.def("log_performance", &nlm::python_logging::PythonLogger::logPerformance, "Log performance metrics");
    m.def("log_memory", &nlm::python_logging::PythonLogger::logMemoryUsage, "Log memory usage statistics");
    
    // Add convenience logging functions
    m.def("log_info", [](const std::string& message) {
        NLM_LOG_INFO(message);
    }, "Log info message");
    
    m.def("log_debug", [](const std::string& message) {
        NLM_LOG_DEBUG(message);
    }, "Log debug message");
    
    m.def("log_warning", [](const std::string& message) {
        NLM_LOG_WARNING(message);
    }, "Log warning message");
    
    m.def("log_error", [](const std::string& message) {
        NLM_LOG_ERROR(message);
    }, "Log error message");
    
    m.def("log_stream", [](nlm::LogLevel level, const std::string& message) {
        NLM_LOG_STREAM(level) << message;
    }, "Log message with stream-style formatting");
}