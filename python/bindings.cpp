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

// Template function to create Python bindings for ID classes
// Reduces code duplication for NeuronId, SynapseId, RegionId, PopulationId
template<typename T>
void bindIdClass(py::module_& m, const std::string& name, const std::string& doc) {
    py::class_<T>(m, name.c_str(), doc.c_str())
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &T::value)
        .def("index", &T::index)
        .def("__eq__", &T::operator==)
        .def("__ne__", &T::operator!=)
        .def("__hash__", [](const T& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const T& id) {
            return "<" + name + ": " + std::to_string(id.value) + ">";
        });
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Bind ID classes using template function to reduce duplication
    bindIdClass<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc");
    bindIdClass<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc");
    bindIdClass<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc");
    bindIdClass<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc");

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
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            bool success = self.loadFromFile(filepath);
            if (!success) {
                py::raise(PyExc(IOError), "Failed to load configuration from file: " + filepath);
            }
            return success;
        }, py::arg("filepath"),
             "Load configuration from a JSON file. Raises IOError if file cannot be read.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            bool success = self.saveToFile(filepath);
            if (!success) {
                py::raise(PyExc(IOError), "Failed to save configuration to file: " + filepath);
            }
            return success;
        }, py::arg("filepath"),
             "Save configuration to a JSON file. Raises IOError if file cannot be written.")
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
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        .def("__len__", [](const Config& cfg) {
            return static_cast<int>(cfg.getKeys().size());
        })
        // Convenience methods
        .def("getBool", [](const Config& cfg, const std::string& key, bool defaultValue) {
            auto val = cfg.get<bool>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get boolean value with default")
        .def("getInt", [](const Config& cfg, const std::string& key, int64_t defaultValue) {
            auto val = cfg.get<int64_t>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get integer value with default")
        .def("getDouble", [](const Config& cfg, const std::string& key, double defaultValue) {
            auto val = cfg.get<double>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get double value with default")
        .def("getString", [](const Config& cfg, const std::string& key, const std::string& defaultValue) {
            auto val = cfg.get<std::string>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get string value with default")
        .def("setBool", [](Config& cfg, const std::string& key, bool value, ConfigSource source) {
            cfg.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set boolean value")
        .def("setInt", [](Config& cfg, const std::string& key, int64_t value, ConfigSource source) {
            cfg.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set integer value")
        .def("setDouble", [](Config& cfg, const std::string& key, double value, ConfigSource source) {
            cfg.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set double value")
        .def("setString", [](Config& cfg, const std::string& key, const std::string& value, ConfigSource source) {
            cfg.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set string value")
        // Enum access convenience methods
        .def("getSourceName", [](const Config& cfg, const std::string& key) {
            // This would need to be implemented in Config class
            return "Config source lookup for " + key;
        }, py::arg("key"), "Get source name for key (requires implementation)");

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
        .def("getArea", [](const Vision& v) {
            return v.getWidth() * v.getHeight();
        }, "Get total number of pixels")
        .def("getPixelCount", [](const Vision& v) {
            return v.getWidth() * v.getHeight() * v.getChannels();
        }, "Get total number of pixels including channels")
        .def("__repr__", [](const Vision& v) {
            return "<Vision: " + std::to_string(v.getWidth()) + "x" + 
                   std::to_string(v.getHeight()) + "x" + 
                   std::to_string(v.getChannels()) + ">";
        })
        .def("__str__", [](const Vision& v) {
            return "Vision(width=" + std::to_string(v.getWidth()) + 
                   ", height=" + std::to_string(v.getHeight()) + 
                   ", channels=" + std::to_string(v.getChannels()) + ")";
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
        .def("getDurationSeconds", [](const Audio& a) {
            return static_cast<double>(a.getNumSamples()) / a.getSampleRate();
        }, "Get audio duration in seconds")
        .def("getSampleData", [](const Audio& a) {
            return a.getData();
        }, "Get audio sample data as vector")
        .def("__repr__", [](const Audio& a) {
            return "<Audio: " + std::to_string(a.getSampleRate()) + "Hz, " +
                   std::to_string(a.getNumSamples()) + " samples>";
        })
        .def("__str__", [](const Audio& a) {
            return "Audio(sampleRate=" + std::to_string(a.getSampleRate()) + 
                   ", numSamples=" + std::to_string(a.getNumSamples()) + ")";
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("__repr__", [](const InternalSignals& s) {
            return "<InternalSignals: " + std::to_string(s.getData().size()) + 
                   " signals>";
        })
        .def("__str__", [](const InternalSignals& s) {
            return "InternalSignals(signals=" + std::to_string(s.getData().size()) + ")";
        });

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
        // Convenience methods
        .def("getParameterCount", [](const Action& a) {
            return a.getParameters().size();
        }, "Get number of parameters")
        .def("getParameter", [](const Action& a, size_t index) {
            const auto& params = a.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            py::raise(PyExc(IndexError), "Parameter index out of range");
            return 0.0f; // Never reached, but needed for compilation
        }, py::arg("index"), "Get parameter at index")
        .def("addParameter", [](Action& a, float value) {
            auto params = a.getParameters();
            params.push_back(value);
            a.setParameters(params);
        }, py::arg("value"), "Add parameter to action")
        .def("__repr__", [](const Action& a) {
            return "<Action: " + a.getName() + " (type=" + 
                   std::to_string(static_cast<int>(a.getType())) + ")>";
        })
        .def("__str__", [](const Action& a) {
            return a.getName() + "(type=" + 
                   std::to_string(static_cast<int>(a.getType())) + ")";
        });

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        // Convenience methods
        .def("hasVision", [](const SensoryPercept& p) {
            return p.getVisionWidth() > 0 && p.getVisionHeight() > 0;
        }, "Check if vision data is present")
        .def("hasTouch", [](const SensoryPercept& p) {
            return p.getTouch() != nullptr;
        }, "Check if touch data is present")
        .def("hasInternal", [](const SensoryPercept& p) {
            return p.getInternal() != nullptr;
        }, "Check if internal signals are present")
        .def("hasAudio", [](const SensoryPercept& p) {
            return p.getAudio() != nullptr;
        }, "Check if audio data is present")
        .def("getCombinedSignalCount", [](const SensoryPercept& p) {
            size_t count = 0;
            if (p.getVision()) count++;
            if (p.getTouch()) count++;
            if (p.getInternal()) count++;
            if (p.getAudio()) count++;
            return count;
        }, "Get total number of active signal types")
        .def("__repr__", [](const SensoryPercept& p) {
            return "<SensoryPercept: timestamp=" + std::to_string(p.getTimestamp()) + ">";
        })
        .def("__str__", [](const SensoryPercept& p) {
            return "SensoryPercept(timestamp=" + std::to_string(p.getTimestamp()) + ")";
        });

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
        // Convenience methods
        .def("getWorldSize", [](const SimpleWorld& w) {
            return std::make_pair(w.getWidth(), w.getHeight());
        }, "Get world dimensions as (width, height)")
        .def("getAgentPosition", [](const SimpleWorld& w) {
            const auto& body = w.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, "Get agent position as (x, y)")
        .def("getAgentEnergy", [](const SimpleWorld& w) {
            return w.getAgentBody().energy;
        }, "Get agent energy level")
        .def("getAgentHealth", [](const SimpleWorld& w) {
            return w.getAgentBody().health;
        }, "Get agent health level")
        .def("getAgentAge", [](const SimpleWorld& w) {
            return w.getAgentBody().age;
        }, "Get agent age")
        .def("isAgentAlive", [](const SimpleWorld& w) {
            return w.getAgentBody().health > 0;
        }, "Check if agent is alive")
        .def("isPositionEmpty", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (empty or accessible)")
        .def("addWall", [](SimpleWorld& w, float x, float y, float radius) {
            WorldObject wall;
            wall.x = x;
            wall.y = y;
            wall.type = WorldObjectType::Wall;
            wall.radius = radius;
            wall.active = true;
            w.addObject(wall);
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Add a wall object to the world")
        .def("addResource", [](SimpleWorld& w, float x, float y, float value) {
            WorldObject resource;
            resource.x = x;
            resource.y = y;
            resource.type = WorldObjectType::Resource;
            resource.value = value;
            resource.radius = 0.3f;
            resource.active = true;
            w.addObject(resource);
        }, py::arg("x"), py::arg("y"), py::arg("value"), "Add a resource object to the world")
        .def("__repr__", [](const SimpleWorld& w) {
            return "<SimpleWorld: " + std::to_string(w.getWidth()) + "x" +
                   std::to_string(w.getHeight()) + " (time=" +
                   std::to_string(w.getSimulationTime()) + ")>";
        })
        .def("__str__", [](const SimpleWorld& w) {
            return "SimpleWorld(width=" + std::to_string(w.getWidth()) + 
                   ", height=" + std::to_string(w.getHeight()) + 
                   ", time=" + std::to_string(w.getSimulationTime()) + ")";
        });

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
        .def("__repr__", [](const Brain& b) {
            return "<Brain: " + std::to_string(b.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(b.getTotalSynapseCount()) + " synapses, " +
                   std::to_string(b.getFiringNeuronCount()) + " firing>";
        })
        .def("__str__", [](const Brain& b) {
            return "Brain(neurons=" + std::to_string(b.getTotalNeuronCount()) + 
                   ", synapses=" + std::to_string(b.getTotalSynapseCount()) + 
                   ", firing=" + std::to_string(b.getFiringNeuronCount()) + ")";
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
        .def("__repr__", [](const AgentBrain& ab) {
            return "<AgentBrain: " + std::to_string(ab.getNeuromodulationLevel()) + 
                   ", curiosity=" + std::to_string(ab.getCuriosityLevel()) + ", " +
                   "novelty=" + std::to_string(ab.getNoveltyLevel()) + ", " +
                   "prediction_error=" + std::to_string(ab.getPredictionError()) + ">";
        })
        .def("__str__", [](const AgentBrain& ab) {
            return "AgentBrain(neuromodulation=" + std::to_string(ab.getNeuromodulationLevel()) + 
                   ", curiosity=" + std::to_string(ab.getCuriosityLevel()) + 
                   ", novelty=" + std::to_string(ab.getNoveltyLevel()) + 
                   ", prediction_error=" + std::to_string(ab.getPredictionError()) + ")";
        });

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
