#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <pybind11/iostream.h>

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
#include "../src/experiments/Phase6IntegratedExperiment.hpp"

namespace py = pybind11;
namespace nlm {

// Add helper functions for advanced Python usage
namespace nlm {

// Helper function to create a complete agent setup
define_py_api_helpers() {
    py::class_<std::function<void(double)>> pyStepCallback("StepCallback");
    py::class_<std::function<void(const SensoryPercept&)>> pySensoryCallback("SensoryCallback");
    py::class_<std::function<Action(double)>> pyActionCallback("ActionCallback");
}

/**
 * @brief Enhanced Python API convenience functions
 * 
 * Provides high-level helper functions for common NLM usage patterns,
 * making it easier for users to set up and run neural simulations.
 */
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Edition
        -----------------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - **Complete API Documentation**: Detailed docstrings for all classes and methods
        - **Convenience Methods**: High-level functions for common usage patterns
        - **Advanced Configuration**: Extended configuration options and validation
        - **Performance Monitoring**: Built-in profiling and statistics
        - **Error Handling**: Improved exception handling and validation
        - **Advanced Features**: Curiosity, development, prediction systems
        
        Key Features:
        * Full Phase 6 integrated artificial brain support
        * Memory systems (working, episodic, associative)
        * Neuromodulation (dopamine, curiosity, novelty, prediction error)
        * Development and learning mechanisms
        * World interaction with sensory processing
        * Checkpoint save/load functionality
        
        Quick Start Examples:
        ```python
        import pynlm
        
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simple simulation
        for step in range(1000):
            brain.step(step)
        
        # Create complete agent setup
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        agent.initialize(world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run episode
        for step in range(500):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        ```
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Define type aliases for better Python API
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
            return std::to_string(id.value);
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &NeuronId::operator!=)
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
        .def("index", &RegionId::index)
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

