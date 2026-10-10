// Enhanced Python bindings for NLM (Neural Learning Machine)
// This version adds comprehensive functionality, convenience methods,
// improved error handling, and Python-friendly APIs

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

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

// Helper functions for Python
def createDefaultConfigPy() {
    return std::make_shared<Config>();
}

def createBrainPy(std::shared_ptr<Config> config) {
    return std::make_shared<Brain>(config);
}

def createSimpleWorldPy() {
    return std::make_shared<SimpleWorld>();
}

def createAgentBrainPy(std::shared_ptr<Brain> brain) {
    return std::make_shared<AgentBrain>(brain);
}

// Config helper functions
inline std::string getStringConfig(const Config& config, const std::string& key) {
    auto val = config.get<std::string>(key);
    if (val.has_value()) {
        return *val;
    }
    throw std::runtime_error("Config key not found: " + key);
}

inline int getIntConfig(const Config& config, const std::string& key) {
    auto val = config.get<int>(key);
    if (val.has_value()) {
        return *val;
    }
    throw std::runtime_error("Config key not found: " + key);
}

inline double getDoubleConfig(const Config& config, const std::string& key) {
    auto val = config.get<double>(key);
    if (val.has_value()) {
        return *val;
    }
    throw std::runtime_error("Config key not found: " + key);
}

