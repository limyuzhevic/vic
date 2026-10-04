#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/command/Command.hpp"
#include "../src/command/CommandManager.hpp"
#include "../src/factory/BrainFactory.hpp"
#include "../src/threading/ThreadSafeSimulationManager.hpp"
#include "../src/topology/AdvancedTopologyManager.hpp"
#include "../src/core/ErrorHandling.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace py = pybind11;
namespace nlm {

// Forward declarations for complex types
class Brain;
class Config;
class Command;
class CommandManager;
class BrainFactory;
class ThreadSafeSimulationManager;
class AdvancedTopologyManager;

// Python bindings for NLM - Enhanced Edition
// This module provides comprehensive access to all NLM advanced features

PYBIND11_MODULE(pynlm_advanced, m) {
    m.doc() = R"pbdoc(
        NLM Advanced Python Bindings
        =============================

        Enhanced Python interface for the Neural Learning Machine (NLM) framework.
        Provides access to advanced features including command system, factory patterns,
        thread-safe simulation, advanced topologies, and comprehensive error handling.

        Key Features:
        - Brain Factory for creating custom brain architectures
        - Command System for sophisticated action selection
        - Thread-safe parallel simulation management
        - Advanced neural network topologies (CNN, ResNet, Autoencoders, etc.)
        - Enhanced error handling and validation
        - Memory management and monitoring tools
        - Performance optimization utilities

        Usage Examples:
        ```python
        import pynlm_advanced as nlm

        # Create an enhanced brain using the factory
        config = nlm.BrainFactoryConfig()
        config.brainType = nlm.BrainComponentType.EnhancedBrain
        config.neuronCount = 5000
        brain = nlm.BrainFactory.createBrain(config)

        # Use advanced topology
        topo_config = nlm.AdvancedTopologyConfig()
        topo_config.topologyType = nlm.AdvancedTopologyType.Convolutional
        topo_config.layerSizes = [256, 128, 64, 32, 16]
        advanced_brain = nlm.AdvancedTopologyManager.createFromTopology(topo_config)

        # Set up thread-safe simulation
        thread_pool = nlm.ThreadPool(4)
        sim_manager = nlm.ThreadSafeSimulationManager(thread_pool)
        sim_manager.startSimulation(lambda: brain.step(step))

        # Use command system
        command_manager = nlm.CommandManager()
        action = command_manager.executeCommands(brain)
        ```
    )pbdoc";

    // Register exception classes
    py::register_exception<nlm::NLMException>(m, "NLMException");
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Brain Configuration
    py::class_<nlm::Config>(m, "Config")
        .def("loadFromFile", &nlm::Config::loadFromFile)
        .def("loadFromArgs", &nlm::Config::loadFromArgs)
        .def("saveToFile", &nlm::Config::saveToFile)
        .def("has", &nlm::Config::has)
        .def("getKeys", &nlm::Config::getKeys)
        .def("clear", &nlm::Config::clear)
        .def("summary", &nlm::Config::summary);

    // Brain Factory Configuration
    py::class_<nlm::BrainFactoryConfig>(m, "BrainFactoryConfig")
        .def_readwrite("brainType", &nlm::BrainFactoryConfig::brainType)
        .def_readwrite("neuronType", &nlm::BrainFactoryConfig::neuronType)
        .def_readwrite("topology", &nlm::BrainFactoryConfig::topology)
        .def_readwrite("neuronCount", &nlm::BrainFactoryConfig::neuronCount)
        .def_readwrite("connectionDensity", &nlm::BrainFactoryConfig::connectionDensity)
        .def_readwrite("enablePlasticity", &nlm::BrainFactoryConfig::enablePlasticity)
        .def_readwrite("enableDevelopment", &nlm::BrainFactoryConfig::enableDevelopment)
        .def_readwrite("enableNeuromodulation", &nlm::BrainFactoryConfig::enableNeuromodulation)
        .def_readwrite("enableMemory", &nlm::BrainFactoryConfig::enableMemory)
        .def_readwrite("enablePrediction", &nlm::BrainFactoryConfig::enablePrediction)
        .def_readwrite("enableCognition", &nlm::BrainFactoryConfig::enableCognition)
        .def("isValid", &nlm::BrainFactoryConfig::isValid);

