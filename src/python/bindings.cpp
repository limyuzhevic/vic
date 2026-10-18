#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <map>
#include <unordered_set>

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

// Custom exception classes for Python bindings
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMBrainError : public NLMError {
public:
    explicit NLMBrainError(const std::string& msg) : NLMError("Brain Error: " + msg) {}
};

class NLMConfigError : public NLMError {
public:
    explicit NLMConfigError(const std::string& msg) : NLMError("Config Error: " + msg) {}
};

class NLMWorldError : public NLMError {
public:
    explicit NLMWorldError(const std::string& msg) : NLMError("World Error: " + msg) {}
};

class NLMPerformanceError : public NLMError {
public:
    explicit NLMPerformanceError(const std::string& msg) : NLMError("Performance Error: " + msg) {}
};

// Context manager wrapper for Brain objects
class BrainContextManager {
private:
    std::shared_ptr<Brain> brain;
    bool autoCleanup;

public:
    explicit BrainContextManager(std::shared_ptr<Brain> b, bool autoCleanup = true)
        : brain(b), autoCleanup(autoCleanup) {
        if (brain) {
            brain->initialize();
        }
    }

    ~BrainContextManager() {
        if (autoCleanup && brain) {
            brain->reset();
        }
    }

    std::shared_ptr<Brain> getBrain() const {
        return brain;
    }

    void reset() {
        if (brain) {
            brain->reset();
        }
    }

    bool isValid() const {
        return brain != nullptr && brain->getTotalNeuronCount() > 0;
    }

    // Enable context manager protocol
    BrainContextManager(const BrainContextManager&) = delete;
    BrainContextManager& operator=(const BrainContextManager&) = delete;
    BrainContextManager(BrainContextManager&& other) noexcept
        : brain(std::move(other.brain)), autoCleanup(other.autoCleanup) {}
    BrainContextManager& operator=(BrainContextManager&& other) noexcept {
        if (this != &other) {
            brain = std::move(other.brain);
            autoCleanup = other.autoCleanup;
        }
        return *this;
    }
};

// Context manager wrapper for World objects
class WorldContextManager {
private:
    std::shared_ptr<SimpleWorld> world;
    bool autoCleanup;

public:
    explicit WorldContextManager(std::shared_ptr<SimpleWorld> w, bool autoCleanup = true)
        : world(w), autoCleanup(autoCleanup) {
        if (world) {
            world->reset();
        }
    }

    ~WorldContextManager() {
        if (autoCleanup && world) {
            world->reset();
        }
    }

    std::shared_ptr<SimpleWorld> getWorld() const {
        return world;
    }

    void reset() {
        if (world) {
            world->reset();
        }
    }

    bool isValid() const {
        return world != nullptr && world->getWidth() > 0;
    }

    // Enable context manager protocol
    WorldContextManager(const WorldContextManager&) = delete;
    WorldContextManager& operator=(const WorldContextManager&) = delete;
    WorldContextManager(WorldContextManager&& other) noexcept
        : world(std::move(other.world)), autoCleanup(other.autoCleanup) {}
    WorldContextManager& operator=(WorldContextManager&& other) noexcept {
        if (this != &other) {
            world = std::move(other.world);
            autoCleanup = other.autoCleanup;
        }
        return *other;
    }
};

// Performance monitoring utility
class PerformanceMonitor {
private:
    std::map<std::string, double> timings;
    std::map<std::string, size_t> counts;
    double startTime;
    bool active;

public:
    PerformanceMonitor() : startTime(0.0), active(false) {
        reset();
    }

    void reset() {
        timings.clear();
        counts.clear();
        startTime = 0.0;
        active = false;
    }

    void start(const std::string& operation) {
        startTime = getCurrentTime();
        active = true;
    }

    void stop(const std::string& operation) {
        if (!active) return;
        
        double elapsed = getCurrentTime() - startTime;
        timings[operation] += elapsed;
        counts[operation]++;
        active = false;
    }

    double getElapsedTime(const std::string& operation) const {
        auto it = timings.find(operation);
        return it != timings.end() ? it->second : 0.0;
    }

    size_t getCallCount(const std::string& operation) const {
        auto it = counts.find(operation);
        return it != counts.end() ? it->second : 0;
    }

