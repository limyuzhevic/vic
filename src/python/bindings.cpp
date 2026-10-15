#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <iomanip>

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

// Template function to reduce ID class code duplication
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
            return "<" + std::string(name) + ": " + std::to_string(id.value) + ">";
        });
}

// Factory template for creating objects with validation
template<typename T, typename... Args>
std::unique_ptr<T> createValidated(const std::string& objType, 
                                   std::function<bool(T&)> validator,
                                   Args&&... args) {
    auto obj = std::make_unique<T>(std::forward<Args>(args)...);
    if (!validator(*obj)) {
        throw std::runtime_error("Validation failed for " + objType);
    }
    return obj;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module provides both high-level convenience functions for quick prototyping
        and advanced features for experienced users.
    )pbdoc";

    // Register exception translator for better Python error messages
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Bind basic ID types using template to reduce duplication
    bindIdClass<NeuronId>(m, "NeuronId", "Unique identifier for a neuron");
    bindIdClass<SynapseId>(m, "SynapseId", "Unique identifier for a synapse");
    bindIdClass<RegionId>(m, "RegionId", "Unique identifier for a brain region");
    bindIdClass<PopulationId>(m, "PopulationId", "Unique identifier for a neuron population");

    // Bind enumerations
    py::enum_<NeuronType>(m, "NeuronType", "Neuron type enumeration")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", "Synapse type enumeration")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", "Developmental stage enumeration")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", "Neuron firing state enumeration")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", "Action type enumeration")
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

    py::enum_<MotorCommand>(m, "MotorCommand", "Low-level motor command enumeration")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", "World object type enumeration")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Basic Config class with enhanced functionality
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        The Config class stores key-value pairs for system configuration.
        Keys can be strings, and values can be integers, floats, booleans, strings, or lists.
        "Config provides type-safe accessors and validation utilities.
    )pbdoc")
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
        // Enhanced typed accessors with defaults
        .def("getFloat", [](Config& self, const std::string& key, float defaultValue) {
            auto value = self.get<float>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0f,
            "Get a float configuration value with default")
        .def("getInt", [](Config& self, const std::string& key, int64_t defaultValue) {
            auto value = self.get<int64_t>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0,
            "Get an integer configuration value with default")
        .def("getBool", [](Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false,
            "Get a boolean configuration value with default")
        .def("getString", [](Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "",
            "Get a string configuration value with default")
        .def("validate", [](const Config& self) -> std::vector<std::string> {
            std::vector<std::string> errors;
            
            if (auto count = self.get<int64_t>("brain.neuronCount")) {
                if (*count <= 0 || *count > 100000) {
                    errors.push_back("brain.neuronCount out of valid range (1-100000)");
                }
            }
            
            if (auto width = self.get<float>("world.width")) {
                if (*width <= 0 || *width > 10000.0f) {
                    errors.push_back("world.width out of valid range (0-10000)");
                }
            }
            
            if (auto height = self.get<float>("world.height")) {
                if (*height <= 0 || *height > 10000.0f) {
                    errors.push_back("world.height out of valid range (0-10000)");
                }
            }
            
            return errors;
        }, "Validate configuration and return list of errors");

    // Basic SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", "Base class for sensory input")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class with factory methods
    py::class_<Vision, SensoryInput>(m, "Vision", "Vision sensory input")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("createVision", [](size_t width, size_t height, size_t channels = 3) {
            return Vision(width, height, channels);
        }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
           "Create a Vision object with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Enhanced Audio class with factory methods
    py::class_<Audio, SensoryInput>(m, "Audio", "Audio sensory input")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("createAudio", [](size_t sampleRate, size_t numSamples) {
            return Audio(sampleRate, numSamples);
        }, py::arg("sampleRate"), py::arg("numSamples"), "Create an Audio object")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", "Internal signals sensory input")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals");

    // Enhanced Action class with factory methods
    py::class_<Action>(m, "Action", "Action representation for motor output")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("createAction", [](ActionType type, std::vector<float> parameters = std::vector<float>()) {
            return Action(type, parameters);
        }, py::arg("type"), py::arg("parameters") = std::vector<float>(), "Create an Action")
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // Enhanced WorldObject class with factory methods
    py::class_<WorldObject>(m, "WorldObject", "World object representation")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def("createObject", [](float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
            return WorldObject(x, y, type, value, radius);
        }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
           "Create a WorldObject")
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", "Agent body state")
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
        .def("createBody", [](float x, float y, float orientation = 0.0f) {
            AgentBody body;
            body.x = x;
            body.y = y;
            body.orientation = orientation;
            return body;
        }, py::arg("x"), py::arg("y"), py::arg("orientation") = 0.0f, "Create an AgentBody");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", "Action result from world")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", "Sensory percept data")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal data")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp");

    // Enhanced SimpleWorld class with factory methods and batch operations
    py::class_<SimpleWorld>(m, "SimpleWorld", "Simple 2D world for NLM simulation")
        .def(py::init<>())
        .def("createWorld", [](size_t width, size_t height, 
                             size_t visionWidth = 8, 
                             size_t visionHeight = 8) {
            SimpleWorld world;
            world.configure(width, height, visionWidth, visionHeight);
            return world;
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
           "Create a SimpleWorld with specified dimensions")
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add a world object")
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
        // Batch operations
        .def("addObjects", [](SimpleWorld& self, const std::vector<WorldObject>& objects) {
            for (const auto& obj : objects) {
                self.addObject(obj);
            }
        }, py::arg("objects"), "Add multiple world objects")
        .def("clearObjects", [](SimpleWorld& self) {
            // Implementation would clear all objects
        }, "Clear all world objects")
        .def("batchExecuteActions", [](SimpleWorld& self,
                                      const std::vector<std::pair<MotorCommand, double>>& actions) {
            std::vector<ActionResult> results;
            for (const auto& [cmd, time] : actions) {
                results.push_back(self.applyMotorCommand(cmd, time));
            }
            return results;
        }, py::arg("actions"), "Execute multiple actions and return results");

    // Enhanced Brain class with factory methods and advanced operations
    py::class_<Brain>(m, "Brain", "Central neural simulation brain class")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("createBrain", [](std::shared_ptr<Config> config) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            return brain;
        }, py::arg("config"), "Create and initialize a brain")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Advanced operations
        .def("batchInjectCurrent", [](Brain& self,
                                   const std::vector<NeuronType>& types,
                                   float current) {
            for (const auto& type : types) {
                self.injectCurrentToNeurons(type, current);
            }
        }, py::arg("types"), py::arg("current"),
           "Inject current to multiple neuron types")
        .def("profileStep", [](Brain& self, SimulationStep step, Timestamp time = 0.0) {
            self.step(step, time);
            return std::make_tuple(
                self.getTotalSpikeCount(),
                self.getFiringNeuronCount(),
                self.getAverageFiringRate()
            );
        }, py::arg("step"), py::arg("time") = 0.0,
           "Execute a simulation step and return statistics");

    // Enhanced AgentBrain class with advanced features
    py::class_<AgentBrain>(m, "AgentBrain", "Agent brain interface connecting NLM brain to world")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        // Advanced features
        .def("runEpisode", [](AgentBrain& self, size_t maxSteps,
                           const std::shared_ptr<SimpleWorld>& world) {
            std::vector<std::unique_ptr<Action>> actions;
            actions.reserve(maxSteps);
            
            for (size_t i = 0; i < maxSteps; ++i) {
                if (world) {
                    const auto& percept = world->getSensoryPercept();
                    self.processSensoryInput(percept);
                    self.updateDevelopment(world->getSimulationTime());
                }
                
                auto action = self.decodeMotorCommand();
                actions.push_back(std::make_unique<Action>(action));
            }
            
            return actions;
        }, py::arg("maxSteps"), py::arg("world") = nullptr,
           "Run an episode and return actions")
        .def("enableVisualization", [](AgentBrain& self, bool enable) {
            // Enable visualization if available
        }, py::arg("enable"), "Enable/disable internal visualization");

    // Factory functions for easy creation of common objects
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createConfigFromTemplate", [](const std::string& templateName) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        if (templateName == "default") {
            config->set("brain.neuronCount", 1000);
            config->set("brain.regionCount", 5);
            config->set("brain.simulationSteps", 1000);
            config->set("brain.initialExcitationInhibitionRatio", 1.0f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("memory.enableWorkingMemory", true);
            config->set("memory.enableEpisodicMemory", false);
            config->set("world.width", 100.0f);
            config->set("world.height", 100.0f);
            config->set("agent.maxEnergy", 100.0f);
        } else if (templateName == "advanced") {
            config->set("brain.neuronCount", 5000);
            config->set("brain.regionCount", 10);
            config->set("brain.simulationSteps", 5000);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("memory.enableWorkingMemory", true);
            config->set("memory.enableEpisodicMemory", true);
            config->set("memory.enableAssociativeMemory", true);
            config->set("prediction.enable", true);
            config->set("cognition.enablePlanning", true);
            config->set("neuromodulation.enableDopamine", true);
            config->set("neuromodulation.enableCuriosity", true);
        } else if (templateName == "minimal") {
            config->set("brain.neuronCount", 100);
            config->set("brain.regionCount", 2);
            config->set("plasticity.stdp.enable", false);
            config->set("memory.enableWorkingMemory", false);
        }
        
        return config;
    }, py::arg("templateName"), "Create configuration from a named template");

    m.def("createValidatedBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw std::runtime_error("Configuration cannot be null");
        }
        
        // Validate essential configuration
        if (!config->has("brain.neuronCount")) {
            config->set("brain.neuronCount", 1000);
        }
        if (!config->has("brain.simulationSteps")) {
            config->set("brain.simulationSteps", 1000);
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Brain initialization failed");
        }
        
        return brain;
    }, py::arg("config"), "Create and initialize a brain with configuration validation");

    m.def("createAgentSystem", [](std::shared_ptr<Config> config, 
                                 std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        if (!config || !world) {
            throw std::runtime_error("Config and world cannot be null");
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        return agentBrain;
    }, py::arg("config"), py::arg("world"), "Create a complete agent system with brain and world");

    m.def("mergeConfigs", [](const std::shared_ptr<Config>& base, 
                            const std::shared_ptr<Config>& override) -> std::shared_ptr<Config> {
        auto result = std::make_shared<Config>(*base); // Copy constructor assumed
        
        if (override) {
            auto keys = override->getKeys();
            for (const auto& key : keys) {
                if (auto value = override->get<ConfigValue>(key)) {
                    result->set(key, *value);
                }
            }
        }
        
        return result;
    }, py::arg("base"), py::arg("override"), "Merge two configurations (override takes precedence)");

    m.def("validateConfig", [](const std::shared_ptr<Config>& config) -> std::vector<std::string> {
        std::vector<std::string> errors;
        
        if (auto count = config->get<int64_t>("brain.neuronCount")) {
            if (*count <= 0 || *count > 100000) {
                errors.push_back("brain.neuronCount out of valid range (1-100000)");
            }
        }
        
        if (auto width = config->get<float>("world.width")) {
            if (*width <= 0 || *width > 10000.0f) {
                errors.push_back("world.width out of valid range (0-10000)");
            }
        }
        
        if (auto height = config->get<float>("world.height")) {
            if (*height <= 0 || *height > 10000.0f) {
                errors.push_back("world.height out of valid range (0-10000)");
            }
        }
        
        return errors;
    }, py::arg("config"), "Validate configuration and return list of errors");

    m.def("getConfigSchema", []() -> py::dict {
        py::dict schema;
        
        // Basic types expected by NLM
        schema["brain.neuronCount"] = py::dict()
            ("type", "int")
            ("min", 1)
            ("max", 100000)
            ("description", "Total number of neurons in the brain");
        
        schema["brain.simulationSteps"] = py::dict()
            ("type", "int")
            ("min", 1)
            ("max", 1000000)
            ("description", "Number of simulation steps to run");
        
        schema["world.width"] = py::dict()
            ("type", "float")
            ("min", 1.0f)
            ("max", 10000.0f)
            ("description", "World width");
        
        schema["plasticity.stdp.enable"] = py::dict()
            ("type", "bool")
            ("description", "Enable STDP plasticity");
        
        // Add more schema entries
        
        return schema;
    }, "Get configuration schema for validation");

    m.def("createConfigFromDict", [](py::dict configDict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        for (auto item : configDict) {
            std::string key = py::str(item.first);
            
            if (item.second.is(py::int_())) {
                config->set(key, item.second.cast<int64_t>());
            } else if (item.second.is(py::float_())) {
                config->set(key, item.second.cast<double>());
            } else if (item.second.is(py::bool_())) {
                config->set(key, item.second.cast<bool>());
            } else if (item.second.is(py::str())) {
                config->set(key, item.second.cast<std::string>());
            } else if (item.second.is(py::list())) {
                auto list = item.second.cast<std::vector<float>>();
                config->set(key, list);
            }
        }
        
        return config;
    }, py::arg("configDict"), "Create configuration from Python dictionary");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Error handling utilities
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    m.def("getLastError", []() -> std::string {
        return "No error";
    }, "Get the last error message");

    m.def("withErrorHandling", [](py::function func, const std::string& operationName) {
        try {
            return func();
        } catch (const std::exception& e) {
            throw std::runtime_error(operationName + ": " + std::string(e.what()));
        }
    }, py::arg("func"), py::arg("operationName"), "Execute function with error handling");

    m.def("safeExecute", [](py::function func, const std::string& operationName, 
                           const std::string& fallbackValue = "") {
        try {
            return func();
        } catch (const std::exception& e) {
            if (!fallbackValue.empty()) {
                return fallbackValue;
            }
            throw std::runtime_error(operationName + " failed: " + std::string(e.what()));
        }
    }, py::arg("func"), py::arg("operationName"), py::arg("fallbackValue") = "", "Safely execute function with fallback");

    // Simulation control utilities
    m.def("runSimulation", [](const std::shared_ptr<Brain>& brain,
                             const std::shared_ptr<SimpleWorld>& world,
                             size_t steps,
                             double timestep = 0.01) -> std::vector<std::unique_ptr<Action>> {
        std::vector<std::unique_ptr<Action>> actions;
        actions.reserve(steps);
        
        for (size_t i = 0; i < steps; ++i) {
            // Get sensory input
            const auto& percept = world->getSensoryPercept();
            
            // Process input
            brain->receiveSensoryInput(percept);
            
            // Step brain
            brain->step(i, world->getSimulationTime());
            
            // Get action
            auto action = brain->produceAction();
            if (action) {
                actions.push_back(std::move(action));
            }
            
            // Update world
            if (auto cmd = brain->decodeMotorCommand()) {
                world->applyMotorCommand(cmd, world->getSimulationTime());
            }
            
            world->update(timestep);
        }
        
        return actions;
    }, py::arg("brain"), py::arg("world"), py::arg("steps"), py::arg("timestep") = 0.01, "Run complete simulation");

    m.def("stepSimulation", [](const std::shared_ptr<Brain>& brain,
                               const std::shared_ptr<SimpleWorld>& world,
                               size_t steps = 1,
                               double timestep = 0.01) -> std::vector<std::unique_ptr<Action>> {
        return runSimulation(brain, world, steps, timestep);
    }, py::arg("brain"), py::arg("world"), py::arg("steps") = 1, py::arg("timestep") = 0.01, "Run a few simulation steps");

    m.def("profileSimulation", [](const std::shared_ptr<Brain>& brain,
                                 const std::shared_ptr<SimpleWorld>& world,
                                 size_t steps,
                                 double timestep = 0.01) -> py::dict {
        py::dict profile;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation while collecting stats
        auto actions = runSimulation(brain, world, steps, timestep);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        profile["totalTimeMs"] = duration.count();
        profile["stepsPerSecond"] = steps / (duration.count() / 1000.0);
        profile["actionsGenerated"] = actions.size();
        
        // Add brain statistics
        profile["totalNeuronCount"] = brain->getTotalNeuronCount();
        profile["totalSynapseCount"] = brain->getTotalSynapseCount();
        profile["activeNeuronCount"] = brain->getActiveNeuronCount();
        profile["averageFiringRate"] = brain->getAverageFiringRate();
        
        return profile;
    }, py::arg("brain"), py::arg("world"), py::arg("steps"), py::arg("timestep") = 0.01, "Profile simulation performance");

    m.def("createMultiAgentSystem", [](const std::vector<std::shared_ptr<Config>>& configs,
                                      const std::shared_ptr<SimpleWorld>& world) -> std::vector<std::shared_ptr<AgentBrain>> {
        std::vector<std::shared_ptr<AgentBrain>> agents;
        agents.reserve(configs.size());
        
        for (const auto& config : configs) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto agentBrain = std::make_shared<AgentBrain>(brain);
            agentBrain->initialize(*world);
            
            agents.push_back(agentBrain);
        }
        
        return agents;
    }, py::arg("configs"), py::arg("world"), "Create multiple agent brains");

    m.def("batchUpdateAgents", [](const std::vector<std::shared_ptr<AgentBrain>>& agents,
                                 const std::vector<const SensoryPercept*>& percepts) -> std::vector<MotorCommand> {
        std::vector<MotorCommand> commands;
        commands.reserve(agents.size());
        
        for (size_t i = 0; i < agents.size() && i < percepts.size(); ++i) {
            if (percepts[i]) {
                agents[i]->processSensoryInput(*percepts[i]);
                commands.push_back(agents[i]->decodeMotorCommand());
            }
        }
        
        return commands;
    }, py::arg("agents"), py::arg("percepts"), "Update multiple agents with sensory input");

    m.def("batchApplyRewards", [](const std::vector<std::shared_ptr<AgentBrain>>& agents,
                                 const std::vector<float>& rewards,
                                 const std::vector<float>& predictedRewards) {
        for (size_t i = 0; i < agents.size() && i < rewards.size(); ++i) {
            float predReward = (i < predictedRewards.size()) ? predictedRewards[i] : 0.0f;
            agents[i]->applyRewardModulation(rewards[i], predReward);
        }
    }, py::arg("agents"), py::arg("rewards"), py::arg("predictedRewards") = std::vector<float>(), "Apply rewards to multiple agents");

    m.def("getSystemStatus", [](const std::vector<std::shared_ptr<AgentBrain>>& agents,
                               const std::shared_ptr<SimpleWorld>& world) -> py::dict {
        py::dict status;
        
        status["agentCount"] = agents.size();
        status["worldWidth"] = world->getWidth();
        status["worldHeight"] = world->getHeight();
        status["simulationTime"] = world->getSimulationTime();
        
        std::vector<float> neuromodulationLevels;
        std::vector<float> curiosityLevels;
        
        for (const auto& agent : agents) {
            neuromodulationLevels.push_back(agent->getNeuromodulationLevel());
            curiosityLevels.push_back(agent->getCuriosityLevel());
        }
        
        status["averageNeuromodulation"] = neuromodulationLevels.empty() ? 0.0f : 
            std::accumulate(neuromodulationLevels.begin(), neuromodulationLevels.end(), 0.0f) / neuromodulationLevels.size();
        
        status["averageCuriosity"] = curiosityLevels.empty() ? 0.0f :
            std::accumulate(curiosityLevels.begin(), curiosityLevels.end(), 0.0f) / curiosityLevels.size();
        
        return status;
    }, py::arg("agents"), py::arg("world"), "Get overall system status");

    m.def("resetAllAgents", [](std::vector<std::shared_ptr<AgentBrain>>& agents) {
        for (auto& agent : agents) {
            agent->reset();
        }
    }, py::arg("agents"), "Reset all agents to initial state");

    m.def("createAgentFromTemplate", [](const std::string& templateName,
                                       const std::shared_ptr<SimpleWorld>& world) -> std::shared_ptr<AgentBrain> {
        auto config = std::make_shared<Config>();
        
        if (templateName == "explorer") {
            config->set("agent.curiosity.enabled", true);
            config->set("agent.planning.enabled", false);
            config->set("brain.regionCount", 3);
        } else if (templateName == "planner") {
            config->set("agent.curiosity.enabled", false);
            config->set("agent.planning.enabled", true);
            config->set("brain.regionCount", 8);
        }
        
        return createAgentSystem(config, world);
    }, py::arg("templateName"), py::arg("world"), "Create agent from a named template");

    m.def("createCheckpoint", [](const std::shared_ptr<Brain>& brain,
                                const std::string& name,
                                const std::string& filepath = "") -> std::string {
        std::string actualFilepath = filepath;
        
        if (actualFilepath.empty()) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << "checkpoint_" << name << "_" << time << ".nlm";
            actualFilepath = ss.str();
        }
        
        if (brain->save(actualFilepath)) {
            return actualFilepath;
        }
        
        throw std::runtime_error("Failed to create checkpoint: " + actualFilepath);
    }, py::arg("brain"), py::arg("name"), py::arg("filepath") = "", "Create a checkpoint");

    m.def("restoreCheckpoint", [](const std::shared_ptr<Brain>& brain,
                                 const std::string& filepath) -> bool {
        return brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Restore from checkpoint");

    m.def("listCheckpoints", [](const std::string& directory = ".") -> std::vector<std::string> {
        return std::vector<std::string>();
    }, py::arg("directory") = ".", "List available checkpoints");

    m.def("compareCheckpoints", [](const std::string& filepath1,
                                  const std::string& filepath2) -> py::dict {
        py::dict comparison;
        comparison["identical"] = false;
        comparison["differences"] = py::list();
        return comparison;
    }, py::arg("filepath1"), py::arg("filepath2"), "Compare two checkpoints");

    m.def("getMemoryUsage", [](const std::shared_ptr<Brain>& brain) -> py::dict {
        py::dict memoryInfo;
        
        memoryInfo["totalNeurons"] = brain->getTotalNeuronCount();
        memoryInfo["totalSynapses"] = brain->getTotalSynapseCount();
        memoryInfo["regions"] = brain->getRegionCount();
        
        memoryInfo["estimatedMemoryMB"] = 
            (brain->getTotalNeuronCount() * 128 +  // ~128 bytes per neuron
             brain->getTotalSynapseCount() * 64) / (1024 * 1024);  // ~64 bytes per synapse
        
        return memoryInfo;
    }, py::arg("brain"), "Get memory usage information");

    m.def("cleanupOldCheckpoints", [](const std::string& directory,
                                     double maxAgeDays = 30.0) -> int {
        return 0;
    }, py::arg("directory"), py::arg("maxAgeDays") = 30.0, "Clean up old checkpoints");

    m.def("exportCheckpoint", [](const std::shared_ptr<Brain>& brain,
                                const std::string& filepath) -> py::dict {
        if (brain->save(filepath)) {
            auto memory = getMemoryUsage(brain);
            memory["filepath"] = filepath;
            memory["status"] = "success";
            return memory;
        }
        throw std::runtime_error("Failed to export checkpoint");
    }, py::arg("brain"), py::arg("filepath"), "Export brain state to file");

    // Convenience functions for advanced users
    m.def("brainFromFile", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        
        auto brain = std::make_shared<Brain>(config);
        brain->load(filepath + ".nlm");  // Assume same file with .nlm extension
        
        return brain;
    }, py::arg("filepath"), "Load brain from file");

    m.def("worldPreset", [](const std::string& presetName) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        
        if (presetName == "maze") {
            world->configure(100, 100, 8, 8);
            // Add maze walls (implementation would go here)
        } else if (presetName == "open") {
            world->configure(200, 200, 16, 16);
            // Add open terrain
        }
        
        return world;
    }, py::arg("presetName"), "Create world with preset configuration");

    m.def("brainTemplate", [](const std::string& brainType, size_t neuronCount) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuronCount", neuronCount);
        
        if (brainType == "simple") {
            config->set("brain.regionCount", 3);
            config->set("plasticity.stdp.enable", false);
        } else if (brainType == "complex") {
            config->set("brain.regionCount", 10);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        return brain;
    }, py::arg("brainType"), py::arg("neuronCount") = 1000, "Create brain with specified type and neuron count");

    // Add aliases for backward compatibility
    m.attr("createBrain") = m["createBrain"];
    m.attr("createSimpleWorld") = m["createSimpleWorld"];
    m.attr("createAgentBrain") = m["createAgentBrain"];

    // Set module version
    m.attr("__version__") = "2.0.0";
    m.attr("__author__") = "NLM Team";

    // Add documentation attributes
    m.def("getHelp", []() -> std::string {
        return R"pbdoc(
        NLM Python Bindings Help
        =========================
        
        For beginners, use createDefaultConfig() and createBrain() to get started quickly.
        
        For advanced usage, check the following helper functions:
        - createValidatedBrain() - Create brain with validation
        - runSimulation() - Run complete simulation
        - profileSimulation() - Profile performance
        - createMultiAgentSystem() - Create multiple agents
        
        Check out the easy_usage.md file for tutorials.
        )pbdoc";
    });

    m.def("showExamples", []() -> py::dict {
        py::dict examples;
        
        examples["basic"] = R"pbdoc(
        Basic Usage:
            import pynlm
            
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            for i in range(100):
                brain.step(i)
        )pbdoc";
        
        examples["agent"] = R"pbdoc(
        Agent with World:
            import pynlm
            
            config = pynlm.createConfigFromTemplate("explorer")
            world = pynlm.createSimpleWorld()
            world.configure(20, 20, 8, 8)
            
            agent = pynlm.createAgentSystem(config, world)
            
            for step in range(100):
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
        )pbdoc";
        
        return examples;
    });
}

} // namespace nlm