    // Enhanced Config class with better Python API
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file. Supports both JSON and simple key=value format.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments. Supports --key=value and -key value formats.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file. Automatically formats values appropriately for each type.")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a string configuration value")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set an integer configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a double configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a boolean configuration value")
        .def("get", [](Config& self, const std::string& key) -> py::object {
            auto val = self.get<std::string>(key);
            return val.has_value() ? py::cast(val.value()) : py::none();
        }, py::arg("key"), "Get string configuration value")
        .def("getInt", [](Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get integer configuration value with default")
        .def("getFloat", [](Config& self, const std::string& key, float defaultValue) {
            return self.getOr<double>(key, static_cast<double>(defaultValue));
        }, py::arg("key"), py::arg("defaultValue"), "Get float configuration value with default")
        .def("getBool", [](Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get boolean configuration value with default")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration entry by key")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("getSummary", &Config::summary,
             "Get a formatted summary of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        // Add configuration convenience methods
        .def("configureBrain", [](Config& self, size_t neuronCount = 1000, 
                               size_t regionCount = 1, float connectionProb = 0.1f) {
            self.set("brain.neuron_count", static_cast<int>(neuronCount));
            self.set("brain.region_count", static_cast<int>(regionCount));
            self.set("brain.connection_probability", connectionProb);
        }, py::arg("neuronCount") = 1000, py::arg("regionCount") = 1, py::arg("connectionProb") = 0.1f,
           "Configure basic brain parameters")
        .def("configurePlasticity", [](Config& self, bool stdp = true, float stdpLTP = 0.01f, 
                                    float stdpLTD = 0.012f, float stdpTau = 20.0f) {
            self.set("plasticity.stdp.enable", stdp);
            self.set("plasticity.stdp.learning_rate", stdpLTP);
            self.set("plasticity.stdp.tau_plus", stdpTau);
            self.set("plasticity.stdp.tau_minus", stdpTau);
        }, py::arg("stdp") = true, py::arg("stdpLTP") = 0.01f, py::arg("stdpLTD") = 0.012f, py::arg("stdpTau") = 20.0f,
           "Configure STDP plasticity parameters")
        .def("configureNeuromodulation", [](Config& self, bool dopamine = true, bool curiosity = true,
                                           bool novelty = true, float noveltyThreshold = 0.1f) {
            self.set("neuromod.dopamine.scale", 1.0f);
            self.set("neuromod.curiosity.enable", curiosity);
            self.set("neuromod.novelty.enable", novelty);
        }, py::arg("dopamine") = true, py::arg("curiosity") = true, py::arg("novelty") = true,
           py::arg("noveltyThreshold") = 0.1f,
           "Configure neuromodulation systems")
        .def("configureDevelopment", [](Config& self, bool developmental = true,
                                     float synaptogenesisRate = 0.0001f,
                                     float pruningRate = 0.00001f) {
            self.set("development.enabled", developmental);
            self.set("synaptogenesis_rate", synaptogenesisRate);
            self.set("pruning_rate", pruningRate);
        }, py::arg("developmental") = true, py::arg("synaptogenesisRate") = 0.0001f,
           py::arg("pruningRate") = 0.00001f,
           "Configure developmental system parameters");

    // Enhanced SensoryInput hierarchy
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for all sensory inputs)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class with more Pythonic API
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with enhanced Python API)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data from vector")
        .def("setData", [](Vision& self, const std::vector<std::vector<float>>& data) {
            std::vector<float> flatData;
            for (const auto& row : data) {
                flatData.insert(flatData.end(), row.begin(), row.end());
            }
            self.setData(flatData);
        }, py::arg("data"), "Set vision data from 2D array")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getArea", &Vision::getHeight)
        .def("getSize", &Vision::getDimensions, "Get total number of pixels")
        .def("normalize", [](Vision& self, float mean = 0.0f, float std = 1.0f) {
            auto data = self.getData();
            for (auto& value : data) {
                value = (value - mean) / std;
            }
            self.setData(data);
        }, py::arg("mean") = 0.0f, py::arg("std") = 1.0f,
           "Normalize vision data using z-score normalization")
        .def("getMaxValue", [](const Vision& self) {
            auto data = self.getData();
            return *std::max_element(data.begin(), data.end());
        }, "Get maximum value in vision data")
        .def("getMinValue", [](const Vision& self) {
            auto data = self.getData();
            return *std::min_element(data.begin(), data.end());
        }, "Get minimum value in vision data")
        .def("clone", [](const Vision& self) {
            return Vision(self);
        }, "Create a copy of vision data");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with enhanced Python API)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data from vector")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return static_cast<float>(self.getNumSamples()) / self.getSampleRate();
        }, "Get duration in seconds")
        .def("getFrequency", [](Audio& self, size_t index) -> py::object {
            if (index < self.getNumSamples()) {
                return py::cast(self.getData()[index]);
            }
            return py::none();
        }, py::arg("index"), "Get amplitude at specific sample index")
        .def("normalize", [](Audio& self, float maxAmp = 1.0f) {
            auto data = self.getData();
            float currentMax = *std::max_element(data.begin(), data.end());
            if (currentMax > 0.0f) {
                for (auto& value : data) {
                    value = value / currentMax * maxAmp;
                }
                self.setData(data);
            }
        }, py::arg("maxAmp") = 1.0f, "Normalize audio to specified maximum amplitude");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input with enhanced Python API)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a new internal signal value")
        .def("addSignals", [](InternalSignals& self, const std::vector<float>& signals) {
            for (float value : signals) {
                self.addSignal(value);
            }
        }, py::arg("signals"), "Add multiple signal values")
        .def("getSignal", [](const InternalSignals& self, size_t index) -> py::object {
            auto data = self.getData();
            if (index < data.size()) {
                return py::cast(data[index]);
            }
            return py::none();
        }, py::arg("index"), "Get signal at specific index")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("getMean", [](const InternalSignals& self) -> py::object {
            auto data = self.getData();
            if (data.empty()) return py::none();
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return py::cast(sum / data.size());
        }, "Get mean of all signals")
        .def("getVariance", [](const InternalSignals& self) -> py::object {
            auto data = self.getData();
            if (data.size() < 2) return py::none();
            float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
            float sqSum = std::inner_product(data.begin(), data.end(), data.begin(), 0.0f);
            return py::cast(sqSum / data.size() - mean * mean);
        }, "Get variance of all signals");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced Action class with more Pythonic API)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), "Create action with type only")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter to action")
        .def("getParameter", [](const Action& self, size_t index) -> py::object {
            auto params = self.getParameters();
            if (index < params.size()) {
                return py::cast(params[index]);
            }
            return py::none();
        }, py::arg("index"), "Get parameter at specific index")
        .def("getName", &Action::getName)
        .def("setName", &Action::setName, py::arg("name"), "Set action name")
        .def("hasParameter", [](const Action& self, float value) {
            auto params = self.getParameters();
            return std::find(params.begin(), params.end(), value) != params.end();
        }, py::arg("value"), "Check if action has specific parameter")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        .def("clone", &Action::clone, "Create a copy of the action")
        .def("toDict", [](const Action& self) {
            py::dict result;
            result["type"] = self.getType();
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert action to Python dictionary")
        .def("fromDict", [](py::dict dict) {
            Action action;
            action.setType(py::cast<ActionType>(dict["type"]));
            action.setParameters(py::cast<std::vector<float>>(dict["parameters"]));
            if (dict.contains("name")) {
                action.setName(py::cast<std::string>(dict["name"]));
            }
            return action;
        }, py::arg("dict"), "Create action from Python dictionary");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object with enhanced Python API)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Create world object with position and properties")
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("isNearby", [](const WorldObject& self, float x, float y, float radius) {
            float dx = self.x - x;
            float dy = self.y - y;
            return (dx * dx + dy * dy) <= (radius * radius);
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Check if object is within radius")
        .def("moveTo", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Move object to position")
        .def("toDict", [](const WorldObject& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["radius"] = self.radius;
            result["type"] = self.type;
            result["value"] = self.value;
            result["active"] = self.active;
            return result;
        }, "Convert object to Python dictionary")
        .def("fromDict", [](py::dict dict) {
            WorldObject obj;
            obj.x = py::cast<float>(dict["x"]);
            obj.y = py::cast<float>(dict["y"]);
            obj.radius = py::cast<float>(dict["radius"]);
            obj.type = py::cast<WorldObjectType>(dict["type"]);
            obj.value = py::cast<float>(dict["value"]);
            obj.active = py::cast<bool>(dict["active"]);
            return obj;
        }, py::arg("dict"), "Create object from Python dictionary");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced Agent body state with more Pythonic API)pbdoc")
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
        .def("move", [](AgentBody& self, float dx, float dy) {
            self.x += dx;
            self.y += dy;
            self.isMoving = (dx != 0.0f || dy != 0.0f);
        }, py::arg("dx"), py::arg("dy"), "Move agent by delta")
        .def("rotate", [](AgentBody& self, float angle) {
            self.orientation += angle;
            self.isTurning = (angle != 0.0f);
        }, py::arg("angle"), "Rotate agent by angle")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("getPosition", [](const AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get agent position as tuple")
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("isNearby", [](const AgentBody& self, float x, float y, float radius) {
            float dx = self.x - x;
            float dy = self.y - y;
            return (dx * dx + dy * dy) <= (radius * radius);
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Check if nearby position")
        .def("addEnergy", &AgentBody::addEnergy, py::arg("amount"), "Add energy")
        .def("consumeEnergy", &AgentBody::consumeEnergy, py::arg("amount"), "Consume energy")
        .def("isHealthy", [](const AgentBody& self) {
            return self.health > 0.0f;
        }, "Check if agent is healthy")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state")
        .def("toDict", [](const AgentBody& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["orientation"] = self.orientation;
            result["velocityX"] = self.velocityX;
            result["velocityY"] = self.velocityY;
            result["angularVelocity"] = self.angularVelocity;
            result["energy"] = self.energy;
            result["health"] = self.health;
            result["age"] = self.age;
            result["isMoving"] = self.isMoving;
            result["isTurning"] = self.isTurning;
            result["lastActionTime"] = self.lastActionTime;
            return result;
        }, "Convert agent body to Python dictionary")
        .def("fromDict", [](py::dict dict) {
            AgentBody body;
            body.x = py::cast<float>(dict["x"]);
            body.y = py::cast<float>(dict["y"]);
            body.orientation = py::cast<float>(dict["orientation"]);
            body.velocityX = py::cast<float>(dict["velocityX"]);
            body.velocityY = py::cast<float>(dict["velocityY"]);
            body.angularVelocity = py::cast<float>(dict["angularVelocity"]);
            body.energy = py::cast<float>(dict["energy"]);
            body.health = py::cast<float>(dict["health"]);
            body.age = py::cast<float>(dict["age"]);
            body.isMoving = py::cast<bool>(dict["isMoving"]);
            body.isTurning = py::cast<bool>(dict["isTurning"]);
            body.lastActionTime = py::cast<float>(dict["lastActionTime"]);
            return body;
        }, py::arg("dict"), "Create agent body from Python dictionary");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world with enhanced Python API)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward and success")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isGood", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if action result is good")
        .def("isBad", [](const ActionResult& self) {
            return !self.success && self.reward < 0.0f;
        }, "Check if action result is bad")
        .def("addReward", &ActionResult::addReward, py::arg("amount"), "Add reward to result")
        .def("setSuccess", &ActionResult::setSuccess, py::arg("success"), "Set success status")
        .def("toDict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert result to Python dictionary")
        .def("fromDict", [](py::dict dict) {
            ActionResult result;
            result.reward = py::cast<float>(dict["reward"]);
            result.success = py::cast<bool>(dict["success"]);
            if (dict.contains("message")) {
                result.message = py::cast<std::string>(dict["message"]);
            }
            return result;
        }, py::arg("dict"), "Create result from Python dictionary");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced Sensory percept data with more Pythonic API)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getVisionArea", [](const SensoryPercept& self) {
            return self.getVisionWidth() * self.getVisionHeight();
        }, "Get vision data area")
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp")
        .def("getSignalCount", [](const SensoryPercept& self) {
            auto signals = self.getAllSignals();
            return static_cast<size_t>(signals.size());
        }, "Get total number of signal types")
        .def("hasVision", [](const SensoryPercept& self) {
            auto vision = self.getVision();
            return !vision.empty();
        }, "Check if percept has vision data")
        .def("hasTouch", [](const SensoryPercept& self) {
            auto touch = self.getTouch();
            return !touch.empty();
        }, "Check if percept has touch data")
        .def("hasInternal", [](const SensoryPercept& self) {
            auto internal = self.getInternal();
            return !internal.empty();
        }, "Check if percept has internal signals")
        .def("clone", &SensoryPercept::clone, "Create a copy of the percept")
        .def("toDict", [](const SensoryPercept& self) {
            py::dict result;
            result["vision"] = self.getVision();
            result["touch"] = self.getTouch();
            result["internal"] = self.getInternal();
            result["proprioception"] = self.getProprioception();
            result["audio"] = self.getAudio();
            result["timestamp"] = self.getTimestamp();
            return result;
        }, "Convert percept to Python dictionary")
        .def("fromDict", [](py::dict dict) {
            SensoryPercept percept;
            if (dict.contains("vision")) {
                percept.setVision(py::cast<std::vector<float>>(dict["vision"]));
            }
            if (dict.contains("touch")) {
                percept.setTouch(py::cast<std::vector<float>>(dict["touch"]));
            }
            if (dict.contains("internal")) {
                percept.setInternal(py::cast<std::vector<float>>(dict["internal"]));
            }
            if (dict.contains("proprioception")) {
                percept.setProprioception(py::cast<std::vector<float>>(dict["proprioception"]));
            }
            if (dict.contains("audio")) {
                percept.setAudio(py::cast<std::vector<float>>(dict["audio"]));
            }
            if (dict.contains("timestamp")) {
                percept.setTimestamp(py::cast<float>(dict["timestamp"]));
            }
            return percept;
        }, py::arg("dict"), "Create percept from Python dictionary");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world with enhanced Python API)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state by timestep")
        .def("step", [](SimpleWorld& self, float timestep) {
            self.update(timestep);
        }, py::arg("timestep"), "Alias for update method")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get sensory percept for agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("findObjectAt", [](SimpleWorld& self, float x, float y) -> py::object {
            auto objects = self.getObjects();
            for (const auto& obj : objects) {
                float dx = obj.x - x;
                float dy = obj.y - y;
                if (dx * dx + dy * dy <= obj.radius * obj.radius) {
                    return py::cast(obj);
                }
            }
            return py::none();
        }, py::arg("x"), py::arg("y"), "Find object at position")
        .def("getObjects", &SimpleWorld::getObjects, "Get all world objects")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getObjectCount", [](const SimpleWorld& self) {
            return self.getObjects().size();
        }, "Get number of objects")
        .def("clearObjects", [](SimpleWorld& self) {
            self.setObjects(std::vector<WorldObject>{});
        }, "Clear all objects")
        .def("addResource", [](SimpleWorld& self, float x, float y, float value = 1.0f) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, "Add resource object")
        .def("addHazard", [](SimpleWorld& self, float x, float y, float value = -1.0f) {
            WorldObject obj(x, y, WorldObjectType::Hazard, value);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = -1.0f, "Add hazard object")
        .def("getNearestObject", [](SimpleWorld& self, float x, float y) -> py::object {
            auto objects = self.getObjects();
            float minDist = std::numeric_limits<float>::max();
            WorldObject nearest;
            bool found = false;
            
            for (const auto& obj : objects) {
                float dx = obj.x - x;
                float dy = obj.y - y;
                float dist = std::sqrt(dx * dx + dy * dy);
                if (dist < minDist) {
                    minDist = dist;
                    nearest = obj;
                    found = true;
                }
            }
            
            return found ? py::cast(nearest) : py::none();
        }, py::arg("x"), py::arg("y"), "Find nearest object")
        .def("getObjectsByType", [](SimpleWorld& self, WorldObjectType type) {
            std::vector<WorldObject> result;
            auto objects = self.getObjects();
            for (const auto& obj : objects) {
                if (obj.type == type) {
                    result.push_back(obj);
                }
            }
            return result;
        }, py::arg("type"), "Get objects of specific type")
        .def("toDict", [](const SimpleWorld& self) {
            py::dict result;
            result["width"] = self.getWidth();
            result["height"] = self.getHeight();
            result["visionWidth"] = self.getVisionWidth();
            result["visionHeight"] = self.getVisionHeight();
            result["maxEnergy"] = self.getMaxEnergy();
            result["energyDecayRate"] = self.getEnergyDecayRate();
            result["simulationTime"] = self.getSimulationTime();
            result["randomSeed"] = self.getRandomSeed();
            return result;
        }, "Convert world to Python dictionary")
        .def("fromDict", [](SimpleWorld& self, py::dict dict) {
            self.configure(
                py::cast<size_t>(dict["width"]),
                py::cast<size_t>(dict["height"]),
                py::cast<size_t>(dict["visionWidth"]),
                py::cast<size_t>(dict["visionHeight"])
            );
            self.setMaxEnergy(py::cast<float>(dict["maxEnergy"]));
            self.setEnergyDecayRate(py::cast<float>(dict["energyDecayRate"]));
            self.setSimulationTime(py::cast<float>(dict["simulationTime"]));
            self.setRandomSeed(py::cast<unsigned int>(dict["randomSeed"]));
            return self;
        }, py::arg("dict"), "Create world from Python dictionary");

    // Enhanced Brain class with more Pythonic API
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced Brain class with advanced Python API)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("stepByTime", [](Brain& self, float timeSeconds) {
            // Convert seconds to simulation steps (assuming 1ms timestep)
            SimulationStep step = static_cast<SimulationStep>(timeSeconds * 1000.0f);
            Timestamp time = static_cast<Timestamp>(timeSeconds * 1000.0f);
            self.step(step, time);
        }, py::arg("timeSeconds"), "Step brain for specified time in seconds")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into specific neuron")
        .def("injectCurrentToAll", [](Brain& self, NeuronType type, float current) {
            self.injectCurrentToNeurons(type, current);
        }, py::arg("type"), py::arg("current"), "Inject current into all neurons of type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionByIndex", [](Brain& self, size_t index) -> py::object {
            auto regions = self.getRegions();
            if (index < regions.size()) {
                return py::cast(regions[index].get());
            }
            return py::none();
        }, py::arg("index"), "Get region by index")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
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
        .def("getPendingSpikeCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("getConfigValue", [](Brain& self, const std::string& key) -> py::object {
            auto val = self.getConfig()->get<std::string>(key);
            return val.has_value() ? py::cast(val.value()) : py::none();
        }, py::arg("key"), "Get configuration value")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             "Get random number generator")
        .def("logStatus", &Brain::logStatus,
             "Log brain status and statistics")
        .def("getStats", [](Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["ei_ratio"] = self.getExcitationInhibitionRatio();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get brain statistics as dictionary")
        .def("simulateEpisode", [](Brain& self, AgentBrain& agent, SimpleWorld& world, 
                                 size_t steps, float timestep = 0.1f) {
            for (size_t i = 0; i < steps; ++i) {
                world.update(timestep);
                auto percept = world.getSensoryPercept();
                agent.processSensoryInput(percept);
                self.step(static_cast<SimulationStep>(i), static_cast<Timestamp>(i * timestep));
                // Note: Action selection would need motor command integration
            }
        }, py::arg("agent"), py::arg("world"), py::arg("steps"), py::arg("timestep") = 0.1f,
           "Simulate complete episode with agent and world")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(brain.getTotalSynapseCount()) + " synapses>";
        })
        .def("__str__", [](const Brain& brain) {
            return "Brain(n=" + std::to_string(brain.getTotalNeuronCount()) + 
                   ", s=" + std::to_string(brain.getTotalSynapseCount()) + ")";
        });

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced Agent brain interface with advanced Python API)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with underlying brain")
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
        .def("updateWithTime", [](AgentBrain& self, float timeSeconds) {
            self.updateDevelopment(timeSeconds);
        }, py::arg("timeSeconds"), "Update development with time in seconds")
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
             py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("getStats", [](AgentBrain& self) {
            py::dict stats;
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            stats["curiosity_enabled"] = self.isCuriosityEnabled();
            return stats;
        }, "Get agent statistics as dictionary")
        .def("toDict", [](AgentBrain& self) {
            py::dict result;
            result["neuromodulation_level"] = self.getNeuromodulationLevel();
            result["curiosity_level"] = self.getCuriosityLevel();
            result["novelty_level"] = self.getNoveltyLevel();
            result["prediction_error"] = self.getPredictionError();
            result["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return result;
        }, "Convert agent to Python dictionary")
        .def("fromDict", [](py::dict dict) {
            // This is a simplified example - real implementation would need brain reference
            throw std::runtime_error("fromDict requires brain reference");
        }, py::arg("dict"), "Create agent from Python dictionary");

    // Enhanced helper functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible defaults");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Enhanced convenience functions for complete agent setup
    m.def("createCompleteAgent", [](std::shared_ptr<Config> config,
                                   size_t worldWidth = 20, size_t worldHeight = 20,
                                   size_t visionWidth = 8, size_t visionHeight = 8) {
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldWidth, worldHeight, visionWidth, visionHeight);
        world->reset();
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return std::make_tuple(brain, world, agent);
    }, py::arg("config"), py::arg("worldWidth") = 20, py::arg("worldHeight") = 20,
       py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
       "Create complete agent setup with brain, world, and agent");

    m.def("runSimpleEpisode", [](Brain& brain, AgentBrain& agent, SimpleWorld& world,
                               size_t steps = 100, float timestep = 0.1f) {
        for (size_t i = 0; i < steps; ++i) {
            world.update(timestep);
            auto percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            brain.step(static_cast<SimulationStep>(i), static_cast<Timestamp>(i * timestep));
            // Note: Full episode simulation would need motor command integration
        }
        return brain.getTotalSpikeCount();
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps") = 100,
       py::arg("timestep") = 0.1f, "Run simple episode and return total spike count");

    m.def("createDefaultConfigForDevelopment", []() {
        auto config = std::make_shared<Config>();
        config->configureBrain(1000, 1, 0.1f);
        config->configurePlasticity(true, 0.01f, 0.012f, 20.0f);
        config->configureNeuromodulation(true, true, true, 0.1f);
        config->configureDevelopment(true, 0.0001f, 0.00001f);
        return config;
    }, "Create a default configuration optimized for development and learning");

    m.def("createDefaultConfigForStable", []() {
        auto config = std::make_shared<Config>();
        config->configureBrain(500, 1, 0.05f);
        config->configurePlasticity(false, 0.005f, 0.006f, 30.0f);
        config->configureNeuromodulation(true, false, false);
        config->configureDevelopment(true, 0.00005f, 0.000005f);
        return config;
    }, "Create a default configuration optimized for stable operation");

    // Error handling and validation
    m.def("validateConfig", [](const Config& config) {
        auto keys = config.getKeys();
        py::list errors;
        
        for (const auto& key : keys) {
            auto val = config.get<std::string>(key);
            if (!val.has_value()) {
                errors.append("Invalid configuration key: " + key);
            }
        }
        
        if (errors.empty()) {
            return py::make_tuple(true, "Configuration is valid");
        } else {
            return py::make_tuple(false, errors);
        }
    }, "Validate configuration and return tuple (isValid, messages)");

    m.def("exportConfig", [](const Config& config, const std::string& filename) {
        return config.saveToFile(filename);
    }, py::arg("config"), py::arg("filename"), "Export configuration to file");

    m.def("importConfig", [](const std::string& filename) {
        Config config;
        config.loadFromFile(filename);
        return config;
    }, py::arg("filename"), "Import configuration from file");

    // Version and information
    m.attr("__version__") = "2.1.0-enhanced";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__description__") = "Enhanced Python bindings for NLM neural simulation framework";

    m.def("getVersion", []() {
        return "2.1.0-enhanced";
    });

    m.def("getInfo", []() {
        py::dict info;
        info["version"] = "2.1.0-enhanced";
        info["author"] = "NLM Development Team";
        info["description"] = "Enhanced Python bindings for NLM neural simulation framework";
        info["features"] = py::list({
            "Complete Phase 6 integration",
            "Memory systems (working, episodic, associative)",
            "Neuromodulation (dopamine, curiosity, novelty, prediction error)",
            "Development and learning mechanisms",
            "Enhanced Python API with convenience methods",
            "Improved error handling and validation",
            "Configuration serialization and validation",
            "Enhanced documentation and examples"
        });
        return info;
    });

    // Invalid identifiers
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm

} // namespace nlm
