#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>

// Include local headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/brain/BrainBuilder.hpp"

namespace py = pybind11;
namespace nlm {

// Type aliases for Python convenience
using FloatArray = std::vector<float>;
using IntArray = std::vector<size_t>;
using StringArray = std::vector<std::string>;
using StringMap = std::map<std::string, std::string>;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ==============================================
        
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        Key Features:
        - Brain simulation with spiking neural dynamics
        - Event-driven spike processing with synaptic delays
        - STDP and Hebbian plasticity rules
        - Structural plasticity (synaptogenesis/pruning)
        - Integrated memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Prediction systems for temporal sequence learning
        - Cognitive systems (attention, planning, concept formation)
        - Development systems with staged maturation
        - Agent interface for world interaction
        - Simulation world with agent-environment interaction
        - Performance monitoring and analysis tools
        
        Quick Start Examples:
        
        ```python
        import pynlm
        
        # Create a learning-optimized brain
        brain = pynlm.createLearningBrain()
        brain.initialize()
        
        # Run simulation with world interaction
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        for step in range(1000):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"Simulation completed with {brain.getTotalSpikeCount()} spikes")
        ```
        
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ====================================================================
    // Type Aliases and Helper Functions
    // ====================================================================
    
    m.def("setup_logging", &Logger::setGlobal, py::arg("logger"),
          "Setup global logging system");
    
    m.def("get_global_logger", []() {
        return Logger::getGlobal();
    }, "Get global logger instance");
    
    // ====================================================================
    // Enhanced Type Bindings with Pythonic Interface
    // ====================================================================
    
    // NeuronId with enhanced Python support
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron with enhanced Python support.
        