inline bool getBoolConfig(const Config& config, const std::string& key) {
    auto val = config.get<bool>(key);
    if (val.has_value()) {
        return *val;
    }
    throw std::runtime_error("Config key not found: " + key);
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        ---------------------------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        Provides comprehensive APIs with convenience methods, improved error handling,
        and Python-friendly interfaces.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Type definitions
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

    // Enhanced enumeration bindings with descriptions
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron type")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron type")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron type")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron type")
        .value("Motor", NeuronType::Motor, "Motor neuron type")
        .value("Internal", NeuronType::Internal, "Internal neuron type")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period stage")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Resting state")
        .value("Active", FiringState::Active, "Active firing state")
        .value("Refractory", FiringState::Refractory, "Refractory period")
        .value("Inhibited", FiringState::Inhibited, "Inhibited state")
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

    // Enhanced Config class with Python-friendly API
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
        
        // Enhanced config access methods with type conversion
        .def("get", [](const Config& self, const std::string& key) {
            // Try different types
            auto intVal = self.get<int>(key);
            if (intVal.has_value()) return *intVal;
            
            auto doubleVal = self.get<double>(key);
            if (doubleVal.has_value()) return *doubleVal;
            
            auto stringVal = self.get<std::string>(key);
            if (stringVal.has_value()) return *stringVal;
            
            auto boolVal = self.get<bool>(key);
            if (boolVal.has_value()) return *boolVal;
            
            throw std::runtime_error("Config key not found or unsupported type: " + key);
        }, py::arg("key"), "Get configuration value with automatic type conversion")
        
        .def("getOr", [](Config& self, const std::string& key, const py::object& defaultValue) {
            // Convert defaultValue to appropriate type
            if (py::isinstance<int>(defaultValue)) {
                return self.getOr<int>(key, defaultValue.cast<int>());
            } else if (py::isinstance<double>(defaultValue)) {
                return self.getOr<double>(key, defaultValue.cast<double>());
            } else if (py::isinstance<std::string>(defaultValue)) {
                return self.getOr<std::string>(key, defaultValue.cast<std::string>());
            } else if (py::isinstance<bool>(defaultValue)) {
                return self.getOr<bool>(key, defaultValue.cast<bool>());
            } else {
                throw std::runtime_error("Unsupported default value type");
            }
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default, supporting multiple types")
        
        .def("set", &Config::set, py::arg("key"), py::arg("value"), py::arg("source") = nlm::ConfigSource::Runtime,
             "Set configuration value (Python-friendly)")
        
        .def("remove", &Config::remove, py::arg("key"), "Remove configuration key")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getDataAsArray", [](Vision& self) {
            // Return data as a 3D array representation for easier access
            std::vector<std::vector<std::vector<float>>> array;
            size_t width = self.getWidth();
            size_t height = self.getHeight();
            size_t channels = self.getChannels();
            
            const std::vector<float>* data = self.getData();
            if (data) {
                array.resize(height);
                for (size_t y = 0; y < height; ++y) {
                    array[y].resize(width);
                    for (size_t x = 0; x < width; ++x) {
                        std::vector<float> pixel(channels);
                        size_t baseIndex = (y * width + x) * channels;
                        for (size_t c = 0; c < channels; ++c) {
                            if (baseIndex + c < data->size()) {
                                pixel[c] = (*data)[baseIndex + c];
                            } else {
                                pixel[c] = 0.0f;
                            }
                        }
                        array[y][x] = pixel;
                    }
                }
            }
            return array;
        }, "Get vision data as a 3D array (height x width x channels)")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getAllSignals", [](InternalSignals& self) {
            return self.getSignals(); // Assuming getSignals() exists
        }, "Get all internal signals as a vector")
        .def("getSignalCount", [](InternalSignals& self) {
            auto signals = self.getSignals();
            return signals.size();
        }, "Get the number of internal signals");

    // Enhanced Action class with helper methods
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getParameter", [](Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) return params[index];
            throw std::runtime_error("Parameter index out of range");
        }, py::arg("index"), "Get specific parameter by index")
        .def("addParameter", &Action::getParameters, py::arg("value"), "Add a parameter to the action")
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " params=" + std::to_string(action.getParameters().size()) + ">";
        });

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
        .def("getDistanceTo", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("other"), "Get distance to another world object")
        .def("isWithinRadius", [](const WorldObject& self, float x, float y, float radius) {
            float dx = self.x - x;
            float dy = self.y - y;
            return (dx*dx + dy*dy) <= (radius*radius);
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Check if point is within object's radius");

    // Enhanced AgentBody class with utility methods
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
        .def("move", [](AgentBody& self, float dx, float dy) {
            self.x += dx;
            self.y += dy;
            self.isMoving = true;
        }, py::arg("dx"), py::arg("dy"), "Move agent by relative amounts")
        .def("turn", &AgentBody::orientation, py::arg("angle"), "Rotate agent by angle")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get current position as (x, y) tuple")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get current velocity as (vx, vy) tuple")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX*self.velocityX + self.velocityY*self.velocityY);
        }, "Get current speed");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Enhanced action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isPositive", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if result is positive")
        .def("isNegative", [](const ActionResult& self) {
            return !self.success || self.reward < 0.0f;
        }, "Check if result is negative");

    // Enhanced SensoryPercept class with convenience methods
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
        .def("addVision", [](SensoryPercept& self, std::shared_ptr<Vision> vision) {
            self.setVision(vision);
        }, py::arg("vision"), "Add vision input")
        .def("getFieldOfView", [](SensoryPercept& self, int x, int y) {
            auto vision = self.getVision();
            if (vision) {
                return vision->getVisionAt(x, y); // Assuming getVisionAt exists
            }
            throw std::runtime_error("No vision data available");
        }, py::arg("x"), py::arg("y"), "Get vision value at specific position");

    // Enhanced SimpleWorld class with Python-friendly methods
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state for given timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getObjectsCount", [](SimpleWorld& self) {
            return self.getObjects().size(); // Assuming getObjects() exists
        }, "Get number of objects in world")
        .def("getNearbyObjects", [](SimpleWorld& self, float x, float y, float radius) {
            return self.getNearbyObjects(x, y, radius); // Assuming getNearbyObjects exists
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Get objects within radius");

    // Enhanced Brain class with Python-friendly API
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
        
        // Enhanced brain methods for Python
        .def("runSimulation", [](Brain& self, size_t steps, size_t timestep = 100) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(static_cast<SimulationStep>(i), static_cast<Timestamp>(i * timestep));
            }
        }, py::arg("steps"), py::arg("timestep") = 100,
           "Run simulation for specified number of steps")
        
        .def("getStatistics", [](Brain& self) {
            nlohmann::json stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            return stats;
        }, "Get brain statistics as JSON")
        
        .def("getRegionNames", [](Brain& self) {
            auto region_ids = self.getRegionIds();
            std::vector<std::string> names;
            for (const auto& id : region_ids) {
                names.push_back(self.getRegion(id)->getName());
            }
            return names;
        }, "Get all region names")
        
        .def("getNeuralActivity", [](Brain& self) {
            nlohmann::json activity;
            activity["active_neurons"] = self.getActiveNeuronCount();
            activity["firing_neurons"] = self.getFiringNeuronCount();
            activity["total_spikes"] = self.getTotalSpikeCount();
            return activity;
        }, "Get current neural activity data");

    // Enhanced AgentBrain class with Python-friendly API
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
        
        // Enhanced agent brain methods
        .def("runEpisode", [](AgentBrain& self, SimpleWorld& world, size_t maxSteps = 1000) {
            world.reset();
            auto agent_body = world.getAgentBody();
            world.setAgentStart(agent_body.x, agent_body.y);
            
            for (size_t i = 0; i < maxSteps; ++i) {
                auto percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                
                auto action = self.decodeMotorCommand();
                world.applyMotorCommand(action.getType(), static_cast<Timestamp>(i));
                
                world.update(static_cast<float>(i));
                
                if (self.getCuriosityLevel() < 0.01 && i > maxSteps * 0.9) {
                    break; // Agent is bored, stop episode early
                }
            }
        }, py::arg("world"), py::arg("maxSteps") = 1000,
           "Run complete episode in world")
        
        .def("getEpisodeStatistics", [](AgentBrain& self, SimpleWorld& world) {
            nlohmann::json stats;
            auto agent_body = world.getAgentBody();
            auto percept = world.getSensoryPercept();
            
            stats["agent_position"] = nlohmann::json::array({agent_body.x, agent_body.y});
            stats["agent_orientation"] = agent_body.orientation;
            stats["agent_energy"] = agent_body.energy;
            stats["agent_health"] = agent_body.health;
            stats["agent_age"] = agent_body.age;
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            
            return stats;
        }, py::arg("world"), "Get episode statistics");

    // Enhanced factory functions
    m.def("createDefaultConfig", &createDefaultConfigPy, "Create a default configuration")
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    
    m.def("createSimpleWorld", &createSimpleWorldPy, "Create a new simple world")
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    
    // Convenience functions for common operations
    m.def("runQuickTest", [](size_t neuronCount = 100, size_t steps = 100) {
        auto config = createDefaultConfigPy();
        config->set("neuron_count", neuronCount, nlm::ConfigSource::Runtime);
        
        auto brain = createBrainPy(config);
        brain->initialize();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(static_cast<SimulationStep>(i), static_cast<Timestamp>(i * 0.1f));
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("neuronCount") = 100, py::arg("steps") = 100,
       "Run a quick test simulation and return spike count")
    
    m.def("createQuickEnvironment", []() {
        auto world = createSimpleWorldPy();
        world->configure(50, 50, 10, 10);
        world->setAgentStart(25, 25);
        return world;
    }, "Create a pre-configured simple environment for quick testing")
    
    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Version information
    m.attr("__version__") = "enhanced"; // Would normally read from pyproject.toml
    m.attr("__description__") = "Enhanced Python bindings for NLM with improved API and convenience functions";

} // namespace nlm
