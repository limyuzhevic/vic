#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

// Include all NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/dynamics/NeuralDynamics.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/MotorSystem.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"

namespace py = pybind11;
namespace nlm {

// Helper functions for Python
namespace python_helpers {

    // Convert vector of floats to neural pattern
    inline std::vector<float> toPattern(const std::vector<float>& data) {
        return data;
    }
    
    // Convert float array to neural pattern
    inline std::vector<float> toPattern(py::array_t<float> array) {
        auto buf = array.request();
        float* ptr = (float*)buf.ptr;
        size_t size = buf.size;
        return std::vector<float>(ptr, ptr + size);
    }
    
    // Convert neural pattern to Python list
    inline py::list fromPattern(const std::vector<float>& pattern) {
        py::list result;
        for (float value : pattern) {
            result.append(value);
        }
        return result;
    }
    
    // Convert neuron ID to Python string
    inline std::string neuronIdToString(const NeuronId& id) {
        return "<NeuronId: " + std::to_string(id.value) + ">";
    }
    
    // Convert synapse ID to Python string
    inline std::string synapseIdToString(const SynapseId& id) {
        return "<SynapseId: " + std::to_string(id.value) + ">";
    }
    
    // Convert region ID to Python string
    inline std::string regionIdToString(const RegionId& id) {
        return "<RegionId: " + std::to_string(id.value) + ">";
    }
    
    // Validate neural pattern dimensions
    inline void validatePattern(const std::vector<float>& pattern, size_t expectedSize,
                               const std::string& context) {
        if (pattern.size() != expectedSize) {
            throw std::runtime_error("Invalid pattern size in " + context + ": expected " +
                                   std::to_string(expectedSize) + ", got " +
                                   std::to_string(pattern.size()));
        }
    }
    
    // Create a simple neural network with specified parameters
    inline std::shared_ptr<Brain> createSimpleBrain(uint64_t seed = 42) {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Runtime);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        config->set("connection_probability", 0.15f, ConfigSource::Runtime);
        return std::make_shared<Brain>(config);
    }
    
