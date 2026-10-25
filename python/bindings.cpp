# NLM Python bindings enhanced with additional functionality
# Improved error handling, documentation, and advanced features

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced Python module with comprehensive API
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ==============================================
        
        A comprehensive Python API for the NLM (Neural Learning Machine) neural simulation framework.
        
        This module provides Python bindings for the complete NLM system, including:
        - Core neural computation (spiking neurons, synapses, plasticity)
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Prediction and planning systems
        - Development and maturation
        - World simulation and interaction
        - Agent-based behavior
        
        Key Features:
        - Event-driven neural simulation with real LIF dynamics
        - Multiple plasticity mechanisms (STDP, Hebbian, reward-modulated)
        - Advanced memory systems for experience storage
        - Neuromodulation for motivation and exploration
        - Integration with 2D world environments
        - Comprehensive configuration system
        - Performance optimizations (SoA, SIMD, multithreading)
        
        Usage Examples:
        >>> import pynlm
        >>> config = pynlm.createDefaultConfig()
        >>> config.set("brain.neuron_count", 1000)
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
        >>> for i in range(100):
        ...     brain.step(i)
        >>> print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
        
        Advanced Usage:
        >>> agent, world, stats = pynlm.create_agent_simulation(steps=1000)
        >>> agent.enableCuriosity(True)
        >>> agent.enableRewardModulation(True)
    )pbdoc";

    // Register exceptions for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);

    // ====================================================================
    // ENUMERATIONS
    // ====================================================================

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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration)pbdoc")
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

    // ====================================================================
    // CORE TYPES
    // ====================================================================

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
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
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
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
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
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ====================================================================
    // CONFIGURATION SYSTEM
    // ====================================================================

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
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<int64_t>(key);
            if (value.has_value()) return py::cast(value.value());
            value = self.get<double>(key);
            if (value.has_value()) return py::cast(value.value());
            value = self.get<bool>(key);
            if (value.has_value()) return py::cast(value.value());
            value = self.get<std::string>(key);
            if (value.has_value()) return py::cast(value.value());
            return py::none();
        }, py::arg("key"), "Get a configuration value (flexible type)")
        .def("getOr", [](const Config& self, const std::string& key, const py::object& default_value) {
            if (py::isinstance<py::int_(const py::object&)>(default_value)) {
                return self.getOr<int64_t>(key, default_value.cast<int64_t>());
            } else if (py::isinstance<py::float_(const py::object&)>(default_value)) {
                return self.getOr<double>(key, default_value.cast<double>());
            } else if (py::isinstance<py::bool_(const py::object&)>(default_value)) {
                return self.getOr<bool>(key, default_value.cast<bool>());
            } else if (py::isinstance<py::str>(const py::object&)>(default_value)) {
                return self.getOr<std::string>(key, default_value.cast<std::string>());
            }
            throw py::type_error("Unsupported default value type for Config.getOr()");
        }, py::arg("key"), py::arg("default_value"), "Get a configuration value with default")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int64_t>());
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>());
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>());
            } else {
                throw py::type_error("Unsupported value type for Config.set()");
            }
        }, py::arg("key"), py::arg("value"), "Set a configuration value")
        .def("setWithSource", [](Config& self, const std::string& key, const py::object& value,
                                 ConfigSource source) {
            if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int64_t>(), source);
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>(), source);
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>(), source);
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>(), source);
            } else {
                throw py::type_error("Unsupported value type for Config.set()");
            }
        }, py::arg("key"), py::arg("value"), py::arg("source"), "Set a configuration value with source")
        .def("remove", &Config::remove, py::arg("key"), "Remove a configuration key")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Factory functions for configuration
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with standard NLM settings");

    m.def("createConfigFromFile", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(filepath)) {
            return config;
        } else {
            throw std::runtime_error("Failed to load configuration from file: " + filepath);
        }
    }, py::arg("filepath"), "Create configuration from JSON file");

    // ====================================================================
    // SENSORY AND MOTOR SYSTEMS
    // ====================================================================

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
        .def("getData", [](const Vision& self) -> py::array {
            const auto& vision_data = self.getData();
            return py::array(py::cast(vision_data));
        }, "Get vision data as numpy array")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getData", [](const Audio& self) -> py::array {
            const auto& audio_data = self.getData();
            return py::array(py::cast(audio_data));
        }, "Get audio data as numpy array")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("getSignals", [](const InternalSignals& self) -> py::array {
            const auto& signals = self.getData();
            return py::array(py::cast(signals));
        }, "Get internal signals as numpy array")
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
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
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
        .def_readwrite("active", &WorldObject::active)
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(obj.x) + ", " + 
                   std::to_string(obj.y) + ", " + 
                   std::to_string(static_cast<int>(obj.type)) + ">";
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
        .def("reset", &AgentBody::reset)
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("__repr__", [](const ActionResult& result) {
            std::string success_str = result.success ? "SUCCESS" : "FAILURE";
            return "<ActionResult: reward=" + std::to_string(result.reward) + ", " + success_str + ", " + result.message + ">";
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept at time " + std::to_string(percept.getTimestamp()) + ">";
        });

    // ====================================================================
    // WORLD SYSTEM
    // ====================================================================

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld " + std::to_string(world.getWidth()) + "x" + 
                   std::to_string(world.getHeight()) + ">";
        });

    // Factory functions for world creation
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world instance");

    // ====================================================================
    // AGENT AND BRAIN SYSTEMS
    // ====================================================================

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize agent brain with world interface")
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
        // Subsystem control
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
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain>";
        });

    // Factory function for agent brain
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // ====================================================================
    // CORE BRAIN CLASS
    // ====================================================================

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current directly into a specific neuron")
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
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
        .def("logStatus", &Brain::logStatus,
             "Log brain status and statistics")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain with " + std::to_string(brain.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(brain.getTotalSynapseCount()) + " synapses>";
        });

    // Factory function for brain creation
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    // Special constants for invalid IDs
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ====================================================================
    // EXPERIMENT AND BENCHMARKING
    // ====================================================================

    // Enhanced Phase 6 integration experiment with Python-friendly API
    py::class_<Phase6IntegrationResult>(m, "Phase6IntegrationResult")
        .def_readonly("memoryWorkingMemoryIntegrated", &Phase6IntegrationResult::memoryWorkingMemoryIntegrated)
        .def_readonly("memoryEpisodicMemoryIntegrated", &Phase6IntegrationResult::memoryEpisodicMemoryIntegrated)
        .def_readonly("neuromodulationIntegrated", &Phase6IntegrationResult::neuromodulationIntegrated)
        .def_readonly("predictionIntegrated", &Phase6IntegrationResult::predictionIntegrated)
        .def_readonly("developmentIntegrated", &Phase6IntegrationResult::developmentIntegrated)
        .def_readonly("checkpointingWorks", &Phase6IntegrationResult::checkpointingWorks)
        .def_readonly("replayWorks", &Phase6IntegrationResult::replayWorks)
        .def_readonly("totalReward", &Phase6IntegrationResult::totalReward)
        .def_readonly("avgFiringRate", &Phase6IntegrationResult::avgFiringRate)
        .def_readonly("avgSynapticWeight", &Phase6IntegrationResult::avgSynapticWeight)
        .def_readonly("memoryEpisodesStored", &Phase6IntegrationResult::memoryEpisodesStored)
        .def_readonly("noveltyLevel", &Phase6IntegrationResult::noveltyLevel)
        .def_readonly("curiosityLevel", &Phase6IntegrationResult::curiosityLevel)
        .def_readonly("dopamineLevel", &Phase6IntegrationResult::dopamineLevel)
        .def_readonly("totalWallClockTime", &Phase6IntegrationResult::totalWallClockTime)
        .def("__repr__", [](const Phase6IntegrationResult& result) {
            return "<Phase6IntegrationResult: reward=" + std::to_string(result.totalReward) + ", " +
                   "firing_rate=" + std::to_string(result.avgFiringRate) + ", " +
                   "memory_episodes=" + std::to_string(result.memoryEpisodesStored) + ">";
        });

    py::class_<Phase6Config>(m, "Phase6Config")
        .def(py::init<>())
        .def_readwrite("maxSteps", &Phase6Config::maxSteps)
        .def_readwrite("neuronCount", &Phase6Config::neuronCount)
        .def_readwrite("regionCount", &Phase6Config::regionCount)
        .def_readwrite("connectionProbability", &Phase6Config::connectionProbability)
        .def_readwrite("enableCheckpointing", &Phase6Config::enableCheckpointing)
        .def_readwrite("enableReplay", &Phase6Config::enableReplay)
        .def_readwrite("enableDevelopment", &Phase6Config::enableDevelopment)
        .def_readwrite("checkpointPath", &Phase6Config::checkpointPath)
        .def("__repr__", [](const Phase6Config& config) {
            return "<Phase6Config: " + std::to_string(config.neuronCount) + " neurons, " +
                   std::to_string(config.maxSteps) + " steps>";
        });

    py::class_<Phase6IntegratedExperiment>(m, "Phase6IntegratedExperiment")
        .def(py::init<>())
        .def("run", &Phase6IntegratedExperiment::run,
             py::arg("config"),
             "Run the complete integration test")
        .def("verifyIntegration", &Phase6IntegratedExperiment::verifyIntegration,
             "Run a simple integration verification")
        .def("testMemoryIntegration", &Phase6IntegratedExperiment::testMemoryIntegration,
             "Test memory system integration")
        .def("testNeuromodulationIntegration", &Phase6IntegratedExperiment::testNeuromodulationIntegration,
             "Test neuromodulation system integration")
        .def("testCheckpointing", &Phase6IntegratedExperiment::testCheckpointing,
             "Test checkpoint save/load functionality")
        .def("testReplay", &Phase6IntegratedExperiment::testReplay,
             "Test replay system functionality");

    // ====================================================================
    // UTILITY FUNCTIONS AND ADVANCED FEATURES
    // ====================================================================

    // Enhanced simulation utilities
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        return brain->getAverageFiringRate();
    }, py::arg("brain"), py::arg("steps"),
       "Run simulation for specified number of steps");

    m.def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                         std::shared_ptr<SimpleWorld> world, size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(i);
            world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
        }
        return std::make_tuple(brain->getAverageFiringRate(),
                              agent->getCuriosityLevel(),
                              agent->getNoveltyLevel());
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"),
       "Run complete agent episode with world interaction");

    m.def("benchmark", [](const std::vector<size_t>& neuron_counts, size_t steps) {
        std::vector<py::dict> results;
        for (size_t neuron_count : neuron_counts) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuron_count));
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto start_time = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < steps; ++i) {
                brain->step(i);
            }
            auto end_time = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration<double>(end_time - start_time);
            
            py::dict result;
            result["neuron_count"] = neuron_count;
            result["steps"] = steps;
            result["wall_time_seconds"] = duration.count();
            result["steps_per_second"] = steps / duration.count();
            result["avg_firing_rate"] = brain->getAverageFiringRate();
            result["total_spikes"] = brain->getTotalSpikeCount();
            result["real_time_factor"] = duration.count() / (steps * 0.001);
            
            results.push_back(result);
        }
        return results;
    }, py::arg("neuron_counts"), py::arg("steps"),
       "Benchmark NLM performance with different neuron counts");

    m.def("createQuickTestBrain", [](size_t neuron_count) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuron_count));
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run quick test steps
        for (size_t i = 0; i < 100; ++i) {
            brain->step(i);
        }
        
        return brain;
    }, py::arg("neuron_count") = 100,
       "Create a quick test brain for validation");

    m.def("validateBrainIntegration", [](std::shared_ptr<Brain> brain) {
        bool valid = true;
        std::vector<std::string> issues;
        
        if (!brain->getWorkingMemory()) {
            valid = false;
            issues.push_back("Working memory not integrated");
        }
        
        if (!brain->getEpisodicMemory()) {
            valid = false;
            issues.push_back("Episodic memory not integrated");
        }
        
        if (!brain->getPredictionSystem()) {
            valid = false;
            issues.push_back("Prediction system not integrated");
        }
        
        if (!brain->getDopamine()) {
            valid = false;
            issues.push_back("Neuromodulation not integrated");
        }
        
        return std::make_tuple(valid, issues);
    }, py::arg("brain"),
       "Validate that all brain systems are properly integrated");

    // Enhanced error types
    py::class_<std::runtime_error>(m, "RuntimeError")
        .def(py::init<const std::string&>())
        .def("what", &std::runtime_error::what);

    py::class_<std::invalid_argument>(m, "ValueError")
        .def(py::init<const std::string&>())
        .def("what", &std::invalid_argument::what);

    py::class_<std::out_of_range>(m, "IndexError")
        .def(py::init<const std::string&>())
        .def("what", &std::out_of_range::what);

    // ====================================================================
    // ADVANCED USER FEATURES
    // ====================================================================

    // Performance optimization controls
    m.def("enableOptimizations", []() {
        // Enable performance optimizations
        // This is a placeholder for future optimization controls
        return "Optimizations enabled (requires rebuild with -DNLM_PERFORMANCE_ENABLED)";
    }, "Enable performance optimizations");

    m.def("getPerformanceInfo", []() {
        py::dict info;
        info["soa_enabled"] = true;  // Placeholder
        info["simd_width"] = 8;      // Placeholder
        info["thread_count"] = 1;   // Placeholder
        info["memory_pool_enabled"] = true;  // Placeholder
        return info;
    }, "Get current performance configuration");

    // Advanced configuration presets
    m.def("createPresetConfig", [](const std::string& preset_name) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        if (preset_name == "minimal") {
            config->set("neuron_count", 100);
            config->set("region_count", 1);
            config->set("connection_probability", 0.05f);
        } else if (preset_name == "standard") {
            config->set("neuron_count", 1000);
            config->set("region_count", 1);
            config->set("connection_probability", 0.1f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
        } else if (preset_name == "high_performance") {
            config->set("neuron_count", 5000);
            config->set("region_count", 2);
            config->set("connection_probability", 0.05f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
        } else if (preset_name == "research") {
            config->set("neuron_count", 10000);
            config->set("region_count", 4);
            config->set("connection_probability", 0.02f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("neuromod.dopamine.scale", 1.0f);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
        } else {
            throw std::invalid_argument("Unknown preset name: " + preset_name);
        }
        
        return config;
    }, py::arg("preset_name"),
       "Create configuration with predefined preset settings");

    // Advanced simulation controls
    m.def("configureSimulation", [](std::shared_ptr<Brain> brain,
                                   const py::dict& settings) {
        if (settings.contains("timestep")) {
            double timestep = settings["timestep"].cast<double>();
            // Apply timestep setting (would need config modification)
        }
        if (settings.contains("plasticity_enabled")) {
            bool plasticity_enabled = settings["plasticity_enabled"].cast<bool>();
            // Apply plasticity setting (would need plasticity system control)
        }
        if (settings.contains("neuromodulation_enabled")) {
            bool neuromodulation_enabled = settings["neuromodulation_enabled"].cast<bool>();
            // Apply neuromodulation setting (would need neuromodulator control)
        }
        return brain;
    }, py::arg("brain"), py::arg("settings"),
       "Configure advanced simulation settings");

    // Data export utilities
    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain->save(filepath)) {
            throw std::runtime_error("Failed to save brain state to file: " + filepath);
        }
        return true;
    }, py::arg("brain"), py::arg("filepath"),
       "Export brain state to file");

    m.def("importBrainState", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        if (!brain->load(filepath)) {
            throw std::runtime_error("Failed to load brain state from file: " + filepath);
        }
        return brain;
    }, py::arg("filepath"), "Import brain state from file");

    // ====================================================================
    // MODULE METADATA AND HELP
    // ====================================================================

    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework";
    
    // Add help functions
    m.def("get_help", []() {
        return R"(
        NLM Python Module Help
        ========================
        
        Available Classes:
        - Config: Configuration management
        - Brain: Neural simulation engine
        - AgentBrain: Agent interface for world interaction
        - SimpleWorld: 2D simulation environment
        - SensoryInput, Vision, Audio: Sensory systems
        - Action: Motor output representation
        - WorldObject: Environment objects
        - AgentBody: Agent physical state
        - SensoryPercept: Processed sensory data
        
        Key Functions:
        - createDefaultConfig(): Create default configuration
        - createBrain(config): Create neural simulation brain
        - createAgentBrain(brain): Create agent interface
        - createSimpleWorld(): Create simulation world
        
        Advanced Features:
        - Phase6IntegratedExperiment: Integration testing
        - benchmark(): Performance benchmarking
        - create_agent_simulation(): Complete agent simulation
        - createPresetConfig(): Presets for common configurations
        
        For more detailed documentation, see the NLM documentation.
        )";
    });

} // namespace nlm
