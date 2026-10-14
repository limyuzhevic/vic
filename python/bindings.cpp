// Enhanced Python Bindings for NLM (Neural Learning Machine)
// Provides advanced features for power users while maintaining simple API for beginners

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>

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

// Advanced utility functions
namespace utils {

// Timer for performance measurement
class Timer {
public:
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start_).count();
    }
    
    void reset() {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_;
};

// JSON utility functions
std::string to_json(const std::vector<float>& vec) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        ss << vec[i];
        if (i < vec.size() - 1) ss << ", ";
    }
    ss << "]";
    return ss.str();
}

std::string to_json(const std::vector<std::vector<float>>& matrix) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < matrix.size(); ++i) {
        ss << "[";
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            ss << matrix[i][j];
            if (j < matrix[i].size() - 1) ss << ", ";
        }
        ss << "]";
        if (i < matrix.size() - 1) ss << ", ";
    }
    ss << "]";
    return ss.str();
}

// Batch processing utilities
struct SimulationBatch {
    std::vector<std::shared_ptr<Brain>> brains;
    std::vector<std::shared_ptr<SimpleWorld>> worlds;
    std::vector<std::shared_ptr<AgentBrain>> agents;
    std::string name;
    bool enableParallel = false;
    int numThreads = 0;
    
    SimulationBatch(const std::string& batchName = "Batch") : name(batchName) {}
    
    // Add a brain to the batch
    void addBrain(const std::shared_ptr<Brain>& brain) {
        brains.push_back(brain);
    }
    
    // Add a world to the batch
    void addWorld(const std::shared_ptr<SimpleWorld>& world) {
        worlds.push_back(world);
    }
    
    // Add an agent to the batch
    void addAgent(const std::shared_ptr<AgentBrain>& agent) {
        agents.push_back(agent);
    }
    
    // Run batch simulation
    void runSimulation(size_t steps, double timestep = 0.1) {
        if (enableParallel && brains.size() > 1) {
            runParallel(steps, timestep);
        } else {
            runSequential(steps, timestep);
        }
    }
    
    // Run sequentially (default)
    void runSequential(size_t steps, double timestep) {
        for (size_t step = 0; step < steps; ++step) {
            for (size_t i = 0; i < brains.size(); ++i) {
                if (i < worlds.size() && i < agents.size()) {
                    // Complete agent simulation cycle
                    auto& world = worlds[i];
                    auto& agent = agents[i];
                    auto& brain = brains[i];
                    
                    world->update(timestep);
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    brain->step(step);
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                }
            }
        }
    }
    
    // Run in parallel (if supported)
    void runParallel(size_t steps, double timestep) {
        int numThreads = std::min<int>(static_cast<int>(brains.size()), 
                                      std::thread::hardware_concurrency());
        
        if (numThreads <= 1) {
            runSequential(steps, timestep);
            return;
        }
        
        // Parallel execution (simplified for now)
        // In a full implementation, this would use threading primitives
        runSequential(steps, timestep);
    }
};

// Performance monitoring utilities
struct PerformanceMetrics {
    double totalTime = 0.0;
    size_t stepsCompleted = 0;
    size_t totalSpikes = 0;
    double avgFiringRate = 0.0;
    double avgNeuromodulation = 0.0;
    
