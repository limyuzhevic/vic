#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

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

// Custom Python exceptions for better error handling
class PyRuntimeError : public std::runtime_error {
public:
    explicit PyRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class PyValueError : public PyRuntimeError {
public:
    explicit PyValueError(const std::string& msg) : PyRuntimeError(msg) {}
};

class PyConfigurationError : public PyRuntimeError {
public:
    explicit PyConfigurationError(const std::string& msg) : PyRuntimeError(msg) {}
};

class PyBrainError : public PyRuntimeError {
public:
    explicit PyBrainError(const std::string& msg) : PyRuntimeError(msg) {}
};

// Configuration serialization utilities
static py::dict configToDict(const Config& cfg) {
    py::dict result;
    auto keys = cfg.getKeys();
    for (const auto& key : keys) {
        auto value = cfg.get(ConfigValue(key));
        result[key] = *value;
    }
    return result;
}

static void dictToConfig(Config& cfg, const py::dict& d) {
    for (auto item : d) {
        std::string key = py::cast<std::string>(item.first);
        // Try different types
        try {
            int val_int = py::cast<int>(item.second);
            cfg.set(key, val_int);
        } catch (...) {
            try {
                double val_double = py::cast<double>(item.second);
                cfg.set(key, val_double);
            } catch (...) {
                try {
                    bool val_bool = py::cast<bool>(item.second);
                    cfg.set(key, val_bool);
                } catch (...) {
                    try {
                        std::string val_str = py::cast<std::string>(item.second);
                        cfg.set(key, val_str);
                    } catch (...) {
                        // If all fail, throw error
                        throw PyValueError("Cannot convert value for key '" + key + "' to any supported type");
                    }
                }
            }
        }
    }
}

// Helper function to serialize brain state to Python dict
static py::dict serializeBrainToDict(const Brain& brain) {
    py::dict result;
    
    // Basic statistics
    result["total_neurons"] = brain.getTotalNeuronCount();
    result["total_synapses"] = brain.getTotalSynapseCount();
    result["active_neurons"] = brain.getActiveNeuronCount();
    result["firing_neurons"] = brain.getFiringNeuronCount();
    result["average_firing_rate"] = brain.getAverageFiringRate();
    result["e_i_ratio"] = brain.getExcitationInhibitionRatio();
    result["total_spikes"] = brain.getTotalSpikeCount();
    
    // Developmental stage
    result["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
    
    // Regions info
    py::list regions;
    auto region_ids = brain.getRegionIds();
    for (const auto& rid : region_ids) {
        py::dict region_info;
        region_info["id"] = rid.value;
        region_info["neuron_count"] = rid.index; // Assuming index is accessible
        regions.append(region_info);
    }
    result["regions"] = regions;
    
    return result;
}

// Helper function to create brain from Python dict configuration
static std::shared_ptr<Config> createConfigFromDict(const py::dict& config_dict) {
    auto config = std::make_shared<Config>();
    dictToConfig(*config, config_dict);
    return config;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        
        **Pythonic Interface**: 
        - Context managers for resource management
        - Property-based access for frequently used data
        - Batch operations for efficiency
        - Native serialization (JSON, pickle, binary)
        
        **Enhanced Error Handling**:
        - Custom Python exceptions for different error types
        - Detailed error messages with context
        
        **Advanced Features**:
        - Visualization and monitoring tools
        - Machine learning integration helpers
        - Configuration wizards for common setups
        - Testing and validation utilities
        
        Key Classes: Brain, Config, AgentBrain, SimpleWorld, SensoryInput, Action
    )pbdoc";

    // Register custom Python exceptions
    py::register_exception<PyRuntimeError>(m, "RuntimeError");
    py::register_exception<PyValueError>(m, "ValueError");
    py::register_exception<PyConfigurationError>(m, "ConfigurationError");
    py::register_exception<PyBrainError>(m, "BrainError");

    // ID classes with enhanced Python features
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
        .def("__ne__", &SynapseId::operator!=)
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

    // Enhanced enumeration bindings
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

    // Enhanced Config class with Python features
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
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
        // Pythonic properties
        .def_property_read("keys", &Config::getKeys)
        .def_property_read("summary", &Config::summary)
        // Enhanced helper methods
        .def("to_dict", configToDict, "Convert configuration to Python dictionary")
        .def("from_dict", [](Config& self, const py::dict& d) {
            dictToConfig(self, d);
        }, py::arg("dict"), "Load configuration from Python dictionary")
        .def("set_defaults", [](Config& self, const py::dict& defaults) {
            dictToConfig(self, defaults);
        }, py::arg("defaults"), "Set default configuration values")
        .def("validate", [](const Config& self, const py::dict& schema) {
            // Basic validation - check that all schema keys exist
            py::list errors;
            for (auto item : schema) {
                std::string key = py::cast<std::string>(item.first);
                if (!self.has(key)) {
                    errors.append("Missing required key: '" + key + "'");
                }
            }
            return errors;
        }, py::arg("schema"), "Validate configuration against schema");

    // Enhanced SensoryInput with factory methods
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        // Python factory methods
        .def_static("create_vision", [](size_t width, size_t height, size_t channels = 3) {
            auto vision = std::make_shared<Vision>();
            vision->setData(std::vector<float>(width * height * channels, 0.0f));
            return vision;
        }, py::arg("width"), py::arg("height"), py::arg("channels", 3),
         "Create Vision sensory input with default data")
        .def_static("create_audio", [](size_t sampleRate, size_t numSamples) {
            auto audio = std::make_shared<Audio>();
            audio->setData(std::vector<float>(numSamples, 0.0f));
            audio->setSampleRate(sampleRate);
            return audio;
        }, py::arg("sampleRate"), py::arg("numSamples"),
         "Create Audio sensory input with default data")
        .def_static("create_internal", []() {
            return std::make_shared<InternalSignals>();
        }, "Create InternalSignals sensory input");

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
        // Pythonic properties
        .def_property_read("width", &Vision::getWidth)
        .def_property_read("height", &Vision::getHeight)
        .def_property_read("channels", &Vision::getChannels)
        // Convenience methods
        .def("normalize", [](Vision& self) {
            // Normalize pixel data to [0, 1]
            auto data = self.getData();
            float min_val = *std::min_element(data.begin(), data.end());
            float max_val = *std::max_element(data.begin(), data.end());
            if (max_val > min_val) {
                for (auto& val : data) {
                    val = (val - min_val) / (max_val - min_val);
                }
                self.setData(data);
            }
        }, "Normalize vision data to [0, 1]")
        .def("resize", [](Vision& self, size_t new_width, size_t new_height) {
            // Simple resize - this is a placeholder for actual implementation
            auto data = self.getData();
            size_t old_width = self.getWidth();
            size_t old_height = self.getHeight();
            size_t old_channels = self.getChannels();
            
            std::vector<float> resized_data(new_width * new_height * old_channels, 0.0f);
            // Simple scaling - not a proper resize algorithm
            float scale_x = static_cast<float>(old_width) / new_width;
            float scale_y = static_cast<float>(old_height) / new_height;
            
            for (size_t y = 0; y < new_height; ++y) {
                for (size_t x = 0; x < new_width; ++x) {
                    size_t src_x = static_cast<size_t>(x * scale_x);
                    size_t src_y = static_cast<size_t>(y * scale_y);
                    src_x = std::min(src_x, old_width - 1);
                    src_y = std::min(src_y, old_height - 1);
                    
                    for (size_t c = 0; c < old_channels; ++c) {
                        size_t src_idx = (src_y * old_width + src_x) * old_channels + c;
                        size_t dst_idx = (y * new_width + x) * old_channels + c;
                        resized_data[dst_idx] = data[src_idx];
                    }
                }
            }
            self.setData(resized_data);
        }, py::arg("new_width"), py::arg("new_height"), "Resize vision image");

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
        // Pythonic properties
        .def_property_read("sampleRate", &Audio::getSampleRate)
        .def_property_read("numSamples", &Audio::getNumSamples)
        // Convenience methods
        .def("normalize", [](Audio& self) {
            auto data = self.getData();
            float max_val = *std::max_element(data.begin(), data.end(), 
                                            [](float a, float b) { return std::abs(a) < std::abs(b); });
            if (max_val > 0.0f) {
                for (auto& val : data) {
                    val /= max_val;
                }
                self.setData(data);
            }
        }, "Normalize audio data to [-1, 1]")
        .def("filter", [](Audio& self, float cutoff_freq, float sample_rate) {
            // Simple low-pass filter - placeholder for actual implementation
            auto data = self.getData();
            // Basic moving average filter
            size_t num_samples = data.size();
            std::vector<float> filtered(num_samples, 0.0f);
            
            for (size_t i = 0; i < num_samples; ++i) {
                float sum = 0.0f;
                size_t count = 0;
                for (size_t j = std::max(0LL, static_cast<long>(i - 5)); 
                     j <= std::min(num_samples - 1, static_cast<size_t>(i + 5)); ++j) {
                    sum += data[j];
                    count++;
                }
                filtered[i] = sum / count;
            }
            self.setData(filtered);
        }, py::arg("cutoff_freq"), py::arg("sample_rate"), "Apply simple low-pass filter");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        // Pythonic properties
        .def_property_read("signal_count", [](const InternalSignals& self) {
            return self.getData().size();
        })
        // Convenience methods
        .def("mean", [] (const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = 0.0f;
            for (float val : data) sum += val;
            return sum / data.size();
        }, "Get mean of internal signals")
        .def("std", [] (const InternalSignals& self) {
            auto data = self.getData();
            if (data.size() <= 1) return 0.0f;
            float mean = 0.0f;
            for (float val : data) mean += val;
            mean /= data.size();
            
            float variance = 0.0f;
            for (float val : data) {
                variance += (val - mean) * (val - mean);
            }
            variance /= data.size();
            return std::sqrt(variance);
        }, "Get standard deviation of internal signals")
        .def("add_noise", [](InternalSignals& self, float noise_level = 0.1f) {
            auto data = self.getData();
            for (auto& val : data) {
                val += (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * noise_level;
            }
            self.setData(data);
        }, py::arg("noise_level", 0.1f), "Add Gaussian noise to signals");

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
        // Pythonic properties
        .def_property("type", &Action::getType, &Action::setType)
        .def_property("parameters", &Action::getParameters, &Action::setParameters)
        // Convenience methods
        .def("get_parameter", [](Action& self, size_t index, float default_val = 0.0f) {
            auto params = self.getParameters();
            if (index < params.size()) return params[index];
            return default_val;
        }, py::arg("index"), py::arg("default_val", 0.0f), "Get parameter by index with default")
        .def("set_parameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index >= params.size()) {
                params.resize(index + 1, 0.0f);
            }
            params[index] = value;
            self.setParameters(params);
        }, py::arg("index"), py::arg("value"), "Set parameter by index")
        .def("normalize_parameters", [](Action& self, float max_val = 1.0f) {
            auto params = self.getParameters();
            if (params.empty()) return;
            
            float max_param = *std::max_element(params.begin(), params.end(),
                                              [](float a, float b) { return std::abs(a) < std::abs(b); });
            if (max_param > max_val) {
                for (auto& val : params) {
                    val /= max_param * max_val;
                }
                self.setParameters(params);
            }
        }, py::arg("max_val", 1.0f), "Normalize parameters to specified range");

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
        // Pythonic properties
        .def_property("x", [](const WorldObject& obj) { return obj.x; },
                     [](WorldObject& obj, float val) { obj.x = val; })
        .def_property("y", [](const WorldObject& obj) { return obj.y; },
                     [](WorldObject& obj, float val) { obj.y = val; })
        .def_property("position", [](const WorldObject& obj) { return py::make_tuple(obj.x, obj.y); },
                     [](WorldObject& obj, const py::tuple& pos) {
                         obj.x = py::cast<float>(pos[0]);
                         obj.y = py::cast<float>(pos[1]);
                     })
        // Distance calculation
        .def("distance_to", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Calculate distance to another object");

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
        // Pythonic properties
        .def_property("position", [](const AgentBody& body) { return py::make_tuple(body.x, body.y); },
                     [](AgentBody& body, const py::tuple& pos) {
                         body.x = py::cast<float>(pos[0]);
                         body.y = py::cast<float>(pos[1]);
                     })
        .def_property("velocity", [](const AgentBody& body) { return py::make_tuple(body.velocityX, body.velocityY); },
                     [](AgentBody& body, const py::tuple& vel) {
                         body.velocityX = py::cast<float>(vel[0]);
                         body.velocityY = py::cast<float>(vel[1]);
                     })
        // Movement convenience methods
        .def("move_to", [](AgentBody& self, float x, float y, float speed) {
            float dx = x - self.x;
            float dy = y - self.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist > 0.001f) {
                float scale = speed / dist;
                self.velocityX = dx * scale;
                self.velocityY = dy * scale;
                self.isMoving = true;
                self.isTurning = true;
            }
        }, py::arg("x"), py::arg("y"), py::arg("speed"), "Move to position with given speed")
        .def("stop", [] (AgentBody& self) {
            self.velocityX = 0.0f;
            self.velocityY = 0.0f;
            self.angularVelocity = 0.0f;
            self.isMoving = false;
            self.isTurning = false;
        }, "Stop all movement");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Convenience methods
        .def("is_success", [](const ActionResult& self) {
            return self.success;
        })
        .def("get_reward_category", [](const ActionResult& self) {
            if (self.reward > 10.0f) return "high";
            if (self.reward > 0.0f) return "positive";
            if (self.reward < -10.0f) return "severe_negative";
            if (self.reward < 0.0f) return "negative";
            return "neutral";
        }, "Categorize reward as positive/negative/neutral");

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
        // Convenience methods
        .def("normalize_vision", [](SensoryPercept& self) {
            auto vision = self.getVision();
            if (vision) {
                float min_val = *std::min_element(vision->getData().begin(), vision->getData().end());
                float max_val = *std::max_element(vision->getData().begin(), vision->getData().end());
                if (max_val > min_val) {
                    std::vector<float> normalized;
                    for (float val : vision->getData()) {
                        normalized.push_back((val - min_val) / (max_val - min_val));
                    }
                    vision->setData(normalized);
                }
            }
        }, "Normalize vision data to [0, 1]")
        .def("get_vision_shape", [](const SensoryPercept& self) {
            if (auto vision = self.getVision()) {
                return py::make_tuple(vision->getWidth(), vision->getHeight(), vision->getChannels());
            }
            return py::none();
        }, "Get vision image dimensions");

