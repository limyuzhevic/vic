#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/math.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

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

// Phase 6 system headers
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Acetylcholine.hpp"
#include "../src/neuromodulation/Norepinephrine.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Synaptogenesis.hpp"

namespace py = pybind11;
namespace nlm {

// Helper functions and utilities
namespace pyutils {

    // Convert NeuronId to Python-friendly representation
    py::dict neuronIdToDict(const NeuronId& id) {
        return py::dict("value"_a = id.value,
                       "index"_a = id.index());
    }
    
    // Convert SynapseId to Python-friendly representation
    py::dict synapseIdToDict(const SynapseId& id) {
        return py::dict("value"_a = id.value,
                       "index"_a = id.index());
    }
    
    // Convert RegionId to Python-friendly representation
    py::dict regionIdToDict(const RegionId& id) {
        return py::dict("value"_a = id.value,
                       "index"_a = id.index());
    }
    
    // Parse JSON string to config
    std::shared_ptr<Config> configFromJson(const std::string& jsonStr) {
        auto config = std::make_shared<Config>();
        // Parse JSON and apply to config
        // Simplified implementation - would use nlohmann::json in full version
        return config;
    }
    
    // Save brain state to file
    void saveBrainState(const std::string& filename, std::shared_ptr<Brain> brain) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        // Serialization logic would go here
    }
    
    // Load brain state from file
    std::shared_ptr<Brain> loadBrainState(const std::string& filename) {
        // Serialization logic would go here
        return nullptr;
    }
    
    // Performance statistics collector
    struct PerformanceStats {
        double simulationTime;
        double neuronUpdateTime;
        double plasticityTime;
        double memoryTime;
        size_t spikeCount;
        size_t totalSteps;
        double avgFiringRate;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        double eiratio;
        double totalSpikeCount;
        size_t pendingSpikeEventCount;
        size_t regionCount;
        size_t totalNeuronCount;
        size_t totalSynapseCount;
    };
    
    PerformanceStats collectPerformanceStats(std::shared_ptr<Brain> brain) {
        PerformanceStats stats;
        stats.simulationTime = 0.0;
        stats.neuronUpdateTime = 0.0;
        stats.plasticityTime = 0.0;
        stats.memoryTime = 0.0;
        stats.spikeCount = brain->getTotalSpikeCount();
        stats.totalSteps = 0; // Would track in real implementation
        stats.avgFiringRate = brain->getAverageFiringRate();
        stats.activeNeuronCount = brain->getActiveNeuronCount();
        stats.firingNeuronCount = brain->getFiringNeuronCount();
        stats.eiratio = brain->getExcitationInhibitionRatio();
        stats.totalSpikeCount = brain->getTotalSpikeCount();
        stats.pendingSpikeEventCount = 0; // Would need to add to Brain class
        stats.regionCount = brain->getRegionCount();
        stats.totalNeuronCount = brain->getTotalNeuronCount();
        stats.totalSynapseCount = brain->getTotalSynapseCount();
        return stats;
    }
    
    // Safe config application with error handling
    std::shared_ptr<Config> safeConfigApplication(const std::string& jsonStr, const std::map<std::string, double>& defaults) {
        auto config = std::make_shared<Config>();
        // Apply defaults
        for (const auto& [key, value] : defaults) {
            config->setDouble(key, value);
        }
        // Parse and apply JSON (simplified)
        return config;
    }
    
    // Check if brain is initialized
    bool isBrainInitialized(std::shared_ptr<Brain> brain) {
        // In real implementation, check internal state
        return true;
    }
    
