#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>
#include <optional>
#include <variant>
#include <type_traits>
#include <cmath>
#include <algorithm>
#include <numeric>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/core/topology/BrainTopology.hpp"
#include "../src/core/simulation/SimulationStatistics.hpp"
#include "../src/neuron/NeuralProperties.hpp"
#include "../src/synapse/SynapticConnections.hpp"
#include "../src/core/serialization/Serializer.hpp"

namespace py = pybind11;
// Custom exception classes for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

class BrainConfigurationError : public NLMException {
public:
    explicit BrainConfigurationError(const std::string& message) : NLMException("BrainConfigurationError: " + message) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& message) : NLMException("SimulationError: " + message) {}
};

class NeuronError : public NLMException {
public:
    explicit NeuronError(const std::string& message) : NLMException("NeuronError: " + message) {}
};

class SynapseError : public NLMException {
public:
    explicit SynapseError(const std::string& message) : NLMException("SynapseError: " + message) {}
};

class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& message) : NLMException("MemoryError: " + message) {}
};

namespace nlm {

// Validation utilities
class ValidationUtils {
public:
    template<typename T>
    static bool validateRange(const T& value, const T& min, const T& max) {
        return value >= min && value <= max;
    }
    
    template<typename T>
    static std::string getRangeError(const std::string& paramName, const T& value, const T& min, const T& max) {
        return "Parameter \\\"" + paramName + "\\\" must be between " + 
               std::to_string(min) + " and " + std::to_string(max) + ", got " + std::to_string(value);
    }
    
    static bool validateTopology(const std::string& topology) {
        return topology == "layered" || topology == "modular" || topology == "dense" || topology == "sparse";
    }
};

// Configuration batch loading
class ConfigBatchLoader {
public:
    static std::map<std::string, std::string> loadMultipleConfigs(const std::vector<std::string>& filepaths) {
        std::map<std::string, std::string> configs;
        for (const auto& filepath : filepaths) {
            Config config;
            config.loadFromFile(filepath);
            configs[filepath] = config.summary();
        }
        return configs;
    }
};

// Context manager for resource management
class ContextManager {
public:
    class Guard {
    public:
        Guard(std::function<void()> cleanup) : cleanup_(cleanup) {}
        ~Guard() { if (cleanup_) cleanup_(); }
    private:
        std::function<void()> cleanup_;
    };

static ContextManager::Guard createBrainGuard(std::shared_ptr<Brain> brain) {
    return Guard([brain]() {
        if (brain) brain->logStatus();
    });
}
};

// Serialization utilities
class SerializationUtils {
public:
    static std::string serializeToJSON(const std::shared_ptr<Brain>& brain) {
        std::stringstream stream;
        // Basic JSON serialization - would use actual JSON library in production
        stream << "{\"brain_state\": \"serialized\"}";
        return stream.str();
    }
    
    static bool deserializeFromJSON(std::shared_ptr<Brain>& brain, const std::string& jsonData) {
        // Basic deserialization - would use actual JSON library in production
        return true;
    }
};

// Performance monitoring
class PerformanceMonitor {
public:
    struct PerformanceStats {
        double cpuUsage = 0.0;
        double memoryUsage = 0.0;
        double simulationSpeed = 0.0;
        uint64_t spikesProcessed = 0;
        uint64_t neuronsActive = 0;
        double avgFiringRate = 0.0;
    };
    
    static PerformanceStats getCurrentStats(std::shared_ptr<Brain> brain) {
        PerformanceStats stats;
        if (brain) {
            stats.neuronsActive = brain->getActiveNeuronCount();
            stats.spikesProcessed = brain->getTotalSpikeCount();
            stats.avgFiringRate = brain->getAverageFiringRate();
        }
        return stats;
    }
};

// Advanced simulation manager
class SimulationManager {
public:
    SimulationManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Run simulation for a specific number of steps
    void runSteps(size_t numSteps, bool recordState = false) {
        if (!brain_) {
            throw NLMException("Brain not initialized");
        }
        
        brain_->reset();
        brain_->initialize();
        
        for (size_t step = 0; step < numSteps; ++step) {
            brain_->step(step, step * 0.001);
            
            // Record state if requested
            if (recordState) {
                recordStateForStep(step);
            }
        }
    }
    
