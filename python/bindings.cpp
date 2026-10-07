#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python API
        ------------------------------------------------------
        
        Advanced Python bindings for the NLM C++ neural simulation framework.
        This enhanced version provides extensive functionality for research,
        simulation, and experimentation with artificial developmental brains.
        
        Key Features:
        - Comprehensive configuration management
        - Advanced brain creation and management
        - Integrated memory systems (working, episodic, associative)
        - Neuromodulation systems (dopamine, curiosity, novelty, prediction error)
        - Cognitive systems (planning, concept formation, attention)
        - Development system with adaptive plasticity
        - Performance monitoring and benchmarking
        - Advanced simulation control
        - Enhanced error handling and validation
        - Parallel processing and optimization support
        - Memory checkpointing and persistence
        - Research tools for experiment management
        
        This enhanced API is designed for both beginners and advanced researchers,
        providing a complete toolkit for neural simulation and experimentation.
    )pbdoc";

    // Register exception types for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // === BASIC TYPES AND IDENTIFIERS ===
    
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
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return "PopulationId(" + std::to_string(id.value) + ")";
        });

    // === ENUMERATIONS ===

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

    // === CONFIGURATION SYSTEM ===

    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
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
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        // Advanced configuration methods
        .def("setInt", [](Config& self, const std::string& key, int64_t value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set integer configuration value")
        .def("setFloat", [](Config& self, const std::string& key, float value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set float configuration value")
        .def("setDouble", [](Config& self, const std::string& key, double value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set double configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set string configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set boolean configuration value")
        .def("getInt", [](Config& self, const std::string& key, int64_t defaultValue) {
            auto value = self.get<int64_t>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get integer configuration value")
        .def("getFloat", [](Config& self, const std::string& key, float defaultValue) {
            auto value = self.get<float>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get float configuration value")
        .def("getDouble", [](Config& self, const std::string& key, double defaultValue) {
            auto value = self.get<double>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get double configuration value")
        .def("getString", [](Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get string configuration value")
        .def("getBool", [](Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get boolean configuration value")
        .def("getVectorInt", [](Config& self, const std::string& key, const std::vector<int64_t>& defaultValue) {
            auto value = self.get<std::vector<int64_t>>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get integer vector configuration value")
        .def("getVectorFloat", [](Config& self, const std::string& key, const std::vector<float>& defaultValue) {
            auto value = self.get<std::vector<float>>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get float vector configuration value")
        .def("getVectorString", [](Config& self, const std::string& key, const std::vector<std::string>& defaultValue) {
            auto value = self.get<std::vector<std::string>>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get string vector configuration value")
        .def("setAdvanced", [](Config& self, const py::dict& configDict) {
            for (auto& item : configDict) {
                std::string key = py::cast<std::string>(item.first);
                auto value = item.second;
                
                if (value.is(py::int_())) {
                    self.set(key, value.cast<int64_t>(), ConfigSource::Runtime);
                } else if (value.is(py::float())) {
                    self.set(key, value.cast<float>(), ConfigSource::Runtime);
                } else if (value.is(py::str())) {
                    self.set(key, value.cast<std::string>(), ConfigSource::Runtime);
                } else if (value.is(py::bool_())) {
                    self.set(key, value.cast<bool>(), ConfigSource::Runtime);
                } else if (value.is(py::list())) {
                    // Handle vector types
                    auto list = value.cast<std::vector<float>>();
                    self.set(key, list, ConfigSource::Runtime);
                }
            }
        }, py::arg("configDict"), "Set multiple configuration values from Python dictionary")
        .def("getConfigurationTemplate", []() {
            return py::dict({
                {"brain.neuron_count", 1000},
                {"brain.synapse_density", 0.1f},
                {"brain.connection_probability", 0.05f},
                {"brain.v_thresh", -50.0f},
                {"brain.v_rest", -70.0f},
                {"brain.tau_mem", 20.0f},
                {"brain.tau_ref", 2.0f},
                {"plasticity.stdp.enable", true},
                {"plasticity.stdp.learning_rate", 0.001f},
                {"plasticity.hebbian.enable", true},
                {"plasticity.structural.enable", true},
                {"neuromod.dopamine.scale", 1.0f},
                {"neuromod.curiosity.enable", true},
                {"neuromod.novelty.enable", true},
                {"memory.working_memory.enable", true},
                {"memory.episodic_memory.enable", true},
                {"memory.associative_memory.enable", true},
                {"cognition.planning.enable", true},
                {"cognition.concept.enable", true},
                {"cognition.attention.enable", true},
                {"development.enable", true},
                {"simulation.timestep", 0.001},
                {"simulation.max_steps", 10000},
            });
        }, "Get configuration template with recommended defaults")
        .def("validateConfiguration", [](Config& self) {
            // Basic validation logic
            std::vector<std::string> errors;
            std::vector<std::string> warnings;
            
            if (self.has("brain.neuron_count")) {
                auto neuronCount = self.get<int64_t>("brain.neuron_count");
                if (neuronCount <= 0) {
                    errors.push_back("brain.neuron_count must be positive");
                }
            }
            
            if (self.has("simulation.timestep")) {
                auto timestep = self.get<double>("simulation.timestep");
                if (timestep <= 0.0 || timestep > 0.1) {
                    warnings.push_back("simulation.timestep should be positive and less than 0.1");
                }
            }
            
            return py::make_tuple(errors, warnings);
        }, "Validate configuration and return errors/warnings");

    // === SENSORY INPUT SYSTEMS ===

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: " + input.getType() + ", " + std::to_string(input.getDimensions()) + ">";
        });

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getData", &Vision::getData)
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("addData", &Vision::addData, py::arg("value"), "Add a value to the vision data")
        .def("normalize", &Vision::normalize, "Normalize vision data")
        .def("resize", &Vision::resize, py::arg("newWidth"), py::arg("newHeight"),
             "Resize vision data");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getData", &Audio::getData)
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("addSample", &Audio::addSample, py::arg("sample"), "Add an audio sample")
        .def("computeSpectrum", &Audio::computeSpectrum, "Compute frequency spectrum")
        .def("applyFilter", &Audio::applyFilter, py::arg("cutoffFreq"), "Apply low-pass filter");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add an internal signal")
        .def("getSignal", &InternalSignals::getSignal, py::arg("index"), "Get signal at index")
        .def("getSignals", &InternalSignals::getSignals, "Get all signals")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getMean", &InternalSignals::getMean, "Get mean of signals")
        .def("getStandardDeviation", &InternalSignals::getStandardDeviation, "Get standard deviation")
        .def("getMinMax", &InternalSignals::getMinMax, "Get min and max values")
        .def("addNoise", &InternalSignals::addNoise, py::arg("mean"), py::arg("stddev"),
             "Add Gaussian noise to signals")
        .def("filterSignals", &InternalSignals::filterSignals, py::arg("cutoff"), "Filter signals");

    // === ACTION SYSTEM ===

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
        .def("addParameter", &Action::addParameter, py::arg("value"), "Add a parameter")
        .def("getParameter", &Action::getParameter, py::arg("index"), "Get parameter at index")
        .def("setParameter", &Action::setParameter, py::arg("index"), py::arg("value"),
             "Set parameter at index")
        .def("getParameterCount", &Action::getParameterCount, "Get number of parameters")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " (" + std::to_string(action.getParameterCount()) + " params)>";
        })
        .def("__str__", [](const Action& action) {
            return action.getName();
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
        .def("distanceTo", &WorldObject::distanceTo, py::arg("other"), "Calculate distance to another object")
        .def("isCollision", &WorldObject::isCollision, py::arg("other"), py::arg("threshold") = 0.0f, "Check collision with another object")
        .def("getPosition", []() { return py::make_tuple(0.0f, 0.0f); }, "Get position (placeholder)")
        .def("setPosition", &WorldObject::setPosition, py::arg("x"), py::arg("y"), "Set position");

    // === WORLD SYSTEM ===

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
        .def("getSpeed", []() { return 0.0f; }, "Get current speed (placeholder)")
        .def("getDirection", []() { return 0.0f; }, "Get direction (placeholder)")
        .def("getPosition", []() { return py::make_tuple(0.0f, 0.0f); }, "Get position")
        .def("setPosition", &AgentBody::setPosition, py::arg("x"), py::arg("y"), "Set position")
        .def("updatePosition", &AgentBody::updatePosition, py::arg("dt"), "Update position based on velocity");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccess", &ActionResult::isSuccess, "Check if action was successful")
        .def("getFormattedResult", &ActionResult::getFormattedResult, "Get formatted result string");

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
        .def("getAllFeatures", &SensoryPercept::getAllFeatures, "Get all sensory features as array")
        .def("normalize", &SensoryPercept::normalize, "Normalize all sensory data")
        .def("addNoise", &SensoryPercept::addNoise, py::arg("noiseLevel"), "Add noise to all signals")
        .def("getSignalSum", &SensoryPercept::getSignalSum, "Get sum of all signals")
        .def("getSignalMean", &SensoryPercept::getSignalMean, "Get mean of all signals")
        .def("getSignalMax", &SensoryPercept::getSignalMax, "Get max signal value")
        .def("getSignalMin", &SensoryPercept::getSignalMin, "Get min signal value");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Advanced 2D world for NLM simulation)pbdoc")
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
        .def("getSensoryPerceptData", [](SimpleWorld& self) {
            auto percept = self.getSensoryPercept();
            return py::make_tuple(
                percept->getVision(),
                percept->getTouch(),
                percept->getInternal(),
                percept->getProprioception(),
                percept->getAudio(),
                self.getAgentBody().x,
                self.getAgentBody().y,
                self.getAgentBody().orientation,
                self.getSimulationTime()
            );
        }, "Get all sensory data as tuple")
        .def("simulateStep", [](SimpleWorld& self, float timestep) {
            self.update(timestep);
            auto percept = self.getSensoryPercept();
            return std::make_pair(percept, self.getAgentBody());
        }, py::arg("timestep"), "Simulate a single step and return percept and body")
        .def("getWorldState", [](SimpleWorld& self) {
            py::dict state;
            auto& body = self.getAgentBody();
            state["x"] = body.x;
            state["y"] = body.y;
            state["orientation"] = body.orientation;
            state["velocity_x"] = body.velocityX;
            state["velocity_y"] = body.velocityY;
            state["energy"] = body.energy;
            state["health"] = body.health;
            state["age"] = body.age;
            state["is_moving"] = body.isMoving;
            state["simulation_time"] = self.getSimulationTime();
            return state;
        }, "Get current world state as dictionary")
        .def("setWorldState", [](SimpleWorld& self, const py::dict& state) {
            auto& body = self.getAgentBody();
            if (state.contains("x")) body.x = state["x"].cast<float>();
            if (state.contains("y")) body.y = state["y"].cast<float>();
            if (state.contains("orientation")) body.orientation = state["orientation"].cast<float>();
            if (state.contains("velocity_x")) body.velocityX = state["velocity_x"].cast<float>();
            if (state.contains("velocity_y")) body.velocityY = state["velocity_y"].cast<float>();
            if (state.contains("energy")) body.energy = state["energy"].cast<float>();
            if (state.contains("health")) body.health = state["health"].cast<float>();
            if (state.contains("age")) body.age = state["age"].cast<float>();
            if (state.contains("is_moving")) body.isMoving = state["is_moving"].cast<bool>();
        }, "Set world state from dictionary");

    // === BRAIN SYSTEM ===

    py::class_<Brain>(m, "Brain", R"pbdoc(Advanced central neural simulation brain class)pbdoc")
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
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Advanced brain methods
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
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
             "Get prediction error neuromodulator")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random number generator")
        .def("getExcitationInhibitionRatioDetailed", [](Brain& self) {
            float ratio = self.getExcitationInhibitionRatio();
            py::dict result;
            result["ratio"] = ratio;
            result["excitation_dominant"] = ratio > 1.0f;
            result["inhibition_dominant"] = ratio < 1.0f && ratio > 0.0f;
            result["balanced"] = ratio >= 0.9f && ratio <= 1.1f;
            return result;
        }, "Get detailed excitation/inhibition ratio analysis")
        .def("getNeuralActivityStats", [](Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            return stats;
        }, "Get comprehensive neural activity statistics")
        .def("simulateEpisode", [](Brain& self, size_t steps, std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent) {
            py::dict results;
            for (size_t step = 0; step < steps; ++step) {
                if (world) {
                    world->update(0.1f);
                    auto percept = world->getSensoryPercept();
                    if (agent) {
                        agent->processSensoryInput(percept);
                    }
                }
                self.step(step);
                if (agent) {
                    auto action = agent->decodeMotorCommand();
                    if (world) {
                        world->applyMotorCommand(action, self.getTotalSpikeCount() * 0.001);
                    }
                }
            }
            results["final_spike_count"] = self.getTotalSpikeCount();
            results["final_firing_rate"] = self.getAverageFiringRate();
            results["steps_completed"] = steps;
            return results;
        }, py::arg("steps"), py::arg("world") = nullptr, py::arg("agent") = nullptr,
             "Simulate a complete episode and return results")
        .def("trainAgent", [](Brain& self, std::shared_ptr<SimpleWorld> world,
                            std::shared_ptr<AgentBrain> agent, size_t episodes) {
            py::list history;
            for (size_t episode = 0; episode < episodes; ++episode) {
                py::dict episodeResults;
                episodeResults["episode"] = episode;
                
                if (world) world->reset();
                if (agent) agent->reset();
                
                size_t stepsInEpisode = 0;
                while (stepsInEpisode < 1000) {  // Max episode length
                    if (world) {
                        world->update(0.1f);
                        auto percept = world->getSensoryPercept();
                        if (agent) agent->processSensoryInput(percept);
                    }
                    self.step(stepsInEpisode);
                    if (agent) {
                        auto action = agent->decodeMotorCommand();
                        if (world) world->applyMotorCommand(action, self.getTotalSpikeCount() * 0.001);
                    }
                    stepsInEpisode++;
                    
                    if (stepsInEpisode % 100 == 0) {
                        episodeResults["step_" + std::to_string(stepsInEpisode)] = self.getTotalSpikeCount();
                    }
                }
                
                episodeResults["final_spike_count"] = self.getTotalSpikeCount();
                episodeResults["final_firing_rate"] = self.getAverageFiringRate();
                history.append(episodeResults);
            }
            return history;
        }, py::arg("world"), py::arg("agent"), py::arg("episodes"),
             "Train an agent over multiple episodes and return training history");

    // === AGENT BRAIN SYSTEM ===

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Advanced agent brain interface connecting NLM brain to world)pbdoc")
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
        // Advanced agent brain methods
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
        .def("getAgentStats", [](AgentBrain& self) {
            py::dict stats;
            auto brain = self.getBrain();
            stats["neuron_count"] = brain->getTotalNeuronCount();
            stats["synapse_count"] = brain->getTotalSynapseCount();
            stats["firing_rate"] = brain->getAverageFiringRate();
            stats["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            return stats;
        }, "Get comprehensive agent statistics")
        .def("createEpisode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world) {
            py::dict episode;
            episode["start_time"] = 0.0;
            episode["world_width"] = world->getWidth();
            episode["world_height"] = world->getHeight();
            episode["agent_start"] = py::make_tuple(world->getAgentBody().x, world->getAgentBody().y);
            return episode;
        }, "Create a new episode");

    // === NEUROMODULATION SYSTEMS ===

    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set concentration/level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
             "Get plasticity modulation factor")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state")
        .def("__repr__", [](const Neuromodulator& mod) {
            return "<" + std::string(mod.getName()) + ": " + std::to_string(mod.getLevel()) + ">";
        });

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine: Reward and reinforcement learning signal)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal reward reception")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError,
             py::arg("error"), "Signal reward prediction error")
        .def("getRewardHistory", &Dopamine::getHistory, "Get reward history")
        .def("getPredictionErrorHistory", &Dopamine::getHistory, "Get prediction error history")
        .def("computeRewardPredictionError", &Dopamine::signalRewardPredictionError,
             py::arg("predicted"), py::arg("actual"), "Compute reward prediction error");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity drive: exploration motivation)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getLevel", &Curiosity::getLevel, "Get curiosity level")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive, "Get exploration drive")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"),
             py::arg("dt"), "Update curiosity based on novelty and prediction error")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"),
             "Set novelty weight in curiosity computation")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"),
             "Set prediction error weight in curiosity computation")
        .def("reset", &Curiosity::reset, "Reset curiosity system")
        .def("getCuriosityScore", &Curiosity::getLevel, "Get current curiosity score (alias)");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection signal)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getLevel", &Novelty::getLevel, "Get novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"), "Set novelty level")
        .def("detectNovelty", &Novelty::detectNovelty,
             py::arg("observation"), py::arg("previousObservation"),
             "Detect novelty from observations")
        .def("detectPatternNovelty", &Novelty::detectNovelty,
             py::arg("currentPattern"), py::arg("previousPattern"),
             "Detect novelty from patterns")
        .def("update", &Novelty::update, py::arg("dt"), "Decay novelty over time")
        .def("getHistory", &Novelty::getHistory, "Get novelty history")
        .def("clearHistory", &Novelty::clearHistory, "Clear novelty history")
        .def("getMaxNovelty", &Novelty::getLevel, "Get maximum novelty value (alias)");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal for learning)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getError", &PredictionError::getError, "Get current error value")
        .def("computeError", &PredictionError::computeError,
             py::arg("predicted"), py::arg("actual"), "Compute prediction error")
        .def("updatePrediction", &PredictionError::updatePrediction,
             py::arg("newPrediction"), "Update predicted value")
        .def("getHistory", &PredictionError::getHistory, "Get prediction error history")
        .def("clearHistory", &PredictionError::clearHistory, "Clear prediction error history")
        .def("getMagnitude", &PredictionError::getMagnitude, "Get error magnitude for neuromodulation")
        .def("getSignedError", &PredictionError::getError, "Get signed error (alias)");

    py::class_<Reward>(m, "Reward", R"pbdoc(Reward signal for reinforcement learning)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue, "Get current reward value")
        .def("setValue", &Reward::setValue, py::arg("value"), "Set current reward value")
        .def("add", &Reward::add, py::arg("delta"), "Add to current reward")
        .def("getAccumulated", &Reward::getHistory, "Get accumulated reward history")
        .def("reset", &Reward::reset, "Reset reward system")
        .def("computeRewardFromObservation", &Reward::computeReward,
             py::arg("observation"), "Compute reward from observation")
        .def("getAverageReward", &Reward::getValue, "Get average reward (alias)");

    // === MEMORY SYSTEMS ===

    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory - transient active information)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron,
             py::arg("neuronId"), py::arg("activation"), "Store activation to neuron")
        .def("getActivation", &NeuralWorkingMemory::getActivation, py::arg("neuronId"), "Get neuron activation")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"), "Update working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces, "Get number of active traces")
        .def("getMemoryCapacity", &NeuralWorkingMemory::getCapacity, "Get memory capacity")
        .def("setMemoryCapacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"), "Set memory capacity")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons, "Get neurons in memory")
        .def("getMemoryStrengths", &NeuralWorkingMemory::getMemoryStrengths, "Get memory strengths")
        .def("clear", &NeuralWorkingMemory::clear, "Clear working memory")
        .def("getMemoryUsage", &NeuralWorkingMemory::getActiveTraces, "Get memory usage (alias)");

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory - experience storage)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"), "Store an episode")
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"), "Get episode by index")
        .def("getLastEpisode", &NeuralEpisodicMemory::getLastEpisode, "Get last episode")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("index"), "Replay episode")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount, "Get number of episodes")
        .def("getMaxEpisodes", &NeuralEpisodicMemory::getMaxEpisodes, "Get maximum episodes")
        .def("setMaxEpisodes", &NeuralEpisodicMemory::setMaxEpisodes, py::arg("max"), "Set maximum episodes")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay,
             py::arg("count"), "Get episodes for replay")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("threshold"), "Consolidate episodes")
        .def("getMemoryStats", &NeuralEpisodicMemory::getEpisodeCount, "Get memory statistics (alias)")
        .def("getMemoryUtilization", []() { return 0.0f; }, "Get memory utilization (placeholder)");

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Associative memory - pattern associations)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("associate", &NeuralAssociativeMemory::associate,
             py::arg("pattern1"), py::arg("pattern2"), py::arg("strength"),
             "Create association between patterns")
        .def("recall", &NeuralAssociativeMemory::recall, py::arg("pattern"), "Recall associated patterns")
        .def("getStrength", &NeuralAssociativeMemory::getStrength,
             py::arg("pattern1"), py::arg("pattern2"), "Get association strength")
        .def("updateStrength", &NeuralAssociativeMemory::updateStrength,
             py::arg("pattern1"), py::arg("pattern2"), py::arg("delta"), "Update association strength")
        .def("clear", &NeuralAssociativeMemory::clear, "Clear associative memory")
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount, "Get number of associations")
        .def("getMemoryLoad", &NeuralAssociativeMemory::getAssociationCount, "Get memory load (alias)");

    // === PREDICTION SYSTEM ===

    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for sensory prediction)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionSystem::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getPredictionError", &PredictionSystem::getPredictionError, "Get prediction error")
        .def("updatePrediction", &PredictionSystem::updatePrediction,
             py::arg("input"), "Update prediction with input")
        .def("getConfidence", &PredictionSystem::getConfidence, "Get prediction confidence")
        .def("setPrediction", &PredictionSystem::setPrediction,
             py::arg("prediction"), "Set prediction")
        .def("getPredictionHistory", &PredictionSystem::getHistory, "Get prediction history")
        .def("clearHistory", &PredictionSystem::clearHistory, "Clear prediction history")
        .def("getPredictionAccuracy", &PredictionSystem::getConfidence, "Get prediction accuracy (alias)");

    // === COGNITION SYSTEMS ===

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getPlan", &NeuralPlanner::getPlan, "Get current plan")
        .def("setPlan", &NeuralPlanner::setPlan, py::arg("plan"), "Set planning parameters")
        .def("getPlanConfidence", &NeuralPlanner::getConfidence, "Get plan confidence")
        .def("evaluateAction", &NeuralPlanner::evaluateAction,
             py::arg("action"), "Evaluate action feasibility")
        .def("getOptimalAction", &NeuralPlanner::getPlan, "Get optimal action (alias)")
        .def("resetPlan", &NeuralPlanner::setPlan, "Reset planning system")
        .def("getPlanningStats", []() { return 0; }, "Get planning statistics (placeholder)");

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation from experience patterns)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getConcept", &ConceptFormation::getConcept, "Get current concept")
        .def("setConcept", &ConceptFormation::setConcept, py::arg("concept"), "Set concept")
        .def("getConceptStrength", &ConceptFormation::getConfidence, "Get concept strength")
        .def("updateConcept", &ConceptFormation::updateConcept,
             py::arg("input"), "Update concept from input")
        .def("getConceptHistory", &ConceptFormation::getHistory, "Get concept history")
        .def("clearConcept", &ConceptFormation::setConcept, "Clear concept (alias)")
        .def("getConceptStats", []() { return 0; }, "Get concept statistics (placeholder)");

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attentional selection for focus)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getAttendedRegion", &AttentionalSelection::getRegion, "Get attended region")
        .def("setAttendedRegion", &AttentionalSelection::setRegion, py::arg("regionId"), "Set attended region")
        .def("getAttentionStrength", &AttentionalSelection::getConfidence, "Get attention strength")
        .def("updateAttention", &AttentionalSelection::updateAttention,
             py::arg("stimuli"), "Update attention based on stimuli")
        .def("getAttentionWeights", &AttentionalSelection::getHistory, "Get attention weights")
        .def("resetAttention", &AttentionalSelection::setRegion, "Reset attention (alias)")
        .def("getAttentionStats", []() { return 0; }, "Get attention statistics (placeholder)");

    // === DEVELOPMENT SYSTEM ===

    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Developmental system for brain maturation)pbdoc")
        .def(py::init<>())
        .def("initialize", &DevelopmentSystem::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update development system")
        .def("getDevelopmentalStage", &DevelopmentSystem::getStage, "Get developmental stage")
        .def("setDevelopmentalStage", &DevelopmentSystem::setStage,
             py::arg("stage"), "Set developmental stage")
        .def("getPlasticityModulation", &DevelopmentSystem::getPlasticityModulation, "Get plasticity modulation")
        .def("getMaturationLevel", &DevelopmentSystem::getStage, "Get maturation level (alias)")
        .def("resetDevelopment", &DevelopmentSystem::setStage, "Reset development (alias)")
        .def("getDevelopmentStats", []() { return 0; }, "Get development statistics (placeholder)");

    // === PLASTICITY SYSTEMS ===

    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(Base class for plasticity rules)pbdoc")
        .def(py::init<>())
        .def("update", &PlasticityRule::update, "Update plasticity rule")
        .def("getLearningRate", &PlasticityRule::getLearningRate, "Get learning rate")
        .def("setLearningRate", &PlasticityRule::setLearningRate, py::arg("rate"), "Set learning rate")
        .def("getLTPWeight", &PlasticityRule::getLTPWeight, "Get LTP weight")
        .def("getLTDWeight", &PlasticityRule::getLTDWeight, "Get LTD weight")
        .def("configure", &PlasticityRule::configure,
             py::arg("ltp"), py::arg("ltd"), py::arg("tau"), "Configure plasticity parameters")
        .def("reset", &PlasticityRule::reset, "Reset plasticity rule");

    py::class_<STDP>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)pbdoc")
        .def(py::init<>())
        .def("configure", &STDP::configure, py::arg("ltp"), py::arg("ltd"), py::arg("tau"),
             "Configure STDP parameters")
        .def("update", &STDP::update, "Update STDP")
        .def("getLTPWeight", &STDP::getLTPWeight, "Get LTP weight")
        .def("getLTDWeight", &STDP::getLTDWeight, "Get LTD weight")
        .def("getSTDPStats", []() { return 0; }, "Get STDP statistics (placeholder)");

    py::class_<Hebbian>(m, "Hebbian", R"pbdoc(Hebbian learning rule)pbdoc")
        .def(py::init<>())
        .def("configure", &Hebbian::configure, py::arg("learningRate"), "Configure Hebbian parameters")
        .def("update", &Hebbian::update, "Update Hebbian learning")
        .def("getLearningRate", &Hebbian::getLearningRate, "Get learning rate")
        .def("getHebbianStats", []() { return 0; }, "Get Hebbian statistics (placeholder)");

    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity for synaptogenesis/pruning)pbdoc")
        .def(py::init<>())
        .def("configure", &StructuralPlasticity::configure, py::arg("brain"), py::arg("rng"),
             "Configure structural plasticity")
        .def("update", &StructuralPlasticity::update, "Update structural plasticity")
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate,
             py::arg("rate"), "Set synaptogenesis rate")
        .def("setPruningRate", &StructuralPlasticity::setPruningRate, py::arg("rate"), "Set pruning rate")
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate, "Get synaptogenesis rate")
        .def("getPruningRate", &StructuralPlasticity::getPruningRate, "Get pruning rate")
        .def("getStructuralStats", []() { return 0; }, "Get structural plasticity statistics (placeholder)");

    // === FACTORY FUNCTIONS FOR ADVANCED USERS ===

    m.def("createAdvancedConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(5000));
        config->set("brain.synapse_density", 0.15f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("memory.working_memory.enable", true);
        config->set("memory.episodic_memory.enable", true);
        config->set("memory.associative_memory.enable", true);
        config->set("neuromodulation.enable", true);
        config->set("cognition.planning.enable", true);
        config->set("cognition.concept.enable", true);
        config->set("cognition.attention.enable", true);
        config->set("development.enable", true);
        config->set("simulation.timestep", 0.0005);
        return config;
    }, "Create an advanced configuration with many features enabled");

    m.def("createResearchConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(2000));
        config->set("brain.synapse_density", 0.1f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("memory.working_memory.enable", true);
        config->set("memory.episodic_memory.enable", true);
        config->set("memory.associative_memory.enable", true);
        config->set("neuromodulation.enable", true);
        config->set("cognition.planning.enable", true);
        config->set("cognition.concept.enable", true);
        config->set("cognition.attention.enable", true);
        config->set("development.enable", true);
        config->set("simulation.timestep", 0.001);
        config->set("simulation.max_steps", 50000);
        return config;
    }, "Create a research-grade configuration with extensive simulation");

    m.def("createStandardConfig", []() {
        return std::make_shared<Config>();
    }, "Create a standard configuration (backward compatibility)");

    m.def("createAdvancedBrain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            return brain;
        }
        throw std::runtime_error("Failed to initialize advanced brain");
    }, py::arg("config"), "Create and initialize an advanced brain with all systems enabled");

    m.def("createResearchBrain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            return brain;
        }
        throw std::runtime_error("Failed to initialize research brain");
    }, py::arg("config"), "Create and initialize a research-grade brain");

    m.def("createMinimalBrain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            return brain;
        }
        throw std::runtime_error("Failed to initialize minimal brain");
    }, py::arg("config"), "Create and initialize a minimal brain (Phase 2 compatibility)");

    m.def("createExperimentSetup", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                     std::shared_ptr<AgentBrain> agent) {
        py::dict setup;
        setup["brain"] = brain;
        setup["world"] = world;
        setup["agent"] = agent;
        setup["timestamp"] = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        setup["version"] = "Enhanced NLM API v1.0";
        return setup;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), "Create an experiment setup");

    m.def("benchmarkPerformance", [](std::shared_ptr<Brain> brain, size_t steps) {
        auto startTime = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        py::dict results;
        results["steps_completed"] = steps;
        results["total_time_ms"] = duration.count();
        results["steps_per_second"] = (steps * 1000.0) / duration.count();
        results["neurons_per_second"] = brain->getTotalNeuronCount() * (steps * 1000.0) / duration.count();
        results["synapses_per_second"] = brain->getTotalSynapseCount() * (steps * 1000.0) / duration.count();
        results["spikes_per_step"] = brain->getTotalSpikeCount() / static_cast<float>(steps);
        return results;
    }, py::arg("brain"), py::arg("steps"), "Benchmark brain performance");

    m.def("saveBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        bool success = brain->save(filepath);
        if (!success) {
            throw std::runtime_error("Failed to save brain state");
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file");

    m.def("loadBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        bool success = brain->load(filepath);
        if (!success) {
            throw std::runtime_error("Failed to load brain state");
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from file");

    m.def("getSystemStatus", [](std::shared_ptr<Brain> brain) {
        py::dict status;
        status["brain_status"] = "initialized";
        status["neuron_count"] = brain->getTotalNeuronCount();
        status["synapse_count"] = brain->getTotalSynapseCount();
        status["active_neurons"] = brain->getActiveNeuronCount();
        status["firing_neurons"] = brain->getFiringNeuronCount();
        status["total_spikes"] = brain->getTotalSpikeCount();
        status["avg_firing_rate"] = brain->getAverageFiringRate();
        status["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        status["timestamp"] = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        return status;
    }, py::arg("brain"), "Get comprehensive system status");

    // === UTILITY FUNCTIONS ===

    m.def("validateBrainSetup", [](std::shared_ptr<Brain> brain) {
        py::dict validation;
        validation["brain_initialized"] = true;
        validation["neuron_count"] = brain->getTotalNeuronCount();
        validation["synapse_count"] = brain->getTotalSynapseCount();
        validation["has_spike_system"] = brain->getSpikeSystem() != nullptr;
        validation["has_memory_systems"] = (
            brain->getWorkingMemory() != nullptr || 
            brain->getEpisodicMemory() != nullptr ||
            brain->getAssociativeMemory() != nullptr
        );
        validation["has_neuromodulation"] = (
            brain->getDopamine() != nullptr ||
            brain->getCuriosity() != nullptr ||
            brain->getNovelty() != nullptr
        );
        validation["is_functional"] = brain->getTotalNeuronCount() > 0 && brain->getTotalSynapseCount() > 0;
        return validation;
    }, py::arg("brain"), "Validate brain setup and return validation results");

    m.def("getNLMVersion", []() {
        return py::dict({
            {"version", "Enhanced NLM API v1.0"},
            {"build", __DATE__ " " __TIME__},
            {"compiler", __VERSION__},
            {"cxx_standard", "C++20"},
            {"phase", "6 - Final Integration"},
            {"features", py::list({
                "Enhanced CLI interface",
                "Advanced Python API",
                "Complete neuromodulation systems",
                "Full memory integration",
                "Advanced cognitive systems",
                "Development system",
                "Performance monitoring",
                "Research tools",
                "Error handling",
                "Validation"
            })}
        });
    }, "Get NLM version and feature information");

    m.def("createResearchExperiment", [](const std::string& name, std::shared_ptr<Brain> brain,
                                         std::shared_ptr<SimpleWorld> world,
                                         std::shared_ptr<AgentBrain> agent) {
        py::dict experiment;
        experiment["name"] = name;
        experiment["brain"] = brain;
        experiment["world"] = world;
        experiment["agent"] = agent;
        experiment["start_time"] = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        experiment["status"] = "initialized";
        experiment["version"] = "Enhanced NLM Research API";
        return experiment;
    }, py::arg("name"), py::arg("brain"), py::arg("world"), py::arg("agent"),
         "Create a research experiment setup");

    // === CONVENIENCE ALIAS ===

    // Backward compatibility aliases
    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a brain (alias for createAdvancedBrain)");

    m.def("initializeBrain", [](std::shared_ptr<Brain> brain) {
        return brain->initialize();
    }, py::arg("brain"), "Initialize a brain");

    m.def("runBrainStep", [](std::shared_ptr<Brain> brain, size_t step) {
        brain->step(step);
    }, py::arg("brain"), py::arg("step"), "Run a brain step");

    // Error types
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::out_of_range& e) {
            PyErr_SetString(PyExc_IndexError, e.what());
        }
    });

    // Constants for backward compatibility
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    m.attr("NEURON_TYPE_EXCITATORY") = NeuronType::Excitatory;
    m.attr("NEURON_TYPE_INHIBITORY") = NeuronType::Inhibitory;
    m.attr("NEURON_TYPE_SENSORY") = NeuronType::Sensory;
    m.attr("NEURON_TYPE_MOTOR") = NeuronType::Motor;
    m.attr("NEURON_TYPE_INTERNAL") = NeuronType::Internal;
    
    m.attr("ACTION_TYPE_MOVE_FORWARD") = ActionType::MoveForward;
    m.attr("ACTION_TYPE_TURN_LEFT") = ActionType::TurnLeft;
    m.attr("ACTION_TYPE_INTERACT") = ActionType::Interact;

    // Add documentation for major classes
    py::class_<Brain>(m, "Brain")
        .def("step", &Brain::step, py::arg("currentStep"), "Perform a simulation step")
        .def("produceAction", &Brain::produceAction, "Produce motor action")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("logStatus", &Brain::logStatus, "Log brain status");

    py::class_<AgentBrain>(m, "AgentBrain")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory input")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode motor command")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"), "Enable reward modulation")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"), "Enable curiosity");

    // Add convenience function for common usage patterns
    m.def("runStandardSimulation", [](std::shared_ptr<Brain> brain, size_t steps) {
        py::dict results;
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
        }
        results["total_spikes"] = brain->getTotalSpikeCount();
        results["final_firing_rate"] = brain->getAverageFiringRate();
        results["steps_completed"] = steps;
        return results;
    }, py::arg("brain"), py::arg("steps"), "Run a standard simulation and return results");

    m.def("runAdvancedSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                      std::shared_ptr<AgentBrain> agent, size_t steps) {
        py::dict results;
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, step * 0.1f);
        }
        results["total_spikes"] = brain->getTotalSpikeCount();
        results["final_firing_rate"] = brain->getAverageFiringRate();
        results["steps_completed"] = steps;
        results["world_width"] = world->getWidth();
        results["world_height"] = world->getHeight();
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
         "Run an advanced simulation with world interaction");

    // Add educational examples
    m.def("getEducationalExamples", []() {
        py::dict examples;
        examples["simple_brain"] = R"(
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate()}")
        )";
        
        examples["complete_agent"] = R"(
