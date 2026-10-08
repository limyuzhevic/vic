#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

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
        
        This module provides a Pythonic interface to the NLM neural simulation framework,
        enabling brain simulation, learning, and experimentation in Python.
        
        Usage:
            import pynlm
            
            # Create a brain
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            # Or use with a world and agent
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Run simulation
            for step in range(100):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
        
        The module follows Python conventions and provides context managers for
        automatic resource cleanup.
    )pbdoc";

    // Register Python exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // NeuronId class
    py::class_<NeuronId>(m, "NeuronId")
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
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // SynapseId class
    py::class_<SynapseId>(m, "SynapseId")
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
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    // RegionId class
    py::class_<RegionId>(m, "RegionId")
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
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enumeration
    py::enum_<ActionType>(m, "ActionType")
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
    py::enum_<MotorCommand>(m, "MotorCommand")
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
    py::enum_<WorldObjectType>(m, "WorldObjectType")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class
    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file
             
             Args:
                 filepath: Path to the JSON configuration file
                 
             Returns:
                 bool: True if loading succeeded, False otherwise
                 
             Raises:
                 ValueError: If file cannot be read or parsed
             )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file
             
             Args:
                 filepath: Path to save the JSON configuration file
                 
             Returns:
                 bool: True if saving succeeded, False otherwise
                 
             Raises:
                 ValueError: If file cannot be written
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists
             
             Args:
                 key: Configuration key to check
                 
             Returns:
                 bool: True if key exists, False otherwise
             )pbdoc")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set a string configuration value
             
             Args:
                 key: Configuration key
                 value: String value to set
                 
             Example:
                 config.set("brain.neuron_count", "1000")
             )pbdoc")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set an integer configuration value
             
             Args:
                 key: Configuration key
                 value: Integer value to set
                 
             Example:
                 config.set("brain.neuron_count", 1000)
             )pbdoc")
        .def("set", [](Config& self, const std::string& key, float value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set a float configuration value
             
             Args:
                 key: Configuration key
                 value: Float value to set
                 
             Example:
                 config.set("brain.connection_probability", 0.1)
             )pbdoc")
        .def("set", [](Config& self, const std::string& key, bool value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set a boolean configuration value
             
             Args:
                 key: Configuration key
                 value: Boolean value to set
                 
             Example:
                 config.set("neuromod.curiosity.enable", True)
             )pbdoc")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) {
                return *value;
            }
            throw py::key_error("Configuration key not found: " + key);
        }, py::arg("key"),
             R"pbdoc(Get a string configuration value
             
             Args:
                 key: Configuration key
                 
             Returns:
                 str: String value
                 
             Raises:
                 KeyError: If key doesn't exist
             )pbdoc")
        .def("get", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            if (value) {
                return *value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
             R"pbdoc(Get a string configuration value with default
             
             Args:
                 key: Configuration key
                 defaultValue: Default value if key doesn't exist
                 
             Returns:
                 str: String value or default
             )pbdoc")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<int64_t>(key);
            if (value) {
                return *value;
            }
            throw py::key_error("Configuration key not found: " + key);
        }, py::arg("key"),
             R"pbdoc(Get an integer configuration value
             
             Args:
                 key: Configuration key
                 
             Returns:
                 int: Integer value
                 
             Raises:
                 KeyError: If key doesn't exist
             )pbdoc")
        .def("get", [](const Config& self, const std::string& key, int64_t defaultValue) {
            auto value = self.get<int64_t>(key);
            if (value) {
                return *value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
             R"pbdoc(Get an integer configuration value with default
             
             Args:
                 key: Configuration key
                 defaultValue: Default value if key doesn't exist
                 
             Returns:
                 int: Integer value or default
             )pbdoc")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<float>(key);
            if (value) {
                return *value;
            }
            throw py::key_error("Configuration key not found: " + key);
        }, py::arg("key"),
             R"pbdoc(Get a float configuration value
             
             Args:
                 key: Configuration key
                 
             Returns:
                 float: Float value
                 
             Raises:
                 KeyError: If key doesn't exist
             )pbdoc")
        .def("get", [](const Config& self, const std::string& key, float defaultValue) {
            auto value = self.get<float>(key);
            if (value) {
                return *value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
             R"pbdoc(Get a float configuration value with default
             
             Args:
                 key: Configuration key
                 defaultValue: Default value if key doesn't exist
                 
             Returns:
                 float: Float value or default
             )pbdoc")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (value) {
                return *value;
            }
            throw py::key_error("Configuration key not found: " + key);
        }, py::arg("key"),
             R"pbdoc(Get a boolean configuration value
             
             Args:
                 key: Configuration key
                 
             Returns:
                 bool: Boolean value
                 
             Raises:
                 KeyError: If key doesn't exist
             )pbdoc")
        .def("get", [](const Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            if (value) {
                return *value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
             R"pbdoc(Get a boolean configuration value with default
             
             Args:
                 key: Configuration key
                 defaultValue: Default value if key doesn't exist
                 
             Returns:
                 bool: Boolean value or default
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys
             
             Returns:
                 list[str]: List of all configuration keys
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries
             
             This removes all configuration values from the config.
             """)
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"));

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class
    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& a) {
            return "<Action: " + a.getName() + ">";
        });

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject")
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

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody")
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

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept")
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

    // SimpleWorld class with context manager
    py::class_<SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("__enter__", [](SimpleWorld& self) -> SimpleWorld& {
            return self;
        })
        .def("__exit__", [](SimpleWorld& self, py::args) {
            try {
                self.reset();
            } catch (const std::exception& e) {
                py::print("Warning: Error during SimpleWorld cleanup: ", e.what());
            }
        })
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("initialize", &SimpleWorld::initialize, py::arg("width") = 20.0f, py::arg("height") = 20.0f,
             py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
             py::arg("maxEnergy") = 100.0f, py::arg("energyDecayRate") = 0.01f)
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

    // Brain class with context manager
    py::class_<Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("__enter__", [](Brain& self) -> Brain& {
            return self;
        })
        .def("__exit__", [](Brain& self, py::args) {
            try {
                self.reset();
            } catch (const std::exception& e) {
                py::print("Warning: Error during Brain cleanup: ", e.what());
            }
        })
        .def("initialize", &Brain::initialize)
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"))
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"))
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"))
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"))
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"))
        .def("produceAction", &Brain::produceAction)
        .def("reset", &Brain::reset)
        .def("save", &Brain::save, py::arg("filepath"))
        .def("load", &Brain::load, py::arg("filepath"))
        .def("addRegion", &Brain::addRegion, py::arg("name") = "")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal)
        .def("getRegionCount", &Brain::getRegionCount)
        .def("getRegionIds", &Brain::getRegionIds)
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal)
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount)
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &Brain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount)
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount)
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage)
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"))
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal)
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal)
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal)
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal)
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal)
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal)
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal)
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal)
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal)
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal)
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal)
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal)
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal)
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal)
        .def("logStatus", &Brain::logStatus);

    // AgentBrain class with context manager
    py::class_<AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("__enter__", [](AgentBrain& self) -> AgentBrain& {
            return self;
        })
        .def("__exit__", [](AgentBrain& self, py::args) {
            try {
                self.reset();
            } catch (const std::exception& e) {
                py::print("Warning: Error during AgentBrain cleanup: ", e.what());
            }
        })
        .def("initialize", &AgentBrain::initialize, py::arg("world"))
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize)
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"))
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"))
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"))
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &AgentBrain::getPredictionError)
        .def("reset", &AgentBrain::reset)
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal)
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Helper functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    });
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"));
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    });
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"));
    
    m.def("create_vision_input", [](size_t width, size_t height, size_t channels) {
        return std::make_shared<Vision>(width, height, channels);
    }, py::arg("width") = 8, py::arg("height") = 8, py::arg("channels") = 3);
    
    m.def("create_audio_input", [](size_t sampleRate, size_t numSamples) {
        return std::make_shared<Audio>(sampleRate, numSamples);
    }, py::arg("sampleRate") = 16000, py::arg("numSamples") = 1024);
    
    m.def("create_internal_signals", []() {
        return std::make_shared<InternalSignals>();
    });
    
    m.def("create_motor_action", [](ActionType type, std::vector<float> params) {
        return std::make_shared<Action>(type, params);
    }, py::arg("type"), py::arg("params") = std::vector<float>());
    
    m.def("create_motor_action", [](ActionType type) {
        return std::make_shared<Action>(type);
    }, py::arg("type"));
    
    m.def("create_world_object", [](float x, float y, WorldObjectType type, float value, float radius) {
        return std::make_shared<WorldObject>(x, y, type, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f);
    
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                             std::shared_ptr<AgentBrain> agent, int numSteps, double timestep) {
        if (!brain || !world || !agent) {
            throw py::value_error("Brain, world, and agent must all be initialized");
        }
        
        if (!brain->getConfig()) {
            throw py::runtime_error("Brain must be initialized before running simulation");
        }
        
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            if (step < numSteps - 1) {
                auto motorCmd = agent->decodeMotorCommand();
                world->applyMotorCommand(motorCmd, world->getSimulationTime());
            }
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(), 
                              brain->getFiringNeuronCount(),
                              agent->getNeuromodulationLevel());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
         py::arg("numSteps") = 100, py::arg("timestep") = 0.1);
    
    m.def("run_episode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                          std::shared_ptr<AgentBrain> agent, int maxSteps) {
        if (!brain || !world || !agent) {
            throw py::value_error("Brain, world, and agent must all be initialized");
        }
        
        // Initialize if needed
        if (!brain->getConfig() || !brain->getTotalNeuronCount()) {
            brain->initialize();
        }
        
        world->reset();
        agent->reset();
        agent->initialize(*world);
        
        int steps = 0;
        float totalReward = 0.0f;
        
        for (steps = 0; steps < maxSteps; ++steps) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(steps);
            
            auto action = agent->decodeMotorCommand();
            auto result = world->applyMotorCommand(action, world->getSimulationTime());
            
            if (result.success) {
                totalReward += result.reward;
            }
            
            agent->applyRewardModulation(result.reward, 0.0f);
            agent->updateDevelopment(0.1);
        }
        
        return std::make_tuple(steps, totalReward, 
                              brain->getTotalSpikeCount(),
                              agent->getNeuromodulationLevel());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
         py::arg("maxSteps") = 1000);
    
    // Export constants for backward compatibility
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}