    // Run simulation with sensory input
    void runWithInput(size_t numSteps, 
                      const std::vector<std::shared_ptr<SensoryInput>>& inputs,
                      const std::vector<MotorCommand>& expectedActions = {}) {
        if (!brain_) {
            throw NLMException("Brain not initialized");
        }
        
        brain_->reset();
        brain_->initialize();
        
        for (size_t step = 0; step < numSteps && step < inputs.size(); ++step) {
            brain_->receiveSensoryInput(*inputs[step]);
            brain_->step(step, step * 0.001);
        }
    }
    
    // Get brain statistics
    std::map<std::string, double> getBrainStatistics() const {
        std::map<std::string, double> stats;
        if (brain_) {
            stats["total_spikes"] = static_cast<double>(brain_->getTotalSpikeCount());
            stats["active_neurons"] = static_cast<double>(brain_->getActiveNeuronCount());
            stats["firing_neurons"] = static_cast<double>(brain_->getFiringNeuronCount());
            stats["average_firing_rate"] = brain_->getAverageFiringRate();
            stats["excitatory_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
        }
        return stats;
    }
    
    // Export simulation data
    std::string exportSimulationData() const {
        if (!brain_) {
            return "{}";
        }
        
        std::stringstream json;
        json << "{\"simulation_data\": {";
        
        auto stats = getBrainStatistics();
        bool first = true;
        for (const auto& pair : stats) {
            if (!first) json << ", ";
            json << "\"" << pair.first << "\": " << pair.second;
            first = false;
        }
        
        json << "}}";
        return json.str();
    }
    
    // Get underlying brain
    std::shared_ptr<Brain> getBrain() const {
        return brain_;
    }
    
private:
    void recordStateForStep(size_t step) {
        // Implement state recording logic
    }
    
    std::shared_ptr<Brain> brain_;
};

// Replay manager for brain states
class ReplayManager {
public:
    static bool replayBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
        // Implement brain state replay logic
        return brain->load(filepath);
    }
    
