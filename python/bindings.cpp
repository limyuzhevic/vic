#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <chrono>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/memory/Memory.hpp"

namespace py = pybind11;
namespace nlm {

// Custom Python exceptions for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

// Context manager for automatic brain cleanup
class BrainContextManager {
public:
    BrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void __enter__() { return; }
    void __exit__(int /*exc_type*/, char* /*exc_value*/, char* /*traceback*/) {
        if (brain_) {
            brain_->reset();
        }
    }
    
    std::shared_ptr<Brain> getBrain() { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Batch operations helper class
class BrainBatchManager {
public:
    BrainBatchManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Inject current to multiple neurons efficiently
    void injectCurrentBatch(const std::vector<std::pair<NeuronId, float>>& injections) {
        for (const auto& injection : injections) {
            brain_->injectCurrent(injection.first, injection.second);
        }
    }
    
    // Run multiple simulation steps efficiently
    void runSteps(size_t steps, double timestep = 0.001) {
        for (size_t i = 0; i < steps; ++i) {
            brain_->step(i, i * timestep);
        }
    }
    
    // Get statistics summary
    std::unordered_map<std::string, double> getStats() {
        std::unordered_map<std::string, double> stats;
        stats["total_neurons"] = brain_->getTotalNeuronCount();
        stats["total_synapses"] = brain_->getTotalSynapseCount();
        stats["total_spikes"] = brain_->getTotalSpikeCount();
        stats["active_neurons"] = brain_->getActiveNeuronCount();
        stats["firing_neurons"] = brain_->getFiringNeuronCount();
        stats["avg_firing_rate"] = brain_->getAverageFiringRate();
        stats["e_i_ratio"] = brain_->getExcitationInhibitionRatio();
        return stats;
    }
    
    // Get brain reference
    std::shared_ptr<Brain> getBrain() { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module provides an enhanced Python API with:
        - Convenience methods for common operations
        - Context managers for resource management
        - Batch operations for improved performance
        - Advanced simulation helpers
        - Enhanced error handling with custom exceptions
        - Configuration management tools
        - Performance monitoring utilities
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<SimulationError>(m, "SimulationError");
    
    // Enable Python output capture for better debugging
    py::add_ostream_redirect(m, "ostream_redirect");

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

    // Add convenience methods and batch operations to Config
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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
        // Enhanced convenience methods
        .def("getOr", [](Config& self, const std::string& key, double default_value) {
            return self.getOr<double>(key, default_value);
        }, py::arg("key"), py::arg("default_value"), "Get value with default")
        .def("getOr", [](Config& self, const std::string& key, int64_t default_value) {
            return self.getOr<int64_t>(key, default_value);
        }, py::arg("key"), py::arg("default_value"), "Get value with default")
        .def("getOr", [](Config& self, const std::string& key, float default_value) {
            return self.getOr<float>(key, default_value);
        }, py::arg("key"), py::arg("default_value"), "Get value with default")
        .def("getOr", [](Config& self, const std::string& key, const std::string& default_value) {
            return self.getOr<std::string>(key, default_value);
        }, py::arg("key"), py::arg("default_value"), "Get value with default")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), py::arg("source") = 0,
             "Set configuration value")
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.get<int64_t>(key);
            return value;
        }, py::arg("key"), "Get int64_t value")
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            return value;
        }, py::arg("key"), "Get double value")
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.get<float>(key);
            return value;
        }, py::arg("key"), "Get float value")
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            return value;
        }, py::arg("key"), "Get string value")
        .def("validate", [](Config& self) {
            // Basic validation - check for required keys
            std::vector<std::string> required_keys = {
                "random_seed", "simulation_timestep", "neuron_count",
                "region_count", "connection_probability"
            };
            
            std::vector<std::string> missing_keys;
            for (const auto& key : required_keys) {
                if (!self.has(key)) {
                    missing_keys.push_back(key);
                }
            }
            
            if (!missing_keys.empty()) {
                throw ConfigurationError("Missing required configuration keys: " + 
                    [](const std::vector<std::string>& keys) {
                        std::string result;
                        for (size_t i = 0; i < keys.size(); ++i) {
                            if (i > 0) result += ", ";
                            result += keys[i];
                        }
                        return result;
                    }(missing_keys));
            }
            
            return true;
        }, "Validate configuration")
        .def("copy", []() {
            auto config = std::make_shared<Config>();
            return config;
        }, "Create a copy of default config");

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        // Convenience methods
        .def("createFromImage", [](const std::vector<std::vector<std::vector<float>>>& image_data) {
            auto vision = std::make_shared<Vision>();
            // Flatten image data for compatibility
            std::vector<float> flat_data;
            for (const auto& row : image_data) {
                for (const auto& pixel : row) {
                    flat_data.insert(flat_data.end(), pixel.begin(), pixel.end());
                }
            }
            vision->setData(flat_data);
            return vision;
        }, py::arg("image_data"), "Create Vision from 2D/3D image data")
        .def("normalize", &Vision::normalize, "Normalize pixel values to [0,1]");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Enhanced audio methods
        .def("calculateEnergy", &Audio::calculateEnergy, "Calculate audio signal energy")
        .def("applyEffects", [](Audio& self, float gain, float cutoff_freq) {
            // Placeholder for audio effects - gain control, low-pass filter
            self.setData([self, gain, cutoff_freq](const std::vector<float>& data) {
                std::vector<float> result = data;
                for (auto& sample : result) {
                    sample = sample * gain;
                    // Simple low-pass filter implementation
                    // This is a simplified version - in production would use proper DSP
                }
                return result;
            }("data"));
        }, py::arg("gain"), py::arg("cutoff_freq") = 1000.0f, "Apply audio effects")
        .def("convertToMono", &Audio::convertToMono, "Convert stereo to mono");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        // Enhanced internal signal processing
        .def("computeStatistics", &InternalSignals::computeStatistics, "Compute signal statistics")
        .def("normalize", &InternalSignals::normalize, "Normalize signal values")
        .def("filter", [](InternalSignals& self, float alpha) {
            // Simple exponential moving average filter
            self.filter([alpha](const std::vector<float>& data) {
                std::vector<float> result = data;
                float smoothed = 0.0f;
                for (size_t i = 0; i < data.size(); ++i) {
                    smoothed = alpha * data[i] + (1 - alpha) * smoothed;
                    result[i] = smoothed;
                }
                return result;
            }("data"));
        }, py::arg("alpha") = 0.1f, "Apply exponential moving average filter")
        .def("detectPatterns", &InternalSignals::detectPatterns, "Detect patterns in signal data");

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
        // Enhanced action methods
        .def("updateParameters", [](Action& self, const std::string& param_name, float value) {
            auto params = self.getParameters();
            for (auto& param : params) {
                // Simple parameter matching - in practice would use parameter names
                if (param > -1e6f && param < 1e6f) {  // Check if looks like a value
                    param = value;
                    break;
                }
            }
            self.setParameters(params);
        }, py::arg("param_name"), py::arg("value"), "Update action parameter")
        .def("execute", [](Action& self, std::shared_ptr<SimpleWorld> world) {
            ActionResult result;
            // Simple action execution - in practice would be more complex
            if (self.getType() == ActionType::MoveForward) {
                // Move agent forward
                auto body = world->getAgentBody();
                body->x += 1.0f;
                body->isMoving = true;
                result.reward = 1.0f;
                result.success = true;
            } else if (self.getType() == ActionType::Wait) {
                // Do nothing
                result.reward = 0.0f;
                result.success = true;
            }
            return result;
        }, py::arg("world"), "Execute action in world")
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = self.getType();
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert action to dictionary")
        .def("from_dict", [](py::dict dict) {
            Action action;
            auto type_it = dict.find("type");
            if (type_it != dict.end()) {
                action.setType(py::cast<ActionType>(type_it->second));
            }
            auto params_it = dict.find("parameters");
            if (params_it != dict.end()) {
                action.setParameters(py::cast<std::vector<float>>(params_it->second));
            }
            return action;
        }, py::arg("dict"), "Create action from dictionary");

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
        // Enhanced world object methods
        .def("contains", [](const WorldObject& self, float x, float y) {
            float dx = x - self.x;
            float dy = y - self.y;
            return (dx * dx + dy * dy) <= (self.radius * self.radius);
        }, py::arg("x"), py::arg("y"), "Check if point is inside object")
        .def("distance_to", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Calculate distance to another object")
        .def("getArea", [](const WorldObject& self) {
            return 3.141592653589793f * self.radius * self.radius;
        }, "Get object area")
        .def("isCollidingWith", [](const WorldObject& self, const WorldObject& other) {
            return (self.x - other.x)*(self.x - other.x) + 
                   (self.y - other.y)*(self.y - other.y) <= 
                   (self.radius + other.radius)*(self.radius + other.radius);
        }, py::arg("other"), "Check collision with another object")
        .def("to_dict", [](const WorldObject& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["radius"] = self.radius;
            result["type"] = self.type;
            result["value"] = self.value;
            result["active"] = self.active;
            return result;
        }, "Convert object to dictionary")
        .def("from_dict", [](py::dict dict) {
            WorldObject obj;
            if (dict.contains("x")) obj.x = py::cast<float>(dict["x"]);
            if (dict.contains("y")) obj.y = py::cast<float>(dict["y"]);
            if (dict.contains("radius")) obj.radius = py::cast<float>(dict["radius"]);
            if (dict.contains("type")) obj.type = py::cast<WorldObjectType>(dict["type"]);
            if (dict.contains("value")) obj.value = py::cast<float>(dict["value"]);
            if (dict.contains("active")) obj.active = py::cast<bool>(dict["active"]);
            return obj;
        }, py::arg("dict"), "Create object from dictionary");

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
        // Enhanced agent body methods
        .def("move", [](AgentBody& self, float dx, float dy, float dt) {
            self.x += dx;
            self.y += dy;
            self.velocityX = dx / dt;
            self.velocityY = dy / dt;
            self.isMoving = (std::abs(dx) > 0.0f || std::abs(dy) > 0.0f);
        }, py::arg("dx"), py::arg("dy"), py::arg("dt") = 0.001f, "Move agent")
        .def("rotate", &AgentBody::rotate, py::arg("angle"), "Rotate agent")
        .def("updateEnergy", [](AgentBody& self, float energy_change) {
            self.energy += energy_change;
            if (self.energy < 0) self.energy = 0;
            if (self.energy > 100.0f) self.energy = 100.0f;
        }, py::arg("energy_change"), "Update energy level")
        .def("applyDamage", &AgentBody::applyDamage, py::arg("damage"), "Apply damage")
        .def("heal", &AgentBody::heal, py::arg("healing"), "Heal damage")
        .def("getPosition", [](const AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get agent position as tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("distanceTo", [](const AgentBody& self, const AgentBody& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Calculate distance to another agent")
        .def("isNear", [](const AgentBody& self, const AgentBody& other, float threshold) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return (dx * dx + dy * dy) <= (threshold * threshold);
        }, py::arg("other"), py::arg("threshold"), "Check if near another agent");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Enhanced action result methods
        .def("to_dict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert action result to dictionary")
        .def("from_dict", [](py::dict dict) {
            ActionResult result;
            if (dict.contains("reward")) result.reward = py::cast<float>(dict["reward"]);
            if (dict.contains("success")) result.success = py::cast<bool>(dict["success"]);
            if (dict.contains("message")) result.message = py::cast<std::string>(dict["message"]);
            return result;
        }, py::arg("dict"), "Create action result from dictionary")
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if result is positive")
        .def("isFailure", [](const ActionResult& self) {
            return !self.success;
        }, "Check if result is failure");

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
        // Enhanced sensory percept methods
        .def("updateFromVision", [](SensoryPercept& self, std::shared_ptr<Vision> vision) {
            self.setVision(vision);
            self.setTimestamp(std::chrono::duration<double>::zero());
        }, py::arg("vision"), "Update perception from vision input")
        .def("combinePercepts", [](SensoryPercept& self, const SensoryPercept& other) {
            // Simple percept combination - in practice would be more sophisticated
            auto vision = self.getVision();
            auto other_vision = other.getVision();
            if (vision && other_vision) {
                // Average vision data if both exist
                std::vector<float> combined_data;
                const auto& v1_data = vision->getData();
                const auto& v2_data = other_vision->getData();
                size_t min_size = std::min(v1_data.size(), v2_data.size());
                for (size_t i = 0; i < min_size; ++i) {
                    combined_data.push_back((v1_data[i] + v2_data[i]) * 0.5f);
                }
                vision->setData(combined_data);
                self.setVision(vision);
            }
        }, py::arg("other"), "Combine with another percept")
        .def("normalize", [](SensoryPercept& self) {
            auto vision = self.getVision();
            if (vision) vision->normalize();
            // Normalize other sensory modalities as needed
        }, "Normalize all sensory inputs")
        .def("to_dict", [](const SensoryPercept& self) {
            py::dict result;
            auto vision = self.getVision();
            if (vision) {
                py::dict vision_dict;
                vision_dict["width"] = vision->getWidth();
                vision_dict["height"] = vision->getHeight();
                vision_dict["channels"] = vision->getChannels();
                vision_dict["data"] = vision->getData();
                result["vision"] = vision_dict;
            }
            // Include other modalities
            return result;
        }, "Convert percept to dictionary")
        .def("getFeatureVector", [](const SensoryPercept& self) {
            // Extract features from percept for ML models
            std::vector<float> features;
            auto vision = self.getVision();
            if (vision) {
                const auto& data = vision->getData();
                if (!data.empty()) {
                    // Basic statistics as features
                    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
                    float mean = sum / data.size();
                    float variance = 0.0f;
                    for (float val : data) {
                        variance += (val - mean) * (val - mean);
                    }
                    variance /= data.size();
                    float stddev = std::sqrt(variance);
                    
                    features.push_back(mean);
                    features.push_back(stddev);
                    features.push_back(*std::min_element(data.begin(), data.end()));
                    features.push_back(*std::max_element(data.begin(), data.end()));
                }
            }
            return features;
        }, "Extract feature vector from percept");

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
             "Log brain status");

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
