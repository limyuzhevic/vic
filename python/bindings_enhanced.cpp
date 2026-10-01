#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <stdexcept>

// NumPy support
#ifdef HAVE_NUMPY
#include <numpy/arrayobject.h>
#endif

// Forward declarations for all NLM classes
namespace nlm {
    // Core types
    class Config;
    class Config;
    class NeuronId;
    class SynapseId;
    class RegionId;
    class PopulationId;
    class SimulationStep;
    class Timestamp;
    class MembranePotential;
    class Delay;
    
    // Basic types
    enum class NeuronType;
    enum class SynapseType;
    enum class DevelopmentalStage;
    enum class FiringState;
    enum class ActionType;
    enum class MotorCommand;
    enum class WorldObjectType;
    
    // Configuration
    class Config;
    
    // Sensory input
    class SensoryInput;
    class Vision;
    class Audio;
    class InternalSignals;
    
    // Motor and actions
    class Action;
    class WorldObject;
    class AgentBody;
    class ActionResult;
    
    // Perception and world
    class SensoryPercept;
    class SimpleWorld;
    
    // Brain and agent
    class Brain;
    class AgentBrain;
    
    // Memory systems
    class NeuralWorkingMemory;
    class NeuralEpisodicMemory;
    class NeuralAssociativeMemory;
    
    // Prediction systems
    class PredictionSystem;
    class NeuralPrediction;
    
    // Cognition systems
    class NeuralPlanner;
    class ConceptFormation;
    class AttentionalSelection;
    
    // Neuromodulation
    class Neuromodulator;
    class Dopamine;
    class Curiosity;
    class Novelty;
    class PredictionError;
    
    // Development
    class DevelopmentSystem;
    class Synaptogenesis;
    class Maturation;
    class Pruning;
    
    // Plasticity
    class STDP;
    class Hebbian;
    class StructuralPlasticity;
    
    // Environment
    class Environment;
    class Observation;
    
    // Experiments
    class Experiment;
    class ExperimentRunner;
    
    // Performance
    class MemoryPool;
    class EventQueue;
    
    // Visualization
    class VisualizationInterface;
}

namespace py = pybind11;

// Constants
namespace nlm {
    extern const NeuronId INVALID_NEURON_ID;
    extern const SynapseId INVALID_SYNAPSE_ID;
    extern const RegionId INVALID_REGION_ID;
    extern const PopulationId INVALID_POPULATION_ID;
}

// Helper functions for Python integration
namespace pybind11 {
    template<typename T>
    py::array_t<T> to_numpy(const std::vector<T>& vec) {
#ifdef HAVE_NUMPY
        py::array_t<T> array({vec.size()});
        std::copy(vec.begin(), vec.end(), array.mutable_data());
        return array;
#else
        throw std::runtime_error("NumPy not available");
#endif
    }
    
    template<typename T>
    std::vector<T> from_numpy(py::array_t<T> array) {
        return std::vector<T>(array.data(), array.data() + array.size());
    }
}

// Type conversion utilities
namespace pybind11::detail {
    // Helper to convert Python dict to ConfigValue
    inline nlm::ConfigValue dict_to_config_value(py::dict dict) {
        if (dict.contains("int")) return dict["int"].cast<int>();
        if (dict.contains("int64")) return dict["int64"].cast<int64_t>();
        if (dict.contains("double")) return dict["double"].cast<double>();
        if (dict.contains("bool")) return dict["bool"].cast<bool>();
        if (dict.contains("string")) return dict["string"].cast<std::string>();
        if (dict.contains("int_list")) return dict["int_list"].cast<std::vector<int>>();
        if (dict.contains("double_list")) return dict["double_list"].cast<std::vector<double>>();
        if (dict.contains("string_list")) return dict["string_list"].cast<std::vector<std::string>>();
        throw py::type_error("Unsupported ConfigValue type");
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ==============================================
        
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides access to all neural simulation capabilities including:
        - Core neural dynamics and plasticity
        - Memory systems (working, episodic, associative)
        - Prediction and cognition systems
        - Neuromodulation (dopamine, curiosity, novelty)
        - Development and learning
        - Complete agent-environment interaction
        - Advanced scientific computing integration
        
        Features:
        - NumPy integration for vector operations
        - Rich type conversion utilities
        - Comprehensive error handling
        - High-level convenience functions
        - Extensive documentation and examples
        )pbdoc";
    
    // Error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    
    // Type aliases for convenience
    using NeuralVector = std::vector<float>;
    using NeuralMatrix = std::vector<std::vector<float>>;
    
    // Forward declare factory functions for later use
    std::shared_ptr<nlm::Config> create_default_config();
    std::shared_ptr<nlm::Brain> create_brain(std::shared_ptr<nlm::Config> config);
    std::shared_ptr<nlm::SimpleWorld> create_simple_world();
    std::shared_ptr<nlm::AgentBrain> create_agent_brain(std::shared_ptr<nlm::Brain> brain);

    // ========== CORE TYPES ==========
    
