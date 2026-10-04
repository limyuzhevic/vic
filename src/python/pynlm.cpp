#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../command/Command.hpp"
#include "../command/CommandManager.hpp"
#include "../factory/BrainFactory.hpp"
#include "../threading/ThreadSafeSimulationManager.hpp"
#include "../topology/AdvancedTopologyManager.hpp"
#include "../core/ErrorHandling.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace py = pybind11;

// Python bindings for NLM - Advanced Edition

// Basic brain configuration
void bindBrainConfig(py::module_ &m) {
    py::class_<nlm::Config, std::shared_ptr<nlm::Config>>(m, "Config")
        .def(py::init<>())
        .def("has", &nlm::Config::has)
        .def("getOr", &nlm::Config::getOr<int64_t>, py::arg("default"))
        .def("getOr", &nlm::Config::getOr<double>, py::arg("default"))
        .def("getOr", &nlm::Config::getOr<float>, py::arg("default"))
        .def("getOr", &nlm::Config::getOr<bool>, py::arg("default"))
        .def("set", &nlm::Config::set)
        .def("loadFromFile", &nlm::Config::loadFromFile)
        .def("saveToFile", &nlm::Config::saveToFile);
}

// Advanced brain factory
void bindBrainFactory(py::module_ &m) {
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
        .def("isValid", &nlm::BrainFactoryConfig::isValid);
    
    py::enum_<nlm::BrainComponentType>(m, "BrainComponentType")
        .value("DefaultBrain", nlm::BrainComponentType::DefaultBrain)
        .value("EnhancedBrain", nlm::BrainComponentType::EnhancedBrain)
        .value("MinimalBrain", nlm::BrainComponentType::MinimalBrain)
        .value("CustomBrain", nlm::BrainComponentType::CustomBrain);
    
    py::enum_<nlm::NeuronTypeConfig>(m, "NeuronTypeConfig")
        .value("Standard", nlm::NeuronTypeConfig::Standard)
        .value("HighConductance", nlm::NeuronTypeConfig::HighConductance)
        .value("LowThreshold", nlm::NeuronTypeConfig::LowThreshold)
        .value("Adaptive", nlm::NeuronTypeConfig::Adaptive);
    
    py::enum_<nlm::NetworkTopology>(m, "NetworkTopology")
        .value("Feedforward", nlm::NetworkTopology::Feedforward)
        .value("Recurrent", nlm::NetworkTopology::Recurrent)
        .value("Hierarchical", nlm::NetworkTopology::Hierarchical)
        .value("Modular", nlm::NetworkTopology::Modular);
}

// Command system bindings
void bindCommandSystem(py::module_ &m) {
    py::class_<nlm::Command>(m, "Command", py::multiple_lifetime")
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
}

// Advanced topology bindings
void bindAdvancedTopology(py::module_ &m) {
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
}

// Threading system bindings
void bindThreadingSystem(py::module_ &m) {
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
}

// Error handling bindings
void bindErrorHandling(py::module_ &m) {
    py::class_<nlm::ErrorResult>(m, "ErrorResult")
        .def(py::init<>())
        .def("isOk", &nlm::ErrorResult::isOk)
        .def("isError", &nlm::ErrorResult::isError)
        .def_readonly("success", &nlm::ErrorResult::success)
        .def_readonly("message", &nlm::ErrorResult::message)
        .def_readonly("category", &nlm::ErrorResult::category);
    
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
    
    py::class_<nlm::NLMException, std::runtime_error>(m, "NLMException")
        .def("getCategory", &nlm::NLMException::getCategory);
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "NLM Python Bindings - Advanced Edition\n\n"
             "This module provides advanced Python interfaces for the NLM (Neural Learning Machine) framework.\n\n"
             "Key features:\n"
             "- Advanced brain factory for creating custom brain architectures\n"
             "- Command system for sophisticated action selection\n"
             "- Thread-safe simulation management for parallel processing\n"
             "- Advanced neural network topologies (CNN, ResNet, Autoencoders, etc.)\n"
             "- Enhanced error handling and validation\n"
             "- Performance optimization tools\n"
             "- Memory management and monitoring\n\n"
             "Usage examples:\n"
             "```python\n"
             "import pynlm\n\n"
             "# Create an enhanced brain using the factory\n"
             "config = pynlm.BrainFactoryConfig()\n"
             "config.brainType = pynlm.BrainComponentType.EnhancedBrain\n"
             "config.neuronCount = 5000\n"
             "config.enablePlasticity = True\n"
             "brain = pynlm.BrainFactory.createBrain(config)\n\n"
             "# Create an advanced topology\n"
             "conv_config = pynlm.AdvancedTopologyConfig()\n"
             "conv_config.topologyType = pynlm.AdvancedTopologyType.Convolutional\n"
             "conv_config.layerSizes = [256, 128, 64, 32, 16]\n"
             "advanced_brain = pynlm.AdvancedTopologyManager.createFromTopology(conv_config)\n\n"
             "# Use command system for action selection\n"
             "command_manager = pynlm.CommandManager()\n"
             "# Add custom commands...\n"
             "action = command_manager.executeCommands(brain)\n"
             "```";
    
    // Bind all modules
    bindBrainConfig(m);
    bindBrainFactory(m);
    bindCommandSystem(m);
    bindAdvancedTopology(m);
    bindThreadingSystem(m);
    bindErrorHandling(m);
    
    // Create a convenience function for creating default brain
    m.def("createDefaultBrain", []() {
        auto config = nlm::BrainFactory::getDefaultConfig();
        return nlm::BrainFactory::createBrain(config);
    });
    
    m.def("createEnhancedBrain", []() {
        auto config = nlm::BrainFactory::getEnhancedConfig();
        return nlm::BrainFactory::createBrain(config);
    });
    
    m.def("createMinimalBrain", [](size_t neuronCount) {
        return nlm::BrainFactory::createMinimalBrain(neuronCount);
    });
}