    // Command System
    py::class_<nlm::Command>(m, "Command")
        .def("execute", &nlm::Command::execute)
        .def("getName", &nlm::Command::getName)
        .def("getDescription", &nlm::Command::getDescription)
        .def("isEnabled", &nlm::Command::isEnabled)
        .def("setEnabled", &nlm::Command::setEnabled)
        .def("getPriority", &nlm::Command::getPriority)
        .def("setPriority", &nlm::Command::setPriority);

    py::class_<nlm::SimpleCommand, nlm::Command>(m, "SimpleCommand")
        .def(py::init<std::unique_ptr<nlm::Action>, float>())
        .def("getAction", &nlm::SimpleCommand::getAction);

    py::class_<nlm::CommandManager>(m, "CommandManager")
        .def(py::init<>())
        .def("addCommand", &nlm::CommandManager::addCommand)
        .def("removeCommand", &nlm::CommandManager::removeCommand)
        .def("executeCommands", &nlm::CommandManager::executeCommands)
        .def("getCommand", &nlm::CommandManager::getCommand)
        .def("setCommandEnabled", &nlm::CommandManager::setCommandEnabled)
        .def("setCommandPriority", &nlm::CommandManager::setCommandPriority)
        .def("getEnabledCommands", &nlm::CommandManager::getEnabledCommands)
        .def("getCommandCount", &nlm::CommandManager::getCommandCount);

    // Advanced Topology System
    py::enum_<nlm::AdvancedTopologyType>(m, "AdvancedTopologyType")
        .value("Convolutional", nlm::AdvancedTopologyType::Convolutional)
        .value("Residual", nlm::AdvancedTopologyType::Residual)
        .value("SkipConnection", nlm::AdvancedTopologyType::SkipConnection)
        .value("AutoEncoder", nlm::AdvancedTopologyType::AutoEncoder)
        .value("GAN", nlm::AdvancedTopologyType::GAN)
        .value("AttentionNetwork", nlm::AdvancedTopologyType::AttentionNetwork)
        .value("GraphNeuralNetwork", nlm::AdvancedTopologyType::GraphNeuralNetwork)
        .value("HybridTopology", nlm::AdvancedTopologyType::HybridTopology);

    py::enum_<nlm::LayerType>(m, "LayerType")
        .value("Input", nlm::LayerType::Input)
        .value("Hidden", nlm::LayerType::Hidden)
        .value("Output", nlm::LayerType::Output)
        .value("Recurrent", nlm::LayerType::Recurrent)
        .value("Convolutional", nlm::LayerType::Convolutional)
        .value("Pooling", nlm::LayerType::Pooling)
        .value("Activation", nlm::LayerType::Activation)
        .value("Normalization", nlm::LayerType::Normalization)
        .value("Custom", nlm::LayerType::Custom);

    py::class_<nlm::AdvancedTopologyConfig>(m, "AdvancedTopologyConfig")
        .def(py::init<>())
        .def_readwrite("topologyType", &nlm::AdvancedTopologyConfig::topologyType)
        .def_readwrite("layerSizes", &nlm::AdvancedTopologyConfig::layerSizes)
        .def_readwrite("layerTypes", &nlm::AdvancedTopologyConfig::layerTypes)
        .def_readwrite("layerNames", &nlm::AdvancedTopologyConfig::layerNames)
        .def_readwrite("connectionMatrix", &nlm::AdvancedTopologyConfig::connectionMatrix)
        .def_readwrite("activationFunction", &nlm::AdvancedTopologyConfig::activationFunction)
        .def_readwrite("learningRate", &nlm::AdvancedTopologyConfig::learningRate)
        .def_readwrite("dropoutRate", &nlm::AdvancedTopologyConfig::dropoutRate)
        .def_readwrite("batchNormalization", &nlm::AdvancedTopologyConfig::batchNormalization)
        .def_readwrite("residualConnections", &nlm::AdvancedTopologyConfig::residualConnections)
        .def_readwrite("maxDepth", &nlm::AdvancedTopologyConfig::maxDepth)
        .def_readwrite("branchingFactor", &nlm::AdvancedTopologyConfig::branchingFactor)
        .def_readwrite("sparsity", &nlm::AdvancedTopologyConfig::sparsity)
        .def("isValid", &nlm::AdvancedTopologyConfig::isValid);