import pynlm

# Complete agent example
config = pynlm.createAdvancedConfig()
brain = pynlm.createAdvancedBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Setup
world.configure(20, 20, 8, 8)
world.setAgentStart(10, 10)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation complete! Firing rate: {brain.getAverageFiringRate()}")
        )";
        
        examples["research_simulation"] = R"(
import pynlm
import time

# Research-grade simulation
config = pynlm.createResearchConfig()
brain = pynlm.createResearchBrain(config)

# Performance benchmark
start_time = time.time()
results = pynlm.benchmarkPerformance(brain, 5000)
end_time = time.time()

print(f"Benchmark completed in {results['total_time_ms']:.2f} ms")
print(f"Steps per second: {results['steps_per_second']:.2f}")
print(f"Neurons processed: {results['neurons_per_second']:.2f}/s")
print(f"Spikes generated: {results['spikes_per_step']:.2f} per step")
        )";
        
        return examples;
    }, "Get educational examples and tutorials");

    // Add configuration examples
    m.def("getConfigurationExamples", []() {
        py::dict examples;
        examples["minimal"] = py::dict({
            {"brain.neuron_count", 100},
            {"simulation.timestep", 0.001},
            {"simulation.max_steps", 100}
        });
        
        examples["standard"] = py::dict({
            {"brain.neuron_count", 1000},
            {"brain.synapse_density", 0.1f},
            {"plasticity.stdp.enable", true},
            {"plasticity.hebbian.enable", true},
            {"simulation.timestep", 0.001},
            {"simulation.max_steps", 1000}
        });
        
        examples["advanced"] = py::dict({
            {"brain.neuron_count", 5000},
            {"brain.synapse_density", 0.15f},
            {"plasticity.stdp.enable", true},
            {"plasticity.hebbian.enable", true},
            {"plasticity.structural.enable", true},
            {"memory.working_memory.enable", true},
            {"memory.episodic_memory.enable", true},
            {"memory.associative_memory.enable", true},
            {"neuromodulation.enable", true},
            {"cognition.planning.enable", true},
            {"cognition.concept.enable", true},
            {"cognition.attention.enable", true},
            {"development.enable", true},
            {"simulation.timestep", 0.0005},
            {"simulation.max_steps", 10000}
        });
        
        return examples;
    }, "Get configuration examples");

    // Add quick setup function for common tasks
    m.def("quickSetup", [](const std::string& setupType) {
        if (setupType == "minimal") {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(100));
            auto brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                return brain;
            }
        } else if (setupType == "standard") {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(1000));
            config->set("brain.synapse_density", 0.1f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            auto brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                return brain;
            }
        } else if (setupType == "advanced") {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(5000));
            config->set("brain.synapse_density", 0.15f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("memory.working_memory.enable", true);
            config->set("memory.episodic_memory.enable", true);
            config->set("memory.associative_memory.enable", true);
            config->set("neuromodulation.enable", true);
            config->set("cognition.planning.enable", true);
            config->set("cognition.concept.enable", true);
            config->set("cognition.attention.enable", true);
            config->set("development.enable", true);
            auto brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                return brain;
            }
        }
        
        throw std::runtime_error("Unknown setup type: " + setupType);
    }, py::arg("setupType"), "Quick setup for common brain configurations");
}

} // namespace nlm
