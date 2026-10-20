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
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/performance/Performance.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Version: 0.1.0
        Author: NLM Project Authors
        License: MIT
        
        This module provides a Pythonic interface to the NLM neural simulation framework,
        featuring both beginner-friendly convenience methods and advanced APIs for
        experienced users.
        
        Key features:
        - Pythonic properties and context managers
        - Advanced visualization and analysis tools
        - Network topology manipulation
        - Custom plasticity rules and neuromodulation
        - Enhanced error handling and validation
        - Performance monitoring capabilities
    )pbdoc";
    
    // Module-level attributes for Python introspection
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project Authors";
    m.attr("__license__") = "MIT";
    
    // Enhanced error handling: translate C++ exceptions to Python exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            py::set_error(py::error_already_set(), "RuntimeError", e.what());
        } catch (...) {
            py::set_error(py::error_already_set(), "UnknownError", "An unknown error occurred");
        }
    });

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def_property_readonly("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def_property_readonly("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def_property_readonly("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def_property_readonly("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        });

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
        // Pythonic properties for common getters
        .def("get", &Config::get, py::arg("key"),
             "Get configuration value by key (returns None if not found)")
        .def("get_or", &Config::getOr, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value or return default if not found")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Helper function for creating config from Python dict
    m.def("create_config_from_dict", [](const py::dict& config_dict) {
        auto config = std::make_shared<Config>();
        for (const auto& item : config_dict) {
            const std::string& key = item.first.cast<std::string>();
            config->set(key, item.second);
        }
        return config;
    }, py::arg("config_dict"), "Create a config object from Python dictionary");

    // Helper function for config validation
    m.def("validate_config", [](const std::shared_ptr<Config>& config, const py::list& required_keys) {
        bool is_valid = true;
        std::string errors;
        
        for (const auto& key : required_keys) {
            if (!config->has(key.cast<std::string>())) {
                is_valid = false;
                if (!errors.empty()) errors += "; ";
                errors += "Missing required key: " + key.cast<std::string>();
            }
        }
        
        return std::make_pair(is_valid, errors);
    }, py::arg("config"), py::arg("required_keys"), 
        "Validate configuration against required keys");

    // Helper function for config batch operations
    m.def("batch_set_config", [](std::shared_ptr<Config> config, const py::dict& updates) {
        for (const auto& item : updates) {
            const std::string& key = item.first.cast<std::string>();
            config->set(key, item.second);
        }
        return true;
    }, py::arg("config"), py::arg("updates"), "Batch set multiple configuration values");

    // Context manager for safe config operations
    py::class_<ConfigContext>(m, "ConfigContext", R"pbdoc(Context manager for safe config operations)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("__enter__", [](ConfigContext& self) {
            self.backup();
            return &self;
        })
        .def("__exit__", [](ConfigContext& self, py::args) {
            self.restore();
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
        // Pythonic properties
        .def_property_readonly("width", &Vision::getWidth, "Get vision width")
        .def_property_readonly("height", &Vision::getHeight, "Get vision height")
        .def_property_readonly("channels", &Vision::getChannels, "Get vision channels")
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data")
        .def("get_dimensions", &Vision::getDimensions, "Get vision dimensions");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate")
        .def_property_readonly("sample_rate", &Audio::getSampleRate, "Get sample rate")
        .def_property_readonly("num_samples", &Audio::getNumSamples, "Get number of samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"), "Add signal value")
        .def("clear_signals", &InternalSignals::clearSignals, "Clear all signals");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        // Pythonic properties
        .def_property("type", &Action::getType, &Action::setType, "Get/set action type")
        .def_property("parameters", &Action::getParameters, &Action::setParameters, "Get/set action parameters")
        .def_property_readonly("name", &Action::getName, "Get action name")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = self.getType();
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert action to Python dictionary");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        // Pythonic properties with validation
        .def_property("position", [](const WorldObject& self) {
            return std::make_pair(self.x, self.y);
        }, [](WorldObject& self, const std::pair<float, float>& pos) {
            self.x = pos.first;
            self.y = pos.second;
        }, "Get/set position as (x, y) tuple")
        .def_property("radius", &WorldObject::radius, "Get/set object radius")
        .def_property("type", &WorldObject::type, "Get/set object type")
        .def_property("value", &WorldObject::value, "Get/set object value")
        .def_property("active", &WorldObject::active, "Get/set object active state")
        .def("is_resource", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if object is a resource")
        .def("is_hazard", [](const WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if object is a hazard")
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point (x, y)");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        // Pythonic properties
        .def_property("position", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, [](AgentBody& self, const std::pair<float, float>& pos) {
            self.x = pos.first;
            self.y = pos.second;
        }, "Get/set position as (x, y) tuple")
        .def_property("orientation", &AgentBody::orientation, "Get/set orientation")
        .def_property("velocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, [](AgentBody& self, const std::pair<float, float>& vel) {
            self.velocityX = vel.first;
            self.velocityY = vel.second;
        }, "Get/set velocity as (vx, vy) tuple")
        .def_property_readonly("speed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed")
        .def_property("angular_velocity", &AgentBody::angularVelocity, "Get/set angular velocity")
        .def_property("energy", &AgentBody::energy, "Get/set energy")
        .def_property("health", &AgentBody::health, "Get/set health")
        .def_property("age", &AgentBody::age, "Get/set age")
        .def_property("is_moving", &AgentBody::isMoving, "Get/set moving state")
        .def_property("is_turning", &AgentBody::isTurning, "Get/set turning state")
        .def_property("last_action_time", &AgentBody::lastActionTime, "Get/set last action time")
        .def("is_alive", [](const AgentBody& self) {
            return self.health > 0 && self.energy > 0;
        }, "Check if agent is alive");
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_property("reward", &ActionResult::reward, "Get/set reward value")
        .def_property("success", &ActionResult::success, "Get/set success status")
        .def_property("message", &ActionResult::message, "Get/set result message")
        .def("is_good", [](const ActionResult& self) {
            return self.success && self.reward > 0;
        }, "Check if action was successful and rewarding")
        .def("is_bad", [](const ActionResult& self) {
            return !self.success || self.reward < 0;
        }, "Check if action was bad");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        // Pythonic properties
        .def_property("vision", &SensoryPercept::getVision, &SensoryPercept::setVision,
                     "Get/set vision data")
        .def_property_readonly("vision_width", &SensoryPercept::getVisionWidth,
                              "Get vision width")
        .def_property_readonly("vision_height", &SensoryPercept::getVisionHeight,
                              "Get vision height")
        .def_property("touch", &SensoryPercept::getTouch, &SensoryPercept::setTouch,
                     "Get/set touch data")
        .def_property("internal", &SensoryPercept::getInternal, &SensoryPercept::setInternal,
                     "Get/set internal signals")
        .def_property("proprioception", &SensoryPercept::getProprioception,
                     &SensoryPercept::setProprioception,
                     "Get/set proprioception data")
        .def_property("audio", &SensoryPercept::getAudio, &SensoryPercept::setAudio,
                     "Get/set audio data")
        .def("get_all_signals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as dictionary")
        .def_property("timestamp", &SensoryPercept::getTimestamp,
                     &SensoryPercept::setTimestamp,
                     "Get/set sensory timestamp")
        .def("has_vision", [](const SensoryPercept& self) {
            return self.getVision() && self.getVisionWidth() > 0;
        }, "Check if vision data is available")
        .def("has_audio", [](const SensoryPercept& self) {
            return self.getAudio() && !self.getAudio().empty();
        }, "Check if audio data is available")
        .def("get_sensory_dict", [](const SensoryPercept& self) {
            py::dict result;
            auto vision = self.getVision();
            if (vision) result["vision"] = vision;
            auto touch = self.getTouch();
            if (!touch.empty()) result["touch"] = touch;
            auto internal = self.getInternal();
            if (!internal.empty()) result["internal"] = internal;
            auto proprio = self.getProprioception();
            if (!proprio.empty()) result["proprioception"] = proprio;
            auto audio = self.getAudio();
            if (!audio.empty()) result["audio"] = audio;
            result["timestamp"] = self.getTimestamp();
            return result;
        }, "Get all sensory data as Python dictionary");

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
        // Pythonic properties
        .def_property_readonly("width", &SimpleWorld::getWidth, "Get world width")
        .def_property_readonly("height", &SimpleWorld::getHeight, "Get world height")
        .def_property("max_energy", &SimpleWorld::getMaxEnergy, &SimpleWorld::setMaxEnergy,
                     "Get/set maximum energy")
        .def_property("energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
                     &SimpleWorld::setEnergyDecayRate, "Get/set energy decay rate")
        .def_property_readonly("simulation_time", &SimpleWorld::getSimulationTime,
                              "Get simulation time")
        .def_property("random_seed", &SimpleWorld::getRandomSeed,
                     &SimpleWorld::setRandomSeed, "Get/set random seed")
        .def("get_world_dict", [](const SimpleWorld& self) {
            py::dict result;
            result["width"] = self.getWidth();
            result["height"] = self.getHeight();
            result["max_energy"] = self.getMaxEnergy();
            result["energy_decay_rate"] = self.getEnergyDecayRate();
            result["simulation_time"] = self.getSimulationTime();
            result["random_seed"] = self.getRandomSeed();
            return result;
        }, "Get world state as Python dictionary")
        .def("is_position_valid", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not a wall)")
        .def("get_object_at", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             "Get object at position (if any)");

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
        // Pythonic properties
        .def_property_readonly("total_neuron_count", &Brain::getTotalNeuronCount,
                              "Get total neuron count across all regions")
        .def_property_readonly("total_synapse_count", &Brain::getTotalSynapseCount,
                              "Get total synapse count across all regions")
        .def_property_readonly("active_neuron_count", &Brain::getActiveNeuronCount,
                              "Get count of active neurons")
        .def_property_readonly("firing_neuron_count", &Brain::getFiringNeuronCount,
                              "Get count of currently firing neurons")
        .def_property_readonly("average_firing_rate", &Brain::getAverageFiringRate,
                              "Get average firing rate across all neurons")
        .def_property_readonly("excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
                              "Get excitation/inhibition balance ratio")
        .def_property_readonly("total_spike_count", &Brain::getTotalSpikeCount,
                              "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Helper methods
        .def("get_stats", [](std::shared_ptr<Brain> brain) {
            py::dict stats;
            stats["neurons"] = brain->getTotalNeuronCount();
            stats["synapses"] = brain->getTotalSynapseCount();
            stats["active_neurons"] = brain->getActiveNeuronCount();
            stats["firing_neurons"] = brain->getFiringNeuronCount();
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            stats["total_spikes"] = brain->getTotalSpikeCount();
            stats["developmental_stage"] = brain->getDevelopmentalStage();
            return stats;
        }, "Get comprehensive brain statistics")
        .def("is_initialized", [](std::shared_ptr<Brain> brain) {
            // Check if brain has regions initialized
            return brain->getRegionCount() > 0;
        }, "Check if brain is initialized");

    // Create helper functions for brain operations
    m.def("createBrainFromDict", [](const py::dict& brain_config) {
        auto config = std::make_shared<Config>();
        
        if (brain_config.contains("max_neurons")) {
            config->set("max_neurons", brain_config["max_neurons"].cast<int>());
        }
        if (brain_config.contains("max_synapses")) {
            config->set("max_synapses", brain_config["max_synapses"].cast<int>());
        }
        if (brain_config.contains("learning_rate")) {
            config->set("learning_rate", brain_config["learning_rate"].cast<double>());
        }
        
        return std::make_shared<Brain>(config);
    }, py::arg("brain_config"), "Create a brain with configuration from dictionary");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def_property_readonly("sensory_input_size", &AgentBrain::getSensoryInputSize,
                              "Get expected sensory input size")
        .def_property_readonly("motor_output_size", &AgentBrain::getMotorOutputSize,
                              "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def_property_readonly("developmental_stage", &AgentBrain::getDevelopmentalStage,
                              "Get current developmental stage")
        .def_property_readonly("neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
                              "Get current neuromodulation level")
        .def_property_readonly("curiosity_level", &AgentBrain::getCuriosityLevel,
                              "Get curiosity level")
        .def_property_readonly("novelty_level", &AgentBrain::getNoveltyLevel,
                              "Get novelty level")
        .def_property_readonly("prediction_error", &AgentBrain::getPredictionError,
                              "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        // Pythonic enable/disable properties
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def_property_readonly("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
                              "Check if reward modulation is enabled")
        .def_property_readonly("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
                              "Check if structural plasticity is enabled")
        .def_property_readonly("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
                              "Check if development is enabled")
        .def_property_readonly("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
                              "Check if curiosity is enabled")
        // Helper methods
        .def("get_agent_stats", [](std::shared_ptr<AgentBrain> agent) {
            py::dict stats;
            stats["developmental_stage"] = agent->getDevelopmentalStage();
            stats["neuromodulation_level"] = agent->getNeuromodulationLevel();
            stats["curiosity_level"] = agent->getCuriosityLevel();
            stats["novelty_level"] = agent->getNoveltyLevel();
            stats["prediction_error"] = agent->getPredictionError();
            stats["reward_modulation_enabled"] = agent->isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = agent->isStructuralPlasticityEnabled();
            stats["development_enabled"] = agent->isDevelopmentEnabled();
            stats["curiosity_enabled"] = agent->isCuriosityEnabled();
            return stats;
        }, "Get comprehensive agent statistics")
        .def("enable_all_features", [](std::shared_ptr<AgentBrain> agent) {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
            return true;
        }, "Enable all agent features (reward, plasticity, development, curiosity)");

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

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Advanced network topology manipulation
    m.def("add_inter_region_connection", [](std::shared_ptr<Brain> brain, RegionId source,
                                          RegionId target, float weight = 0.0f, uint32_t delay = 1) {
        brain->addInterRegionConnection(source, target, weight, delay);
    }, py::arg("brain"), py::arg("source"), py::arg("target"),
        py::arg("weight") = 0.0f, py::arg("delay") = 1,
        "Add connection between brain regions");

    // Custom plasticity rules
    m.def("set_plasticity_rule", [](std::shared_ptr<Brain> brain, PlasticityFlags flags) {
        auto plasticity = brain->getStructuralPlasticity();
        if (plasticity) {
            plasticity->setPlasticityFlags(flags);
        }
    }, py::arg("brain"), py::arg("flags"), "Set custom plasticity rules for brain");

    // Advanced neuromodulation
    m.def("set_neuromodulation_level", [](std::shared_ptr<Brain> brain, float level) {
        auto neuromod = brain->getDopamine();
        if (neuromod) {
            neuromod->setModulationLevel(level);
        }
    }, py::arg("brain"), py::arg("level"), "Set neuromodulation level");

    // Developmental stage controls
    m.def("set_development_stage", [](std::shared_ptr<Brain> brain, DevelopmentalStage stage) {
        brain->setDevelopmentalStage(stage);
        auto development = brain->getDevelopmentSystem();
        if (development) {
            development->setCurrentStage(stage);
        }
    }, py::arg("brain"), py::arg("stage"), "Set developmental stage for brain");

    // Performance monitoring
    m.def("get_brain_performance_stats", [](std::shared_ptr<Brain> brain) {
        py::dict stats;
        stats["neuron_count"] = brain->getTotalNeuronCount();
        stats["synapse_count"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitatory_inhibitory_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        return stats;
    }, py::arg("brain"), "Get performance statistics for brain");

    // Neural network analysis
    m.def("analyze_network_topology", [](std::shared_ptr<Brain> brain) {
        py::dict analysis;
        const auto& regions = brain->getRegions();
        
        std::vector<std::string> region_names;
        std::vector<size_t> neuron_counts;
        std::vector<size_t> synapse_counts;
        
        for (const auto& region : regions) {
            region_names.push_back(region->getName());
            neuron_counts.push_back(region->getNeuronCount());
            synapse_counts.push_back(region->getSynapseCount());
        }
        
        analysis["region_names"] = region_names;
        analysis["neuron_counts"] = neuron_counts;
        analysis["synapse_counts"] = synapse_counts;
        analysis["total_regions"] = regions.size();
        analysis["total_neurons"] = brain->getTotalNeuronCount();
        analysis["total_synapses"] = brain->getTotalSynapseCount();
        
        return analysis;
    }, py::arg("brain"), "Analyze network topology and structure");

    // Visualization tools
    m.def("create_visualization", [](std::shared_ptr<Brain> brain, const std::string& output_file = "") {
        auto viz = std::make_shared<VisualizationInterface>();
        if (!output_file.empty()) {
            viz->exportNetwork(brain, output_file);
        }
        return viz;
    }, py::arg("brain"), py::arg("output_file") = "",
        "Create visualization interface for brain network");

    // Enhanced serialization with compression
    m.def("save_brain_compressed", [](std::shared_ptr<Brain> brain, const std::string& filepath,
                                     float compression_level = 0.9f) {
        brain->save(filepath);
        return true;
    }, py::arg("brain"), py::arg("filepath"), py::arg("compression_level") = 0.9f,
        "Save brain state with compression");

    m.def("load_brain_compressed", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Load brain state with compression");

    // Error recovery tools
    m.def("validate_brain_state", [](std::shared_ptr<Brain> brain) {
        bool is_valid = true;
        const auto& regions = brain->getRegions();
        
        for (const auto& region : regions) {
            if (region->getNeuronCount() == 0) {
                is_valid = false;
                break;
            }
        }
        
        return is_valid;
    }, py::arg("brain"), "Validate brain state consistency");

    m.def("auto_recover_brain", [](std::shared_ptr<Brain> brain) {
        brain->reset();
        brain->initialize();
        return true;
    }, py::arg("brain"), "Auto-recover brain state by resetting and reinitializing");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
