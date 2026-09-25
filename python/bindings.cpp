#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Module version for compatibility checking
    m.attr("version") = "0.1.0";
    m.attr("__version__") = "0.1.0";

    // Register C++ exceptions for Python compatibility
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    py::register_exception<std::logic_error>(m, "RuntimeError");

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
        })
        // Convenience methods for type-safe value retrieval
        .def("getFloat", [](const Config& self, const std::string& key) {
            return self.getFloat(key);
        }, py::arg("key"), R"pbdoc(Get a float value by key)pbdoc")
        .def("getInt", [](const Config& self, const std::string& key) {
            return self.getInt(key);
        }, py::arg("key"), R"pbdoc(Get an integer value by key)pbdoc")
        .def("getString", [](const Config& self, const std::string& key) {
            return self.getString(key);
        }, py::arg("key"), R"pbdoc(Get a string value by key)pbdoc")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.setFloat(key, value);
        }, py::arg("key"), py::arg("value"), R"pbdoc(Set a float value)pbdoc")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.setInt(key, value);
        }, py::arg("key"), py::arg("value"), R"pbdoc(Set an integer value)pbdoc")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.setString(key, value);
        }, py::arg("key"), py::arg("value"), R"pbdoc(Set a string value)pbdoc")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get(key);
        }, py::arg("key"), R"pbdoc(Generic get value by key)pbdoc")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), R"pbdoc(Generic set value by key)pbdoc");

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
        .def("getChannels", &Vision::getChannels)
        // Context manager support for automatic cleanup
        .def("__enter__", [](Vision& self) {
            return self;
        })
        .def("__exit__", [](Vision&, const py::object&) {
            // Perform cleanup if needed
        })
        .def("__repr__", [](const Vision& v) {
            return "<Vision width=" + std::to_string(v.getWidth()) +
                   " height=" + std::to_string(v.getHeight()) +
                   " channels=" + std::to_string(v.getChannels()) + ">";
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Property access for better Pythonic API
        .def_property("sampleRate", &Audio::getSampleRate, &Audio::setSampleRate)
        .def_property("numSamples", &Audio::getNumSamples, [](Audio& self, size_t n) {
            // Note: Audio class doesn't have setNumSamples, but we can simulate it
            // For now, this just provides read-only access
        })
        .def("__repr__", [](const Audio& a) {
            return "<Audio sampleRate=" + std::to_string(a.getSampleRate()) +
                   " numSamples=" + std::to_string(a.getNumSamples()) + ">";
        });

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
        .def("clone", &Action::clone)
        // Pythonic methods
        .def("__str__", &Action::getName)
        .def("__repr__", [](const Action& a) {
            return "<Action type=" + std::to_string(static_cast<int>(a.getType())) +
                   " parameters=[...]>";
        })
        // Context manager for action lifecycle
        .def("__enter__", [](Action& self) {
            return &self;
        })
        .def("__exit__", [](Action&, const py::object&) {
            // Perform cleanup if needed
        });

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        // Input validation
        .def("validate", [](WorldObject& self) {
            if (self.radius < 0.0f) {
                throw std::invalid_argument("WorldObject radius cannot be negative");
            }
            if (self.value < 0.0f) {
                throw std::invalid_argument("WorldObject value cannot be negative");
            }
            return true;
        }, R"pbdoc(Validate world object parameters)pbdoc")
        .def("getDistanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Get distance to point (x, y))pbdoc")
        .def("isCollidingWith", [](const WorldObject& self, const WorldObject& other) {
            float distance = self.getDistanceTo(other.x, other.y);
            return distance < (self.radius + other.radius);
        }, py::arg("other"), R"pbdoc(Check if colliding with another world object)pbdoc")
        // Context manager for object lifecycle
        .def("__enter__", [](WorldObject& self) {
            self.validate();
            return &self;
        })
        .def("__exit__", [](WorldObject&, const py::object&) {
            // Perform cleanup if needed
        })
        .def("__repr__", [](const WorldObject& w) {
            return "<WorldObject type=" + std::to_string(static_cast<int>(w.type)) +
                   " pos=(" + std::to_string(w.x) + "," + std::to_string(w.y) +
                   ") radius=" + std::to_string(w.radius) +
                   " value=" + std::to_string(w.value) + ">";
        });

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
        .def_readwrite("message", &ActionResult::message)
        // Validation methods
        .def("validate", [](ActionResult& self) {
            if (self.reward < -100.0f || self.reward > 100.0f) {
                throw std::out_of_range("Reward value out of valid range [-100.0, 100.0]");
            }
            return true;
        }, R"pbdoc(Validate action result, throws if reward out of range)pbdoc")
        .def("getNormalizedReward", [](const ActionResult& self) {
            return std::max(-1.0f, std::min(1.0f, self.reward / 100.0f));
        }, R"pbdoc(Get reward normalized to [-1, 1] range)pbdoc")
        .def("getSuccessScore", [](const ActionResult& self) {
            return self.success ? 1.0f : 0.0f;
        }, R"pbdoc(Get binary success score)pbdoc")
        // Context manager
        .def("__enter__", [](ActionResult& self) {
            return &self;
        })
        .def("__exit__", [](ActionResult&, const py::object&) {
            // Perform cleanup if needed
        })
        .def("__repr__", [](const ActionResult& r) {
            return "<ActionResult reward=" + std::to_string(r.reward) +
                   " success=" + std::to_string(r.success) +
                   " message=\"" + r.message + "\">";
        });

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
        // Enhanced configure method with validation and Pythonic API
        .def("configure", [](SimpleWorld& self, int width, int height, 
                           int visionWidth = 8, int visionHeight = 8) {
            if (width <= 0 || height <= 0 || visionWidth <= 0 || visionHeight <= 0) {
                throw std::invalid_argument("World dimensions must be positive integers");
            }
            if (visionWidth > width || visionHeight > height) {
                throw std::invalid_argument("Vision dimensions cannot exceed world dimensions");
            }
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"),
           py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
           R"pbdoc(Configure world dimensions with validation)pbdoc")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        // Enhanced update method with timestamp
        .def("update", [](SimpleWorld& self, float timestep = 0.1f) {
            if (timestep <= 0.0f) {
                throw std::invalid_argument("Timestep must be positive");
            }
            self.update(timestep);
        }, py::arg("timestep") = 0.1f, R"pbdoc(Update world with optional timestep)pbdoc")
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
        // Convenience methods
        .def("isAgentAtPosition", [](const SimpleWorld& self, float x, float y) {
            const auto& body = self.getAgentBody();
            return std::abs(body.x - x) < 0.1f && std::abs(body.y - y) < 0.1f;
        }, py::arg("x"), py::arg("y"), R"pbdoc(Check if agent is at position)pbdoc")
        .def("getDistanceToAgent", [](const SimpleWorld& self, float x, float y) {
            const auto& body = self.getAgentBody();
            float dx = body.x - x;
            float dy = body.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Get distance from agent to position)pbdoc")
        .def("getMapString", [](const SimpleWorld& self) {
            std::string result = "World Map:\n";
            result += "  Width: " + std::to_string(self.getWidth()) + "\n";
            result += "  Height: " + std::to_string(self.getHeight()) + "\n";
            result += "  Agent: (" + std::to_string(self.getAgentBody().x) + "," + 
                      std::to_string(self.getAgentBody().y) + ")\n";
            result += "  Energy: " + std::to_string(self.getAgentBody().energy) + "\n";
            return result;
        }, R"pbdoc(Get string representation of world state)pbdoc")
        // Context manager for world lifecycle
        .def("__enter__", [](SimpleWorld& self) {
            return &self;
        })
        .def("__exit__", [](SimpleWorld&, const py::object&) {
            // Perform cleanup if needed
        });

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
              "Initialize the brain with configuration")
        // Enhanced step methods with validation
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
        // Enhanced region methods
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
        // Statistics and validation methods
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
        // Convenience methods and validation
        .def("getConfig", &Brain::getConfig,
              py::return_value_policy::reference_internal,
              "Get the configuration")
        .def("logStatus", &Brain::logStatus,
              "Log brain status")
        .def("isHealthy", [](const Brain& self) {
            return self.getActiveNeuronCount() > 0 && 
                   self.getTotalSynapseCount() > 0;
        }, R"pbdoc(Check if brain is in a healthy state)pbdoc")
        .def("getStatistics", [](const Brain& self) {
            std::string result = "Brain Statistics:\n";
            result += "  Total Neurons: " + std::to_string(self.getTotalNeuronCount()) + "\n";
            result += "  Active Neurons: " + std::to_string(self.getActiveNeuronCount()) + "\n";
            result += "  Firing Neurons: " + std::to_string(self.getFiringNeuronCount()) + "\n";
            result += "  Total Synapses: " + std::to_string(self.getTotalSynapseCount()) + "\n";
            result += "  Average Firing Rate: " + std::to_string(self.getAverageFiringRate()) + "\n";
            result += "  Total Spikes: " + std::to_string(self.getTotalSpikeCount()) + "\n";
            result += "  E/I Ratio: " + std::to_string(self.getExcitationInhibitionRatio()) + "\n";
            result += "  Developmental Stage: " + std::to_string(static_cast<int>(self.getDevelopmentalStage())) + "\n";
            return result;
        }, R"pbdoc(Get comprehensive brain statistics string)pbdoc")
        // Context manager for brain lifecycle
        .def("__enter__", [](Brain& self) {
            return &self;
        })
        .def("__exit__", [](Brain&, const py::object&) {
            // Perform cleanup if needed
        });

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
        // Convenience methods
        .def("getLearningStatus", [](const AgentBrain& self) {
            return std::make_tuple(
                self.isRewardModulationEnabled(),
                self.isStructuralPlasticityEnabled(),
                self.isDevelopmentEnabled(),
                self.isCuriosityEnabled()
            );
        }, R"pbdoc(Get comprehensive learning status tuple)pbdoc")
        .def("isReady", &AgentBrain::initialize, py::arg("world"),
              R"pbdoc(Check if agent brain is properly initialized)pbdoc")
        .def("validateInput", [](AgentBrain& self, const SensoryPercept& percept) {
            return self.getSensoryInputSize() == percept.getAllSignals().size();
        }, py::arg("percept"), R"pbdoc(Validate sensory input size)pbdoc");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Additional factory functions for common use cases
    m.def("createCompleteAgent", [](std::shared_ptr<Config> config, 
                                  int width = 20, int height = 20,
                                  int visionWidth = 8, int visionHeight = 8) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("config"), py::arg("width") = 20, py::arg("height") = 20,
       py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
       "Create a complete agent setup with brain, world, and agent interface");

    m.def("createSimulatedEnvironment", [](int width = 30, int height = 30,
                                          float energyDecay = 0.01f,
                                          float maxEnergy = 100.0f) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->setMaxEnergy(maxEnergy);
        world->setEnergyDecayRate(energyDecay);
        world->reset();
        return world;
    }, py::arg("width") = 30, py::arg("height") = 30,
       py::arg("energyDecay") = 0.01f, py::arg("maxEnergy") = 100.0f,
       "Create a simulated environment with configurable parameters");

    // Module utility functions
    m.def("createIdentityConfig", []() {
        auto config = std::make_shared<Config>();
        config->setFloat("brain.neuron_count", 1000.0f);
        config->setFloat("brain.synapse_density", 0.1f);
        config->setFloat("brain.v_thresh", -50.0f);
        config->setFloat("brain.v_rest", -70.0f);
        return config;
    }, "Create an identity/default configuration with standard parameters");

    m.def("runSimpleSimulation", [](int numSteps = 100) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (int i = 0; i < numSteps; ++i) {
            brain->step(i);
        }
        
        std::string result = "Simulation completed in " + std::to_string(numSteps) + " steps.\n";
        result += "Total neurons: " + std::to_string(brain->getTotalNeuronCount()) + "\n";
        result += "Total spikes: " + std::to_string(brain->getTotalSpikeCount()) + "\n";
        result += "Firing neurons: " + std::to_string(brain->getFiringNeuronCount()) + "\n";
        return result;
    }, py::arg("numSteps") = 100,
       "Run a simple brain simulation and return statistics");

    // Constants and module attributes
    m.attr("VERSION") = "0.1.0";
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_SYNAPSE_DENSITY") = 0.1f;
    m.attr("DEFAULT_V_THRESH") = -50.0f;
    m.attr("DEFAULT_V_REST") = -70.0f;
    m.attr("DEFAULT_MAX_ENERGY") = 100.0f;

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

} // namespace nlm
