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
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, Vision, Audio, InternalSignals, Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Error types for better Pythonic error handling
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    py::register_exception<std::out_of_range>(m, "IndexError");
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Error types for better Pythonic error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Configuration type
    enum class ConfigType {
        Default,
        Performance,
        Learning,
        Research,
        Experimental
    };

    // Config class with improved documentation and error handling
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
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value by key")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get(key);
        }, py::arg("key"), "Get a configuration value by key")
        .def("setBool", &Config::setBool, py::arg("key"), py::arg("value"),
             "Set a boolean configuration value")
        .def("setFloat", &Config::setFloat, py::arg("key"), py::arg("value"),
             "Set a float configuration value")
        .def("setInt", &Config::setInt, py::arg("key"), py::arg("value"),
             "Set an integer configuration value")
        .def("setDouble", &Config::setDouble, py::arg("key"), py::arg("value"),
             "Set a double configuration value")
        .def("getBool", &Config::getBool, py::arg("key"), "Get a boolean configuration value")
        .def("getFloat", &Config::getFloat, py::arg("key"), "Get a float configuration value")
        .def("getInt", &Config::getInt, py::arg("key"), "Get an integer configuration value")
        .def("getDouble", &Config::getDouble, py::arg("key"), "Get a double configuration value")
        .def("setDefaults", [](Config& self, ConfigType configType) {
            // Apply preset configurations based on type
            switch (configType) {
                case ConfigType::Default:
                    self.setBool("plasticity.stdp.enable", false);
                    self.setBool("plasticity.hebbian.enable", false);
                    self.setBool("plasticity.structural.enable", false);
                    self.setInt("brain.neuron_count", 100);
                    self.setFloat("brain.synapse_density", 0.1);
                    break;
                case ConfigType::Performance:
                    self.setBool("plasticity.stdp.enable", false);
                    self.setBool("plasticity.hebbian.enable", false);
                    self.setBool("plasticity.structural.enable", false);
                    self.setInt("brain.neuron_count", 1000);
                    self.setFloat("brain.synapse_density", 0.2);
                    break;
                case ConfigType::Learning:
                    self.setBool("plasticity.stdp.enable", true);
                    self.setBool("plasticity.hebbian.enable", true);
                    self.setBool("plasticity.structural.enable", true);
                    self.setInt("brain.neuron_count", 500);
                    self.setFloat("brain.synapse_density", 0.15);
                    break;
                case ConfigType::Research:
                    self.setBool("plasticity.stdp.enable", true);
                    self.setBool("plasticity.hebbian.enable", true);
                    self.setBool("plasticity.structural.enable", true);
                    self.setInt("brain.neuron_count", 2000);
                    self.setFloat("brain.synapse_density", 0.25);
                    break;
                case ConfigType::Experimental:
                    self.setBool("plasticity.stdp.enable", true);
                    self.setBool("plasticity.hebbian.enable", true);
                    self.setBool("plasticity.structural.enable", true);
                    self.setBool("neuromod.dopamine.enable", true);
                    self.setBool("neuromod.curiosity.enable", true);
                    self.setBool("neuromod.novelty.enable", true);
                    self.setInt("brain.neuron_count", 3000);
                    self.setFloat("brain.synapse_density", 0.3);
                    break;
            }
        }, py::arg("configType"),
           "Set preset configuration based on configuration type")
        .def("getStats", [](const Config& self) {
            // Get detailed configuration statistics
            py::dict stats;
            stats["entry_count"] = self.getKeys().size();
            
            // Count by source
            int defaultCount = 0, fileCount = 0, commandLineCount = 0, runtimeCount = 0;
            for (const auto& key : self.getKeys()) {
                auto value = self.get(key);
                if (value.has_value()) {
                    // Note: Config doesn't have direct source access, this is a simplified version
                    defaultCount++; // Placeholder
                }
            }
            stats["source_default"] = defaultCount;
            stats["source_file"] = fileCount;
            stats["source_command_line"] = commandLineCount;
            stats["source_runtime"] = runtimeCount;
            
            return stats;
        }, "Get detailed configuration statistics")
        .def("clone", [](const Config& self) {
            // Create a copy of the configuration
            auto newConfig = std::make_shared<Config>();
            for (const auto& key : self.getKeys()) {
                auto value = self.get(key);
                if (value.has_value()) {
                    newConfig->set(key, value.value());
                }
            }
            return newConfig;
        }, "Create a copy of this configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Advanced configuration methods
    py::enum_<ConfigType>(m, "ConfigType", R"pbdoc(Configuration type enumeration)pbdoc")
        .value("Default", ConfigType::Default)
        .value("Performance", ConfigType::Performance)
        .value("Learning", ConfigType::Learning)
        .value("Research", ConfigType::Research)
        .value("Experimental", ConfigType::Experimental)
        .export_values();

    // Advanced configuration methods for Config class
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
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value by key")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get(key);
        }, py::arg("key"), "Get a configuration value by key")
        .def("setBool", &Config::setBool, py::arg("key"), py::arg("value"),
             "Set a boolean configuration value")
        .def("setFloat", &Config::setFloat, py::arg("key"), py::arg("value"),
             "Set a float configuration value")
        .def("setInt", &Config::setInt, py::arg("key"), py::arg("value"),
             "Set an integer configuration value")
        .def("setDouble", &Config::setDouble, py::arg("key"), py::arg("value"),
             "Set a double configuration value")
        .def("getBool", &Config::getBool, py::arg("key"), "Get a boolean configuration value")
        .def("getFloat", &Config::getFloat, py::arg("key"), "Get a float configuration value")
        .def("getInt", &Config::getInt, py::arg("key"), "Get an integer configuration value")
        .def("getDouble", &Config::getDouble, py::arg("key"), "Get a double configuration value")
        .def("setDefaults", [](Config& self, ConfigType configType) {
            // Apply preset configurations based on type
            switch (configType) {
                case ConfigType::Default:
                    self.setBool("plasticity.stdp.enable", false);
                    self.setBool("plasticity.hebbian.enable", false);
                    self.setBool("plasticity.structural.enable", false);
                    self.setInt("brain.neuron_count", 100);
                    self.setFloat("brain.synapse_density", 0.1);
                    break;
                case ConfigType::Performance:
                    self.setBool("plasticity.stdp.enable", false);
                    self.setBool("plasticity.hebbian.enable", false);
                    self.setBool("plasticity.structural.enable", false);
                    self.setInt("brain.neuron_count", 1000);
                    self.setFloat("brain.synapse_density", 0.2);
                    break;
                case ConfigType::Learning:
                    self.setBool("plasticity.stdp.enable", true);
                    self.setBool("plasticity.hebbian.enable", true);
                    self.setBool("plasticity.structural.enable", true);
                    self.setInt("brain.neuron_count", 500);
                    self.setFloat("brain.synapse_density", 0.15);
                    break;
                case ConfigType::Research:
                    self.setBool("plasticity.stdp.enable", true);
                    self.setBool("plasticity.hebbian.enable", true);
                    self.setBool("plasticity.structural.enable", true);
                    self.setInt("brain.neuron_count", 2000);
                    self.setFloat("brain.synapse_density", 0.25);
                    break;
                case ConfigType::Experimental:
                    self.setBool("plasticity.stdp.enable", true);
                    self.setBool("plasticity.hebbian.enable", true);
                    self.setBool("plasticity.structural.enable", true);
                    self.setBool("neuromod.dopamine.enable", true);
                    self.setBool("neuromod.curiosity.enable", true);
                    self.setBool("neuromod.novelty.enable", true);
                    self.setInt("brain.neuron_count", 3000);
                    self.setFloat("brain.synapse_density", 0.3);
                    break;
            }
        }, py::arg("configType"),
           "Set preset configuration based on configuration type")
        .def("getStats", [](const Config& self) {
            // Get detailed configuration statistics
            py::dict stats;
            stats["entry_count"] = self.getKeys().size();
            
            // Count by source
            int defaultCount = 0, fileCount = 0, commandLineCount = 0, runtimeCount = 0;
            for (const auto& key : self.getKeys()) {
                auto value = self.get(key);
                if (value.has_value()) {
                    // Note: Config doesn't have direct source access, this is a simplified version
                    defaultCount++; // Placeholder
                }
            }
            stats["source_default"] = defaultCount;
            stats["source_file"] = fileCount;
            stats["source_command_line"] = commandLineCount;
            stats["source_runtime"] = runtimeCount;
            
            return stats;
        }, "Get detailed configuration statistics")
        .def("clone", [](const Config& self) {
            // Create a copy of the configuration
            auto newConfig = std::make_shared<Config>();
            for (const auto& key : self.getKeys()) {
                auto value = self.get(key);
                if (value.has_value()) {
                    newConfig->set(key, value.value());
                }
            }
            return newConfig;
        }, "Create a copy of this configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for all sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("getTypeName", [](const SensoryInput& self) {
            return self.getType();
        }, "Get the human-readable type name of sensory input")
        .def("toJson", [](const SensoryInput& self) {
            // Convert sensory input to JSON string
            // This would need to be implemented in C++
            throw std::runtime_error("toJson not yet implemented for SensoryInput");
        }, "Convert sensory input to JSON representation");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input - processed visual input from world)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataPtr", [](Vision& self, float* data, size_t size) {
            self.setData(data, size);
        }, py::arg("data"), py::arg("size"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getDimensions", &Vision::getDimensions)
        .def("normalize", &Vision::normalize, "Normalize vision data to [0, 1] range")
        .def("applyThreshold", &Vision::applyThreshold, py::arg("threshold"), "Apply threshold to vision data")
        .def("resize", &Vision::resize, py::arg("width"), py::arg("height"), "Resize vision data")
        .def("convertToGrayscale", &Vision::convertToGrayscale, "Convert vision to grayscale")
        .def("clone", &Vision::clone, "Create a copy of this Vision input");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input - audio data from world)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", &Audio::getDuration, "Get audio duration in seconds")
        .def("getFrequency", &Audio::getFrequency, "Get dominant frequency (placeholder)")
        .def("normalize", &Audio::normalize, "Normalize audio data to [-1, 1] range")
        .def("applyGain", &Audio::applyGain, py::arg("gain"), "Apply gain to audio data")
        .def("filter", [](Audio& self, float lowFreq, float highFreq) {
            // Simple bandpass filter (placeholder)
            self.normalize();
        }, py::arg("lowFreq"), py::arg("highFreq"), "Apply bandpass filter to audio")
        .def("computeSpectrogram", &Audio::computeSpectrogram, "Compute spectrogram (placeholder)")
        .def("clone", &Audio::clone, "Create a copy of this Audio input");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input - neural activity from within the brain)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            const auto& data = self.getData();
            if (index >= data.size()) {
                throw py::index_error("Signal index out of range");
            }
            return data[index];
        }, py::arg("index"), "Get signal value at index")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getHomeostaticLevel", &InternalSignals::getHomeostaticLevel, "Get current homeostatic level")
        .def("getAllSignals", &InternalSignals::getAllSignals, "Get all signal values as list")
        .def("normalize", &InternalSignals::normalize, "Normalize internal signals")
        .def("clone", &InternalSignals::clone, "Create a copy of these internal signals");

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getParameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index >= params.size()) {
                throw py::index_error("Parameter index out of range");
            }
            return params[index];
        }, py::arg("index"), "Get parameter value at index")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index >= params.size()) {
                throw py::index_error("Parameter index out of range");
            }
            params[index] = value;
            self.setParameters(params);
        }, py::arg("index"), py::arg("value"), "Set parameter value at index")
        .def("addParameter", &Action::addParameter, py::arg("value"), "Add a parameter")
        .def("clone", &Action::clone, "Create a copy of this Action")
        .def("isValid", [](const Action& self) {
            // Basic validation - type must be valid, parameters must be reasonable
            ActionType type = self.getType();
            const auto& params = self.getParameters();
            
            // Check if type is Custom and parameters are provided
            if (type == ActionType::Custom && params.empty()) {
                return false;
            }
            
            // Check for reasonable parameter values
            for (float param : params) {
                if (std::isnan(param) || std::isinf(param)) {
                    return false;
                }
            }
            
            return true;
        }, "Check if action is valid");

    // WorldObject class
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
        .def("isResource", &WorldObject::isResource, "Check if this is a resource object")
        .def("isHazard", &WorldObject::isHazard, "Check if this is a hazard object")
        .def("getDistanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Get distance to position");

    // AgentBody class
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
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get agent position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get agent velocity as (vx, vy) tuple")
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
            self.isMoving = (vx != 0.0f || vy != 0.0f);
        }, py::arg("vx"), py::arg("vy"), "Set agent velocity")
        .def("getSpeed", &AgentBody::getSpeed, "Get agent speed (magnitude of velocity)")
        .def("getDirection", &AgentBody::getDirection, "Get agent direction in radians")
        .def("isAlive", [](const AgentBody& self) {
            return self.health > 0.0f;
        }, "Check if agent is alive");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccessful", [](const ActionResult& self) {
            return self.success;
        }, "Check if action was successful")
        .def("getReward", &ActionResult::getReward, "Get reward value")
        .def("getMessage", &ActionResult::getMessage, "Get result message");

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data - combined sensory input from world)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("hasVision", [](const SensoryPercept& self) {
            return static_cast<bool>(self.getVision());
        }, "Check if percept has vision data")
        .def("hasTouch", [](const SensoryPercept& self) {
            return static_cast<bool>(self.getTouch());
        }, "Check if percept has touch data")
        .def("hasInternal", [](const SensoryPercept& self) {
            return static_cast<bool>(self.getInternal());
        }, "Check if percept has internal signals")
        .def("hasProprioception", [](const SensoryPercept& self) {
            return static_cast<bool>(self.getProprioception());
        }, "Check if percept has proprioception")
        .def("hasAudio", [](const SensoryPercept& self) {
            return static_cast<bool>(self.getAudio());;
        }, "Check if percept has audio data")
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
        .def("addSignal", [](SensoryPercept& self, const std::string& type, const std::vector<float>& data) {
            // This is a convenience method to add signals of different types
            if (type == "vision" && self.getVision()) {
                auto vision = self.getVision();
                vision->setData(data);
                self.setVision(vision);
            }
            // Additional signal types can be added here
        }, py::arg("type"), py::arg("data"), "Add signal of a specific type")
        .def("merge", &SensoryPercept::merge, py::arg("other"), "Merge with another percept")
        .def("clone", &SensoryPercept::clone, "Create a copy of this percept")
        .def("toDict", [](const SensoryPercept& self) {
            py::dict result;
            auto vision = self.getVision();
            if (vision) {
                result["vision"] = py::make_tuple(vision->getWidth(), vision->getHeight(), vision->getChannels());
            }
            // Add other modalities as needed
            return result;
        }, "Convert percept to Python dictionary");

    // SimpleWorld class
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
        .def("getAgentPosition", [](const SimpleWorld& self) {
            const auto& body = self.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, "Get agent position as (x, y) tuple")
        .def("getAgentOrientation", [](const SimpleWorld& self) {
            const auto& body = self.getAgentBody();
            return body.orientation;
        }, "Get agent orientation")
        .def("setAgentPosition", [](SimpleWorld& self, float x, float y) {
            auto& body = self.getAgentBody();
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
    .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getAgentPosition", [](const SimpleWorld& self) {
            const auto& body = self.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, "Get agent position as (x, y) tuple")
        .def("getAgentOrientation", [](const SimpleWorld& self) {
            const auto& body = self.getAgentBody();
            return body.orientation;
        }, "Get agent orientation")
        .def("setAgentPosition", [](SimpleWorld& self, float x, float y) {
            auto& body = self.getAgentBody();
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("isAtPosition", [](const SimpleWorld& self, float x, float y, float tolerance = 0.5f) {
            const auto& body = self.getAgentBody();
            float dx = body.x - x;
            float dy = body.y - y;
            return (dx*dx + dy*dy) <= (tolerance*tolerance);
        }, py::arg("x"), py::arg("y"), py::arg("tolerance") = 0.5f, "Check if agent is at position")
        // Visualization methods
        .def("createMap", [](SimpleWorld& self) {
            // Create world map visualization
            std::vector<std::vector<int>> map(self.getHeight(), std::vector<int>(self.getWidth(), 0));
            
            // Mark walls
            for (const auto& obj : self.objects_) {
                if (obj.type == WorldObjectType::Wall) {
                    int x = static_cast<int>(obj.x);
                    int y = static_cast<int>(obj.y);
                    if (x >= 0 && x < self.getWidth() && y >= 0 && y < self.getHeight()) {
                        map[y][x] = 1; // Wall
                    }
                } else if (obj.type == WorldObjectType::Resource) {
                    int x = static_cast<int>(obj.x);
                    int y = static_cast<int>(obj.y);
                    if (x >= 0 && x < self.getWidth() && y >= 0 && y < self.getHeight()) {
                        map[y][x] = 2; // Resource
                    }
                } else if (obj.type == WorldObjectType::Hazard) {
                    int x = static_cast<int>(obj.x);
                    int y = static_cast<int>(obj.y);
                    if (x >= 0 && x < self.getWidth() && y >= 0 && y < self.getHeight()) {
                        map[y][x] = 3; // Hazard
                    }
                }
            }
            
            // Mark agent start position
            int agentX = static_cast<int>(self.getAgentBody().x);
            int agentY = static_cast<int>(self.getAgentBody().y);
            if (agentX >= 0 && agentX < self.getWidth() && agentY >= 0 && agentY < self.getHeight()) {
                map[agentY][agentX] = 4; // Agent
            }
            
            return map;
        }, "Create world map visualization");

    // Brain class with enhanced step() method
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with step number")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with step number and timestamp")
        .def("stepWithClock", [](Brain& self, const SimulationClock& clock) {
            // Use clock's current time and step
            self.step(clock.getStep(), clock.getTime());
        }, py::arg("clock"), "Perform a simulation step using a simulation clock")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("receiveVision", [](Brain& self, const Vision& vision) {
            auto sensoryInput = std::make_shared<Vision>(vision);
            self.receiveSensoryInput(sensoryInput);
        }, py::arg("vision"), "Inject vision input into the brain")
        .def("receiveInternalSignals", [](Brain& self, const InternalSignals& signals) {
            auto sensoryInput = std::make_shared<InternalSignals>(signals);
            self.receiveSensoryInput(sensoryInput);
        }, py::arg("signals"), "Inject internal signals into the brain")
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
        .def("getNeuronCountByType", [](const Brain& self, NeuronType type) {
            // This would need to be implemented in Brain.hpp
            throw std::runtime_error("getNeuronCountByType not yet implemented");
        }, py::arg("type"), "Get neuron count by type")
        .def("getSynapseCountByType", [](const Brain& self, SynapseType type) {
            // This would need to be implemented in Brain.hpp
            throw std::runtime_error("getSynapseCountByType not yet implemented");
        }, py::arg("type"), "Get synapse count by type")
        .def("getPopulationCount", [](const Brain& self, RegionId regionId) {
            // This would need to be implemented in Brain.hpp
            throw std::runtime_error("getPopulationCount not yet implemented");
        }, py::arg("regionId"), "Get population count in region")
        // ========== MEMORY SYSTEMS ==========
        
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory system")
        .def("getSemanticMemory", &Brain::getSemanticMemory,
             "Get semantic memory system")
        .def("getProceduralMemory", &Brain::getProceduralMemory,
             "Get procedural memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty detection system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             "Get prediction error signal")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             "Get random number generator");

    // Working memory system bindings
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory - transient active information)pbdoc")
        .def(py::init<>())
        .def("initialize", &WorkingMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"),
             "Store information in working memory")
        .def("storeToNeuron", &WorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"),
             "Store specific neuron activation")
        .def("retrieve", &WorkingMemory::retrieve,
             "Retrieve current working memory content as activity levels")
        .def("contains", &WorkingMemory::contains, py::arg("neuron"),
             "Check if specific neuron is part of working memory")
        .def("getNeuronActivation", &WorkingMemory::getNeuronActivation, py::arg("neuron"),
             "Get activation level of a specific neuron in working memory")
        .def("update", &WorkingMemory::update, py::arg("dt"),
             "Update working memory (maintenance and decay)")
        .def("clear", &WorkingMemory::clear, "Clear working memory")
        .def("getActiveTraces", &WorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &WorkingMemory::getCapacity,
             "Get capacity")
        .def("setCapacity", &WorkingMemory::setCapacity, py::arg("cap"),
             "Set capacity")
        .def("getDecayRate", &WorkingMemory::getDecayRate,
             "Get decay rate for memory traces")
        .def("setDecayRate", &WorkingMemory::setDecayRate, py::arg("rate"),
             "Set decay rate")
        .def("getMemoryNeurons", &WorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory")
        .def("strengthenMemory", &WorkingMemory::strengthenMemory, py::arg("factor"),
             "Strengthen working memory representation (for rehearsal)")
        .def("runCompetition", &WorkingMemory::runCompetition,
             "Competition between memory traces")
        .def("isWinning", &WorkingMemory::isWinning, py::arg("neuron"),
             "Is this neuron part of the winning population?")
        .def("getMemoryActivity", &WorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    // AgentBrain class with event handlers and callbacks
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("processVision", [](AgentBrain& self, const Vision& vision) {
            SensoryPercept percept;
            percept.setVision(vision);
            self.processSensoryInput(percept);
        }, py::arg("vision"), "Process vision input")
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
        .def("getSensoryInputStats", [](const AgentBrain& self) {
            // Get statistics about expected sensory input
            size_t size = self.getSensoryInputSize();
            return std::make_tuple(size, std::vector<float>(size, 0.0f));
        }, "Get sensory input statistics (size and default values)")
        .def("getMotorOutputStats", [](const AgentBrain& self) {
            // Get statistics about expected motor output
            size_t size = self.getMotorOutputSize();
            return std::make_tuple(size, std::vector<float>(size, 0.0f));
        }, "Get motor output statistics (size and default values)")
        // Event handlers and callbacks
        .def("addRewardCallback", [](AgentBrain& self, std::function<void(float)> callback) {
            // Simple reward callback registration
            // In a real implementation, this would store the callback for future use
            // For now, just call it immediately as an example
            if (callback) {
                callback(0.0f); // Placeholder - would be actual reward in real usage
            }
        }, py::arg("callback"), "Add reward callback function")
        .def("addNoveltyCallback", [](AgentBrain& self, std::function<void(float)> callback) {
            // Novelty callback registration
            if (callback) {
                callback(0.0f); // Placeholder - would be actual novelty in real usage
            }
        }, py::arg("callback"), "Add novelty callback function")
        .def("addDevelopmentCallback", [](AgentBrain& self, std::function<void(double)> callback) {
            // Development callback registration
            if (callback) {
                callback(0.0); // Placeholder - would be actual developmental stage in real usage
            }
        }, py::arg("callback"), "Add development callback function")
        .def("createSensoryMap", [](AgentBrain& self) {
            // Create sensory input map (simplified)
            std::vector<std::vector<float>> sensoryMap(10, std::vector<float>(10, 0.0f));
            return sensoryMap;
        }, "Create sensory input map");
    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory - storage of events and experiences)pbdoc")
        .def(py::init<>())
        .def("initialize", &EpisodicMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("episode"),
             "Store a new experience")
        .def("retrieveSimilar", &EpisodicMemory::retrieveSimilar,
             py::arg("sensoryPattern"), py::arg("maxResults") = 5,
             "Retrieve episodes similar to a query pattern")
        .def("retrieveTemporal", &EpisodicMemory::retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10,
             "Retrieve episodes from a specific time window")
        .def("retrieveByLocation", &EpisodicMemory::retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5,
             "Retrieve episodes by location")
        .def("retrieveAfterAction", &EpisodicMemory::retrieveAfterAction,
             py::arg("action"), py::arg("maxResults") = 5,
             "Retrieve episodes following a specific action")
        .def("replayEpisode", &EpisodicMemory::replayEpisode, py::arg("episode"),
             "Replay an episode - reactivates the neural patterns associated with it")
        .def("updateRelevance", &EpisodicMemory::updateRelevance,
             py::arg("episodeId"), py::arg("relevanceDelta"),
             "Update episode relevance based on recent use")
        .def("getEpisode", &EpisodicMemory::getEpisode, py::arg("index"),
             "Get episode by index")
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount,
             "Get total number of episodes")
        .def("getRecentEpisodes", &EpisodicMemory::getRecentEpisodes, py::arg("count"),
             "Get recent episodes")
        .def("getAverageReward", &EpisodicMemory::getAverageReward,
             "Get average reward from episodes")
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("relevanceThreshold"),
             "Consolidate old episodes")
        .def("clear", &EpisodicMemory::clear, "Clear all episodes")
        .def("enableReplay", &EpisodicMemory::enableReplay, py::arg("enable"),
             "Enable/disable replay")
        .def("isReplayEnabled", &EpisodicMemory::isReplayEnabled,
             "Check if replay is enabled")
        .def("getEpisodesForReplay", &EpisodicMemory::getEpisodesForReplay, py::arg("count"),
             "Get episodes for replay (selection based on relevance and recency)")
        .def("replaySequence", &EpisodicMemory::replaySequence, py::arg("episodeIds"),
             "Replay multiple episodes (for sleep-like consolidation)");

    // Semantic memory bindings
    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(Semantic memory - knowledge facts and concepts)pbdoc")
        .def(py::init<>())
        .def("initialize", &SemanticMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getBrain", &SemanticMemory::getBrain,
             "Get brain reference")
        .def("storeFact", &SemanticMemory::storeFact, py::arg("key"), py::arg("value"),
             "Store fact")
        .def("retrieveFact", &SemanticMemory::retrieveFact, py::arg("key"),
             "Retrieve fact")
        .def("hasFact", &SemanticMemory::hasFact, py::arg("key"),
             "Check if fact exists")
        .def("getAllFacts", &SemanticMemory::getAllFacts,
             "Get all facts")
        .def("clear", &SemanticMemory::clear, "Clear all facts")
        .def("getConcepts", [](const SemanticMemory& self) {
            // Get learned concepts (simplified implementation)
            auto facts = self.getAllFacts();
            std::vector<std::string> concepts;
            for (const auto& fact : facts) {
                concepts.push_back(fact.first);
            }
            return concepts;
        }, "Get learned concepts");

    // NeuralPlanner bindings
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("planAction", &NeuralPlanner::planAction,
             py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence,
             py::arg("actions"), py::arg("startState"),
             "Evaluate a potential action sequence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
             "Get number of steps to look ahead")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set planning depth")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence,
             "Get planning confidence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality,
             py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plans based on actual outcome")
        .def("clearCache", &NeuralPlanner::clearCache,
             "Clear planning cache")
        .def("setActionQuality", &NeuralPlanner::setActionQuality,
             py::arg("action"), py::arg("quality"),
             "Set action quality function (from experience)")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal,
             "Get current goal")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"),
             "Set current goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Has recent planning been successful?")
        .def("getActionPlan", [](const NeuralPlanner& self) {
            // Get action plan (simplified implementation)
            return std::vector<ActionType>(); // Placeholder
        }, "Get action plan");

    // Procedural memory bindings
    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(Procedural memory - learned skills and habits)pbdoc")
        .def(py::init<>())
        .def("initialize", &ProceduralMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getBrain", &ProceduralMemory::getBrain,
             "Get brain reference")
        .def("learnSkill", &ProceduralMemory::learnSkill, py::arg("name"), py::arg("pattern"),
             "Learn skill")
        .def("getSkill", &ProceduralMemory::getSkill, py::arg("name"),
             "Retrieve skill")
        .def("getAllSkills", &ProceduralMemory::getAllSkills,
             "Get all skills")
        .def("updateProficiency", &ProceduralMemory::updateProficiency,
             py::arg("name"), py::arg("delta"),
             "Update proficiency")
        .def("clear", &ProceduralMemory::clear, "Clear all skills");

    // Semantic memory bindings
    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(Semantic memory - knowledge facts and concepts)pbdoc")
        .def(py::init<>())
        .def("initialize", &SemanticMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getBrain", &SemanticMemory::getBrain,
             "Get brain reference")
        .def("storeFact", &SemanticMemory::storeFact, py::arg("key"), py::arg("value"),
             "Store fact")
        .def("retrieveFact", &SemanticMemory::retrieveFact, py::arg("key"),
             "Retrieve fact")
        .def("hasFact", &SemanticMemory::hasFact, py::arg("key"),
             "Check if fact exists")
        .def("getAllFacts", &SemanticMemory::getAllFacts,
             "Get all facts")
        .def("clear", &SemanticMemory::clear, "Clear all facts")
        .def("getConcepts", [](const SemanticMemory& self) {
            // Get learned concepts (simplified implementation)
            auto facts = self.getAllFacts();
            std::vector<std::string> concepts;
            for (const auto& fact : facts) {
                concepts.push_back(fact.first);
            }
            return concepts;
        }, "Get learned concepts");

    // Prediction system bindings
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for sensory prediction and error computation)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionSystem::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getBrain", &PredictionSystem::getBrain,
             "Get brain reference")
        .def("predictNextState", &PredictionSystem::predictNextState,
             py::arg("currentState"), "Make prediction for next timestep")
        .def("updatePredictions", &PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"),
             "Update predictions based on actual observation")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory,
             "Get prediction error history")
        .def("clearHistory", &PredictionSystem::clearHistory,
             "Clear prediction history")
        .def("train", &PredictionSystem::train, py::arg("observation"),
             "Train prediction model")
        .def("getPredictions", [](const PredictionSystem& self) {
            // Get predictions (simplified implementation)
            return std::vector<float>(); // Placeholder
        }, "Get predictions");

    // Factory functions
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

    // Advanced Factory Functions
    m.def("createComplexBrain", [](size_t neuronCount, size_t regionCount) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->setInt("brain.neuron_count", neuronCount);
        config->setInt("brain.region_count", regionCount);
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            return brain;
        } else {
            throw std::runtime_error("Failed to initialize complex brain");
        }
    }, py::arg("neuronCount"), py::arg("regionCount"),
       "Create a complex brain with specific neuron and region counts");

    m.def("createAgentWithAllFeatures", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all advanced features
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        
        return agent;
    }, py::arg("brain"), py::arg("world"),
       "Create an agent brain with all advanced features enabled");

    m.def("createExperimentEnvironment", [](std::shared_ptr<Config> config) -> std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(50, 50, 12, 12);
        world->reset();
        world->setAgentStart(25, 25);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Configure for experiment
        config->setBool("plasticity.stdp.enable", true);
        config->setBool("plasticity.hebbian.enable", true);
        config->setBool("plasticity.structural.enable", true);
        config->setBool("neuromod.dopamine.enable", true);
        config->setBool("neuromod.curiosity.enable", true);
        config->setBool("neuromod.novelty.enable", true);
        
        // Enable all features
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("config"),
       "Set up a complete experiment environment with brain, world, and agent");

    // Convenience factory functions for enhanced API
    m.def("createVision", [](size_t width, size_t height, size_t channels = 3) -> std::shared_ptr<Vision> {
        return std::make_shared<Vision>(width, height, channels);
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
       "Create a Vision sensory input with specified dimensions");

    m.def("createAudio", [](size_t sampleRate, size_t numSamples) -> std::shared_ptr<Audio> {
        return std::make_shared<Audio>(sampleRate, numSamples);
    }, py::arg("sampleRate"), py::arg("numSamples"),
       "Create an Audio sensory input with specified sample rate and duration");

    m.def("createInternalSignals", []() -> std::shared_ptr<InternalSignals> {
        return std::make_shared<InternalSignals>();
    }, "Create internal signals sensory input");

    m.def("createAction", [](ActionType type, std::vector<float> parameters = std::vector<float>()) -> std::shared_ptr<Action> {
        return std::make_shared<Action>(type, parameters);
    }, py::arg("type"), py::arg("parameters") = std::vector<float>(),
       "Create an Action with specified type and optional parameters");

    m.def("createActionFromParams", [](const std::string& actionName, std::vector<float> parameters = std::vector<float>()) -> std::shared_ptr<Action> {
        // Find ActionType from name (placeholder - would need implementation)
        ActionType type = ActionType::Custom;
        return std::make_shared<Action>(type, parameters);
    }, py::arg("actionName"), py::arg("parameters") = std::vector<float>(),
       "Create an Action from its name string");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Utility functions
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                            std::shared_ptr<AgentBrain> agent, size_t steps) {
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            
            // Get sensory input from world
            auto percept = world->getSensoryPercept();
            
            // Process sensory input in brain
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Decode motor command from brain activity
            auto motor_cmd = agent->decodeMotorCommand();
            
            // Apply motor command to world
            world->applyMotorCommand(motor_cmd, world->getSimulationTime());
            
            // Apply reward modulation based on simple heuristic
            float reward = 1.0f / (1.0f + world->getSimulationTime()); // Decreasing reward over time
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1);
            
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": "
                          << "Firing=" << brain->getFiringNeuronCount() << ", "
                          << "Curiosity=" << agent->getCuriosityLevel() << ", "
                          << "Novelty=" << agent->getNoveltyLevel() << ", "
                          << "Dev=" << brain->getDevelopmentalStage().name << std::endl;
            }
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Run complete simulation with agent stepping in world");

    m.def("compareBrains", [](const std::shared_ptr<Brain> brain1, const std::shared_ptr<Brain> brain2) {
        py::dict comparison;
        comparison["neuron_count_match"] = brain1->getTotalNeuronCount() == brain2->getTotalNeuronCount();
        comparison["synapse_count_match"] = brain1->getTotalSynapseCount() == brain2->getTotalSynapseCount();
        comparison["spike_count_ratio"] = (double)brain2->getTotalSpikeCount() / (double)brain1->getTotalSpikeCount();
        comparison["firing_rate_ratio"] = (double)brain2->getAverageFiringRate() / (double)brain1->getAverageFiringRate();
        
        // Check developmental stage
        comparison["dev_stage_match"] = brain1->getDevelopmentalStage() == brain2->getDevelopmentalStage();
        
        // Get difference in key metrics
        comparison["neuron_count_diff"] = (long long)brain2->getTotalNeuronCount() - (long long)brain1->getTotalNeuronCount();
        comparison["spike_count_diff"] = (long long)brain2->getTotalSpikeCount() - (long long)brain1->getTotalSpikeCount();
        
        return comparison;
    }, py::arg("brain1"), py::arg("brain2"),
       "Compare two brain states and return comparison metrics");

    m.def("benchmarkPerformance", [](std::shared_ptr<Config> config, size_t numSteps = 1000) {
        auto start = std::chrono::high_resolution_clock::now();
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all learning features for benchmark
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        
        // Run simulation
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto motor_cmd = agent->decodeMotorCommand();
            world->applyMotorCommand(motor_cmd, world->getSimulationTime());
            agent->applyRewardModulation(0.1f, 0.0f);
            agent->updateDevelopment(0.1);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Return performance metrics
        py::dict metrics;
        metrics["total_steps"] = numSteps;
        metrics["total_time_ms"] = duration.count();
        metrics["steps_per_second"] = (double)numSteps / (duration.count() / 1000.0);
        metrics["final_spike_count"] = brain->getTotalSpikeCount();
        metrics["average_firing_rate"] = brain->getAverageFiringRate();
        metrics["final_developmental_stage"] = brain->getDevelopmentalStage().name;
        metrics["neuromodulation_level"] = agent->getNeuromodulationLevel();
        metrics["curiosity_level"] = agent->getCuriosityLevel();
        metrics["novelty_level"] = agent->getNoveltyLevel();
        
        return metrics;
    }, py::arg("config"), py::arg("numSteps") = 1000,
       "Run performance benchmark and return timing and performance metrics");

    // Version and metadata
    m.attr("__version__") = "0.2.0";
    m.attr("__author__") = "NLM Team";
    m.attr("__description__") = "Enhanced Python bindings for NLM neural learning machine";
}

} // namespace nlm