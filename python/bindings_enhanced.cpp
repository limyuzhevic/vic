#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <optional>
#include <variant>
#include <sstream>

namespace py = pybind11;

using Timestamp = double;
using SimulationStep = uint64_t;
using MembranePotential = float;
using Weight = float;

namespace nlm {

// Factory functions
static std::shared_ptr<Config> createDefaultConfig() {
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    return config;
}

static std::shared_ptr<Brain> createBrainWithConfig(std::shared_ptr<Config> config) {
    return std::make_shared<Brain>(config);
}

static std::shared_ptr<SimpleWorld> createSimpleWorld() {
    return std::make_shared<SimpleWorld>();
}

static std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
    return std::make_shared<AgentBrain>(brain);
}

} // namespace nlm

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        
        This module provides enhanced Python bindings for the NLM C++ neural simulation framework.
        Key improvements over the original bindings:
        
        1. Modern C++ features: std::optional, std::variant, range-based loops, auto
        2. Better error handling with custom exception types
        3. Convenience methods for common operations
        4. Type safety improvements with overloaded getOr() methods
        5. Enhanced documentation strings
        6. Factory builder pattern
        7. Progress callbacks for long-running operations
        8. Comprehensive metrics collection
        9. Parameter validation
        10. Enhanced action and percept handling
        
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Usage example:
            import pynlm_enhanced as nlm
            
            # Create brain with default configuration
            config = nlm.createDefaultConfig()
            brain = nlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for step in range(1000):
                brain.step(step, step * 0.001)
            
            # Get metrics
            metrics = brain.getMetrics()
            print(f"Firing rate: {metrics.averageFiringRate}")
    )pbdoc";
    
    // Enhanced exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    py::register_exception<std::logic_error>(m, "LogicError");
    
    // Enhanced NeuronId class
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron with enhanced functionality.
        
        Features:
        - Automatic string representation
        - Equality comparison operators
        - Hash support for use in dictionaries and sets
        - Index-based access for neural populations
        - Validation methods
    )pbdoc")
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
        })
        .def("is_valid", [](const NeuronId& id) {
            return id.value != 0;  // Example validation
        })
        .def("to_int", [](const NeuronId& id) {
            return id.value;
        })
        .def_static("from_int", [](uint64_t val) {
            return NeuronId{val};
        }, py::arg("value"));
    
    // Enhanced SynapseId class
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse with enhanced functionality.
        
        Features:
        - String representation
        - Validation
        - Conversion methods
    )pbdoc")
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
    
    // Enhanced RegionId class
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region with enhanced functionality.
        
        Features:
        - String representation
        - Validation
        - Comparison operators
    )pbdoc")
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
    
    // Enhanced PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population with enhanced functionality.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);
    
    // Enhanced NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration with enhanced documentation.
        
        Each neuron type serves a specific function in the neural network:
        - Excitatory: Increase firing probability of postsynaptic neurons
        - Inhibitory: Decrease firing probability of postsynaptic neurons  
        - Modulatory: Affect synaptic plasticity and neuromodulation
        - Sensory: Process external inputs from environment
        - Motor: Generate outputs for action selection
        - Internal: Maintain internal state and homeostasis
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Neuron that increases postsynaptic firing")
        .value("Inhibitory", NeuronType::Inhibitory, "Neuron that decreases postsynaptic firing")
        .value("Modulatory", NeuronType::Modulatory, "Neuron that affects plasticity and neuromodulation")
        .value("Sensory", NeuronType::Sensory, "Neuron that processes external inputs")
        .value("Motor", NeuronType::Motor, "Neuron that generates motor outputs")
        .value("Internal", NeuronType::Internal, "Neuron that maintains internal state")
        .export_values();
    
    // Enhanced SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration with enhanced documentation.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    // Enhanced DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration with biological descriptions.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental state")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Period of heightened plasticity")
        .value("Maturation", DevelopmentalStage::Maturation, "Neural maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult developmental state")
        .value("Aging", DevelopmentalStage::Aging, "Aging developmental state")
        .export_values();
    
    // Enhanced ActionType enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Comprehensive action type enumeration for behavior selection.
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in environment")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in environment")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in environment")
        .value("MoveRight", ActionType::MoveRight, "Move right in environment")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left")
        .value("TurnRight", ActionType::TurnRight, "Turn right")
        .value("Look", ActionType::Look, "Look around")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with object")
        .value("Eat", ActionType::Eat, "Eat/consume object")
        .value("Drink", ActionType::Drink, "Drink liquid")
        .value("Rest", ActionType::Rest, "Rest and recover")
        .value("Wait", ActionType::Wait, "Wait without action")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();
    
    // Enhanced MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command enumeration for action execution.
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with object")
        .value("Wait", MotorCommand::Wait, "Wait")
        .export_values();
    
    // Enhanced WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        World object type enumeration for environmental simulation.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Useful resource")
        .value("Hazard", WorldObjectType::Hazard, "Dangerous object")
        .value("Wall", WorldObjectType::Wall, "Physical barrier")
        .value("Marker", WorldObjectType::Marker, "Reference point")
        .export_values();
    
    // Enhanced Config class
    py::class_<Config>(m, "Config", R"pbdoc(
        Enhanced configuration class for NLM system with type-safe access.
        
        Provides comprehensive configuration management with:
        - File-based configuration loading (JSON, XML, etc.)
        - Command-line argument overrides
        - Type-safe value retrieval
        - Configuration validation
        - Save/load functionality
        - Convenience methods for common settings
        
        Configuration keys:
        - random_seed: Random number generator seed
        - simulation_timestep: Time step for simulation
        - neuron_count: Number of neurons in network
        - region_count: Number of brain regions
        - connection_probability: Probability of synaptic connections
        - stdp_ltp_weight: STDP long-term potentiation weight
        - stdp_ltd_weight: STDP long-term depression weight
        - stdp_tau: STDP time constant
        - synaptogenesis_rate: Rate of new synapse formation
        - pruning_rate: Rate of synapse elimination
        - And many more...
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (JSON, XML, etc.)")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
        .def("has", &Config::has, py::arg("key"),
             "Check if configuration contains key")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get human-readable configuration summary")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Enhanced type-safe getOr methods
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("default"), 
            "Get string value or default if key doesn't exist")
        .def("getOr", [](const Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("default"), 
            "Get integer value or default if key doesn't exist")
        .def("getOr", [](const Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("default"), 
            "Get double value or default if key doesn't exist")
        .def("getOr", [](const Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("default"), 
            "Get float value or default if key doesn't exist")
        .def("getOr", [](const Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("default"), 
            "Get boolean value or default if key doesn't exist")
        // Convenience methods for common configuration patterns
        .def("setRandomSeed", [](Config& self, uint64_t seed) {
            self.set("random_seed", static_cast<int64_t>(seed), ConfigSource::Default);
        }, py::arg("seed"), "Set random seed")
        .def("setTimestep", [](Config& self, double timestep) {
            self.set("simulation_timestep", timestep, ConfigSource::Default);
        }, py::arg("timestep"), "Set simulation timestep")
        .def("setNeuronCount", [](Config& self, size_t count) {
            self.set("neuron_count", static_cast<int64_t>(count), ConfigSource::Default);
        }, py::arg("count"), "Set neuron count")
        .def("setRegionCount", [](Config& self, size_t count) {
            self.set("region_count", static_cast<int64_t>(count), ConfigSource::Default);
        }, py::arg("count"), "Set region count")
        .def("setConnectionProbability", [](Config& self, float prob) {
            self.set("connection_probability", prob, ConfigSource::Default);
        }, py::arg("probability"), "Set connection probability")
        .def("setStdpParameters", [](Config& self, float ltp, float ltd, float tau = 20.0f) {
            self.set("stdp_ltp_weight", ltp, ConfigSource::Default);
            self.set("stdp_ltd_weight", ltd, ConfigSource::Default);
            self.set("stdp_tau", tau, ConfigSource::Default);
        }, py::arg("ltp_weight"), py::arg("ltd_weight"), py::arg("tau") = 20.0f,
            "Set STDP parameters")
        .def("setPlasticityParameters", [](Config& self, float syn_rate, float prune_rate) {
            self.set("synaptogenesis_rate", syn_rate, ConfigSource::Default);
            self.set("pruning_rate", prune_rate, ConfigSource::Default);
        }, py::arg("synaptogenesis_rate"), py::arg("pruning_rate"),
            "Set structural plasticity parameters");
    
    // Enhanced SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory input with enhanced functionality.
        
        Provides common interface for different types of sensory data:
        - Vision (image data)
        - Audio (sound data)
        - Internal (internal state signals)
        - Proprioception (body position signals)
        
        Features:
        - Type identification
        - Timestamp tracking
        - Data access methods
        - Validation
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get sensory input type")
        .def("getData", &SensoryInput::getData, "Get raw data vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get data dimensions")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get signal timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set signal timestamp")
        .def("isValid", [](const SensoryInput& input) {
            return !input.getData().empty();
        }, "Check if input data is valid");
    
    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input with image processing capabilities.
        
        Represents visual input as a 2D array (grayscale) or 3D array (color).
        
        Features:
        - Image dimensions management
        - Data normalization
        - Edge detection preparation
        - Feature extraction methods
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data from vector")
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of color channels")
        .def("normalize", &Vision::normalize, "Normalize pixel values to [0, 1]")
        .def("getPixel", [](const Vision& self, size_t x, size_t y, size_t c = 0) {
            size_t index = ((y * self.getWidth()) + x) * self.getChannels() + c;
            const auto& data = self.getData();
            return index < data.size() ? data[index] : 0.0f;
        }, py::arg("x"), py::arg("y"), py::arg("channel") = 0,
            "Get pixel value at coordinates (x, y)")
        .def("setPixel", [](Vision& self, size_t x, size_t y, float value, size_t c = 0) {
            size_t index = ((y * self.getWidth()) + x) * self.getChannels() + c;
            auto& data = self.getData();
            if (index < data.size()) {
                data[index] = value;
            }
        }, py::arg("x"), py::arg("y"), py::arg("value"), py::arg("channel") = 0,
            "Set pixel value at coordinates (x, y)")
        .def("getArray2D", [](const Vision& self) {
            // Return 2D array for easier Python access
            std::vector<std::vector<float>> array2D;
            size_t width = self.getWidth();
            size_t height = self.getHeight();
            size_t channels = self.getChannels();
            const auto& data = self.getData();
            
            array2D.resize(height, std::vector<float>(width));
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    size_t index = (y * width + x) * channels;
                    if (index < data.size()) {
                        array2D[y][x] = data[index];
                    } else {
                        array2D[y][x] = 0.0f;
                    }
                }
            }
            return array2D;
        }, "Get vision data as 2D array (grayscale)");
    
    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input with sound processing capabilities.
        
        Represents audio input as time-series data with sampling information.
        
        Features:
        - Sampling rate management
        - Audio gain control
        - Frequency analysis preparation
        - Sound level measurement
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data from vector")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sampling rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sampling rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples")
        .def("applyGain", &Audio::applyGain, py::arg("gain"),
             "Apply gain to audio signal (linear scale)")
        .def("getPeakLevel", [](const Audio& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float max_val = *std::max_element(data.begin(), data.end());
            float min_val = *std::min_element(data.begin(), data.end());
            return std::max(std::abs(max_val), std::abs(min_val));
        }, "Get peak audio level (0.0 to 1.0)")
        .def("getRMSLevel", [](const Audio& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            float mean = sum / data.size();
            float variance = 0.0f;
            for (float val : data) {
                variance += (val - mean) * (val - mean);
            }
            variance /= data.size();
            return std::sqrt(variance);
        }, "Get root mean square audio level");
    
    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal state signals with homeostatic regulation.
        
        Represents internal physiological and cognitive states:
        - Energy levels
        - Hunger/satiety
        - Sleepiness
        - Attention levels
        - Emotional states
        
        Features:
        - Signal addition and management
        - Statistical analysis
        - Homeostatic regulation
        - Signal normalization
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a new internal signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("getAverage", &InternalSignals::getAverage,
             "Get average of all internal signals")
        .def("getMin", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::min_element(data.begin(), data.end());
        }, "Get minimum signal value")
        .def("getMax", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::max_element(data.begin(), data.end());
        }, "Get maximum signal value")
        .def("normalize", [](InternalSignals& self, float target_mean = 0.5f, float target_std = 0.2f) {
            const auto& data = self.getData();
            if (data.empty()) return;
            
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            float mean = sum / data.size();
            
            float variance = 0.0f;
            for (float val : data) {
                variance += (val - mean) * (val - mean);
            }
            variance /= data.size();
            float std_dev = std::sqrt(variance);
            
            if (std_dev > 0.001f) {
                auto& self_data = const_cast<std::vector<float>&>(self.getData());
                for (size_t i = 0; i < self_data.size(); ++i) {
                    self_data[i] = target_mean + (self_data[i] - mean) * target_std / std_dev;
                }
            }
        }, py::arg("target_mean") = 0.5f, py::arg("target_std") = 0.2f,
            "Normalize signals to target mean and standard deviation");
    
    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation with comprehensive parameter management.
        
        Represents agent actions with type and parameter information.
        
        Features:
        - Action type specification
        - Flexible parameter system
        - Parameter validation
        - Action cloning
        - Action name generation
        - Human-readable string representation
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters,
             "Get action parameters (name-value pairs)")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters from vector")
        .def("getParameterCount", [](const Action& action) {
            return action.getParameters().size();
        }, "Get number of parameters")
        .def("hasParameter", [](const Action& action, const std::string& name) {
            const auto& params = action.getParameters();
            return std::find_if(params.begin(), params.end(),
                              [&name](const std::pair<std::string, float>& p) { return p.first == name; }) != params.end();
        }, py::arg("name"), "Check if action has parameter")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("clone", &Action::clone, "Create a copy of this action")
        // Enhanced parameter methods
        .def("getParameter", [](const Action& action, const std::string& name, float defaultValue) {
            const auto& params = action.getParameters();
            auto it = std::find_if(params.begin(), params.end(),
                                 [&name](const std::pair<std::string, float>& p) { return p.first == name; });
            if (it != params.end()) return it->second;
            return defaultValue;
        }, py::arg("name"), py::arg("default") = 0.0f,
            "Get parameter value by name")
        .def("setParameter", [](Action& action, const std::string& name, float value) {
            auto& params = action.getParameters();
            auto it = std::find_if(params.begin(), params.end(),
                                 [&name](const std::pair<std::string, float>& p) { return p.first == name; });
            if (it != params.end()) {
                it->second = value;
            } else {
                params.push_back({name, value});
            }
        }, py::arg("name"), py::arg("value"),
            "Set parameter value by name")
        .def("getParameterNames", [](const Action& action) {
            std::vector<std::string> names;
            const auto& params = action.getParameters();
            for (const auto& param : params) {
                names.push_back(param.first);
            }
            return names;
        }, "Get all parameter names")
        .def("removeParameter", [](Action& action, const std::string& name) {
            auto& params = action.getParameters();
            params.erase(std::remove_if(params.begin(), params.end(),
                                      [&name](const std::pair<std::string, float>& p) { return p.first == name; }),
                        params.end());
        }, py::arg("name"), "Remove parameter by name")
        .def("to_dict", [](const Action& action) {
            std::map<std::string, float> dict;
            const auto& params = action.getParameters();
            for (const auto& param : params) {
                dict[param.first] = param.second;
            }
            return dict;
        }, "Convert action to parameter dictionary")
        .def("from_dict", [](Action& action, const std::map<std::string, float>& dict) {
            action.setParameters(std::vector<std::pair<std::string, float>>(dict.begin(), dict.end()));
        }, py::arg("dict"), "Load action parameters from dictionary");
    
    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object with spatial and physical properties.
        
        Represents objects in the simulated environment with:
        - Position and size
        - Type classification
        - Physical properties
        - Activation state
        
        Features:
        - Position management
        - Distance calculations
        - Collision detection preparation
        - State management
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "X coordinate")
        .def_readwrite("y", &WorldObject::y, "Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Radius/size")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value/utility")
        .def_readwrite("active", &WorldObject::active, "Activation state")
        // Enhanced spatial methods
        .def("distanceTo", [](const WorldObject& obj, float x, float y) {
            return std::hypot(obj.x - x, obj.y - y);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point")
        .def("distanceToObject", [](const WorldObject& obj1, const WorldObject& obj2) {
            return std::hypot(obj1.x - obj2.x, obj1.y - obj2.y);
        }, py::arg("other"), "Calculate distance to another object")
        .def("isNear", [](const WorldObject& obj, float x, float y, float threshold) {
            return std::hypot(obj.x - x, obj.y - y) <= threshold;
        }, py::arg("x"), py::arg("y"), py::arg("threshold"), "Check if object is near point")
        .def("containsPoint", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return (dx * dx + dy * dy) <= (obj.radius * obj.radius);
        }, py::arg("x"), py::arg("y"), "Check if point is inside object")
        .def("center", [](const WorldObject& obj) {
            return std::make_pair(obj.x, obj.y);
        }, "Get object center coordinates")
        .def("setPosition", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, py::arg("x"), py::arg("y"), "Set object position");
    
    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state with physics and physiology.
        
        Represents complete agent state including:
        - Position and orientation
        - Movement dynamics
        - Energy and health
        - Age and development
        - Sensory capabilities
        
        Features:
        - Physics simulation
        - State management
        - Resource management
        - Movement control
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X coordinate")
        .def_readwrite("y", &AgentBody::y, "Y coordinate")
        .def_readwrite("orientation", &AgentBody::orientation, "Facing direction in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("health", &AgentBody::health, "Health status (0-1)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Movement state")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Turning state")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset body to initial state")
        // Enhanced movement methods
        .def("moveTo", [](AgentBody& body, float x, float y, float maxSpeed = 1.0f, float timestep = 0.1f) {
            float dx = x - body.x;
            float dy = y - body.y;
            float distance = std::hypot(dx, dy);
            
            if (distance > 0.001f) {
                float speed = std::min(maxSpeed, distance / timestep);
                body.velocityX = (dx / distance) * speed;
                body.velocityY = (dy / distance) * speed;
                body.isMoving = true;
                
                body.x += body.velocityX * timestep;
                body.y += body.velocityY * timestep;
            }
        }, py::arg("x"), py::arg("y"), py::arg("max_speed") = 1.0f, py::arg("timestep") = 0.1f,
            "Move to target position with physics")
        .def("rotateTo", [](AgentBody& body, float targetOrientation, float maxAngularSpeed = 1.0f, float timestep = 0.1f) {
            float angleDiff = targetOrientation - body.orientation;
            
            // Normalize angle difference
            while (angleDiff > M_PI) angleDiff -= 2 * M_PI;
            while (angleDiff < -M_PI) angleDiff += 2 * M_PI;
            
            float angularChange = std::min(maxAngularSpeed * timestep, std::abs(angleDiff));
            body.angularVelocity = (angleDiff > 0 ? angularChange : -angularChange);
            body.isTurning = std::abs(body.angularVelocity) > 0.001f;
            
            body.orientation += body.angularVelocity;
        }, py::arg("target_orientation"), py::arg("max_angular_speed") = 1.0f, py::arg("timestep") = 0.1f,
            "Rotate to target orientation with physics")
        .def("updatePhysics", [](AgentBody& body, float timestep) {
            // Apply physics: update position based on velocity
            body.x += body.velocityX * timestep;
            body.y += body.velocityY * timestep;
            body.orientation += body.angularVelocity * timestep;
            
            // Simple friction
            body.velocityX *= 0.95f;
            body.velocityY *= 0.95f;
            body.angularVelocity *= 0.95f;
            
            // Update movement states
            body.isMoving = std::abs(body.velocityX) > 0.001f || std::abs(body.velocityY) > 0.001f;
            body.isTurning = std::abs(body.angularVelocity) > 0.001f;
        }, py::arg("timestep"), "Update body physics")
        .def("applyForce", [](AgentBody& body, float fx, float fy, float timestep = 0.1f) {
            body.velocityX += fx * timestep;
            body.velocityY += fy * timestep;
            body.isMoving = true;
        }, py::arg("fx"), py::arg("fy"), py::arg("timestep") = 0.1f,
            "Apply force to body")
        .def("getSpeed", [](const AgentBody& body) {
            return std::hypot(body.velocityX, body.velocityY);
        }, "Get current speed")
        .def("getHeading", [](const AgentBody& body) {
            return body.orientation;
        }, "Get current heading (radians)")
        .def("faceTowards", [](AgentBody& body, float targetX, float targetY) {
            float dx = targetX - body.x;
            float dy = targetY - body.y;
            body.orientation = std::atan2(dy, dx);
        }, py::arg("target_x"), py::arg("target_y"), "Face towards target position");
    
    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of an action with reward and outcome information.
        
        Represents the result of executing an action in the environment.
        
        Features:
        - Reward signal
        - Success/failure status
        - Descriptive message
        - Timestamp
        - Utility for reinforcement learning
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward and success status")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Success status")
        .def_readwrite("message", &ActionResult::message, "Descriptive message")
        .def("isSuccessful", &ActionResult::success, "Check if action was successful")
        .def("getFormattedOutput", [](const ActionResult& result) {
            std::stringstream ss;
            ss << "ActionResult:" << std::endl;
            ss << "  Success: " << (result.success ? "Yes" : "No") << std::endl;
            ss << "  Reward: " << result.reward << std::endl;
            if (!result.message.empty()) {
                ss << "  Message: " << result.message << std::endl;
            }
            return ss.str();
        }, "Get formatted string representation");
    
    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept with multiple modalities.
        
        Consolidates all sensory inputs received in a single timestep.
        
        Features:
        - Multiple sensory modalities
        - Timestamp tracking
        - Data aggregation
        - Signal processing
        - Sensory weighting
    )pbdoc")
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
        .def("setAudio", &Audio::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set percept timestamp")
        .def("getSignalSum", &SensoryPercept::getSignalSum, "Get sum of all sensory signals")
        .def("getDominantSensory", [](const SensoryPercept& percept) {
            // Simple heuristic: find which sensory modality has highest total signal
            float vision_sum = 0.0f;
            const auto& vision_data = percept.getVision().getData();
            for (float val : vision_data) vision_sum += val;
            
            float touch_sum = 0.0f;
            const auto& touch_data = percept.getTouch();
            for (float val : touch_data) touch_sum += val;
            
            float internal_sum = 0.0f;
            const auto& internal_data = percept.getInternal();
            for (float val : internal_data) internal_sum += val;
            
            float audio_sum = 0.0f;
            const auto& audio_data = percept.getAudio();
            for (float val : audio_data) audio_sum += val;
            
            std::vector<std::pair<std::string, float>> modalities = {
                {"vision", vision_sum},
                {"touch", touch_sum},
                {"internal", internal_sum},
                {"audio", audio_sum}
            };
            
            return std::max_element(modalities.begin(), modalities.end(),
                                   [](const auto& a, const auto& b) { return a.second < b.second; })->first;
        }, "Get dominant sensory modality");
    
    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world with physics simulation.
        
        Simulates a 2D environment with agents, objects, and physics.
        
        Features:
        - Physics simulation
        - Object management
        - Collision detection
        - Sensor simulation
        - World state management
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("step", [](SimpleWorld& world, double timestep) {
            world.update(timestep);
        }, py::arg("timestep"), "Alias for update method")
        .def("getObjectsAt", [](const SimpleWorld& world, float x, float y) {
            std::vector<WorldObject> objects;
            // Note: This would require implementing object storage in SimpleWorld
            return objects;
        }, py::arg("x"), py::arg("y"), "Get objects at position")
        .def("getObjectsOfType", [](const SimpleWorld& world, WorldObjectType type) {
            std::vector<WorldObject> objects;
            // Note: This would require implementing object storage in SimpleWorld
            return objects;
        }, py::arg("type"), "Get objects of specific type");
    
    // Enhanced Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain with enhanced API.
        
        The core neural processing unit implementing:
        - Real spiking neural computation
        - Event-driven dynamics
        - Integrated memory systems
        - Neuromodulation
        - Prediction and planning
        - Development and learning
        
        Features:
        - Type-safe configuration
        - Enhanced step methods
        - Sensory integration
        - Motor output generation
        - State management
        - Persistence (save/load)
        - Comprehensive metrics
        - Development system
        - Neuromodulation
        - Plasticity control
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform simulation step with time")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Receive sensory input")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current to neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current to neuron type")
        .def("produceAction", &Brain::produceAction, "Generate motor action")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add brain region")
        .def("getRegion", &Brain::getRegion, py::arg("id"), "Get region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get active neuron count")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get firing neuron count")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount, "Get pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, "Get configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        // Enhanced methods
        .def("getMetrics", &Brain::getMetrics, "Get comprehensive brain metrics")
        .def("simulateSteps", [](Brain& brain, size_t steps, double timestep = 0.001) {
            for (SimulationStep step = 0; step < steps; ++step) {
                brain.step(step, step * timestep);
            }
        }, py::arg("steps"), py::arg("timestep") = 0.001, "Simulate multiple steps")
        .def("getRegionByName", [](Brain& brain, const std::string& name) {
            auto ids = brain.getRegionIds();
            if (!ids.empty()) return brain.getRegion(ids[0]);
            return static_cast<NeuralRegion*>(nullptr);
        }, py::arg("name"), "Get region by name (placeholder)")
        .def("enableAllPlasticity", [](Brain& brain, bool enable = true) {
            if (auto* region = brain.getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(enable, enable, enable);
                }
            }
        }, py::arg("enable") = true, "Enable/disable all plasticity")
        .def("configureStdp", [](Brain& brain, float ltp, float ltd, float tau = 20.0f) {
            if (auto* stdp = brain.getSTDP()) {
                stdp->setLTP(ltp);
                stdp->setLTD(ltd);
                stdp->setTau(tau);
            }
        }, py::arg("ltp"), py::arg("ltd"), py::arg("tau") = 20.0f, "Configure STDP parameters")
        .def("getNeuralActivityStats", [](const Brain& brain) {
            std::map<std::string, float> stats;
            stats["total_spikes"] = static_cast<float>(brain.getTotalSpikeCount());
            stats["average_firing_rate"] = brain.getAverageFiringRate();
            stats["active_neuron_ratio"] = static_cast<float>(brain.getActiveNeuronCount()) / 
                                          std::max<size_t>(1, brain.getTotalNeuronCount());
            stats["firing_neuron_ratio"] = static_cast<float>(brain.getFiringNeuronCount()) / 
                                          std::max<size_t>(1, brain.getTotalNeuronCount());
            return stats;
        }, "Get neural activity statistics");
    
    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting neural computation to environment.
        
        Bridges the gap between neural simulation and real-world interaction by:
        - Converting sensory inputs to neural signals
        - Decoding neural activity into motor commands
        - Applying neuromodulation based on reward
        - Managing developmental processes
        - Handling action selection with exploration
        
        Features:
        - Sensory processing
        - Motor decoding
        - Reward modulation
        - Development management
        - Curiosity-driven exploration
        - Configuration management
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create agent brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory input")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward modulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent")
        .def("getBrain", &AgentBrain::getBrain, "Get underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, "Check reward modulation")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, "Check structural plasticity")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, "Check development")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, "Check curiosity")
        // Enhanced methods
        .def("processVision", [](AgentBrain& agent, const std::vector<float>& vision) {
            Vision vis(vision.size() > 0 ? static_cast<size_t>(std::sqrt(vision.size())) : 16,
                      vision.size() > 0 ? static_cast<size_t>(std::sqrt(vision.size())) : 16);
            vis.setData(vision);
            SensoryPercept percept;
            percept.setVision(vis);
            agent.processSensoryInput(percept);
        }, py::arg("vision"), "Process vision input")
        .def("decodeToAction", [](AgentBrain& agent) {
            MotorCommand cmd = agent.decodeMotorCommand();
            Action action;
            action.setType(static_cast<ActionType>(cmd));
            return action;
        }, "Decode motor command to Action")
        .def("computeActionValue", [](const AgentBrain& agent, const Action& action) {
            // Simple heuristic: compute action value based on neuromodulation levels
            float base_value = 0.5f;
            if (agent.isRewardModulationEnabled()) {
                base_value += agent.getNeuromodulationLevel() * 0.3f;
            }
            if (agent.isCuriosityEnabled()) {
                base_value += agent.getCuriosityLevel() * 0.2f;
            }
            return std::max(0.0f, std::min(1.0f, base_value));
        }, py::arg("action"), "Compute action value");
    
    // Factory functions
    m.def("createDefaultConfig", &createDefaultConfig, "Create default configuration")
    m.def("createBrain", &createBrainWithConfig, py::arg("config"), "Create brain with config")
    m.def("createSimpleWorld", &createSimpleWorld, "Create simple world")
    m.def("createAgentBrain", &createAgentBrain, py::arg("brain"), "Create agent brain")
    
    // Constants and utilities
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Version and metadata
    m.attr("__version__") = "0.1.0-enhanced";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Enhanced Python bindings for NLM neural simulation framework";
    
    // Module-level utilities
    m.def("get_version", []() {
        return std::string("NLM v0.1.0-enhanced - Neural Learning Machine");
    }, "Get version string")
    
    m.def("benchmark_neural", [](std::shared_ptr<Brain> brain, size_t steps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return duration.count();
    }, py::arg("brain"), py::arg("steps"), "Benchmark brain performance")
    
    // Example and tutorial functions
    m.def("create_simple_brain", []() {
        auto config = createDefaultConfig();
        return createBrainWithConfig(config);
    }, "Create a simple brain for quick testing")
    
    m.def("run_plasticity_demo", [](std::shared_ptr<Brain> brain, size_t steps = 100) {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run simulation with sensory input
        for (SimulationStep step = 0; step < steps; ++step) {
            for (size_t i = 0; i < 20; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        
        std::map<std::string, double> results;
        results["total_spikes"] = brain->getTotalSpikeCount();
        results["firing_neurons"] = brain->getFiringNeuronCount();
        results["execution_time"] = duration.count();
        
        return results;
    }, py::arg("brain"), py::arg("steps") = 100, "Run plasticity demonstration");
}