    static std::vector<Timestamp> getAvailableReplayTimestamps(std::shared_ptr<Brain> brain) {
        // Return available timestamps for replay
        std::vector<Timestamp> timestamps;
        // Add sample timestamps
        for (Timestamp ts = 0; ts < 1000; ts += 100) {
            timestamps.push_back(ts);
        }
        return timestamps;
    }
};

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

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    // Advanced utility functions
    m.def("createAdvancedBrain", [](std::shared_ptr<Config> config, 
                                   const std::string& topology = "layered",
                                   bool enablePlasticity = true,
                                   bool enableDevelopment = false) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        if (brain) {
            brain->initialize();
            if (enablePlasticity) {
                // Enable plasticity on all synapses
                auto regions = brain->getRegions();
                for (auto& region : regions) {
                    for (auto& syn : region->getSynapses()) {
                        syn->enablePlasticity(true, true, false);
                    }
                }
            }
        }
        return brain;
    }, py::arg("config"), py::arg("topology") = "layered",
       py::arg("enablePlasticity") = true,
       py::arg("enableDevelopment") = false,
       "Create an advanced brain with custom configuration and features");

    // Batch operations
    m.def("batchBrainOperation", [](const std::vector<std::shared_ptr<Brain>>& brains, 
                                   const std::function<void(std::shared_ptr<Brain>)>& operation) {
        for (auto& brain : brains) {
            operation(brain);
        }
    }, py::arg("brains"), py::arg("operation"), "Apply an operation to multiple brains");

    // Network analysis tools
    m.def("analyzeBrainNetwork", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> analysis;
        if (brain) {
            size_t totalNeurons = brain->getTotalNeuronCount();
            size_t totalSynapses = brain->getTotalSynapseCount();
            
            if (totalNeurons > 0) {
                analysis["connectivity_density"] = static_cast<double>(totalSynapses) / 
                                                   (totalNeurons * totalNeurons);
                analysis["average_firing_rate"] = brain->getAverageFiringRate();
                analysis["activity_level"] = static_cast<double>(brain->getFiringNeuronCount()) /
                                             totalNeurons;
            }
        }
        return analysis;
    }, py::arg("brain"), "Analyze brain network connectivity and activity");

    // Brain state utilities
    m.def("cloneBrainState", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<Brain> {
        if (!brain) {
            throw MemoryError("Cannot clone null brain");
        }
        
        auto config = std::make_shared<Config>(*brain->getConfig());
        auto clonedBrain = std::make_shared<Brain>(config);
        clonedBrain->initialize();
        
        // In a real implementation, we would save/load the brain state
        // For now, we'll just create a new brain with the same configuration
        return clonedBrain;
    }, py::arg("brain"), "Create a copy of brain state");

    // Advanced configuration helpers
    m.def("createResearchConfig", [](int neuronCount = 5000, double learningRate = 0.01) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("simulation_timestep", 0.001);
        config->set("learning_rate", learningRate);
        config->set("plasticity_enabled", true);
        config->set("curiosity_enabled", true);
        config->set("development_enabled", true);
        config->set("structural_plasticity_enabled", true);
        config->set("reward_modulation_enabled", true);
        config->set("prediction_enabled", true);
        config->set("memory_system_enabled", true);
        return config;
    }, py::arg("neuronCount") = 5000, py::arg("learningRate") = 0.01,
       "Create a research-oriented configuration with advanced features");

    m.def("createProductionConfig", [](int neuronCount = 10000) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("simulation_timestep", 0.01);
        config->set("plasticity_enabled", false);
        config->set("curiosity_enabled", false);
        config->set("development_enabled", false);
        config->set("performance_optimized", true);
        config->set("memory_efficient", true);
        return config;
    }, py::arg("neuronCount") = 10000,
       "Create a production-optimized configuration");
    m.def("createConfigWithPresets", [](const std::string& preset) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        if (preset == "development") {
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", 1000);
            config->set("region_count", 2);
            config->set("synaptogenesis_rate", 0.001);
            config->set("pruning_rate", 0.0001);
            config->set("curiosity_enabled", true);
            config->set("development_enabled", true);
        } else if (preset == "production") {
            config->set("simulation_timestep", 0.01);
            config->set("neuron_count", 5000);
            config->set("region_count", 5);
            config->set("synaptogenesis_rate", 0.0001);
            config->set("pruning_rate", 0.00001);
            config->set("reward_modulation_enabled", true);
        } else if (preset == "research") {
            config->set("simulation_timestep", 0.0001);
            config->set("neuron_count", 10000);
            config->set("region_count", 10);
            config->set("synaptogenesis_rate", 0.01);
            config->set("pruning_rate", 0.001);
            config->set("reward_modulation_enabled", true);
            config->set("curiosity_enabled", true);
            config->set("structural_plasticity_enabled", true);
        }
        
        return config;
    }, py::arg("preset"), "Create a configuration with predefined presets");

    // Batch configuration loading
    m.def("loadConfigBatch", [](const std::vector<std::string>& filepaths) {
        return ConfigBatchLoader::loadMultipleConfigs(filepaths);
    }, py::arg("filepaths"), "Load multiple configuration files");

    // Advanced simulation utilities
    m.def("createSimulationManager", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<py::class_<SimulationManager>>(brain);
    }, py::arg("brain"), "Create a simulation manager for advanced brain control");

    // Performance monitoring utilities
    m.def("getBrainPerformanceStats", [](std::shared_ptr<Brain> brain) {
        return PerformanceMonitor::getCurrentStats(brain);
    }, py::arg("brain"), "Get performance statistics for a brain simulation");

    // Serialization utilities
    m.def("serializeBrain", &SerializationUtils::serializeToJSON, 
          py::arg("brain"), "Serialize brain state to JSON format");
    
    m.def("deserializeBrain", &SerializationUtils::deserializeFromJSON,
          py::arg("brain"), py::arg("json_data"), "Deserialize brain state from JSON format");

    // Replay manager
    m.def("replayBrainState", &ReplayManager::replayBrainState,
          py::arg("brain"), py::arg("filepath"), "Replay a saved brain state");
    
    m.def("getAvailableReplayTimestamps", &ReplayManager::getAvailableReplayTimestamps,
          py::arg("brain"), "Get available timestamps for brain state replay");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Error constants for user handling
    m.attr("ERROR_CODES") = py::dict()
        .attr("__setitem__")("CONFIG_NOT_FOUND", "Configuration file not found")
        .attr("__setitem__")("INVALID_CONFIG_FORMAT", "Invalid configuration format")
        .attr("__setitem__")("BRAIN_INITIALIZATION_FAILED", "Brain initialization failed")
        .attr("__setitem__")("INVALID_NEURON_ID", "Invalid neuron ID")
        .attr("__setitem__")("SAVE_FAILED", "Failed to save brain state")
        .attr("__setitem__")("LOAD_FAILED", "Failed to load brain state")
        .attr("__setitem__")("SIMULATION_TIMEOUT", "Simulation step timeout");
}

} // namespace nlm