    // NeuronId - Unique neuron identifier
    py::class_<nlm::NeuronId> neuron_id_class(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc");
    neuron_id_class
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::NeuronId::value)
        .def("index", &nlm::NeuronId::index)
        .def("__eq__", &nlm::NeuronId::operator==)
        .def("__ne__", &nlm::NeuronId::operator!=)
        .def("__hash__", [](const nlm::NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const nlm::NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });
    
    // SynapseId - Unique synapse identifier
    py::class_<nlm::SynapseId> synapse_id_class(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc");
    synapse_id_class
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::SynapseId::value)
        .def("index", &nlm::SynapseId::index)
        .def("__eq__", &nlm::SynapseId::operator==)
        .def("__ne__", &nlm::SynapseId::operator!=)
        .def("__hash__", [](const nlm::SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });
    
    // RegionId - Unique region identifier
    py::class_<nlm::RegionId> region_id_class(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc");
    region_id_class
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::RegionId::value)
        .def("index", &nlm::RegionId::index)
        .def("__eq__", &nlm::RegionId::operator==)
        .def("__ne__", &nlm::RegionId::operator!=)
        .def("__hash__", [](const nlm::RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });
    
    // PopulationId - Unique population identifier
    py::class_<nlm::PopulationId> population_id_class(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc");
    population_id_class
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::PopulationId::value)
        .def("index", &nlm::PopulationId::index)
        .def("__eq__", &nlm::PopulationId::operator==)
        .def("__ne__", &nlm::PopulationId::operator!=)
        .def("__hash__", [](const nlm::PopulationId& id) { return std::hash<uint64_t>{}(id.value); });
    
    // ========== ENUMS ==========
    
    // NeuronType enumeration
    py::enum_<nlm::NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();
    
    // SynapseType enumeration
    py::enum_<nlm::SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", nlm::SynapseType::Excitatory)
        .value("Inhibitory", nlm::SynapseType::Inhibitory)
        .value("Modulatory", nlm::SynapseType::Modulatory)
        .value("Electrical", nlm::SynapseType::Electrical)
        .value("GapJunction", nlm::SynapseType::GapJunction)
        .export_values();
    
    // DevelopmentalStage enumeration
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();
    
    // FiringState enumeration
    py::enum_<nlm::FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", nlm::FiringState::Resting)
        .value("Active", nlm::FiringState::Active)
        .value("Refractory", nlm::FiringState::Refractory)
        .value("Inhibited", nlm::FiringState::Inhibited)
        .export_values();
    
    // ActionType enumeration
    py::enum_<nlm::ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
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
        .value("Custom", nlm::ActionType::Custom)
        .export_values();
    
    // MotorCommand enumeration
    py::enum_<nlm::MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", nlm::MotorCommand::MoveForward)
        .value("MoveBackward", nlm::MotorCommand::MoveBackward)
        .value("TurnLeft", nlm::MotorCommand::TurnLeft)
        .value("TurnRight", nlm::MotorCommand::TurnRight)
        .value("LookLeft", nlm::MotorCommand::LookLeft)
        .value("LookRight", nlm::MotorCommand::LookRight)
        .value("Interact", nlm::MotorCommand::Interact)
        .value("Wait", nlm::MotorCommand::Wait)
        .export_values();
    
