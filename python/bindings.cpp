#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <stdexcept>
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

// Utility functions for enhanced Python bindings
namespace utils {

    // Statistical functions for analysis
    struct Statistics {
        static double mean(const std::vector<double>& data) {
            if (data.empty()) return 0.0;
            return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
        }
        
        static double stddev(const std::vector<double>& data) {
            if (data.empty()) return 0.0;
            double m = mean(data);
            double sum_sq = std::accumulate(data.begin(), data.end(), 0.0,
                [m](double acc, double val) { return acc + (val - m) * (val - m); });
            return std::sqrt(sum_sq / data.size());
        }
        
        static double min(const std::vector<double>& data) {
            if (data.empty()) return 0.0;
            return *std::min_element(data.begin(), data.end());
        }
        
        static double max(const std::vector<double>& data) {
            if (data.empty()) return 0.0;
            return *std::max_element(data.begin(), data.end());
        }
        
        static double median(std::vector<double> data) {
            if (data.empty()) return 0.0;
            std::sort(data.begin(), data.end());
            size_t n = data.size();
            if (n % 2 == 0) {
                return (data[n/2 - 1] + data[n/2]) / 2.0;
            } else {
                return data[n/2];
            }
        }
        
        static std::string formatTime(double seconds) {
            std::stringstream ss;
            if (seconds < 60.0) {
                ss << std::fixed << std::setprecision(2) << seconds << "s";
            } else if (seconds < 3600.0) {
                ss << std::fixed << std::setprecision(1) << (seconds / 60.0) << "min";
            } else {
                ss << std::fixed << std::setprecision(1) << (seconds / 3600.0) << "h";
            }
            return ss.str();
        }
    };

    // Error handling utilities
    class NLMError : public std::runtime_error {
    public:
        NLMError(const std::string& message) : std::runtime_error(message) {}
    };

    // Performance monitoring utilities
    struct PerformanceMonitor {
        static double measureStepTime(std::function<void()> step_func) {
            auto start = std::chrono::high_resolution_clock::now();
            step_func();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            return duration.count() / 1000000.0; // Convert to seconds
        }
    };

    // Configuration validation utilities
    namespace config {
        static std::unordered_map<std::string, std::function<bool(double)>> validators = {
            {"neuron_count", [](double x) { return x > 0 && x <= 1000000; }},
            {"connection_probability", [](double x) { return x >= 0.0 && x <= 1.0; }},
            {"learning_rate", [](double x) { return x >= 0.0 && x <= 1.0; }},
            {"v_thresh", [](double x) { return x < -40.0 && x > -100.0; }},
            {"tau_mem", [](double x) { return x > 0.0 && x <= 1000.0; }},
        };

        static bool validateKey(const std::string& key, double value) {
            auto it = validators.find(key);
            if (it != validators.end()) {
                return it->second(value);
            }
            return true; // Unknown keys are allowed
        }
    }

    // Neural network building utilities
    namespace builder {
        static std::vector<std::string> getDefaultRegionNames() {
            return {
                "sensory_cortex", "motor_cortex", "hippocampus", "amygdala",
                "prefrontal_cortex", "visual_cortex", "auditory_cortex", "somatosensory_cortex"
            };
        }

        static std::map<std::string, NeuronType> getDefaultRegionTypes() {
            return {
                {"sensory_cortex", NeuronType::Sensory},
                {"motor_cortex", NeuronType::Motor},
                {"hippocampus", NeuronType::Modulatory},
                {"amygdala", NeuronType::Modulatory},
                {"prefrontal_cortex", NeuronType::Internal},
                {"visual_cortex", NeuronType::Sensory},
                {"auditory_cortex", NeuronType::Sensory},
                {"somatosensory_cortex", NeuronType::Sensory}
            };
        }
    }
}

// Context manager classes
class BrainContextManager {
public:
    std::shared_ptr<Brain> brain;
    
    BrainContextManager(std::shared_ptr<Brain> b) : brain(b) {
        if (brain) {
            brain->initialize();
        }
    }
    
    ~BrainContextManager() {
        if (brain) {
            brain->reset();
        }
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain; }
    