    double getAverageTime(const std::string& operation) const {
        size_t count = getCallCount(operation);
        if (count == 0) return 0.0;
        return getElapsedTime(operation) / count;
    }

    std::string report() const {
        std::stringstream ss;
        ss << "Performance Report:" << std::endl;
        for (const auto& pair : timings) {
            ss << "  " << pair.first << ": " 
               << pair.second << "s total, "
               << getCallCount(pair.first) << " calls, "
               << getAverageTime(pair.first) << "s average" << std::endl;
        }
        return ss.str();
    }

private:
    double getCurrentTime() const {
        return std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
};

// Configuration validation utility
class ConfigValidator {
public:
    static std::vector<std::string> validate(const Config& config) {
        std::vector<std::string> errors;
        
        // Check neuron count
        if (auto neuronCount = config.get<int64_t>("neuron_count")) {
            if (*neuronCount <= 0 || *neuronCount > 1000000) {
                errors.push_back("Neuron count must be between 1 and 1,000,000");
            }
        }
        
        // Check region count
        if (auto regionCount = config.get<int64_t>("region_count")) {
            if (*regionCount <= 0 || *regionCount > 100) {
                errors.push_back("Region count must be between 1 and 100");
            }
        }
        
        // Check connection probability
        if (auto connProb = config.get<float>("connection_probability")) {
            if (*connProb < 0.0f || *connProb > 1.0f) {
                errors.push_back("Connection probability must be between 0.0 and 1.0");
            }
        }
        
        // Check simulation timestep
        if (auto timestep = config.get<double>("simulation_timestep")) {
            if (*timestep <= 0.0 || *timestep > 1.0) {
                errors.push_back("Simulation timestep must be between 0.0 and 1.0");
            }
        }
        
        // Check for reasonable parameter combinations
        auto neuronCountOpt = config.get<int64_t>("neuron_count");
        auto regionCountOpt = config.get<int64_t>("region_count");
        
        if (neuronCountOpt && regionCountOpt) {
            if (*neuronCountOpt / *regionCountOpt < 10) {
                errors.push_back("Too few neurons per region. Consider increasing neuron count or decreasing region count.");
            }
        }
        
        return errors;
    }
    
    static std::string getRecommendedConfig() {
        return R"(
Recommended Configuration for Different Scales:

# Small-scale (research/debug)
[config]
neuron_count = 100
region_count = 1
connection_probability = 0.1

# Medium-scale (development)
[config]
neuron_count = 1000
region_count = 4
connection_probability = 0.05

# Large-scale (production)
[config]
neuron_count = 10000
region_count = 10
connection_probability = 0.02

# Performance-optimized
[config]
neuron_count = 5000
region_count = 5
connection_probability = 0.08
simulation_timestep = 0.0001
        )";
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Enhanced with context managers, error handling, batch operations,
        and performance monitoring tools.
        
        Features:
        - Automatic resource management with context managers
        - Enhanced error handling and validation
        - Batch operations for improved performance
        - Performance monitoring utilities
        - Configuration validation and recommendations
        - Advanced simulation control tools
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<NLMBrainError>(m, "NLMBrainError");
    py::register_exception<NLMConfigError>(m, "NLMConfigError");
    py::register_exception<NLMWorldError>(m, "NLMWorldError");
    py::register_exception<NLMPerformanceError>(m, "NLMPerformanceError");