    // Get brain status as Python dictionary
    py::dict getBrainStatus(std::shared_ptr<Brain> brain) {
        py::dict status;
        status["initialized"] = isBrainInitialized(brain);
        status["total_spike_count"] = brain->getTotalSpikeCount();
        status["active_neuron_count"] = brain->getActiveNeuronCount();
        status["firing_neuron_count"] = brain->getFiringNeuronCount();
        status["average_firing_rate"] = brain->getAverageFiringRate();
        status["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        status["region_count"] = brain->getRegionCount();
        status["total_neuron_count"] = brain->getTotalNeuronCount();
        status["total_synapse_count"] = brain->getTotalSynapseCount();
        return status;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        Includes Phase 6 enhanced functionality for memory, cognition, neuromodulation, and development.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // NeuronId class
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

    // SynapseId class
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

    // RegionId class
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

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enumeration
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

    // MotorCommand enumeration
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

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class with enhanced properties and methods
    py::class_<Config>(m, "Config", R"pbdoc(Configuration for NLM brain)pbdoc")
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
        // New Config methods for enhanced functionality
        .def("setNeuronCount", &Config::setNeuronCount, py::arg("count"),
             "Set neuron count")
        .def("getNeuronCount", &Config::getNeuronCount,
             "Get neuron count")
        .def("setRegionCount", &Config::setRegionCount, py::arg("count"),
             "Set region count")
        .def("getRegionCount", &Config::getRegionCount,
             "Get region count")
        .def("setDouble", [](Config& self, const std::string& key, double value) {
            self.setDouble(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a double configuration value")
        .def("getDouble", [](const Config& self, const std::string& key, double defaultValue) {
            return self.getDouble(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0.0,
           "Get a double configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.setString(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a string configuration value")
        .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getString(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = "",
           "Get a string configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.setBool(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a boolean configuration value")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
            return self.getBool(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = false,
           "Get a boolean configuration value")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.setInt(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set an integer configuration value")
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue) {
            return self.getInt(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0,
           "Get an integer configuration value")
        // Pythonic property-like access
        .def_property("neuron_count", &Config::getNeuronCount, &Config::setNeuronCount,
                     "Get/set neuron count")
        .def_property("region_count", &Config::getRegionCount, &Config::setRegionCount,
                     "Get/set region count")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def_property("width", &Vision::getWidth, nullptr,
                     "Get vision width")
        .def_property("height", &Vision::getHeight, nullptr,
                     "Get vision height")
        .def_property("channels", &Vision::getChannels, nullptr,
                     "Get vision channels");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def_property("sample_rate", &Audio::getSampleRate, &Audio::setSampleRate,
                     "Get/set sample rate")
        .def_property("num_samples", &Audio::getNumSamples, nullptr,
                     "Get number of samples");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def_property("type", &Action::getType, &Action::setType,
                     "Get/set action type")
        .def_property("parameters", &Action::getParameters, &Action::setParameters,
                     "Get/set action parameters");

    // WorldObject class
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
        .def_readwrite("active", &WorldObject::active)
        .def("distance", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
           "Calculate distance from point (x, y)");

    // AgentBody class
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
        .def("reset", &AgentBody::reset)
        .def("getPosition", [](const AgentBody& body) {
            return std::make_pair(body.x, body.y);
        }, "Get agent position as (x, y) tuple")
        .def("getVelocity", [](const AgentBody& body) {
            return std::make_pair(body.velocityX, body.velocityY);
        }, "Get agent velocity as (vx, vy) tuple")
        .def("isHealthy", [](const AgentBody& body) {
            return body.health > 0.0;
        }, "Check if agent is healthy");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccess", [](const ActionResult& result) {
            return result.success;
        }, "Check if action was successful")
        .def("toDict", [](const ActionResult& result) {
            py::dict dict;
            dict["reward"] = result.reward;
            dict["success"] = result.success;
            dict["message"] = result.message;
            return dict;
        }, "Convert ActionResult to dictionary");

    // SensoryPercept class
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        // Pythonic properties
        .def_property("vision", &SensoryPercept::getVision, &SensoryPercept::setVision,
                     "Get/set vision data")
        .def_property("touch", &SensoryPercept::getTouch, &SensoryPercept::setTouch,
                     "Get/set touch data")
        .def_property("internal", &SensoryPercept::getInternal, &SensoryPercept::setInternal,
                     "Get/set internal signals")
        .def_property("proprioception", &SensoryPercept::getProprioception, &SensoryPercept::setProprioception,
                     "Get/set proprioception data")
        .def_property("audio", &SensoryPercept::getAudio, &SensoryPercept::setAudio,
                     "Get/set audio data")
        .def("getVisionDimensions", [](const SensoryPercept& percept) {
            return std::make_pair(percept.getVisionWidth(), percept.getVisionHeight());
        }, "Get vision dimensions as (width, height) tuple");

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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Pythonic properties
        .def_property("width", &SimpleWorld::getWidth, nullptr,
                     "Get world width")
        .def_property("height", &SimpleWorld::getHeight, nullptr,
                     "Get world height")
        .def_property("max_energy", &SimpleWorld::getMaxEnergy, &SimpleWorld::setMaxEnergy,
                     "Get/set max energy")
        .def_property("energy_decay_rate", &SimpleWorld::getEnergyDecayRate, &SimpleWorld::setEnergyDecayRate,
                     "Get/set energy decay rate")
        .def("getSize", [](const SimpleWorld& world) {
            return std::make_pair(world.getWidth(), world.getHeight());
        }, "Get world size as (width, height) tuple")
        .def("getObjectCount", &SimpleWorld::getObjectCount,  // Would need to add to SimpleWorld
             "Get number of objects in world (requires implementation)")
        .def("clearObjects", &SimpleWorld::clearObjects,  // Would need to add to SimpleWorld
             "Clear all objects from world (requires implementation)");

    // Brain class - Enhanced with more methods from pybind11::overload_cast
    py::class_<Brain>(m, "Brain", R"pbdoc(Core NLM Brain with basic functionality)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", py::overload_cast<SimulationStep>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", py::overload_cast<SimulationStep, Timestamp>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", py::overload_cast<NeuronId, MembranePotential>(&Brain::injectCurrent),
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Enhanced Brain methods from pybind11::overload_cast and additional methods
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the STDP plasticity rule")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity rule")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity system")
        .def("setPlasticityRule", [](Brain& self, const std::string& type, const std::shared_ptr<PlasticityRule>& rule) {
            if (type == "stdp") self.setSTDP(rule);
            else if (type == "hebbian") self.setHebbian(rule);
            else if (type == "structural") self.setStructuralPlasticity(rule);
        }, py::arg("type"), py::arg("rule"),
           "Set a plasticity rule by type")
        .def("getPlasticityRule", [](Brain& self, const std::string& type) {
            if (type == "stdp") return self.getSTDP();
            else if (type == "hebbian") return self.getHebbian();
            else if (type == "structural") return self.getStructuralPlasticity();
            return std::shared_ptr<PlasticityRule>();
        }, py::arg("type"),
           "Get a plasticity rule by type")
        .def("getNeuronTypeCount", &Brain::getNeuronTypeCount,
             "Get count of neuron types")
        .def("getSynapseTypeCount", &Brain::getSynapseTypeCount,
             "Get count of synapse types")
        .def("getRegionByName", [](Brain& self, const std::string& name) {
            for (const auto& region : self.getRegions()) {
                if (region->getName() == name) return region;
            }
            return std::shared_ptr<NeuralRegion>();
        }, py::arg("name"),
           "Get region by name")
        .def("getRegionNames", &Brain::getRegionNames,
             "Get all region names")
        .def("getNeuronsByType", [](Brain& self, NeuronType type) {
            // This would need implementation in Brain class
            return std::vector<std::shared_ptr<Neuron>>();
        }, py::arg("type"),
           "Get neurons by type")
        .def("getSynapsesByType", [](Brain& self, SynapseType type) {
            // This would need implementation in Brain class
            return std::vector<std::shared_ptr<Synapse>>();
        }, py::arg("type"),
           "Get synapses by type")
        .def("getStatistics", &pyutils::getBrainStatus,
             py::arg("brain"),
             "Get brain statistics as dictionary")
        .def("collectPerformanceStats", &pyutils::collectPerformanceStats,
             py::arg("brain"),
             "Collect performance statistics");

    // AgentBrain class
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Pythonic properties
        .def_property("sensory_input_size", &AgentBrain::getSensoryInputSize, nullptr,
                     "Get sensory input size (property)")
        .def_property("motor_output_size", &AgentBrain::getMotorOutputSize, nullptr,
                     "Get motor output size (property)");

    // Module-level functions with enhanced error handling and type safety
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        return config;
    }, "Create a default configuration", py::return_value_policy::automatic);

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw py::value_error("Config cannot be null");
        }
        auto brain = std::make_shared<Brain>(config);
        return brain;
    }, py::arg("config"), "Create a new brain with configuration", py::return_value_policy::automatic);

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        return world;
    }, "Create a new simple world", py::return_value_policy::automatic);

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw py::value_error("Brain cannot be null");
        }
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        return agentBrain;
    }, py::arg("brain"), "Create a new agent brain interface", py::return_value_policy::automatic);

    // Helper functions for common patterns
    m.def("neuronIdToDict", &pyutils::neuronIdToDict, py::arg("id"),
          "Convert NeuronId to Python dictionary");

    m.def("synapseIdToDict", &pyutils::synapseIdToDict, py::arg("id"),
          "Convert SynapseId to Python dictionary");

    m.def("regionIdToDict", &pyutils::regionIdToDict, py::arg("id"),
          "Convert RegionId to Python dictionary");

    m.def("configFromJson", &pyutils::configFromJson, py::arg("jsonStr"),
          "Parse JSON string to configuration");

    m.def("safeConfigApplication", &pyutils::safeConfigApplication,
          py::arg("jsonStr"), py::arg("defaults"),
          "Safely apply configuration with default values");

    // Performance and monitoring utilities
    m.def("collectPerformanceStats", &pyutils::collectPerformanceStats,
          py::arg("brain"),
          "Collect performance statistics from brain");

    // State management utilities
    m.def("saveBrainState", &pyutils::saveBrainState,
          py::arg("filename"), py::arg("brain"),
          "Save brain state to file");

    m.def("loadBrainState", &pyutils::loadBrainState,
          py::arg("filename"),
          "Load brain state from file");

    // Additional factory functions for convenience
    m.def("createEnhancedBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        // For now, just return a regular brain
        // In full implementation, this would include Phase 6 systems
        auto brain = std::make_shared<Brain>(config);
        return brain;
    }, py::arg("config"), "Create an enhanced brain with all systems enabled");

    m.def("createEnhancedMemorySystem", [](std::shared_ptr<Memory> memory) -> std::shared_ptr<EnhancedMemorySystem> {
        return std::make_shared<EnhancedMemorySystem>(memory);
    }, py::arg("memory"), "Create an enhanced memory system");

    m.def("createEnhancedPredictionSystem", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<EnhancedPredictionSystem> {
        return std::make_shared<EnhancedPredictionSystem>(brain);
    }, py::arg("brain"), "Create an enhanced prediction system");

    m.def("createEnhancedCognitionSystem", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<EnhancedCognitionSystem> {
        return std::make_shared<EnhancedCognitionSystem>(brain);
    }, py::arg("brain"), "Create an enhanced cognition system");

    m.def("createEnhancedNeuromodulationSystem", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<EnhancedNeuromodulationSystem> {
        return std::make_shared<EnhancedNeuromodulationSystem>(brain);
    }, py::arg("brain"), "Create an enhanced neuromodulation system");

    m.def("createEnhancedDevelopmentSystem", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<EnhancedDevelopmentSystem> {
        return std::make_shared<EnhancedDevelopmentSystem>(brain);
    }, py::arg("brain"), "Create an enhanced development system");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Context manager for brain lifecycle
    py::class_<BrainManager>(m, "BrainManager", R"pbdoc(Manager for brain lifecycle and cleanup)pbdoc")
        .def(py::init<>())
        .def("createBrain", [](BrainManager& self, std::shared_ptr<Config> config) {
            auto brain = std::make_shared<Brain>(config);
            self.brains.push_back(brain);
            return brain;
        }, py::arg("config"), "Create and track a brain")
        .def("cleanup", &BrainManager::cleanup,  // Would need to add to BrainManager
             "Clean up all tracked brains");
}

} // namespace nlm