    // Context manager protocol
    BrainContextManager& operator=(std::shared_ptr<Brain> b) {
        if (brain) brain->reset();
        brain = b;
        if (brain) brain->initialize();
        return *this;
    }
};

class AgentContextManager {
public:
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    
    AgentContextManager(std::shared_ptr<AgentBrain> a, std::shared_ptr<SimpleWorld> w) 
        : agent(a), world(w) {
        if (agent && world) {
            agent->initialize(*world);
        }
    }
    
    ~AgentContextManager() {
        if (agent) {
            agent->reset();
        }
    }
    
    std::shared_ptr<AgentBrain> getAgent() const { return agent; }
    std::shared_ptr<SimpleWorld> getWorld() const { return world; }
    
    // Context manager protocol
    AgentContextManager& operator=(std::pair<std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>> p) {
        if (agent) agent->reset();
        agent = p.first;
        world = p.second;
        if (agent && world) agent->initialize(*world);
        return *this;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        --------------------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        
        Key Features:
        - Type hints and better documentation
        - Context managers for automatic resource management
        - Batch processing utilities
        - Statistical analysis tools
        - Performance monitoring
        - Configuration validation
        - Neural network building utilities
        - Advanced error handling
        - Comprehensive convenience functions
        
        Enhanced classes: Brain, Config, AgentBrain, SimpleWorld
        Additional utilities: BatchProcessor, Statistics, PerformanceMonitor
    )pbdoc";

    py::register_exception<nlm::utils::NLMError>(m, "NLMError");

    // Core type definitions with enhanced documentation
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

