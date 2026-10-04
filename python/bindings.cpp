#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

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

// Enhanced Python bindings for NLM
// This module provides comprehensive access to all NLM features

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        =============================================

        Enhanced Python interface for the Neural Learning Machine (NLM) framework.
        Provides comprehensive access to all NLM features including command system,
        factory patterns, thread-safe simulation, advanced topologies, and comprehensive
        error handling.

        Key Features:
        - **Brain Factory** - Create custom brain architectures with various configurations
        - **Command System** - Sophisticated action selection with priority-based execution
        - **Thread-safe Simulation** - Parallel processing and monitoring utilities
        - **Advanced Topologies** - CNN, ResNet, Autoencoder, Graph Neural Networks, etc.
        - **Enhanced Error Handling** - Comprehensive exception classes and validation
        - **Memory Management** - Thread-safe allocators with monitoring
        - **Performance Optimization** - Thread pools and simulation statistics

        Quick Start Examples:
        ```python
        import pynlm as nlm

        # Create an enhanced brain using the factory
        config = nlm.BrainFactoryConfig()
        config.brainType = nlm.BrainComponentType.EnhancedBrain
        config.neuronCount = 5000
        config.enablePlasticity = True
        config.enableDevelopment = True
        brain = nlm.BrainFactory.createBrain(config)

        # Use advanced topology
        topo_config = nlm.AdvancedTopologyConfig()
        topo_config.topologyType = nlm.AdvancedTopologyType.Convolutional
        topo_config.layerSizes = [256, 128, 64, 32, 16]
        advanced_brain = nlm.AdvancedTopologyManager.createFromTopology(topo_config)

        # Set up thread-safe simulation
        thread_pool = nlm.ThreadPool(4)
        sim_manager = nlm.ThreadSafeSimulationManager(thread_pool)

        # Use command system for action selection
        command_manager = nlm.CommandManager()
        action = command_manager.executeCommands(brain)
        ```
    )pbdoc";

    // Register exception classes
    py::register_exception<nlm::NLMException>(m, "NLMException");
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Basic class bindings (existing functionality)
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

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Pythonic property access and context managers
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
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
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced Brain Factory Configuration
    py::class_<nlm::BrainFactoryConfig>(m, "BrainFactoryConfig")
        .def(py::init<>())
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
        .def("isValid", &nlm::BrainFactoryConfig::isValid)
        .def("__repr__", [](const nlm::BrainFactoryConfig& config) {
            std::stringstream ss;
            ss << "<BrainFactoryConfig: neurons=" << config.neuronCount
               << ", plasticity=" << (config.enablePlasticity ? "enabled" : "disabled")
               << ", development=" << (config.enableDevelopment ? "enabled" : "disabled")
               << ">";
            return ss.str();
        });

    // Factory methods for enhanced brain creation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

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

    m.def("validateConfig", &nlm::BrainFactory::validateConfig,
          "Validate a BrainFactory configuration");

    // Enum aliases for convenience
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

    // Command System
    py::class_<nlm::Command>(m, "Command")
        .def("execute", &nlm::Command::execute)
        .def("getName", &nlm::Command::getName)
        .def("getDescription", &nlm::Command::getDescription)
        .def("isEnabled", &nlm::Command::isEnabled)
        .def("setEnabled", &nlm::Command::setEnabled, py::arg("enable"))
        .def("getPriority", &nlm::Command::getPriority)
        .def("setPriority", &nlm::Command::setPriority, py::arg("priority"));

    py::class_<nlm::SimpleCommand, nlm::Command>(m, "SimpleCommand")
        .def(py::init<std::unique_ptr<nlm::Action>, float>())
        .def("getAction", &nlm::SimpleCommand::getAction);

    py::class_<nlm::CommandManager>(m, "CommandManager")
        .def(py::init<>())
        .def("addCommand", &nlm::CommandManager::addCommand)
        .def("removeCommand", &nlm::CommandManager::removeCommand)
        .def("executeCommands", &nlm::CommandManager::executeCommands)
        .def("getCommand", &nlm::CommandManager::getCommand)
        .def("setCommandEnabled", &nlm::CommandManager::setCommandEnabled,
             py::arg("commandName"), py::arg("enabled"))
        .def("setCommandPriority", &nlm::CommandManager::setCommandPriority,
             py::arg("commandName"), py::arg("priority"))
        .def("getEnabledCommands", &nlm::CommandManager::getEnabledCommands)
        .def("getCommandCount", &nlm::CommandManager::getCommandCount)
        .def("getStats", &nlm::CommandManager::getStats);

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
        .def("isValid", &nlm::AdvancedTopologyConfig::isValid)
        .def("__repr__", [](const nlm::AdvancedTopologyConfig& config) {
            std::stringstream ss;
            ss << "<AdvancedTopologyConfig: type=" << static_cast<int>(config.topologyType)
               << ", layers=" << config.layerSizes.size() << ">";
            return ss.str();
        });

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

    // Threading and Parallel Processing
    py::class_<nlm::ThreadPool>(m, "ThreadPool")
        .def(py::init<size_t>(), py::arg("numThreads") = 0)
        .def("execute", &nlm::ThreadPool::execute, py::arg("task"))
        .def("waitForCompletion", &nlm::ThreadPool::waitForCompletion)
        .def("stop", &nlm::ThreadPool::stop)
        .def("threadCount", &nlm::ThreadPool::threadCount);

    py::class_<nlm::ThreadSafeAllocator>(m, "ThreadSafeAllocator")
        .def(py::init<size_t>(), py::arg("initialPoolSize") = 1024)
        .def("allocate", &nlm::ThreadSafeAllocator::allocate, py::arg("size"))
        .def("deallocate", &nlm::ThreadSafeAllocator::deallocate, py::arg("ptr"), py::arg("size"))
        .def("getTotalAllocated", &nlm::ThreadSafeAllocator::getTotalAllocated)
        .def("getPoolSize", &nlm::ThreadSafeAllocator::getPoolSize)
        .def("setTrackingEnabled", &nlm::ThreadSafeAllocator::setTrackingEnabled, py::arg("enabled"))
        .def("getStats", &nlm::ThreadSafeAllocator::getStats)
        .def("getStatsAsDict", [](const nlm::ThreadSafeAllocator& allocator) {
            auto stats = allocator.getStats();
            py::dict result;
            result["totalAllocated"] = stats.totalAllocated;
            result["currentPoolSize"] = stats.currentPoolSize;
            result["peakUsage"] = stats.peakUsage;
            result["fragmentationRatio"] = stats.fragmentationRatio;
            return result;
        });

    py::class_<nlm::ThreadSafeAllocator::MemoryStats>(m, "MemoryStats")
        .def_readonly("totalAllocated", &nlm::ThreadSafeAllocator::MemoryStats::totalAllocated)
        .def_readonly("currentPoolSize", &nlm::ThreadSafeAllocator::MemoryStats::currentPoolSize)
        .def_readonly("peakUsage", &nlm::ThreadSafeAllocator::MemoryStats::peakUsage)
        .def_readonly("fragmentationRatio", &nlm::ThreadSafeAllocator::MemoryStats::fragmentationRatio);

    py::class_<nlm::ThreadSafeSimulationManager>(m, "ThreadSafeSimulationManager")
        .def(py::init<std::shared_ptr<nlm::ThreadPool>, std::shared_ptr<nlm::ThreadSafeAllocator>>(),
             py::arg("threadPool") = nullptr, py::arg("allocator") = nullptr)
        .def("startSimulation", &nlm::ThreadSafeSimulationManager::startSimulation,
             py::arg("simulationTask"))
        .def("stopSimulation", &nlm::ThreadSafeSimulationManager::stopSimulation)
        .def("isRunning", &nlm::ThreadSafeSimulationManager::isRunning)
        .def("getStats", &nlm::ThreadSafeSimulationManager::getStats)
        .def("getStatsAsDict", [](nlm::ThreadSafeSimulationManager& manager) {
            auto stats = manager.getStats();
            py::dict result;
            result["simulationSpeed"] = stats.simulationSpeed;
            result["activeThreads"] = stats.activeThreads;
            result["totalTasksCompleted"] = stats.totalTasksCompleted;
            result["averageTaskTime"] = stats.averageTaskTime;
            result["memoryUsage"] = stats.memoryUsage;
            result["cpuUsage"] = stats.cpuUsage;
            return result;
        })
        .def("addNeuralOperation", &nlm::ThreadSafeSimulationManager::addNeuralOperation,
             py::arg("operation"))
        .def("executeOperations", &nlm::ThreadSafeSimulationManager::executeOperations)
        .def("setThreadPool", &nlm::ThreadSafeSimulationManager::setThreadPool,
             py::arg("threadPool"))
        .def("setAllocator", &nlm::ThreadSafeSimulationManager::setAllocator,
             py::arg("allocator"));

    py::class_<nlm::ThreadSafeSimulationManager::SimulationStats>(m, "SimulationStats")
        .def_readonly("simulationSpeed", &nlm::ThreadSafeSimulationManager::SimulationStats::simulationSpeed)
        .def_readonly("activeThreads", &nlm::ThreadSafeSimulationManager::SimulationStats::activeThreads)
        .def_readonly("totalTasksCompleted", &nlm::ThreadSafeSimulationManager::SimulationStats::totalTasksCompleted)
        .def_readonly("averageTaskTime", &nlm::ThreadSafeSimulationManager::SimulationStats::averageTaskTime)
        .def_readonly("memoryUsage", &nlm::ThreadSafeSimulationManager::SimulationStats::memoryUsage)
        .def_readonly("cpuUsage", &nlm::ThreadSafeSimulationManager::SimulationStats::cpuUsage);

    // Error Handling and Validation
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
        .def("isError", &nlm::ErrorResult::isError)
        .def("getMessage", &nlm::ErrorResult::getMessage)
        .def("getCategory", &nlm::ErrorResult::getCategory);

    py::class_<nlm::NLMException, std::runtime_error>(m, "NLMException")
        .def("getCategory", &nlm::NLMException::getCategory)
        .def("what", &nlm::NLMException::what);

    // Original basic bindings (preserving existing functionality)
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
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

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version information
    m.attr("__version__") = "2.0.0";
    m.attr("__description__") = "NLM Neural Learning Machine - Enhanced Python Bindings";
    m.attr("__author__") = "NLM Research Team";
}

} // namespace nlm