        Supports arithmetic operations, comparison, and string conversion
        for convenient use in Python code.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__str__", [](const NeuronId& id) {
            return "n" + std::to_string(id.index());
        })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__gt__", &NeuronId::operator>)
        .def("__le__", &NeuronId::operator<=)
        .def("__ge__", &NeuronId::operator>=)
        .def("__hash__", [](const NeuronId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        })
        .def("to_index", &NeuronId::index)
        .def("to_value", &NeuronId::value)
        .def_static("from_index", [](size_t idx) { return NeuronId(static_cast<uint64_t>(idx)); })
        .def_static("from_value", [](uint64_t val) { return NeuronId(val); });

    // SynapseId with enhanced Python support
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse with enhanced Python support.
        
        Supports arithmetic operations, comparison, and string conversion
        for convenient use in Python code.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__str__", [](const SynapseId& id) {
            return "s" + std::to_string(id.index());
        })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        });

    // RegionId with enhanced Python support
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region with enhanced Python support.
        
        Supports arithmetic operations, comparison, and string conversion
        for convenient use in Python code.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__str__", [](const RegionId& id) {
            return "r" + std::to_string(id.index());
        })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        });

    // PopulationId with enhanced Python support
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population with enhanced Python support.
        
        Supports arithmetic operations, comparison, and string conversion
        for convenient use in Python code.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__str__", [](const PopulationId& id) {
            return "p" + std::to_string(id.index());
        })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        });

    // ====================================================================
    // Enumeration Types with Pythonic Enum Support
    // ====================================================================
    
    py::enum_<NeuronType>(m, "NeuronType", py::arithmetic(), R"pbdoc(
        Neuron type enumeration for different neuron functionalities.
        
        Excitatory: Positive feedback neurons that promote firing
        Inhibitory: Negative feedback neurons that suppress firing
        Modulatory: Neurons that affect plasticity and other processes
        Sensory: Neurons that receive external input
        Motor: Neurons that produce motor output
        Internal: General-purpose neurons for internal processing
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Positive feedback neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Negative feedback neurons")
        .value("Modulatory", NeuronType::Modulatory, "Plasticity-affecting neurons")
        .value("Sensory", NeuronType::Sensory, "External input receivers")
        .value("Motor", NeuronType::Motor, "Motor output producers")
        .value("Internal", NeuronType::Internal, "General-purpose processing neurons")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", py::arithmetic(), R"pbdoc(
        Synapse type enumeration for different connection types.
        
        Excitatory: Promotes firing in postsynaptic neuron
        Inhibitory: Suppresses firing in postsynaptic neuron
        Modulatory: Affects plasticity and other processes
        Electrical: Direct electrical coupling
        GapJunction: Electrical gap junction
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Positive feedback connections")
        .value("Inhibitory", SynapseType::Inhibitory, "Negative feedback connections")
        .value("Modulatory", SynapseType::Modulatory, "Plasticity-affecting connections")
        .value("Electrical", SynapseType::Electrical, "Direct electrical coupling")
        .value("GapJunction", SynapseType::GapJunction, "Electrical gap junction")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", py::arithmetic(), R"pbdoc(
        Brain developmental stage enumeration.
        
        Initial: Primitive developmental state
        CriticalPeriod: Heightened plasticity phase
        Maturation: Normal development phase
        Adult: Stable adult state
        Aging: Age-related decline phase
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Primitive developmental state")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Heightened plasticity phase")
        .value("Maturation", DevelopmentalStage::Maturation, "Normal development phase")
        .value("Adult", DevelopmentalStage::Adult, "Stable adult state")
        .value("Aging", DevelopmentalStage::Aging, "Age-related decline phase")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", py::arithmetic(), R"pbdoc(
        Neuron firing state enumeration.
        
        Resting: Not currently firing, below threshold
        Active: Currently firing or just fired
        Refractory: Recovery period after firing
        Inhibited: Temporarily suppressed from firing
    )pbdoc")
        .value("Resting", FiringState::Resting, "Not currently firing")
        .value("Active", FiringState::Active, "Currently firing")
        .value("Refractory", FiringState::Refractory, "Recovery period after firing")
        .value("Inhibited", FiringState::Inhibited, "Temporarily suppressed")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", py::arithmetic(), R"pbdoc(
        Action type enumeration for different motor behaviors.
        
        Movement actions for navigating the environment
        Perception actions for sensing the world
        Interaction actions for manipulating objects
        Maintenance actions for self-preservation
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in world")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in world")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in world")
        .value("MoveRight", ActionType::MoveRight, "Move right in world")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left")
        .value("TurnRight", ActionType::TurnRight, "Turn right")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with nearby object")
        .value("Eat", ActionType::Eat, "Attempt to eat")
        .value("Drink", ActionType::Drink, "Attempt to drink")
        .value("Rest", ActionType::Rest, "Enter resting state")
        .value("Wait", ActionType::Wait, "Wait for future events")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();

    // ====================================================================
    // Enhanced Configuration Class
    // ====================================================================
    
    py::class_<Config>(m, "Config", R"pbdoc(
        Enhanced configuration class for NLM system with validation and schema support.
        
        Provides configuration management for all NLM components including
        brain parameters, plasticity rules, memory systems, and neuromodulation.
    )pbdoc")
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
        // Enhanced configuration methods
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (int64_t)")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (double)")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (float)")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (string)")
        .def("getOr", [](const Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get configuration value with default (int64_t)")
        .def("getOr", [](const Config& self, const std::string& key, double defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get configuration value with default (double)")
        .def("getOr", [](const Config& self, const std::string& key, float defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get configuration value with default (float)")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get configuration value with default (string)")
        .def("getInt64", [](const Config& self, const std::string& key) {
            auto value = self.get<int64_t>(key);
            if (value) return *value;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"), "Get integer configuration value")
        .def("getFloat", [](const Config& self, const std::string& key) {
            auto value = self.get<float>(key);
            if (value) return *value;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"), "Get float configuration value")
        .def("getDouble", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (value) return *value;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"), "Get double configuration value")
        .def("getString", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"), "Get string configuration value")
        .def("validate", [](const Config& self) {
            // Basic validation - can be extended with schema validation
            std::vector<std::string> errors;
            // Check required keys
            auto keys = self.getKeys();
            if (!self.has("neuron_count")) errors.push_back("Missing required key: neuron_count");
            if (!self.has("region_count")) errors.push_back("Missing required key: region_count");
            if (!self.has("connection_probability")) errors.push_back("Missing required key: connection_probability");
            return errors;
        }, "Validate configuration and return list of errors")
        .def("merge", [](Config& self, const Config& other) {
            // Simple merge - other config overrides self
            for (const auto& key : other.getKeys()) {
                if (auto value = other.get<int64_t>(key)) {
                    self.set(key, *value);
                } else if (auto value = other.get<float>(key)) {
                    self.set(key, *value);
                } else if (auto value = other.get<double>(key)) {
                    self.set(key, *value);
                } else if (auto value = other.get<std::string>(key)) {
                    self.set(key, *value);
                }
            }
        }, py::arg("other"), "Merge another configuration into this one");

    // ====================================================================
    // Enhanced Sensory Input Classes
    // ====================================================================
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Enhanced base class for all sensory input types.
        
        Provides common functionality for sensory processing including
        data management, timestamps, and dimensionality information.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("getDataSize", [](const SensoryInput& self) {
            return self.getData().size();
        }, "Get size of sensory data")
        .def("getMean", [](const SensoryInput& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get mean of sensory data")
        .def("getStdDev", [](const SensoryInput& self) {
            const auto& data = self.getData();
            if (data.size() <= 1) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            float mean = sum / data.size();
            float sq_sum = 0.0f;
            for (float val : data) {
                sq_sum += (val - mean) * (val - mean);
            }
            return std::sqrt(sq_sum / (data.size() - 1));
        }, "Get standard deviation of sensory data")
        .def("normalize", [](SensoryInput& self, float mean, float stddev) {
            auto& data = self.getData();
            for (float& val : data) {
                val = (val - mean) / (stddev + 1e-6f);
            }
        }, py::arg("mean"), py::arg("stddev"), "Normalize sensory data to zero mean, unit variance");

    // Vision class with enhanced Python support
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Enhanced vision sensory input with Pythonic interface.
        
        Provides multi-channel visual input with image-like operations
        and enhanced Python support for computer vision tasks.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromArray", [](Vision& self, py::array data) {
            // Convert numpy array to vector
            auto buf = data.request();
            size_t size = buf.size;
            std::vector<float> vec(size);
            std::memcpy(vec.data(), buf.ptr, size * sizeof(float));
            self.setData(vec);
        }, "Set data from numpy array", py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getPixelCount", [](const Vision& self) {
            return self.getWidth() * self.getHeight() * self.getChannels();
        })
        .def("getFlattenedData", &Vision::getData)
        .def("get2DData", [](const Vision& self) {
            // Return as list of rows for easier Python processing
            const auto& data = self.getData();
            size_t width = self.getWidth();
            size_t height = self.getHeight();
            size_t channels = self.getChannels();
            
            std::vector<std::vector<float>> rows(height, std::vector<float>(width * channels));
            
            for (size_t row = 0; row < height; ++row) {
                size_t row_start = row * width * channels;
                for (size_t col = 0; col < width * channels; ++col) {
                    rows[row][col] = data[row_start + col];
                }
            }
            
            return rows;
        }, "Get image data as 2D array")
        .def("applyGaussianBlur", [](Vision& self, float sigma) {
            // Simple Gaussian blur implementation
            // In a real implementation, this would use proper convolution
            auto& data = self.getData();
            size_t width = self.getWidth();
            size_t height = self.getHeight();
            size_t channels = self.getChannels();
            
            std::vector<float> blurred = data;
            // Simple smoothing - would be more sophisticated in practice
            for (size_t i = 0; i < data.size(); ++i) {
                if (i > 0 && i < data.size() - 1) {
                    blurred[i] = (data[i-1] + data[i] + data[i+1]) / 3.0f;
                }
            }
            self.setData(blurred);
        }, "Apply Gaussian blur to image", py::arg("sigma"))
        .def("resize", [](Vision& self, size_t newWidth, size_t newHeight) {
            // Simple resize implementation
            // Would use more sophisticated algorithms in practice
            auto& data = self.getData();
            size_t channels = self.getChannels();
            
            size_t oldWidth = self.getWidth();
            size_t oldHeight = self.getHeight();
            
            std::vector<float> resized(newWidth * newHeight * channels);
            
            for (size_t newRow = 0; newRow < newHeight; ++newRow) {
                for (size_t newCol = 0; newCol < newWidth; ++newCol) {
                    size_t oldRow = (newRow * oldHeight) / newHeight;
                    size_t oldCol = (newCol * oldWidth) / newWidth;
                    
                    size_t oldIdx = (oldRow * oldWidth + oldCol) * channels;
                    size_t newIdx = (newRow * newWidth + newCol) * channels;
                    
                    for (size_t c = 0; c < channels; ++c) {
                        resized[newIdx + c] = data[oldIdx + c];
                    }
                }
            }
            
            self.setData(resized);
        }, "Resize image to specified dimensions", py::arg("new_width"), py::arg("new_height"));

    // Audio class with enhanced Python support
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Enhanced audio sensory input with signal processing capabilities.
        
        Provides audio signal processing with Python-friendly interfaces
        for feature extraction and analysis.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        })
        .def("computeFFT", [](const Audio& self) {
            // Simple FFT computation - would use proper FFT library in practice
            const auto& data = self.getData();
            size_t n = data.size();
            if (n == 0) return std::vector<float>();
            
            std::vector<float> magnitude(n / 2);
            for (size_t i = 0; i < n / 2; ++i) {
                magnitude[i] = 0.0f;
                for (size_t j = 0; j < n; ++j) {
                    magnitude[i] += data[j] * std::cos(2.0f * M_PI * i * j / n);
                }
                magnitude[i] = std::sqrt(magnitude[i] * magnitude[i]);
            }
            return magnitude;
        }, "Compute FFT magnitude spectrum")
        .def("computeMFCC", [](const Audio& self, size_t numCoefficients = 13) {
            // Simple MFCC computation - would use proper libraries in practice
            const auto& data = self.getData();
            if (data.empty()) return std::vector<float>();
            
            // Pre-emphasis
            std::vector<float> emphasized(data.size());
            emphasized[0] = data[0];
            for (size_t i = 1; i < data.size(); ++i) {
                emphasized[i] = data[i] - 0.97f * data[i-1];
            }
            
            // Simple feature extraction (would be much more sophisticated in practice)
            std::vector<float> mfcc(numCoefficients, 0.0f);
            for (size_t i = 0; i < numCoefficients && i < data.size(); ++i) {
                mfcc[i] = emphasized[i];
            }
            
            return mfcc;
        }, "Compute Mel-frequency cepstral coefficients", py::arg("num_coefficients") = 13)
        .def("applyFilterBank", [](Audio& self, const std::vector<float>& filterBank) {
            // Apply mel filter bank
            auto& data = self.getData();
            std::vector<float> filtered(data.size(), 0.0f);
            
            for (size_t i = 0; i < data.size(); ++i) {
                float sum = 0.0f;
                for (size_t j = 0; j < filterBank.size(); ++j) {
                    if (i >= j && i < data.size() - (filterBank.size() - j)) {
                        sum += data[i - j] * filterBank[j];
                    }
                }
                filtered[i] = sum;
            }
            
            self.setData(filtered);
        }, "Apply mel filter bank", py::arg("filter_bank"));

    // InternalSignals class with enhanced Python support
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input for representing internal brain state.
        
        Provides access to internal brain variables like working memory,
        attention, and other cognitive signals.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("addSignal", [](InternalSignals& self, const std::string& name, float value) {
            // Store signal with name for later retrieval
            auto& data = self.getData();
            data.push_back(value);
            // In a real implementation, this would maintain a name-value map
        }, py::arg("name"), py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getData().size)
        .def("getSignalAt", [](const InternalSignals& self, size_t index) {
            const auto& data = self.getData();
            if (index < data.size()) return data[index];
            throw py::index_error("Signal index out of range");
        }, "Get signal value at index")
        .def("getAllSignalsAsDict", [](const InternalSignals& self) {
            // Convert signals to dictionary for easier Python access
            const auto& data = self.getData();
            std::map<std::string, float> signals;
            for (size_t i = 0; i < data.size(); ++i) {
                signals["signal_" + std::to_string(i)] = data[i];
            }
            return signals;
        }, "Get all signals as dictionary")
        .def("addWorkingMemorySignal", [](InternalSignals& self, float activation) {
            self.addSignal(activation);
        }, "Add working memory signal")
        .def("addAttentionSignal", [](InternalSignals& self, float attention) {
            self.addSignal(attention);
        }, "Add attention signal")
        .def("addExcitabilitySignal", [](InternalSignals& self, float excitability) {
            self.addSignal(excitability);
        }, "Add excitability signal");

    // ====================================================================
    // Enhanced Action Class
    // ====================================================================
    
    py::class_<Action>(m, "Action", R"pbdoc(
        Enhanced action representation with Pythonic interface for motor control.
        
        Provides flexible action specification with parameters, cloning,
        and serialization support for Python-based agent control.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getParameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index < params.size()) return params[index];
            throw py::index_error("Parameter index out of range");
        }, "Get parameter value at index")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto& params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
            } else {
                params.resize(index + 1, 0.0f);
                params[index] = value;
            }
        }, "Set parameter value at index")
        .def("getName", &Action::getName)
        .def("getDescription", &Action::getName)  // Alias for getName
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("isEmpty", [](const Action& self) {
            return self.getParameters().empty();
        }, "Check if action has no parameters")
        .def("to_dict", [](const Action& self) {
            return std::map<std::string, py::object>{
                {"type", py::cast(self.getType())},
                {"parameters", py::cast(self.getParameters())}
            };
        }, "Convert action to dictionary")
        .def_static("from_dict", [](const std::map<std::string, py::object>& actionDict) {
            Action action;
            auto it = actionDict.find("type");
            if (it != actionDict.end()) {
                action.setType(py::cast<ActionType>(it->second));
            }
            it = actionDict.find("parameters");
            if (it != actionDict.end()) {
                action.setParameters(py::cast<std::vector<float>>(it->second));
            }
            return action;
        }, "Create action from dictionary", py::arg("action_dict"));

    // ActionResult class with enhanced Python support
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Enhanced action result with comprehensive feedback information.
        
        Provides detailed results from actions including rewards, success status,
        and explanatory messages for agent learning and debugging.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccessful", [](const ActionResult& self) {
            return self.success;
        }, "Check if action was successful")
        .def("getNormalizedReward", [](const ActionResult& self) {
            // Normalize reward to range [0, 1] for learning algorithms
            float absReward = std::abs(self.reward);
            return absReward / (absReward + 1.0f);
        }, "Get normalized reward in range [0, 1]")
        .def("getConfidence", [](const ActionResult& self) {
            // Simple confidence based on message length and success
            float baseConfidence = 0.5f;
            if (self.success) baseConfidence += 0.3f;
            else baseConfidence -= 0.2f;
            baseConfidence += std::min(0.2f, self.message.size() * 0.01f);
            return std::max(0.0f, std::min(1.0f, baseConfidence));
        }, "Get confidence level (0-1)")
        .def("to_dict", [](const ActionResult& self) {
            return std::map<std::string, py::object>{
                {"reward", py::cast(self.reward)},
                {"success", py::cast(self.success)},
                {"message", py::cast(self.message)},
                {"normalized_reward", py::cast(self.getNormalizedReward())},
                {"confidence", py::cast(self.getConfidence())}
            };
        }, "Convert action result to dictionary")
        .def_static("success", [](float reward, const std::string& message = "") {
            return ActionResult(reward, true, message);
        }, "Create successful action result", py::arg("reward"), py::arg("message") = "")
        .def_static("failure", [](float reward, const std::string& message = "") {
            return ActionResult(reward, false, message);
        }, "Create failed action result", py::arg("reward"), py::arg("message") = "");

    // ====================================================================
    // Enhanced SimpleWorld Class
    // ====================================================================
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Enhanced simple 2D world for NLM simulation with Pythonic interface.
        
        Provides a complete agent-environment interaction framework with
        sensor processing, action execution, and world dynamics simulation.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent sensors")
        .def("reset", &SimpleWorld::reset, "Reset world and agent to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position")
        .def("setAgentStartWithOrientation", [](SimpleWorld& self, float x, float y, float orientation) {
            self.setAgentStart(x, y);
            // Note: SimpleWorld may not support orientation directly, but this provides forward compatibility
        }, "Set agent's starting position and orientation", py::arg("x"), py::arg("y"), py::arg("orientation"))
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world dynamics for specified time step")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command and get result")
        .def("applyMotorCommandAndGetResult", [](SimpleWorld& self, const Action& cmd, float currentTime) {
            ActionResult result = self.applyMotorCommand(cmd, currentTime);
            return result;
        }, "Apply motor command and return result", py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get agent's sensory percept of the world")
        .def("getSensoryPerceptAsDict", [](SimpleWorld& self) {
            SensoryPercept percept = self.getSensoryPercept();
            std::map<std::string, py::object> result;
            
            Vision vision = percept.getVision();
            result["vision"] = std::map<std::string, py::object>{
                {"width", vision.getWidth()},
                {"height", vision.getHeight()},
                {"channels", vision.getChannels()},
                {"data", vision.getData()}
            };
            
            Audio audio = percept.getAudio();
            result["audio"] = std::map<std::string, py::object>{
                {"sample_rate", audio.getSampleRate()},
                {"num_samples", audio.getNumSamples()},
                {"data", audio.getData()}
            };
            
            InternalSignals internal = percept.getInternal();
            result["internal"] = std::map<std::string, py::object>{
                {"signals", internal.getData()}
            };
            
            result["timestamp"] = percept.getTimestamp();
            
            return result;
        }, "Get sensory percept as dictionary for easy Python access")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("getAgentBodyAsDict", [](SimpleWorld& self) {
            AgentBody body = self.getAgentBody();
            return std::map<std::string, py::object>{
                {"x", body.x},
                {"y", body.y},
                {"orientation", body.orientation},
                {"velocity_x", body.velocityX},
                {"velocity_y", body.velocityY},
                {"angular_velocity", body.angularVelocity},
                {"energy", body.energy},
                {"health", body.health},
                {"age", body.age},
                {"is_moving", body.isMoving},
                {"is_turning", body.isTurning},
                {"last_action_time", body.lastActionTime}
            };
        }, "Get agent body state as dictionary")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("addObjectWithParams", [](SimpleWorld& self, float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
            WorldObject obj(x, y, type, value, radius);
            self.addObject(obj);
        }, "Add object with parameters", py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at specified position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not blocked)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("step", &SimpleWorld::update, py::arg("timestep"), "Alias for update method")
        .def("act", [](SimpleWorld& self, const Action& action, float timestep) {
            ActionResult result = self.applyMotorCommand(action, timestep);
            return result;
        }, "Perform action and return result", py::arg("action"), py::arg("timestep"));

    // ====================================================================
    // Enhanced Brain Class with BrainBuilder Support
    // ====================================================================
    
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Enhanced central neural simulation brain class with Pythonic interface.
        
        Provides real spiking neural computation with event-driven dynamics
        and integrated memory, prediction, cognition, and neuromodulation systems.
        Now includes support for BrainBuilder for advanced configuration.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def(py::init<BrainBuilder&>(), py::arg("builder"),
             "Create brain using BrainBuilder")
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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
        // Enhanced brain methods
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system interface")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random number generator")
        .def("getStatistics", [](Brain& brain) {
            std::map<std::string, py::object> stats;
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["pending_spikes"] = brain.getPendingSpikeEventCount();
            stats["avg_firing_rate"] = brain.getAverageFiringRate();
            stats["e_i_ratio"] = brain.getExcitationInhibitionRatio();
            stats["development_stage"] = py::cast(brain.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive brain statistics as dictionary")
        .def("to_dict", [](Brain& brain) {
            std::map<std::string, py::object> brainState;
            brainState["total_neurons"] = brain.getTotalNeuronCount();
            brainState["total_synapses"] = brain.getTotalSynapseCount();
            brainState["total_spikes"] = brain.getTotalSpikeCount();
            brainState["firing_rate"] = brain.getAverageFiringRate();
            brainState["development_stage"] = py::cast(brain.getDevelopmentalStage());
            brainState["config"] = brain.getConfig();
            return brainState;
        }, "Convert brain state to dictionary")
        .def_static("from_dict", [](const std::map<std::string, py::object>& brainState) {
            // Note: This would require creating a brain from saved state
            // For now, it's a placeholder that returns a default brain
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain from saved state");
            }
            return brain;
        }, "Create brain from saved state dictionary", py::arg("brain_state"));

    // ====================================================================
    // Enhanced AgentBrain Class
    // ====================================================================
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Enhanced agent brain interface connecting NLM brain to world.
        
        Provides simplified agent interface for common agent-brain interactions
        with Pythonic methods and enhanced error handling.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("processSensoryInputAndGetResult", [](AgentBrain& self, const SensoryPercept& percept) {
            self.processSensoryInput(percept);
            return;
        }, "Process sensory input and return success status", py::arg("percept"))
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
        // Enhanced agent methods
        .def("enableLearning", [](AgentBrain& self, bool enable) {
            self.enableRewardModulation(enable);
            self.enableCuriosity(enable);
        }, "Enable learning systems", py::arg("enable"))
        .def("enablePlasticity", [](AgentBrain& self, bool enable) {
            self.enableStructuralPlasticity(enable);
        }, "Enable structural plasticity", py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable development system")
        .def("getLearningStatus", [](AgentBrain& self) {
            std::map<std::string, bool> status;
            status["reward_modulation"] = self.isRewardModulationEnabled();
            status["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            status["development"] = self.isDevelopmentEnabled();
            status["curiosity"] = self.isCuriosityEnabled();
            return status;
        }, "Get current learning system status")
        .def("getNeuromodulationState", [](AgentBrain& self) {
            std::map<std::string, float> state;
            state["dopamine"] = self.getNeuromodulationLevel();
            state["curiosity"] = self.getCuriosityLevel();
            state["novelty"] = self.getNoveltyLevel();
            state["prediction_error"] = self.getPredictionError();
            return state;
        }, "Get current neuromodulation state")
        .def("trainEpisode", [](AgentBrain& self, SimpleWorld& world, int maxSteps) {
            float totalReward = 0.0f;
            int steps = 0;
            
            while (steps < maxSteps) {
                world.update(0.1f);
                SensoryPercept percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                
                auto action = self.decodeMotorCommand();
                ActionResult result = world.applyMotorCommand(action, world.getSimulationTime());
                
                self.applyRewardModulation(result.reward, 0.0f);  // Simplified prediction
                
                totalReward += result.reward;
                steps++;
                
                if (result.success) {
                    break;
                }
            }
            
            return totalReward;
        }, "Train agent for one episode and return total reward", py::arg("world"), py::arg("max_steps"));

    // ====================================================================
    // Enhanced Convenience Functions with BrainBuilder Support
    // ====================================================================
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createMinimalBrain", []() -> std::shared_ptr<Brain> {
        return BrainBuilder().buildMinimal();
    }, "Create a minimal brain for testing");
    
    m.def("createStandardBrain", []() -> std::shared_ptr<Brain> {
        return BrainBuilder().buildStandard();
    }, "Create a standard brain for general research");
    
    m.def("createLearningBrain", []() -> std::shared_ptr<Brain> {
        return BrainBuilder().buildLearning();
    }, "Create a learning-focused brain");
    
    m.def("createCognitionBrain", []() -> std::shared_ptr<Brain> {
        return BrainBuilder().buildCognition();
    }, "Create a cognitive brain");
    
    m.def("createDevelopmentBrain", []() -> std::shared_ptr<Brain> {
        return BrainBuilder().buildDevelopment();
    }, "Create a brain for development studies");
    
    m.def("createPerformanceBrain", []() -> std::shared_ptr<Brain> {
        return BrainBuilder().buildPerformance();
    }, "Create a brain with performance optimizations");
    
    m.def("createResearchBrain", []() -> std::shared_ptr<Brain> {
        return BrainBuilder().buildResearch();
    }, "Create a comprehensive research-grade brain");
    
    // BrainBuilder Python interface
    py::class_<BrainBuilder>(m, "BrainBuilder", R"pbdoc(
        Python interface for BrainBuilder with fluent configuration API.
        
        Provides a Pythonic way to create and configure NLM brains using
        method chaining for intuitive and readable brain creation.
    )pbdoc")
        .def(py::init<>())
        .def("add_region", &BrainBuilder::addRegion, py::arg("name"), py::arg("neuron_count"),
             "Add a neural region with specified neuron count")
        .def("set_neuron_count", &BrainBuilder::setNeuronCount, py::arg("neuron_count"),
             "Set total neuron count")
        .def("set_region_count", &BrainBuilder::setRegionCount, py::arg("region_count"),
             "Set number of regions")
        .def("set_connection_probability", &BrainBuilder::setConnectionProbability, py::arg("probability"),
             "Set connection probability")
        .def("enable_stdp", &BrainBuilder::enableSTDP, py::arg("enable"),
             "Enable or disable STDP")
        .def("enable_hebbian", &BrainBuilder::enableHebbian, py::arg("enable"),
             "Enable or disable Hebbian learning")
        .def("enable_structural_plasticity", &BrainBuilder::enableStructuralPlasticity, py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enable_memory", &BrainBuilder::enableMemorySystem, py::arg("enable"),
             "Enable or disable memory systems")
        .def("enable_prediction", &BrainBuilder::enablePredictionSystem, py::arg("enable"),
             "Enable or disable prediction system")
        .def("enable_cognition", &BrainBuilder::enableCognition, py::arg("enable"),
             "Enable or disable cognition systems")
        .def("enable_neuromodulation", &BrainBuilder::enableNeuromodulation, py::arg("enable"),
             "Enable or disable neuromodulation systems")
        .def("enable_development", &BrainBuilder::enableDevelopment, py::arg("enable"),
             "Enable or disable development system")
        .def("enable_development_stages", &BrainBuilder::enableDevelopmentStages, py::arg("enable"),
             "Enable or disable development stages")
        .def("set_stdp_parameters", &BrainBuilder::setSTDPParameters, py::arg("ltp_weight"), py::arg("ltd_weight"), py::arg("tau"),
             "Set STDP parameters")
        .def("set_structural_plasticity_parameters", &BrainBuilder::setStructuralPlasticityParameters, py::arg("synaptogenesis_rate"), py::arg("pruning_rate"),
             "Set structural plasticity parameters")
        .def("set_development_parameters", &BrainBuilder::setDevelopmentParameters, py::arg("enable_critical_period"), py::arg("maturation_rate"),
             "Set development parameters")
        .def("configure_for_learning", &BrainBuilder::configureForLearning,
             "Configure for rapid adaptation and learning")
        .def("configure_for_cognition", &BrainBuilder::configureForCognition,
             "Configure for cognitive tasks and planning")
        .def("configure_for_performance", &BrainBuilder::configureForPerformance,
             "Configure for speed and efficiency")
        .def("configure_for_development", &BrainBuilder::configureForDevelopment,
             "Configure for development studies")
        .def("configure_for_standard_research", &BrainBuilder::configureForStandardResearch,
             "Configure for standard research use")
        .def("build_minimal", &BrainBuilder::buildMinimal,
             "Build a minimal brain for testing")
        .def("build_standard", &BrainBuilder::buildStandard,
             "Build a standard brain for research")
        .def("build_learning", &BrainBuilder::buildLearning,
             "Build a learning-focused brain")
        .def("build_cognition", &BrainBuilder::buildCognition,
             "Build a cognitive brain")
        .def("build_development", &BrainBuilder::buildDevelopment,
             "Build a brain for development studies")
        .def("build_performance", &BrainBuilder::buildPerformance,
             "Build a brain with performance optimizations")
        .def("build_research", &BrainBuilder::buildResearch,
             "Build a research-grade brain");
    
    // Convenience factory functions
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
    
    // Performance monitoring constants
    m.attr("DEFAULT_BRAIN_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_BRAIN_REGION_COUNT") = 1;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.15f;
    m.attr("DEFAULT_SIMULATION_TIMESTEP") = 0.001f;
    
    // Utility functions
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                              std::shared_ptr<AgentBrain> agent, int num_steps) {
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, "Run a complete simulation", py::arg("brain"), py::arg("world"), 
        py::arg("agent"), py::arg("num_steps"));
    
    m.def("benchmark_brain", [](std::shared_ptr<Brain> brain, int steps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return duration.count();
    }, "Benchmark brain performance", py::arg("brain"), py::arg("steps"));
    
    m.def("get_version", []() {
        return "NLM Python Bindings v2.0 Enhanced";
    }, "Get NLM version");
    
    m.def("get_supported_features", []() {
        return std::vector<std::string>{
            "Spiking Neural Dynamics",
            "Event-driven Spike Processing",
            "STDP and Hebbian Plasticity",
            "Structural Plasticity",
            "Integrated Memory Systems",
            "Neuromodulation",
            "Prediction Systems",
            "Cognitive Systems",
            "Development Systems",
            "BrainBuilder Fluent Interface",
            "Enhanced Python Support",
            "Signal Processing",
            "Visualization Support",
            "Performance Monitoring",
            "Agent-World Interaction"
        };
    }, "Get list of supported features");
    
    // Error types
    py::register_exception<std::runtime_error>(m, "NLMError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    
    // Examples module for documentation
    py::class_<py::module>(m, "examples")
        .def_static("quick_start", []() {
            // Return example code as string
            return R"(
import pynlm

# Quick start: Create a learning brain and run simulation
brain = pynlm.createLearningBrain()
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation completed with {brain.getTotalSpikeCount()} spikes")
)";
        }, "Get quick start example code");
    
    py::class_<py::module>(m, "tutorials")
        .def_static("getting_started", []() {
            return R"(
# Getting Started with NLM Python Bindings

1. Basic Brain Creation
brain = pynlm.createStandardBrain()
brain.initialize()

2. World and Agent Setup
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

3. Simulation Loop
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

4. Brain Analysis
stats = brain.getStatistics()
print(f"Neurons: {stats['total_neurons']}, Spikes: {stats['total_spikes']}")
)";
        }, "Get getting started tutorial")
        .def_static("brain_builder_tutorial", []() {
            return R"(
# BrainBuilder Tutorial

# Create a learning brain using BrainBuilder
brain = pynlm.BrainBuilder() \
    .configure_for_learning() \
    .add_region("Sensory", 200) \
    .add_region("Association", 300) \
    .add_region("Motor", 200) \
    .enable_memory(true) \
    .enable_cognition(true) \
    .build_research()

# Or use factory functions for convenience
brain = pynlm.createLearningBrain()
)";
        }, "Get BrainBuilder tutorial");
}

} // namespace nlm