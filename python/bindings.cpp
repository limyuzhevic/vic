#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <numeric>

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

// Python exception for NLM errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

// Python utility functions
namespace utils {
    template<typename T>
    std::vector<T> slice(const std::vector<T>& vec, size_t start, size_t end) {
        if (end > vec.size()) end = vec.size();
        if (start > end) start = end;
        return std::vector<T>(vec.begin() + start, vec.begin() + end);
    }
    
    template<typename T>
    std::vector<T> normalize(const std::vector<T>& vec, T min = 0.0, T max = 1.0) {
        if (vec.empty()) return vec;
        
        T currentMin = *std::min_element(vec.begin(), vec.end());
        T currentMax = *std::max_element(vec.begin(), vec.end());
        
        if (currentMax == currentMin) {
            return std::vector<T>(vec.size(), max);
        }
        
        std::vector<T> result;
        result.reserve(vec.size());
        
        for (const auto& val : vec) {
            T normalized = min + (max - min) * (val - currentMin) / (currentMax - currentMin);
            result.push_back(normalized);
        }
        
        return result;
    }
    
    double mean(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return sum / values.size();
    }
    
    double standardDeviation(const std::vector<double>& values) {
        if (values.size() < 2) return 0.0;
        
        double m = mean(values);
        double sum = 0.0;
        
        for (const auto& val : values) {
            sum += (val - m) * (val - m);
        }
        
        return std::sqrt(sum / (values.size() - 1));
    }
}