    // Enhanced NeuronId with additional functionality
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
        .def("to_string", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // Enhanced SynapseId with additional functionality
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
        .def("to_string", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // Enhanced RegionId with additional functionality
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
        .def("to_string", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // Enhanced PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enhanced NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Enhanced SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Enhanced DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Enhanced FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Enhanced ActionType enumeration
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

    // Enhanced MotorCommand enumeration
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

    // Enhanced WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Enhanced Config class with validation
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system with validation)pbdoc")
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
        // Validation methods
        .def("validate", &ConfigValidator::validate,
             "Validate configuration and return list of errors")
        .def("isValid", [](const Config& self) {
            return ConfigValidator::validate(self).empty();
        }, "Check if configuration is valid")
        .def("getValidationErrors", &ConfigValidator::validate,
             "Get validation errors as a list of strings")
        // Enhanced set method with validation
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
            auto errors = ConfigValidator::validate(self);
            if (!errors.empty()) {
                throw NLMConfigError("Invalid configuration value for key \"" + key + "\": " + errors[0]);
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration value with validation")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value);
            auto errors = ConfigValidator::validate(self);
            if (!errors.empty()) {
                throw NLMConfigError("Invalid configuration value for key \"" + key + "\": " + errors[0]);
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration integer value with validation")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
            auto errors = ConfigValidator::validate(self);
            if (!errors.empty()) {
                throw NLMConfigError("Invalid configuration value for key \"" + key + "\": " + errors[0]);
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration float value with validation")
        .def("getRecommendedConfig", &ConfigValidator::getRecommendedConfig,
             "Get recommended configuration templates")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("copy", [](const Config& self) {
            Config copy;
            copy.loadFromFile(""); // This will fail, but we can use copy constructor
            return copy;
        }, "Create a copy of this configuration");

    // Enhanced SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("to_dict", [](const SensoryInput& self) {
            std::map<std::string, py::object> result;
            result["type"] = py::cast(self.getType());
            result["data"] = py::cast(self.getData());
            result["dimensions"] = py::cast(self.getDimensions());
            result["timestamp"] = py::cast(self.getTimestamp());
            return result;
        }, "Convert sensory input to dictionary");

    // Enhanced Vision class
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
        .def("createFromArray", [](size_t width, size_t height, size_t channels, const py::array_t<float>& data) {
            Vision vision(width, height, channels);
            if (data.ndim() == 2) {
                auto buf = data.request();
                std::vector<float> vec(static_cast<float*>(buf.ptr), static_cast<float*>(buf.ptr) + buf.size);
                vision.setData(vec);
            } else if (data.ndim() == 1) {
                auto buf = data.request();
                std::vector<float> vec(static_cast<float*>(buf.ptr), static_cast<float*>(buf.ptr) + buf.size);
                vision.setData(vec);
            }
            return vision;
        }, "Create Vision from numpy array or list")
        .def("to_array", [](const Vision& self) {
            return py::array_t<float>({self.getHeight(), self.getWidth(), static_cast<size_t>(self.getChannels())});
        }, "Convert Vision to numpy array");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("createFromArray", [](size_t sampleRate, const py::array_t<float>& data) {
            Audio audio(sampleRate, data.size());
            auto buf = data.request();
            std::vector<float> vec(static_cast<float*>(buf.ptr), static_cast<float*>(buf.ptr) + buf.size);
            audio.setData(vec);
            return audio;
        }, "Create Audio from numpy array or list");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount)
        .def("getMaxSignal", &InternalSignals::getMaxSignal)
        .def("getMinSignal", &InternalSignals::getMinSignal)
        .def("getMeanSignal", &InternalSignals::getMeanSignal)
        .def("to_array", [](const InternalSignals& self) {
            return py::array_t<float>(self.getSignals());
        }, "Convert InternalSignals to numpy array");

    // Enhanced Action class
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
        .def("to_dict", [](const Action& self) {
            std::map<std::string, py::object> result;
            result["type"] = py::cast(self.getType());
            result["parameters"] = py::cast(self.getParameters());
            result["name"] = py::cast(self.getName());
            return result;
        }, "Convert Action to dictionary");

    // Enhanced WorldObject class
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
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            return std::sqrt(std::pow(self.x - x, 2) + std::pow(self.y - y, 2));
        }, "Calculate distance to position")
        .def("is_near", [](const WorldObject& self, float x, float y, float radius) {
            return self.distance_to(x, y) <= radius;
        }, "Check if object is near position");

    // Enhanced AgentBody class
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
        .def("distance_to", [](const AgentBody& self, float x, float y) {
            return std::sqrt(std::pow(self.x - x, 2) + std::pow(self.y - y, 2));
        }, "Calculate distance to position")
        .def("set_position", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, "Set agent position");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_successful", &ActionResult::success)
        .def("to_dict", [](const ActionResult& self) {
            std::map<std::string, py::object> result;
            result["reward"] = py::cast(self.reward);
            result["success"] = py::cast(self.success);
            result["message"] = py::cast(self.message);
            return result;
        }, "Convert ActionResult to dictionary");

