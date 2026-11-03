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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
        // Python-friendly convenience methods
        .def("get_int", [](Config& self, const std::string& key, int default_value) {
            auto value = self.get<int>(key);
            return value.has_value() ? value.value() : default_value;
        }, py::arg("key"), py::arg("default_value"),
           "Get integer value, returns default_value if key doesn't exist")
        .def("get_float", [](Config& self, const std::string& key, float default_value) {
            auto value = self.get<double>(key);
            return value.has_value() ? static_cast<float>(value.value()) : default_value;
        }, py::arg("key"), py::arg("default_value"),
           "Get float value, returns default_value if key doesn't exist")
        .def("get_str", [](Config& self, const std::string& key, const std::string& default_value) {
            auto value = self.get<std::string>(key);
            return value.has_value() ? value.value() : default_value;
        }, py::arg("key"), py::arg("default_value"),
           "Get string value, returns default_value if key doesn't exist")
        .def("get_bool", [](Config& self, const std::string& key, bool default_value) {
            auto value = self.get<bool>(key);
            return value.has_value() ? value.value() : default_value;
        }, py::arg("key"), py::arg("default_value"),
           "Get boolean value, returns default_value if key doesn't exist")
        .def("set_int", [](Config& self, const std::string& key, int value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set integer value")
        .def("set_float", [](Config& self, const std::string& key, float value) {
            self.set(key, static_cast<double>(value), nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set float value")
        .def("set_str", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set string value")
        .def("set_bool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set boolean value")
        .def("get_all", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                if (value.has_value()) {
                    result[key.c_str()] = value.value();
                } else {
                    result[key.c_str()] = "";
                }
            }
            return result;
        }, "Get all configuration values as a dictionary")
        .def("load_from_dict", [](Config& self, const py::dict& data) {
            for (auto item : data) {
                std::string key = py::cast<std::string>(item.first);
                auto value = item.second;
                if (value.is(py::int_())) {
                    self.set(key, py::cast<int>(value), nlm::ConfigSource::Runtime);
                } else if (value.is(py::float_())) {
                    self.set(key, py::cast<double>(value), nlm::ConfigSource::Runtime);
                } else if (value.is(py::str())) {
                    self.set(key, py::cast<std::string>(value), nlm::ConfigSource::Runtime);
                } else if (value.is(py::bool_())) {
                    self.set(key, py::cast<bool>(value), nlm::ConfigSource::Runtime);
                }
            }
        }, py::arg("data"),
           "Load configuration from a dictionary")
        .def("to_dict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                if (value.has_value()) {
                    result[key.c_str()] = value.value();
                } else {
                    result[key.c_str()] = "";
                }
            }
            return result;
        }, "Convert configuration to dictionary")
        .def("update", [](Config& self, const py::dict& data) {
            for (auto item : data) {
                std::string key = py::cast<std::string>(item.first);
                auto value = item.second;
                if (value.is(py::int_())) {
                    self.set(key, py::cast<int>(value), nlm::ConfigSource::Runtime);
                } else if (value.is(py::float_())) {
                    self.set(key, py::cast<double>(value), nlm::ConfigSource::Runtime);
                } else if (value.is(py::str())) {
                    self.set(key, py::cast<std::string>(value), nlm::ConfigSource::Runtime);
                } else if (value.is(py::bool_())) {
                    self.set(key, py::cast<bool>(value), nlm::ConfigSource::Runtime);
                }
            }
        }, py::arg("data"),
           "Update configuration from a dictionary");

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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    // Add Pythonic convenience methods to SimpleWorld
    m.def("create_simple_world", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world (standalone factory)");

    m.def("configure_world", [](std::shared_ptr<SimpleWorld> world, size_t width, size_t height,
                                size_t visionWidth = 8, size_t visionHeight = 8) {
        world->configure(width, height, visionWidth, visionHeight);
    }, py::arg("world"), py::arg("width"), py::arg("height"),
       py::arg("vision_width") = 8, py::arg("vision_height") = 8,
       "Configure world dimensions (standalone function)");

    m.def("update_world", [](std::shared_ptr<SimpleWorld> world, double timestep) {
        world->update(timestep);
    }, py::arg("world"), py::arg("timestep"),
       "Update world timestep (standalone function)");

    m.def("run_world_simulation", [](std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent,
                                     int num_steps, double timestep = 0.1) {
        world->reset();
        for (int step = 0; step < num_steps; ++step) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
        }
    }, py::arg("world"), py::arg("agent"), py::arg("num_steps"), py::arg("timestep") = 0.1,
       "Run a complete world simulation with agent");

    m.def("get_world_status", [](const std::shared_ptr<SimpleWorld>& world) {
        py::dict status;
        status["width"] = world->getWidth();
        status["height"] = world->getHeight();
        status["simulation_time"] = world->getSimulationTime();
        status["max_energy"] = world->getMaxEnergy();
        status["energy_decay_rate"] = world->getEnergyDecayRate();
        return status;
    }, py::arg("world"),
       "Get world status as dictionary (standalone function)");

    m.def("is_world_healthy", [](const std::shared_ptr<SimpleWorld>& world) {
        return world->getWidth() > 0 && world->getHeight() > 0 &&
               world->getSimulationTime() >= 0.0;
    }, py::arg("world"),
       "Check if world is in a healthy state (standalone function)");

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("step_with_time", [](Brain& self, SimulationStep step, Timestamp time) {
            self.step(step, time);
        }, py::arg("step"), py::arg("time"),
           "Perform a simulation step with explicit timestamp (Pythonic alias)")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current", [](Brain& self, NeuronId neuron, float current) {
            self.injectCurrent(neuron, current);
        }, py::arg("neuron"), py::arg("current"),
           "Inject current into a specific neuron (Pythonic alias)")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("inject_current_to_neurons", [](Brain& self, NeuronType type, float current) {
            self.injectCurrentToNeurons(type, current);
        }, py::arg("type"), py::arg("current"),
           "Inject current into all neurons of a specific type (Pythonic alias)")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", [](Brain& self, const std::string& name = "") {
            return self.addRegion(name);
        }, py::arg("name") = "",
           "Add a new neural region (Pythonic alias)")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region", [](Brain& self, RegionId id) {
            return self.getRegion(id);
        }, py::arg("id"),
           "Get a region by ID (Pythonic alias)")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions (Pythonic alias)")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs (Pythonic alias)")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions (Pythonic alias)")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions (Pythonic alias)")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions (Pythonic alias)")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons (Pythonic alias)")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons (Pythonic alias)")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons (Pythonic alias)")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_ei_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio (Pythonic alias)")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count (Pythonic alias)")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage (Pythonic alias)")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("set_developmental_stage", [](Brain& self, DevelopmentalStage stage) {
            self.setDevelopmentalStage(stage);
        }, py::arg("stage"),
           "Set developmental stage (Pythonic alias)")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("get_config", [](Brain& self) {
            return self.getConfig();
        }, "Get the configuration (Pythonic alias)")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("log_status", [](Brain& self) {
            self.logStatus();
        }, "Log brain status (Pythonic alias)")
        .def("run_simulation", [](Brain& self, int num_steps, Timestamp time_step = 0.0) {
            for (int step = 0; step < num_steps; ++step) {
                self.step(step, time_step * step);
            }
        }, py::arg("num_steps"), py::arg("time_step") = 0.0,
           "Run a simulation for a specified number of steps")
        .def("get_summary", [](const Brain& self) {
            std::string summary = "Brain Summary:\n";
            summary += "  Total Neurons: " + std::to_string(self.getTotalNeuronCount()) + "\n";
            summary += "  Total Synapses: " + std::to_string(self.getTotalSynapseCount()) + "\n";
            summary += "  Firing Neurons: " + std::to_string(self.getFiringNeuronCount()) + "\n";
            summary += "  Average Firing Rate: " + std::to_string(self.getAverageFiringRate()) + "\n";
            summary += "  Total Spikes: " + std::to_string(self.getTotalSpikeCount()) + "\n";
            summary += "  Region Count: " + std::to_string(self.getRegionCount()) + "\n";
            summary += "  Developmental Stage: " + std::to_string(static_cast<int>(self.getDevelopmentalStage())) + "\n";
            return summary;
        }, "Get a human-readable summary of the brain state")
        .def("get_stats", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["region_count"] = self.getRegionCount();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["ei_ratio"] = self.getExcitationInhibitionRatio();
            return stats;
        }, "Get brain statistics as a dictionary");

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size (Pythonic alias)")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size (Pythonic alias)")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("process_sensory_input", [](AgentBrain& self, const SensoryPercept& percept) {
            self.processSensoryInput(percept);
        }, py::arg("percept"),
           "Process sensory percept and inject into brain (Pythonic alias)")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("decode_motor_command", [](AgentBrain& self) {
            return self.decodeMotorCommand();
        }, "Decode brain motor activity into motor command (Pythonic alias)")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("apply_reward_modulation", [](AgentBrain& self, float reward, float predictedReward) {
            self.applyRewardModulation(reward, predictedReward);
        }, py::arg("reward"), py::arg("predictedReward"),
           "Apply reward-based neuromodulation (Pythonic alias)")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("update_development", [](AgentBrain& self, double timestep) {
            self.updateDevelopment(timestep);
        }, py::arg("timestep"),
           "Update development system (Pythonic alias)")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage (Pythonic alias)")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level (Pythonic alias)")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level (Pythonic alias)")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level (Pythonic alias)")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error (Pythonic alias)")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("get_brain", [](AgentBrain& self) {
            return self.getBrain();
        }, "Get the underlying brain (Pythonic alias)")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enable_reward_modulation", [](AgentBrain& self, bool enable) {
            self.enableRewardModulation(enable);
        }, py::arg("enable"),
           "Enable/disable reward modulation (Pythonic alias)")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enable_structural_plasticity", [](AgentBrain& self, bool enable) {
            self.enableStructuralPlasticity(enable);
        }, py::arg("enable"),
           "Enable/disable structural plasticity (Pythonic alias)")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development")
        .def("enable_development", [](AgentBrain& self, bool enable) {
            self.enableDevelopment(enable);
        }, py::arg("enable"),
           "Enable/disable development (Pythonic alias)")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity")
        .def("enable_curiosity", [](AgentBrain& self, bool enable) {
            self.enableCuriosity(enable);
        }, py::arg("enable"),
           "Enable/disable curiosity (Pythonic alias)")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled (Pythonic alias)")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled (Pythonic alias)")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled (Pythonic alias)")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled (Pythonic alias)")
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
             "Get thought trace (diagnostic)")
        .def("get_thought_trace", &AgentBrain::getThoughtTrace,
             "Get thought trace (diagnostic) (Pythonic alias)")
        .def("getLearnedPreference", &AgentBrain::getLearnedPreference,
             "Get learned preference trait")
        .def("get_learned_preference", &AgentBrain::getLearnedPreference,
             "Get learned preference trait (Pythonic alias)")
        // Phase 5 diagnostic counters
        .def("get_reflex_bonus_count", &AgentBrain::getReflexBonusCount,
             "Get vision-guided reflex bonus count")
        .def("get_reflex_suppress_count", &AgentBrain::getReflexSuppressCount,
             "Get hazard suppression count")
        .def("get_wta_pulse_count", &AgentBrain::getWtaPulseCount,
             "Get winner-take-all pulse count")
        // Phase 6 text encoding
        .def("encode_text", &AgentBrain::encodeText, py::arg("text"),
             "Encode text into neuron pattern")
        .def("encode_text_py", [](const std::string& text) {
            return AgentBrain::encodeText(text);
        }, py::arg("text"),
           "Encode text into neuron pattern (standalone Pythonic function)")
        .def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
             "Get internal firing rates for text recall")
        .def("get_internal_firing_rates", &AgentBrain::getInternalFiringRates,
             "Get internal firing rates for text recall (Pythonic alias)")
        // Phase 6 cognition counters
        .def("get_planner_override_count", &AgentBrain::getPlannerOverrideCount,
             "Get planner override count")
        .def("get_planner_agreement_count", &AgentBrain::getPlannerAgreementCount,
             "Get planner agreement count")
        .def("get_planner_decision_count", &AgentBrain::getPlannerDecisionCount,
             "Get planner decision count")
        .def("get_concept_bias_count", &AgentBrain::getConceptBiasCount,
             "Get concept bias count")
        .def("get_self_bias_count", &AgentBrain::getSelfBiasCount,
             "Get self-bias count")
        .def("get_social_bias_count", &AgentBrain::getSocialBiasCount,
             "Get social bias count")
        // Phase 6 diagnostic
        .def("getConceptCount", &AgentBrain::getConceptCount,
             "Get concept count")
        .def("get_concept_count", &AgentBrain::getConceptCount,
             "Get concept count (Pythonic alias)")
        .def("getSelfModelConfidence", &AgentBrain::getSelfModelConfidence,
             "Get self-model confidence")
        .def("get_self_model_confidence", &AgentBrain::getSelfModelConfidence,
             "Get self-model confidence (Pythonic alias)")
        .def("getSocialObservationCount", &AgentBrain::getSocialObservationCount,
             "Get social observation count")
        .def("get_social_observation_count", &AgentBrain::getSocialObservationCount,
             "Get social observation count (Pythonic alias)")
        // Convenience methods
        .def("run_episode", [](AgentBrain& self, const SimpleWorld& world, int num_steps) {
            for (int step = 0; step < num_steps; ++step) {
                // Simulate one step of agent interaction
                // Note: This is a simplified version - full episode would need access to world
                self.updateDevelopment(0.1);
            }
        }, py::arg("world"), py::arg("num_steps"),
           "Run an episode of length num_steps")
        .def("get_whole_brain_state", [](const AgentBrain& self) {
            py::dict state;
            auto brain = self.getBrain();
            if (brain) {
                state["total_neurons"] = brain->getTotalNeuronCount();
                state["total_synapses"] = brain->getTotalSynapseCount();
                state["firing_neurons"] = brain->getFiringNeuronCount();
                state["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
                state["neuromodulation_level"] = self.getNeuromodulationLevel();
                state["curiosity_level"] = self.getCuriosityLevel();
                state["novelty_level"] = self.getNoveltyLevel();
                state["prediction_error"] = self.getPredictionError();
            }
            return state;
        }, "Get complete brain state as dictionary")
        .def("is_healthy", [](const AgentBrain& self) {
            auto brain = self.getBrain();
            if (!brain) return false;
            return brain->getTotalNeuronCount() > 0 && 
                   brain->getFiringNeuronCount() >= 0;
        }, "Check if brain is in a healthy state");
}

} // namespace nlm
