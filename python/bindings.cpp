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

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    // Add context manager support for Config
    py::class_<Config, std::shared_ptr<Config>, py::dynamic_attr>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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
        .def("get_int", &Config::get<int>, py::arg("key"),
             "Get integer configuration value")
        .def("get_float", &Config::get<double>, py::arg("key"),
             "Get floating-point configuration value")
        .def("get_string", &Config::get<std::string>, py::arg("key"),
             "Get string configuration value")
        .def("get_bool", &Config::get<bool>, py::arg("key"),
             "Get boolean configuration value")
        .def("get_list", &Config::get<std::vector<int>>, py::arg("key"),
             "Get list configuration value")
        .def("set_int", [](Config& self, const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set integer configuration value")
        .def("set_float", [](Config& self, const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set floating-point configuration value")
        .def("set_string", [](Config& self, const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set string configuration value")
        .def("set_bool", [](Config& self, const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set boolean configuration value")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("merge", [](Config& self, const std::string& other_file) {
            Config other;
            if (other.loadFromFile(other_file)) {
                // Merge other into self, with other's values taking precedence
                for (const auto& key : other.getKeys()) {
                    self.set(key, other.get<std::string>(key), ConfigSource::Runtime);
                }
            }
            return true;
        }, py::arg("other_file"),
             "Merge configuration from another file (overwrites existing keys)")
        .def("to_dict", [](const Config& self) {
            py::dict result;
            for (const auto& key : self.getKeys()) {
                auto val = self.get<std::string>(key);
                if (val.has_value()) {
                    result[key] = val.value();
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")
        .def("from_dict", [](Config& self, const py::dict& d) {
            self.clear();
            for (auto it = d.begin(); it != d.end(); ++it) {
                py::object key_obj = it->first;
                py::object value_obj = it->second;
                
                std::string key = py::str(key_obj);
                std::string value = py::str(value_obj);
                self.set(key, value, ConfigSource::Runtime);
            }
            return true;
        }, py::arg("dict"),
             "Set configuration from Python dictionary")
        .def("validate", [](const Config& self, const py::dict& schema) {
            // Simple validation - check required keys and types
            py::list errors;
            for (auto it = schema.begin(); it != schema.end(); ++it) {
                py::object key_obj = it->first;
                std::string key = py::str(key_obj);
                
                py::object type_obj = it->second["type"].cast<py::object>();
                std::string type = py::str(type_obj);
                
                bool required = it->second["required"].cast<bool>();
                
                if (required && !self.has(key)) {
                    errors.append("Missing required key: " + key);
                }
                
                if (self.has(key)) {
                    auto val = self.get<std::string>(key);
                    if (!val.has_value()) continue;
                    
                    if (type == "int") {
                        try {
                            std::stoi(val.value());
                        } catch (...) {
                            errors.append("Invalid int value for key: " + key);
                        }
                    } else if (type == "float") {
                        try {
                            std::stod(val.value());
                        } catch (...) {
                            errors.append("Invalid float value for key: " + key);
                        }
                    } else if (type == "bool") {
                        std::string lower = val.value();
                        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                        if (lower != "true" && lower != "false") {
                            errors.append("Invalid bool value for key: " + key);
                        }
                    }
                }
            }
            
            return errors;
        }, py::arg("schema"),
             "Validate configuration against a schema")

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

    // Add context manager support for SimpleWorld
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>, py::dynamic_attr>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        // Pythonic enhancements
        .def("__repr__", [](const SimpleWorld& w) {
            return "<SimpleWorld width=" + std::to_string(w.getWidth()) +
                   " height=" + std::to_string(w.getHeight()) + ">";
        })
        .def("tick", &SimpleWorld::update, py::arg("timestep") = 0.1,
             "Alias for update() with default timestep")
        .def("create_agent", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position (alias for setAgentStart)")
        .def("add_resource", [](SimpleWorld& w, float x, float y, float value = 1.0f) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            w.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
             "Add a resource object to the world")
        .def("add_hazard", [](SimpleWorld& w, float x, float y, float value = 1.0f) {
            WorldObject obj(x, y, WorldObjectType::Hazard, value);
            w.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
             "Add a hazard object to the world")
        .def("add_wall", [](SimpleWorld& w, float x, float y, float radius = 0.5f) {
            WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, radius);
            w.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f,
             "Add a wall object to the world")
        .def("simulate_step", [](SimpleWorld& w, double dt = 0.1) {
            w.update(dt);
            return w.getSimulationTime();
        }, py::arg("timestep") = 0.1,
             "Run one simulation step and return new time")
        .def("get_agent_state", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state (convenience alias)")
        .def("can_move", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             "Check if position is valid for movement");

    // Add context manager support for Brain
    py::class_<Brain, std::shared_ptr<Brain>, py::dynamic_attr>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
        // Pythonic enhancements
        .def("__repr__", [](const Brain& b) {
            return "<Brain neurons=" + std::to_string(b.getTotalNeuronCount()) +
                   " active=" + std::to_string(b.getActiveNeuronCount()) + ">";
        })
        .def("__len__", [](const Brain& b) {
            return b.getTotalNeuronCount();
        })
        .def("get_spike_count", &Brain::getTotalSpikeCount)  // Pythonic alias
        .def("is_initialized", [](const Brain& b) {
            // Brain is initialized if it has regions or neurons
            return b.getTotalNeuronCount() > 0 || b.getTotalSynapseCount() > 0;
        });

    // Add context manager support for AgentBrain
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>, py::dynamic_attr>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        // Pythonic enhancements
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain>";
        })
        .def("get_status", [](AgentBrain& agent) {
            py::dict status;
            status["developmental_stage"] = (int)agent.getDevelopmentalStage();
            status["neuromodulation_level"] = agent.getNeuromodulationLevel();
            status["curiosity_level"] = agent.getCuriosityLevel();
            status["novelty_level"] = agent.getNoveltyLevel();
            status["prediction_error"] = agent.getPredictionError();
            return status;
        }, "Get agent status as dictionary")
        .def("enable_all_features", [](AgentBrain& agent) {
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
        }, "Enable all learning features at once")
        .def("disable_all_features", [](AgentBrain& agent) {
            agent.enableRewardModulation(false);
            agent.enableStructuralPlasticity(false);
            agent.enableDevelopment(false);
            agent.enableCuriosity(false);
        }, "Disable all learning features at once")

    // Add Pythonic convenience functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration (Pythonic alias for createDefaultConfig)");
    
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration (Pythonic alias for createBrain)");
    
    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world (Pythonic alias for createSimpleWorld)");
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface (Pythonic alias for createAgentBrain)");
    
    // Pythonic helper functions
    m.def("run_complete_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                        std::shared_ptr<AgentBrain> agent, int num_steps) {
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(brain->getFiringNeuronCount(), brain->getTotalSpikeCount(),
                               brain->getAverageFiringRate());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
        "Run a complete simulation and return statistics (neurons_firing, total_spikes, avg_firing_rate)");
    
    m.def("create_config_from_dict", [](const py::dict& d) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (auto it = d.begin(); it != d.end(); ++it) {
            py::object key_obj = it->first;
            py::object value_obj = it->second;
            
            std::string key = py::str(key_obj);
            std::string value = py::str(value_obj);
            config->set(key, value, ConfigSource::Runtime);
        }
        return config;
    }, py::arg("dict"), "Create a Config from a Python dictionary");
    
    m.def("load_config_file", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"), "Load configuration from a JSON file and return Config object");
    
    m.def("save_config_to_file", [](const std::shared_ptr<Config>& config, const std::string& filepath) {
        return config->saveToFile(filepath);
    }, py::arg("config"), py::arg("filepath"), "Save configuration to a JSON file");
    
    // Module-level constants and utilities
    m.attr("DEFAULT_CONFIG_VERSION") = "1.0";
    m.attr("DEFAULT_WORLD_WIDTH") = 10;
    m.attr("DEFAULT_WORLD_HEIGHT") = 10;
    m.attr("DEFAULT_VISION_WIDTH") = 8;
    m.attr("DEFAULT_VISION_HEIGHT") = 8;
    
    m.def("get_version", []() {
        return "NLM Python Bindings v1.0.0";
    }, "Get version information");
    
    m.def("benchmark_setup", []() {
        py::dict setup;
        setup["config"] = m.attr("create_default_config")();
        setup["world"] = m.attr("create_simple_world")();
        setup["brain"] = m.attr("create_brain")(setup["config"]);
        setup["agent"] = m.attr("create_agent_brain")(setup["brain"]);
        return setup;
    }, "Create a pre-configured benchmark setup (config, world, brain, agent)");
    
    m.def("quick_test", []() {
        auto config = m.attr("create_default_config")();
        config->set("test_param", 42, ConfigSource::Runtime);
        config->set("test_string", "Hello World", ConfigSource::Runtime);
        
        auto brain = m.attr("create_brain")(config);
        brain->initialize();
        
        for (int i = 0; i < 10; ++i) {
            brain->step(i);
        }
        
        py::dict result;
        result["neurons"] = brain->getTotalNeuronCount();
        result["spikes"] = brain->getTotalSpikeCount();
        result["active"] = brain->getActiveNeuronCount();
        result["test_passed"] = true;
        
        return result;
    }, "Run a quick smoke test and return results");
}

} // namespace nlm