    // Create world with specified dimensions
    inline std::shared_ptr<SimpleWorld> createSimpleWorld(size_t width, size_t height,
                                                          size_t visionWidth, size_t visionHeight) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }
    
    // Create agent brain with given brain and world
    inline std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain,
                                                       std::shared_ptr<SimpleWorld> world) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        return agentBrain;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        This enhanced version provides:
        - Complete integration with all NLM systems
        - Enhanced error handling and validation
        - Advanced neural pattern processing utilities
        - Brain lifecycle management functions
        - World and agent interfaces
        - Memory system access
        - Cognition system interfaces
        - Neuromodulation system access
        - Development system controls
        - Advanced configuration management
        - Performance monitoring utilities
        - Pattern matching and similarity functions
        - Learning experiment helpers
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // =============================================================
    // BASIC TYPES AND IDENTIFIERS
    // =============================================================

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

    // =============================================================
    // ENUMERATIONS
    // =============================================================

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

    // =============================================================
    // CONFIGURATION SYSTEM
    // =============================================================

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
        })
        // Type-safe get methods
        .def("get", [](Config& self, const std::string& key) {
            return self.get<std::string>(key);
        }, py::arg("key"), "Generic get method")
        .def("getInt", [](Config& self, const std::string& key) {
            return self.get<int64_t>(key);
        }, py::arg("key"), "Get integer value")
        .def("getFloat", [](Config& self, const std::string& key) {
            return self.get<double>(key);
        }, py::arg("key"), "Get float value")
        .def("getBool", [](Config& self, const std::string& key) {
            return self.get<bool>(key);
        }, py::arg("key"), "Get boolean value")
        .def("getString", [](Config& self, const std::string& key) {
            return self.get<std::string>(key);
        }, py::arg("key"), "Get string value")
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set integer value")
        .def("setFloat", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set float value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set boolean value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set string value");

    // =============================================================
    // SENSORY SYSTEMS
    // =============================================================

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

    // =============================================================
    // MOTOR SYSTEMS
    // =============================================================

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

    // =============================================================
    // WORLD SYSTEM
    // =============================================================

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

    // =============================================================
    // MEMORY SYSTEMS
    // =============================================================

    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"))
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f)
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"))
        .def("retrieve", &NeuralWorkingMemory::retrieve)
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"))
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"))
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"))
        .def("clear", &NeuralWorkingMemory::clear)
        .def("getCurrentPattern", [](NeuralWorkingMemory& self) {
            return python_helpers::fromPattern(self.retrieve());
        })
        .def("getPatternSize", [](NeuralWorkingMemory& self) {
            return self.retrieve().size();
        });

    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory system)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("episode"))
        .def("retrieveEpisode", &EpisodicMemory::retrieveEpisode, py::arg("index"))
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount)
        .def("getRecentEpisodes", &EpisodicMemory::getRecentEpisodes, py::arg("count"))
        .def("clear", &EpisodicMemory::clear)
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("threshold"))
        .def("getEpisodeAt", [](EpisodicMemory& self, size_t index) {
            auto episode = self.retrieveEpisode(index);
            py::dict result;
            result["timestamp"] = episode.timestamp;
            result["reward"] = episode.reward;
            result["action"] = static_cast<int>(episode.action);
            result["position"] = py::make_tuple(episode.positionX, episode.positionY, episode.orientation);
            return result;
        });

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Associative memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"))
        .def("createAssociation", &NeuralAssociativeMemory::createAssociation,
             py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 0.5f)
        .def("retrieveAssociation", &NeuralAssociativeMemory::retrieveAssociation,
             py::arg("pattern"), py::arg("threshold") = 0.3f)
        .def("hasAssociations", &NeuralAssociativeMemory::hasAssociations, py::arg("pattern"))
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength,
             py::arg("patternA"), py::arg("patternB"))
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation,
             py::arg("patternA"), py::arg("patternB"), py::arg("learningRate") = 0.01f,
             py::arg("shouldStrengthen") = true)
        .def("getAllPatterns", &NeuralAssociativeMemory::getAllPatterns)
        .def("clear", &NeuralAssociativeMemory::clear)
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount)
        .def("getAverageAssociationStrength", &NeuralAssociativeMemory::getAverageAssociationStrength)
        .def("consolidate", &NeuralAssociativeMemory::consolidate,
             py::arg("strengthenThreshold") = 0.7f, py::arg("weakenThreshold") = 0.2f)
        .def("getAssociations", &NeuralAssociativeMemory::getAssociations,
             py::arg("pattern"), py::arg("maxResults") = 5)
        .def("getPatternSize", [](NeuralAssociativeMemory& self) {
            auto patterns = self.getAllPatterns();
            if (!patterns.empty()) {
                return patterns[0].size();
            }
            return size_t(0);
        })
        .def("setHebbianRate", &NeuralAssociativeMemory::setHebbianRate, py::arg("rate"))
        .def("setStrengthBounds", &NeuralAssociativeMemory::setStrengthBounds,
             py::arg("min"), py::arg("max"));

    // =============================================================
    // COGNITION SYSTEMS
    // =============================================================

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planning system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"))
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f)
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence,
             py::arg("actions"), py::arg("startState"))
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth)
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"))
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence)
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality,
             py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"))
        .def("clearCache", &NeuralPlanner::clearCache)
        .def("setActionQuality", &NeuralPlanner::setActionQuality,
             py::arg("action"), py::arg("quality"))
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal)
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"))
        .def("getConceptCount", &ConceptFormation::getConceptCount)
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f)
        .def("isNovel", &ConceptFormation::isNovel,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f)
        .def("computeSimilarity", &ConceptFormation::computeSimilarity,
             py::arg("a"), py::arg("b"))
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold, py::arg("threshold"))
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow, py::arg("window"));

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"))
        .def("presentExperience", &ConceptFormation::presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"))
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f)
        .def("getConcepts", &ConceptFormation::getConcepts)
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"))
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"))
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"))
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"))
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"))
        .def("updateConcept", &ConceptFormation::updateConcept,
             py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"))
        .def("clear", &ConceptFormation::clear)
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("conceptId"));

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attentional selection system)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"))
        .def("processSensoryInput", &AttentionalSelection::processSensoryInput,
             py::arg("sensorySignals"), py::arg("availableNeurons"))
        .def("updateBasedOnReward", &AttentionalSelection::updateBasedOnReward,
             py::arg("reward"), py::arg("selectedNeurons"))
        .def("getCurrentFocus", &AttentionalSelection::getCurrentFocus)
        .def("getAttentionGain", &AttentionalSelection::getAttentionGain, py::arg("neuron"))
        .def("setCompetitionStrength", &AttentionalSelection::setCompetitionStrength, py::arg("strength"))
        .def("setLearningRate", &AttentionalSelection::setLearningRate, py::arg("rate"))
        .def("setSelectionThreshold", &AttentionalSelection::setSelectionThreshold, py::arg("threshold"))
        .def("deservesAttention", &AttentionalSelection::deservesAttention,
             py::arg("novelty"), py::arg("urgency"), py::arg("relevance"))
        .def("getAverageAttentionalFocus", &AttentionalSelection::getAverageAttentionalFocus)
        .def("getActiveAttentionalNeurons", &AttentionalSelection::getActiveAttentionalNeurons)
        .def("clear", &AttentionalSelection::clear)
        .def("learnFromReward", &AttentionalSelection::learnFromReward,
             py::arg("reward"), py::arg("neurons"), py::arg("attentionValues"))
        .def("updateBasedOnPredictionError", &AttentionalSelection::updateBasedOnPredictionError,
             py::arg("error"), py::arg("currentFocus"))
        .def("getNeuralAttentionPattern", &AttentionalSelection::getNeuralAttentionPattern);

    // =============================================================
    // BRAIN SYSTEM
    // =============================================================

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
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Memory systems
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        // Prediction system
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        // Cognition systems
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        // Neuromodulation systems
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error system");

    // =============================================================
    // AGENT SYSTEM
    // =============================================================

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

    // =============================================================
    // FACTORY FUNCTIONS
    // =============================================================

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createDefaultBrain", []() -> std::shared_ptr<Brain> {
        return python_helpers::createSimpleBrain();
    }, "Create a brain with default configuration");

    m.def("createVision", [](size_t width, size_t height, size_t channels = 3) -> std::unique_ptr<Vision> {
        return std::make_unique<Vision>(width, height, channels);
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, "Create a vision sensory input");

    m.def("createAudio", [](size_t sampleRate, size_t numSamples) -> std::unique_ptr<Audio> {
        return std::make_unique<Audio>(sampleRate, numSamples);
    }, py::arg("sampleRate"), py::arg("numSamples"), "Create an audio sensory input");

    m.def("createAction", [](ActionType type, std::vector<float> parameters) -> std::unique_ptr<Action> {
        return std::make_unique<Action>(type, parameters);
    }, py::arg("type"), py::arg("parameters") = std::vector<float>(), "Create an action");

    // =============================================================
    // UTILITY FUNCTIONS
    // =============================================================

    m.def("patternSimilarity", [](const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Patterns must have the same size");
        }
        if (a.empty()) {
            return 1.0f;
        }
        float dotProduct = 0.0f;
        float normA = 0.0f;
        float normB = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            dotProduct += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        if (normA == 0.0f || normB == 0.0f) {
            return 0.0f;
        }
        return dotProduct / std::sqrt(normA * normB);
    }, py::arg("patternA"), py::arg("patternB"), "Compute cosine similarity between two patterns");

    m.def("normalizePattern", [](const std::vector<float>& pattern) {
        float norm = 0.0f;
        for (float value : pattern) {
            norm += value * value;
        }
        norm = std::sqrt(norm);
        if (norm == 0.0f) {
            return pattern;
        }
        std::vector<float> result;
        result.reserve(pattern.size());
        for (float value : pattern) {
            result.push_back(value / norm);
        }
        return result;
    }, py::arg("pattern"), "Normalize a pattern to unit length");

    m.def("generateRandomPattern", [](size_t size, float minValue = 0.0f, float maxValue = 1.0f) {
        std::vector<float> result;
        result.reserve(size);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(minValue, maxValue);
        for (size_t i = 0; i < size; ++i) {
            result.push_back(dist(gen));
        }
        return result;
    }, py::arg("size"), py::arg("minValue") = 0.0f, py::arg("maxValue") = 1.0f, "Generate a random pattern");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", [](size_t width, size_t height,
                                 size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"), "Create a simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        return agentBrain;
    }, py::arg("brain"), py::arg("world"), "Create an agent brain interface");

    // =============================================================
    // CONSTANTS
    // =============================================================

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // =============================================================
    // HELP AND DOCUMENTATION
    // =============================================================

    m.def("printBanner", &printBanner, "Print the NLM banner");
    m.def("printHelp", &printHelp, py::arg("programName"), "Print help information");

    // =============================================================
    // ERROR HANDLING
    // =============================================================

    m.def("setErrorHandling", [](bool throwOnError, bool verboseErrors) {
        // Set global error handling behavior
        // This can be extended to configure error handling globally
    }, py::arg("throwOnError") = true, py::arg("verboseErrors") = true, "Configure error handling behavior");

    m.def("getVersion", []() {
        py::dict version;
        version["major"] = 2;
        version["minor"] = 0;
        version["patch"] = 0;
        version["phase"] = "2";
        version["description"] = "Real Neural Computation";
        return version;
    }, "Get NLM version information");

    m.def("getSystemInfo", []() {
        py::dict info;
        info["name"] = "NLM (Neural Learning Machine)";
        info["version"] = "2.0.0";
        info["phase"] = "2: Real Neural Computation";
        info["description"] = "An experimental artificial developmental brain";
        info["features"] = py::list({
            "LIF neuron dynamics",
            "Event-driven spike propagation",
            "STDP and Hebbian plasticity",
            "Structural plasticity",
            "Memory systems (working, episodic, associative)",
            "Prediction system",
            "Cognition systems (planning, concept formation, attention)",
            "Neuromodulation systems (dopamine, curiosity, novelty)",
            "Development system",
            "Python bindings with enhanced functionality"
        });
        return info;
    }, "Get system information and features");

} // namespace nlm