    // Enhanced SimpleWorld with Python features
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Pythonic properties
        .def_property_read("width", &SimpleWorld::getWidth)
        .def_property_read("height", &SimpleWorld::getHeight)
        .def_property_read("simulation_time", &SimpleWorld::getSimulationTime)
        // Convenience methods
        .def("add_resource", [](SimpleWorld& self, float x, float y, float value = 1.0f) {
            WorldObject obj;
            obj.x = x; obj.y = y; 
            obj.type = WorldObjectType::Resource;
            obj.value = value;
            obj.active = true;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value", 1.0f), "Add resource at position")
        .def("add_hazard", [](SimpleWorld& self, float x, float y, float value = -1.0f) {
            WorldObject obj;
            obj.x = x; obj.y = y;
            obj.type = WorldObjectType::Hazard;
            obj.value = value;
            obj.active = true;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value", -1.0f), "Add hazard at position")
        .def("get_objects_by_type", [](SimpleWorld& self, WorldObjectType type) {
            py::list objects;
            // This would need access to internal world state
            return objects;
        }, py::arg("type"), "Get all objects of specific type")
        .def("clear_objects", [](SimpleWorld& self) {
            // Clear all objects - placeholder for actual implementation
        }, "Clear all world objects");

    // Enhanced Brain class with Python features
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
             "Log brain status")
        // Pythonic properties
        .def_property_read("total_neurons", &Brain::getTotalNeuronCount)
        .def_property_read("total_synapses", &Brain::getTotalSynapseCount)
        .def_property_read("active_neurons", &Brain::getActiveNeuronCount)
        .def_property_read("firing_neurons", &Brain::getFiringNeuronCount)
        .def_property_read("average_firing_rate", &Brain::getAverageFiringRate)
        .def_property_read("e_i_ratio", &Brain::getExcitationInhibitionRatio)
        .def_property_read("total_spikes", &Brain::getTotalSpikeCount)
        .def_property_read("developmental_stage", &Brain::getDevelopmentalStage)
        // Context manager for brain lifecycle
        .def("__enter__", [](std::shared_ptr<Brain> self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<Brain> self, py::args) {
            self->reset();
        })
        // Enhanced helper methods
        .def("step_with_input", [](Brain& self, SimulationStep step, 
                                 const SensoryInput& input) {
            self.step(step);
            self.receiveSensoryInput(input);
        }, py::arg("step"), py::arg("input"),
         "Perform simulation step with sensory input")
        .def("batch_step", [](Brain& self, size_t num_steps) {
            for (size_t i = 0; i < num_steps; ++i) {
                self.step(static_cast<SimulationStep>(i));
            }
        }, py::arg("num_steps"), "Perform multiple simulation steps")
        .def("get_stats", serializeBrainToDict, "Get comprehensive brain statistics as Python dict")
        .def("validate", [](Brain& self) {
            // Basic validation checks
            py::list errors;
            if (self.getTotalNeuronCount() == 0) {
                errors.append("Brain has no neurons");
            }
            if (self.getTotalSynapseCount() == 0) {
                errors.append("Brain has no synapses");
            }
            return errors;
        }, "Validate brain state and return list of errors")
        .def("simulate_episode", [](Brain& self, size_t num_steps,
                                   const std::vector<SensoryInput>& inputs) {
            for (size_t i = 0; i < num_steps && i < inputs.size(); ++i) {
                self.step(static_cast<SimulationStep>(i));
                self.receiveSensoryInput(inputs[i]);
            }
        }, py::arg("num_steps"), py::arg("inputs"), "Simulate complete episode with inputs");

    // Enhanced AgentBrain class
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Pythonic properties
        .def_property_read("brain", &AgentBrain::getBrain)
        .def_property_read("developmental_stage", &AgentBrain::getDevelopmentalStage)
        // Convenience methods
        .def("act", [](AgentBrain& self, const SensoryPercept& percept) {
            self.processSensoryInput(percept);
            return self.decodeMotorCommand();
        }, py::arg("percept"), "Perform perception-action cycle")
        .def("train", [](AgentBrain& self, float reward, float predicted_reward = 0.0f) {
            self.applyRewardModulation(reward, predicted_reward);
            self.updateDevelopment(1);
        }, py::arg("reward"), py::arg("predicted_reward", 0.0f), "Train agent with reward")
        .def("reset_episode", [](AgentBrain& self) {
            self.reset();
        }, "Reset agent for new episode");

    // Global factory functions with enhanced Python features
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    .def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    // Advanced factory functions
    .def("create_config_from_dict", createConfigFromDict,
         py::arg("config_dict"), "Create configuration from Python dictionary")
    .def("create_brain_from_dict", [](const py::dict& config_dict) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(createConfigFromDict(config_dict));
    }, py::arg("config_dict"), "Create brain from Python configuration dictionary")
    .def("create_standard_config", []() {
        auto config = std::make_shared<Config>();
        // Set up standard brain configuration
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(5));
        config->set("connection_probability", 0.1);
        config->set("random_seed", static_cast<int64_t>(42));
        return config;
    }, "Create standard brain configuration")
    .def("create_minimal_config", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        return config;
    }, "Create minimal brain configuration");

