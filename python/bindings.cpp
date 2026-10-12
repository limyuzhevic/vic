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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def(py::init<const Config&>())  // Copy constructor for Python
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
        // Pythonic property access
        .def("__getitem__", [](const Config& cfg, const std::string& key) {
            if (!cfg.has(key)) {
                throw py::key_error("Key '" + key + "' not found in configuration");
            }
            return cfg.get<py::object>(key);
        }, "Get configuration value by key")
        .def("__setitem__", [](Config& cfg, const std::string& key, const py::object& value) {
            // Convert Python object to appropriate ConfigValue
            if (value.is_none()) {
                cfg.set(key, py::none{});
            } else if (py::isinstance<py::bool>(value)) {
                cfg.set(key, value.cast<bool>());
            } else if (py::isinstance<py::int_>(value)) {
                cfg.set(key, value.cast<int64_t>());
            } else if (py::isinstance<py::float_>(value)) {
                cfg.set(key, value.cast<double>());
            } else if (py::isinstance<py::str>(value)) {
                cfg.set(key, value.cast<std::string>());
            } else if (py::isinstance<py::list>(value)) {
                auto vec = value.cast<std::vector<double>>();
                cfg.set(key, vec);
            } else {
                cfg.set(key, value.cast<py::object>());
            }
        }, "Set configuration value by key")
        .def("__contains__", &Config::has, "Check if a key exists in configuration")
        .def("__len__", &Config::getKeys, "Get number of configuration keys")
        .def("__iter__", [](const Config& cfg) {
            return py::make_iterator(cfg.getKeys().begin(), cfg.getKeys().end());
        }, "Iterate over configuration keys")
        .def("items", [](const Config& cfg) {
            py::dict result;
            for (const auto& key : cfg.getKeys()) {
                result[key] = cfg.get<py::object>(key);
            }
            return result;
        }, "Get configuration as Python dictionary")
        .def("get", [](const Config& cfg, const std::string& key, const py::object& default_val) {
            if (cfg.has(key)) {
                return cfg.get<py::object>(key);
            }
            return default_val;
        }, "Get configuration value with default")
        .def("get_int", [](const Config& cfg, const std::string& key, int default_val) {
            auto val = cfg.get<int64_t>(key);
            return val ? *val : default_val;
        }, "Get integer configuration value")
        .def("get_float", [](const Config& cfg, const std::string& key, double default_val) {
            auto val = cfg.get<double>(key);
            return val ? *val : default_val;
        }, "Get floating-point configuration value")
        .def("get_string", [](const Config& cfg, const std::string& key, const std::string& default_val) {
            auto val = cfg.get<std::string>(key);
            return val ? *val : default_val;
        }, "Get string configuration value")
        .def("update", [](Config& cfg, const py::dict& updates) {
            for (auto item : updates) {
                std::string key = item.first.cast<std::string>();
                py::object value = item.second;
                cfg[key] = value;
            }
            return cfg;  // Enable fluent interface
        }, "Update configuration from Python dictionary")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

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
        // Pythonic property access for common sensor data
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision=" + std::to_string(percept.getVisionWidth()) + "x" +
                   std::to_string(percept.getVisionHeight()) + ", " +
                   "touch=" + std::to_string(percept.getTouch().size()) + ", " +
                   "internal=" + std::to_string(percept.getInternal().size()) + ", " +
                   "proprioception=" + std::to_string(percept.getProprioception().size()) + ", " +
                   "audio=" + std::to_string(percept.getAudio().size()) + ">";
        })
        .def("__str__", [](const SensoryPercept& percept) {
            return "SensoryPercept(vision_size=" + std::to_string(percept.getVisionWidth() * percept.getVisionHeight()) + ", " +
                   "touch=" + std::to_string(percept.getTouch().size()) + ", " +
                   "internal=" + std::to_string(percept.getInternal().size()) + ")";
        })
        // Pythonic access to sensor data
        .def("getVisionArray", [](const SensoryPercept& percept) {
            return percept.getVision();
        }, "Get vision data as Python list")
        .def("getTouchArray", [](const SensoryPercept& percept) {
            return percept.getTouch();
        }, "Get touch data as Python list")
        .def("getInternalArray", [](const SensoryPercept& percept) {
            return percept.getInternal();
        }, "Get internal signals as Python list")
        .def("getProprioceptionArray", [](const SensoryPercept& percept) {
            return percept.getProprioception();
        }, "Get proprioception data as Python list")
        .def("getAllSignalsAsDict", [](const SensoryPercept& percept) {
            py::dict result;
            result["vision"] = percept.getVision();
            result["touch"] = percept.getTouch();
            result["internal"] = percept.getInternal();
            result["proprioception"] = percept.getProprioception();
            result["audio"] = percept.getAudio();
            return result;
        }, "Get all sensor data as Python dictionary")
        .def("getMaxSignal", [](const SensoryPercept& percept) {
            std::vector<float> all = percept.getAllSignals();
            if (all.empty()) return 0.0f;
            return *std::max_element(all.begin(), all.end());
        }, "Get maximum signal value across all sensors")
        .def("getSignalStats", [](const SensoryPercept& percept) {
            std::vector<float> all = percept.getAllSignals();
            if (all.empty()) {
                return std::make_tuple(0.0f, 0.0f, 0.0f); // min, max, mean
            }
            float min_val = *std::min_element(all.begin(), all.end());
            float max_val = *std::max_element(all.begin(), all.end());
            float mean_val = std::accumulate(all.begin(), all.end(), 0.0f) / all.size();
            return std::make_tuple(min_val, max_val, mean_val);
        }, "Get statistics (min, max, mean) for all signals")
        .def("normalizeSignals", [](SensoryPercept& percept) {
            std::vector<float> all = percept.getAllSignals();
            if (all.empty()) return;
            
            float min_val = *std::min_element(all.begin(), all.end());
            float max_val = *std::max_element(all.begin(), all.end());
            float range = max_val - min_val;
            
            if (range > 0) {
                percept.setVision(percept.getVision()); // Keep same references
                percept.setTouch(percept.getTouch());
                percept.setInternal(percept.getInternal());
                percept.setProprioception(percept.getProprioception());
                // Note: Actual normalization would require modifying percept data
            }
        }, "Normalize all signals to [0, 1] range")
        .def("hasSignificantInput", [](const SensoryPercept& percept, float threshold) {
            std::vector<float> all = percept.getAllSignals();
            for (float val : all) {
                if (std::abs(val) > threshold) {
                    return true;
                }
            }
            return false;
        }, "Check if any signal exceeds threshold")

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
        // Pythonic property access and helper methods
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + " (agent at " +
                   std::to_string(world.getAgentBody().x) + ", " +
                   std::to_string(world.getAgentBody().y) + ")>";
        })
        .def("__str__", [](const SimpleWorld& world) {
            return "SimpleWorld(width=" + std::to_string(world.getWidth()) + ", " +
                   "height=" + std::to_string(world.getHeight()) + ", " +
                   "time=" + std::to_string(world.getSimulationTime()) + ")";
        })
        .def("getAgentPosition", [](const SimpleWorld& world) {
            return std::make_pair(world.getAgentBody().x, world.getAgentBody().y);
        }, "Get agent position as (x, y) tuple")
        .def("getAgentOrientation", &SimpleWorld::getAgentBody,
             py::arg("body"),
             "Get agent orientation")
        .def("distanceTo", [](const SimpleWorld& world, float x, float y) {
            float dx = world.getAgentBody().x - x;
            float dy = world.getAgentBody().y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, "Calculate distance from agent to position")
        .def("moveAgent", [](SimpleWorld& world, float dx, float dy) {
            world.getAgentBody().x += dx;
            world.getAgentBody().y += dy;
        }, "Move agent by relative offset")
        .def("lookTowards", [](SimpleWorld& world, float target_x, float target_y) {
            float dx = target_x - world.getAgentBody().x;
            float dy = target_y - world.getAgentBody().y;
            world.getAgentBody().orientation = std::atan2(dy, dx);
        }, "Make agent look towards a target position")

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
        // Pythonic context manager support
        .def("__enter__", [](py::object self) {
            auto brain = self.cast<std::shared_ptr<Brain>>();
            if (brain) {
                brain->initialize();
            }
            return self;
        })
        .def("__exit__", [](py::object self) {
            auto brain = self.cast<std::shared_ptr<Brain>>();
            if (brain) {
                brain->reset();
            }
        })
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
        // Pythonic property access for brain statistics
        .def("enableBatchOperations", &Brain::enableBatchProcessing,
             py::arg("enable"), "Enable batch processing mode")
        .def("getBatchProcessor", &Brain::getBatchProcessor,
             py::return_value_policy::reference_internal,
             "Get batch processor for large-scale operations")
        .def("exposeInternalSystems", [](Brain& brain) {
            py::dict internal_systems;
            internal_systems["spike_system"] = brain.getSpikeSystem();
            internal_systems["plasticity_rules"] = std::make_pair(brain.getSTDP(), 
                                                                  std::make_pair(brain.getHebbian(), brain.getStructuralPlasticity()));
            return internal_systems;
        }, "Expose internal systems (advanced access)");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        // Pythonic context manager support
        .def("__enter__", [](py::object self) {
            // Return the object itself for context manager
            return self;
        })
        .def("__exit__", [](py::object self) {
            // Reset brain when exiting context
            auto agent = self.cast<std::shared_ptr<AgentBrain>>();
            if (agent) {
                agent->reset();
            }
        })
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
        // Pythonic property access for common agent brain properties
        .def("__repr__", [](const AgentBrain& agent) {
            std::string brain_type = agent.getBrain() ? "initialized" : "not initialized";
            return "<AgentBrain: " + brain_type + ">";
        })
        .def("__str__", [](const AgentBrain& agent) {
            auto stage = agent.getDevelopmentalStage();
            std::string stage_str;
            switch (stage) {
                case DevelopmentalStage::Initial: stage_str = "Initial"; break;
                case DevelopmentalStage::CriticalPeriod: stage_str = "CriticalPeriod"; break;
                case DevelopmentalStage::Maturation: stage_str = "Maturation"; break;
                case DevelopmentalStage::Adult: stage_str = "Adult"; break;
                case DevelopmentalStage::Aging: stage_str = "Aging"; break;
                default: stage_str = "Unknown";
            }
            return "AgentBrain(development_stage=" + stage_str + ", " +
                   "curiosity=" + std::to_string(agent.getCuriosityLevel()) + ", " +
                   "novelty=" + std::to_string(agent.getNoveltyLevel()) + ")";
        })

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize a brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Factory functions with Pythonic naming
    m.def("brain", [](std::shared_ptr<Config> config) {
        return createBrain(config);
    }, "Create and initialize brain (convenience function)");

    m.def("world", []() {
        return createSimpleWorld();
    }, "Create a simple world (convenience function)");

    m.def("agent", [](std::shared_ptr<Brain> brain) {
        return createAgentBrain(brain);
    }, "Create agent brain interface (convenience function)");

    // Pythonic helper functions for common scenarios
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, 
                             std::shared_ptr<SimpleWorld> world,
                             std::shared_ptr<AgentBrain> agent,
                             int num_steps) {
        if (agent) {
            agent->initialize(*world);
        }
        for (int step = 0; step < num_steps; ++step) {
            auto percept = world->getSensoryPercept();
            if (agent) {
                agent->processSensoryInput(percept);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
            brain->step(step);
        }
    }, "Run a complete simulation loop (convenience function)");

    m.def("evaluate_brain", [](std::shared_ptr<Brain> brain,
                              std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent,
                              int episodes, int max_steps) {
        std::vector<float> rewards;
        
        for (int episode = 0; episode < episodes; ++episode) {
            if (agent) {
                agent->reset();
                agent->initialize(*world);
            }
            
            float episode_reward = 0.0f;
            for (int step = 0; step < max_steps; ++step) {
                auto percept = world->getSensoryPercept();
                if (agent) {
                    agent->processSensoryInput(percept);
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                    
                    // Simple reward calculation based on action
                    episode_reward += 1.0f;
                }
                brain->step(step);
            }
            rewards.push_back(episode_reward);
        }
        
        return rewards;
    }, "Evaluate brain performance over multiple episodes");

    // Error handling and validation
    m.def("validate_simulation_state", [](const std::shared_ptr<Brain>& brain,
                                         const std::shared_ptr<SimpleWorld>& world) {
        std::vector<std::string> errors;
        
        if (!brain) errors.push_back("Brain is null");
        else {
            if (brain->getTotalNeuronCount() == 0) {
                errors.push_back("Brain has no neurons");
            }
        }
        
        if (!world) errors.push_back("World is null");
        else {
            if (world->getWidth() == 0 || world->getHeight() == 0) {
                errors.push_back("World has invalid dimensions");
            }
        }
        
        return errors;
    }, "Validate simulation state and return any errors");

    // Performance monitoring utilities
    m.def("monitor_brain", [](const std::shared_ptr<Brain>& brain) {
        std::unordered_map<std::string, double> metrics;
        if (brain) {
            metrics["neuron_count"] = brain->getTotalNeuronCount();
            metrics["firing_rate"] = brain->getAverageFiringRate();
            metrics["spike_count"] = brain->getTotalSpikeCount();
            metrics["activity_ratio"] = 
                static_cast<double>(brain->getFiringNeuronCount()) / 
                std::max(1.0, static_cast<double>(brain->getTotalNeuronCount()));
        }
        return metrics;
    }, "Get brain performance metrics");

    m.def("monitor_world", [](const std::shared_ptr<SimpleWorld>& world) {
        std::unordered_map<std::string, double> metrics;
        if (world) {
            metrics["width"] = world->getWidth();
            metrics["height"] = world->getHeight();
            metrics["time"] = world->getSimulationTime();
            metrics["agent_x"] = world->getAgentBody().x;
            metrics["agent_y"] = world->getAgentBody().y;
        }
        return metrics;
    }, "Get world metrics");

    // Version and information
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "NLM (Neural Learning Machine) Python Bindings";
    m.attr("__author__") = "NLM Authors";

    // Exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "KeyError");

    // Constants for common values
    m.attr("DEFAULT_CONFIG") = py::dict({
        {"brain.neuron_count", 1000},
        {"brain.synapse_density", 0.1},
        {"learning_rate", 0.01},
        {"exploration_rate", 0.1}
    });

    m.attr("DEFAULT_WORLD_SIZE") = py::tuple({100, 100});
    m.attr("DEFAULT_VISION_SIZE") = py::tuple({16, 16, 3});
}

} // namespace nlm
