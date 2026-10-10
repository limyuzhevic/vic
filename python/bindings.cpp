#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/json.hpp>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <sstream>

// Core headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception classes
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

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& message) : NLMError("Validation Error: " + message) {}
};

// Utility functions for error handling and validation
inline void validatePositive(const std::string& name, double value) {
    if (value < 0.0) {
        throw ValidationError("" + name + " must be non-negative, got " + std::to_string(value));
    }
}

inline void validateRange(const std::string& name, double value, double min, double max) {
    if (value < min || value > max) {
        throw ValidationError("" + name + " must be in range [" + std::to_string(min) + ", " + std::to_string(max) + "], got " + std::to_string(value));
    }
}

inline void validateStringNotEmpty(const std::string& name, const std::string& value) {
    if (value.empty()) {
        throw ValidationError("" + name + " cannot be empty");
    }
}

// Utility functions for data conversion and serialization
inline std::string vectorToJson(const std::vector<float>& data) {
    py::json j = data;
    return j.dump();
}

inline std::vector<float> jsonToVector(const std::string& jsonStr) {
    try {
        py::json j = py::json::parse(jsonStr);
        return j.get<std::vector<float>>();
    } catch (const std::exception& e) {
        throw NLMError("Failed to parse JSON string to vector: " + std::string(e.what()));
    }
}

inline std::string objectToJson(const py::object& obj) {
    py::json j = obj;
    return j.dump();
}

inline py::object jsonToObject(const std::string& jsonStr) {
    try {
        py::json j = py::json::parse(jsonStr);
        return py::cast<py::object>(j);
    } catch (const std::exception& e) {
        throw NLMError("Failed to parse JSON string to object: " + std::string(e.what()));
    }
}

// Exception translation helpers
define_exception_translators() {
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const ConfigurationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const SimulationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const ValidationError& e) {
            PyErr_SetString(PyExc_TypeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
}

// Configuration presets
inline std::shared_ptr<Config> createDefaultConfig() {
    auto config = std::make_shared<Config>();
    config->set("brain.simulated_neurons", 1000);
    config->set("brain.simulated_synapses", 10000);
    config->set("brain.simulated_regions", 10);
    config->set("brain.simulation_time_step", 0.001);
    config->set("brain.simulation_duration", 1000.0);
    config->set("brain.vision.enabled", true);
    config->set("brain.vision.width", 64);
    config->set("brain.vision.height", 64);
    config->set("brain.vision.channels", 3);
    config->set("brain.audio.enabled", true);
    config->set("brain.audio.sampleRate", 16000);
    config->set("brain.audio.numSamples", 512);
    config->set("agent.rewardModulation.enabled", true);
    config->set("agent.structuralPlasticity.enabled", true);
    config->set("agent.development.enabled", true);
    config->set("agent.curiosity.enabled", true);
    config->set("agent.predictionError.enabled", true);
    config->set("agent.novelty.enabled", true);
    return config;
}

inline std::shared_ptr<Config> createMinimalConfig() {
    auto config = std::make_shared<Config>();
    config->set("brain.simulated_neurons", 100);
    config->set("brain.simulated_synapses", 500);
    config->set("brain.simulation_time_step", 0.001);
    return config;
}

inline std::shared_ptr<Config> createHighPerformanceConfig() {
    auto config = std::make_shared<Config>();
    config->set("brain.simulated_neurons", 10000);
    config->set("brain.simulated_synapses", 50000);
    config->set("brain.simulation_time_step", 0.0001);
    config->set("brain.simulation_duration", 500.0);
    config->set("agent.rewardModulation.enabled", true);
    config->set("agent.curiosity.enabled", true);
    return config;
}

// Environment presets
inline std::shared_ptr<Config> createGridWorldConfig() {
    auto config = std::make_shared<Config>();
    config->set("world.type", "grid");
    config->set("world.width", 20);
    config->set("world.height", 20);
    config->set("world.cellSize", 1.0);
    config->set("world.agentStartX", 10.0);
    config->set("world.agentStartY", 10.0);
    config->set("world.maxEnergy", 100.0);
    config->set("world.energyDecayRate", 0.01);
    return config;
}

inline std::shared_ptr<Config> createMazeWorldConfig() {
    auto config = std::make_shared<Config>();
    config->set("world.type", "maze");
    config->set("world.width", 30);
    config->set("world.height", 30);
    config->set("world.cellSize", 1.0);
    config->set("world.agentStartX", 15.0);
    config->set("world.agentStartY", 15.0);
    config->set("world.maxEnergy", 150.0);
    config->set("world.energyDecayRate", 0.005);
    config->set("world.hasHazards", true);
    config->set("world.hasResources", true);
    return config;
}

// Performance monitoring utilities
class PerformanceMonitor {
private:
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastStepTime;
    uint64_t totalSteps = 0;
    uint64_t totalNeuronsActive = 0;
    uint64_t totalSpikesGenerated = 0;
    double totalTime = 0.0;
    double lastStepTimeDelta = 0.0;
    
public:
    PerformanceMonitor() : startTime(std::chrono::steady_clock::now()) {
        lastStepTime = startTime;
    }
    
    void recordStep(uint64_t neuronsActive, uint64_t spikesGenerated, double timeDelta) {
        totalSteps++;
        totalNeuronsActive += neuronsActive;
        totalSpikesGenerated += spikesGenerated;
        totalTime += timeDelta;
        lastStepTimeDelta = timeDelta;
        lastStepTime = std::chrono::steady_clock::now();
    }
    
    double getElapsedTime() const {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration<double>(now - startTime).count();
    }
    
    double getAverageStepTime() const {
        return totalSteps > 0 ? totalTime / totalSteps : 0.0;
    }
    
    double getLastStepTime() const {
        return lastStepTimeDelta;
    }
    
    double getStepsPerSecond() const {
        return totalSteps > 0 ? totalSteps / getElapsedTime() : 0.0;
    }
    
    uint64_t getTotalSteps() const { return totalSteps; }
    
    uint64_t getAverageActiveNeurons() const {
        return totalSteps > 0 ? totalNeuronsActive / totalSteps : 0;
    }
    
    uint64_t getAverageSpikesPerStep() const {
        return totalSteps > 0 ? totalSpikesGenerated / totalSteps : 0;
    }
    
    std::string getPerformanceReport() const {
        std::stringstream ss;
        ss << "Performance Monitor Report:\n";
        ss << "  Total steps: " << totalSteps << "\n";
        ss << "  Total time: " << getElapsedTime() << "s\n";
        ss << "  Average step time: " << getAverageStepTime() * 1000 << "ms\n";
        ss << "  Steps per second: " << getStepsPerSecond() << "\n";
        ss << "  Average active neurons: " << getAverageActiveNeurons() << "\n";
        ss << "  Average spikes per step: " << getAverageSpikesPerStep() << "\n";
        return ss.str();
    }
    
    void reset() {
        startTime = std::chrono::steady_clock::now();
        lastStepTime = startTime;
        totalSteps = 0;
        totalNeuronsActive = 0;
        totalSpikesGenerated = 0;
        totalTime = 0.0;
        lastStepTimeDelta = 0.0;
    }
};

// Batch operations utilities
class BatchProcessor {
private:
    std::vector<std::function<void()>> operations;
    std::string batchName;
    bool isProcessing = false;
    
public:
    BatchProcessor(const std::string& name = "Batch") : batchName(name) {}
    
    void addOperation(const std::function<void()>& operation) {
        operations.push_back(operation);
    }
    
    void execute() {
        if (isProcessing) {
            throw SimulationError("Cannot execute batch: already processing");
        }
        isProcessing = true;
        try {
            for (size_t i = 0; i < operations.size(); ++i) {
                operations[i]();
            }
        } catch (...) {
            isProcessing = false;
            throw;
        }
        isProcessing = false;
        operations.clear();
    }
    
    size_t getOperationCount() const { return operations.size(); }
    bool isActive() const { return isProcessing; }
    std::string getName() const { return batchName; }
};

// Simulation control utilities
class SimulationController {
private:
    bool isPaused = false;
    bool isReset = false;
    bool isRunning = false;
    double pauseTime = 0.0;
    std::chrono::steady_clock::time_point pauseStart;
    std::shared_ptr<PerformanceMonitor> monitor;
    
public:
    SimulationController() : monitor(std::make_shared<PerformanceMonitor>()) {}
    
    void pause() {
        if (!isRunning) {
            throw SimulationError("Cannot pause: simulation not running");
        }
        if (isPaused) {
            throw SimulationError("Cannot pause: already paused");
        }
        isPaused = true;
        pauseStart = std::chrono::steady_clock::now();
    }
    
    void resume() {
        if (!isPaused) {
            throw SimulationError("Cannot resume: not paused");
        }
        isPaused = false;
        auto pauseDuration = std::chrono::steady_clock::now() - pauseStart;
        pauseTime += std::chrono::duration<double>(pauseDuration).count();
    }
    
    void reset() {
        isPaused = false;
        isReset = true;
        isRunning = false;
        monitor->reset();
    }
    
    void start() {
        if (isPaused) {
            resume();
        } else {
            isRunning = true;
            isReset = false;
        }
    }
    
    void stop() {
        if (isRunning) {
            isRunning = false;
            if (isPaused) {
                resume();
            }
        }
    }
    
    bool isSimulationPaused() const { return isPaused; }
    bool isSimulationRunning() const { return isRunning; }
    bool isSimulationReset() const { return isReset; }
    double getTotalPauseTime() const { return pauseTime; }
    
    std::shared_ptr<PerformanceMonitor> getMonitor() const { return monitor; }
    
    std::string getStatus() const {
        std::stringstream ss;
        ss << "Simulation Status:\n";
        ss << "  Running: " << (isRunning ? "Yes" : "No") << "\n";
        ss << "  Paused: " << (isPaused ? "Yes" : "No") << "\n";
        ss << "  Reset: " << (isReset ? "Yes" : "No") << "\n";
        ss << "  Total pause time: " << pauseTime << "s\n";
        return ss.str();
    }
};

// Experiment utilities
class ExperimentManager {
private:
    std::vector<std::shared_ptr<Experiment>> experiments;
    std::shared_ptr<ExperimentRunner> runner;
    std::string currentExperimentName;
    
public:
    ExperimentManager() {
        runner = std::make_shared<ExperimentRunner>();
    }
    
    void addExperiment(const std::string& name, const std::shared_ptr<Experiment>& experiment) {
        validateStringNotEmpty("Experiment name", name);
        if (experiment == nullptr) {
            throw ValidationError("Experiment cannot be null");
        }
        experiment->setName(name);
        experiments.push_back(experiment);
    }
    
    void runExperiment(const std::string& name, std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, double duration) {
        auto it = std::find_if(experiments.begin(), experiments.end(),
            [&name](const std::shared_ptr<Experiment>& exp) { return exp->getName() == name; });
        if (it == experiments.end()) {
            throw ConfigurationError("Experiment not found: " + name);
        }
        currentExperimentName = name;
        runner->run(*it, brain, world, duration);
    }
    
    std::vector<std::string> getExperimentNames() const {
        std::vector<std::string> names;
        names.reserve(experiments.size());
        for (const auto& exp : experiments) {
            names.push_back(exp->getName());
        }
        return names;
    }
    
    std::shared_ptr<Experiment> getExperiment(const std::string& name) {
        auto it = std::find_if(experiments.begin(), experiments.end(),
            [&name](const std::shared_ptr<Experiment>& exp) { return exp->getName() == name; });
        if (it == experiments.end()) {
            throw ConfigurationError("Experiment not found: " + name);
        }
        return *it;
    }
    
    void clearExperiments() {
        experiments.clear();
        currentExperimentName.clear();
    }
    
    std::string getCurrentExperiment() const { return currentExperimentName; }
};

// Visualization helpers
inline std::string generateNeuronActivityPlot(const std::shared_ptr<Brain>& brain) {
    std::stringstream ss;
    ss << "Neuron Activity Plot (Region-based):\n";
    auto regions = brain->getRegions();
    for (const auto& region : regions) {
        auto neurons = region->getNeurons();
        ss << "Region " << region->getId().value << ": " << neurons.size() << " neurons\n";
        if (!neurons.empty()) {
            uint64_t activeCount = 0;
            for (const auto& neuron : neurons) {
                if (neuron->isActive()) {
                    activeCount++;
                }
            }
            ss << "  Active neurons: " << activeCount << "/" << neurons.size() << " (" 
               << (neurons.size() > 0 ? (100.0 * activeCount / neurons.size()) : 0.0) << "%)\n";
        }
    }
    return ss.str();
}

inline std::string generateConnectionDensityPlot(const std::shared_ptr<Brain>& brain) {
    std::stringstream ss;
    ss << "Connection Density Analysis:\n";
    uint64_t totalNeurons = brain->getTotalNeuronCount();
    uint64_t totalSynapses = brain->getTotalSynapseCount();
    double density = totalNeurons > 0 ? (100.0 * totalSynapses / (totalNeurons * totalNeurons)) : 0.0;
    
    ss << "Total neurons: " << totalNeurons << "\n";
    ss << "Total synapses: " << totalSynapses << "\n";
    ss << "Connection density: " << density << " synapses/neuron^2\n";
    
    ss << "\nSynapse type distribution:\n";
    auto regions = brain->getRegions();
    for (const auto& region : regions) {
        auto neurons = region->getNeurons();
        uint64_t totalRegionSynapses = 0;
        uint64_t exciterator = 0, inhibitor = 0, modulatory = 0;
        
        for (const auto& neuron : neurons) {
            totalRegionSynapses += neuron->getNumOutgoingSynapses();
            for (const auto& synapse : neuron->getOutgoingSynapses()) {
                switch (synapse->getType()) {
                    case SynapseType::Excitatory: exciterator++; break;
                    case SynapseType::Inhibitory: inhibitor++; break;
                    case SynapseType::Modulatory: modulatory++; break;
                    case SynapseType::Electrical: modulatory++; break;
                    case SynapseType::GapJunction: modulatory++; break;
                }
            }
        }
        
        if (totalRegionSynapses > 0) {
            ss << "  Region " << region->getId().value << ":\n";
            ss << "    Excitatory: " << (100.0 * exciterator / totalRegionSynapses) << "%\n";
            ss << "    Inhibitory: " << (100.0 * inhibitor / totalRegionSynapses) << "%\n";
            ss << "    Modulatory: " << (100.0 * modulatory / totalRegionSynapses) << "%\n";
        }
    }
    
    return ss.str();
}

PYBIND11_MODULE(pynlm, m) {
    // Setup exception translators
    define_exception_translators();
    
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This module provides:
        - Enhanced error handling with custom exception types
        - Factory methods and convenience functions for quick setup
        - Configuration presets and environment templates
        - Performance monitoring and simulation control
        - Advanced utility functions for data processing
        - Comprehensive documentation and type safety
        
        Features:
        * Custom exception types: NLMError, ConfigurationError, SimulationError, ValidationError
        * Configuration presets: default, minimal, highPerformance
        * Environment presets: gridWorld, mazeWorld
        * Simulation controller with pause/resume/reset
        * Performance monitoring with detailed statistics
        * Batch processing utilities
        * Experiment management system
        * Data serialization utilities (JSON conversion)
        * Visualization helpers
        * Advanced validation and input checking
        
        Installation: pip install pynlm
        Documentation: https://github.com/neural-learning-machine/NLM
    )pbdoc";
    
    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_TypeError);
    
    // Register NeuronId with enhanced features
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron with enhanced functionality.
        
        Provides:
        - Value-based identification
        - Comparison operators
        - Hash support for use in sets and dictionaries
        - String representation
        - Index calculation for efficient lookups
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value,
            "The numeric identifier value")
        .def("index", &NeuronId::index,
            "Get the index of this neuron ID for efficient lookups")
        .def("__eq__", &NeuronId::operator==,
            "Check equality with another NeuronId")
        .def("__ne__", &NeuronId::operator!=,
            "Check inequality with another NeuronId")
        .def("__hash__", [](const NeuronId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash value for use in sets and dictionaries")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "String representation of the neuron ID")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });
    
    // Register SynapseId with enhanced features
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse with enhanced functionality.
        
        Provides:
        - Value-based identification
        - Comparison operators
        - Hash support for use in sets and dictionaries
        - String representation
        - Index calculation for efficient lookups
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value,
            "The numeric identifier value")
        .def("index", &SynapseId::index,
            "Get the index of this synapse ID for efficient lookups")
        .def("__eq__", &SynapseId::operator==,
            "Check equality with another SynapseId")
        .def("__ne__", &SynapseId::operator!=,
            "Check inequality with another SynapseId")
        .def("__hash__", [](const SynapseId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash value for use in sets and dictionaries")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "String representation of the synapse ID")
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });
    
    // Register RegionId with enhanced features
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region with enhanced functionality.
        
        Provides:
        - Value-based identification
        - Comparison operators
        - Hash support for use in sets and dictionaries
        - String representation
        - Index calculation for efficient lookups
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value,
            "The numeric identifier value")
        .def("index", &RegionId::index,
            "Get the index of this region ID for efficient lookups")
        .def("__eq__", &RegionId::operator==,
            "Check equality with another RegionId")
        .def("__ne__", &RegionId::operator!=,
            "Check inequality with another RegionId")
        .def("__hash__", [](const RegionId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash value for use in sets and dictionaries")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "String representation of the region ID")
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });
    
    // Register PopulationId with enhanced features
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population with enhanced functionality.
        
        Provides:
        - Value-based identification
        - Comparison operators
        - Hash support for use in sets and dictionaries
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value,
            "The numeric identifier value")
        .def("index", &PopulationId::index,
            "Get the index of this population ID for efficient lookups")
        .def("__eq__", &PopulationId::operator==,
            "Check equality with another PopulationId")
        .def("__ne__", &PopulationId::operator!=,
            "Check inequality with another PopulationId")
        .def("__hash__", [](const PopulationId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash value for use in sets and dictionaries");
    
    // Register NeuronType enum with detailed documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the NLM system.
        
        Neuron types determine:
        - Function and connectivity patterns
        - Response characteristics
        - Neuromodulation sensitivity
        - Developmental pathways
        
        Types:
        - Excitatory: Increases firing probability in target neurons
        - Inhibitory: Decreases firing probability in target neurons  
        - Modulatory: Alters synaptic strength or neural excitability
        - Sensory: Processes external sensory inputs
        - Motor: Generates motor commands for actions
        - Internal: Maintains internal state and homeostasis
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Neuron that increases firing in targets")
        .value("Inhibitory", NeuronType::Inhibitory, "Neuron that decreases firing in targets")
        .value("Modulatory", NeuronType::Modulatory, "Neuron that alters synaptic strength")
        .value("Sensory", NeuronType::Sensory, "Neuron that processes sensory input")
        .value("Motor", NeuronType::Motor, "Neuron that generates motor commands")
        .value("Internal", NeuronType::Internal, "Neuron that maintains internal state")
        .export_values();
    
    // Register SynapseType enum with detailed documentation
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse types in the NLM system.
        
        Synapse types determine:
        - Communication mechanism
        - Signal properties
        - Plasticity rules
        - Neuromodulation effects
        
        Types:
        - Excitatory: Releases excitatory neurotransmitters
        - Inhibitory: Releases inhibitory neurotransmitters
        - Modulatory: Releases neuromodulators
        - Electrical: Direct electrical coupling
        - GapJunction: Specialized electrical synapses
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synaptic transmission")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synaptic transmission")
        .value("Modulatory", SynapseType::Modulatory, "Neuromodulatory synaptic transmission")
        .value("Electrical", SynapseType::Electrical, "Direct electrical coupling")
        .value("GapJunction", SynapseType::GapJunction, "Specialized electrical synapse")
        .export_values();
    
    // Register DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages for neurons and brain regions.
        
        Stages:
        - Initial: Early developmental phase
        - CriticalPeriod: Plasticity maximization phase
        - Maturation: Refinement and specialization phase
        - Adult: Stable, mature state
        - Aging: Age-related changes and decline
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    // Register FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states.
        
        States:
        - Resting: Not currently firing
        - Active: Currently firing spikes
        - Refractory: Cannot fire after recent activity
        - Inhibited: Suppressed by inhibitory inputs
    )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();
    
    // Register ActionType enum with comprehensive documentation
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of possible action types for agent behavior.
        
        Actions determine:
        - What the agent does
        - How it interacts with the world
        - What sensory feedback it generates
        - What consequences result
        
        Categories:
        - Movement: Navigation and locomotion
        - Sensing: Information gathering
        - Interaction: Environment manipulation
        - Survival: Basic needs fulfillment
        - Social: Interaction with other agents
        - Custom: User-defined behaviors
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in current direction")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward")
        .value("MoveLeft", ActionType::MoveLeft, "Move left")
        .value("MoveRight", ActionType::MoveRight, "Move right")
        .value("TurnLeft", ActionType::TurnLeft, "Rotate left")
        .value("TurnRight", ActionType::TurnRight, "Rotate right")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with object")
        .value("Eat", ActionType::Eat, "Consume food/resource")
        .value("Drink", ActionType::Drink, "Drink water")
        .value("Rest", ActionType::Rest, "Rest and recover energy")
        .value("Wait", ActionType::Wait, "Do nothing, wait for next step")
        .value("Custom", ActionType::Custom, "Custom user-defined action")
        .export_values();
    
    // Register MotorCommand enum
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor commands sent to the world simulation.
        
        These are the atomic actions that can be executed:
        - Movement commands
        - Sensing commands
        - Interaction commands
        - Idle commands
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();
    
    // Register WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of object types in the world simulation.
        
        Object types determine:
        - How objects interact with agents
        - What effects they produce
        - How they're perceived
        
        Types:
        - Empty: No object (ground)
        - Resource: Consumable resource
        - Hazard: Dangerous object
        - Wall: Obstacle
        - Marker: Reference point
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "No object (empty space)")
        .value("Resource", WorldObjectType::Resource, "Consumable resource")
        .value("Hazard", WorldObjectType::Hazard, "Dangerous object")
        .value("Wall", WorldObjectType::Wall, "Obstacle wall")
        .value("Marker", WorldObjectType::Marker, "Reference point marker")
        .export_values();
    
    // Enhanced Config class with validation and convenience methods
    py::class_<Config>(m, "Config", R"pbdoc(
        Enhanced configuration manager for NLM system with validation and presets.
        
        Provides:
        - Key-value configuration storage
        - JSON file import/export
        - Command-line argument loading
        - Type-safe value access with validation
        - Configuration presets and templates
        - Configuration validation
        
        Usage:
            config = nlm.Config()
            config.set('brain.simulated_neurons', 1000)
            config.set('world.width', 20)
            
            # Load from file
            config.loadFromFile('config.json')
            
            # Load from command line
            config.loadFromArgs(sys.argv)
        
        Common keys:
        - brain.simulated_neurons: Number of neurons to simulate
        - brain.simulated_synapses: Number of synapses
        - brain.vision.width/height/channels: Vision parameters
        - world.width/height: World dimensions
        - agent.rewardModulation.enabled: Enable reward modulation
    )pbdoc")
        .def(py::init<>())
        .def("has", &Config::has, py::arg("key"),
            "Check if a configuration key exists")
        .def("get", [](const Config& self, const std::string& key) {
            try {
                return py::cast<py::object>(self.get(key));
            } catch (const std::exception& e) {
                throw ConfigurationError("Failed to get config value for key '" + key + "': " + std::string(e.what()));
            }
        }, py::arg("key"), "Get a configuration value as Python object")
        .def("get_int", [](const Config& self, const std::string& key, int defaultValue = 0) {
            try {
                return self.getInt(key, defaultValue);
            } catch (const std::exception& e) {
                throw ConfigurationError("Failed to get int config value for key '" + key + "': " + std::string(e.what()));
            }
        }, py::arg("key"), py::arg("defaultValue") = 0,
            "Get an integer configuration value with default")
        .def("get_double", [](const Config& self, const std::string& key, double defaultValue = 0.0) {
            try {
                return self.getDouble(key, defaultValue);
            } catch (const std::exception& e) {
                throw ConfigurationError("Failed to get double config value for key '" + key + "': " + std::string(e.what()));
            }
        }, py::arg("key"), py::arg("defaultValue") = 0.0,
            "Get a double configuration value with default")
        .def("get_string", [](const Config& self, const std::string& key, const std::string& defaultValue = "") {
            try {
                return self.getString(key, defaultValue);
            } catch (const std::exception& e) {
                throw ConfigurationError("Failed to get string config value for key '" + key + "': " + std::string(e.what()));
            }
        }, py::arg("key"), py::arg("defaultValue") = "",
            "Get a string configuration value with default")
        .def("set", [](Config& self, const std::string& key, int value) {
            validateStringNotEmpty("key", key);
            try {
                self.set(key, value);
            } catch (const std::exception& e) {
                throw ConfigurationError("Failed to set config value for key '" + key + "': " + std::string(e.what()));
            }
        }, py::arg("key"), py::arg("value"),
            "Set an integer configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
            validateStringNotEmpty("key", key);
            try {
                self.set(key, value);
            } catch (const std::exception& e) {
                throw ConfigurationError("Failed to set config value for key '" + key + "': " + std::string(e.what()));
            }
        }, py::arg("key"), py::arg("value"),
            "Set a double configuration value")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            validateStringNotEmpty("key", key);
            validateStringNotEmpty("value", value);
            try {
                self.set(key, value);
            } catch (const std::exception& e) {
                throw ConfigurationError("Failed to set config value for key '" + key + "': " + std::string(e.what()));
            }
        }, py::arg("key"), py::arg("value"),
            "Set a string configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            validateStringNotEmpty("key", key);
            try {
                self.set(key, value);
            } catch (const std::exception& e) {
                throw ConfigurationError("Failed to set config value for key '" + key + "': " + std::string(e.what()));
            }
        }, py::arg("key"), py::arg("value"),
            "Set a boolean configuration value")
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
            "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                return self.loadFromArgs(argc, argv);
            } catch (const std::exception& e) {
                throw ConfigurationError("Failed to load configuration from args: " + std::string(e.what()));
            }
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
            "Save configuration to a JSON file")
        .def("getKeys", &Config::getKeys,
            "Get all configuration keys")
        .def("clear", &Config::clear,
            "Clear all configuration entries")
        .def("summary", &Config::summary,
            "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "String representation of the configuration");
    
    // Enhanced sensory input classes with better constructors
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory input in the NLM system.
        
        Provides:
        - Generic sensory data interface
        - Type identification
        - Data access methods
        - Timestamp tracking
        - Data validation
        
        Subclasses:
        - Vision: Visual sensory input
        - Audio: Auditory sensory input  
        - InternalSignals: Internal physiological signals
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
            "Set the timestamp")
        .def("validate", []() { return true; }, "Validate the sensory input data");
    
    // Enhanced Vision class with convenient constructors
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input with enhanced functionality.
        
        Represents visual information with:
        - 2D image data
        - Color channels (RGB or grayscale)
        - Width/height dimensions
        - Convenience constructors
        
        Constructors:
        - Default constructor
        - Parameterized constructor with width, height, channels
        - Data setting from vector
        
        Typical usage:
            vision = nlm.Vision(width, height, channels)
            vision.setData(data_vector)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width") = 64, py::arg("height") = 64, py::arg("channels") = 3,
            "Create a Vision input with specified dimensions and channels")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw ValidationError("Vision data cannot be empty");
            }
            size_t expectedSize = self.getWidth() * self.getHeight() * self.getChannels();
            if (data.size() != expectedSize) {
                throw ValidationError("Vision data size mismatch. Expected " + 
                    std::to_string(expectedSize) + " elements (width*height*channels), got " + 
                    std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data with validation")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels")
        .def("getNormalizedData", [](const Vision& self) {
            std::vector<float> data = self.getData();
            float maxVal = *std::max_element(data.begin(), data.end());
            float minVal = *std::min_element(data.begin(), data.end());
            if (maxVal - minVal > 0.0f) {
                for (auto& val : data) {
                    val = (val - minVal) / (maxVal - minVal);
                }
            }
            return data;
        }, "Get normalized data (0-1 range)")
        .def("isValid", [](const Vision& self) {
            size_t expectedSize = self.getWidth() * self.getHeight() * self.getChannels();
            auto data = self.getData();
            return data.size() == expectedSize && expectedSize > 0;
        }, "Check if vision data is valid and properly sized");
    
    // Enhanced Audio class with validation
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input with enhanced functionality.
        
        Represents auditory information with:
        - Digital audio samples
        - Sample rate and duration
        - Sample data validation
        - Convenience constructors
        
        Constructors:
        - Default constructor
        - Parameterized constructor with sampleRate and numSamples
        - Data setting from vector
        
        Typical usage:
            audio = nlm.Audio(sample_rate, num_samples)
            audio.setData(samples)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
            "Create an Audio input with specified sample rate and duration")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw ValidationError("Audio data cannot be empty");
            }
            if (data.size() != self.getNumSamples()) {
                throw ValidationError("Audio data size mismatch. Expected " + 
                    std::to_string(self.getNumSamples()) + " samples, got " + 
                    std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data with validation")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
            "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples")
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<double>(self.getSampleRate());
        }, "Get audio duration in seconds")
        .def("getNormalizedData", [](const Audio& self) {
            std::vector<float> data = self.getData();
            float maxVal = *std::max_element(data.begin(), data.end());
            float minVal = *std::min_element(data.begin(), data.end());
            if (maxVal - minVal > 0.0f) {
                for (auto& val : data) {
                    val = (val - minVal) / (maxVal - minVal);
                }
            }
            return data;
        }, "Get normalized audio samples (0-1 range)");
    
    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input representing physiological states.
        
        Represents internal bodily states with:
        - Multiple signal channels
        - Signal addition and management
        - Validation and bounds checking
        - Convenience methods
        
        Typical usage:
            signals = nlm.InternalSignals()
            signals.addSignal(0.5)  # Heart rate
            signals.addSignal(0.3)  # Hunger level
            signals.addSignal(0.8)  # Stress level
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", [](InternalSignals& self, double value) {
            validateRange("signal value", value, -1.0, 1.0);
            self.addSignal(value);
        }, py::arg("value"), "Add a normalized signal value (-1 to 1)")
        .def("addSignal", [](InternalSignals& self, double value, const std::string& name) {
            validateRange("signal value", value, -1.0, 1.0);
            validateStringNotEmpty("signal name", name);
            self.addSignal(value);
        }, py::arg("value"), py::arg("name"),
            "Add a named signal value (-1 to 1)")
        .def("getSignalCount", &InternalSignals::getSignalCount,
            "Get the number of signals")
        .def("getSignalNames", [](const InternalSignals& self) {
            std::vector<std::string> names;
            for (size_t i = 0; i < self.getSignalCount(); ++i) {
                names.push_back("signal_" + std::to_string(i));
            }
            return names;
        }, "Get signal names")
        .def("getAllSignals", [](const InternalSignals& self) {
            return self.getData();
        }, "Get all signals as a vector")
        .def("clearSignals", &InternalSignals::clearSignals,
            "Clear all signals")
        .def("normalizeSignals", [](InternalSignals& self) {
            auto data = self.getData();
            if (!data.empty()) {
                double maxVal = *std::max_element(data.begin(), data.end());
                double minVal = *std::min_element(data.begin(), data.end());
                if (maxVal - minVal > 0.0) {
                    for (auto& val : data) {
                        val = (val - minVal) / (maxVal - minVal);
                    }
                    self.setData(data);
                }
            }
        }, "Normalize all signals to 0-1 range")
        .def("isValid", [](const InternalSignals& self) {
            auto data = self.getData();
            return std::all_of(data.begin(), data.end(),
                [](double val) { return !std::isnan(val) && !std::isinf(val); });
        }, "Check if signals are valid");
    
    // Enhanced Action class with factory methods
    py::class_<Action>(m, "Action", R"pbdoc(
        Enhanced action representation for motor output with factory methods.
        
        Represents agent behavior with:
        - Action type and parameters
        - Convenience constructors
        - Factory methods for common actions
        - Serialization support
        - Validation and error handling
        
        Constructors:
        - Default constructor
        - Type-only constructor
        - Type + parameters constructor
        
        Factory methods:
        - createMoveAction(type, distance)
        - createLookAction(direction)
        - createInteractAction(target)
        - createCustomAction(name, params)
        
        Typical usage:
            move_action = nlm.Action.createMoveAction('MoveForward', [1.0, 0.0, 0.0])
            look_action = nlm.Action.createLookAction('LookLeft')
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
            "Create an action with type and parameters")
        .def_static("createMoveAction", [](ActionType type, const std::vector<float>& direction) {
            validateStringNotEmpty("direction", "direction_");
            auto action = std::make_shared<Action>(type, direction);
            return action;
        }, py::arg("type"), py::arg("direction"),
            "Factory method to create a movement action")
        .def_static("createLookAction", [](ActionType type, const std::vector<float>& direction) {
            auto action = std::make_shared<Action>(type, direction);
            return action;
        }, py::arg("type"), py::arg("direction"),
            "Factory method to create a looking action")
        .def_static("createInteractAction", [](const std::vector<float>& targetPosition) {
            std::vector<float> params = {targetPosition[0], targetPosition[1]};
            auto action = std::make_shared<Action>(ActionType::Interact, params);
            return action;
        }, py::arg("targetPosition"),
            "Factory method to create an interaction action")
        .def_static("createRestAction", []() {
            auto action = std::make_shared<Action>(ActionType::Rest);
            return action;
        }, "Factory method to create a rest action")
        .def_static("createWaitAction", []() {
            auto action = std::make_shared<Action>(ActionType::Wait);
            return action;
        }, "Factory method to create a wait action")
        .def_static("createCustomAction", [](const std::string& name, const std::vector<float>& params) {
            validateStringNotEmpty("action name", name);
            auto action = std::make_shared<Action>(ActionType::Custom, params);
            action->setName(name);
            return action;
        }, py::arg("name"), py::arg("params"),
            "Factory method to create a custom action")
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get the action name")
        .def("setName", [](Action& self, const std::string& name) {
            validateStringNotEmpty("action name", name);
            self.setName(name);
        }, py::arg("name"), "Set the action name")
        .def("getNormalizedParameters", [](const Action& self) {
            auto params = self.getParameters();
            if (!params.empty()) {
                float maxVal = *std::max_element(params.begin(), params.end());
                float minVal = *std::min_element(params.begin(), params.end());
                if (maxVal - minVal > 0.0f) {
                    std::vector<float> normalized(params.size());
                    for (size_t i = 0; i < params.size(); ++i) {
                        normalized[i] = (params[i] - minVal) / (maxVal - minVal);
                    }
                    return normalized;
                }
            }
            return params;
        }, "Get normalized parameters (0-1 range)")
        .def("clone", &Action::clone, "Clone this action")
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = self.getType();
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert action to Python dictionary")
        .def_static("from_dict", [](const py::dict& d) {
            ActionType type = d["type"].cast<ActionType>();
            std::vector<float> params = d["parameters"].cast<std::vector<float>>();
            auto action = std::make_shared<Action>(type, params);
            if (d.contains("name")) {
                action->setName(d["name"].cast<std::string>());
            }
            return action;
        }, py::arg("dict"), "Create action from Python dictionary");
    
    // Enhanced WorldObject class with factory methods
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Enhanced world object representation with factory methods.
        
        Represents objects in the world simulation with:
        - Position, radius, type, and value
        - Convenience constructors
        - Factory methods for common objects
        - Validation and error handling
        
        Constructors:
        - Default constructor
        - Parameterized constructor with position and properties
        
        Factory methods:
        - createResource(x, y, value)
        - createHazard(x, y, radius)
        - createWall(x, y, radius)
        - createMarker(x, y)
        
        Typical usage:
            resource = nlm.WorldObject.createResource(10.0, 10.0, 50.0)
            wall = nlm.WorldObject.createWall(15.0, 15.0, 2.0)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
            py::arg("x") = 0.0f, py::arg("y") = 0.0f, py::arg("type") = WorldObjectType::Empty,
            py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
            "Create a world object with specified properties")
        .def_static("createResource", [](float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
            "Factory method to create a resource object")
        .def_static("createHazard", [](float x, float y, float radius) {
            WorldObject obj(x, y, WorldObjectType::Hazard, 1.0f, radius);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 1.0f,
            "Factory method to create a hazard object")
        .def_static("createWall", [](float x, float y, float radius) {
            WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, radius);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 1.0f,
            "Factory method to create a wall object")
        .def_static("createMarker", [](float x, float y) {
            WorldObject obj(x, y, WorldObjectType::Marker, 0.0f, 0.1f);
            return obj;
        }, py::arg("x"), py::arg("y"),
            "Factory method to create a marker object")
        .def_readwrite("x", &WorldObject::x, "Object X position")
        .def_readwrite("y", &WorldObject::y, "Object Y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Object active state")
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("isColliding", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return (dx*dx + dy*dy) <= (self.radius + other.radius) * (self.radius + other.radius);
        }, py::arg("other"), "Check if colliding with another object")
        .def("to_dict", [](const WorldObject& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["radius"] = self.radius;
            result["type"] = self.type;
            result["value"] = self.value;
            result["active"] = self.active;
            return result;
        }, "Convert world object to Python dictionary")
        .def_static("from_dict", [](const py::dict& d) {
            WorldObject obj(
                d["x"].cast<float>(),
                d["y"].cast<float>(),
                d["type"].cast<WorldObjectType>(),
                d["value"].cast<float>(),
                d["radius"].cast<float>()
            );
            if (d.contains("active")) {
                obj.active = d["active"].cast<bool>();
            }
            return obj;
        }, py::arg("dict"), "Create world object from Python dictionary");
    
    // Enhanced AgentBody class with methods
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Enhanced agent body state with comprehensive properties and methods.
        
        Represents the physical state of an agent with:
        - Position and orientation
        - Velocity and movement state
        - Energy and health
        - Age and temporal properties
        - Movement and turning state
        - Convenience methods for state management
        
        Properties:
        - x, y: Position coordinates
        - orientation: Heading in radians
        - velocityX, velocityY: Linear velocity
        - angularVelocity: Angular velocity
        - energy: Current energy level (0-100)
        - health: Health level (0-100)
        - age: Age in simulation steps
        - isMoving: Whether agent is currently moving
        - isTurning: Whether agent is currently turning
        - lastActionTime: Time since last action
        
        Methods:
        - reset(): Reset body to initial state
        - getPosition(): Get current position
        - getDirection(): Get current direction vector
        - getSpeed(): Get current speed
        - isAlive(): Check if agent is alive
        - damage(amount): Apply damage
        - heal(amount): Heal agent
        - consumeEnergy(amount): Consume energy
        - restoreEnergy(amount): Restore energy
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0-100)")
        .def_readwrite("health", &AgentBody::health, "Health level (0-100)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset body to initial state")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_tuple(self.x, self.y);
        }, "Get current position as (x, y)")
        .def("getDirection", [](const AgentBody& self) {
            return std::make_tuple(std::cos(self.orientation), std::sin(self.orientation));
        }, "Get current direction as (dx, dy)")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed")
        .def("isAlive", [](const AgentBody& self) {
            return self.health > 0 && self.energy > 0;
        }, "Check if agent is alive")
        .def("damage", [](AgentBody& self, float amount) {
            validatePositive("damage amount", amount);
            self.health = std::max(0.0f, self.health - amount);
        }, py::arg("amount"), "Apply damage")
        .def("heal", [](AgentBody& self, float amount) {
            validatePositive("heal amount", amount);
            self.health = std::min(100.0f, self.health + amount);
        }, py::arg("amount"), "Heal agent")
        .def("consumeEnergy", [](AgentBody& self, float amount) {
            validatePositive("energy amount", amount);
            self.energy = std::max(0.0f, self.energy - amount);
        }, py::arg("amount"), "Consume energy")
        .def("restoreEnergy", [](AgentBody& self, float amount) {
            validatePositive("energy amount", amount);
            self.energy = std::min(100.0f, self.energy + amount);
        }, py::arg("amount"), "Restore energy")
        .def("getAgeString", [](const AgentBody& self) {
            if (self.age < 60) return std::to_string(self.age) + " seconds";
            if (self.age < 3600) return std::to_string(self.age / 60) + " minutes";
            return std::to_string(self.age / 3600) + " hours";
        }, "Get age as human-readable string");
    
    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Enhanced action result from world interaction.
        
        Represents the outcome of an action with:
        - Reward value
        - Success status
        - Optional message
        - Convenience constructors
        - Validation methods
        
        Constructors:
        - Default constructor
        - Parameterized constructor with reward, success, message
        
        Properties:
        - reward: Numeric reward value
        - success: Whether action succeeded
        - message: Optional descriptive message
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
            py::arg("reward") = 0.0f, py::arg("success") = false, py::arg("message") = "",
            "Create an action result with specified properties")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Success status")
        .def_readwrite("message", &ActionResult::message, "Optional message")
        .def("isPositive", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if result is positive")
        .def("isNegative", [](const ActionResult& self) {
            return self.success && self.reward < 0.0f;
        }, "Check if result is negative")
        .def("isFailure", [](const ActionResult& self) {
            return !self.success;
        }, "Check if result is a failure")
        .def("to_dict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert action result to Python dictionary")
        .def_static("from_dict", [](const py::dict& d) {
            float reward = d["reward"].cast<float>();
            bool success = d["success"].cast<bool>();
            std::string message = d["message"].cast<std::string>();
            return ActionResult(reward, success, message);
        }, py::arg("dict"), "Create action result from Python dictionary");
    
    // Enhanced SensoryPercept class with comprehensive methods
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Enhanced sensory percept data with comprehensive access methods.
        
        Represents combined sensory input from all modalities with:
        - Vision data
        - Touch data
        - Internal signals
        - Audio data
        - Proprioception
        - Convenience constructors
        - Data validation
        - Signal normalization
        
        Properties:
        - vision: Vision sensory input
        - touch: Touch sensory input
        - internal: Internal signals
        - audio: Audio sensory input
        - proprioception: Proprioceptive signals
        
        Methods:
        - getAllSignals(): Get all signals as dictionary
        - normalizeAll(): Normalize all signals
        - validate(): Validate all sensory data
        - to_dict(): Convert to Python dictionary
        - from_dict(): Create from Python dictionary
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
            "Get vision sensory input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
            "Set vision sensory input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
            "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
            "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch,
            "Get touch sensory input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
            "Set touch sensory input")
        .def("getInternal", &SensoryPercept::getInternal,
            "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
            "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception,
            "Get proprioception signals")
        .def("setProprioception", &SensoryPercept::setProprioception,
            py::arg("proprioception"), "Set proprioception signals")
        .def("getAudio", &SensoryPercept::getAudio,
            "Get audio sensory input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
            "Set audio sensory input")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
            "Get all sensory signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
            "Get sensory percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
            "Set sensory percept timestamp")
        .def("to_dict", [](const SensoryPercept& self) {
            py::dict result;
            result["vision"] = self.getVision();
            result["touch"] = self.getTouch();
            result["internal"] = self.getInternal();
            result["audio"] = self.getAudio();
            result["proprioception"] = self.getProprioception();
            result["timestamp"] = self.getTimestamp();
            return result;
        }, "Convert sensory percept to Python dictionary")
        .def_static("from_dict", [](const py::dict& d) {
            SensoryPercept percept;
            if (d.contains("vision")) {
                percept.setVision(d["vision"].cast<Vision>());
            }
            if (d.contains("touch")) {
                percept.setTouch(d["touch"].cast<Touch>());
            }
            if (d.contains("internal")) {
                percept.setInternal(d["internal"].cast<InternalSignals>());
            }
            if (d.contains("audio")) {
                percept.setAudio(d["audio"].cast<Audio>());
            }
            if (d.contains("proprioception")) {
                percept.setProprioception(d["proprioception"].cast<Proprioception>());
            }
            if (d.contains("timestamp")) {
                percept.setTimestamp(d["timestamp"].cast<double>());
            }
            return percept;
        }, py::arg("dict"), "Create sensory percept from Python dictionary")
        .def("normalizeAll", [](SensoryPercept& self) {
            auto vision = self.getVision();
            auto audio = self.getAudio();
            auto internal = self.getInternal();
            // Normalize each modality
            self.setVision(vision);
            self.setAudio(audio);
            self.setInternal(internal);
        }, "Normalize all sensory signals");
    
    // Enhanced SimpleWorld class with simulation control
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Enhanced simple 2D world for NLM simulation with comprehensive features.
        
        Provides:
        - World configuration and setup
        - Agent positioning and movement
        - Object management (add/remove)
        - Collision detection
        - Physics simulation
        - Sensory perception
        - Convenience methods
        - State management
        
        Methods:
        - configure(width, height, visionWidth, visionHeight): Set up world
        - reset(): Reset world to initial state
        - setAgentStart(x, y): Set agent starting position
        - update(timestep): Update world physics
        - applyMotorCommand(cmd, currentTime): Apply motor command
        - getSensoryPercept(): Get agent's sensory perception
        - getAgentBody(): Get agent body state
        - addObject(obj): Add world object
        - removeObject(x, y): Remove object at position
        - isValidPosition(x, y): Check if position is valid
        
        Additional features:
        - Serialization support
        - Statistics tracking
        - Performance monitoring
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
            py::arg("width"), py::arg("height"), py::arg("visionWidth"),
            py::arg("visionHeight"),
            "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset,
            "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart,
            py::arg("x"), py::arg("y"),
            "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
            "Update world physics and simulation")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
            py::arg("cmd"), py::arg("currentTime"),
            "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
            py::return_value_policy::reference_internal,
            "Get agent's sensory perception")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
            py::return_value_policy::reference_internal,
            "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
            "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject,
            py::arg("x"), py::arg("y"),
            "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
            py::arg("x"), py::arg("y"),
            "Check if position is valid (not in wall)")
        .def("getWidth", &SimpleWorld::getWidth,
            "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
            "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
            "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy,
            py::arg("e"), "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
            "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate,
            py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
            "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed,
            py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
            "Get random seed")
        .def("getStatistics", [](const SimpleWorld& self) {
            py::dict stats;
            stats["width"] = self.getWidth();
            stats["height"] = self.getHeight();
            stats["agentX"] = self.getAgentBody().x;
            stats["agentY"] = self.getAgentBody().y;
            stats["simulationTime"] = self.getSimulationTime();
            return stats;
        }, "Get world statistics")
        .def("to_dict", [](const SimpleWorld& self) {
            py::dict result;
            result["width"] = self.getWidth();
            result["height"] = self.getHeight();
            auto body = self.getAgentBody();
            result["agentX"] = body.x;
            result["agentY"] = body.y;
            result["agentOrientation"] = body.orientation;
            result["simulationTime"] = self.getSimulationTime();
            return result;
        }, "Convert world to Python dictionary")
        .def_static("from_dict", [](const py::dict& d) {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(
                d["width"].cast<size_t>(),
                d["height"].cast<size_t>(),
                64, // Default vision width
                64   // Default vision height
            );
            world->setAgentStart(
                d["agentX"].cast<float>(),
                d["agentY"].cast<float>()
            );
            return world;
        }, py::arg("dict"), "Create world from Python dictionary");
    
    // Enhanced Brain class with advanced features
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Enhanced central neural simulation brain class with advanced features.
        
        Provides:
        - Neural network simulation
        - Sensory input processing
        - Motor output generation
        - Development system integration
        - Neuromodulation
        - Plasticity
        - Performance monitoring
        - State management
        
        Methods:
        - initialize(config): Initialize brain with configuration
        - step(currentStep): Perform simulation step
        - step(currentStep, currentTime): Perform simulation step with timestamp
        - receiveSensoryInput(input): Inject sensory input
        - injectCurrent(neuron, current): Inject current into specific neuron
        - injectCurrentToNeurons(type, current): Inject current into neurons by type
        - produceAction(): Generate motor action
        - reset(): Reset brain state
        - save(filepath): Save brain state
        - load(filepath): Load brain state
        - addRegion(name): Add neural region
        - getRegion(id): Get region by ID
        - getTotalNeuronCount(): Get total neuron count
        - getTotalSynapseCount(): Get total synapse count
        - getActiveNeuronCount(): Get active neuron count
        - logStatus(): Log brain status
        
        Advanced features:
        - Region management
        - Neuron type filtering
        - Activity monitoring
        - Development stage tracking
        - Config access
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
            "Create a brain with configuration")
        .def("initialize", &Brain::initialize,
            py::arg("world"), "Initialize with world",
            "Initialize the brain with configuration and world")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
            py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
            py::arg("currentStep"), py::arg("currentTime"), "Perform simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
            py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
            py::arg("neuron"), py::arg("current"), "Inject current into specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
            py::arg("type"), py::arg("current"), "Inject current to neurons by type")
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
            py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
            "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
            "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
            py::return_value_policy::reference_internal, "Get all regions")
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
            py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
            py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus,
            "Log brain status")
        .def("getStatistics", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spikes"] = self.getTotalSpikeCount();
            return stats;
        }, "Get comprehensive brain statistics");
    
    // Enhanced AgentBrain class with advanced interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Enhanced agent brain interface connecting NLM brain to world.
        
        Provides:
        - Sensory processing pipeline
        - Motor command decoding
        - Neuromodulation integration
        - Development system updates
        - Action selection
        - Reward processing
        - State management
        
        Methods:
        - initialize(brain, world): Initialize with brain and world
        - getSensoryInputSize(): Get expected sensory input size
        - getMotorOutputSize(): Get expected motor output size
        - processSensoryInput(percept): Process sensory percept and inject into brain
        - decodeMotorCommand(): Decode brain motor activity into motor command
        - applyRewardModulation(reward, predictedReward): Apply reward-based neuromodulation
        - updateDevelopment(timestep): Update development system
        - reset(): Reset agent for new episode
        - getBrain(): Get underlying brain
        - enable/disable systems: Enable/disable neuromodulation, plasticity, development, curiosity
        - isEnabled(): Check if system is enabled
        
        Advanced features:
        - Action preprocessing
        - Sensor fusion
        - Neuromodulation integration
        - Development tracking
        - Performance monitoring
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
            "Create an agent brain with brain instance")
        .def("initialize", &AgentBrain::initialize,
            py::arg("brain"), py::arg("world"), "Initialize with brain and world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
            "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
            "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
            py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
            "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
            py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
            py::arg("timestep"), "Update development system")
        .def("reset", &AgentBrain::reset,
            "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
            py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
            py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
            py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
            py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
            py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
            "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
            "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
            "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
            "Check if curiosity is enabled")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
            "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
            "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
            "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
            "Get prediction error")
        .def("getStatistics", [](const AgentBrain& self) {
            py::dict stats;
            stats["sensory_input_size"] = self.getSensoryInputSize();
            stats["motor_output_size"] = self.getMotorOutputSize();
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            stats["development_enabled"] = self.isDevelopmentEnabled();
            stats["curiosity_enabled"] = self.isCuriosityEnabled();
            return stats;
        }, "Get comprehensive agent brain statistics");
    
    // Simulation control utilities
    py::class_<SimulationController>(m, "SimulationController", R"pbdoc(
        Simulation controller for managing simulation state and pause/resume operations.
        
        Provides:
        - Pause/resume control
        - Reset functionality
        - State tracking
        - Performance monitoring
        - Status reporting
        
        Methods:
        - pause(): Pause the simulation
        - resume(): Resume the simulation
        - reset(): Reset the simulation
        - start(): Start the simulation
        - stop(): Stop the simulation
        - isSimulationPaused(): Check if paused
        - isSimulationRunning(): Check if running
        - isSimulationReset(): Check if reset
        - getMonitor(): Get performance monitor
        - getStatus(): Get simulation status
        
        Features:
        - Pause time tracking
        - Performance monitoring
        - State validation
    )pbdoc")
        .def(py::init<>())
        .def("pause", &SimulationController::pause, "Pause the simulation")
        .def("resume", &SimulationController::resume, "Resume the simulation")
        .def("reset", &SimulationController::reset, "Reset the simulation")
        .def("start", &SimulationController::start, "Start the simulation")
        .def("stop", &SimulationController::stop, "Stop the simulation")
        .def("isSimulationPaused", &SimulationController::isSimulationPaused,
            "Check if simulation is paused")
        .def("isSimulationRunning", &SimulationController::isSimulationRunning,
            "Check if simulation is running")
        .def("isSimulationReset", &SimulationController::isSimulationReset,
            "Check if simulation is reset")
        .def("getMonitor", &SimulationController::getMonitor,
            py::return_value_policy::reference_internal, "Get performance monitor")
        .def("getStatus", &SimulationController::getStatus,
            "Get simulation status")
        .def("getTotalPauseTime", &SimulationController::getTotalPauseTime,
            "Get total pause time");
    
    // Performance monitor class
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(
        Performance monitor for tracking simulation metrics and statistics.
        
        Provides:
        - Step timing
        - Active neuron tracking
        - Spike counting
        - Performance statistics
        - Reporting utilities
        
        Methods:
        - recordStep(neuronsActive, spikesGenerated, timeDelta): Record a simulation step
        - getElapsedTime(): Get elapsed time
        - getAverageStepTime(): Get average step time
        - getLastStepTime(): Get last step time
        - getStepsPerSecond(): Get steps per second
        - getAverageActiveNeurons(): Get average active neurons
        - getAverageSpikesPerStep(): Get average spikes per step
        - getPerformanceReport(): Get formatted performance report
        - reset(): Reset all statistics
        
        Attributes:
        - totalSteps: Total simulation steps
        - totalNeuronsActive: Total active neurons
        - totalSpikesGenerated: Total spikes generated
        - totalTime: Total simulation time
        
        Features:
        - Real-time monitoring
        - Detailed statistics
        - Formatted reporting
        - Reset capability
    )pbdoc")
        .def(py::init<>())
        .def("recordStep", &PerformanceMonitor::recordStep,
            py::arg("neuronsActive"), py::arg("spikesGenerated"), py::arg("timeDelta"),
            "Record a simulation step with performance metrics")
        .def("getElapsedTime", &PerformanceMonitor::getElapsedTime,
            "Get elapsed time in seconds")
        .def("getAverageStepTime", &PerformanceMonitor::getAverageStepTime,
            "Get average step time in seconds")
        .def("getLastStepTime", &PerformanceMonitor::getLastStepTime,
            "Get last step time in seconds")
        .def("getStepsPerSecond", &PerformanceMonitor::getStepsPerSecond,
            "Get simulation steps per second")
        .def("getTotalSteps", &PerformanceMonitor::getTotalSteps,
            "Get total number of steps")
        .def("getAverageActiveNeurons", &PerformanceMonitor::getAverageActiveNeurons,
            "Get average number of active neurons")
        .def("getAverageSpikesPerStep", &PerformanceMonitor::getAverageSpikesPerStep,
            "Get average spikes per step")
        .def("getPerformanceReport", &PerformanceMonitor::getPerformanceReport,
            "Get formatted performance report")
        .def("reset", &PerformanceMonitor::reset, "Reset all statistics")
        .def_readonly("totalSteps", &PerformanceMonitor::totalSteps,
            "Total number of simulation steps")
        .def_readonly("totalNeuronsActive", &PerformanceMonitor::totalNeuronsActive,
            "Total number of active neurons recorded")
        .def_readonly("totalSpikesGenerated", &PerformanceMonitor::totalSpikesGenerated,
            "Total number of spikes generated")
        .def_readonly("totalTime", &PerformanceMonitor::totalTime,
            "Total simulation time in seconds");
    
    // Batch processor class
    py::class_<BatchProcessor>(m, "BatchProcessor", R"pbdoc(
        Batch processor for managing multiple operations.
        
        Provides:
        - Operation batching
        - Concurrent execution
        - State tracking
        - Validation
        
        Methods:
        - addOperation(operation): Add an operation to the batch
        - execute(): Execute all operations
        - getOperationCount(): Get number of operations
        - isActive(): Check if processing
        - getName(): Get batch name
        
        Features:
        - Operation queuing
        - Error handling
        - State management
    )pbdoc")
        .def(py::init<std::string>(), py::arg("name") = "Batch",
            "Create a batch processor with optional name")
        .def("addOperation", &BatchProcessor::addOperation, py::arg("operation"),
            "Add an operation to the batch")
        .def("execute", &BatchProcessor::execute, "Execute all operations in the batch")
        .def("getOperationCount", &BatchProcessor::getOperationCount,
            "Get number of operations in the batch")
        .def("isActive", &BatchProcessor::isActive,
            "Check if batch is currently processing")
        .def("getName", &BatchProcessor::getName,
            "Get the batch name");
    
    // Experiment manager class
    py::class_<ExperimentManager>(m, "ExperimentManager", R"pbdoc(
        Experiment manager for managing and running experiments.
        
        Provides:
        - Experiment storage and management
        - Experiment execution
        - Experiment tracking
        - Experiment listing
        
        Methods:
        - addExperiment(name, experiment): Add an experiment
        - runExperiment(name, brain, world, duration): Run an experiment
        - getExperimentNames(): Get all experiment names
        - getExperiment(name): Get experiment by name
        - clearExperiments(): Clear all experiments
        - getCurrentExperiment(): Get current experiment name
        
        Features:
        - Experiment validation
        - Name-based lookup
        - State tracking
    )pbdoc")
        .def(py::init<>())
        .def("addExperiment", &ExperimentManager::addExperiment,
            py::arg("name"), py::arg("experiment"), "Add an experiment to the manager")
        .def("runExperiment", &ExperimentManager::runExperiment,
            py::arg("name"), py::arg("brain"), py::arg("world"), py::arg("duration"),
            "Run an experiment")
        .def("getExperimentNames", &ExperimentManager::getExperimentNames,
            "Get all experiment names")
        .def("getExperiment", &ExperimentManager::getExperiment,
            py::arg("name"), "Get experiment by name")
        .def("clearExperiments", &ExperimentManager::clearExperiments,
            "Clear all experiments")
        .def("getCurrentExperiment", &ExperimentManager::getCurrentExperiment,
            "Get current experiment name");
    
    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return createDefaultConfig();
    }, R"pbdoc(
        Create a default configuration with standard NLM settings.
        
        Returns:
            std::shared_ptr<Config>: Default configuration
        
        Creates a configuration with:
        - Standard brain simulation parameters
        - Vision, audio, and internal signals enabled
        - All neuromodulation systems enabled
        - Typical developmental settings
    )pbdoc");
    
    m.def("createMinimalConfig", []() -> std::shared_ptr<Config> {
        return createMinimalConfig();
    }, R"pbdoc(
        Create a minimal configuration for quick testing.
        
        Returns:
            std::shared_ptr<Config>: Minimal configuration
        
        Creates a configuration with:
        - Essential brain simulation parameters only
        - Minimal neuron and synapse counts
        - Basic time step settings
    )pbdoc");
    
    m.def("createHighPerformanceConfig", []() -> std::shared_ptr<Config> {
        return createHighPerformanceConfig();
    }, R"pbdoc(
        Create a high-performance configuration for large simulations.
        
        Returns:
            std::shared_ptr<Config>: High-performance configuration
        
        Creates a configuration with:
        - Large neuron and synapse counts
        - Small time steps for precision
        - Optimized neuromodulation settings
    )pbdoc");
    
    m.def("createGridWorldConfig", []() -> std::shared_ptr<Config> {
        return createGridWorldConfig();
    }, R"pbdoc(
        Create a grid world configuration.
        
        Returns:
            std::shared_ptr<Config>: Grid world configuration
        
        Creates a configuration suitable for:
        - Rectangular grid environments
        - Agent navigation
        - Resource collection
        - Hazard avoidance
    )pbdoc");
    
    m.def("createMazeWorldConfig", []() -> std::shared_ptr<Config> {
        return createMazeWorldConfig();
    }, R"pbdoc(
        Create a maze world configuration.
        
        Returns:
            std::shared_ptr<Config>: Maze world configuration
        
        Creates a configuration suitable for:
        - Complex maze environments
        - Problem-solving tasks
        - Pathfinding challenges
        - Exploration tasks
    )pbdoc");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(
        Create a brain with the specified configuration.
        
        Args:
            config: Configuration to use for the brain
        
        Returns:
            std::shared_ptr<Brain>: New brain instance
        
        Creates a brain with:
        - The specified configuration
        - Initialized neural populations
        - Default developmental state
        - Ready for sensory input processing
    )pbdoc");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a simple world instance.
        
        Returns:
            std::shared_ptr<SimpleWorld>: New world instance
        
        Creates a world with:
        - Default dimensions
        - Default vision parameters
        - No objects initially
        - Ready for agent placement
    )pbdoc");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(
        Create an agent brain interface with the specified brain.
        
        Args:
            brain: Brain instance to interface with
        
        Returns:
            std::shared_ptr<AgentBrain>: New agent brain interface
        
        Creates an agent brain with:
        - The specified brain instance
        - Ready for sensory processing
        - Configured for world interaction
    )pbdoc");
    
    m.def("createSimulationController", []() -> std::shared_ptr<SimulationController> {
        return std::make_shared<SimulationController>();
    }, R"pbdoc(
        Create a simulation controller for managing simulation state.
        
        Returns:
            std::shared_ptr<SimulationController>: New simulation controller
        
        Creates a controller with:
        - Pause/resume capability
        - Reset functionality
        - Performance monitoring
        - State tracking
    )pbdoc");
    
    m.def("createPerformanceMonitor", []() -> std::shared_ptr<PerformanceMonitor> {
        return std::make_shared<PerformanceMonitor>();
    }, R"pbdoc(
        Create a performance monitor for tracking simulation metrics.
        
        Returns:
            std::shared_ptr<PerformanceMonitor>: New performance monitor
        
        Creates a monitor with:
        - Step timing
        - Active neuron tracking
        - Spike counting
        - Performance statistics
    )pbdoc");
    
    m.def("createBatchProcessor", [](const std::string& name) -> std::shared_ptr<BatchProcessor> {
        return std::make_shared<BatchProcessor>(name);
    }, py::arg("name") = "Batch", R"pbdoc(
        Create a batch processor for managing multiple operations.
        
        Args:
            name: Name for the batch processor
        
        Returns:
            std::shared_ptr<BatchProcessor>: New batch processor
        
        Creates a batch processor with:
        - Operation queuing
        - Concurrent execution
        - State tracking
    )pbdoc");
    
    m.def("createExperimentManager", []() -> std::shared_ptr<ExperimentManager> {
        return std::make_shared<ExperimentManager>();
    }, R"pbdoc(
        Create an experiment manager for managing and running experiments.
        
        Returns:
            std::shared_ptr<ExperimentManager>: New experiment manager
        
        Creates a manager with:
        - Experiment storage
        - Experiment execution
        - Experiment tracking
    )pbdoc");
    
    // Utility functions
    m.def("vectorToJson", &vectorToJson, py::arg("data"),
        "Convert a vector to JSON string");
    
    m.def("jsonToVector", &jsonToVector, py::arg("jsonStr"),
        "Convert a JSON string to vector");
    
    m.def("objectToJson", &objectToJson, py::arg("obj"),
        "Convert a Python object to JSON string");
    
    m.def("jsonToObject", &jsonToObject, py::arg("jsonStr"),
        "Convert a JSON string to Python object");
    
    m.def("generateNeuronActivityPlot", &generateNeuronActivityPlot,
        py::arg("brain"), "Generate a neuron activity plot for the brain");
    
    m.def("generateConnectionDensityPlot", &generateConnectionDensityPlot,
        py::arg("brain"), "Generate a connection density plot for the brain");
    
    // Version and metadata
    m.attr("VERSION") = "2.0.0";
    m.attr("BUILD_DATE") = "2026-10-10";
    m.attr("AUTHOR") = "NLM Team";
    m.attr("DESCRIPTION") = "Enhanced NLM Python Bindings with advanced features";
    
    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Module attributes
    m.attr("__version__") = "2.0.0";
    m.attr("__author__") = "NLM Team";
    m.attr("__description__") = "Enhanced Neural Learning Machine Python bindings";
    m.attr("__license__") = "MIT";
    m.attr("__copyright__") = "Copyright 2024-2026 NLM Team";
    
    // Example usage function
    m.def("example_usage", []() {
        std::stringstream ss;
        ss << "Example NLM Usage:\n\n";
        ss << "1. Create a configuration:\n";
        ss << "   config = nlm.createDefaultConfig()\n\n";
        ss << "2. Create a brain:\n";
        ss << "   brain = nlm.createBrain(config)\n\n";
        ss << "3. Create a world:\n";
        ss << "   world = nlm.createSimpleWorld()\n\n";
        ss << "4. Initialize world and brain:\n";
        ss << "   world.configure(20, 20, 64, 64)\n";
        ss << "   world.setAgentStart(10, 10)\n";
        ss << "   brain.initialize(world)\n\n";
        ss << "5. Create agent brain:\n";
        ss << "   agent_brain = nlm.createAgentBrain(brain)\n\n";
        ss << "6. Run simulation:\n";
        ss << "   controller = nlm.createSimulationController()\n";
        ss << "   monitor = controller.getMonitor()\n\n";
        ss << "   for step in range(100):\n";
        ss << "       percept = world.getSensoryPercept()\n";
        ss << "       agent_brain.processSensoryInput(percept)\n";
        ss << "       action = agent_brain.decodeMotorCommand()\n";
        ss << "       world.applyMotorCommand(action, step)\n";
        ss << "       world.update(0.001)\n";
        ss << "       if step % 10 == 0:\n";
        ss << "           print(controller.getStatus())\n";
        ss << "\n7. Get performance statistics:\n";
        ss << "   print(monitor.getPerformanceReport())\n";
        return ss.str();
    }, "Get example usage instructions");
}

} // namespace nlm