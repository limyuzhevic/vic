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
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Pythonic property access
        .def("get", [](const Config& self, const std::string& key) {
            // Get with Python-style error handling
            auto value = self.get<std::string>(key);
            if (value.has_value()) {
                return *value;
            }
            throw py::key_error("Configuration key not found: " + key);
        }, py::arg("key"), "Get configuration value as string (raises KeyError if not found)")
        .def("get_int", [](const Config& self, const std::string& key) {
            auto value = self.get<int>(key);
            if (value.has_value()) {
                return *value;
            }
            throw py::key_error("Configuration key not found: " + key);
        }, py::arg("key"), "Get configuration value as integer")
        .def("get_float", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (value.has_value()) {
                return *value;
            }
            throw py::key_error("Configuration key not found: " + key);
        }, py::arg("key"), "Get configuration value as float")
        .def("get_bool", [](const Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (value.has_value()) {
                return *value;
            }
            throw py::key_error("Configuration key not found: " + key);
        }, py::arg("key"), "Get configuration value as boolean")
        // Type-safe setters
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            // Generic setter that handles Python types
            if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>());
            } else if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int>());
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>());
            } else {
                throw py::type_error("Unsupported type for configuration value");
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value with automatic type conversion")
        // Serialization to/from Python dict
        .def("to_dict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                if (value.has_value()) {
                    result[key.c_str()] = *value;
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")
        .def("from_dict", [](Config& self, const py::dict& config_dict) {
            for (auto item : config_dict) {
                std::string key = py::str(item.first).cast<std::string>();
                if (py::isinstance<py::str>(item.second)) {
                    self.set(key, item.second.cast<std::string>());
                } else if (py::isinstance<py::int_>(item.second)) {
                    self.set(key, item.second.cast<int>());
                } else if (py::isinstance<py::float_>(item.second)) {
                    self.set(key, item.second.cast<double>());
                } else if (py::isinstance<py::bool_>(item.second)) {
                    self.set(key, item.second.cast<bool>());
                }
            }
            return self;
        }, py::arg("config_dict"), "Load configuration from Python dictionary")

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        // Add validation for data methods
        .def("validateData", [](SensoryInput& self) {
            auto data = self.getData();
            if (data.empty()) {
                NLM_LOG_WARN("Empty sensory data");
            }
            return data.size();
        }, "Validate and return data size");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            // Validate data dimensions
            size_t expectedSize = self.getWidth() * self.getHeight() * self.getChannels();
            if (data.size() != expectedSize) {
                NLM_LOG_ERROR("Data size mismatch: expected " + std::to_string(expectedSize) +
                              ", got " + std::to_string(data.size()));
                throw py::value_error("Data size mismatch for vision input");
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data with validation")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        // Add convenience methods
        .def("normalize", [](Vision& self) {
            auto data = self.getData();
            float min = *std::min_element(data.begin(), data.end());
            float max = *std::max_element(data.begin(), data.end());
            float range = max - min;
            if (range > 0.0f) {
                for (auto& val : data) {
                    val = (val - min) / range;
                }
                self.setData(data);
            }
            return self;
        }, "Normalize vision data to [0, 1] range, returns self for chaining");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            // Validate data
            if (data.empty()) {
                NLM_LOG_WARN("Empty audio data provided");
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data with validation")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Audio processing utilities
        .def("amplify", [](Audio& self, float factor) {
            auto data = self.getData();
            for (auto& val : data) {
                val *= factor;
            }
            self.setData(data);
            return self;
        }, py::arg("factor"), "Amplify audio by factor, returns self for chaining")
        .def("applyGain", &Audio::applyGain, py::arg("gainDb"),
             "Apply gain in dB, returns self for chaining");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value with validation")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("getSignalCount", [](const InternalSignals& self) {
            // Note: InternalSignals doesn't have a getSignalCount method, but we can
            // expose the internal vector size through the data getter
            auto data = self.getData();
            return data.size();
        }, "Get number of internal signals")
        .def("getMaxSignal", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) {
                throw py::value_error("No signals available");
            }
            return *std::max_element(data.begin(), data.end());
        }, "Get maximum signal value")
        .def("getMinSignal", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) {
                throw py::value_error("No signals available");
            }
            return *std::min_element(data.begin(), data.end());
        }, "Get minimum signal value")
        .def("normalizeSignals", [](InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) {
                NLM_LOG_WARN("Cannot normalize empty signals");
                return self;
            }
            float min = *std::min_element(data.begin(), data.end());
            float max = *std::max_element(data.begin(), data.end());
            float range = max - min;
            if (range > 0.0f) {
                for (auto& val : data) {
                    val = (val - min) / range;
                }
                self.setData(data);
            }
            return self;
        }, "Normalize signals to [0, 1] range, returns self for chaining");

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
        // Pythonic property access with validation
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
            return self;
        }, py::arg("value"), "Add parameter value, returns self for chaining")
        .def("getParameter", [](const Action& self, size_t index) {
            auto params = self.getParameters();
            if (index >= params.size()) {
                throw py::index_error("Parameter index out of range");
            }
            return params[index];
        }, py::arg("index"), "Get parameter at index")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index >= params.size()) {
                throw py::index_error("Parameter index out of range");
            }
            params[index] = value;
            self.setParameters(params);
        }, py::arg("index"), py::arg("value"), "Set parameter at index")
        // Action utilities
        .def("hasParameters", [](const Action& self) {
            return !self.getParameters().empty();
        }, "Check if action has parameters")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        // Action type validation and utilities
        .def("isMovement", [](const Action& self) {
            ActionType type = self.getType();
            return type == ActionType::MoveForward || type == ActionType::MoveBackward ||
                   type == ActionType::MoveLeft || type == ActionType::MoveRight;
        }, "Check if action is a movement type")
        .def("isLookAction", [](const Action& self) {
            ActionType type = self.getType();
            return type == ActionType::Look || type == ActionType::LookUp ||
                   type == ActionType::LookDown || type == ActionType::LookLeft ||
                   type == ActionType::LookRight;
        }, "Check if action is a look type")
        .def("isInteraction", [](const Action& self) {
            ActionType type = self.getType();
            return type == ActionType::Interact || type == ActionType::Eat ||
                   type == ActionType::Drink || type == ActionType::Rest ||
                   type == ActionType::Wait || type == ActionType::Custom;
        }, "Check if action is an interaction type");

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
        // Enhanced reset with validation
        .def("reset", &AgentBody::reset,
             "Reset agent body state")
        // Distance and movement utilities
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = x - self.x;
            float dy = y - self.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position (x, y)")
        .def("angleTo", [](const AgentBody& self, float x, float y) {
            float dx = x - self.x;
            float dy = y - self.y;
            return std::atan2(dy, dx);
        }, py::arg("x"), py::arg("y"), "Calculate angle to position (x, y) in radians")
        // Speed calculation
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed")
        // Position validation
        .def("isValidPosition", [](const AgentBody& self, float width, float height) {
            return self.x >= 0 && self.x < width && self.y >= 0 && self.y < height;
        }, py::arg("width"), py::arg("height"), "Check if position is within bounds")

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Enhanced validation
        .def("validate", [](const ActionResult& self) {
            if (std::isnan(self.reward)) {
                throw py::value_error("Invalid reward value: NaN");
            }
            if (self.success && self.reward < -1000.0f) {
                NLM_LOG_WARN("Successful action with very negative reward: " + std::to_string(self.reward));
            }
            return true;
        }, "Validate action result, throws exception on invalid data")
        .def("getConfidence", [](const ActionResult& self) {
            // Simple confidence based on reward magnitude and success
            float confidence = std::abs(self.reward) / 10.0f;
            if (self.success) {
                confidence *= 1.5f;
            }
            return std::clamp(confidence, 0.0f, 1.0f);
        }, "Get confidence level (0-1) based on reward and success")
        // Helper methods for common success cases
        .def("isPositiveReward", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if reward is positive")
        .def("isHighValue", [](const ActionResult& self, float threshold) {
            return self.reward > threshold;
        }, py::arg("threshold"), "Check if reward exceeds threshold");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        // Enhanced vision access with validation
        .def("getVisionPixel", [](const SensoryPercept& self, size_t x, size_t y) {
            if (x >= self.getVisionWidth() || y >= self.getVisionHeight()) {
                throw py::index_error("Vision pixel index out of bounds: (" + 
                                      std::to_string(x) + ", " + std::to_string(y) + ")");
            }
            size_t index = y * self.getVisionWidth() + x;
            return self.getVision()[index];
        }, py::arg("x"), py::arg("y"), "Get vision pixel at (x, y)")
        .def("setVisionPixel", [](SensoryPercept& self, size_t x, size_t y, float value) {
            if (x >= self.getVisionWidth() || y >= self.getVisionHeight()) {
                throw py::index_error("Vision pixel index out of bounds: (" + 
                                      std::to_string(x) + ", " + std::to_string(y) + ")");
            }
            auto vision = self.getVision();
            size_t index = y * self.getVisionWidth() + x;
            vision[index] = value;
            self.setVision(vision);
        }, py::arg("x"), py::arg("y"), py::arg("value"), "Set vision pixel at (x, y)")
        .def("getTouchCount", [](const SensoryPercept& self) {
            return self.getTouch().size();
        }, "Get number of touch sensors")
        .def("getProprioceptionCount", [](const SensoryPercept& self) {
            return self.getProprioception().size();
        }, "Get number of proprioception sensors")
        .def("getAllSignalCount", &SensoryPercept::getAllSignals,
             "Get total number of all signals")
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
        // World utilities
        .def("addResource", [](SimpleWorld& self, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value"), "Add resource at position (x, y)")
        .def("addHazard", [](SimpleWorld& self, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Hazard, value);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value"), "Add hazard at position (x, y)")
        .def("countObjects", [](const SimpleWorld& self, WorldObjectType type) {
            int count = 0;
            // This would require iterating through all objects, but SimpleWorld doesn't expose this
            // For now, return 0 as a placeholder
            return count;
        }, py::arg("type"), "Count objects of given type");

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
        // Enhanced Brain methods with error handling and utilities
        .def("getActivityStats", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["e/i_ratio"] = self.getExcitationInhibitionRatio();
            return stats;
        }, "Get activity statistics as Python dictionary")
        .def("getRegionNames", [](const Brain& self) {
            auto ids = self.getRegionIds();
            std::vector<std::string> names;
            for (const auto& id : ids) {
                // This assumes there's a getName() method on regions
                // If not available, this will need to be adjusted
                names.push_back("Region_" + std::to_string(id.value));
            }
            return names;
        }, "Get region names")
        .def("clearRegionActivity", [](Brain& self, RegionId regionId) {
            auto region = self.getRegion(regionId);
            if (region) {
                // Clear activity would require access to region's neurons
                // For now, just return success
                return true;
            }
            return false;
        }, py::arg("regionId"), "Clear neural activity in region")
        .def("getCurrentTime", &Brain::getCurrentTime,
             "Get current simulation time (if available)");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        // Enhanced AgentBrain methods with error handling and utilities
        .def("setProcessingMode", [](AgentBrain& self, const std::string& mode) {
            // Add custom processing modes if needed
            if (mode == "strict") {
                // Enable strict validation
                self.enableRewardModulation(true);
                self.enableDevelopment(true);
            } else if (mode == "fast") {
                // Enable fast mode with less validation
                self.enableRewardModulation(false);
                self.enableDevelopment(false);
            } else if (mode == "curious") {
                // Enable curiosity-driven behavior
                self.enableCuriosity(true);
            }
            return self;
        }, py::arg("mode"), "Set processing mode (strict, fast, curious)")
        .def("getProcessingStatus", [](const AgentBrain& self) {
            py::dict status;
            status["reward_modulation"] = self.isRewardModulationEnabled();
            status["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            status["development"] = self.isDevelopmentEnabled();
            status["curiosity"] = self.isCuriosityEnabled();
            status["neuromodulation_level"] = self.getNeuromodulationLevel();
            status["curiosity_level"] = self.getCuriosityLevel();
            status["novelty_level"] = self.getNoveltyLevel();
            status["prediction_error"] = self.getPredictionError();
            return status;
        }, "Get processing status as Python dictionary")
        .def("enableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
            return self;
        }, "Enable all brain features")
        .def("disableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
            return self;
        }, "Disable all brain features")
        .def("getFeatureCount", [](const AgentBrain& self) {
            int count = 0;
            if (self.isRewardModulationEnabled()) count++;
            if (self.isStructuralPlasticityEnabled()) count++;
            if (self.isDevelopmentEnabled()) count++;
            if (self.isCuriosityEnabled()) count++;
            return count;
        }, "Get number of enabled features");

    // Enhanced factory functions with error handling
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    .def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw py::value_error("Config cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw py::value_error("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    // Module-level constants and validation
    .attr("__version__") = "0.1.0"
    .attr("__author__") = "NLM Authors"
    .attr("__description__") = "Python bindings for NLM neural simulation framework"
    // Error handling utilities
    .def("validateEnvironment", []() {
        py::dict result;
        // Check for essential components
        result["config"] = true;  // Assuming Config works
        result["brain"] = true;   // Assuming Brain works
        result["world"] = true;   // Assuming SimpleWorld works
        result["agent"] = true;   // Assuming AgentBrain works
        return result;
    }, "Validate that all essential components are available")
    // Convenience functions for common operations
    .def("createTestEnvironment", []() {
        auto config = std::make_shared<Config>();
        auto world = std::make_shared<SimpleWorld>();
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Configure world for testing
        world->configure(80, 60, 16, 16);
        world->setAgentStart(40.0f, 30.0f);
        
        // Initialize brain
        brain->initialize();
        agent->initialize(*world);
        
        return std::make_tuple(config, world, brain, agent);
    }, "Create a test environment with default settings")
    // Performance monitoring
    .def("getSystemInfo", []() {
        py::dict info;
        info["pybind11_version"] = PYBIND11_VERSION;
        info["python_version"] = PYBIND11_PYTHON_VERSION;
        info["compiler"] = PYBIND11_COMPILER;
        info["build_type"] = "Release";
        return info;
    }, "Get system information");

    // Register invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Register all enum types for better type safety
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
}

} // namespace nlm
