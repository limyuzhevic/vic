#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <typeinfo>

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

// Custom Python exceptions for NLM
class PyNLMError : public std::runtime_error {
public:
    explicit PyNLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class PyConfigurationError : public PyNLMError {
public:
    explicit PyConfigurationError(const std::string& msg) : PyNLMError(msg) {}
};

class PyBrainError : public PyNLMError {
public:
    explicit PyBrainError(const std::string& msg) : PyNLMError(msg) {}
};

class PyWorldError : public PyNLMError {
public:
    explicit PyWorldError(const std::string& msg) : PyNLMError(msg) {}
};

// Helper function to validate neuron pointer
Neuron* validateNeuronPointer(std::shared_ptr<Neuron> neuron_ptr, const std::string& operation_name) {
    if (!neuron_ptr) {
        throw PyBrainError("Neuron pointer is null in " + operation_name);
    }
    return neuron_ptr.get();
}

// Python wrapper for AgentBrain initialization with validation
void initializeWithValidation(AgentBrain& agent, const SimpleWorld& world) {
    if (!agent.isValid()) {
        throw PyBrainError("AgentBrain is not properly initialized or is invalid");
    }
    agent.initialize(world);
}

// Helper function to safely get sensory input size
size_t getSensoryInputSizeSafe(const AgentBrain& agent) {
    if (!agent.isValid()) {
        throw PyBrainError("Cannot get sensory input size: AgentBrain is not valid");
    }
    return agent.getSensoryInputSize();
}

// Helper function to safely decode motor command
MotorCommand decodeMotorCommandSafe(AgentBrain& agent) {
    if (!agent.isValid()) {
        throw PyBrainError("Cannot decode motor command: AgentBrain is not valid");
    }
    return agent.decodeMotorCommand();
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key improvements:
        - Enhanced error handling with custom exceptions
        - Pythonic convenience methods and validation
        - Type safety and parameter validation
        - Better documentation and examples
        - Memory safety with proper pointer handling
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const PyNLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // NeuronId class with enhanced validation
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
        .def("is_valid", &NeuronId::is_valid, R"pbdoc(Check if NeuronId is valid)pbdoc");

    // SynapseId class with enhanced validation
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
        .def("is_valid", &SynapseId::is_valid, R"pbdoc(Check if SynapseId is valid)pbdoc");

    // RegionId class with enhanced validation
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
        .def("is_valid", &RegionId::is_valid, R"pbdoc(Check if RegionId is valid)pbdoc");

    // PopulationId class with enhanced validation
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

    // Enhanced Config class with Pythonic interface
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
        // Pythonic convenience methods
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value (Pythonic alias for set)")
        .def("get", &Config::get, py::arg("key"), py::arg("default"),
             "Get a configuration value with default")
        .def("get_int", &Config::get<int>, py::arg("key"), py::arg("default"),
             "Get an integer configuration value")
        .def("get_float", &Config::get<float>, py::arg("key"), py::arg("default"),
             "Get a float configuration value")
        .def("get_bool", &Config::get<bool>, py::arg("key"), py::arg("default"),
             "Get a boolean configuration value")
        .def("merge", &Config::merge, py::arg("other"),
             "Merge another configuration into this one");

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("is_empty", &SensoryInput::isEmpty, R"pbdoc(Check if sensory input is empty)pbdoc");

    // Enhanced Vision class
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
        .def("setResolution", &Vision::setResolution, py::arg("width"), py::arg("height"),
             "Set vision resolution")
        .def("getResolution", &Vision::getResolution, R"pbdoc(Get vision resolution (width, height))pbdoc");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("setNumSamples", &Audio::setNumSamples, py::arg("numSamples"),
             "Set number of audio samples");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount,
             R"pbdoc(Get number of internal signals)pbdoc")
        .def("getSignal", &InternalSignals::getSignal, py::arg("index"),
             R"pbdoc(Get internal signal at index)");

    // Enhanced Action class
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
        .def("is_valid", &Action::isValid, R"pbdoc(Check if action is valid)pbdoc")
        .def("to_dict", [](const Action& action) {
            return std::map<std::string, py::object>{{"type", py::cast(action.getType())}};
        }, R"pbdoc(Convert action to dictionary representation)pbdoc");

