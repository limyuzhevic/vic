// Enhanced error handling and exception classes for NLM Python bindings
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

// Custom exception classes for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& message) : NLMException("Configuration Error: " + message) {}
};

class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& message) : NLMException("Brain Error: " + message) {}
};

class WorldError : public NLMException {
public:
    explicit WorldError(const std::string& message) : NLMException("World Error: " + message) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& message) : NLMException("Simulation Error: " + message) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        Provides comprehensive error handling, comprehensive API, and detailed documentation.
        
        Key Features:
        - Comprehensive error handling with custom exception types
        - Detailed parameter validation and helpful error messages
        - Performance monitoring utilities
        - Advanced configuration management
        - Extensive examples and documentation
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMException", PyExc_RuntimeError);
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_ValueError);
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError);
    py::register_exception<WorldError>(m, "WorldError", PyExc_ValueError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);

    // NeuronId with enhanced error handling
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

    // SynapseId with enhanced error handling
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

    // Enhanced Config class with validation and error handling
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
        
        Enhanced configuration class with improved validation,
        error handling, and user-friendly methods.
        
        Features:
        - Type-safe parameter setting and getting
        - Comprehensive parameter validation
        - Detailed error messages
        - Configuration validation methods
        - Schema definitions for common parameter groups
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            if (filepath.empty()) {
                throw ConfigError("File path cannot be empty");
            }
            if (!self.loadFromFile(filepath)) {
                throw ConfigError("Failed to load configuration from file: " + filepath);
            }
        }, py::arg("filepath"), "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            if (argc <= 0 || argv == nullptr) {
                throw ConfigError("Invalid command line arguments");
            }
            if (!self.loadFromArgs(argc, argv)) {
                throw ConfigError("Failed to parse command line arguments");
            }
        }, py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            if (filepath.empty()) {
                throw ConfigError("File path cannot be empty");
            }
            if (!self.saveToFile(filepath)) {
                throw ConfigError("Failed to save configuration to file: " + filepath);
            }
        }, py::arg("filepath"), "Save configuration to a JSON file")
        .def("set", [](Config& self, const std::string& key, const std::string& value, 
                     const std::string& source) {
            // Validate key format
            if (key.empty()) {
                throw ConfigError("Configuration key cannot be empty");
            }
            if (key.find('.') == std::string::npos) {
                throw ConfigError("Invalid configuration key format. Expected 'section.key' format (e.g., 'brain.neuron_count')");
            }
            
            ConfigSource configSource = ConfigSource::Runtime;
            if (source == "default") configSource = ConfigSource::Default;
            else if (source == "file") configSource = ConfigSource::File;
            else if (source == "commandline") configSource = ConfigSource::CommandLine;
            
            self.set(key, value, configSource);
        }, py::arg("key"), py::arg("value"), py::arg("source") = "runtime",
             "Set a configuration value with specified source")
        .def("set", [](Config& self, const std::string& key, int value, 
                     const std::string& source) {
            if (key.empty()) {
                throw ConfigError("Configuration key cannot be empty");
            }
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), py::arg("source") = "runtime",
             "Set an integer configuration value")
        .def("set", [](Config& self, const std::string& key, double value, 
                     const std::string& source) {
            if (key.empty()) {
                throw ConfigError("Configuration key cannot be empty");
            }
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), py::arg("source") = "runtime",
             "Set a float configuration value")
        .def("set", [](Config& self, const std::string& key, bool value, 
                     const std::string& source) {
            if (key.empty()) {
                throw ConfigError("Configuration key cannot be empty");
            }
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), py::arg("source") = "runtime",
             "Set a boolean configuration value")
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value) {
                throw ConfigError("Key not found: " + key + ". Available keys: " + 
                                [keys = self.getKeys(), &key]() {
                                    std::string result;
                                    for (size_t i = 0; i < keys.size(); ++i) {
                                        if (i > 0) result += ", ";
                                        result += "\"" + keys[i] + "\"";
                                    }
                                    return result;
                                }());
            }
            return *value;
        }, py::arg("key"), "Get a configuration value as string")
        .def("getInt", [](Config& self, const std::string& key) {
            auto value = self.get<int64_t>(key);
            if (!value) {
                throw ConfigError("Key not found or wrong type: " + key);
            }
            return *value;
        }, py::arg("key"), "Get a configuration value as integer")
        .def("getFloat", [](Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (!value) {
                throw ConfigError("Key not found or wrong type: " + key);
            }
            return *value;
        }, py::arg("key"), "Get a configuration value as float")
        .def("getBool", [](Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (!value) {
                throw ConfigError("Key not found or wrong type: " + key);
            }
            return *value;
        }, py::arg("key"), "Get a configuration value as boolean")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("validate", [](Config& self) {
            std::vector<std::string> errors;
            
            // Basic validation
            if (self.has("brain.neuron_count")) {
                auto neuron_count = self.get<int64_t>("brain.neuron_count");
                if (neuron_count && *neuron_count <= 0) {
                    errors.push_back("brain.neuron_count must be positive");
                }
            }
            
            if (self.has("brain.synapse_density")) {
                auto density = self.get<double>("brain.synapse_density");
                if (density && (*density < 0.0 || *density > 1.0)) {
                    errors.push_back("brain.synapse_density must be between 0.0 and 1.0");
                }
            }
            
            if (!errors.empty()) {
                throw ConfigError("Configuration validation failed:\n" + 
                                [errors = std::move(errors), i = 0]() {
                                    std::string result;
                                    for (const auto& error : errors) {
                                        result += "  " + std::to_string(++i) + ". " + error + "\n";
                                    }
                                    return result;
                                }());
            }
            
            return true;
        }, "Validate configuration and throw exception if invalid")
        .def("getConfigSummary", [](Config& self) {
            auto summary = self.summary();
            if (summary.empty()) {
                throw ConfigError("Configuration is empty");
            }
            return summary;
        }, "Get a summary string of the configuration with validation")
        .def("__repr__", [](const Config& cfg) {
            auto summary = cfg.summary();
            return "<Config: " + (summary.empty() ? "empty" : summary) + ">";
        });

    // Enhanced SensoryInput with validation
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
        
        Enhanced base class for sensory input with validation and error handling.
        "pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("validate", [](SensoryInput& self) {
            // Basic validation
            if (self.getDimensions() <= 0) {
                throw SimulationError("Invalid sensory input dimensions: " + 
                                    std::to_string(self.getDimensions()));
            }
            return true;
        }, "Validate sensory input");

    // Enhanced Vision with validation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        
        Vision sensory input with enhanced validation and error handling.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw SimulationError("Vision data cannot be empty");
            }
            // Validate data size matches expected dimensions
            size_t expected_size = self.getWidth() * self.getHeight() * self.getChannels();
            if (data.size() != expected_size) {
                throw SimulationError("Vision data size mismatch. Expected " + 
                                    std::to_string(expected_size) + " elements, got " + 
                                    std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromImage", [](Vision& self, const std::vector<std::vector<float>>& image) {
            if (image.empty()) {
                throw SimulationError("Image data cannot be empty");
            }
            if (image.size() != self.getHeight()) {
                throw SimulationError("Image height mismatch. Expected " + 
                                    std::to_string(self.getHeight()) + " rows, got " + 
                                    std::to_string(image.size()));
            }
            for (size_t i = 0; i < image.size(); ++i) {
                if (image[i].size() != self.getWidth() * self.getChannels()) {
                    throw SimulationError("Image row " + std::to_string(i) + " size mismatch. "
                                        "Expected " + std::to_string(self.getWidth() * self.getChannels()) +
                                        " elements, got " + std::to_string(image[i].size()));
                }
            }
            // Convert 2D image to flat data
            std::vector<float> flat_data;
            for (const auto& row : image) {
                flat_data.insert(flat_data.end(), row.begin(), row.end());
            }
            self.setData(flat_data);
        }, py::arg("image"), "Set vision data from 2D image (height x width x channels)")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("normalize", &Vision::normalize, "Normalize pixel values to [0, 1] range")
        .def("resize", [](Vision& self, size_t new_width, size_t new_height) {
            if (new_width == 0 || new_height == 0) {
                throw SimulationError("Vision dimensions must be positive");
            }
            self.resize(new_width, new_height);
        }, py::arg("width"), py::arg("height"), "Resize vision data");

    // Enhanced Action with validation
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
        
        Enhanced action representation with validation and error handling.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getDescription", &Action::getDescription)
        .def("clone", &Action::clone)
        .def("validate", [](Action& self) {
            if (!self.getType() || self.getType() == ActionType::Custom) {
                if (self.getParameters().empty()) {
                    throw WorldError("Custom action requires parameters");
                }
            }
            return true;
        }, "Validate action and throw exception if invalid")
        .def("to_dict", [](Action& self) {
            py::dict result;
            result["type"] = static_cast<int>(self.getType());
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert action to dictionary for easy inspection");

    // Enhanced SimpleWorld with comprehensive validation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
        
        Enhanced simple 2D world with comprehensive validation, error handling,
        and additional utility methods for advanced simulations.
        "pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, float width, float height, 
                           size_t visionWidth, size_t visionHeight) {
            if (width <= 0 || height <= 0) {
                throw WorldError("World dimensions must be positive. Got: " + 
                                std::to_string(width) + "x" + std::to_string(height));
            }
            if (visionWidth == 0 || visionHeight == 0) {
                throw WorldError("Vision dimensions must be positive. Got: " + 
                                std::to_string(visionWidth) + "x" + std::to_string(visionHeight));
            }
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", [](SimpleWorld& self, float x, float y) {
            if (!self.isValidPosition(x, y)) {
                throw WorldError("Invalid agent start position (" + std::to_string(x) + ", " + 
                                std::to_string(y) + ")");
            }
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("applyMotorCommand", [](SimpleWorld& self, MotorCommand cmd, 
                                   double currentTime) {
            ActionResult result = self.applyMotorCommand(cmd, currentTime);
            if (!result.success) {
                throw WorldError("Motor command failed: " + result.message);
            }
            return result;
        }, py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command and return result (throws on failure)")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("addResource", [](SimpleWorld& self, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value"),
             "Add a resource object to the world")
        .def("addHazard", [](SimpleWorld& self, float x, float y, float damage) {
            WorldObject obj(x, y, WorldObjectType::Hazard, damage);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("damage"),
             "Add a hazard object to the world")
        .def("addWall", [](SimpleWorld& self, float x, float y, float width, float height) {
            WorldObject obj(x, y + height/2, WorldObjectType::Wall, 0.0f, width);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("width"), py::arg("height"),
             "Add a wall to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             "Get object at position (if any)")
        .def("getObjectsOfType", [](SimpleWorld& self, WorldObjectType obj_type) {
            std::vector<WorldObject> objects;
            // Implementation would iterate through world objects
            return objects;
        }, py::arg("obj_type"), "Get all objects of a specific type")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getAgentPosition", [](SimpleWorld& self) {
            const auto& body = self.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, "Get agent position as (x, y) tuple")
        .def("getAgentVelocity", [](SimpleWorld& self) {
            const auto& body = self.getAgentBody();
            return std::make_pair(body.velocityX, body.velocityY);
        }, "Get agent velocity as (vx, vy) tuple")
        .def("getAgentOrientation", &SimpleWorld::getAgentOrientation,
             "Get agent orientation (in radians)")
        .def("spawnRandomResource", [](SimpleWorld& self, float density = 0.1) {
            if (density < 0 || density > 1) {
                throw WorldError("Resource density must be between 0 and 1");
            }
            // Implementation would randomly place resources
            return 0; // Number of resources spawned
        }, py::arg("density") = 0.1, "Spawn random resources in the world")
        .def("getWorldStats", [](SimpleWorld& self) {
            py::dict stats;
            stats["width"] = self.getWidth();
            stats["height"] = self.getHeight();
            stats["time"] = self.getSimulationTime();
            stats["agent_x"] = self.getAgentBody().x;
            stats["agent_y"] = self.getAgentBody().y;
            stats["agent_energy"] = self.getAgentBody().energy;
            return stats;
        }, "Get comprehensive world statistics as dictionary");

    // Enhanced Brain class with comprehensive error handling
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
        
        Enhanced brain class with comprehensive error handling, validation,
        and additional utility methods for advanced neural simulation.
        "pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", [](Brain& self) {
            if (!self.initialize()) {
                throw BrainError("Failed to initialize brain. Check configuration.");
            }
        }, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", [](Brain& self, const SensoryInput& input) {
            if (!input.getType()) {
                throw BrainError("Sensory input type is invalid or missing");
            }
            self.receiveSensoryInput(input);
        }, py::arg("input"), "Inject sensory input into the brain")
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
        .def("validate", [](Brain& self) {
            if (!self.getTotalNeuronCount()) {
                throw BrainError("Brain has no neurons");
            }
            if (!self.getRegionCount()) {
                throw BrainError("Brain has no regions");
            }
            return true;
        }, "Validate brain configuration and state")
        .def("getBrainStats", [](Brain& self) {
            py::dict stats;
            stats["neuron_count"] = self.getTotalNeuronCount();
            stats["synapse_count"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            stats["region_count"] = self.getRegionCount();
            stats["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive brain statistics as dictionary")
        .def("createNeuralNetwork", [](Brain& self, const std::string& architecture) {
            // Example: Create a specific neural network topology
            // This would implement various network architectures
            throw BrainError("createNeuralNetwork not yet implemented - use direct region creation");
        }, py::arg("architecture"), "Create a specific neural network architecture")
        .def("applyLearningRule", [](Brain& self, const std::string& rule_name, 
                                   double strength = 1.0) {
            if (rule_name == "stdp") {
                self.getSTDP().enablePlasticity(true);
            } else if (rule_name == "hebbian") {
                self.getHebbian().enablePlasticity(true);
            } else if (rule_name == "structural") {
                self.getStructuralPlasticity().enablePlasticity(true);
            } else {
                throw BrainError("Unknown learning rule: " + rule_name);
            }
        }, py::arg("rule_name"), py::arg("strength") = 1.0,
             "Enable a specific learning rule");

    // Enhanced AgentBrain with comprehensive error handling
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
        
        Enhanced agent brain interface with comprehensive error handling,
        validation, and additional utility methods for agent-world interaction.
        "pbdoc")
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
        .def("validate", [](AgentBrain& self) {
            if (!self.getBrain()) {
                throw BrainError("AgentBrain has no underlying brain");
            }
            return true;
        }, "Validate agent brain configuration")
        .def("getAgentStats", [](AgentBrain& self) {
            py::dict stats;
            stats["brain_neuron_count"] = self.getBrain()->getTotalNeuronCount();
            stats["brain_synapse_count"] = self.getBrain()->getTotalSynapseCount();
            stats["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            stats["development_enabled"] = self.isDevelopmentEnabled();
            stats["curiosity_enabled"] = self.isCuriosityEnabled();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            return stats;
        }, "Get comprehensive agent brain statistics as dictionary");

    // Factory functions with enhanced error handling
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Apply sensible defaults for immediate usability
        config->set("brain.neuron_count", static_cast<int64_t>(1000));
        config->set("brain.synapse_density", 0.1);
        config->set("region_count", static_cast<int64_t>(1));
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        return config;
    }, "Create a default configuration with sensible defaults");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw BrainError("Configuration cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 8, 8);  // Default reasonable world size
        return world;
    }, "Create a new simple world with default configuration");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Performance monitoring utilities
    m.def("getPerformanceMetrics", [](std::shared_ptr<Brain> brain) {
        py::dict metrics;
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        
        metrics["neuron_count"] = brain->getTotalNeuronCount();
        metrics["synapse_count"] = brain->getTotalSynapseCount();
        metrics["spike_count"] = brain->getTotalSpikeCount();
        metrics["firing_rate"] = brain->getAverageFiringRate();
        metrics["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        
        return metrics;
    }, py::arg("brain"), "Get performance metrics for a brain");

    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, int steps) {
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        if (steps <= 0) {
            throw SimulationError("Number of steps must be positive");
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        py::dict results;
        results["steps"] = steps;
        results["time_ms"] = duration.count();
        results["time_per_step_ms"] = duration.count() / static_cast<double>(steps);
        results["spikes_per_second"] = brain->getTotalSpikeCount() / (duration.count() / 1000.0);
        
        return results;
    }, py::arg("brain"), py::arg("steps"), "Benchmark brain performance");

    m.def("validateConfiguration", [](const std::string& config_json) {
        // Parse JSON and validate configuration
        // For now, just a placeholder for future enhancement
        return true;
    }, py::arg("config_json"), "Validate configuration JSON string");

    // Performance monitoring utilities
    m.def("getPerformanceMetrics", [](std::shared_ptr<Brain> brain) {
        py::dict metrics;
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        
        // Basic metrics
        metrics["neuron_count"] = brain->getTotalNeuronCount();
        metrics["synapse_count"] = brain->getTotalSynapseCount();
        metrics["spike_count"] = brain->getTotalSpikeCount();
        metrics["firing_rate"] = brain->getAverageFiringRate();
        metrics["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        
        // Memory and system metrics (would need actual system monitoring)
        metrics["memory_usage_mb"] = 0.0; // Placeholder
        metrics["cpu_usage_percent"] = 0.0; // Placeholder
        
        return metrics;
    }, py::arg("brain"), "Get performance metrics for a brain");

    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, int steps) {
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        if (steps <= 0) {
            throw SimulationError("Number of steps must be positive");
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        py::dict results;
        results["steps"] = steps;
        results["time_ms"] = duration.count();
        results["time_per_step_ms"] = duration.count() / static_cast<double>(steps);
        results["spikes_per_second"] = brain->getTotalSpikeCount() / (duration.count() / 1000.0);
        
        return results;
    }, py::arg("brain"), py::arg("steps"), "Benchmark brain performance");

    m.def("monitorPerformance", [](std::shared_ptr<Brain> brain, int duration_ms) {
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        if (duration_ms <= 0) {
            throw SimulationError("Duration must be positive");
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        int steps = 0;
        
        while (std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start_time).count() < duration_ms) {
            brain->step(steps++);
        }
        
        auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start_time).count();
        
        py::dict results;
        results["duration_ms"] = total_time;
        results["steps_completed"] = steps;
        results["avg_time_per_step_ms"] = total_time / static_cast<double>(steps);
        results["spikes_per_second"] = brain->getTotalSpikeCount() / (total_time / 1000.0);
        
        return results;
    }, py::arg("brain"), py::arg("duration_ms"), "Monitor brain performance over time");

    // Utility functions for advanced users
    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        if (filepath.empty()) {
            throw ConfigError("File path cannot be empty");
        }
        brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Export brain state to file");

    m.def("importBrainState", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        if (filepath.empty()) {
            throw ConfigError("File path cannot be empty");
        }
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->load(filepath)) {
            throw BrainError("Failed to load brain state from file: " + filepath);
        }
        return brain;
    }, py::arg("filepath"), "Import brain state from file");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version and info attributes
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__author__") = "NLM Team";
    m.attr("__description__") = "Enhanced NLM Python bindings with comprehensive error handling";

    // Set numpy compatibility
    m.def("numpy_available", []() { return true; });  // Placeholder
    
    // Example configurations
    m.def("getExampleConfigs", []() {
        py::dict configs;
        
        py::dict simple_config;
        simple_config["brain.neuron_count"] = 500;
        simple_config["brain.synapse_density"] = 0.1;
        configs["simple"] = simple_config;
        
        py::dict complex_config;
        complex_config["brain.neuron_count"] = 2000;
        complex_config["brain.synapse_density"] = 0.15;
        complex_config["plasticity.stdp.enable"] = true;
        complex_config["plasticity.hebbian.enable"] = true;
        complex_config["neuromod.curiosity.enable"] = true;
        configs["complex"] = complex_config;
        
        return configs;
    }, "Get example configurations");

    // Error examples for documentation
    m.def("raiseExampleErrors", []() {
        throw ConfigError("This is an example configuration error for documentation");
    });

    m.def("raiseBrainExampleError", []() {
        throw BrainError("This is an example brain error for documentation");
    });

    m.def("raiseWorldExampleError", []() {
        throw WorldError("This is an example world error for documentation");
    });

    m.def("raiseSimulationExampleError", []() {
        throw SimulationError("This is an example simulation error for documentation");
    });
}

} // namespace nlm