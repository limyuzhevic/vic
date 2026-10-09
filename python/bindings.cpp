#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// ==============================================================================
// UTILITY FUNCTIONS AND HELPERS
// ==============================================================================

namespace detail {
    // Type conversion helpers
    template<typename T>
    T from_py(py::object obj) {
        if (py::isinstance<py::int_>(obj)) {
            return obj.cast<T>();
        } else if (py::isinstance<py::float_>(obj)) {
            return obj.cast<T>();
        } else if constexpr (std::is_same_v<T, std::string>) {
            return obj.cast<T>();
        } else {
            throw py::type_error("Cannot convert Python object to requested type");
        }
    }
    
    // Vector conversion helper
    template<typename T>
    std::vector<T> to_vector(py::object obj) {
        if (py::isinstance<py::list>(obj)) {
            return obj.cast<std::vector<T>>();
        } else if (py::isinstance<py::tuple>(obj)) {
            auto tuple = obj.cast<py::tuple>();
            std::vector<T> result;
            result.reserve(tuple.size());
            for (size_t i = 0; i < tuple.size(); ++i) {
                result.push_back(tuple[i].cast<T>());
            }
            return result;
        } else {
            throw py::type_error("Cannot convert Python object to vector");
        }
    }
    
    // Dictionary conversion helper
    template<typename K, typename V>
    std::map<K, V> to_map(py::object obj) {
        if (!py::isinstance<py::dict>(obj)) {
            throw py::type_error("Expected dict for map conversion");
        }
        auto dict = obj.cast<py::dict>();
        std::map<K, V> result;
        for (auto item : dict) {
            result[item.first.cast<K>()] = item.second.cast<V>();
        }
        return result;
    }
}