    // WorldObjectType enumeration
    py::enum_<nlm::WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker)
        .export_values();
    
    // ========== CONFIG CLASS ==========
    
    py::class_<nlm::Config> config_class(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc");
    config_class
        .def(py::init<>())
        .def("loadFromFile", &nlm::Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](nlm::Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &nlm::Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &nlm::Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &nlm::Config::getKeys,
             "Get all configuration keys")
        .def("clear", &nlm::Config::clear,
             "Clear all configuration entries")
        .def("summary", &nlm::Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const nlm::Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
    
    // Enhanced Config methods for Python
    config_class.def("get", [](const nlm::Config& self, const std::string& key) {
        return self.get<std::string>(key); // This will need proper type deduction
    }, py::arg("key"), "Get configuration value by key (overloaded)")
        .def("get", [](const nlm::Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default");
    
    // ========== SENSORY INPUT CLASSES ==========
    
    py::class_<nlm::SensoryInput> sensory_input_class(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc");
    sensory_input_class
        .def("getType", &nlm::SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &nlm::SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &nlm::SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &nlm::SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &nlm::SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");
    
    // Vision class
    py::class_<nlm::Vision, nlm::SensoryInput> vision_class(m, "Vision", R"pbdoc(Vision sensory input)pbdoc");
    vision_class
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](nlm::Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &nlm::Vision::getWidth)
        .def("getHeight", &nlm::Vision::getHeight)
        .def("getChannels", &nlm::Vision::getChannels);
    
    // Audio class
    py::class_<nlm::Audio, nlm::SensoryInput> audio_class(m, "Audio", R"pbdoc(Audio sensory input)pbdoc");
    audio_class
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](nlm::Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &nlm::Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &nlm::Audio::getSampleRate)
        .def("getNumSamples", &nlm::Audio::getNumSamples);
    
    // InternalSignals class
    py::class_<nlm::InternalSignals, nlm::SensoryInput> internal_signals_class(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc");
    internal_signals_class
        .def(py::init<>())
        .def("addSignal", &nlm::InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &nlm::InternalSignals::clearSignals);
    
    // ========== ACTION AND WORLD OBJECT CLASSES ==========
    
    // Action class
    py::class_<nlm::Action> action_class(m, "Action", R"pbdoc(Action representation for motor output)pbdoc");
    action_class
        .def(py::init<>())
        .def(py::init<nlm::ActionType>(), py::arg("type"))
        .def(py::init<nlm::ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &nlm::Action::getType)
        .def("setType", &nlm::Action::setType, py::arg("type"))
        .def("getParameters", &nlm::Action::getParameters)
        .def("setParameters", &nlm::Action::setParameters, py::arg("params"))
        .def("getName", &nlm::Action::getName)
        .def("clone", &nlm::Action::clone)
        .def("__repr__", [](const nlm::Action& action) {
            return "<Action: " + action.getName() + ">";
        });
    
    // WorldObject class
    py::class_<nlm::WorldObject> world_object_class(m, "WorldObject", R"pbdoc(World object representation)pbdoc");
    world_object_class
        .def(py::init<>())
        .def(py::init<float, float, nlm::WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &nlm::WorldObject::x)
        .def_readwrite("y", &nlm::WorldObject::y)
        .def_readwrite("radius", &nlm::WorldObject::radius)
        .def_readwrite("type", &nlm::WorldObject::type)
        .def_readwrite("value", &nlm::WorldObject::value)
        .def_readwrite("active", &nlm::WorldObject::active)
        .def("__repr__", [](const nlm::WorldObject& obj) {
            return "<WorldObject at (" + std::to_string(obj.x) + ", " + std::to_string(obj.y) + ") type=" + std::to_string(static_cast<int>(obj.type)) + ">";
        });
    
    // AgentBody class
    py::class_<nlm::AgentBody> agent_body_class(m, "AgentBody", R"pbdoc(Agent body state)pbdoc");
    agent_body_class
        .def(py::init<>())
        .def_readwrite("x", &nlm::AgentBody::x)
        .def_readwrite("y", &nlm::AgentBody::y)
        .def_readwrite("orientation", &nlm::AgentBody::orientation)
        .def_readwrite("velocityX", &nlm::AgentBody::velocityX)
        .def_readwrite("velocityY", &nlm::AgentBody::velocityY)
        .def_readwrite("angularVelocity", &nlm::AgentBody::angularVelocity)
        .def_readwrite("energy", &nlm::AgentBody::energy)
        .def_readwrite("health", &nlm::AgentBody::health)
        .def_readwrite("age", &nlm::AgentBody::age)
        .def_readwrite("isMoving", &nlm::AgentBody::isMoving)
        .def_readwrite("isTurning", &nlm::AgentBody::isTurning)
        .def_readwrite("lastActionTime", &nlm::AgentBody::lastActionTime)
        .def("reset", &nlm::AgentBody::reset)
        .def("__repr__", [](const nlm::AgentBody& body) {
            return "<AgentBody at (" + std::to_string(body.x) + ", " + std::to_string(body.y) + ") energy=" + std::to_string(body.energy) + ">";
        });
    
    // ActionResult class
    py::class_<nlm::ActionResult> action_result_class(m, "ActionResult", R"pbdoc(Action result from world)pbdoc");
    action_result_class
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &nlm::ActionResult::reward)
        .def_readwrite("success", &nlm::ActionResult::success)
        .def_readwrite("message", &nlm::ActionResult::message)
        .def("__repr__", [](const nlm::ActionResult& result) {
            return "<ActionResult reward=" + std::to_string(result.reward) + " success=" + std::to_string(result.success) + ">";
        });
    
    // ========== PERCEPTION AND WORLD CLASSES ==========
    
    // SensoryPercept class
    py::class_<nlm::SensoryPercept> sensory_percept_class(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc");
    sensory_percept_class
        .def(py::init<>())
        .def("getVision", &nlm::SensoryPercept::getVision)
        .def("setVision", &nlm::SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &nlm::SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &nlm::SensoryPercept::getVisionHeight)
        .def("getTouch", &nlm::SensoryPercept::getTouch)
        .def("setTouch", &nlm::SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &nlm::SensoryPercept::getInternal)
        .def("setInternal", &nlm::SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &nlm::SensoryPercept::getProprioception)
        .def("setProprioception", &nlm::SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &nlm::SensoryPercept::getAudio)
        .def("setAudio", &nlm::SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &nlm::SensoryPercept::getAllSignals)
        .def("getTimestamp", &nlm::SensoryPercept::getTimestamp)
        .def("setTimestamp", &nlm::SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("__repr__", [](const nlm::SensoryPercept& percept) {
            return "<SensoryPercept vision=" + std::to_string(percept.getVisionWidth()) + "x" + std::to_string(percept.getVisionHeight()) + ">";
        });
    
    // SimpleWorld class
    py::class_<nlm::SimpleWorld> simple_world_class(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc");
    simple_world_class
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision")
        .def("reset", &nlm::SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &nlm::SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("getAgentBody", &nlm::SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &nlm::SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &nlm::SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &nlm::SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getObjectAt", &nlm::SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             "Get object at position (if any)")
        .def("getWidth", &nlm::SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &nlm::SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &nlm::SimpleWorld::getMaxEnergy, "Get max energy")
        .def("setMaxEnergy", &nlm::SimpleWorld::setMaxEnergy, py::arg("e"), "Set max energy")
        .def("getEnergyDecayRate", &nlm::SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &nlm::SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &nlm::SimpleWorld::getRandomSeed, "Get random seed")
        .def("__repr__", [](const nlm::SimpleWorld& world) {
            return "<SimpleWorld " + std::to_string(world.getWidth()) + "x" + std::to_string(world.getHeight()) + "s>";
        });
    
    // ========== BRAIN AND AGENT CLASSES ==========
    
    // Brain class
    py::class_<nlm::Brain> brain_class(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc");
    brain_class
        .def(py::init<std::shared_ptr<nlm::Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &nlm::Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep)>(&nlm::Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep, nlm::Timestamp)>(&nlm::Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &nlm::Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &nlm::Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &nlm::Brain::reset,
             "Reset brain state")
        .def("save", &nlm::Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &nlm::Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &nlm::Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &nlm::Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &nlm::Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &nlm::Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &nlm::Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &nlm::Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &nlm::Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &nlm::Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &nlm::Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &nlm::Brain::logStatus,
             "Log brain status")
        .def("__repr__", [](const nlm::Brain& brain) {
            return "<Brain regions=" + std::to_string(brain.getRegionCount()) + ">";
        });
    
    // AgentBrain class
    py::class_<nlm::AgentBrain> agent_brain_class(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc");
    agent_brain_class
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"),
             "Create agent brain with brain reference")
        .def("initialize", &nlm::AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &nlm::AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &nlm::AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &nlm::AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &nlm::AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &nlm::AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &nlm::AgentBrain::isCuriosityEnabled);
    
    // ========== ADVANCED SYSTEMS (EXPERT LEVEL) ==========
    
    // Memory Systems
    py::class_<nlm::NeuralWorkingMemory> working_memory_class(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory system)pbdoc");
    working_memory_class
        .def(py::init<>())
        .def("initialize", &nlm::NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &nlm::NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information in working memory")
        .def("storeToNeuron", &nlm::NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"),
             "Store specific neuron activation")
        .def("retrieve", &nlm::NeuralWorkingMemory::retrieve, "Retrieve current working memory content")
        .def("contains", &nlm::NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if specific neuron is part of working memory")
        .def("getNeuronActivation", &nlm::NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"),
             "Get activation level of a specific neuron in working memory")
        .def("update", &nlm::NeuralWorkingMemory::update, py::arg("dt"),
             "Update working memory (maintenance and decay)")
        .def("clear", &nlm::NeuralWorkingMemory::clear, "Clear working memory")
        .def("getActiveTraces", &nlm::NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &nlm::NeuralWorkingMemory::getCapacity, "Get capacity")
        .def("setCapacity", &nlm::NeuralWorkingMemory::setCapacity, py::arg("cap"), "Set capacity")
        .def("getDecayRate", &nlm::NeuralWorkingMemory::getDecayRate, "Get decay rate")
        .def("setDecayRate", &nlm::NeuralWorkingMemory::setDecayRate, py::arg("rate"), "Set decay rate")
        .def("getMemoryNeurons", &nlm::NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory")
        .def("strengthenMemory", &nlm::NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
             "Strengthen working memory representation")
        .def("runCompetition", &nlm::NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("isWinning", &nlm::NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Is this neuron part of the winning population?")
        .def("getMemoryActivity", &nlm::NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");
    
    // Prediction System
    py::class_<nlm::PredictionSystem> prediction_system_class(m, "PredictionSystem", R"pbdoc(Prediction system for sensory prediction and error computation)pbdoc");
    prediction_system_class
        .def(py::init<>())
        .def("initialize", &nlm::PredictionSystem::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("update", &nlm::PredictionSystem::update, py::arg("input"), py::arg("dt"),
             "Update prediction system")
        .def("getPrediction", &nlm::PredictionSystem::getPrediction, "Get current prediction")
        .def("getPredictionError", &nlm::PredictionSystem::getPredictionError, "Get prediction error")
        .def("getConfidence", &nlm::PredictionSystem::getConfidence, "Get prediction confidence")
        .def("reset", &nlm::PredictionSystem::reset, "Reset prediction system");
    
    // Neural Prediction
    py::class_<nlm::NeuralPrediction> neural_prediction_class(m, "NeuralPrediction", R"pbdoc(Neural prediction module)pbdoc");
    neural_prediction_class
        .def(py::init<>())
        .def("predict", [](nlm::NeuralPrediction& self, const std::vector<float>& input) {
            return self.predict(input);
        }, py::arg("input"), "Make a prediction")
        .def("getPredictionError", &nlm::NeuralPrediction::getPredictionError, "Get prediction error")
        .def("getExpectedValue", &nlm::NeuralPrediction::getExpectedValue, "Get expected value")
        .def("getUncertainty", &nlm::NeuralPrediction::getUncertainty, "Get prediction uncertainty");
    
    // Cognition Systems
    py::class_<nlm::NeuralPlanner> neural_planner_class(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning)pbdoc");
    neural_planner_class
        .def(py::init<>())
        .def("planAction", [](nlm::NeuralPlanner& self, const std::vector<float>& state) {
            return self.planAction(state);
        }, py::arg("state"), "Plan action for given state")
        .def("evaluateAction", &nlm::NeuralPlanner::evaluateAction, py::arg("action"), py::arg("state"),
             "Evaluate action for given state")
        .def("getActionSequence", &nlm::NeuralPlanner::getActionSequence,
             "Get planned action sequence")
        .def("clearPlan", &nlm::NeuralPlanner::clearPlan, "Clear current plan");
    
    // Concept Formation
    py::class_<nlm::ConceptFormation> concept_formation_class(m, "ConceptFormation", R"pbdoc(Concept formation from experience patterns)pbdoc");
    concept_formation_class
        .def(py::init<>())
        .def("learnConcept", [](nlm::ConceptFormation& self, const std::vector<float>& pattern, const std::string& name) {
            self.learnConcept(pattern, name);
        }, py::arg("pattern"), py::arg("name"), "Learn a new concept")
        .def("getConcept", &nlm::ConceptFormation::getConcept, py::arg("name"),
             "Get concept by name")
        .def("getAllConcepts", &nlm::ConceptFormation::getAllConcepts,
             "Get all learned concepts")
        .def("applyConcept", [](nlm::ConceptFormation& self, const std::string& name, const std::vector<float>& pattern) {
            return self.applyConcept(name, pattern);
        }, py::arg("name"), py::arg("pattern"), "Apply concept to pattern");
    
    // Attentional Selection
    py::class_<nlm::AttentionalSelection> attentional_selection_class(m, "AttentionalSelection", R"pbdoc(Attention with competitive selection dynamics)pbdoc");
    attentional_selection_class
        .def(py::init<>())
        .def("initialize", &nlm::AttentionalSelection::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("focusOnRegion", &nlm::AttentionalSelection::focusOnRegion, py::arg("region"),
             "Apply attention to a specific region")
        .def("releaseAttention", &nlm::AttentionalSelection::releaseAttention,
             "Release attention (allow all regions equal processing)")
        .def("getAttendedRegions", &nlm::AttentionalSelection::getAttendedRegions,
             "Get current attention focus")
        .def("setInhibitionStrength", &nlm::AttentionalSelection::setInhibitionStrength, py::arg("strength"),
             "Set inhibition strength")
        .def("setExcitationStrength", &nlm::AttentionalSelection::setExcitationStrength, py::arg("strength"),
             "Set excitation strength")
        .def("setCompetitionThreshold", &nlm::AttentionalSelection::setCompetitionThreshold, py::arg("threshold"),
             "Set competition threshold")
        .def("getInhibitionFor", &nlm::AttentionalSelection::getInhibitionFor, py::arg("neuron"),
             "Get inhibition level for a neuron")
        .def("getExcitationFor", &nlm::AttentionalSelection::getExcitationFor, py::arg("neuron"),
             "Get excitation level for a neuron")
        .def("update", &nlm::AttentionalSelection::update, py::arg("dt"),
             "Update attention dynamics")
        .def("isAttended", &nlm::AttentionalSelection::isAttended, py::arg("neuron"),
             "Is this neuron currently attended?")
        .def("applyTopDownBias", &nlm::AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"),
             "Apply top-down attention bias")
        .def("applyBottomUpSalience", &nlm::AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"),
             "Apply bottom-up salience")
        .def("reset", &nlm::AttentionalSelection::reset, "Reset attention");
    
    // Neuromodulation Systems
    py::class_<nlm::Dopamine> dopamine_class(m, "Dopamine", R"pbdoc(Dopamine neuromodulator - reward and reinforcement)pbdoc");
    dopamine_class
        .def(py::init<>())
        .def("initialize", &nlm::Dopamine::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &nlm::Dopamine::getLevel, "Get dopamine level")
        .def("update", &nlm::Dopamine::update, py::arg("reward"), py::arg("predictionError"), py::arg("dt"),
             "Update dopamine based on reward and prediction error")
        .def("getRewardSignal", &nlm::Dopamine::getRewardSignal, "Get reward signal")
        .def("getPredictionErrorSignal", &nlm::Dopamine::getPredictionErrorSignal, "Get prediction error signal")
        .def("reset", &nlm::Dopamine::reset, "Reset dopamine system");
    
    // Curiosity
    py::class_<nlm::Curiosity> curiosity_class(m, "Curiosity", R"pbdoc(Curiosity drive - exploration motivation)pbdoc");
    curiosity_class
        .def(py::init<>())
        .def("initialize", &nlm::Curiosity::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &nlm::Curiosity::getLevel, "Get curiosity level")
        .def("update", &nlm::Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error")
        .def("getExplorationDrive", &nlm::Curiosity::getExplorationDrive, "Get exploration drive")
        .def("setNoveltyWeight", &nlm::Curiosity::setNoveltyWeight, py::arg("weight"),
             "Set novelty weight")
        .def("setPredictionErrorWeight", &nlm::Curiosity::setPredictionErrorWeight, py::arg("weight"),
             "Set prediction error weight")
        .def("reset", &nlm::Curiosity::reset, "Reset curiosity system");
    
    // Novelty
    py::class_<nlm::Novelty> novelty_class(m, "Novelty", R"pbdoc(Novelty detection - detects new stimuli)pbdoc");
    novelty_class
        .def(py::init<>())
        .def("initialize", &nlm::Novelty::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &nlm::Novelty::getLevel, "Get novelty level")
        .def("update", &nlm::Novelty::update, py::arg("input"), py::arg("dt"),
             "Update novelty based on input")
        .def("getNoveltySignal", &nlm::Novelty::getNoveltySignal, "Get novelty signal")
        .def("getSurprise", &nlm::Novelty::getSurprise, "Get surprise value")
        .def("reset", &nlm::Novelty::reset, "Reset novelty system");
    
    // PredictionError
    py::class_<nlm::PredictionError> prediction_error_class(m, "PredictionError", R"pbdoc(Prediction error signal)pbdoc");
    prediction_error_class
        .def(py::init<>())
        .def("initialize", &nlm::PredictionError::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &nlm::PredictionError::getLevel, "Get prediction error level")
        .def("update", &nlm::PredictionError::update, py::arg("prediction"), py::arg("actual"), py::arg("dt"),
             "Update prediction error")
        .def("getErrorSignal", &nlm::PredictionError::getErrorSignal, "Get error signal")
        .def("getAbsoluteError", &nlm::PredictionError::getAbsoluteError, "Get absolute error")
        .def("getSquaredError", &nlm::PredictionError::getSquaredError, "Get squared error")
        .def("reset", &nlm::PredictionError::reset, "Reset prediction error system");
    
    // ========== DEVELOPMENT SYSTEMS ==========
    
    // DevelopmentSystem
    py::class_<nlm::DevelopmentSystem> development_system_class(m, "DevelopmentSystem", R"pbdoc(Development system for learning and maturation)pbdoc");
    development_system_class
        .def(py::init<>())
        .def("initialize", &nlm::DevelopmentSystem::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("update", &nlm::DevelopmentSystem::update, py::arg("dt"),
             "Update development system")
        .def("getStage", &nlm::DevelopmentSystem::getStage, "Get current developmental stage")
        .def("setStage", &nlm::DevelopmentSystem::setStage, py::arg("stage"), "Set developmental stage")
        .def("getAge", &nlm::DevelopmentSystem::getAge, "Get developmental age")
        .def("getPlasticityRate", &nlm::DevelopmentSystem::getPlasticityRate, "Get plasticity rate")
        .def("getMaturationLevel", &nlm::DevelopmentSystem::getMaturationLevel, "Get maturation level")
        .def("reset", &nlm::DevelopmentSystem::reset, "Reset development system");
    
    // Synaptogenesis
    py::class_<nlm::Synaptogenesis> synaptogenesis_class(m, "Synaptogenesis", R"pbdoc(Neural synapse formation during development)pbdoc");
    synaptogenesis_class
        .def(py::init<>())
        .def("update", &nlm::Synaptogenesis::update, py::arg("dt"),
             "Update synaptogenesis process")
        .def("getGrowthRate", &nlm::Synaptogenesis::getGrowthRate, "Get growth rate")
        .def("setGrowthRate", &nlm::Synaptogenesis::setGrowthRate, py::arg("rate"),
             "Set growth rate")
        .def("getPruningThreshold", &nlm::Synaptogenesis::getPruningThreshold, "Get pruning threshold")
        .def("getConnectionDensity", &nlm::Synaptogenesis::getConnectionDensity, "Get connection density")
        .def("reset", &nlm::Synaptogenesis::reset, "Reset synaptogenesis");
    
    // Maturation
    py::class_<nlm::Maturation> maturation_class(m, "Maturation", R"pbdoc(Neural maturation and specialization)pbdoc");
    maturation_class
        .def(py::init<>())
        .def("update", &nlm::Maturation::update, py::arg("dt"),
             "Update maturation process")
        .def("getSpecialization", &nlm::Maturation::getSpecialization, "Get specialization level")
        .def("setSpecialization", &nlm::Maturation::setSpecialization, py::arg("level"),
             "Set specialization level")
        .def("getSpeed", &nlm::Maturation::getSpeed, "Get maturation speed")
        .def("setSpeed", &nlm::Maturation::setSpeed, py::arg("speed"),
             "Set maturation speed")
        .def("getEfficiency", &nlm::Maturation::getEfficiency, "Get efficiency")
        .def("reset", &nlm::Maturation::reset, "Reset maturation");
    
    // Pruning
    py::class_<nlm::Pruning> pruning_class(m, "Pruning", R"pbdoc(Neural pruning and elimination)pbdoc");
    pruning_class
        .def(py::init<>())
        .def("update", &nlm::Pruning::update, py::arg("dt"),
             "Update pruning process")
        .def("getPruneRate", &nlm::Pruning::getPruneRate, "Get pruning rate")
        .def("setPruneRate", &nlm::Pruning::setPruneRate, py::arg("rate"),
             "Set pruning rate")
        .def("getWeakConnectionThreshold", &nlm::Pruning::getWeakConnectionThreshold,
             "Get weak connection threshold")
        .def("getPrunedCount", &nlm::Pruning::getPrunedCount, "Get number of pruned connections")
        .def("reset", &nlm::Pruning::reset, "Reset pruning");
    
    // ========== PLASTICITY SYSTEMS ==========
    
    // STDP
    py::class_<nlm::STDP> stdp_class(m, "STDP", R"pbdoc(Spiking-based STDP plasticity)pbdoc");
    stdp_class
        .def(py::init<>())
        .def("update", &nlm::STDP::update, py::arg("preSpike"), py::arg("postSpike"), py::arg("dt"),
             "Update STDP weights")
        .def("getLearningRate", &nlm::STDP::getLearningRate, "Get learning rate")
        .def("setLearningRate", &nlm::STDP::setLearningRate, py::arg("rate"),
             "Set learning rate")
        .def("getLTPwindow", &nlm::STDP::getLTPwindow, "Get LTP time window")
        .def("getLTDwindow", &nlm::STDP::getLTDwindow, "Get LTD time window")
        .def("getMaxWeight", &nlm::STDP::getMaxWeight, "Get max weight")
        .def("getMinWeight", &nlm::STDP::getMinWeight, "Get min weight");
    
    // Hebbian
    py::class_<nlm::Hebbian> hebbian_class(m, "Hebbian", R"pbdoc(Hebbian plasticity rule)pbdoc");
    hebbian_class
        .def(py::init<>())
        .def("update", &nlm::Hebbian::update, py::arg("preSpike"), py::arg("postSpike"), py::arg("coherence"), py::arg("dt"),
             "Update Hebbian weights")
        .def("getLearningRate", &nlm::Hebbian::getLearningRate, "Get learning rate")
        .def("setLearningRate", &nlm::Hebbian::setLearningRate, py::arg("rate"),
             "Set learning rate")
        .def("getCoherenceThreshold", &nlm::Hebbian::getCoherenceThreshold, "Get coherence threshold")
        .def("setCoherenceThreshold", &nlm::Hebbian::setCoherenceThreshold, py::arg("threshold"),
             "Set coherence threshold");
    
    // StructuralPlasticity
    py::class_<nlm::StructuralPlasticity> structural_plasticity_class(m, "StructuralPlasticity", R"pbdoc(Structural plasticity - changing neural connections)pbdoc");
    structural_plasticity_class
        .def(py::init<>())
        .def("addConnection", &nlm::StructuralPlasticity::addConnection, py::arg("source"), py::arg("target"), py::arg("strength"),
             "Add new structural connection")
        .def("removeConnection", &nlm::StructuralPlasticity::removeConnection, py::arg("source"), py::arg("target"),
             "Remove structural connection")
        .def("update", &nlm::StructuralPlasticity::update, py::arg("dt"),
             "Update structural plasticity")
        .def("getConnectionCount", &nlm::StructuralPlasticity::getConnectionCount,
             "Get number of structural connections")
        .def("getConnectionStrength", &nlm::StructuralPlasticity::getConnectionStrength, py::arg("source"), py::arg("target"),
             "Get connection strength")
        .def("setGrowthRate", &nlm::StructuralPlasticity::setGrowthRate, py::arg("rate"),
             "Set growth rate")
        .def("setPruneRate", &nlm::StructuralPlasticity::setPruneRate, py::arg("rate"),
             "Set prune rate");
    
    // ========== ENVIRONMENT AND EXPERIMENT SYSTEMS ==========
    
    // Environment
    py::class_<nlm::Environment> environment_class(m, "Environment", R"pbdoc(Environment interface)pbdoc");
    environment_class
        .def(py::init<>())
        .def("reset", &nlm::Environment::reset, "Reset environment")
        .def("update", &nlm::Environment::update, py::arg("dt"),
             "Update environment")
        .def("getObservation", &nlm::Environment::getObservation, "Get current observation")
        .def("applyAction", &nlm::Environment::applyAction, py::arg("action"), "Apply action to environment");
    
    // Experiment
    py::class_<nlm::Experiment> experiment_class(m, "Experiment", R"pbdoc(Experiment framework)pbdoc");
    experiment_class
        .def(py::init<>("name"), py::arg("name"))
        .def("run", &nlm::Experiment::run, "Run experiment")
        .def("getResults", &nlm::Experiment::getResults, "Get experiment results")
        .def("addMetric", &nlm::Experiment::addMetric, py::arg("name"), py::arg("value"),
             "Add metric to experiment");
    
    // ========== HIGH-LEVEL CONVENIENCE FUNCTIONS ==========
    
    // Factory functions
    m.def("createDefaultConfig", &create_default_config, "Create a default configuration")
    m.def("createBrain", &create_brain, py::arg("config"), "Create a new brain with configuration")
    m.def("createSimpleWorld", &create_simple_world, "Create a new simple world")
    m.def("createAgentBrain", &create_agent_brain, py::arg("brain"), "Create a new agent brain interface")
    
    // High-level convenience functions
    m.def("createDemoEnvironment", []() {
        auto config = create_default_config();
        auto world = create_simple_world();
        auto brain = create_brain(config);
        auto agent = create_agent_brain(brain);
        agent->initialize(*world);
        return std::make_tuple(config, world, brain, agent);
    }, "Create a demo environment with all components")
    
    // Simulation convenience functions
    m.def("runEpisode", [](std::shared_ptr<nlm::AgentBrain> agent, std::shared_ptr<nlm::SimpleWorld> world, int maxSteps) {
        for (int step = 0; step < maxSteps; ++step) {
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            world->update(0.1); // Fixed timestep
        }
        return world->getSimulationTime();
    }, py::arg("agent"), py::arg("world"), py::arg("maxSteps"), "Run a complete episode");
    
    // Advanced utility functions
    m.def("benchmarkPerformance", [](std::shared_ptr<nlm::Brain> brain, int iterations) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            brain->step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return duration.count() / 1000.0; // Return seconds
    }, py::arg("brain"), py::arg("iterations"), "Benchmark brain performance");
    
    // Validation and testing utilities
    m.def("validateConfiguration", [](std::shared_ptr<nlm::Config> config) {
        std::vector<std::string> errors;
        if (!config->has("brain/neurons")) errors.push_back("Missing neurons configuration");
        if (!config->has("brain/regions")) errors.push_back("Missing regions configuration");
        if (config->getOr<int>("brain/neurons", 0) <= 0) errors.push_back("Invalid neuron count");
        return errors;
    }, py::arg("config"), "Validate configuration");
    
    // NumPy integration utilities
#ifdef HAVE_NUMPY
    m.def("to_numpy", &pybind11::to_numpy<float>, "Convert vector to NumPy array");
    m.def("from_numpy", &pybind11::from_numpy<float>, "Convert NumPy array to vector");
    
    // Add numpy array conversion methods to relevant classes
    vision_class.def("to_numpy", [](nlm::Vision& self) {
        return pybind11::to_numpy(self.getData());
    }, "Convert vision data to NumPy array");
    
    audio_class.def("to_numpy", [](nlm::Audio& self) {
        return pybind11::to_numpy(self.getData());
    }, "Convert audio data to NumPy array");
#endif
    
    // ========= CONSTANTS =========
    
    m.attr("INVALID_NEURON_ID") = py::cast(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(nlm::INVALID_POPULATION_ID);
    
    // Simulation constants
    m.attr("DEFAULT_TIMESTEP") = 0.1;
    m.attr("DEFAULT_SIMULATION_STEPS") = 1000;
    
    // Development constants
    m.attr("INITIAL_DEVELOPMENT_STAGES") = 3;
    m.attr("CRITICAL_PERIOD_MULTIPLIER") = 2.0f;
    
    // Memory constants
    m.attr("WORKING_MEMORY_CAPACITY") = 100;
    m.attr("WORKING_MEMORY_DECAY_RATE") = 0.01f;
    
    // Neuromodulation constants
    m.attr("MAX_DOPAMINE_LEVEL") = 1.0f;
    m.attr("MAX_CURIOUSITY_LEVEL") = 1.0f;
    m.attr("MAX_NOVELTY_LEVEL") = 1.0f;
}

// Factory function implementations
std::shared_ptr<nlm::Config> create_default_config() {
    return std::make_shared<nlm::Config>();
}

std::shared_ptr<nlm::Brain> create_brain(std::shared_ptr<nlm::Config> config) {
    return std::make_shared<nlm::Brain>(config);
}

std::shared_ptr<nlm::SimpleWorld> create_simple_world() {
    return std::make_shared<nlm::SimpleWorld>();
}

std::shared_ptr<nlm::AgentBrain> create_agent_brain(std::shared_ptr<nlm::Brain> brain) {
    return std::make_shared<nlm::AgentBrain>(brain);
}