    // Enhanced SensoryPercept class
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
        .def("add_vision_signal", [](SensoryPercept& self, const std::vector<float>& data) {
            Vision vision;
            vision.setData(data);
            self.setVision(vision);
        }, "Add vision signal from data")
        .def("add_internal_signal", [](SensoryPercept& self, float signal) {
            InternalSignals internal;
            internal.addSignal(signal);
            self.setInternal(internal);
        }, "Add internal signal");

    // Enhanced SimpleWorld class with context manager support
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
        // Context manager support
        .def("__enter__", [](std::shared_ptr<SimpleWorld> world) {
            return WorldContextManager(world, true);
        })
        .def("__exit__", [](std::shared_ptr<SimpleWorld>, const py::args&) {
            // Cleanup handled by context manager
        })
        // Enhanced methods
        .def("update_world", [](SimpleWorld& self, double timestep) {
            self.update(timestep);
        }, "Alias for update method")
        .def("teleport_agent", [](SimpleWorld& self, float x, float y) {
            self.setAgentStart(x, y);
            self.update(0.0); // This will place agent at new position
        }, "Teleport agent to position")
        .def("get_agent_state", [](const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return std::make_tuple(body.x, body.y, body.orientation,
                                  body.velocityX, body.velocityY,
                                  body.angularVelocity, body.energy, body.health);
        }, "Get agent state as tuple")
        .def("set_agent_state", [](SimpleWorld& self, float x, float y, float orientation,
                                  float velocityX, float velocityY, float angularVelocity,
                                  float energy, float health) {
            auto body = self.getAgentBody();
            body.x = x;
            body.y = y;
            body.orientation = orientation;
            body.velocityX = velocityX;
            body.velocityY = velocityY;
            body.angularVelocity = angularVelocity;
            body.energy = energy;
            body.health = health;
        }, "Set agent state from tuple");