    py::class_<nlm::AdvancedTopologyManager>(m, "AdvancedTopologyManager")
        .def_static("createFromTopology", &nlm::AdvancedTopologyManager::createFromTopology)
        .def_static("createConvolutionalTopology", &nlm::AdvancedTopologyManager::createConvolutionalTopology)
        .def_static("createResidualTopology", &nlm::AdvancedTopologyManager::createResidualTopology)
        .def_static("createSkipConnectionTopology", &nlm::AdvancedTopologyManager::createSkipConnectionTopology)
        .def_static("createAutoencoderTopology", &nlm::AdvancedTopologyManager::createAutoencoderTopology)
        .def_static("createGANTopology", &nlm::AdvancedTopologyManager::createGANTopology)
        .def_static("createAttentionTopology", &nlm::AdvancedTopologyManager::createAttentionTopology)
        .def_static("createGraphNeuralNetworkTopology", &nlm::AdvancedTopologyManager::createGraphNeuralNetworkTopology)
        .def_static("createHybridTopology", &nlm::AdvancedTopologyManager::createHybridTopology)
        .def_static("getTopologyTypeName", &nlm::AdvancedTopologyManager::getTopologyTypeName)
        .def_static("getLayerTypeName", &nlm::AdvancedTopologyManager::getLayerTypeName)
        .def_static("getActivationFunctionName", &nlm::AdvancedTopologyManager::getActivationFunctionName)
        .def_static("generateRandomTopology", &nlm::AdvancedTopologyManager::generateRandomTopology)
        .def_static("validateTopologyConfig", &nlm::AdvancedTopologyManager::validateTopologyConfig)
        .def_static("createConnectivityMatrix", &nlm::AdvancedTopologyManager::createConnectivityMatrix)
        .def_static("getAvailableTopologyTypes", &nlm::AdvancedTopologyManager::getAvailableTopologyTypes)
        .def_static("getAvailableLayerTypes", &nlm::AdvancedTopologyManager::getAvailableLayerTypes)
        .def_static("getAvailableActivationFunctions", &nlm::AdvancedTopologyManager::getAvailableActivationFunctions);

    // Threading System
    py::class_<nlm::ThreadPool>(m, "ThreadPool")
        .def(py::init<size_t>(), py::arg("numThreads") = 0)
        .def("execute", &nlm::ThreadPool::execute)
        .def("waitForCompletion", &nlm::ThreadPool::waitForCompletion)
        .def("stop", &nlm::ThreadPool::stop)
        .def("threadCount", &nlm::ThreadPool::threadCount);

    py::class_<nlm::ThreadSafeAllocator>(m, "ThreadSafeAllocator")
        .def(py::init<size_t>(), py::arg("initialPoolSize") = 1024)
        .def("allocate", &nlm::ThreadSafeAllocator::allocate)
        .def("deallocate", &nlm::ThreadSafeAllocator::deallocate)
        .def("getTotalAllocated", &nlm::ThreadSafeAllocator::getTotalAllocated)
        .def("getPoolSize", &nlm::ThreadSafeAllocator::getPoolSize)
        .def("setTrackingEnabled", &nlm::ThreadSafeAllocator::setTrackingEnabled)
        .def("getStats", &nlm::ThreadSafeAllocator::getStats);

    py::class_<nlm::ThreadSafeAllocator::MemoryStats>(m, "MemoryStats")
        .def_readonly("totalAllocated", &nlm::ThreadSafeAllocator::MemoryStats::totalAllocated)
        .def_readonly("currentPoolSize", &nlm::ThreadSafeAllocator::MemoryStats::currentPoolSize)
        .def_readonly("peakUsage", &nlm::ThreadSafeAllocator::MemoryStats::peakUsage)
        .def_readonly("fragmentationRatio", &nlm::ThreadSafeAllocator::MemoryStats::fragmentationRatio);

    py::class_<nlm::ThreadSafeSimulationManager>(m, "ThreadSafeSimulationManager")
        .def(py::init<std::shared_ptr<nlm::ThreadPool>, std::shared_ptr<nlm::ThreadSafeAllocator>>(),
             py::arg("threadPool") = nullptr, py::arg("allocator") = nullptr)
        .def("startSimulation", &nlm::ThreadSafeSimulationManager::startSimulation)
        .def("stopSimulation", &nlm::ThreadSafeSimulationManager::stopSimulation)
        .def("isRunning", &nlm::ThreadSafeSimulationManager::isRunning)
        .def("getStats", &nlm::ThreadSafeSimulationManager::getStats)
        .def("addNeuralOperation", &nlm::ThreadSafeSimulationManager::addNeuralOperation)
        .def("executeOperations", &nlm::ThreadSafeSimulationManager::executeOperations)
        .def("setThreadPool", &nlm::ThreadSafeSimulationManager::setThreadPool)
        .def("setAllocator", &nlm::ThreadSafeSimulationManager::setAllocator);

