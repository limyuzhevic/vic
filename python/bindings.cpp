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
        
        // Pythonic interface - dict-like access
        .def("__getitem__", [](const Config& self, const std::string& key) {
            auto val = self.get<std::string>(key);
            if (!val) {
                auto valInt = self.get<int64_t>(key);
                if (valInt) return py::int_(valInt.value());
                auto valFloat = self.get<double>(key);
                if (valFloat) return py::float_(valFloat.value());
                auto valBool = self.get<bool>(key);
                if (valBool) return py::bool_(valBool.value());
                throw py::key_error("Key not found: " + key);
            }
            return py::str(val.value());
        })
        
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Default);
        })
        
        .def("__setitem__", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Default);
        })
        
        .def("__setitem__", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::Default);
        })
        
        .def("__setitem__", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Default);
        })
        
        .def("__contains__", &Config::has, py::arg("key"))
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        
        // Additional Pythonic convenience methods
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue) {
            auto val = self.get<int64_t>(key);
            return val.value_or(defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0)
        
        .def("getFloat", [](const Config& self, const std::string& key, double defaultValue) {
            auto val = self.get<double>(key);
            return val.value_or(defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0.0)
        
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
            auto val = self.get<bool>(key);
            return val.value_or(defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = false)
        
        .def("configure", [](Config& self, const py::dict& settings) {
            for (auto item : settings) {
                std::string key = py::cast<std::string>(item.first);
                auto value = item.second;
                if (py::isinstance(value, py::int_())) {
                    self.set(key, py::cast<int64_t>(value), ConfigSource::Default);
                } else if (py::isinstance(value, py::float_())) {
                    self.set(key, py::cast<double>(value), ConfigSource::Default);
                } else if (py::isinstance(value, py::bool_())) {
                    self.set(key, py::cast<bool>(value), ConfigSource::Default);
                } else {
                    self.set(key, py::cast<std::string>(value), ConfigSource::Default);
                }
            }
        }, py::arg("settings"));

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
        
        // Pythonic property accessors
        .def("type", py::overload_cast<>(&Action::getType), "Get action type")
        .def("set_type", &Action::setType, py::arg("type"), "Set action type")
        .def("parameters", py::overload_cast<>(&Action::getParameters), "Get action parameters")
        .def("set_parameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("name", &Action::getName, "Get action name")
        
        // Pythonic factory methods
        .def("create_move", [](const std::string& direction, float speed = 1.0f) {
            ActionType type;
            if (direction == "forward" || direction == "moveForward") type = ActionType::MoveForward;
            else if (direction == "backward" || direction == "moveBackward") type = ActionType::MoveBackward;
            else if (direction == "left" || direction == "moveLeft") type = ActionType::MoveLeft;
            else if (direction == "right" || direction == "moveRight") type = ActionType::MoveRight;
            else if (direction == "turnLeft") type = ActionType::TurnLeft;
            else if (direction == "turnRight") type = ActionType::TurnRight;
            else type = ActionType::Custom;
            
            Action action(type);
            if (speed != 1.0f) {
                std::vector<float> params = action.getParameters();
                params.push_back(speed);
                action.setParameters(params);
            }
            return action;
        }, py::arg("direction"), py::arg("speed") = 1.0f,
             "Create movement action with optional speed")
        
        .def("create_look", [](const std::string& direction) {
            ActionType type;
            if (direction == "up" || direction == "lookUp") type = ActionType::LookUp;
            else if (direction == "down" || direction == "lookDown") type = ActionType::LookDown;
            else if (direction == "left" || direction == "lookLeft") type = ActionType::LookLeft;
            else if (direction == "right" || direction == "lookRight") type = ActionType::LookRight;
            else type = ActionType::Look;
            
            return Action(type);
        }, py::arg("direction"),
             "Create look action")
        
        .def("create_interact", [](const std::string& target = "") {
            Action action(ActionType::Interact);
            if (!target.empty()) {
                std::vector<float> params = action.getParameters();
                params.push_back(static_cast<float>(target.size()));
                action.setParameters(params);
            }
            return action;
        }, py::arg("target") = "",
             "Create interaction action with optional target")
        
        // Clone method
        .def("clone", &Action::clone,
             "Create a copy of the action")
        
        // String representation
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        })
        .def("__str__", [](const Action& action) {
            return action.getName();
        })

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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        
        // World control methods
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        
        // Sensory and state access
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        
        // World object management
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        
        // World properties
        .def("width", &SimpleWorld::getWidth,
             "Get world width")
        .def("height", &SimpleWorld::getHeight,
             "Get world height")
        .def("simulation_time", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("max_energy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("random_seed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        
        // Context manager support
        .def("__enter__", [](SimpleWorld& self) -> SimpleWorld& {
            return self;
        })
        .def("__exit__", [](SimpleWorld& self, py::object, py::object, py::object) {
            // Nothing special needed for cleanup
        })
        
        // Convenience methods for common operations
        .def("create_resource", [](SimpleWorld& self, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value"),
             "Create a resource object at position")
        .def("create_hazard", [](SimpleWorld& self, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Hazard, value);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value"),
             "Create a hazard object at position")
        .def("create_wall", [](SimpleWorld& self, float x, float y) {
            WorldObject obj(x, y, WorldObjectType::Wall);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"),
             "Create a wall object at position")

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
        
        // Pythonic property-like accessors for brain statistics
        .def("total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        
        // Brain state manipulation
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        
        // Context manager support for automatic initialization/cleanup
        .def("__enter__", [](Brain& self) -> Brain& {
            self.initialize();
            return self;
        })
        .def("__exit__", [](Brain& self, py::object, py::object, py::object) {
            self.reset();
        })
        
        // Configuration access
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the brain configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status and statistics")
        
        // Additional convenience methods
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current directly into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        
        // Advanced features for expert users
        .def("enable_reward_modulation", &Brain::enableRewardModulation,
             py::arg("enable") = true,
             "Enable or disable reward modulation")
        .def("enable_structural_plasticity", &Brain::enableStructuralPlasticity,
             py::arg("enable") = true,
             "Enable or disable structural plasticity")
        .def("enable_development", &Brain::enableDevelopment,
             py::arg("enable") = true,
             "Enable or disable development system")
        .def("enable_curiosity", &Brain::enableCuriosity,
             py::arg("enable") = true,
             "Enable or disable curiosity system")
        
        // Getters for advanced features
        .def("is_reward_modulation_enabled", &Brain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &Brain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &Brain::isDevelopmentEnabled,
             "Check if development system is enabled")
        .def("is_curiosity_enabled", &Brain::isCuriosityEnabled,
             "Check if curiosity system is enabled")

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        
        // Pythonic property-like accessors for agent brain
        .def("sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        
        // Core agent methods
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        
        // Neuromodulation
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        
        // Development
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        
        // State information
        .def("developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        
        // Agent lifecycle
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        
        // Subsystem controls (Pythonic names)
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable") = true,
             "Enable or disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable") = true,
             "Enable or disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable") = true,
             "Enable or disable development system")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable") = true,
             "Enable or disable curiosity system")
        
        // Subsystem status
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development system is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity system is enabled")

    // Module-level convenience functions for common patterns
    m.def("run_complete_simulation", [](std::shared_ptr<Brain> brain,
                                         std::shared_ptr<SimpleWorld> world,
                                         std::shared_ptr<AgentBrain> agent,
                                         size_t steps) {
        world->reset();
        agent->initialize(*world);
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Optional: yield control back to Python periodically
            if (step % 10 == 0 && Py_IsInitialized()) {
                Py_BEGIN_ALLOW_THREADS
                // Allow Python thread to run
                Py_END_ALLOW_THREADS
            }
        }
        
        return py::dict(
            "brain", brain,
            "world", world,
            "agent", agent,
            "total_spikes", brain->getTotalSpikeCount(),
            "firing_neurons", brain->getFiringNeuronCount(),
            "firing_rate", brain->getAverageFiringRate(),
            "steps_completed", steps
        );
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
         "Run a complete simulation episode with world and agent. Returns results dict.")
    
    m.def("create_brain_with_config", [](const py::dict& config_dict) {
        auto config = std::make_shared<Config>();
        
        for (auto item : config_dict) {
            std::string key = py::cast<std::string>(item.first);
            auto value = item.second;
            if (py::isinstance(value, py::int_())) {
                config->set(key, py::cast<int64_t>(value), ConfigSource::Default);
            } else if (py::isinstance(value, py::float_())) {
                config->set(key, py::cast<double>(value), ConfigSource::Default);
            } else if (py::isinstance(value, py::bool_())) {
                config->set(key, py::cast<bool>(value), ConfigSource::Default);
            } else {
                config->set(key, py::cast<std::string>(value), ConfigSource::Default);
            }
        }
        
        return std::make_shared<Brain>(config);
    }, py::arg("config_dict"),
         "Create a brain with configuration provided as a dict (Pythonic alternative to createDefaultConfig)")
    
    m.def("create_world", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
         "Create and configure a world (convenience function)")
    
    m.def("create_agent", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"),
         "Create an agent brain interface (convenience function)")
    
    m.def("standard_world_config", []() {
        return py::dict(
            ("brain", py::dict(
                ("neuron_count", 500),
                ("connection_probability", 0.05),
                ("v_thresh", -50.0),
                ("v_rest", -70.0),
                ("v_reset", -75.0),
                ("tau_mem", 20.0),
                ("tau_ref", 2.0)
            )),
            ("world", py::dict(
                ("width", 20),
                ("height", 20),
                ("visionWidth", 8),
                ("visionHeight", 8),
                ("max_energy", 100.0),
                ("energy_decay_rate", 0.01)
            ))
        );
    }, "Return a standard configuration dictionary for common use cases");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