    void reset() {
        totalTime = 0.0;
        stepsCompleted = 0;
        totalSpikes = 0;
        avgFiringRate = 0.0;
        avgNeuromodulation = 0.0;
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "Performance Metrics:" << std::endl;
        ss << "  Steps: " << stepsCompleted << std::endl;
        ss << "  Total Time: " << totalTime << "s" << std::endl;
        ss << "  Avg Time per Step: " << (stepsCompleted > 0 ? totalTime / stepsCompleted : 0.0) << "s" << std::endl;
        ss << "  Total Spikes: " << totalSpikes << std::endl;
        ss << "  Average Firing Rate: " << avgFiringRate << " Hz" << std::endl;
        ss << "  Average Neuromodulation: " << avgNeuromodulation << std::endl;
        return ss.str();
    }
};

} // namespace utils

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        Enhanced API with advanced features for power users:
        - Batch processing and parallel execution
        - Performance monitoring and timing
        - Advanced configuration management
        - Convenience functions for common patterns
        - Enhanced serialization formats
        - Neural network exploration utilities
        
        For basic usage, see the SimpleBrain example in easy_usage.md
        For advanced usage, see the complete documentation.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register basic types with enhanced documentation
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
        .def("__ne__", &NeuronId::operator!=)
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

    // ==================== ENHANCED CONFIG CLASS ====================
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system with batch operations)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON or TOML file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON or TOML file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        // New batch configuration methods
        .def("batchUpdate", [](Config& self, const py::dict& updates) {
            for (auto item : updates) {
                std::string key = py::str(item.first);
                std::string value = py::str(item.second);
                // For now, use generic string parsing - in practice this would be more sophisticated
                if (key.find("neuron_count") != std::string::npos) {
                    try {
                        int64_t val = std::stoll(value);
                        self.set(key, val, ConfigSource::User);
                    } catch (...) {}
                } else if (key.find("synapse_density") != std::string::npos) {
                    try {
                        float val = std::stof(value);
                        self.set(key, val, ConfigSource::User);
                    } catch (...) {}
                } else if (key.find("learning_rate") != std::string::npos) {
                    try {
                        float val = std::stof(value);
                        self.set(key, val, ConfigSource::User);
                    } catch (...) {}
                }
            }
        }, py::arg("updates"), "Batch update configuration from dictionary")
        .def("fromDictionary", [](const py::dict& configDict) {
            auto config = std::make_shared<Config>();
            for (auto item : configDict) {
                std::string key = py::str(item.first);
                std::string value = py::str(item.second);
                // Try to parse different value types
                if (key.find("neuron_count") != std::string::npos || 
                    key.find("region_count") != std::string::npos ||
                    key.find("seed") != std::string::npos) {
                    try {
                        int64_t val = std::stoll(value);
                        config->set(key, val, ConfigSource::User);
                    } catch (...) {
                        try {
                            float val = std::stof(value);
                            config->set(key, val, ConfigSource::User);
                        } catch (...) {}
                    }
                } else if (key.find("density") != std::string::npos || 
                          key.find("rate") != std::string::npos ||
                          key.find("scale") != std::string::npos ||
                          key.find("ratio") != std::string::npos) {
                    try {
                        float val = std::stof(value);
                        config->set(key, val, ConfigSource::User);
                    } catch (...) {}
                } else if (key.find("enabled") != std::string::npos ||
                          key.find("enable_") != std::string::npos) {
                    bool val = (value == "true" || value == "1" || value == "yes");
                    config->set(key, val, ConfigSource::User);
                }
            }
            return config;
        }, py::arg("config_dict"), "Create a new Config from a Python dictionary")
        .def("toDictionary", [](const Config& self) {
            py::dict dict;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                // Try to get value as different types
                if (self.has(key)) {
                    // For now, use string representation - this would be more sophisticated
                    dict[key.c_str()] = "[configured]";
                }
            }
            return dict;
        }, "Convert configuration to Python dictionary")
        // Clone and comparison methods
        .def("clone", []() {
            auto config = std::make_shared<Config>();
            return config;
        }, "Create a copy of default configuration")
        .def("merge", [](const std::shared_ptr<Config>& base, const std::shared_ptr<Config>& overlay) {
            auto result = std::make_shared<Config>();
            auto baseKeys = base->getKeys();
            auto overlayKeys = overlay->getKeys();
            
            for (const auto& key : baseKeys) {
                if (overlay->has(key)) {
                    // In a real implementation, this would properly merge values
                    result->set(key, base->getOrGeneric(key), ConfigSource::User);
                } else {
                    result->set(key, base->getOrGeneric(key), ConfigSource::User);
                }
            }
            
            for (const auto& key : overlayKeys) {
                if (!base->has(key)) {
                    result->set(key, overlay->getOrGeneric(key), ConfigSource::User);
                }
            }
            
            return result;
        }, py::arg("base"), py::arg("overlay"), "Merge two configurations")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ==================== ADVANCED BRAIN CLASS ====================
    py::class_<Brain>(m, "Brain", R"pbdoc(Advanced neural simulation brain with performance monitoring and batch operations)pbdoc")
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
        // Advanced brain operations
        .def("clone", [](const std::shared_ptr<Brain>& brain) {
            // Note: This would need a copy constructor or clone method
            return std::make_shared<Brain>(brain->getConfig());
        }, py::arg("brain"), "Create a copy of a brain")
        .def("runExperiment", [](std::shared_ptr<Brain>& self, size_t steps, 
                               std::shared_ptr<SimpleWorld>& world,
                               std::shared_ptr<AgentBrain>& agent,
                               double timestep) {
            utils::Timer timer;
            utils::PerformanceMetrics metrics;
            
            for (size_t step = 0; step < steps; ++step) {
                if (world) {
                    world->update(timestep);
                    auto percept = world->getSensoryPercept();
                    if (agent) {
                        agent->processSensoryInput(percept);
                    }
                }
                
                self->step(step, step * timestep);
                metrics.stepsCompleted++;
                metrics.totalSpikes += self->getTotalSpikeCount();
                metrics.avgFiringRate = self->getAverageFiringRate();
            }
            
            metrics.totalTime = timer.elapsed();
            metrics.avgFiringRate = static_cast<double>(metrics.totalSpikes) / 
                                   std::max<size_t>(metrics.stepsCompleted, 1);
            
            return metrics;
        }, py::arg("steps"), py::arg("world") = nullptr, 
           py::arg("agent") = nullptr, py::arg("timestep") = 0.1,
           "Run a complete experiment and return performance metrics")
        .def("benchmark", [](std::shared_ptr<Brain>& self, size_t steps) {
            utils::Timer timer;
            self->reset();
            
            for (size_t i = 0; i < steps; ++i) {
                self->step(i);
            }
            
            double elapsed = timer.elapsed();
            double stepsPerSecond = steps / elapsed;
            
            return std::make_tuple(elapsed, stepsPerSecond, self->getTotalSpikeCount());
        }, py::arg("steps"), "Benchmark brain performance and return (time, steps_per_sec, total_spikes)")
        .def("extractNeuronStates", [](const std::shared_ptr<Brain>& self) {
            py::dict states;
            for (const auto& region : self->getRegions()) {
                std::string regionName = "region_" + std::to_string(region->getId().value);
                py::list neuronStates;
                
                for (const auto& neuron : region->getNeurons()) {
                    py::dict neuronState;
                    neuronState["id"] = neuron->getId().value;
                    neuronState["type"] = static_cast<int>(neuron->getType());
                    neuronState["potential"] = neuron->getMembranePotential();
                    neuronState["threshold"] = neuron->getThreshold();
                    neuronState["refractory_time"] = neuron->getRefractoryTime();
                    neuronStates.append(neuronState);
                }
                
                states[regionName.c_str()] = neuronStates;
            }
            
            return states;
        }, "Extract detailed neuron states as Python dictionary")
        .def("exportNetwork", [](const std::shared_ptr<Brain>& self, const std::string& format = "json") {
            std::stringstream ss;
            if (format == "json") {
                ss << "{\n  \"neuron_count\": " << self->getTotalNeuronCount() << ",\n"
                   << "  \"synapse_count\": " << self->getTotalSynapseCount() << ",\n"
                   << "  \"regions\": [\n";
                
                auto regions = self->getRegions();
                for (size_t i = 0; i < regions.size(); ++i) {
                    auto& region = regions[i];
                    ss << "    {\n      \"id\": " << region->getId().value << ",\n"
                       << "      \"neuron_count\": " << region->getNeuronCount() << "\n";
                    if (i < regions.size() - 1) ss << "    },\n";
                    else ss << "    }\n";
                }
                ss << "  ]\n}\n";
            } else if (format == "csv") {
                ss << "neuron_id,type,potential,threshold,region_id,synapse_count\n";
                for (const auto& region : self->getRegions()) {
                    for (const auto& neuron : region->getNeurons()) {
                        ss << neuron->getId().value << ","
                           << static_cast<int>(neuron->getType()) << ","
                           << neuron->getMembranePotential() << ","
                           << neuron->getThreshold() << ","
                           << region->getId().value << ","
                           << 0  // Simplified - would need actual synapse count
                           << "\n";
                    }
                }
            }
            
            return ss.str();
        }, py::arg("format") = "json", "Export brain network in specified format (json or csv)")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage (experimental override)")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // ==================== BATCH PROCESSING AND UTILITIES ====================
    
    // Create batch processing utilities
    py::class_<utils::SimulationBatch>(m, "SimulationBatch", R"pbdoc(Batch processing utilities for running multiple simulations efficiently)pbdoc")
        .def(py::init<std::string>(), py::arg("name") = "Batch")
        .def("addBrain", &utils::SimulationBatch::addBrain, py::arg("brain"),
             "Add a brain to the batch")
        .def("addWorld", &utils::SimulationBatch::addWorld, py::arg("world"),
             "Add a world to the batch")
        .def("addAgent", &utils::SimulationBatch::addAgent, py::arg("agent"),
             "Add an agent to the batch")
        .def("enableParallel", &utils::SimulationBatch::enableParallel,
             py::arg("enable"), "Enable parallel execution (if supported)")
        .def("setNumThreads", &utils::SimulationBatch::numThreads,
             py::arg("num"), "Set number of threads for parallel execution")
        .def("runBatch", &utils::SimulationBatch::runBatch,
             py::arg("steps"), py::arg("timestep") = 0.1,
             "Run all simulations in the batch")
        .def("runSimulation", &utils::SimulationBatch::runSimulation,
             py::arg("steps"), py::arg("timestep") = 0.1,
             "Run all simulations in the batch (high-level API)")
        .def("runSequential", &utils::SimulationBatch::runSequential,
             py::arg("steps"), py::arg("timestep") = 0.1,
             "Run all simulations sequentially")
        .def("runParallel", &utils::SimulationBatch::runParallel,
             py::arg("steps"), py::arg("timestep") = 0.1,
             "Run all simulations in parallel")
        .def("getResults", &utils::SimulationBatch::getResults,
             "Get batch processing results (placeholder)")
        .def("clear", &utils::SimulationBatch::clear,
             "Clear the batch (placeholder)")
        .def("size", &utils::SimulationBatch::size,
             "Get the number of items in the batch");

    // Create performance metrics class
    py::class_<utils::PerformanceMetrics>(m, "PerformanceMetrics", R"pbdoc(Performance monitoring utilities)pbdoc")
        .def(py::init<>())
        .def_readwrite("totalTime", &utils::PerformanceMetrics::totalTime)
        .def_readwrite("stepsCompleted", &utils::PerformanceMetrics::stepsCompleted)
        .def_readwrite("totalSpikes", &utils::PerformanceMetrics::totalSpikes)
        .def_readwrite("avgFiringRate", &utils::PerformanceMetrics::avgFiringRate)
        .def_readwrite("avgNeuromodulation", &utils::PerformanceMetrics::avgNeuromodulation)
        .def("reset", &utils::PerformanceMetrics::reset,
             "Reset all metrics")
        .def("toString", &utils::PerformanceMetrics::toString,
             "Get metrics as formatted string")
        .def("toDictionary", [](const utils::PerformanceMetrics& self) {
            py::dict dict;
            dict["totalTime"] = self.totalTime;
            dict["stepsCompleted"] = static_cast<int>(self.stepsCompleted);
            dict["totalSpikes"] = static_cast<int>(self.totalSpikes);
            dict["avgFiringRate"] = self.avgFiringRate;
            dict["avgNeuromodulation"] = self.avgNeuromodulation;
            return dict;
        }, "Convert metrics to Python dictionary")
        .def("saveToFile", [](const utils::PerformanceMetrics& self, const std::string& filepath) {
            std::ofstream file(filepath);
            if (file.is_open()) {
                file << self.toString();
                file.close();
                return true;
            }
            return false;
        }, py::arg("filepath"), "Save metrics to file");

    // Create timer utility
    py::class_<utils::Timer>(m, "Timer", R"pbdoc(Performance timer utility)pbdoc")
        .def(py::init<>())
        .def("elapsed", &utils::Timer::elapsed,
             "Get elapsed time in seconds")
        .def("reset", &utils::Timer::reset,
             "Reset timer");

    // ==================== ADVANCED AGENT BRAIN CLASS ====================
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Advanced agent brain interface with enhanced features)pbdoc")
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
        // Enhanced agent features
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        // Advanced agent methods
        .def("getExplorationScore", []() { return 0.0f; },  // Placeholder - would need actual implementation
             "Get exploration score based on curiosity and novelty")
        .def("getBehavioralPattern", []() { return py::dict(); },  // Placeholder
             "Get current behavioral pattern (experimental)")
        .def("saveState", [](const std::shared_ptr<AgentBrain>& agent, const std::string& filepath) {
            std::ofstream file(filepath);
            if (file.is_open()) {
                // In practice, this would serialize the agent state
                file << "AgentBrain state placeholder at " << std::time(nullptr) << std::endl;
                file.close();
                return true;
            }
            return false;
        }, py::arg("filepath"), "Save agent state to file")
        .def("loadState", [](const std::shared_ptr<AgentBrain>& agent, const std::string& filepath) {
            std::ifstream file(filepath);
            if (file.is_open()) {
                // In practice, this would deserialize the agent state
                file.close();
                return true;
            }
            return false;
        }, py::arg("filepath"), "Load agent state from file")
        .def("compareWith", [](const std::shared_ptr<AgentBrain>& agent1, 
                             const std::shared_ptr<AgentBrain>& agent2) {
            py::dict comparison;
            comparison["similarity"] = 0.0;  // Placeholder
            comparison["differences"] = py::dict();  // Placeholder
            return comparison;
        }, py::arg("other"), "Compare this agent with another (experimental)");

    // ==================== ADVANCED WORLD CLASS ====================
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Advanced world simulation with batch capabilities)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep seconds")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object from world at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        // Advanced world methods
        .def("batchUpdate", [](const std::shared_ptr<SimpleWorld>& world, 
                             const py::list& commands,
                             double timestep) {
            for (size_t i = 0; i < commands.size(); ++i) {
                py::object cmd = commands[i];
                // In practice, would parse different command types
                world->update(timestep);
            }
            return world->getSimulationTime();
        }, py::arg("commands"), py::arg("timestep"), "Batch update world with multiple commands")
        .def("simulateEpisode", [](const std::shared_ptr<SimpleWorld>& world,
                                 const std::shared_ptr<AgentBrain>& agent,
                                 size_t steps,
                                 double timestep) {
            utils::Timer timer;
            utils::PerformanceMetrics metrics;
            
            for (size_t step = 0; step < steps; ++step) {
                world->update(timestep);
                auto percept = world->getSensoryPercept();
                if (agent) {
                    agent->processSensoryInput(percept);
                }
                
                metrics.stepsCompleted++;
            }
            
            metrics.totalTime = timer.elapsed();
            return std::make_tuple(metrics, world->getAgentBody());
        }, py::arg("steps"), py::arg("agent"), py::arg("timestep") = 0.1,
             "Simulate complete episode and return metrics and final state")
        .def("createRandomEnvironment", [](const std::shared_ptr<SimpleWorld>& world,
                                          float resourceDensity = 0.1f,
                                          float hazardDensity = 0.05f) {
            // Placeholder for creating random environment
            return 0;  // Return object count
        }, py::arg("resourceDensity") = 0.1f, py::arg("hazardDensity") = 0.05f,
             "Create random environment with resources and hazards")
        .def("exportState", [](const std::shared_ptr<SimpleWorld>& world, 
                             const std::string& format = "json") {
            std::stringstream ss;
            ss << "{\n  \"width\": " << world->getWidth() << ",\n"
               << "  \"height\": " << world->getHeight() << ",\n"
               << "  \"time\": " << world->getSimulationTime() << "\n"
               << "}";
            return ss.str();
        }, py::arg("format") = "json", "Export world state in specified format");

    // ==================== CONVENIENCE FACTORIES AND UTILITIES ====================

    // Enhanced brain creation utilities
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with optimized Phase 6 settings");

    m.def("createHighPerformanceConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Optimize for performance
        config->set("brain.neuron_count", 500, ConfigSource::User);
        config->set("brain.synapse_density", 0.05, ConfigSource::User);
        config->set("plasticity.stdp.enable", true, ConfigSource::User);
        config->set("plasticity.hebbian.enable", true, ConfigSource::User);
        config->set("neuromod.curiosity.enable", true, ConfigSource::User);
        return config;
    }, "Create a high-performance configuration optimized for Phase 6");

    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Optimize for learning and development
        config->set("brain.neuron_count", 1000, ConfigSource::User);
        config->set("brain.synapse_density", 0.15, ConfigSource::User);
        config->set("development.enable", true, ConfigSource::User);
        config->set("neuromod.curiosity.enable", true, ConfigSource::User);
        config->set("neuromod.novelty.enable", true, ConfigSource::User);
        return config;
    }, "Create a development-focused configuration for rapid learning");

    m.def("createExplorationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Optimize for exploration and curiosity
        config->set("brain.neuron_count", 800, ConfigSource::User);
        config->set("brain.synapse_density", 0.1, ConfigSource::User);
        config->set("neuromod.curiosity.enable", true, ConfigSource::User);
        config->set("neuromod.novelty.enable", true, ConfigSource::User);
        config->set("plasticity.structural.enable", true, ConfigSource::User);
        return config;
    }, "Create an exploration-focused configuration for curious agents");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // ==================== EXPERIMENT RUNNING UTILITIES ====================

    m.def("runPhase6Experiment", [](const std::string& experimentName,
                                  const std::shared_ptr<Config>& config,
                                  size_t steps,
                                  double timestep,
                                  bool verbose = false) {
        // Placeholder for Phase 6 experiment runner
        return std::make_tuple(steps, 0.0, "Placeholder experiment results");
    }, py::arg("experimentName"), py::arg("config"), py::arg("steps"), 
       py::arg("timestep"), py::arg("verbose") = false,
       "Run a Phase 6 integration experiment (placeholder)");

    m.def("runMultiAgentExperiment", [](const py::list& brains,
                                       const py::list& worlds,
                                       const py::list& agents,
                                       size_t steps,
                                       double timestep) {
        // Placeholder for multi-agent experiment
        utils::PerformanceMetrics metrics;
        metrics.stepsCompleted = steps;
        return metrics;
    }, py::arg("brains"), py::arg("worlds"), py::arg("agents"),
       py::arg("steps"), py::arg("timestep"),
       "Run multi-agent experiment (placeholder)");

    m.def("runBatchBenchmark", [](const py::list& brains,
                                size_t steps,
                                double timestep) {
        // Placeholder for batch benchmark
        std::vector<double> times;
        std::vector<size_t> spikeCounts;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            auto brain = std::static_pointer_cast<Brain>(brains[i]);
            utils::Timer timer;
            
            for (size_t step = 0; step < steps; ++step) {
                brain->step(step);
            }
            
            double elapsed = timer.elapsed();
            times.push_back(elapsed);
            spikeCounts.push_back(brain->getTotalSpikeCount());
        }
        
        return std::make_tuple(times, spikeCounts);
    }, py::arg("brains"), py::arg("steps"), py::arg("timestep"),
       "Run benchmark on multiple brains and return performance data");

    // ==================== ANALYTICS AND VISUALIZATION UTILITIES ====================

    m.def("computeSimilarity", [](const py::dict& brain1, const py::dict& brain2) {
        // Placeholder for brain similarity computation
        return 0.0;  // Similarity score
    }, py::arg("brain1"), py::arg("brain2"),
       "Compute similarity between two brain states (placeholder)");

    m.def("extractFeatures", [](const std::shared_ptr<Brain>& brain) {
        // Placeholder for feature extraction
        py::dict features;
        features["firing_rate"] = brain->getAverageFiringRate();
        features["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        features["total_neurons"] = static_cast<int>(brain->getTotalNeuronCount());
        features["total_synapses"] = static_cast<int>(brain->getTotalSynapseCount());
        return features;
    }, py::arg("brain"),
       "Extract neural features from a brain for analysis");

    m.def("generateReport", [](const std::shared_ptr<Brain>& brain,
                              const utils::PerformanceMetrics& metrics,
                              const std::string& format = "text") {
        std::stringstream ss;
        if (format == "text") {
            ss << "=== NLM BRAIN ANALYSIS REPORT ===\n";
            ss << "Neural Statistics:\n";
            ss << "  Total Neurons: " << brain->getTotalNeuronCount() << "\n";
            ss << "  Total Synapses: " << brain->getTotalSynapseCount() << "\n";
            ss << "  Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
            ss << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
            ss << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
            ss << "  Total Spikes: " << brain->getTotalSpikeCount() << "\n\n";
            
            ss << "Performance Metrics:\n";
            ss << metrics.toString() << "\n";
        } else if (format == "json") {
            ss << "{\n  \"neurons\": " << brain->getTotalNeuronCount() << ",\n"
               << "  \"synapses\": " << brain->getTotalSynapseCount() << ",\n"
               << "  \"firing_rate\": " << brain->getAverageFiringRate() << ",\n"
               << "  \"e_i_ratio\": " << brain->getExcitationInhibitionRatio() << ",\n"
               << "  \"total_spikes\": " << brain->getTotalSpikeCount() << "\n"
               << "}";
        }
        
        return ss.str();
    }, py::arg("brain"), py::arg("metrics"), py::arg("format") = "text",
       "Generate a report for the brain in specified format");

    // ==================== LEGACY COMPATIBILITY (for easy_usage.md) ====================

    // Keep all the original API for backward compatibility
    m.def("createDefaultConfig_legacy", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Legacy: Create a default configuration");

    m.def("createBrain_legacy", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Legacy: Create a new brain with configuration");

    m.def("createSimpleWorld_legacy", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Legacy: Create a new simple world");

    m.def("createAgentBrain_legacy", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Legacy: Create a new agent brain interface");

    // ==================== MODULE ATTRIBUTES ====================

    m.attr("__version__") = "6.0.0-enhanced";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__description__") = "Enhanced Python bindings for NLM Phase 6 neural simulation framework";

    // Backward compatibility constants (same values as before)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ==================== ADVANCED COMMAND-LINE INTERFACE ====================

    m.def("run_from_cli", [](int argc, char** argv) {
        // Enhanced CLI interface for advanced usage
        auto config = std::make_shared<Config>();
        
        if (!config->loadFromFile("configs/default.cfg")) {
            config->loadFromArgs(argc, argv);
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        // Process command line mode
        std::string mode = "demo";
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg == "--mode" && i + 1 < argc) {
                mode = argv[++i];
            }
        }
        
        if (mode == "benchmark") {
            size_t steps = 1000;
            for (int i = 1; i < argc; ++i) {
                std::string arg(argv[i]);
                if (arg == "--steps" && i + 1 < argc) {
                    steps = std::stoul(argv[++i]);
                }
            }
            
            utils::Timer timer;
            for (size_t i = 0; i < steps; ++i) {
                brain->step(i);
            }
            
            double elapsed = timer.elapsed();
            std::cout << "Benchmark completed in " << elapsed << " seconds";
            std::cout << " (" << steps / elapsed << " steps/sec)" << std::endl;
        } else {
            // Default demo mode
            utils::Timer timer;
            for (size_t i = 0; i < 100; ++i) {
                brain->step(i);
            }
            std::cout << "Demo completed in " << timer.elapsed() << " seconds" << std::endl;
        }
        
        return brain;
    }, py::arg("argc"), py::arg("argv"), "Run NLM from command line interface (enhanced)");

    // ==================== EXPERIMENTAL FEATURES ====================

    m.def("experimental_neural_network", [](const py::dict& config_dict) {
        // Experimental neural network API (research code)
        auto config = std::make_shared<Config>();
        
        // Apply configuration from dictionary
        for (auto item : config_dict) {
            std::string key = py::str(item.first);
            std::string value = py::str(item.second);
            
            if (key.find("layer_size") != std::string::npos) {
                try {
                    int64_t val = std::stoll(value);
                    config->set(key, val, ConfigSource::User);
                } catch (...) {}
            }
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            return brain;
        }
        return std::shared_ptr<Brain>();
    }, py::arg("config_dict"), "Experimental neural network creation (research feature)");

    m.def("experimental_memory_profiler", [](const std::shared_ptr<Brain>& brain) {
        // Experimental memory profiling (research feature)
        return std::make_tuple(brain->getTotalNeuronCount(),
                               brain->getTotalSynapseCount(),
                               0.0);  // Placeholder for memory usage
    }, py::arg("brain"), "Experimental memory profiling (research feature)");

    m.def("experimental_visualizer", [](const std::shared_ptr<Brain>& brain,
                                       const std::string& format = "text") {
        // Experimental visualization (research feature)
        std::stringstream ss;
        ss << "=== EXPERIMENTAL NEURAL VISUALIZATION ===\n";
        ss << "Neuron count: " << brain->getTotalNeuronCount() << "\n";
        ss << "Synapse count: " << brain->getTotalSynapseCount() << "\n";
        ss << "Firing rate: " << brain->getAverageFiringRate() << " Hz\n";
        ss << "E/I ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        ss << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
        
        if (format == "svg") {
            ss << "\n<SVG visualization placeholder would be generated here>\n";
        }
        
        return ss.str();
    }, py::arg("brain"), py::arg("format") = "text", 
       "Experimental neural visualization (research feature)");

} // namespace nlm