// ==============================================================================
// BASIC TYPES AND ENUMS
// ==============================================================================

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module provides an intuitive, Pythonic interface to the powerful NLM neural
        simulation framework, making it easy to create, train, and interact with artificial
        neural systems that learn from experience.
    )pbdoc";

    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ==============================================================================
    // TYPE DEFINITIONS
    // ==============================================================================

    // NeuronId
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

    // SynapseId
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

    // RegionId
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

    // PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enum
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

    // MotorCommand enum
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

    // WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ==============================================================================
    // CONFIGURATION CLASS
    // ==============================================================================

    // Config class with improved Python interface
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
        // Configuration helpers
        .def("getInt", [](const Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get integer configuration value")
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get float configuration value")
        .def("getDouble", [](const Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get double configuration value")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
            // Note: This might need to be implemented in Config class
            if (self.has(key)) {
                // Try to parse as boolean
                return defaultValue; // Placeholder - actual implementation would be more complex
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get boolean configuration value")
        // Set helpers
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set float configuration value")
        .def("setDouble", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set double configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            // Similar implementation needed
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration value");

    // ==============================================================================
    // SENSORY INPUT CLASSES
    // ==============================================================================

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput type=" + std::to_string(static_cast<int>(input.getType())) + ">";
        });

    // Vision sensory input
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
        .def("setDataFromImage", [](Vision& self, const std::vector<std::vector<std::vector<float>>>& image) {
            // Flatten 3D image to 1D vector
            std::vector<float> flatData;
            flatData.reserve(image.size() * image[0].size() * image[0][0].size());
            for (const auto& row : image) {
                for (const auto& pixel : row) {
                    flatData.insert(flatData.end(), pixel.begin(), pixel.end());
                }
            }
            self.setData(flatData);
        }, py::arg("image"))
        .def("getImageData", [](const Vision& self) {
            size_t width = self.getWidth();
            size_t height = self.getHeight();
            size_t channels = self.getChannels();
            std::vector<std::vector<std::vector<float>>> image(height, std::vector<std::vector<float>>(width, std::vector<float>(channels)));
            
            auto data = self.getData();
            size_t index = 0;
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    for (size_t c = 0; c < channels; ++c) {
                        image[y][x][c] = data[index++];
                    }
                }
            }
            return image;
        });

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getWaveform", [](const Audio& self) {
            auto data = self.getData();
            return data;
        });

    // InternalSignals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount)
        .def("getAllSignals", [](const InternalSignals& self) {
            return self.getData();
        });

    // ==============================================================================
    // ACTION CLASS
    // ==============================================================================

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
        .def("getParameter", [](const Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            throw py::index_error("Parameter index out of range");
        }, py::arg("index"), "Get parameter at index")
        .def("addParameter", &Action::addParameter, py::arg("value"), "Add a parameter value")
        .def("clearParameters", &Action::clearParameters, "Clear all parameters")
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = static_cast<int>(self.getType());
            result["parameters"] = self.getParameters();
            return result;
        }, "Convert action to dictionary")
        .def_static("from_dict", [](py::dict d) {
            Action action;
            if (d.contains("type")) {
                action.setType(static_cast<ActionType>(d["type"].cast<int>()));
            }
            if (d.contains("parameters")) {
                action.setParameters(d["parameters"].cast<std::vector<float>>());
            }
            return action;
        }, py::arg("d"), "Create action from dictionary");

    // ==============================================================================
    // WORLD OBJECTS
    // ==============================================================================

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
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("isNear", [](const WorldObject& self, float x, float y, float distance) {
            float dx = self.x - x;
            float dy = self.y - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            return dist <= distance;
        }, py::arg("x"), py::arg("y"), py::arg("distance"), "Check if object is near position")
        .def("getPosition", [](const WorldObject& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get object position as tuple")
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set object position");

    // ==============================================================================
    // AGENT BODY
    // ==============================================================================

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
        .def("getPosition", [](const AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get agent position as tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("getVelocity", [](const AgentBody& self) {
            return py::make_tuple(self.velocityX, self.velocityY);
        }, "Get agent velocity as tuple")
        .def("getHeading", [](const AgentBody& self) {
            return self.orientation;  // In radians
        }, "Get agent heading in radians")
        .def("isAlive", [](const AgentBody& self) {
            return self.health > 0.0f;
        }, "Check if agent is alive");

    // ==============================================================================
    // SENSORY PERCEPT
    // ==============================================================================

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
        // Convenience methods
        .def("updateVision", [](SensoryPercept& self, size_t width, size_t height, const std::vector<float>& data) {
            Vision vision(width, height, 3);
            vision.setData(data);
            self.setVision(vision);
        }, py::arg("width"), py::arg("height"), py::arg("data"), "Update vision percept")
        .def("getVisionData", [](const SensoryPercept& self) {
            auto vision = self.getVision();
            return vision.getData();
        }, "Get flattened vision data")
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVision().getData().empty();
        }, "Check if vision data is available");

    // ==============================================================================
    // SIMPLE WORLD
    // ==============================================================================

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             "Get object at position (if any)")
        // World configuration helpers
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        // World creation helpers
        .def_static("createEmpty", []() {
            auto world = std::make_shared<SimpleWorld>();
            return world;
        }, "Create an empty world")
        .def_static("createWithResources", [](float width, float height, size_t visionWidth, size_t visionHeight) {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, visionWidth, visionHeight);
            // Add some default resources
            world->addObject(WorldObject(width/2, height/2, WorldObjectType::Resource, 100.0f, 0.3f));
            return world;
        }, py::arg("width") = 100.0f, py::arg("height") = 100.0f,
           py::arg("visionWidth") = 20, py::arg("visionHeight") = 20,
           "Create a world with default resources")
        .def("createTestObjects", [](SimpleWorld& self) {
            // Create test environment
            self.addObject(WorldObject(10, 10, WorldObjectType::Resource, 50.0f, 0.5f));
            self.addObject(WorldObject(90, 10, WorldObjectType::Hazard, 25.0f, 0.5f));
            self.addObject(WorldObject(10, 90, WorldObjectType::Marker, 0.0f, 0.5f));
            self.addObject(WorldObject(50, 50, WorldObjectType::Resource, 75.0f, 0.8f));
        }, "Create test objects in the world");

    // ==============================================================================
    // AGENT BRAIN
    // ==============================================================================

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
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        // AgentBrain configuration
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
        // Convenience methods
        .def("processVisionData", [](AgentBrain& self, const std::vector<std::vector<std::vector<float>>>& vision) {
            // Convert 3D image to Vision object
            size_t width = vision.empty() ? 0 : vision[0].size();
            size_t height = vision.size();
            size_t channels = width > 0 && height > 0 ? vision[0][0].size() : 0;
            
            Vision vis(width, height, channels);
            std::vector<float> flatData;
            flatData.reserve(width * height * channels);
            
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    for (size_t c = 0; c < channels; ++c) {
                        flatData.push_back(vision[y][x][c]);
                    }
                }
            }
            vis.setData(flatData);
            
            SensoryPercept percept;
            percept.setVision(vis);
            self.processSensoryInput(percept);
        }, py::arg("vision"), "Process vision data from 3D image")
        .def("getActionCommand", &AgentBrain::decodeMotorCommand,
             "Get action command from brain");

    // ==============================================================================
    // FACTORY FUNCTIONS
    // ==============================================================================

    // Global factory functions
    m.def("createDefaultConfig", []() {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createDefaultBrain", []() {
        auto config = std::make_shared<Config>();
        // Set sensible defaults for Phase 2 simulation
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration");

    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createCompleteAgentSystem", [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world) {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        return std::make_tuple(brain, agentBrain, world);
    }, py::arg("config"), py::arg("world"), 
       "Create a complete agent system (brain, agent brain, world)");

    m.def("run_test_simulation", [](std::shared_ptr<Brain> brain, int steps) {
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        for (int i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(), brain->getFiringNeuronCount());
    }, py::arg("brain"), py::arg("steps"), "Run a test simulation and return statistics");

    // ==============================================================================
    // BRAIN CONVENIENCE CLASS
    // ==============================================================================

    // Enhanced Brain class with Pythonic interfaces
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
        // Batch processing
        .def("runSteps", [](Brain& self, size_t steps) {
            double timestep = self.getConfig()->getOr<double>("simulation_timestep", 0.001);
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
        }, py::arg("steps"), "Run multiple simulation steps")
        .def("runTimedSteps", [](Brain& self, size_t steps, double timestep) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
        }, py::arg("steps"), py::arg("timestep"), "Run multiple simulation steps with custom timestep")
        // Sensory input convenience
        .def("injectSensoryData", [](Brain& self, const SensoryInput& input) {
            self.receiveSensoryInput(input);
        }, py::arg("input"), "Inject sensory data")
        .def("injectVisionData", [](Brain& self, size_t width, size_t height, const std::vector<float>& data) {
            Vision vision(width, height, 3);
            vision.setData(data);
            self.receiveSensoryInput(vision);
        }, py::arg("width"), py::arg("height"), py::arg("data"), "Inject vision data")
        // Action production
        .def("produceAction", &Brain::produceAction,
             "Produce motor/action output based on neural activity")
        .def("produceMotorCommand", [](Brain& self) {
            auto action = self.produceAction();
            if (action) {
                return action->getType();
            }
            return ActionType::Wait;
        }, "Produce motor command from brain activity")
        // Reset and state management
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        // Region management
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        // Statistics
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
             "Get pending spike event count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        // System access
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        // Logging and diagnostics
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("getBrainStatus", [](const Brain& self) {
            py::dict status;
            status["total_neurons"] = self.getTotalNeuronCount();
            status["total_synapses"] = self.getTotalSynapseCount();
            status["active_neurons"] = self.getActiveNeuronCount();
            status["firing_neurons"] = self.getFiringNeuronCount();
            status["total_spikes"] = self.getTotalSpikeCount();
            status["avg_firing_rate"] = self.getAverageFiringRate();
            status["e_i_ratio"] = self.getExcitationInhibitionRatio();
            status["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return status;
        }, "Get brain status as dictionary");

    // ==============================================================================
    // CONSTANTS AND FACTORIES
    // ==============================================================================

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Helper functions
    m.def("create_test_config", []() {
        auto config = std::make_shared<Config>();
        // Set default values for testing
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        return config;
    }, "Create a test configuration with default values");

    m.def("create_test_world", [](float width, float height) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 10, 10);
        return world;
    }, py::arg("width") = 100.0f, py::arg("height") = 100.0f,
       "Create a test world with specified dimensions");

    m.def("create_test_agent_system", [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world) {
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        // Create agent brain
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        return std::make_tuple(brain, agentBrain, world);
    }, py::arg("config"), py::arg("world"),
       "Create a complete test agent system");

    // ==============================================================================
    // UTILITY FUNCTIONS
    // ==============================================================================

    // Simulation utilities
    m.def("run_learning_experiment", [](std::shared_ptr<Brain> brain, size_t steps) {
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run with random input
        auto* spikeSystem = brain->getSpikeSystem();
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        for (size_t i = 0; i < steps; ++i) {
            // Inject random input to sensory neurons
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f + (rand() % 50));
            brain->step(i, i * 0.001);
            
            if (i % 100 == 0) {
                spikeSystem->checkPendingSpikes();
            }
        }
        
        size_t finalSpikes = brain->getTotalSpikeCount();
        size_t newSpikes = finalSpikes - initialSpikes;
        
        return std::make_tuple(
            newSpikes,
            brain->getFiringNeuronCount(),
            brain->getAverageFiringRate()
        );
    }, py::arg("brain"), py::arg("steps"), "Run a learning experiment and return statistics");

    m.def("create_plasticity_test", [](std::shared_ptr<Brain> brain) {
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        // Reset and test STDP
        brain->reset();
        brain->initialize();
        
        // Record initial weights
        std::vector<float> initialWeights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        // Create correlated pre->post activity (LTP)
        auto* spikeSystem = brain->getSpikeSystem();
        for (int trial = 0; trial < 20; ++trial) {
            // Fire pre-synaptic neuron
            if (auto* region = brain->getRegion(RegionId(1))) {
                auto neurons = region->getAllNeurons();
                if (neurons.size() >= 2) {
                    neurons[0]->injectCurrent(50.0f);
                    neurons[1]->injectCurrent(50.0f);
                }
            }
            brain->step(trial * 2, trial * 2 * 0.001);
            spikeSystem->checkPendingSpikes();
        }
        
        // Check weight changes
        std::vector<float> finalWeights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        // Calculate average change
        float totalChange = 0.0f;
        size_t count = 0;
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            totalChange += finalWeights[i] - initialWeights[i];
            ++count;
        }
        
        return std::make_tuple(
            totalChange / static_cast<float>(count),
            count,
            static_cast<bool>(totalChange > 0.001f) // LTP occurred
        );
    }, py::arg("brain"), "Test synaptic plasticity and return statistics");

    // ==============================================================================
    // MAIN MODULE ENTRY POINT
    // ==============================================================================

    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        This module provides a comprehensive Python interface to the NLM neural simulation
        framework. It enables you to create, train, and interact with artificial neural
        systems that learn from experience through real spiking dynamics.
        
        KEY FEATURES:
        
        1. **Intuitive API**: Pythonic interfaces that make neural network creation simple
        2. **Factory Functions**: Pre-built helpers for common use cases
        3. **Convenience Methods**: Simplified operations for common tasks
        4. **Type Safety**: Strong typing with clear error messages
        5. **Iterative Processing**: Batch operations for efficient simulation
        6. **Serialization**: Save and load brain states for persistent learning
        7. **Development Support**: Tools for creating and testing agent systems
        
        QUICK START:
        
        ```python
        import pynlm
        
        # Create a basic brain
        brain = pynlm.createDefaultBrain()
        
        # Initialize and run simulation
        brain.initialize()
        brain.runSteps(1000)
        
        # Get statistics
        status = brain.getBrainStatus()
        print(f"Total neurons: {status['total_neurons']}")
        print(f"Total spikes: {status['total_spikes']}")
        ```
        
        For more advanced usage, see the agent system factory functions
        and convenience methods in the documentation.
    )pbdoc";
}

} // namespace nlm