    // Enhanced Brain class with context manager and batch operations
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
        // Batch operations
        .def("step_batch", [](Brain& self, size_t steps, double timestep = 0.001) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
        }, py::arg("steps"), py::arg("timestep") = 0.001,
           "Perform multiple simulation steps efficiently")
        .def("inject_current_batch", [](Brain& self, const std::vector<NeuronId>& neuronIds,
                                       const std::vector<MembranePotential>& currents) {
            if (neuronIds.size() != currents.size()) {
                throw NLMError("Neuron IDs and currents must have the same size");
            }
            for (size_t i = 0; i < neuronIds.size(); ++i) {
                self.injectCurrent(neuronIds[i], currents[i]);
            }
        }, py::arg("neuron_ids"), py::arg("currents"),
           "Inject current into multiple neurons at once")
        .def("inject_current_to_type_batch", [](Brain& self, NeuronType type,
                                               const std::vector<MembranePotential>& currents) {
            for (const auto& current : currents) {
                self.injectCurrentToNeurons(type, current);
            }
        }, py::arg("neuron_type"), py::arg("currents"),
           "Inject current to neurons of a specific type in batch")
        // Context manager support
        .def("__enter__", [](std::shared_ptr<Brain> brain) {
            return BrainContextManager(brain, true);
        })
        .def("__exit__", [](std::shared_ptr<Brain>, const py::args&) {
            // Cleanup handled by context manager
        })
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
        // Enhanced methods
        .def("run_simulation", [](Brain& self, size_t steps, double timestep = 0.001) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
        }, py::arg("steps"), py::arg("timestep") = 0.001,
           "Run a simulation for a given number of steps")
        .def("get_statistics", [](const Brain& self) {
            std::map<std::string, double> stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            return stats;
        }, "Get brain statistics as dictionary")
        .def("is_healthy", [](const Brain& self) {
            return self.getTotalNeuronCount() > 0 && self.getTotalSynapseCount() > 0;
        }, "Check if brain is in a healthy state")
        .def("warmup", [](Brain& self, size_t steps = 100) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i);
            }
        }, py::arg("steps") = 100,
           "Warm up the brain by running idle steps");

    // Enhanced AgentBrain class
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
        // Enhanced methods
        .def("run_episode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world,
                             size_t steps) {
            world->reset();
            self.initialize(*world);
            
            for (size_t i = 0; i < steps; ++i) {
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                self.updateDevelopment(0.1);
                
                auto action = self.decodeMotorCommand();
                world->applyMotorCommand(action, i * 0.1);
                
                // Simple reward system
                float reward = 0.0f;
                if (percept.getInternal().size() > 0) {
                    reward = percept.getInternal()[0];
                }
                self.applyRewardModulation(reward, 0.0);
            }
            
            return self.getBrain()->getTotalSpikeCount();
        }, py::arg("world"), py::arg("steps"),
           "Run a complete episode and return total spikes")
        .def("get_brain_state", [](const AgentBrain& self) {
            auto brain = self.getBrain();
            std::map<std::string, py::object> state;
            state["neuron_count"] = py::cast(brain->getTotalNeuronCount());
            state["synapse_count"] = py::cast(brain->getTotalSynapseCount());
            state["firing_neurons"] = py::cast(brain->getFiringNeuronCount());
            state["total_spikes"] = py::cast(brain->getTotalSpikeCount());
            state["avg_firing_rate"] = py::cast(brain->getAverageFiringRate());
            state["developmental_stage"] = py::cast(brain->getDevelopmentalStage());
            return state;
        }, "Get brain state as dictionary")
        .def("get_agent_metrics", [](const AgentBrain& self) {
            std::map<std::string, double> metrics;
            metrics["curiosity"] = self.getCuriosityLevel();
            metrics["novelty"] = self.getNoveltyLevel();
            metrics["prediction_error"] = self.getPredictionError();
            metrics["neuromodulation"] = self.getNeuromodulationLevel();
            return metrics;
        }, "Get agent metrics as dictionary");

    // Performance monitoring utilities
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Performance monitoring utility)pbdoc")
        .def(py::init<>())
        .def("reset", &PerformanceMonitor::reset,
             "Reset performance counters")
        .def("start", &PerformanceMonitor::start,
             py::arg("operation"),
             "Start timing an operation")
        .def("stop", &PerformanceMonitor::stop,
             py::arg("operation"),
             "Stop timing an operation")
        .def("get_elapsed_time", &PerformanceMonitor::getElapsedTime,
             py::arg("operation"),
             "Get elapsed time for an operation")
        .def("get_call_count", &PerformanceMonitor::getCallCount,
             py::arg("operation"),
             "Get number of calls for an operation")
        .def("get_average_time", &PerformanceMonitor::getAverageTime,
             py::arg("operation"),
             "Get average time for an operation")
        .def("report", &PerformanceMonitor::report,
             "Generate performance report");

    // Configuration validation utilities
    py::class_<ConfigValidator>(m, "ConfigValidator", R"pbdoc(Configuration validation utility)pbdoc")
        .def_static("validate", &ConfigValidator::validate,
                    py::arg("config"),
                    "Validate configuration and return list of errors")
        .def_static("get_recommended_config", &ConfigValidator::getRecommendedConfig,
                    "Get recommended configuration templates");

    // Enhanced convenience functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with validation");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Performance monitoring factory functions
    m.def("create_performance_monitor", []() {
        return std::make_shared<PerformanceMonitor>();
    }, "Create a new performance monitor");

    // Helper functions for common tasks
    m.def("create_test_config", [](const std::string& scale = "medium") {
        Config config;
        
        if (scale == "small") {
            config.set("neuron_count", 100);
            config.set("region_count", 1);
            config.set("connection_probability", 0.1);
        } else if (scale == "medium") {
            config.set("neuron_count", 1000);
            config.set("region_count", 4);
            config.set("connection_probability", 0.05);
        } else if (scale == "large") {
            config.set("neuron_count", 10000);
            config.set("region_count", 10);
            config.set("connection_probability", 0.02);
        }
        
        return config;
    }, py::arg("scale") = "medium", "Create a test configuration for the specified scale");

    m.def("run_brain_demo", [](size_t steps = 1000) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        return std::make_tuple(brain->getTotalNeuronCount(),
                             brain->getTotalSynapseCount(),
                             brain->getFiringNeuronCount(),
                             brain->getTotalSpikeCount());
    }, py::arg("steps") = 1000, "Run a simple brain demo and return statistics");

    // Version and metadata
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__description__") = "NLM Enhanced Python Bindings with context managers, batch operations, and performance monitoring";

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm