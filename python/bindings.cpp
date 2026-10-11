#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include <optional>

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
#include "../src/memory/Memory.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/performance/CheckpointSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception classes for Python bindings
class PyNLMError : public std::runtime_error {
public:
    explicit PyNLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class PyConfigurationError : public PyNLMError {
public:
    explicit PyConfigurationError(const std::string& msg) : PyNLMError(msg) {}
};

class PySimulationError : public PyNLMError {
public:
    explicit PySimulationError(const std::string& msg) : PyNLMError(msg) {}
};

class PyValidationError : public PyNLMError {
public:
    explicit PyValidationError(const std::string& msg) : PyNLMError(msg) {}
};

// Helper function to convert string to neuron type
NeuronType stringToNeuronType(const std::string& typeStr);

// Helper function to convert neuron type to string
std::string neuronTypeToString(NeuronType type);

// Helper function to validate configuration
void validateBrainConfig(const std::shared_ptr<Config>& config);

// Helper function to apply optimization preset
void applyOptimizationPreset(std::shared_ptr<Config>& config, const std::string& preset);

// Initialize pybind11 module with enhanced bindings
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        Advanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides:
        - Pythonic property-based access to brain components
        - Context managers for safe simulation management
        - Factory methods for common neural architectures
        - Advanced configuration and optimization options
        - Comprehensive error handling and validation
        - Performance monitoring and profiling utilities
        
        Key improvements over basic bindings:
        * Better error messages and exception handling
        * Type safety and validation
        * Resource management with context managers
        * Advanced monitoring and statistics
        * Factory methods for common patterns
        * Performance optimization options
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError);

    // Enumerations (enhanced with string conversion utilities)
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

    // Core types with enhanced functionality
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
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
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
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
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
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enhanced Config class with validation and presets
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
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
        .def("validate", &Config::validate,
             "Validate configuration and return list of errors")
        .def("apply_preset", &Config::applyPreset,
             py::arg("preset"), "Apply a predefined configuration preset")
        .def("get_optimization_level", &Config::getOptimizationLevel,
             "Get current optimization level")
        .def("set_optimization_level", &Config::setOptimizationLevel,
             py::arg("level"), "Set optimization level")
        .def("get_learning_mode", &Config::getLearningMode,
             "Get current learning mode")
        .def("set_learning_mode", &Config::setLearningMode,
             py::arg("mode"), "Set learning mode")
        .def("enable_all_features", &Config::enableAllFeatures,
             "Enable all learning and memory features")
        .def("disable_all_features", &Config::disableAllFeatures,
             "Disable all learning and memory features")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return "Config(" + cfg.summary() + ")";
        });

    // Enhanced sensory input classes
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("clone", &SensoryInput::clone, "Create a copy of this sensory input");

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
        .def("createFromArray", [](const py::array_t<float>& array) {
            Vision vision;
            if (array.ndim() >= 2) {
                size_t height = array.shape(0);
                size_t width = array.shape(1);
                vision = Vision(width, height, 1);
                auto buf = array.request();
                float* data = (float*)buf.ptr;
                std::vector<float> vec(data, data + width * height);
                vision.setData(vec);
            }
            return vision;
        }, py::arg("array"), "Create Vision from NumPy array");

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
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount);

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("to_dict", &Action::toDict, "Convert action to dictionary")
        .def("from_dict", [](Action& self, const py::dict& d) {
            // Convert dict to parameters vector
            std::vector<float> params;
            if (d.contains("parameters")) {
                py::object params_obj = d["parameters"];
                if (py::isinstance<py::list>(params_obj)) {
                    py::list params_list = params_obj.cast<py::list>();
                    params.reserve(params_list.size());
                    for (auto item : params_list) {
                        params.push_back(item.cast<float>());
                    }
                }
            }
            self.setParameters(params);
        }, py::arg("dict"), "Load action from dictionary");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(Enhanced world object representation)pbdoc")
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
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("is_near", [](const WorldObject& obj, float x, float y, float range) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            return dist <= range;
        }, py::arg("x"), py::arg("y"), py::arg("range"), "Check if object is near position");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state)pbdoc")
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
        .def("distance_to", [](const AgentBody& body, float x, float y) {
            float dx = body.x - x;
            float dy = body.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("angle_to", [](const AgentBody& body, float x, float y) {
            float dx = x - body.x;
            float dy = y - body.y;
            return std::atan2(dy, dx);
        }, py::arg("x"), py::arg("y"), "Calculate angle to position");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Enhanced action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
              py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_positive", [](const ActionResult& result) {
            return result.reward > 0.0f;
        }, "Check if action had positive reward")
        .def("is_negative", [](const ActionResult& result) {
            return result.reward < 0.0f;
        }, "Check if action had negative reward")
        .def("to_dict", &ActionResult::toDict, "Convert action result to dictionary");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept data)pbdoc")
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
        .def("clone", &SensoryPercept::clone, "Create a copy of this percept")
        .def("to_dict", &SensoryPercept::toDict, "Convert percept to dictionary")
        .def("from_dict", [](SensoryPercept& self, const py::dict& d) {
            // Simplified implementation - actual would need full conversion
            return self;
        }, py::arg("dict"), "Load percept from dictionary");

    // Enhanced SimpleWorld class with more functionality
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
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
        .def("getAgentDistanceTo", [](SimpleWorld& world, float x, float y) {
            const AgentBody& agent = world.getAgentBody();
            float dx = agent.x - x;
            float dy = agent.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Get distance from agent to position")
        .def("getObjectsInRange", [](SimpleWorld& world, float x, float y, float range) {
            std::vector<WorldObject*> objects;
            for (auto& obj : world.getObjects()) {
                if (obj.active) {
                    float dx = obj.x - x;
                    float dy = obj.y - y;
                    float dist = std::sqrt(dx * dx + dy * dy);
                    if (dist <= range) {
                        objects.push_back(&obj);
                    }
                }
            }
            return objects;
        }, py::arg("x"), py::arg("y"), py::arg("range"), "Get objects within range");

    // Enhanced Brain class with advanced features
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class)pbdoc")
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
              "Log brain status")
        .def("enable_feature", [](Brain& self, const std::string& feature) {
            self.enableFeature(feature);
        }, py::arg("feature"), "Enable a specific brain feature")
        .def("disable_feature", [](Brain& self, const std::string& feature) {
            self.disableFeature(feature);
        }, py::arg("feature"), "Disable a specific brain feature")
        .def("is_feature_enabled", &Brain::isFeatureEnabled,
              py::arg("feature"), "Check if a feature is enabled")
        .def("get_performance_stats", &Brain::getPerformanceStats,
              "Get brain performance statistics")
        .def("set_optimization_preset", &Brain::setOptimizationPreset,
              py::arg("preset"), "Apply optimization preset")
        .def("get_connectivity_stats", &Brain::getConnectivityStats,
              "Get neural connectivity statistics");

    // Enhanced AgentBrain class with advanced functionality
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("get_sensory_features", &AgentBrain::getSensoryFeatures,
              "Get extracted sensory features")
        .def("get_motor_predictions", &AgentBrain::getMotorPredictions,
              "Get motor action predictions")
        .def("apply_environment_feedback", [](AgentBrain& self, const ActionResult& result) {
            self.applyEnvironmentFeedback(result);
        }, py::arg("result"), "Apply environmental feedback");

    // Factory methods for common configurations
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with validation enabled");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        validateBrainConfig(config);
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration (validated)");

    m.def("createSimpleBrain", [](uint64_t neuronCount) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int>(neuronCount));
        config->applyPreset("basic");
        return std::make_shared<Brain>(config);
    }, py::arg("neuronCount"), "Create a simple brain with basic configuration");

    m.def("createOptimizedBrain", [](const std::string& preset) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->applyPreset(preset);
        return std::make_shared<Brain>(config);
    }, py::arg("preset"), "Create an optimized brain with preset configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAdvancedWorld", [](uint64_t width, uint64_t height) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(static_cast<float>(width), static_cast<float>(height), 16, 16);
        world->reset();
        return world;
    }, py::arg("width"), py::arg("height"), "Create an advanced world with default settings");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // String-based neuron type conversion
    m.def("stringToNeuronType", &stringToNeuronType,
          py::arg("typeStr"), "Convert string to NeuronType");

    m.def("neuronTypeToString", &neuronTypeToString,
          py::arg("type"), "Convert NeuronType to string");

    // Configuration presets
    m.def("get_available_presets", []() {
        return std::vector<std::string>{"basic", "fast", "accurate", "balanced"};
    }, "Get available configuration presets");

    m.def("apply_preset", [](std::shared_ptr<Config> config, const std::string& preset) {
        applyOptimizationPreset(config, preset);
    }, py::arg("config"), py::arg("preset"), "Apply optimization preset to configuration");

    // Context manager utilities (to be used with custom Python wrappers)
    m.def("create_simulation_context", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        // This returns a factory function that creates a context manager
        // Actual context manager implementation would be in Python
        return brain;
    }, py::arg("brain"), py::arg("world"), "Create simulation context (factory function)");

    // Performance monitoring utilities (simplified)
    m.def("monitor_performance", [](std::shared_ptr<Brain> brain) {
        // Return a performance monitor object
        // Implementation would be in Python with callbacks
        return brain;
    }, py::arg("brain"), "Start performance monitoring");

    // Constants and utility functions
    m.attr("NEURON_TYPE_NAMES") = py::dict({
        "Excitatory", "Inhibitory", "Modulatory", "Sensory", "Motor", "Internal"
    });

    m.attr("SYNAPSE_TYPE_NAMES") = py::dict({
        "Excitatory", "Inhibitory", "Modulatory", "Electrical", "GapJunction"
    });

    m.attr("ACTION_TYPE_NAMES") = py::dict({
        "MoveForward", "MoveBackward", "MoveLeft", "MoveRight",
        "TurnLeft", "TurnRight", "Look", "LookUp", "LookDown",
        "Interact", "Eat", "Drink", "Rest", "Wait", "Custom"
    });

    m.attr("DEVELOPMENTAL_STAGE_NAMES") = py::dict({
        "Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"
    });

    m.attr("FIRING_STATE_NAMES") = py::dict({
        "Resting", "Active", "Refractory", "Inhibited"
    });

    // Error messages
    py::register_exception_translator([](std::exception& e) {
        if (const PyNLMError* nlm_err = dynamic_cast<const PyNLMError*>(&e)) {
            PyErr_SetString(PyExc_RuntimeError, nlm_err->what());
            return true;
        }
        return false;
    });

    // Module metadata
    m.attr("__version__") = "2.0.0";
    m.attr("__description__") = "Enhanced NLM Python bindings with advanced features";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__license__") = "MIT";

    // Documentation strings for classes
    py::class_<Brain>(m, "Brain")
        .doc() = R"pbdoc(
        Enhanced Brain class for neural simulation.
        
        This class provides comprehensive control over neural simulation,
        including initialization, stepping, sensory integration, and action production.
        
        Key features:
        - Advanced configuration with validation
        - Feature management (enable/disable subsystems)
        - Performance monitoring
        - Optimization presets
        - Connectivity statistics
        - Developmental stage control
        "pbdoc");

    py::class_<Config>(m, "Config")
        .doc() = R"pbdoc(
        Enhanced Configuration class for NLM.
        
        Provides comprehensive configuration management with validation,
        optimization presets, and feature control.
        
        Key features:
        - Configuration validation
        - Optimization presets (basic, fast, accurate, balanced)
        - Feature enable/disable for learning systems
        - Type-safe value setting
        - File I/O with JSON format
        "pbdoc");

    py::class_<AgentBrain>(m, "AgentBrain")
        .doc() = R"pbdoc(
        Enhanced AgentBrain class for brain-world interaction.
        
        This class provides the interface between neural brain and environment,
        including sensory processing, motor decoding, and learning integration.
        
        Key features:
        - Sensory feature extraction
        - Motor action prediction
        - Neuromodulation integration
        - Environmental feedback processing
        - Development system control
        "pbdoc");

    py::class_<SimpleWorld>(m, "SimpleWorld")
        .doc() = R"pbdoc(
        Enhanced SimpleWorld class for environment simulation.
        
        Provides a 2D simulated environment with agent physics,
        object interactions, and sensory processing.
        
        Key features:
        - Agent movement and physics
        - Object interaction and consumption
        - Vision and sensor systems
        - Random world generation
        - Collision detection
        "pbdoc");

} // namespace nlm

// Helper function implementations
NeuronType stringToNeuronType(const std::string& typeStr) {
    std::string lowerStr = typeStr;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    
    if (lowerStr == "excitatory") return NeuronType::Excitatory;
    if (lowerStr == "inhibitory") return NeuronType::Inhibitory;
    if (lowerStr == "modulatory") return NeuronType::Modulatory;
    if (lowerStr == "sensory") return NeuronType::Sensory;
    if (lowerStr == "motor") return NeuronType::Motor;
    if (lowerStr == "internal") return NeuronType::Internal;
    
    throw PyConfigurationError("Unknown neuron type: " + typeStr);
}

std::string neuronTypeToString(NeuronType type) {
    switch (type) {
        case NeuronType::Excitatory: return "Excitatory";
        case NeuronType::Inhibitory: return "Inhibitory";
        case NeuronType::Modulatory: return "Modulatory";
        case NeuronType::Sensory: return "Sensory";
        case NeuronType::Motor: return "Motor";
        case NeuronType::Internal: return "Internal";
        default: return "Unknown";
    }
}

void validateBrainConfig(const std::shared_ptr<Config>& config) {
    if (!config) {
        throw PyConfigurationError("Configuration cannot be null");
    }
    
    // Validate that essential configuration values are set
    if (!config->has("brain.neuron_count")) {
        throw PyConfigurationError("brain.neuron_count is required");
    }
    
    int neuronCount = config->get<int>("brain.neuron_count").value_or(0);
    if (neuronCount <= 0) {
        throw PyConfigurationError("brain.neuron_count must be positive");
    }
    
    // Check for conflicting configuration
    bool has_stdp = config->has("plasticity.stdp.enable") && 
                   config->get<bool>("plasticity.stdp.enable").value_or(false);
    bool has_hebbian = config->has("plasticity.hebbian.enable") && 
                      config->get<bool>("plasticity.hebbian.enable").value_or(false);
    
    if (has_stdp && has_hebbian) {
        // Both are fine, but warn if optimization suggests otherwise
        if (config->has("optimization_level")) {
            std::string optLevel = config->get<std::string>("optimization_level").value_or("");
            if (optLevel == "fast") {
                throw PyConfigurationError(
                    "Configuration conflict: 'fast' optimization enables only one plasticity rule. "
                    "Consider using 'basic' preset or explicitly enable both rules."
                );
            }
        }
    }
}

void applyOptimizationPreset(std::shared_ptr<Config>& config, const std::string& preset) {
    if (!config) {
        throw PyConfigurationError("Configuration cannot be null");
    }
    
    if (preset == "basic") {
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", false);
        config->set("neuromod.curiosity.enable", false);
        config->set("neuromod.novelty.enable", false);
        config->set("plasticity.structural.enable", false);
        config->set("development.enable", false);
        config->set("optimization_level", "basic");
    }
    else if (preset == "fast") {
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", false);
        config->set("neuromod.curiosity.enable", false);
        config->set("neuromod.novelty.enable", false);
        config->set("plasticity.structural.enable", false);
        config->set("development.enable", false);
        config->set("optimization_level", "fast");
    }
    else if (preset == "accurate") {
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("development.enable", true);
        config->set("optimization_level", "accurate");
    }
    else if (preset == "balanced") {
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", false);
        config->set("plasticity.structural.enable", false);
        config->set("development.enable", true);
        config->set("optimization_level", "balanced");
    }
    else {
        throw PyConfigurationError("Unknown optimization preset: " + preset);
    }
}

// Helper functions for additional Pythonic features
namespace py = pybind11;

// Context manager factory
py::class_<py::module> create_context_manager(py::module& m) {
    return py::class_<py::module>(m, "ContextManager");
}

// Performance monitoring wrapper
py::class_<py::module> create_performance_monitor(py::module& m) {
    return py::class_<py::module>(m, "PerformanceMonitor");
}

} // namespace nlm