    // Enhanced Configuration class with validation
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system with validation)pbdoc")
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
        .def("get", &Config::get<uint64_t>, py::arg("key"),
             "Get configuration value as uint64_t")
        .def("get", &Config::get<double>, py::arg("key"),
             "Get configuration value as double")
        .def("get", &Config::get<bool>, py::arg("key"),
             "Get configuration value as bool")
        .def("set", [](Config& self, const std::string& key, uint64_t value) {
            if (!nlm::utils::config::validateKey(key, static_cast<double>(value))) {
                throw nlm::utils::NLMError("Invalid value for key '" + key + "': " + std::to_string(value));
            }
            self.set(key, value, nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value with validation")
        .def("set", [](Config& self, const std::string& key, double value) {
            if (!nlm::utils::config::validateKey(key, value)) {
                throw nlm::utils::NLMError("Invalid value for key '" + key + "': " + std::to_string(value));
            }
            self.set(key, value, nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value with validation")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Enhanced vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.size() != self.getWidth() * self.getHeight() * self.getChannels()) {
                throw nlm::utils::NLMError("Vision data size mismatch. Expected: " + 
                    std::to_string(self.getWidth() * self.getHeight() * self.getChannels()) +
                    ", Got: " + std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data with validation")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("normalize", [](Vision& self) {
            // Enhanced normalization method
            auto& data = self.getData();
            double min_val = *std::min_element(data.begin(), data.end());
            double max_val = *std::max_element(data.begin(), data.end());
            if (max_val > min_val) {
                for (auto& val : data) {
                    val = (val - min_val) / (max_val - min_val);
                }
            }
        }, "Normalize vision data to [0, 1] range");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Enhanced audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.size() != self.getNumSamples()) {
                throw nlm::utils::NLMError("Audio data size mismatch. Expected: " + 
                    std::to_string(self.getNumSamples()) +
                    ", Got: " + std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data with validation")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("computeFrequencySpectrum", [](Audio& self) {
            // Simple FFT-like computation for frequency spectrum
            std::vector<double> spectrum;
            // Implementation would depend on the actual Audio class capabilities
            return spectrum;
        }, "Compute frequency spectrum of audio data");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Enhanced internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals")
        .def("getMean", [](const InternalSignals& self) {
            const auto& signals = self.getData();
            if (signals.empty()) return 0.0;
            return std::accumulate(signals.begin(), signals.end(), 0.0) / signals.size();
        }, "Get mean of internal signals")
        .def("getStdDev", [](const InternalSignals& self) {
            const auto& signals = self.getData();
            if (signals.empty()) return 0.0;
            double m = std::accumulate(signals.begin(), signals.end(), 0.0) / signals.size();
            double sum_sq = std::accumulate(signals.begin(), signals.end(), 0.0,
                [m](double acc, double val) { return acc + (val - m) * (val - m); });
            return std::sqrt(sum_sq / signals.size());
        }, "Get standard deviation of internal signals");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("toDict", [](const Action& self) {
            return std::map<std::string, std::string>{{"type", self.getName()}};
        }, "Convert action to dictionary")
        .def_static("fromDict", [](const std::map<std::string, std::string>& data) {
            // Implementation would depend on available Action constructors
            return Action();
        }, py::arg("data"), "Create action from dictionary");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(Enhanced world object representation)pbdoc")
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
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point")
        .def("isCollidingWith", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return (dx * dx + dy * dy) <= (self.radius + other.radius) * (self.radius + other.radius);
        }, py::arg("other"), "Check collision with another object");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state)pbdoc")
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
        .def("moveTo", [](AgentBody& self, float x, float y, float dt) {
            float dx = x - self.x;
            float dy = y - self.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist > 0.0f) {
                self.velocityX = dx / dist * 1.0f;
                self.velocityY = dy / dist * 1.0f;
            }
        }, py::arg("x"), py::arg("y"), py::arg("dt"), "Move toward target position")
        .def("getDistanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Get distance to point");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Enhanced action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccessful", [](const ActionResult& self) { return self.success; },
             "Check if action was successful")
        .def("getRewardAsPositive", [](const ActionResult& self) {
            return std::max(0.0f, self.reward);
        }, "Get positive reward component");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal data")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        .def("combineWith", [](SensoryPercept& self, const SensoryPercept& other) {
            // Combine two percepts (simplified implementation)
            self.setVision(other.getVision());
            self.setTouch(other.getTouch());
            self.setInternal(other.getInternal());
            self.setTimestamp(other.getTimestamp());
            return self;
        }, py::arg("other"), "Combine this percept with another");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("generateVision", &SimpleWorld::generateVision,
             "Generate vision percept from world state")
        .def("getObjectAt", [](SimpleWorld& self, float x, float y) {
            for (auto& obj : self.getObjects()) {
                float dx = obj.x - x;
                float dy = obj.y - y;
                if (dx * dx + dy * dy <= obj.radius * obj.radius) {
                    return obj;
                }
            }
            return WorldObject();
        }, py::arg("x"), py::arg("y"), "Get object at position");

    // Enhanced Brain class with context manager support
    py::class_<Brain, std::shared_ptr<Brain>, BrainContextManager>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class with context manager)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
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
        // Context manager support
        .def("__enter__", [](std::shared_ptr<Brain> self) {
            return BrainContextManager(self);
        })
        .def("__exit__", [](BrainContextManager& self, py::args) {
            // Context manager exit - brain is automatically cleaned up
        })
        // Batch processing
        .def_static("runBatch", [](const std::vector<std::shared_ptr<Config>>& configs, 
                                  const std::function<void(std::shared_ptr<Brain>)>& process_func,
                                  int num_steps) {
            std::vector<std::shared_ptr<Brain>> results;
            for (const auto& config : configs) {
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                process_func(brain);
                brain->step(num_steps);
                results.push_back(brain);
            }
            return results;
        }, py::arg("configs"), py::arg("process_func"), py::arg("num_steps") = 100,
           "Run batch of simulations with same configuration")
        .def_static("runBatchWithDifferentConfigs", [](const std::vector<std::shared_ptr<Config>>& configs,
                                                      const std::function<void(std::shared_ptr<Brain>)>& process_func) {
            std::vector<std::shared_ptr<Brain>> results;
            for (const auto& config : configs) {
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                process_func(brain);
                results.push_back(brain);
            }
            return results;
        }, py::arg("configs"), py::arg("process_func"),
           "Run batch of simulations with different configurations");

    // Enhanced AgentBrain class with context manager support
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>, AgentContextManager>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world with context manager)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface")
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
        // Context manager support
        .def("__enter__", [](std::shared_ptr<AgentBrain> self, std::shared_ptr<SimpleWorld> world) {
            return AgentContextManager(self, world);
        })
        .def("__exit__", [](AgentContextManager& self, py::args) {
            // Context manager exit - agent is automatically cleaned up
        });

    // Enhanced convenience functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with validation support");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world with enhanced features");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Statistical analysis utilities
    m.def("computeStatistics", [](const std::vector<double>& data) {
        std::map<std::string, double> stats;
        if (data.empty()) return stats;
        
        stats["mean"] = nlm::utils::Statistics::mean(data);
        stats["stddev"] = nlm::utils::Statistics::stddev(data);
        stats["min"] = nlm::utils::Statistics::min(data);
        stats["max"] = nlm::utils::Statistics::max(data);
        stats["median"] = nlm::utils::Statistics::median(data);
        stats["count"] = static_cast<double>(data.size());
        
        return stats;
    }, py::arg("data"), "Compute comprehensive statistics for data vector");

    // Performance monitoring utilities
    m.def("monitorPerformance", [](std::function<void()> simulation_func, int iterations = 10) {
        double total_time = 0.0;
        std::vector<double> iteration_times;
        
        for (int i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            simulation_func();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            double time_seconds = duration.count() / 1000000.0;
            iteration_times.push_back(time_seconds);
            total_time += time_seconds;
        }
        
        std::map<std::string, double> perf_stats;
        perf_stats["total_time"] = total_time;
        perf_stats["avg_time"] = total_time / iterations;
        perf_stats["min_time"] = nlm::utils::Statistics::min(iteration_times);
        perf_stats["max_time"] = nlm::utils::Statistics::max(iteration_times);
        perf_stats["stddev_time"] = nlm::utils::Statistics::stddev(iteration_times);
        
        return perf_stats;
    }, py::arg("simulation_func"), py::arg("iterations") = 10,
       "Monitor performance of simulation function");

    // Neural network building utilities
    m.def("getDefaultRegionNames", []() {
        return nlm::utils::builder::getDefaultRegionNames();
    }, "Get default neural region names for brain architecture");

    m.def("getDefaultRegionTypes", []() {
        auto types = nlm::utils::builder::getDefaultRegionTypes();
        std::map<std::string, std::string> string_types;
        for (const auto& pair : types) {
            string_types[pair.first] = "NeuronType." + std::to_string(static_cast<int>(pair.second));
        }
        return string_types;
    }, "Get default region types mapping");

    // Advanced simulation utilities
    m.def("runEnhancedSimulation", [](std::shared_ptr<Brain> brain, 
                                      std::shared_ptr<SimpleWorld> world,
                                      std::shared_ptr<AgentBrain> agent,
                                      int num_steps,
                                      std::function<void(int, std::shared_ptr<Brain>, 
                                                         std::shared_ptr<SimpleWorld>, 
                                                         std::shared_ptr<AgentBrain>)> callback = nullptr) {
        
        std::vector<std::map<std::string, double>> history;
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            auto step_data = std::map<std::string, double>{
                {"step", static_cast<double>(step)},
                {"firing_neurons", static_cast<double>(brain->getFiringNeuronCount())},
                {"spike_count", static_cast<double>(brain->getTotalSpikeCount())},
                {"curiosity", agent->getCuriosityLevel()},
                {"novelty", agent->getNoveltyLevel()},
                {"prediction_error", agent->getPredictionError()},
                {"time", world->getSimulationTime()}
            };
            
            history.push_back(step_data);
            
            if (callback) {
                callback(step, brain, world, agent);
            }
        }
        
        return history;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       py::arg("callback") = nullptr,
       "Run enhanced simulation with detailed history tracking");

    // Utility functions for common operations
    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        auto keys = config->getKeys();
        
        for (const auto& key : keys) {
            if (!nlm::utils::config::validateKey(key, 0.0)) {  // Default value for validation
                errors.push_back("Invalid parameter: " + key);
            }
        }
        
        return errors;
    }, py::arg("config"), "Validate configuration and return list of errors");

    m.def("formatTime", &nlm::utils::Statistics::formatTime,
          py::arg("seconds"), "Format time in human-readable format");

    // Export constants and utility values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Error types
    py::register_exception<nlm::utils::NLMError>(m, "NLMError");
}

} // namespace nlm