    // Enhanced WorldObject class
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
        .def("is_valid", &WorldObject::isValid, R"pbdoc(Check if world object is valid)pbdoc")
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            return std::hypot(obj.x - x, obj.y - y);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Calculate distance to position (x, y))pbdoc");

    // Enhanced AgentBody class
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
        .def("distance_to", [](const AgentBody& body, float x, float y) {
            return std::hypot(body.x - x, body.y - y);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Calculate distance to position (x, y))pbdoc")
        .def("is_near", [](const AgentBody& body, float x, float y, float threshold) {
            return std::hypot(body.x - x, body.y - y) <= threshold;
        }, py::arg("x"), py::arg("y"), py::arg("threshold"),
           R"pbdoc(Check if agent is within threshold distance of position)pbdoc");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_success", &ActionResult::isSuccess, R"pbdoc(Check if action was successful)pbdoc")
        .def("to_dict", [](const ActionResult& result) {
            return std::map<std::string, py::object>{{"reward", py::cast(result.reward)},
                                                   {"success", py::cast(result.success)},
                                                   {"message", py::cast(result.message)}};
        }, R"pbdoc(Convert action result to dictionary representation)pbdoc");

    // Enhanced SensoryPercept class
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
        .def("is_valid", &SensoryPercept::isValid, R"pbdoc(Check if sensory percept is valid)pbdoc")
        .def("normalize", &SensoryPercept::normalize, R"pbdoc(Normalize all sensory signals)")
        .def("scale", [](SensoryPercept& percept, float factor) {
            auto vision = percept.getVision();
            auto touch = percept.getTouch();
            auto internal = percept.getInternal();
            auto proprioception = percept.getProprioception();
            auto audio = percept.getAudio();
            
            std::transform(vision.begin(), vision.end(), vision.begin(), 
                          [factor](float v) { return v * factor; });
            std::transform(touch.begin(), touch.end(), touch.begin(),
                          [factor](float v) { return v * factor; });
            std::transform(internal.begin(), internal.end(), internal.begin(),
                          [factor](float v) { return v * factor; });
            std::transform(proprioception.begin(), proprioception.end(), proprioception.begin(),
                          [factor](float v) { return v * factor; });
            std::transform(audio.begin(), audio.end(), audio.begin(),
                          [factor](float v) { return v * factor; });
            
            percept.setVision(vision);
            percept.setTouch(touch);
            percept.setInternal(internal);
            percept.setProprioception(proprioception);
            percept.setAudio(audio);
        }, py::arg("factor"), R"pbdoc(Scale all sensory signals by factor)");

    // Enhanced SimpleWorld class
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
        // Pythonic convenience methods
        .def("is_within_bounds", [](const SimpleWorld& world, float x, float y) {
            return world.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Check if position (x, y) is within world bounds)pbdoc")
        .def("get_distance_to_agent", [](const SimpleWorld& world, float x, float y) {
            auto body = world.getAgentBody();
            return std::hypot(body.x - x, body.y - y);
        }, py::arg("x"), py::arg("y"),
           R"pbdoc(Calculate distance from position to agent)pbdoc")
        .def("add_resource", [](SimpleWorld& world, float x, float y, float value) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Resource;
            obj.value = value;
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value"),
           R"pbdoc(Add a resource object at position)pbdoc")
        .def("add_wall", [](SimpleWorld& world, float x, float y) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Wall;
            obj.radius = 0.5f;
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Add a wall at position)pbdoc");

    // Enhanced Brain class
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
             "Log brain status")
        // Pythonic convenience methods
        .def("is_initialized", &Brain::isInitialized,
             R"pbdoc(Check if brain is initialized)pbdoc")
        .def("get_brain_stats", [](Brain& brain) {
            return std::map<std::string, py::object>{{"neuron_count", py::cast(brain.getTotalNeuronCount())},
                                                   {"synapse_count", py::cast(brain.getTotalSynapseCount())},
                                                   {"firing_count", py::cast(brain.getFiringNeuronCount())},
                                                   {"avg_firing_rate", py::cast(brain.getAverageFiringRate())},
                                                   {"stage", py::cast(brain.getDevelopmentalStage())}};
        }, R"pbdoc(Get brain statistics as dictionary)pbdoc")
        .def("run_simulation", [](Brain& brain, int steps, double timestep = 1.0) {
            for (int i = 0; i < steps; ++i) {
                brain.step(i, i * timestep);
            }
        }, py::arg("steps"), py::arg("timestep") = 1.0,
           R"pbdoc(Run brain simulation for given number of steps)pbdoc");

    // Enhanced AgentBrain class with validation and Pythonic interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &initializeWithValidation, py::arg("world"),
             "Initialize with world (with validation)")
        .def("getSensoryInputSize", &getSensoryInputSizeSafe,
             "Get expected sensory input size (with validation)")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &decodeMotorCommandSafe,
             "Decode brain motor activity into motor command (with validation)")
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
        // Configuration methods
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
        // Pythonic convenience methods
        .def("set_sensor_novelty_decay", &AgentBrain::setSensorNoveltyDecay,
             py::arg("decay"), R"pbdoc(Set sensor novelty decay rate)pbdoc")
        .def("get_sensor_novelty_decay", &AgentBrain::getSensorNoveltyDecay,
             R"pbdoc(Get sensor novelty decay rate)pbdoc")
        // Debug and monitoring utilities
        .def("get_total_motor_neurons", &AgentBrain::getTotalMotorNeurons,
             R"pbdoc(Get total number of motor neurons)pbdoc")
        .def("get_total_sensory_neurons", &AgentBrain::getTotalSensoryNeurons,
             R"pbdoc(Get total number of sensory neurons)pbdoc")
        .def("get_brain_stats", [](AgentBrain& agent) {
            return std::map<std::string, py::object>{{"motor_neurons", py::cast(agent.getTotalMotorNeurons())},
                                                   {"sensory_neurons", py::cast(agent.getTotalSensoryNeurons())},
                                                   {"curiosity", py::cast(agent.getCuriosityLevel())},
                                                   {"novelty", py::cast(agent.getNoveltyLevel())},
                                                   {"dev_stage", py::cast(agent.getDevelopmentalStage())}};
        }, R"pbdoc(Get agent statistics as dictionary)pbdoc")
        .def("is_valid", &AgentBrain::isValid,
             R"pbdoc(Check if AgentBrain is valid and ready for use)pbdoc")
        .def("is_functional", &AgentBrain::isValid,
             R"pbdoc(Check if AgentBrain is functional)pbdoc");

    // Factory functions with validation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw PyConfigurationError("Cannot create brain with null config");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw PyBrainError("Cannot create AgentBrain with null brain");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience factory functions
    m.def("create_simple_simulation", [](int neuron_count = 1000) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neuron_count);
        return std::make_tuple(std::make_shared<Brain>(config),
                               std::make_shared<SimpleWorld>());
    }, py::arg("neuron_count") = 1000,
       R"pbdoc(Create a simple simulation with brain and world)pbdoc");

    // Global attributes
    // Utility functions for simulation management
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, int steps, double timestep = 1.0) {
        if (!brain) {
            throw PyBrainError("Cannot run simulation with null brain");
        }
        brain->initialize();
        for (int i = 0; i < steps; ++i) {
            brain->step(i, i * timestep);
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 1.0,
       R"pbdoc(Run a brain simulation for specified number of steps)pbdoc");

    m.def("create_enhanced_agent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                    bool enable_learning = true, bool enable_curiosity = true,
                                    bool enable_development = true) -> std::shared_ptr<AgentBrain> {
        if (!brain || !world) {
            throw PyBrainError("Cannot create enhanced agent with null brain or world");
        }
        
        auto agent = std::make_shared<AgentBrain>(brain);
        
        if (!agent->isValid()) {
            throw PyBrainError("Created invalid agent");
        }
        
        agent->initialize(*world);
        
        if (enable_learning) {
            agent->enableRewardModulation(true);
        }
        if (enable_curiosity) {
            agent->enableCuriosity(true);
        }
        if (enable_development) {
            agent->enableDevelopment(true);
        }
        
        return agent;
    }, py::arg("brain"), py::arg("world"), py::arg("enable_learning") = true,
       py::arg("enable_curiosity") = true, py::arg("enable_development") = true,
       R"pbdoc(Create an enhanced agent with specified features enabled)pbdoc");

    m.def("simulate_agent_episode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world,
                                      int steps, double timestep = 0.1, bool validate_inputs = true) {
        if (!agent || !world) {
            throw PyBrainError("Cannot simulate with null agent or world");
        }
        
        if (!agent->isValid()) {
            throw PyBrainError("Cannot simulate with invalid agent");
        }
        
        int successful_steps = 0;
        for (int step = 0; step < steps; ++step) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            
            if (validate_inputs && !percept.isValid()) {
                continue; // Skip invalid percepts
            }
            
            agent->processSensoryInput(percept);
            auto brain = agent->getBrain();
            if (brain) {
                brain->step(step, step * timestep);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Apply reward modulation
                float reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0;
                agent->applyRewardModulation(reward, 0.0);
                
                successful_steps++;
            }
            
            agent->updateDevelopment(timestep);
        }
        
        return successful_steps;
    }, py::arg("agent"), py::arg("world"), py::arg("steps"), py::arg("timestep") = 0.1,
       py::arg("validate_inputs") = true,
       R"pbdoc(Run an agent simulation episode with specified number of steps)pbdoc");

    m.def("compare_agents", [](const std::vector<std::shared_ptr<AgentBrain>>& agents,
                               std::shared_ptr<SimpleWorld> world, int steps) {
        if (agents.empty()) {
            throw PyBrainError("Cannot compare empty list of agents");
        }
        
        if (!world) {
            throw PyBrainError("Cannot compare agents with null world");
        }
        
        std::vector<double> final_curiosity;
        std::vector<int> total_spikes;
        std::vector<double> average_rewards;
        
        for (auto& agent : agents) {
            if (!agent->isValid()) {
                throw PyBrainError("Invalid agent in comparison");
            }
            
            world->reset();
            agent->reset();
            
            double curiosity_sum = 0.0;
            int spikes = 0;
            double reward_sum = 0.0;
            
            for (int step = 0; step < steps; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                
                if (percept.isValid()) {
                    agent->processSensoryInput(percept);
                    auto brain = agent->getBrain();
                    if (brain) {
                        brain->step(step);
                        spikes += brain->getTotalSpikeCount();
                        
                        auto action = agent->decodeMotorCommand();
                        world->applyMotorCommand(action, world->getSimulationTime());
                        
                        float reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0;
                        agent->applyRewardModulation(reward, 0.0);
                        reward_sum += reward;
                    }
                }
                
                curiosity_sum += agent->getCuriosityLevel();
                agent->updateDevelopment(0.1);
            }
            
            final_curiosity.push_back(agent->getCuriosityLevel());
            total_spikes.push_back(spikes);
            average_rewards.push_back(reward_sum / steps);
        }
        
        return std::make_tuple(final_curiosity, total_spikes, average_rewards);
    }, py::arg("agents"), py::arg("world"), py::arg("steps"),
       R"pbdoc(Compare multiple agents side by side)pbdoc");

    m.def("create_test_scenarios", []() {
        auto scenarios = std::make_shared<std::vector<std::string>>();
        
        // Add common test scenarios
        scenarios->push_back("simple_brain_simulation");
        scenarios->push_back("agent_world_interaction");
        scenarios->push_back("learning_with_reward");
        scenarios->push_back("development_stages");
        scenarios->push_back("curiosity_driven_exploration");
        scenarios->push_back("novelty_detection");
        scenarios->push_back("structural_plasticity");
        scenarios->push_back("multi_agent_comparison");
        
        return scenarios;
    }, R"pbdoc(Get list of available test scenarios)pbdoc");

    m.def("get_agent_statistics", [](const std::vector<std::shared_ptr<AgentBrain>>& agents) {
        if (agents.empty()) {
            return std::make_tuple(std::vector<double>(), std::vector<int>(), std::vector<double>());
        }
        
        std::vector<double> curiosities;
        std::vector<int> spike_counts;
        std::vector<double> rewards;
        
        for (auto& agent : agents) {
            if (agent->isValid()) {
                curiosities.push_back(agent->getCuriosityLevel());
                spike_counts.push_back(agent->getBrain()->getTotalSpikeCount());
                rewards.push_back(agent->getBrain()->getAverageFiringRate());
            }
        }
        
        return std::make_tuple(curiosities, spike_counts, rewards);
    }, py::arg("agents"),
       R"pbdoc(Get statistics from multiple agents)pbdoc");

    m.def("validate_all_agents", [](const std::vector<std::shared_ptr<AgentBrain>>& agents) {
        for (auto& agent : agents) {
            if (!agent->isValid()) {
                all_valid = false;
                break;
            }
        }
        
        return all_valid;
    }, py::arg("agents"),
       R"pbdoc(Check if all agents are valid)pbdoc");

    m.def("reset_all_agents", [](std::vector<std::shared_ptr<AgentBrain>>& agents) {
        for (auto& agent : agents) {
            if (agent && agent->isValid()) {
                agent->reset();
            }
        }
    }, py::arg("agents"),
       R"pbdoc(Reset all agents)pbdoc");

    m.def("example_advanced_simulation", []() {
        py::print("NLM Advanced Example - Complete Agent Simulation");
        py::print("==============================================");
        py::print("""
This example demonstrates the enhanced NLM features:

1. Advanced AgentBrain with memory safety
2. Enhanced validation and error handling
3. Statistical analysis of simulation results
4. Multiple learning subsystems
5. Debugging and monitoring utilities

Example code:
```python
import pynlm

# Create enhanced agent with all features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)

# Create agent with enhanced features
agent = pynlm.create_enhanced_agent(brain, world, 
                                    enable_learning=True,
                                    enable_curiosity=True, 
                                    enable_development=True)

# Run enhanced simulation
steps = agent.simulate_agent_episode(500, timestep=0.1)

# Get statistics
stats = agent.get_brain_stats()
print(f"Simulation complete! Steps: {steps}")
print(f"Statistics: {stats}")
```

The enhanced NLM provides:
✓ Better memory management with smart pointers
✓ Comprehensive validation and error handling
✓ Advanced debugging utilities
✓ Pythonic convenience methods
✓ Enhanced documentation and examples
        """);
    }, R"pbdoc(Show advanced simulation example)pbdoc");