    py::class_<nlm::ThreadSafeSimulationManager::SimulationStats>(m, "SimulationStats")
        .def_readonly("simulationSpeed", &nlm::ThreadSafeSimulationManager::SimulationStats::simulationSpeed)
        .def_readonly("activeThreads", &nlm::ThreadSafeSimulationManager::SimulationStats::activeThreads)
        .def_readonly("totalTasksCompleted", &nlm::ThreadSafeSimulationManager::SimulationStats::totalTasksCompleted)
        .def_readonly("averageTaskTime", &nlm::ThreadSafeSimulationManager::SimulationStats::averageTaskTime)
        .def_readonly("memoryUsage", &nlm::ThreadSafeSimulationManager::SimulationStats::memoryUsage)
        .def_readonly("cpuUsage", &nlm::ThreadSafeSimulationManager::SimulationStats::cpuUsage);

    // Error Handling
    py::enum_<nlm::ErrorCategory>(m, "ErrorCategory")
        .value("Generic", nlm::ErrorCategory::Generic)
        .value("Memory", nlm::ErrorCategory::Memory)
        .value("Validation", nlm::ErrorCategory::Validation)
        .value("Configuration", nlm::ErrorCategory::Configuration)
        .value("Neural", nlm::ErrorCategory::Neural)
        .value("Motor", nlm::ErrorCategory::Motor)
        .value("Sensor", nlm::ErrorCategory::Sensor)
        .value("Plasticity", nlm::ErrorCategory::Plasticity)
        .value("Development", nlm::ErrorCategory::Development)
        .value("System", nlm::ErrorCategory::System);

    py::class_<nlm::ErrorResult>(m, "ErrorResult")
        .def(py::init<>())
        .def("isOk", &nlm::ErrorResult::isOk)
        .def("isError", &nlm::ErrorResult::isError);

    py::class_<nlm::NLMException, std::runtime_error>(m, "NLMException")
        .def("getCategory", &nlm::NLMException::getCategory);

    // Factory Functions for Enhanced Interface
    m.def("createDefaultBrain", []() {
        return nlm::BrainFactory::createBrain(nlm::BrainFactory::getDefaultConfig());
    }, "Create a default brain with standard configuration");

    m.def("createEnhancedBrain", []() {
        return nlm::BrainFactory::createBrain(nlm::BrainFactory::getEnhancedConfig());
    }, "Create an enhanced brain with all features enabled");

    m.def("createMinimalBrain", [](size_t neuronCount) {
        return nlm::BrainFactory::createMinimalBrain(neuronCount);
    }, "Create a minimal brain for testing and development");

    m.def("createBrainFromFile", [](const std::string& configFile) {
        return nlm::BrainFactory::createBrainFromFile(configFile);
    }, "Create a brain from a configuration file");

    // Enum aliases for backward compatibility
    py::enum_<nlm::BrainComponentType>(m, "BrainType")
        .value("Default", nlm::BrainComponentType::DefaultBrain)
        .value("Enhanced", nlm::BrainComponentType::EnhancedBrain)
        .value("Minimal", nlm::BrainComponentType::MinimalBrain)
        .value("Custom", nlm::BrainComponentType::CustomBrain);

    py::enum_<nlm::NeuronTypeConfig>(m, "NeuronType")
        .value("Standard", nlm::NeuronTypeConfig::Standard)
        .value("HighConductance", nlm::NeuronTypeConfig::HighConductance)
        .value("LowThreshold", nlm::NeuronTypeConfig::LowThreshold)
        .value("Adaptive", nlm::NeuronTypeConfig::Adaptive);

    py::enum_<nlm::NetworkTopology>(m, "Topology")
        .value("Feedforward", nlm::NetworkTopology::Feedforward)
        .value("Recurrent", nlm::NetworkTopology::Recurrent)
        .value("Hierarchical", nlm::NetworkTopology::Hierarchical)
        .value("Modular", nlm::NetworkTopology::Modular);

    // Error category aliases
    py::enum_<nlm::ErrorCategory>(m, "ErrorType")
        .value("Generic", nlm::ErrorCategory::Generic)
        .value("Memory", nlm::ErrorCategory::Memory)
        .value("Validation", nlm::ErrorCategory::Validation)
        .value("Configuration", nlm::ErrorCategory::Configuration);
}

} // namespace nlm
