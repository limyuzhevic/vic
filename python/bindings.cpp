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

#include <stdexcept>
#include <chrono>
#include <cmath>

namespace py = pybind11;
namespace nlm {

// Custom Python exception types
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& msg) : std::runtime_error("ConfigError: " + msg) {}
};

class SensoryError : public std::runtime_error {
public:
    explicit SensoryError(const std::string& msg) : std::runtime_error("SensoryError: " + msg) {}
};

class BrainError : public std::runtime_error {
public:
    explicit BrainError(const std::string& msg) : std::runtime_error("BrainError: " + msg) {}
};

class WorldError : public std::runtime_error {
public:
    explicit WorldError(const std::string& msg) : std::runtime_error("WorldError: " + msg) {}
};

class AgentError : public std::runtime_error {
public:
    explicit AgentError(const std::string& msg) : std::runtime_error("AgentError: " + msg) {}
};

class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& msg) : std::runtime_error("ValidationError: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register custom exception types
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<SensoryError>(m, "SensoryError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");
    py::register_exception<AgentError>(m, "AgentError");
    py::register_exception<ValidationError>(m, "ValidationError");

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

        // Enhanced type-safe getters with validation
        .def("getInt", [](const Config& self, const std::string& key, int64_t defaultValue) {
            if (!self.has(key)) return defaultValue;
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get integer value by key, returns default if key doesn't exist")

        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue) {
            if (!self.has(key)) return defaultValue;
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get float value by key, returns default if key doesn't exist")

        .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            if (!self.has(key)) return defaultValue;
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get string value by key, returns default if key doesn't exist")

        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
            if (!self.has(key)) return defaultValue;
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get boolean value by key, returns default if key doesn't exist")

        // Enhanced type-safe setters with validation
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set integer value by key")

        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set float value by key")

        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set string value by key")

        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set boolean value by key")

        // Batch operations
        .def("setMultiple", [](Config& self, const py::dict& settings) {
            for (auto item : settings) {
                std::string key = py::cast<std::string>(item.first);
                if (py::isinstance<py::int_>(item.second)) {
                    self.set(key, py::cast<int64_t>(item.second));
                } else if (py::isinstance<py::float_>(item.second)) {
                    self.set(key, py::cast<float>(item.second));
                } else if (py::isinstance<py::str>(item.second)) {
                    self.set(key, py::cast<std::string>(item.second));
                } else if (py::isinstance<py::bool_>(item.second)) {
                    self.set(key, py::cast<bool>(item.second));
                }
            }
        }, py::arg("settings"),
           "Set multiple configuration values at once")

        .def("merge", [](Config& self, const py::dict& settings) {
            for (auto item : settings) {
                std::string key = py::cast<std::string>(item.first);
                if (py::isinstance<py::int_>(item.second)) {
                    self.set(key, py::cast<int64_t>(item.second));
                } else if (py::isinstance<py::float_>(item.second)) {
                    self.set(key, py::cast<float>(item.second));
                } else if (py::isinstance<py::str>(item.second)) {
                    self.set(key, py::cast<std::string>(item.second));
                } else if (py::isinstance<py::bool_>(item.second)) {
                    self.set(key, py::cast<bool>(item.second));
                }
            }
        }, py::arg("settings"),
           "Merge configuration with another dict (like dict.update)")

        // Convenient getters with type inference
        .def("get", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw py::key_error("Configuration key not found: " + key);
            }
            // Try to get as int first, then float, then string
            try {
                return self.getOr<int64_t>(key, 0);
            } catch (...) {
                try {
                    return self.getOr<float>(key, 0.0f);
                } catch (...) {
                    return self.getOr<std::string>(key, "");
                }
            }
        }, py::arg("key"),
           "Get configuration value by key, auto-detects type (int, float, or string)")

        // Smart getters with type conversion
        .def("getWithType", [](const Config& self, const std::string& key, const std::string& typeHint) {
            if (!self.has(key)) {
                throw py::key_error("Configuration key not found: " + key);
            }
            if (typeHint == "int" || typeHint == "integer") {
                return self.getOr<int64_t>(key, 0);
            } else if (typeHint == "float" || typeHint == "double") {
                return self.getOr<float>(key, 0.0f);
            } else if (typeHint == "bool" || typeHint == "boolean") {
                return self.getOr<bool>(key, false);
            } else {
                return self.getOr<std::string>(key, "");
            }
        }, py::arg("key"), py::arg("typeHint"),
           "Get configuration value with explicit type hint")

        // Convenience methods for common NLM configuration patterns
        .def("configureBrain", [](Config& self, int64_t neuronCount, float connectionProbability) {
            self.set("brain.neuron_count", neuronCount);
            self.set("brain.connection_probability", connectionProbability);
        }, py::arg("neuronCount"), py::arg("connectionProbability"),
           "Configure basic brain parameters")

        .def("configurePlasticity", [](Config& self, bool enableSTDP, bool enableHebbian, bool enableStructural) {
            if (enableSTDP) {
                self.set("plasticity.stdp.enable", true);
                self.set("plasticity.stdp.learning_rate", 0.001f);
            }
            if (enableHebbian) {
                self.set("plasticity.hebbian.enable", true);
                self.set("plasticity.hebbian.learning_rate", 0.01f);
            }
            if (enableStructural) {
                self.set("plasticity.structural.enable", true);
                self.set("plasticity.structural.synaptogenesis_rate", 0.0001f);
                self.set("plasticity.structural.pruning_rate", 0.00001f);
            }
        }, py::arg("enableSTDP"), py::arg("enableHebbian"), py::arg("enableStructural"),
           "Configure plasticity mechanisms")

        .def("configureNeuromodulation", [](Config& self, bool enableCuriosity, bool enableNovelty, bool enableReward) {
            if (enableCuriosity) {
                self.set("neuromod.curiosity.enable", true);
                self.set("neuromod.curiosity.strength", 0.5f);
            }
            if (enableNovelty) {
                self.set("neuromod.novelty.enable", true);
                self.set("neuromod.novelty.threshold", 0.3f);
            }
            if (enableReward) {
                self.set("neuromod.reward.enable", true);
                self.set("neuromod.reward.scale", 1.0f);
            }
        }, py::arg("enableCuriosity"), py::arg("enableNovelty"), py::arg("enableReward"),
           "Configure neuromodulation systems")

        // JSON export/import utilities
        .def("toDict", [](const Config& self) {
            py::dict result;
            for (const auto& key : self.getKeys()) {
                // Try to get value as different types
                try {
                    result[key] = self.getOr<int64_t>(key, 0);
                } catch (...) {
                    try {
                        result[key] = self.getOr<float>(key, 0.0f);
                    } catch (...) {
                        try {
                            result[key] = self.getOr<bool>(key, false);
                        } catch (...) {
                            result[key] = self.getOr<std::string>(key, "");
                        }
                    }
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")

        .def("fromDict", [](Config& self, const py::dict& settings) {
            self.clear();
            for (auto item : settings) {
                std::string key = py::cast<std::string>(item.first);
                if (py::isinstance<py::int_>(item.second)) {
                    self.set(key, py::cast<int64_t>(item.second));
                } else if (py::isinstance<py::float_>(item.second)) {
                    self.set(key, py::cast<float>(item.second));
                } else if (py::isinstance<py::str>(item.second)) {
                    self.set(key, py::cast<std::string>(item.second));
                } else if (py::isinstance<py::bool_>(item.second)) {
                    self.set(key, py::cast<bool>(item.second));
                }
            }
        }, py::arg("settings"),
           "Load configuration from Python dictionary")

        // Validation and error handling
        .def("validate", [](const Config& self, const py::dict& schema) {
            bool valid = true;
            std::string errorMsg;
            
            for (auto item : schema) {
                std::string key = py::cast<std::string>(item.first);
                std::string type = py::cast<std::string>(item.second);
                
                if (!self.has(key)) {
                    errorMsg += "Missing required key: " + key + "; ";
                    valid = false;
                    continue;
                }
                
                // Type checking could be added here
                // For now, just check presence
            }
            
            if (!valid) {
                throw py::value_error("Configuration validation failed: " + errorMsg);
            }
            
            return valid;
        }, py::arg("schema"),
           "Validate configuration against a schema (dict of key-type pairs)")

        .def("validateRequired", [](const Config& self, const std::vector<std::string>& requiredKeys) {
            bool valid = true;
            std::string errorMsg;
            
            for (const auto& key : requiredKeys) {
                if (!self.has(key)) {
                    errorMsg += "Missing required key: " + key + "; ";
                    valid = false;
                }
            }
            
            if (!valid) {
                throw py::value_error("Configuration validation failed: " + errorMsg);
            }
            
            return valid;
        }, py::arg("requiredKeys"),
           "Validate that all required keys are present")

        // Export
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        // Convenience method for easy data access
        .def("getFlattenedData", [](const SensoryInput& self) {
            return self.getData();
        }, "Get all sensory data as a single flattened vector")
        .def("getTotalSize", [](const SensoryInput& self) {
            return self.getDimensions();
        }, "Get total size of all sensory data");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        // Enhanced Vision setters with shape validation
        .def("setDataFrom2D", [](Vision& self, const py::list& data2D) {
            // Convert 2D list to flat vector
            std::vector<float> flatData;
            for (const auto& row : data2D) {
                py::list rowList = py::cast<py::list>(row);
                for (const auto& val : rowList) {
                    flatData.push_back(py::cast<float>(val));
                }
            }
            self.setData(flatData);
        }, py::arg("data2D"), "Set vision data from 2D Python list (rows x columns)")
        .def("setDataFrom3D", [](Vision& self, const py::list& data3D) {
            // Convert 3D list to flat vector
            std::vector<float> flatData;
            for (const auto& plane : data3D) {
                py::list planeList = py::cast<py::list>(plane);
                for (const auto& row : planeList) {
                    py::list rowList = py::cast<py::list>(row);
                    for (const auto& val : rowList) {
                        flatData.push_back(py::cast<float>(val));
                    }
                }
            }
            self.setData(flatData);
        }, py::arg("data3D"), "Set vision data from 3D Python list (planes x rows x columns)")
        // Vision-specific convenience methods
        .def("getPixel", [](const Vision& self, size_t x, size_t y) {
            size_t width = self.getWidth();
            if (x >= width) {
                throw py::index_error("X coordinate out of range");
            }
            if (y >= self.getHeight()) {
                throw py::index_error("Y coordinate out of range");
            }
            return self.getData()[y * width + x];
        }, py::arg("x"), py::arg("y"),
           "Get pixel value at (x, y) coordinates")

        .def("setPixel", [](Vision& self, size_t x, size_t y, float value) {
            size_t width = self.getWidth();
            if (x >= width) {
                throw py::index_error("X coordinate out of range");
            }
            if (y >= self.getHeight()) {
                throw py::index_error("Y coordinate out of range");
            }
            self.getData()[y * width + x] = value;
        }, py::arg("x"), py::arg("y"), py::arg("value"),
           "Set pixel value at (x, y) coordinates")

        .def("getRegion", [](const Vision& self, size_t x1, size_t y1, size_t x2, size_t y2) {
            size_t width = self.getWidth();
            if (x2 <= x1 || y2 <= y1) {
                throw py::value_error("Invalid region coordinates");
            }
            if (x2 > width || y2 > self.getHeight()) {
                throw py::index_error("Region coordinates out of range");
            }
            
            std::vector<float> region;
            for (size_t y = y1; y < y2; ++y) {
                for (size_t x = x1; x < x2; ++x) {
                    region.push_back(self.getData()[y * width + x]);
                }
            }
            return region;
        }, py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"),
           "Extract a rectangular region from vision data")

        .def("normalize", [](Vision& self, float min, float max) {
            std::vector<float>& data = self.getData();
            float currentMin = *std::min_element(data.begin(), data.end());
            float currentMax = *std::max_element(data.begin(), data.end());
            
            if (currentMax == currentMin) {
                for (auto& val : data) {
                    val = 0.5f;
                }
            } else {
                for (auto& val : data) {
                    val = (val - currentMin) / (currentMax - currentMin) * (max - min) + min;
                }
            }
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f,
           "Normalize vision data to specified range [min, max]")

        .def("createFrom2DArray", [](size_t width, size_t height, const py::list& data2D) {
            auto vision = std::make_shared<Vision>(width, height, 1);
            std::vector<float>& visionData = vision->getData();
            
            for (size_t y = 0; y < height; ++y) {
                py::list rowList = py::cast<py::list>(data2D[y]);
                if (rowList.size() != width) {
                    throw py::value_error("Row length doesn't match width");
                }
                for (size_t x = 0; x < width; ++x) {
                    visionData[y * width + x] = py::cast<float>(rowList[x]);
                }
            }
            return vision;
        }, py::arg("width"), py::arg("height"), py::arg("data2D"),
           "Create a Vision object from 2D Python array");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        // Enhanced Audio setters
        .def("setSamplesFromList", [](Audio& self, const py::list& samples) {
            std::vector<float> audioData;
            for (const auto& sample : samples) {
                audioData.push_back(py::cast<float>(sample));
            }
            self.setData(audioData);
        }, py::arg("samples"), "Set audio samples from Python list")
        .def("getSample", [](const Audio& self, size_t index) {
            const std::vector<float>& data = self.getData();
            if (index >= data.size()) {
                throw py::index_error("Sample index out of range");
            }
            return data[index];
        }, py::arg("index"), "Get audio sample at given index")
        .def("setSample", [](Audio& self, size_t index, float value) {
            std::vector<float>& data = self.getData();
            if (index >= data.size()) {
                throw py::index_error("Sample index out of range");
            }
            data[index] = value;
        }, py::arg("index"), py::arg("value"), "Set audio sample at given index")
        .def("normalizeAmplitude", [](Audio& self, float targetPeak = 1.0f) {
            std::vector<float>& data = self.getData();
            if (data.empty()) return;
            
            float currentPeak = 0.0f;
            for (float sample : data) {
                currentPeak = std::max(currentPeak, std::abs(sample));
            }
            
            if (currentPeak > 0.0f) {
                float scale = targetPeak / currentPeak;
                for (auto& sample : data) {
                    sample *= scale;
                }
            }
        }, py::arg("targetPeak") = 1.0f,
           "Normalize audio amplitude to target peak value")

        .def("createFromSamples", [](size_t sampleRate, const py::list& samples) {
            auto audio = std::make_shared<Audio>(sampleRate, samples.size());
            std::vector<float>& audioData = audio->getData();
            
            for (size_t i = 0; i < samples.size(); ++i) {
                audioData[i] = py::cast<float>(samples[i]);
            }
            return audio;
        }, py::arg("sampleRate"), py::arg("samples"),
           "Create an Audio object from Python list of samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals")
        // Enhanced InternalSignals methods
        .def("addSignalsFromList", [](InternalSignals& self, const py::list& values) {
            for (const auto& val : values) {
                self.addSignal(py::cast<float>(val));
            }
        }, py::arg("values"), "Add multiple signal values from Python list")
        .def("getSignalAt", [](const InternalSignals& self, size_t index) {
            const std::vector<float>& signals = self.getInternal();
            if (index >= signals.size()) {
                throw py::index_error("Signal index out of range");
            }
            return signals[index];
        }, py::arg("index"), "Get signal at given index")
        .def("setSignalAt", [](InternalSignals& self, size_t index, float value) {
            std::vector<float>& signals = self.getInternal();
            if (index >= signals.size()) {
                throw py::index_error("Signal index out of range");
            }
            signals[index] = value;
        }, py::arg("index"), py::arg("value"), "Set signal at given index")
        .def("getStats", [](const InternalSignals& self) {
            const std::vector<float>& signals = self.getInternal();
            if (signals.empty()) {
                return py::dict();
            }
            
            float sum = std::accumulate(signals.begin(), signals.end(), 0.0f);
            float mean = sum / signals.size();
            
            float min = *std::min_element(signals.begin(), signals.end());
            float max = *std::max_element(signals.begin(), signals.end());
            
            float variance = 0.0f;
            for (float val : signals) {
                variance += (val - mean) * (val - mean);
            }
            variance /= signals.size();
            float stddev = std::sqrt(variance);
            
            py::dict stats;
            stats["count"] = (int)signals.size();
            stats["mean"] = mean;
            stats["min"] = min;
            stats["max"] = max;
            stats["stddev"] = stddev;
            return stats;
        }, "Get statistics for internal signals");

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
        // Enhanced Action methods
        .def("createAction", [](ActionType type, const py::list& paramsList) {
            std::vector<float> params(paramsList.size());
            for (size_t i = 0; i < paramsList.size(); ++i) {
                params[i] = py::cast<float>(paramsList[i]);
            }
            return std::make_shared<Action>(type, params);
        }, py::arg("type"), py::arg("paramsList"),
           "Create an Action with type and parameters list")

        .def("createMovementAction", [](const std::string& direction, float intensity = 1.0f) {
            ActionType actionType;
            if (direction == "forward" || direction == "move_forward") {
                actionType = ActionType::MoveForward;
            } else if (direction == "backward" || direction == "move_backward") {
                actionType = ActionType::MoveBackward;
            } else if (direction == "left" || direction == "turn_left") {
                actionType = ActionType::TurnLeft;
            } else if (direction == "right" || direction == "turn_right") {
                actionType = ActionType::TurnRight;
            } else {
                actionType = ActionType::Wait;
            }
            return std::make_shared<Action>(actionType, std::vector<float>{intensity});
        }, py::arg("direction"), py::arg("intensity") = 1.0f,
           "Create a movement action from direction string")

        .def("createInteractionAction", [](const std::string& objectType, float value = 1.0f) {
            ActionType actionType;
            if (objectType == "interact") {
                actionType = ActionType::Interact;
            } else if (objectType == "eat") {
                actionType = ActionType::Eat;
            } else if (objectType == "drink") {
                actionType = ActionType::Drink;
            } else if (objectType == "rest") {
                actionType = ActionType::Rest;
            } else {
                actionType = ActionType::Wait;
            }
            return std::make_shared<Action>(actionType, std::vector<float>{value});
        }, py::arg("objectType"), py::arg("value") = 1.0f,
           "Create an interaction action from object type")

        .def("fromDict", [](const py::dict& actionDict) {
            std::string typeStr = py::cast<std::string>(actionDict["type"]);
            ActionType type;
            
            // Map string to ActionType
            if (typeStr == "MoveForward") type = ActionType::MoveForward;
            else if (typeStr == "MoveBackward") type = ActionType::MoveBackward;
            else if (typeStr == "TurnLeft") type = ActionType::TurnLeft;
            else if (typeStr == "TurnRight") type = ActionType::TurnRight;
            else if (typeStr == "Interact") type = ActionType::Interact;
            else if (typeStr == "LookLeft") type = ActionType::LookLeft;
            else if (typeStr == "LookRight") type = ActionType::LookRight;
            else if (typeStr == "Look") type = ActionType::Look;
            else if (typeStr == "LookUp") type = ActionType::LookUp;
            else if (typeStr == "LookDown") type = ActionType::LookDown;
            else if (typeStr == "Eat") type = ActionType::Eat;
            else if (typeStr == "Drink") type = ActionType::Drink;
            else if (typeStr == "Rest") type = ActionType::Rest;
            else if (typeStr == "Wait") type = ActionType::Wait;
            else if (typeStr == "Custom") type = ActionType::Custom;
            else {
                throw py::value_error("Unknown action type: " + typeStr);
            }
            
            std::vector<float> params;
            if (actionDict.contains("parameters")) {
                py::list paramsList = py::cast<py::list>(actionDict["parameters"]);
                for (const auto& param : paramsList) {
                    params.push_back(py::cast<float>(param));
                }
            }
            
            return std::make_shared<Action>(type, params);
        }, py::arg("actionDict"), "Create an Action from Python dictionary")

        .def("toDict", [](const Action& self) {
            py::dict actionDict;
            
            // Convert ActionType to string
            const char* typeStr;
            switch (self.getType()) {
                case ActionType::MoveForward: typeStr = "MoveForward"; break;
                case ActionType::MoveBackward: typeStr = "MoveBackward"; break;
                case ActionType::TurnLeft: typeStr = "TurnLeft"; break;
                case ActionType::TurnRight: typeStr = "TurnRight"; break;
                case ActionType::LookLeft: typeStr = "LookLeft"; break;
                case ActionType::LookRight: typeStr = "LookRight"; break;
                case ActionType::Look: typeStr = "Look"; break;
                case ActionType::LookUp: typeStr = "LookUp"; break;
                case ActionType::LookDown: typeStr = "LookDown"; break;
                case ActionType::Interact: typeStr = "Interact"; break;
                case ActionType::Eat: typeStr = "Eat"; break;
                case ActionType::Drink: typeStr = "Drink"; break;
                case ActionType::Rest: typeStr = "Rest"; break;
                case ActionType::Wait: typeStr = "Wait"; break;
                case ActionType::Custom: typeStr = "Custom"; break;
                default: typeStr = "Unknown";
            }
            
            actionDict["type"] = typeStr;
            actionDict["parameters"] = py::list();
            
            const std::vector<float>& params = self.getParameters();
            for (float param : params) {
                py::cast<py::list>(actionDict["parameters"]).append(param);
            }
            
            actionDict["name"] = self.getName();
            
            return actionDict;
        }, "Convert Action to Python dictionary")

        .def("isMovement", [](const Action& self) {
            ActionType type = self.getType();
            return type == ActionType::MoveForward || type == ActionType::MoveBackward ||
                   type == ActionType::TurnLeft || type == ActionType::TurnRight;
        }, "Check if action is a movement action")

        .def("isInteraction", [](const Action& self) {
            ActionType type = self.getType();
            return type == ActionType::Interact || type == ActionType::Eat ||
                   type == ActionType::Drink || type == ActionType::Rest;
        }, "Check if action is an interaction action");
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
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

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
             "Log brain status");

        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Enhanced Brain statistics and monitoring methods
    .def("getNeuralConnectivityStats", [](const Brain& self) {
        auto* region = self.getRegion(RegionId(1));
        if (!region) {
            throw py::value_error("No regions found in brain");
        }
        
        py::dict stats;
        stats["totalNeurons"] = self.getTotalNeuronCount();
        stats["totalSynapses"] = self.getTotalSynapseCount();
        stats["activeNeurons"] = self.getActiveNeuronCount();
        stats["firingNeurons"] = self.getFiringNeuronCount();
        stats["averageFiringRate"] = self.getAverageFiringRate();
        stats["excitationInhibitionRatio"] = self.getExcitationInhibitionRatio();
        stats["totalSpikes"] = self.getTotalSpikeCount();
        
        // Calculate connectivity metrics
        float totalPossibleConnections = static_cast<float>(self.getTotalNeuronCount()) * 
                                         static_cast<float>(self.getTotalNeuronCount()) - 1;
        float actualConnections = static_cast<float>(self.getTotalSynapseCount());
        float connectionDensity = totalPossibleConnections > 0 ? 
                                  actualConnections / totalPossibleConnections : 0.0f;
        stats["connectionDensity"] = connectionDensity;
        
        // Calculate regional statistics
        auto regionIds = self.getRegionIds();
        if (!regionIds.empty()) {
            auto* region = self.getRegion(regionIds[0]);
            if (region) {
                size_t regionNeurons = region->getTotalNeuronCount();
                size_t regionSynapses = region->getTotalSynapseCount();
                stats["regionNeuronCount"] = regionNeurons;
                stats["regionSynapseCount"] = regionSynapses;
                stats["regionDensity"] = regionNeurons > 0 ? 
                                       static_cast<float>(regionSynapses) / regionNeurons : 0.0f;
            }
        }
        
        return stats;
    }, "Get detailed neural connectivity statistics");

    .def("getMemoryStats", [](const Brain& self) {
        py::dict stats;
        
        // Estimate memory usage (rough approximation)
        float neuronMemory = static_cast<float>(self.getTotalNeuronCount()) * 100.0f; // ~100 bytes per neuron
        float synapseMemory = static_cast<float>(self.getTotalSynapseCount()) * 50.0f;  // ~50 bytes per synapse
        float configMemory = 1000.0f; // ~1KB for config
        
        stats["totalNeurons"] = self.getTotalNeuronCount();
        stats["neuronMemoryKB"] = neuronMemory / 1024.0f;
        stats["totalSynapses"] = self.getTotalSynapseCount();
        stats["synapseMemoryKB"] = synapseMemory / 1024.0f;
        stats["configMemoryKB"] = configMemory / 1024.0f;
        stats["estimatedTotalMemoryMB"] = (neuronMemory + synapseMemory + configMemory) / (1024.0f * 1024.0f);
        
        // Memory efficiency metrics
        float memoryPerNeuron = static_cast<float>(self.getTotalNeuronCount()) > 0 ? 
                                (neuronMemory + synapseMemory) / self.getTotalNeuronCount() : 0.0f;
        stats["memoryPerNeuronKB"] = memoryPerNeuron / 1024.0f;
        
        return stats;
    }, "Get memory usage statistics");

    .def("getPerformanceStats", [](const Brain& self) {
        py::dict stats;
        
        // Calculate performance metrics based on current state
        float firingRate = self.getAverageFiringRate();
        float activeRatio = static_cast<float>(self.getActiveNeuronCount()) / 
                           std::max(static_cast<size_t>(1), self.getTotalNeuronCount());
        float firingRatio = static_cast<float>(self.getFiringNeuronCount()) / 
                           std::max(static_cast<size_t>(1), self.getTotalNeuronCount());
        
        stats["averageFiringRateHz"] = firingRate;
        stats["activeNeuronRatio"] = activeRatio;
        stats["firingNeuronRatio"] = firingRatio;
        stats["excitatoryInhibitionBalance"] = self.getExcitationInhibitionRatio();
        
        // Performance assessment
        float performanceScore = 0.0f;
        if (firingRate > 0.1f && firingRate < 100.0f) performanceScore += 25.0f;
        if (activeRatio > 0.1f && activeRatio < 0.9f) performanceScore += 25.0f;
        if (firingRatio > 0.01f && firingRatio < 0.5f) performanceScore += 25.0f;
        if (self.getExcitationInhibitionRatio() > 0.5f && 
            self.getExcitationInhibitionRatio() < 2.0f) performanceScore += 25.0f;
        
        stats["performanceScore"] = performanceScore;
        stats["performanceStatus"] = performanceScore > 50.0f ? "good" : 
                                     (performanceScore > 25.0f ? "fair" : "poor");
        
        return stats;
    }, "Get performance metrics and assessment");

    .def("getDetailedStats", [](const Brain& self) {
        py::dict stats;
        
        // Basic brain metrics
        stats["totalNeurons"] = self.getTotalNeuronCount();
        stats["totalSynapses"] = self.getTotalSynapseCount();
        stats["activeNeurons"] = self.getActiveNeuronCount();
        stats["firingNeurons"] = self.getFiringNeuronCount();
        stats["averageFiringRate"] = self.getAverageFiringRate();
        stats["excitationInhibitionRatio"] = self.getExcitationInhibitionRatio();
        stats["totalSpikeCount"] = self.getTotalSpikeCount();
        stats["developmentalStage"] = static_cast<int>(self.getDevelopmentalStage());
        
        // Calculate derived metrics
        float totalActivity = static_cast<float>(self.getActiveNeuronCount() + 
                                                 self.getFiringNeuronCount());
        stats["activityLevel"] = totalActivity / static_cast<float>(self.getTotalNeuronCount());
        
        float synapticStrength = self.getTotalSynapseCount() > 0 ? 
                                self.getAverageFiringRate() / static_cast<float>(self.getTotalSynapseCount()) : 
                                0.0f;
        stats["synapticStrength"] = synapticStrength;
        
        // Get configuration summary
        auto config = self.getConfig();
        if (config) {
            stats["configKeys"] = py::list();
            for (const auto& key : config->getKeys()) {
                py::cast<py::list>(stats["configKeys"]).append(key);
            }
            
            // Extract key config values if available
            if (config->has("brain.neuron_count")) {
                stats["configuredNeuronCount"] = config->getOr<int64_t>("brain.neuron_count", 0);
            }
            if (config->has("brain.connection_probability")) {
                stats["configuredConnectionProbability"] = config->getOr<float>("brain.connection_probability", 0.0f);
            }
        }
        
        return stats;
    }, "Get comprehensive brain statistics");

    .def("getHealthScore", [](const Brain& self) {
        float score = 0.0f;
        
        // Firing rate health (ideal: 0.1-10 Hz)
        float firingRate = self.getAverageFiringRate();
        if (firingRate >= 0.1f && firingRate <= 10.0f) score += 25.0f;
        else if (firingRate > 0.0f && firingRate < 50.0f) score += 12.5f;
        
        // Activity level health
        float activeRatio = static_cast<float>(self.getActiveNeuronCount()) / 
                           std::max(static_cast<size_t>(1), self.getTotalNeuronCount());
        if (activeRatio >= 0.1f && activeRatio <= 0.5f) score += 25.0f;
        else if (activeRatio > 0.0f && activeRatio < 0.8f) score += 12.5f;
        
        // Excitatory/inhibitory balance
        float eiRatio = self.getExcitationInhibitionRatio();
        if (eiRatio >= 0.5f && eiRatio <= 2.0f) score += 25.0f;
        else if (eiRatio > 0.1f && eiRatio < 5.0f) score += 12.5f;
        
        // Connectivity
        float connectivity = static_cast<float>(self.getTotalSynapseCount()) / 
                             std::max(static_cast<size_t>(1), self.getTotalNeuronCount());
        if (connectivity >= 0.01f && connectivity <= 0.5f) score += 25.0f;
        
        return score;
    }, "Get overall brain health score (0-100)");

    .def("getStatusReport", [](const Brain& self) {
        py::dict report;
        
        report["timestamp"] = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
        report["status"] = "healthy";
        report["score"] = self.getHealthScore();
        report["details"] = py::dict();
        
        // Add key metrics to details
        auto details = py::cast<py::dict>(report["details"]);
        details["neurons"] = self.getTotalNeuronCount();
        details["synapses"] = self.getTotalSynapseCount();
        details["firing_rate"] = self.getAverageFiringRate();
        details["active_ratio"] = static_cast<float>(self.getActiveNeuronCount()) / 
                                  std::max(static_cast<size_t>(1), self.getTotalNeuronCount());
        details["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
        
        return report;
    }, "Get comprehensive status report");

    // Enhanced AgentBrain statistics and monitoring methods
    .def("getCognitiveStats", [](const AgentBrain& self) {
        py::dict stats;
        
        // Cognitive metrics
        stats["curiosity_level"] = self.getCuriosityLevel();
        stats["novelty_level"] = self.getNoveltyLevel();
        stats["prediction_error"] = self.getPredictionError();
        stats["neuromodulation_level"] = self.getNeuromodulationLevel();
        
        // Cognitive assessment
        float cognitiveScore = 0.0f;
        if (self.getCuriosityLevel() >= 0.1f && self.getCuriosityLevel() <= 0.8f) {
            cognitiveScore += 25.0f;
        }
        if (self.getNoveltyLevel() >= 0.0f && self.getNoveltyLevel() <= 0.5f) {
            cognitiveScore += 25.0f;
        }
        if (std::abs(self.getPredictionError()) < 0.5f) {
            cognitiveScore += 25.0f;
        }
        if (self.getNeuromodulationLevel() >= 0.0f && self.getNeuromodulationLevel() <= 1.0f) {
            cognitiveScore += 25.0f;
        }
        
        stats["cognitive_score"] = cognitiveScore;
        stats["cognitive_status"] = cognitiveScore > 50.0f ? "good" : 
                                    (cognitiveScore > 25.0f ? "fair" : "poor");
        
        return stats;
    }, "Get cognitive performance statistics");

    .def("getMotivationalStats", [](const AgentBrain& self) {
        py::dict stats;
        
        // Motivational metrics
        stats["curiosity_level"] = self.getCuriosityLevel();
        stats["novelty_level"] = self.getNoveltyLevel();
        stats["neuromodulation_level"] = self.getNeuromodulationLevel();
        
        // Motivational assessment
        float motivationLevel = (self.getCuriosityLevel() + self.getNoveltyLevel() + 
                                std::abs(self.getNeuromodulationLevel())) / 3.0f;
        stats["motivation_level"] = motivationLevel;
        
        if (motivationLevel >= 0.3f && motivationLevel <= 0.7f) {
            stats["motivation_status"] = "balanced";
        } else if (motivationLevel > 0.7f) {
            stats["motivation_status"] = "high";
        } else {
            stats["motivation_status"] = "low";
        }
        
        return stats;
    }, "Get motivational and drive statistics");

    .def("getLearningStats", [](const AgentBrain& self) {
        py::dict stats;
        
        // Learning metrics
        stats["curiosity_level"] = self.getCuriosityLevel();
        stats["prediction_error"] = self.getPredictionError();
        
        // Learning assessment
        float learningRate = self.getCuriosityLevel() * (1.0f - std::abs(self.getPredictionError()));
        stats["learning_rate"] = learningRate;
        
        if (learningRate >= 0.1f && learningRate <= 0.5f) {
            stats["learning_status"] = "steady";
        } else if (learningRate > 0.5f) {
            stats["learning_status"] = "rapid";
        } else {
            stats["learning_status"] = "slow";
        }
        
        return stats;
    }, "Get learning progress statistics");

    .def("getBehavioralStats", [](const AgentBrain& self) {
        py::dict stats;
        
        // Get underlying brain for behavioral analysis
        auto brain = self.getBrain();
        if (!brain) {
            throw py::value_error("AgentBrain has no associated brain");
        }
        
        // Behavioral metrics
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["firing_neutrons"] = brain->getFiringNeuronCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_rate"] = brain->getAverageFiringRate();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        // Behavioral assessment
        float activityLevel = static_cast<float>(brain->getFiringNeuronCount()) / 
                             std::max(static_cast<size_t>(1), brain->getTotalNeuronCount());
        stats["activity_level"] = activityLevel;
        
        if (activityLevel >= 0.01f && activityLevel <= 0.1f) {
            stats["behavioral_status"] = "normal";
        } else if (activityLevel > 0.1f && activityLevel <= 0.3f) {
            stats["behavioral_status"] = "active";
        } else if (activityLevel > 0.3f) {
            stats["behavioral_status"] = "high";
        } else {
            stats["behavioral_status"] = "low";
        }
        
        return stats;
    }, "Get behavioral pattern statistics");

    .def("getEpisodeStats", [](const AgentBrain& self) {
        py::dict stats;
        
        // Episode-level metrics
        stats["curiosity_level"] = self.getCuriosityLevel();
        stats["novelty_level"] = self.getNoveltyLevel();
        stats["neuromodulation_level"] = self.getNeuromodulationLevel();
        stats["prediction_error"] = self.getPredictionError();
        
        // Episode assessment
        float episodeHealth = (self.getCuriosityLevel() + 
                             (1.0f - std::abs(self.getPredictionError())) * 0.5f +
                             (1.0f - std::abs(self.getNeuromodulationLevel()))) / 2.5f;
        stats["episode_health"] = episodeHealth;
        
        if (episodeHealth >= 0.7f) {
            stats["episode_status"] = "healthy";
        } else if (episodeHealth >= 0.4f) {
            stats["episode_status"] = "fair";
        } else {
            stats["episode_status"] = "needs_attention";
        }
        
        return stats;
    }, "Get episode-level statistics");

    .def("getAllStats", [](const AgentBrain& self) {
        py::dict allStats;
        
        // Combine all statistics
        allStats["cognitive"] = self.getCognitiveStats();
        allStats["motivational"] = self.getMotivationalStats();
        allStats["learning"] = self.getLearningStats();
        allStats["behavioral"] = self.getBehavioralStats();
        allStats["episode"] = self.getEpisodeStats();
        
        // Calculate overall health
        float overallHealth = (
            py::cast<float>(allStats["cognitive"]["cognitive_score"]) +
            py::cast<float>(allStats["motivational"]["motivation_level"]) * 100.0f +
            py::cast<float>(allStats["learning"]["learning_rate"]) * 100.0f +
            py::cast<float>(allStats["behavioral"]["activity_level"]) * 100.0f +
            py::cast<float>(allStats["episode"]["episode_health"]) * 100.0f
        ) / 5.0f;
        
        allStats["overall_health"] = overallHealth;
        allStats["overall_status"] = overallHealth > 75.0f ? "excellent" :
                                     (overallHealth > 50.0f ? "good" :
                                      (overallHealth > 25.0f ? "fair" : "needs_improvement"));
        
        return allStats;
    }, "Get comprehensive statistics for all subsystems");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a new configuration (alias for createDefaultConfig)");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createBrainWithDefaultConfig", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration (convenience function)");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new world (alias for createSimpleWorld)");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createBrainAndWorld", []() {
        auto brain = std::make_shared<Brain>(std::make_shared<Config>());
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        return std::make_tuple(brain, world, agent);
    }, "Create a complete agent system (brain, world, agent) - returns tuple");

    m.def("createAgentWithDefaultConfig", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        return std::make_tuple(brain, world, agent);
    }, "Create a complete agent system with default config");

    // Platform detection
    m.attr("PLATFORM_LINUX") = py::bool_(sizeof(void*) == 8);
    m.attr("PLATFORM_WINDOWS") = py::bool_(false);  // Simplified
    m.attr("PLATFORM_MACOS") = py::bool_(false);     // Simplified

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Utility functions that don't fit in classes
    m.def("loadConfigFromFile", &nlm::loadConfigFromFile,
          py::arg("filepath"), "Load configuration from a file");

    m.def("saveConfigToFile", [](std::shared_ptr<Config> config, const std::string& filepath) {
        if (!config) {
            throw std::runtime_error("Config is null");
        }
        if (!config->saveToFile(filepath)) {
            throw std::runtime_error("Failed to save configuration to: " + filepath);
        }
    }, py::arg("config"), py::arg("filepath"), "Save configuration to a file");

    // Simulation utilities
    py::class_<nlm::SimulationResult>(m, "SimulationResult", "Result of a simulation run")
        .def_readonly("duration", &nlm::SimulationResult::duration)
        .def_readonly("totalSteps", &nlm::SimulationResult::totalSteps)
        .def_readonly("maxFiringNeurons", &nlm::SimulationResult::maxFiringNeurons)
        .def_readonly("totalSpikes", &nlm::SimulationResult::totalSpikes)
        .def_readonly("avgFiringRate", &nlm::SimulationResult::avgFiringRate)
        .def_readonly("firingHistory", &nlm::SimulationResult::firingHistory);

    m.def("runSimulation", &nlm::runSimulation,
          py::arg("brain"), py::arg("timestep"), py::arg("numSteps"),
          py::arg("logProgress") = false,
          "Run a brain simulation and return results");

    // Agent utilities
    py::class_<nlm::AgentEpisode>(m, "AgentEpisode", "Agent simulation episode")
        .def_readonly("brain", &nlm::AgentEpisode::brain)
        .def_readonly("world", &nlm::AgentEpisode::world)
        .def_readonly("agent", &nlm::AgentEpisode::agent)
        .def_readonly("visionHistory", &nlm::AgentEpisode::visionHistory)
        .def_readonly("rewardHistory", &nlm::AgentEpisode::rewardHistory)
        .def_readonly("actionHistory", &nlm::AgentEpisode::actionHistory);

    m.def("createAgentEpisode", &nlm::createAgentEpisode,
          py::arg("worldWidth") = 20, py::arg("worldHeight") = 20,
          py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
          "Create a complete agent episode (brain, world, agent)");

    // Comparison utilities
    m.def("brainEquals", &nlm::brainEquals,
          py::arg("brainA"), py::arg("brainB"),
          "Compare two brain objects for equality");

    // Statistics and monitoring
    py::class_<nlm::BrainStatistics>(m, "BrainStatistics", "Statistics about a brain")
        .def_readonly("totalNeurons", &nlm::BrainStatistics::totalNeurons)
        .def_readonly("totalSynapses", &nlm::BrainStatistics::totalSynapses)
        .def_readonly("activeNeurons", &nlm::BrainStatistics::activeNeurons)
        .def_readonly("firingNeurons", &nlm::BrainStatistics::firingNeurons)
        .def_readonly("averageFiringRate", &nlm::BrainStatistics::averageFiringRate)
        .def_readonly("excitationInhibitionRatio", &nlm::BrainStatistics::excitationInhibitionRatio)
        .def_readonly("memoryUsage", &nlm::BrainStatistics::memoryUsage)
        .def_readonly("lastUpdateTime", &nlm::BrainStatistics::lastUpdateTime);

    // Additional Python utilities
    namespace py = pybind11;
    namespace nlm {

        // Complete agent simulation utilities
        class CompleteAgentSetup {
        public:
            std::shared_ptr<Brain> brain;
            std::shared_ptr<SimpleWorld> world;
            std::shared_ptr<AgentBrain> agent;
            
            CompleteAgentSetup(std::shared_ptr<Brain> b, std::shared_ptr<SimpleWorld> w, std::shared_ptr<AgentBrain> a)
                : brain(b), world(w), agent(a) {}
        };

        CompleteAgentSetup createAgentSetup(size_t worldWidth = 20, size_t worldHeight = 20,
                                            size_t visionWidth = 8, size_t visionHeight = 8,
                                            size_t neuronCount = 500) {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
            config->set("brain.connection_probability", 0.05f);
            
            auto brain = std::make_shared<Brain>(config);
            auto world = std::make_shared<SimpleWorld>();
            auto agent = std::make_shared<AgentBrain>(brain);
            
            world->configure(worldWidth, worldHeight, visionWidth, visionHeight);
            world->reset();
            world->setAgentStart(worldWidth / 2.0f, worldHeight / 2.0f);
            agent->initialize(*world);
            
            return CompleteAgentSetup(brain, world, agent);
        }

        CompleteAgentSetup createAgentSetupForResearch(size_t worldWidth = 10, size_t worldHeight = 10,
                                                       size_t visionWidth = 4, size_t visionHeight = 4,
                                                       size_t neuronCount = 200) {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
            config->set("brain.connection_probability", 0.02f);
            
            // Research configuration
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", false);
            
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.reward.enable", true);
            
            auto brain = std::make_shared<Brain>(config);
            auto world = std::make_shared<SimpleWorld>();
            auto agent = std::make_shared<AgentBrain>(brain);
            
            world->configure(worldWidth, worldHeight, visionWidth, visionHeight);
            world->reset();
            world->setAgentStart(worldWidth / 2.0f, worldHeight / 2.0f);
            agent->initialize(*world);
            
            // Enable research features
            agent->enableCuriosity(true);
            agent->enableRewardModulation(true);
            
            return CompleteAgentSetup(brain, world, agent);
        }

        CompleteAgentSetup createAgentSetupForDevelopment(size_t worldWidth = 30, size_t worldHeight = 30,
                                                          size_t visionWidth = 12, size_t visionHeight = 12,
                                                          size_t neuronCount = 1000) {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
            config->set("brain.connection_probability", 0.1f);
            
            // Development configuration
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.reward.enable", true);
            
            config->set("development.enabled", true);
            config->set("development.stages", 5);
            
            auto brain = std::make_shared<Brain>(config);
            auto world = std::make_shared<SimpleWorld>();
            auto agent = std::make_shared<AgentBrain>(brain);
            
            world->configure(worldWidth, worldHeight, visionWidth, visionHeight);
            world->reset();
            world->setAgentStart(worldWidth / 2.0f, worldHeight / 2.0f);
            agent->initialize(*world);
            
            // Enable development features
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
            agent->enableStructuralPlasticity(true);
            agent->enableRewardModulation(true);
            
            return CompleteAgentSetup(brain, world, agent);
        }

        // Configuration presets
        std::shared_ptr<Config> createResearchConfig(size_t neuronCount = 500) {
            auto config = std::make_shared<Config>();
            
            config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
            config->set("brain.connection_probability", 0.02f);
            
            // Research configuration
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.001f);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.hebbian.learning_rate", 0.01f);
            config->set("plasticity.structural.enable", false);
            
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.curiosity.strength", 0.5f);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.novelty.sensitivity", 0.5f);
            config->set("neuromod.reward.enable", true);
            config->set("neuromod.reward.scale", 1.0f);
            
            return config;
        }

        std::shared_ptr<Config> createDevelopmentConfig(size_t neuronCount = 1000) {
            auto config = std::make_shared<Config>();
            
            config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
            config->set("brain.connection_probability", 0.1f);
            
            // Development configuration
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.002f);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.hebbian.learning_rate", 0.02f);
            config->set("plasticity.structural.enable", true);
            config->set("plasticity.structural.synaptogenesis_rate", 0.0001f);
            config->set("plasticity.structural.pruning_rate", 0.00001f);
            
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.curiosity.strength", 0.8f);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.novelty.sensitivity", 0.7f);
            config->set("neuromod.reward.enable", true);
            config->set("neuromod.reward.scale", 1.5f);
            
            config->set("development.enabled", true);
            config->set("development.stages", 5);
            
            return config;
        }

        std::shared_ptr<Config> createProductionConfig(size_t neuronCount = 2000) {
            auto config = std::make_shared<Config>();
            
            config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
            config->set("brain.connection_probability", 0.05f);
            
            // Production configuration - balanced for stability
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.001f);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.hebbian.learning_rate", 0.005f);
            config->set("plasticity.structural.enable", true);
            config->set("plasticity.structural.synaptogenesis_rate", 0.00005f);
            config->set("plasticity.structural.pruning_rate", 0.000005f);
            
            config->set("neuromod.curiosity.enable", false);  // Reduced for stability
            config->set("neuromod.novelty.enable", false);   // Reduced for stability
            config->set("neuromod.reward.enable", true);
            config->set("neuromod.reward.scale", 0.8f);
            
            config->set("development.enabled", false);  // Mature system
            
            return config;
        }

        // Data input utilities
        py::dict loadVisionFromFile(const std::string& filepath) {
            py::dict result;
            
            std::ifstream file(filepath);
            if (!file.is_open()) {
                throw SensoryError("Failed to open vision file: " + filepath);
            }
            
            // Parse simple vision data format (width height then pixel values)
            size_t width, height;
            file >> width >> height;
            
            if (width == 0 || height == 0) {
                throw SensoryError("Invalid vision dimensions: " + std::to_string(width) + "x" + std::to_string(height));
            }
            
            std::vector<float> visionData(width * height, 0.0f);
            
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    float value;
                    file >> value;
                    if (file.fail()) {
                        throw SensoryError("Failed to read vision data at position (" + std::to_string(x) + ", " + std::to_string(y) + ")");
                    }
                    // Clamp value to [0, 1] range
                    value = std::max(0.0f, std::min(1.0f, value));
                    visionData[y * width + x] = value;
                }
            }
            
            result["data"] = visionData;
            result["width"] = static_cast<int>(width);
            result["height"] = static_cast<int>(height);
            
            return result;
        }

        std::vector<float> loadAudioFromFile(const std::string& filepath) {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                throw SensoryError("Failed to open audio file: " + filepath);
            }
            
            std::vector<float> samples;
            float sample;
            
            while (file >> sample) {
                samples.push_back(sample);
            }
            
            if (file.fail() && !file.eof()) {
                throw SensoryError("Failed to read audio file: " + filepath);
            }
            
            return samples;
        }

        // Logging and monitoring helpers
        class SimulationLogger {
        public:
            std::vector<std::string> logMessages;
            size_t maxLogMessages;
            
            SimulationLogger(size_t max = 1000) : maxLogMessages(max) {}
            
            void log(const std::string& message) {
                if (logMessages.size() >= maxLogMessages) {
                    logMessages.erase(logMessages.begin());
                }
                logMessages.push_back(message);
            }
            
            std::vector<std::string> getLog() const {
                return logMessages;
            }
            
            void clear() {
                logMessages.clear();
            }
            
            size_t size() const {
                return logMessages.size();
            }
        };

        SimulationLogger createLogger(size_t maxMessages = 1000) {
            return SimulationLogger(maxMessages);
        }

        // Persistence helpers
        std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> 
        loadAgentFromCheckpoint(const std::string& checkpointFile) {
            // Load brain from checkpoint
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            
            // Load brain state from file
            if (!brain->load(checkpointFile)) {
                throw WorldError("Failed to load agent from checkpoint: " + checkpointFile);
            }
            
            // Create world and agent
            auto world = std::make_shared<SimpleWorld>();
            auto agent = std::make_shared<AgentBrain>(brain);
            
            return std::make_tuple(brain, world, agent);
        }

        bool saveAgentToCheckpoint(const std::shared_ptr<Brain>& brain, 
                                   const std::shared_ptr<SimpleWorld>& world,
                                   const std::shared_ptr<AgentBrain>& agent,
                                   const std::string& checkpointFile,
                                   const std::string& description = "") {
            if (!brain || !world || !agent) {
                throw ValidationError("Brain, world, and agent must all be provided");
            }
            
            // Save brain state
            if (!brain->save(checkpointFile)) {
                throw WorldError("Failed to save agent to checkpoint: " + checkpointFile);
            }
            
            // Save description to a separate file
            if (!description.empty()) {
                std::ofstream descFile(checkpointFile + ".desc");
                if (descFile.is_open()) {
                    descFile << description;
                    descFile.close();
                }
            }
            
            return true;
        }

        // Quick simulation helpers
        std::vector<std::vector<float>> runQuickSimulation(size_t steps = 100, 
                                                          size_t neuronCount = 100,
                                                          size_t worldWidth = 20,
                                                          size_t worldHeight = 20) {
            // Create and run a quick simulation
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
            config->set("brain.connection_probability", 0.05f);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            std::vector<std::vector<float>> firingHistory;
            
            for (size_t step = 0; step < steps; ++step) {
                brain->step(step);
                
                // Collect firing statistics
                std::vector<float> stepStats;
                stepStats.push_back(static_cast<float>(brain->getTotalSpikeCount()));
                stepStats.push_back(static_cast<float>(brain->getFiringNeuronCount()));
                stepStats.push_back(brain->getAverageFiringRate());
                
                firingHistory.push_back(stepStats);
            }
            
            return firingHistory;
        }

        py::dict getSimulationSummary(const std::vector<std::vector<float>>& firingHistory) {
            py::dict summary;
            
            if (firingHistory.empty()) {
                throw ValidationError("No simulation data provided");
            }
            
            size_t steps = firingHistory.size();
            float totalSpikes = 0.0f;
            float maxFiring = 0.0f;
            float avgFiring = 0.0f;
            
            for (const auto& step : firingHistory) {
                if (step.size() >= 3) {
                    totalSpikes += step[0];
                    if (step[1] > maxFiring) maxFiring = step[1];
                    avgFiring += step[2];
                }
            }
            
            if (steps > 0) {
                avgFiring /= steps;
            }
            
            summary["total_steps"] = static_cast<int>(steps);
            summary["total_spikes"] = totalSpikes;
            summary["max_firing_neurons"] = maxFiring;
            summary["average_firing_rate"] = avgFiring;
            summary["average_spikes_per_step"] = totalSpikes / steps;
            
            return summary;
        }

        // Environment and setup helpers
        py::dict getSystemInfo() {
            py::dict info;
            
            // Basic system information
            info["platform"] = std::string(sizeof(void*) == 8 ? "64-bit" : "32-bit");
            info["python_version"] = std::to_string(PY_MAJOR_VERSION) + "." + 
                                   std::to_string(PY_MINOR_VERSION) + "." + 
                                   std::to_string(PY_MICRO_VERSION);
            info["nlm_version"] = "0.1.0";
            
            // Memory information
            info["estimated_max_neurons"] = 10000;
            info["estimated_max_synapses"] = 50000;
            info["recommended_min_memory_mb"] = 512;
            
            return info;
        }

        std::string getQuickStartGuide() {
            return R"(
Quick Start Guide for NLM Python Bindings
==========================================

1. Basic Usage:
   import pynlm
   brain = pynlm.createBrain(pynlm.createDefaultConfig())
   brain.initialize()
   for i in range(100):
       brain.step(i)

2. Complete Agent Setup:
   import pynlm
   setup = pynlm.createAgentSetup()
   brain, world, agent = setup.brain, setup.world, setup.agent

3. Configuration Presets:
   import pynlm
   config = pynlm.createResearchConfig(500)  # For research
   config = pynlm.createDevelopmentConfig(1000)  # For development
   config = pynlm.createProductionConfig(2000)  # For production

4. Running Simulations:
   import pynlm
   history = pynlm.runQuickSimulation(50, 200)
   summary = pynlm.getSimulationSummary(history)

5. Error Handling:
   Use try/except blocks with pynlm.ConfigError, pynlm.BrainError, etc.

6. Logging:
   logger = pynlm.createLogger(1000)
   logger.log("Simulation started")
   print(logger.getLog())

Enjoy using NLM!
            )";
        }

    } // namespace nlm

    // Configuration utilities
    std::shared_ptr<Config> loadConfigFromFile(const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load configuration from: " + filepath);
        }
        return config;
    }

    void saveConfigToFile(std::shared_ptr<Config> config, const std::string& filepath) {
        if (!config) {
            throw std::runtime_error("Config is null");
        }
        if (!config->saveToFile(filepath)) {
            throw std::runtime_error("Failed to save configuration to: " + filepath);
        }
    }

    // Simulation utilities
    class SimulationResult {
    public:
        double duration;
        size_t totalSteps;
        size_t maxFiringNeurons;
        size_t totalSpikes;
        float avgFiringRate;
        std::vector<float> firingHistory;
        
        SimulationResult(double dur, size_t steps, size_t maxFire, 
                        size_t totalSp, float avgRate, std::vector<float> history)
            : duration(dur), totalSteps(steps), maxFiringNeurons(maxFire),
              totalSpikes(totalSp), avgFiringRate(avgRate), firingHistory(history) {}
    };

    SimulationResult runSimulation(std::shared_ptr<Brain> brain, 
                                   double timestep, 
                                   size_t numSteps,
                                   bool logProgress = false) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        brain->initialize();
        std::vector<float> firingHistory;
        size_t maxFiringNeurons = 0;
        size_t totalSpikes = 0;
        
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step, step * timestep);
            
            if (logProgress && step % 100 == 0) {
                firingHistory.push_back(brain->getFiringNeuronCount());
            }
            
            maxFiringNeurons = std::max(maxFiringNeurons, brain->getFiringNeuronCount());
            totalSpikes = brain->getTotalSpikeCount();
        }
        
        double duration = numSteps * timestep;
        float avgFiringRate = numSteps > 0 ? (totalSpikes / (float)numSteps) : 0.0f;
        
        return SimulationResult(duration, numSteps, maxFiringNeurons, totalSpikes, avgFiringRate, firingHistory);
    }

    // Agent utilities
    class AgentEpisode {
    public:
        std::shared_ptr<Brain> brain;
        std::shared_ptr<SimpleWorld> world;
        std::shared_ptr<AgentBrain> agent;
        std::vector<std::vector<float>> visionHistory;
        std::vector<float> rewardHistory;
        std::vector<float> actionHistory;
        
        AgentEpisode(std::shared_ptr<Brain> b, std::shared_ptr<SimpleWorld> w, std::shared_ptr<AgentBrain> a)
            : brain(b), world(w), agent(a) {}
    };

    AgentEpisode createAgentEpisode(size_t worldWidth = 20, size_t worldHeight = 20,
                                    size_t visionWidth = 8, size_t visionHeight = 8) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        world->configure(worldWidth, worldHeight, visionWidth, visionHeight);
        world->reset();
        world->setAgentStart(worldWidth / 2.0f, worldHeight / 2.0f);
        agent->initialize(*world);
        
        return AgentEpisode(brain, world, agent);
    }

    // Comparison utilities
    bool brainEquals(const std::shared_ptr<Brain>& a, const std::shared_ptr<Brain>& b) {
        if (!a || !b) return false;
        return a.get() == b.get();
    }

    // Statistics and monitoring
    class BrainStatistics {
    public:
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        float averageFiringRate;
        float excitationInhibitionRatio;
        float memoryUsage;
        double lastUpdateTime;
        
        BrainStatistics(size_t tn, size_t ts, size_t an, size_t fn, float afr, float eir, float mu, double lut)
            : totalNeurons(tn), totalSynapses(ts), activeNeurons(an), firingNeurons(fn),
              averageFiringRate(afr), excitationInhibitionRatio(eir), memoryUsage(mu),
              lastUpdateTime(lut) {}
    };

    BrainStatistics getBrainStatistics(std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto epoch = now_ms.time_since_epoch();
        double lastUpdateTime = std::chrono::duration<double, std::milli>(epoch).count();
        
        // Estimate memory usage (rough approximation)
        float memoryUsage = sizeof(Brain) + (brain->getTotalNeuronCount() * sizeof(Neuron)) +
                           (brain->getTotalSynapseCount() * sizeof(Synapse)) +
                           (brain->getRegionCount() * sizeof(NeuralRegion)) * 0.001f; // Scale factor
        
        return BrainStatistics(
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getActiveNeuronCount(),
            brain->getFiringNeuronCount(),
            brain->getAverageFiringRate(),
            brain->getExcitationInhibitionRatio(),
            memoryUsage,
            lastUpdateTime
        );
    }

} // namespace nlm