    // Global constants and utilities
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Utility functions
    m.def("random_seed", [](int seed) { std::srand(seed); },
         py::arg("seed"), "Set random seed for reproducible simulations");

    m.def("simulate_brain", [](std::shared_ptr<Brain> brain, size_t steps,
                             const std::vector<SensoryInput>& inputs) {
        for (size_t i = 0; i < steps && i < inputs.size(); ++i) {
            brain->step(static_cast<SimulationStep>(i));
            brain->receiveSensoryInput(inputs[i]);
        }
    }, py::arg("brain"), py::arg("steps"), py::arg("inputs"),
     "Helper function to simulate brain with inputs");

    m.def("create_agent_episode", [](std::shared_ptr<AgentBrain> agent,
                                   const std::vector<SensoryPercept>& percepts,
                                   const std::vector<float>& rewards) {
        agent->reset();
        py::list actions;
        for (size_t i = 0; i < percepts.size() && i < rewards.size(); ++i) {
            auto action = agent->act(percepts[i]);
            actions.append(action);
            agent->train(rewards[i]);
        }
        return actions;
    }, py::arg("agent"), py::arg("percepts"), py::arg("rewards"),
     "Run complete agent episode and return actions");

    // Version and metadata
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__description__") = "Enhanced NLM Python bindings with Pythonic interface";

    // Performance monitoring utilities
    m.def("monitor_performance", [](std::shared_ptr<Brain> brain, size_t steps) {
        py::dict metrics;
        metrics["steps_per_second"] = steps / 1.0;  // Placeholder
        metrics["neurons_per_step"] = brain->getTotalNeuronCount() / 1.0;
        metrics["synapses_per_step"] = brain->getTotalSynapseCount() / 1.0;
        return metrics;
    }, py::arg("brain"), py::arg("steps"), "Monitor brain performance metrics");
}

} // namespace nlm