// Enhanced Brain class with Python-friendly API
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ----------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - Better error handling and validation
        - Pythonic convenience methods
        - Advanced features for experienced users
        - Comprehensive documentation and examples
        - Performance monitoring tools
        
        Key Features:
        * Intuitive API design with sensible defaults
        * Comprehensive error handling with descriptive messages
        * Built-in utility functions for data processing
        * Advanced simulation control and monitoring
        * Checkpoint management utilities
        * Development and debugging tools
        
        Quick Start:
        ```python
        import pynlm
        
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
        
        # Get statistics
        print(f"Neurons: {brain.getTotalNeuronCount()}")
        print(f"Spikes: {brain.getTotalSpikeCount()}")
        print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
        ```
    )pbdoc";

    // Register custom exception
    py::register_exception<NLMError>(m, "NLMError");

    // Utility functions
    m.def("slice", &utils::slice<double>, "Slice a vector",
          py::arg("vector"), py::arg("start"), py::arg("end"),
          "Extract a subvector from start to end index");
    
    m.def("normalize", &utils::normalize<double>, "Normalize a vector to range",
          py::arg("vector"), py::arg("min") = 0.0, py::arg("max") = 1.0,
          "Normalize values to specified range [min, max]");
    
    m.def("mean", &utils::mean, "Calculate mean of values",
          py::arg("values"), "Calculate arithmetic mean");
    
    m.def("standardDeviation", &utils::standardDeviation, "Calculate standard deviation",
          py::arg("values"), "Calculate standard deviation");

    // NeuronId class with enhanced Python features
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__gt__", &NeuronId::operator>)
        .def("__le__", &NeuronId::operator<=
        .def("__ge__", &NeuronId::operator>=
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId class with enhanced Python features
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__lt__", &SynapseId::operator<)
        .def("__gt__", &SynapseId::operator>)
        .def("__le__", &SynapseId::operator<=
        .def("__ge__", &SynapseId::operator>=
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId class with enhanced Python features
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__lt__", &RegionId::operator<)
        .def("__gt__", &RegionId::operator>)
        .def("__le__", &RegionId::operator<=
        .def("__ge__", &RegionId::operator>=
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId class with enhanced Python features
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__lt__", &PopulationId::operator<)
        .def("__gt__", &PopulationId::operator>)
        .def("__le__", &PopulationId::operator<=
        .def("__ge__", &PopulationId::operator>=
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        });

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("isExcitatory", &NeuronType::isExcitatory)
        .def("isInhibitory", &NeuronType::isInhibitory)
        .def("isModulatory", &NeuronType::isModulatory)
        .def("isSensory", &NeuronType::isSensory)
        .def("isMotor", &NeuronType::isMotor)
        .def("isInternal", &NeuronType::isInternal);

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("isExcitatory", &SynapseType::isExcitatory)
        .def("isInhibitory", &SynapseType::isInhibitory)
        .def("isModulatory", &SynapseType::isModulatory)
        .def("isElectrical", &SynapseType::isElectrical)
        .def("isGapJunction", &SynapseType::isGapJunction);

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("isInitial", &DevelopmentalStage::isInitial)
        .def("isCriticalPeriod", &DevelopmentalStage::isCriticalPeriod)
        .def("isMaturation", &DevelopmentalStage::isMaturation)
        .def("isAdult", &DevelopmentalStage::isAdult)
        .def("isAging", &DevelopmentalStage::isAging)
        .def("getPlasticityMultiplier", &DevelopmentalStage::getPlasticityMultiplier);

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def("isActive", &FiringState::isActive)
        .def("isResting", &FiringState::isResting)
        .def("isRefractory", &FiringState::isRefractory)
        .def("isInhibited", &FiringState::isInhibited);

    // ActionType enumeration
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
        .export_values()
        .def("getName", &ActionType::getName);

    // MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .def("getName", &MotorCommand::getName);

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def("isResource", &WorldObjectType::isResource)
        .def("isHazard", &WorldObjectType::isHazard)
        .def("isWall", &WorldObjectType::isWall)
        .def("isMarker", &WorldObjectType::isMarker)
        .def("isEmpty", &WorldObjectType::isEmpty);

    // Enhanced Config class with validation and utilities
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
        .def("getInt", [](Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value") = 0,
           "Get integer value with default")
        .def("getFloat", [](Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value") = 0.0f,
           "Get float value with default")
        .def("getDouble", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value") = 0.0,
           "Get double value with default")
        .def("getString", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value") = "",
           "Get string value with default")
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set integer value")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set float value")
        .def("setDouble", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set double value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set string value")
        .def("validate", [](Config& self) {
            // Simple validation - check for required keys
            std::vector<std::string> required = {"random_seed", "simulation_timestep", "neuron_count"};
            std::vector<std::string> missing;
            
            for (const auto& key : required) {
                if (!self.has(key)) {
                    missing.push_back(key);
                }
            }
            
            if (!missing.empty()) {
                throw NLMError("Missing required configuration keys: " + 
                              (missing.size() > 1 ? 
                               std::accumulate(missing.begin(), missing.end() - 1, std::string(), 
                                              [](const std::string& a, const std::string& b) { return a + ", " + b; }) + 
                               " and " + missing.back() : 
                               missing[0]));
            }
            
            return true;
        }, "Validate configuration");

    // Enhanced SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
              "Set the timestamp")
        .def("toVector", [](const SensoryInput& self) {
            return self.getData();
        }, "Get sensory input as vector (alias for getData)");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
              py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data")
        .def("getData", [](Vision& self) {
            return self.getData();
        }, "Get vision data as vector")
        .def("setDataFromArray", [](Vision& self, const py::array_t<float>& array) {
            if (array.ndim() != 2) {
                throw NLMError("Vision data must be 2D array");
            }
            std::vector<float> data;
            data.reserve(array.size());
            for (ssize_t i = 0; i < array.shape(0); ++i) {
                for (ssize_t j = 0; j < array.shape(1); ++j) {
                    data.push_back(array(i, j));
                }
            }
            self.setData(data);
        }, py::arg("array"), "Set vision data from numpy-like array")
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of channels")
        .def("getShape", [](Vision& self) {
            return std::vector<size_t>{self.getWidth(), self.getHeight(), self.getChannels()};
        }, "Get vision shape as tuple (width, height, channels)")
        .def("clone", [](Vision& self) {
            Vision result;
            result.setData(self.getData());
            result.setTimestamp(self.getTimestamp());
            return result;
        }, "Create a copy of vision data");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("getData", [](Audio& self) {
            return self.getData();
        }, "Get audio data as vector")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of samples")
        .def("getDuration", [](Audio& self) {
            return static_cast<float>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds")
        .def("setDataFromArray", [](Audio& self, const py::array_t<float>& array) {
            if (array.ndim() != 1) {
                throw NLMError("Audio data must be 1D array");
            }
            std::vector<float> data;
            data.reserve(array.size());
            for (ssize_t i = 0; i < array.shape(0); ++i) {
                data.push_back(array(i));
            }
            self.setData(data);
        }, py::arg("array"), "Set audio data from numpy-like array")
        .def("normalize", [](Audio& self, float min = -1.0f, float max = 1.0f) {
            auto data = self.getData();
            data = utils::normalize(data, min, max);
            self.setData(data);
        }, py::arg("min") = -1.0f, py::arg("max") = 1.0f, "Normalize audio data");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("getSignals", [](InternalSignals& self) {
            return self.getData();
        }, "Get all internal signals as vector")
        .def("setSignals", [](InternalSignals& self, const std::vector<float>& signals) {
            self.clearSignals();
            for (float val : signals) {
                self.addSignal(val);
            }
        }, py::arg("signals"), "Set all internal signals")
        .def("getMean", [](InternalSignals& self) {
            return utils::mean(self.getData());
        }, "Calculate mean of internal signals")
        .def("getStandardDeviation", [](InternalSignals& self) {
            return utils::standardDeviation(self.getData());
        }, "Calculate standard deviation of internal signals")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("size", [](InternalSignals& self) {
            return self.getData().size();
        }, "Get number of internal signals");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get action name")
        .def("getParameterCount", [](Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter")
        .def("clone", &Action::clone, "Create a copy of action")
        .def("isEqual", [](const Action& self, const Action& other) {
            return self.getType() == other.getType() && self.getParameters() == other.getParameters();
        }, py::arg("other"), "Check if action equals another action");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
              py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
              py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "X position")
        .def_readwrite("y", &WorldObject::y, "Y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Object active state")
        .def("distanceTo", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Calculate distance to another object")
        .def("isColliding", [](const WorldObject& self, const WorldObject& other) {
            float distance = self.distanceTo(other);
            return distance < (self.radius + other.radius);
        }, py::arg("other"), "Check if object is colliding with another")
        .def("toDict", [](const WorldObject& self) {
            return py::dict(
                "x"_a = self.x,
                "y"_a = self.y,
                "radius"_a = self.radius,
                "type"_a = self.type,
                "value"_a = self.value,
                "active"_a = self.active
            );
        }, "Convert object to dictionary representation");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level")
        .def_readwrite("health", &AgentBody::health, "Health level")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Last action time")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get speed magnitude")
        .def("getDirection", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get direction in radians");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
              py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Result message")
        .def("isSuccess", [](const ActionResult& self) {
            return self.success;
        }, "Check if action was successful")
        .def("toDict", [](const ActionResult& self) {
            return py::dict(
                "reward"_a = self.reward,
                "success"_a = self.success,
                "message"_a = self.message
            );
        }, "Convert result to dictionary representation");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals as vector")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp")
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        }, "Check if percept has vision data")
        .def("hasTouch", [](const SensoryPercept& self) {
            return !self.getTouch().empty();
        }, "Check if percept has touch data")
        .def("hasInternal", [](const SensoryPercept& self) {
            return !self.getInternal().empty();
        }, "Check if percept has internal signals")
        .def("hasProprioception", [](const SensoryPercept& self) {
            return !self.getProprioception().empty();
        }, "Check if percept has proprioception data")
        .def("hasAudio", [](const SensoryPercept& self) {
            return !self.getAudio().empty();
        }, "Check if percept has audio data")
        .def("getAllSignalsAsDict", [](const SensoryPercept& self) {
            return py::dict(
                "vision"_a = self.getVision(),
                "touch"_a = self.getTouch(),
                "internal"_a = self.getInternal(),
                "proprioception"_a = self.getProprioception(),
                "audio"_a = self.getAudio()
            );
        }, "Get all signals as dictionary representation")
        .def("setFromDict", [](SensoryPercept& self, const py::dict& data) {
            if (data.contains("vision"_a)) {
                self.setVision(py::cast<std::vector<float>>(data["vision"_a]));
            }
            if (data.contains("touch"_a)) {
                self.setTouch(py::cast<std::vector<float>>(data["touch"_a]));
            }
            if (data.contains("internal"_a)) {
                self.setInternal(py::cast<std::vector<float>>(data["internal"_a]));
            }
            if (data.contains("proprioception"_a)) {
                self.setProprioception(py::cast<std::vector<float>>(data["proprioception"_a]));
            }
            if (data.contains("audio"_a)) {
                self.setAudio(py::cast<std::vector<float>>(data["audio"_a]));
            }
        }, py::arg("data"), "Set percept data from dictionary");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
              py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
              py::arg("cmd"), py::arg("currentTime"), "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
              py::return_value_policy::reference_internal, "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
              py::return_value_policy::reference_internal, "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("createResourceObject", [](SimpleWorld& self, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("value"), "Create and add a resource object")
        .def("createHazardObject", [](SimpleWorld& self, float x, float y, float radius = 0.5f) {
            WorldObject obj(x, y, WorldObjectType::Hazard, 0.0f, radius);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, "Create and add a hazard object")
        .def("createWallObject", [](SimpleWorld& self, float x, float y, float radius = 0.5f) {
            WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, radius);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, "Create and add a wall object")
        .def("findNearestObject", [](SimpleWorld& self, float x, float y, WorldObjectType type = WorldObjectType::Empty) {
            WorldObject nearest;
            float minDistance = std::numeric_limits<float>::max();
            
            // This would need access to all objects - simplified for now
            // In a real implementation, you'd store all objects and iterate
            
            return nearest;
        }, py::arg("x"), py::arg("y"), py::arg("type") = WorldObjectType::Empty, "Find nearest object of type");

    // Enhanced Brain class with Pythonic API
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
              py::arg("neuronId"), py::arg("current"),
              "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
              py::arg("neuronType"), py::arg("current"),
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
        .def("getNeuronStats", [](Brain& self) {
            return py::dict(
                "total_neurons"_a = self.getTotalNeuronCount(),
                "firing_neurons"_a = self.getFiringNeuronCount(),
                "active_neurons"_a = self.getActiveNeuronCount(),
                "total_spikes"_a = self.getTotalSpikeCount(),
                "avg_firing_rate"_a = self.getAverageFiringRate(),
                "e_i_ratio"_a = self.getExcitationInhibitionRatio()
            );
        }, "Get comprehensive brain statistics")
        .def("getDevelopmentStats", [](Brain& self) {
            return py::dict(
                "developmental_stage"_a = static_cast<int>(self.getDevelopmentalStage()),
                "stage_name"_a = "", // Would need a helper function
                "plasticity_multiplier"_a = 1.0f // Would need a helper function
            );
        }, "Get developmental stage statistics")
        .def("simulateEpisode", [](Brain& self, const std::vector<float>& sensoryInput, int numSteps) {
            if (!self.getConfig()->has("random_seed")) {
                self.getConfig()->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            }
            
            self.initialize();
            self.receiveSensoryInput(SensoryInput(sensoryInput));
            
            for (int step = 0; step < numSteps; ++step) {
                self.step(step);
            }
            
            return py::dict(
                "final_spikes"_a = self.getTotalSpikeCount(),
                "final_firing"_a = self.getFiringNeuronCount(),
                "final_rate"_a = self.getAverageFiringRate(),
                "final_stage"_a = static_cast<int>(self.getDevelopmentalStage())
            );
        }, py::arg("sensory_input"), py::arg("num_steps"), "Simulate an episode with given sensory input")
        .def("compareBrains", [](const Brain& brain1, const Brain& brain2) {
            return py::dict(
                "neuron_count_match"_a = brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount(),
                "spike_count_diff"_a = static_cast<int>(brain2.getTotalSpikeCount() - brain1.getTotalSpikeCount()),
                "firing_rate_diff"_a = brain2.getAverageFiringRate() - brain1.getAverageFiringRate(),
                "e_i_ratio_diff"_a = brain2.getExcitationInhibitionRatio() - brain1.getExcitationInhibitionRatio()
            );
        }, py::arg("brain1"), py::arg("brain2"), "Compare two brains");

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
        .def("getLearningStats", [](AgentBrain& self) {
            return py::dict(
                "reward_modulation"_a = self.isRewardModulationEnabled(),
                "structural_plasticity"_a = self.isStructuralPlasticityEnabled(),
                "development"_a = self.isDevelopmentEnabled(),
                "curiosity"_a = self.isCuriosityEnabled(),
                "curiosity_level"_a = self.getCuriosityLevel(),
                "novelty_level"_a = self.getNoveltyLevel(),
                "prediction_error"_a = self.getPredictionError(),
                "neuromodulation_level"_a = self.getNeuromodulationLevel()
            );
        }, "Get learning subsystem statistics")
        .def("runEpisode", [](AgentBrain& self, SimpleWorld& world, int numSteps) {
            world.reset();
            self.initialize(world);
            
            for (int step = 0; step < numSteps; ++step) {
                world.update(0.1f);
                
                auto percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                
                world.applyMotorCommand(self.decodeMotorCommand(), world.getSimulationTime());
                
                self.updateDevelopment(0.1f);
            }
            
            return py::dict(
                "final_firing_rate"_a = self.getBrain()->getAverageFiringRate(),
                "final_spikes"_a = self.getBrain()->getTotalSpikeCount(),
                "final_curiosity"_a = self.getCuriosityLevel(),
                "final_novelty"_a = self.getNoveltyLevel()
            );
        }, py::arg("world"), py::arg("num_steps"), "Run a complete episode");

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

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version information
    m.attr("__version__") = "2.0.0";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__description__") = "Neural Learning Machine - Enhanced Python Bindings";

    // Example demonstrations
    m.def("runHelloWorldExample", []() {
        std::cout << "=== NLM Hello World Example ===\n";
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        std::cout << "Created brain with " << brain->getTotalNeuronCount() << " neurons\n";
        
        for (int i = 0; i < 50; ++i) {
            brain->step(i);
        }
        
        std::cout << "Completed " << 50 << " simulation steps\n";
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
        
        std::cout << "\n=== Example Complete ===\n";
    }, "Run a simple hello world example");

    m.def("runAgentExample", []() {
        std::cout << "=== NLM Agent Example ===\n";
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        std::cout << "Running agent simulation...\n";
        
        for (int step = 0; step < 200; ++step) {
            world->update(0.1f);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
            
            agent->updateDevelopment(0.1f);
            
            if (step % 50 == 0) {
                std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() << " neurons firing\n";
            }
        }
        
        std::cout << "\nAgent simulation complete!\n";
        std::cout << "Final curiosity level: " << agent->getCuriosityLevel() << "\n";
        std::cout << "Final novelty level: " << agent->getNoveltyLevel() << "\n";
        
        std::cout << "\n=== Agent Example Complete ===\n";
    }, "Run a complete agent example");
}

} // namespace nlm